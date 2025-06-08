//-----------------------------------------------------------------------------
// File: CScene.cpp
//-----------------------------------------------------------------------------

#include "stdafx.h"
#include "Scene.h"

CScene::CScene()
{
}

CScene::~CScene()
{
}

//#define _WITH_TEXT_MODEL_FILE
#define _WITH_BINARY_MODEL_FILE

void CScene::BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList)
{
	m_pd3dGraphicsRootSignature = CreateGraphicsRootSignature(pd3dDevice);

#ifdef _WITH_TEXT_MODEL_FILE
	CMesh *pUfoMesh = new CMesh(pd3dDevice, pd3dCommandList, "Models/UFO.txt", true);
	CMesh *pFlyerMesh = new CMesh(pd3dDevice, pd3dCommandList, "Models/FlyerPlayership.txt", true);
#endif
#ifdef _WITH_BINARY_MODEL_FILE
	CMesh *pUfoMesh = new CMesh(pd3dDevice, pd3dCommandList, "Models/UFO.bin", false);
	CMesh *pFlyerMesh = new CMesh(pd3dDevice, pd3dCommandList, "Models/FlyerPlayership.bin", false);

	// 총알용 메쉬 초기화 - 별도의 메쉬 생성
	m_pBulletMesh = new CMesh(pd3dDevice, pd3dCommandList, "Models/UFO.bin", false);
	m_pBulletMesh->AddRef();  // 참조 카운트 증가
	
	// 셰이더 생성
	CPseudoLightingShader *pShader = new CPseudoLightingShader();
	pShader->CreateShader(pd3dDevice, m_pd3dGraphicsRootSignature);
	pShader->CreateShaderVariables(pd3dDevice, pd3dCommandList);
	
	// 총알용 셰이더도 같은 셰이더 사용
	m_pBulletShader = pShader;
	m_pBulletShader->AddRef();  // 참조 카운트 증가

	// 디버그 메시지로 초기화 확인
	if (m_pBulletMesh && m_pBulletShader)
		OutputDebugString(L"Bullet Mesh and Shader initialized successfully!\n");
	else
		OutputDebugString(L"Failed to initialize Bullet Mesh or Shader!\n");
#endif

	m_nObjects = 5;  // 다시 5개로 변경 (총알은 동적 생성)
	m_ppObjects = new CGameObject*[m_nObjects];

	m_ppObjects[0] = new CGameObject();
	m_ppObjects[0]->SetMesh(pUfoMesh);
	m_ppObjects[0]->SetShader(pShader);
	m_ppObjects[0]->SetPosition(6.0f, 0.0f, 13.0f);
	m_ppObjects[0]->SetColor(XMFLOAT3(0.7f, 0.0f, 0.0f));

	m_ppObjects[1] = new CGameObject();
	m_ppObjects[1]->SetMesh(pUfoMesh);
	m_ppObjects[1]->SetShader(pShader);
	m_ppObjects[1]->SetPosition(10.0f, -2.0f, 8.0f);
	m_ppObjects[1]->SetColor(XMFLOAT3(0.0f, 0.7f, 0.0f));

	m_ppObjects[2] = new CGameObject();
	m_ppObjects[2]->SetMesh(pUfoMesh);
	m_ppObjects[2]->SetShader(pShader);
	m_ppObjects[2]->SetPosition(-5.0f, -4.0f, 11.0f);
	m_ppObjects[2]->SetColor(XMFLOAT3(0.0f, 0.0f, 0.7f));

	m_ppObjects[3] = new CGameObject();
	m_ppObjects[3]->SetMesh(pUfoMesh);
	m_ppObjects[3]->SetShader(pShader);
	m_ppObjects[3]->SetPosition(-10.0f, -2.0f, 8.0f);

	m_ppObjects[4] = new CGameObject();
	m_ppObjects[4]->SetMesh(pFlyerMesh);
	m_ppObjects[4]->SetShader(pShader);
	m_ppObjects[4]->SetPosition(0.0f, 4.0f, 20.0f);
	m_ppObjects[4]->Rotate(0.0f, 180.0f, 0.0f);
	m_ppObjects[4]->SetColor(XMFLOAT3(0.25f, 0.75f, 0.65f));
}

