// Lab1.cpp
// Lab 1 example, simple coloured triangle mesh
#include "App1.h"

App1::App1()
{
	mountainPlaneMesh = nullptr;
	sphereMesh = nullptr;
	mountainShader = nullptr;
	waterShader = nullptr;
	depthShader = nullptr;
	shadowShader = nullptr;
	lights = nullptr;
	//shadowMap = nullptr;
	totalTime = 0.0f;
}

void App1::init(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight, Input *in, bool VSYNC, bool FULL_SCREEN)
{
	// Call super/parent init function (required!)
	BaseApplication::init(hinstance, hwnd, screenWidth, screenHeight, in, VSYNC, FULL_SCREEN);

	// Initalise scene variables.
	
	// Load texture.
	textureMgr->loadTexture(L"height", L"res/seaBedMap.png");
	textureMgr->loadTexture(L"water", L"res/water.png");
	textureMgr->loadTexture(L"textureMap", L"res/textureMap.png");
	textureMgr->loadTexture(L"grass", L"res/grass.png");
	textureMgr->loadTexture(L"dirt", L"res/dirt.png");
	textureMgr->loadTexture(L"brick", L"res/brick1.dds");

	mountTextures[0] = textureMgr->getTexture(L"textureMap");
	mountTextures[1] = textureMgr->getTexture(L"dirt");
	mountTextures[2] = textureMgr->getTexture(L"grass");

	// Create Mesh object 
	model = new AModel(renderer->getDevice(), "res/teapot.obj");
	mountainPlaneMesh = new PlaneMesh(renderer->getDevice(), renderer->getDeviceContext());
	waterPlaneMesh = new PlaneMesh(renderer->getDevice(), renderer->getDeviceContext());
	shadowAreaFloor = new PlaneMesh(renderer->getDevice(), renderer->getDeviceContext());
	sphereMesh = new SphereMesh(renderer->getDevice(), renderer->getDeviceContext());
	sphereMeshLP = new SphereMesh(renderer->getDevice(), renderer->getDeviceContext());
	cubeMesh = new CubeMesh(renderer->getDevice(), renderer->getDeviceContext());
	testPlaneMesh = new PlaneMesh(renderer->getDevice(), renderer->getDeviceContext());
	orthoMesh_Shadow = new OrthoMesh(renderer->getDevice(), renderer->getDeviceContext(), screenWidth / 4, screenHeight / 4, screenWidth / 4, screenHeight / 4);
	orthoMesh_Shadow2 = new OrthoMesh(renderer->getDevice(), renderer->getDeviceContext(), screenWidth / 4, screenHeight / 4, screenWidth / -4, screenHeight / -4);
	orthoMesh_EdgeDet = new OrthoMesh(renderer->getDevice(), renderer->getDeviceContext(), screenWidth, screenHeight);	// Full screen size
	orthoMesh_DownSamp = new OrthoMesh(renderer->getDevice(), renderer->getDeviceContext(), screenWidth / 2, screenHeight / 2);
	orthoMesh_UpSamp = new OrthoMesh(renderer->getDevice(), renderer->getDeviceContext(), screenWidth * 2, screenHeight * 2);

	// create shaders
	textureShader = new TextureShader(renderer->getDevice(), hwnd);
	depthShader = new DepthShader(renderer->getDevice(), hwnd);
	shadowShader = new ShadowShader(renderer->getDevice(), hwnd);
	mountainShader = new MountainShader(renderer->getDevice(), hwnd);
	mountainDepthShader = new MountainDepthShader(renderer->getDevice(), hwnd);
	mountainShadowShader = new MountainShadowShader(renderer->getDevice(), hwnd);
	waterShader = new WaterShader(renderer->getDevice(), hwnd);
	waterDepthShader_ = new waterDepthShader(renderer->getDevice(), hwnd);
	waterShadowShader = new WaterShadowShader(renderer->getDevice(), hwnd);
	lightShader = new LightShader(renderer->getDevice(), hwnd);
	horizontalEDShader = new HorizontalEDShader(renderer->getDevice(), hwnd);
	verticalEDShader = new VerticalEDShader(renderer->getDevice(), hwnd);

	lights = new Lights();

	downScale = 2;

	//Initialise required render textures. We need a RT for the first pass at the scene (no edge detection), one to store the horizontal edge detection, and one to store the combination of horiontal and vertical edge detection.
	renderTexture_DS = new RenderTexture(renderer->getDevice(), screenWidth / downScale, screenHeight / downScale, SCREEN_NEAR, SCREEN_DEPTH);
	horizontalEDTexture = new RenderTexture(renderer->getDevice(), screenWidth, screenHeight, SCREEN_NEAR, SCREEN_DEPTH);
	verticalEDTexture = new RenderTexture(renderer->getDevice(), screenWidth, screenHeight, SCREEN_NEAR, SCREEN_DEPTH);

	edgeDet_Toggle = false;
	shadowMap_Toggle = true;
	shadows_Toggle = false;
	normal_Toggle = 0.0f;
	// Variables for defining shadow map
	shadowmapWidth = 2024;
	shadowmapHeight = 2024;
	
	nearPlane = 0.1f;
	farPlane = 100.f;
	// This is your shadow map
	shadowMap = new ShadowMap(renderer->getDevice(), shadowmapWidth, shadowmapHeight);
	shadowMap2 = new ShadowMap(renderer->getDevice(), shadowmapWidth, shadowmapHeight);
	
	// Confirgure directional light
	lights->init();

	lightFrust[0] = { (float)lights->sceneWidth };
	lightFrust[1] = { (float)lights->sceneHeight };
	lightFrust[2] = { nearPlane };
	lightFrust[3] = { farPlane };
	//mountPos[0] = mountainPlaneMesh
	camera->setPosition(10, 20, 10);
	
	lightArr_Shadow[0] = lights->shadowLight;
	lightArr_Shadow[1] = lights->shadowLight2;


	lightArr[0] = lights->spotLight;
	lightArr[1] = lights->dirLight;
	lightArr[2] = lights->shadowLight;

	modelPos[0] = -50.0f;
	modelPos[1] = 16.0f;
	modelPos[2] = 30.0f;
}


