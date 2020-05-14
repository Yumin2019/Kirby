#include "Twizzy.h"
#include "../Animation/Animation.h"
#include "../Core/SceneManager.h"
#include "../Math.h"
#include "../Collision/Collider.h"

enum TWIZZY_CLIP
{
	TW_HIT,
	TW_FLY ,
	TW_JUMP,
	TW_END
};

const char* g_pTwizzyAnimationName[SD_END][TW_END] =
{
	{"TwizzyHitLeft", "TwizzyFlyLeft", "TwizzyJumpLeft"},
	{"TwizzyHitRight", "TwizzyFlyRight", "TwizzyJumpRight"}
};

CTwizzy::CTwizzy()
{


	m_bTypeFly = false;
	m_bJump = false;
	m_fPower = 0.f;
	m_fPowerTime;

	m_tStageSize = GET_SINGLE(CSceneManager)->GetStageSize();
	m_pPlayer = nullptr;



}


CTwizzy::CTwizzy(const CTwizzy & obj) :
	CMonster(obj)
{
	m_bTypeFly = obj.m_bTypeFly;
	m_bJump = false;
	m_fPower = 0.f;
	m_fPowerTime = 0.f;
	m_pPlayer = nullptr;

	// �ݺ��Ǵ� �ִϸ��̼��� �����Ѵ�.
	if (!m_bTypeFly)
	{
		m_pAnimation->ChangeClip(g_pTwizzyAnimationName[m_eDir][TW_JUMP]);
		m_pAnimation->SetDefaultClip(g_pTwizzyAnimationName[m_eDir][TW_JUMP]);
	}
	else
	{
		m_pAnimation->ChangeClip(g_pTwizzyAnimationName[m_eDir][TW_FLY]);
		m_pAnimation->SetDefaultClip(g_pTwizzyAnimationName[m_eDir][TW_FLY]);

	}

	m_tStageSize = GET_SINGLE(CSceneManager)->GetStageSize();


}

CTwizzy::~CTwizzy()
{
	SAFE_RELEASE(m_pPlayer);

}

void CTwizzy::Start()
{
	CMonster::Start();


	CCollider* pCollider = FindCollider();

	if (pCollider)
		pCollider->SetCollisionCallback<CTwizzy>(CS_BLOCK, &CTwizzy::HitStart, this);

	m_pPlayer = m_pScene->FindObject("Kirby");
}

bool CTwizzy::Init()
{


	if (!CMonster::Init())
		return false;


	CreateAnimation("TwizzyAnimation"); // �÷��̾� �ִϸ��̼��� �����Ѵ�.



	m_pAnimation->AddClip(g_pTwizzyAnimationName[SD_LEFT][TW_FLY], AO_LOOP, 1.f);
	m_pAnimation->SetClipTexture(g_pTwizzyAnimationName[SD_LEFT][TW_FLY], "NormalMonsterLeft", TEXT("NormalMonsterLeft.bmp"));


	m_pAnimation->AddClip(g_pTwizzyAnimationName[SD_RIGHT][TW_FLY], AO_LOOP, 1.f);
	m_pAnimation->SetClipTexture(g_pTwizzyAnimationName[SD_RIGHT][TW_FLY], "NormalMonsterRight", TEXT("NormalMonsterRight.bmp"));


	m_pAnimation->AddClip(g_pTwizzyAnimationName[SD_LEFT][TW_HIT], AO_LOOP, 1.f);
	m_pAnimation->SetClipTexture(g_pTwizzyAnimationName[SD_LEFT][TW_HIT], "NormalMonsterLeft", TEXT("NormalMonsterLeft.bmp"));

	m_pAnimation->AddClip(g_pTwizzyAnimationName[SD_RIGHT][TW_HIT], AO_LOOP, 1.f);
	m_pAnimation->SetClipTexture(g_pTwizzyAnimationName[SD_RIGHT][TW_HIT], "NormalMonsterRight", TEXT("NormalMonsterRight.bmp"));


	m_pAnimation->AddClip(g_pTwizzyAnimationName[SD_LEFT][TW_JUMP], AO_LOOP, 1.f);
	m_pAnimation->SetClipTexture(g_pTwizzyAnimationName[SD_LEFT][TW_JUMP], "NormalMonsterLeft", TEXT("NormalMonsterLeft.bmp"));

	m_pAnimation->AddClip(g_pTwizzyAnimationName[SD_RIGHT][TW_JUMP], AO_LOOP, 1.f);
	m_pAnimation->SetClipTexture(g_pTwizzyAnimationName[SD_RIGHT][TW_JUMP], "NormalMonsterRight", TEXT("NormalMonsterRight.bmp"));


	m_pAnimation->LoadClip("../Bin/Save/Twizzy/TwizzyHitLeft.clip");
	m_pAnimation->LoadClip("../Bin/Save/Twizzy/TwizzyHitRight.clip");


	m_pAnimation->LoadClip("../Bin/Save/Twizzy/TwizzyJumpLeft.clip");
	m_pAnimation->LoadClip("../Bin/Save/Twizzy/TwizzyJumpRight.clip");


	m_pAnimation->LoadClip("../Bin/Save/Twizzy/TwizzyFlyLeft.clip");
	m_pAnimation->LoadClip("../Bin/Save/Twizzy/TwizzyFlyRight.clip");

	m_pAnimation->ChangeClip(m_pAnimation->GetDefaultClip());
	m_pAnimation->SetTextureColorKeyAll(0, 72, 80);



	// �����
	m_pAnimation->FindClip(g_pTwizzyAnimationName[SD_RIGHT][TW_HIT])->eOption = AO_ONCE_RETURN;
	m_pAnimation->FindClip(g_pTwizzyAnimationName[SD_LEFT][TW_HIT])->eOption = AO_ONCE_RETURN;

	SetStaticSize(60.f, 60.f);


	return true;
}

