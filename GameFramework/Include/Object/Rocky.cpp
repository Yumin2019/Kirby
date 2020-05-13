#include "Rocky.h"
#include "../Animation/Animation.h"
#include "../Collision/Collider.h"

enum ROCKY_CLIP
{
	RC_IDLE, // STOP
	RC_WALK, // WALK
	RC_HIT,
	RC_END
};

const char* g_pRockyAnimation[SD_END][RC_END] =
{
	{"RockyIdleLeft", "RockyWalkLeft", "RockyHitLeft"},
	{"RockyIdleRight", "RockyWalkRight", "RockyHitRight"}
};

CRocky::CRocky()
{
}


CRocky::CRocky(const CRocky & obj) :
	CMonster(obj)
{

}

CRocky::~CRocky()
{
}

void CRocky::Start()
{
	CMonster::Start();



	CCollider* pCollider = FindCollider();

	if (pCollider)
		pCollider->SetCollisionCallback<CRocky>(CS_BLOCK, &CRocky::HitStart, this);

	
}

bool CRocky::Init()
{
	if (!CMonster::Init())
		return false;

	CreateAnimation("RockyAnimation");

	// Rocky RC_IDLE
	m_pAnimation->AddClip(g_pRockyAnimation[SD_LEFT][RC_IDLE]);
	m_pAnimation->SetClipTexture(g_pRockyAnimation[SD_LEFT][RC_IDLE], "AbilityMonsterLeft", TEXT("AbilityMonsterLeft.bmp"));

	m_pAnimation->AddClip(g_pRockyAnimation[SD_RIGHT][RC_IDLE]);
	m_pAnimation->SetClipTexture(g_pRockyAnimation[SD_RIGHT][RC_IDLE], "AbilityMonsterRight", TEXT("AbilityMonsterRight.bmp"));


	// Rocky RC_WALK
	m_pAnimation->AddClip(g_pRockyAnimation[SD_LEFT][RC_WALK]);
	m_pAnimation->SetClipTexture(g_pRockyAnimation[SD_LEFT][RC_WALK], "AbilityMonsterLeft", TEXT("AbilityMonsterLeft.bmp"));

	m_pAnimation->AddClip(g_pRockyAnimation[SD_RIGHT][RC_WALK]);
	m_pAnimation->SetClipTexture(g_pRockyAnimation[SD_RIGHT][RC_WALK], "AbilityMonsterRight", TEXT("AbilityMonsterRight.bmp"));



	// Rocky RC_HIT
	m_pAnimation->AddClip(g_pRockyAnimation[SD_LEFT][RC_HIT]);
	m_pAnimation->SetClipTexture(g_pRockyAnimation[SD_LEFT][RC_HIT], "AbilityMonsterLeft", TEXT("AbilityMonsterLeft.bmp"));

	m_pAnimation->AddClip(g_pRockyAnimation[SD_RIGHT][RC_HIT]);
	m_pAnimation->SetClipTexture(g_pRockyAnimation[SD_RIGHT][RC_HIT], "AbilityMonsterRight", TEXT("AbilityMonsterRight.bmp"));


	m_pAnimation->LoadClip("../Bin/Save/Rocky/RockyHitLeft.clip");
	m_pAnimation->LoadClip("../Bin/Save/Rocky/RockyHitRight.clip");

	m_pAnimation->LoadClip("../Bin/Save/Rocky/RockyIdleLeft.clip");
	m_pAnimation->LoadClip("../Bin/Save/Rocky/RockyIdleRight.clip");

	m_pAnimation->LoadClip("../Bin/Save/Rocky/RockyWalkLeft.clip");
	m_pAnimation->LoadClip("../Bin/Save/Rocky/RockyWalkRight.clip");

	ChangeDefaultClip();
	m_pAnimation->SetTextureColorKeyAll(0, 72, 80);

	SetStaticSize(80.f, 80.f);

	return true;
}

void CRocky::Update(float fTime)
{
	CMonster::Update(fTime);


	if (m_bMove)
	{
		m_fChangeTime += fTime;

		// 모드에 따라서 처리를 다르게 한다.


		switch (m_eMode)
		{
		case MM_IDLE:

			//  자신의 스피드 만큼 이동한다.
			MoveX(fTime);

			if (m_fChangeTime >= m_fChangePlayTime)
			{
				// 한 쪽 방향으로 걷다가 일정한 시간이 지난 경우에는 멈춘다.
				m_eMode = MM_STOP;
				m_fChangeTime = 0.f;
				m_fChangePlayTime = 5.f;

				m_pAnimation->ChangeClip(g_pRockyAnimation[m_eDir][RC_IDLE]);

			}

			break;

		case MM_STOP:

			if (m_fChangeTime >= m_fChangePlayTime)
			{
				// 공격이 끝난 이후에는 방향을 바꿔서 움직이도록 해야 한다.
				m_eMode = MM_IDLE;
				m_fChangeTime = 0.f;
				m_fChangePlayTime = 3.f;


				DirChange();

				m_pAnimation->ChangeClip(g_pRockyAnimation[m_eDir][RC_WALK]);
				
			}
			break;
		}
	}
	
}

void CRocky::LateUpdate(float fTime)
{
	CMonster::LateUpdate(fTime);

}

void CRocky::Collision(float fTime)
{
	CMonster::Collision(fTime);

}

void CRocky::PrevRender(HDC hDC, float fTime)
{
	CMonster::PrevRender(hDC, fTime);

}

void CRocky::Render(HDC hDC, float fTime)
{
	CMonster::Render(hDC, fTime);


}

void CRocky::PostRender(HDC hDC, float fTime)
{
	CMonster::PostRender(hDC, fTime);

}

CRocky * CRocky::Clone() const
{
	return new CRocky(*this);
}

void CRocky::HitAnimation()
{

	CMonster::HitAnimation();

	m_pAnimation->ChangeClip(g_pRockyAnimation[m_eDir][RC_HIT]);

}

void CRocky::AddClipName(vector<string>& vecClipName)
{
	for (int j = 0; j < RC_END; ++j)
	{
		for (int i = 0; i < SD_END; ++i)
		{
			vecClipName.push_back(g_pRockyAnimation[i][j]);
		}
	}
}

void CRocky::IdleAnimation()
{
	m_pAnimation->ChangeClip(g_pRockyAnimation[m_eDir][RC_WALK]);

}

void CRocky::HitStart(CCollider * pSrc, CCollider * pDest, float fTime)
{
	CMonster::HitStart(pSrc, pDest, fTime);
}

void CRocky::DefaultIdleAnimation()
{
	m_pAnimation->SetDefaultClip(g_pRockyAnimation[m_eDir][RC_WALK]);

}
