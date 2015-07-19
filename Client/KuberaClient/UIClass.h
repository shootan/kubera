#pragma once
#include "Mesh.h"


class UIClass : public CMesh
{
private:
	struct VertexType
	{
		D3DXVECTOR3 position;
		D3DXVECTOR2 texture;
	};

public:
	UIClass(ID3D11Device *pd3dDevice);
	~UIClass();

	bool Initialize(ID3D11Device*, int, int, WCHAR*, float, float);
	void Shutdown();
	bool Render(ID3D11DeviceContext*, float, float);

	int GetIndexCount();
	ID3D11ShaderResourceView* GetTexture();

	virtual void CreateRasterizerState(ID3D11Device *pd3dDevice);


	int GetScreenW(){ return m_screenWidth;}
	int GetScreenH(){ return m_screenHeight;}
	void SetScreenWH(int x, int y) { m_screenWidth = x; m_screenHeight = y;}
	void SetBitmapWH(float x, float y) { m_bitmapWidth = x; m_bitmapHeight = y;}

private:
	bool InitializeBuffers(ID3D11Device*);
	void ShutdownBuffers();
	bool UpdateBuffers(ID3D11DeviceContext*, float, float);

	bool LoadTexture(ID3D11Device*, WCHAR*);
	void ReleaseTexture();

private:
	CTextureclass* m_pTexture;
	int m_screenWidth, m_screenHeight;
	int m_bitmapWidth, m_bitmapHeight;
	int m_previousPosX, m_previousPosY;
};
