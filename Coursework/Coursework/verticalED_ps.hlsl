Texture2D shaderTexture : register(t0);
SamplerState SampleType : register(s0);

cbuffer ScreenSizeBuffer : register(b0)
{
    float screenHeight;
    float3 padding;
};

struct InputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
};

float4 main(InputType input) : SV_TARGET
{
    float4 colour;

    // Initialize the colour to black.
    colour = float4(0.0f, 0.0f, 0.0f, 0.0f);

    float texelSize = 1.0f / screenHeight;
 
    colour += shaderTexture.Sample(SampleType, input.tex + float2(texelSize * -1.0f, texelSize * 1.0f)) * -1.0; // bottom left
    colour += shaderTexture.Sample(SampleType, input.tex + float2(texelSize * 0.0f, texelSize * 1.0f)) * 0.0; // bottom middle
    colour += shaderTexture.Sample(SampleType, input.tex + float2(texelSize * 1.0f, texelSize * 1.0f)) * 1.0; // bottom right
    colour += shaderTexture.Sample(SampleType, input.tex + float2(texelSize * -1.0f, texelSize * 0.0f)) * -2.0; // left
    colour += shaderTexture.Sample(SampleType, input.tex) * 0.0; // middle
    colour += shaderTexture.Sample(SampleType, input.tex + float2(texelSize * 1.0f, texelSize * 0.0f)) * 2.0; // right
    colour += shaderTexture.Sample(SampleType, input.tex + float2(texelSize * -1.0f, texelSize * -1.0f)) * -1.0; // top left
    colour += shaderTexture.Sample(SampleType, input.tex + float2(texelSize * 0.0f, texelSize * -1.0f)) * 0.0; // top middle
    colour += shaderTexture.Sample(SampleType, input.tex + float2(texelSize * 1.0f, texelSize * -1.0f)) * 1.0; // top right
 
    colour = saturate(colour + shaderTexture.Sample(SampleType, input.tex));

    // Set the alpha channel to one.
    colour.a = 1.0f;

    return colour;
}