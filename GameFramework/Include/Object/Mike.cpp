#include "Mike.h"
#include "../Animation/Animation.h"
#include "../Collision/Collider.h"
#include "../Math.h"
#include "../Object/Bullet.h"
#include "../Sound/SoundManager.h"

enum MIKE_CLIP
{
	MC_WALK,
	MC_ATTACK,
	MC_HIT,
	MC_END
};

const char* g_pMikeAnimation[SD_END][MC_END] =
{
	{"MikeWalkLeft", "MikeAttackLeft", "MikeHitLeft"},
	{"MikeWalkRight", "MikeAttackRight", "MikeHitRight"}
};

CMike::CMike()
{
	m_pPlayer = nullptr;
	m_bBullet = false;
}


CMike::CMike(const CMike & obj) :
	CMonster(obj)
{
	m_pPlayer = nullptr;
	m_bBullet = false;
}

CMike::~CMike()
{
	SAFE_RELEASE(m_pPlayer);
}

void CMike::BulletCreate()
{
	// 중심이 되는  Angle이 들어오면 이것을 기준으로 += 30도 정도로 설정한다.

	float fAngle = -PI / 6.f;  // 오른쪽으로 쏘는 상황.

	if (m_eDir == SD_LEFT)
		fAngle = PI - PI / 6.f; // 왼쪽으로 쏘는 상황.

	for (int i = 0; i < 3; ++i)
	{
		CBullet* pBullet = CObj::CreateCloneObject<CBullet>(MIKE_BULLET, MIKE_BULLET, "Default", m_pScene);
		pBullet->SetAngle(fAngle);

		switch (m_eDir)
		{
		case SD_LEFT:
			pBullet->SetPos(GetPos() + Position(-100.f, 0.f));

			break;

		case SD_RIGHT:
			pBullet->SetPos(GetPos() + Position(100.f, 0.f));
			break;
		}

		fAngle += PI / 6.f;

		pBullet->SetPlayTime(1.f);
		pBullet->SetMoveSpeed(300.f);

	}


}

void CMike::Start()
{
	CMonster::Start();

	m_pPlayer = m_pScene->FindObject("Kirby");

	CCollider* pCollider = FindCollider();

	if (pCollider)
		pCollider->SetCollisionCallback<CMike>(CS_BLOCK, &CMike::HitStart, this);
}

