#pragma once


#define  RANDOM_COLOR D3DXCOLOR ((rand() * 0xFFFFFF) / RAND_MAX)


class CDiffusedVertex
{
	D3DXVECTOR3 m_d3dxvPosition;
	// 정점의 색상을 나타내는 멤버변수 (D3DXCOLOR 구조체)를 선언한다.
	D3DXCOLOR m_d3dxcDiffuse;
	
public:
	CDiffusedVertex (float x, float y, float z, D3DXCOLOR d3dxcDiffuse)
	{
		m_d3dxvPosition = D3DXVECTOR3 (x, y, z);
		m_d3dxcDiffuse = d3dxcDiffuse;
	}
	
	CDiffusedVertex (D3DXVECTOR3 d3dxvPosition, D3DXCOLOR d3dxcDiffuse)
	{
		m_d3dxvPosition = d3dxvPosition;
		m_d3dxcDiffuse = d3dxcDiffuse;
	}

	CDiffusedVertex() 
	{
		m_d3dxvPosition = D3DXVECTOR3 (0.0f, 0.0f, 0.0f);
		m_d3dxcDiffuse = D3DXCOLOR (0.0f, 0.0f, 0.0f, 0.0f);
	}

	~CDiffusedVertex() {}
};

class CVertex
{
	// 정점의 위치 정보 (3차원 벡터)를 저장하기 위한 멤버 변수를 선언한다.
	D3DXVECTOR3 m_d3dxvPosition;
public:
	// CVertex 클래스의 생성자와 소멸자를 다음과 같이 선언한다.
	CVertex() {m_d3dxvPosition = D3DXVECTOR3 (0, 0, 0);}
	CVertex(D3DXVECTOR3 d3dxvPosition) {m_d3dxvPosition = d3dxvPosition;}
	~CVertex() {}
};

class CMesh
{
public:
	// CMesh 클래스의 생성자와 소멸자를 호출한다
	CMesh(ID3D11Device *pd3dDevice);
	virtual ~CMesh();


private:
	int m_nReferences;
public:
	void AddRef();
	void Release();

protected:
	// 정점 데이터를 저장하기 위한 정점 버퍼 인터페이스 포인터를 선언한다.
	ID3D11Buffer *m_pd3dVertexBuffer;

	/* 정점 버퍼의 정점 개수, 정점의 바이트 수, 정점 데이터가 정점 버퍼와 어디에서부터 시작하는 가를
	   나타내는 변수를 선언한다. */
	UINT m_nVertices;
	UINT m_nStride;
	UINT m_nOffset;

	// 정점 데이터가 어떤 프리미티브를 표현하고 있는 가를 나타내는 멤버 변수를 선언한다.
	D3D11_PRIMITIVE_TOPOLOGY m_d3dPrimitiveTopology;

	// 레스터라이저 상태 객체에 대한 인터페이스 포인터를 선언한다.
	ID3D11RasterizerState *m_pd3dRasterizerState;
public:
	virtual void Render (ID3D11DeviceContext *pd3dDeviceContext);

	virtual void CreateRasterizerState (ID3D11Device *pd3dDevice);

public:
	ID3D11Buffer *m_pd3dIndexBuffer;
	UINT m_nIndices;
	UINT m_nStartIndex;
	int m_nBaseVertex;
	
};

// CMesh 클래스를 기반 클래스로 하여 파생 클래스 CTriangleMesh를 선언한다

class CTriangleMesh : public CMesh
{
public:
	CTriangleMesh (ID3D11Device *pd3dDevice);
	virtual ~CTriangleMesh();

	virtual void Render (ID3D11DeviceContext *pd3dDeviceContext);

	virtual void CreateRasterizerState (ID3D11Device *pd3dDevice);
};



class CCubeMesh : public CMesh
{
public:
	// 직육면체의 가로 세로 높이의 크기를 지정하여 직육면체 메쉬를 생성한다.
	CCubeMesh (ID3D11Device *pd3dDevice, float fWidth = 2.0f, float fHeight = 2.0f, float fDepth = 2.0f);
	virtual ~CCubeMesh();

	virtual void CreateRasterizerState(ID3D11Device *pd3dDevice);
	virtual void Render (ID3D11DeviceContext *pd3dDeviceContext);
};

class CAirplaneMesh : public CMesh
{
public:
	CAirplaneMesh (ID3D11Device *pd3dDevice, float fWidth = 20.0f, float fHeight = 20.0f, float fDepth = 4.0f,
					D3DXCOLOR d3dxColor = D3DXCOLOR(1.0f, 1.0f, 0.0f, 0.0f));
	virtual ~CAirplaneMesh();

	virtual void CreateRasterizerState(ID3D11Device *pd3dDevice);
	virtual void Render (ID3D11DeviceContext *pd3dDeviceContext);

};