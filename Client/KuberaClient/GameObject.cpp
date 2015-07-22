#include "GameObject.h"
#include "Shader.h"

CMaterial::CMaterial()
{
	m_nReferences = 0;
}

CMaterial::~CMaterial()
{
}


CGameObject::CGameObject(void)
{
	D3DXMatrixIdentity(&m_d3dxmtxWorld);
	
	m_pMesh = NULL;
	m_pAniMesh = NULL;

	m_Pos = D3DXVECTOR3(0, 0, 0);
	m_vDestination = m_Pos;
	m_Scale = D3DXVECTOR3(1,1,1);
	m_Rot = 1.0f;
	m_axis = 2;
	m_vWalkIncrement = D3DXVECTOR3(0,0,0);
	m_vFacingDirection= D3DXVECTOR3(0,0,1);

	m_nReferences = 1;

	m_HP = 0.f;
	m_Damage = 0.f;
	m_Defense = 0;
	m_Visible = TRUE;
	m_fWalkSpeed = 0.1f;
	m_iTag = NONE;
	m_iTargetID = 0;
	m_iState = IDLE;
	m_iTeam = NONE_TEAM;

	BoundsizeX = 0.0f;
	BoundsizeY = 0.0f;
	BoundsizeZ = 0.0f;

	m_pTarget = NULL;
	m_bSelected = FALSE;

	m_pMaterial = NULL;
}


CGameObject::~CGameObject(void)
{
	if (m_pMesh) m_pMesh->Release();
	if (m_pMaterial) m_pMaterial->Release();
}

void CGameObject::AddRef() 
{ 
	m_nReferences++; 
}

void CGameObject::Release() 
{ 
	if (m_nReferences > 0) m_nReferences--; 
	if (m_nReferences <= 0) delete this;
}

void CGameObject::SetMesh(CMesh *pMesh)
{
	if (m_pMesh) m_pMesh->Release();
	m_pMesh = pMesh;
	if (m_pMesh) m_pMesh->AddRef();

	if (pMesh)
	{
		AABB bcBoundingCube = pMesh->GetBoundingCube();
		m_bcMeshBoundingCube.Union(&bcBoundingCube);
	}
}

void CGameObject::SetMesh(GFBX::Mesh *pMesh)
{
	m_pAniMesh = pMesh;

	if (pMesh)
	{
		AABB bcBoundingCube = pMesh->GetSubset(0)->GetBoundingCube();
		m_bcMeshBoundingCube.Union(&bcBoundingCube);
	}
}

void CGameObject::Render(ID3D11DeviceContext *pd3dDeviceContext, CCamera *pCamera)
{
	if (m_pMaterial) CMaterialShader::UpdateMaterialShaderVariable(pd3dDeviceContext, &m_pMaterial->m_Material);

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
	D3DXMATRIX mtxRotate;
	D3DXMatrixIdentity(&mtxRotate);

	if(m_axis == 1)
		D3DXMatrixRotationX(&mtxRotate, D3DX_PI / m_Rot);
	else if(m_axis == 2)
		D3DXMatrixRotationY(&mtxRotate, D3DX_PI / m_Rot);
	else if(m_axis == 3)
		D3DXMatrixRotationZ(&mtxRotate, D3DX_PI / m_Rot);

	mWorld *= mtxRotate;

	//T
	D3DXMATRIX mtxTrans;
	D3DXMatrixIdentity(&mtxTrans);

	D3DXMatrixTranslation(&mtxTrans, m_Pos.x, m_Pos.y, m_Pos.z);
	//mWorld *= mtxTrans;

	mWorld = mtxScale * mtxRotate * mtxTrans;
	m_d3dxmtxWorld = mWorld;



	if (m_pMesh)
	{
		bool bIsVisible = true;
		if (pCamera)
		{
			AABB bcBoundingCube = m_pMesh->GetBoundingCube();
			bcBoundingCube.Update(&m_d3dxmtxWorld);
			bIsVisible = pCamera->IsInFrustum(&bcBoundingCube); 
		}
		if (bIsVisible) 
			m_pMesh->Render(pd3dDeviceContext);
	}
	else if (m_pAniMesh)
	{
		bool bIsVisible = true;

		for(int i=0; i<m_pAniMesh->GetSubsetCount(); i++)
		{
			if (pCamera)
			{
				AABB bcBoundingCube = m_pAniMesh->GetSubset(i)->GetBoundingCube();
				bcBoundingCube.Update(&m_d3dxmtxWorld);
				bIsVisible = pCamera->IsInFrustum(&bcBoundingCube); 

				if (bIsVisible) break;
			}
		}
		if (bIsVisible)	m_pAniMesh->Render(pd3dDeviceContext);
	}
}

void CGameObject::SetScale(D3DXVECTOR3 _size)
{
	m_Scale = _size;
}

void CGameObject::SetRotation(int xyz, float _rot)
{
	m_axis = xyz;
	m_Rot = _rot;
}

void CGameObject::SetPosition(D3DXVECTOR3 _pos)
{
	m_Pos = _pos;
	m_vDestination = _pos;
}


Vector3 CGameObject::GetPos()
{
	Vector3 v;
	v.x = m_Pos.x;
	v.y = m_Pos.y;
	v.z = m_Pos.z;

	return v;
}

Vector3 CGameObject::GetScale()
{
	Vector3 v;
	v.x = m_Scale.x;
	v.y = m_Scale.y;
	v.z = m_Scale.z;

	return v;
}

float CGameObject::GetRot()
{
	return m_Rot;
}

void CGameObject::SetPos(Vector3 v)
{
	D3DXVECTOR3 vec;

	vec.x = v.x;
	vec.y = v.y;
	vec.z = v.z;

	m_Pos.x = vec.x;
	m_Pos.y = vec.y;
	m_Pos.z = vec.z;
}

