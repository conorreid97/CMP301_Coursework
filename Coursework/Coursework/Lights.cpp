#include "Lights.h"

Lights::Lights() {
	spotLight = new Light();
	dirLight = new Light();
	shadowLight = new Light();
	shadowLight2 = new Light();
}

void Lights::init(/*HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight, Input* in, bool VSYNC, bool FULL_SCREEN*/) {
	//BaseApplication::init(hinstance, hwnd, screenWidth, screenHeight, in, VSYNC, FULL_SCREEN);
	
	// variables defining shadow map
	sceneWidth = 100;
	sceneHeight = 100;

	spotLight->setAmbientColour(0.2f, 0.2f, 0.2f, 1.0f);
	spotLight->setDiffuseColour(1.0f, 0.0f, 0.0f, 1.0f);
	spotLight->setPosition(27.0f, 11.0f, 20.0f);
	spotLight->setDirection(1.0f, 0.0f, 0.0f);
	spotLight->setSpecularColour(1.0f, 1.0f, 1.0f, 1.0f);
	spotLight->setSpecularPower(32.0f);
	spotLight_Pos[0] = { spotLight->getPosition().x };
	spotLight_Pos[1] = { spotLight->getPosition().y };
	spotLight_Pos[2] = { spotLight->getPosition().z };

	//light2 = new Light();
	dirLight->setAmbientColour(0.1f, 0.1f, 0.3, 1.0f);
	dirLight->setDiffuseColour(0.0f, 1.0f, 1.0f, 1.0f);
	dirLight->setPosition(20.0f, 10.0f, 7.0f);
	dirLight->setDirection(1.0f, 0.0f, 0.0f);
	dirLight->setSpecularColour(1.0f, 1.0f, 1.0f, 1.0f);
	dirLight->setSpecularPower(32.0f);
	dirLight_Pos[0] = { dirLight->getPosition().x };
	dirLight_Pos[1] = { dirLight->getPosition().y };
	dirLight_Pos[2] = { dirLight->getPosition().z };

	dirLight_Dir[0] = { dirLight->getDirection().x };
	dirLight_Dir[1] = { dirLight->getDirection().y };
	dirLight_Dir[2] = { dirLight->getDirection().z };

	shadowLight->setAmbientColour(0.3f, 0.3f, 0.3f, 1.0f);
	shadowLight->setDiffuseColour(1.0f, 1.0f, 1.0f, 1.0f);
	shadowLight->setDirection(0.0f, -0.5f, 0.7f);
	shadowLight->setPosition(0.f,0.f, -10.f);
	shadowLight->generateOrthoMatrix((float)sceneWidth, (float)sceneHeight, 0.1f, 100.f);

	shadowLight2->setAmbientColour(0.3f, 0.3f, 0.3f, 1.0f);
	shadowLight2->setDiffuseColour(1.0f, 1.0f, 1.0f, 1.0f);
	shadowLight2->setDirection(-0.5f, -0.5f, 0.7f);
	shadowLight2->setPosition(0.0f, 0.0f, -10.f);
	shadowLight2->generateOrthoMatrix((float)sceneWidth, (float)sceneHeight, 0.1f, 100.f);

	shadLight_Dir[0] = { shadowLight->getDirection().x };
	shadLight_Dir[1] = { shadowLight->getDirection().y };
	shadLight_Dir[2] = { shadowLight->getDirection().z };

	shadLight_Pos[0] = { shadowLight->getPosition().x };
	shadLight_Pos[1] = { shadowLight->getPosition().y };
	shadLight_Pos[2] = { shadowLight->getPosition().z };

	shadLight_2_Dir[0] = { shadowLight2->getDirection().x };
	shadLight_2_Dir[1] = { shadowLight2->getDirection().y };
	shadLight_2_Dir[2] = { shadowLight2->getDirection().z };

	shadLight_2_Pos[0] = { shadowLight2->getPosition().x };
	shadLight_2_Pos[1] = { shadowLight2->getPosition().y };
	shadLight_2_Pos[2] = { shadowLight2->getPosition().z };

}

void Lights::update() {
	spotLight->setPosition(spotLight_Pos[0], spotLight_Pos[1], spotLight_Pos[2]);
	dirLight->setPosition(dirLight_Pos[0], dirLight_Pos[1], dirLight_Pos[2]);
	shadowLight->setPosition(shadLight_Pos[0], shadLight_Pos[1], shadLight_Pos[2]);
	shadowLight->setDirection(shadLight_Dir[0], shadLight_Dir[1], shadLight_Dir[2]);
	shadowLight2->setPosition(shadLight_2_Pos[0], shadLight_2_Pos[1], shadLight_2_Pos[2]);
	shadowLight2->setDirection(shadLight_2_Dir[0], shadLight_2_Dir[1], shadLight_2_Dir[2]);
}

