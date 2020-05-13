#pragma once
#include "MainScene.h"
class CStage1Scene :
	public CMainScene
{
public:
	CStage1Scene();
	virtual ~CStage1Scene();


public:
	virtual void Start();
	virtual bool Init();
	virtual void Update(float fTime);

public:

	virtual bool SoundInit();
	virtual void CreatePrototype();
	virtual void EffectPrototype();
	virtual void StageLineCollision(class CCollider* pSrc, class CCollider* pDest, float fTime);
	virtual void CreateMonster();
};

