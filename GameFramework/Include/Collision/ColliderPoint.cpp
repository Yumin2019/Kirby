#include "ColliderPoint.h"
#include "../Object/Obj.h"
#include "../Core/GameManager.h"
#include "../Core/SceneManager.h"


CColliderPoint::CColliderPoint()
{
	m_eColliderType = CT_POINT;
	SetTag("Point");
}

CColliderPoint::CColliderPoint(const CColliderPoint & collider) :
	CCollider(collider)
{
	m_tRelative = collider.m_tRelative;
	m_tInfo = collider.m_tInfo;
}


CColliderPoint::~CColliderPoint()
{
}

void CColliderPoint::Start()
{
	CCollider::Start();
}

bool CColliderPoint::Init()
{
	return true;
}

void CColliderPoint::Update(float fTime)
{
	CCollider::Update(fTime);

}

void CColliderPoint::LateUpdate(float fTime)
{
	// ������Ʈ�� ó���ϰ�, ��ü�� �Ű�ٸ�, ���Ŀ� �浹ü�� ��ġ�� �Ű��ش�.

	m_tInfo = m_pObj->GetPos() + m_tRelative;
}

void CColliderPoint::PrevRender(HDC hDC, float fTime)
{
}

void CColliderPoint::Render(HDC hDC, float fTime)
{
#ifdef _DEBUG
	HBRUSH hBrush = GREENBRUSH;

	if (m_bCollision)
		hBrush = REDBRUSH;

	// ���� �簢���� ������ش�.

	RECT tRC = { (int)(m_tInfo.x - 5.f), (int)(m_tInfo.y - 5.f)
		, (int)(m_tInfo.x + 5.f), (int)(m_tInfo.y + 5.f) };

	if (!m_bIsUI)
	{
		Position tCameraPos = GET_SINGLE(CSceneManager)->GetCameraPos();

		tRC.left -= (int)tCameraPos.x;
		tRC.right -= (int)tCameraPos.x;
		tRC.top -= (int)tCameraPos.y;
		tRC.bottom -= (int)tCameraPos.y;
	}

	FrameRect(hDC, &tRC, hBrush);

#endif
}

void CColliderPoint::PostRender(HDC hDC, float fTime)
{
}

CColliderPoint * CColliderPoint::Clone() const
{
	return new CColliderPoint(*this);
}

bool CColliderPoint::Collision(CCollider * pCollider)
{
	switch (pCollider->GetColliderType())
	{
		// ����� �浹Ÿ�Կ� ���� �ٸ� ó���� �Ѵ�.
	case CT_RECT:
		return CollisionRectToPoint(pCollider, this);
	case CT_SPHERE:
		return CollisionSphereToPoint(pCollider, this);
	case CT_POINT:
		return CollisionPointToPoint(this, pCollider);
	}

	return false;
}

void CColliderPoint::UpdateObjPos(const Position & tPos)
{
	m_tInfo = tPos + m_tRelative;
}
