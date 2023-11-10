
//Texture2D shaderTexture : register(t0);
//Texture2D depthMapTexture : register(t1);

//SamplerState diffuseSampler  : register(s0);
//SamplerState shadowSampler : register(s1);

//cbuffer LightBuffer : register(b0)
//{
//    float4 ambient;
//    float4 diffuse;
//    float3 direction;
//};

//struct InputType
//{
//    float4 position : SV_POSITION;
//    float2 tex : TEXCOORD0;
//    float3 normal : NORMAL;
//    float4 lightViewPos : TEXCOORD1;
//};

//// Calculate lighting intensity based on direction and normal. Combine with light colour.
//float4 calculateLighting(float3 lightDirection, float3 normal, float4 diffuse)
//{
//    float intensity = saturate(dot(normal, lightDirection));
//    float4 colour = saturate(diffuse * intensity);
//    return colour;
//}

//// Is the gemoetry in our shadow map
//bool hasDepthData(float2 uv)
//{
//    if (uv.x < 0.f || uv.x > 1.f || uv.y < 0.f || uv.y > 1.f)
//    {
//        return false;
//    }
//    return true;
//}

//bool isInShadow(Texture2D sMap, float2 uv, float4 lightViewPosition, float bias)
//{
//    // Sample the shadow map (get depth of geometry)
//    float depthValue = sMap.Sample(shadowSampler, uv).r;
//    // Calculate the depth from the light.
//    float lightDepthValue = lightViewPosition.z / lightViewPosition.w;
//    lightDepthValue -= bias;

//    // Compare the depth of the shadow map value and the depth of the light to determine whether to shadow or to light this pixel.
//    if (lightDepthValue < depthValue)
//    {
//        return false;
//    }
//    return true;
//}

//float2 getProjectiveCoords(float4 lightViewPosition)
//{
//    // Calculate the projected texture coordinates.
//    float2 projTex = lightViewPosition.xy / lightViewPosition.w;
//    projTex *= float2(0.5, -0.5);
//    projTex += float2(0.5f, 0.5f);
//    return projTex;
//}

//float4 main(InputType input) : SV_TARGET
//{
//    float shadowMapBias = 0.005f;
//    float4 colour = float4(0.f, 0.f, 0.f, 1.f);
//    float4 textureColour = shaderTexture.Sample(diffuseSampler, input.tex);

//    // Calculate the projected texture coordinates.
//    float2 pTexCoord = getProjectiveCoords(input.lightViewPos);

//    // Shadow test. Is or isn't in shadow
//    if (hasDepthData(pTexCoord))
//    {
//        // Has depth map data
//        if (!isInShadow(depthMapTexture, pTexCoord, input.lightViewPos, shadowMapBias))
//        {
//            // is NOT in shadow, therefore light
//            colour = calculateLighting(-direction, input.normal, diffuse);
//        }
//    }

//    colour = saturate(colour + ambient);
//    return saturate(colour) * textureColour;
//}


Texture2D shaderTexture : register(t0);
Texture2D depthMapTexture : register(t1);
Texture2D depthMapTexture2 : register(t2);

SamplerState diffuseSampler : register(s0);
SamplerState shadowSampler : register(s1);

cbuffer LightBuffer : register(b0)
{
    float4 ambient[2];
    float4 diffuse[2];
    float3 direction[2];
};

struct InputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
    float4 lightViewPos : TEXCOORD1;
    float4 lightViewPos2 : TEXCOORD2;
};

// Calculate lighting intensity based on direction and normal. Combine with light colour.
float4 calculateLighting(float3 lightDirection, float3 normal, float4 diffuse)
{
    float intensity = saturate(dot(normal, lightDirection));
    float4 colour = saturate(diffuse * intensity);
    return colour;
}

// Is the gemoetry in our shadow map
bool hasDepthData(float2 uv)
{
    if (uv.x < 0.f || uv.x > 1.f || uv.y < 0.f || uv.y > 1.f)
    {
        return false;
    }
    return true;
}

bool isInShadow(Texture2D sMap, float2 uv, float4 lightViewPosition, float bias)
{
    // Sample the shadow map (get depth of geometry)
    float depthValue = sMap.Sample(shadowSampler, uv).r;
	// Calculate the depth from the light.
    float lightDepthValue = lightViewPosition.z / lightViewPosition.w;
    lightDepthValue -= bias;
    
	// Compare the depth of the shadow map value and the depth of the light to determine whether to shadow or to light this pixel.
    if (lightDepthValue < depthValue)
    {
        return false;
    }
  
    return true;
}

