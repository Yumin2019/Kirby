#pragma once
#include "Obj.h"
class CAniObject :
	public CObj
{
	DECLARE_OBJECT

public:
	CAniObject();
	CAniObject(const CAniObject& obj);
	~CAniObject();


public:
	virtual void Start();
	virtual bool Init();
	virtual void Update(float fTime);
	virtual void LateUpdate(float fTime);
	virtual void Collision(float fTime);
	virtual void PrevRender(HDC hDC, float fTime);
	virtual void Render(HDC hDC, float fTime);
	virtual void PostRender(HDC hDC, float fTime);
	virtual CAniObject* Clone() const;
	virtual void AddClipName(vector<string>& vecClipName);

};

