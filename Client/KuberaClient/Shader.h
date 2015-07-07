#pragma once
#include "header.h"
#include "GameObject.h"
#include "ObstacleObject.h"
#include "MissileObject.h"
#include "Mesh.h"
#include "FBX/GFBXMeshLoader.h"
#include "MissileManager.h"
#include "ObstacleManager.h"
#include "TowerManager.h"
#include "MinionManager.h"

struct VS_CB_WORLD_MATRIX
{
	D3DXMATRIX m_d3dxmtxWorld;         
};

class CShader
{
public:
	CShader(void);
	~CShader(void);

	//파일에서 정점-쉐이더를 생성하는 함수를 선언한다. 
	void CreateVertexShaderFromFile(ID3D11Device *pd3dDevice, WCHAR *pszFileName, LPCSTR pszShaderName, LPCSTR pszShaderModel, ID3D11VertexShader **ppd3dVertexShader, D3D11_INPUT_ELEMENT_DESC *pd3dInputLayout, UINT nElements, ID3D11InputLayout **ppd3dVertexLayout);

	//파일에서 픽셀-쉐이더를 생성하는 함수를 선언한다. 
	void CreatePixelShaderFromFile(ID3D11Device *pd3dDevice, WCHAR *pszFileName, LPCSTR pszShaderName, LPCSTR pszShaderModel, ID3D11PixelShader **ppd3dPixelShader);

	//CShader 파생 클래스에서 쉐이더를 생성하기 위해 호출하는 가상함수를 선언한다.
	virtual void CreateShader(ID3D11Device *pd3dDevice, int nObjects);

	//CShader 파생 클래스에서 렌더링하기 위해 호출하는 가상함수를 선언한다.
	virtual void Render(ID3D11DeviceContext *pd3dDeviceContext);

	//쉐이더 클래스의 상수 버퍼를 생성하고 반환하는 멤버 함수를 선언한다. 
	virtual void CreateShaderVariables(ID3D11Device *pd3dDevice);
	virtual void ReleaseShaderVariables();
	//쉐이더 클래스의 상수 버퍼를 갱신하는 멤버 함수를 선언한다.
	virtual void UpdateShaderVariables(ID3D11DeviceContext *pd3dDeviceContext);
	virtual void BuildObjects(ID3D11Device *pd3dDevice){}

	virtual void AddObject(CGameObject *pObject){}
	virtual CGameObject* GetObject(int nObjectsNum){return m_ppObjects[nObjectsNum];}
	int GetObjectsNumber() {return m_nObjects;}

public:
	//정점-쉐이더 인터페이스 포인터와 입력-레이아웃 인터페이스 포인터를 선언한다.
	ID3D11VertexShader *m_pd3dVertexShader;
	ID3D11InputLayout *m_pd3dVertexLayout;

	//픽셀-쉐이더 인터페이스 포인터를 선언한다.
	ID3D11PixelShader *m_pd3dPixelShader;
	ID3D11SamplerState *m_pSamLinear;

protected:
	//쉐이더에서 렌더링할 게임 객체에 대한 포인터들의 리스트를 선언한다. 
	CGameObject **m_ppObjects;       
	int m_nObjects;
	//쉐이더에서 렌더링할 게임 객체를 추가할 위치를 나타내는 인덱스를 선언한다. 
	int m_nIndexToAdd;

	CCamera *m_pCamera;
};

class CObjectShader : public CShader
{
public:
	CObjectShader();
	~CObjectShader();

	//쉐이더를 생성하는 함수를 선언한다. nObjects는 쉐이더가 렌더링할 객체의 개수이다.
	virtual void CreateShader(ID3D11Device *pd3dDevice, int nObjects);
	virtual void CreateShaderVariables(ID3D11Device *pd3dDevice);
	virtual void ReleaseShaderVariables();
	virtual void UpdateShaderVariables(ID3D11DeviceContext *pd3dDeviceContext, D3DXMATRIX *pd3dxmtxWorld);

	virtual void Render(ID3D11DeviceContext *pd3dDeviceContext);

private:
	ID3D11Buffer *m_pd3dcbWorldMatrix;

public:
	//쉐이더에서 렌더링할 게임 객체를 추가한다. 
	virtual void AddObject(CGameObject *pObject); 
};

class CParticleShader : public CShader
{
public:
	CParticleShader();
	~CParticleShader();

	//쉐이더를 생성하는 함수를 선언한다. nObjects는 쉐이더가 렌더링할 객체의 개수이다.
	virtual void CreateShader(ID3D11Device *pd3dDevice, int nObjects);
	virtual void CreateShaderVariables(ID3D11Device *pd3dDevice);
	virtual void ReleaseShaderVariables();
	virtual void UpdateShaderVariables(ID3D11DeviceContext *pd3dDeviceContext, D3DXMATRIX *pd3dxmtxWorld);

