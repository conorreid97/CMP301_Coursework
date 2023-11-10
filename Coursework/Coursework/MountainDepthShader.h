// Colour shader.h
// Simple shader example.
#pragma once

#include "DXF.h"

using namespace std;
using namespace DirectX;


class MountainDepthShader : public BaseShader
{
private:

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

	MountainDepthShader(ID3D11Device* device, HWND hwnd);
	~MountainDepthShader();

	void setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX& world, const XMMATRIX& view, const XMMATRIX& projection, float timer);
	float setAmplitude(float amp_);

private:
	void initShader(const wchar_t* vs, const wchar_t* ps);

private:
	ID3D11Buffer* matrixBuffer;
	ID3D11Buffer* timeBuffer;
	TimeBufferType* timePtr;

	float myAmplitude;
	float mySpeed;
	float myFreq;
};
