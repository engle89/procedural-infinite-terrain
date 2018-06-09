#include "worldclass.h"

WorldClass::WorldClass()
{

}

WorldClass::WorldClass(const WorldClass&)
{

}

WorldClass::~WorldClass()
{

}

bool WorldClass::CreateChunk(ID3D11Device* device, ID3D11DeviceContext* deviceContext,HWND hwnd,int chunkSize, int ox, int oz)
{
	int height, depth, width;
	height = depth = width = chunkSize;
	XMFLOAT3 cubeSize(chunkSize, chunkSize, chunkSize);
	//2.0f to decrease density
	XMFLOAT3 cubeStep(2.0f / cubeSize.x, 2.0f / cubeSize.y, 2.0f / cubeSize.z);
	XMFLOAT4 dataStep(10.0f / (float)width, 10.0f / (float)height, 10.0f / (float)depth, 1);
	CubeClass* newcube = new CubeClass(width,height,depth,cubeSize,cubeStep,dataStep,ox,oz);

	bool result = newcube->Initialize(device, deviceContext, hwnd);

	if (!result)
		return false;
	m_chunks.push_back(newcube);
	return true;
}

//create 9 chunks
bool WorldClass::Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext,HWND hwnd,int chunkSize)
{
	m_chunkSize = chunkSize;

	bool v1 = CreateChunk(device, deviceContext, hwnd, chunkSize,    0, 0);
	if (v1 != true)
		return false;

	bool v2 = CreateChunk(device, deviceContext, hwnd, chunkSize,    63, 0);
	if (v2 != true)
		return false;

	bool v3 = CreateChunk(device, deviceContext, hwnd, chunkSize,   0, 63);
	if (v3 != true)
		return false;

	bool v4 = CreateChunk(device, deviceContext, hwnd, chunkSize,   63, 63);
	if (v4 != true)
		return false;

}

void WorldClass::Shutdown()
{
	std::vector<CubeClass*>::iterator it;
	for (it = m_chunks.begin(); it != m_chunks.end(); it++)
		(*it)->Shutdown();
}

bool WorldClass::Render(ID3D11DeviceContext* deviceContext, 
	XMMATRIX worldMatrix, XMMATRIX worldMatrix2, XMMATRIX worldMatrix3, XMMATRIX worldMatrix4, 
	XMMATRIX viewMatrix, XMMATRIX projectionMatrix, XMFLOAT4 lightPosition)
{
	std::vector<CubeClass*>::iterator it;
	int i = 1;
	for (it = m_chunks.begin(); it != m_chunks.end(); it++, i++)
	{
		bool v;
		switch (i)
		{ 
		case 1:
			v = (*it)->Render(deviceContext, worldMatrix, viewMatrix, projectionMatrix, lightPosition);
			if (v != true)
				return false;
			break;
		case 2:
			v = (*it)->Render(deviceContext, worldMatrix2, viewMatrix, projectionMatrix, lightPosition);
			if (v != true)
				return false;
			break;
		case 3:
			v = (*it)->Render(deviceContext, worldMatrix3, viewMatrix, projectionMatrix, lightPosition);
			if (v != true)
				return false;
			break;
		case 4:
			v = (*it)->Render(deviceContext, worldMatrix4, viewMatrix, projectionMatrix, lightPosition);
			if (v != true)
				return false;
			break;
		default:
			return false;
		}

	}
}

bool WorldClass::Update(ID3D11Device* device, ID3D11DeviceContext* deviceContext, HWND hwnd, int posz)
{
	std::vector<CubeClass*>::iterator it;
	it = m_chunks.begin();
	m_chunks.erase(it);
	it = m_chunks.begin();
	m_chunks.erase(it);

	bool v1 = CreateChunk(device, deviceContext, hwnd, m_chunkSize, 0, posz);
	if (v1 != true)
		return false;

	bool v2 = CreateChunk(device, deviceContext, hwnd, m_chunkSize, 63, posz);
	if (v2 != true)
		return false;


	return true;
}