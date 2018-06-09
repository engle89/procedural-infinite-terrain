#include "terrainclass.h"

TerrainClass::TerrainClass()
{
	m_vertexBuffer = 0;
	m_indexBuffer = 0;
	m_terrainFilename = 0;
	m_heightMap = 0;
	m_terrainModel = 0;
}

TerrainClass::TerrainClass(const TerrainClass& other)
{

}

TerrainClass::~TerrainClass()
{

}

bool TerrainClass::Initialize(ID3D11Device* device, char* setupFilename)
{
	bool result;

	result = LoadSetupFile(setupFilename);
	if (!result)
		return false;

	result = LoadBitmapHeightMap();
	if (!result)
		return false;

	SetTerrainCoordinates();

	result = BuildTerrainModel();
	if (!result)
		return false;

	ShutdownHeightMap();

	//Load the rendering buffers with the terrain data
	result = InitializeBuffers(device);
	if (!result)
		return false;

	ShutdownTerrainModel();

	return true;
}

void TerrainClass::Shutdown()
{
	//Release the rendering buffers
	ShutdownBuffers();

	ShutdownTerrainModel();

	ShutdownHeightMap();

	return;
}

bool TerrainClass::Render(ID3D11DeviceContext* deviceContext)
{
	//put the vertex and index buffers on the graphics pipeline to prepare them for drawing
	RenderBuffers(deviceContext);
	return true;
}

int TerrainClass::GetIndexCount()
{
	return m_indexCount;
}

bool TerrainClass::LoadSetupFile(char* filename)
{
	int stringLength;
	ifstream fin;
	char input;

	stringLength = 256;
	m_terrainFilename = new char[stringLength];
	if (!m_terrainFilename)
		return false;

	fin.open(filename);
	if (fin.fail())
		return false;

	//read up tp the terrain file name
	fin.get(input);
	while (input != ':')
		fin.get(input);
	//read the terrain file name
	fin >> m_terrainFilename;

	fin.get(input);
	while (input != ':')
		fin.get(input);
	fin >> m_terrainHeight;

	fin.get(input);
	while (input != ':')
		fin.get(input);
	fin >> m_terrainWidth;

	fin.get(input);
	while (input != ':')
		fin.get(input);
	fin >> m_heightScale;

	fin.close();
	return true;
}

bool TerrainClass::LoadBitmapHeightMap()
{
	int error, imageSize, i, j, k, index;
	FILE* filePtr;
	unsigned long long count;
	BITMAPFILEHEADER bitmapFileHeader;
	BITMAPINFOHEADER bitmapInfoHeader;
	unsigned char* bitmapImage;
	unsigned char height;

	m_heightMap = new HeightMapType[m_terrainWidth*m_terrainHeight];
	if (!m_heightMap)
		return false;

	error = fopen_s(&filePtr, m_terrainFilename, "rb");
	if (error != 0)
		return false;

	count = fread(&bitmapFileHeader, sizeof(BITMAPFILEHEADER), 1, filePtr);
	if (count != 1)
		return false;

	count = fread(&bitmapInfoHeader, sizeof(BITMAPINFOHEADER), 1, filePtr);
	if (count != 1)
		return false;

	//Make sure the height map dimensions are the same as the terrain dimensions for easy 1 to 1 mapping
	if ((bitmapInfoHeader.biHeight != m_terrainHeight) || (bitmapInfoHeader.biWidth != m_terrainWidth))
		return false;

	//Calculate the size of the bitmap image data.  
	//Since we use non-divide by 2 dimensions (eg. 257x257) we need to add an extra byte to each line
	imageSize = m_terrainHeight * ((m_terrainWidth * 3) + 1);

	bitmapImage = new unsigned char[imageSize];
	if (!bitmapImage)
		return false;

	//Move to the beginning of the bitmap data
	fseek(filePtr, bitmapFileHeader.bfOffBits, SEEK_SET);

	count = fread(bitmapImage, 1, imageSize, filePtr);
	if (count != imageSize)
		return false;

	error = fclose(filePtr);
	if (error != 0)
		return false;

	//Initialize the position in the image data buffer
	k = 0;
	//Read the image data into the height map array
	for (j = 0; j < m_terrainHeight; j++)
	{
		for (i = 0; i < m_terrainWidth; i++)
		{
			//Bitmaps are upside down so load bottom to top into the height map array
			index = (m_terrainWidth*(m_terrainHeight - 1 - j)) + i;

			height = bitmapImage[k];
			m_heightMap[index].y = (float)height;
			k += 3;
		}
		//Compensate for the extra byte at end of each line in non-divide by 2 bitmaps (eg. 257x257)
		k++;
	}

	delete[] bitmapImage;
	bitmapImage = 0;

	delete[] m_terrainFilename;
	m_terrainFilename = 0;

	return true;
}

void TerrainClass::ShutdownHeightMap()
{
	if (m_heightMap)
	{
		delete[] m_heightMap;
		m_heightMap = 0;
	}
	return;
}

