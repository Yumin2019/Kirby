#include "HPHud.h"
#include "../Object/Player.h"
#include "../Texture.h"


CHPHud::CHPHud()
{
}


CHPHud::CHPHud(const CHPHud & obj) :
	CTargetUI(obj)
{
}

CHPHud::~CHPHud()
{
}

void CHPHud::Start()
{
	CTargetUI::Start();

	m_pTarget = m_pScene->FindObject("Kirby");
}

bool CHPHud::Init()
{
	if (!CTargetUI::Init())
		return false;

	SetTexture("HPHud", TEXT("UI/HPHud.bmp"));
	m_pTexture->SetColorKey(255, 0, 255);



	return true;
}

void CHPHud::Update(float fTime)
{
}

void CHPHud::LateUpdate(float fTime)
{

}

void CHPHud::Collision(float fTime)
{

}

void CHPHud::PrevRender(HDC hDC, float fTime)
{

}

void CHPHud::Render(HDC hDC, float fTime)
{
	// 자신의 위치에서 체력을 출력해준다.

	int iHP = ((CPlayer*)m_pTarget)->GetHP();
	int iHPMax = ((CPlayer*)m_pTarget)->GetHPMax();

	// 커비 이미지를 출력한다.

	m_pTexture->Render(hDC, m_tPos, Position(0.f, 0.f), Size(70.f, 70.f));
	

	for (int i = 0; i < iHPMax; ++i)
	{
		// 3 / 5
		// x : 20간격

		if (i < iHP)
		{
			// 0 , 1 , 2	3번은 하트를 출력한다.
			m_pTexture->Render(hDC, Position(78.f + m_tPos.x + 21.f * i, m_tPos.y), Position(78.f, 3.f), Size(19.f, 64.f));
		}
		else
		{
			// 나머지는 빈 하트를 출력한다.
			m_pTexture->Render(hDC, Position(78.f + m_tPos.x + 21.f * i, m_tPos.y), Position(108.f, 3.f), Size(19.f, 64.f));

		}
	}
}

void CHPHud::PostRender(HDC hDC, float fTime)
{
	CTargetUI::PostRender(hDC, fTime);

}

CHPHud * CHPHud::Clone() const
{
	return new CHPHud(*this);
}

void CHPHud::AddClipName(vector<string>& vecClipName)
{
}
