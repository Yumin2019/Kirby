#pragma once
#include "Monster.h"
class CBurt :
	public CMonster
{
	DECLARE_OBJECT

public:
	CBurt();
	CBurt(const CBurt& obj);
	virtual ~CBurt();

private:
	// ¸â¹ö º¯¼ö
	Size m_tStageSize;

public:
	virtual void Start();
	virtual bool Init();
	virtual void Update(float fTime);
	virtual void LateUpdate(float fTime);
	virtual void Collision(float fTime);
	virtual void PrevRender(HDC hDC, float fTime);
	virtual void Render(HDC hDC, float fTime);
	virtual void PostRender(HDC hDC, float fTime);
	virtual CBurt* Clone() 	const;
	virtual void HitAnimation();
	virtual void AddClipName(vector<string>& vecClipName);
	virtual void IdleAnimation();
	virtual void HitStart(CCollider* pSrc, CCollider* pDest, float fTime);
	virtual void DefaultIdleAnimation();

};

