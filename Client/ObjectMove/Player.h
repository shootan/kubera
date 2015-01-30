#pragma once

#include "Timer.h"
#include "Camera.h"
#include "Object.h"
#include "Shader.h"

class CPlayer : public CGameObject
{
protected:
	CCamera *m_pCamera;

	// �÷��̾��� ��ġ ����, x,y,z �� �����̴�
	D3DXVECTOR3 m_d3dxvPosition;
	D3DXVECTOR3 m_d3dxvRight;
	D3DXVECTOR3 m_d3dxvUp;
	D3DXVECTOR3 m_d3dxvLook;

	// �÷��̾ ���� x,y,z ������ �󸶸�ŭ ȸ���ߴ°��� ��Ÿ����.
	float m_fPitch, m_fYaw, m_fRoll;

	// �÷��̾��� �̵��ӵ��� ��Ÿ���� �����̴�
	D3DXVECTOR3 m_d3dxvVelocity;

	// �÷��̾ �ۿ��ϴ� �߷��� ��Ÿ���� �����̴�
	D3DXVECTOR3 m_d3dxvGravity;

	// xy��鿡�� �������ӵ��� �÷��̾��� �̵��ӵ��� �ִ밪�� ��Ÿ����
	float m_fMaxVelocityXZ;

	// y�� �������� �������ӵ��� �÷��̾��� �̵��ӵ��� �ִ밪�� ��Ÿ����.
	float m_fMaxVelocityY;

	// �÷��̾ �ۿ��ϴ� �������� ��Ÿ����
	float m_fFriction;

	// �÷��̾��� ��ġ�� �ٲ𶧸��� ȣ��Ǵ� OnPlayerUpdated() �Լ����� ����ϴ� �������̴�
	LPVOID m_pPlayerUpdatedContext;

	// ī�޶��� ��ġ�� �ٲ𶧸��� ȣ��Ǵ� OnCameraUpdated() �Լ����� ����ϴ� �������̴�
	LPVOID m_pCameraUpdatedContext;

	// �÷��̾��� ���� ī�Ŷ��̴�
//	CCamera *m_pCamera;

	// 3��Ī ī�޶��϶� �÷��̾ �׸��� ���ؼ� ����ϴ� ���̴��̴�
	CPlayerShader *m_pShader;

public:
	CPlayer();
	~CPlayer();

	D3DXVECTOR3 GetPosition() {return m_d3dxvPosition;}
	D3DXVECTOR3 GetLookVector() {return m_d3dxvLook;}
	D3DXVECTOR3 GetUpVector() {return m_d3dxvUp;}
	D3DXVECTOR3 GetRightVector() {return m_d3dxvRight;}

	void SetFriction (float fFriction) {m_fFriction = fFriction;}
	void SetGravity (const D3DXVECTOR3& d3dxvGravity) {m_d3dxvGravity = d3dxvGravity;}
	void SetMaxVelocityXZ (float fMaxVelocity) {m_fMaxVelocityXZ = fMaxVelocity;}
	void SetMaxVelocityY (float fMaxVelocity) {m_fMaxVelocityY = fMaxVelocity;}
	void SetVelocity (const D3DXVECTOR3& d3dxvVelocity) {m_d3dxvVelocity = d3dxvVelocity;}

	void SetPosition (const D3DXVECTOR3& d3dxvPosition) { Move ((d3dxvPosition - m_d3dxvPosition), false);}

	const D3DXVECTOR3& GetVelocity() const {return (m_d3dxvVelocity);}
	float GetYaw() const {return m_fYaw;}
	float GetPitch() const {return m_fPitch;}
	float GetRoll() const {return m_fRoll;}

	CCamera *GetCamera() {return m_pCamera;}
	void SetCamera (CCamera *pCamera) {m_pCamera = pCamera;}

	// �÷��̾ �̵��ϴ� �Լ��̴�
	void Move (ULONG nDirection, float fDistance, bool bVelocity = false);
	void Move (const D3DXVECTOR3& d3dxvShift, bool bVelocity = false);
	void Move (float fxOffset = 0.0f, float fyOffset = 0.0f, float fzOffset = 0.0f);

	// �÷��̾��� ��ġ�� ȸ�� ������ ����ð��� ���� �����ϴ� �Լ��̴�
	void Update (float fTimeElapsed);

	// �÷��̾��� ��ġ�� ȸ�������κ��� ���庯ȯ����� �����ϴ� �Լ��̴�
	void RegenerateWorldMatrix ();

	// �÷��̾��� ��ġ�� �ٲ𶧸��� ȣ��Ǵ� �Լ��� �� �Լ����� ����ϴ� ������ �����ϴ� �Լ�
	virtual void OnplayerUpdated (float fTimeElapsed);
	void SetPlayerUpdateContext (LPVOID pContext) {m_pPlayerUpdatedContext = pContext;}

	// ī�޶��� ��ġ�� �ٲ𶧸��� ȣ��Ǵ� �Լ��� �� �Լ����� ����ϴ� ������ �����ϴ� �Լ�
	virtual void OnCameraUpdated (float fTimeElapsed);
	void SetCameraUpdatedContext (LPVOID pContext) {m_pCameraUpdatedContext = pContext;}

	// �÷��̾��� ��� ���۸� �����ϰ� �����ϴ� ����Լ��� �����Ѵ�
	void CreateShaderVariables (ID3D11Device *pd3dDevice);
	void UpdateShaderVariables (ID3D11DeviceContext *pd3dDeviceContext);

	// �÷��̾��� ���� ī�޶� �����ϰ� ��ȯ�ϴ� ����Լ��� �����Ѵ�.
	//void SetCamera (CCamera *pCamera) {m_pCamera = pCamera;}
	//CCamera *GetCamera() {return (m_pCamera);}

	// ī�޸��� ������ �� ȣ��Ǵ� �Լ��̴�
	CCamera *OnChangeCamera (ID3D11Device *pd3dDevice, DWORD nNewCameraMode, DWORD nCurrentCameraMode);
	virtual void ChangeCamera (ID3D11Device *pd3dDevice, DWORD nNewCameraMode, float fTimeElapsed) {}

	// �÷��̾��� ī�޶� 3��Ī ī�޶� �ɶ� �÷��̾��� �޽��� ������ �Ѵ�
	virtual void Render (ID3D11DeviceContext *pd3dDeviceContext);
	
	void Rotate (float x, float y, float z);
};

class CAirplanePlayer  : public CPlayer
{
public:
	CAirplanePlayer (ID3D11Device *pd3dDevice);
	~CAirplanePlayer();

	virtual void ChangeCamera(ID3D11Device *pd3dDevice, DWORD nNewCameraMode, float fTimeElapsed);
	virtual void Render (ID3D11DeviceContext *pd3dDeviceContext);
};