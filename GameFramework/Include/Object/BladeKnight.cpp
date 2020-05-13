#include "BladeKnight.h"
#include "../Animation/Animation.h"
#include "../Collision/Collider.h"
#include "../Math.h"
#include "../Object/Bullet.h"


enum BLADE_CLIP
{
	BC_IDLE,
	BC_ATTACK,
	BC_HIT,
	BC_END
};

const char* g_pBladeAnimation[SD_END][BC_END] =
{
	{"BladeIdleLeft", "BladeAttackLeft", "BladeHitLeft"},
	{"BladeIdleRight", "BladeAttackRight", "BladeHitRight"}
};

CBladeKnight::CBladeKnight() :
	m_pPlayer(nullptr)
{
}


CBladeKnight::CBladeKnight(const CBladeKnight & obj) :
	CMonster(obj)
{
	m_pPlayer = nullptr;
}

CBladeKnight::~CBladeKnight()
{
	SAFE_RELEASE(m_pPlayer);
}

void CBladeKnight::BulletCreate()
{
	// 플레이어를 향해서 공격을 하는데, 단순히 좌우에 대한 공격을 한다.
	
	/*
	
			pBullet = CObj::CreateCloneObject<CBullet>(SWORD_BULLET_LEFT, "SwordBulletL", "Default", m_pScene);
		pBullet->SetPos(GetPos() + Position(-150.f, 20.f * sinf(m_fSwordAttackTime * 20.f)));


		pBullet->SetPlayTime(0.3f);
		pBullet->SetMoveSpeed(1000.f);
	*/
	CBullet* pBullet = nullptr;

	if (m_tPos.x < m_pPlayer->GetPos().x)
	{
		// 오른쪽으로
		pBullet = CObj::CreateCloneObject<CBullet>(BLADE_BULLET_RIGHT, BLADE_BULLET_RIGHT, "Default", m_pScene);
		pBullet->SetAngle(0.f);
		pBullet->SetPos(GetPos() + Position(100.f, -40.f));
		
	}
	else
	{
		// 왼쪽으로
		pBullet = CObj::CreateCloneObject<CBullet>(BLADE_BULLET_LEFT, BLADE_BULLET_LEFT, "Default", m_pScene);
		pBullet->SetAngle(PI);
		pBullet->SetPos(GetPos() + Position(-100.f, -40.f));



	}



	// 총알을 생성한다.
	
}

void CBladeKnight::Start()
{
	CMonster::Start();
	
	m_pPlayer = m_pScene->FindObject("Kirby");


	CCollider* pCollider = FindCollider();

	if (pCollider)
		pCollider->SetCollisionCallback<CBladeKnight>(CS_BLOCK, &CBladeKnight::HitStart, this);
}

bool CBladeKnight::Init()
{
	if (!CMonster::Init())
		return false;

	// 애니메이션을 추가한다.

	CreateAnimation("BladeAnimation");

	// Blade BC_IDLE
	m_pAnimation->AddClip(g_pBladeAnimation[SD_LEFT][BC_IDLE]);
	m_pAnimation->SetClipTexture(g_pBladeAnimation[SD_LEFT][BC_IDLE], "AbilityMonsterLeft", TEXT("AbilityMonsterLeft.bmp"));

	m_pAnimation->AddClip(g_pBladeAnimation[SD_RIGHT][BC_IDLE]);
	m_pAnimation->SetClipTexture(g_pBladeAnimation[SD_RIGHT][BC_IDLE], "AbilityMonsterRight", TEXT("AbilityMonsterRight.bmp"));


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

	m_pAnimation->LoadClip("../Bin/Save/BladeKnight/BladeAttackLeft.clip");
	m_pAnimation->LoadClip("../Bin/Save/BladeKnight/BladeAttackRight.clip");


	m_pAnimation->LoadClip("../Bin/Save/BladeKnight/BladeHitLeft.clip");
	m_pAnimation->LoadClip("../Bin/Save/BladeKnight/BladeHitRight.clip");


	m_pAnimation->LoadClip("../Bin/Save/BladeKnight/BladeIdleLeft.clip");
	m_pAnimation->LoadClip("../Bin/Save/BladeKnight/BladeIdleRight.clip");


	ChangeDefaultClip();
	m_pAnimation->SetTextureColorKeyAll(0, 72, 80);

	SetStaticSize(74.f, 74.f);
	/*
	
	if (!CMonster::Init())
		return false;

	CreateAnimation("GrizooAnimation");


	m_pAnimation->AddClip(g_pGrizooAnimationName[SD_LEFT][GR_WALK], AO_LOOP, 1.f);
	m_pAnimation->SetClipTexture(g_pGrizooAnimationName[SD_LEFT][GR_WALK], "NormalMonsterLeft", TEXT("NormalMonsterLeft.bmp"));

	m_pAnimation->AddClip(g_pGrizooAnimationName[SD_RIGHT][GR_WALK], AO_LOOP, 1.f);
	m_pAnimation->SetClipTexture(g_pGrizooAnimationName[SD_RIGHT][GR_WALK], "NormalMonsterRight", TEXT("NormalMonsterRight.bmp"));


	m_pAnimation->AddClip(g_pGrizooAnimationName[SD_LEFT][GR_HIT], AO_LOOP, 1.f);
	m_pAnimation->SetClipTexture(g_pGrizooAnimationName[SD_LEFT][GR_HIT], "NormalMonsterLeft", TEXT("NormalMonsterLeft.bmp"));

	m_pAnimation->AddClip(g_pGrizooAnimationName[SD_RIGHT][GR_HIT], AO_LOOP, 1.f);
	m_pAnimation->SetClipTexture(g_pGrizooAnimationName[SD_RIGHT][GR_HIT], "NormalMonsterRight", TEXT("NormalMonsterRight.bmp"));


	m_pAnimation->LoadClip("../Bin/Save/Grizoo/GrizooWalkLeft.clip");
	m_pAnimation->LoadClip("../Bin/Save/Grizoo/GrizooWalkRight.clip");


	m_pAnimation->LoadClip("../Bin/Save/Grizoo/GrizooHitLeft.clip");
	m_pAnimation->LoadClip("../Bin/Save/Grizoo/GrizooHitRight.clip");


	// 나중에 처리.
	m_pAnimation->FindClip(g_pGrizooAnimationName[SD_RIGHT][GR_HIT])->eOption = AO_ONCE_RETURN;
	m_pAnimation->FindClip(g_pGrizooAnimationName[SD_LEFT][GR_HIT])->eOption = AO_ONCE_RETURN;


	m_pAnimation->ChangeClip(m_pAnimation->GetDefaultClip());
	m_pAnimation->SetTextureColorKeyAll(0, 72, 80);

	SetStaticSize(130.f, 120.f);
	*/

	return true;
}

