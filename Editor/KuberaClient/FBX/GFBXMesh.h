#pragma once

#include "GFBXSkeleton.h"
#include <vector>
#include "../Mesh.h"

#define MAX_BONE_MATRICES 70

namespace GFBX {

struct Vertex
{
	Vertex() {
		indices[0] = indices[1] = indices[2] = indices[3] = 0;
		weights = D3DXVECTOR4(1,0,0,0);
	}
	D3DXVECTOR3 pos;
	D3DXVECTOR3 norm;
	D3DXVECTOR2 uv;
	D3DXVECTOR3 tan;
	D3DXVECTOR3 binorm;
	BYTE indices[4];
	D3DXVECTOR4 weights;
};
typedef std::vector<Vertex> VERTS;
typedef std::vector<DWORD> INDICES;

struct CBConstBoneWorld
{
	D3DXMATRIX                      g_mConstBoneWorld[MAX_BONE_MATRICES];
};

class MeshSubset : public CMesh
{
public:
	MeshSubset(ID3D11Device *pd3dDevice);
	virtual ~MeshSubset();

	HRESULT OnCreateDevice(ID3D11Device* pd3dDevice);
	HRESULT CreateBuffers(ID3D11Device* pd3dDevice);

	virtual void Render(ID3D11DeviceContext *pd3dDeviceContext);
	virtual void CreateRasterizerState(ID3D11Device *pd3dDevice);
	void ResetCULLNONECreateRasterizerState(ID3D11Device *pd3dDevice);
	virtual void RenderInstanced(ID3D11DeviceContext *pd3dDeviceContext, int nInstances=0, int nStartInstance=0);
	virtual bool LoadTexture(ID3D11Device* pd3dDevice, WCHAR* filename);
	virtual void ReleaseTexture();

	void SetUVTilling(int _tilenum);

	CTextureclass* m_pTexture;

	VERTS m_verts;
	INDICES m_indices;

	std::string m_texDiffuseName;
	std::string m_texNormalName;

};


class Mesh
{
public:
	Mesh();
	virtual ~Mesh();

public:
	HRESULT OnCreateDevice(ID3D11Device* pd3dDevice);
	HRESULT Render(ID3D11DeviceContext* pd3dImmediateContext);
	HRESULT Render(ID3D11DeviceContext* pd3dImmediateContext, float t);
	void RenderInstanced(ID3D11DeviceContext *pd3dDeviceContext, int nInstances=0, int nStartInstance=0);

	MeshSubset* CreateSubset(ID3D11Device* pd3dDevice);
	int GetSubsetCount() { return m_Subsets.size(); }
	MeshSubset* GetSubset(int nIdx) { return m_Subsets[nIdx]; }

	std::vector<MeshSubset*>	m_Subsets;

	Skeleton	m_Initialskeleton;
	Skeleton	m_Animationkeleton;


	D3DXVECTOR3 m_vBoundingCenter;
	float m_fBoundingRadius;

	CBConstBoneWorld    g_CBConstBoneWorld;
	ID3D11Buffer*       g_pCBConstBoneWorld;

};

};
