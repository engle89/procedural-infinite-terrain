#include "userinterfaceclass.h"

UserInterfaceClass::UserInterfaceClass()
{
	m_Font1 = 0;
	m_FpsString = 0;
	m_VideoStrings = 0;
	m_PositionStrings = 0;
}


UserInterfaceClass::UserInterfaceClass(const UserInterfaceClass& other)
{

}


UserInterfaceClass::~UserInterfaceClass()
{

}

bool UserInterfaceClass::Initialize(D3DClass* Direct3D, int screenHeight, int screenWidth)
{
	bool result;
	char videoCard[128];
	int  videoMemory;
	char videoString[144];
	char memoryString[32];
	char tempString[16];
	int  i;

	m_Font1 = new FontClass;
	if (!m_Font1)
		return false;

	char font1[44] = "D:/engle/terrain/terrain/terrain/font01.txt";
	char font2[44] = "D:/engle/terrain/terrain/terrain/font01.tga";
	result = m_Font1->Initialize(Direct3D->GetDevice(), Direct3D->GetDeviceContext(), font1, font2, 32.0f, 3);
	if (!result)
		return false;

	m_FpsString = new TextClass;
	if (!m_FpsString)
		return false;

	char fps[7] = "Fps: 0";
	result = m_FpsString->Initialize(Direct3D->GetDevice(), Direct3D->GetDeviceContext(), screenWidth, screenHeight, 16, false, m_Font1,
		fps, 10, 50, 0.0f, 1.0f, 0.0f);
	if (!result)
		return false;

	m_previousFps = -1;

	Direct3D->GetVideoCardInfo(videoCard, videoMemory);
	strcpy_s(videoString, "Video Card: ");
	strcat_s(videoString, videoCard);

	_itoa_s(videoMemory, tempString, 10);

	strcpy_s(memoryString, "Video Memory: ");
	strcat_s(memoryString, tempString);
	strcat_s(memoryString, " MB");

	m_VideoStrings = new TextClass[2];
	if (!m_VideoStrings)
		return false;

	result = m_VideoStrings[0].Initialize(Direct3D->GetDevice(), Direct3D->GetDeviceContext(), screenWidth, screenHeight, 256, false, m_Font1,
		videoString, 10, 10, 1.0f, 1.0f, 1.0f);
	if (!result)
		return false;

	result = m_VideoStrings[1].Initialize(Direct3D->GetDevice(), Direct3D->GetDeviceContext(), screenWidth, screenHeight, 32, false, m_Font1,
		memoryString, 10, 30, 1.0f, 1.0f, 1.0f);
	if (!result)
		return false;

	m_PositionStrings = new TextClass[6];
	if (!m_PositionStrings)
		return false;
	
	char p1[5] = "X: 0";
	result = m_PositionStrings[0].Initialize(Direct3D->GetDevice(), Direct3D->GetDeviceContext(), screenWidth, screenHeight, 16, false, m_Font1,
		p1, 10, 310, 1.0f, 1.0f, 1.0f);
	if (!result)
		return false;

	char p2[5] = "Y: 0";
	result = m_PositionStrings[1].Initialize(Direct3D->GetDevice(), Direct3D->GetDeviceContext(), screenWidth, screenHeight, 16, false, m_Font1,
		p2, 10, 330, 1.0f, 1.0f, 1.0f);
	if (!result)
		return false;

	char p3[5] = "Z: 0";
	result = m_PositionStrings[2].Initialize(Direct3D->GetDevice(), Direct3D->GetDeviceContext(), screenWidth, screenHeight, 16, false, m_Font1,
		p3, 10, 350, 1.0f, 1.0f, 1.0f);
	if (!result)
		return false;

	char p4[6] = "rX: 0";
	result = m_PositionStrings[3].Initialize(Direct3D->GetDevice(), Direct3D->GetDeviceContext(), screenWidth, screenHeight, 16, false, m_Font1,
		p4, 10, 370, 1.0f, 1.0f, 1.0f);
	if (!result)
		return false;

	char p5[6] = "rY: 0";
	result = m_PositionStrings[4].Initialize(Direct3D->GetDevice(), Direct3D->GetDeviceContext(), screenWidth, screenHeight, 16, false, m_Font1,
		p5, 10, 390, 1.0f, 1.0f, 1.0f);
	if (!result)
		return false;

	char p6[6] = "rZ: 0";
	result = m_PositionStrings[5].Initialize(Direct3D->GetDevice(), Direct3D->GetDeviceContext(), screenWidth, screenHeight, 16, false, m_Font1,
		p6, 10, 410, 1.0f, 1.0f, 1.0f);
	if (!result)
		return false;

	for (i = 0; i<6; i++)
		m_previousPosition[i] = -1;

	return true;
}

