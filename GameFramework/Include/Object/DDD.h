#pragma once
#include "Monster.h"

enum DDD_STATE
{
	DS_IDLE,
	DS_WALK,
	DS_JUMP_ATTACK,
	DS_BALLOON_ATTACK,
	DS_VOMIT,
	DS_ATTACK,
	DS_RUN
};
class CDDD :
	public CMonster
{
	DECLARE_OBJECT

public:
	CDDD();
	CDDD(const CDDD& obj);
	virtual ~CDDD();

private:

	DDD_STATE m_eState;

	int m_iJumpCount;
	bool m_bJump;
	float m_fPower;

	float m_fTime;
	float m_fPlayTime;

	CPlayer* m_pPlayer;

	float m_fPowerTime;

	bool m_bJumpTime;
	float m_fJumpTime;
	bool m_bJumpTimeEnable;

	float m_fJumpSpeedX;

	float m_fWalkTime;
	bool m_bAttack;

	bool m_bFalling;

	bool m_bDDD;

	Position m_tDiePos;


public:
	void InitIdle();
	DDD_STATE GetState() const
	{
		return m_eState;
	}
	


private:
	void JumpAttackMoveStar();
	void AttackMoveStar();
public:

	bool GetFalling() const
	{
		return m_bFalling;
	}

	void SetFalling(bool bFalling)
	{
		m_bFalling = bFalling;
	}

	void SetPlayer(class CPlayer* pPlayer);

	bool GetJump() const
	{
		return m_bJump;
	}

	void JumpDisEnable()
	{
		m_bJump = false;
	}	

	bool JumpDown() const
	{
		return m_fPower < 500.f;
	}

	bool GetDie() const
	{
		return m_bDDD;
	}
	

private:
	void ChangeDDDClip(enum DDD_CLIP eClip);
	void DefaultDDDClip(enum DDD_CLIP eClip);


public:
	virtual void Damage(int iDamage);

	virtual void Start();
	virtual bool Init();
	virtual void Update(float fTime);
	virtual void LateUpdate(float fTime);
	virtual void Collision(float fTime);
	virtual void PrevRender(HDC hDC, float fTime);
	virtual void Render(HDC hDC, float fTime);
	virtual void PostRender(HDC hDC, float fTime);
	virtual CDDD* Clone() 	const;
	virtual void HitAnimation();
	virtual void AddClipName(vector<string>& vecClipName);
	virtual void IdleAnimation();
	virtual void HitStart(CCollider* pSrc, CCollider* pDest, float fTime);

	void AttackColliderDisEnable();
};

