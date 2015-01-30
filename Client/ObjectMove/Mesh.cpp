#include "StdAfx.h"
#include "Mesh.h"


CMesh::CMesh(ID3D11Device *pd3dDevice)
{
	m_nStride = sizeof(CVertex);
	m_nOffset = 0;
	m_d3dPrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	m_nReferences = 1;

	m_pd3dRasterizerState = NULL;

	m_pd3dIndexBuffer = NULL;
	m_nIndices = 0;
	m_nStartIndex = 0;
	m_nBaseVertex = 0;
}

CMesh::~CMesh()
{
	if (m_pd3dVertexBuffer) m_pd3dVertexBuffer->Release();

	if (m_pd3dRasterizerState) m_pd3dRasterizerState->Release();

	if (m_pd3dIndexBuffer) m_pd3dIndexBuffer->Release();
}

void CMesh::AddRef()
{
	m_nReferences++;
}

void CMesh::Release()
{
	m_nReferences--;
	if (m_nReferences == 0) delete this;
}

void CMesh::Render(ID3D11DeviceContext *pd3dDeviceContext)
{
	if (m_pd3dVertexBuffer) pd3dDeviceContext->IASetVertexBuffers(0, 1, &m_pd3dVertexBuffer, &m_nStride, &m_nOffset);
	if (m_pd3dIndexBuffer) pd3dDeviceContext->IASetIndexBuffer(m_pd3dIndexBuffer, DXGI_FORMAT_R16_UINT, 0);
	pd3dDeviceContext->IASetPrimitiveTopology(m_d3dPrimitiveTopology);
	if (m_pd3dRasterizerState) pd3dDeviceContext->RSSetState(m_pd3dRasterizerState);

	if (m_pd3dIndexBuffer)
		pd3dDeviceContext->DrawIndexed(m_nIndices, m_nStartIndex, m_nBaseVertex);
	else 
		pd3dDeviceContext->Draw(m_nVertices, m_nOffset);
}


void CMesh::CreateRasterizerState(ID3D11Device *pd3dDevice)
{

}


CTriangleMesh::CTriangleMesh(ID3D11Device *pd3dDevice) : CMesh(pd3dDevice)
{
	m_nVertices = 3;
	m_nStride = sizeof (CDiffusedVertex);
	m_nOffset = 0;
	m_d3dPrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
/*
	���� (�ﰢ���� ������)�� ������ �ð���� ������� ��, ��, �� �� �����Ѵ�.
	D3DXCOLOR ��ũ�δ� RGBA 4���� �Ķ���͸� ����Ͽ� ������ ǥ���ϱ� ���Ͽ� ����Ѵ�
	�� �Ķ���ʹ� 0.0 ~ 1.0 ������ �Ǽ����� ������.*/

	CDiffusedVertex pvertices[3];
	pvertices[0] = CDiffusedVertex (D3DXVECTOR3 (0.0f, 0.5f, 0.0f), D3DXCOLOR (1.0f, 0.0f, 0.0f, 1.0f));
	pvertices[1] = CDiffusedVertex (D3DXVECTOR3 (0.5f, -0.5f, 0.0f), D3DXCOLOR (0.0f, 1.0f, 0.0f, 1.0f));
	pvertices[2] = CDiffusedVertex (D3DXVECTOR3 (-0.5f, -0.5f, 0.0f), D3DXCOLOR (0.0f, 0.0f, 1.0f, 1.0f));

	D3D11_BUFFER_DESC d3dBufferDesc;
	ZeroMemory (&d3dBufferDesc, sizeof (D3D11_BUFFER_DESC));
	d3dBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	d3dBufferDesc.ByteWidth = m_nStride * m_nVertices;
	d3dBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	d3dBufferDesc.CPUAccessFlags = 0;
	D3D11_SUBRESOURCE_DATA d3dBufferData;
	ZeroMemory (&d3dBufferData, sizeof (D3D11_SUBRESOURCE_DATA));
	d3dBufferData.pSysMem = pvertices;
	pd3dDevice->CreateBuffer(&d3dBufferDesc, &d3dBufferData, &m_pd3dVertexBuffer);

	CreateRasterizerState(pd3dDevice);
}

CTriangleMesh::~CTriangleMesh()
{

}

void CTriangleMesh::Render(ID3D11DeviceContext *pd3dDeviceContext)
{
	CMesh::Render(pd3dDeviceContext);
}

