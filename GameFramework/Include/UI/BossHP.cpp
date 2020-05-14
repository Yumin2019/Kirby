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
		// Ÿ���� HP�� ���´�.
		int iHP = ((CMonster*)m_pTarget)->GetHP();
		int iHPMax = ((CMonster*)m_pTarget)->GetHPMax();


		// HP�� ������ ���س���.
		float fPercent = (float)iHP / iHPMax;

		// �� ������ ������ ������ ����� ���Ѵ�. 
		// ��� ��ġ�� �����̰� ����� �ٲٴ� �����̴�. 

		// ������ ����� ����ϴ� ũ���� X�� ���س���.

		Size tRenderSize(m_tStaticSize.x * fPercent, m_tStaticSize.y);

		m_pTexture->Render(hDC, m_tPos, Position(0.f, 0.f), tRenderSize);


		if (fPercent == 0.f)
		{
			// Ÿ���� ���� ��쿡�� ������ �ڵ����� Ǯ������� ó���� �ߴ�.
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
