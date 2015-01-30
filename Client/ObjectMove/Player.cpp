#include "StdAfx.h"
#include "Player.h"


CPlayer::CPlayer(void)
{
	m_pCamera = NULL;

	m_d3dxvPosition = D3DXVECTOR3 (0.0f, 0.0f, 0.0f);
	m_d3dxvRight = D3DXVECTOR3 (1.0f, 0.0f, 0.0f);
	m_d3dxvUp = D3DXVECTOR3 (0.0f, 1.0f, 0.0f);
	m_d3dxvLook = D3DXVECTOR3 (0.0f, 0.0f, 1.0f);

	m_d3dxvVelocity = D3DXVECTOR3 (0.0f, 0.0f, 0.0f);
	m_d3dxvGravity = D3DXVECTOR3 (0.0f, 0.0f, 0.0f);
	m_fMaxVelocityXZ = 0.0f;
	m_fMaxVelocityY = 0.0f;
	m_fFriction = 0.0f;

	m_fPitch = 0.0f;
	m_fRoll = 0.0f;
	m_fYaw = 0.0f;
	
	m_pPlayerUpdatedContext = NULL;
	m_pCameraUpdatedContext = NULL;

	m_pShader = NULL;
}


CPlayer::~CPlayer(void)
{
	if (m_pCamera) delete m_pCamera;
}

void CPlayer::CreateShaderVariables(ID3D11Device *pd3dDevice)
{

}

void CPlayer::UpdateShaderVariables(ID3D11DeviceContext *pd3dDeviceContext)
{
	// 플레이어와 현재 카메라의 UpdateShaderVariavbles() 멤버 함수를 호출한다.
	if (m_pCamera) m_pCamera->UpdateShaderVariables(pd3dDeviceContext);
}

void CPlayer::RegenerateWorldMatrix()
{
	m_d3dxmtxWorld._11 = m_d3dxvRight.x; 
	m_d3dxmtxWorld._12 = m_d3dxvRight.y; 
	m_d3dxmtxWorld._13 = m_d3dxvRight.z; 
	m_d3dxmtxWorld._21 = m_d3dxvUp.x; 
	m_d3dxmtxWorld._22 = m_d3dxvUp.y; 
	m_d3dxmtxWorld._23 = m_d3dxvUp.z; 
	m_d3dxmtxWorld._31 = m_d3dxvLook.x; 
	m_d3dxmtxWorld._32 = m_d3dxvLook.y; 
	m_d3dxmtxWorld._33 = m_d3dxvLook.z;
	m_d3dxmtxWorld._41 = m_d3dxvPosition.x; 
	m_d3dxmtxWorld._42 = m_d3dxvPosition.y; 
	m_d3dxmtxWorld._43 = m_d3dxvPosition.z;
}

void CPlayer::Move (DWORD dwDirection, float fDistance, bool bUpdateVelocity)
{
	if (dwDirection)
	{
		D3DXVECTOR3 d3dxvShift = D3DXVECTOR3 (0, 0, 0);

		// 화살표키 ^ 를 누르면 z축 방향으로 이동 반대를 누르면 반대로 이동
		if (dwDirection & DIR_FORWARD) d3dxvShift += m_d3dxvUp * fDistance;
		if (dwDirection & DIR_BACKWARD) d3dxvShift -= m_d3dxvUp * fDistance;

		// 화살표 -> , <- 를 누르면 x축 방향으로 이동
		if (dwDirection & DIR_RIGHT) d3dxvShift += m_d3dxvRight * fDistance;
		if (dwDirection & DIR_LEFT) d3dxvShift -= m_d3dxvRight * fDistance;

		//if (dwDirection & DIR_UP) d3dxvShift += m_d3dxvUp * fDistance;
		//if (dwDirection & DIR_DOWN) d3dxvShift -= m_d3dxvUp * fDistance;

		// 플레이어의 위치를 현재 위치벡터에서 d3dxvShift 벡터만큼 이동한다
		Move (d3dxvShift, bUpdateVelocity);
	}
}

