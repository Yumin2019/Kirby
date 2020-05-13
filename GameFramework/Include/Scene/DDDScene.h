#pragma once
#include "MainScene.h"
class CDDDScene :
	public CMainScene
{
public:
	CDDDScene();
	virtual ~CDDDScene();


public:
	virtual void Start();
	virtual bool Init();
	virtual void Update(float fTime);
	virtual bool SoundInit();

public:
	virtual void CreatePrototype();
	virtual void EffectPrototype();
	virtual void StageLineCollision(class CCollider* pSrc, class CCollider* pDest, float fTime);
	virtual void CreateMonster();
};

