#include "Mesh.h"


CMesh::CMesh(ID3D11Device *pd3dDevice)
{
	m_ppd3dVertexBuffers = NULL;
	m_nVertexSlot = 0;
	m_nVertexBuffers = 0;
	m_pnVertexStrides = NULL;
	m_pnVertexOffsets = NULL;
	m_nVertices = 0;
	m_nStartVertex = 0;

	m_d3dPrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	m_pd3dIndexBuffer = NULL;
	m_dxgiIndexFormat = DXGI_FORMAT_R16_UINT;
	m_nIndexOffset = 0;
	m_nIndices = 0;
	m_nStartIndex = 0;
	m_nBaseVertex = 0;

	m_pd3dRasterizerState = NULL;

	m_nReferences = 0;

	//
	//m_nStride = sizeof(CVertex);
	//m_nOffset = 0;
	//m_d3dPrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	//m_nReferences = 1;
	//m_pd3dRasterizerState = NULL;

}

CMesh::~CMesh(void)
{
	//if (m_pd3dVertexBuffer) m_pd3dVertexBuffer->Release();
	//if (m_pd3dIndexBuffer) m_pd3dIndexBuffer->Release();
	if (m_pd3dRasterizerState) m_pd3dRasterizerState->Release();

	if (m_ppd3dVertexBuffers)
	{
		for (UINT i = 0; i < m_nVertexBuffers; i++) if (m_ppd3dVertexBuffers[i]) m_ppd3dVertexBuffers[i]->Release();
		delete [] m_ppd3dVertexBuffers;
	}
	if (m_pd3dIndexBuffer) m_pd3dIndexBuffer->Release();

	if (m_pnVertexStrides) delete [] m_pnVertexStrides;
	if (m_pnVertexOffsets) delete [] m_pnVertexOffsets;
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

void CMesh::AppendVertexBuffer(ID3D11Buffer *pd3dBuffer, UINT nStride, UINT nOffset)
{
	//기존의 배열들 보다 하나 큰 배열을 생성하고 기존의 배열을 복사한 후 새로운 원소를 추가한다.
	UINT *pnVertexStrides = new UINT[m_nVertexBuffers+1];
	UINT *pnVertexOffsets = new UINT[m_nVertexBuffers+1];
	for (UINT i = 0; i < m_nVertexBuffers; i++) 
	{
		pnVertexStrides[i] = m_pnVertexStrides[i];
		pnVertexOffsets[i] = m_pnVertexOffsets[i];
	}
	delete [] m_pnVertexStrides;
	delete [] m_pnVertexOffsets;
	pnVertexStrides[m_nVertexBuffers] = nStride;
	pnVertexOffsets[m_nVertexBuffers] = nOffset;
	m_pnVertexStrides = pnVertexStrides;
	m_pnVertexOffsets = pnVertexOffsets;

	//기존의 정점 배열들 보다 하나 큰 배열을 생성하고 기존의 배열을 복사한 후 새로운 원소를 추가한다.
	ID3D11Buffer **ppd3dVertexBuffers = new ID3D11Buffer*[m_nVertexBuffers+1];
	for (UINT i = 0; i < m_nVertexBuffers; i++)
	{
		ppd3dVertexBuffers[i] = m_ppd3dVertexBuffers[i];
	}
	delete [] m_ppd3dVertexBuffers;
	ppd3dVertexBuffers[m_nVertexBuffers] = pd3dBuffer;
	if (pd3dBuffer) pd3dBuffer->AddRef();
	m_ppd3dVertexBuffers = ppd3dVertexBuffers;

	m_nVertexBuffers++;
}


void CMesh::Render(ID3D11DeviceContext *pd3dDeviceContext)
{
	//if (m_pd3dVertexBuffer) pd3dDeviceContext->IASetVertexBuffers(0, 1, &m_pd3dVertexBuffer, &m_nStride, &m_nOffset);
	//if (m_pd3dIndexBuffer) pd3dDeviceContext->IASetIndexBuffer(m_pd3dIndexBuffer, DXGI_FORMAT_R32_UINT, m_nOffset);
	if (m_ppd3dVertexBuffers) pd3dDeviceContext->IASetVertexBuffers( m_nVertexSlot, m_nVertexBuffers, m_ppd3dVertexBuffers, m_pnVertexStrides, m_pnVertexOffsets);
	if (m_pd3dIndexBuffer) pd3dDeviceContext->IASetIndexBuffer( m_pd3dIndexBuffer, m_dxgiIndexFormat, m_nIndexOffset);

	pd3dDeviceContext->IASetPrimitiveTopology(m_d3dPrimitiveTopology);
	//래스터라이저 상태를 디바이스 컨텍스트에 설정한다.
	if (m_pd3dRasterizerState) pd3dDeviceContext->RSSetState(m_pd3dRasterizerState);

	//pd3dDeviceContext->Draw(m_nVertices, m_nOffset);
	if (m_pd3dIndexBuffer) 
		pd3dDeviceContext->DrawIndexed(m_nIndices, m_nStartIndex, m_nBaseVertex);
	else
		pd3dDeviceContext->Draw(m_nVertices, m_nStartVertex);

}

void CMesh::RenderInstanced(ID3D11DeviceContext *pd3dDeviceContext, int nInstances, int nStartInstance)
{
	if (m_ppd3dVertexBuffers) pd3dDeviceContext->IASetVertexBuffers( m_nVertexSlot, m_nVertexBuffers, m_ppd3dVertexBuffers, m_pnVertexStrides, m_pnVertexOffsets);
	if (m_pd3dIndexBuffer) pd3dDeviceContext->IASetIndexBuffer( m_pd3dIndexBuffer, m_dxgiIndexFormat, m_nIndexOffset);
	pd3dDeviceContext->IASetPrimitiveTopology( m_d3dPrimitiveTopology);
	if (m_pd3dRasterizerState) pd3dDeviceContext->RSSetState( m_pd3dRasterizerState);

	//객체들의 인스턴스들을 렌더링한다. 
	if (m_pd3dIndexBuffer) 
		pd3dDeviceContext->DrawIndexedInstanced(m_nIndices, nInstances, m_nStartIndex, m_nBaseVertex, nStartInstance);
	else
		pd3dDeviceContext->DrawInstanced(m_nVertices, nInstances, m_nStartVertex, nStartInstance);
}

void CMesh::CreateRasterizerState(ID3D11Device *pd3dDevice)
{
}


CTriangleMesh::CTriangleMesh(ID3D11Device *pd3dDevice) : CMesh(pd3dDevice)
{
	m_nVertices = 3;
	//m_nStride = sizeof(CDiffusedVertex);
	//m_nOffset = 0;
	m_d3dPrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	/*정점(삼각형의 꼭지점)의 색상은 시계방향 순서대로 빨간색, 녹색, 파란색으로 지정한다. D3DXCOLOR 매크로는 RGBA(Red, Green, Blue, Alpha) 4개의 파라메터를 사용하여 색상을 표현하기 위하여 사용한다. 각 파라메터는 0.0~1.0 사이의 실수값을 가진다.*/
	CDiffusedVertex pVertices[3];
	pVertices[0] = CDiffusedVertex(D3DXVECTOR3(0.0f, 0.5f, 0.0f), D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f));
	pVertices[1] = CDiffusedVertex(D3DXVECTOR3(0.5f, -0.5f, 0.0f), D3DXCOLOR(0.0f, 1.0f, 0.0f, 1.0f));
	pVertices[2] = CDiffusedVertex(D3DXVECTOR3(-0.5f, -0.5f, 0.0f), D3DXCOLOR(0.0f, 0.0f, 1.0f, 1.0f));


	//정점 버퍼를 생성한다. 
	/*D3D11_BUFFER_DESC d3dBufferDesc;
	ZeroMemory(&d3dBufferDesc, sizeof(D3D11_BUFFER_DESC));
	d3dBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	d3dBufferDesc.ByteWidth = m_nStride * m_nVertices;
	d3dBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	d3dBufferDesc.CPUAccessFlags = 0;
	D3D11_SUBRESOURCE_DATA d3dBufferData;
	ZeroMemory(&d3dBufferData, sizeof(D3D11_SUBRESOURCE_DATA));
	d3dBufferData.pSysMem = pVertices;
	pd3dDevice->CreateBuffer(&d3dBufferDesc, &d3dBufferData, &m_ppd3dVertexBuffers[0]);*/



	CreateRasterizerState(pd3dDevice);

}

