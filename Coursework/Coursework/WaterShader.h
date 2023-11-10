#pragma once

#include "DXF.h"

using namespace std;
using namespace DirectX;

class WaterShader : public BaseShader
{
private:
	struct LightBufferType
	{
		XMFLOAT4 ambient[3];
		XMFLOAT4 diffuse[3];
		XMFLOAT4 position[3];
		//XMFLOAT3 direction;
		float specularPower[3];
		XMFLOAT3 padding[3];
		XMFLOAT4 specular[3];
		//float padding;
	};

	struct TimeBufferType
	{
		float time;
		float amplitude;
		float speed;
		//float padding;
		float frequency;
		//float padding2;
	};

	struct CameraBufferType
	{
		XMFLOAT3 cameraPosition;
		float padding;
	};


public:
	WaterShader(ID3D11Device* device, HWND hwnd);
	~WaterShader();

	void setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX& world, const XMMATRIX& view, const XMMATRIX& projection, ID3D11ShaderResourceView* texture, Light* light[3], float timer, XMFLOAT3 cameraPos);
	void setAmplitude(float amp_);
	float getAmplitude();
	void setSpeed(float speed_);
	float getSpeed();
	void setFrequency(float freq_);
	float getFrequency();
	void setNormalToggle(float tog_);
	float getNormalToggle();

private:
	void initShader(const wchar_t* cs, const wchar_t* ps);

private:
	ID3D11Buffer* matrixBuffer;
	ID3D11Buffer* timeBuffer;
	ID3D11SamplerState* sampleState;
	ID3D11Buffer* lightBuffer;
	TimeBufferType* timePtr;
	ID3D11Buffer* cameraBuffer;

	float myAmplitude_;
	float mySpeed_;
	float myFrequency_;
	float normalToggle_;

};