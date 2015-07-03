#include "Particle2Mesh.h"

Particle2Mesh::Particle2Mesh(ID3D11Device *pd3dDevice) : CMesh(pd3dDevice)
{
	m_Texture = 0;

	m_particleList = 0;
	m_vertices = 0;

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


	m_fCreateInterval = 0.1f;

	m_vCamRight = D3DXVECTOR3(0, 0, 0);
	m_vCamUp = D3DXVECTOR3(0, 0, 0);
}

Particle2Mesh::~Particle2Mesh(void)
{
}

void Particle2Mesh::Render(ID3D11DeviceContext *pd3dDeviceContext)
{

	pd3dDeviceContext->PSSetShaderResources(0, 1, &m_Texture->m_texture);

	UpdateBuffers(pd3dDeviceContext);
	CMesh::Render(pd3dDeviceContext);
}

void Particle2Mesh::CreateRasterizerState(ID3D11Device *pd3dDevice)
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


void Particle2Mesh::RenderInstanced(ID3D11DeviceContext *pd3dDeviceContext, int nInstances, int nStartInstance)
{
	//pd3dDeviceContext->PSSetShaderResources(0, 1, &m_pTexture->m_texture );
	CMesh::RenderInstanced(pd3dDeviceContext, nInstances, nStartInstance);
}

bool Particle2Mesh::Initialize(ID3D11Device* pd3dDevice, WCHAR* textureFilename)
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

