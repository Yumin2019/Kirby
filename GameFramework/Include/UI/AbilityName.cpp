#include "AbilityName.h"
#include "../Object/Player.h"
#include "../Texture.h"

CAbilityName::CAbilityName()
{
}


CAbilityName::CAbilityName(const CAbilityName & obj) :
	CTargetUI(obj)
{
}

CAbilityName::~CAbilityName()
{
}

void CAbilityName::Start()
{
	CTargetUI::Start();

	m_pTarget = m_pScene->FindObject("Kirby");

}

bool CAbilityName::Init()
{
	if (!CTargetUI::Init())
		return false;

	SetTexture("AbilityName", TEXT("UI/AbilityName.bmp"));
	m_pTexture->SetColorKey(255, 0, 255);



	return true;
}

void CAbilityName::Update(float fTime)
{
	CTargetUI::Update(fTime);
}

void CAbilityName::LateUpdate(float fTime)
{

}

void CAbilityName::Collision(float fTime)
{
}

void CAbilityName::PrevRender(HDC hDC, float fTime)
{

}

void CAbilityName::Render(HDC hDC, float fTime)
{
	int iKirbyType = ((CPlayer*)m_pTarget)->GetKirbyType();


	m_pTexture->Render(hDC, m_tPos, Position(0.f, iKirbyType * 100.f), Size(212.f, 100.f));
}

void CAbilityName::PostRender(HDC hDC, float fTime)
{
	CTargetUI::PostRender(hDC, fTime);

}

CAbilityName * CAbilityName::Clone() const
{
	return new CAbilityName(*this);
}

void CAbilityName::AddClipName(vector<string>& vecClipName)
{
}
