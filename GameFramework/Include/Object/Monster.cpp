#include "Monster.h"
#include "../Math.h"
#include "../Core/SceneManager.h"
#include "../Collision/Collider.h"
#include "../Object/Effect.h"
#include "../Collision/CollisionManager.h"

#include "../Object/DDD.h"
#include "../Sound/SoundManager.h"

CMonster::CMonster() 
{

	m_fChangeTime = 0.f;
	m_fChangePlayTime = 0.f;
	m_bMove = true;

	m_eObjectType = OT_NORMAL_MONSTER;


	m_bStop = false;
	m_fStopTime = 0.f;
	m_fStopLimitTime = 0.f;

	m_eMode = MM_IDLE;

	m_bChange = false;
	m_eMonsterType = MT_NORMAL;

	m_fInitChangeTime = 0.f;

	m_bHitAnimation = false;
	m_fHitTime = 0.f;
	m_fHitPlayTime = 0.f;

	m_bPrevGravity = false;
}


CMonster::CMonster(const CMonster & obj) :
	CCharacter(obj)
{


	m_eMode = MM_IDLE;
	m_fChangeTime = 0.f;
	m_fChangePlayTime = obj.m_fChangePlayTime;
	m_bMove = obj.m_bMove;

	m_bStop = false;
	m_fStopTime = 0.f;
	m_fStopLimitTime = 0.f;

	m_eMonsterType = obj.m_eMonsterType;
	m_bChange = false;
	m_fInitChangeTime = obj.m_fInitChangeTime;

	m_bHitAnimation = false;
	m_fHitTime = 0.f;
	m_fHitPlayTime = 0.f;

	m_bPrevGravity = obj.m_bGravity;

	
}

CMonster::~CMonster()
{
}

void CMonster::MoveDisEnable(float fStopLimitTime, SIDE_DIR eCollisionDir)
{
	// �̹� ó���� �Ǿ� �ִ� ���� �ִٸ� ���� �ʴ´�.


	m_bStop = true;
	m_fStopTime = 0.f;
	m_fStopLimitTime = fStopLimitTime;
	m_bMove = false;

	// �ʱ�ȭ !
	m_fChangeTime = 0.f;


	// �浹����� �ݴ�� �����̵��� �����Ѵ�. (���� ���Ŀ�)
	switch (eCollisionDir)
	{
	case SD_LEFT:

		m_eDir = SD_RIGHT;
		break;

	case SD_RIGHT:
		m_eDir = SD_LEFT;
		break;
	}

	// �����ؾ� �Ѵ�.
	HitAnimation();

	// �ٲ� ���⿡ ���ؼ� Idle
	DefaultIdleAnimation();

}

void CMonster::Damage(int iDamage)
{
		CCharacter::Damage(iDamage);

		if (IsDie())
		{
			SetDieTime(0.5f, true);

			// ���Ͱ� ���� ��쿡�� 0.5s ���Ŀ� ����Ʈ�� �����ϵ��� �Ѵ�.

			// �׸��� ���带 ������ش�.
			int iRand = rand() % 3;

			switch (iRand)
			{
			case 0:
				GET_SINGLE(CSoundManager)->Play("MonsterHit1", CT_EFFECT);

				break;

			case 1:
				GET_SINGLE(CSoundManager)->Play("MonsterHit2", CT_EFFECT);

				break;

			case 2:
				GET_SINGLE(CSoundManager)->Play("MonsterHit3", CT_EFFECT);

				break;
			}
		}
}

void CMonster::MoveX(float fTime)
{
	// �ڽ��� ������ ������ �̵��ϴ� �Լ��̴�.
	switch (m_eDir)
	{
	case SD_LEFT:
		Move(Position(-1.f, 0.f), fTime);

		break;

	case SD_RIGHT:
		Move(Position(1.f, 0.f), fTime);

		break;
	}

}

void CMonster::MoveX(float fSpeed, float fTime)
{
	switch (m_eDir)
	{
	case SD_LEFT:
		Move(Position(-1.f, 0.f), fSpeed, fTime);

		break;

	case SD_RIGHT:
		Move(Position(1.f, 0.f), fSpeed, fTime);

		break;
	}
}