void UserInterfaceClass::Shutdown()
{
	if (m_PositionStrings)
	{
		m_PositionStrings[0].Shutdown();
		m_PositionStrings[1].Shutdown();
		m_PositionStrings[2].Shutdown();
		m_PositionStrings[3].Shutdown();
		m_PositionStrings[4].Shutdown();
		m_PositionStrings[5].Shutdown();

		delete[] m_PositionStrings;
		m_PositionStrings = 0;
	}

	if (m_VideoStrings)
	{
		m_VideoStrings[0].Shutdown();
		m_VideoStrings[1].Shutdown();

		delete[] m_VideoStrings;
		m_VideoStrings = 0;
	}

	if (m_FpsString)
	{
		m_FpsString->Shutdown();
		delete m_FpsString;
		m_FpsString = 0;
	}

	if (m_Font1)
	{
		m_Font1->Shutdown();
		delete m_Font1;
		m_Font1 = 0;
	}

	return;
}

bool UserInterfaceClass::Frame(ID3D11DeviceContext* deviceContext, int fps, float posX, float posY, float posZ,
	float rotX, float rotY, float rotZ)
{
	bool result;

	result = UpdateFpsString(deviceContext, fps);
	if (!result)
		return false;

	result = UpdatePositionStrings(deviceContext, posX, posY, posZ, rotX, rotY, rotZ);
	if (!result)
		return false;

	return true;
}


bool UserInterfaceClass::Render(D3DClass* Direct3D, ShaderManagerClass* ShaderManager, XMMATRIX worldMatrix, XMMATRIX viewMatrix,
	XMMATRIX orthoMatrix)
{
	int i;

	//Turn off the Z buffer and enable alpha blending to begin 2D rendering
	Direct3D->TurnZBufferOff();
	Direct3D->EnableAlphaBlending();

	m_FpsString->Render(Direct3D->GetDeviceContext(), ShaderManager, worldMatrix, viewMatrix, orthoMatrix, m_Font1->GetTexture());

	m_VideoStrings[0].Render(Direct3D->GetDeviceContext(), ShaderManager, worldMatrix, viewMatrix, orthoMatrix, m_Font1->GetTexture());
	m_VideoStrings[1].Render(Direct3D->GetDeviceContext(), ShaderManager, worldMatrix, viewMatrix, orthoMatrix, m_Font1->GetTexture());

	for (i = 0; i<6; i++)
		m_PositionStrings[i].Render(Direct3D->GetDeviceContext(), ShaderManager, worldMatrix, viewMatrix, orthoMatrix, m_Font1->GetTexture());

	//Turn off alpha blending now that the text has been rendered
	Direct3D->DisableAlphaBlending();

	//Turn the Z buffer back on now that the 2D rendering has completed
	Direct3D->TurnZBufferOn();

	return true;
}


