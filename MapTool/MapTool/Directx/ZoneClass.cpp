#include "ZoneClass.h"

ZoneClass::ZoneClass()
{
	m_UserInterface = NULL;
	m_Camera = NULL;
	m_Light = NULL;
	m_Frustum = NULL;
	m_SkyDome = NULL;
	m_Terrain = NULL;
	m_Pick = NULL;
	m_Brush = NULL;
}

ZoneClass::ZoneClass(const ZoneClass &other)
{
}

ZoneClass::~ZoneClass()
{
}

bool ZoneClass::Initialize(D3DClass *direct3D, 
	HWND hwnd, 
	int screenWidth, 
	int screenHeight, 
	float screenDepth,
	const MyStruct::TERRAIN_DESC &terrainDesc)
{
	bool result;

	m_IsDisplayUI = true;
	m_IsWireFrame = false;
	m_IsPlay = false;
	m_IsCellLines = true;
	m_IsHeightLocked = false;

	m_UserInterface = new UserInterfaceClass;
	if (!m_UserInterface)
		return false;

	result = m_UserInterface->Initialize(direct3D, screenHeight, screenWidth);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the user interface object.", L"Error", MB_OK);
		return false;
	}

	m_Camera = new CameraClass;
	if (!m_Camera)
		return false;

	m_Camera->SetPosition(0.0f, 0.0f, -10.0f);
	m_Camera->Render();
	m_Camera->RenderBaseViewMatrix();

	m_Light = new LightClass;
	if (!m_Light)
		return false;

	m_Light->SetDiffuseColor(1.0f, 1.0f, 1.0f, 1.0f);
	m_Light->SetDirection(0.5f, -1.0f, -0.5f);

	m_Frustum = new FrustumClass;
	if (!m_Frustum)
		return false;

	m_Frustum->Initialize(screenDepth);

	m_SkyDome = new SkyDomeClass;
	if (!m_SkyDome)
		return false;

	result = m_SkyDome->Initialize(direct3D->GetDevice());
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the sky dome object.", L"Error", MB_OK);
		return false;
	}

	m_Terrain = new TerrainClass;
	if (!m_Terrain)
		return false;

	result = m_Terrain->Initialize(direct3D->GetDevice(), terrainDesc);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the terrain object.", L"Error", MB_OK);
		return false;
	}

	m_Pick = new PickingToolClass;
	if (!m_Pick)
		return false;

	m_Brush = new BrushClass;
	if (!m_Brush)
		return false;

	result = m_Brush->Initialize(direct3D->GetDevice(), XMFLOAT4(255, 0, 0, 0));
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the brush object.", L"Error", MB_OK);
		return false;
	}
	
	m_Camera->SetPosition(0.0f, 5.0f, 0.0f);
	m_Camera->SetRotation(36.0f, 44.0f, 0.0f);

	return true;
}

void ZoneClass::Shutdown()
{
	if (m_Terrain)
	{
		m_Terrain->Shutdown();
		delete m_Terrain;
		m_Terrain = NULL;
	}
	if (m_SkyDome)
	{
		m_SkyDome->Shutdown();
		delete m_SkyDome;
		m_SkyDome = NULL;
	}
	if (m_Frustum)
	{
		delete m_Frustum;
		m_Frustum = NULL;
	}
	if (m_Light)
	{
		delete m_Light;
		m_Light = NULL;
	}
	if (m_Camera)
	{
		m_Camera->Shutdown();
		delete m_Camera;
		m_Camera = NULL;
	}
	if (m_UserInterface)
	{
		m_UserInterface->Shutdown();
		delete m_UserInterface;
		m_UserInterface = NULL;
	}
	if (m_Pick)
	{
		delete m_Pick;
		m_Pick = NULL;
	}
	if (m_Brush)
	{
		m_Brush->Shutdown();
		delete m_Brush;
		m_Brush = NULL;
	}
}

bool ZoneClass::Frame(D3DClass *direct3D, 
	InputClass *input,
	ShaderManagerClass *shaderManager, 
	TextureManagerClass *textureManager,
	float frameTime,
	int fps,
	const MyStruct::TERRAIN_DESC &terrainDesc)
{
	bool result, foundHeight;
	float posX, posY, posZ, rotX, rotY, rotZ, height;
	int mousePosX, mousePosY;

	HandleMovementInput(direct3D, input, mousePosX, mousePosY);

	m_Camera->SetFrameTime(frameTime);
	m_Camera->Frame(input);
	m_Camera->GetPosition(posX, posY, posZ);
	m_Camera->GetRotation(rotX, rotY, rotZ);
	
	if(m_IsPlay)
		PushedF3Button(frameTime);
	
	result = m_UserInterface->Frame(direct3D->GetDeviceContext(),
		fps,
		posX,
		posY,
		posZ,
		rotX,
		rotY,
		rotZ,
		m_SkyDome->GetApexColor(),
		m_SkyDome->GetCenterColor(),
		mousePosX,
		mousePosY);
	if (!result)
		return false;

	m_Terrain->Frame();
	if (m_IsHeightLocked)
	{
		foundHeight = m_Terrain->GetHeightAtPosition(posX, posZ, height);
		if (foundHeight)
			m_Camera->SetPosition(posX, height + 1.0f, posZ);
	}

	result = Render(direct3D, shaderManager, textureManager, terrainDesc);
	if (!result)
		return false;

	return true;
}

