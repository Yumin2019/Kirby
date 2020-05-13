#include "UI.h"



CUI::CUI()
{
	m_bIsUI = true;
	m_strLayerName = "UI"; // default °ª.

	m_eObjectType = OT_UI;


}

CUI::CUI(const CUI & obj) :
	CObj(obj)
{
	m_bIsUI = true;
	m_eObjectType = OT_UI;
	m_strLayerName = "UI";


}


CUI::~CUI()
{
}

void CUI::Start()
{
	CObj::Start();
}

bool CUI::Init()
{
	if (!CObj::Init())
		return false;
	

	return true;
}

void CUI::Update(float fTime)
{
	CObj::Update(fTime);
}

void CUI::LateUpdate(float fTime)
{
	CObj::LateUpdate(fTime);

}

void CUI::Collision(float fTime)
{
	CObj::Collision(fTime);

}

void CUI::PrevRender(HDC hDC, float fTime)
{
	CObj::PrevRender(hDC, fTime);

}

void CUI::Render(HDC hDC, float fTime)
{
	CObj::Render(hDC, fTime);

}

void CUI::PostRender(HDC hDC, float fTime)
{
	CObj::PostRender(hDC, fTime);

}

CUI * CUI::Clone() const
{
	return nullptr;
}

void CUI::AddClipName(vector<string>& vecClipName)
{
}
