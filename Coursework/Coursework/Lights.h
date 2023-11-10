#pragma once

#include "DXF.h"	// include dxframework

class Lights /*: public BaseApplication*/
{
public:
	Lights();
	~Lights();
	void init(/*HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight, Input* in, bool VSYNC, bool FULL_SCREEN*/);
	void update();
	
	float spotLight_Pos[3];
	float dirLight_Pos[3];
	float dirLight_Dir[3];


	Light* spotLight;
	Light* dirLight;
	Light* shadowLight;
	Light* shadowLight2;

	int sceneWidth;
	int sceneHeight;
	float shadLight_Dir[3];
	float shadLight_Pos[3];
	float shadLight_2_Dir[3];
	float shadLight_2_Pos[3];
private:


	

};

