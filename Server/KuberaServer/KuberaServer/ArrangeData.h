#pragma once
#include "stdafx.h"
#include "ST.h"



#define ROOTCOUNT 57
class ArrangeData
{
private:
	// ¾È¾¸
	Vector3 Root1[ROOTCOUNT];
	Vector3 Root2[ROOTCOUNT];
	Vector3 Root3[ROOTCOUNT];
	Vector3 Root4[ROOTCOUNT];

	Minion Minion1[40];
	Minion Minion2[40];
	Minion Minion3[40];
	Minion Minion4[40];

	Minion m_bMinion[160];

	float m_fRegenTime;
	float m_fUnitTime;

	int RootTurn1[160];
	int RootTurn2[40];
	int RootTurn3[40];
	int RootTurn4[40];

	int MinionCount;
	int MinionID[160];

	float m_fWalkSpeed;

	bool m_bMinionLive1;
	bool m_bMinionLive2;
	bool m_bMinionLive3;
	bool m_bMinionLive4;

	MinionInfo MI1[40];
	MinionInfo MI2[40];
	MinionInfo MI3[40];
	MinionInfo MI4[40];

	MinionInfo MI[160];

	Player* Playerlist;

	void SetRoot();
	void SetMinionPosition(float _dt);
	void RegenMinion();
	

	void SetRoot1();
	void SetRoot2();
	void SetRoot3();
	void SetRoot4();

	void CheckMinionLive();

	//ObjectInfo

	ObjectInfo MonsterInfo[6];
	//ObjectInfo TowerInfo
	
	

public:
	ArrangeData();
	~ArrangeData();

	
	void SetTime(float _time, IOBuffer* _list);
	int SetID();
	void UpdateMonster();
	void SetObjectInfo();
	void AttackData(IOBuffer* _list);
};