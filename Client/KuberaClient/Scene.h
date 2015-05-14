#pragma once
#include "header.h"
#include "GameObject.h"
#include "MinionObject.h"
#include "HeroObject.h"
#include "ObstacleObject.h"
#include "Shader.h"
#include "ControlManager.h"
#include "TowerObject.h"
#include "ST.h"
#include "OtherPlayerObject.h"
#include "TowerManager.h"
#include "OtherPlayerManager.h"

class CScene
{
private:

// 	MinionObject* m_pMinion1[MAX_MINION];
// 	MinionObject* m_pMinion2[MAX_MINION];
// 	MinionObject* m_pMinion3[MAX_MINION];
// 	MinionObject* m_pMinion4[MAX_MINION];

	MinionObject* m_pMinion[MAX_MINION*4];

	CObjectShader *m_pObjectShaders;
	CInstancingShader *m_pInstancingShaders;
	int m_nShaders;

	int m_nObjects;
	int m_nIntanceObjects;

	int m_MousePosX;
	int m_MousePosY;

	BOOL m_bRbutton;
	BOOL m_bLbutton;

	CFBXMesh *pHeroMesh;
	CFBXMesh *pMinionDragonMesh;

	float m_fMinionRespawnTime;
	float m_fMissileAttackTime;
	int m_nMinionObjects;

	CGameObject *m_pPlane;

	float m_DistanceToHero;
	float m_DistanceToOtherPlayer;
	CGameObject *pBoundBox[2];
	CGameObject *m_pBlueNexus;
	CGameObject *m_pRedNexus;

public:
	CScene(void);
	~CScene(void);

	CModelViewerCamera* m_Camera;
	ControlManager		m_Control;

	bool OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	bool OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);

	void BuildObjects(ID3D11Device *pd3dDevice);
	void ReleaseObjects();

	bool ProcessInput();
	void AnimateObjects(float fTimeElapsed, ID3D11Device *pd3dDevice);
	void Render(ID3D11DeviceContext *pd3dDeviceContext);

	int GetMousePosX();
	int GetMousePosY();

	void AddOtherPlayer(ID3D11Device *pd3dDevice);
	void AddMinion(ID3D11Device *pd3dDevice);

	//
	BOOL CheckCollisionAABBAxis( float c1, float w1, float c2, float w2);
	BOOL CheckCollision(D3DXVECTOR3 c1, float w1, float h1, float d1, D3DXVECTOR3 c2, float w2, float h2, float d2);
	void GameCollision(CGameObject* obj1, CGameObject* obj2);

	CGameObject* GetObject(int num);
	BOOL m_bJoinOtherPlayer;
	BOOL m_bJoin;

	// net

	void SetOtherClient(PlayerStruct* _PI, int _Count);
	void UpdateOtherClient(PlayerStruct* _PI, int _Count);

	void SetMinionInfo(MinionInfo* _MI);
	void OtherPlayerTargetSetting();
};