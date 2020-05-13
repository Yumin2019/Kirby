#include "JumpStar.h"
#include "../Sound/SoundManager.h"

CJumpStar::CJumpStar()
{
	m_bJump = false; // 생성되자 마자 점프 시작.
	m_fPowerTime = 0.f;
	m_fPower = 0.f;
	m_iJumpCount = 0;
	m_iJumpCountMax = 0;

	m_eObjectType = OT_JUMP_STAR;

	m_bMove = true;
}


CJumpStar::CJumpStar(const CJumpStar & obj) :
	CStar(obj)
{
	m_fPower = obj.m_fPower;
	m_bJump = false;
	m_fPowerTime = 0.f;

	m_eObjectType = OT_JUMP_STAR;

	m_iJumpCount = 0;
	m_iJumpCountMax = obj.m_iJumpCountMax;

	m_bMove = true;

}

CJumpStar::~CJumpStar()
{
}

void CJumpStar::Start()
{
	CObj::Start();
}

bool CJumpStar::Init()
{
	if (!CStar::Init())
		return false;


	return true;
}

void CJumpStar::Update(float fTime)
{
	CStar::Update(fTime);

	if (!m_bMove)
		return;

	if (m_bJump)
	{
		m_fPowerTime += fTime;

		if (m_fPowerTime >= 0.1f)
		{
			m_fPowerTime -= 0.1f;
			m_fPower -= 200.f;
		}

		Move(Position(0.f, -1.f), m_fPower, fTime);

		// 자신의 방향대로 움직이도록 한다.
		// 자신의 방향대로 움직이는데, 이것은 Angle로 처리한다. 즉, 왼쪽을 향해서 가는 이펙트 따로 오른쪽으로 가는 이펙트 따로 두는 것이다.
	}

	else
	{
		// 점프가 false인 경우는 Line 혹은 Stage와 충돌이 일어난 경우인데 이 경우에는 Jump를 다시 호출하며 Power값의 차이를 줘서 
		// 생동감 있게 구현하자.

		GET_SINGLE(CSoundManager)->Play("JumpStarBounce", CT_EFFECT);
		
		m_bJump = true;
		m_fPowerTime = 0.f;
		m_fPower = 1600.f + (rand() % 4) * 100.f;
		// 플레이어의 경우에는 2000 정도를 주고 -200.f 정도의 시간을 처리한다. 1700 += 200 정도로 하자.
			// 즉, 1500 ~ 1900.

		++m_iJumpCount;

		if (m_iJumpCount == m_iJumpCountMax)
		{
			if (m_fAngle == 0.f)
			{
				m_fAngle = PI;
			}
			else
				m_fAngle = 0.f;

			// 카운트를 모두 세었다면, 방향을 바꾸고 초기화.
			m_iJumpCount = 0;

			// 이후에 점프 횟수를 가변적으로 설정하고 싶다면...
			m_iJumpCountMax = rand() % 5 + 3; // 3 ~ 7번으로 설정.
		}
	}

	
}

void CJumpStar::LateUpdate(float fTime)
{
	CStar::LateUpdate(fTime);

}

void CJumpStar::Collision(float fTime)
{
	CStar::Collision(fTime);

}

void CJumpStar::PrevRender(HDC hDC, float fTime)
{
	CStar::PrevRender(hDC, fTime);

}

void CJumpStar::Render(HDC hDC, float fTime)
{
	CStar::Render(hDC, fTime);

}

void CJumpStar::PostRender(HDC hDC, float fTime)
{
	CStar::PostRender(hDC, fTime);

}

CJumpStar * CJumpStar::Clone() const
{
	return new CJumpStar(*this);
}

void CJumpStar::AddClipName(vector<string>& vecClipName)
{
}