void CTriangleMesh::CreateRasterizerState(ID3D11Device *pd3dDevice)
{
	D3D11_RASTERIZER_DESC d3dRasterizerDesc;
	ZeroMemory (&d3dRasterizerDesc, sizeof(D3D11_RASTERIZER_DESC));
	// �����Ͷ����� �ܰ迡�� �ø�(��������)�� ���� �ʵ��� �����Ͷ����� ���¸� �����Ѵ�.
	d3dRasterizerDesc.CullMode = D3D11_CULL_NONE;
	d3dRasterizerDesc.FillMode = D3D11_FILL_SOLID;
	pd3dDevice->CreateRasterizerState(&d3dRasterizerDesc, &m_pd3dRasterizerState);
}

CCubeMesh::CCubeMesh(ID3D11Device *pd3dDevice, float fWidth, float fHeight, float fDepth) : CMesh (pd3dDevice)
{
	m_nVertices = 8;
	m_nStride = sizeof(CDiffusedVertex);
	m_nOffset = 0;
	
	// ������Ƽ�� ������ �ﰢ�� ��Ʈ������ �����Ѵ�.
	m_d3dPrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;

	float fx = fWidth*0.5f, 
		fy = fHeight*0.5f, 
		fz = fDepth*0.5f;

	CDiffusedVertex pVertices[8];
	pVertices[0] = CDiffusedVertex (D3DXVECTOR3 (-fx, -fy, -fz), D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f));
	pVertices[1] = CDiffusedVertex (D3DXVECTOR3 (-fx, -fy, +fz), D3DXCOLOR(0.0f, 1.0f, 0.0f, 1.0f));
	pVertices[2] = CDiffusedVertex (D3DXVECTOR3 (+fx, -fy, +fz), D3DXCOLOR(0.0f, 0.0f, 1.0f, 1.0f));
	pVertices[3] = CDiffusedVertex (D3DXVECTOR3 (+fx, -fy, -fz), D3DXCOLOR(1.0f, 1.0f, 0.0f, 1.0f));
	pVertices[4] = CDiffusedVertex (D3DXVECTOR3 (-fx, +fy, -fz), D3DXCOLOR(1.0f, 0.0f, 1.0f, 1.0f));
	pVertices[5] = CDiffusedVertex (D3DXVECTOR3 (-fx, +fy, +fz), D3DXCOLOR(0.0f, 1.0f, 1.0f, 1.0f));
	pVertices[6] = CDiffusedVertex (D3DXVECTOR3 (+fx, +fy, +fz), D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f));
	pVertices[7] = CDiffusedVertex (D3DXVECTOR3 (+fx, +fy, -fz), D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));

	D3D11_BUFFER_DESC d3dBufferDesc;
	ZeroMemory (&d3dBufferDesc, sizeof (D3D11_BUFFER_DESC));
	d3dBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	d3dBufferDesc.ByteWidth = sizeof(CDiffusedVertex) * m_nVertices;
	d3dBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	d3dBufferDesc.CPUAccessFlags = 0;
	D3D11_SUBRESOURCE_DATA d3dBufferData;
	ZeroMemory (&d3dBufferData, sizeof(D3D11_SUBRESOURCE_DATA));
	d3dBufferData.pSysMem = pVertices;
	pd3dDevice->CreateBuffer(&d3dBufferDesc, &d3dBufferData, &m_pd3dVertexBuffer);
	
	/* 
	�ﰢ�� ��Ʈ���� ��� �� �ﰢ���� ������ 2���� ������ ���ο� ���� 1���� ����Ͽ� �����ȴ�.
	�׸��� �ﰢ���� ���ε� ������ �ð����� �ݽð������ ������ ��Ÿ���� �Ѵ�. �ݽð������
	�������� ���̷�Ʈ���� ���������� �ð�������� �ٲ�� �ȴ�. ������ü�� �ﰢ�� ��Ʈ������ 
	ǥ���� �� ������ ���� ���� �ε����� ����ϴ� �ϳ��� ����� 16���� �ε����� ����ϴ� ���̴�.
	���̷�Ʈ���� �ﰢ���� �׸� �� �ﰢ���� �� �������� ���������� �ﰢ���� ��Ÿ���� ������
	�׷����� �ʰ� �ȴ�. ���� ���, �� �� �̻��� ���� ������ ���̰ų� �� ���� ���� �ϳ��� ���� ����
	�ִ� ��쿡�� �ﰢ���� �׷����� �ʰ� �ȴ�. ���� �ﰢ�� ��Ʈ������ �ð������ �ﰢ���� �׸���
	������ �ݽð������ �������� ������ �ﰢ���� �־����� �׷����� �ʴ´�. �ݽð������ ��쿡 
	�ð������ �������� ������ �ﰢ���� �־��� ���� �׷����� ���� ���̴�. 
	*/
	
	m_nIndices = 18;
	WORD pIndices[18];
	
	pIndices[0] = 5;
	pIndices[1] = 6;
	pIndices[2] = 4;

	pIndices[3] = 7;
	pIndices[4] = 0;
	pIndices[5] = 3;

	pIndices[6] = 1;
	pIndices[7] = 2;
	
	pIndices[8] = 2;
	pIndices[9] = 3;
	pIndices[10] = 3;
	
	pIndices[11] = 7; 
	pIndices[12] = 2;
	pIndices[13] = 6;
	pIndices[14] = 1;
	pIndices[15] = 5;
	pIndices[16] = 0;
	pIndices[17] = 4;

	ZeroMemory(&d3dBufferDesc, sizeof(D3D11_BUFFER_DESC));
	d3dBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	d3dBufferDesc.ByteWidth = sizeof(WORD) * m_nIndices;        
	d3dBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	d3dBufferDesc.CPUAccessFlags = 0;
	ZeroMemory(&d3dBufferData, sizeof(D3D11_SUBRESOURCE_DATA));
	d3dBufferData.pSysMem = pIndices;
	//�ε��� ���۸� �����Ѵ�.
	pd3dDevice->CreateBuffer(&d3dBufferDesc, &d3dBufferData, &m_pd3dIndexBuffer);

	
	CreateRasterizerState(pd3dDevice);
}