void ZoneClass::HandleMovementInput(D3DClass *direct3D, InputClass *input, int &mousePosX, int &mousePosY)
{
	bool result;

	result = input->GetMouseWindowPosition(mousePosX, mousePosY);
	if (result)
	{
		if (input->IsMouseLeftClick())
		{
			m_Pick->InitPick(direct3D,
				m_Camera,
				DIRECT_WND_WIDTH,
				DIRECT_WND_HEIGHT,
				mousePosX,
				mousePosY);

			m_Pick->Picking(direct3D, m_Terrain, m_PickPos);
			XMFLOAT4 pos;
			XMStoreFloat4(&pos, m_PickPos);
			cout << "X: " << pos.x << endl << "Y: " << pos.y << endl << "Z: " << pos.z << endl << endl;
		}
	}
		
	if (input->IsF1Toggled())
		m_IsDisplayUI = !m_IsDisplayUI;

	if (input->IsF2Toggled())
		m_IsWireFrame = !m_IsWireFrame;

	if (input->IsF3Toggled())
		m_IsPlay = !m_IsPlay;

	if (input->IsF4Toggled())
		m_IsCellLines = !m_IsCellLines;

	if (input->IsF5Toggled())
		m_IsHeightLocked = !m_IsHeightLocked;
}

bool ZoneClass::Render(D3DClass *direct3D, 
	ShaderManagerClass *shaderManager,
	TextureManagerClass *textureManager,
	const MyStruct::TERRAIN_DESC &terrainDesc)
{
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix, baseViewMatrix, orthoMatrix;
	bool result;
	XMFLOAT3 cameraPosition;
	int i;

	m_Camera->Render();

	m_Camera->GetViewMatrix(viewMatrix);
	direct3D->GetProjectionMatrix(projectionMatrix);
	m_Camera->GetBaseViewMatrix(baseViewMatrix);
	direct3D->GetOrthoMatrix(orthoMatrix);

	m_Camera->GetPosition(cameraPosition.x, cameraPosition.y, cameraPosition.z);

	m_Frustum->ConstructFrustum(projectionMatrix, viewMatrix);

	direct3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

	direct3D->TurnOffCulling();
	direct3D->TurnZBufferOff();

	worldMatrix = XMMatrixTranslation(cameraPosition.x, cameraPosition.y, cameraPosition.z);

	m_SkyDome->Render(direct3D->GetDeviceContext());
	
	result = shaderManager->RenderSkyDomeShader(direct3D->GetDeviceContext(),
		m_SkyDome->GetIndexCount(),
		worldMatrix,
		viewMatrix,
		projectionMatrix,
		m_SkyDome->GetApexColor(),
		m_SkyDome->GetCenterColor());

	if (!result)
		return false;

	direct3D->GetWorldMatrix(worldMatrix);
	
	direct3D->TurnZBufferOn();
	direct3D->TurnOnCulling();

	if (m_IsWireFrame)
		direct3D->EnableWireframe();

	for (i = 0; i < m_Terrain->GetCellCount(); ++i)
	{
		result = m_Terrain->RenderCell(direct3D->GetDeviceContext(), i, m_Frustum);
		if (result)
		{			
			result = shaderManager->RenderLightShader(direct3D->GetDeviceContext(),
				m_Terrain->GetCellIndexCount(i),
				worldMatrix,
				viewMatrix,
				projectionMatrix,
				textureManager->GetTexture(terrainDesc.textureCurSel),
				m_Light->GetDirection(),
				m_Light->GetDiffuseColor());
			if (!result)
				return false;

			if (m_IsCellLines)
			{
				m_Terrain->RenderCellLines(direct3D->GetDeviceContext(), i);
				result = shaderManager->RenderColorShader(direct3D->GetDeviceContext(),
					m_Terrain->GetCellLinesIndexCount(i),
					worldMatrix,
					viewMatrix,
					projectionMatrix);
				if (!result)
					return false;
			}
		}
	}

	result = m_Brush->Render(direct3D->GetDeviceContext());
	if (!result)
		return false;

	result = shaderManager->RenderColorShader(direct3D->GetDeviceContext(),
		m_Brush->GetIndexCount(),
		worldMatrix,
		viewMatrix,
		projectionMatrix);
	if (!result)
		return false;

	if (m_IsWireFrame)
		direct3D->DisableWireframe();

	result = m_UserInterface->UpdateRenderCounts(direct3D->GetDeviceContext(),
		m_Terrain->GetRenderCount(),
		m_Terrain->GetCellsDrawn(),
		m_Terrain->GetCellsCulled());
	if (!result)
		return false;

	if (m_IsDisplayUI)
	{
		result = m_UserInterface->Render(direct3D,
			shaderManager,
			worldMatrix,
			baseViewMatrix,
			orthoMatrix);
		if (!result)
			return false;
	}

	direct3D->EndScene();

	return true;
}

// 0 - 257 : 129
void ZoneClass::PushedF3Button(float frameTime)
{
	XMFLOAT3 dir;
	
	dir = m_Light->GetDirection();
	
	if (dir.x >= 360) dir.x -= 360;
	if (dir.y >= 360) dir.y -= 360;
	if (dir.z >= 360) dir.z -= 360;
	
	dir.z -= frameTime / 5;
	
	m_Light->SetDirection(dir.x, dir.y, dir.z);

	//XMFLOAT4 tempColor;
	//
	//tempColor = m_SkyDome->GetApexColor();
	//
	//tempColor.x -= frameTime / 5; // R
	//tempColor.y -= frameTime / 5; // G
	//tempColor.z -= frameTime / 5; // B
	//
	//m_SkyDome->SetApexColor(tempColor);

	return;
}
