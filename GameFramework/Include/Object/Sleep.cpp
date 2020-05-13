#include "Sleep.h"
#include "../Animation/Animation.h"
#include "../Collision/Collider.h"


enum SLEEP_CLIP
{
	SC_WALK,
	SC_SLEEP,
	SC_HIT,
	SC_END
};

const char* g_pSleepAnimation[SD_END][SC_END] =
{
	{"SleepWalkLeft", "SleepSleepLeft", "SleepHitLeft"},
	{"SleepWalkRight", "SleepSleepRight", "SleepHitRight"}
};

CSleep::CSleep()
{
}


CSleep::CSleep(const CSleep & obj) :
	CMonster(obj)
{
}

CSleep::~CSleep()
{
}

void CSleep::Start()
{
	CMonster::Start();



	CCollider* pCollider = FindCollider();

	if (pCollider)
		pCollider->SetCollisionCallback<CSleep>(CS_BLOCK, &CSleep::HitStart, this);
}

bool CSleep::Init()
{
	if (!CMonster::Init())
		return false;

	CreateAnimation("SleepAnimation");

	// Sleep SC_WALK
	m_pAnimation->AddClip(g_pSleepAnimation[SD_LEFT][SC_WALK]);
	m_pAnimation->SetClipTexture(g_pSleepAnimation[SD_LEFT][SC_WALK], "AbilityMonsterLeft", TEXT("AbilityMonsterLeft.bmp"));

	m_pAnimation->AddClip(g_pSleepAnimation[SD_RIGHT][SC_WALK]);
	m_pAnimation->SetClipTexture(g_pSleepAnimation[SD_RIGHT][SC_WALK], "AbilityMonsterRight", TEXT("AbilityMonsterRight.bmp"));

	// Sleep SC_SLEEP
	m_pAnimation->AddClip(g_pSleepAnimation[SD_LEFT][SC_SLEEP]);
	m_pAnimation->SetClipTexture(g_pSleepAnimation[SD_LEFT][SC_SLEEP], "AbilityMonsterLeft", TEXT("AbilityMonsterLeft.bmp"));

	m_pAnimation->AddClip(g_pSleepAnimation[SD_RIGHT][SC_SLEEP]);
	m_pAnimation->SetClipTexture(g_pSleepAnimation[SD_RIGHT][SC_SLEEP], "AbilityMonsterRight", TEXT("AbilityMonsterRight.bmp"));

	// Sleep SC_HIT
	m_pAnimation->AddClip(g_pSleepAnimation[SD_LEFT][SC_HIT]);
	m_pAnimation->SetClipTexture(g_pSleepAnimation[SD_LEFT][SC_HIT], "AbilityMonsterLeft", TEXT("AbilityMonsterLeft.bmp"));

	m_pAnimation->AddClip(g_pSleepAnimation[SD_RIGHT][SC_HIT]);
	m_pAnimation->SetClipTexture(g_pSleepAnimation[SD_RIGHT][SC_HIT], "AbilityMonsterRight", TEXT("AbilityMonsterRight.bmp"));

	m_pAnimation->LoadClip("../Bin/Save/Sleep/SleepSleepLeft.clip");
	m_pAnimation->LoadClip("../Bin/Save/Sleep/SleepSleepRight.clip");

	m_pAnimation->LoadClip("../Bin/Save/Sleep/SleepWalkLeft.clip");
	m_pAnimation->LoadClip("../Bin/Save/Sleep/SleepWalkRight.clip");

	m_pAnimation->LoadClip("../Bin/Save/Sleep/SleepHitLeft.clip");
	m_pAnimation->LoadClip("../Bin/Save/Sleep/SleepHitRight.clip");

	ChangeDefaultClip();
	m_pAnimation->SetTextureColorKeyAll(0, 72, 80);

	SetStaticSize(70.f, 70.f);

	return true;
}

void CSleep::Update(float fTime)
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

				m_pAnimation->ChangeClip(g_pSleepAnimation[m_eDir][SC_SLEEP]);

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

				m_pAnimation->ChangeClip(g_pSleepAnimation[m_eDir][SC_WALK]);


			}
			break;
		}
	}
}

void CSleep::LateUpdate(float fTime)
{
	CMonster::LateUpdate(fTime);

}

void CSleep::Collision(float fTime)
{
	CMonster::Collision(fTime);

}

void CSleep::PrevRender(HDC hDC, float fTime)
{
	CMonster::PrevRender(hDC, fTime);

}

void CSleep::Render(HDC hDC, float fTime)
{
	CMonster::Render(hDC, fTime);


}

void CSleep::PostRender(HDC hDC, float fTime)
{
	CMonster::PostRender(hDC, fTime);

}

CSleep * CSleep::Clone() const
{
	return new CSleep(*this);
}

void CSleep::HitAnimation()
{

	CMonster::HitAnimation();

	m_pAnimation->ChangeClip(g_pSleepAnimation[m_eDir][SC_HIT]);

}

void CSleep::AddClipName(vector<string>& vecClipName)
{
	for (int j = 0; j < SC_END; ++j)
	{
		for (int i = 0; i < SD_END; ++i)
		{
			vecClipName.push_back(g_pSleepAnimation[i][j]);
		}
	}
}

void CSleep::IdleAnimation()
{
	m_pAnimation->ChangeClip(g_pSleepAnimation[m_eDir][SC_WALK]);
}

void CSleep::HitStart(CCollider * pSrc, CCollider * pDest, float fTime)
{
	CMonster::HitStart(pSrc, pDest, fTime);
}

void CSleep::DefaultIdleAnimation()
{
	m_pAnimation->SetDefaultClip(g_pSleepAnimation[m_eDir][SC_WALK]);

}
