#include "ParticleObject.h"

ParticleObject::ParticleObject(void)
{
	CGameObject::CGameObject();

	m_iTag = EFFECT;
	m_vFacingDirection= D3DXVECTOR3(0,0,-1);
	m_Visible = TRUE;
	m_bUsed = FALSE;
	m_time = 0.f;
	m_iType = EFFECTTYPE_NONE;
	m_fWalkSpeed = 45.0f;

	m_CameraPosition = D3DXVECTOR3(0, 0, 0);
}

ParticleObject::~ParticleObject(void)
{
	CGameObject::~CGameObject();
}

void ParticleObject::Render(ID3D11DeviceContext *pd3dDeviceContext)
{
	if(m_Visible != TRUE) return;

	if(m_iTag == OTHERPLAYER)
	{
		int asdfdf;
		asdfdf = 0;
	}
	D3DXMATRIX mWorld;
	D3DXMatrixIdentity(&mWorld);

	//S
	D3DXMATRIX mtxScale;
	D3DXMatrixIdentity(&mtxScale);

	D3DXMatrixScaling(&mtxScale, m_Scale.x, m_Scale.y, m_Scale.z);
	//mWorld *= mtxScale;

	//R
	double angleY, angleX, angleZ;
	float rotationY , rotationX, rotationZ;
	D3DXMATRIX mtxRotate;
	D3DXMatrixIdentity(&mtxRotate);

	if(m_axis == 1)
		D3DXMatrixRotationX(&mtxRotate, D3DX_PI / m_Rot);
	else if(m_axis == 2)
		D3DXMatrixRotationY(&mtxRotate, D3DX_PI / m_Rot);
	else if(m_axis == 3)
		D3DXMatrixRotationZ(&mtxRotate, D3DX_PI / m_Rot);

	//angleY = atan2(m_Pos.x - m_CameraPosition.x, m_Pos.z - m_CameraPosition.z + 17.0f) * (180.0 / D3DX_PI);
	//rotationY = (float)angleY * 0.0174532925f;
	//angleX = atan2(m_Pos.y - m_CameraPosition.y, m_Pos.z - m_CameraPosition.z + 17.0f) * (180.0 / D3DX_PI);
	//rotationX = (float)angleX * 0.0174532925f;
	//angleZ = atan2(m_Pos.y - m_CameraPosition.y, m_Pos.x - m_CameraPosition.x) * (180.0 / D3DX_PI);
	//rotationZ = (float)angleZ * 0.0174532925f;
	//D3DXMatrixRotationY(&mtxRotate, rotationY);
	//D3DXMatrixRotationX(&mtxRotate, rotationX);
	//D3DXMatrixRotationZ(&mtxRotate, rotationZ)
	//D3DXMatrixRotationYawPitchRoll(&mtxRotate, rotationY, rotationX, 0);


	mWorld *= mtxRotate;

	//T
	D3DXMATRIX mtxTrans;
	D3DXMatrixIdentity(&mtxTrans);

	D3DXMatrixTranslation(&mtxTrans, m_Pos.x, m_Pos.y, m_Pos.z);
	//mWorld *= mtxTrans;

	mWorld = mtxScale * mtxRotate * mtxTrans;
	m_d3dxmtxWorld = mWorld;

	if (m_pMesh) m_pMesh->Render(pd3dDeviceContext);
}

void ParticleObject::Update(float fTimeElapsed)
{
	m_pMesh->Frame(fTimeElapsed);

	if(m_pTarget == NULL) return;

	if(m_iType == WIZARD_SKILL_BODY)
		SetPosition(m_pTarget->GetPosition());
	else if(m_iType == WIZARD_SKILL_MISSILE)
	{

		m_Pos += m_vWalkIncrement * fTimeElapsed * m_fWalkSpeed;

		if (ST::sharedManager()->GetDistance(this->GetPos(), m_pTarget->GetPos()) >= 100.f)
		{
			m_pTarget = NULL;
			m_Pos = D3DXVECTOR3(1200, 0, 0);
			m_bUsed = FALSE;
		}
	}

	//m_vWalkIncrement = m_vDestination - m_Pos;
	//D3DXVec3Normalize ( &m_vWalkIncrement, &m_vWalkIncrement );
}