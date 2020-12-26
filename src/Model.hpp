#pragma once
#include <CubismFramework.hpp>
#include <CubismModelSettingJson.hpp>
#include <Model/CubismUserModel.hpp>
#include <Model/CubismModel.hpp>
#include <Physics/CubismPhysics.hpp>
#include <Id/CubismIdManager.hpp>
#include <CubismDefaultParameterId.hpp>
#include <Rendering/OpenGL/CubismRenderer_OpenGLES2.hpp>
#include "Detector.hpp"

extern Detector det;

using namespace Live2D::Cubism::Framework;
using namespace Live2D::Cubism::Framework::DefaultParameterId;
using namespace Csm;

class l2dModel: public CubismUserModel {
	public:
	CubismModel* model(){
		return _model;
	}
	CubismBreath* breath(){
		return _breath;
	}
	CubismPhysics* physics(){
		return _physics;
	}
};

void LoadAssets(const char* dir, const char* filename, l2dModel * userModel);

void modelUpdate(l2dModel * userModel);
