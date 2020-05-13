#include "Collision\ColliderLine.h"
#include "Core/GameManager.h"
#include "Core/SceneManager.h"

CColliderLine::CColliderLine()
{
	SetTag("Line");

	m_eColliderType = CT_LINE;
}

CColliderLine::CColliderLine(const CColliderLine & collider) :
	CCollider(collider)
{
	m_tSrcPos = collider.m_tSrcPos;
	m_tDestPos = collider.m_tDestPos;
	m_fInclination = collider.m_fInclination;
}


CColliderLine::~CColliderLine()
{
}

void CColliderLine::SetLineRect()
{
	// 두 점의 위치가 설정이 되어 있다.
	// 우리는 어떤 식으로 위치가 들어오든지 표현할 수 있는 Rect를 만들면 되는 것이다.

	if (m_tSrcPos.x <= m_tDestPos.x)
	{
		// src의 x 좌표를 왼쪽으로
		// dest의 x 좌표를 오른쪽으로

		m_tLineRect.l = m_tSrcPos.x;
		m_tLineRect.r = m_tDestPos.x;
	}
	else
	{
		// 반대.
		m_tLineRect.l = m_tDestPos.x;
		m_tLineRect.r = m_tSrcPos.x;
	}

	if (m_tSrcPos.y <= m_tDestPos.y)
	{
		// src의 y 좌표를 위쪽으로
		// dest의 y 좌표를 아래쪽으로
		m_tLineRect.t = m_tSrcPos.y;
		m_tLineRect.b = m_tDestPos.y;
	}
	else
	{
		m_tLineRect.t = m_tDestPos.y;
		m_tLineRect.b = m_tSrcPos.y;
	}
}

float CColliderLine::GetX(float y)
{
	// (y - y1 + mx1) / m = x 

	// 선분에서 정의할 수 없는 y값이 들어온 경우.

	if (m_tLineRect.b < y || m_tLineRect.t > y)
		return -1.f;

	return (y - m_tSrcPos.y + m_fInclination * m_tSrcPos.x) / m_fInclination;
}

float CColliderLine::GetY(float x)
{
	// y = m(x - x1) + y1


	if (m_tLineRect.r < x || m_tLineRect.l > x)
		return -1.f;

	return m_fInclination * (x - m_tSrcPos.x) + m_tSrcPos.y;
}

void CColliderLine::Start()
{
	CCollider::Start();
}

bool CColliderLine::Init()
{
	CCollider::Init();

	return true;
}

void CColliderLine::Update(float fTime)
{
	CCollider::Update(fTime);
}

void CColliderLine::LateUpdate(float fTime)
{
}

void CColliderLine::PrevRender(HDC hDC, float fTime)
{
	
}

void CColliderLine::Render(HDC hDC, float fTime)
{
#ifdef _DEBUG

	Position tCameraPos = GET_SINGLE(CSceneManager)->GetCameraPos();

	HPEN hPen = WHITEPEN;

	if (m_bCollision)
		hPen = REDPEN;

	HPEN hPrevPen = (HPEN)SelectObject(hDC, hPen);

	MoveToEx(hDC, (int)(m_tSrcPos.x - tCameraPos.x), (int)(m_tSrcPos.y - tCameraPos.y), nullptr);
	LineTo(hDC, (int)(m_tDestPos.x - tCameraPos.x), (int)(m_tDestPos.y - tCameraPos.y));


	SelectObject(hDC, hPrevPen);

#endif
}

void CColliderLine::PostRender(HDC hDC, float fTime)
{
}

CColliderLine * CColliderLine::Clone() const
{
	return new CColliderLine(*this);
}

bool CColliderLine::Collision(CCollider * pCollider)
{
	switch (pCollider->GetColliderType())
	{
	case CT_RECT:
		return CollisionRectToLine(pCollider, this);
	case CT_SPHERE:
	case CT_POINT:
	case CT_LINE:
		break;
	}

	return false;
}

void CColliderLine::UpdateObjPos(const Position & tPos)
{
}
