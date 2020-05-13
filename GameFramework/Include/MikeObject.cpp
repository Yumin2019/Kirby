#include "MikeObject.h"
#include "Collision/Collider.h"
#include "Object/Monster.h"
#include "Core/SceneManager.h"

CMikeObject::CMikeObject()
{
}


CMikeObject::CMikeObject(const CMikeObject & obj) :
	CObj(obj)
{
}

CMikeObject::~CMikeObject()
{
}

void CMikeObject::SetMikeObject(float fIntervalTime, int iLimitCount, int iBlendMin, int iBlendMax)
{
	if (m_bEnable)
		return;

	SetBlend(fIntervalTime, iLimitCount, iBlendMin, iBlendMax);

	m_bEnable = true;

}


void CMikeObject::Start()
{
	CObj::Start();
}

bool CMikeObject::Init()
{
	if (!CObj::Init())
		return false;

	SetTexture("MikeObject", TEXT("Effect/MikeObject.bmp"), "MikeObject");

	// NO Color Key

	m_bEnable = false;

	SetStaticSize(RS_WIDTH, RS_HEIGHT);
	SetSize(RS_WIDTH, RS_HEIGHT);



	return true;
}

void CMikeObject::Update(float fTime)
{
	CObj::Update(fTime);

	m_tPos = GET_SINGLE(CSceneManager)->GetCameraPos();

	
	// 흔들리는 시간이 끝난 경우에 해당 오브젝트는 비활성화 되어야 한다.
	if (GET_SINGLE(CSceneManager)->GetLeftSwayTime() == 0.f || !m_bBlend)
	{
		m_bEnable = false;
	}
}

void CMikeObject::LateUpdate(float fTime)
{
	CObj::LateUpdate(fTime);

}

void CMikeObject::Collision(float fTime)
{
	CObj::Collision(fTime);

}

void CMikeObject::PrevRender(HDC hDC, float fTime)
{
	CObj::PrevRender(hDC, fTime);

}

void CMikeObject::Render(HDC hDC, float fTime)
{
	CObj::Render(hDC, fTime);

}

void CMikeObject::PostRender(HDC hDC, float fTime)
{
	CObj::PostRender(hDC, fTime);

}

CMikeObject * CMikeObject::Clone() const
{
	return new CMikeObject(*this);
}

void CMikeObject::AddClipName(vector<string>& vecClipName)
{
}

