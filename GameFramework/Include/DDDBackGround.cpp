#include "Object\DDDBackGround.h"


CDDDBackGround::CDDDBackGround()
{
}


CDDDBackGround::CDDDBackGround(const CDDDBackGround & obj) :
	CAniObject(obj)
{
}

CDDDBackGround::~CDDDBackGround()
{
}

void CDDDBackGround::Start()
{
	CObj::Start();
}

bool CDDDBackGround::Init()
{
	if (!CObj::Init())
		return false;

	return true;
}

void CDDDBackGround::Update(float fTime)
{
	CObj::Update(fTime);
}

void CDDDBackGround::LateUpdate(float fTime)
{
	CObj::LateUpdate(fTime);

}

void CDDDBackGround::Collision(float fTime)
{
	CObj::Collision(fTime);

}

void CDDDBackGround::PrevRender(HDC hDC, float fTime)
{
	CObj::PrevRender(hDC, fTime);

}

void CDDDBackGround::Render(HDC hDC, float fTime)
{
	CObj::Render(hDC, fTime);

}

void CDDDBackGround::PostRender(HDC hDC, float fTime)
{
	CObj::PostRender(hDC, fTime);

}

CDDDBackGround * CDDDBackGround::Clone() const
{
	return new CDDDBackGround(*this);
}

void CDDDBackGround::AddClipName(vector<string>& vecClipName)
{
}
