#include "Effect.h"



CEffect::CEffect()
{
	m_pTarget = nullptr;
	SetPivot(0.5f, 0.5f);

	m_fKnockBackPower = 0.f;

	m_bDuplexCollision = false;

	m_iDamage = 0;
}


CEffect::CEffect(const CEffect & obj) :
	CObj(obj)
{
	SetPivot(0.5f, 0.5f);

	m_tRelative = obj.m_tRelative;
	m_pTarget = nullptr;

	m_bPlayTime = obj.m_bPlayTime;
	m_fPlayLimitTime = obj.m_fPlayLimitTime;
	m_fPlayTime = 0.f;
	m_bDie = false;

	m_fKnockBackPower = obj.m_fKnockBackPower;

	m_bDuplexCollision = obj.m_bDuplexCollision;

	m_iDamage = obj.m_iDamage;


}

CEffect::~CEffect()
{
	SAFE_RELEASE(m_pTarget);
}


SIDE_DIR CEffect::GetDir() const
{

	if (!m_bAngle)
		return SD_NONE;
	// ������ ������� �ʴ� ��쿡�� none �� return.

	// ������ ����ϴ� ���.

	if (m_fAngle == 0.f)
		return SD_RIGHT;
	else if (m_fAngle == PI)
		return SD_LEFT;

	// 0 �� 180 �� ������ ���Ǹ� �ϰ�, �������� ���� none�� ó���� �Ѵ�.

	return SD_NONE;

}

void CEffect::Start()
{
	CObj::Start();
}

bool CEffect::Init()
{
	if (!CObj::Init())
		return false;

	return true;
}

void CEffect::Update(float fTime)
{
	CObj::Update(fTime);


	
	if (!m_bDie && m_bPlayTime)
	{
		m_fPlayTime += fTime;

		if (m_fPlayTime >= m_fPlayLimitTime)
		{
			m_bActive = false;
		}
	}
	
}

void CEffect::LateUpdate(float fTime)
{
	CObj::LateUpdate(fTime);

	
}

void CEffect::Collision(float fTime)
{
	CObj::Collision(fTime);

}

void CEffect::PrevRender(HDC hDC, float fTime)
{
	if (m_pTarget)
	{
		m_tPos = m_pTarget->GetPos() + m_tRelative;

		if (!m_pTarget->IsActive())
		{
			SAFE_RELEASE(m_pTarget);
			// ���� �ٴϴ� ��ü�� �׾��ٸ�, ������ Ǯ�������.
			// �׸��� �ڽ��� ��ġ�� ����Ʈ�� �����ϸ鼭 ���δ�.
			// ��ü������ ���ϸ� Effect�� ������ �ִ� Ÿ���� �״� ���� JumpStar MoveStar�� ��� �ۿ��� ����.
			// ��, ����Ʈ �������ְ� ���� ������� �Ǵ� ���̴�.

			CObj::StarEffectCreate(GetPos());
			m_bActive = false;
		}
	}

	CObj::PrevRender(hDC, fTime);

}

void CEffect::Render(HDC hDC, float fTime)
{
	CObj::Render(hDC, fTime);

}

void CEffect::PostRender(HDC hDC, float fTime)
{
	CObj::PostRender(hDC, fTime);

}

CEffect * CEffect::Clone() const
{
	return new CEffect(*this);
}

void CEffect::AddClipName(vector<string>& vecClipName)
{
}
