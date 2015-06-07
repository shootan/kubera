#pragma once
#include "header.h"
#include "ParticleObject.h"
#include "Mesh.h"

#define MAX_PARTICLE 256

class ParticleManager
{
public:
	ParticleObject* m_pParticle[MAX_PARTICLE];

private:
	ParticleManager()
	{
		memset(&m_pParticle,NULL,sizeof(m_pParticle));
	}
public:
	~ParticleManager()
	{
		for(int i=0; i<MAX_PARTICLE; i++)
			m_pParticle[i]->Release();

		delete instance;
	}
public:
	static ParticleManager* instance;
	static ParticleManager* sharedManager()
	{
		if(instance == NULL) 
			instance = new ParticleManager;
		return instance;
	}
public:
	void CreateParticle(D3DXVECTOR3 pos, CMesh* mesh, int _type)
	{
		for(int i=0; i<MAX_PARTICLE; i++)
		{
			if(m_pParticle[i] != NULL) continue;
			m_pParticle[i] = new ParticleObject();
			m_pParticle[i]->SetMesh(mesh);
			m_pParticle[i]->SetPosition(pos);
			m_pParticle[i]->SetType(_type);
			break;
		}
	}
};