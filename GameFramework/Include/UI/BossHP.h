#pragma once
#include "TargetUI.h"
class CBossHP :
	public CTargetUI
{
	DECLARE_OBJECT

protected:
	CBossHP();
	CBossHP(const CBossHP& obj);
	~CBossHP();

public:
	virtual void Start();
	virtual bool Init();
	virtual void Update(float fTime);
	virtual void LateUpdate(float fTime);
	virtual void Collision(float fTime);
	virtual void PrevRender(HDC hDC, float fTime);
	virtual void Render(HDC hDC, float fTime);
	virtual void PostRender(HDC hDC, float fTime);
	virtual CBossHP* Clone()	const;
	virtual void AddClipName(vector<string>& vecClipName);
};

