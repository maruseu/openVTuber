#include <stdio.h>
#include <string.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <CubismFramework.hpp>
#include <CubismModelSettingJson.hpp>
#include <Model/CubismUserModel.hpp>
#include <Model/CubismModel.hpp>
#include <Id/CubismIdManager.hpp>
#include <CubismDefaultParameterId.hpp>
#include <Rendering/OpenGL/CubismRenderer_OpenGLES2.hpp>
#include "LAppAllocator.hpp"            ///< Implement my own bullshit later
#include "Model.hpp"

LAppAllocator _cubismAllocator;              ///< Cubism SDK Allocator from the sample
CubismFramework::Option _cubismOption;

int winHeight = 200;
int winWidth = 200;

int main(void){
	if(glfwInit() == GL_FALSE){
		printf("glfwInit Failed\n");
		return 1;
	}

	glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, 1);

	GLFWwindow* window = glfwCreateWindow(winHeight, winWidth, "vttscpp", NULL, NULL);
	if(window == NULL){
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
    CubismMatrix44 projection;

	l2dModel userModel;

	LoadAssets("res/", "pinctov2_f", &userModel);
	cfg.camIndex=1;
	det.Exit();
	det.Exit();

	unsigned char _frames = 0;
	char _titleBuffer[64];
	double _previousTime = 0;
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
		modelUpdate(&userModel);
		det.updateSmooth();
        glfwGetWindowSize(window, &now_w, &now_h);
        if((winWidth!=now_w || winHeight!=now_h) && now_w>0 && now_h>0){
            winWidth = now_w;
            winHeight = now_h;
            glViewport(0, 0, now_w, now_h);
		}

		projection.Scale(1.0f , static_cast<float>(now_w) / static_cast<float>(now_h));
		glClearColor(0.0f, 0.5f, 1.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearDepth(1.0);
		userModel.GetRenderer<Rendering::CubismRenderer_OpenGLES2>()->SetMvpMatrix(&projection);
		userModel.GetRenderer<Rendering::CubismRenderer_OpenGLES2>()->DrawModel();

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwDestroyWindow(window);
	glfwTerminate();

	CubismFramework::Dispose();

	return 0;
}
