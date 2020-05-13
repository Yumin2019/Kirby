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
	// 이미 처리가 되어 있는 것이 있다면 하지 않는다.


	m_bStop = true;
	m_fStopTime = 0.f;
	m_fStopLimitTime = fStopLimitTime;
	m_bMove = false;

	// 초기화 !
	m_fChangeTime = 0.f;


	// 충돌방향과 반대로 움직이도록 설정한다. (날라간 이후에)
	switch (eCollisionDir)
	{
	case SD_LEFT:

		m_eDir = SD_RIGHT;
		break;

	case SD_RIGHT:
		m_eDir = SD_LEFT;
		break;
	}

	// 아파해야 한다.
	HitAnimation();

	// 바뀐 방향에 대해서 Idle
	DefaultIdleAnimation();

}

void CMonster::Damage(int iDamage)
{
		CCharacter::Damage(iDamage);

		if (IsDie())
		{
			SetDieTime(0.5f, true);

			// 몬스터가 죽은 경우에는 0.5s 이후에 이펙트를 생성하도록 한다.

			// 그리고 사운드를 출력해준다.
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
	// 자신이 설정된 방향대로 이동하는 함수이다.
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
			// 이전에 중력 처리를 하던 몬스터라면 중력 처리를 다시 시작하도록 한다.

			switch (m_eObjectType)
			{
				case OT_BOSS_DDD:
				{
					CDDD* pDDD = (CDDD*)this;
					pDDD->InitIdle();

					pDDD->SetFalling(pDDD->GetJump());

					// 점프를 하고 있었다면, falling = true로 처리해서, 패턴에 대한 처리를 하지 않도록 한다.

				}


				// DDD가 마이크의 공격을 받은 경우에 초기 상태로 돌려버린다.

				break;
			}
		}

		// Hit Animation이 잡혀있는 동안에는 무조건 Hit애니메이션이 호출이 되도록 한다.
		// 마이크의 공격 시간 만큼은 무조건 이 처리를 하도록 하면 되는 것이다.
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
					m_fChangePlayTime = m_fInitChangeTime; // 초기의 상태로 완벽하게 돌리기 위한 변수.
					m_eMode = MM_IDLE;

					IdleAnimation();

					m_bChange = true;

					break;

					case OT_BOSS_DDD:

					{
						CDDD* pDDD = (CDDD*)this;

						if (!pDDD->GetDie())
						{
							// 죽은 이후의 처리가 아니라면.
							pDDD->InitIdle();
							pDDD->SetFalling(pDDD->GetJump());
						}
						else
						{
							// 죽은 이후의 처리를 하려면..
							int i = 0;
						}
						// Idle 모션으로 바꿔버리는데, 만약 점프를 하고 있었다면,  falling처리를 하여 이 바닥에 떨어질 동안에는 패턴의 처리를 하지 않도록 한다.
						// 즉, 맞아서 바닥에 붙어있었다면 바로 패턴 시작, 점프 상태에서 맞은 경우에는 떨어진 시간을 제외하고 패턴이 시작된다.
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
	// 복사생성자에서 이미 중력처리를 하는 물체인 경우에는 이 값을 받아둔 상태이다.
	// 따라서, 중력처리를 받지 않는 물체의 경우 Prev = false 받는 물체라면 true인 상태일 것이다.
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
	// 맞은 물체가 무엇인지 판단한다.

	CObj* pDestObj = pDest->GetObj();

	switch (pDestObj->GetObjectType())
	{
		// 처리막는 곳.
	case OT_STAGE:
		// 몬스터를 올리는 처리는 다른 곳에서 하고 있으므로 스테이지에 대한 함수를 호출시키지 않는다.
		return;
	}

	if (m_bBlend)
	{
		// 몬스터가 무적인 경우 처리를 막는다. : 총알은 상관없음.
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

					// 맞은 물체가 입김이고 현재 오브젝트가 보스인 경우에 처리하지 않는다.
				}

				CObj::StarEffectCreate(m_tPos); // 맞은 물체의 중심을 넘겨준다.
				/*
				// 무슨 물체와 맞았든지 간에 해당 오브젝트의 공격력을 기반으로 해서 죽일 물체는 죽여주고 죽이지 않을 물체는
				// 죽이지 않으면 된다. 아무튼 날려주는 처리 및 무적에 대한 처리는 동일하다 !
				데미지 값을 이용해서 해당 물체가 죽을 것인지 아닌지 알 수 있다.
				
				Dest가 공격. src 가 몬스터
				*/

				CEffect* pDestEffect = (CEffect*)pDestObj;

				// 맞는 방향대로.
				SIDE_DIR eCollisionDir = pDestEffect->GetDir();
				MoveDisEnable(1.f, eCollisionDir);
				SetKnockBack(0.8f, pDestEffect->GetKnockBackPower(), eCollisionDir);

				// 방향성을 구해준다.

				// 입김이 사라질 필요는 없는데, 하나의 오브젝트에 대한 처리를 한 이후에는 다른 물체와 충돌하지 않도록 해야 한다.

				if (!pDestEffect->GetDuplexCollision())
				{
					pDest->DisEnable();
				}
				// 다중 충돌이 활성화가 되어있지 않은 경우(한번 충돌하고 끝)에만, 충돌체를 비활성화 시킨다.

				// 별, 입김, Star에 대해서 몬스터와 충돌인 경우에 DieTime을 설정해줘서 사라지는 처리를 해준다.
				// 몬스터와 충돌이 일어난 경우에 몬스터의 중심을 기준으로 이펙트를 생성해주고 있으니, 여기서
				// 굳이 또 이펙트를 생성할 필요가 없다.


				SetBlend(0.4f, 2);

				// 플레이어의 총알이 갖는 데미지에 따라서 처리가 달라지는 부분이다.
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