App1::~App1()
{
	// Run base application deconstructor
	BaseApplication::~BaseApplication();

	// Release the Direct3D object.
		// Release the Direct3D object.
	if (mountainPlaneMesh)
	{
		delete mountainPlaneMesh;
		mountainPlaneMesh = 0;
	}
	if (sphereMesh)
	{
		delete sphereMesh;
		sphereMesh = 0;
	}
	if (mountainShader)
	{
		delete mountainShader;
		mountainShader = 0;
	}
	if (waterShader)
	{
		delete waterShader;
		waterShader = 0;
	}
}

bool App1::frame()
{
	bool result;

	result = BaseApplication::frame();
	if (!result)
	{
		return false;
	}
	
	// accumulates time since the program strted
	totalTime += timer->getTime();

	// Render the graphics.
	result = render();
	if (!result)
	{
		return false;
	}

	return true;
}

void App1::vertexManipulation(XMMATRIX worldMatrix_, XMMATRIX viewMatrix_, XMMATRIX projectionMatrix_) {

	// shadows for the water and mountain
	if (shadows_Toggle == true) {

		mountainPlaneMesh->sendData(renderer->getDeviceContext());
		mountainShadowShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix_, viewMatrix_, projectionMatrix_,
			textureMgr->getTexture(L"dirt"), shadowMap->getDepthMapSRV(), textureMgr->getTexture(L"height"), lights->shadowLight, totalTime);
		mountainShadowShader->render(renderer->getDeviceContext(), mountainPlaneMesh->getIndexCount());

		renderer->setAlphaBlending(true);
		worldMatrix_ = XMMatrixTranslation(0.f, 5.f, 0.f);
		waterPlaneMesh->sendData(renderer->getDeviceContext());
		waterShadowShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix_, viewMatrix_, projectionMatrix_,
			textureMgr->getTexture(L"water"), shadowMap->getDepthMapSRV(), lights->shadowLight, totalTime);
		waterShadowShader->render(renderer->getDeviceContext(), waterPlaneMesh->getIndexCount());
		renderer->setAlphaBlending(false);
	}
	// Send geometry data, set shader parameters, render object with shader
	worldMatrix_ = XMMatrixTranslation(mountPos[0], mountPos[1], mountPos[2]);
	mountainPlaneMesh->sendData(renderer->getDeviceContext());
	mountainShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix_, viewMatrix_, projectionMatrix_, textureMgr->getTexture(L"textureMap"), textureMgr->getTexture(L"dirt"), 
										textureMgr->getTexture(L"grass"), textureMgr->getTexture(L"height"), lightArr, totalTime, cameraPos);
	mountainShader->render(renderer->getDeviceContext(), mountainPlaneMesh->getIndexCount());

	renderer->setAlphaBlending(true);
	worldMatrix_ = XMMatrixTranslation(0.f, 5.f, 0.f);
	waterPlaneMesh->sendData(renderer->getDeviceContext());
	waterShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix_, viewMatrix_, projectionMatrix_, textureMgr->getTexture(L"water"), lightArr, totalTime, cameraPos);
	waterShader->render(renderer->getDeviceContext(), waterPlaneMesh->getIndexCount());
	renderer->setAlphaBlending(false);

}