bool UserInterfaceClass::UpdateFpsString(ID3D11DeviceContext* deviceContext, int fps)
{
	char tempString[16];
	char finalString[16];
	float red, green, blue;
	bool result;

	if (m_previousFps == fps)
		return true;

	m_previousFps = fps;

	if (fps > 99999)
		fps = 99999;

	_itoa_s(fps, tempString, 10);

	strcpy_s(finalString, "Fps: ");
	strcat_s(finalString, tempString);

	if (fps >= 60)
	{
		red = 0.0f;
		green = 1.0f;
		blue = 0.0f;
	}

	if (fps < 60)
	{
		red = 1.0f;
		green = 1.0f;
		blue = 0.0f;
	}

	if (fps < 30)
	{
		red = 1.0f;
		green = 0.0f;
		blue = 0.0f;
	}

	result = m_FpsString->UpdateSentence(deviceContext, m_Font1, finalString, 10, 50, red, green, blue);
	if (!result)
		return false;

	return true;
}

bool UserInterfaceClass::UpdatePositionStrings(ID3D11DeviceContext* deviceContext, float posX, float posY, float posZ,
	float rotX, float rotY, float rotZ)
{
	int positionX, positionY, positionZ, rotationX, rotationY, rotationZ;
	char tempString[16];
	char finalString[16];
	bool result;

	positionX = (int)posX;
	positionY = (int)posY;
	positionZ = (int)posZ;
	rotationX = (int)rotX;
	rotationY = (int)rotY;
	rotationZ = (int)rotZ;

	if (positionX != m_previousPosition[0])
	{
		m_previousPosition[0] = positionX;
		_itoa_s(positionX, tempString, 10);
		strcpy_s(finalString, "X: ");
		strcat_s(finalString, tempString);
		result = m_PositionStrings[0].UpdateSentence(deviceContext, m_Font1, finalString, 10, 100, 1.0f, 1.0f, 1.0f);
		if (!result) { return false; }
	}

	if (positionY != m_previousPosition[1])
	{
		m_previousPosition[1] = positionY;
		_itoa_s(positionY, tempString, 10);
		strcpy_s(finalString, "Y: ");
		strcat_s(finalString, tempString);
		result = m_PositionStrings[1].UpdateSentence(deviceContext, m_Font1, finalString, 10, 120, 1.0f, 1.0f, 1.0f);
		if (!result) { return false; }
	}

	if (positionZ != m_previousPosition[2])
	{
		m_previousPosition[2] = positionZ;
		_itoa_s(positionZ, tempString, 10);
		strcpy_s(finalString, "Z: ");
		strcat_s(finalString, tempString);
		result = m_PositionStrings[2].UpdateSentence(deviceContext, m_Font1, finalString, 10, 140, 1.0f, 1.0f, 1.0f);
		if (!result) { return false; }
	}

	if (rotationX != m_previousPosition[3])
	{
		m_previousPosition[3] = rotationX;
		_itoa_s(rotationX, tempString, 10);
		strcpy_s(finalString, "rX: ");
		strcat_s(finalString, tempString);
		result = m_PositionStrings[3].UpdateSentence(deviceContext, m_Font1, finalString, 10, 180, 1.0f, 1.0f, 1.0f);
		if (!result) { return false; }
	}

	if (rotationY != m_previousPosition[4])
	{
		m_previousPosition[4] = rotationY;
		_itoa_s(rotationY, tempString, 10);
		strcpy_s(finalString, "rY: ");
		strcat_s(finalString, tempString);
		result = m_PositionStrings[4].UpdateSentence(deviceContext, m_Font1, finalString, 10, 200, 1.0f, 1.0f, 1.0f);
		if (!result) { return false; }
	}

	if (rotationZ != m_previousPosition[5])
	{
		m_previousPosition[5] = rotationZ;
		_itoa_s(rotationZ, tempString, 10);
		strcpy_s(finalString, "rZ: ");
		strcat_s(finalString, tempString);
		result = m_PositionStrings[5].UpdateSentence(deviceContext, m_Font1, finalString, 10, 220, 1.0f, 1.0f, 1.0f);
		if (!result) { return false; }
	}

	return true;
}

