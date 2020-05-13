#pragma once
#include "Collider.h"

class CColliderRect :
	public CCollider
{

	friend class CObj;

private:
	CColliderRect();
	CColliderRect(const CColliderRect& collider);
	~CColliderRect();

private:
	RectInfo	m_tRelative;	// 플레이어의 좌표로부터 l t  r b 의 상대적인 값.
	RectInfo	m_tInfo;		// 소속 오브젝트를 따라다니면서 사용할 직접적인 값이다.
	// 오브젝트의 좌표 + m_tRelative

public:
	void SetRelativeInfo(float l, float t, float r, float b)
	{
		m_tRelative.l = l;
		m_tRelative.t = t;
		m_tRelative.r = r;
		m_tRelative.b = b;
	}

public:
	RectInfo GetInfo() const
	{
		return m_tInfo;
	}

public:
	virtual void Start();
	virtual bool Init();
	virtual void Update(float fTime);
	virtual void LateUpdate(float fTime);
	virtual void PrevRender(HDC hDC, float fTime);
	virtual void Render(HDC hDC, float fTime);
	virtual void PostRender(HDC hDC, float fTime);
	virtual CColliderRect* Clone() const;
	virtual bool Collision(CCollider* pCollider);
	virtual void UpdateObjPos(const Position& tPos);

};