void CPlayer::Move(const D3DXVECTOR3& d3dxvShift, bool bUpdateVelocity)
{
	// bUpdateVelocity가 참이면 플레이어를 이동하지 않고 속도 벡터를 변경한다
	if (bUpdateVelocity)
	{
		m_d3dxvVelocity += d3dxvShift;
	}
	else
	{
		// 플레이어를 현재 위치 벡터에서 d3dxvShift만큼 이동한다
		D3DXVECTOR3 d3dxvPosition = m_d3dxvPosition + d3dxvShift;
		m_d3dxvPosition = d3dxvPosition;
		RegenerateWorldMatrix();

		// 플레이어의 위치가 변경되었으므로 카메라의 위치도 d3dxvShift 벡터 만킁 이동한다
		m_pCamera->Move(d3dxvShift);
	}
}

void CPlayer :: Rotate (float x, float y, float z)
{
	D3DXMATRIX mtxRotate;
	DWORD nCurrentCameraMode = m_pCamera->GetMode();

	// 1인칭 카메라 또는 3인칭 카메라의 경우 플레이어의 회전은 약간의 제약이 따른다.
	if ((nCurrentCameraMode == FIRST_PERSON_CAMERA) || (nCurrentCameraMode == THIRD_PERSON_CAMERA))
	{
		if (x != 0.0f)
		{
			m_fPitch += x;
			if (m_fPitch > +89.0f) {x -= (m_fPitch - 89.0f); m_fPitch = +89.0f;}
			if (m_fPitch < -89.0f) {x -= (m_fPitch + 89.0f); m_fPitch = -89.0f;}
		}
		if (y != 0.0f)
		{
			m_fYaw += y;
			if (m_fYaw > 360.0f) m_fYaw -= 360.0f;
			if (m_fYaw < 0.0f) m_fYaw += 360.0f;
		}
		if (z != 0.0f)
		{
			m_fRoll = z;
			if (m_fRoll > +20.0f) {z -= (m_fRoll - 20.0f); m_fRoll = +20.0f;}
			if (m_fRoll < -20.0f) {z -= (m_fRoll + 20.0f); m_fRoll = -20.0f;}
		}

		m_pCamera->Rotate(x, y, z);

		if (y != 0.0f)
		{
			D3DXMatrixRotationAxis (&mtxRotate, &m_d3dxvUp, (float) D3DXToRadian(y));
			D3DXVec3TransformNormal(&m_d3dxvLook, &m_d3dxvLook, &mtxRotate);
			D3DXVec3TransformNormal(&m_d3dxvRight, &m_d3dxvRight, &mtxRotate);
		}
	}

	else if (nCurrentCameraMode == SPACESHIP_CAMERA)
	{
		// 스페이스쉽 카메라에서 플레이어의 회전은 회전각도의 제한이 없다. 그리고 모든 축을 중심으로 회전을 할수 있다
		m_pCamera->Rotate (x, y, z);
		if (x != 0.0f)
		{
			D3DXMatrixRotationAxis(&mtxRotate, &m_d3dxvRight, (float) D3DXToRadian(x));
			D3DXVec3TransformNormal(&m_d3dxvLook, &m_d3dxvLook, &mtxRotate);
			D3DXVec3TransformNormal(&m_d3dxvUp, &m_d3dxvUp, &mtxRotate);
		}
		if (y != 0.0f)
		{
			D3DXMatrixRotationAxis(&mtxRotate, &m_d3dxvUp, (float) D3DXToRadian(y));
			D3DXVec3TransformNormal(&m_d3dxvLook, &m_d3dxvLook, &mtxRotate);
			D3DXVec3TransformNormal(&m_d3dxvRight, &m_d3dxvRight, &mtxRotate);
		}
		if (z != 0.0f)
		{
			D3DXMatrixRotationAxis(&mtxRotate, &m_d3dxvLook, (float)D3DXToRadian(z));
			D3DXVec3TransformNormal(&m_d3dxvUp, &m_d3dxvUp, &mtxRotate);
			D3DXVec3TransformNormal(&m_d3dxvRight, &m_d3dxvRight, &mtxRotate);
		}
	}

	D3DXVec3Normalize (&m_d3dxvLook, &m_d3dxvLook);
	D3DXVec3Cross(&m_d3dxvRight, &m_d3dxvUp, &m_d3dxvLook);
	D3DXVec3Normalize(&m_d3dxvRight, &m_d3dxvRight);
	D3DXVec3Cross(&m_d3dxvUp, &m_d3dxvLook, &m_d3dxvRight);
	D3DXVec3Normalize(&m_d3dxvUp, &m_d3dxvUp);

	RegenerateWorldMatrix();
}

