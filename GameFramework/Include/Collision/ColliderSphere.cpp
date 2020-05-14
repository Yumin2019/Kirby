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
	// ������Ʈ�� ó���ϰ�, ��ü�� �Ű�ٸ�, ���Ŀ� �浹ü�� ��ġ�� �Ű��ش�.

	m_tInfo.tCenter = m_pObj->GetPos() + m_tRelative.tCenter;
}

void CColliderSphere::PrevRender(HDC hDC, float fTime)
{
}

void CColliderSphere::Render(HDC hDC, float fTime)
{
#ifdef _DEBUG
	// ���� �� ���� ����ϴ� �Լ��� ����.
	// ����, ���� ���ؼ� ���� �׷��ֵ��� �Ѵ�.

	HPEN hPen = GREENPEN;

	if (m_bCollision)
		hPen = REDPEN;

	// ���� ���� ��Ƶд�.
	HPEN hPrevPen = (HPEN)SelectObject(hDC, hPen);

	// 30���� 12 .. �������� �����ϱ� ���ؼ� 1���� �� �ʿ��ϴ�.

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

	// ���� �׷����� ���� ���� ���� �ٽ� ��´�.

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
	// Info�� �߽ɸ� �ٲٸ� �Ǵµ�, �÷��̾��� ��ġ + ������� ��ġ�� ���ؼ� Center �� ��ġ�� �������ش�.
	// ������ ��ü�� ������� ��ǥ�� Center�� �ް� ����.
}

