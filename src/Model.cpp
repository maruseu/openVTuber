#include <string.h>
#define STBI_NO_STDIO
#define STBI_ONLY_PNG
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "Model.hpp"

Detector det;
ICubismModelSetting* modSetting = NULL;
CubismCdiJson* Cdi;
const Csm::CubismId* idAngX;
const Csm::CubismId* idAngY;
const Csm::CubismId* idAngZ;
const Csm::CubismId* idPosX;
const Csm::CubismId* idPosY;
const Csm::CubismId* idPosZ;
const Csm::CubismId* idEyePosX;
const Csm::CubismId* idEyePosY;
const Csm::CubismId* idMouthOpen;
const Csm::CubismId* idEyeOpenL;
const Csm::CubismId* idEyeOpenR;

struct texInfo
{
	GLuint id;
	int width;
	int height;
	char* filename;
};

csmByte* LoadFile(const char* path, size_t* size){
	FILE* file_p = fopen(path,"rb");
	fseek(file_p, 0, SEEK_END);
	*size = ftell(file_p);
	rewind(file_p);
	csmByte* buf = (csmByte*)malloc((*size)*sizeof(csmByte));
	fread(buf, *size, 1, file_p);
	fclose(file_p);
	return (csmByte *)buf;

}
texInfo* TextureFromPng(char * filename)
{
    GLuint textureId;
    int width, height, channels;
    size_t size;
    unsigned char* png;
    unsigned char* address;
    address = LoadFile(filename, &size);

    png = stbi_load_from_memory(
        address,
        static_cast<int>(size),
        &width,
        &height,
        &channels,
        STBI_rgb_alpha);

    glGenTextures(1, &textureId);
    glBindTexture(GL_TEXTURE_2D, textureId);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, png);
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);

    stbi_image_free(png);
	free(address);
	address=NULL;

    texInfo* info = (texInfo*)malloc(sizeof(texInfo));
    if (info)
    {
        info->filename = filename;
        info->width = width;
        info->height = height;
        info->id = textureId;
    }

    return info;

}

void mkStr(size_t bufSize, char * buf, ...){
	va_list args;
	char * currentStr;
	va_start(args, buf);
		currentStr = va_arg(args,char *);
		strncpy(buf, currentStr, bufSize);
		while(strcmp(currentStr,"\0")!=0){
			currentStr = va_arg(args,char *);
			strncat(buf, currentStr, bufSize);
		}
	va_end(args);
}