void App1::scene() {
	XMMATRIX worldMatrix, cubeWorldMatrix, viewMatrix, projectionMatrix;

	// Clear the scene. (default blue colour)
	renderer->beginScene(0.39f, 0.58f, 0.92f, 1.0f);

	// Generate the view matrix based on the camera's position.
	camera->update();
	cameraPos = camera->getPosition();

	// Get the world, view, projection, and ortho matrices from the camera and Direct3D objects.
	worldMatrix = renderer->getWorldMatrix();
	cubeWorldMatrix = renderer->getWorldMatrix();
	viewMatrix = camera->getViewMatrix();
	projectionMatrix = renderer->getProjectionMatrix();

	if (edgeDet_Toggle == false) {

		vertexManipulation(worldMatrix, viewMatrix, projectionMatrix);
	}

	// render spheres 
	lightSpheres(worldMatrix, viewMatrix, projectionMatrix);

	shadowArea(worldMatrix, viewMatrix, projectionMatrix);
	
	// Reset the render target back to the original back buffer and not the render to texture anymore.
	renderer->setBackBufferRenderTarget();
	
}

bool App1::render()
{
	firstPass();
	// Apply horizontal edge detection stage
	horizontalED();
	// Apply vertical edge detection to the horizontal edge detection stage
	verticalED();

	depthPass();

	depthPass2();

	finalPass();
	return true;
}

