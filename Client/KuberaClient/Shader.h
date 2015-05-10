#pragma once
#include "header.h"
#include "GameObject.h"
#include "ObstacleObject.h"
#include "MissileObject.h"
#include "Mesh.h"
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


class CInstancingShader : public CShader
{
public:
	CInstancingShader();
	~CInstancingShader();

	virtual void CreateShader(ID3D11Device *pd3dDevice, int nObjects);
	virtual void CreateShaderVariables(ID3D11Device *pd3dDevice);
	virtual void UpdateShaderVariables(ID3D11DeviceContext *pd3dDeviceContext);

	virtual void ReleaseObjects();
	virtual void AnimateObjects(float fTimeElapsed);
	virtual void Render(ID3D11DeviceContext *pd3dDeviceContext);
	virtual void BuildObjects(ID3D11Device *pd3dDevice);

	virtual void AddObject(CGameObject *pObject); 

	//쉐이더에서 렌더링할 메쉬이다.
	CMesh *m_pBush3Mesh;
	CMesh *m_pRock2Mesh;
	CMesh *m_pRock3Mesh;
	CMesh *m_pMissileMesh;
	CMesh *m_pTowerMesh;
	CMesh *m_pMinionMesh;

	int m_nBush3Objects;
	int m_nRock2Objects;
	int m_nRock3Objects;
	int m_nMissileObjects;
	int m_nTowerObjects;
	int m_nMinionObjects;

	//월드 변환 행렬과 월드 변환 행렬을 위한 상수 버퍼이다.
	D3DXMATRIX m_d3dxmtxWorld;         
	ID3D11Buffer *m_pd3dcbWorldMatrix;

	//인스턴싱 데이터(월드 변환 행렬의 배열)이다.
	ID3D11Buffer *m_pd3dcbBush3InstanceMatrices;
	ID3D11Buffer *m_pd3dcbRock2InstanceMatrices;
	ID3D11Buffer *m_pd3dcbRock3InstanceMatrices;
	ID3D11Buffer *m_pd3dcbMissileInstanceMatrices;
	ID3D11Buffer *m_pd3dcbTowerInstanceMatrices;
	ID3D11Buffer *m_pd3dcbMinionInstanceMatrices;

	//인스턴싱으로 그릴 객체의 색상들의 배열을 저장할 상수 버퍼이다.
	ID3D11Buffer *m_pd3dcbInstanceColors;
	UINT m_nColorBufferStride;
	UINT m_nColorBufferOffset;
};