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
	// �÷��̾ ���ؼ� ������ �ϴµ�, �ܼ��� �¿쿡 ���� ������ �Ѵ�.
	
	/*
	
			pBullet = CObj::CreateCloneObject<CBullet>(SWORD_BULLET_LEFT, "SwordBulletL", "Default", m_pScene);
		pBullet->SetPos(GetPos() + Position(-150.f, 20.f * sinf(m_fSwordAttackTime * 20.f)));


		pBullet->SetPlayTime(0.3f);
		pBullet->SetMoveSpeed(1000.f);
	*/
	CBullet* pBullet = nullptr;

	if (m_tPos.x < m_pPlayer->GetPos().x)
	{
		// ����������
		pBullet = CObj::CreateCloneObject<CBullet>(BLADE_BULLET_RIGHT, BLADE_BULLET_RIGHT, "Default", m_pScene);
		pBullet->SetAngle(0.f);
		pBullet->SetPos(GetPos() + Position(100.f, -40.f));
		
	}
	else
	{
		// ��������
		pBullet = CObj::CreateCloneObject<CBullet>(BLADE_BULLET_LEFT, BLADE_BULLET_LEFT, "Default", m_pScene);
		pBullet->SetAngle(PI);
		pBullet->SetPos(GetPos() + Position(-100.f, -40.f));



	}



	// �Ѿ��� �����Ѵ�.
	
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

	// �ִϸ��̼��� �߰��Ѵ�.

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


	// ���߿� ó��.
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

	// ���� ChangeTime ���� ������ �� �ʰ� �¿�� �̵����� ���Ѵ�.

	if (m_bMove)
	{
		m_fChangeTime += fTime;

		// ��忡 ���� ó���� �ٸ��� �Ѵ�.


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

			}
			else
			{


				if (m_tPos.x < tPlayerPos.x)
					m_eDir = SD_RIGHT;
				else
					m_eDir = SD_LEFT;

				// ������ �Ÿ��� �ִ� ���.

				// �÷��̾��� ��ġ�� ���ؼ� �Ѿ� ����.
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
