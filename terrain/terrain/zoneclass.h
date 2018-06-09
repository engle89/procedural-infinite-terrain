#ifndef _ZONECLASS_H_
#define _ZONECLASS_H_

#include "d3dclass.h"
#include "inputclass.h"
#include "shadermanagerclass.h"
#include "timerclass.h"
#include "userinterfaceclass.h"
#include "cameraclass.h"
#include "positionclass.h"
#include "terrainclass.h"

class ZoneClass
{
public:
	ZoneClass();
	ZoneClass(const ZoneClass&);
	~ZoneClass();

	bool Initialize(D3DClass*, HWND, int, int, float);
	void Shutdown();
	bool Frame(D3DClass*, InputClass*, ShaderManagerClass*, float, int);

private:
	void HandleMovementInput(InputClass*, float);
	bool Render(D3DClass*, ShaderManagerClass*);

private:
	UserInterfaceClass * m_UserInterface;
	CameraClass* m_Camera;
	PositionClass* m_Position;
	TerrainClass* m_Terrain;
	bool m_displayUI;
	bool m_wireFrame;
	HWND m_hwnd;
	XMMATRIX worldMatrix = XMMatrixSet(1, 0, 0, 0,
		                               0, 1, 0, 0,
		                               0, 0, 1, 0,
		                               -1, 0, 1, 1);

	XMMATRIX worldMatrix2 = XMMatrixSet(1, 0, 0, 0,
		                                0, 1, 0, 0,
		                                0, 0, 1, 0,
		                                1, 0, 1, 1);

	XMMATRIX worldMatrix3 = XMMatrixSet(1, 0, 0, 0,
		                                0, 1, 0, 0,
		                                0, 0, 1, 0,
		                                -1, 0, 3, 1);

	XMMATRIX worldMatrix4 = XMMatrixSet(1, 0, 0, 0,
		                                0, 1, 0, 0,
		                                0, 0, 1, 0,
		                                1, 0, 3, 1);
	bool update = false;
	int before = 1;
};

#endif