void CTwizzy::Update(float fTime)
{
	CMonster::Update(fTime);

	// �ϴ�, TypeFly�� ������ �����.

	if (!m_bMove)
		return;


	if (m_bTypeFly) // �׳� ���ƴٴϴ� �����, �� ������ ���� ���ƿ´�. ���Ʒ��� ���������� �ʴ´�.
	{
		MoveX(fTime);

		m_pAnimation->ChangeClip(g_pTwizzyAnimationName[m_eDir][TW_FLY]);


		Position tLeftTop = GetPos() - m_tPivot * m_tStaticSize;
		Position tRightBottom = tLeftTop + m_tStaticSize;

		if (tRightBottom.x < 0.f || tLeftTop.x > m_tStageSize.x)
			m_bActive = false;

	

	}
	else
	{
		m_fChangeTime += fTime;

		if (m_fChangeTime >= m_fChangePlayTime)
		{
			m_fChangeTime -= m_fChangePlayTime;
			m_bJump = !m_bJump;
			
			if (m_bJump)
			{
				m_fPower = 1100.f;
				m_fPowerTime = 0.f;
			}
		
		}


			


		if (m_bJump)
		{
			m_fPowerTime += fTime;

			if (m_fPowerTime >= 0.1f)
			{
				m_fPowerTime -= 0.1f;
				m_fPower -= 100.f;

				m_fGravityTime -= 0.1f;
			}

			// ������ �ϴ� ���ȿ� x������ �ڽ��� ���ǵ� ��ŭ �̵��ϵ��� �Ѵ�.
			// ���ÿ� ���� �����ϴ� ���� ����.
			Move(Position(0.f, -1.f), m_fPower, fTime);
			
			MoveX(fTime);
		}
		else
		{
			// �������� �ƴ� �����, LineCollision ������ ��쿡�� �׳� �����̵��� �Ѵ�.
			if (m_bLineCollision)
			{
				MoveX(fTime);
			}
		}



		// ������ �÷��̾ �ٶ󺸵��� �Ѵ�.

		// Ÿ���� ������ ���󰣴�.
		Position tPos = m_pPlayer->GetPos();

		float fDist = CMath::GetDist(tPos, m_tPos);

		if (fDist <= 700.f && fDist >= 60.f)
		{
			// �÷��̾��� �Ÿ��� �ľ��ؼ� 700������ ����� ������ ��Ƽ� �ش� �������� �̵��ϵ��� �ϰ�,
			// �÷��̾���� �Ÿ��� ��� ���� ���̰� ���ٸ�, �������� �ʰ� ������ �ϵ��� �Ѵ�.
			// �ִϸ��̼ǵ� �����ϴ�.

			if (tPos.x < m_tPos.x)
			{
				// �÷��̾ ���ʿ� �ִ� ���.
				m_eDir = SD_LEFT;
			}
			else
			{
				// ������
				m_eDir = SD_RIGHT;
			}
		}
		else
			m_eDir = SD_NONE;



		if (m_eDir != SD_NONE)
		{
			m_pAnimation->ChangeClip(g_pTwizzyAnimationName[m_eDir][TW_JUMP]);
			m_pAnimation->SetDefaultClip(g_pTwizzyAnimationName[m_eDir][TW_JUMP]);
		}




	}
}

void CTwizzy::LateUpdate(float fTime)
{
	CMonster::LateUpdate(fTime);

}

void CTwizzy::Collision(float fTime)
{
	CMonster::Collision(fTime);

}

void CTwizzy::PrevRender(HDC hDC, float fTime)
{
	CMonster::PrevRender(hDC, fTime);

}

void CTwizzy::Render(HDC hDC, float fTime)
{
	CMonster::Render(hDC, fTime);

}

void CTwizzy::PostRender(HDC hDC, float fTime)
{
	CMonster::PostRender(hDC, fTime);

}

CTwizzy * CTwizzy::Clone() const
{
	return new CTwizzy(*this);
}

void CTwizzy::HitAnimation()
{
	CMonster::HitAnimation();


	if (m_eDir == SD_NONE)
		return;

	m_pAnimation->ChangeClip(g_pTwizzyAnimationName[m_eDir][TW_HIT]);
}

void CTwizzy::AddClipName(vector<string>& vecClipName)
{
	for (int j = 0; j < TW_END; ++j)
	{
		for (int i = 0; i < SD_END; ++i)
		{
			vecClipName.push_back(g_pTwizzyAnimationName[i][j]);
		}
	}
}

void CTwizzy::IdleAnimation()
{
	m_pAnimation->ChangeClip(g_pTwizzyAnimationName[m_eDir][TW_FLY]);
}

void CTwizzy::HitStart(CCollider * pSrc, CCollider * pDest, float fTime)
{
	CMonster::HitStart(pSrc, pDest, fTime);

	
}

void CTwizzy::DefaultIdleAnimation()
{
	m_pAnimation->SetDefaultClip(g_pTwizzyAnimationName[m_eDir][TW_FLY]);

}