CTriangleMesh::~CTriangleMesh(void)
{
}

void CTriangleMesh::Render(ID3D11DeviceContext *pd3dDeviceContext)
{
	CMesh::Render(pd3dDeviceContext);
}

void CTriangleMesh::CreateRasterizerState(ID3D11Device *pd3dDevice)
{
	D3D11_RASTERIZER_DESC d3dRasterizerDesc;
	ZeroMemory(&d3dRasterizerDesc, sizeof(D3D11_RASTERIZER_DESC));
	//래스터라이저 단계에서 컬링(은면 제거)을 하지 않도록 래스터라이저 상태를 생성한다.
	d3dRasterizerDesc.CullMode = D3D11_CULL_NONE;
	d3dRasterizerDesc.FillMode = D3D11_FILL_SOLID;
	pd3dDevice->CreateRasterizerState(&d3dRasterizerDesc, &m_pd3dRasterizerState);
}


CCubeMesh::CCubeMesh(ID3D11Device *pd3dDevice, float fWidth, float fHeight, float fDepth) : CMesh(pd3dDevice)
{
	m_nVertices = 8;
	//m_nStride = sizeof(CDiffusedVertex);
	//m_nOffset = 0;
	m_pnVertexStrides = new UINT[1];
	m_pnVertexStrides[0] = sizeof(CDiffusedVertex);
	m_pnVertexOffsets = new UINT[1];
	m_pnVertexOffsets[0] = 0;

	m_nVertexSlot = 0;

	m_nVertexBuffers = 1;
	m_ppd3dVertexBuffers = new ID3D11Buffer*[m_nVertexBuffers];

	m_d3dPrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;

	float fx = fWidth*0.5f, fy = fHeight*0.5f, fz = fDepth*0.5f;
	CDiffusedVertex pVertices[8];
	int i = 0;

	pVertices[0] = CDiffusedVertex(D3DXVECTOR3(-fx, -fy, -fz), D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f));
	pVertices[1] = CDiffusedVertex(D3DXVECTOR3(-fx, -fy, +fz), D3DXCOLOR(0.0f, 1.0f, 0.0f, 1.0f));
	pVertices[2] = CDiffusedVertex(D3DXVECTOR3(+fx, -fy, +fz), D3DXCOLOR(0.0f, 0.0f, 1.0f, 1.0f));
	pVertices[3] = CDiffusedVertex(D3DXVECTOR3(+fx, -fy, -fz), D3DXCOLOR(1.0f, 1.0f, 0.0f, 1.0f));
	pVertices[4] = CDiffusedVertex(D3DXVECTOR3(-fx, +fy, -fz), D3DXCOLOR(1.0f, 0.0f, 1.0f, 1.0f));
	pVertices[5] = CDiffusedVertex(D3DXVECTOR3(-fx, +fy, +fz), D3DXCOLOR(0.0f, 1.0f, 1.0f, 1.0f));
	pVertices[6] = CDiffusedVertex(D3DXVECTOR3(+fx, +fy, +fz), D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f));
	pVertices[7] = CDiffusedVertex(D3DXVECTOR3(+fx, +fy, -fz), D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));

	D3D11_BUFFER_DESC d3dBufferDesc;
	ZeroMemory(&d3dBufferDesc, sizeof(D3D11_BUFFER_DESC));
	d3dBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	d3dBufferDesc.ByteWidth = sizeof(CDiffusedVertex) * m_nVertices;
	d3dBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	d3dBufferDesc.CPUAccessFlags = 0;
	D3D11_SUBRESOURCE_DATA d3dBufferData;
	ZeroMemory(&d3dBufferData, sizeof(D3D11_SUBRESOURCE_DATA));
	d3dBufferData.pSysMem = pVertices;
	pd3dDevice->CreateBuffer(&d3dBufferDesc, &d3dBufferData, &m_ppd3dVertexBuffers[0]);

	m_nIndices = 18;
	WORD pIndices[18];
	pIndices[0] = 5; //5,6,4 - cw
	pIndices[1] = 6; //6,4,7 - ccw
	pIndices[2] = 4; //4,7,0 - cw
	pIndices[3] = 7; //7,0,3 - ccw
	pIndices[4] = 0; //0,3,1 - cw
	pIndices[5] = 3; //3,1,2 - ccw
	pIndices[6] = 1; //1,2,2 - cw 
	pIndices[7] = 2; //2,2,3 - ccw
	pIndices[8] = 2; //2,3,3 - cw  - Degenerated Index(2)
	pIndices[9] = 3; //3,3,7 - ccw - Degenerated Index(3)
	pIndices[10] = 3;//3,7,2 - cw  - Degenerated Index(3)
	pIndices[11] = 7;//7,2,6 - ccw
	pIndices[12] = 2;//2,6,1 - cw
	pIndices[13] = 6;//6,1,5 - ccw
	pIndices[14] = 1;//1,5,0 - cw
	pIndices[15] = 5;//5,0,4 - ccw
	pIndices[16] = 0;
	pIndices[17] = 4;


	ZeroMemory(&d3dBufferDesc, sizeof(D3D11_BUFFER_DESC));
	d3dBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	d3dBufferDesc.ByteWidth = sizeof(WORD) * m_nIndices;        
	d3dBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	d3dBufferDesc.CPUAccessFlags = 0;
	ZeroMemory(&d3dBufferData, sizeof(D3D11_SUBRESOURCE_DATA));
	d3dBufferData.pSysMem = pIndices;
	pd3dDevice->CreateBuffer(&d3dBufferDesc, &d3dBufferData, &m_pd3dIndexBuffer);

	CreateRasterizerState(pd3dDevice);
}