void CMonster::DirChange()
{
	switch (m_eDir)
	{
	case SD_LEFT:
		m_eDir = SD_RIGHT;
		break;

	case SD_RIGHT:
		m_eDir = SD_LEFT;
		break;
	}
}


void CMonster::Start()
{
	CCharacter::Start();
}

bool CMonster::Init()
{
	if (!CCharacter::Init())
		return false;

	return true;
}

void CMonster::Update(float fTime)
{
	CCharacter::Update(fTime);


	// Hit Time
	if (m_bHitAnimation)
	{

		HitAnimation();

		m_fHitTime += fTime;
		if (m_fHitTime >= m_fHitPlayTime)
		{
			m_bHitAnimation = false;
			m_bMove = true;

			if (m_bPrevGravity)
				m_bGravity = true;
			// ������ �߷� ó���� �ϴ� ���Ͷ�� �߷� ó���� �ٽ� �����ϵ��� �Ѵ�.

			switch (m_eObjectType)
			{
				case OT_BOSS_DDD:
				{
					CDDD* pDDD = (CDDD*)this;
					pDDD->InitIdle();

					pDDD->SetFalling(pDDD->GetJump());

					// ������ �ϰ� �־��ٸ�, falling = true�� ó���ؼ�, ���Ͽ� ���� ó���� ���� �ʵ��� �Ѵ�.

				}


				// DDD�� ����ũ�� ������ ���� ��쿡 �ʱ� ���·� ����������.

				break;
			}
		}

		// Hit Animation�� �����ִ� ���ȿ��� ������ Hit�ִϸ��̼��� ȣ���� �ǵ��� �Ѵ�.
		// ����ũ�� ���� �ð� ��ŭ�� ������ �� ó���� �ϵ��� �ϸ� �Ǵ� ���̴�.
	}


	if (m_bStop)
	{
		m_fStopTime += fTime;


		if (m_fStopTime >= m_fStopLimitTime)
		{
			m_bStop = false;
			m_bMove = true;
			m_bChange = false;
		}

		if (!m_bChange)
		{
			if (m_fStopTime >= 0.5f)
			{

				switch (m_eObjectType)
				{
				case OT_NORMAL_MONSTER:

					m_fChangeTime = 0.f;
					m_fChangePlayTime = m_fInitChangeTime; // �ʱ��� ���·� �Ϻ��ϰ� ������ ���� ����.
					m_eMode = MM_IDLE;

					IdleAnimation();

					m_bChange = true;

					break;

					case OT_BOSS_DDD:

					{
						CDDD* pDDD = (CDDD*)this;

						if (!pDDD->GetDie())
						{
							// ���� ������ ó���� �ƴ϶��.
							pDDD->InitIdle();
							pDDD->SetFalling(pDDD->GetJump());
						}
						else
						{
							// ���� ������ ó���� �Ϸ���..
							int i = 0;
						}
						// Idle ������� �ٲ�����µ�, ���� ������ �ϰ� �־��ٸ�,  fallingó���� �Ͽ� �� �ٴڿ� ������ ���ȿ��� ������ ó���� ���� �ʵ��� �Ѵ�.
						// ��, �¾Ƽ� �ٴڿ� �پ��־��ٸ� �ٷ� ���� ����, ���� ���¿��� ���� ��쿡�� ������ �ð��� �����ϰ� ������ ���۵ȴ�.
					}
					break;
				}
				
			}
		}
		
		
	}
}

void CMonster::LateUpdate(float fTime)
{


	CCharacter::LateUpdate(fTime);

}

void CMonster::Collision(float fTime)
{

	CCharacter::Collision(fTime);
}

void CMonster::PrevRender(HDC hDC, float fTime)
{
	CCharacter::PrevRender(hDC, fTime);

}

void CMonster::Render(HDC hDC, float fTime)
{
	CCharacter::Render(hDC, fTime);

}

void CMonster::PostRender(HDC hDC, float fTime)
{
	CCharacter::PostRender(hDC, fTime);

}

CMonster * CMonster::Clone() const
{
	return nullptr;
}

void CMonster::HitAnimation()
{
	m_bMove = false;
}

