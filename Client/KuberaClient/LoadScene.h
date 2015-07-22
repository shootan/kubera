#pragma once
#include "UIClass.h"
#include "UIObject.h"
#include "header.h"
#include "Shader.h"

class LoadScene
{
private:
	
	
	int m_Count;
	int m_Width;
	int m_Height;
	UIClass* m_BackImage;
	UIObject* m_BackImageObject;
	UIClass* m_ButtonImage[50];
	UIObject* m_ButtonImageObject[50];
	CUIShader* m_pUIShaders;


public:
	LoadScene();
	~LoadScene();

	void LoadData(ID3D11Device *m_pd3dDevice, ID3D11DeviceContext* m_pd3dDeviceContext);
	void BuildObject(ID3D11Device *m_pd3dDevice);
	void SetSize(int _width, int _height)
	{
		m_Width = _width;
		m_Height = _height;
	}
	int GetInfo()
	{
		return m_Count;
	}
};