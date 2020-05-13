#pragma once
#include "Monster.h"
class CGrizoo :
	public CMonster
{

	DECLARE_OBJECT

public:
	CGrizoo();
	CGrizoo(const CGrizoo& obj);
	virtual ~CGrizoo();

private:
	// ¸â¹ö º¯¼ö

public:
	void DirChange();


public:
	virtual void Start();
	virtual bool Init();
	virtual void Update(float fTime);
	virtual void LateUpdate(float fTime);
	virtual void Collision(float fTime);
	virtual void PrevRender(HDC hDC, float fTime);
	virtual void Render(HDC hDC, float fTime);
	virtual void PostRender(HDC hDC, float fTime);
	virtual CGrizoo* Clone() 	const;
	virtual void HitAnimation();
	virtual void AddClipName(vector<string>& vecClipName);
	virtual void IdleAnimation();
	virtual void DefaultIdleAnimation();

	virtual void HitStart(CCollider* pSrc, CCollider* pDest, float fTime);
};

