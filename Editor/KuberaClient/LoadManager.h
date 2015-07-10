#pragma once
#include "header.h"
#include "GameObject.h"
#include "HeroObject.h"
#include "ObstacleObject.h"
#include "Shader.h"
#include "ControlManager.h"
#include "TowerObject.h"
#include "ST.h"
#include "OtherPlayerObject.h"
#include "TowerManager.h"
#include "OtherPlayerManager.h"
#include "ParticleManager.h"
#include "FBX\GFBXMeshLoader.h"
#include "ParticleMesh.h"
#include "Particle2Mesh.h"
#include "Particle3Mesh.h"
#include "Camera.h"
#include "MapEditorManager.h"

class LoadManager
{
public:

private:
	LoadManager()
	{

	}
public:
	~LoadManager()
	{
		delete instance;	
	}
public:
	static LoadManager* instance;
	static LoadManager* sharedManager()
	{
		if(instance == NULL) 
			instance = new LoadManager;
		return instance;
	}
public:
	void LoadObject()
	{

	}
	

};