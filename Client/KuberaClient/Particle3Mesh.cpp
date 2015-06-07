#include "Particle3Mesh.h"

Particle3Mesh::Particle3Mesh(ID3D11Device *pd3dDevice) : CMesh(pd3dDevice)
{
	m_Texture = 0;

	m_vertices = 0;

	m_nVertices = 0;
	m_nIndices = 0;
	m_dxgiIndexFormat = DXGI_FORMAT_R32_UINT;

	m_pnVertexStrides = new UINT[1];
	m_pnVertexStrides[0] = sizeof(VertexType);
	m_pnVertexOffsets = new UINT[1];
	m_pnVertexOffsets[0] = 0;
	m_nVertexSlot = 0;
	m_nVertexBuffers = 1;
	m_ppd3dVertexBuffers = new ID3D11Buffer*[m_nVertexBuffers];


	CreateRasterizerState(pd3dDevice);


	m_fCreateInterval = 0.8f;
	m_fAccumulatedTime = 0.f;
	m_fLastAddedTime = 0.f;
	m_vDir = D3DXVECTOR3(1, 0, 0);
	m_fTTL = 0.6f;

	m_vCamRight = D3DXVECTOR3(0, 0, 0);
	m_vCamUp = D3DXVECTOR3(0, 0, 0);
}

Particle3Mesh::~Particle3Mesh(void)
{
}

void Particle3Mesh::Render(ID3D11DeviceContext *pd3dDeviceContext)
{

	pd3dDeviceContext->PSSetShaderResources(0, 1, &m_Texture->m_texture);

	UpdateBuffers(pd3dDeviceContext);
	CMesh::Render(pd3dDeviceContext);
}

void Particle3Mesh::CreateRasterizerState(ID3D11Device *pd3dDevice)
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
	d3dRasterizerDesc.CullMode = D3D11_CULL_NONE;
	d3dRasterizerDesc.FillMode = D3D11_FILL_SOLID;
	pd3dDevice->CreateRasterizerState(&d3dRasterizerDesc, &m_pd3dRasterizerState);
}


void Particle3Mesh::RenderInstanced(ID3D11DeviceContext *pd3dDeviceContext, int nInstances, int nStartInstance)
{
	//pd3dDeviceContext->PSSetShaderResources(0, 1, &m_pTexture->m_texture );
	CMesh::RenderInstanced(pd3dDeviceContext, nInstances, nStartInstance);
}

bool Particle3Mesh::Initialize(ID3D11Device* pd3dDevice, WCHAR* textureFilename)
{
	bool result;

	// Load the textures for this model.
	result = LoadTexture(pd3dDevice, textureFilename);
	if(!result)
	{
		return false;
	}

	result = InitializeParticleSystem();
	if(!result)
	{
		return false;
	}

	// Initialize the vertex and index buffers.
	result = InitializeBuffers(pd3dDevice);
	if(!result)
	{
		return false;
	}

	return true;
}

bool Particle3Mesh::InitializeBuffers(ID3D11Device* pd3dDevice)
{
	unsigned long* indices;
	int i;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;


	// Set the maximum number of vertices in the vertex array.
	m_nVertices = 100 * 6;

	// Set the maximum number of indices in the index array.
	m_nIndices = m_nVertices;

	// Create the vertex array for the particles that will be rendered.
	m_vertices = new VertexType[m_nVertices];
	if(!m_vertices)
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
	memset(m_vertices, 0, (sizeof(VertexType) * m_nVertices));

	// Initialize the index array.
	for(i=0; i<m_nIndices; i++)
	{
		indices[i] = i;
	}

	// Set up the description of the dynamic vertex buffer.
	vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_nVertices;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the vertex data.
	vertexData.pSysMem = m_vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// Now finally create the vertex buffer.
	result = pd3dDevice->CreateBuffer(&vertexBufferDesc, &vertexData, &m_ppd3dVertexBuffers[0]);
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
	result = pd3dDevice->CreateBuffer(&indexBufferDesc, &indexData, &m_pd3dIndexBuffer);
	if(FAILED(result))
	{
		return false;
	}

	// Release the index array since it is no longer needed.
	delete [] indices;
	indices = 0;

	return true;
}

bool Particle3Mesh::LoadTexture(ID3D11Device* pd3dDevice, WCHAR* filename)
{
	bool result;


	// Create the texture object.
	m_Texture = new CTextureclass;
	if(!m_Texture)
	{
		return false;
	}

	// Initialize the texture object.
	result = m_Texture->Initialize(pd3dDevice, filename);
	if(!result)
	{
		return false;
	}

	return true;
}

void Particle3Mesh::ReleaseTexture()
{
	if(m_Texture)
	{
		m_Texture->Release();
		delete m_Texture;
		m_Texture = 0;
	}

	return;
}



ID3D11ShaderResourceView* Particle3Mesh::GetTexture()
{
	return m_Texture->GetTexture();
}