void CBladeKnight::Update(float fTime)
{
	CMonster::Update(fTime);

	// 들어온 ChangeTime 값을 가지고 몇 초간 좌우로 이동할지 정한다.

	if (m_bMove)
	{
		m_fChangeTime += fTime;

		// 모드에 따라서 처리를 다르게 한다.


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

			}
			else
			{


				if (m_tPos.x < tPlayerPos.x)
					m_eDir = SD_RIGHT;
				else
					m_eDir = SD_LEFT;

				// 일정한 거리에 있는 경우.

				// 플레이어의 위치를 향해서 총알 생성.
				BulletCreate();

				//  Attack Motion
				m_pAnimation->ChangeClip(g_pBladeAnimation[m_eDir][BC_ATTACK]);
				m_pAnimation->SetDefaultClip(g_pBladeAnimation[m_eDir][BC_ATTACK]);

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
					BulletCreate();

					//  Attack Motion
					m_pAnimation->ChangeClip(g_pBladeAnimation[m_eDir][BC_ATTACK]);
					m_pAnimation->SetDefaultClip(g_pBladeAnimation[m_eDir][BC_ATTACK]);

					m_fChangeTime = 0.f;

				}

			}
			break;
		}
	}
}

void CBladeKnight::LateUpdate(float fTime)
{
	CMonster::LateUpdate(fTime);

}

void CBladeKnight::Collision(float fTime)
{
	CMonster::Collision(fTime);

}

void CBladeKnight::PrevRender(HDC hDC, float fTime)
{
	CMonster::PrevRender(hDC, fTime);

}

void CBladeKnight::Render(HDC hDC, float fTime)
{
	CMonster::Render(hDC, fTime);


}

void CBladeKnight::PostRender(HDC hDC, float fTime)
{
	CMonster::PostRender(hDC, fTime);

}

CBladeKnight * CBladeKnight::Clone() const
{
	return new CBladeKnight(*this);
}

void CBladeKnight::HitAnimation()
{
	CMonster::HitAnimation();

	m_pAnimation->ChangeClip(g_pBladeAnimation[m_eDir][BC_HIT]);

}

void CBladeKnight::AddClipName(vector<string>& vecClipName)
{
	for (int j = 0; j < BC_END; ++j)
	{
		for (int i = 0; i < SD_END; ++i)
		{
			vecClipName.push_back(g_pBladeAnimation[i][j]);
		}
	}
}

void CBladeKnight::IdleAnimation()
{
	m_pAnimation->ChangeClip(g_pBladeAnimation[m_eDir][BC_IDLE]);

}

void CBladeKnight::HitStart(CCollider * pSrc, CCollider * pDest, float fTime)
{
	CMonster::HitStart(pSrc, pDest, fTime);
}

void CBladeKnight::DefaultIdleAnimation()
{
	m_pAnimation->SetDefaultClip(g_pBladeAnimation[m_eDir][BC_IDLE]);

}
