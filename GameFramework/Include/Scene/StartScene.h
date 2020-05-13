#pragma once
#include "Scene.h"
class CStartScene :
	public CScene
{
public:
	CStartScene();
	~CStartScene();


public:
	virtual void Start();
	virtual bool Init();
	virtual bool SoundInit();
private:
	void StartButton(float fTime);
	void MapEditorButton(float fTime);
	void AnimationEditorButton(float fTime);
	void AddKey();
};