CCubeMesh::~CCubeMesh() {}

void CCubeMesh::CreateRasterizerState(ID3D11Device *pd3dDevice)
{
	D3D11_RASTERIZER_DESC d3dRasterizerDesc;
	ZeroMemory (&d3dRasterizerDesc, sizeof(D3D11_RASTERIZER_DESC));
	d3dRasterizerDesc.CullMode = D3D11_CULL_NONE;
	d3dRasterizerDesc.FillMode = D3D11_FILL_SOLID;
	pd3dDevice->CreateRasterizerState(&d3dRasterizerDesc, &m_pd3dRasterizerState);	
}

void CCubeMesh::Render(ID3D11DeviceContext *pd3dDeviceContext)
{
	CMesh::Render(pd3dDeviceContext);
}

CAirplaneMesh::CAirplaneMesh(ID3D11Device *pd3dDevice, float fWidth /* = 20.0f */, float fHeight /* = 20.0f */, float fDepth /* = 4.0f */, 
								D3DXCOLOR d3dxColor /* = D3DXCOLOR */) : CMesh(pd3dDevice)
{
	m_nVertices = 24 * 3;
	m_nStride = sizeof (CDiffusedVertex);
	m_nOffset = 0;
	m_d3dPrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	CDiffusedVertex pVertices[24*3];

	float fx = fWidth * 0.5f, fy = fHeight * 0.5f, fz = fDepth * 0.5f;
	float x1 = fx * 0.2f, y1 = fy * 0.2f, x2 = fx * 0.1f, y3 = fy * 0.3f, y2 = ((y1-(fy-y3))/x1)*x2+(fy-y3);
	int i = 0;

	pVertices[i++] = CDiffusedVertex(0.0f, +(fy+y3), -fz, D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f));
	pVertices[i++] = CDiffusedVertex(+x1, -y1, -fz, RANDOM_COLOR);
	pVertices[i++] = CDiffusedVertex(0.0f, 0.0f, -fz, RANDOM_COLOR);

	pVertices[i++] = CDiffusedVertex(0.0f, +(fy+y3), -fz, D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f));
	pVertices[i++] = CDiffusedVertex(0.0f, 0.0f, -fz, RANDOM_COLOR);
	pVertices[i++] = CDiffusedVertex(-x1, -y1, -fz, RANDOM_COLOR);

	pVertices[i++] = CDiffusedVertex(+x2, +y2, -fz, D3DXCOLOR(0.0f, 0.0f, 1.0f, 1.0f));
	pVertices[i++] = CDiffusedVertex(+fx, -y3, -fz, RANDOM_COLOR);
	pVertices[i++] = CDiffusedVertex(+x1, -y1, -fz, RANDOM_COLOR);

	pVertices[i++] = CDiffusedVertex(-x2, +y2, -fz, D3DXCOLOR(0.0f, 0.0f, 1.0f, 1.0f));
	pVertices[i++] = CDiffusedVertex(-x1, -y1, -fz, RANDOM_COLOR);
	pVertices[i++] = CDiffusedVertex(-fx, -y3, -fz, RANDOM_COLOR);

	//����� �޽��� �Ʒ��� ���
	pVertices[i++] = CDiffusedVertex(0.0f, +(fy+y3), +fz, D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f));
	pVertices[i++] = CDiffusedVertex(0.0f, 0.0f, +fz, RANDOM_COLOR);
	pVertices[i++] = CDiffusedVertex(+x1, -y1, +fz, RANDOM_COLOR);

	pVertices[i++] = CDiffusedVertex(0.0f, +(fy+y3), +fz, D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f));
	pVertices[i++] = CDiffusedVertex(-x1, -y1, +fz, RANDOM_COLOR);
	pVertices[i++] = CDiffusedVertex(0.0f, 0.0f, +fz, RANDOM_COLOR);

	pVertices[i++] = CDiffusedVertex(+x2, +y2, +fz, D3DXCOLOR(0.0f, 0.0f, 1.0f, 1.0f));
	pVertices[i++] = CDiffusedVertex(+x1, -y1, +fz, RANDOM_COLOR);
	pVertices[i++] = CDiffusedVertex(+fx, -y3, +fz, RANDOM_COLOR);

	pVertices[i++] = CDiffusedVertex(-x2, +y2, +fz, D3DXCOLOR(0.0f, 0.0f, 1.0f, 1.0f));
	pVertices[i++] = CDiffusedVertex(-fx, -y3, +fz, RANDOM_COLOR);
	pVertices[i++] = CDiffusedVertex(-x1, -y1, +fz, RANDOM_COLOR);

	//����� �޽��� ������ ���
	pVertices[i++] = CDiffusedVertex(0.0f, +(fy+y3), -fz, RANDOM_COLOR);
	pVertices[i++] = CDiffusedVertex(0.0f, +(fy+y3), +fz, RANDOM_COLOR);
	pVertices[i++] = CDiffusedVertex(+x2, +y2, -fz, RANDOM_COLOR);

	pVertices[i++] = CDiffusedVertex(+x2, +y2, -fz, RANDOM_COLOR);
	pVertices[i++] = CDiffusedVertex(0.0f, +(fy+y3), +fz, RANDOM_COLOR);
	pVertices[i++] = CDiffusedVertex(+x2, +y2, +fz, RANDOM_COLOR);

	pVertices[i++] = CDiffusedVertex(+x2, +y2, -fz, RANDOM_COLOR);
	pVertices[i++] = CDiffusedVertex(+x2, +y2, +fz, RANDOM_COLOR);
	pVertices[i++] = CDiffusedVertex(+fx, -y3, -fz, RANDOM_COLOR);

	pVertices[i++] = CDiffusedVertex(+fx, -y3, -fz, RANDOM_COLOR);
	pVertices[i++] = CDiffusedVertex(+x2, +y2, +fz, RANDOM_COLOR);
	pVertices[i++] = CDiffusedVertex(+fx, -y3, +fz, RANDOM_COLOR);

	//����� �޽��� ����/������ ���
	pVertices[i++] = CDiffusedVertex(+x1, -y1, -fz, RANDOM_COLOR);
	pVertices[i++] = CDiffusedVertex(+fx, -y3, -fz, RANDOM_COLOR);
	pVertices[i++] = CDiffusedVertex(+fx, -y3, +fz, RANDOM_COLOR);

	pVertices[i++] = CDiffusedVertex(+x1, -y1, -fz, RANDOM_COLOR);
	pVertices[i++] = CDiffusedVertex(+fx, -y3, +fz, RANDOM_COLOR);
	pVertices[i++] = CDiffusedVertex(+x1, -y1, +fz, RANDOM_COLOR);

	pVertices[i++] = CDiffusedVertex(0.0f, 0.0f, -fz, RANDOM_COLOR);
	pVertices[i++] = CDiffusedVertex(+x1, -y1, -fz, RANDOM_COLOR);
	pVertices[i++] = CDiffusedVertex(+x1, -y1, +fz, RANDOM_COLOR);

	pVertices[i++] = CDiffusedVertex(0.0f, 0.0f, -fz, RANDOM_COLOR);
	pVertices[i++] = CDiffusedVertex(+x1, -y1, +fz, RANDOM_COLOR);
	pVertices[i++] = CDiffusedVertex(0.0f, 0.0f, +fz, RANDOM_COLOR);
	pVertices[i++] = CDiffusedVertex(0.0f, +(fy+y3), +fz, RANDOM_COLOR);
	pVertices[i++] = CDiffusedVertex(0.0f, +(fy+y3), -fz, RANDOM_COLOR);
	pVertices[i++] = CDiffusedVertex(-x2, +y2, -fz, RANDOM_COLOR);

	//����� �޽��� ���� ���
	pVertices[i++] = CDiffusedVertex(0.0f, +(fy+y3), +fz, RANDOM_COLOR);
	pVertices[i++] = CDiffusedVertex(-x2, +y2, -fz, RANDOM_COLOR);
	pVertices[i++] = CDiffusedVertex(-x2, +y2, +fz, RANDOM_COLOR);

	pVertices[i++] = CDiffusedVertex(-x2, +y2, +fz, RANDOM_COLOR);
	pVertices[i++] = CDiffusedVertex(-x2, +y2, -fz, RANDOM_COLOR);
	pVertices[i++] = CDiffusedVertex(-fx, -y3, -fz, RANDOM_COLOR);

	pVertices[i++] = CDiffusedVertex(-x2, +y2, +fz, RANDOM_COLOR);
	pVertices[i++] = CDiffusedVertex(-fx, -y3, -fz, RANDOM_COLOR);
	pVertices[i++] = CDiffusedVertex(-fx, -y3, +fz, RANDOM_COLOR);

	//����� �޽��� ����/���� ���
	pVertices[i++] = CDiffusedVertex(0.0f, 0.0f, -fz, RANDOM_COLOR);
	pVertices[i++] = CDiffusedVertex(0.0f, 0.0f, +fz, RANDOM_COLOR);
	pVertices[i++] = CDiffusedVertex(-x1, -y1, +fz, RANDOM_COLOR);

	pVertices[i++] = CDiffusedVertex(0.0f, 0.0f, -fz, RANDOM_COLOR);
	pVertices[i++] = CDiffusedVertex(-x1, -y1, +fz, RANDOM_COLOR);
	pVertices[i++] = CDiffusedVertex(-x1, -y1, -fz, RANDOM_COLOR);

	pVertices[i++] = CDiffusedVertex(-x1, -y1, -fz, RANDOM_COLOR);
	pVertices[i++] = CDiffusedVertex(-x1, -y1, +fz, RANDOM_COLOR);
	pVertices[i++] = CDiffusedVertex(-fx, -y3, +fz, RANDOM_COLOR);

	pVertices[i++] = CDiffusedVertex(-x1, -y1, -fz, RANDOM_COLOR);
	pVertices[i++] = CDiffusedVertex(-fx, -y3, +fz, RANDOM_COLOR);
	pVertices[i++] = CDiffusedVertex(-fx, -y3, -fz, RANDOM_COLOR);

	D3D11_BUFFER_DESC d3dBufferDesc;
	ZeroMemory(&d3dBufferDesc, sizeof(D3D11_BUFFER_DESC));
	d3dBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	d3dBufferDesc.ByteWidth = sizeof(CDiffusedVertex) * m_nVertices;
	d3dBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	d3dBufferDesc.CPUAccessFlags = 0;
	D3D11_SUBRESOURCE_DATA d3dBufferData;
	ZeroMemory(&d3dBufferData, sizeof(D3D11_SUBRESOURCE_DATA));
	d3dBufferData.pSysMem = pVertices;
	pd3dDevice->CreateBuffer(&d3dBufferDesc, &d3dBufferData, &m_pd3dVertexBuffer);

	CreateRasterizerState(pd3dDevice);
}

CAirplaneMesh :: ~CAirplaneMesh()
{

}

void CAirplaneMesh::CreateRasterizerState(ID3D11Device *pd3dDevice)
{
	D3D11_RASTERIZER_DESC d3dRasterizerDesc;
	ZeroMemory (&d3dRasterizerDesc, sizeof (D3D11_RASTERIZER_DESC));
	d3dRasterizerDesc.CullMode = D3D11_CULL_FRONT;
	d3dRasterizerDesc.FillMode = D3D11_FILL_SOLID;
	d3dRasterizerDesc.DepthClipEnable = true;
	pd3dDevice->CreateRasterizerState(&d3dRasterizerDesc, &m_pd3dRasterizerState);
}

void CAirplaneMesh::Render(ID3D11DeviceContext *pd3dDeviceContext)
{
	CMesh::Render(pd3dDeviceContext);
}