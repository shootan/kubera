#include "DXUT.h"
#include "Textureclass.h"


CTextureclass::CTextureclass(void)
{
	m_texture = 0;
}


CTextureclass::~CTextureclass(void)
{
}

bool CTextureclass::Initialize(ID3D11Device* pd3dDevice, WCHAR* filename)
{
	HRESULT result;

	result = D3DX11CreateShaderResourceViewFromFile(pd3dDevice, filename, NULL, NULL, &m_texture, NULL);
	if(FAILED(result))
	{
		return false;
	}

	return true;

}

void CTextureclass::Release()
{
	if(m_texture)
	{
		m_texture->Release();
		m_texture = 0;
	}

	return;	
}

ID3D11ShaderResourceView* CTextureclass::GetTexture()
{
	return m_texture;
}

ID3D11ShaderResourceView* const*CTextureclass::GetTextureA()
{
	return &m_texture;
}