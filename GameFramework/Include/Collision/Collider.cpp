#include "Collider.h"
#include "../Core/GameManager.h"
#include "../Collision/CollisionManager.h"
#include "../Collision/ColliderRect.h"
#include "../Collision/ColliderSphere.h"
#include "../Collision/ColliderPoint.h"
#include "../Collision/ColliderLine.h"
#include "../Math.h"

CCollider::CCollider() :
	m_pObj(nullptr),
	m_pProfile(nullptr),
	m_iChannelIndex(0),
	m_bCollision(false),
	m_bIsUI(false),
	m_iSerialNumber(UINT_MAX)
{
	// UINT_MAX�� �ø��� �ѹ��� ���� ���� ���¸� ��Ÿ����.
}

CCollider::CCollider(const CCollider & collider)
{
	*this = collider;
	m_bCollision = false;
	m_pObj = nullptr;
}


CCollider::~CCollider()
{
	if (m_iSerialNumber != UINT_MAX)
	{
		// ������ Ÿ�Կ� �� �ִ� ��ü�� �浹ü���� Ȯ���ϴ� �κ��̴�.
		// ������ Ÿ���� �浹ü�� ��쿡�� ���� ���ҽ��� ��ȯ�� �ʿ䰡 �����ϱ�.

		float fTime = GET_SINGLE(CGameManager)->GetDeltaTime();
		// �浹ó���ϴٰ� ���ڱ� ����� ��ü�� ������ release ���¿��� ȣ���� �Լ��� ���ڷ� 
		// 0.f�� �ƴ� fTime�� �־��ֱ� ���ؼ� �ð��� �޾��ְ� �ִ�.

		list<CCollider*>::iterator iter;
		list<CCollider*>::iterator iterEnd = m_PrevCollisionList.end();

		for (iter = m_PrevCollisionList.begin(); iter != iterEnd; ++iter)
		{
			// ���࿡ ������ �浹�ϰ� �ִ� �浹ü�� �־��µ�, �� �浹ü�� ������� �ϴ� ��쿡��
			// �浹 ���¸� ���ֹ�����, release���¿� �ش��ϴ� �Լ��� ȣ�����ش�.

			// �ϴ� �浹���¸� ���ش�.
			// �����, ���࿡ A��ü�� B��ü�� �̹� �浹 ���°� ���� ��쿡�� �� List���� ������� ���� ���̰�,
			// ó���� �ϴٰ� ���ڱ� ���ʿ��� ������ ���ܼ� �浹ü�� ������� ��쿡
			// �̰��� ������ ���̴�. ��, ��� �浹ü�� �浹 ������ ��쿡�� List�� �߰��� ���ִ� ����̴�.
			// ���� �浹�� �ߴ��� ���δ� �Ǵ��� �ʿ���� �׳�, �浹 ���� false�� ���ָ� �׸��̴�.
		

			// ��������� �������ش�.
			GET_SINGLE(CCollisionManager)->DeletePrevCollisionAdj(m_iSerialNumber, (*iter)->GetSerialNumber());

			// ��� �浹ü���� ���� ���ش�.
			(*iter)->DeletePrevCollisionList(this);
			// ����� �� �Ϳ����� �ڵ����� �����ϱ� ������ ����.

			// release�� �ش��ϴ� �Լ��� ȣ���Ѵ�.

			Call(CS_RELEASE, (*iter), fTime);
			(*iter)->Call(CS_RELEASE, this, fTime);

		}
		
		// �̷��� �Ͽ�, �ڽŰ� �浹���迡 �ִ� �浹ü�κ��� ���� ������ ���ְ�,
		// ������ĵ� �������ְ�, ���������� release�� �ش��ϴ� �Լ��� ȣ���ϴ� ������ �������Ѵ�.
		// ���� �浹ü�� ���� �ʿ�� ���� ������ �� ��ü�� ������� �����̴�.
		GET_SINGLE(CCollisionManager)->ReturnSerialNumber(m_iSerialNumber);

	}
}

void CCollider::PrevCollisionListClear()
{
	list<CCollider*>::iterator iter;
	list<CCollider*>::iterator iterEnd = m_PrevCollisionList.end();

	for (iter = m_PrevCollisionList.begin(); iter != iterEnd; ++iter)
	{
		// �ڽ��� List�� �����ϱ� ������ �ڽŰ� �浹�� �Ͼ�� ��ü�� ���� ó���� Ǯ�������.

		(*iter)->DeletePrevCollisionList(this);
	}

	m_PrevCollisionList.clear();
}

void CCollider::SetProfile(const string & strName)
{
	m_pProfile = GET_SINGLE(CCollisionManager)->FindProfile(strName);
}

void CCollider::SetChannel(const string & strName)
{
	m_iChannelIndex = GET_SINGLE(CCollisionManager)->GetChannelIndex(strName);
}

bool CCollider::CollisionRectToRect(CCollider * pSrc, CCollider * pDest)
{
	Position	tIntersect;

	if (CMath::CollisionRectToRect(((CColliderRect*)pSrc)->GetInfo(),
		((CColliderRect*)pDest)->GetInfo(), tIntersect))
	{
		// �浹�� ����.

		pSrc->m_tIntersect = tIntersect;
		pDest->m_tIntersect = tIntersect;

		pSrc->m_bCollision = true;
		pDest->m_bCollision = true;

		return true;
	}

	return false;
}

