#pragma once
#include "GameObject.h"
#include "UIClass.h"

class UIObject : public CGameObject
{
private:
	BOOL m_bUsed;
	UIClass* m_pUI;
public:
	UIObject();
	~UIObject();

	virtual void Animate(float fTimeElapsed){}
	virtual void Render(ID3D11DeviceContext *pd3dDeviceContext, float x, float y);
	virtual void SetNewDestination ( D3DXVECTOR3 _pos ){}
	virtual void Update(float fTimeElapsed){}

	void SetUI(UIClass* _UI);
	UIClass* GetUI(){ return m_pUI;}

	void SetUsed(BOOL _used) { m_bUsed = _used;}
	BOOL GetUsed() { return m_bUsed; }
};