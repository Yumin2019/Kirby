#include "BossHP.h"
#include "../Object/Monster.h"
#include "../Texture.h"

CBossHP::CBossHP() 
{
}


CBossHP::CBossHP(const CBossHP & obj) :
	CTargetUI(obj)
{
}

CBossHP::~CBossHP()
{
}

void CBossHP::Start()
{
	CTargetUI::Start();
}

bool CBossHP::Init()
{
	if (!CTargetUI::Init())
		return false;

	SetTexture("BossHP", TEXT("UI/BossHP.bmp"));
	// NO ColorKey
	SetStaticSize(420.f, 45.f);

	return true;
}

void CBossHP::Update(float fTime)
{
}

void CBossHP::LateUpdate(float fTime)
{

}

void CBossHP::Collision(float fTime)
{

}

void CBossHP::PrevRender(HDC hDC, float fTime)
{

}

void CBossHP::Render(HDC hDC, float fTime)
{

	if (m_pTarget)
	{
		// 타겟의 HP를 얻어온다.
		int iHP = ((CMonster*)m_pTarget)->GetHP();
		int iHPMax = ((CMonster*)m_pTarget)->GetHPMax();


		// HP의 비율을 구해낸다.
		float fPercent = (float)iHP / iHPMax;

		// 이 비율을 가지고 고정된 사이즈에 곱한다. 
		// 출력 위치는 고정이고 사이즈만 바꾸는 형식이다. 

		// 고정된 사이즈에 출력하는 크기의 X를 구해낸다.

		Size tRenderSize(m_tStaticSize.x * fPercent, m_tStaticSize.y);

		m_pTexture->Render(hDC, m_tPos, Position(0.f, 0.f), tRenderSize);


		if (fPercent == 0.f)
		{
			// 타겟이 죽은 경우에는 참조를 자동으로 풀어버리는 처리를 했다.
			SAFE_RELEASE(m_pTarget);
			SetEnable(false);


			CObj* pBorder = m_pScene->FindObject("BossHPBorder");
			pBorder->SetEnable(false);
			SAFE_RELEASE(pBorder);

		}

	}

}

void CBossHP::PostRender(HDC hDC, float fTime)
{
	CUI::PostRender(hDC, fTime);

}

CBossHP * CBossHP::Clone() const
{
	return new CBossHP(*this);
}

void CBossHP::AddClipName(vector<string>& vecClipName)
{
}
