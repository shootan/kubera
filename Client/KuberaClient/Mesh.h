#pragma once
#include "header.h"
#include "Textureclass.h"
#include <fbxsdk.h>
#include <vector>
#include "AABB.h"

#define RANDOM_COLOR D3DXCOLOR((rand() * 0xFFFFFF) / RAND_MAX)


class CVertex
{
	D3DXVECTOR3 m_d3dxvPosition;
public:
	CVertex() {m_d3dxvPosition = D3DXVECTOR3(0,0,0);}
	CVertex(D3DXVECTOR3 d3dxvPosition) { m_d3dxvPosition = d3dxvPosition; }
	~CVertex() {}
};


class CMesh
{
public:
	CMesh(ID3D11Device *pd3dDevice);
	virtual ~CMesh(void);


	int m_nReferences;

public:
	void AddRef() { m_nReferences++; }
	void Release() { if (--m_nReferences <= 0) delete this; }

public:
	/*�� ������ ��ġ ���͸� ��ŷ�� ���Ͽ� �����Ѵ�(���� ���۸� DYNAMIC���� �����ϰ� Map()�� ���� �ʾƵ� �ǵ���).*/
	D3DXVECTOR3 *m_pd3dxvPositions;
	/*�޽��� �ε����� �����Ѵ�(�ε��� ���۸� DYNAMIC���� �����ϰ� Map()�� ���� �ʾƵ� �ǵ���).*/ 
	UINT *m_pnIndices; 
	int CheckRayIntersection(D3DXVECTOR3 *pd3dxvRayPosition, D3DXVECTOR3 *pd3dxvRayDirection, MESHINTERSECTINFO *pd3dxIntersectInfo);
	AABB GetBoundingCube() { return(m_bcBoundingCube); }
	void SetBoundingCube(D3DXVECTOR3 _boundsize);

protected:
	AABB m_bcBoundingCube;  

protected:
	ID3D11Buffer **m_ppd3dVertexBuffers;

	UINT m_nSlot;
	int m_nBuffers;

	UINT *m_pnVertexStrides;
	UINT *m_pnVertexOffsets;
	UINT m_nVertices;
	UINT m_nStartVertex;

	ID3D11Buffer *m_pd3dIndexBuffer;

	UINT m_nIndices;
	DXGI_FORMAT m_dxgiIndexFormat;
	UINT m_nIndexOffset;
	UINT m_nStartIndex;
	int m_nBaseVertex;

	ID3D11RasterizerState *m_pd3dRasterizerState;
	D3D11_PRIMITIVE_TOPOLOGY m_d3dPrimitiveTopology;

public:
	virtual void Render(ID3D11DeviceContext *pd3dDeviceContext);
	virtual void CreateRasterizerState(ID3D11Device *pd3dDevice);

	//���� ���� �迭�� �ν��Ͻ� �����͸� ��Ÿ���� ���� ���۸� �߰��Ѵ�.

	void AssembleToVertexBuffer(int nBuffers=0, ID3D11Buffer **m_pd3dBuffers=NULL, UINT *pnBufferStrides=NULL, UINT *pnBufferOffsets=NULL);

	virtual void RenderInstanced(ID3D11DeviceContext *pd3dDeviceContext, int nInstances=0, int nStartInstance=0);
	virtual bool LoadTexture(ID3D11Device* pd3dDevice, WCHAR* filename){return 0;}
	virtual void ReleaseTexture(){}
	virtual void Frame(float frametime){}
};



class CTriangleMesh : public CMesh
{
public:
	CTriangleMesh(ID3D11Device *pd3dDevice);
	virtual ~CTriangleMesh();

	virtual void Render(ID3D11DeviceContext *pd3dDeviceContext);
	virtual void CreateRasterizerState(ID3D11Device *pd3dDevice);

};

class CDiffusedVertex
{
	D3DXVECTOR3 m_d3dxvPosition;
	//������ ������ ��Ÿ���� ��� ����(D3DXCOLOR ����ü)�� �����Ѵ�. 
	D3DXCOLOR m_d3dxcDiffuse;
public:
	//�����ڿ� �Ҹ��ڸ� �����Ѵ�.
	CDiffusedVertex(float x, float y, float z, D3DXCOLOR d3dxcDiffuse) { m_d3dxvPosition = D3DXVECTOR3(x, y, z); m_d3dxcDiffuse = d3dxcDiffuse; }
	CDiffusedVertex(D3DXVECTOR3 d3dxvPosition, D3DXCOLOR d3dxcDiffuse) { m_d3dxvPosition = d3dxvPosition; m_d3dxcDiffuse = d3dxcDiffuse; }
	CDiffusedVertex() { m_d3dxvPosition = D3DXVECTOR3(0.0f, 0.0f, 0.0f); m_d3dxcDiffuse = D3DXCOLOR(0.0f, 0.0f, 0.0f, 0.0f); }
	~CDiffusedVertex() { }
};


class CCubeMesh : public CMesh
{
public:
	//������ü�� ����, ����, ������ ũ�⸦ �����Ͽ� ������ü �޽��� �����Ѵ�.
	CCubeMesh(ID3D11Device *pd3dDevice, float fWidth=2.0f, float fHeight=2.0f, float fDepth=2.0f);
	virtual ~CCubeMesh();

	virtual void CreateRasterizerState(ID3D11Device *pd3dDevice);
	virtual void Render(ID3D11DeviceContext *pd3dDeviceContext);
};

class CFBXMesh : public CMesh
{
	struct FBXVertex
	{
		D3DXVECTOR3 pos;
		D3DXVECTOR3 norm;
		D3DXVECTOR2 uv;
	};
	typedef std::vector<FBXVertex> FBXVERTS;
	FBXVERTS m_verts;

public:
	BOOL m_barray;
	float m_fuv;

	CFBXMesh(ID3D11Device *pd3dDevice, LPCWSTR filename);
	CFBXMesh(ID3D11Device *pd3dDevice, LPCWSTR filename, float UV);
	virtual ~CFBXMesh();

	HRESULT OnCreateFBXDevice(ID3D11Device* pd3dDevice, LPCWSTR filename);
	HRESULT LoadFBX(const char* filename, std::vector<FBXVertex>* pOutVertexVector, ID3D11Device* pd3dDevice);
	virtual void CreateRasterizerState(ID3D11Device *pd3dDevice);
	virtual void Render(ID3D11DeviceContext *pd3dDeviceContext);
	virtual void RenderInstanced(ID3D11DeviceContext *pd3dDeviceContext, int nInstances=0, int nStartInstance=0);
	virtual bool LoadTexture(ID3D11Device* pd3dDevice, WCHAR* filename);
	virtual void ReleaseTexture();


	CTextureclass* m_pTexture;
};