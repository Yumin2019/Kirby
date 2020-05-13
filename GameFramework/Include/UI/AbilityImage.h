#pragma once
#include "TargetUI.h"
class CAbilityImage :
	public CTargetUI
{
	DECLARE_OBJECT


protected:
	CAbilityImage();
	CAbilityImage(const CAbilityImage& obj);
	~CAbilityImage();

public:
	virtual void Start();
	virtual bool Init();
	virtual void Update(float fTime);
	virtual void LateUpdate(float fTime);
	virtual void Collision(float fTime);
	virtual void PrevRender(HDC hDC, float fTime);
	virtual void Render(HDC hDC, float fTime);
	virtual void PostRender(HDC hDC, float fTime);
	virtual CAbilityImage* Clone()	const;
	virtual void AddClipName(vector<string>& vecClipName);
};

