#pragma once

#include "Mesh.h"

#include "Camera.h"

#define DIR_FORWARD 0x01
#define DIR_BACKWARD 0x02
#define DIR_LEFT 0x04
#define DIR_RIGHT 0x08
#define DIR_UP 0x10
#define DIR_DOWN 0x20

class CGameObject
{
public:
	CGameObject();
	virtual ~CGameObject();

private:
	int m_nReferences;
public:
	void AddRef ();
	void Release ();

	D3DXMATRIX m_d3dxmtxWorld;
	CMesh *m_pMesh;

	virtual void SetMesh (CMesh *pMesh);
	virtual void Animate (float fTimeElapsed);
	virtual void Render (ID3D11DeviceContext *pd3dDeviceContext, CCamera *pCamera = NULL);

	virtual void SetPosition (float x, float y, float z);
	virtual void SetPosition (D3DXVECTOR3 d3dxvPosition);

	virtual void MoveStrafe (float fDistance = 1.0f);
	virtual void MoveUp (float fDistance = 1.0f);
	virtual void MoveForward (float fDistance = 1.0f);

	virtual void Rotate (float fPitch = 10.0f, float fYaw = 10.0f, float fRoll = 10.0f);
	virtual void Rotate (D3DXVECTOR3 *pd3dxvAxis, float fAngle);

	D3DXVECTOR3 GetPosition();
	D3DXVECTOR3 GetLookAt();
	D3DXVECTOR3 GetUp();
	D3DXVECTOR3 GetRight();

};

class CRotatingObject : public CGameObject
{
public:
	CRotatingObject();
	virtual ~CRotatingObject();

	virtual void Animate (float fTimeElapsed);
	virtual void Render (ID3D11DeviceContext *pd3dDeviceContext, CCamera *pCamera = NULL);

	// 자전 속도와 회전축 벡터를 설정하는 함수
	void SetRotationSpeed (float fRotationSpeed) {m_fRotationSpeed = fRotationSpeed;}
	void SetRotationAxis (D3DXVECTOR3 d3dxvRotationAxis) {m_d3dxvRotationAxis = d3dxvRotationAxis;}

private:
	// 자전 속도와 회전축 벡터를 나타내는 멤버변수
	float m_fRotationSpeed;
	D3DXVECTOR3 m_d3dxvRotationAxis;
};