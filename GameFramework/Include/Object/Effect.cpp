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
	// 각도를 사용하지 않는 경우에는 none 을 return.

	// 각도를 사용하는 경우.

	if (m_fAngle == 0.f)
		return SD_RIGHT;
	else if (m_fAngle == PI)
		return SD_LEFT;

	// 0 도 180 도 에서만 정의를 하고, 나머지의 경우는 none을 처리를 한다.

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
			// 따라 다니던 물체가 죽었다면, 참조를 풀어버린다.
			// 그리고 자신의 위치에 이펙트를 생성하면서 죽인다.
			// 구체적으로 말하면 Effect가 가지고 있는 타겟이 죽는 경우는 JumpStar MoveStar의 경우 밖에는 없다.
			// 즉, 이펙트 생성해주고 같이 사라지면 되는 것이다.

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
