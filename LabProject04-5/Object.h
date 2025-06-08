//------------------------------------------------------- ----------------------
// File: Object.h
//-----------------------------------------------------------------------------

#pragma once

#include "Mesh.h"
#include "Camera.h"

class CShader;

class CGameObject
{
public:
	CGameObject();
	virtual ~CGameObject();

public:
	XMFLOAT4X4						m_xmf4x4World;
	CMesh							*m_pMesh = NULL;

	CShader							*m_pShader = NULL;

	XMFLOAT3						m_xmf3Color = XMFLOAT3(1.0f, 1.0f, 1.0f);

	void SetMesh(CMesh *pMesh);
	void SetShader(CShader *pShader);

	CMesh* GetMesh() { return m_pMesh; }
	CShader* GetShader() { return m_pShader; }

	virtual void CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void ReleaseShaderVariables();

	virtual void Animate(float fTimeElapsed);
	virtual void OnPrepareRender() { }
	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera = NULL);

	virtual void ReleaseUploadBuffers();

	XMFLOAT3 GetPosition();
	XMFLOAT3 GetLook();
	XMFLOAT3 GetUp();
	XMFLOAT3 GetRight();

	void SetPosition(float x, float y, float z);
	void SetPosition(XMFLOAT3 xmf3Position);

	void SetColor(XMFLOAT3 xmf3Color) { m_xmf3Color = xmf3Color; }

	void MoveStrafe(float fDistance = 1.0f);
	void MoveUp(float fDistance = 1.0f);
	void MoveForward(float fDistance = 1.0f);

	void Rotate(float fPitch = 10.0f, float fYaw = 10.0f, float fRoll = 10.0f);
	void Rotate(XMFLOAT3 *pxmf3Axis, float fAngle);

	// 크기 조절을 위한 함수 추가
	void SetScale(float x, float y, float z);
};

class CUfoObject : public CGameObject
{
public:
	CUfoObject();
	virtual ~CUfoObject();
};

class CBulletObject : public CGameObject
{
public:
	CBulletObject();
	virtual ~CBulletObject();

	virtual void Animate(float fTimeElapsed);
	
	void SetVelocity(XMFLOAT3 xmf3Velocity) { m_xmf3Velocity = xmf3Velocity; }
	XMFLOAT3 GetVelocity() const { return m_xmf3Velocity; }

	// 총알이 제거되어야 하는지 확인
	bool IsOutOfRange() const { return m_fTravelDistance >= m_fMaxTravelDistance; }
	
	// 최대 이동 거리 설정
	void SetMaxTravelDistance(float fMaxDistance) { m_fMaxTravelDistance = fMaxDistance; }

protected:
	XMFLOAT3 m_xmf3Velocity = XMFLOAT3(0.0f, 0.0f, 0.0f);  // 총알의 속도 벡터
	float m_fTravelDistance = 0.0f;  // 총알이 이동한 총 거리
	float m_fMaxTravelDistance = 100.0f;  // 최대 이동 거리 (기본값 100.0f)
};