bool CCollider::CollisionSphereToSphere(CCollider * pSrc, CCollider * pDest)
{
	Position	tIntersect;

	if (CMath::CollisionSphereToSphere(((CColliderSphere*)pSrc)->GetInfo(),
		((CColliderSphere*)pDest)->GetInfo(), tIntersect))
	{

		pSrc->m_tIntersect = tIntersect;
		pDest->m_tIntersect = tIntersect;

		pSrc->m_bCollision = true;
		pDest->m_bCollision = true;

		return true;
	}

	return false;
}

bool CCollider::CollisionRectToSphere(CCollider * pSrc, CCollider * pDest)
{
	Position	tIntersect;

	if (CMath::CollisionRectToSphere(((CColliderRect*)pSrc)->GetInfo(),
		((CColliderSphere*)pDest)->GetInfo(), tIntersect))
	{

		pSrc->m_tIntersect = tIntersect;
		pDest->m_tIntersect = tIntersect;

		pSrc->m_bCollision = true;
		pDest->m_bCollision = true;

		return true;
	}

	return false;
}

bool CCollider::CollisionRectToPoint(CCollider * pSrc, CCollider * pDest)
{

	Position	tIntersect;

	if (CMath::CollisionRectToPoint(((CColliderRect*)pSrc)->GetInfo(),
		((CColliderPoint*)pDest)->GetInfo(), tIntersect))
	{

		pSrc->m_tIntersect = tIntersect;
		pDest->m_tIntersect = tIntersect;

		pSrc->m_bCollision = true;
		pDest->m_bCollision = true;

		return true;
	}

	return false;
}

bool CCollider::CollisionSphereToPoint(CCollider * pSrc, CCollider * pDest)
{
	Position	tIntersect;

	if (CMath::CollisionSphereToPoint(((CColliderSphere*)pSrc)->GetInfo(),
		((CColliderPoint*)pDest)->GetInfo(), tIntersect))
	{

		pSrc->m_tIntersect = tIntersect;
		pDest->m_tIntersect = tIntersect;

		pSrc->m_bCollision = true;
		pDest->m_bCollision = true;

		return true;
	}

	return false;
}

bool CCollider::CollisionPointToPoint(CCollider * pSrc, CCollider * pDest)
{
	Position	tIntersect;

	if (CMath::CollisionPointToPoint(((CColliderPoint*)pSrc)->GetInfo(),
		((CColliderPoint*)pDest)->GetInfo(), tIntersect))
	{

		pSrc->m_tIntersect = tIntersect;
		pDest->m_tIntersect = tIntersect;

		pSrc->m_bCollision = true;
		pDest->m_bCollision = true;

		return true;
	}

	return false;
}

bool CCollider::CollisionRectToLine(CCollider * pSrc, CCollider * pDest)
{
	Position	tIntersect;

	CColliderLine* pLine = (CColliderLine*)pDest;

	if (CMath::CollisionRectToLine(((CColliderRect*)pSrc)->GetInfo(),
		pLine->GetSrcPos(), pLine->GetDestPos(), pLine->GetLineRect(), tIntersect))
	{

		pSrc->m_tIntersect = tIntersect;
		pDest->m_tIntersect = tIntersect;

		pSrc->m_bCollision = true;
		pDest->m_bCollision = true;

		return true;
	}

	return false;
}




void CCollider::AddPrevCollisionList(CCollider * pCollider)
{
	m_PrevCollisionList.push_back(pCollider);
}

void CCollider::DeletePrevCollisionList(CCollider * pCollider)
{
	list<CCollider*>::iterator iter;
	list<CCollider*>::iterator iterEnd = m_PrevCollisionList.end();

	for (iter = m_PrevCollisionList.begin(); iter != iterEnd; ++iter)
	{
		if ((*iter) == pCollider)
		{
			m_PrevCollisionList.erase(iter);
			return;
		}
	}

}

void CCollider::Start()
{
	// �浹ü�� �ʿ� ��ġ�� �Ǿ�� �ϴ� �������� SerialNumber�� �Ҵ��Ѵ�.
	m_iSerialNumber = GET_SINGLE(CCollisionManager)->CreateSerialNumber();
	GET_SINGLE(CCollisionManager)->AddCollider(this);
	// �浹�Ŵ������� �� ������ ������ �ֵ��� �ϱ����ؼ� �浹ü ����Ʈ�� �־��ش�.

	// �浹ü�� ��쿡�� ref �⺻������ 2���̴�.

	//  ������Ʈ�� ���� ��쿡 �浹ü�� ���Ͽ� release
	//  CollisionManager�� �浹ü ������ ������ �ִٰ� �̰��� release �ϸ� ��.
}

bool CCollider::Init()
{
	// ��� �浹ü���� �����ؾ��ϴ� �⺻���� Init�� �����Ѵ�.
	m_pProfile = GET_SINGLE(CCollisionManager)->FindProfile("BlockAll");
	m_iChannelIndex = GET_SINGLE(CCollisionManager)->GetChannelIndex("WorldStatic");

	// ���� ������ ���� ������ �⺻������ ��� ä�ΰ� �浹ó���� �ϵ��� �ϴ� Profile�� �����صΰ�,
	// �̰��� ä���� Worldstatic���� �����Ѵٴ� �ǹ��̴�.
	
	return true;
}

void CCollider::Update(float fTime)
{
	m_bCollision = false;
}

void CCollider::LateUpdate(float fTime)
{
}

void CCollider::PrevRender(HDC hDC, float fTime)
{
}

void CCollider::Render(HDC hDC, float fTime)
{
}

void CCollider::PostRender(HDC hDC, float fTime)
{
}

CCollider * CCollider::Clone() const
{
	return nullptr;
}

void CCollider::UpdateObjPos(const Position & tPos)
{
}
