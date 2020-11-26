#include "Detector.hpp"
#include <math.h>

dlib::frontal_face_detector det;
dlib::shape_predictor predMod;

cv::Mat frame;
cv::VideoCapture cap;

dlib::image_window dlibWin;
dlib::full_object_detection shape;
Vec2xS shapes[sFrames];
Vec2xS smoothShape;
dlib::rectangle _face;

pthread_t UpdateCam;

void * Update(void * arg);

double distance(Vec2xS, int, int);

void calcPosX(Vec2xS);
void calcPosY(Vec2xS);
void calcPosZ();
void calcAngX(Vec2xS);
void calcAngY(Vec2xS);
void calcAngZ(Vec2xS);
void calcMouthOpen(Vec2xS);
void calcEyeOpen(Vec2xS);
double calcBlink(Vec2xS);

bool _shapeReady=false;
bool stop=false;
int sFrame=0;

double in_angX=0;
double in_angY=0;
double in_angZ=0;

double in_posX=0;
double in_posY=0;
double in_posZ=0;
double in_mouthOpen=0;
double in_eyeOpen=0;
#define mouthThr 0.1
#define mouthEx 2.0

void Vec2xS::copyShape(dlib::full_object_detection _shape){
	for(int i = 0; i < 68; i++){
		x[i]=_shape.part(i).x();
		y[i]=_shape.part(i).y();
	}
}

Detector::Detector():
	angX(0),
	angY(0),
	angZ(0),
	posX(0),
	posY(0),
	posZ(0),
	mouthOpen(0),
	eyeOpenL(0),
	eyeOpenR(0)
{

	_shapeReady = false;
	sFrame=0;
	cap.open(0);

	det = dlib::get_frontal_face_detector();
	dlib::deserialize("shape_predictor.dat")>>predMod;
	stop = false;
	pthread_create(&UpdateCam, NULL, Update,this);
}

void Detector::updateSmooth(){
	angX+=(in_angX-angX)/8;
	angY+=(in_angY-angY)/8;
	angZ+=(in_angZ-angZ)/8;

	posX+=(in_posX-posX)/8;
	posY+=(in_posY-posY)/8;
	posZ+=(in_posZ-posZ)/8;

	mouthOpen+=(((in_mouthOpen-mouthThr)*mouthEx)-mouthOpen)/4;
	eyeOpenL+=(in_eyeOpen - eyeOpenL)/8;
	eyeOpenL=(in_eyeOpen == 0) ? 0 : eyeOpenL;
	eyeOpenR+=(in_eyeOpen - eyeOpenR)/8;
	eyeOpenR=(in_eyeOpen == 0) ? 0 : eyeOpenR;
}

void Detector::Exit(){
	stop = true;
}

void * Update(void * arg){
	while(!stop){
		if(cap.read(frame)){
			cv::resize(frame, frame, cv::Size(), 0.5, 0.5);
			frame.convertTo(frame, -1, 2.0, -0);
			dlib::cv_image<dlib::bgr_pixel> cimg(frame);
			std::vector<dlib::rectangle> faces = det(cimg);
			if(faces.size() > 0){
				_face = faces[0];
				shape=(predMod(cimg,_face));

				shapes[sFrame].copyShape(shape);
				if(!_shapeReady){
					for(int i=1;i < sFrames;i++)
						shapes[i]=shapes[sFrame];
					_shapeReady=true;
				}
				

				dlibWin.clear_overlay();
				dlibWin.add_overlay(dlib::render_face_detections(shape));

			}
			if(_shapeReady){

				for(int j = 0; j < 68; j ++){
					smoothShape.x[j]=0;smoothShape.y[j]=0;
					for(int i = 0; i < sFrames;i++){
						smoothShape.x[j]+=shapes[i].x[j];
						smoothShape.y[j]+=shapes[i].y[j];
					}
					smoothShape.x[j]/=sFrames;smoothShape.y[j]/=sFrames;
				}

				calcPosX(smoothShape);
				calcPosY(smoothShape);
				if(in_eyeOpen - calcBlink(shapes[sFrame]) > 0.07){
					in_eyeOpen=0;
				} else
					calcEyeOpen(smoothShape);


				calcAngX(smoothShape);
				calcAngY(smoothShape);
				calcAngZ(smoothShape);
				calcPosZ();
				calcMouthOpen(smoothShape);

				sFrame=(sFrame+1)%sFrames;
			}

				dlibWin.set_image(cimg);
		}
	}
	return NULL;
}
double distance(Vec2xS shapeVec, int i, int j){
	return sqrt(pow(shapeVec.x[i] - shapeVec.x[j],2.0) + pow(shapeVec.y[i] - shapeVec.y[j],2.0));
}
void calcPosY(Vec2xS detShape){
	in_posY=0;
	for(int i = 0; i < 68; i ++) // 0 through 67 = whole face
		in_posY+=detShape.y[i];
	in_posY=((in_posY/68)/frame.cols*2)-1;
}
void calcPosX(Vec2xS detShape){
	in_posX=0;
	for(int i = 0; i < 68; i ++) // 0 through 67 = whole face
		in_posX+=detShape.x[i];
	in_posX=((in_posX/68)/frame.cols*2)-1;
}
void calcAngX(Vec2xS detShape){
	double noseX=(detShape.x[27]+detShape.x[28])/2;
	double eyeInL=detShape.x[39];
	double eyeInR=detShape.x[42];
	in_angX=((noseX-eyeInL)+(noseX-eyeInR))*2;
}
void calcAngY(Vec2xS detShape){
	in_angY=distance(detShape,39,30);
	in_angY+=distance(detShape,42,30);
	in_angY/=6*distance(detShape,31,35);
	in_angY=(in_angY-0.45)*(-1.0/0.15);
}
void calcAngZ(Vec2xS detShape){
	double zx1,zy1,zx2,zy2;
	zx1 = 0;
	for(int i = 36; i < 39; i++) // 36 through 39 = left eye
		zx1 += detShape.x[i];
	zy1 = 0;
	for(int i = 36; i < 39; i++)
		zy1 += detShape.y[i];
	zx2 = 0;
	for(int i = 42; i < 45; i++) // 42 through 45 = right eye
		zx2 += detShape.x[i];
	zy2 = 0;
	for(int i = 42; i < 45; i++)
		zy2 += detShape.y[i];

	in_angZ=fmod((atan2(zy1-zy2,zx1-zx2) * 180 / M_PI) + 360, 360)-180;
}
void calcPosZ(){
	in_posZ=in_angZ*(abs(in_posX)/2+0.2);
}
void calcMouthOpen(Vec2xS detShape){
	in_mouthOpen=distance(detShape,61,67);
	in_mouthOpen+=distance(detShape,62,66);
	in_mouthOpen+=distance(detShape,63,65);
	in_mouthOpen/=distance(detShape,60,64)*3;
}
double calcBlink(Vec2xS detShape){
	double i=distance(detShape,43,47);
	i+=distance(detShape,44,46);
	i/=distance(detShape,42,45)*2;
	return i;
}
void calcEyeOpen(Vec2xS detShape){
	in_eyeOpen=distance(detShape,43,47);
	in_eyeOpen+=distance(detShape,44,46);
	in_eyeOpen/=distance(detShape,42,45)*2;
}
