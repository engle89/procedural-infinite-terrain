#include "shadermanagerclass.h"

ShaderManagerClass::ShaderManagerClass()
{
	m_ColorShader = 0;
	m_FontShader = 0;
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

	m_ColorShader = new ColorShaderClass;
	if (!m_ColorShader)
		return false;

	result = m_ColorShader->Initialize(device, hwnd);
	if (!result)
		return false;

	m_FontShader = new FontShaderClass;
	if (!m_FontShader)
		return false;

	result = m_FontShader->Initialize(device, hwnd);
	if (!result)
		return false;

	m_Cubes = new WorldClass();
	result = m_Cubes->Initialize(device, deviceContext, hwnd, 64);
	if (!result)
		return false;

	return true;
}


void ShaderManagerClass::Shutdown()
{
	if (m_FontShader)
	{
		m_FontShader->Shutdown();
		delete m_FontShader;
		m_FontShader = 0;
	}

	if (m_ColorShader)
	{
		m_ColorShader->Shutdown();
		delete m_ColorShader;
		m_ColorShader = 0;
	}

	if (m_Cubes)
	{
		m_Cubes->Shutdown();
		delete m_Cubes;
		m_Cubes = 0;
	}

	return;
}


bool ShaderManagerClass::RenderColorShader(ID3D11DeviceContext* deviceContext, int indexCount, XMMATRIX worldMatrix, XMMATRIX viewMatrix,
	XMMATRIX projectionMatrix)
{
	return m_ColorShader->Render(deviceContext, indexCount, worldMatrix, viewMatrix, projectionMatrix);
}


bool ShaderManagerClass::RenderFontShader(ID3D11DeviceContext* deviceContext, int indexCount, XMMATRIX worldMatrix, XMMATRIX viewMatrix,
	XMMATRIX projectionMatrix, ID3D11ShaderResourceView* texture, XMFLOAT4 color)
{
	return m_FontShader->Render(deviceContext, indexCount, worldMatrix, viewMatrix, projectionMatrix, texture, color);
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