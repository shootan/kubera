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

	// 파일에서 정점-쉐이더를 생성하는 함수를 선언한다.
	void CreateVertexShaderFromFile (ID3D11Device *pd3dDevice, WCHAR *pszFileName, LPCSTR pszShaderName,
		LPCSTR pszShaderModel, ID3D11VertexShader **ppd3dVertexShader, D3D11_INPUT_ELEMENT_DESC *pd3dInputLayout,
		UINT nElements, ID3D11InputLayout **ppd3dVertexLayout);

	// 파일에서 픽셀-쉐이더를 생성하는 함수를 선언한다.
	void CreatePixelShaderFromFile (ID3D11Device *pd3dDevice, WCHAR *pszFileName, LPCSTR pszShaderName, 
		LPCSTR pszShaderModel, ID3D11PixelShader **ppd3dPixelShader);

	// CShader 파생 클래스에서 쉐이더를 생성하기 위해 호출하는 가상함수를 선언한다.
	virtual void CreateShader (ID3D11Device *pd3dDevice, int nObjects);

	// CShader 파생 클ㄹ스에서 렌더링하기 위해 호출하는 가상함수를 선언한다.
	virtual void Render (ID3D11DeviceContext *pd3dDeviceContext);

	// 쉐이더를 생성하는 멤버함수를 선언한다.
	//virtual void CreateShader (ID3D11Device *pd3dDevice, int nObjects);

	// 쉐이더 클래스의 상수 버퍼를 생성하고 반환하는 멤버 함수를 선언한다.
	virtual void CreateShaderVariables (ID3D11Device *pd3dDevice);
	virtual void ReleaseShaderVariables ();

	// 쉐이더 클래스의 상수 버퍼를 갱신하는 멤버 함수를 선언한다.
	virtual void UpdateShaderVariables (ID3D11DeviceContext *pd3dDeviceContext);

public:
	// 정점-쉐이더 인터페이스 포인터와 입력-레이아웃 인터페이스 포인터를 선언한다.
	ID3D11VertexShader *m_pd3dVertexShader;
	ID3D11InputLayout *m_pd3dVertexLayout;

	// 픽셀-쉐이더 인터페이스 포인터를 선언한다.
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
//	// 쉐이더에서 렌더링할 게임 객체에 대한 포인터들의 리스트를 선언한다.
//	CGameObject **m_ppObjects;
//	int m_nObjects;
//
//	// 쉐이더에서 렌더링할 게임 객체를 추가할 위치를 나타내는 인덱스를 선언한다.
//	int m_nIndexToAdd;
//
//	// 월드 변환 행렬을 위한 상수 버퍼에 대한 인터페이스 포인터를 선언한다.
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