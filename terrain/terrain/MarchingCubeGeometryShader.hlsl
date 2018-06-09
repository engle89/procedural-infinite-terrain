//input
struct GS_INPUT
{
	float4 position : SV_POSITION;
	float4 color    : COLOR0;
};

//output
struct GS_OUTPUT
{
	float4 position : SV_POSITION;
	float4 color    : COLOR0;
	float4 world    : TEXCOORD;
};

//generic sampler
SamplerState samplerPoint : register(s0);
SamplerState noiseSamplerPoint : register(s1);
//the texture containing the density value
Texture3D<float> densityTex : register(t1);
//triangle look up table
Texture2D<int> tritableTex : register(t0);

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

//Linearly interpolate the position where an isosurface cuts
//an edge between two vertices, each with their own scalar value
float3 vertexinterp(float isovalue, float3 p1, float3 p2, float v1, float v2)
{
	float lerper = (isovalue - v1) / (v2 - v1);
	return lerp(p1, p2, lerper);
}

//look up table
//tells you the triangles for the intersected surface
int triTableValue(int i, int j)
{
	if (i >= 256 || j >= 16)
		return -1;

	return tritableTex.Load(int3(j, i, 0));
}

float4 getProjectionPos (float4 position)
{
	position = mul(position, worldMatrix);
	position = mul(position, viewMatrix);
	position = mul(position, projectionMatrix);
	return position;
}

[maxvertexcount(18)]
void MarchingCubeGeometryShader(point GS_INPUT input[1], inout TriangleStream<GS_OUTPUT> triStream)
{
	float isovalue = 0.0f;

	float4 position = float4(input[0].position.xyz, 1);

	float3 cubePoses[8];
	float  cubeVals[8];
	for (int i = 0; i < 8; i++)
	{
		//get the position of each vertex position
		cubePoses[i] = position.xyz + decal[i].xyz;
		float3 temp = cubePoses[i];
		//get the value of each vertex on each cube
		cubeVals[i] = densityTex.SampleLevel(samplerPoint, (cubePoses[i] + 1.0f) / 2.0f, 0);
	}

	int cubeindex = 0;
	cubeindex =  int(cubeVals[0] < isovalue);
	cubeindex += int(cubeVals[1] < isovalue) * 2;
	cubeindex += int(cubeVals[2] < isovalue) * 4;
	cubeindex += int(cubeVals[3] < isovalue) * 8;
	cubeindex += int(cubeVals[4] < isovalue) * 16;
	cubeindex += int(cubeVals[5] < isovalue) * 32;
	cubeindex += int(cubeVals[6] < isovalue) * 64;
	cubeindex += int(cubeVals[7] < isovalue) * 128;

	GS_OUTPUT output;

	//these lines create the color differencies between different distances to the center of the area
	float4 thecolor = float4(0.0f, 0.0f, 1.0f, 1.0f);
	float3 relativePos = (position.xyz + 1.0f) / 2.0f;
	float3 center = float3(0.5, 0.5f, 0.5f);
	float distanceToCenter = length(center - relativePos);
	float maxDistance = 1.65f;

	float relativeDistance = distanceToCenter / maxDistance;
	float colorg = relativeDistance;

	thecolor.g = colorg;
	thecolor.b -= (colorg*3.0f);
	output.color = thecolor;

	if (cubeindex != 0 && cubeindex != 255) //completely inside or outside a cube
	{
		float3 vertexList[12];

		//find the 12 possible vertices where the surface intersects the cube
		//notice we have 12 edges
		vertexList[0] = vertexinterp(isovalue, cubePoses[0], cubePoses[1], cubeVals[0], cubeVals[1]);
		vertexList[1] = vertexinterp(isovalue, cubePoses[1], cubePoses[2], cubeVals[1], cubeVals[2]);
		vertexList[2] = vertexinterp(isovalue, cubePoses[2], cubePoses[3], cubeVals[2], cubeVals[3]);
		vertexList[3] = vertexinterp(isovalue, cubePoses[3], cubePoses[0], cubeVals[3], cubeVals[0]);
		vertexList[4] = vertexinterp(isovalue, cubePoses[4], cubePoses[5], cubeVals[4], cubeVals[5]);
		vertexList[5] = vertexinterp(isovalue, cubePoses[5], cubePoses[6], cubeVals[5], cubeVals[6]);
		vertexList[6] = vertexinterp(isovalue, cubePoses[6], cubePoses[7], cubeVals[6], cubeVals[7]);
		vertexList[7] = vertexinterp(isovalue, cubePoses[7], cubePoses[4], cubeVals[7], cubeVals[4]);
		vertexList[8] = vertexinterp(isovalue, cubePoses[0], cubePoses[4], cubeVals[0], cubeVals[4]);
		vertexList[9] = vertexinterp(isovalue, cubePoses[1], cubePoses[5], cubeVals[1], cubeVals[5]);
		vertexList[10] = vertexinterp(isovalue, cubePoses[2], cubePoses[6], cubeVals[2], cubeVals[6]);
		vertexList[11] = vertexinterp(isovalue, cubePoses[3], cubePoses[7], cubeVals[3], cubeVals[7]);


		GS_OUTPUT point1;
		GS_OUTPUT point2;
		GS_OUTPUT point3;

		float4 worldPos1;
		float4 worldPos2;
		float4 worldPos3;
		//float2 worldPos1;
		//float2 worldPos2;
		//float2 worldPos3;

		float3 vector1;
		float3 vector2;

		//lookup table provides info needed to build up to five triangles within the cell
		//notice we have 2^8=256 posibbilities
		//and can be characterized to 15 fundamental cases
		for (int i = 0; triTableValue(cubeindex, i) != -1; i += 3)
		{
			//add vertices to the output stream
			output.position = float4(vertexList[triTableValue(cubeindex, i + 0)], 1);
			worldPos1 = output.position;
			//worldPos1 = float2(output.position.x, output.position.y);
			output.position = getProjectionPos(output.position);
			point1 = output;

			output.position = float4(vertexList[triTableValue(cubeindex, i + 1)], 1);
			worldPos2 = output.position;
			//worldPos2 = float2(output.position.x, output.position.y);
			output.position = getProjectionPos(output.position);
			point2 = output;

			output.position = float4(vertexList[triTableValue(cubeindex, i + 2)], 1);
			worldPos3 = output.position;
			//worldPos3 = float2(output.position.x, output.position.y);
			output.position = getProjectionPos(output.position);
			point3 = output;

			point1.world = worldPos1;
			point2.world = worldPos2;
			point3.world = worldPos3;

			triStream.Append(point1);
			triStream.Append(point2);
			triStream.Append(point3);

			triStream.RestartStrip();
		}
		
	}
}