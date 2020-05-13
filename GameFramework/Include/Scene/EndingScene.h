#pragma once
#include "MainScene.h"
class CEndingScene :
	public CMainScene
{
public:
	CEndingScene();
	virtual ~CEndingScene();

private:
	float m_fStarTime;
	float m_fStarLimitTime;

public:
	virtual void Start();
	virtual bool Init();
	virtual void Update(float fTime);
	virtual void Render(HDC hDC, float fTime);

public:
	virtual void CreatePrototype();
	virtual void EffectPrototype();
	virtual void StageLineCollision(class CCollider* pSrc, class CCollider* pDest, float fTime);
	virtual void CreateMonster();
	virtual bool SoundInit();
};

