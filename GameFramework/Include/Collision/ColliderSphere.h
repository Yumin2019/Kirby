#pragma once
#include "Collider.h"
class CColliderSphere :
	public CCollider
{
	friend class CObj;

private:
	CColliderSphere();
	CColliderSphere(const CColliderSphere& collider);
	~CColliderSphere();

private:
	SphereInfo	m_tRelative;	
	SphereInfo	m_tInfo;		

public:
	void SetRelativeInfo(const Position& tCenter, float fRadius)
	{
		// 반지름은 사용하지 않음.
		// 플레이어의 위치에서 상대적인 위치값을 받는다.

		m_tRelative.tCenter = tCenter;
		m_tRelative.fRadius = fRadius;

		// 반지름 설정.
		m_tInfo.fRadius = fRadius;
	}

	void SetRelativeInfo(float x, float y, float fRadius)
	{
		m_tRelative.tCenter = Position(x, y);
		m_tRelative.fRadius = fRadius;

		m_tInfo.fRadius = fRadius;
	}

public:
	SphereInfo GetInfo() const
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
	virtual CColliderSphere* Clone() const;
	virtual bool Collision(CCollider* pCollider);
	virtual void UpdateObjPos(const Position& tPos);
};