void LoadAssets(const char* dir, const char* filename, l2dModel * userModel){
	const char json[] = ".model3.json";
	const char cdi[] = ".cdi3.json";
	if(modSetting){
		delete modSetting;
		modSetting = NULL;
	}

	char path[512];
	//Load model setting
	mkStr(sizeof(path),path,dir,filename,"/",filename,json,"\0");
	printf("Loading Model Setting %s\n",path);

	size_t size;
	csmByte* buffer = LoadFile(path,&size);
	modSetting = new CubismModelSettingJson(buffer, size);
	free(buffer);
	buffer = NULL;

	//Load cdi
	mkStr(sizeof(path),path,dir,filename,"/",filename,cdi,"\0");
	printf("Loading Cdi %s\n",path);

	buffer = LoadFile(path,&size);
	Cdi = new CubismCdiJson(buffer, size);
	free(buffer);
	buffer = NULL;

	//Load Model
	mkStr(sizeof(path),path,dir,filename,"/",modSetting->GetModelFileName(),"\0");
	printf("Loading Model File %s\n",path);

	buffer = LoadFile(path, &size);
	userModel->LoadModel(buffer, size);
	free(buffer);
	buffer = NULL;

	userModel->CreateRenderer();
	//Load Textures
	for(unsigned int i = 0; i < modSetting->GetTextureCount();i++){
        if (strcmp(modSetting->GetTextureFileName(i), "") == 0)
        {
            continue;
        }
		mkStr(sizeof(path),path,dir,filename,"/",modSetting->GetTextureFileName(i),"\0");
		printf("Loading Texture %s\n",path);

        texInfo* info = TextureFromPng(path);
        userModel->GetRenderer<Rendering::CubismRenderer_OpenGLES2>()->BindTexture(i, info->id);
	}
    userModel->GetRenderer<Rendering::CubismRenderer_OpenGLES2>()->IsPremultipliedAlpha(false);

	//Load Physics
    if (strcmp(modSetting->GetPhysicsFileName(), ""))
    {
		mkStr(sizeof(path),path,dir,filename,"/",modSetting->GetPhysicsFileName(),"\0");
		printf("Loading Physics %s\n",path);

		buffer = LoadFile(path, &size);

        userModel->LoadPhysics(buffer, size);
    	free(buffer);
    	buffer = NULL;
    }

	//Setup Breath
	{
		userModel->createBreath();
		csmVector<CubismBreath::BreathParameterData> breathParameter;
		breathParameter.PushBack(CubismBreath::BreathParameterData(CubismFramework::GetIdManager()->GetId(ParamBreath), 0.5f, 0.5f, 3.2345f, 0.5f));
		userModel->breath()->SetParameters(breathParameter);
	}


	//Assign Default Parameters
    idAngX = CubismFramework::GetIdManager()->GetId(ParamAngleX);
    idAngY = CubismFramework::GetIdManager()->GetId(ParamAngleY);
    idAngZ = CubismFramework::GetIdManager()->GetId(ParamAngleZ);
    idPosX = CubismFramework::GetIdManager()->GetId(ParamBodyAngleX);
    idPosY = CubismFramework::GetIdManager()->GetId(ParamBodyAngleY);
    idPosZ = CubismFramework::GetIdManager()->GetId(ParamBodyAngleZ);
    idEyePosX = CubismFramework::GetIdManager()->GetId(ParamEyeBallX);
    idEyePosY = CubismFramework::GetIdManager()->GetId(ParamEyeBallY);
    idMouthOpen = CubismFramework::GetIdManager()->GetId(ParamMouthOpenY);
    idEyeOpenL = CubismFramework::GetIdManager()->GetId(ParamEyeLOpen);
    idEyeOpenR = CubismFramework::GetIdManager()->GetId(ParamEyeROpen);
}

void modelUpdate(l2dModel *userModel){
    userModel->model()->LoadParameters();
    userModel->model()->SaveParameters();

	/* these go from -10 to 10 */
    userModel->model()->AddParameterValue(idPosX, (det.posX+cfg.posXOff)*cfg.posXM);
    userModel->model()->AddParameterValue(idPosY, (det.posY+cfg.posYOff)*cfg.posYM);
    userModel->model()->AddParameterValue(idPosZ, (det.posZ+cfg.posZOff)*cfg.posZM);

	/* these 3 goes from -30 to 30 by default */                                               
    userModel->model()->AddParameterValue(idAngX, (det.angX+cfg.angXOff)*cfg.angXM); 
    userModel->model()->AddParameterValue(idAngY, (det.angY+cfg.angYOff)*cfg.angYM);
    userModel->model()->AddParameterValue(idAngZ, (det.angZ+cfg.angZOff)*cfg.angZM);

    userModel->model()->AddParameterValue(idMouthOpen, det.mouthOpen);/* 0 to 1 */

	/* the eye open parameters goes from 0 to 1 however its default value is 1
	 * so the value to be added needs to be between -1 and 0 */
    userModel->model()->AddParameterValue(idEyeOpenL, det.eyeOpenL*6 - 2);
    userModel->model()->AddParameterValue(idEyeOpenR, det.eyeOpenR*6 - 2);

	/* Eyeball position parameter -1 to 1 */
	/* Todo: actually track these values :^) */
    userModel->model()->AddParameterValue(idEyePosX, 0);
    userModel->model()->AddParameterValue(idEyePosY, 0);

    if (userModel->breath())
    {
        userModel->breath()->UpdateParameters(userModel->model(), 1.0f/60.0f);
    }
    if (userModel->physics())
    {
        userModel->physics()->Evaluate(userModel->model(), 1.0f/60.0f);
    }
	userModel->model()->Update();
}
