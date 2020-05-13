#pragma once
#include "AniObject.h"
class CGrass :
	public CAniObject
{
	DECLARE_OBJECT

public:
	CGrass();
	CGrass(const CGrass& obj);
	~CGrass();


public:
	virtual void Start();
	virtual bool Init();
	virtual void Update(float fTime);
	virtual void LateUpdate(float fTime);
	virtual void Collision(float fTime);
	virtual void PrevRender(HDC hDC, float fTime);
	virtual void Render(HDC hDC, float fTime);
	virtual void PostRender(HDC hDC, float fTime);
	virtual CGrass* Clone() const;
	virtual void AddClipName(vector<string>& vecClipName);
};

