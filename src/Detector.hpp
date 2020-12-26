#pragma once
#include <dlib/image_processing.h>
#define sFrames 3
struct Vec2xS{
	double x[68];
	double y[68];
	void copyShape(dlib::full_object_detection);
};
	
struct DetectorConfig{
	double mouthThr;
	double mouthEx;
	double posXOff;
	double posYOff;
	double posZOff;
	double angXOff;
	double angYOff;
	double angZOff;
	double posXM;
	double posYM;
	double posZM;
	double angXM;
	double angYM;
	double angZM;
	double camalpha;
	double cambeta;
	double smooth;
	bool toggleCam;
	int camIndex;
};

struct Detector{
	Detector();
	double angX;
	double angY;
	double angZ;

	double posX;
	double posY;
	double posZ;

	double mouthOpen;
	double eyeOpenL;
	double eyeOpenR;
	void updateSmooth();
	void Exit();
};
extern DetectorConfig cfg;
extern clock_t t5;
