
#include "Monster.h"
class CTwizzy :
	public CMonster
{
	DECLARE_OBJECT

public:
	CTwizzy();
	CTwizzy(const CTwizzy& obj);
	virtual ~CTwizzy();

private:
	// ¸â¹ö º¯¼ö
	bool	m_bTypeFly;

	bool m_bJump;
	float m_fPower;
	float m_fPowerTime;

	Size m_tStageSize;

	CObj* m_pPlayer;


public:
	void JumpDisEnable()
	{
		m_bJump = false;
	}

	bool GetJump() const
	{
		return m_bJump;
	}

	void SetFlyEnable()
	{
		m_bTypeFly = true;
	}

	void SetLineCollision()
	{
		m_fChangeTime = 0.f;
		m_bJump = false;
	}

	virtual void Start();
	virtual bool Init();
	virtual void Update(float fTime);
	virtual void LateUpdate(float fTime);
	virtual void Collision(float fTime);
	virtual void PrevRender(HDC hDC, float fTime);
	virtual void Render(HDC hDC, float fTime);
	virtual void PostRender(HDC hDC, float fTime);
	virtual CTwizzy* Clone() 	const;
	virtual void HitAnimation();
	virtual void AddClipName(vector<string>& vecClipName);
	virtual void IdleAnimation();
	virtual void HitStart(CCollider* pSrc, CCollider* pDest, float fTime);
	virtual void DefaultIdleAnimation();

};
