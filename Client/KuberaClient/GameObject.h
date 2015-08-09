#pragma once
#include "Mesh.h"
#include "header.h"
#include "FBX\GFBXMesh.h"
#include "Camera.h"

//#include "astarclass.h"

//재질의 색상을 나타내는 구조체이다.
struct MATERIAL
{
	D3DXCOLOR m_d3dxcAmbient;
	D3DXCOLOR m_d3dxcDiffuse;
	D3DXCOLOR m_d3dxcSpecular; //(r,g,b,a=power)
	D3DXCOLOR m_d3dxcEmissive;
};

class CMaterial
{
public:
	CMaterial();
	virtual ~CMaterial();

private:
	int m_nReferences;

public:
	void AddRef() { m_nReferences++; }
	void Release() { if (--m_nReferences <= 0) delete this; }

	MATERIAL m_Material;
};


class CGameObject
{
public:
	CGameObject(void);
	~CGameObject(void);

protected:
	//
	int			m_ID;
	float		m_HP;
	int			m_Level;
	float		m_Damage;
	float		m_SkillDamage;
	int			m_Defense;
	float		m_fWalkSpeed;
	int			m_Exp;
	BOOL		m_Visible;
	int			m_iTag;
	int			m_iState;
	int			m_iTargetID;
	int			m_iTeam;
	int			m_iType;
	int			m_iFaceType;
	//ui hp바를 위해 만듬
	float		m_PrevHP;

	//
	int m_nReferences;
	D3DXVECTOR3 m_Pos;
	D3DXVECTOR3 m_Scale;
	FLOAT		m_Rot;
	int			m_axis;
	D3DXVECTOR3 m_vDestination;
	D3DXVECTOR3 m_vWalkIncrement;
	D3DXVECTOR3 m_vFacingDirection;

	float BoundsizeX, BoundsizeY, BoundsizeZ;

	BOOL		 m_bSelected;
	CGameObject* m_pTarget;
	CGameObject* m_pAttacker;
	//BOOL m_bAstar;
	//BOOL m_bFindPath;

public:
	void AddRef();
	void Release();

	D3DXMATRIX m_d3dxmtxWorld;         
	CMesh *m_pMesh;
	GFBX::Mesh *m_pAniMesh;

	AABB m_bcMeshBoundingCube;

	//게임 객체는 하나의 재질을 가질 수 있다.
	CMaterial *m_pMaterial;
	void SetMaterial(CMaterial *pMaterial);


	//Astar* m_pAstar;
	//node_t* m_pBestWay;

	void SetMesh(CMesh *pMesh);
	void SetMesh(GFBX::Mesh *pMesh);
	virtual void Animate(float fTimeElapsed){}
	virtual void Render(ID3D11DeviceContext *pd3dDeviceContext, CCamera *pCamera);
	virtual void SetNewDestination ( D3DXVECTOR3 _pos ){}
	virtual void Update(float fTimeElapsed){}

	void SetScale(D3DXVECTOR3 _size);
	void SetRotation(int xyz, float _rot);  // x =1 y = 2 z =3
	void SetPosition(D3DXVECTOR3 _pos);
	D3DXVECTOR3 GetPosition() { return m_Pos; }

	void SetP(float x, float y, float z);


	Vector3 GetPos();
	Vector3 GetScale();
	float GetRot();
	void SetPos(Vector3 v);
	void SetScale(Vector3 v);
	void SetRot(float f);

	float GetHP() {return m_HP;}
	void SetHP(float _hp) { m_HP = _hp; }

	int GetLevel() { return m_Level;}

	float GetDamage() {return m_Damage;}
	void SetDamage(float _damage) { m_Damage = _damage; }
	float GetSkillDamage() { return m_SkillDamage;}

	int GetDefense() { return m_Defense;}
	void SetDefense(int _defense) { m_Defense = _defense; }

	float GetSpeed() { return m_fWalkSpeed; }
	void SetSpeed(float _speed) { m_fWalkSpeed = _speed; } 

	void SetPrevHP(int _hp) { m_PrevHP = _hp;}
	float GetPrevHP() { return m_PrevHP;}

	void SetAttackDamage(float _damage) { m_HP -= _damage; }
	void SetExp(int _exp) { m_Exp -= _exp;}

	int GetType() { return m_iType; }
	void SetType(int _type) { m_iType = _type; }
	int	GetID() { return m_ID; }
	void SetID(int _id) { m_ID= _id;}
	void SetVisible(BOOL _vi) { m_Visible  = _vi; }
	BOOL GetVisible() { return m_Visible; }
	int GetState() { return m_iState; }
	void SetState(int _state) 
	{
		int dfdfdf;
		dfdfdf = 0;
		m_iState = _state; 
	}
	int GetTag() { return m_iTag; }
	void SetTag(int _tag) { m_iTag = _tag;}
	void SetTargetID(int _id) { m_iTargetID = _id;}
	int GetTargetID() { return m_iTargetID; }

	void SetTarget(CGameObject* _target) { m_pTarget = _target; }
	CGameObject* GetTarget() { return m_pTarget; }
	void SetAttacker(CGameObject* _attacker) { m_pAttacker = _attacker; }
	CGameObject* GetAttacker() { return m_pAttacker; }

	void SetFaceType(int _type) { m_iFaceType = _type;}
	int GetFaceType() { return m_iFaceType; }

	void SetBoundSize(float x, float y, float z);
	float GetBoundSizeX() {return BoundsizeX;}
	float GetBoundSizeY() {return BoundsizeY;}
	float GetBoundSizeZ() {return BoundsizeZ;}
	
//	void SetAstar(BOOL _astar) {m_bAstar = _astar;}
//	BOOL GetAstar() { return m_bAstar; }

	void SetSelected(BOOL _selected) { m_bSelected = _selected; }
	BOOL GetSelected() { return m_bSelected; }

	void SetTeam(int _team) { m_iTeam = _team; }
	int GetTeam() { return m_iTeam; }


	D3DXVECTOR3 GetDirection() { return m_vWalkIncrement; }
	void SetDirection(D3DXVECTOR3 _dir) { m_vWalkIncrement = _dir; }

	//월드 좌표계의 픽킹 광선을 생성한다.
	void GenerateRayForPicking(D3DXVECTOR3 *pd3dxvPickPosition, D3DXMATRIX *pd3dxmtxWorld, D3DXMATRIX *pd3dxmtxView, D3DXVECTOR3 *pd3dxvPickRayPosition, D3DXVECTOR3 *pd3dxvPickRayDirection);
	//월드 좌표계의 픽킹 광선을 생성한다.
	int PickObjectByRayIntersection(D3DXVECTOR3 *pd3dxvPickPosition, D3DXMATRIX *pd3dxmtxView, MESHINTERSECTINFO *pd3dxIntersectInfo);

	bool IsVisible(CCamera *pCamera=NULL);
};
