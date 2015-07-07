#pragma once

#include "Mesh.h"
#include <fstream>
using namespace std;


class ParticleMesh : public CMesh
{
private:
	struct ParticleType
	{
		float positionX, positionY, positionZ;
		float red, green, blue;
		float velocity;
		bool active;
	};

	struct VertexType
	{
		D3DXVECTOR3 position;
		D3DXVECTOR2 texture;
		D3DXVECTOR4 color;
	};

	CTextureclass *m_Texture;
	ParticleType* m_particleList;
	VertexType* m_vertices;

	float m_particleDeviationX, m_particleDeviationY, m_particleDeviationZ;
	float m_particleVelocity, m_particleVelocityVariation;
	float m_particleSize, m_particlesPerSecond;
	int m_maxParticles;

	int m_currentParticleCount;
	float m_accumulatedTime;


	D3DXVECTOR3 m_vCamRight;
	D3DXVECTOR3 m_vCamUp;

public:
	ParticleMesh(ID3D11Device *pd3dDevice);
	virtual ~ParticleMesh();

	virtual void Render(ID3D11DeviceContext *pd3dDeviceContext);
	virtual void CreateRasterizerState(ID3D11Device *pd3dDevice);
	virtual void RenderInstanced(ID3D11DeviceContext *pd3dDeviceContext, int nInstances=0, int nStartInstance=0);
	virtual bool LoadTexture(ID3D11Device*, WCHAR*);
	virtual void ReleaseTexture();
	virtual void Frame(float frametime);

	bool Initialize(ID3D11Device*, WCHAR*);
	bool InitializeBuffers(ID3D11Device*);

	bool InitializeParticleSystem();
	void EmitParticles(float);
	void UpdateParticles(float);
	void KillParticles();
	bool UpdateBuffers(ID3D11DeviceContext*);
	//bool Frame(float frameTime, ID3D11DeviceContext* deviceContext);

	ID3D11ShaderResourceView* GetTexture();
	
	int GetIndexCount();

	void SetCamVec(D3DXVECTOR3 _vCamRight, D3DXVECTOR3 _vCamUp);
};