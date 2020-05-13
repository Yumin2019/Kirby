#include "AbilityImage.h"
#include "../Object/Player.h"
#include "../Texture.h"



CAbilityImage::CAbilityImage()
{
}


CAbilityImage::CAbilityImage(const CAbilityImage & obj) :
	CTargetUI(obj)
{
}

CAbilityImage::~CAbilityImage()
{
}

void CAbilityImage::Start()
{
	CTargetUI::Start();

	m_pTarget = m_pScene->FindObject("Kirby");

}

bool CAbilityImage::Init()
{
	if (!CTargetUI::Init())
		return false;

	SetTexture("AbilityImage", TEXT("UI/AbilityImage.bmp"));
	m_pTexture->SetColorKey(255, 0, 255);



	return true;
}

void CAbilityImage::Update(float fTime)
{
}

void CAbilityImage::LateUpdate(float fTime)
{

}

void CAbilityImage::Collision(float fTime)
{

}

void CAbilityImage::PrevRender(HDC hDC, float fTime)
{

}

void CAbilityImage::Render(HDC hDC, float fTime)
{
	int iKirbyType = ((CPlayer*)m_pTarget)->GetKirbyType();

	// 170 * 170 

	// 만약, 마이크가 아닌 경우에는 src에서의 x좌표는 0
	// 마이크인 경우에는 X 좌표가 달라진다. End인 경우에는 커비의 위치로 고정시키도록.

	if (iKirbyType != KIRBY_MIKE)
	{
		// 마이크가 아닌 나머지의 경우.
		// x : 0 y : 170 * iKirbyType 

		m_pTexture->Render(hDC, m_tPos, Position(0.f, 170.f * iKirbyType), Size(170.f, 170.f));
	}
	else
	{
		// 마이크인 경우.
		// X 의 좌표가 달라진다. 
		// y : 170 * iKirbyType x : 165 * iMikeType

		int iMikeType = ((CPlayer*)m_pTarget)->GetMikeType();

		m_pTexture->Render(hDC, m_tPos, Position(165.f * iMikeType, 170.f * iKirbyType), Size(165.f, 170.f));

	}
}

void CAbilityImage::PostRender(HDC hDC, float fTime)
{
	CTargetUI::PostRender(hDC, fTime);

}

CAbilityImage * CAbilityImage::Clone() const
{
	return new CAbilityImage(*this);
}

void CAbilityImage::AddClipName(vector<string>& vecClipName)
{
}
