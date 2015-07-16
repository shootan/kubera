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

	bool Initialize(ID3D11Device*, int, int, WCHAR*, int, int);
	void Shutdown();
	bool Render(ID3D11DeviceContext*, int, int);

	int GetIndexCount();
	ID3D11ShaderResourceView* GetTexture();

	virtual void CreateRasterizerState(ID3D11Device *pd3dDevice);

private:
	bool InitializeBuffers(ID3D11Device*);
	void ShutdownBuffers();
	bool UpdateBuffers(ID3D11DeviceContext*, int, int);

	bool LoadTexture(ID3D11Device*, WCHAR*);
	void ReleaseTexture();

private:
	//ID3D11Buffer *m_vertexBuffer, *m_indexBuffer;
	//int m_vertexCount, m_indexCount;
	CTextureclass* m_pTexture;
	int m_screenWidth, m_screenHeight;
	int m_bitmapWidth, m_bitmapHeight;
	int m_previousPosX, m_previousPosY;
};
