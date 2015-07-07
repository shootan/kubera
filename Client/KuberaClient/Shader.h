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

	//���Ͽ��� ����-���̴��� �����ϴ� �Լ��� �����Ѵ�. 
	void CreateVertexShaderFromFile(ID3D11Device *pd3dDevice, WCHAR *pszFileName, LPCSTR pszShaderName, LPCSTR pszShaderModel, ID3D11VertexShader **ppd3dVertexShader, D3D11_INPUT_ELEMENT_DESC *pd3dInputLayout, UINT nElements, ID3D11InputLayout **ppd3dVertexLayout);

	//���Ͽ��� �ȼ�-���̴��� �����ϴ� �Լ��� �����Ѵ�. 
	void CreatePixelShaderFromFile(ID3D11Device *pd3dDevice, WCHAR *pszFileName, LPCSTR pszShaderName, LPCSTR pszShaderModel, ID3D11PixelShader **ppd3dPixelShader);

	//CShader �Ļ� Ŭ�������� ���̴��� �����ϱ� ���� ȣ���ϴ� �����Լ��� �����Ѵ�.
	virtual void CreateShader(ID3D11Device *pd3dDevice, int nObjects);

	//CShader �Ļ� Ŭ�������� �������ϱ� ���� ȣ���ϴ� �����Լ��� �����Ѵ�.
	virtual void Render(ID3D11DeviceContext *pd3dDeviceContext);

	//���̴� Ŭ������ ��� ���۸� �����ϰ� ��ȯ�ϴ� ��� �Լ��� �����Ѵ�. 
	virtual void CreateShaderVariables(ID3D11Device *pd3dDevice);
	virtual void ReleaseShaderVariables();
	//���̴� Ŭ������ ��� ���۸� �����ϴ� ��� �Լ��� �����Ѵ�.
	virtual void UpdateShaderVariables(ID3D11DeviceContext *pd3dDeviceContext);
	virtual void BuildObjects(ID3D11Device *pd3dDevice){}

	virtual void AddObject(CGameObject *pObject){}
	virtual CGameObject* GetObject(int nObjectsNum){return m_ppObjects[nObjectsNum];}
	int GetObjectsNumber() {return m_nObjects;}

public:
	//����-���̴� �������̽� �����Ϳ� �Է�-���̾ƿ� �������̽� �����͸� �����Ѵ�.
	ID3D11VertexShader *m_pd3dVertexShader;
	ID3D11InputLayout *m_pd3dVertexLayout;

	//�ȼ�-���̴� �������̽� �����͸� �����Ѵ�.
	ID3D11PixelShader *m_pd3dPixelShader;
	ID3D11SamplerState *m_pSamLinear;

protected:
	//���̴����� �������� ���� ��ü�� ���� �����͵��� ����Ʈ�� �����Ѵ�. 
	CGameObject **m_ppObjects;       
	int m_nObjects;
	//���̴����� �������� ���� ��ü�� �߰��� ��ġ�� ��Ÿ���� �ε����� �����Ѵ�. 
	int m_nIndexToAdd;

	CCamera *m_pCamera;
};

class CObjectShader : public CShader
{
public:
	CObjectShader();
	~CObjectShader();

	//���̴��� �����ϴ� �Լ��� �����Ѵ�. nObjects�� ���̴��� �������� ��ü�� �����̴�.
	virtual void CreateShader(ID3D11Device *pd3dDevice, int nObjects);
	virtual void CreateShaderVariables(ID3D11Device *pd3dDevice);
	virtual void ReleaseShaderVariables();
	virtual void UpdateShaderVariables(ID3D11DeviceContext *pd3dDeviceContext, D3DXMATRIX *pd3dxmtxWorld);

	virtual void Render(ID3D11DeviceContext *pd3dDeviceContext);

private:
	ID3D11Buffer *m_pd3dcbWorldMatrix;

public:
	//���̴����� �������� ���� ��ü�� �߰��Ѵ�. 
	virtual void AddObject(CGameObject *pObject); 
};

class CParticleShader : public CShader
{
public:
	CParticleShader();
	~CParticleShader();

	//���̴��� �����ϴ� �Լ��� �����Ѵ�. nObjects�� ���̴��� �������� ��ü�� �����̴�.
	virtual void CreateShader(ID3D11Device *pd3dDevice, int nObjects);
	virtual void CreateShaderVariables(ID3D11Device *pd3dDevice);
	virtual void ReleaseShaderVariables();
	virtual void UpdateShaderVariables(ID3D11DeviceContext *pd3dDeviceContext, D3DXMATRIX *pd3dxmtxWorld);

	virtual void Render(ID3D11DeviceContext *pd3dDeviceContext);

private:
	ID3D11Buffer *m_pd3dcbWorldMatrix;

public:
	//���̴����� �������� ���� ��ü�� �߰��Ѵ�. 
	virtual void AddObject(CGameObject *pObject); 
};


class CAnimationShader : public CShader
{
public:
	CAnimationShader();
	~CAnimationShader();

	//���̴��� �����ϴ� �Լ��� �����Ѵ�. nObjects�� ���̴��� �������� ��ü�� �����̴�.
	virtual void CreateShader(ID3D11Device *pd3dDevice, int nObjects);
	virtual void CreateShaderVariables(ID3D11Device *pd3dDevice);
	virtual void ReleaseShaderVariables();
	virtual void UpdateShaderVariables(ID3D11DeviceContext *pd3dDeviceContext, D3DXMATRIX *pd3dxmtxWorld);

	virtual void Render(ID3D11DeviceContext *pd3dDeviceContext);

private:
	ID3D11Buffer *m_pd3dcbWorldMatrix;

public:
	//���̴����� �������� ���� ��ü�� �߰��Ѵ�. 
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

	//���̴����� �������� �޽��̴�.
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

	//���� ��ȯ ��İ� ���� ��ȯ ����� ���� ��� �����̴�.
	D3DXMATRIX m_d3dxmtxWorld;         
	ID3D11Buffer *m_pd3dcbWorldMatrix;

	//�ν��Ͻ� ������(���� ��ȯ ����� �迭)�̴�.
	ID3D11Buffer *m_pd3dcbBush3InstanceMatrices;
	ID3D11Buffer *m_pd3dcbRock2InstanceMatrices;
	ID3D11Buffer *m_pd3dcbRock3InstanceMatrices;
	ID3D11Buffer *m_pd3dcbMissileInstanceMatrices;
	ID3D11Buffer *m_pd3dcbBlueTowerInstanceMatrices;
	ID3D11Buffer *m_pd3dcbRedTowerInstanceMatrices;
	ID3D11Buffer *m_pd3dcbMinionInstanceMatrices;
	ID3D11Buffer *m_pd3dcbDestroyTowerInstanceMatrices;

	//�ν��Ͻ����� �׸� ��ü�� ������� �迭�� ������ ��� �����̴�.
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

	//������ �����ϱ� ���� ��� �����̴�.
	static ID3D11Buffer	 *m_pd3dcbMaterial;

	static void CreateMaterialShaderVariables(ID3D11Device *pd3dDevice);
	static void ReleaseMaterialShaderVariables();
	//������ ���̴� ������ ����(����)�ϱ� ���� �Լ��̴�.
	static void UpdateMaterialShaderVariable(ID3D11DeviceContext *pd3dDeviceContext, MATERIAL *pMaterial);
};