void App1::gui()
{
	// Force turn off unnecessary shader stages.
	renderer->getDeviceContext()->GSSetShader(NULL, NULL, 0);
	renderer->getDeviceContext()->HSSetShader(NULL, NULL, 0);
	renderer->getDeviceContext()->DSSetShader(NULL, NULL, 0);

	// Build UI
	ImGui::Text("FPS: %.2f", timer->getFPS());
	ImGui::Checkbox("Wireframe mode", &wireframeToggle);

	// controls for waves
	ImGui::SliderFloat("Amplitude", &waterAmp_, 0.1f, 1.0f, "%.3f", 1.0f);
	ImGui::SliderFloat("Speed", &waterSpeed_, 0.1f, 2.0f, "%.3f", 1.0f);
	ImGui::SliderFloat("Frequency", &waterFreq_, 0.1f, 1.0f, "%.3f", 1.0f);

	ImGui::SliderFloat("Mountain Amplitude", &mountAmp_, -50.0f, 50.0f, "%.3f", 1.0f);
	ImGui::SliderFloat3("Mountain Pos ", mountPos, -200.0f, 200.0f);
	
	ImGui::SliderFloat3("Cube Pos ", cubePos, -20.0f, 20.0f);

	ImGui::SliderFloat3("Model Pos ", modelPos, -100.0f, 100.0f);

	ImGui::Text("Light Pos: ");
	ImGui::SliderFloat3("Light Pos: ", lights->spotLight_Pos, -100.1f, 50.f);
	ImGui::SliderFloat3("Light Pos2: ", lights->dirLight_Pos, -100.1f, 50.f);
	// shadow light
	ImGui::SliderFloat3("Shadow Light Pos: ", lights->shadLight_Pos, -100.1f, 50.f);
	ImGui::SliderFloat3("Shadow Light Dir: ", lights->shadLight_Dir, -100.1f, 180.f);
	
	ImGui::SliderFloat3("Light Position2: ", lights->shadLight_2_Pos, -100.1f, 50.f);
	ImGui::SliderFloat3("Light Direction2: ", lights->shadLight_2_Dir, -100.1f, 10.f, "%.9f", 1.0f);
	
	ImGui::SliderFloat4("Scene Width", lightFrust, 0.1f, 150.f);


	ImGui::CollapsingHeader("test", true);

	ImGui::SliderInt("post process up/down scaling", &downScale, 1, 6);
	
	ImGui::Checkbox("Edge Detection", &edgeDet_Toggle);
	ImGui::Checkbox("Shadow Map", &shadowMap_Toggle);
	ImGui::Checkbox("Shadows On/Off", &shadows_Toggle);
	ImGui::SliderFloat("Mountain Normals", &normal_Toggle, 0.0f, 1.0f);

	// updates the variables to send to the vs
	waterShader->setAmplitude(waterAmp_);
	waterShader->setSpeed(waterSpeed_);
	waterShader->setFrequency(waterFreq_);
	waterDepthShader_->setAmplitude(waterAmp_);
	waterDepthShader_->setSpeed(waterSpeed_);
	waterDepthShader_->setFrequency(waterFreq_);
	waterShadowShader->setAmplitude(waterAmp_);
	waterShadowShader->setSpeed(waterSpeed_);
	waterShadowShader->setFrequency(waterFreq_);

	mountainShader->setAmplitude(mountAmp_);
	mountainDepthShader->setAmplitude(mountAmp_);
	mountainShadowShader->setAmplitude(mountAmp_);
	mountainShader->setNormalToggle(normal_Toggle);


	// Render UI
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

void App1::firstPass() {

	renderTexture_DS->setRenderTarget(renderer->getDeviceContext());
	renderTexture_DS->clearRenderTarget(renderer->getDeviceContext(), 0.0f, 0.0f, 1.0f, 1.0f);

	// Get matrices
	camera->update();
	XMMATRIX worldMatrix = renderer->getWorldMatrix();
	XMMATRIX viewMatrix = camera->getViewMatrix();
	XMMATRIX projectionMatrix = renderer->getProjectionMatrix();

	// renders water and mountain
	vertexManipulation(worldMatrix, viewMatrix, projectionMatrix);
	// render spheres at lights positions
	lightSpheres(worldMatrix, viewMatrix, projectionMatrix);

	shadowArea(worldMatrix, viewMatrix, projectionMatrix);

	// Reset the render target back to the original back buffer and not the render to texture anymore.
	renderer->setBackBufferRenderTarget();
}

void App1::horizontalED()
{
	XMMATRIX worldMatrix, baseViewMatrix, orthoMatrix;

	float screenSizeX = (float)horizontalEDTexture->getTextureWidth();
	horizontalEDTexture->setRenderTarget(renderer->getDeviceContext());
	horizontalEDTexture->clearRenderTarget(renderer->getDeviceContext(), 1.0f, 1.0f, 0.0f, 1.0f);

	worldMatrix = renderer->getWorldMatrix();
	baseViewMatrix = camera->getOrthoViewMatrix();
	orthoMatrix = horizontalEDTexture->getOrthoMatrix();

	// Render for Horizontal edge detection
	renderer->setZBuffer(false);
	orthoMesh_DownSamp->sendData(renderer->getDeviceContext());
	horizontalEDShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, baseViewMatrix, orthoMatrix, renderTexture_DS->getShaderResourceView(), screenSizeX);
	horizontalEDShader->render(renderer->getDeviceContext(), orthoMesh_DownSamp->getIndexCount());
	renderer->setZBuffer(true);

	// Reset the render target back to the original back buffer and not the render to texture anymore.
	renderer->setBackBufferRenderTarget();
}

void App1::verticalED()
{
	XMMATRIX worldMatrix, baseViewMatrix, orthoMatrix;

	float screenSizeY = (float)verticalEDTexture->getTextureHeight();
	verticalEDTexture->setRenderTarget(renderer->getDeviceContext());
	verticalEDTexture->clearRenderTarget(renderer->getDeviceContext(), 0.0f, 1.0f, 1.0f, 1.0f);

	worldMatrix = renderer->getWorldMatrix();
	baseViewMatrix = camera->getOrthoViewMatrix();
	// Get the ortho matrix from the render to texture since texture has different dimensions being that it is smaller.
	orthoMatrix = verticalEDTexture->getOrthoMatrix();

	// Render for Vertical edge detection
	renderer->setZBuffer(false);
	orthoMesh_EdgeDet->sendData(renderer->getDeviceContext());
	verticalEDShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, baseViewMatrix, orthoMatrix, horizontalEDTexture->getShaderResourceView(), screenSizeY);
	verticalEDShader->render(renderer->getDeviceContext(), orthoMesh_EdgeDet->getIndexCount());
	renderer->setZBuffer(true);

	// Reset the render target back to the original back buffer and not the render to texture anymore.
	renderer->setBackBufferRenderTarget();
}

