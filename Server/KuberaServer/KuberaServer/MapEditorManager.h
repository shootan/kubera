#pragma once
#include "stdafx.h"
#include <vector>

using namespace std;
#define MAX_MINION 160

class MapEditorManager
{
public:
	vector<Information> BlueTower;
	vector<Information> RedTower;
	vector<Information> Nexus;

	vector<Information>::iterator iterList;

	int RockSize;
	int Rock2Size;
	int TreeSize;
	int TowerSize;

private:
	MapEditorManager()
	{
		
	}
public:
	~MapEditorManager()
	{
		delete instance;	
	}
public:
	static MapEditorManager* instance;
	static MapEditorManager* sharedManager()
	{
		if(instance == NULL) 
			instance = new MapEditorManager;
		return instance;
	}
public:

	void LoadMapData()
	{
		FILE *fp;
		fopen_s(&fp, "Map.txt", "r");

		fscanf_s(fp, "%d", &TowerSize);

		for(int i=0; i<TowerSize; i++)
		{
			Information Info;
			fscanf_s(fp, "%d", &Info.team);
			fscanf_s(fp, "%f", &Info.x);
			fscanf_s(fp, "%f", &Info.z);
			if(Info.team == 1)
			{
				RedTower.push_back(Info);
			}
			if(Info.team == 2)
			{
				BlueTower.push_back(Info);
			}
		}

		int q;

		fscanf_s(fp, "%d", &q);

		Information Info;
		fscanf_s(fp, "%d", &Info.team);
		fscanf_s(fp, "%f", &Info.x);
		fscanf_s(fp, "%f", &Info.z);
		Nexus.push_back(Info);

		fscanf_s(fp, "%d", &Info.team);
		fscanf_s(fp, "%f", &Info.x);
		fscanf_s(fp, "%f", &Info.z);
		Nexus.push_back(Info);

		fclose(fp);
	}

	Vector3 GetRedNexusPos()
	{
		Vector3 p;
		ZeroMemory(&p, sizeof(Vector3));
		for(iterList = Nexus.begin(); iterList!= Nexus.end(); iterList++)
		{
			if(iterList->team != 1)
				continue;
			p.x = iterList->x;
			p.z = iterList->z;
		}
		return p;
	}

	Vector3 GetBlueNexusPos()
	{
		Vector3 p;
		ZeroMemory(&p, sizeof(Vector3));
		for(iterList = Nexus.begin(); iterList!= Nexus.end(); iterList++)
		{
			if(iterList->team != 2)
				continue;
			p.x = iterList->x;
			p.z = iterList->z;
		}
		return p;
	}

	int GetRockSize(){
		return RockSize;
	}
	int GetRock2Size()
	{
		return Rock2Size;
	}
	int GetTreeSize()
	{
		return TreeSize;
	}
	int GetRedTowerSize()
	{
		return RedTower.size();
	}

	int GetBlueTowerSize()
	{
		return BlueTower.size();
	}

	

	Information GetRedTowerData(int i)
	{
		return RedTower.at(i);
	}

	Information GetBlueTowerData(int i)
	{
		return BlueTower.at(i);
	}
};