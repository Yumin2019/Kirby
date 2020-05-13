#include "UIImage.h"



CUIImage::CUIImage()
{
}


CUIImage::CUIImage(const CUIImage & obj) :
	CUI(obj)
{
}

CUIImage::~CUIImage()
{
}

void CUIImage::Start()
{
	CObj::Start();
}

bool CUIImage::Init()
{
	if (!CUI::Init())
		return false;

	
	return true;
}

void CUIImage::Update(float fTime)
{
	CUI::Update(fTime);
}

void CUIImage::LateUpdate(float fTime)
{
	CUI::LateUpdate(fTime);

}

void CUIImage::Collision(float fTime)
{
	CUI::Collision(fTime);

}

void CUIImage::PrevRender(HDC hDC, float fTime)
{
	CUI::PrevRender(hDC, fTime);

}

void CUIImage::Render(HDC hDC, float fTime)
{
	CUI::Render(hDC, fTime);

}

void CUIImage::PostRender(HDC hDC, float fTime)
{
	CUI::PostRender(hDC, fTime);

}

CUIImage * CUIImage::Clone() const
{
	return new CUIImage(*this);
}

void CUIImage::AddClipName(vector<string>& vecClipName)
{
}
