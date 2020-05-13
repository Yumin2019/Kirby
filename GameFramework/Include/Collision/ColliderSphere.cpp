#include "ColliderSphere.h"
#include "../Object/Obj.h"
#include "../Core/GameManager.h"
#include "../Core/SceneManager.h"


CColliderSphere::CColliderSphere()
{
	m_eColliderType = CT_SPHERE;
	SetTag("Sphere");
}

CColliderSphere::CColliderSphere(const CColliderSphere & collider) :
	CCollider(collider)
{
	m_tRelative = collider.m_tRelative;
	m_tInfo = collider.m_tInfo;
}


CColliderSphere::~CColliderSphere()
{
}

void CColliderSphere::Start()
{
	CCollider::Start();
}

bool CColliderSphere::Init()
{
	return true;
}

void CColliderSphere::Update(float fTime)
{
	CCollider::Update(fTime);
}

void CColliderSphere::LateUpdate(float fTime)
{
	// 업데이트를 처리하고, 물체를 옮겼다면, 이후에 충돌체의 위치를 옮겨준다.

	m_tInfo.tCenter = m_pObj->GetPos() + m_tRelative.tCenter;
}

void CColliderSphere::PrevRender(HDC hDC, float fTime)
{
}

void CColliderSphere::Render(HDC hDC, float fTime)
{
#ifdef _DEBUG
	// 속이 빈 원을 출력하는 함수는 없다.
	// 따라서, 펜을 통해서 직접 그려주도록 한다.

	HPEN hPen = GREENPEN;

	if (m_bCollision)
		hPen = REDPEN;

	// 이전 펜을 잡아둔다.
	HPEN hPrevPen = (HPEN)SelectObject(hDC, hPen);

	// 30도씩 12 .. 마지막과 연결하기 위해서 1점이 더 필요하다.

	Position tLinePos[13];

	Position tCameraPos;

	if (!m_bIsUI)
		tCameraPos = GET_SINGLE(CSceneManager)->GetCameraPos();

	float fAngle = 0;
	for (int i = 0; i < 13; ++i)
	{
		
		tLinePos[i].x = m_tInfo.tCenter.x - tCameraPos.x + cosf(fAngle) * m_tInfo.fRadius;
		tLinePos[i].y = m_tInfo.tCenter.y - tCameraPos.y + sinf(fAngle) * m_tInfo.fRadius;

		fAngle += PI / 6.f;
	}

	MoveToEx(hDC, (int)tLinePos[0].x, (int)tLinePos[0].y, nullptr);

	for (int i = 1; i < 13; ++i)
	{
		LineTo(hDC, (int)tLinePos[i].x, (int)tLinePos[i].y);
	}

	// 원을 그려내고 나서 이전 펜을 다시 잡는다.

	SelectObject(hDC, hPrevPen);
#endif
}

void CColliderSphere::PostRender(HDC hDC, float fTime)
{
}

CColliderSphere * CColliderSphere::Clone() const
{
	return new CColliderSphere(*this);
}

bool CColliderSphere::Collision(CCollider * pCollider)
{
	switch (pCollider->GetColliderType())
	{
	case CT_RECT:
		return CollisionRectToSphere(pCollider, this);
	case CT_SPHERE:
		return CollisionSphereToSphere(this, pCollider);
	case CT_POINT:
		return CollisionSphereToPoint(this, pCollider);
	}

	return false;
}

void CColliderSphere::UpdateObjPos(const Position & tPos)
{
	m_tInfo.tCenter = tPos + m_tRelative.tCenter;
	// Info의 중심만 바꾸면 되는데, 플레이어의 위치 + 상대적인 위치를 통해서 Center 의 위치를 설정해준다.
	// 원래가 구체는 상대적인 좌표를 Center에 받고 있음.
}

