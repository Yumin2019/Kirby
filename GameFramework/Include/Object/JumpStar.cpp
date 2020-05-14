#include "JumpStar.h"
#include "../Sound/SoundManager.h"

CJumpStar::CJumpStar()
{
	m_bJump = false; // �������� ���� ���� ����.
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

		// �ڽ��� ������ �����̵��� �Ѵ�.
		// �ڽ��� ������ �����̴µ�, �̰��� Angle�� ó���Ѵ�. ��, ������ ���ؼ� ���� ����Ʈ ���� ���������� ���� ����Ʈ ���� �δ� ���̴�.
	}

	else
	{
		// ������ false�� ���� Line Ȥ�� Stage�� �浹�� �Ͼ ����ε� �� ��쿡�� Jump�� �ٽ� ȣ���ϸ� Power���� ���̸� �༭ 
		// ������ �ְ� ��������.

		GET_SINGLE(CSoundManager)->Play("JumpStarBounce", CT_EFFECT);
		
		m_bJump = true;
		m_fPowerTime = 0.f;
		m_fPower = 1600.f + (rand() % 4) * 100.f;
		// �÷��̾��� ��쿡�� 2000 ������ �ְ� -200.f ������ �ð��� ó���Ѵ�. 1700 += 200 ������ ����.
			// ��, 1500 ~ 1900.

		++m_iJumpCount;

		if (m_iJumpCount == m_iJumpCountMax)
		{
			if (m_fAngle == 0.f)
			{
				m_fAngle = PI;
			}
			else
				m_fAngle = 0.f;

			// ī��Ʈ�� ��� �����ٸ�, ������ �ٲٰ� �ʱ�ȭ.
			m_iJumpCount = 0;

			// ���Ŀ� ���� Ƚ���� ���������� �����ϰ� �ʹٸ�...
			m_iJumpCountMax = rand() % 5 + 3; // 3 ~ 7������ ����.
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