void CPlayer::Update(float fTimeElapsed)
{
	m_d3dxvVelocity += m_d3dxvGravity * fTimeElapsed;
	float fLength = sqrtf(m_d3dxvVelocity.x * m_d3dxvVelocity.x + m_d3dxvVelocity.z * m_d3dxvVelocity.z);

	if (fLength > m_fMaxVelocityXZ)
	{
		m_d3dxvVelocity.x *= (m_fMaxVelocityXZ / fLength);
		m_d3dxvVelocity.z *= (m_fMaxVelocityXZ / fLength);
	}

	fLength = sqrtf (m_d3dxvVelocity.y * m_d3dxvVelocity.y);
	if (fLength > m_fMaxVelocityY) m_d3dxvVelocity.y *= (m_fMaxVelocityY / fLength);

	Move (m_d3dxvVelocity * fTimeElapsed, false);

	if (m_pPlayerUpdatedContext) OnplayerUpdated(fTimeElapsed);

	DWORD nCurrentCameraMode = m_pCamera->GetMode();

	if (nCurrentCameraMode == THIRD_PERSON_CAMERA)	m_pCamera->Update(fTimeElapsed);
	if (m_pCameraUpdatedContext)	OnCameraUpdated(fTimeElapsed);
	if (nCurrentCameraMode == THIRD_PERSON_CAMERA)	m_pCamera->SetLookAt(m_d3dxvPosition);

	m_pCamera->RegenerateViewMatrix();

	D3DXVECTOR3 d3dxvDeceleration = -m_d3dxvVelocity;
	D3DXVec3Normalize(&d3dxvDeceleration, &d3dxvDeceleration);
	fLength = D3DXVec3Length(&m_d3dxvVelocity);
	float fDeceleration = (m_fFriction * fTimeElapsed);
	if (fDeceleration > fLength) fDeceleration = fLength;
	m_d3dxvVelocity += d3dxvDeceleration * fDeceleration;
}
CCamera *CPlayer :: OnChangeCamera(ID3D11Device *pd3dDevice, DWORD nNewCameraMode, DWORD nCurrentCameraMode)
{
	CCamera *pNewCamera = NULL;

	// 새로운 카메라의 모드에 따라 카메라를 새로 생성한다
	switch (nNewCameraMode)
	{
	case FIRST_PERSON_CAMERA:
		pNewCamera = new CFirstPersonCamera(m_pCamera);
		break;
	case THIRD_PERSON_CAMERA:
		pNewCamera = new CThirdPersonCamera(m_pCamera);
		break;
	case SPACESHIP_CAMERA:
		pNewCamera = new CSpaceShipCamera(m_pCamera);
		break;
	}

	if (nCurrentCameraMode == SPACESHIP_CAMERA)
	{
		m_d3dxvUp = D3DXVECTOR3 (0.0f, 1.0f, 0.0f);
		m_d3dxvRight.y = 0.0f;
		m_d3dxvLook.y = 0.0f;
		D3DXVec3Normalize(&m_d3dxvRight, &m_d3dxvRight);
		D3DXVec3Normalize(&m_d3dxvLook, &m_d3dxvLook);
		m_fPitch = 0.0f;
		m_fRoll = 0.0f;
		m_fYaw = (float) D3DXToDegree(acosf(D3DXVec3Dot(&D3DXVECTOR3(0.0f, 0.0f, 1.0f), &m_d3dxvLook)));
		if (m_d3dxvLook.x < 0.0f) m_fYaw = -m_fYaw;
	}

	else if ((nNewCameraMode == SPACESHIP_CAMERA) && m_pCamera)
	{
		m_d3dxvRight = m_pCamera->GetRightVector();
		m_d3dxvUp = m_pCamera->GetUpVector();
		m_d3dxvLook = m_pCamera->GetLookVector();
	}

	if (pNewCamera)
	{
		if (!m_pCamera) pNewCamera->CreateShaderVariables(pd3dDevice);
		pNewCamera->SetMode(nNewCameraMode);
		pNewCamera->SetPlayer(this);
	}

	if (m_pCamera) delete m_pCamera;

	return pNewCamera;
}

