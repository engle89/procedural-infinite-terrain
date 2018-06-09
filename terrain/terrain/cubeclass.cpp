#include "cubesclass.h"

CubeClass::CubeClass(int x, int y, int z, XMFLOAT3 CubeSize, XMFLOAT3 CubeStep, XMFLOAT4 dataStep,int ox,int oz)
{
	m_width = x;
	m_height = y;
	m_depth = z;
	m_cubeSize = CubeSize;
	m_cubeStep = CubeStep;
	m_dataStep = dataStep;
	m_offsetx = ox;
	m_offsetz = oz;
}

CubeClass::CubeClass(const CubeClass&)
{

}

CubeClass::~CubeClass()
{

}

bool CubeClass::Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, HWND hwnd)
{
	HRESULT result;

	result = SetUpDataResource(device);
	if (FAILED(result))
		return false;

	WCHAR vsFilename[63] = L"D:/engle/terrain/terrain/terrain/MarchingCubeVertexShader.hlsl";
	WCHAR gsFilename[65] = L"D:/engle/terrain/terrain/terrain/MarchingCubeGeometryShader.hlsl";
	WCHAR psFilename[62] = L"D:/engle/terrain/terrain/terrain/MarchingCubePixelShader.hlsl";
	result = InitializeShader(device, hwnd, vsFilename, gsFilename,psFilename);
	if (FAILED(result))
		return false;

	m_terrain = new TextureClass();
	char grass[43] = "D:/engle/terrain/terrain/terrain/rock.tga";
	bool v = m_terrain->Initialize(device, deviceContext, grass);
	if (v != true)
		return false;
	m_terrainData = m_terrain->GetTexture();

	return true;
}

void CubeClass::Shutdown()
{
	ShutdownShader();
	return;
}

bool CubeClass::Render(ID3D11DeviceContext* deviceContext, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix, XMFLOAT4 lightPosition)
{
	bool result;
	result = SetShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix, lightPosition);
	if (!result)
		return false;

	RenderShader(deviceContext);

	return true;
}

