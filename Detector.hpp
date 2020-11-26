#pragma once
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <dlib/opencv.h>
#include <dlib/image_processing/frontal_face_detector.h>
#include <dlib/image_processing/render_face_detections.h>
#include <dlib/image_processing.h>
#define sFrames 3
struct Vec2xS{
	double x[68];
	double y[68];
	void copyShape(dlib::full_object_detection);
};
	
struct DetectorConfig{
	double mouthThr;
	double mouthM;
	double posXM;
	double posYM;
	double posZM;
	double angXM;
	double angYM;
	double angZM;
	int smooth;
}

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
