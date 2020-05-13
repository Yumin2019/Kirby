#include "Burt.h"
#include "../Animation/Animation.h"
#include "../Core/SceneManager.h"
#include "../Collision/Collider.h"

enum BURT_CLIP
{
	BU_FLY, 
	BU_HIT,
	BU_END
};

const char* g_pBurtAnimationName[SD_END][BU_END] =
{
	{"BurtFlyLeft", "BurtHitLeft"},
	{"BurtFlyRight", "BurtHitRight"}
};


CBurt::CBurt()
{
	m_tStageSize = GET_SINGLE(CSceneManager)->GetStageSize();

}


CBurt::CBurt(const CBurt & obj) :
	CMonster(obj)
{
	m_tStageSize = GET_SINGLE(CSceneManager)->GetStageSize();

	

}

CBurt::~CBurt()
{
}

void CBurt::Start()
{
	CMonster::Start();

	CCollider* pCollider = FindCollider();

	if (pCollider && this)
		pCollider->SetCollisionCallback<CBurt>(CS_BLOCK, &CBurt::HitStart, this);
}

bool CBurt::Init()
{

	if (!CMonster::Init())
		return false;

	CreateAnimation("BurtAnimation");

	m_pAnimation->AddClip(g_pBurtAnimationName[SD_LEFT][BU_FLY], AO_LOOP, 1.f);
	m_pAnimation->SetClipTexture(g_pBurtAnimationName[SD_LEFT][BU_FLY], "NormalMonsterLeft", TEXT("NormalMonsterLeft.bmp"));


	m_pAnimation->AddClip(g_pBurtAnimationName[SD_RIGHT][BU_FLY], AO_LOOP, 1.f);
	m_pAnimation->SetClipTexture(g_pBurtAnimationName[SD_RIGHT][BU_FLY], "NormalMonsterRight", TEXT("NormalMonsterRight.bmp"));




	m_pAnimation->AddClip(g_pBurtAnimationName[SD_LEFT][BU_HIT], AO_LOOP, 1.f);
	m_pAnimation->SetClipTexture(g_pBurtAnimationName[SD_LEFT][BU_HIT], "NormalMonsterLeft", TEXT("NormalMonsterLeft.bmp"));


	m_pAnimation->AddClip(g_pBurtAnimationName[SD_RIGHT][BU_HIT], AO_LOOP, 1.f);
	m_pAnimation->SetClipTexture(g_pBurtAnimationName[SD_RIGHT][BU_HIT], "NormalMonsterRight", TEXT("NormalMonsterRight.bmp"));





	m_pAnimation->LoadClip("../Bin/Save/Burt/BurtFlyLeft.clip");
	m_pAnimation->LoadClip("../Bin/Save/Burt/BurtFlyRight.clip");


	m_pAnimation->LoadClip("../Bin/Save/Burt/BurtHitLeft.clip");
	m_pAnimation->LoadClip("../Bin/Save/Burt/BurtHitRight.clip");

	m_pAnimation->ChangeClip(m_pAnimation->GetDefaultClip());
	m_pAnimation->SetTextureColorKeyAll(0, 72, 80);


	// 지우기
	m_pAnimation->FindClip(g_pBurtAnimationName[SD_RIGHT][BU_HIT])->eOption = AO_ONCE_RETURN;
	m_pAnimation->FindClip(g_pBurtAnimationName[SD_LEFT][BU_HIT])->eOption = AO_ONCE_RETURN;

	SetStaticSize(70.f, 70.f);


	return true;
}

void CBurt::Update(float fTime)
{
	CMonster::Update(fTime);

	if (m_bMove)
	{
		m_fChangeTime += fTime;

		switch (m_eDir)
		{
		case SD_LEFT:
			Move(Position(-1.f, 0.f), fTime); 
			m_tPos.y += 200.f * sinf(m_fChangeTime) * fTime;

			break;

		case SD_RIGHT:
			Move(Position(1.f, 0.f), fTime); 
			m_tPos.y += 200.f * sinf(m_fChangeTime) * fTime;
			break;
		}
		// -300 ~ 300의 범위가 순차적으로 나오게 되는데, 시간을 곱해서 천천히 이동하도록 한다.
		//MoveAngle()
	}

	

	Position tLeftTop = GetPos() - m_tPivot * m_tStaticSize;
	Position tRightBottom = tLeftTop + m_tStaticSize;

	if (tRightBottom.x < 0.f || tLeftTop.x > m_tStageSize.x)
		m_bActive = false;
	// x에 대한 처리만 하면 될 것 같다.

}

void CBurt::LateUpdate(float fTime)
{
	CMonster::LateUpdate(fTime);

}

void CBurt::Collision(float fTime)
{
	CMonster::Collision(fTime);

}

void CBurt::PrevRender(HDC hDC, float fTime)
{
	CMonster::PrevRender(hDC, fTime);

}

void CBurt::Render(HDC hDC, float fTime)
{
	CMonster::Render(hDC, fTime);

}

void CBurt::PostRender(HDC hDC, float fTime)
{
	CMonster::PostRender(hDC, fTime);

}

CBurt * CBurt::Clone() const
{
	return new CBurt(*this);
}

void CBurt::HitAnimation()
{
	CMonster::HitAnimation();

	m_pAnimation->ChangeClip(g_pBurtAnimationName[m_eDir][BU_HIT]);
}

void CBurt::AddClipName(vector<string>& vecClipName)
{
	for (int j = 0; j < BU_END; ++j)
	{
		for (int i = 0; i < SD_END; ++i)
		{
			vecClipName.push_back(g_pBurtAnimationName[i][j]);
		}
	}
}

void CBurt::IdleAnimation()
{
	m_pAnimation->ChangeClip(g_pBurtAnimationName[m_eDir][BU_FLY]);

}

void CBurt::HitStart(CCollider * pSrc, CCollider * pDest, float fTime)
{
	CMonster::HitStart(pSrc, pDest, fTime);



}

void CBurt::DefaultIdleAnimation()
{
	m_pAnimation->SetDefaultClip(g_pBurtAnimationName[m_eDir][BU_FLY]);

}

