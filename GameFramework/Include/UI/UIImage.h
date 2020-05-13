#pragma once

#include "UI.h"

class CUIImage :
	public CUI
{
	DECLARE_OBJECT

protected:
	CUIImage();
	CUIImage(const CUIImage& obj);
	~CUIImage();

public:
	virtual void Start();
	virtual bool Init();
	virtual void Update(float fTime);
	virtual void LateUpdate(float fTime);
	virtual void Collision(float fTime);
	virtual void PrevRender(HDC hDC, float fTime);
	virtual void Render(HDC hDC, float fTime);
	virtual void PostRender(HDC hDC, float fTime);
	virtual CUIImage* Clone()	const;
	virtual void AddClipName(vector<string>& vecClipName);

};