void CMonster::SetHitAnimation(float fPlayTime)
{
	m_bHitAnimation = true;
	m_fHitTime = 0.f;
	m_fHitPlayTime = fPlayTime;
	// ��������ڿ��� �̹� �߷�ó���� �ϴ� ��ü�� ��쿡�� �� ���� �޾Ƶ� �����̴�.
	// ����, �߷�ó���� ���� �ʴ� ��ü�� ��� Prev = false �޴� ��ü��� true�� ������ ���̴�.
	m_bGravity = false;
	m_bMove = false;
}

void CMonster::IdleAnimation()
{
}

void CMonster::DefaultIdleAnimation()
{
}

void CMonster::HitStart(CCollider * pSrc, CCollider * pDest, float fTime)
{
	// ���� ��ü�� �������� �Ǵ��Ѵ�.

	CObj* pDestObj = pDest->GetObj();

	switch (pDestObj->GetObjectType())
	{
		// ó������ ��.
	case OT_STAGE:
		// ���͸� �ø��� ó���� �ٸ� ������ �ϰ� �����Ƿ� ���������� ���� �Լ��� ȣ���Ű�� �ʴ´�.
		return;
	}

	if (m_bBlend)
	{
		// ���Ͱ� ������ ��� ó���� ���´�. : �Ѿ��� �������.
		pSrc->DeletePrevCollisionList(pDest);
		pDest->DeletePrevCollisionList(pSrc);
		GET_SINGLE(CCollisionManager)->DeletePrevCollisionAdj(pSrc->GetSerialNumber(), pDest->GetSerialNumber());
		return;
	}


	switch (pDestObj->GetObjectType())
	{
			case OT_PLAYER_ATTACK:
			{
				if (pDestObj->GetTag() == "Breath")
				{
					if (m_eObjectType == OT_BOSS_DDD)
						return;

					// ���� ��ü�� �Ա��̰� ���� ������Ʈ�� ������ ��쿡 ó������ �ʴ´�.
				}

				CObj::StarEffectCreate(m_tPos); // ���� ��ü�� �߽��� �Ѱ��ش�.
				/*
				// ���� ��ü�� �¾ҵ��� ���� �ش� ������Ʈ�� ���ݷ��� ������� �ؼ� ���� ��ü�� �׿��ְ� ������ ���� ��ü��
				// ������ ������ �ȴ�. �ƹ�ư �����ִ� ó�� �� ������ ���� ó���� �����ϴ� !
				������ ���� �̿��ؼ� �ش� ��ü�� ���� ������ �ƴ��� �� �� �ִ�.
				
				Dest�� ����. src �� ����
				*/

				CEffect* pDestEffect = (CEffect*)pDestObj;

				// �´� ������.
				SIDE_DIR eCollisionDir = pDestEffect->GetDir();
				MoveDisEnable(1.f, eCollisionDir);
				SetKnockBack(0.8f, pDestEffect->GetKnockBackPower(), eCollisionDir);

				// ���⼺�� �����ش�.

				// �Ա��� ����� �ʿ�� ���µ�, �ϳ��� ������Ʈ�� ���� ó���� �� ���Ŀ��� �ٸ� ��ü�� �浹���� �ʵ��� �ؾ� �Ѵ�.

				if (!pDestEffect->GetDuplexCollision())
				{
					pDest->DisEnable();
				}
				// ���� �浹�� Ȱ��ȭ�� �Ǿ����� ���� ���(�ѹ� �浹�ϰ� ��)����, �浹ü�� ��Ȱ��ȭ ��Ų��.

				// ��, �Ա�, Star�� ���ؼ� ���Ϳ� �浹�� ��쿡 DieTime�� �������༭ ������� ó���� ���ش�.
				// ���Ϳ� �浹�� �Ͼ ��쿡 ������ �߽��� �������� ����Ʈ�� �������ְ� ������, ���⼭
				// ���� �� ����Ʈ�� ������ �ʿ䰡 ����.


				SetBlend(0.4f, 2);

				// �÷��̾��� �Ѿ��� ���� �������� ���� ó���� �޶����� �κ��̴�.
				Damage(pDestEffect->GetDamage());


			}
			break;
	}

	return;
	
}

void CMonster::SetDir(SIDE_DIR eDir)
{
	m_eDir = eDir;

	IdleAnimation();

}