void App1::depthPass() {
	// Set the render target to be the render to texture.
	
	shadowMap->BindDsvAndSetNullRenderTarget(renderer->getDeviceContext());

	// get the world, view, and projection matrices from the camera and d3d objects.
	lights->shadowLight->generateViewMatrix();
	XMMATRIX lightViewMatrix = lights->shadowLight->getViewMatrix();
	XMMATRIX lightProjectionMatrix = lights->shadowLight->getOrthoMatrix();
	XMMATRIX worldMatrix = renderer->getWorldMatrix();



	mountainPlaneMesh->sendData(renderer->getDeviceContext());
	mountainDepthShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, lightViewMatrix, lightProjectionMatrix, totalTime);
	mountainDepthShader->render(renderer->getDeviceContext(), mountainPlaneMesh->getIndexCount());

	waterPlaneMesh->sendData(renderer->getDeviceContext());
	waterDepthShader_->setShaderParameters(renderer->getDeviceContext(), worldMatrix, lightViewMatrix, lightProjectionMatrix, totalTime);
	waterDepthShader_->render(renderer->getDeviceContext(), waterPlaneMesh->getIndexCount());

	cubeMesh->sendData(renderer->getDeviceContext());
	depthShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, lightViewMatrix, lightProjectionMatrix);
	depthShader->render(renderer->getDeviceContext(), cubeMesh->getIndexCount());

	worldMatrix = XMMatrixTranslation(lights->spotLight_Pos[0], lights->spotLight_Pos[1], lights->spotLight_Pos[2]);
	sphereMesh->sendData(renderer->getDeviceContext());
	depthShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, lightViewMatrix, lightProjectionMatrix);
	depthShader->render(renderer->getDeviceContext(), sphereMesh->getIndexCount());

	worldMatrix = XMMatrixTranslation(lights->dirLight_Pos[0], lights->dirLight_Pos[1], lights->dirLight_Pos[2]);
	sphereMesh->sendData(renderer->getDeviceContext());
	depthShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, lightViewMatrix, lightProjectionMatrix);
	depthShader->render(renderer->getDeviceContext(), sphereMesh->getIndexCount());

	// sphere for shadow light
	worldMatrix = XMMatrixTranslation(lights->shadLight_Pos[0], lights->shadLight_Pos[1], lights->shadLight_Pos[2]);
	sphereMesh->sendData(renderer->getDeviceContext());
	depthShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, lightViewMatrix, lightProjectionMatrix);
	depthShader->render(renderer->getDeviceContext(), sphereMesh->getIndexCount());


	// shadow area
	worldMatrix = XMMatrixTranslation(-100.f, 0.f, -10.f);
	// Render floor
	shadowAreaFloor->sendData(renderer->getDeviceContext());
	depthShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, lightViewMatrix, lightProjectionMatrix);
	depthShader->render(renderer->getDeviceContext(), shadowAreaFloor->getIndexCount());

	worldMatrix = XMMatrixTranslation(modelPos[0], modelPos[1], modelPos[2]);
	XMMATRIX scaleMatrix = XMMatrixScaling(0.5f, 0.5f, 0.5f);
	worldMatrix = XMMatrixMultiply(worldMatrix, scaleMatrix);
	model->sendData(renderer->getDeviceContext());
	depthShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, lightViewMatrix, lightProjectionMatrix);
	depthShader->render(renderer->getDeviceContext(), model->getIndexCount());

	// Render sphere
	XMMATRIX newworldMatrix = XMMatrixTranslation(20.f, 3.f, 5.f);
	sphereMesh->sendData(renderer->getDeviceContext());
	depthShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, lightViewMatrix, lightProjectionMatrix);
	depthShader->render(renderer->getDeviceContext(), sphereMesh->getIndexCount());

	// Render cube
	newworldMatrix = XMMatrixTranslation(lights->shadLight_Pos[0], lights->shadLight_Pos[1], lights->shadLight_Pos[2]);
	cubeMesh->sendData(renderer->getDeviceContext());
	depthShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, lightViewMatrix, lightProjectionMatrix);
	depthShader->render(renderer->getDeviceContext(), cubeMesh->getIndexCount());

	// render sphere at lights position
	newworldMatrix = XMMatrixTranslation(lights->shadLight_2_Pos[0], lights->shadLight_2_Pos[1], lights->shadLight_2_Pos[2]);
	sphereMeshLP->sendData(renderer->getDeviceContext());
	depthShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, lightViewMatrix, lightProjectionMatrix);
	depthShader->render(renderer->getDeviceContext(), sphereMeshLP->getIndexCount());

	
	renderer->setBackBufferRenderTarget();
	renderer->resetViewport();
}

