#pragma once
#include "MainScene.h"
class CStage3Scene :
	public CMainScene
{
public:
	CStage3Scene();
	virtual ~CStage3Scene();


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

