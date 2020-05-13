#include "TargetUI.h"



CTargetUI::CTargetUI() :
	m_pTarget(nullptr)
{
}


CTargetUI::CTargetUI(const CTargetUI & obj) :
	CUIImage(obj)
{
	m_pTarget = nullptr;
}

CTargetUI::~CTargetUI()
{
	SAFE_RELEASE(m_pTarget);
}

void CTargetUI::Start()
{
	CObj::Start();
}

bool CTargetUI::Init()
{
	if (!CUI::Init())
		return false;


	return true;
}

void CTargetUI::Update(float fTime)
{
	CUI::Update(fTime);
}

void CTargetUI::LateUpdate(float fTime)
{
	CUI::LateUpdate(fTime);

}

void CTargetUI::Collision(float fTime)
{
	CUI::Collision(fTime);

}

void CTargetUI::PrevRender(HDC hDC, float fTime)
{
	CUI::PrevRender(hDC, fTime);

}

void CTargetUI::Render(HDC hDC, float fTime)
{
	CUI::Render(hDC, fTime);

}

void CTargetUI::PostRender(HDC hDC, float fTime)
{
	CUI::PostRender(hDC, fTime);

}

CTargetUI * CTargetUI::Clone() const
{
	return new CTargetUI(*this);
}

void CTargetUI::AddClipName(vector<string>& vecClipName)
{
}