CCubeMesh::~CCubeMesh()
{
}

void CCubeMesh::CreateRasterizerState(ID3D11Device *pd3dDevice)
{
	D3D11_RASTERIZER_DESC d3dRasterizerDesc;
	ZeroMemory(&d3dRasterizerDesc, sizeof(D3D11_RASTERIZER_DESC));
	//d3dRasterizerDesc.CullMode = D3D11_CULL_NONE;
	d3dRasterizerDesc.FillMode = D3D11_FILL_WIREFRAME;
	//d3dRasterizerDesc.FillMode = D3D11_FILL_SOLID;
	d3dRasterizerDesc.CullMode = D3D11_CULL_BACK;
	pd3dDevice->CreateRasterizerState(&d3dRasterizerDesc, &m_pd3dRasterizerState);
}

void CCubeMesh::Render(ID3D11DeviceContext *pd3dDeviceContext)
{
	CMesh::Render(pd3dDeviceContext);
}


CFBXMesh::CFBXMesh(ID3D11Device *pd3dDevice, LPCWSTR filename) : CMesh(pd3dDevice)
{
	m_nVertices = 0;
	//m_nStride = sizeof(FBXVertex);
	//m_nOffset = 0;
	m_pnVertexStrides = new UINT[1];
	m_pnVertexStrides[0] = sizeof(FBXVertex);
	m_pnVertexOffsets = new UINT[1];
	m_pnVertexOffsets[0] = 0;
	m_nVertexSlot = 0;
	m_nVertexBuffers = 1;
	m_ppd3dVertexBuffers = new ID3D11Buffer*[m_nVertexBuffers];

	m_d3dPrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	m_pTexture = NULL;
	OnCreateFBXDevice(pd3dDevice, filename);
}