HRESULT CubeClass::InitializeShader(ID3D11Device* device, HWND hwnd, WCHAR* vsFilename, WCHAR* gsFilename, WCHAR* psFilename)
{
	HRESULT result;
	ID3D10Blob* errorMessage = 0;
	ID3D10Blob* vertexShaderBuffer = 0;
	ID3D10Blob* geometryShaderBuffer = 0;
	ID3D10Blob* pixelShaderBuffer = 0;
	D3D11_INPUT_ELEMENT_DESC polygonLayout[2];
	unsigned int numElements;
	VertexType* vertices;
	D3D11_BUFFER_DESC vertexBufferDesc;
	D3D11_BUFFER_DESC matrixBufferDesc;
	D3D11_BUFFER_DESC soBufferDesc;

	//compile shaders
	result = D3DCompileFromFile(vsFilename, NULL, NULL, "MarchingCubeVertexShader", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &vertexShaderBuffer, &errorMessage);
	if (FAILED(result))
	{
		if (errorMessage)
			OutputShaderErrorMessage(errorMessage, hwnd, vsFilename);
		else
			MessageBox(hwnd, vsFilename, L"Missing Shader File", MB_OK);

		return false;
	}

	result = D3DCompileFromFile(gsFilename, NULL, NULL, "MarchingCubeGeometryShader", "gs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &geometryShaderBuffer, &errorMessage);
	if (FAILED(result))
	{
		if (errorMessage)
			OutputShaderErrorMessage(errorMessage, hwnd, gsFilename);
		else
			MessageBox(hwnd, vsFilename, L"Missing Shader File", MB_OK);

		return false;
	}

	result = D3DCompileFromFile(psFilename, NULL, NULL, "MarchingCubePixelShader", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &pixelShaderBuffer, &errorMessage);
	if (FAILED(result))
	{
		if (errorMessage)
			OutputShaderErrorMessage(errorMessage, hwnd, psFilename);
		else
			MessageBox(hwnd, psFilename, L"Missing Shader File", MB_OK);

		return false;
	}

	result = device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, &m_vertexShader);
	if (FAILED(result))
		return false;

	//stream outputstage input signature declaration
	D3D11_SO_DECLARATION_ENTRY decl[] = 
	{
		{ 0, "SV_POSITION", 0, 0, 4, 0 },
	    { 0, "COLOR", 0, 0, 4, 0 },
	    { 0, "TEXCOORD", 0, 0, 4, 0 }
	};
	UINT stream = (UINT)0;
	result = device->CreateGeometryShaderWithStreamOutput(geometryShaderBuffer->GetBufferPointer(), 
		                                                  geometryShaderBuffer->GetBufferSize(), 
		                                                  decl, //so declaration
		                                                  (UINT)3, //num of entries
		                                                  NULL, 
		                                                  0,
		                                                  stream,
		                                                  NULL,
		                                                  &m_geometryShader);
	if (FAILED(result))
		return false;

	result = device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, &m_pixelShader);
	if (FAILED(result))
		return false;

	//vertex shader input data
	polygonLayout[0].SemanticName = "POSITION";
	polygonLayout[0].SemanticIndex = 0;
	polygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[0].InputSlot = 0;
	polygonLayout[0].AlignedByteOffset = 0;
	polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[0].InstanceDataStepRate = 0;

	polygonLayout[1].SemanticName = "COLOR";
	polygonLayout[1].SemanticIndex = 0;
	polygonLayout[1].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	polygonLayout[1].InputSlot = 0;
	polygonLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[1].InstanceDataStepRate = 0;

	numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

	result = device->CreateInputLayout(polygonLayout, numElements, vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), &m_layout);
	if (FAILED(result))
		return false;

	vertexShaderBuffer->Release();
	vertexShaderBuffer = 0;

	geometryShaderBuffer->Release();
    geometryShaderBuffer = 0;

	pixelShaderBuffer->Release();
	pixelShaderBuffer = 0;



	//set up shader buffers
	//vertex buffer
	int size = int(2.0f / m_cubeStep.x);
	size = size * size * size;
	int vertexCount = size;
	m_vertexCount = size;
	vertices = new VertexType[vertexCount];
	int id = 0;
	for (float z = -1; z < 1.0f; z += m_cubeStep.z) {
		for (float y = -1; y < 1.0f; y += m_cubeStep.y) {
			for (float x = -1; x < 1.0f; x += m_cubeStep.x) {
				vertices[id].position = XMFLOAT4(x, y, z, 1);
				vertices[id].color    = XMFLOAT4((y + 1.0f) / 2.0f, 0.0f, 0.0f, 1.0f);
				id++;
			}
		}
	}

	ZeroMemory(&vertexBufferDesc, sizeof(vertexBufferDesc));
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	//vertex buffer initial data description
	D3D11_SUBRESOURCE_DATA initData;
	ZeroMemory(&initData, sizeof(initData));
	initData.pSysMem = vertices;
	result = device->CreateBuffer(&vertexBufferDesc, &initData, &m_vertexBuffer);
	if (FAILED(result))
		return false;


	//constant buffer
	//matrix buffer
	matrixBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = 0;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;

	result = device->CreateBuffer(&matrixBufferDesc, NULL, &m_matrixBuffer);
	if (FAILED(result))
		return false;


	//constant buffer
	//decal buffer
	D3D11_BUFFER_DESC Desc;
	ZeroMemory(&Desc, sizeof(Desc));
	Desc.Usage = D3D11_USAGE_DEFAULT;
	Desc.ByteWidth = sizeof(VertexDecalType);
	Desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	Desc.CPUAccessFlags = 0;
	Desc.MiscFlags = 0;
	Desc.StructureByteStride = 0;

	result = device->CreateBuffer(&Desc, NULL, &m_decalBuffer);
	if (FAILED(result))
		return false;

	//SO stage buffer
	ZeroMemory(&soBufferDesc, sizeof(soBufferDesc));
	soBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	soBufferDesc.ByteWidth = m_vertexCount*sizeof(SOType);
	soBufferDesc.BindFlags = D3D11_BIND_STREAM_OUTPUT;
	soBufferDesc.CPUAccessFlags = 0;
	soBufferDesc.MiscFlags = 0;
	soBufferDesc.StructureByteStride = 0;

	result = device->CreateBuffer(&soBufferDesc, NULL, &m_soBuffer);
	if (FAILED(result))
		return false;


	//create a basic point sampler for sampling our density data in the gpu
	//should refactor this elsewhere
	D3D11_SAMPLER_DESC sampleDesc;
	ZeroMemory(&sampleDesc, sizeof(sampleDesc));
	sampleDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampleDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampleDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampleDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampleDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampleDesc.MinLOD = 0;
	sampleDesc.MaxLOD = 0;
	result = device->CreateSamplerState(&sampleDesc, &m_SamplerPoint);
	if (FAILED(result))
		return false;

	//create a basic point sampler for sampling our terrain data in the gpu
	//should refactor this elsewhere
	D3D11_SAMPLER_DESC sampleDesc2;
	ZeroMemory(&sampleDesc2, sizeof(sampleDesc2));
	sampleDesc2.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampleDesc2.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampleDesc2.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	//sampleDesc2.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampleDesc2.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	//sampleDesc2.BorderColor[0] = 0;
	//sampleDesc2.BorderColor[1] = 0;
	//sampleDesc2.BorderColor[2] = 0;
	//sampleDesc2.BorderColor[3] = 0;
	sampleDesc2.MinLOD = 0;
	sampleDesc2.MaxLOD = 0;
	result = device->CreateSamplerState(&sampleDesc2, &m_TerrainSamplerPoint);
	if (FAILED(result))
		return false;

	return result;
}

