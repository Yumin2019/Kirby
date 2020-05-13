#pragma once
#include "Scene.h"
class CAnimationEditScene :
	public CScene
{
public:
	CAnimationEditScene();
	~CAnimationEditScene();

public:
	virtual void Start();
	virtual bool Init();

private:
	void CreatePrototype();
};

