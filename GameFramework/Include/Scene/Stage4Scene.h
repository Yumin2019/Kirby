#pragma once
#include "MainScene.h"
class CStage4Scene :
	public CMainScene
{
public:
	CStage4Scene();
	virtual ~CStage4Scene();


public:
	virtual void Start();
	virtual bool Init();
	virtual void Update(float fTime);

public:
	virtual void CreatePrototype();
	virtual void EffectPrototype();
	virtual void StageLineCollision(class CCollider* pSrc, class CCollider* pDest, float fTime);
	virtual void CreateMonster();
	virtual bool SoundInit();
};

