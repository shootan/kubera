#pragma once
#include <d3d11.h>
#include <d3dx11tex.h>


class CTextureclass
{
public:
	CTextureclass(void);
	~CTextureclass(void);

	bool Initialize(ID3D11Device* pd3dDevice, WCHAR* filename);
	void Release();
	ID3D11ShaderResourceView* GetTexture();
	ID3D11ShaderResourceView* const*GetTextureA();

private:
	ID3D11ShaderResourceView* m_texture;
};