ID3D12RootSignature *CScene::CreateGraphicsRootSignature(ID3D12Device *pd3dDevice)
{
	ID3D12RootSignature *pd3dGraphicsRootSignature = NULL;

	D3D12_ROOT_PARAMETER pd3dRootParameters[3];
	pd3dRootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
	pd3dRootParameters[0].Constants.Num32BitValues = 4; //Time, ElapsedTime, xCursor, yCursor
	pd3dRootParameters[0].Constants.ShaderRegister = 0; //Time
	pd3dRootParameters[0].Constants.RegisterSpace = 0;
	pd3dRootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	pd3dRootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
	pd3dRootParameters[1].Constants.Num32BitValues = 19; //16 + 3
	pd3dRootParameters[1].Constants.ShaderRegister = 1; //World
	pd3dRootParameters[1].Constants.RegisterSpace = 0;
	pd3dRootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	pd3dRootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
	pd3dRootParameters[2].Constants.Num32BitValues = 35; //16 + 16 + 3
	pd3dRootParameters[2].Constants.ShaderRegister = 2; //Camera
	pd3dRootParameters[2].Constants.RegisterSpace = 0;
	pd3dRootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	D3D12_ROOT_SIGNATURE_FLAGS d3dRootSignatureFlags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
	D3D12_ROOT_SIGNATURE_DESC d3dRootSignatureDesc;
	::ZeroMemory(&d3dRootSignatureDesc, sizeof(D3D12_ROOT_SIGNATURE_DESC));
	d3dRootSignatureDesc.NumParameters = _countof(pd3dRootParameters);
	d3dRootSignatureDesc.pParameters = pd3dRootParameters;
	d3dRootSignatureDesc.NumStaticSamplers = 0;
	d3dRootSignatureDesc.pStaticSamplers = NULL;
	d3dRootSignatureDesc.Flags = d3dRootSignatureFlags;

	ID3DBlob *pd3dSignatureBlob = NULL;
	ID3DBlob *pd3dErrorBlob = NULL;
	D3D12SerializeRootSignature(&d3dRootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &pd3dSignatureBlob, &pd3dErrorBlob);
	pd3dDevice->CreateRootSignature(0, pd3dSignatureBlob->GetBufferPointer(), pd3dSignatureBlob->GetBufferSize(), __uuidof(ID3D12RootSignature), (void **)&pd3dGraphicsRootSignature);
	if (pd3dSignatureBlob) pd3dSignatureBlob->Release();
	if (pd3dErrorBlob) pd3dErrorBlob->Release();

	return(pd3dGraphicsRootSignature);
}

void CScene::ReleaseObjects()
{
	if (m_pd3dGraphicsRootSignature) m_pd3dGraphicsRootSignature->Release();

	if (m_ppObjects)
	{
		for (int j = 0; j < m_nObjects; j++) if (m_ppObjects[j]) delete m_ppObjects[j];
		delete[] m_ppObjects;
	}

	// 총알 리소스 해제
	if (m_pBulletMesh) m_pBulletMesh->Release();
	if (m_pBulletShader) m_pBulletShader->Release();
}

void CScene::ReleaseUploadBuffers()
{
	if (m_ppObjects)
	{
		for (int j = 0; j < m_nObjects; j++) if (m_ppObjects[j]) m_ppObjects[j]->ReleaseUploadBuffers();
	}
}

bool CScene::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	return(false);
}

bool CScene::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{

	switch (nMessageID)
	{
	case WM_KEYDOWN:
		break;
	default:
		break;
	}
	return(false);
}

bool CScene::ProcessInput()
{
	return(false);
}