CFBXMesh::~CFBXMesh()
{

}

HRESULT CFBXMesh::OnCreateFBXDevice(ID3D11Device* pd3dDevice , LPCWSTR filename)
{
	HRESULT hr;
	WCHAR s[MAX_PATH];
	V_RETURN(DXUTFindDXSDKMediaFileCch(s, MAX_PATH, filename));
	std::wstring wstr(s);
	std::string str("");
	str.assign(wstr.begin(), wstr.end());
	if(LoadFBX(str.c_str(), &m_verts, pd3dDevice) != S_OK)
		return S_FALSE;

	CreateRasterizerState(pd3dDevice);

	return S_OK;
}

FbxManager* g_pFbxSdkManager = nullptr;
HRESULT CFBXMesh::LoadFBX(const char* filename, std::vector<FBXVertex>* pOutVertexVector, ID3D11Device* pd3dDevice)
{
	if(g_pFbxSdkManager == nullptr)
	{
		g_pFbxSdkManager = FbxManager::Create();
		FbxIOSettings* pIOsettings = FbxIOSettings::Create(g_pFbxSdkManager, IOSROOT);
		g_pFbxSdkManager->SetIOSettings(pIOsettings);
	}

	FbxImporter* pImporter = FbxImporter::Create(g_pFbxSdkManager, "");
	FbxScene* pFbxScene = FbxScene::Create(g_pFbxSdkManager, "");

	bool bSuccess = pImporter->Initialize(filename, -1, g_pFbxSdkManager->GetIOSettings());
	if(!bSuccess)
		return E_FAIL;
	bSuccess = pImporter->Import(pFbxScene);
	if(!bSuccess)
		return E_FAIL;

	pImporter->Destroy();

	FbxNode* pFbxRootNode = pFbxScene->GetRootNode();

	if(pFbxRootNode)
	{
		int childCount = pFbxRootNode->GetChildCount();
		for(int i=0; i<childCount; i++)
		{
			FbxNode* pFbxChildNode = pFbxRootNode->GetChild(i);

			if(pFbxChildNode->GetNodeAttribute() == NULL)
				continue;

			FbxNodeAttribute::EType AttributeType = pFbxChildNode->GetNodeAttribute()->GetAttributeType();

			if(AttributeType != FbxNodeAttribute::eMesh)
				continue;

			FbxMesh* pMesh = (FbxMesh*)pFbxChildNode->GetNodeAttribute();

			FbxGeometryElementUV* IUVElement = pMesh->GetElementUV(0);
			if(!IUVElement)
				return S_FALSE;

			if(IUVElement->GetMappingMode() != FbxGeometryElement::eByPolygonVertex && IUVElement->GetMappingMode() != FbxGeometryElement::eByControlPoint)
				return S_FALSE;

			FbxVector4* pVertices = pMesh->GetControlPoints();

			int ployCount = pMesh->GetPolygonCount();

			for(int j=0; j<ployCount; j++)
			{
				int iNumVertices = pMesh->GetPolygonSize(j);
				assert(iNumVertices == 3);

				for(int k = 0; k<iNumVertices; k++)
				{
					FBXVertex vertex;
					//pos
					int iControlPointIndex = pMesh->GetPolygonVertex(j, k);
					vertex.pos.x = (float)pVertices[iControlPointIndex].mData[0];
					vertex.pos.y = (float)pVertices[iControlPointIndex].mData[1];
					vertex.pos.z = (float)pVertices[iControlPointIndex].mData[2];
					//norm

					FbxVector4 fbxVecNorm;
					pMesh->GetPolygonVertexNormal(j, k, fbxVecNorm);
					vertex.norm.x = fbxVecNorm.mData[0];
					vertex.norm.y = fbxVecNorm.mData[1];
					vertex.norm.z = fbxVecNorm.mData[2];

					//uv
					int ITextureUVIndex = pMesh->GetTextureUVIndex(j, k);
					FbxVector2 fbxVecUV = IUVElement->GetDirectArray().GetAt(ITextureUVIndex);
					vertex.uv.x = (float)fbxVecUV.mData[0];
					vertex.uv.y = 1- (float)fbxVecUV.mData[1];

					pOutVertexVector->push_back(vertex);
				}
			}
		}
	}

	//정점 버퍼를 생성한다. 
	D3D11_BUFFER_DESC d3dBufferDesc;
	ZeroMemory(&d3dBufferDesc, sizeof(D3D11_BUFFER_DESC));
	d3dBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	d3dBufferDesc.ByteWidth = sizeof(FBXVertex) * m_verts.size();
	d3dBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	d3dBufferDesc.CPUAccessFlags = 0;
	d3dBufferDesc.MiscFlags = 0;
	d3dBufferDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA d3dBufferData;
	ZeroMemory(&d3dBufferData, sizeof(D3D11_SUBRESOURCE_DATA));
	d3dBufferData.pSysMem = &m_verts[0];
	pd3dDevice->CreateBuffer(&d3dBufferDesc, &d3dBufferData, &m_ppd3dVertexBuffers[0]);


	//unsigned long* indices;
	//indices = new unsigned long[ m_verts.size()];
	//for(int i=0; i<  m_verts.size(); i++)
	//{
	//	indices[i] = i;
	//}

	//D3D11_BUFFER_DESC indexBufferDesc;
	//indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	//indexBufferDesc.ByteWidth = sizeof(unsigned long) *  m_verts.size();
	//indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	//indexBufferDesc.CPUAccessFlags = 0;
	//indexBufferDesc.MiscFlags = 0;
	//indexBufferDesc.StructureByteStride = 0;

	//// Give the subresource structure a pointer to the index data.
	//D3D11_SUBRESOURCE_DATA indexData;
	//indexData.pSysMem = indices;
	//indexData.SysMemPitch = 0;
	//indexData.SysMemSlicePitch = 0;

	//// Create the index buffer.
	//pd3dDevice->CreateBuffer(&indexBufferDesc, &indexData, &m_pd3dIndexBuffer);

	return S_OK;
}

