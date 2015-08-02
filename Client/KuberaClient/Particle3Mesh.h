//#pragma once
//
//#include "Mesh.h"
//#include <fstream>
//#include <List>
//using namespace std;
//
//
//class Particle3Mesh : public CMesh
//{
//private:
//	struct ParticleType
//	{
//		float		m_fFadeFactor;  //투명도
//		float		m_fTime; //경과시간
//
//		bool		m_bFlag;
//		float		m_fTTL;
//		D3DXVECTOR3	m_vPos;
//		D3DXVECTOR3	m_vDir;
//		D3DXVECTOR2	m_vScale;
//	};
//
//	struct VertexType
//	{
//		D3DXVECTOR3 position;
//		D3DXVECTOR2 texture;
//		D3DXVECTOR4 color;
//	};
//
//	CTextureclass *m_Texture;
//	//ParticleType* m_particleList;
//	std::list<ParticleType> m_particles;
//	VertexType* m_vertices;
//
//
//	FLOAT	m_fCreateInterval;
//	FLOAT	m_fAccumulatedTime;
//	FLOAT	m_fLastAddedTime;
//	D3DXVECTOR3	m_vDir;
//	FLOAT	m_fTTL;
//	D3DXVECTOR2	m_vScale;
//
//	D3DXVECTOR3 m_vCamRight;
//	D3DXVECTOR3 m_vCamUp;
//
//public:
//	Particle3Mesh(ID3D11Device *pd3dDevice);
//	virtual ~Particle3Mesh();
//
//	virtual void Render(ID3D11DeviceContext *pd3dDeviceContext);
//	virtual void CreateRasterizerState(ID3D11Device *pd3dDevice);
//	virtual void RenderInstanced(ID3D11DeviceContext *pd3dDeviceContext, int nInstances=0, int nStartInstance=0);
//	virtual bool LoadTexture(ID3D11Device*, WCHAR*);
//	virtual void ReleaseTexture();
//	virtual void Frame(float frametime);
//	virtual void SetScale(D3DXVECTOR2 _scale){ m_vScale = _scale; } 
//
//	bool Initialize(ID3D11Device*, WCHAR*);
//	bool InitializeBuffers(ID3D11Device*);
//
//	bool InitializeParticleSystem();
//	void EmitParticles(float);
//	void UpdateParticles(float);
//	void KillParticles();
//	bool UpdateBuffers(ID3D11DeviceContext*);
//	
//	//bool Frame(float frameTime, ID3D11DeviceContext* deviceContext);
//
//	ID3D11ShaderResourceView* GetTexture();
//
//	int GetIndexCount();
//
//	void SetCamVec(D3DXVECTOR3 _vCamRight, D3DXVECTOR3 _vCamUp);
//};