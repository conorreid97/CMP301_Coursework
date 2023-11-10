Texture2D texture0 : register(t0);
SamplerState sampler0 : register(s0);

cbuffer MatrixBuffer : register(b0)
{
    matrix worldMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
    matrix lightViewMatrix;
    matrix lightProjectionMatrix;
};

cbuffer TimeBuffer : register(b1)
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
    float4 lightViewPos : TEXCOORD1;
};

OutputType main(InputType input)
{
    OutputType output;
    
    // offset position based on sine wave
    input.position.y += amplitude * sin((input.position.x * frequency) + (time * speed));
	 //cosine wave on the z axis
    input.position.y += amplitude * cos((input.position.z * frequency) + (time * speed));
    input.position.y += amplitude * sin(input.position.x + time);
	 //modify the normals
    input.normal = float3(-cos(input.position.x + time), 1, 0);
    input.normal += float3(amplitude * -cos((input.position.x * frequency) + (time * speed)), 1, 0);
    
    // Calculate the position of the vertex against the world, view, and projection matrices.
    output.position = mul(input.position, worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);

    // Calculate the position of the vertice as viewed by the light source.
    output.lightViewPos = mul(input.position, worldMatrix);
    output.lightViewPos = mul(output.lightViewPos, lightViewMatrix);
    output.lightViewPos = mul(output.lightViewPos, lightProjectionMatrix);

    output.tex = input.tex;
    output.normal = mul(input.normal, (float3x3) worldMatrix);
    output.normal = normalize(output.normal);

    return output;
}