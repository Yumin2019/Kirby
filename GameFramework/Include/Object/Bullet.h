#pragma once
#include "Effect.h"
class CBullet :
	public CEffect
{
	DECLARE_OBJECT

protected:
	CBullet();
	CBullet(const CBullet& obj);
	virtual ~CBullet();

private:


public:
	virtual void Start();
	virtual bool Init();
	virtual void Update(float fTime);
	virtual void LateUpdate(float fTime);
	virtual void Collision(float fTime);
	virtual void PrevRender(HDC hDC, float fTime);
	virtual void Render(HDC hDC, float fTime);
	virtual void PostRender(HDC hDC, float fTime);
	virtual CBullet* Clone()	const;
	virtual void AddClipName(vector<string>& vecClipName);
	void BulletCollisionStart(CCollider* pSrc, CCollider* pDest, float fTime);
};