void CGameObject::SetScale(Vector3 v)
{
	D3DXVECTOR3 vec;

	vec.x = v.x;
	vec.y = v.y;
	vec.z = v.z;

	m_Scale.x = vec.x;
	m_Scale.y = vec.y;
	m_Scale.z = vec.z;
}

void CGameObject::SetRot(float f)
{
	m_Rot = f;
}

void CGameObject::SetBoundSize(float x, float y, float z)
{
	BoundsizeX = x;
	BoundsizeY = y;
	BoundsizeZ = z;
}

void CGameObject::SetP(float x, float y, float z)
{
	m_d3dxmtxWorld._41 = x; 
	m_d3dxmtxWorld._42 = y; 
	m_d3dxmtxWorld._43 = z; 
}


void CGameObject::GenerateRayForPicking(D3DXVECTOR3 *pd3dxvPickPosition, D3DXMATRIX *pd3dxmtxWorld, D3DXMATRIX *pd3dxmtxView, D3DXVECTOR3 *pd3dxvPickRayPosition, D3DXVECTOR3 *pd3dxvPickRayDirection)
{
	//pd3dxvPickPosition: 카메라 좌표계의 점(화면 좌표계에서 마우스를 클릭한 점을 역변환한 점)
	//pd3dxmtxWorld: 월드 변환 행렬, pd3dxmtxView: 카메라 변환 행렬
	//pd3dxvPickRayPosition: 픽킹 광선의 시작점, pd3dxvPickRayDirection: 픽킹 광선 벡터
	/*객체의 월드 변환 행렬이 주어지면 객체의 월드 변환 행렬과 카메라 변환 행렬을 곱하고 역행렬을 구한다. 이것은 카메라 변환 행렬의 역행렬과 객체의 월드 변환 행렬의 역행렬의 곱과 같다. 객체의 월드 변환 행렬이 주어지지 않으면 카메라 변환 행렬의 역행렬을 구한다. 객체의 월드 변환 행렬이 주어지면 모델 좌표계의 픽킹 광선을 구하고 그렇지 않으면 월드 좌표계의 픽킹 광선을 구한다.*/
	D3DXMATRIX d3dxmtxInverse;
	D3DXMATRIX d3dxmtxWorldView = *pd3dxmtxView;
	if (pd3dxmtxWorld) D3DXMatrixMultiply(&d3dxmtxWorldView, pd3dxmtxWorld, pd3dxmtxView);
	D3DXMatrixInverse(&d3dxmtxInverse, NULL, &d3dxmtxWorldView);
	D3DXVECTOR3 d3dxvCameraOrigin(0.0f, 0.0f, 0.0f);
	/*카메라 좌표계의 원점 (0, 0, 0)을 위에서 구한 역행렬로 변환한다. 변환의 결과는 카메라 좌표계의 원점에 대응되는 모델 좌표계의 점 또는 월드 좌표계의 점이다.*/
	D3DXVec3TransformCoord(pd3dxvPickRayPosition, &d3dxvCameraOrigin, &d3dxmtxInverse);
	/*카메라 좌표계의 점을 위에서 구한 역행렬로 변환한다. 변환의 결과는 마우스를 클릭한 점에 대응되는 모델 좌표계의 점 또는 월드 좌표계의 점이다.*/
	D3DXVec3TransformCoord(pd3dxvPickRayDirection, pd3dxvPickPosition, &d3dxmtxInverse);
	//픽킹 광선의 방향 벡터를 구한다.
	*pd3dxvPickRayDirection = *pd3dxvPickRayDirection - *pd3dxvPickRayPosition;
}

int CGameObject::PickObjectByRayIntersection(D3DXVECTOR3 *pd3dxvPickPosition, D3DXMATRIX *pd3dxmtxView, MESHINTERSECTINFO *pd3dxIntersectInfo)
{
	D3DXVECTOR3 d3dxvPickRayPosition, d3dxvPickRayDirection;
	int nIntersected = 0;
	if (m_pMesh)
	{
		GenerateRayForPicking(pd3dxvPickPosition, &m_d3dxmtxWorld, pd3dxmtxView, &d3dxvPickRayPosition, &d3dxvPickRayDirection);
		nIntersected = m_pMesh->CheckRayIntersection(&d3dxvPickRayPosition, &d3dxvPickRayDirection, pd3dxIntersectInfo);
		if (nIntersected > 0) return(nIntersected);
	}
	else if( m_pAniMesh)
	{
		GenerateRayForPicking(pd3dxvPickPosition, &m_d3dxmtxWorld, pd3dxmtxView, &d3dxvPickRayPosition, &d3dxvPickRayDirection);
		for(int i=0; i<m_pAniMesh->GetSubsetCount(); i++)
		{
			nIntersected = m_pAniMesh->GetSubset(i)->CheckRayIntersection(&d3dxvPickRayPosition, &d3dxvPickRayDirection, pd3dxIntersectInfo);
			if (nIntersected > 0) break;
		}
	}
	return(nIntersected);
}

bool CGameObject::IsVisible(CCamera *pCamera)
{
	bool bIsVisible = false;

	AABB bcBoundingCube = m_bcMeshBoundingCube;
	bcBoundingCube.Update(&m_d3dxmtxWorld);
	if (pCamera) bIsVisible = pCamera->IsInFrustum(&bcBoundingCube);
	
	return(bIsVisible);
}

void CGameObject::SetMaterial(CMaterial *pMaterial)
{
	if (m_pMaterial) m_pMaterial->Release();
	m_pMaterial = pMaterial;
	if (m_pMaterial) m_pMaterial->AddRef();
}