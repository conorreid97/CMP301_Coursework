// Light vertex shader
// Standard issue vertex shader, apply matrices, pass info to pixel shader
Texture2D texture0 : register(t0);
SamplerState sampler0 : register(s0);

cbuffer MatrixBuffer : register(b0)
{
    matrix worldMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
};

cbuffer CameraBuffer : register(b1)
{
    float3 cameraPosition;
    float padding;
}
cbuffer TimeBuffer : register(b2)
{
    float time;
    float amplitude;
    float speed;
	//float padding;
    float frequency;
    //float padding2;
}

struct InputType
{
    float4 position : POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
};

struct OutputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
    float3 viewVector : TEXCOORD1;
    float3 worldPosition : TEXCOORD2;
	
};

float3 normCalc(float2 uv)
{
    uint texW, texH, numLevels;
	
    texture0.GetDimensions(0, texW, texH, numLevels);
	
    float sideStep = 1.0 / 100.0;
	
    float2 leftTex = uv + float2(-sideStep, 0);
    float2 rightTex = uv + float2(sideStep, 0);
    float2 botTex = uv + float2(0, sideStep);
    float2 topTex = uv + float2(0, -sideStep);
	
    float leftY = texture0.SampleLevel(sampler0, leftTex, 0).r;
    float rightY = texture0.SampleLevel(sampler0, rightTex, 0).r;
    float bottomY = texture0.SampleLevel(sampler0, botTex, 0).r;
    float topY = texture0.SampleLevel(sampler0, topTex, 0).r;
	
    float3 tang = normalize(float3(2.0f * (sideStep), (rightY - leftY), 0.0f));
    float3 bitan = normalize(float3(0.0f, (bottomY - topY), -2.f * sideStep));
    float3 normalW = cross(tang, bitan);
    return normalW;
}

float getHeight(float2 tex)
{
    float textureColour;
    textureColour = texture0.SampleLevel(sampler0, tex, 0);
    return textureColour * amplitude;
}

OutputType main(InputType input)
{
    OutputType output;
	
	
	//// offset position based on sine wave
 //   input.position.y += amplitude * sin((input.position.x * frequency) + (time * speed));
	// //cosine wave on the z axis
 //   input.position.y += amplitude * cos((input.position.z * frequency) + (time * speed));
 //   input.position.y += amplitude * sin(input.position.x + time);
	// //modify the normals
 //   input.normal = float3(-cos(input.position.x + time), 1, 0);
 //   input.normal += float3(amplitude * -cos((input.position.x * frequency) + (time * speed)), 1, 0);

	//height map
    input.normal = normCalc(input.tex);
    input.position.y += getHeight(input.tex);
	
	// Calculate the position of the vertex against the world, view, and projection matrices.
    output.position = mul(input.position, worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);

	// Store the texture coordinates for the pixel shader.
    output.tex = input.tex;

	// Calculate the normal vector against the world matrix only and normalise.
    output.normal = mul(input.normal, (float3x3) worldMatrix);
    output.normal = normalize(output.normal);

    // Calculate the position of the vertex in the world (for specular)
    float4 worldPosition = mul(input.position, worldMatrix);
    output.viewVector = cameraPosition.xyz - worldPosition.xyz;
    output.viewVector = normalize(output.viewVector);
	
    output.worldPosition = mul(input.position, worldMatrix).xyz;
    
    return output;
}