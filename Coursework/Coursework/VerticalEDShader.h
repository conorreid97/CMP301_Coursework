// Vertical edge detection shader handler
// Loads vertical edge detection shaders (vs and ps)
// Passes screen height to shaders, for sample coordinate calculation
#pragma once

#include "DXF.h"

using namespace std;
using namespace DirectX;

class VerticalEDShader : public BaseShader
{
private:
	struct ScreenSizeBufferType
	{
		float screenHeight;
		XMFLOAT3 padding;
	};

public:

	VerticalEDShader(ID3D11Device* device, HWND hwnd);
	~VerticalEDShader();

	void setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX& world, const XMMATRIX& view, const XMMATRIX& projection, ID3D11ShaderResourceView* texture, float width);

private:
	void initShader(const wchar_t* vs, const wchar_t* ps);

private:
	ID3D11Buffer* matrixBuffer;
	ID3D11SamplerState* sampleState;
	ID3D11Buffer* screenSizeBuffer;
};