bool isInShadow2(Texture2D sMap2, float2 uv, float4 lightViewPosition2, float bias)
{
    float depthValue2 = sMap2.Sample(shadowSampler, uv).r;
    float lightDepthValue2 = lightViewPosition2.z / lightViewPosition2.w;
    lightDepthValue2 -= bias;
    
    // Compare the depth of the shadow map value and the depth of the light to determine whether to shadow or to light this pixel.
    if (lightDepthValue2 < depthValue2)
    {
        return false;
    }
    return true;
}

bool isInShadowBoth(Texture2D sMap, Texture2D sMap2, float2 uv, float2 uv2, float4 lightViewPosition, float4 lightViewPosition2, float bias)
{
    // Sample the shadow map (get depth of geometry)
    float depthValue = sMap.Sample(shadowSampler, uv).r;
    float depthValue2 = sMap2.Sample(shadowSampler, uv2).r;
	// Calculate the depth from the light.
    float lightDepthValue = lightViewPosition.z / lightViewPosition.w;
    float lightDepthValue2 = lightViewPosition2.z / lightViewPosition2.w;
    lightDepthValue -= bias;
    lightDepthValue2 -= bias;
    
	// Compare the depth of the shadow map value and the depth of the light to determine whether to shadow or to light this pixel.
    if (lightDepthValue < depthValue)
    {
        if (lightDepthValue2 < depthValue2)
        {  
            return false;
        }
    }
    return true;
}

float2 getProjectiveCoords(float4 lightViewPosition)
{
    // Calculate the projected texture coordinates.
    float2 projTex = lightViewPosition.xy / lightViewPosition.w;
    projTex *= float2(0.5, -0.5);
    projTex += float2(0.5f, 0.5f);
    return projTex;
}

float2 getProjectiveCoords2(float4 lightViewPosition2)
{
    // Calculate the projected texture coordinates.
    float2 projTex2 = lightViewPosition2.xy / lightViewPosition2.w;
    projTex2 *= float2(0.5, -0.5);
    projTex2 += float2(0.5f, 0.5f);
    return projTex2;
}

float4 main(InputType input) : SV_TARGET
{
    float shadowMapBias = 0.005f;
    float4 colour[2];
    colour[0] = float4(0.f, 0.f, 0.f, 1.f);
    colour[1] = float4(0.f, 0.f, 0.f, 1.f);
    float4 textureColour = shaderTexture.Sample(diffuseSampler, input.tex);

	// Calculate the projected texture coordinates.
    float2 pTexCoord = getProjectiveCoords(input.lightViewPos);
    float2 pTexCoord2 = getProjectiveCoords2(input.lightViewPos2);

	
    // Shadow test. Is or isn't in shadow
    if (hasDepthData(pTexCoord))
    {
         //Has depth map data
        //if (!isInShadow(depthMapTexture, pTexCoord, input.lightViewPos, shadowMapBias))
        //{
             
        //    colour[0] = calculateLighting(-direction[0], input.normal, diffuse[0]);
        //    colour[1] = calculateLighting(-direction[1], input.normal, diffuse[1]);
        //}
        
        //if (!isInShadow(depthMapTexture2, pTexCoord, input.lightViewPos2, shadowMapBias))
        //{
             
        //    colour[0] = calculateLighting(-direction[0], input.normal, diffuse[0]);
        //    colour[1] = calculateLighting(-direction[1], input.normal, diffuse[1]);
        //}
        
        if (!isInShadowBoth(depthMapTexture, depthMapTexture2, pTexCoord, pTexCoord2, input.lightViewPos, input.lightViewPos2, shadowMapBias))
        {
             
            colour[0] = calculateLighting(-direction[0].xyz, input.normal, diffuse[0]);
            colour[1] = calculateLighting(-direction[1].xyz, input.normal, diffuse[1]);
        }
    }
    
    //if (hasDepthData(pTexCoord2))
    //{
    //    if (!isInShadow2(depthMapTexture2, pTexCoord, input.lightViewPos2, shadowMapBias))
    //    {
    //                  // is NOT in shadow, therefore light
    //        colour[0] = calculateLighting(-direction[0], input.normal, diffuse[0]);
    //        colour[1] = calculateLighting(-direction[1], input.normal, diffuse[1]);
    //    }
    //}
    
    colour[0] = saturate(colour[0] + ambient[0]);
    colour[1] = saturate(colour[1] + ambient[1]);
    float4 finalColour = colour[0] + colour[1];
    return saturate(finalColour) * textureColour;
}