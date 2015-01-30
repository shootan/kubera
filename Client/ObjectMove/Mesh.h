#pragma once


#define  RANDOM_COLOR D3DXCOLOR ((rand() * 0xFFFFFF) / RAND_MAX)


class CDiffusedVertex
{
	D3DXVECTOR3 m_d3dxvPosition;
	// ������ ������ ��Ÿ���� ������� (D3DXCOLOR ����ü)�� �����Ѵ�.
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
	// ������ ��ġ ���� (3���� ����)�� �����ϱ� ���� ��� ������ �����Ѵ�.
	D3DXVECTOR3 m_d3dxvPosition;
public:
	// CVertex Ŭ������ �����ڿ� �Ҹ��ڸ� ������ ���� �����Ѵ�.
	CVertex() {m_d3dxvPosition = D3DXVECTOR3 (0, 0, 0);}
	CVertex(D3DXVECTOR3 d3dxvPosition) {m_d3dxvPosition = d3dxvPosition;}
	~CVertex() {}
};

class CMesh
{
public:
	// CMesh Ŭ������ �����ڿ� �Ҹ��ڸ� ȣ���Ѵ�
	CMesh(ID3D11Device *pd3dDevice);
	virtual ~CMesh();


private:
	int m_nReferences;
public:
	void AddRef();
	void Release();

protected:
	// ���� �����͸� �����ϱ� ���� ���� ���� �������̽� �����͸� �����Ѵ�.
	ID3D11Buffer *m_pd3dVertexBuffer;

	/* ���� ������ ���� ����, ������ ����Ʈ ��, ���� �����Ͱ� ���� ���ۿ� ��𿡼����� �����ϴ� ����
	   ��Ÿ���� ������ �����Ѵ�. */
	UINT m_nVertices;
	UINT m_nStride;
	UINT m_nOffset;

	// ���� �����Ͱ� � ������Ƽ�긦 ǥ���ϰ� �ִ� ���� ��Ÿ���� ��� ������ �����Ѵ�.
	D3D11_PRIMITIVE_TOPOLOGY m_d3dPrimitiveTopology;

	// �����Ͷ����� ���� ��ü�� ���� �������̽� �����͸� �����Ѵ�.
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

// CMesh Ŭ������ ��� Ŭ������ �Ͽ� �Ļ� Ŭ���� CTriangleMesh�� �����Ѵ�

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
	// ������ü�� ���� ���� ������ ũ�⸦ �����Ͽ� ������ü �޽��� �����Ѵ�.
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