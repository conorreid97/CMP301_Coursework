// Horizontal edge detection shader handler
// Loads horizontal edge detection shaders (vs and ps)
// Passes screen width to shaders, for sample coordinate calculation
#pragma once

#include "DXF.h"

using namespace std;
using namespace DirectX;

class HorizontalEDShader : public BaseShader
{
private:
	struct ScreenSizeBufferType
	{
		float screenWidth;
		XMFLOAT3 padding;
	};

public:

	HorizontalEDShader(ID3D11Device* device, HWND hwnd);
	~HorizontalEDShader();

	void setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX& world, const XMMATRIX& view, const XMMATRIX& projection, ID3D11ShaderResourceView* texture, float width);

private:
	void initShader(const wchar_t* vs, const wchar_t* ps);

private:
	ID3D11Buffer* matrixBuffer;
	ID3D11SamplerState* sampleState;
	ID3D11Buffer* screenSizeBuffer;
};

