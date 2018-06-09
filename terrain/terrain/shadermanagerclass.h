#ifndef _SHADERMANAGERCLASS_H_
#define _SHADERMANAGERCLASS_H_

#include "d3dclass.h"
#include "colorshaderclass.h"
#include "fontshaderclass.h"
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

	bool RenderColorShader(ID3D11DeviceContext*, int, XMMATRIX, XMMATRIX, XMMATRIX);
	bool RenderFontShader(ID3D11DeviceContext*, int, XMMATRIX, XMMATRIX, XMMATRIX, ID3D11ShaderResourceView*, XMFLOAT4);
	bool RenderCubesShader(ID3D11DeviceContext*, XMMATRIX, XMMATRIX, XMMATRIX, XMMATRIX,
		                   XMMATRIX, XMMATRIX, XMFLOAT4);
	bool UpdateCubeShader(ID3D11Device*, ID3D11DeviceContext*, HWND, int);

private:
	ColorShaderClass * m_ColorShader;
	FontShaderClass* m_FontShader;
	WorldClass* m_Cubes;
};

#endif