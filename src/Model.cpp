#include <string.h>
#define STBI_NO_STDIO
#define STBI_ONLY_PNG
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "Model.hpp"

Detector det;
ICubismModelSetting* modSetting = NULL;
const Csm::CubismId* _idParamAngleX;
const Csm::CubismId* _idParamAngleY;
const Csm::CubismId* _idParamAngleZ;
const Csm::CubismId* _idParamBodyAngleX;
const Csm::CubismId* _idParamBodyAngleY;
const Csm::CubismId* _idParamBodyAngleZ;
const Csm::CubismId* _idParamEyeBallX;
const Csm::CubismId* _idParamEyeBallY;
const Csm::CubismId* _idParamMouthOpen;
const Csm::CubismId* _idParamEyeLOpen;
const Csm::CubismId* _idParamEyeROpen;

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
	printf("%s\n", filename);
    address = LoadFile(filename, &size);

    png = stbi_load_from_memory(
        address,
        static_cast<int>(size),
        &width,
        &height,
        &channels,
        STBI_rgb_alpha);
    {

#ifdef PREMULTIPLIED_ALPHA_ENABLE
        unsigned int* fourBytes = reinterpret_cast<unsigned int*>(png);
        for (int i = 0; i < width * height; i++)
        {
            unsigned char* p = png + i * 4;
            fourBytes[i] = Premultiply(p[0], p[1], p[2], p[3]);
        }
#endif
    }

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


void LoadAssets(const char* dir, const char* filename, l2dModel * userModel){
	const char json[] = ".model3.json";
	if(modSetting){
		delete modSetting;
		modSetting = NULL;
	}

	csmSizeInt pathlen = sizeof(char)*
		(strlen(dir)+strlen(filename)+1+strlen(filename)+strlen(json)+1);
	char * path = (char*)malloc(pathlen);
	snprintf(path,pathlen,"%s%s/%s%s",dir,filename,filename,json);


	size_t size;
	csmByte* buffer = LoadFile(path,&size);
	modSetting = new CubismModelSettingJson(buffer, size);
	free(buffer);
	buffer = NULL;
	free(path);
	path = NULL;


	size_t modpathlen = sizeof(char)*
		(strlen(modSetting->GetModelFileName())+strlen(filename)+1+strlen(dir)+1);
	char * modpath = (char *)malloc(modpathlen);
	snprintf(modpath,modpathlen,"%s%s/%s",dir,filename,modSetting->GetModelFileName());
	printf("%s\n",modpath);

	buffer = LoadFile(modpath, &size);
	userModel->LoadModel(buffer, size);
	free(buffer);
	buffer = NULL;
	free(modpath);
	modpath = NULL;


	userModel->CreateRenderer();
	for(unsigned int i = 0; i < modSetting->GetTextureCount();i++){
        if (strcmp(modSetting->GetTextureFileName(i), "") == 0)
        {
            continue;
        }
		size_t texpathlen = sizeof(char)*
			(strlen(dir)+strlen(filename)+1+strlen(modSetting->GetTextureFileName(i))+1);
		char* texpath = (char*)malloc(texpathlen);
		snprintf(texpath,texpathlen,"%s%s/%s",dir,filename,modSetting->GetTextureFileName(i));

        texInfo* info = TextureFromPng(texpath);
		free(texpath);
		texpath = NULL;

        userModel->GetRenderer<Rendering::CubismRenderer_OpenGLES2>()->BindTexture(i, info->id);
	}
#ifdef PREMULTIPLIED_ALPHA_ENABLE
    userModel->GetRenderer<Rendering::CubismRenderer_OpenGLES2>()->IsPremultipliedAlpha(true);
#else
    userModel->GetRenderer<Rendering::CubismRenderer_OpenGLES2>()->IsPremultipliedAlpha(false);
#endif
    if (strcmp(modSetting->GetPhysicsFileName(), ""))
    {
    	size_t phypathlen = sizeof(char)*
    		(strlen(dir)+strlen(filename)+1+strlen(modSetting->GetPhysicsFileName())+1);
    	char* phypath = (char*)malloc(phypathlen);
    	snprintf(phypath,phypathlen,"%s%s/%s",dir,filename,modSetting->GetPhysicsFileName());
    	printf("%s\n",phypath);

		buffer = LoadFile(phypath, &size);

        userModel->LoadPhysics(buffer, size);
    	free(buffer);
    	buffer = NULL;
    }
    _idParamAngleX = CubismFramework::GetIdManager()->GetId(ParamAngleX);
    _idParamAngleY = CubismFramework::GetIdManager()->GetId(ParamAngleY);
    _idParamAngleZ = CubismFramework::GetIdManager()->GetId(ParamAngleZ);
    _idParamBodyAngleX = CubismFramework::GetIdManager()->GetId(ParamBodyAngleX);
    _idParamBodyAngleY = CubismFramework::GetIdManager()->GetId(ParamBodyAngleY);
    _idParamBodyAngleZ = CubismFramework::GetIdManager()->GetId(ParamBodyAngleZ);
    _idParamEyeBallX = CubismFramework::GetIdManager()->GetId(ParamEyeBallX);
    _idParamEyeBallY = CubismFramework::GetIdManager()->GetId(ParamEyeBallY);
    _idParamMouthOpen = CubismFramework::GetIdManager()->GetId(ParamMouthOpenY);
    _idParamEyeLOpen = CubismFramework::GetIdManager()->GetId(ParamEyeLOpen);
    _idParamEyeROpen = CubismFramework::GetIdManager()->GetId(ParamEyeROpen);

}

void modelUpdate(l2dModel *userModel){
    userModel->model()->LoadParameters();
    userModel->model()->SaveParameters();

	/* these go from -10 to 10 */
    userModel->model()->AddParameterValue(_idParamBodyAngleX, (det.posX+cfg.posXOff)*cfg.posXM);
    userModel->model()->AddParameterValue(_idParamBodyAngleY, (det.posY+cfg.posYOff)*cfg.posYM);
    userModel->model()->AddParameterValue(_idParamBodyAngleZ, (det.posZ+cfg.posZOff)*cfg.posZM);

	/* these 3 goes from -30 to 30 by default */                                               
    userModel->model()->AddParameterValue(_idParamAngleX, (det.angX+cfg.angXOff)*cfg.angXM); 
    userModel->model()->AddParameterValue(_idParamAngleY, (det.angY+cfg.angYOff)*cfg.angYM);
    userModel->model()->AddParameterValue(_idParamAngleZ, (det.angZ+cfg.angZOff)*cfg.angZM);

    userModel->model()->AddParameterValue(_idParamMouthOpen, det.mouthOpen);/* 0 to 1 */

	/* the eye open parameters goes from 0 to 1 however its default value is 1
	 * so the value to be added needs to be between -1 and 0 */
    userModel->model()->AddParameterValue(_idParamEyeLOpen, det.eyeOpenL*4 - 1);
    userModel->model()->AddParameterValue(_idParamEyeROpen, det.eyeOpenR*4 - 1);

	/* Eyeball position parameter -1 to 1 */
	/* Todo: actually track these values :^) */
    userModel->model()->AddParameterValue(_idParamEyeBallX, 0);
    userModel->model()->AddParameterValue(_idParamEyeBallY, 0);

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