void App1::depthPass2() {

	shadowMap2->BindDsvAndSetNullRenderTarget(renderer->getDeviceContext());


	// get the world, view, and projection matrices from the camera and d3d objects.
	//lights->getLight()->generateViewMatrix();
	lights->shadowLight2->generateViewMatrix();
	XMMATRIX lightViewMatrix = lights->shadowLight2->getViewMatrix();
	XMMATRIX lightProjectionMatrix = lights->shadowLight2->getOrthoMatrix();
	XMMATRIX worldMatrix = renderer->getWorldMatrix();

	mountainPlaneMesh->sendData(renderer->getDeviceContext());
	mountainDepthShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, lightViewMatrix, lightProjectionMatrix, totalTime);
	mountainDepthShader->render(renderer->getDeviceContext(), mountainPlaneMesh->getIndexCount());

	waterPlaneMesh->sendData(renderer->getDeviceContext());
	waterDepthShader_->setShaderParameters(renderer->getDeviceContext(), worldMatrix, lightViewMatrix, lightProjectionMatrix, totalTime);
	waterDepthShader_->render(renderer->getDeviceContext(), waterPlaneMesh->getIndexCount());

	cubeMesh->sendData(renderer->getDeviceContext());
	depthShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, lightViewMatrix, lightProjectionMatrix);
	depthShader->render(renderer->getDeviceContext(), cubeMesh->getIndexCount());

	worldMatrix = XMMatrixTranslation(lights->spotLight_Pos[0], lights->spotLight_Pos[1], lights->spotLight_Pos[2]);
	sphereMesh->sendData(renderer->getDeviceContext());
	depthShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, lightViewMatrix, lightProjectionMatrix);
	depthShader->render(renderer->getDeviceContext(), sphereMesh->getIndexCount());

	worldMatrix = XMMatrixTranslation(lights->dirLight_Pos[0], lights->dirLight_Pos[1], lights->dirLight_Pos[2]);
	sphereMesh->sendData(renderer->getDeviceContext());
	depthShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, lightViewMatrix, lightProjectionMatrix);
	depthShader->render(renderer->getDeviceContext(), sphereMesh->getIndexCount());

	// sphere for shadow light
	worldMatrix = XMMatrixTranslation(lights->shadLight_Pos[0], lights->shadLight_Pos[1], lights->shadLight_Pos[2]);
	sphereMesh->sendData(renderer->getDeviceContext());
	depthShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, lightViewMatrix, lightProjectionMatrix);
	depthShader->render(renderer->getDeviceContext(), sphereMesh->getIndexCount());


	// shadow area

	worldMatrix = XMMatrixTranslation(-100.f, 0.f, -10.f);
	// Render floor
	shadowAreaFloor->sendData(renderer->getDeviceContext());
	depthShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, lightViewMatrix, lightProjectionMatrix);
	depthShader->render(renderer->getDeviceContext(), shadowAreaFloor->getIndexCount());

	worldMatrix = XMMatrixTranslation(modelPos[0], modelPos[1], modelPos[2]);
	XMMATRIX scaleMatrix = XMMatrixScaling(0.5f, 0.5f, 0.5f);
	worldMatrix = XMMatrixMultiply(worldMatrix, scaleMatrix);
	model->sendData(renderer->getDeviceContext());
	depthShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, lightViewMatrix, lightProjectionMatrix);
	depthShader->render(renderer->getDeviceContext(), model->getIndexCount());

	// Render sphere
	XMMATRIX newworldMatrix = XMMatrixTranslation(20.f, 3.f, 5.f);
	sphereMesh->sendData(renderer->getDeviceContext());
	depthShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, lightViewMatrix, lightProjectionMatrix);
	depthShader->render(renderer->getDeviceContext(), sphereMesh->getIndexCount());

	// Render cube
	newworldMatrix = XMMatrixTranslation(lights->shadLight_Pos[0], lights->shadLight_Pos[1], lights->shadLight_Pos[2]);
	cubeMesh->sendData(renderer->getDeviceContext());
	depthShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, lightViewMatrix, lightProjectionMatrix);
	depthShader->render(renderer->getDeviceContext(), cubeMesh->getIndexCount());

	// render sphere at lights position
	newworldMatrix = XMMatrixTranslation(lights->shadLight_2_Pos[0], lights->shadLight_2_Pos[1], lights->shadLight_2_Pos[2]);
	sphereMeshLP->sendData(renderer->getDeviceContext());
	depthShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, lightViewMatrix, lightProjectionMatrix);
	depthShader->render(renderer->getDeviceContext(), sphereMeshLP->getIndexCount());


	renderer->setBackBufferRenderTarget();
	renderer->resetViewport();
}

