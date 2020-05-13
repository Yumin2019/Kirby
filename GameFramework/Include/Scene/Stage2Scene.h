#pragma once
#include "MainScene.h"
class CStage2Scene :
	public CMainScene
{
public:
	CStage2Scene();
	virtual ~CStage2Scene();


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

