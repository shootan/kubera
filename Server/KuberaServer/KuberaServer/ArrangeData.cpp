#include "ArrangeData.h"

ArrangeData::ArrangeData()
{
	ZeroMemory(&Minion1, sizeof(Minion) * 40);
	ZeroMemory(&Minion2, sizeof(Minion) * 40);
	ZeroMemory(&Minion3, sizeof(Minion) * 40);
	ZeroMemory(&Minion4, sizeof(Minion) * 40);

	ZeroMemory(&MI1, sizeof(MinionInfo) * 40);
	ZeroMemory(&MI2, sizeof(MinionInfo) * 40);
	ZeroMemory(&MI3, sizeof(MinionInfo) * 40);
	ZeroMemory(&MI4, sizeof(MinionInfo) * 40);

	ZeroMemory(Root1, sizeof(Vector3)*13);
	ZeroMemory(Root2, sizeof(Vector3)*13);
	ZeroMemory(Root3, sizeof(Vector3)*13);
	ZeroMemory(Root4, sizeof(Vector3)*13);

	int size = sizeof(MinionInfo);

	m_fRegenTime = 0.0f;

	ZeroMemory(&RootTurn1, sizeof(int)*40);
	ZeroMemory(&RootTurn2, sizeof(int)*40);
	ZeroMemory(&RootTurn3, sizeof(int)*40);
	ZeroMemory(&RootTurn4, sizeof(int)*40);

	ZeroMemory(MinionID, sizeof(int)*160);

	MinionCount1 = 0;
	MinionCount2 = 0;
	MinionCount3 = 0;
	MinionCount4 = 0;

	m_fWalkSpeed = 10.0f;
}

ArrangeData::~ArrangeData()
{

}

void ArrangeData::SetRoot()
{
	for(int i=0; i<14; i++)
	{
		Root1[i].y = 0;
		Root2[i].y = 0;
		Root3[i].y = 0;
		Root4[i].y = 0;
	}

	Root1[0].x = 546;
	Root1[1].x = 446;
	Root1[2].x = 446;
	Root1[3].x = 436;
	Root1[4].x = 326;
	Root1[5].x = 245;
	Root1[6].x = 158;
	Root1[7].x = 0;
	Root1[8].x = -31;
	Root1[9].x = -230;
	Root1[10].x = -350;
	Root1[11].x = -437;
	Root1[12].x = -446;

	Root1[0].z = -14;
	Root1[1].z = 7;
	Root1[2].z = 88;
	Root1[3].z = 196;
	Root1[4].z = 193;
	Root1[5].z = 188;
	Root1[6].z = 188;
	Root1[7].z = 188;
	Root1[8].z = 188;
	Root1[9].z = 188;
	Root1[10].z = 188;
	Root1[11].z = 188;
	Root1[12].z = 188;
}

void ArrangeData::RegenMinion()
{
	if(m_fRegenTime < 30.0f) return;

	for(int i=0; i<40; i++)
	{
		if(Minion1[i].m_Live != FALSE) continue;
		Minion1[i].m_Live = TRUE;
		Minion1[i].m_Pos = Root1[RootTurn1[i]];
		
		RootTurn1[i] += 1;
		Minion1[i].m_TargetPos = Root1[RootTurn1[i]];
		MinionCount1++;

		MI1[i].m_Live = TRUE;
		MI1[i].m_ID = i;

		if(MinionCount1 > 3) 
		{
			MinionCount1 = 0;
			break;
		}
	}

	m_fRegenTime = 0.0f;
}

void ArrangeData::SetMinionPosition(float _dt)
{
	for(int i=0; i<40; i++)
	{
		if(Minion1[i].m_Live != TRUE) continue;
		float distance = ST::sharedManager()->GetDistance(Minion1[i].m_TargetPos, Minion1[i].m_Pos);
		if (distance > 1.0f)
		{ 
			D3DXVECTOR3 m_vWalkIncrement;
			m_vWalkIncrement.x = Minion1[i].m_TargetPos.x - Minion1[i].m_Pos.x;
			m_vWalkIncrement.y = Minion1[i].m_TargetPos.y - Minion1[i].m_Pos.y;
			m_vWalkIncrement.z = Minion1[i].m_TargetPos.z - Minion1[i].m_Pos.z;
			D3DXVec3Normalize ( &m_vWalkIncrement, &m_vWalkIncrement );
			
			//float fAngle = D3DXVec3Dot( &m_vWalkIncrement, &m_vFacingDirection );
// 			D3DXVECTOR3 cross;
// 			D3DXVec3Cross( &cross, &m_vWalkIncrement, &m_vFacingDirection );
// 			fAngle = acosf( fAngle );
// 			if ( cross.y >  0.0f ) {
// 				fAngle *=-1.0f;
// 			}
// 			fAngle /= D3DX_PI;
// 			this->SetRotation(2, 1/fAngle);

			m_vWalkIncrement.x *= m_fWalkSpeed; 
			m_vWalkIncrement.y *= m_fWalkSpeed; 
			m_vWalkIncrement.z *= m_fWalkSpeed; 

			m_vWalkIncrement*= _dt;
			Minion1[i].m_Pos.x += m_vWalkIncrement.x;
			Minion1[i].m_Pos.y += m_vWalkIncrement.y;
			Minion1[i].m_Pos.z += m_vWalkIncrement.z;

			MI1[i].m_Pos = Minion1[i].m_Pos;

			//printf("¹Ì´Ï¾ð : %d , X: %.2f, Y: %.2f, Z:%.2f \n", i, Minion1[i].m_Pos.x,Minion1[i].m_Pos.y,Minion1[i].m_Pos.z);
		}
		else
		{
			RootTurn1[i]++;
			Minion1[i].m_TargetPos = Root1[RootTurn1[i]];
		}
	}
}

void ArrangeData::SetTime(float _time)
{
	m_fRegenTime += _time;
	//printf("%.4f \n", m_fRegenTime);
}