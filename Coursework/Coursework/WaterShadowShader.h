// Light shader.h
// Basic single light shader setup
//#ifndef _SHADOWSHADER_H_
//#define _SHADOWSHADER_H_

#include "DXF.h"

using namespace std;
using namespace DirectX;


class WaterShadowShader : public BaseShader
{
private:
	struct MatrixBufferType
	{
		XMMATRIX world;
		XMMATRIX view;
		XMMATRIX projection;
		XMMATRIX lightView;
		XMMATRIX lightProjection;
	};

	struct LightBufferType
	{
		XMFLOAT4 ambient;
		XMFLOAT4 diffuse;
		XMFLOAT3 direction;
		float padding;
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

public:

	WaterShadowShader(ID3D11Device* device, HWND hwnd);
	~WaterShadowShader();

	void setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX& world, const XMMATRIX& view, const XMMATRIX& projection, 
							ID3D11ShaderResourceView* texture, ID3D11ShaderResourceView* depthMap, Light* light, float timer);

	void setAmplitude(float amp_);
	float getAmplitude();
	void setSpeed(float speed_);
	float getSpeed();
	void setFrequency(float freq_);
	float getFrequency();

private:
	void initShader(const wchar_t* vs, const wchar_t* ps);

private:
	ID3D11Buffer* matrixBuffer;
	ID3D11Buffer* timeBuffer;
	ID3D11SamplerState* sampleState;
	ID3D11SamplerState* sampleStateShadow;
	ID3D11Buffer* lightBuffer;
	TimeBufferType* timePtr;

	float myAmplitude_;
	float mySpeed_;
	float myFrequency_;
};
//
//#endif
