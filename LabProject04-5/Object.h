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
	bool							m_bSplitUFO = false;  // 분열된 UFO인지 표시하는 플래그

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
	XMFLOAT3 GetColor() { return m_xmf3Color; }

	void SetSplitUFO(bool bSplit) { m_bSplitUFO = bSplit; }
	bool IsSplitUFO() const { return m_bSplitUFO; }

	void MoveStrafe(float fDistance = 1.0f);
	void MoveUp(float fDistance = 1.0f);
	void MoveForward(float fDistance = 1.0f);

	void Rotate(float fPitch = 10.0f, float fYaw = 10.0f, float fRoll = 10.0f);
	void Rotate(XMFLOAT3 *pxmf3Axis, float fAngle);

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

	bool IsOutOfRange() const { return m_fTravelDistance >= m_fMaxTravelDistance; }
	
	void SetMaxTravelDistance(float fMaxDistance) { m_fMaxTravelDistance = fMaxDistance; }

protected:
	XMFLOAT3 m_xmf3Velocity = XMFLOAT3(0.0f, 0.0f, 0.0f);
	float m_fTravelDistance = 0.0f;
	float m_fMaxTravelDistance = 100.0f;
};

class CSplitedUFO : public CGameObject
{
public:
	CSplitedUFO();
	virtual ~CSplitedUFO();

	virtual void Animate(float fTimeElapsed);
	
	void SetVelocity(XMFLOAT3 xmf3Velocity) { m_xmf3Velocity = xmf3Velocity; }
	XMFLOAT3 GetVelocity() const { return m_xmf3Velocity; }
	
	bool IsExpired() const { return (m_fLifeTime >= m_fMaxLifeTime); }

protected:
	XMFLOAT3 m_xmf3Velocity = XMFLOAT3(0.0f, 0.0f, 0.0f);
	float m_fLifeTime = 0.0f;  // 생성 후 경과 시간
	const float m_fMaxLifeTime = 6.0f;  // 최대 수명 (6초)
};

