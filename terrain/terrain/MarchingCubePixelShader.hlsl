struct PS_INPUT
{
	float4 position : SV_POSITION;
	float4 color    : COLOR0;
	float4 world    : TEXCOORD;
};

//constant buffer that can be updated every frame
cbuffer matrixBuffer : register(b0)
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
	float4 lightPosition;
}

//constant buffer that is only updated once per application run
cbuffer cbOncePerAppRun : register(b1)
{
	float4 decal[8];
	float4 dataStep;
}

//the sampler state used to sample textures
SamplerState samplerPoint : register(s0);
SamplerState samplerPoint2 : register(s1);
//the texture containing the density value
Texture3D<float> densityTex : register(t0);
//the texture containing the terrain info
Texture2D<float> terrainTex : register(t1);

//sample the texture and gets a value at the specific position
float getTexValue(float3 position)
{
	return densityTex.Sample(samplerPoint, position);
}

float3 getBlend(float3 wNormal)
{
	//wNormal is the world space normal
	float3 blending = abs(wNormal);
	blending = normalize(max(blending, 0.00001));
	float b = blending.x + blending.y + blending.z;
	blending /= float3(b,b,b);
	return blending;
}

float4 MarchingCubePixelShader(PS_INPUT input) : SV_TARGET
{
	//normal for the triangle mesh	
	float3 gradient = float3(
		getTexValue((input.world.xyz + float3(dataStep.x,0,0) + 1.0f) / 2.0f) - getTexValue((input.world.xyz + float3(-dataStep.x,0,0) + 1.0f) / 2.0f),
		getTexValue((input.world.xyz + float3(0,dataStep.y,0) + 1.0f) / 2.0f) - getTexValue((input.world.xyz + float3(0,-dataStep.y,0) + 1.0f) / 2.0f),
		getTexValue((input.world.xyz + float3(0,0,dataStep.z) + 1.0f) / 2.0f) - getTexValue((input.world.xyz + float3(0,0,-dataStep.z) + 1.0f) / 2.0f)
	);

	float3 normal = normalize(gradient);
	normal = -normal;
	float3 blending = getBlend(normal);

	float2 coord1 = ((input.world+1.0f)/2.0f).yz;

	float2 coord2 = ((input.world+1.0f)/2.0f).xz;
	
	float2 coord3 = ((input.world+1.0f)/2.0f).xy;
	

	float4 xaxis = terrainTex.Sample(samplerPoint2,coord1);
	float4 yaxis = terrainTex.Sample(samplerPoint2,coord2);
	float4 zaxis = terrainTex.Sample(samplerPoint2,coord3);

	float4 color = xaxis * blending.x + yaxis * blending.y + zaxis * blending.z;

	//phong lighting
	float3 V = normalize(lightPosition.xyz - input.world.xyz);
	float3 N = normalize(mul(normal, worldMatrix));
	float3 R = 2 * (dot(V, N))*N - V;
	float4 Ia = 0.3*float4(0.5, 0.5, 0.5, 0);
	float4 Id = 0.4*saturate(dot(V, N));
	float d = sqrt(V.x*V.x + V.y*V.y + V.z*V.z);
	float A = 1 / (1+0.2*d+0.1*d*d);
	float4 Is = 0.3*pow(saturate(dot(R, V)), A);
	
	color = Ia + (Id + Is)*color;
	color = float4(color.xyz, 1);
	return color;
}