//globals
//constant buffer that can be updated every frame
cbuffer matrixBuffer : register(b0)
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
	float4 lightPosition;
};

//typedefs
struct VertexInputType
{
	float4 position : POSITION;
	float4 color    : COLOR;
};

struct PixelInputType
{
	float4 position : SV_POSITION;
	float4 color    : COLOR;
};

PixelInputType MarchingCubeVertexShader(VertexInputType input)
{
	PixelInputType output;

	output.position = float4(input.position.xyz, 1);
	output.color    = input.color;

	return output;
}