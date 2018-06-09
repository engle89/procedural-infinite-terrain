#ifndef _CUBE_H_
#define _CUBE_H_

#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <fstream>
#include "Perlin.h"
#include "textureclass.h"

using namespace DirectX;
using namespace std;

extern int m_TriTable[][16];

class CubeClass
{
private:

	struct MatrixBufferType
	{
		XMMATRIX world;
		XMMATRIX view;
		XMMATRIX projection;
		XMFLOAT4 lightPosition;
	};

	struct VertexType 
	{
		XMFLOAT4 position;
		XMFLOAT4 color;
	};

	struct SOType
	{
		XMFLOAT4 position;
		XMFLOAT4 color;
		XMFLOAT4 tex;
	};

	//vertex decals 
	struct VertexDecalType
	{
		XMFLOAT4 decal[8];
		XMFLOAT4 dataStep;
	};

public:
	CubeClass(int, int, int, XMFLOAT3, XMFLOAT3, XMFLOAT4,int,int);
	CubeClass(const CubeClass&);
	~CubeClass();

	bool Initialize(ID3D11Device*, ID3D11DeviceContext*, HWND);
	void Shutdown();
	bool Render(ID3D11DeviceContext*, XMMATRIX, XMMATRIX, XMMATRIX, XMFLOAT4);

	void GetDecals(VertexDecalType*);
	HRESULT SetUpDataResource(ID3D11Device*);

private:
	HRESULT InitializeShader(ID3D11Device*, HWND, WCHAR*, WCHAR*, WCHAR*);
	void ShutdownShader();
	void OutputShaderErrorMessage(ID3D10Blob*, HWND, WCHAR*);

	bool SetShaderParameters(ID3D11DeviceContext*, XMMATRIX, XMMATRIX, XMMATRIX, XMFLOAT4);

private:
	void RenderShader(ID3D11DeviceContext*);

	HRESULT CreateTriTableResource(ID3D11Device*);
	HRESULT CreateDensityResource(ID3D11Device*);
	
private:
	int m_vertexCount;
	int m_width, m_height, m_depth;
	XMFLOAT3 m_cubeSize;
	XMFLOAT3 m_cubeStep;
	float* m_data;	
	XMFLOAT4 m_dataStep;
	ID3D11VertexShader *  m_vertexShader;
	ID3D11GeometryShader* m_geometryShader;
	ID3D11PixelShader*    m_pixelShader;
	ID3D11InputLayout* m_layout;
	ID3D11Buffer* m_vertexBuffer;
	ID3D11Buffer* m_matrixBuffer;
	ID3D11Buffer* m_soBuffer;
	ID3D11Buffer* m_decalBuffer;
	ID3D11SamplerState* m_SamplerPoint;
	ID3D11ShaderResourceView* m_densityData = NULL;
	ID3D11ShaderResourceView* m_triTableSRV = NULL;
	ID3D11Texture3D* m_texture;
	TextureClass* m_terrain;
	ID3D11ShaderResourceView* m_terrainData;
	ID3D11SamplerState* m_TerrainSamplerPoint;

	int m_offsetx;
	int m_offsetz;
};
#endif
