#include "UIClass.h"


UIClass::UIClass(ID3D11Device *pd3dDevice) : CMesh(pd3dDevice)
{
	m_pTexture = 0;

	m_nVertices = 0;
	m_nIndices = 0;
	m_dxgiIndexFormat = DXGI_FORMAT_R32_UINT;

	m_pnVertexStrides = new UINT[1];
	m_pnVertexStrides[0] = sizeof(VertexType);
	m_pnVertexOffsets = new UINT[1];
	m_pnVertexOffsets[0] = 0;
	m_nSlot = 0;
	m_nBuffers = 1;
	m_ppd3dVertexBuffers = new ID3D11Buffer*[m_nBuffers];

	CreateRasterizerState(pd3dDevice);
}

UIClass::~UIClass()
{
}


bool UIClass::Initialize(ID3D11Device* device, int screenWidth, int screenHeight, WCHAR* textureFilename, float bitmapWidth, float bitmapHeight)
{
	bool result;


	// Store the screen size.
	m_screenWidth = screenWidth;
	m_screenHeight = screenHeight;

	// Store the size in pixels that this bitmap should be rendered at.
	m_bitmapWidth = bitmapWidth;
	m_bitmapHeight = bitmapHeight;

	// Initialize the previous rendering position to negative one.
	m_previousPosX = -1;
	m_previousPosY = -1;

	// Initialize the vertex and index buffers.
	result = InitializeBuffers(device);
	if(!result)
	{
		return false;
	}

	// Load the texture for this bitmap.
	result = LoadTexture(device, textureFilename);
	if(!result)
	{
		return false;
	}

	return true;
}

void UIClass::CreateRasterizerState(ID3D11Device *pd3dDevice)
{
	D3D11_RASTERIZER_DESC d3dRasterizerDesc;
	ZeroMemory(&d3dRasterizerDesc, sizeof(D3D11_RASTERIZER_DESC));
	//래스터라이저 단계에서 컬링(은면 제거)을 하지 않도록 래스터라이저 상태를 생성한다.
	d3dRasterizerDesc.AntialiasedLineEnable = false;
	d3dRasterizerDesc.DepthBias = 0;
	d3dRasterizerDesc.DepthBiasClamp = 0.0f;
	d3dRasterizerDesc.DepthClipEnable = true;
	d3dRasterizerDesc.FrontCounterClockwise = false;
	d3dRasterizerDesc.MultisampleEnable = false;
	d3dRasterizerDesc.ScissorEnable = false;
	d3dRasterizerDesc.SlopeScaledDepthBias = 0.0f;
	d3dRasterizerDesc.CullMode = D3D11_CULL_BACK;
	d3dRasterizerDesc.FillMode = D3D11_FILL_SOLID;
	pd3dDevice->CreateRasterizerState(&d3dRasterizerDesc, &m_pd3dRasterizerState);
}


void UIClass::Shutdown()
{
	// Release the bitmap texture.
	ReleaseTexture();

	// Shutdown the vertex and index buffers.
	ShutdownBuffers();

	return;
}


bool UIClass::Render(ID3D11DeviceContext* deviceContext, float positionX, float positionY)
{
	bool result;

	deviceContext->PSSetShaderResources(0, 1, &m_pTexture->m_texture );
	// Re-build the dynamic vertex buffer for rendering to possibly a different location on the screen.
	result = UpdateBuffers(deviceContext, positionX, positionY);
	if(!result)
	{
		return false;
	}

	// Put the vertex and index buffers on the graphics pipeline to prepare them for drawing.

	CMesh::Render(deviceContext);

	return true;
}


int UIClass::GetIndexCount()
{
	return m_nIndices;
}


ID3D11ShaderResourceView* UIClass::GetTexture()
{
	return m_pTexture->GetTexture();
}


bool UIClass::InitializeBuffers(ID3D11Device* device)
{
	VertexType* vertices;
	unsigned long* indices;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;
	int i;


	// Set the number of vertices in the vertex array.
	//m_vertexCount = 6;
	m_nVertices = 6;

	// Set the number of indices in the index array.
	//m_indexCount = m_vertexCount;
	m_nIndices = m_nVertices;

	// Create the vertex array.
	vertices = new VertexType[m_nVertices];
	if(!vertices)
	{
		return false;
	}

	// Create the index array.
	indices = new unsigned long[m_nIndices];
	if(!indices)
	{
		return false;
	}

	// Initialize vertex array to zeros at first.
	memset(vertices, 0, (sizeof(VertexType) * m_nVertices));

	// Load the index array with data.
	for(i=0; i<m_nIndices; i++)
	{
		indices[i] = i;
	}

	// Set up the description of the static vertex buffer.
	vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_nVertices;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the vertex data.
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// Now create the vertex buffer.
	result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_ppd3dVertexBuffers[0]);
	if(FAILED(result))
	{
		return false;
	}

	// Set up the description of the static index buffer.
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_nIndices;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the index data.
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// Create the index buffer.
	result = device->CreateBuffer(&indexBufferDesc, &indexData, &m_pd3dIndexBuffer);
	if(FAILED(result))
	{
		return false;
	}

	// Release the arrays now that the vertex and index buffers have been created and loaded.
	delete [] vertices;
	vertices = 0;

	delete [] indices;
	indices = 0;

	return true;
}


