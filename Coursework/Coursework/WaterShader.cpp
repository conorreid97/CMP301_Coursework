#include "WaterShader.h"

WaterShader::WaterShader(ID3D11Device* device, HWND hwnd) : BaseShader(device, hwnd)
{
	initShader(L"waterShader_vs.cso", L"waterShader_ps.cso");
}


WaterShader::~WaterShader()
{
	// Release the sampler state.
	if (sampleState)
	{
		sampleState->Release();
		sampleState = 0;
	}

	// Release the matrix constant buffer.
	if (matrixBuffer)
	{
		matrixBuffer->Release();
		matrixBuffer = 0;
	}

	if (timeBuffer)
	{
		timeBuffer->Release();
		timeBuffer = 0;
	}
	// Release the layout.
	if (layout)
	{
		layout->Release();
		layout = 0;
	}

	// Release the light constant buffer.
	if (lightBuffer)
	{
		lightBuffer->Release();
		lightBuffer = 0;
	}

	//Release base shader components
	BaseShader::~BaseShader();
}

void WaterShader::initShader(const wchar_t* vsFilename, const wchar_t* psFilename)
{
	D3D11_BUFFER_DESC matrixBufferDesc;
	D3D11_BUFFER_DESC timeBufferDesc;
	D3D11_SAMPLER_DESC samplerDesc;
	D3D11_BUFFER_DESC lightBufferDesc;
	D3D11_BUFFER_DESC cameraBufferDesc;

	// Load (+ compile) shader files
	loadVertexShader(vsFilename);
	loadPixelShader(psFilename);

	// Setup the description of the dynamic matrix constant buffer that is in the vertex shader.
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;
	renderer->CreateBuffer(&matrixBufferDesc, NULL, &matrixBuffer);

	// setup the description for time buffer
	timeBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	timeBufferDesc.ByteWidth = sizeof(TimeBufferType);
	timeBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	timeBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	timeBufferDesc.MiscFlags = 0;
	timeBufferDesc.StructureByteStride = 0;
	renderer->CreateBuffer(&timeBufferDesc, NULL, &timeBuffer);

	// Create a texture sampler state description.
	samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	renderer->CreateSamplerState(&samplerDesc, &sampleState);

	// Setup light buffer
	// Setup the description of the light dynamic constant buffer that is in the pixel shader.
	// Note that ByteWidth always needs to be a multiple of 16 if using D3D11_BIND_CONSTANT_BUFFER or CreateBuffer will fail.
	lightBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	lightBufferDesc.ByteWidth = sizeof(LightBufferType);
	lightBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	lightBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	lightBufferDesc.MiscFlags = 0;
	lightBufferDesc.StructureByteStride = 0;
	renderer->CreateBuffer(&lightBufferDesc, NULL, &lightBuffer);

	cameraBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	cameraBufferDesc.ByteWidth = sizeof(CameraBufferType);
	cameraBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cameraBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cameraBufferDesc.MiscFlags = 0;
	cameraBufferDesc.StructureByteStride = 0;
	renderer->CreateBuffer(&cameraBufferDesc, NULL, &cameraBuffer);

	myAmplitude_ = 1.0f;
	mySpeed_ = 2.0f;
	myFrequency_ = 2.0f;
	normalToggle_ = 0.0f;
}


void WaterShader::setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX& worldMatrix, const XMMATRIX& viewMatrix, 
										const XMMATRIX& projectionMatrix, ID3D11ShaderResourceView* texture, Light* light[3], float timer, XMFLOAT3 cameraPos)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	MatrixBufferType* dataPtr;
	XMMATRIX tworld, tview, tproj;

	// Transpose the matrices to prepare them for the shader.
	tworld = XMMatrixTranspose(worldMatrix);
	tview = XMMatrixTranspose(viewMatrix);
	tproj = XMMatrixTranspose(projectionMatrix);
	result = deviceContext->Map(matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	dataPtr = (MatrixBufferType*)mappedResource.pData;
	dataPtr->world = tworld;// worldMatrix;
	dataPtr->view = tview;
	dataPtr->projection = tproj;
	deviceContext->Unmap(matrixBuffer, 0);
	deviceContext->VSSetConstantBuffers(0, 1, &matrixBuffer);

	// timer data to send to verte shader
	//TimeBufferType* timePtr;
	deviceContext->Map(timeBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	XMFLOAT3 pad(0.0f, 0.0, 0.0f);
	XMFLOAT3 ampli(0.0f, 5.0f, 0.0f);
	XMFLOAT3 freq(10.0, 0.0f, 0.0f);
	timePtr = (TimeBufferType*)mappedResource.pData;
	timePtr->time = timer;
	timePtr->amplitude = myAmplitude_;
	timePtr->speed = mySpeed_;
	timePtr->frequency = myFrequency_;
	deviceContext->Unmap(timeBuffer, 0);
	deviceContext->VSSetConstantBuffers(2, 1, &timeBuffer);

	// send camera data tewo vertex shader
	CameraBufferType* camPtr;
	deviceContext->Map(cameraBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	camPtr = (CameraBufferType*)mappedResource.pData;
	camPtr->cameraPosition = cameraPos;
	camPtr->padding = 0.0f;
	//camPtr->normalToggle = normalToggle_;
	deviceContext->Unmap(cameraBuffer, 0);
	deviceContext->PSSetConstantBuffers(0, 1, &cameraBuffer);

	//Additional
	// Send light data to pixel shader
	LightBufferType* lightPtr;
	deviceContext->Map(lightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	lightPtr = (LightBufferType*)mappedResource.pData;
	for (int i = 0; i < 3; i++) {
		lightPtr->ambient[i] = light[i]->getAmbientColour();
		//lightPtr->ambient[1] = lightArr[i]->getAmbientColour();
		lightPtr->diffuse[i] = light[i]->getDiffuseColour();
		lightPtr->specular[i] = light[i]->getSpecularColour();
		lightPtr->position[i] = XMFLOAT4(light[i]->getPosition().x, light[i]->getPosition().y, light[i]->getPosition().z, 0.0f);
		lightPtr->padding[i] = XMFLOAT3(0.0f, 0.0f, 0.0f);
		lightPtr->specularPower[i] = light[i]->getSpecularPower();
	}
	
	deviceContext->Unmap(lightBuffer, 0);
	deviceContext->PSSetConstantBuffers(0, 1, &lightBuffer);

	// Set shader texture resource in the pixel shader.
	deviceContext->PSSetShaderResources(0, 1, &texture);
	deviceContext->PSSetSamplers(0, 1, &sampleState);

	// set shader texture resource in the vertex shader
	deviceContext->VSSetShaderResources(0, 1, &texture);
	deviceContext->VSSetSamplers(0, 1, &sampleState);
}

void WaterShader::setAmplitude(float amp_) {
	myAmplitude_ = amp_;
	//return timePtr->amplitude;
}
float WaterShader::getAmplitude() {
	return myAmplitude_;
}

void WaterShader::setSpeed(float speed_) {
	mySpeed_ = speed_;
}

float WaterShader::getSpeed() {
	return mySpeed_;
}

void WaterShader::setFrequency(float freq_) {
	myFrequency_ = freq_;
}

float WaterShader::getFrequency() {
	return myFrequency_;
}

void WaterShader::setNormalToggle(float tog_) {
	normalToggle_ = tog_;
}

float WaterShader::getNormalToggle() {
	return myFrequency_;
}