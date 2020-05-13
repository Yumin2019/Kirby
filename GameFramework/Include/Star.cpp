#include "Object\Star.h"



CStar::CStar()
{
	m_eAbility = MT_NORMAL;
}


CStar::CStar(const CStar & obj) :
	CEffect(obj)
{
	m_eAbility = MT_NORMAL;

}

CStar::~CStar()
{
}

void CStar::Start()
{
	CObj::Start();
}

bool CStar::Init()
{
	if (!CEffect::Init())
		return false;


	return true;
}

void CStar::Update(float fTime)
{
	CEffect::Update(fTime);
}

void CStar::LateUpdate(float fTime)
{
	CEffect::LateUpdate(fTime);

}

void CStar::Collision(float fTime)
{
	CEffect::Collision(fTime);

}

void CStar::PrevRender(HDC hDC, float fTime)
{
	CEffect::PrevRender(hDC, fTime);

}

void CStar::Render(HDC hDC, float fTime)
{
	CEffect::Render(hDC, fTime);

}

void CStar::PostRender(HDC hDC, float fTime)
{
	CEffect::PostRender(hDC, fTime);

}

CStar * CStar::Clone() const
{
	return new CStar(*this);
}

void CStar::AddClipName(vector<string>& vecClipName)
{
}
