// Light pixel shader
// Calculate diffuse lighting for a single directional light (also texturing)

//Texture2D texture0[3] : register(t0);
Texture2D texture0 : register(t0);
SamplerState sampler0 : register(s0);

cbuffer LightBuffer : register(b0)
{
    float4 ambient[3];
    float4 diffuse[3];
    float4 position[3];
    float specularPower[3];
    float3 padding[3];
    float4 specular[3];

	//float padding;
};

cbuffer CameraBuffer : register(b1)
{
    float3 cameraPosition;
    float normalToggle;
}

struct InputType
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
    return textureColour * 50;
}

// Calculate lighting intensity based on direction and normal. Combine with light colour.
float4 calculateLighting(float3 lightDirection, float3 normal, float4 ldiffuse)
{
    float intensity = saturate(dot(normal, lightDirection));
    float4 colour = saturate(ldiffuse * intensity);
    return colour;
}

float4 calcSpecular(float3 lightDirection, float3 normal, float3 viewVector, float4 specularColour, float specularPower)
{
	// blinn-phong specular calculation
    float3 halfway = normalize(lightDirection + viewVector);
    float specularIntensity = pow(max(dot(normal, halfway), 0.0), specularPower);
    return saturate(specularColour /*float4(1.0, 0.0, 0.0, 1.0)*/ * specularIntensity);
}

float4 main(InputType input) : SV_TARGET
{
 //Attenuation


    float dist[3];
    dist[0] = length(position[0].xyz - input.worldPosition);
    dist[1] = length(position[1].xyz - input.worldPosition);
    dist[2] = length(position[2].xyz - input.worldPosition);
    
    float constantFactor = 0.5f;
    float linearFactor = 0.125f;
    float quadraticFactor = 0.0f;
    
    // Calculate attenuation
    float attenuation[3];
    attenuation[0] = 1 / (constantFactor + (linearFactor * dist[0]) + quadraticFactor * pow(dist[0], 2));
    attenuation[1] = 1 / (constantFactor + (linearFactor * dist[1]) + quadraticFactor * pow(dist[1], 2));
    attenuation[2] = 1 / (constantFactor + (linearFactor * dist[2]) + quadraticFactor * pow(dist[2], 2));

   // multiply diffuse by attenuation
    float4 updated_diffuse[3];
    updated_diffuse[0] = diffuse[0] * attenuation[0];
    updated_diffuse[1] = diffuse[1] * attenuation[1];
    updated_diffuse[2] = diffuse[2] * attenuation[2];
    
    // Sample the texture. Calculate light intensity and colour, return light*texture for final pixel colour.
    float4 textureColour = texture0.Sample(sampler0, input.tex);
    
    float3 lightVector[3];
    lightVector[0] = normalize(position[0].xyz - input.worldPosition);
    lightVector[1] = normalize(position[1].xyz - input.worldPosition);
    lightVector[2] = normalize(position[2].xyz - input.worldPosition);
	
    float4 lightColour[3];
    lightColour[0] = ambient[0] + calculateLighting(lightVector[0], input.normal, updated_diffuse[0]);
    lightColour[1] = ambient[1] + calculateLighting(lightVector[1], input.normal, updated_diffuse[1]);
    lightColour[2] = ambient[2] + calculateLighting(lightVector[2], input.normal, updated_diffuse[2]);
    //float4 lightColour = calculateLighting(lightVector, input.normal, float4(0.0, 0.0, 0.0, 1.0));
   // float4 specColour = calcSpecular(lightVector, input.normal, input.viewVector, specular/*float4(1.0, 0.0, 0.0, 1.0)*/, specularPower);
    float4 specColour[3];
    specColour[0] = calcSpecular(lightVector[0], input.normal, input.viewVector, specular[0] /*float4(1.0, 0.0, 0.0, 1.0)*/, specularPower[0]);
    specColour[1] = calcSpecular(lightVector[1], input.normal, input.viewVector, specular[1] /*float4(1.0, 0.0, 0.0, 1.0)*/, specularPower[1]);
    specColour[2] = calcSpecular(lightVector[2], input.normal, input.viewVector, specular[2] /*float4(1.0, 0.0, 0.0, 1.0)*/, specularPower[2]);
    float4 n = float4(input.normal, 1.0f) + float4(1.0, 1.0, 1.0, 1.0);
    n *= 0.5;
    //return n;
    //return float4(input.normal, 1.0f);
    
    float4 totalLightColour = saturate(lightColour[0] + lightColour[1] + lightColour[2]);
    float4 totalSpecColour = specColour[0] + specColour[1] + specColour[2];

    // outputs normals
    if (normalToggle >= 1.0)
    {
        return float4(input.normal, 1.0f);    
    }
    else
    {
        return (totalLightColour * textureColour) + totalSpecColour;
    }

    
   
}