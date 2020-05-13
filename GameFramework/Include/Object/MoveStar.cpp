#include "MoveStar.h"


CMoveStar::CMoveStar()
{
	m_bMove = true;
	m_fMovePlayTime = 0.f;
	m_fMoveTime = 0.f;

	m_eObjectType = OT_MOVE_STAR;

	// Normal Type
}


CMoveStar::CMoveStar(const CMoveStar & obj) :
	CStar(obj)
{
	m_bMove = true;
	m_fMoveTime = 0.f;
	m_fMovePlayTime = obj.m_fMovePlayTime;

	m_eObjectType = OT_MOVE_STAR;

}

CMoveStar::~CMoveStar()
{
}

void CMoveStar::Start()
{
	CObj::Start();
}

bool CMoveStar::Init()
{
	if (!CStar::Init())
		return false;


	return true;
}

void CMoveStar::Update(float fTime)
{
	CStar::Update(fTime);


	// True : 즉 움직이고 있는 경우에만 처리를 하도록 조건을 걸자.

	if (m_bMove)
	{
		m_fMoveTime += fTime;

		if (m_fMoveTime >= m_fMovePlayTime)
		{
			m_bMove = false;
			m_bAngle = false;

			// PlayTime이 지나면 움직이지 않도록 Angle = false로 주고
			// 이 조건문을 다시 검사하지 않도록 Move = false로 준다.
		}
	}


}

void CMoveStar::LateUpdate(float fTime)
{
	CStar::LateUpdate(fTime);

}

void CMoveStar::Collision(float fTime)
{
	CStar::Collision(fTime);

}

void CMoveStar::PrevRender(HDC hDC, float fTime)
{
	CStar::PrevRender(hDC, fTime);

}

void CMoveStar::Render(HDC hDC, float fTime)
{
	CStar::Render(hDC, fTime);

}

void CMoveStar::PostRender(HDC hDC, float fTime)
{
	CStar::PostRender(hDC, fTime);

}

CMoveStar * CMoveStar::Clone() const
{
	return new CMoveStar(*this);
}

void CMoveStar::AddClipName(vector<string>& vecClipName)
{
}
