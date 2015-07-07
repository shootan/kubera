#pragma once
#include "header.h"

class CTextureclass
{
public:
	CTextureclass(void);
	~CTextureclass(void);

	bool Initialize(ID3D11Device* pd3dDevice, WCHAR* filename);
	void Release();
	ID3D11ShaderResourceView* GetTexture();

	ID3D11ShaderResourceView* m_texture;
};

