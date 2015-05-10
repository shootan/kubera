#include "stdafx.h"
#include "ST.h"

#define ROOTCOUNT 57
class ArrangeData
{
private:
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

	int RootTurn1[40];
	int RootTurn2[40];
	int RootTurn3[40];
	int RootTurn4[40];

	int MinionCount;
	

	int MinionID[160];

	float m_fWalkSpeed;

	

public:
	ArrangeData();
	~ArrangeData();

	MinionInfo MI1[40];
	MinionInfo MI2[40];
	MinionInfo MI3[40];
	MinionInfo MI4[40];

	MinionInfo MI[160];

	bool m_bMinionLive1;
	bool m_bMinionLive2;
	bool m_bMinionLive3;
	bool m_bMinionLive4;

	void SetRoot();
	void SetMinionPosition(float _dt);
	void RegenMinion();
	void SetTime(float _time);

	void SetRoot1();
	void SetRoot2();
	void SetRoot3();
	void SetRoot4();
	
	void CheckMinionLive();

	int SetID();	
	
};