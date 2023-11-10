
Texture2D texture0 : register(t0);
SamplerState sampler0 : register(s0);

cbuffer LightBuffer : register(b0)
{
    float4 diffuseColour;
    float4 ambientColour;
    float3 lightDirection;
    float padding;
};

struct InputType
{
    float4 position : SV_POSITION;
    float4 depthPosition : TEXCOORD0;
};

float4 calculateLighting(float3 lightDirection, float3 normal, float4 diffuse)
{
    float intensity = saturate(dot(normal, lightDirection));
    float4 colour = ambientColour + saturate(diffuse * intensity);
    float4 lightColour = ambientColour + saturate(diffuse * intensity);
    return colour;
}

float4 main(InputType input) : SV_TARGET
{
    float4 textureColour;
    float4 lightColour;
    float depthValue;
// Get the depth value of the pixel by dividing the Z pixel depth by the homogeneous W coordinate.
    depthValue = input.depthPosition.z / input.depthPosition.w;
    return float4(depthValue, depthValue, depthValue, 1.0f);

}