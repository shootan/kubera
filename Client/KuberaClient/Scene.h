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
#include "Camera.h"
#include "Light.h"
#include "UIObject.h"
#include "UIClass.h"

#define MAX_UI 31

class CScene
{
private:

	BOOL m_bFinishSound;

	CObjectShader *m_pObjectShaders;
	CInstancingShader *m_pInstancingShaders;
	CAnimationShader *m_pAnimationShaders;
	CParticleShader *m_pParticleShaders;
	int m_nShaders;

	int m_nObjects;
	int m_nIntanceObjects;
	int m_nAnimationObjects;

	int m_MousePosX;
	int m_MousePosY;

	BOOL m_bRbutton;
	BOOL m_bLbutton;

	
	GFBX::Mesh *m_pWarriorMesh;
	GFBX::Mesh *m_pWizardMesh;

	//ParticleMesh *m_pParticleMesh;
	//Particle3Mesh *m_pParticle2Mesh;
	//Particle3Mesh *m_pParticle3Mesh;


	float m_fMissileAttackTime;


	CGameObject *m_pPlane;

	float m_DistanceToHero;
	float m_DistanceToOtherPlayer;

	CGameObject *m_pNexusBox[2];
	CGameObject *m_pTowerBox[MAX_TOWER];
	CGameObject *m_pPlayerBox[2];
	CGameObject *m_pMinionBox[MAX_MINION];

	CGameObject *m_pBlueNexus;
	CGameObject *m_pRedNexus;
	CGameObject *m_pDestroyNexus;

	BOOL m_bGameOver;


	//조명
	LIGHTS *m_pLights;
	ID3D11Buffer *m_pd3dcbLights;

	//UI
	UIClass* m_pUI[MAX_UI];
	CUIShader *m_pUIShaders;
	UIObject* m_pUIObjects[MAX_UI];

	float m_UIskillbarWidth, m_UIskillbarHeight; //스킬바 UI
	float m_UIMinimapWidth, m_UIMinimapHeight;   //미니맵 UI
	float m_UIInfoWidth, m_UIInfoHeight;			//캐릭터 정보창 UI
	float m_UIScoreWidth, m_UIScoreHeight;		//점수판 UI
	float m_UITargetInfoWidth, m_UITargetInfoHeight;			//타겟 정보창 UI

	float m_SwordWidth, m_SwordHeight; //특성 아이콘
	float m_ShielWidth, m_ShielHeight;
	float m_BootsWidth, m_BootsHeight;

	float m_HpbarRWidth, m_HpbarRHeight; //hp바
	float m_HpbarGWidth, m_HpbarGHeight;
	float m_Hpbar;

	float m_TargetHpbarRWidth, m_TargetHpbarRHeight; //타겟 hp바
	float m_TargetHpbarGWidth, m_TargetHpbarGHeight;
	float m_TargetHpbar;

	float m_CharacterFaceWidth , m_CharacterFaceHeight; //캐릭터 정보창 얼굴

	float m_Skillq_buttonWidth, m_Skillq_buttonHeight; //스킬 버튼
	float m_Skillw_buttonWidth, m_Skillw_buttonHeight; //스킬 버튼
	float m_Skille_buttonWidth, m_Skille_buttonHeight; //스킬 버튼
	float m_Skillr_buttonWidth, m_Skillr_buttonHeight; //스킬 버튼

	float m_UpgradeSwordWidth, m_UpgradeSwordHeight;
	float m_UpgradeShieldWidth, m_UpgradeShieldHeight;
	float m_UpgradeBootsWidth, m_UpgradeBootsHeight;

	float m_UpgradeButtonWidth, m_UpgradeButtonHeight;  //업그레이드 버튼
	float m_UpgradeButtonNumberWidth, m_UpgradeButtonNumberHeight; //업그레이드 버튼 숫자
	float m_GameWinLoseWidth, m_GameWinLoseHeight;

	int m_nWndClientWidth;
	int m_nWndClientHeight;
	int m_nPrevWndClientWidth;
	int m_nPrevWndClientHeight;

	float m_fGameTimeSecond;
	float m_fGameTimeMinute;
	BOOL m_bChangeImage;
	BOOL m_bChangeHpbar;
	int m_iPrevFaceType;
	int m_iPrevHpType;

	BOOL m_bUIReset;
public:
	CScene(void);
	~CScene(void);

	CCamera* m_Camera;
	ControlManager		m_Control;

	bool OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	bool OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);

	void BuildObjects(ID3D11Device *pd3dDevice);
	void ReleaseObjects();

	bool ProcessInput();
	void AnimateObjects(float fTimeElapsed, ID3D11Device *pd3dDevice);
	void Render(ID3D11DeviceContext *pd3dDeviceContext, float fTimeElapsed, CCamera *pCamera);

	int GetMousePosX();
	int GetMousePosY();

	void SetMousePos()
	{
		m_MousePosX = 450;
		m_MousePosY = 400;
	}

	void AddOtherPlayer(ID3D11Device *pd3dDevice);

	//
	BOOL CheckCollisionAABBAxis( float c1, float w1, float c2, float w2);
	BOOL CheckCollision(D3DXVECTOR3 c1, float w1, float h1, float d1, D3DXVECTOR3 c2, float w2, float h2, float d2);
	void GameCollision(CGameObject* obj1, CGameObject* obj2);

	CGameObject* GetObject(int num);
	BOOL m_bJoinOtherPlayer;
	BOOL m_bJoin;

	// net

	void SetOtherClient(PlayerPacket _PI);
	void UpdateOtherClient(PlayerPacket* _PI);

	void OtherPlayerTargetSetting();

	//블렌딩
	ID3D11BlendState* m_particleEnableBlendingState;
	ID3D11BlendState* m_particleDisableBlendingState;
	void TurnOnAlphaBlending(ID3D11DeviceContext *pd3dDeviceContext, ID3D11BlendState* blendstate);
	void TurnOffAlphaBlending(ID3D11DeviceContext *pd3dDeviceContext, ID3D11BlendState* blendstate);
	HRESULT CreateBlend(ID3D11Device *pd3dDevice);

	//조명
	void CreateLightShaderVariables(ID3D11Device *pd3dDevice);
	void UpdateLightShaderVariable(ID3D11DeviceContext *pd3dDeviceContext, LIGHTS *pLights);
	void ReleaseLightShaderVariables();

	//ui
	void CreateUI(ID3D11Device *pd3dDevice,int  _wndWidth,int  _wndHeight);
	void RenderUI(ID3D11DeviceContext *pd3dDeviceContext, int _wndWidth, int _wndHeight);
	float GetMinimapUIWidth() {return m_UIMinimapWidth;}
	float GetMinimapUIHeight() {	return m_UIMinimapHeight;}

	void SetUIHpUpdate();
	void SetFontUI(CDXUTTextHelper* _text, int _num);

	void SetUISize();
};