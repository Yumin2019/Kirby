#pragma once
#include "Monster.h"
class CSleep :
	public CMonster
{
	DECLARE_OBJECT

public:
	CSleep();
	CSleep(const CSleep& obj);
	virtual ~CSleep();

private:
	// ¸â¹ö º¯¼ö


public:

	virtual void Start();
	virtual bool Init();
	virtual void Update(float fTime);
	virtual void LateUpdate(float fTime);
	virtual void Collision(float fTime);
	virtual void PrevRender(HDC hDC, float fTime);
	virtual void Render(HDC hDC, float fTime);
	virtual void PostRender(HDC hDC, float fTime);
	virtual CSleep* Clone() 	const;
	virtual void HitAnimation();
	virtual void AddClipName(vector<string>& vecClipName);
	virtual void IdleAnimation();
	virtual void HitStart(CCollider* pSrc, CCollider* pDest, float fTime);
	virtual void DefaultIdleAnimation();
};

