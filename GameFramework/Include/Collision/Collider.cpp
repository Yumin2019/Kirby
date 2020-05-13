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
	// UINT_MAX는 시리얼 넘버를 받지 않은 상태를 나타낸다.
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
		// 프로토 타입에 들어가 있는 객체의 충돌체인지 확인하는 부분이다.
		// 프로토 타입의 충돌체의 경우에는 따로 리소스를 반환할 필요가 없으니까.

		float fTime = GET_SINGLE(CGameManager)->GetDeltaTime();
		// 충돌처리하다가 갑자기 사라진 객체의 마무리 release 상태에서 호출할 함수의 인자로 
		// 0.f가 아닌 fTime을 넣어주기 위해서 시간을 받아주고 있다.

		list<CCollider*>::iterator iter;
		list<CCollider*>::iterator iterEnd = m_PrevCollisionList.end();

		for (iter = m_PrevCollisionList.begin(); iter != iterEnd; ++iter)
		{
			// 만약에 이전에 충돌하고 있던 충돌체가 있었는데, 이 충돌체가 사라져야 하는 경우에는
			// 충돌 상태를 없애버리고, release상태에 해당하는 함수를 호출해준다.

			// 일단 충돌상태를 없앤다.
			// 참고로, 만약에 A물체와 B물체가 이미 충돌 상태가 끝난 경우에는 이 List에서 들어있지 않을 것이고,
			// 처리를 하다가 갑자기 한쪽에서 문제가 생겨서 충돌체가 사라지는 경우에
			// 이곳에 들어오는 것이다. 즉, 상대 충돌체와 충돌 상태인 경우에만 List에 추가를 해주는 방식이다.
			// 따라서 충돌을 했는지 여부는 판단할 필요없이 그냥, 충돌 상태 false를 해주면 그만이다.
		

			// 인접행렬을 정리해준다.
			GET_SINGLE(CCollisionManager)->DeletePrevCollisionAdj(m_iSerialNumber, (*iter)->GetSerialNumber());

			// 상대 충돌체에서 나를 없앤다.
			(*iter)->DeletePrevCollisionList(this);
			// 참고로 내 것에서는 자동으로 날리니까 문제가 없다.

			// release에 해당하는 함수를 호출한다.

			Call(CS_RELEASE, (*iter), fTime);
			(*iter)->Call(CS_RELEASE, this, fTime);

		}
		
		// 이렇게 하여, 자신과 충돌관계에 있던 충돌체로부터 나의 정보를 없애고,
		// 인접행렬도 정리해주고, 마지막으로 release에 해당하는 함수를 호출하는 것으로 마무리한다.
		// 나의 충돌체를 없앨 필요는 없는 이유는 이 객체는 사라지기 때문이다.
		GET_SINGLE(CCollisionManager)->ReturnSerialNumber(m_iSerialNumber);

	}
}

void CCollider::PrevCollisionListClear()
{
	list<CCollider*>::iterator iter;
	list<CCollider*>::iterator iterEnd = m_PrevCollisionList.end();

	for (iter = m_PrevCollisionList.begin(); iter != iterEnd; ++iter)
	{
		// 자신의 List를 정리하기 이전에 자신과 충돌이 일어났던 물체에 대한 처리를 풀어버린다.

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
		// 충돌이 났다.

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
	// 충돌체가 맵에 배치가 되어야 하는 순간에는 SerialNumber를 할당한다.
	m_iSerialNumber = GET_SINGLE(CCollisionManager)->CreateSerialNumber();
	GET_SINGLE(CCollisionManager)->AddCollider(this);
	// 충돌매니저에서 이 정보를 가지고 있도록 하기위해서 충돌체 리스트에 넣어준다.

	// 충돌체의 경우에는 ref 기본적으로 2개이다.

	//  오브젝트가 죽은 경우에 충돌체에 대하여 release
	//  CollisionManager가 충돌체 정보를 가지고 있다가 이것을 release 하면 끝.
}

bool CCollider::Init()
{
	// 모든 충돌체에서 수행해야하는 기본적인 Init을 정의한다.
	m_pProfile = GET_SINGLE(CCollisionManager)->FindProfile("BlockAll");
	m_iChannelIndex = GET_SINGLE(CCollisionManager)->GetChannelIndex("WorldStatic");

	// 따로 설정을 하지 않으면 기본적으로 모든 채널과 충돌처리를 하도록 하는 Profile을 설정해두고,
	// 이것의 채널을 Worldstatic으로 설정한다는 의미이다.
	
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
