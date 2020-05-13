#pragma once
#include "AniObject.h"
class CDDDBackGround :
	public CAniObject
{
	DECLARE_OBJECT

public:
	CDDDBackGround();
	CDDDBackGround(const CDDDBackGround& obj);
	~CDDDBackGround();


public:
	virtual void Start();
	virtual bool Init();
	virtual void Update(float fTime);
	virtual void LateUpdate(float fTime);
	virtual void Collision(float fTime);
	virtual void PrevRender(HDC hDC, float fTime);
	virtual void Render(HDC hDC, float fTime);
	virtual void PostRender(HDC hDC, float fTime);
	virtual CDDDBackGround* Clone() const;
	virtual void AddClipName(vector<string>& vecClipName);
};

