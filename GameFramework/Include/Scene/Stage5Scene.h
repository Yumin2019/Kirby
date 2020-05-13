#pragma once
#include "MainScene.h"
class CStage5Scene :
	public CMainScene
{
public:
	CStage5Scene();
	virtual ~CStage5Scene();


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

