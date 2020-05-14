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
	// �߽��� �Ǵ�  Angle�� ������ �̰��� �������� += 30�� ������ �����Ѵ�.

	float fAngle = -PI / 6.f;  // ���������� ��� ��Ȳ.

	if (m_eDir == SD_LEFT)
		fAngle = PI - PI / 6.f; // �������� ��� ��Ȳ.

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


	

	// ���� ChangeTime ���� ������ �� �ʰ� �¿�� �̵����� ���Ѵ�.

	if (m_bMove)
	{
		m_fChangeTime += fTime;

		// ��忡 ���� ó���� �ٸ��� �Ѵ�.

		if (m_bBullet)
		{
			PAnimationClip pClip = m_pAnimation->GetClip();

			if ((pClip->strName == "MikeAttackLeft" || pClip->strName == "MikeAttackRight") && pClip->iFrame == 1)
			{
				BulletCreate();
				m_bBullet = false;

				GET_SINGLE(CSoundManager)->Play("MikeAttack", CT_EFFECT);
			}
			// �Ѿ��� �����ؾ� �ϴ� ��쿡 ���� �ִϸ��̼�(���� �ִϸ��̼�)�� �������� 1�̶��,  (�����ϴ� ����)
			// �Ѿ��� �������ְ� �������� �������� �ʵ��� false�� �ش�.
		}

		switch (m_eMode)
		{
		case MM_IDLE:
		{

			m_fChangeTime += fTime;

			//  �ڽ��� ���ǵ� ��ŭ �̵��Ѵ�.
			MoveX(fTime);

			Position tPlayerPos = m_pPlayer->GetPos();

			float fDist = CMath::GetDist(m_tPos, tPlayerPos);

			if (fDist >= 400.f)
			{
				// ���� �Ÿ��� �ƴ� ���.
				// ������ �ٲ㼭 �ٽ� �̵��Ѵ�.

				if (m_fChangeTime >= m_fChangePlayTime)
				{
					m_fChangeTime -= m_fChangePlayTime;
					DirChange();

					IdleAnimation();
					DefaultIdleAnimation();
				}

			

				// ���� Idle �״��.

				m_fChangePlayTime = 5.f;

			}
			else
			{


				if (m_tPos.x < tPlayerPos.x)
					m_eDir = SD_RIGHT;
				else
					m_eDir = SD_LEFT;

				// ������ �Ÿ��� �ִ� ���.

				// �÷��̾��� ��ġ�� ���ؼ� �Ѿ� ����.
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
				// �����ϴ� ���ȿ��� �ð��� ����� �Ѵ�.

				// ������ �� ���Ŀ� �ٽ� ������ ���� / �̵��� ���� ���θ� �˻��Ѵ�.

				Position tPlayerPos = m_pPlayer->GetPos();
				
				float fDist = CMath::GetDist(m_tPos, tPlayerPos);

				if (fDist >= 400.f)
				{
					// ������ �Ÿ� ���̶��, IDLE���·� ������,  �ش� ������ ���ؼ� �̵� �ϵ��� �Ѵ�.

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

					// �ѹ� ������ �� ���Ŀ� �ٽ� ������ �Ÿ��� �ִٸ�, �ٽ� ������ ������.
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
