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
	// 업데이트를 처리하고, 물체를 옮겼다면, 이후에 충돌체의 위치를 옮겨준다.



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

	// 비활성화 rect면 흰색으로 칠한다.

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
	// FrameRect는 속이 빈 사각형을 출력해주는 함수이다.

	TCHAR strNumber[32] = {};

	wsprintf(strNumber, TEXT("Serial : %d"), m_iSerialNumber);

	TextOut(hDC, tRC.left, tRC.top, strNumber, lstrlen(strNumber));

	// DEBUG모드에서 충돌체를 표시해주고, 시리얼 값이 보이도록 한다.
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
		// 상대의 충돌타입에 따라서 다른 처리를 한다.
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
