#pragma once
#define PARAMDROPDOWN(PARAM, CAP) \
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
}                                                                      \
		j++;
float bgColor[4];
bool bgAdd = false;
float guiX=0.0f;
const char defaultItem[] = "Default";
const char* currentItem[32] = {defaultItem};
bool guiActive = true;
ImGuiWindowFlags gui_flags;
void guiInit(GLFWwindow* glfwWin){
	bgColor[0]=1.0f;
	bgColor[1]=1.0f;
	bgColor[2]=1.0f;
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

}
void guiRender(){
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
void guiUpdate(){
	guiX+=((int)guiActive-guiX)/8;
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
	ImGui::SetNextWindowPos(ImVec2(-200+(200*guiX),5), ImGuiCond_Always);
	ImGui::SetNextWindowSize(ImVec2(200,winHeight-10), ImGuiCond_Always);
	{
		ImGui::Begin("Shid",NULL,gui_flags);
		ImGui::ColorEdit4("Color",(float*)bgColor);
		ImGui::Checkbox("BG Blend Addition", &bgAdd);
		ImGui::Checkbox("Gui Active", &guiActive);
		ImGui::End();
	}
	ImGui::SetNextWindowPos(ImVec2(winWidth+(-300*guiX),5), ImGuiCond_Always);
	ImGui::SetNextWindowSize(ImVec2(300,winHeight-10), ImGuiCond_Always);
	{	
		ImGui::Begin("Parameters",NULL,gui_flags);
		int j = 0;
		ImGui::BulletText("Override default parameters");
		PARAMDROPDOWN(idAngX,"H. Angle X")
		PARAMDROPDOWN(idAngY,"H. Angle Y")
		PARAMDROPDOWN(idAngZ,"H. Angle Z")
		PARAMDROPDOWN(idAngX,"H. Pos. X")
		PARAMDROPDOWN(idAngY,"H. Pos. Y")
		PARAMDROPDOWN(idAngZ,"H. Pos. Z")
		ImGui::End();
	}
}
