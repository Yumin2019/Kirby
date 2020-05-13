#pragma once
#include "Monster.h"
class CRocky :
	public CMonster
{
	DECLARE_OBJECT

public:
	CRocky();
	CRocky(const CRocky& obj);
	virtual ~CRocky();

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
	virtual CRocky* Clone() 	const;
	virtual void HitAnimation();
	virtual void AddClipName(vector<string>& vecClipName);
	virtual void IdleAnimation();
	virtual void HitStart(CCollider* pSrc, CCollider* pDest, float fTime);
	virtual void DefaultIdleAnimation();
};

