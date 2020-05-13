#pragma once
#include "Scene.h"
class CMainScene :
	public CScene
{
public:
	CMainScene();
	virtual ~CMainScene();

protected:
	Position m_tStartPos;
	Position m_tDoorPos;


public:
	class CDoor* CreateDoor(const Position& tPos, SCENE_TYPE eArrivalScene);
	
	Position GetStartPos() const
	{
		return m_tStartPos;
	}

public:
	virtual void KirbyInit(class CStage* pStage);
	virtual void Start();
	virtual bool Init();
	virtual void Update(float fTime);
	virtual void PrototypeSave();
	void AddLineCollider(class CStage* pStage, int iSrcX, int iSrcY, int iDestX, int iDestY);
	void LoadObjectPos(class CStage* pStage, const char* pFileName);

public:
	virtual void CreatePrototype();
	virtual void EffectPrototype();
	virtual void ObjectPrototype();
	virtual void StageLineCollision(class CCollider* pSrc, class CCollider* pDest, float fTime);
	virtual void CreateMonster();

};

