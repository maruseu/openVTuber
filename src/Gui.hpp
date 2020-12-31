#pragma once
#include <stdint.h>
#define PARAMDROPDOWN(PARAM, CAP,OUTPUT) \
if(ImGui::BeginCombo(CAP, currentItem[j])){                                       \
	for(int i = 0; i < Cdi->GetParametersCount(); i++)                            \
	{                                                                             \
		if(ImGui::Selectable(Cdi->GetParametersName(i))){                         \
			PARAM=CubismFramework::GetIdManager()->GetId(Cdi->GetParametersId(i));\
			currentItem[j]=Cdi->GetParametersName(i);                             \
		}                                                                         \
	                                                                              \
	}                                                                             \
	ImGui::EndCombo();                                                            \
}                                                                                 \
j++;                                                                              \
snprintf(currentOutput,sizeof(currentOutput)/sizeof(currentOutput[0]),            \
		"Current output:%.2f",OUTPUT);                                            \
ImGui::BulletText(currentOutput);

#define OFFSETSLIDER(CFGOPT, CAP) \
	tempFloat=CFGOPT;\
			ImGui::DragFloat(CAP,&tempFloat,0.02f,-100.0f,100.0f,"%.2f");\
			CFGOPT=tempFloat;

#define MULTIPLIERSLIDER(CFGOPT, CAP) \
	tempFloat=CFGOPT;\
			ImGui::DragFloat(CAP,&tempFloat,0.02f,0.0f,10.0f,"%.2f");\
			CFGOPT=tempFloat;

float nowScale = 1.0f;
float nowModX = 0.0f;
float nowModY = 0.0f;
float camAlpha = 2.0f;
float camBeta = 0.0f;
float tempFloat;
float bgColor[4];
bool bgAdd = false;
float guiX[16]={0.0f};
const char defaultItem[] = "Default";
const char* currentItem[32] = {defaultItem};
char modLoadBuff[128];
uint16_t panelsOpen;
enum guiPanels{
	panMain = 1,
	panParamOver = 2,
	panInSettings = 4,
	panStats = 8,
	panFour = 16
};

