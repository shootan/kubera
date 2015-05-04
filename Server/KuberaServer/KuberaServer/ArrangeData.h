#include "stdafx.h"
#include "ST.h"

class ArrangeData
{
private:
	Vector3 Root1[13];
	Vector3 Root2[13];
	Vector3 Root3[13];
	Vector3 Root4[13];

	Minion Minion1[40];
	Minion Minion2[40];
	Minion Minion3[40];
	Minion Minion4[40];

	

	float m_fRegenTime;

	int RootTurn1[40];
	int RootTurn2[40];
	int RootTurn3[40];
	int RootTurn4[40];

	int MinionCount1;
	int MinionCount2;
	int MinionCount3;
	int MinionCount4;

	int MinionID[160];

	float m_fWalkSpeed;

public:
	ArrangeData();
	~ArrangeData();

	MinionInfo MI1[40];
	MinionInfo MI2[40];
	MinionInfo MI3[40];
	MinionInfo MI4[40];

	void SetRoot();
	void SetMinionPosition(float _dt);
	void RegenMinion();
	void SetTime(float _time);

	int SetID();
	
	
	
	
};