void CubeClass::ShutdownShader()
{
	if (m_matrixBuffer)
	{
		m_matrixBuffer->Release();
		m_matrixBuffer = 0;
	}

	if (m_layout)
	{
		m_layout->Release();
		m_layout = 0;
	}

	if (m_pixelShader)
	{
		m_pixelShader->Release();
		m_pixelShader = 0;
	}

	if (m_vertexShader)
	{
		m_vertexShader->Release();
		m_vertexShader = 0;
	}

	if (m_geometryShader)
	{
		m_geometryShader->Release();
		m_geometryShader = 0;
	}

	if (m_SamplerPoint)
	{
		m_SamplerPoint->Release();
		m_SamplerPoint = 0;
	}

	return;
}

void CubeClass::OutputShaderErrorMessage(ID3D10Blob* errorMessage, HWND hwnd, WCHAR* shaderFilename)
{
	char* compileErrors;
	unsigned long long bufferSize, i;
	ofstream fout;

	compileErrors = (char*)(errorMessage->GetBufferPointer());

	bufferSize = errorMessage->GetBufferSize();

	fout.open("shader-error.txt");

	for (i = 0; i<bufferSize; i++)
		fout << compileErrors[i];

	fout.close();

	errorMessage->Release();
	errorMessage = 0;

	MessageBox(hwnd, L"Error compiling shader.  Check shader-error.txt for message.", shaderFilename, MB_OK);

	return;
}

bool CubeClass::SetShaderParameters(ID3D11DeviceContext* deviceContext, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix, XMFLOAT4 lightPosition)
{
	unsigned int bufferNumber;

	worldMatrix = XMMatrixTranspose(worldMatrix);
	viewMatrix = XMMatrixTranspose(viewMatrix);
	projectionMatrix = XMMatrixTranspose(projectionMatrix);

	//write data to constant matrix buffer
	MatrixBufferType cb;
	cb.world = worldMatrix;
	cb.view = viewMatrix;
	cb.projection = projectionMatrix;
	cb.lightPosition = lightPosition;
	//XMMATRIX mWVP = worldMatrix * viewMatrix*projectionMatrix;
	deviceContext->UpdateSubresource(m_matrixBuffer, 0, NULL, &cb, 0, 0);
	

	//update decal buffer
	VertexDecalType dbuffer;
	GetDecals(&dbuffer);
	dbuffer.dataStep = m_dataStep;
	deviceContext->UpdateSubresource(m_decalBuffer, 0, NULL, &dbuffer, 0, 0);


	//Set the position of the matrix constant buffer in the vertex shader
	bufferNumber = 0;

	//Finanly set the matrix constant buffer in the vertex shader with the updated values
	deviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_matrixBuffer);

	//geomtry shader
	deviceContext->GSSetConstantBuffers(0, 1, &m_matrixBuffer);
	deviceContext->GSSetConstantBuffers(bufferNumber+1, 1, &m_decalBuffer);
	deviceContext->GSSetSamplers(0, 1, &m_SamplerPoint);
	deviceContext->GSSetShaderResources(0, 1, &m_triTableSRV);
	deviceContext->GSSetShaderResources(1, 1, &m_densityData);

	//pixel shader
	deviceContext->PSSetConstantBuffers(bufferNumber, 1, &m_matrixBuffer);
	deviceContext->PSSetConstantBuffers(bufferNumber+1, 1, &m_decalBuffer);
	deviceContext->PSSetSamplers(0, 1, &m_SamplerPoint);
	deviceContext->PSSetSamplers(1, 1, &m_TerrainSamplerPoint);
	deviceContext->PSSetShaderResources(0, 1, &m_densityData);
	deviceContext->PSSetShaderResources(1, 1, &m_terrainData);
	deviceContext->PSSetSamplers(0, 1, &m_SamplerPoint);

	return true;
}

void CubeClass::RenderShader(ID3D11DeviceContext* deviceContext)
{
	deviceContext->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_POINTLIST);

	//Set the vertex input layout
	deviceContext->IASetInputLayout(m_layout);

	//Set the vertex buffer to use
	UINT stride = sizeof(VertexType);
	UINT offset = 0;
	deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);

	//Enable vertex shader
	deviceContext->VSSetShader(m_vertexShader, NULL, 0);

	//Set the SO buffer to use
	UINT soffset[1] = { 0 };
	deviceContext->SOSetTargets(1, &m_soBuffer, soffset);

	//Enable geometry shader
	deviceContext->GSSetShader(m_geometryShader, NULL, 0);

	//Enable pixel shader
	deviceContext->PSSetShader(m_pixelShader, NULL, 0);

	// Render the data.
	deviceContext->Draw(m_vertexCount, 0);


	return;
}

