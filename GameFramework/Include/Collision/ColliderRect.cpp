#include "ColliderRect.h"
#include "../Object/Obj.h"
#include "../Core/GameManager.h"
#include "../Core/SceneManager.h"

CColliderRect::CColliderRect()
{
	m_eColliderType = CT_RECT;
	SetTag("Rectangle");
}

CColliderRect::CColliderRect(const CColliderRect & collider) :
	CCollider(collider)
{
	m_tRelative = collider.m_tRelative;
	m_tInfo = collider.m_tInfo;
}


CColliderRect::~CColliderRect()
{
}

void CColliderRect::Start()
{
	CCollider::Start();
}

bool CColliderRect::Init()
{
	return true;
}

void CColliderRect::Update(float fTime)
{
	CCollider::Update(fTime);
}

void CColliderRect::LateUpdate(float fTime)
{
	// ������Ʈ�� ó���ϰ�, ��ü�� �Ű�ٸ�, ���Ŀ� �浹ü�� ��ġ�� �Ű��ش�.



	if (GetTag() == "MikeAttack")
	{
		Position tPos = GET_SINGLE(CSceneManager)->GetCameraPos();

		m_tInfo.l = tPos.x;
		m_tInfo.t = tPos.y;
		m_tInfo.r = m_tInfo.l + RS_WIDTH;
		m_tInfo.b = m_tInfo.t + RS_HEIGHT;
	}
	else
	{
		Position tPos = m_pObj->GetPos();

		m_tInfo.l = m_tRelative.l + tPos.x;
		m_tInfo.r = m_tRelative.r + tPos.x;
		m_tInfo.b = m_tRelative.b + tPos.y;
		m_tInfo.t = m_tRelative.t + tPos.y;
	}

}

void CColliderRect::PrevRender(HDC hDC, float fTime)
{
}

void CColliderRect::Render(HDC hDC, float fTime)
{
#ifdef _DEBUG
	HBRUSH hBrush = BLACKBRUSH;

	if (m_bCollision)
		hBrush = REDBRUSH;


	else if (!m_bEnable)
		hBrush = WHITEBRUSH;

	// ��Ȱ��ȭ rect�� ������� ĥ�Ѵ�.

	RECT tRC = { (int)m_tInfo.l, (int)m_tInfo.t, (int)m_tInfo.r, (int)m_tInfo.b };

	if (!m_bIsUI)
	{
		Position tCameraPos = GET_SINGLE(CSceneManager)->GetCameraPos();

		tRC.left -= (int)tCameraPos.x;
		tRC.right -= (int)tCameraPos.x;
		tRC.top -= (int)tCameraPos.y;
		tRC.bottom -= (int)tCameraPos.y;

	}

	FrameRect(hDC, &tRC, hBrush);
	// FrameRect�� ���� �� �簢���� ������ִ� �Լ��̴�.

	TCHAR strNumber[32] = {};

	wsprintf(strNumber, TEXT("Serial : %d"), m_iSerialNumber);

	TextOut(hDC, tRC.left, tRC.top, strNumber, lstrlen(strNumber));

	// DEBUG��忡�� �浹ü�� ǥ�����ְ�, �ø��� ���� ���̵��� �Ѵ�.
#endif
}

void CColliderRect::PostRender(HDC hDC, float fTime)
{
}

CColliderRect * CColliderRect::Clone() const
{
	return new CColliderRect(*this);
}

bool CColliderRect::Collision(CCollider * pCollider)
{
	switch (pCollider->GetColliderType())
	{
		// ����� �浹Ÿ�Կ� ���� �ٸ� ó���� �Ѵ�.
	case CT_RECT:
		return CollisionRectToRect(this, pCollider);
	case CT_SPHERE:
		return CollisionRectToSphere(this, pCollider);
	case CT_POINT:
		return CollisionRectToPoint(this, pCollider);
	case CT_LINE:
		return CollisionRectToLine(this, pCollider);
	}

	return false;
}

void CColliderRect::UpdateObjPos(const Position & tPos)
{
	m_tInfo.l = m_tRelative.l + tPos.x;
	m_tInfo.r = m_tRelative.r + tPos.x;
	m_tInfo.b = m_tRelative.b + tPos.y;
	m_tInfo.t = m_tRelative.t + tPos.y;

}
