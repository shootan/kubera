#include "DXUT.h"
#include "DXUTcamera.h"
#include "DXUTgui.h"
#include "DXUTsettingsDlg.h"
#include "SDKmisc.h"
#include "SDKMesh.h"
#include "GFBXMesh.h"
//#include "../GTextureManager.h"

namespace GFBX {

MeshSubset::MeshSubset(ID3D11Device *pd3dDevice) : CMesh(pd3dDevice)
{
	m_nVertices = 0;
	m_pnVertexStrides = new UINT[1];
	m_pnVertexStrides[0] = sizeof(GFBX::Vertex);
	m_pnVertexOffsets = new UINT[1];
	m_pnVertexOffsets[0] = 0;
	m_nVertexSlot = 0;
	m_nVertexBuffers = 1;
	m_ppd3dVertexBuffers = new ID3D11Buffer*[m_nVertexBuffers];

	m_d3dPrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	m_pTexture = NULL;
}

MeshSubset::~MeshSubset()
{
}


HRESULT MeshSubset::OnCreateDevice(ID3D11Device* pd3dDevice) 
{
	CreateBuffers(pd3dDevice);
	return S_OK;
}



void MeshSubset::Render(ID3D11DeviceContext* pd3dImmediateContext)
{
	m_nVertices = m_verts.size();
	m_nIndices = m_indices.size();
	pd3dImmediateContext->PSSetShaderResources(0, 1, &m_pTexture->m_texture );
	CMesh::Render(pd3dImmediateContext);
}

void MeshSubset::CreateRasterizerState(ID3D11Device *pd3dDevice)
{
	D3D11_RASTERIZER_DESC d3dRasterizerDesc;
	ZeroMemory(&d3dRasterizerDesc, sizeof(D3D11_RASTERIZER_DESC));
	d3dRasterizerDesc.CullMode = D3D11_CULL_BACK;
	d3dRasterizerDesc.FillMode = D3D11_FILL_SOLID;
	pd3dDevice->CreateRasterizerState(&d3dRasterizerDesc, &m_pd3dRasterizerState);
}

void MeshSubset::RenderInstanced(ID3D11DeviceContext *pd3dDeviceContext, int nInstances, int nStartInstance)
{
	m_nVertices = m_verts.size();
	m_nIndices = m_indices.size();
	pd3dDeviceContext->PSSetShaderResources(0, 1, &m_pTexture->m_texture );
	CMesh::RenderInstanced(pd3dDeviceContext, nInstances, nStartInstance);
}

bool MeshSubset::LoadTexture(ID3D11Device* pd3dDevice, WCHAR* filename)
{
	bool result;

	m_pTexture = new CTextureclass;
	if(!m_pTexture)
	{
		return false;
	}


	result = m_pTexture->Initialize(pd3dDevice, filename);
	if(!result)
	{
		return false;
	}

	return true;
}

void MeshSubset::ReleaseTexture()
{
	if(m_pTexture)
	{
		m_pTexture->Release();
		delete m_pTexture;
		m_pTexture = 0;
	}

	return;
}

HRESULT MeshSubset::CreateBuffers(ID3D11Device* pd3dDevice)
{
	HRESULT hr;

	//정점 버퍼를 생성한다. 
	D3D11_BUFFER_DESC d3dBufferDesc;
	ZeroMemory(&d3dBufferDesc, sizeof(D3D11_BUFFER_DESC));
	d3dBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	d3dBufferDesc.ByteWidth = sizeof(GFBX::Vertex) * m_verts.size();
	d3dBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	d3dBufferDesc.CPUAccessFlags = 0;
	d3dBufferDesc.MiscFlags = 0;
	d3dBufferDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA d3dBufferData;
	ZeroMemory(&d3dBufferData, sizeof(D3D11_SUBRESOURCE_DATA));
	d3dBufferData.pSysMem = &m_verts[0];
	pd3dDevice->CreateBuffer(&d3dBufferDesc, &d3dBufferData, &m_ppd3dVertexBuffers[0]);


	D3D11_BUFFER_DESC indexBufferDesc;
	ZeroMemory(&indexBufferDesc, sizeof(D3D11_BUFFER_DESC));
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(DWORD) *  m_indices.size();
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the index data.
	D3D11_SUBRESOURCE_DATA indexData;
	ZeroMemory(&indexData, sizeof(D3D11_SUBRESOURCE_DATA));
	indexData.pSysMem = &m_indices[0];
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// Create the index buffer.
	pd3dDevice->CreateBuffer(&indexBufferDesc, &indexData, &m_pd3dIndexBuffer);

	CreateRasterizerState(pd3dDevice);

	return S_OK;
}







#define MAX_MATRICES	70


Mesh::Mesh()
{
	m_fBoundingRadius = 0;
	g_pCBConstBoneWorld = NULL;
}


Mesh::~Mesh()
{
}


HRESULT Mesh::OnCreateDevice(ID3D11Device* pd3dDevice)
{
	HRESULT hr;

	D3D11_BUFFER_DESC Desc;
	Desc.Usage = D3D11_USAGE_DYNAMIC;
	Desc.ByteWidth = sizeof( CBConstBoneWorld );
	Desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	Desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	Desc.MiscFlags = 0;

	V_RETURN( pd3dDevice->CreateBuffer( &Desc, NULL, &g_pCBConstBoneWorld) );
	DXUT_SetDebugName( g_pCBConstBoneWorld, "CBConstBoneWorld" );	

	for (int i = 0; i < m_Subsets.size(); i++)
	{
		if (m_Subsets[i]){
			m_Subsets[i]->OnCreateDevice(pd3dDevice);
		}
	}

	return S_OK;
}


HRESULT Mesh::Render(ID3D11DeviceContext* pd3dImmediateContext)
{
	HRESULT hr;

	D3DXMATRIX mat;
	D3DXMatrixIdentity(&mat);
	std::vector<D3DXMATRIX> mats;
	if (m_Animationkeleton.GetBoneCount() > 0)
	{
		mats.reserve(m_Animationkeleton.GetBoneCount());
		for (int i = 0; i < m_Animationkeleton.GetBoneCount(); i++)
		{
			if (mats.size() < MAX_MATRICES)
				mats.push_back(mat);
		}
	}
	else
	{
		if (mats.size() < MAX_MATRICES)
			mats.push_back(mat);
	}

	D3D11_MAPPED_SUBRESOURCE MappedResource;
	pd3dImmediateContext->Map(g_pCBConstBoneWorld, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource); 
	memcpy(MappedResource.pData, &g_CBConstBoneWorld, sizeof (g_CBConstBoneWorld));
	for(int k=0; k < mats.size(); k++)
		D3DXMatrixTranspose(&g_CBConstBoneWorld.g_mConstBoneWorld[k], &mats[k]);
	pd3dImmediateContext->Unmap(g_pCBConstBoneWorld, 0);
	pd3dImmediateContext->VSSetConstantBuffers(2, 1, &g_pCBConstBoneWorld); 
	

	for (int i = 0; i < m_Subsets.size(); i++)
	{
		MeshSubset* subset = m_Subsets[i];
		subset->Render(pd3dImmediateContext);
	}

	return S_OK;
}


HRESULT Mesh::Render(ID3D11DeviceContext* pd3dImmediateContext , float t)
{
	HRESULT hr;

	if (m_Animationkeleton.GetBoneCount() > 0)
	{
		std::vector<D3DXMATRIX> mats;
		mats.reserve(m_Animationkeleton.GetBoneCount());
		for (int i = 0; i < m_Animationkeleton.GetBoneCount(); i++)
		{
			D3DXMATRIX mat = m_Animationkeleton.GetBoneTransform(i, t);
			if (m_Initialskeleton.bones.size() > i && m_Initialskeleton.bones[i].frames.size() > 0) {
				mat = m_Initialskeleton.bones[i].frames[0].mGlobalBindposeInverse * mat;
			}
			if (mats.size() < MAX_MATRICES)
				mats.push_back(mat);
		}
		D3D11_MAPPED_SUBRESOURCE MappedResource;
		pd3dImmediateContext->Map(g_pCBConstBoneWorld, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource); 
		memcpy(MappedResource.pData, &g_CBConstBoneWorld, sizeof (g_CBConstBoneWorld));
		for(int k=0; k < mats.size(); k++)
			D3DXMatrixTranspose(&g_CBConstBoneWorld.g_mConstBoneWorld[k], &mats[k]);
		pd3dImmediateContext->Unmap(g_pCBConstBoneWorld, 0);
	}
	else
	{
		return Render(pd3dImmediateContext);
	}

	pd3dImmediateContext->VSSetConstantBuffers(2, 1, &g_pCBConstBoneWorld); 


	int a = m_Subsets.size();
	for (int i = 0; i < m_Subsets.size(); i++)
	{
		MeshSubset* subset = m_Subsets[i];
		subset->Render(pd3dImmediateContext);
	}

	return S_OK;
}


MeshSubset* Mesh::CreateSubset(ID3D11Device* pd3dDevice)
{
	MeshSubset* subset = new MeshSubset(pd3dDevice);
	m_Subsets.push_back(subset);
	return subset;
}


};