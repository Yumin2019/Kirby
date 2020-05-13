#pragma once
#include "TargetUI.h"
class CAbilityName :
	public CTargetUI
{
	DECLARE_OBJECT

protected:
	CAbilityName();
	CAbilityName(const CAbilityName& obj);
	~CAbilityName();

public:
	virtual void Start();
	virtual bool Init();
	virtual void Update(float fTime);
	virtual void LateUpdate(float fTime);
	virtual void Collision(float fTime);
	virtual void PrevRender(HDC hDC, float fTime);
	virtual void Render(HDC hDC, float fTime);
	virtual void PostRender(HDC hDC, float fTime);
	virtual CAbilityName* Clone()	const;
	virtual void AddClipName(vector<string>& vecClipName);
};

