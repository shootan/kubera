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
	/*각 정점의 위치 벡터를 픽킹을 위하여 저장한다(정점 버퍼를 DYNAMIC으로 생성하고 Map()을 하지 않아도 되도록).*/
	D3DXVECTOR3 *m_pd3dxvPositions;
	/*메쉬의 인덱스를 저장한다(인덱스 버퍼를 DYNAMIC으로 생성하고 Map()을 하지 않아도 되도록).*/ 
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

	//정점 버퍼 배열에 인스턴스 데이터를 나타내는 정점 버퍼를 추가한다.

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
	//정점의 색상을 나타내는 멤버 변수(D3DXCOLOR 구조체)를 선언한다. 
	D3DXCOLOR m_d3dxcDiffuse;
public:
	//생성자와 소멸자를 선언한다.
	CDiffusedVertex(float x, float y, float z, D3DXCOLOR d3dxcDiffuse) { m_d3dxvPosition = D3DXVECTOR3(x, y, z); m_d3dxcDiffuse = d3dxcDiffuse; }
	CDiffusedVertex(D3DXVECTOR3 d3dxvPosition, D3DXCOLOR d3dxcDiffuse) { m_d3dxvPosition = d3dxvPosition; m_d3dxcDiffuse = d3dxcDiffuse; }
	CDiffusedVertex() { m_d3dxvPosition = D3DXVECTOR3(0.0f, 0.0f, 0.0f); m_d3dxcDiffuse = D3DXCOLOR(0.0f, 0.0f, 0.0f, 0.0f); }
	~CDiffusedVertex() { }
};


class CCubeMesh : public CMesh
{
public:
	//직육면체의 가로, 세로, 높이의 크기를 지정하여 직육면체 메쉬를 생성한다.
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