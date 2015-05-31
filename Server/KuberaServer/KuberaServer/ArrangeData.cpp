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

	ZeroMemory(Root1, sizeof(Vector3)*ROOTCOUNT);
	ZeroMemory(Root2, sizeof(Vector3)*ROOTCOUNT);
	ZeroMemory(Root3, sizeof(Vector3)*ROOTCOUNT);
	ZeroMemory(Root4, sizeof(Vector3)*ROOTCOUNT);

	int size = sizeof(MinionInfo);

	ZeroMemory(&RootTurn1, sizeof(int)*160);
	ZeroMemory(&RootTurn2, sizeof(int)*40);
	ZeroMemory(&RootTurn3, sizeof(int)*40);
	ZeroMemory(&RootTurn4, sizeof(int)*40);

	ZeroMemory(MinionID, sizeof(int)*160);

	MinionCount = 0;

	ZeroMemory(&m_bMinion, sizeof(Minion)*160);
	ZeroMemory(&MI, sizeof(MinionInfo)*160);
	

	m_bMinionLive1 = false;
	m_bMinionLive2 =false;
	m_bMinionLive3 = false;
	m_bMinionLive4 =false;

	m_fWalkSpeed = 25.0f;

	m_fRegenTime = 0.0f;
	m_fUnitTime = 0.0f;
}

ArrangeData::~ArrangeData()
{

}

void ArrangeData::SetRoot()
{
	this->SetRoot1();
	this->SetRoot2();
	this->SetRoot3();
	this->SetRoot4();
}

void ArrangeData::RegenMinion()
{
	//printf("%.1f \n", m_fRegenTime);
	if(m_fUnitTime < 1.0f) return;

	for(int i=0; i<160; i++)
	{
		if(m_bMinion[i].m_Live != FALSE) continue;
		m_bMinion[i].m_Live = TRUE;
		m_bMinion[i].m_Root = 1;
		m_bMinion[i].m_Pos = Root1[RootTurn1[i]];

		RootTurn1[i] += 1;
		m_bMinion[i].m_TargetPos = Root1[RootTurn1[i]];

		MI[i].m_Live = TRUE;
		MI[i].m_ID = i+5;

		break;
	}

	for(int i=0; i<160; i++)
	{
		if(m_bMinion[i].m_Live != FALSE) continue;
		m_bMinion[i].m_Live = TRUE;
		m_bMinion[i].m_Root = 2;
		m_bMinion[i].m_Pos = Root2[RootTurn2[i]];

		RootTurn2[i] += 1;
		m_bMinion[i].m_TargetPos = Root2[RootTurn2[i]];

		MI[i].m_Live = TRUE;
		MI[i].m_ID = i+5;

		break;
	}

	for(int i=0; i<160; i++)
	{
		if(m_bMinion[i].m_Live != FALSE) continue;
		m_bMinion[i].m_Live = TRUE;
		m_bMinion[i].m_Root = 3;
		m_bMinion[i].m_Pos = Root3[RootTurn3[i]];

		RootTurn3[i] += 1;
		m_bMinion[i].m_TargetPos = Root3[RootTurn3[i]];

		MI[i].m_Live = TRUE;
		MI[i].m_ID = i+5;

		break;
	}

	for(int i=0; i<160; i++)
	{
		if(m_bMinion[i].m_Live != FALSE) continue;
		m_bMinion[i].m_Live = TRUE;
		m_bMinion[i].m_Root = 4;
		m_bMinion[i].m_Pos = Root4[RootTurn4[i]];

		RootTurn4[i] += 1;
		m_bMinion[i].m_TargetPos = Root4[RootTurn4[i]];

		MI[i].m_Live = TRUE;
		MI[i].m_ID = i+5;

		break;
	}


	/*for(int i=0; i<40; i++)
	{
		if(Minion1[i].m_Live != FALSE) continue;
		Minion1[i].m_Live = TRUE;
		Minion1[i].m_Pos = Root1[RootTurn1[i]];
		
		RootTurn1[i] += 1;
		Minion1[i].m_TargetPos = Root1[RootTurn1[i]];
		
		MI1[i].m_Live = TRUE;
		MI1[i].m_ID = i;

		break;
	}

	for(int i=0; i<40; i++)
	{
		if(Minion2[i].m_Live != FALSE) continue;
		Minion2[i].m_Live = TRUE;
		Minion2[i].m_Pos = Root2[RootTurn2[i]];

		RootTurn2[i] += 1;
		Minion2[i].m_TargetPos = Root2[RootTurn2[i]];

		MI2[i].m_Live = TRUE;
		MI2[i].m_ID = i;

		break;
	}

	for(int i=0; i<40; i++)
	{
		if(Minion3[i].m_Live != FALSE) continue;
		Minion3[i].m_Live = TRUE;
		Minion3[i].m_Pos = Root3[RootTurn3[i]];

		RootTurn3[i] += 1;
		Minion3[i].m_TargetPos = Root3[RootTurn3[i]];

		MI3[i].m_Live = TRUE;
		MI3[i].m_ID = i;

		break;
	}

	for(int i=0; i<40; i++)
	{
		if(Minion4[i].m_Live != FALSE) continue;
		Minion4[i].m_Live = TRUE;
		Minion4[i].m_Pos = Root4[RootTurn4[i]];

		RootTurn4[i] += 1;
		Minion4[i].m_TargetPos = Root4[RootTurn4[i]];

		MI4[i].m_Live = TRUE;
		MI4[i].m_ID = i;

		break;
	}*/

	MinionCount++;
	m_fUnitTime = 0.0f;
	if(MinionCount > 3)
	{
		m_fRegenTime = 0.0f;
		MinionCount = 0;
	}
}

