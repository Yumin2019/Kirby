#include "Character.h"
#include "../InputComponent.h"



CCharacter::CCharacter() :
	m_pInputComponent(nullptr),
	m_bMove(false),
	m_fStopTime(0.f)

{
	m_eDir = SD_RIGHT;

	m_iHP = 1;
	m_iHPMax = 1;


}

CCharacter::CCharacter(const CCharacter & obj) :
	CObj(obj)
{
	m_eDir = obj.m_eDir;

	m_bMove = false;

	m_iHP = obj.m_iHP;
	m_iHPMax = obj.m_iHPMax;

}


CCharacter::~CCharacter()
{
	if(GetTag() == "Kirby")
		SAFE_RELEASE(m_pInputComponent);

	// 터지는건 일단 막아두고 다른 다른 작업을 하는 것이 정신건강에 좋을 것 같다.
}

void CCharacter::Start()
{
	CObj::Start();
}

bool CCharacter::Init()
{
	if (GetTag() == "Kirby")
		m_pInputComponent = new CInputComponent;

	return true;
}

void CCharacter::Update(float fTime)
{

	CObj::Update(fTime);

	
}

void CCharacter::LateUpdate(float fTime)
{
	CObj::LateUpdate(fTime);

}

void CCharacter::Collision(float fTime)
{
	CObj::Collision(fTime);

}

void CCharacter::PrevRender(HDC hDC, float fTime)
{
	CObj::PrevRender(hDC, fTime);

}

void CCharacter::Render(HDC hDC, float fTime)
{
	CObj::Render(hDC, fTime);

	m_bMove = false;

}

void CCharacter::PostRender(HDC hDC, float fTime)
{
	CObj::PostRender(hDC, fTime);

}

CCharacter * CCharacter::Clone() const
{
	return nullptr;
}

void CCharacter::AddClipName(vector<string>& vecClipName)
{
}