bool Particle2Mesh::InitializeBuffers(ID3D11Device* pd3dDevice)
{
	unsigned long* indices;
	int i;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;


	// Set the maximum number of vertices in the vertex array.
	m_nVertices = m_maxParticles * 6;

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

bool Particle2Mesh::LoadTexture(ID3D11Device* pd3dDevice, WCHAR* filename)
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

void Particle2Mesh::ReleaseTexture()
{
	if(m_Texture)
	{
		m_Texture->Release();
		delete m_Texture;
		m_Texture = 0;
	}

	return;
}



ID3D11ShaderResourceView* Particle2Mesh::GetTexture()
{
	return m_Texture->GetTexture();
}


int Particle2Mesh::GetIndexCount()
{
	return m_nIndices;
}

bool Particle2Mesh::InitializeParticleSystem()
{
	int i;

	// Set the physical size of the particles.
	m_particleSize = 10.0f;//0.2f;

	m_particleVelocity = 1.0f;
	m_particleVelocityVariation = 0.2f;

	// Set the number of particles to emit per second.
	m_particlesPerSecond = 5000.0f;

	// Set the maximum number of particles allowed in the particle system.
	m_maxParticles = 100;

	// Create the particle list.
	m_particleList = new ParticleType[m_maxParticles];
	if(!m_particleList)
	{
		return false;
	}

	// Initialize the particle list.
	for(i=0; i<m_maxParticles; i++)
	{
		m_particleList[i].active = false;
	}

	// Initialize the current particle count to zero since none are emitted yet.
	m_currentParticleCount = 0;

	// Clear the initial accumulated time for the particle per second emission rate.
	m_accumulatedTime = 0.0f;

	return true;
}

void Particle2Mesh::EmitParticles(float frameTime)
{
	bool emitParticle, found;
	float positionX, positionY, positionZ, velocity, red, green, blue;
	int index, i, j;


	// Increment the frame time.
	m_accumulatedTime += frameTime;

	// Set emit particle to false for now.
	emitParticle = false;

	// Check if it is time to emit a new particle or not.
	if(m_accumulatedTime >  0.1f)
	{
		m_accumulatedTime = 0.0f;
		emitParticle = true;
	}

	// If there are particles to emit then emit one per frame.
	if((emitParticle == true) && (m_currentParticleCount < (m_maxParticles - 1)))
	{
		m_currentParticleCount++;

		// Now generate the randomized particle properties.
		positionX = 0;
		positionY = 0;
		positionZ = 0;

		velocity = m_particleVelocity + (((float)rand()-(float)rand())/RAND_MAX) * m_particleVelocityVariation;

		red   = (((float)rand()-(float)rand())/RAND_MAX) + 0.5f;
		green = (((float)rand()-(float)rand())/RAND_MAX) + 0.5f;
		blue  = (((float)rand()-(float)rand())/RAND_MAX) + 0.5f;

		// Now since the particles need to be rendered from back to front for blending we have to sort the particle array.
		// We will sort using Z depth so we need to find where in the list the particle should be inserted.
		index = 0;
		found = false;
		while(!found)
		{
			if((m_particleList[index].active == false) || (m_particleList[index].positionZ < positionZ))
			{
				found = true;
			}
			else
			{
				index++;
			}
		}

		// Now that we know the location to insert into we need to copy the array over by one position from the index to make room for the new particle.
		i = m_currentParticleCount;
		j = i - 1;

		while(i != index)
		{
			m_particleList[i].positionX = m_particleList[j].positionX;
			m_particleList[i].positionY = m_particleList[j].positionY;
			m_particleList[i].positionZ = m_particleList[j].positionZ;
			m_particleList[i].red       = m_particleList[j].red;
			m_particleList[i].green     = m_particleList[j].green;
			m_particleList[i].blue      = m_particleList[j].blue;
			m_particleList[i].active    = m_particleList[j].active;
			m_particleList[i].velocity  = m_particleList[j].velocity;
			m_particleList[i].m_fTime   = m_particleList[i].m_fTime;
			m_particleList[i].m_fFadeFactor   = m_particleList[i].m_fFadeFactor;
			i--;
			j--;
		}

		// Now insert it into the particle array in the correct depth order.
		m_particleList[index].positionX = positionX;
		m_particleList[index].positionY = positionY;
		m_particleList[index].positionZ = positionZ;
		m_particleList[index].red       = red;
		m_particleList[index].green     = green;
		m_particleList[index].blue      = blue;
		m_particleList[index].velocity  = velocity;
		m_particleList[index].active    = true;
		m_particleList[index].m_fTime		= 0.f;
		m_particleList[index].m_fFadeFactor = 0.f;
	}

	return;
}


void Particle2Mesh::UpdateParticles(float frameTime)
{
	int i;


	// Each frame we update all the particles by making them move downwards using their position, velocity, and the frame time.
	for(i=0; i<m_currentParticleCount; i++)
	{
		m_particleList[i].positionY = m_particleList[i].positionY - (m_particleList[i].velocity * frameTime * 10);
		//m_particleList[i].positionX += frameTime * 20;
		m_particleList[i].m_fTime += frameTime;
		m_particleList[i].m_fFadeFactor = min(max(1 - m_particleList[i].m_fTime / 0.6f, 0), 1);
	}

	return;
}


void Particle2Mesh::KillParticles()
{
	int i, j;

	// Kill all the particles that have gone below a certain height range.
	for(i=0; i<m_maxParticles; i++)
	{
		if((m_particleList[i].active == true) && (m_particleList[i].m_fTime < 0.6f))
		{
			m_particleList[i].m_fTime = 0.f;
			m_particleList[i].m_fFadeFactor = 0.f;
			m_particleList[i].active = false;
			m_currentParticleCount--;

			// Now shift all the live particles back up the array to erase the destroyed particle and keep the array sorted correctly.
			for(j=i; j<m_maxParticles-1; j++)
			{
				m_particleList[j].positionX = m_particleList[j+1].positionX;
				m_particleList[j].positionY = m_particleList[j+1].positionY;
				m_particleList[j].positionZ = m_particleList[j+1].positionZ;
				m_particleList[j].red       = m_particleList[j+1].red;
				m_particleList[j].green     = m_particleList[j+1].green;
				m_particleList[j].blue      = m_particleList[j+1].blue;
				m_particleList[j].velocity  = m_particleList[j+1].velocity;
				m_particleList[j].active    = m_particleList[j+1].active;
				m_particleList[i].m_fTime   = m_particleList[j+1].m_fTime;
				m_particleList[i].m_fFadeFactor   = m_particleList[j+1].m_fFadeFactor;
			}
		}
	}

	return;
}

bool Particle2Mesh::UpdateBuffers(ID3D11DeviceContext* deviceContext)
{
	int index, i;
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	VertexType* verticesPtr;


	// Initialize vertex array to zeros at first.
	memset(m_vertices, 0, (sizeof(VertexType) * m_nVertices));

	// Now build the vertex array from the particle list array.  Each particle is a quad made out of two triangles.
	index = 0;

	for(i=0; i<m_currentParticleCount; i++)
	{
		// Bottom left.
		m_vertices[index].position = D3DXVECTOR3(m_particleList[i].positionX - m_particleSize, m_particleList[i].positionY - m_particleSize, m_particleList[i].positionZ) - m_vCamRight - m_vCamUp;
		m_vertices[index].texture = D3DXVECTOR2(0.0f, 1.0f);
		//m_vertices[index].color = D3DXVECTOR4(m_particleList[i].red, m_particleList[i].green, m_particleList[i].blue, 1.0f);
		m_vertices[index].color = D3DXVECTOR4(m_particleList[i].red, m_particleList[i].green, m_particleList[i].blue, m_particleList[i].m_fFadeFactor*255.0f);
		index++;

		// Top left.
		m_vertices[index].position = D3DXVECTOR3(m_particleList[i].positionX - m_particleSize, m_particleList[i].positionY + m_particleSize, m_particleList[i].positionZ) - m_vCamRight + m_vCamUp;
		m_vertices[index].texture = D3DXVECTOR2(0.0f, 0.0f);
		//m_vertices[index].color = D3DXVECTOR4(m_particleList[i].red, m_particleList[i].green, m_particleList[i].blue, 1.0f);
		m_vertices[index].color = D3DXVECTOR4(m_particleList[i].red, m_particleList[i].green, m_particleList[i].blue, m_particleList[i].m_fFadeFactor*255.0f);
		index++;

		// Bottom right.
		m_vertices[index].position = D3DXVECTOR3(m_particleList[i].positionX + m_particleSize, m_particleList[i].positionY - m_particleSize, m_particleList[i].positionZ) + m_vCamRight - m_vCamUp;
		m_vertices[index].texture = D3DXVECTOR2(1.0f, 1.0f);
		//m_vertices[index].color = D3DXVECTOR4(m_particleList[i].red, m_particleList[i].green, m_particleList[i].blue, 1.0f);
		m_vertices[index].color = D3DXVECTOR4(m_particleList[i].red, m_particleList[i].green, m_particleList[i].blue, m_particleList[i].m_fFadeFactor*255.0f);
		index++;

		// Bottom right.
		m_vertices[index].position = D3DXVECTOR3(m_particleList[i].positionX + m_particleSize, m_particleList[i].positionY - m_particleSize, m_particleList[i].positionZ) + m_vCamRight - m_vCamUp;
		m_vertices[index].texture = D3DXVECTOR2(1.0f, 1.0f);
		//m_vertices[index].color = D3DXVECTOR4(m_particleList[i].red, m_particleList[i].green, m_particleList[i].blue, 1.0f);
		m_vertices[index].color = D3DXVECTOR4(m_particleList[i].red, m_particleList[i].green, m_particleList[i].blue, m_particleList[i].m_fFadeFactor*255.0f);
		index++;

		// Top left.
		m_vertices[index].position = D3DXVECTOR3(m_particleList[i].positionX - m_particleSize, m_particleList[i].positionY + m_particleSize, m_particleList[i].positionZ) - m_vCamRight + m_vCamUp;
		m_vertices[index].texture = D3DXVECTOR2(0.0f, 0.0f);
		//m_vertices[index].color = D3DXVECTOR4(m_particleList[i].red, m_particleList[i].green, m_particleList[i].blue, 1.0f);
		m_vertices[index].color = D3DXVECTOR4(m_particleList[i].red, m_particleList[i].green, m_particleList[i].blue, m_particleList[i].m_fFadeFactor*255.0f);
		index++;

		// Top right.
		m_vertices[index].position = D3DXVECTOR3(m_particleList[i].positionX + m_particleSize, m_particleList[i].positionY + m_particleSize, m_particleList[i].positionZ) + m_vCamRight + m_vCamUp;
		m_vertices[index].texture = D3DXVECTOR2(1.0f, 0.0f);
		//m_vertices[index].color = D3DXVECTOR4(m_particleList[i].red, m_particleList[i].green, m_particleList[i].blue, 1.0f);
		m_vertices[index].color = D3DXVECTOR4(m_particleList[i].red, m_particleList[i].green, m_particleList[i].blue, m_particleList[i].m_fFadeFactor*255.0f);
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

void Particle2Mesh::Frame(float frameTime)
{
	//bool result;


	// Release old particles.
	KillParticles();

	// Emit new particles.
	EmitParticles(frameTime);

	// Update the position of the particles.
	UpdateParticles(frameTime);

}


void Particle2Mesh::SetCamVec(D3DXVECTOR3 _vCamRight, D3DXVECTOR3 _vCamUp)
{
	m_vCamRight = _vCamRight;
	m_vCamUp = _vCamUp;
}