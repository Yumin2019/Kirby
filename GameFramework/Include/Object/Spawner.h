#pragma once
#include "Obj.h"


class CSpawner :
	public CObj
{
	DECLARE_OBJECT

	friend class CStage;

protected:
	CSpawner();
	CSpawner(const CSpawner& obj);
	virtual ~CSpawner();

private:
	// 몬스터가 생성이 되는 방향
	SIDE_DIR m_eSpawnDir;

	// 생성할 몬스터의 프로토타입 이름이 들어간다.
	string	 m_strSpawnMonsterTag;

	// 생성위치는 자신의 위치에 생성하는 것으로 한다.

	bool m_bCameraCollision; // 카메라의 내부에 있는지 확인할 변수.
	bool m_bSpawn; // 생성이 되었는지 확인할 변수로, 카메라의 내부에 들어오는 순간 true가 되어 생성 / 아닌 경우에 false로 바뀐다.

	Position m_tCameraSrc;

	bool m_bCount;
	int m_iCount;

	class CObj* m_pTarget;


public:
	void SetSpawner(float x, float y, SIDE_DIR eSpawnDir, 
		const string& strSpawnMonsterTag, bool bCount = false, int iCount = INT_MAX);

public:
	virtual void Start();
	virtual bool Init();
	virtual void Update(float fTime);
	virtual void LateUpdate(float fTime);
	virtual void Collision(float fTime);
	virtual void PrevRender(HDC hDC, float fTime);
	virtual void Render(HDC hDC, float fTime);
	virtual void PostRender(HDC hDC, float fTime);
	virtual CSpawner* Clone()	const;
	virtual void AddClipName(vector<string>& vecClipName);
};

