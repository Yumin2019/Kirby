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
	// ���Ͱ� ������ �Ǵ� ����
	SIDE_DIR m_eSpawnDir;

	// ������ ������ ������Ÿ�� �̸��� ����.
	string	 m_strSpawnMonsterTag;

	// ������ġ�� �ڽ��� ��ġ�� �����ϴ� ������ �Ѵ�.

	bool m_bCameraCollision; // ī�޶��� ���ο� �ִ��� Ȯ���� ����.
	bool m_bSpawn; // ������ �Ǿ����� Ȯ���� ������, ī�޶��� ���ο� ������ ���� true�� �Ǿ� ���� / �ƴ� ��쿡 false�� �ٲ��.

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