//populated the list of cube corner offsets from the center
void CubeClass::GetDecals(VertexDecalType* buffer)
{
	//ZeroMemory(&buffer, sizeof(buffer));
	buffer->decal[0] = XMFLOAT4(0.0f, 0.0f, 0.0f, 1);
	buffer->decal[1] = XMFLOAT4(m_cubeStep.x, 0.0f, 0.0f, 1);
	buffer->decal[2] = XMFLOAT4(m_cubeStep.x, m_cubeStep.y, 0.0f, 1);
	buffer->decal[3] = XMFLOAT4(0.0f, m_cubeStep.y, 0.0f, 1);
	buffer->decal[4] = XMFLOAT4(0.0f, 0.0f, m_cubeStep.z, 1);
	buffer->decal[5] = XMFLOAT4(m_cubeStep.x, 0.0f, m_cubeStep.z, 1);
	buffer->decal[6] = XMFLOAT4(m_cubeStep.x, m_cubeStep.y, m_cubeStep.z, 1);
	buffer->decal[7] = XMFLOAT4(0.0f, m_cubeStep.y, m_cubeStep.z, 1);
}

HRESULT CubeClass::SetUpDataResource(ID3D11Device* device)
{
	HRESULT result;
	result = CreateTriTableResource(device);
	result = CreateDensityResource(device);
	return result;
}

//create a tritable shader resource view for accessing data in a resource
HRESULT CubeClass::CreateTriTableResource(ID3D11Device* device)
{
	D3D11_TEXTURE2D_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.Height = 256;
	desc.Width = 16;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_R32_SINT;
	desc.SampleDesc = { 1,0 };
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA initData;
	ZeroMemory(&initData, sizeof(initData));
	initData.SysMemPitch = 16 * sizeof(int);
	initData.SysMemSlicePitch = 0;

	initData.pSysMem = m_TriTable;

	ID3D11Texture2D* texture = NULL;
	HRESULT result = device->CreateTexture2D(&desc, &initData, &texture);
	if (FAILED(result))
		return result;

	result = device->CreateShaderResourceView(texture, NULL, &m_triTableSRV);
	return result;
}

//create density data
HRESULT CubeClass::CreateDensityResource(ID3D11Device* device)
{
	HRESULT result;

	m_data = new float[m_depth*m_height*m_width];
	Perlin* noise = new Perlin(-1);
	for (int z =m_offsetz ; z < m_depth+m_offsetz; z++)
	{
		for (int y = 0; y < m_height; y++)
		{
			for (int x =m_offsetx; x < m_width+m_offsetx; x++)
			{
				float density;
				density = -y-15;
				
				float warp = noise->perlin(x * 0.004, y * 0.004, z * 0.004);
			    density += warp * 6;

				float temp =noise-> perlin(x * 4.06, y * 4.06, z * 4.06) * 0.25;
				density += temp;
				temp = noise->perlin(x * 1.96, y * 1.96, z * 1.96) * 0.50;
				density += temp;
				temp = noise->perlin(x * 1.01, y * 1.01, z * 1.01) * 1.00;
				density += temp;
				temp = noise->perlin(x * 0.55, y * 0.55, z * 0.55) * 2.00;
				density += temp;
				temp = noise->perlin(x * 0.30, y * 0.30, z * 0.30) * 4.00;
				density += temp;
				temp = noise->perlin(x * 0.14, y * 0.14, z * 0.14) * 8.00;
				density += temp;
				temp = noise->perlin(x * 0.065, y * 0.065, z * 0.065) * 16.00;
				density += temp;
				temp = noise->perlin(x * 0.027, y * 0.027, z * 0.027) * 33.00;
				density += temp;
				

				int idx = (x-m_offsetx) + m_width * (y + m_height * (z-m_offsetz));
				m_data[idx] = density;
			}
		}
	}


	D3D11_TEXTURE3D_DESC Desc;
	Desc.Width = m_width;
	Desc.Height = m_height;
	Desc.Depth = m_depth;
	Desc.MipLevels = 1;
	Desc.Format = DXGI_FORMAT_R32_FLOAT;
	Desc.Usage = D3D11_USAGE_DEFAULT;
	Desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	Desc.CPUAccessFlags = 0;
	Desc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA  subData;
	ZeroMemory(&subData, sizeof(subData));
	subData.SysMemPitch = m_width*sizeof(float);
	subData.SysMemSlicePitch = m_width * m_height * sizeof(float);
	subData.pSysMem = m_data;
	result = device->CreateTexture3D(&Desc, &subData, &m_texture);
	if (FAILED(result))
		return false;

	result = device->CreateShaderResourceView(m_texture, NULL, &m_densityData);
	if (FAILED(result))
		return false;

	return result;
}