void CPlayer::Render(ID3D11DeviceContext *pd3dDeviceContext)
{
	if (m_pShader)
	{
		m_pShader->UpdateShaderVariables (pd3dDeviceContext, &m_d3dxmtxWorld);
		m_pShader->Render (pd3dDeviceContext);
	}

	if (m_pMesh) m_pMesh->Render (pd3dDeviceContext);
}

void CPlayer::OnplayerUpdated(float fTimeElapsed)
{

}

void CPlayer::OnCameraUpdated(float fTimeElapsed)
{

}

CAirplanePlayer::CAirplanePlayer(ID3D11Device *pd3dDevice)
{
	// 비행기 메쉬를 생성
	CMesh *pAirplaneMesh = new CAirplaneMesh(pd3dDevice, 20.0f, 20.0f, 4.0f, D3DCOLOR_XRGB(0,255,0));
	SetMesh(pAirplaneMesh);

	m_pShader = new CPlayerShader();
	m_pShader->CreateShader (pd3dDevice);
	m_pShader->CreateShaderVariables (pd3dDevice);

	CreateShaderVariables (pd3dDevice);
}

CAirplanePlayer::~CAirplanePlayer()
{
	if (m_pShader) delete m_pShader;
}

void CAirplanePlayer::Render(ID3D11DeviceContext *pd3dDeviceContext)
{
	DWORD nCurrentCameraMode = (m_pCamera) ? m_pCamera->GetMode() : 0x00;
	if ((nCurrentCameraMode == THIRD_PERSON_CAMERA) && m_pMesh)
	{
		D3DXMATRIX mtxRotate;
		D3DXMatrixRotationYawPitchRoll(&mtxRotate, 0.0f, (float) D3DXToRadian(90.0f), 0.0f);
		m_d3dxmtxWorld = mtxRotate * m_d3dxmtxWorld;

		CPlayer::Render(pd3dDeviceContext);

	}
}

void CAirplanePlayer::ChangeCamera(ID3D11Device *pd3dDevice, DWORD nNewCameraMode, float fTimeElapsed)
{
	DWORD nCurrentCameraMode = (m_pCamera) ? m_pCamera->GetMode() : 0x00;
	if (nCurrentCameraMode == nNewCameraMode) return;
	switch (nNewCameraMode)
	{
	case FIRST_PERSON_CAMERA:
		SetFriction(200.0f);
		SetGravity(D3DXVECTOR3(0.0f, 0.0f, 0.0f));
		SetMaxVelocityXZ(125.0f);
		SetMaxVelocityY(400.0f);
		m_pCamera = OnChangeCamera(pd3dDevice, FIRST_PERSON_CAMERA, nCurrentCameraMode);
		m_pCamera->SetTimeLag(0.0f);
		m_pCamera->Setoffset(D3DXVECTOR3(0.0f, 20.0f, 0.0f));
		m_pCamera->GenerateProjectionMatrix(1.01f, 5000.0f, ASPECT_RATIO, 60.0f);
		break;

	case SPACESHIP_CAMERA:
		SetFriction(125.0f);
		SetGravity(D3DXVECTOR3(0.0f, 0.0f, 0.0f));
		SetMaxVelocityXZ(400.0f);
		SetMaxVelocityY(400.0f);
		m_pCamera = OnChangeCamera(pd3dDevice, SPACESHIP_CAMERA, nCurrentCameraMode);
		m_pCamera->SetTimeLag(0.0f);
		m_pCamera->Setoffset(D3DXVECTOR3(0.0f, 0.0f, 0.0f));
		m_pCamera->GenerateProjectionMatrix(1.0f, 5000.0f, ASPECT_RATIO, 60.0f);
		break;
		
	case THIRD_PERSON_CAMERA:
		SetFriction(250.0f);
		SetGravity(D3DXVECTOR3(0.0f, 0.0f, 0.0f));
		SetMaxVelocityXZ(125.0f);
		SetMaxVelocityY(400.0f);
		m_pCamera = OnChangeCamera(pd3dDevice, THIRD_PERSON_CAMERA, nCurrentCameraMode);
		m_pCamera->SetTimeLag(0.25f);
		m_pCamera->Setoffset(D3DXVECTOR3(0.0f, 20.0f, -50.0f));
		m_pCamera->GenerateProjectionMatrix(1.01f, 5000.0f, ASPECT_RATIO, 60.0f);
		break;

	default:
		break;
	}

	Update(fTimeElapsed);
}