void App1::finalPass() {
	
	scene();

	XMMATRIX worldMatrix = renderer->getWorldMatrix();
	XMMATRIX cubeWorldMatrix = renderer->getWorldMatrix();
	// render to texture
	renderer->setZBuffer(false);
	XMMATRIX orthoMatrix = renderer->getOrthoMatrix();  // ortho matrix for 2D rendering
	XMMATRIX orthoViewMatrix = camera->getOrthoViewMatrix();


	if (edgeDet_Toggle == true) {
		orthoMesh_UpSamp->sendData(renderer->getDeviceContext());
		textureShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, orthoViewMatrix, orthoMatrix, verticalEDTexture->getShaderResourceView());
		textureShader->render(renderer->getDeviceContext(), orthoMesh_UpSamp->getIndexCount());

	}
	if (shadowMap_Toggle == true) {
		orthoMesh_Shadow->sendData(renderer->getDeviceContext());
		textureShader->setShaderParameters(renderer->getDeviceContext(), cubeWorldMatrix, orthoViewMatrix, orthoMatrix, shadowMap->getDepthMapSRV());
		textureShader->render(renderer->getDeviceContext(), orthoMesh_Shadow->getIndexCount());
		
		orthoMesh_Shadow2->sendData(renderer->getDeviceContext());
		textureShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, orthoViewMatrix, orthoMatrix, shadowMap2->getDepthMapSRV());
		textureShader->render(renderer->getDeviceContext(), orthoMesh_Shadow2->getIndexCount());
	}
	
	renderer->setZBuffer(true);

	// Render GUI
	gui();
	lights->update();

	lights->shadowLight->setDirection(lights->shadLight_Dir[0], lights->shadLight_Dir[1], lights->shadLight_Dir[2]);
	lights->shadowLight->setPosition(lights->shadLight_Pos[0], lights->shadLight_Pos[1], lights->shadLight_Pos[2]);
	lights->shadowLight->generateOrthoMatrix(lightFrust[0], lightFrust[1], lightFrust[2], lightFrust[3]);

	lights->shadowLight2->setDirection(lights->shadLight_2_Dir[0], lights->shadLight_2_Dir[1], lights->shadLight_2_Dir[2]);
	lights->shadowLight2->setPosition(lights->shadLight_2_Pos[0], lights->shadLight_2_Pos[1], lights->shadLight_2_Pos[2]);
	lights->shadowLight2->generateOrthoMatrix(lightFrust[0], lightFrust[1], lightFrust[2], lightFrust[3]);
	
	// Present the rendered scene to the screen.
	renderer->endScene();
}


