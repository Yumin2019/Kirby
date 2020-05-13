#pragma once
#include "Obj.h"
class CDoor :
	public CObj
{

	DECLARE_OBJECT

public:
	CDoor();
	CDoor(const CDoor& obj);
	~CDoor();

private:
	SCENE_TYPE m_eSceneType;

private:
	void DoorColliderStay(CCollider* pSrc, CCollider* pDest, float fTime);

public:
	void SetSceneType(SCENE_TYPE eType)
	{
		m_eSceneType = eType;
	}

	SCENE_TYPE GetSceneType() const
	{
		return m_eSceneType;
	}

public:
	void Stage2Scene();
	void Stage3Scene();
	void Stage4Scene();
	void Stage5Scene();
	void DDDScene();
	void EndingScene();

private:
	void SceneChange();


public:
	virtual void Start();
	virtual bool Init();
	virtual void Update(float fTime);
	virtual void LateUpdate(float fTime);
	virtual void Collision(float fTime);
	virtual void PrevRender(HDC hDC, float fTime);
	virtual void Render(HDC hDC, float fTime);
	virtual void PostRender(HDC hDC, float fTime);
	virtual CDoor* Clone() const;
	virtual void AddClipName(vector<string>& vecClipName);
};

