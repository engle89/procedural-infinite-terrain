#ifndef _WORLD_H_
#define _WORLD_H_

#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <fstream>
#include <vector>
#include "cubesclass.h"

using namespace DirectX;


class WorldClass 
{
public:
	WorldClass();
	WorldClass(const WorldClass&);
	~WorldClass();
	bool CreateChunk(ID3D11Device*, ID3D11DeviceContext*, HWND,int, int, int);
	bool Initialize(ID3D11Device*, ID3D11DeviceContext*, HWND,int);
	void Shutdown();
	bool Render(ID3D11DeviceContext*, XMMATRIX, XMMATRIX, XMMATRIX, XMMATRIX, 
		        XMMATRIX,XMMATRIX, XMFLOAT4);
	bool Update(ID3D11Device*, ID3D11DeviceContext*, HWND,int);
public:
	//map<WorldPos, CubeClass> m_chunks;
	std::vector<CubeClass*> m_chunks;
	int m_chunkSize;

};
#endif