void TerrainClass::SetTerrainCoordinates()
{
	int i, j, index;

	for (j = 0; j < m_terrainHeight; j++)
	{
		for (i = 0; i < m_terrainWidth; i++)
		{
			index = (m_terrainWidth*j) + i;

			m_heightMap[index].x = (float)i;
			m_heightMap[index].z = -(float)j;

			//Move the terrain depth into the positive range. For example from (0, -256) to (256, 0)
			m_heightMap[index].z += (float)(m_terrainHeight - 1);

			m_heightMap[index].y /= m_heightScale;
		} 
	}
	return;
}

bool TerrainClass::BuildTerrainModel()
{
	int i, j, index, index1, index2, index3, index4;

	m_vertexCount = (m_terrainHeight - 1)*(m_terrainWidth - 1) * 6;

	m_terrainModel = new ModelType[m_vertexCount];
	if (!m_terrainModel)
		return false;

	index = 0;

	//Load the 3D terrain model with the height map terrain data
	//We will be creating 2 triangles for each of the four points in a quad
	for (j = 0; j < (m_terrainWidth - 1); j++)
	{
		for (i = 0; i < (m_terrainHeight - 1); i++)
		{
			//Get the indexes to the four points of the quad
			index1 = (m_terrainWidth*j)     +  i;    //Upper left
			index2 = (m_terrainWidth*j)     + (i+1); //Upper right
			index3 = (m_terrainWidth*(j+1)) + i;     //Bottom left
			index4 = (m_terrainWidth*(j+1)) + (i+1); //Bottom right

	        // Now create two triangles for that quad.

		    // Triangle 1 - Upper left.
			m_terrainModel[index].x = m_heightMap[index1].x;
			m_terrainModel[index].y = m_heightMap[index1].y;
			m_terrainModel[index].z = m_heightMap[index1].z;
			index++;

			// Triangle 1 - Upper right.
			m_terrainModel[index].x = m_heightMap[index2].x;
			m_terrainModel[index].y = m_heightMap[index2].y;
			m_terrainModel[index].z = m_heightMap[index2].z;
			index++;

			// Triangle 1 - Bottom left.
			m_terrainModel[index].x = m_heightMap[index3].x;
			m_terrainModel[index].y = m_heightMap[index3].y;
			m_terrainModel[index].z = m_heightMap[index3].z;
			index++;

			// Triangle 2 - Bottom left.
			m_terrainModel[index].x = m_heightMap[index3].x;
			m_terrainModel[index].y = m_heightMap[index3].y;
			m_terrainModel[index].z = m_heightMap[index3].z;
			index++;

			// Triangle 2 - Upper right.
			m_terrainModel[index].x = m_heightMap[index2].x;
			m_terrainModel[index].y = m_heightMap[index2].y;
			m_terrainModel[index].z = m_heightMap[index2].z;
			index++;

			// Triangle 1 - Bottom right.
			m_terrainModel[index].x = m_heightMap[index4].x;
			m_terrainModel[index].y = m_heightMap[index4].y;
			m_terrainModel[index].z = m_heightMap[index4].z;
			index++;
		}
	}
	return true;
}

void TerrainClass::ShutdownTerrainModel()
{
	if (m_terrainModel)
	{
		delete[] m_terrainModel;
		m_terrainModel = 0;
	}

	return;
}

bool TerrainClass::InitializeBuffers(ID3D11Device* device)
{
	VertexType* vertices;
	unsigned long* indices;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;
	int i;
	XMFLOAT4 color;

	color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

	m_vertexCount = (m_terrainWidth - 1)*(m_terrainHeight - 1) * 6;

	m_indexCount = m_vertexCount;

	vertices = new VertexType[m_vertexCount];
	if (!vertices)
		return false;
	indices = new unsigned long[m_indexCount];
	if (!indices)
		return false;

	//Load the vertex array and index array with 3D terrain model data
	for (i = 0; i < m_vertexCount; i++)
	{
		vertices[i].position = XMFLOAT3(m_terrainModel[i].x, m_terrainModel[i].y, m_terrainModel[i].z);
		vertices[i].color = color;
		indices[i] = i;
	}

	//Set up the description of the static vertex buffer
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexType)*m_vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	//Give the subresources structure a pointer to the vertex data
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	//Now create the vertex buffer
	result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer);
	if (FAILED(result))
		return false;

	//Set up the description of the static index buffer
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long)*m_indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	//Give the subresources structure a pointer to the index data
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	//Now create the index buffer
	result = device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer);
	if (FAILED(result))
		return false;

	//Release the arrays now that the buffers have been created and loaded
	delete[] vertices;
	vertices = 0;

	delete[] indices;
	indices = 0;

	return true;
}

void TerrainClass::ShutdownBuffers()
{
	if (m_indexBuffer)
	{
		m_indexBuffer->Release();
		m_indexBuffer = 0;
	}

	if (m_vertexBuffer)
	{
		m_vertexBuffer->Release();
		m_vertexBuffer = 0;
	}

	return;
}

void TerrainClass::RenderBuffers(ID3D11DeviceContext* deviceContext)
{
	unsigned int stride;
	unsigned int offset;

	//Set vertex buffer stride and offset
	stride = sizeof(VertexType);
	offset = 0;

	//Set the vertex buffer to active in the input assembler so it can be rendered
	deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);

	//Set the index buffer to active in the input assembler so it can be rendered
	deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	//Set the type of primitive that should be rendered from this vertex buffer, in this case triangles
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	return;
}