	virtual void Render(ID3D11DeviceContext *pd3dDeviceContext);

private:
	ID3D11Buffer *m_pd3dcbWorldMatrix;

public:
	//쉐이더에서 렌더링할 게임 객체를 추가한다. 
	virtual void AddObject(CGameObject *pObject); 
};


class CAnimationShader : public CShader
{
public:
	CAnimationShader();
	~CAnimationShader();

	//쉐이더를 생성하는 함수를 선언한다. nObjects는 쉐이더가 렌더링할 객체의 개수이다.
	virtual void CreateShader(ID3D11Device *pd3dDevice, int nObjects);
	virtual void CreateShaderVariables(ID3D11Device *pd3dDevice);
	virtual void ReleaseShaderVariables();
	virtual void UpdateShaderVariables(ID3D11DeviceContext *pd3dDeviceContext, D3DXMATRIX *pd3dxmtxWorld);

	virtual void Render(ID3D11DeviceContext *pd3dDeviceContext);

private:
	ID3D11Buffer *m_pd3dcbWorldMatrix;

public:
	//쉐이더에서 렌더링할 게임 객체를 추가한다. 
	virtual void AddObject(CGameObject *pObject); 
};


class CInstancingShader : public CShader
{
public:
	CInstancingShader();
	~CInstancingShader();

	virtual void CreateShader(ID3D11Device *pd3dDevice, int nObjects);
	virtual void CreateShaderVariables(ID3D11Device *pd3dDevice);
	virtual void UpdateShaderVariables(ID3D11DeviceContext *pd3dDeviceContext, CCamera *pCamera);

	virtual void ReleaseObjects();
	virtual void AnimateObjects(float fTimeElapsed);
	virtual void Render(ID3D11DeviceContext *pd3dDeviceContext, CCamera *pCamera);
	virtual void BuildObjects(ID3D11Device *pd3dDevice);

	virtual void AddObject(CGameObject *pObject); 

	UINT m_nInstanceBufferStride;
	UINT m_nInstanceBufferOffset;

	//쉐이더에서 렌더링할 메쉬이다.
	GFBX::Mesh *m_pBush3Mesh;
	GFBX::Mesh *m_pRock2Mesh;
	GFBX::Mesh *m_pRock3Mesh;
	GFBX::Mesh *m_pMissileMesh;
	GFBX::Mesh *m_pBlueTowerMesh;
	GFBX::Mesh *m_pRedTowerMesh;
	GFBX::Mesh *m_pMinionMesh;
	GFBX::Mesh *m_pDestroyTowerMesh;

	CMaterial *m_pMaterial;

	int m_nBush3Objects;
	int m_nRock2Objects;
	int m_nRock3Objects;
	int m_nMissileObjects;
	int m_nBlueTowerObjects;
	int m_nRedTowerObjects;
	int m_nMinionObjects;
	int m_nDestroyTowerObjects;

	//월드 변환 행렬과 월드 변환 행렬을 위한 상수 버퍼이다.
	D3DXMATRIX m_d3dxmtxWorld;         
	ID3D11Buffer *m_pd3dcbWorldMatrix;

	//인스턴싱 데이터(월드 변환 행렬의 배열)이다.
	ID3D11Buffer *m_pd3dcbBush3InstanceMatrices;
	ID3D11Buffer *m_pd3dcbRock2InstanceMatrices;
	ID3D11Buffer *m_pd3dcbRock3InstanceMatrices;
	ID3D11Buffer *m_pd3dcbMissileInstanceMatrices;
	ID3D11Buffer *m_pd3dcbBlueTowerInstanceMatrices;
	ID3D11Buffer *m_pd3dcbRedTowerInstanceMatrices;
	ID3D11Buffer *m_pd3dcbMinionInstanceMatrices;
	ID3D11Buffer *m_pd3dcbDestroyTowerInstanceMatrices;

	//인스턴싱으로 그릴 객체의 색상들의 배열을 저장할 상수 버퍼이다.
	ID3D11Buffer *m_pd3dcbInstanceColors;
	UINT m_nColorBufferStride;
	UINT m_nColorBufferOffset;

public:
	ID3D11Buffer *CreateInstanceBuffer(ID3D11Device *pd3dDevice, int nObjects, UINT nBufferStride, void *pBufferData);
};

class CMaterialShader : public CShader
{
public:
	CMaterialShader();
	virtual ~CMaterialShader();

	//재질을 설정하기 위한 상수 버퍼이다.
	static ID3D11Buffer	 *m_pd3dcbMaterial;

	static void CreateMaterialShaderVariables(ID3D11Device *pd3dDevice);
	static void ReleaseMaterialShaderVariables();
	//재질을 쉐이더 변수에 설정(연결)하기 위한 함수이다.
	static void UpdateMaterialShaderVariable(ID3D11DeviceContext *pd3dDeviceContext, MATERIAL *pMaterial);
};