void ArrangeData::SetMinionPosition(float _dt)
{

	for(int i=0; i<160; i++)
	{
		if(m_bMinion[i].m_Live != TRUE) continue;
		float distance = ST::sharedManager()->GetDistance(m_bMinion[i].m_TargetPos, m_bMinion[i].m_Pos);
		switch(m_bMinion[i].m_Root)
		{
		case 1:
			if (distance > 1.0f)
			{ 
				D3DXVECTOR3 m_vWalkIncrement;
				m_vWalkIncrement.x = m_bMinion[i].m_TargetPos.x - m_bMinion[i].m_Pos.x;
				m_vWalkIncrement.y = m_bMinion[i].m_TargetPos.y - m_bMinion[i].m_Pos.y;
				m_vWalkIncrement.z = m_bMinion[i].m_TargetPos.z - m_bMinion[i].m_Pos.z;
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

				m_vWalkIncrement *= m_fWalkSpeed;  

				m_vWalkIncrement*= _dt;
				m_bMinion[i].m_Pos.x += m_vWalkIncrement.x;
				m_bMinion[i].m_Pos.y += m_vWalkIncrement.y;
				m_bMinion[i].m_Pos.z += m_vWalkIncrement.z;

				MI[i].m_Pos = m_bMinion[i].m_Pos;

				//printf("미니언 : %d , X: %.2f, Y: %.2f, Z:%.2f \n", i, m_bMinion[i].m_Pos.x,m_bMinion[i].m_Pos.y,m_bMinion[i].m_Pos.z);
			}
			else
			{
				RootTurn1[i]++;
				if(RootTurn1[i] > 56)
				{
					RootTurn1[i] = 0;
					m_bMinion[i].m_Pos = Root1[RootTurn1[i]];
				}
				m_bMinion[i].m_TargetPos = Root1[RootTurn1[i]];
			}
			break;

		case 2:
			if (distance > 1.0f)
			{ 
				D3DXVECTOR3 m_vWalkIncrement;
				m_vWalkIncrement.x = m_bMinion[i].m_TargetPos.x - m_bMinion[i].m_Pos.x;
				m_vWalkIncrement.y = m_bMinion[i].m_TargetPos.y - m_bMinion[i].m_Pos.y;
				m_vWalkIncrement.z = m_bMinion[i].m_TargetPos.z - m_bMinion[i].m_Pos.z;
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

				m_vWalkIncrement *= m_fWalkSpeed;  

				m_vWalkIncrement*= _dt;
				m_bMinion[i].m_Pos.x += m_vWalkIncrement.x;
				m_bMinion[i].m_Pos.y += m_vWalkIncrement.y;
				m_bMinion[i].m_Pos.z += m_vWalkIncrement.z;

				MI[i].m_Pos = m_bMinion[i].m_Pos;

				//printf("미니언 : %d , X: %.2f, Y: %.2f, Z:%.2f \n", i, m_bMinion[i].m_Pos.x,m_bMinion[i].m_Pos.y,m_bMinion[i].m_Pos.z);
			}
			else
			{
				RootTurn1[i]++;
				if(RootTurn1[i] > 56)
				{
						RootTurn1[i] = 0;
						m_bMinion[i].m_Pos = Root2[RootTurn1[i]];
				}
				m_bMinion[i].m_TargetPos = Root2[RootTurn1[i]];
			}
			break;
		case 3:
			if (distance > 1.0f)
			{ 
				D3DXVECTOR3 m_vWalkIncrement;
				m_vWalkIncrement.x = m_bMinion[i].m_TargetPos.x - m_bMinion[i].m_Pos.x;
				m_vWalkIncrement.y = m_bMinion[i].m_TargetPos.y - m_bMinion[i].m_Pos.y;
				m_vWalkIncrement.z = m_bMinion[i].m_TargetPos.z - m_bMinion[i].m_Pos.z;
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

				m_vWalkIncrement *= m_fWalkSpeed; 

				m_vWalkIncrement*= _dt;
				m_bMinion[i].m_Pos.x += m_vWalkIncrement.x;
				m_bMinion[i].m_Pos.y += m_vWalkIncrement.y;
				m_bMinion[i].m_Pos.z += m_vWalkIncrement.z;

				MI[i].m_Pos = m_bMinion[i].m_Pos;

				//printf("미니언 : %d , X: %.2f, Y: %.2f, Z:%.2f \n", i, m_bMinion[i].m_Pos.x,m_bMinion[i].m_Pos.y,m_bMinion[i].m_Pos.z);
			}
			else
			{
				RootTurn1[i]++;
				if(RootTurn1[i] > 56)
				{
					RootTurn1[i] = 0;
					m_bMinion[i].m_Pos = Root3[RootTurn1[i]];
				}
				m_bMinion[i].m_TargetPos = Root3[RootTurn1[i]];
			}
			break;
		case 4:
			if (distance > 1.0f)
			{ 
				D3DXVECTOR3 m_vWalkIncrement;
				m_vWalkIncrement.x = m_bMinion[i].m_TargetPos.x - m_bMinion[i].m_Pos.x;
				m_vWalkIncrement.y = m_bMinion[i].m_TargetPos.y - m_bMinion[i].m_Pos.y;
				m_vWalkIncrement.z = m_bMinion[i].m_TargetPos.z - m_bMinion[i].m_Pos.z;
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

				m_vWalkIncrement *= m_fWalkSpeed;  

				m_vWalkIncrement*= _dt;
				m_bMinion[i].m_Pos.x += m_vWalkIncrement.x;
				m_bMinion[i].m_Pos.y += m_vWalkIncrement.y;
				m_bMinion[i].m_Pos.z += m_vWalkIncrement.z;

				MI[i].m_Pos = m_bMinion[i].m_Pos;

				//printf("미니언 : %d , X: %.2f, Y: %.2f, Z:%.2f \n", i, m_bMinion[i].m_Pos.x,m_bMinion[i].m_Pos.y,m_bMinion[i].m_Pos.z);
			}
			else
			{
				RootTurn1[i]++;
				if(RootTurn1[i] > 56)
				{
					RootTurn1[i] = 0;
					m_bMinion[i].m_Pos = Root4[RootTurn1[i]];
				}
				m_bMinion[i].m_TargetPos = Root4[RootTurn1[i]];
			}
			break;
		}
		
	}

	/////////40마리
//	for(int i=0; i<40; i++)
//	{
//		if(Minion1[i].m_Live != TRUE) continue;
//		float distance = ST::sharedManager()->GetDistance(Minion1[i].m_TargetPos, Minion1[i].m_Pos);
//		if (distance > 1.0f)
//		{ 
//			D3DXVECTOR3 m_vWalkIncrement;
//			m_vWalkIncrement.x = Minion1[i].m_TargetPos.x - Minion1[i].m_Pos.x;
//			m_vWalkIncrement.y = Minion1[i].m_TargetPos.y - Minion1[i].m_Pos.y;
//			m_vWalkIncrement.z = Minion1[i].m_TargetPos.z - Minion1[i].m_Pos.z;
//			D3DXVec3Normalize ( &m_vWalkIncrement, &m_vWalkIncrement );
//			
//			//float fAngle = D3DXVec3Dot( &m_vWalkIncrement, &m_vFacingDirection );
//// 			D3DXVECTOR3 cross;
//// 			D3DXVec3Cross( &cross, &m_vWalkIncrement, &m_vFacingDirection );
//// 			fAngle = acosf( fAngle );
//// 			if ( cross.y >  0.0f ) {
//// 				fAngle *=-1.0f;
//// 			}
//// 			fAngle /= D3DX_PI;
//// 			this->SetRotation(2, 1/fAngle);
//
//			m_vWalkIncrement.x *= m_fWalkSpeed; 
//			m_vWalkIncrement.y *= m_fWalkSpeed; 
//			m_vWalkIncrement.z *= m_fWalkSpeed; 
//
//			m_vWalkIncrement*= _dt;
//			Minion1[i].m_Pos.x += m_vWalkIncrement.x;
//			Minion1[i].m_Pos.y += m_vWalkIncrement.y;
//			Minion1[i].m_Pos.z += m_vWalkIncrement.z;
//
//			MI1[i].m_Pos = Minion1[i].m_Pos;
//
//			//printf("미니언 : %d , X: %.2f, Y: %.2f, Z:%.2f \n", i, Minion1[i].m_Pos.x,Minion1[i].m_Pos.y,Minion1[i].m_Pos.z);
//		}
//		else
//		{
//			RootTurn1[i]++;
//			Minion1[i].m_TargetPos = Root1[RootTurn1[i]];
//		}
//	}
//
//	for(int i=0; i<40; i++)
//	{
//		if(Minion2[i].m_Live != TRUE) continue;
//		float distance = ST::sharedManager()->GetDistance(Minion2[i].m_TargetPos, Minion2[i].m_Pos);
//		if (distance > 1.0f)
//		{ 
//			D3DXVECTOR3 m_vWalkIncrement;
//			m_vWalkIncrement.x = Minion2[i].m_TargetPos.x - Minion2[i].m_Pos.x;
//			m_vWalkIncrement.y = Minion2[i].m_TargetPos.y - Minion2[i].m_Pos.y;
//			m_vWalkIncrement.z = Minion2[i].m_TargetPos.z - Minion2[i].m_Pos.z;
//			D3DXVec3Normalize ( &m_vWalkIncrement, &m_vWalkIncrement );
//
//			//float fAngle = D3DXVec3Dot( &m_vWalkIncrement, &m_vFacingDirection );
//			// 			D3DXVECTOR3 cross;
//			// 			D3DXVec3Cross( &cross, &m_vWalkIncrement, &m_vFacingDirection );
//			// 			fAngle = acosf( fAngle );
//			// 			if ( cross.y >  0.0f ) {
//			// 				fAngle *=-1.0f;
//			// 			}
//			// 			fAngle /= D3DX_PI;
//			// 			this->SetRotation(2, 1/fAngle);
//
//			m_vWalkIncrement.x *= m_fWalkSpeed; 
//			m_vWalkIncrement.y *= m_fWalkSpeed; 
//			m_vWalkIncrement.z *= m_fWalkSpeed; 
//
//			m_vWalkIncrement*= _dt;
//			Minion2[i].m_Pos.x += m_vWalkIncrement.x;
//			Minion2[i].m_Pos.y += m_vWalkIncrement.y;
//			Minion2[i].m_Pos.z += m_vWalkIncrement.z;
//
//			MI2[i].m_Pos = Minion2[i].m_Pos;
//
//			//printf("미니언 : %d , X: %.2f, Z:%.2f \n", i, Minion2[i].m_Pos.x,Minion2[i].m_Pos.z);
//		}
//		else
//		{
//			RootTurn2[i]++;
//			Minion2[i].m_TargetPos = Root2[RootTurn2[i]];
//		}
//	}
//
//	for(int i=0; i<40; i++)
//	{
//		if(Minion3[i].m_Live != TRUE) continue;
//		float distance = ST::sharedManager()->GetDistance(Minion3[i].m_TargetPos, Minion3[i].m_Pos);
//		if (distance > 1.0f)
//		{ 
//			D3DXVECTOR3 m_vWalkIncrement;
//			m_vWalkIncrement.x = Minion3[i].m_TargetPos.x - Minion3[i].m_Pos.x;
//			m_vWalkIncrement.y = Minion3[i].m_TargetPos.y - Minion3[i].m_Pos.y;
//			m_vWalkIncrement.z = Minion3[i].m_TargetPos.z - Minion3[i].m_Pos.z;
//			D3DXVec3Normalize ( &m_vWalkIncrement, &m_vWalkIncrement );
//
//			//float fAngle = D3DXVec3Dot( &m_vWalkIncrement, &m_vFacingDirection );
//			// 			D3DXVECTOR3 cross;
//			// 			D3DXVec3Cross( &cross, &m_vWalkIncrement, &m_vFacingDirection );
//			// 			fAngle = acosf( fAngle );
//			// 			if ( cross.y >  0.0f ) {
//			// 				fAngle *=-1.0f;
//			// 			}
//			// 			fAngle /= D3DX_PI;
//			// 			this->SetRotation(2, 1/fAngle);
//
//			m_vWalkIncrement.x *= m_fWalkSpeed; 
//			m_vWalkIncrement.y *= m_fWalkSpeed; 
//			m_vWalkIncrement.z *= m_fWalkSpeed; 
//
//			m_vWalkIncrement*= _dt;
//			Minion3[i].m_Pos.x += m_vWalkIncrement.x;
//			Minion3[i].m_Pos.y += m_vWalkIncrement.y;
//			Minion3[i].m_Pos.z += m_vWalkIncrement.z;
//
//			MI3[i].m_Pos = Minion3[i].m_Pos;
//
//			//printf("미니언 : %d , X: %.2f, Z:%.2f \n", i, Minion3[i].m_Pos.x,Minion3[i].m_Pos.z);
//		}
//		else
//		{
//			RootTurn3[i]++;
//			Minion3[i].m_TargetPos = Root3[RootTurn3[i]];
//		}
//	}
//
//	for(int i=0; i<40; i++)
//	{
//		if(Minion4[i].m_Live != TRUE) continue;
//		float distance = ST::sharedManager()->GetDistance(Minion4[i].m_TargetPos, Minion4[i].m_Pos);
//		if (distance > 1.0f)
//		{ 
//			D3DXVECTOR3 m_vWalkIncrement;
//			m_vWalkIncrement.x = Minion4[i].m_TargetPos.x - Minion4[i].m_Pos.x;
//			m_vWalkIncrement.y = Minion4[i].m_TargetPos.y - Minion4[i].m_Pos.y;
//			m_vWalkIncrement.z = Minion4[i].m_TargetPos.z - Minion4[i].m_Pos.z;
//			D3DXVec3Normalize ( &m_vWalkIncrement, &m_vWalkIncrement );
//
//			//float fAngle = D3DXVec3Dot( &m_vWalkIncrement, &m_vFacingDirection );
//			// 			D3DXVECTOR3 cross;
//			// 			D3DXVec3Cross( &cross, &m_vWalkIncrement, &m_vFacingDirection );
//			// 			fAngle = acosf( fAngle );
//			// 			if ( cross.y >  0.0f ) {
//			// 				fAngle *=-1.0f;
//			// 			}
//			// 			fAngle /= D3DX_PI;
//			// 			this->SetRotation(2, 1/fAngle);
//
//			m_vWalkIncrement.x *= m_fWalkSpeed; 
//			m_vWalkIncrement.y *= m_fWalkSpeed; 
//			m_vWalkIncrement.z *= m_fWalkSpeed; 
//
//			m_vWalkIncrement*= _dt;
//			Minion4[i].m_Pos.x += m_vWalkIncrement.x;
//			Minion4[i].m_Pos.y += m_vWalkIncrement.y;
//			Minion4[i].m_Pos.z += m_vWalkIncrement.z;
//
//			MI4[i].m_Pos = Minion4[i].m_Pos;
//
//			//printf("미니언 : %d , X: %.2f, Z:%.2f \n", i, Minion4[i].m_Pos.x,Minion4[i].m_Pos.z);
//		}
//		else
//		{
//			RootTurn4[i]++;
//			Minion4[i].m_TargetPos = Root4[RootTurn4[i]];
//		}
//	}
}

