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
	// �� ���̴� ��ü�� ���� �����͵��� �迭�� �����Ѵ�.
	m_nShaders = 1;
	m_pShaders = new CSceneShader[m_nShaders];

	// CObjectShader Ŭ���� ��ü�� �����Ѵ�.
	m_pShaders[0].CreateShader(pd3dDevice);
	m_pShaders[0].BuildObjects (pd3dDevice);
	
}

void CScene::ReleaseObjects()
{
	// ���̴� ��ü ����Ʈ�� �� ��ü�� �Ҹ��Ű�� ����Ʈ�� �Ҹ��Ų��.
	if (m_ppShaders)
	{
		for (int j=0; j<m_nShaders; j++) if (m_ppShaders[j]) delete m_ppShaders[j];
		delete [] m_ppShaders;
	}

	// ���� ��ü ����Ʈ�� �� ��ü�� ��ȯ(Release)�ϰ� ����Ʈ�� �Ҹ��Ų��.
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
	// ���̴� ��ü ����Ʈ�� �� ���̴� ��ü�� �������Ѵ�.
	for (int i=0; i<m_nShaders; i++)
	{
		m_pShaders[i].Render (pd3dDeviceContext, pCamera);
	}
}

