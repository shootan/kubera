#include "StdAfx.h"
#include "Scene.h"

CScene::CScene()
{
	m_ppShaders = NULL;
	m_nShaders = 0;

	m_ppObjects = NULL;
	m_nObjects = 0;
}


CScene::~CScene()
{
}

void CScene::BuildObjects(ID3D11Device *pd3dDevice)
{
	// 이 쉐이더 객체에 대한 포인터들의 배열을 정의한다.
	m_nShaders = 1;
	m_pShaders = new CSceneShader[m_nShaders];

	// CObjectShader 클래스 객체를 생성한다.
	m_pShaders[0].CreateShader(pd3dDevice);
	m_pShaders[0].BuildObjects (pd3dDevice);
	
}

void CScene::ReleaseObjects()
{
	// 쉐이더 객체 리스트의 각 객체를 소멸시키고 리스트를 소멸시킨다.
	if (m_ppShaders)
	{
		for (int j=0; j<m_nShaders; j++) if (m_ppShaders[j]) delete m_ppShaders[j];
		delete [] m_ppShaders;
	}

	// 게임 객체 리스트의 각 객체를 반환(Release)하고 리스트를 소멸시킨다.
	if (m_ppObjects)
	{
		for  (int j=0; j<m_nObjects; j++) m_ppObjects[j]->Release();
		delete [] m_ppObjects;
	}
}

bool CScene::OnprocessingMouseMessage(HWND hwnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	return false;
}

bool CScene::OnProcessingKeyboardMessage(HWND hwnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	return false;
}

bool CScene::ProcessInput()
{
	return false;
}

void CScene::AnimateObjects(float fTimeElapsed)
{
	for (int i=0; i<m_nShaders; i++)
	{
		m_pShaders[i].AnimateObjects (fTimeElapsed);
	}
}

void CScene::Render(ID3D11DeviceContext *pd3dDeviceContext, CCamera *pCamera)
{
	// 쉐이더 객체 리스트의 각 쉐이더 객체를 렌더링한다.
	for (int i=0; i<m_nShaders; i++)
	{
		m_pShaders[i].Render (pd3dDeviceContext, pCamera);
	}
}

