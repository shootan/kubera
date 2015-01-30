#pragma once

#include "Object.h"

struct VS_CB_WORLD_MATRIX
{
	D3DXMATRIX m_d3dxmtWorld;
};

class CShader
{
public:
	CShader();
	virtual ~CShader();

	// ���Ͽ��� ����-���̴��� �����ϴ� �Լ��� �����Ѵ�.
	void CreateVertexShaderFromFile (ID3D11Device *pd3dDevice, WCHAR *pszFileName, LPCSTR pszShaderName,
		LPCSTR pszShaderModel, ID3D11VertexShader **ppd3dVertexShader, D3D11_INPUT_ELEMENT_DESC *pd3dInputLayout,
		UINT nElements, ID3D11InputLayout **ppd3dVertexLayout);

	// ���Ͽ��� �ȼ�-���̴��� �����ϴ� �Լ��� �����Ѵ�.
	void CreatePixelShaderFromFile (ID3D11Device *pd3dDevice, WCHAR *pszFileName, LPCSTR pszShaderName, 
		LPCSTR pszShaderModel, ID3D11PixelShader **ppd3dPixelShader);

	// CShader �Ļ� Ŭ�������� ���̴��� �����ϱ� ���� ȣ���ϴ� �����Լ��� �����Ѵ�.
	virtual void CreateShader (ID3D11Device *pd3dDevice, int nObjects);

	// CShader �Ļ� Ŭ�������� �������ϱ� ���� ȣ���ϴ� �����Լ��� �����Ѵ�.
	virtual void Render (ID3D11DeviceContext *pd3dDeviceContext);

	// ���̴��� �����ϴ� ����Լ��� �����Ѵ�.
	//virtual void CreateShader (ID3D11Device *pd3dDevice, int nObjects);

	// ���̴� Ŭ������ ��� ���۸� �����ϰ� ��ȯ�ϴ� ��� �Լ��� �����Ѵ�.
	virtual void CreateShaderVariables (ID3D11Device *pd3dDevice);
	virtual void ReleaseShaderVariables ();

	// ���̴� Ŭ������ ��� ���۸� �����ϴ� ��� �Լ��� �����Ѵ�.
	virtual void UpdateShaderVariables (ID3D11DeviceContext *pd3dDeviceContext);

public:
	// ����-���̴� �������̽� �����Ϳ� �Է�-���̾ƿ� �������̽� �����͸� �����Ѵ�.
	ID3D11VertexShader *m_pd3dVertexShader;
	ID3D11InputLayout *m_pd3dVertexLayout;

	// �ȼ�-���̴� �������̽� �����͸� �����Ѵ�.
	ID3D11PixelShader *m_pd3dPixelShader;
};

//class CSceneShader : public CShader
//{
//public:
//	CSceneShader();
//	virtual ~CSceneShader();
//
//	
//	virtual void CreateShader (ID3D11Device *pd3dDevice);
//	virtual void CreateShaderVariables (ID3D11Device *pd3dDevice);
//	virtual void UpdateShaderVariables (ID3D11DeviceContext *pd3dDeviceContext, D3DXMATRIX *pd3dxmtxWorld);
//	
//	virtual void BuildObjects (ID3D11Device *pd3dDevice);
//	virtual void ReleaseObjects();
//	virtual void AnimateObjects(float fTimeElapsed);	
//	virtual void Render (ID3D11DeviceContext *pd3dDeviceContext, CCamera *pCamera = NULL);
//	
//	
//private:
//	// ���̴����� �������� ���� ��ü�� ���� �����͵��� ����Ʈ�� �����Ѵ�.
//	CGameObject **m_ppObjects;
//	int m_nObjects;
//
//	// ���̴����� �������� ���� ��ü�� �߰��� ��ġ�� ��Ÿ���� �ε����� �����Ѵ�.
//	int m_nIndexToAdd;
//
//	// ���� ��ȯ ����� ���� ��� ���ۿ� ���� �������̽� �����͸� �����Ѵ�.
//	ID3D11Buffer *m_pd3dcbWorldMatrix;
//};

class CDiffusedShader : public CShader
{
public:
	CDiffusedShader();
	~CDiffusedShader();

	virtual void CreateShader (ID3D11Device *pd3dDevice);
	virtual void CreateShaderVariables (ID3D11Device *pd3dDevice);
	virtual void UpdateShaderVariables (ID3D11DeviceContext *pd3dDeviceContext);

	virtual void Render (ID3D11DeviceContext *pd3dDeviceContext);
};

class CSceneShader : public CDiffusedShader
{
public:
	CSceneShader();
	~CSceneShader();

	virtual void CreateShader(ID3D11Device *pd3dDevice);
	virtual void CreateShaderVariables(ID3D11Device *pd3dDevice);
	virtual void UpdateShaderVariables(ID3D11DeviceContext *pd3dDeviceContext, D3DXMATRIX *pd3dxmtxWorld);

	virtual void BuildObjects(ID3D11Device *pd3dDevice);
	virtual void ReleaseObjects();
	virtual void AnimateObjects(float fTimeElapsed);
	virtual void Render (ID3D11DeviceContext *pd3dDeviceContext, CCamera *pCamera  = NULL );

	CGameObject **m_ppObjects;
	int m_nObjects;
	ID3D11Buffer *m_pd3dcbWorldMatrix;
};

class CPlayerShader : public CDiffusedShader
{
public:
	CPlayerShader();
	~CPlayerShader();

	virtual void CreateShader(ID3D11Device *pd3dDevice);
	virtual void CreateShaderVariables(ID3D11Device *pd3dDevice);
	virtual void UpdateShaderVariables(ID3D11DeviceContext *pd3dDeviceContext, D3DXMATRIX *pd3dxmtxWorld);

	ID3D11Buffer *m_pd3dcbWorldMatrix;
};