void UIClass::ShutdownBuffers()
{
	// Release the index buffer.
	if(m_pd3dIndexBuffer)
	{
		m_pd3dIndexBuffer->Release();
		m_pd3dIndexBuffer = 0;
	}

	// Release the vertex buffer.
	if(m_ppd3dVertexBuffers)
	{
		m_ppd3dVertexBuffers[0]->Release();
		m_ppd3dVertexBuffers[0] = 0;
	}

	return;
}


bool UIClass::UpdateBuffers(ID3D11DeviceContext* deviceContext, float positionX, float positionY)
{
	float left, right, top, bottom;
	VertexType* vertices;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	VertexType* verticesPtr;
	HRESULT result;


	// If the position we are rendering this bitmap to has not changed then don't update the vertex buffer since it
	// currently has the correct parameters.
	if((positionX == m_previousPosX) && (positionY == m_previousPosY))
	{
		return true;
	}

	// If it has changed then update the position it is being rendered to.
	m_previousPosX = positionX;
	m_previousPosY = positionY;

	// Calculate the screen coordinates of the left side of the bitmap.
	left = (float)((m_screenWidth / 2) * -1) + (float)positionX;

	// Calculate the screen coordinates of the right side of the bitmap.
	right = left + (float)m_bitmapWidth;

	// Calculate the screen coordinates of the top of the bitmap.
	top = (float)(m_screenHeight / 2) - (float)positionY;

	// Calculate the screen coordinates of the bottom of the bitmap.
	bottom = top - (float)m_bitmapHeight;

	// Create the vertex array.
	vertices = new VertexType[m_nVertices];
	if(!vertices)
	{
		return false;
	}

	// Load the vertex array with data.
	// First triangle.
	vertices[0].position = D3DXVECTOR3(left, top, 0.0f);  // Top left.
	vertices[0].texture = D3DXVECTOR2(0.0f, 0.0f);

	vertices[1].position = D3DXVECTOR3(right, bottom, 0.0f);  // Bottom right.
	vertices[1].texture = D3DXVECTOR2(1.0f, 1.0f);

	vertices[2].position = D3DXVECTOR3(left, bottom, 0.0f);  // Bottom left.
	vertices[2].texture = D3DXVECTOR2(0.0f, 1.0f);

	// Second triangle.
	vertices[3].position = D3DXVECTOR3(left, top, 0.0f);  // Top left.
	vertices[3].texture = D3DXVECTOR2(0.0f, 0.0f);

	vertices[4].position = D3DXVECTOR3(right, top, 0.0f);  // Top right.
	vertices[4].texture = D3DXVECTOR2(1.0f, 0.0f);

	vertices[5].position = D3DXVECTOR3(right, bottom, 0.0f);  // Bottom right.
	vertices[5].texture = D3DXVECTOR2(1.0f, 1.0f);

	// Lock the vertex buffer so it can be written to.
	result = deviceContext->Map(m_ppd3dVertexBuffers[0], 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if(FAILED(result))
	{
		return false;
	}

	// Get a pointer to the data in the vertex buffer.
	verticesPtr = (VertexType*)mappedResource.pData;

	// Copy the data into the vertex buffer.
	memcpy(verticesPtr, (void*)vertices, (sizeof(VertexType) * m_nVertices));

	// Unlock the vertex buffer.
	deviceContext->Unmap(m_ppd3dVertexBuffers[0], 0);

	// Release the vertex array as it is no longer needed.
	delete [] vertices;
	vertices = 0;

	return true;
}


bool UIClass::LoadTexture(ID3D11Device* device, WCHAR* filename)
{
	bool result;

	
	// Create the texture object.
	m_pTexture = new CTextureclass;
	if(!m_pTexture)
	{
		return false;
	}

	// Initialize the texture object.
	result = m_pTexture->Initialize(device, filename);
	if(!result)
	{
		return false;
	}

	return true;
}


void UIClass::ReleaseTexture()
{
	// Release the texture object.
	if(m_pTexture)
	{
		m_pTexture->Release();
		delete m_pTexture;
		m_pTexture = 0;
	}

	return;
}

void UIClass::SetTexture(ID3D11Device* device , WCHAR* textureFilename)
{
	// Load the texture for this bitmap.
	if(m_pTexture)
	{
		m_pTexture->Release();
		delete m_pTexture;
		m_pTexture = 0;
	}

	LoadTexture(device, textureFilename);

}