#include "zoneclass.h"

ZoneClass::ZoneClass()
{
	m_UserInterface = 0;
	m_Camera = 0;
	m_Position = 0;
	m_Terrain = 0;
}

ZoneClass::ZoneClass(const ZoneClass& other)
{

}

ZoneClass::~ZoneClass()
{

}

bool ZoneClass::Initialize(D3DClass* Direct3D, HWND hwnd, int screenWidth, int screenHeight, float screenDepth)
{
	m_hwnd = hwnd;

	bool result;

	m_UserInterface = new UserInterfaceClass;
	if (!m_UserInterface)
		return false;

	result = m_UserInterface->Initialize(Direct3D, screenHeight, screenWidth);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the user interface object.", L"Error", MB_OK);
		return false;
	}


	m_Camera = new CameraClass;
	if (!m_Camera)
		return false;

	//m_Camera->SetPosition(0.0f, 0.0f, -10.0f);
	m_Camera->SetPosition(0.0f, 0.0f, 0.0f);
	m_Camera->SetRotation(0.0f, 0.0f, 0.0f);
	m_Camera->Render();
	m_Camera->RenderBaseViewMatrix();


	m_Position = new PositionClass;
	if (!m_Position)
		return false;

	//m_Position->SetPosition(128.0f, 10.0f, -10.0f);
	m_Position->SetPosition(0.0f, 0.0f, 0.0f);
	m_Position->SetRotation(0.0f, 0.0f, 0.0f);

	m_displayUI = true;

	m_wireFrame = false;

	return true;
}

void ZoneClass::Shutdown()
{
	if (m_Position)
	{
		delete m_Position;
		m_Position = 0;
	}

	if (m_Camera)
	{
		delete m_Camera;
		m_Camera = 0;
	}

	if (m_UserInterface)
	{
		m_UserInterface->Shutdown();
		delete m_UserInterface;
		m_UserInterface = 0;
	}

	return;
}

bool ZoneClass::Frame(D3DClass* Direct3D, InputClass* Input, ShaderManagerClass* ShaderManager, float frameTime, int fps)
{
	bool result;
	float posX=0.0f, posY=0.0f, posZ=0.0f, rotX=0.0f, rotY=0.0f, rotZ=0.0f;

	HandleMovementInput(Input, frameTime);

	m_Position->GetPosition(posX, posY, posZ);
	m_Position->GetRotation(rotX, rotY, rotZ);

	result = m_UserInterface->Frame(Direct3D->GetDeviceContext(), fps, posX, posY, posZ, rotX, rotY, rotZ);
	if (!result)
		return false;

	result = Render(Direct3D, ShaderManager);
	if (!result)
		return false;

	return true;
}

void ZoneClass::HandleMovementInput(InputClass* Input, float frameTime)
{
	bool keyDown;
	float posX=0.0f, posY = 0.0f, posZ = 0.0f, rotX = 0.0f, rotY = 0.0f, rotZ = 0.0f;

	m_Position->setFrameTime(frameTime);

	keyDown = Input->IsLeftPressed();
	m_Position->TurnLeft(keyDown);

	keyDown = Input->IsRightPressed();
	m_Position->TurnRight(keyDown);

	keyDown = Input->IsUpPressed();
	m_Position->MoveForward(keyDown);

	keyDown = Input->IsDownPressed();
	m_Position->MoveBackward(keyDown);

	keyDown = Input->IsAPressed();
	m_Position->MoveUpward(keyDown);

	keyDown = Input->IsZPressed();
	m_Position->MoveDownward(keyDown);

	keyDown = Input->IsPgUpPressed();
	m_Position->LookUpward(keyDown);

	keyDown = Input->IsPgDownPressed();
	m_Position->LookDownward(keyDown);

	m_Position->GetPosition(posX, posY, posZ);
	m_Position->GetRotation(rotX, rotY, rotZ);

	m_Camera->SetPosition(posX, posY, posZ);
	m_Camera->SetRotation(rotX, rotY, rotZ);
	m_Camera->SetYaw(Input->GetYaw());
	m_Camera->SetPitch(Input->GetPitch());

	if (Input->IsF1Toggled())
		m_displayUI = !m_displayUI;

	if (Input->IsF2Toggled())
		m_wireFrame = !m_wireFrame;

	return;
}

bool ZoneClass::Render(D3DClass* Direct3D, ShaderManagerClass* ShaderManager)
{
	XMMATRIX viewMatrix, projectionMatrix, baseViewMatrix, orthoMatrix;
	bool result;

	m_Camera->Render();

	XMFLOAT3 temp = m_Camera->GetPosition();
	int x = temp.x;
	int y = temp.y;
	int z = temp.z;

	//Direct3D->GetWorldMatrix(worldMatrix);

	m_Camera->GetViewMatrix(viewMatrix);
	Direct3D->GetProjectionMatrix(projectionMatrix);
	m_Camera->GetBaseViewMatrix(baseViewMatrix);
	Direct3D->GetOrthoMatrix(orthoMatrix);

	//Clear the buffer to begin the scene
	Direct3D->BeginScene(0.3f, 0.4f, 0.7f, 1.0f);

	if (m_wireFrame)
		Direct3D->EnableWireframe();

	//XMFLOAT4 lightPosition = XMFLOAT4(0, 5.0f, -5.0f, 0);
	XMFLOAT4 lightPosition = XMFLOAT4(0, 10.0f, 0.0f, 0);

	bool v;

	if ((z-before) >= 1.99 )
	{
		v = ShaderManager->UpdateCubeShader(Direct3D->GetDevice(), Direct3D->GetDeviceContext(), m_hwnd,64*(before-1)/2+127-(before+1)/2);
		if (v != true)
			return false;

		worldMatrix = XMMatrixSet(1, 0, 0, 0,
			                      0, 1, 0, 0,
			                      0, 0, 1, 0,
			                      -1, 0, before+2, 1);

		worldMatrix2 = XMMatrixSet(1, 0, 0, 0,
			                       0, 1, 0, 0,
			                       0, 0, 1, 0,
			                       1, 0, before+2, 1);

		worldMatrix3 = XMMatrixSet(1, 0, 0, 0,
			                       0, 1, 0, 0,
			                       0, 0, 1, 0,
			                       -1, 0, before+4, 1);

		worldMatrix4 = XMMatrixSet(1, 0, 0, 0,
			                       0, 1, 0, 0,
			                       0, 0, 1, 0,
			                       1, 0, before+4, 1);
		result = ShaderManager->RenderCubesShader(Direct3D->GetDeviceContext(), worldMatrix, worldMatrix2, worldMatrix3, worldMatrix4,
			viewMatrix, projectionMatrix, lightPosition);
		if (!result)
			return false;

		before += 2;
	}
	else
	{
		result = ShaderManager->RenderCubesShader(Direct3D->GetDeviceContext(), worldMatrix, worldMatrix2, worldMatrix3, worldMatrix4,
			viewMatrix, projectionMatrix, lightPosition);
		if (!result)
			return false;
	}


	if (m_wireFrame)
		Direct3D->DisableWireframe();

	if (m_displayUI)
	{
		result = m_UserInterface->Render(Direct3D, ShaderManager, worldMatrix, baseViewMatrix, orthoMatrix);
		if (!result)
			return false;
	}

	Direct3D->EndScene();

	return true;
}