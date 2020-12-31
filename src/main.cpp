#include <stdio.h>
#include <string.h>
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <CubismFramework.hpp>
#include <CubismModelSettingJson.hpp>
#include <Model/CubismUserModel.hpp>
#include <Model/CubismModel.hpp>
#include <Id/CubismIdManager.hpp>
#include <CubismDefaultParameterId.hpp>
#include <Rendering/OpenGL/CubismRenderer_OpenGLES2.hpp>
#include "Allocator.hpp"
#include "Model.hpp"

Allocator _cubismAllocator;
CubismFramework::Option _cubismOption;

int winHeight = 500;
int winWidth = 500;

#include "Gui.hpp"

int main(void){
	if(glfwInit() == GL_FALSE){
		printf("glfwInit Failed\n");
		return 1;
	}

	glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, 1);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

	GLFWwindow* window = glfwCreateWindow(winHeight, winWidth, "vttscpp", NULL, NULL);
	if(!window){
		printf("glfwCreateWindow Failed\n");
		glfwTerminate();
		return 1;
	}

	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);

	if(glewInit() != GLEW_OK){
		printf("glewInit Failed\n");
		glfwTerminate();
		return 1;
	}

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	CubismFramework::StartUp(&_cubismAllocator, &_cubismOption);
    CubismFramework::Initialize();

	l2dModel userModel;

	LoadAssets("res/", "pinctov2_f", &userModel);
    CubismMatrix44 projection;
	projection.Scale(1.0f , (float)winWidth / (float)winHeight);
	userModel.GetRenderer<Rendering::CubismRenderer_OpenGLES2>()->SetMvpMatrix(&projection);

	cfg.camIndex=1;

	guiInit(window);

	unsigned char _frames = 0;
	char _titleBuffer[64];
	double _previousTime = 0;
	float modelScale = 1.0f;
	float modelX = 0.0f;
	float modelY = 0.0f;
	while(!glfwWindowShouldClose(window)){
		int now_w, now_h;
		double _currentTime = glfwGetTime();
		_frames++;
		if(_currentTime - _previousTime >=1.0){
			sprintf(_titleBuffer,"L2D Face Test: %d FPS", _frames);
			glfwSetWindowTitle(window, _titleBuffer);
			_frames = 0;
			_previousTime = _currentTime;
		}
		guiUpdate();
		modelUpdate(&userModel);
		det.updateSmooth();
        glfwGetWindowSize(window, &now_w, &now_h);
        if((winWidth!=now_w || winHeight!=now_h || modelScale!=nowScale || modelX!=nowModX || modelY!=nowModY) && now_w>0 && now_h>0){
            winWidth = now_w;
            winHeight = now_h;
			modelScale = nowScale;
			modelX = nowModX;
			modelY = nowModY;
            glViewport(0, 0, now_w, now_h);
			projection.Scale(1.0f * modelScale, (float)now_w / (float)now_h * modelScale);
			projection.Translate(modelX*modelScale,(float)now_w / (float)now_h * modelY*modelScale);
			userModel.GetRenderer<Rendering::CubismRenderer_OpenGLES2>()->SetMvpMatrix(&projection);
		}

		if(bgAdd) glClearColor(bgColor[0], bgColor[1], bgColor[2], bgColor[3]);
		else glClearColor(bgColor[0]*bgColor[3], bgColor[1]*bgColor[3], bgColor[2]*bgColor[3], bgColor[3]);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearDepth(1.0);
		userModel.GetRenderer<Rendering::CubismRenderer_OpenGLES2>()->DrawModel();

		guiRender();
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwDestroyWindow(window);
	glfwTerminate();

	CubismFramework::Dispose();

	return 0;
}
