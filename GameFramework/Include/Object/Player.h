#pragma once
#include "Character.h"


enum SWORD_ATTACK
{
	SA_NONE,
	SA_ONE,
	SA_TWO,
	SA_THREE,
	SA_IDLE,
	SA_JUMP_ATTACK
};


enum KIRBY_TYPE
{
	KIRBY_NORMAL,
	KIRBY_STONE,
	KIRBY_SWORD,
	KIRBY_MIKE,
	KIRBY_SLEEP
};

enum MIKE_ATTACK
{
	MA_NONE,
	MA_1,
	MA_2,
	MA_END
};


class CPlayer :
	public CCharacter
{
	DECLARE_OBJECT

public:
	CPlayer();
	CPlayer(const CPlayer& obj);
	virtual ~CPlayer();

private:


	// ���� ����� ���ɼ��� �ִ� ����.
	float	m_fAttackTime;
	bool	m_bAttack;


	// Kirby 
	bool	m_bBig;
	bool	m_bEat;
	bool	m_bTake; // ���� ��������� �ʴ� �� ���䵥 �ѵ�, �ϴ� ��ü�� ������ �ִ��� �Ǵ��ϴµ� �����.
	float	m_fEatTime;
	class CEffect* m_pTornadoEffect;
	list<CObj*> m_vecMonsterList;

	float	m_fTime; // ���� ��ø������ �����.
	float	m_fTakeTime;
	

	// Power������ ���� ����ؼ� ������ ���� ������ �ִ� �������� ���� �д�.
	float m_fVomitTime;
	float m_fBreathTime;

	// �Ϲ� Ŀ���� �浹ü Ȱ��ȭ ��Ȱ��ȭ�� ���� ���θ� ��Ƴ��� �����̴�.
	bool m_bAttackColliderEnable;



	// All

	KIRBY_TYPE	m_eType;
	bool	m_bJump;
	float	m_fJumpTime;
	bool	m_bSlide;
	float	m_fSlideTime;
	bool	m_bVomit;

	bool	m_bDown;
	bool	m_bRun;
	bool	m_bPrevent;

	float m_fSlidePixel;
	float m_fPower;
	float m_fPowerTime;

	bool m_bBalloon;
	bool m_bFalling;

	float m_fRunTime;
	bool m_bBreath;
	float m_fSlideLimitTime;
	float m_fSlideLimitPixel;

	int		m_iAttackedCount;

	bool	m_bIsDie;


	// Stone Kirby

	bool m_bStone;
	bool m_bStoneEffect;

	// Sword Kirby
	SWORD_ATTACK	m_eSwordAttack;
	SWORD_ATTACK	m_eSwordState;
	bool			m_bSwordAttack;
	float			m_fSwordAttackTime;
	bool			m_bDashAttack;
	bool			m_bJumpAttack;
	bool			m_bSwordEffect;

	// Mike Kirby
	MIKE_ATTACK m_eMikeAttack;
	class CMikeObject* m_pMikeObject;

	// Sleep
	bool m_bSleep;

	// ������ �� ���Ŀ� �� �ʰ� ������ �ٽ� �� �� ������ �ϴ� ����.
	bool	m_bAttackEnd;
	float	m_fAttackEndTime;
	float	m_fAttackEndLimitTime;

	// ���� ���� Ÿ���� �޾Ƶд�.
	MONSTER_TYPE m_eMonsterType;

	float m_fDeathTime;

	

private:
	void AddAttacked();
	void ThrowAbility();
	void MonsterCollision(CObj* pObj);

public:
	void ChangeIdleClip(KIRBY_TYPE eType);

	void CreateTonadoEffect();
	void DisEnableTornadoEffect();
	void RunDisEnable();
	void SleepEnable();

	void CreateBreathEffect();
	void BalloonStageCollision();

	// Mike
	bool GetAttack() const
	{
		return m_bAttack;
	}

	MIKE_ATTACK GetMikeType() const
	{
		return m_eMikeAttack;
	}

	// Stone

	bool GetStone() const
	{
		return m_bStone;
	}

	// Normal

	bool GetRun() const
	{
		return m_bRun;
	}

	bool JumpDown() const
	{
		return m_fPower < 1450.f;
	}
	
	void FallingDisEanble()
	{
		m_bFalling = false;
	}

	void JumpDisEnable()
	{
		m_bJump = false;
	}

	bool GetBallon() const
	{
		return m_bBalloon;
	}


	void BallonDisEnable()
	{
		m_bBalloon = false;
	}

	bool GetDown() const
	{
		return m_bDown;
	}

	bool GetJump() const
	{
		return m_bJump;
	}

	bool GetStop() const
	{
		return m_bStop;
	}

	bool GetPrevent() const
	{
		return m_bPrevent;
	}

	bool GetVomit() const
	{
		return m_bVomit;
	}

	KIRBY_TYPE GetKirbyType() const
	{
		return m_eType;
	}
	
	bool GetAttackColliderEnable() const
	{
		return m_bAttackColliderEnable;
	}

public:
	virtual void Start();
	virtual bool Init();
	virtual void Update(float fTime);
	virtual void LateUpdate(float fTime);
	virtual void Collision(float fTime);
	virtual void PrevRender(HDC hDC, float fTime);
	virtual void Render(HDC hDC, float fTime);
	virtual void PostRender(HDC hDC, float fTime);
	virtual CPlayer* Clone() 	const;
	virtual void AddClipName(vector<string>& vecClipName);

	// Damage�Լ� ������

	virtual void Damage(int iDamage)
	{
		CCharacter::Damage(iDamage);

		AddAttacked();
	}


private:
	void AddMonsterVector(CObj* pObj);
	void KirbyChangeClip(enum PLAYER_NORMAL_CLIP eClip);
	void KirbyDefaultClip(enum PLAYER_NORMAL_CLIP eClip);

	void StoneKirbyChangeClip(enum STONE_CLIP eClip);
	void StoneKirbyDefaultClip(enum STONE_CLIP eClip);

	void SwordKirbyChangeClip(enum SWORD_CLIP eClip);
	void SwordKirbyDefaultClip(enum SWORD_CLIP eClip);

	void EasyKirbyChangeClip(enum EASY_CLIP eClip);
	void EasyKirbyDefaultClip(enum EASY_CLIP eClip);

	void SwordAniBulletCreate();
	void SwordBulletCreate();


	void KirbyConversionKey();
	void KirbyConversion();
	void CreateClip();
	void MoveSide(float fScale, float fTime);
	void MoveUp(float fTime);
	void Down(float fTime);
	void DownUp(float fTime);
	void Jump(float fTime);
	void Run(float fTime);
	void Throw(float fTime);
	void Prevent(float fTime);
	void PreventUp(float fTime);

	// ��� : ���ҽ������� ó���� �ٸ��� �װ� �ƴϸ� ���� �κ�.

	void Attack(float fTime);
	void AttackUp(float fTime);
	void JumpUpdate(float fTime);
	void KirbyUpdate(float fTime);

	void StoneAttack(float fTime);
	void StoneAttackUp(float fTime);
	void StoneJumpUpdate(float fTime);
	void StoneUpdate(float fTime);


	void SwordAttack(float fTime);
	void SwordAttackUp(float fTime);
	void SwordJumpUpdate(float fTime);
	void SwordUpdate(float fTime);


	void MikeAttack(float fTime);
	void MikeAttackUp(float fTime);
	void MikeJumpUpdate(float fTime);
	void MikeUpdate(float fTime);


	void SleepAttack(float fTime);
	void SleepAttackUp(float fTime);
	void SleepJumpUpdate(float fTime);
	void SleepUpdate(float fTime);
	
	// �⺻ Ŀ������ �Լ�.
	void PlayerAttack(CCollider* pSrc, CCollider* pDest, float fTime);
	// Ư���� ��Ȳ���� �浹�� �Ǳ� �����Ͽ� ��ü�� ������� ���Ŀ� �ٽ� Ǯ���ֱ� ���ؼ� �� �Լ��� �����.
	void PlayerAttackRelease(CCollider* pSrc, CCollider* pDest, float fTime);

	// �ִϸ��̼� �����ϴ� �Լ�. left right ������� ��� �ǵ���.

	void BodyCollisionStart(CCollider* pSrc, CCollider* pDest, float fTime);
	void BodyCollisionStay(CCollider* pSrc, CCollider* pDest, float fTime);
	void BodyCollisionEnd(CCollider* pSrc, CCollider* pDest, float fTime);

	// Sword JumpAttack Collider
	void SwordJumpAttackColliderStart(CCollider* pSrc, CCollider* pDest, float fTime);

	void SetAttackEndTime(float fTime);


private:
	void EasyKirbySource();
	void StoneKirbySource();
	void SwordKirbySource();





};