void ArrangeData::SetTime(float _time)
{
	m_fRegenTime += _time;
	if(m_fRegenTime > 80.0f)
		m_fUnitTime += _time;
	//printf("%.1f \n", m_fRegenTime);
}

void ArrangeData::SetRoot1()
{
	Root1[0].x = 485;
	Root1[0].z = 20;
	for(int i=1; i<7; i++)
	{
		Root1[i].x = 440;
		Root1[i].z = 20 + (i-1)*34;
	}

	for(int i=7; i<51; i++)
	{
		Root1[i].x = 440 - (i-6)*20;
		Root1[i].z = 190;
	}

	for(int i=51; i<56; i++)
	{	
		Root1[i].x = -440;
		Root1[i].z = 190 - (i-50) * 34;
	}

	Root1[56].x = -485;
	Root1[56].z = 20;
}
void ArrangeData::SetRoot2()
{
	Root2[0].x = -485;
	Root2[0].z = 20;
	
	for(int i=1; i<7; i++)
	{
		Root2[i].x = -440;
		Root2[i].z = 20 + (i-1)*34;
	}

	for(int i=7; i<51; i++)
	{
		Root2[i].x = -440 + (i-6)*20;
		Root2[i].z = 190;
	}

	for(int i=51; i<56; i++)
	{	
		Root2[i].x = 440;
		Root2[i].z = 190 - (i-50) * 34;
	}

	Root2[56].x = 485;
	Root2[56].z = 20;
}
void ArrangeData::SetRoot3()
{
	Root3[0].x = 485;
	Root3[0].z = -20;

	for(int i=1; i<7; i++)
	{
		Root3[i].x = 440;
		Root3[i].z = -20 - (i-1)*34;
	}

	for(int i=7; i<51; i++)
	{
		Root3[i].x = 440 - (i-6)*20;
		Root3[i].z = -190;
	}

	for(int i=51; i<56; i++)
	{	
		Root3[i].x = -440;
		Root3[i].z = -190 + (i-50) * 34;
	}

	Root3[56].x = -485;
	Root3[56].z = -20;
	
}
void ArrangeData::SetRoot4()
{
	Root4[0].x = -485;
	Root4[0].z = -20;

	for(int i=1; i<7; i++)
	{
		Root4[i].x = -440;
		Root4[i].z = -20 - (i-1)*34;
	}

	for(int i=7; i<51; i++)
	{
		Root4[i].x = -440 + (i-6)*20;
		Root4[i].z = -190;
	}

	for(int i=51; i<56; i++)
	{	
		Root4[i].x = 440;
		Root4[i].z = -190 + (i-50) * 34;
	}

	Root4[56].x = 485;
	Root4[56].z = -20;
}

void ArrangeData::CheckMinionLive()
{

	m_bMinionLive1 = false;
	m_bMinionLive2 =false;
	m_bMinionLive3 = false;
	m_bMinionLive4 =false;

	for(int i=0; i<40; i++)
	{
		if(m_bMinionLive1 == false)
		{
			if(MI1[i].m_Live == TRUE) m_bMinionLive1 = true;
		}

		if(m_bMinionLive2 == false)
		{
			if(MI2[i].m_Live == TRUE) m_bMinionLive2 = true;
		}

		if(m_bMinionLive3 == false)
		{
			if(MI3[i].m_Live == TRUE) m_bMinionLive3 = true;
		}

		if(m_bMinionLive4 == false)
		{
			if(MI4[i].m_Live == TRUE) m_bMinionLive4 = true;
		}
	}
}