void App1::lightSpheres(XMMATRIX worldMatrix_, XMMATRIX viewMatrix_, XMMATRIX projectionMatrix_) {

	worldMatrix_ = XMMatrixTranslation(lights->spotLight_Pos[0], lights->spotLight_Pos[1], lights->spotLight_Pos[2]);
	sphereMesh->sendData(renderer->getDeviceContext());
	lightShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix_, viewMatrix_, projectionMatrix_, textureMgr->getTexture(L"brick"), lights->spotLight, lights->dirLight, cameraPos);
	lightShader->render(renderer->getDeviceContext(), sphereMesh->getIndexCount());

	worldMatrix_ = XMMatrixTranslation(lights->dirLight_Pos[0], lights->dirLight_Pos[1], lights->dirLight_Pos[2]);
	sphereMesh->sendData(renderer->getDeviceContext());
	lightShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix_, viewMatrix_, projectionMatrix_, textureMgr->getTexture(L"brick"), lights->spotLight, lights->dirLight, cameraPos);
	lightShader->render(renderer->getDeviceContext(), sphereMesh->getIndexCount());


	// sphere for shadow light

	worldMatrix_ = XMMatrixTranslation(lights->shadLight_Pos[0], lights->shadLight_Pos[1], lights->shadLight_Pos[2]);
	cubeMesh->sendData(renderer->getDeviceContext());
	shadowShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix_, viewMatrix_, projectionMatrix_, textureMgr->getTexture(L"water"),
		shadowMap->getDepthMapSRV(), shadowMap2->getDepthMapSRV(), lights->shadowLight, lights->shadowLight2);
	shadowShader->render(renderer->getDeviceContext(), cubeMesh->getIndexCount());

	// render sphere at lights position
	worldMatrix_ = XMMatrixTranslation(lights->shadLight_2_Pos[0], lights->shadLight_2_Pos[1], lights->shadLight_2_Pos[2]);
	sphereMeshLP->sendData(renderer->getDeviceContext());
	shadowShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix_, viewMatrix_, projectionMatrix_, textureMgr->getTexture(L"grass"),
		shadowMap->getDepthMapSRV(), shadowMap2->getDepthMapSRV(), lights->shadowLight, lights->shadowLight2);
	shadowShader->render(renderer->getDeviceContext(), sphereMeshLP->getIndexCount());

}

void App1::shadowArea(XMMATRIX worldMatrix_, XMMATRIX viewMatrix_, XMMATRIX projectionMatrix_) {

	

	worldMatrix_ = XMMatrixTranslation(-100.f, 0.f, -10.f);
	// Render floor
	shadowAreaFloor->sendData(renderer->getDeviceContext());
	//shadowShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix_, viewMatrix_, projectionMatrix_,
	shadowShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix_, viewMatrix_, projectionMatrix_, textureMgr->getTexture(L"brick"),
			shadowMap->getDepthMapSRV(), shadowMap2->getDepthMapSRV(), lights->shadowLight, lights->shadowLight2);
	shadowShader->render(renderer->getDeviceContext(), shadowAreaFloor->getIndexCount());

	// Render model
	//worldMatrix_ = renderer->getWorldMatrix();
	/*XMFLOAT3*/// modelTrans = XMFLOAT3(0.f, 7.f, 5.f);
	worldMatrix_ = XMMatrixTranslation(modelPos[0], modelPos[1], modelPos[2]);
	XMMATRIX scaleMatrix = XMMatrixScaling(0.5f, 0.5f, 0.5f);
	worldMatrix_ = XMMatrixMultiply(worldMatrix_, scaleMatrix);
	model->sendData(renderer->getDeviceContext());
	shadowShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix_, viewMatrix_, projectionMatrix_, textureMgr->getTexture(L"grass"),
		shadowMap->getDepthMapSRV(), shadowMap2->getDepthMapSRV(), lights->shadowLight, lights->shadowLight2);
	shadowShader->render(renderer->getDeviceContext(), model->getIndexCount());

	XMMATRIX newworldMatrix;
	// Render cube
	newworldMatrix = XMMatrixTranslation(lights->shadLight_Pos[0], lights->shadLight_Pos[1], lights->shadLight_Pos[2]);
	worldMatrix_ = XMMatrixTranslation(20.f, 3.f, 5.f);
	cubeMesh->sendData(renderer->getDeviceContext());
	shadowShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix_, viewMatrix_, projectionMatrix_, textureMgr->getTexture(L"water"),
		shadowMap->getDepthMapSRV(), shadowMap2->getDepthMapSRV(), lights->shadowLight, lights->shadowLight2);
	shadowShader->render(renderer->getDeviceContext(), cubeMesh->getIndexCount());

	// render sphere at lights position
	newworldMatrix = XMMatrixTranslation(lights->shadLight_2_Pos[0], lights->shadLight_2_Pos[1], lights->shadLight_2_Pos[2]);
	sphereMeshLP->sendData(renderer->getDeviceContext());
	shadowShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix_, viewMatrix_, projectionMatrix_, textureMgr->getTexture(L"dirt"),
		shadowMap->getDepthMapSRV(), shadowMap2->getDepthMapSRV(), lights->shadowLight, lights->shadowLight2);
	shadowShader->render(renderer->getDeviceContext(), sphereMeshLP->getIndexCount());
	

}