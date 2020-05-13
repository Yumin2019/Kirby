#include "Object\Bullet.h"
#include "Object/Monster.h"
#include "Collision/Collider.h"
#include "Collision/CollisionManager.h"
#include "Object/Player.h"

CBullet::CBullet()
{
}


CBullet::CBullet(const CBullet & obj) :
	CEffect(obj)
{
}

CBullet::~CBullet()
{
}

void CBullet::Start()
{
	CEffect::Start();

	CCollider* pCollider = FindCollider();

	if (pCollider)
	{
		pCollider->SetCollisionCallback<CBullet>(CS_BLOCK, &CBullet::BulletCollisionStart, this);
	}
}

bool CBullet::Init()
{
	if (!CEffect::Init())
		return false;


	return true;
}

void CBullet::Update(float fTime)
{
	CEffect::Update(fTime);
}

void CBullet::LateUpdate(float fTime)
{
	CEffect::LateUpdate(fTime);

}

void CBullet::Collision(float fTime)
{
	CEffect::Collision(fTime);

}

void CBullet::PrevRender(HDC hDC, float fTime)
{
	CEffect::PrevRender(hDC, fTime);

}

void CBullet::Render(HDC hDC, float fTime)
{
	CEffect::Render(hDC, fTime);


}

void CBullet::PostRender(HDC hDC, float fTime)
{
	CEffect::PostRender(hDC, fTime);

}

CBullet * CBullet::Clone() const
{
	return new CBullet(*this);
}

void CBullet::AddClipName(vector<string>& vecClipName)
{
}

void CBullet::BulletCollisionStart(CCollider * pSrc, CCollider * pDest, float fTime)
{
	// 충돌이 일어난 경우에 빠르게 사라지도록 처리해준다.
	// 총알과 충돌이 일어난 경우에 호출이 되는 함수이다.
	// 그런데, 이것은 타일이나, Line에 대한 처리를 하지 않는 경우에 호출되는 함수이다.

	if (pDest->GetTag() == "Stage" || pDest->GetTag() == "StageLine")
		return;

	CObj* pDestObj = pDest->GetObj();



	switch (pDestObj->GetObjectType())
	{
	case OT_NORMAL_MONSTER:
	case OT_BOSS_DDD:



		if (pDestObj->GetBlend())
		{
			// 몬스터와 총알이 충돌이 된 경우에 이펙트를 생성해주는 처리를 따로 해주고 있고, 여기서는 하지 않는다. (몬스터 충돌, 중복)
			// 근데, 상대가 무적 상태인 경우 해당 오브젝트를 바로 없애주는 것이 맞다.
			// 그렇게 하지 않으면 그 상태로 계속 남아 있는다.. (몬스터는 무적인데)
			m_bActive = false;
			return;
		}


		// 몬스터나 보스가 충돌이 일어나는 경우.

		SetDieTime(0.2f, false);

		break;

	case OT_PLAYER:

		// 몬스터 총알이 플레이어와 부딪힌 경우.

		// 일반 커비의 흡입 충돌체와 충돌이 일어난 경우에는 무시해야 한다.


		if (pDest->GetTag() == "KirbyAttackLeft" || pDest->GetTag() == "KirbyAttackRight")
			return;

		if (pDestObj->GetBlend())
		{
			m_bActive = false;
			return;
		}

		SetDieTime(0.2f, false);

		break;
	}

	/*
	
	*/


}