bool CMike::Init()
{
	if (!CMonster::Init())
		return false;

	CreateAnimation("MikeAnimation");

	// Mike MC_WALK
	m_pAnimation->AddClip(g_pMikeAnimation[SD_LEFT][MC_WALK]);
	m_pAnimation->SetClipTexture(g_pMikeAnimation[SD_LEFT][MC_WALK], "AbilityMonsterLeft", TEXT("AbilityMonsterLeft.bmp"));

	m_pAnimation->AddClip(g_pMikeAnimation[SD_RIGHT][MC_WALK]);
	m_pAnimation->SetClipTexture(g_pMikeAnimation[SD_RIGHT][MC_WALK], "AbilityMonsterRight", TEXT("AbilityMonsterRight.bmp"));

	// Mike MC_ATTACK
	m_pAnimation->AddClip(g_pMikeAnimation[SD_LEFT][MC_ATTACK]);
	m_pAnimation->SetClipTexture(g_pMikeAnimation[SD_LEFT][MC_ATTACK], "AbilityMonsterLeft", TEXT("AbilityMonsterLeft.bmp"));

	m_pAnimation->AddClip(g_pMikeAnimation[SD_RIGHT][MC_ATTACK]);
	m_pAnimation->SetClipTexture(g_pMikeAnimation[SD_RIGHT][MC_ATTACK], "AbilityMonsterRight", TEXT("AbilityMonsterRight.bmp"));


	// Mike MC_HIT
	m_pAnimation->AddClip(g_pMikeAnimation[SD_LEFT][MC_HIT]);
	m_pAnimation->SetClipTexture(g_pMikeAnimation[SD_LEFT][MC_HIT], "AbilityMonsterLeft", TEXT("AbilityMonsterLeft.bmp"));

	m_pAnimation->AddClip(g_pMikeAnimation[SD_RIGHT][MC_HIT]);
	m_pAnimation->SetClipTexture(g_pMikeAnimation[SD_RIGHT][MC_HIT], "AbilityMonsterRight", TEXT("AbilityMonsterRight.bmp"));


	m_pAnimation->LoadClip("../Bin/Save/Mike/MikeAttackLeft.clip");
	m_pAnimation->LoadClip("../Bin/Save/Mike/MikeAttackRight.clip");

	m_pAnimation->LoadClip("../Bin/Save/Mike/MikeHitLeft.clip");
	m_pAnimation->LoadClip("../Bin/Save/Mike/MikeHitRight.clip");

	m_pAnimation->LoadClip("../Bin/Save/Mike/MikeWalkLeft.clip");
	m_pAnimation->LoadClip("../Bin/Save/Mike/MikeWalkRight.clip");




	ChangeDefaultClip();
	m_pAnimation->SetTextureColorKeyAll(0, 72, 80);

	SetStaticSize(70.f, 100.f);
	/*
	CreateAnimation("BladeAnimation");

	// Blade BC_IDLE
	m_pAnimation->AddClip(g_pBladeAnimation[SD_LEFT][BC_IDLE]);
	m_pAnimation->SetClipTexture(g_pBladeAnimation[SD_LEFT][BC_IDLE], "AbilityMonsterLeft", TEXT("AbilityMonsterLeft.bmp"));

	m_pAnimation->AddClip(g_pBladeAnimation[SD_RIGHT][BC_IDLE]);
	m_pAnimation->SetClipTexture(g_pBladeAnimation[SD_RIGHT][BC_IDLE], "AbilityMonsterRight", TEXT("AbilityMonsterRight.bmp"));


	// Blade BC_WALK
	m_pAnimation->AddClip(g_pBladeAnimation[SD_LEFT][BC_WALK]);
	m_pAnimation->SetClipTexture(g_pBladeAnimation[SD_LEFT][BC_WALK], "AbilityMonsterLeft", TEXT("AbilityMonsterLeft.bmp"));

	m_pAnimation->AddClip(g_pBladeAnimation[SD_RIGHT][BC_WALK]);
	m_pAnimation->SetClipTexture(g_pBladeAnimation[SD_RIGHT][BC_WALK], "AbilityMonsterRight", TEXT("AbilityMonsterRight.bmp"));


	// Blade BC_ATTACK
	m_pAnimation->AddClip(g_pBladeAnimation[SD_LEFT][BC_ATTACK]);
	m_pAnimation->SetClipTexture(g_pBladeAnimation[SD_LEFT][BC_ATTACK], "AbilityMonsterLeft", TEXT("AbilityMonsterLeft.bmp"));

	m_pAnimation->AddClip(g_pBladeAnimation[SD_RIGHT][BC_ATTACK]);
	m_pAnimation->SetClipTexture(g_pBladeAnimation[SD_RIGHT][BC_ATTACK], "AbilityMonsterRight", TEXT("AbilityMonsterRight.bmp"));


	// Blade BC_HIT
	m_pAnimation->AddClip(g_pBladeAnimation[SD_LEFT][BC_HIT]);
	m_pAnimation->SetClipTexture(g_pBladeAnimation[SD_LEFT][BC_HIT], "AbilityMonsterLeft", TEXT("AbilityMonsterLeft.bmp"));

	m_pAnimation->AddClip(g_pBladeAnimation[SD_RIGHT][BC_HIT]);
	m_pAnimation->SetClipTexture(g_pBladeAnimation[SD_RIGHT][BC_HIT], "AbilityMonsterRight", TEXT("AbilityMonsterRight.bmp"));


	ChangeDefaultClip();
	m_pAnimation->SetTextureColorKeyAll(0, 72, 80);

	SetStaticSize(74.f, 74.f);
	*/

	return true;
}

