#include "Detector.hpp"
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <dlib/opencv.h>
#include <dlib/image_processing/frontal_face_detector.h>
#include <dlib/image_processing/render_face_detections.h>
#include <dlib/image_processing.h>
#include <math.h>
#define sFrames 3

struct Vec2xS{
	double x[68];
	double y[68];
	void copyShape(dlib::full_object_detection);
};

pthread_t UpdateCam;

void * Update(void * arg);

double distance(Vec2xS, int, int);

void calcPosX(Vec2xS,int);
void calcPosY(Vec2xS,int);
void calcPosZ();
void calcAngX(Vec2xS);
void calcAngY(Vec2xS);
void calcAngZ(Vec2xS);
void calcMouthOpen(Vec2xS);
void calcEyeOpen(Vec2xS);
double calcBlink(Vec2xS);

bool stop=false;

double in_angX=0;
double in_angY=0;
double in_angZ=0;

double in_posX=0;
double in_posY=0;
double in_posZ=0;
double in_mouthOpen=0;
double in_eyeOpen=0;
clock_t t5;

DetectorConfig cfg = {
	.mouthThr = 0.1,
	.mouthEx = 2.0,
	.posXOff = 0.0,
	.posYOff = 0.0,
	.posZOff = 0.0,
	.angXOff = 0.0,
	.angYOff = 0.0,
	.angZOff = 0.0,
	.posXM = 1.0,
	.posYM = 1.0,
	.posZM = 1.0,
	.angXM = 1.0,
	.angYM = 1.0,
	.angZM = 1.0,
	.camalpha = 2.0,
	.cambeta = 0,
	.smooth = 8,
	.toggleCam = false,
	.camIndex = 0
};


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
}

void Detector::updateSmooth(){
	angX+=(in_angX-angX)/cfg.smooth;
	angY+=(in_angY-angY)/cfg.smooth;
	angZ+=(in_angZ-angZ)/cfg.smooth;
                   
	posX+=(in_posX-posX)/cfg.smooth;
	posY+=(in_posY-posY)/cfg.smooth;
	posZ+=(in_posZ-posZ)/cfg.smooth;

	mouthOpen+=(((in_mouthOpen-cfg.mouthThr)*cfg.mouthEx)-mouthOpen)/(cfg.smooth/2);
	eyeOpenL+=(in_eyeOpen - eyeOpenL)/cfg.smooth;
	eyeOpenL=(in_eyeOpen == 0) ? 0 : eyeOpenL;
	eyeOpenR+=(in_eyeOpen - eyeOpenR)/cfg.smooth;
	eyeOpenR=(in_eyeOpen == 0) ? 0 : eyeOpenR;
}

void Detector::Exit(){
	if(stop){
		pthread_create(&UpdateCam, NULL, Update, NULL);
	}
	stop = !stop;
}

void * Update(void * arg){
	dlib::frontal_face_detector det;

	dlib::shape_predictor predMod;
	dlib::deserialize("res/shape_predictor.dat")>>predMod;

	cv::Mat frame;

	cv::VideoCapture cap;
	cap.open(cfg.camIndex);

	dlib::array2d<unsigned char> cimg;
	dlib::full_object_detection shape;
	Vec2xS shapes[sFrames];
	Vec2xS smoothShape;
	dlib::rectangle _face;

	bool _shapeReady = false;
	int sFrame=0;

	det = dlib::get_frontal_face_detector();
	stop = false;
	bool showWin = false;
	cfg.toggleCam = false;
	clock_t _timeinit;
	if(cap.isOpened()){
		while(!stop){
			if(cap.read(frame) && !frame.empty()){
				if(cfg.toggleCam && !showWin){
					cv::namedWindow("Cam", cv::WINDOW_AUTOSIZE | cv::WINDOW_NORMAL | cv::WINDOW_GUI_NORMAL);
					showWin = true; cfg.toggleCam = false;
				} else if(cfg.toggleCam && showWin){
					cv::destroyAllWindows();
					showWin = false; cfg.toggleCam = false;
				}
				_timeinit = clock();
				cv::resize(frame, frame, cv::Size((int)((double)frame.cols/frame.rows*200),200));
				cv::flip(frame, frame,1);

				frame.convertTo(frame, -1, cfg.camalpha, cfg.cambeta);

				dlib::cv_image<dlib::bgr_pixel> cvimg(frame);
				dlib::assign_image(cimg,cvimg);

				std::vector<dlib::rectangle> faces = det(cimg);
				if(faces.size() > 0){
					_face = faces[0];
					shape=(predMod(cimg,_face));

					if(!_shapeReady){
						shapes[sFrame].copyShape(shape);
						for(int i=1;i < sFrames;i++)
							shapes[i]=shapes[sFrame];
						_shapeReady=true;
					}


				}
				if(_shapeReady){
					shapes[sFrame].copyShape(shape);

					for(int j = 0; j < 68; j ++){
						smoothShape.x[j]=0;smoothShape.y[j]=0;
						for(int i = 0; i < sFrames;i++){
							smoothShape.x[j]+=shapes[i].x[j];
							smoothShape.y[j]+=shapes[i].y[j];
						}
						smoothShape.x[j]/=sFrames;smoothShape.y[j]/=sFrames;
						cv::circle(frame,{(int)smoothShape.x[j],(int)smoothShape.y[j]},2,(255,255,255));
						cv::circle(frame,{(int)shape.part(j).x(),(int)shape.part(j).y()},1,(0,0,0));
					}

					calcPosX(smoothShape,frame.rows);
					calcPosY(smoothShape,frame.cols);
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

				if(showWin){
					imshow("Cam", frame);
					cv::waitKey(30);
				}
				t5 = clock() - _timeinit;
			}
		}
		if(showWin){
			cv::destroyAllWindows();
			showWin = false; cfg.toggleCam = false;
		}
		cap.release();
	}
	return NULL;
}
double distance(Vec2xS shapeVec, int i, int j){
	return sqrt(pow(shapeVec.x[i] - shapeVec.x[j],2.0) + pow(shapeVec.y[i] - shapeVec.y[j],2.0));
}
void calcPosY(Vec2xS detShape,int height){
	in_posY=0;
	for(int i = 0; i < 68; i ++) // 0 through 67 = whole face
		in_posY+=detShape.y[i];
	in_posY=(((in_posY/68)/height*2)-1)*-10;
}
void calcPosX(Vec2xS detShape,int width){
	in_posX=0;
	for(int i = 0; i < 68; i ++) // 0 through 67 = whole face
		in_posX+=detShape.x[i];
	in_posX=(((in_posX/68)/width*2)-1)*10;
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
	in_angY=(in_angY-0.45)*(-1.0/0.15) * 30;
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
	in_posZ=in_angZ*(abs(in_posX/10)/2+0.2);
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