void CScene::AnimateObjects(float fTimeElapsed)
{
	for (int j = 0; j < m_nObjects; j++)
	{
		m_ppObjects[j]->Animate(fTimeElapsed);

		// CBulletObject인 경우 거리 체크
		CBulletObject* pBullet = dynamic_cast<CBulletObject*>(m_ppObjects[j]);
		if (pBullet && pBullet->IsOutOfRange())
		{
			// j번째 총알을 제거하고 배열 정리
			delete m_ppObjects[j];
			
			// 마지막 객체를 현재 위치로 이동
			m_ppObjects[j] = m_ppObjects[m_nObjects - 1];
			m_ppObjects[m_nObjects - 1] = nullptr;
			m_nObjects--;
			
			// 인덱스 조정 (같은 위치를 다시 검사하기 위해)
			j--;
		
		}
	}
}

void CScene::PrepareRender(ID3D12GraphicsCommandList* pd3dCommandList)
{
	pd3dCommandList->SetGraphicsRootSignature(m_pd3dGraphicsRootSignature);
}

void CScene::Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera)
{
	pCamera->SetViewportsAndScissorRects(pd3dCommandList);
	pCamera->UpdateShaderVariables(pd3dCommandList);

	for (int j = 0; j < m_nObjects; j++)
	{
		if (m_ppObjects[j]) m_ppObjects[j]->Render(pd3dCommandList, pCamera);
	}
}

void CScene::CreateBullet()
{
	// 디버깅 메시지 출력
	WCHAR szDebugString[256];
	wsprintf(szDebugString, L"CreateBullet Function Called! Now Objects: %d\n", m_nObjects);
	OutputDebugString(szDebugString);

	// 1. 새로운 크기의 오브젝트 배열 생성
	CGameObject** ppNewObjects = new CGameObject*[m_nObjects + 1];

	// 2. 기존 오브젝트들을 새 배열로 복사
	for (int i = 0; i < m_nObjects; i++)
	{
		ppNewObjects[i] = m_ppObjects[i];
	}

	// 3. 새로운 총알 오브젝트 생성
	CBulletObject* pBulletObject = new CBulletObject();
	
	// 4. 플레이어의 위치와 방향을 가져와서 총알 위치 설정
	XMFLOAT3 xmf3PlayerPosition;
	XMFLOAT3 xmf3PlayerLook = m_pPlayer->GetLookVector();
	if (m_pPlayer)
	{
		xmf3PlayerPosition = m_pPlayer->GetPosition();
		
		
		// 플레이어 앞쪽으로 총알 생성 (Look 벡터 방향으로 3.0f 만큼)
		xmf3PlayerPosition.x += xmf3PlayerLook.x * 3.0f;
		xmf3PlayerPosition.y += xmf3PlayerLook.y * 3.0f -0.5f;
		xmf3PlayerPosition.z += xmf3PlayerLook.z * 3.0f;

	}
	else
	{
		OutputDebugString(L"No Player Reference!\n");
		xmf3PlayerPosition = XMFLOAT3(0.0f, 0.0f, 0.0f);
	}
	
	if (m_pBulletMesh && m_pBulletShader)  // 메쉬와 셰이더가 유효한지 확인
	{
		pBulletObject->SetMesh(m_pBulletMesh);
		pBulletObject->SetShader(m_pBulletShader);
		pBulletObject->SetPosition(xmf3PlayerPosition);
		pBulletObject->SetScale(0.1f, 0.4f, 0.1f);  // 크기를 절반으로
		pBulletObject->SetColor(XMFLOAT3(1.0f, 1.0f, 0.0f));  // 노란색
		
		// Look 벡터를 총알의 속도로 설정
		pBulletObject->SetVelocity(xmf3PlayerLook);
		
		// 5. 새로운 총알을 배열의 마지막에 추가
		ppNewObjects[m_nObjects] = pBulletObject;
		
		// 6. 기존 배열 삭제하고 새 배열로 교체
		if (m_ppObjects) delete[] m_ppObjects;
		m_ppObjects = ppNewObjects;
		m_nObjects++;
		
		OutputDebugString(L"Bullet Created!\n");
	}
	else
	{
		OutputDebugString(L"No Mesh or Shader!\n");
		delete pBulletObject;  // 실패하면 메모리 해제
		delete[] ppNewObjects;
	}
}