void CMike::Update(float fTime)
{
	CMonster::Update(fTime);


	

	// 들어온 ChangeTime 값을 가지고 몇 초간 좌우로 이동할지 정한다.

	if (m_bMove)
	{
		m_fChangeTime += fTime;

		// 모드에 따라서 처리를 다르게 한다.

		if (m_bBullet)
		{
			PAnimationClip pClip = m_pAnimation->GetClip();

			if ((pClip->strName == "MikeAttackLeft" || pClip->strName == "MikeAttackRight") && pClip->iFrame == 1)
			{
				BulletCreate();
				m_bBullet = false;

				GET_SINGLE(CSoundManager)->Play("MikeAttack", CT_EFFECT);
			}
			// 총알을 생성해야 하는 경우에 현재 애니메이션(공격 애니메이션)의 프레임이 1이라면,  (공격하는 순간)
			// 총알을 생성해주고 다음에는 생성하지 않도록 false를 준다.
		}

		switch (m_eMode)
		{
		case MM_IDLE:
		{

			m_fChangeTime += fTime;

			//  자신의 스피드 만큼 이동한다.
			MoveX(fTime);

			Position tPlayerPos = m_pPlayer->GetPos();

			float fDist = CMath::GetDist(m_tPos, tPlayerPos);

			if (fDist >= 400.f)
			{
				// 일정 거리가 아닌 경우.
				// 방향을 바꿔서 다시 이동한다.

				if (m_fChangeTime >= m_fChangePlayTime)
				{
					m_fChangeTime -= m_fChangePlayTime;
					DirChange();

					IdleAnimation();
					DefaultIdleAnimation();
				}

			

				// 모드는 Idle 그대로.

				m_fChangePlayTime = 5.f;

			}
			else
			{


				if (m_tPos.x < tPlayerPos.x)
					m_eDir = SD_RIGHT;
				else
					m_eDir = SD_LEFT;

				// 일정한 거리에 있는 경우.

				// 플레이어의 위치를 향해서 총알 생성.
				m_bBullet = true;

				//  Attack Motion
				m_pAnimation->ChangeClip(g_pMikeAnimation[m_eDir][MC_ATTACK]);
				m_pAnimation->SetDefaultClip(g_pMikeAnimation[m_eDir][MC_ATTACK]);

				m_eMode = MM_ATTACK;

				m_fChangeTime = 0.f;
				m_fChangePlayTime = 2.5f;
			}


		}
		break;

		case MM_ATTACK:

			if (m_fChangeTime >= m_fChangePlayTime)
			{
				// 공격하는 동안에는 시간을 재줘야 한다.

				// 공격을 한 이후에 다시 공격을 할지 / 이동을 할지 여부를 검사한다.

				Position tPlayerPos = m_pPlayer->GetPos();
				
				float fDist = CMath::GetDist(m_tPos, tPlayerPos);

				if (fDist >= 400.f)
				{
					// 일정한 거리 밖이라면, IDLE상태로 돌리고,  해당 방향을 향해서 이동 하도록 한다.

					m_eMode = MM_IDLE;

					IdleAnimation();
					DefaultIdleAnimation();

					m_fChangePlayTime = 5.f;
					m_fChangeTime = 0.f;

			


				}

				else
				{

					if (m_tPos.x < tPlayerPos.x)
						m_eDir = SD_RIGHT;
					else
						m_eDir = SD_LEFT;

					// 한번 공격을 한 이후에 다시 일정한 거리에 있다면, 다시 공격을 날린다.
					m_bBullet = true;

					//  Attack Motion
					m_pAnimation->ChangeClip(g_pMikeAnimation[m_eDir][MC_ATTACK]);
					m_pAnimation->SetDefaultClip(g_pMikeAnimation[m_eDir][MC_ATTACK]);

					m_fChangeTime = 0.f;

				}

			}
			break;
		}
	}
}

void CMike::LateUpdate(float fTime)
{
	CMonster::LateUpdate(fTime);

}

void CMike::Collision(float fTime)
{
	CMonster::Collision(fTime);

}

void CMike::PrevRender(HDC hDC, float fTime)
{
	CMonster::PrevRender(hDC, fTime);

}

void CMike::Render(HDC hDC, float fTime)
{
	CMonster::Render(hDC, fTime);


}

void CMike::PostRender(HDC hDC, float fTime)
{
	CMonster::PostRender(hDC, fTime);

}

CMike * CMike::Clone() const
{
	return new CMike(*this);
}

void CMike::HitAnimation()
{

	CMonster::HitAnimation();

	m_pAnimation->ChangeClip(g_pMikeAnimation[m_eDir][MC_HIT]);

}

void CMike::AddClipName(vector<string>& vecClipName)
{
	for (int j = 0; j < MC_END; ++j)
	{
		for (int i = 0; i < SD_END; ++i)
		{
			vecClipName.push_back(g_pMikeAnimation[i][j]);
		}
	}
}

void CMike::IdleAnimation()
{
	m_pAnimation->ChangeClip(g_pMikeAnimation[m_eDir][MC_WALK]);

}

void CMike::HitStart(CCollider * pSrc, CCollider * pDest, float fTime)
{
	CMonster::HitStart(pSrc, pDest, fTime);
}

void CMike::DefaultIdleAnimation()
{
	m_pAnimation->SetDefaultClip(g_pMikeAnimation[m_eDir][MC_WALK]);

}
