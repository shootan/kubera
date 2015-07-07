#include "DXUT.h"
#include "GFBXSkeleton.h"

namespace GFBX {


Skeleton::Skeleton()
{
	animationLength = 0;
}

Skeleton::~Skeleton()
{
}

D3DXMATRIX Skeleton::GetBoneTransform(int i, float t)
{
	D3DXMATRIX matRet;
	D3DXMatrixIdentity(&matRet);

	float frameGap = 0.1f;

	if (i >= bones.size())
		return matRet;
	if (bones[i].frames.size() <= 0)
		return matRet;

	float fFrame = t / frameGap;
	int f = fFrame;
	float l = fFrame - (float)f;
	float f1 = f % bones[i].frames.size();
	//float f2 = f1 + 1;
	//if (f2 >= bones[i].frames.size())
	//	f2 = 0;

	//D3DXVECTOR3 v1(
	//	bones[i].frames[f1].mGlobalBindpose._41,
	//	bones[i].frames[f1].mGlobalBindpose._42,
	//	bones[i].frames[f1].mGlobalBindpose._43);
	//D3DXVECTOR3 v2(
	//	bones[i].frames[f2].mGlobalBindpose._41,
	//	bones[i].frames[f2].mGlobalBindpose._42,
	//	bones[i].frames[f2].mGlobalBindpose._43);
	//D3DXVECTOR3 v = v1 * (1 - l) + v2 * l;

	matRet = bones[i].frames[f1].mGlobalBindpose;
	//matRet._41 = v.x;
	//matRet._42 = v.y;
	//matRet._43 = v.z;

	return matRet;
}

HRESULT Skeleton::DrawBones(IDirect3DDevice9* pd3dDevice, 
	const D3DXMATRIX* mWorld, const D3DXMATRIX* mView, const D3DXMATRIX* mProj, float t, float boneRadius)
{
	HRESULT hr;

	float frameGap = 0.1f;

	LPD3DXMESH pMesh = NULL;
	D3DXCreateSphere(pd3dDevice, boneRadius, 10, 10, &pMesh, NULL);

	D3DXMATRIX mWorldBefore, mViewBefore, mProjBefore;
	V(pd3dDevice->GetTransform(D3DTS_WORLD, &mWorldBefore));
	V(pd3dDevice->GetTransform(D3DTS_VIEW, &mViewBefore));
	V(pd3dDevice->GetTransform(D3DTS_PROJECTION, &mProjBefore));
	V(pd3dDevice->SetTransform(D3DTS_WORLD, mWorld));
	V(pd3dDevice->SetTransform(D3DTS_VIEW, mView));
	V(pd3dDevice->SetTransform(D3DTS_PROJECTION, mProj));
	V(pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE));
	V(pd3dDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_FALSE));
	V(pd3dDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID));
	V(pd3dDevice->SetTexture(0, NULL));

	for (int i = 0; i < bones.size(); i++)
	{
		D3DXMATRIX mat = GetBoneTransform(i, t);
		mat = mat * *mWorld;
		V(pd3dDevice->SetTransform(D3DTS_WORLD, &mat));
		V(pMesh->DrawSubset(0));
	}

	V(pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE));
	V(pd3dDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE));
	V(pd3dDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID));
	V(pd3dDevice->SetTransform(D3DTS_WORLD, &mWorldBefore));
	V(pd3dDevice->SetTransform(D3DTS_VIEW, &mViewBefore));
	V(pd3dDevice->SetTransform(D3DTS_PROJECTION, &mProjBefore));

	SAFE_RELEASE(pMesh);

	return S_OK;
}



};