int Particle3Mesh::GetIndexCount()
{
	return m_nIndices;
}

bool Particle3Mesh::InitializeParticleSystem()
{
	//int i;

	//// Set the physical size of the particles.
	//m_particleSize = 10.0f;//0.2f;

	//m_particleVelocity = 1.0f;
	//m_particleVelocityVariation = 0.2f;

	//// Set the number of particles to emit per second.
	//m_particlesPerSecond = 5000.0f;

	//// Set the maximum number of particles allowed in the particle system.
	//m_maxParticles = 100;

	//// Create the particle list.
	//m_particleList = new ParticleType[m_maxParticles];
	//if(!m_particleList)
	//{
	//	return false;
	//}

	//// Initialize the particle list.
	//for(i=0; i<m_maxParticles; i++)
	//{
	//	m_particleList[i].active = false;
	//}

	//// Initialize the current particle count to zero since none are emitted yet.
	//m_currentParticleCount = 0;

	//// Clear the initial accumulated time for the particle per second emission rate.
	//m_accumulatedTime = 0.0f;

	return true;
}

void Particle3Mesh::EmitParticles(float frameTime)
{
	if (m_fLastAddedTime > 0 && m_fAccumulatedTime < m_fLastAddedTime + m_fCreateInterval)
		return;

	m_fLastAddedTime = m_fAccumulatedTime;

	ParticleType particle;
	//particle.Active(*pPos, m_vDir, D3DXVECTOR2(m_fEX, m_fEY) , m_fTTL, m_bFadeout);

	particle.m_vPos = D3DXVECTOR3(0, 0, 0);
	particle.m_vDir = m_vDir;
	particle.m_fTTL = m_fTTL;
	particle.m_vScale = D3DXVECTOR2(10, 10);
	particle.m_bFlag = true;
	particle.m_fFadeFactor = 0.f;
	particle.m_fTime = 0.f;

	m_particles.push_back(particle);

	return;
}


void Particle3Mesh::UpdateParticles(float frameTime)
{
	int i;


	// Each frame we update all the particles by making them move downwards using their position, velocity, and the frame time.
	//for(i=0; i<m_currentParticleCount; i++)
	//{
	//	m_particleList[i].positionY = m_particleList[i].positionY - (m_particleList[i].velocity * frameTime * 10);
	//	//m_particleList[i].positionX += frameTime * 20;
	//	m_particleList[i].m_fTime += frameTime;
	//	m_particleList[i].m_fFadeFactor = min(max(1 - m_particleList[i].m_fTime / 0.6f, 0), 1);
	//}


	return;
}


void Particle3Mesh::KillParticles()
{

	std::list<ParticleType>::iterator it = m_particles.begin();

	while(it != m_particles.end())
	{
		if((*it).m_bFlag == false)
		{
			it = m_particles.erase(it);
		}
		else
			it++;
	}
}

