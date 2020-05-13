#pragma once
#include "Star.h"
class CMoveStar :
	public CStar
{

	DECLARE_OBJECT

protected:
	CMoveStar();
	CMoveStar(const CMoveStar& obj);
	virtual ~CMoveStar();


protected:
	// ¸â¹ö º¯¼ö.

	float m_fMoveTime;
	float m_fMovePlayTime;
	bool m_bMove;


public:
	void SetMoveTime(float fMovePlayTime)
	{
		m_fMovePlayTime = fMovePlayTime;
		m_fMoveTime = 0.f;
	}

	void MoveDisEnable()
	{
		m_bMove = false;
	}

public:
	virtual void Start();
	virtual bool Init();
	virtual void Update(float fTime);
	virtual void LateUpdate(float fTime);
	virtual void Collision(float fTime);
	virtual void PrevRender(HDC hDC, float fTime);
	virtual void Render(HDC hDC, float fTime);
	virtual void PostRender(HDC hDC, float fTime);
	virtual CMoveStar* Clone()	const;
	virtual void AddClipName(vector<string>& vecClipName);
};

