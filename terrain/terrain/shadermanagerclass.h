#ifndef _SHADERMANAGERCLASS_H_
#define _SHADERMANAGERCLASS_H_

#include "d3dclass.h"
#include "cubesclass.h"
#include "cameraclass.h"
#include "worldclass.h"

class ShaderManagerClass
{
public:
	ShaderManagerClass();
	ShaderManagerClass(const ShaderManagerClass&);
	~ShaderManagerClass();

	bool Initialize(ID3D11Device*, ID3D11DeviceContext*, HWND);
	void Shutdown();

	bool RenderCubesShader(ID3D11DeviceContext*, XMMATRIX, XMMATRIX, XMMATRIX, XMMATRIX,
		                   XMMATRIX, XMMATRIX, XMFLOAT4);
	bool UpdateCubeShader(ID3D11Device*, ID3D11DeviceContext*, HWND, int);

private:
	WorldClass* m_Cubes;
};

#endif