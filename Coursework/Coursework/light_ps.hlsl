// Light pixel shader
// Calculate diffuse lighting for a single directional light (also texturing)

Texture2D texture0 : register(t0);
SamplerState sampler0 : register(s0);

cbuffer LightBuffer : register(b0)
{
    float4 ambient[2];
    float4 diffuse[2];
    float4 position_SpecPowerW[2];
    float4 direction[2];
    //float3 padding[2];
    float4 specular[2];
	//float padding;
};

cbuffer CameraBuffer : register(b1)
{
    float3 cameraPosition;
    float padding2;
}

struct InputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
    float3 viewVector : TEXCOORD1;
    float3 worldPosition : TEXCOORD2;
};
	
// Calculate lighting intensity based on direction and normal. Combine with light colour.
float4 calculatePointLighting(float3 lightDirection, float3 normal, float4 ldiffuse)
{
    float intensity = saturate(dot(normal, lightDirection));
    float4 colour = saturate(ldiffuse * intensity);
    return colour;
}

float4 calculateDirLighting(float3 lightDirection, float3 normal, float4 ldiffuse)
{
    float intensity = saturate(dot(normal, -lightDirection));
    float4 colour = saturate(ldiffuse * intensity);
    return colour;
}

float4 calcSpecular(float3 lightDirection, float3 normal, float3 viewVector, float4 specularColour, float specularPower)
{
    float specularIntensity[2];
	// blinn-phong specular calculation
    float3 halfway = normalize(lightDirection + viewVector);
    specularIntensity[0] = pow(max(dot(normal, halfway), 0.0), position_SpecPowerW[0].w);
    specularIntensity[1] = pow(max(dot(normal, halfway), 0.0), position_SpecPowerW[1].w);

    float specIntensTot = specularIntensity[0] + specularIntensity[1];
    return saturate(specularColour /*float4(1.0, 0.0, 0.0, 1.0)*/ * specIntensTot);
}

float4 main(InputType input) : SV_TARGET
{
    //Attenuation
    // Calculate distance
    float dist[2];
    dist[0] = length(position_SpecPowerW[0].xyz - input.worldPosition);
    dist[1] = length(position_SpecPowerW[1].xyz - input.worldPosition);
    // values for calculation
    float constantFactor = 0.5f;
    float linearFactor = 0.125f;
    float quadraticFactor = 0.0f;
    
    // Calculate attenuation
    float attenuation[2];
    attenuation[0] = 1 / (constantFactor + (linearFactor * dist[0]) + quadraticFactor * pow(dist[0], 2));
    attenuation[1] = 1 / (constantFactor + (linearFactor * dist[1]) + quadraticFactor * pow(dist[1], 2));
    
   // multiply diffuse by attenuation
    float4 updated_diffuse[2];
    updated_diffuse[0] = diffuse[0] * attenuation[0];
    updated_diffuse[1] = diffuse[1] * attenuation[1];
    
    // Sample the texture. Calculate light intensity and colour, return light*texture for final pixel colour.
    float4 textureColour = texture0.Sample(sampler0, input.tex);
    
    // find lights direction
    float3 lightVector[2];
    lightVector[0] = normalize(position_SpecPowerW[0].xyz - input.worldPosition);
    lightVector[1] = normalize(position_SpecPowerW[1].xyz - input.worldPosition);
	
    float4 lightColour[2];
    lightColour[0] = ambient[0] + calculatePointLighting(lightVector[0], input.normal, updated_diffuse[0] /*diffuse[0] /*atten*/);
    lightColour[1] = ambient[1] + calculatePointLighting(lightVector[1], input.normal, updated_diffuse[1]);
    //float4 lightColour = calculateLighting(lightVector, input.normal, float4(0.0, 0.0, 0.0, 1.0));
   // float4 specColour = calcSpecular(lightVector, input.normal, input.viewVector, specular/*float4(1.0, 0.0, 0.0, 1.0)*/, specularPower);
    float4 specColour[2];
    specColour[0] = calcSpecular(lightVector[0], input.normal, input.viewVector, specular[0] /*float4(1.0, 0.0, 0.0, 1.0)*/, position_SpecPowerW[0].w);
    specColour[1] = calcSpecular(lightVector[1], input.normal, input.viewVector, specular[1] /*float4(1.0, 0.0, 0.0, 1.0)*/, position_SpecPowerW[1].w);

    float4 totalLightColour = saturate(lightColour[0] + lightColour[1]);
    float4 totalSpecColour = specColour[0] + specColour[1];

    if (direction[0].w == 0.0)
    {
        
        return (totalLightColour * textureColour) + totalSpecColour;
    }
    
    return 0;
}