ImGuiWindowFlags gui_flags;
void guiInit(GLFWwindow* glfwWin){
	bgColor[0]=0.1f;
	bgColor[1]=0.3f;
	bgColor[2]=0.8f;
	bgColor[3]=0.0f;
	const char* glsl_version = "#version 130";
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui::StyleColorsLight();
	ImGui_ImplGlfw_InitForOpenGL(glfwWin, true);
	ImGui_ImplOpenGL3_Init(glsl_version);
	gui_flags = ImGuiWindowFlags_NoTitleBar;
	gui_flags |= ImGuiWindowFlags_NoResize;
	gui_flags |= ImGuiWindowFlags_NoMove;
	for(int i = 0; i < 32; i++)
		currentItem[i] = defaultItem;
	panelsOpen = panMain;

}
void guiRender(){
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
void guiUpdate(l2dModel* model, CubismMatrix44* proj){
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
	guiX[0]+=((int)(bool)(panelsOpen&panMain)-guiX[0])/8;
	if(guiX[0]>0.01){
		ImGui::SetNextWindowPos(ImVec2(-200+(200*guiX[0]),5), ImGuiCond_Always);
		ImGui::SetNextWindowSize(ImVec2(200,winHeight-10), ImGuiCond_Always);
		{
			ImGui::Begin("Shid",NULL,gui_flags);
			ImGui::InputText("Model name",modLoadBuff,sizeof(modLoadBuff));
			if(ImGui::Button("Load model")){
				if(LoadAssets("res/", modLoadBuff, model))
					model->GetRenderer<Rendering::CubismRenderer_OpenGLES2>()->SetMvpMatrix(proj);
			}

			ImGui::Text("Camera levels");
			ImGui::SliderFloat("Contrast",&camAlpha,-5.0f,10.0f,"%.2f",0);
			cfg.camalpha = camAlpha;
			ImGui::SliderFloat("Brightness",&camBeta,-20.0f,20.0f,"%.2f",0);
			cfg.cambeta = camBeta;
			char cameraIndex[32];
			snprintf(cameraIndex,sizeof(cameraIndex)/sizeof(cameraIndex[0]),
					"Camera index %d",cfg.camIndex);
			if(ImGui::BeginCombo("Camera", cameraIndex)){
				for(int i = 0; i < 5; i++)
				{
					snprintf(cameraIndex,sizeof(cameraIndex)/sizeof(cameraIndex[0]),
							"Camera index %d",i);
					if(ImGui::Selectable(cameraIndex))
						cfg.camIndex=i;
				}
				ImGui::EndCombo();
			}
			if(ImGui::Button("Toggle Cam Window"))
				cfg.toggleCam=true;
			if(ImGui::Button("Toggle Capturing"))
				det.Exit();
			ImGui::DragFloat("Model Scale",&nowScale,0.2f,0.0f,20.0f,"%.2f",0);
			ImGui::DragFloat("Model X",&nowModX,0.02f,-2.0f,2.0f,"%.2f",0);
			ImGui::DragFloat("Model Y",&nowModY,0.02f,-2.0f,2.0f,"%.2f",0);
			ImGui::ColorEdit4("Color",(float*)bgColor);
			ImGui::Checkbox("BG Blend Addition", &bgAdd);
			if(ImGui::Button("Parameters Override"))
				if(panelsOpen&panParamOver) panelsOpen&=~panParamOver;
				else {panelsOpen=panMain;
					panelsOpen|=panParamOver;
				}
			if(ImGui::Button("Inputs Setting"))
				if(panelsOpen&panInSettings) panelsOpen&=~panInSettings;
				else {panelsOpen=panMain;
					 panelsOpen|=panInSettings;
				}
			ImGui::End();
		}
	}
	guiX[1]+=((int)(bool)(panelsOpen&panParamOver)-guiX[1])/8;
	if(guiX[1]>0.01){
		ImGui::SetNextWindowPos(ImVec2(winWidth+(-300*guiX[1]),5), ImGuiCond_Always);
		ImGui::SetNextWindowSize(ImVec2(300,winHeight-10), ImGuiCond_Always);
		{	
			ImGui::Begin("Parameters",NULL,gui_flags);
			int j = 0;
			char currentOutput[64];
			ImGui::Text("Override default parameters");
			PARAMDROPDOWN(idAngX,"H. Angle X",(det.angX+cfg.angXOff)*cfg.angXM)
			PARAMDROPDOWN(idAngY,"H. Angle Y",(det.angY+cfg.angYOff)*cfg.angYM)
			PARAMDROPDOWN(idAngZ,"H. Angle Z",(det.angZ+cfg.angZOff)*cfg.angZM)
			PARAMDROPDOWN(idPosX,"H. Pos. X",(det.posX+cfg.posXOff)*cfg.posXM)
			PARAMDROPDOWN(idPosY,"H. Pos. Y",(det.posY+cfg.posYOff)*cfg.posYM)
			PARAMDROPDOWN(idPosZ,"H. Pos. Z",(det.posZ+cfg.posZOff)*cfg.posZM)
			PARAMDROPDOWN(idEyeOpenL,"Eye Open L",det.eyeOpenL*8 - 2)
			PARAMDROPDOWN(idEyeOpenR,"Eye Open R",det.eyeOpenR*8 - 2)
			PARAMDROPDOWN(idEyePosX,"Eye Pos. X",0.0f)
			PARAMDROPDOWN(idEyePosY,"Eye Pos. Y",0.0f)
			PARAMDROPDOWN(idMouthOpen,"Mouth Open",det.mouthOpen)
			ImGui::End();
		}
	}
	guiX[2]+=((int)(bool)(panelsOpen&panInSettings)-guiX[2])/8;
	if(guiX[2]>0.01){
		ImGui::SetNextWindowPos(ImVec2(winWidth+(-300*guiX[2]),5), ImGuiCond_Always);
		ImGui::SetNextWindowSize(ImVec2(300,winHeight-10), ImGuiCond_Always);
		{	
			ImGui::Begin("Inputs",NULL,gui_flags);
//	double mouthThr;
//	double mouthEx;
			ImGui::Text("Offsets");
			OFFSETSLIDER(cfg.angXOff,"H. Angle X")
			OFFSETSLIDER(cfg.angYOff,"H. Angle Y")
			OFFSETSLIDER(cfg.angZOff,"H. Angle Z")
			OFFSETSLIDER(cfg.posXOff,"H. Pos. X")
			OFFSETSLIDER(cfg.posYOff,"H. Pos. Y")
			OFFSETSLIDER(cfg.posZOff,"H. Pos. Z")
			if(ImGui::Button("Auto-calibrate Offsets")){
				cfg.angXOff = det.angX*-1;
				cfg.angYOff = det.angY*-1;
				cfg.angZOff = det.angZ*-1;
				cfg.posXOff = det.posX*-1;
				cfg.posYOff = det.posY*-1;
				cfg.posZOff = det.posZ*-1;
			}
			ImGui::Text("Sensitivity");
			MULTIPLIERSLIDER(cfg.angXM,"H. Angle X ")
			MULTIPLIERSLIDER(cfg.angYM,"H. Angle Y ")
			MULTIPLIERSLIDER(cfg.angZM,"H. Angle Z ")
			MULTIPLIERSLIDER(cfg.posXM,"H. Pos. X ")
			MULTIPLIERSLIDER(cfg.posYM,"H. Pos. Y ")
			MULTIPLIERSLIDER(cfg.posZM,"H. Pos. Z ")
			tempFloat=cfg.smooth;
			ImGui::Text(" ");
			ImGui::DragFloat("Smoothing",&tempFloat,0.02f,1.0f,50.0f,"%.2f");
			cfg.smooth=tempFloat;

			ImGui::End();
		}
	}
}
