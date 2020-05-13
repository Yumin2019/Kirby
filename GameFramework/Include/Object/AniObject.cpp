#include "AniObject.h"



CAniObject::CAniObject()
{
}


CAniObject::CAniObject(const CAniObject & obj) :
	CObj(obj)
{
}

CAniObject::~CAniObject()
{
}

void CAniObject::Start()
{
	CObj::Start();
}

bool CAniObject::Init()
{
	if (!CObj::Init())
		return false;

	return true;
}

void CAniObject::Update(float fTime)
{
	CObj::Update(fTime);
}

void CAniObject::LateUpdate(float fTime)
{
	CObj::LateUpdate(fTime);

}

void CAniObject::Collision(float fTime)
{
	CObj::Collision(fTime);

}

void CAniObject::PrevRender(HDC hDC, float fTime)
{
	CObj::PrevRender(hDC, fTime);

}

void CAniObject::Render(HDC hDC, float fTime)
{
	CObj::Render(hDC, fTime);

}

void CAniObject::PostRender(HDC hDC, float fTime)
{
	CObj::PostRender(hDC, fTime);

}

CAniObject * CAniObject::Clone() const
{
	return new CAniObject(*this);
}

void CAniObject::AddClipName(vector<string>& vecClipName)
{
}