void CFBXMesh::CreateRasterizerState(ID3D11Device *pd3dDevice)
{
	D3D11_RASTERIZER_DESC d3dRasterizerDesc;
	ZeroMemory(&d3dRasterizerDesc, sizeof(D3D11_RASTERIZER_DESC));
	d3dRasterizerDesc.CullMode = D3D11_CULL_BACK;
	d3dRasterizerDesc.FillMode = D3D11_FILL_SOLID;
	pd3dDevice->CreateRasterizerState(&d3dRasterizerDesc, &m_pd3dRasterizerState);
}

void CFBXMesh::Render(ID3D11DeviceContext *pd3dDeviceContext)
{
	m_nVertices = m_verts.size();
	pd3dDeviceContext->PSSetShaderResources(0, 1, &m_pTexture->m_texture );
	CMesh::Render(pd3dDeviceContext);
}

void CFBXMesh::RenderInstanced(ID3D11DeviceContext *pd3dDeviceContext, int nInstances, int nStartInstance)
{
	m_nVertices = m_verts.size();
	pd3dDeviceContext->PSSetShaderResources(0, 1, &m_pTexture->m_texture );
	CMesh::RenderInstanced(pd3dDeviceContext, nInstances, nStartInstance);
}



bool CFBXMesh::LoadTexture(ID3D11Device* pd3dDevice, WCHAR* filename)
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

void CFBXMesh::ReleaseTexture()
{
	if(m_pTexture)
	{
		m_pTexture->Release();
		delete m_pTexture;
		m_pTexture = 0;
	}

	return;
}
