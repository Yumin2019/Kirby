#include "Grizoo.h"
#include "../Animation/Animation.h"
#include "../Collision/Collider.h"

enum GRIZOO_CLIP
{
	GR_WALK,
	GR_HIT,
	GR_END
};

const char* g_pGrizooAnimationName[SD_END][GR_END] =
{
	{"GrizooWalkLeft", "GrizooHitLeft"},
	{"GrizooWalkRight", "GrizooHitRight"}
};


CGrizoo::CGrizoo()
{

}


CGrizoo::CGrizoo(const CGrizoo & obj) :
	CMonster(obj)
{
}

CGrizoo::~CGrizoo()
{
}

void CGrizoo::DirChange()
{
		m_fChangeTime = 0.f;

		if (m_eDir == SD_LEFT)
			m_eDir = SD_RIGHT;
		else
			m_eDir = SD_LEFT;

		// walk 애니메이션, 방향에 따라서
		IdleAnimation();

}

void CGrizoo::Start()
{
	CMonster::Start();


	CCollider* pCollider = FindCollider();

	if (pCollider)
		pCollider->SetCollisionCallback<CGrizoo>(CS_BLOCK, &CGrizoo::HitStart, this);
}

bool CGrizoo::Init()
{


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


	return true;
}

void CGrizoo::Update(float fTime)
{
	CMonster::Update(fTime);

	// 들어온 ChangeTime 값을 가지고 몇 초간 좌우로 이동할지 정한다.

	if (m_bMove)
	{
		m_fChangeTime += fTime;

		if (m_fChangeTime >= m_fChangePlayTime)
		{
			m_fChangeTime -= m_fChangePlayTime;
			
			DirChange();
			
		}

		// 방향에 따라서 왼쪽 / 오른쪽으로 이동하도록 한다.
		MoveX(fTime);

		// walk 애니메이션, 방향에 따라서
		IdleAnimation();
	}
}

void CGrizoo::LateUpdate(float fTime)
{
	CMonster::LateUpdate(fTime);

}

void CGrizoo::Collision(float fTime)
{
	CMonster::Collision(fTime);

}

void CGrizoo::PrevRender(HDC hDC, float fTime)
{
	CMonster::PrevRender(hDC, fTime);

}

void CGrizoo::Render(HDC hDC, float fTime)
{
	CMonster::Render(hDC, fTime);

}

void CGrizoo::PostRender(HDC hDC, float fTime)
{
	CMonster::PostRender(hDC, fTime);

}

CGrizoo * CGrizoo::Clone() const
{
	return new CGrizoo(*this);
}

void CGrizoo::HitAnimation()
{
	CMonster::HitAnimation();

	m_pAnimation->ChangeClip(g_pGrizooAnimationName[m_eDir][GR_HIT]);
}

void CGrizoo::AddClipName(vector<string>& vecClipName)
{
	for (int j = 0; j < GR_END; ++j)
	{
		for (int i = 0; i < SD_END; ++i)
		{
			vecClipName.push_back(g_pGrizooAnimationName[i][j]);
		}
	}
}

void CGrizoo::IdleAnimation()
{
	m_pAnimation->ChangeClip(g_pGrizooAnimationName[m_eDir][GR_WALK]);
}

void CGrizoo::DefaultIdleAnimation()
{
	m_pAnimation->SetDefaultClip(g_pGrizooAnimationName[m_eDir][GR_WALK]);

}

void CGrizoo::HitStart(CCollider * pSrc, CCollider * pDest, float fTime)
{
	CMonster::HitStart(pSrc, pDest, fTime);

}
