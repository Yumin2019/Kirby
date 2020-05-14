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
	// �浹�� �Ͼ ��쿡 ������ ��������� ó�����ش�.
	// �Ѿ˰� �浹�� �Ͼ ��쿡 ȣ���� �Ǵ� �Լ��̴�.
	// �׷���, �̰��� Ÿ���̳�, Line�� ���� ó���� ���� �ʴ� ��쿡 ȣ��Ǵ� �Լ��̴�.

	if (pDest->GetTag() == "Stage" || pDest->GetTag() == "StageLine")
		return;

	CObj* pDestObj = pDest->GetObj();



	switch (pDestObj->GetObjectType())
	{
	case OT_NORMAL_MONSTER:
	case OT_BOSS_DDD:



		if (pDestObj->GetBlend())
		{
			// ���Ϳ� �Ѿ��� �浹�� �� ��쿡 ����Ʈ�� �������ִ� ó���� ���� ���ְ� �ְ�, ���⼭�� ���� �ʴ´�. (���� �浹, �ߺ�)
			// �ٵ�, ��밡 ���� ������ ��� �ش� ������Ʈ�� �ٷ� �����ִ� ���� �´�.
			// �׷��� ���� ������ �� ���·� ��� ���� �ִ´�.. (���ʹ� �����ε�)
			m_bActive = false;
			return;
		}


		// ���ͳ� ������ �浹�� �Ͼ�� ���.

		SetDieTime(0.2f, false);

		break;

	case OT_PLAYER:

		// ���� �Ѿ��� �÷��̾�� �ε��� ���.

		// �Ϲ� Ŀ���� ���� �浹ü�� �浹�� �Ͼ ��쿡�� �����ؾ� �Ѵ�.


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
