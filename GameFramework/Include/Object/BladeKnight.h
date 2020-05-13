#pragma once
#include "Monster.h"



class CBladeKnight :
	public CMonster
{
	DECLARE_OBJECT

public:
	CBladeKnight();
	CBladeKnight(const CBladeKnight& obj);
	virtual ~CBladeKnight();

private:
	// ¸â¹ö º¯¼ö
	class CObj* m_pPlayer;


private:
	void BulletCreate();

public:

	virtual void Start();
	virtual bool Init();
	virtual void Update(float fTime);
	virtual void LateUpdate(float fTime);
	virtual void Collision(float fTime);
	virtual void PrevRender(HDC hDC, float fTime);
	virtual void Render(HDC hDC, float fTime);
	virtual void PostRender(HDC hDC, float fTime);
	virtual CBladeKnight* Clone() 	const;
	virtual void HitAnimation();
	virtual void AddClipName(vector<string>& vecClipName);
	virtual void IdleAnimation();
	virtual void HitStart(CCollider* pSrc, CCollider* pDest, float fTime);
	virtual void DefaultIdleAnimation();
};

