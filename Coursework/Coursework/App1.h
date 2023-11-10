// Application.h
#ifndef _APP1_H
#define _APP1_H

// Includes
#include "DXF.h"
#include "TextureShader.h"
#include "ShadowShader.h"
#include "DepthShader.h"
#include "MountainShader.h"
#include "MountainDepthShader.h"
#include "MountainShadowShader.h"
#include "WaterShader.h"
#include "waterDepthShader.h"
#include "WaterShadowShader.h"
#include "ShadowShader.h"
#include "LightShader.h"
#include "HorizontalEDShader.h"
#include "VerticalEDShader.h"

#include "Lights.h"

class App1 : public BaseApplication
{
public:

	App1();
	~App1();
	void init(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight, Input* in, bool VSYNC, bool FULL_SCREEN);

	bool frame();

protected:
	void vertexManipulation(XMMATRIX worldMatrix_, XMMATRIX viewMatrix_, XMMATRIX projectionMatrix_);
	void lightSpheres(XMMATRIX worldMatrix_, XMMATRIX viewMatrix_, XMMATRIX projectionMatrix_);
	void shadowArea(XMMATRIX worldMatrix_, XMMATRIX viewMatrix_, XMMATRIX projectionMatrix_);
	bool render();
	void firstPass();
	void verticalED();
	void horizontalED();
	void depthPass();
	void depthPass2();
	void scene();
	void lightPass();
	void finalPass();
	void gui();

private:
	PlaneMesh* mountainPlaneMesh;
	PlaneMesh* waterPlaneMesh;
	PlaneMesh* shadowAreaFloor;
	AModel* model;
	SphereMesh* sphereMesh;
	SphereMesh* sphereMeshLP;
	CubeMesh* cubeMesh;
	PlaneMesh* testPlaneMesh;

	TextureShader* textureShader;
	ShadowShader* shadowShader;
	DepthShader* depthShader;
	LightShader* lightShader;
	MountainShader* mountainShader;
	MountainDepthShader* mountainDepthShader;
	MountainShadowShader* mountainShadowShader;
	WaterShader* waterShader;
	waterDepthShader* waterDepthShader_;
	WaterShadowShader* waterShadowShader;
	HorizontalEDShader* horizontalEDShader;
	
	
	VerticalEDShader* verticalEDShader;
	

	OrthoMesh* orthoMesh_Shadow;
	OrthoMesh* orthoMesh_Shadow2;
	OrthoMesh* orthoMesh_EdgeDet;
	OrthoMesh* orthoMesh_DownSamp;
	OrthoMesh* orthoMesh_UpSamp;
	
	ShadowMap* shadowMap;
	ShadowMap* shadowMap2;
	
	RenderTexture* renderTexture_DS;
	RenderTexture* horizontalEDTexture;
	RenderTexture* verticalEDTexture;

	ID3D11ShaderResourceView* mountTextures[3];
	ID3D11ShaderResourceView* shadowMapArr_DepthMap[2];
	ShadowMap* shadowMapArr[2];
	Light* lightArr_Shadow[2];
	Light* lightArr[3];

	Lights* lights;
	Light* light;
	
	float lightFrust[4];

	float totalTime = 0.0f;

	float waterAmp_ = 0.5f;
	float waterSpeed_ = 0.5f;
	float waterFreq_ = 0.8f;

	float mountAmp_ = 30.0f;
	float mountSpeed_ = 0.5f;
	float mountFreq_ = 0.2f;

	float nearPlane;
	float farPlane;
	int shadowmapWidth;
	int shadowmapHeight;
	
	bool edgeDet_Toggle;
	bool shadowMap_Toggle;
	bool shadows_Toggle;
	float normal_Toggle;
	bool lights_Toggle;

	int downScale;

	float mountPos[3];
	float cubePos[3];
	float modelPos[3];
	XMFLOAT3 cameraPos;
};

#endif