bool Particle3Mesh::UpdateBuffers(ID3D11DeviceContext* deviceContext)
{
	int index, i;
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	VertexType* verticesPtr;
	float colorR = (((float)rand()-(float)rand())/RAND_MAX) + 0.5f;
	float colorG = (((float)rand()-(float)rand())/RAND_MAX) + 0.5f;
	float colorB = (((float)rand()-(float)rand())/RAND_MAX) + 0.5f;

	// Initialize vertex array to zeros at first.
	memset(m_vertices, 0, (sizeof(VertexType) * m_nVertices));

	// Now build the vertex array from the particle list array.  Each particle is a quad made out of two triangles.
	index = 0;
	std::list<ParticleType>::iterator it;

	for(it = m_particles.begin(); it != m_particles.end(); it++)
	{
		// Bottom left.
		m_vertices[index].position = D3DXVECTOR3((*it).m_vPos.x - (*it).m_vScale.x , (*it).m_vPos.y - (*it).m_vScale.y, (*it).m_vPos.z) - m_vCamRight - m_vCamUp;
		m_vertices[index].texture = D3DXVECTOR2(0.0f, 1.0f);
		m_vertices[index].color = D3DXVECTOR4(colorR, colorG, colorB, (*it).m_fFadeFactor*225.0f);
		//m_vertices[index].color = D3DXVECTOR4((*it).m_fFadeFactor*225.0f, (*it).m_fFadeFactor*225.0f, (*it).m_fFadeFactor*225.0f, (*it).m_fFadeFactor*225.0f);
		index++;

		// Top left.
		m_vertices[index].position = D3DXVECTOR3((*it).m_vPos.x - (*it).m_vScale.x , (*it).m_vPos.y + (*it).m_vScale.y, (*it).m_vPos.z) - m_vCamRight + m_vCamUp;
		m_vertices[index].texture = D3DXVECTOR2(0.0f, 0.0f);
		m_vertices[index].color = D3DXVECTOR4(colorR, colorG, colorB, (*it).m_fFadeFactor*225.0f);
		//m_vertices[index].color = D3DXVECTOR4((*it).m_fFadeFactor*225.0f, (*it).m_fFadeFactor*225.0f, (*it).m_fFadeFactor*225.0f, (*it).m_fFadeFactor*225.0f);
		index++;

		// Bottom right.
		m_vertices[index].position = D3DXVECTOR3((*it).m_vPos.x + (*it).m_vScale.x , (*it).m_vPos.y - (*it).m_vScale.y, (*it).m_vPos.z) + m_vCamRight - m_vCamUp;
		m_vertices[index].texture = D3DXVECTOR2(1.0f, 1.0f);
		m_vertices[index].color = D3DXVECTOR4(colorR, colorG, colorB, (*it).m_fFadeFactor*225.0f);
		//m_vertices[index].color = D3DXVECTOR4((*it).m_fFadeFactor*225.0f, (*it).m_fFadeFactor*225.0f, (*it).m_fFadeFactor*225.0f, (*it).m_fFadeFactor*225.0f);
		index++;

		// Bottom right.
		m_vertices[index].position = D3DXVECTOR3((*it).m_vPos.x + (*it).m_vScale.x , (*it).m_vPos.y - (*it).m_vScale.y, (*it).m_vPos.z) + m_vCamRight - m_vCamUp;
		m_vertices[index].texture = D3DXVECTOR2(1.0f, 1.0f);
		m_vertices[index].color = D3DXVECTOR4(colorR, colorG, colorB, (*it).m_fFadeFactor*225.0f);
		//m_vertices[index].color = D3DXVECTOR4((*it).m_fFadeFactor*225.0f, (*it).m_fFadeFactor*225.0f, (*it).m_fFadeFactor*225.0f, (*it).m_fFadeFactor*225.0f);
		index++;
		// Top left.
		m_vertices[index].position = D3DXVECTOR3((*it).m_vPos.x - (*it).m_vScale.x , (*it).m_vPos.y + (*it).m_vScale.y, (*it).m_vPos.z) - m_vCamRight + m_vCamUp;
		m_vertices[index].texture = D3DXVECTOR2(0.0f, 0.0f);
		m_vertices[index].color = D3DXVECTOR4(colorR, colorG, colorB, (*it).m_fFadeFactor*225.0f);
		//m_vertices[index].color = D3DXVECTOR4((*it).m_fFadeFactor*225.0f, (*it).m_fFadeFactor*225.0f, (*it).m_fFadeFactor*225.0f, (*it).m_fFadeFactor*225.0f);
		index++;
		// Top right.
		m_vertices[index].position = D3DXVECTOR3((*it).m_vPos.x + (*it).m_vScale.x , (*it).m_vPos.y + (*it).m_vScale.y, (*it).m_vPos.z) + m_vCamRight + m_vCamUp;
		m_vertices[index].texture = D3DXVECTOR2(1.0f, 0.0f);
		m_vertices[index].color = D3DXVECTOR4(colorR, colorG, colorB, (*it).m_fFadeFactor*225.0f);
		//m_vertices[index].color = D3DXVECTOR4((*it).m_fFadeFactor*225.0f, (*it).m_fFadeFactor*225.0f, (*it).m_fFadeFactor*225.0f, (*it).m_fFadeFactor*225.0f);
		index++;
	}



	// Lock the vertex buffer.
	result = deviceContext->Map(m_ppd3dVertexBuffers[0], 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if(FAILED(result))
	{
		return false;
	}

	// Get a pointer to the data in the vertex buffer.
	verticesPtr = (VertexType*)mappedResource.pData;

	// Copy the data into the vertex buffer.
	memcpy(verticesPtr, (void*)m_vertices, (sizeof(VertexType) * m_nVertices));

	// Unlock the vertex buffer.
	deviceContext->Unmap(m_ppd3dVertexBuffers[0], 0);

	return true;
}

void Particle3Mesh::Frame(float frameTime)
{
	//bool result;

	m_fAccumulatedTime += frameTime;

	std::list<ParticleType>::iterator it;

	for(it = m_particles.begin(); it != m_particles.end(); it++)
	{
		if((*it).m_bFlag)
		{
			if( (*it).m_fTime >= m_fTTL)
			{
				(*it).m_bFlag = false;
			}

			(*it).m_fFadeFactor = min(max(1 - (*it).m_fTime / m_fTTL, 0), 1);

			(*it).m_fTime += frameTime;
			//(*it).m_vPos += m_vDir * frameTime * 30;
		}
	}	

	// Release old particles.
	KillParticles();

	// Emit new particles.
	EmitParticles(frameTime);

	// Update the position of the particles.
	//UpdateParticles(frameTime);

}


void Particle3Mesh::SetCamVec(D3DXVECTOR3 _vCamRight, D3DXVECTOR3 _vCamUp)
{
	m_vCamRight = _vCamRight;
	m_vCamUp = _vCamUp;
}