#pragma once
#include "header.h"
#include "Textureclass.h"
#include <fbxsdk.h>
#include <vector>

/*������ ������ ��������(Random) �����ϱ� ���� ����Ѵ�. �� ������ ������ ����(Random Number)�� �����Ͽ� �����Ѵ�.*/ 
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
	void AddRef();
	void Release();

protected:
	ID3D11Buffer **m_ppd3dVertexBuffers;
	UINT m_nVertexBuffers;  //���� ������ ����
	UINT m_nVertexSlot;     //���� ���۸� ������ ���� �ε���
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

	//ID3D11Buffer *m_pd3dVertexBuffer; //���������͸� �����ϱ� ���� ���� ���� �������̽� ������ ����
	//ID3D11Buffer *m_pd3dIndexBuffer;
	///*���� ������ ���� ����, ������ ����Ʈ ��, ���� �����Ͱ� ���� ������ ��𿡼����� �����ϴ� ���� ��Ÿ���� ������ �����Ѵ�.*/
	//UINT m_nVertices;
	//UINT m_nStride;
	//UINT m_nOffset;

	ID3D11RasterizerState *m_pd3dRasterizerState;
	D3D11_PRIMITIVE_TOPOLOGY m_d3dPrimitiveTopology;

public:
	virtual void Render(ID3D11DeviceContext *pd3dDeviceContext);
	virtual void CreateRasterizerState(ID3D11Device *pd3dDevice);

	//���� ���� �迭�� �ν��Ͻ� �����͸� ��Ÿ���� ���� ���۸� �߰��Ѵ�.
	void AppendVertexBuffer(ID3D11Buffer *pd3dBuffer, UINT nStride=0, UINT nOffset=0);
	virtual void RenderInstanced(ID3D11DeviceContext *pd3dDeviceContext, int nInstances=0, int nStartInstance=0);
	virtual bool LoadTexture(ID3D11Device* pd3dDevice, WCHAR* filename){return 0;}
	virtual void ReleaseTexture(){}
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
	CFBXMesh(ID3D11Device *pd3dDevice, LPCWSTR filename);
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