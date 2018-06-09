#include "shadermanagerclass.h"

ShaderManagerClass::ShaderManagerClass()
{
	m_Cubes = 0;
}


ShaderManagerClass::ShaderManagerClass(const ShaderManagerClass& other)
{

}


ShaderManagerClass::~ShaderManagerClass()
{

}

bool ShaderManagerClass::Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, HWND hwnd)
{
	bool result;

	m_Cubes = new WorldClass();
	result = m_Cubes->Initialize(device, deviceContext, hwnd, 64);
	if (!result)
		return false;

	return true;
}


void ShaderManagerClass::Shutdown()
{
	if (m_Cubes)
	{
		m_Cubes->Shutdown();
		delete m_Cubes;
		m_Cubes = 0;
	}

	return;
}


bool ShaderManagerClass::RenderCubesShader(ID3D11DeviceContext* deviceContext,XMMATRIX worldMatrix, XMMATRIX worldMatrix2, XMMATRIX worldMatrix3, XMMATRIX worldMatrix4,
	                                       XMMATRIX viewMatrix,XMMATRIX projectionMatrix, XMFLOAT4 lightPosition)
{

	bool v1 =  m_Cubes->Render(deviceContext, worldMatrix, worldMatrix2, worldMatrix3, worldMatrix4, 
		                       viewMatrix, projectionMatrix, lightPosition);
	return v1;
}

bool ShaderManagerClass::UpdateCubeShader(ID3D11Device* device, ID3D11DeviceContext* deviceContext, HWND hwnd, int posz)
{
	bool v = m_Cubes->Update(device, deviceContext,hwnd,posz);
	return v;
}