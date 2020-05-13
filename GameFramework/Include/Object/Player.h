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


	// 같이 사용할 가능성이 있는 변수.
	float	m_fAttackTime;
	bool	m_bAttack;


	// Kirby 
	bool	m_bBig;
	bool	m_bEat;
	bool	m_bTake; // 굳이 사용하지는 않는 것 같긴데 한데, 일단 물체를 가지고 있는지 판단하는데 사용함.
	float	m_fEatTime;
	class CEffect* m_pTornadoEffect;
	list<CObj*> m_vecMonsterList;

	float	m_fTime; // 흡입 중첩값으로 사용함.
	float	m_fTakeTime;
	

	// Power변수를 같이 사용해서 문제가 생길 여지가 있는 변수들을 따로 둔다.
	float m_fVomitTime;
	float m_fBreathTime;

	// 일반 커비의 충돌체 활성화 비활성화에 대한 여부를 담아놓을 변수이다.
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

	// 공격을 한 이후에 몇 초간 공격을 다시 할 수 없도록 하는 변수.
	bool	m_bAttackEnd;
	float	m_fAttackEndTime;
	float	m_fAttackEndLimitTime;

	// 먹은 몬스터 타입을 받아둔다.
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

	// Damage함수 재정의

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

	// 상단 : 리소스에서만 처리가 다르고 그게 아니면 같은 부분.

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
	
	// 기본 커비전용 함수.
	void PlayerAttack(CCollider* pSrc, CCollider* pDest, float fTime);
	// 특정한 상황에서 충돌이 되기 시작하여 물체를 집어먹은 이후에 다시 풀어주기 위해서 이 함수를 만든다.
	void PlayerAttackRelease(CCollider* pSrc, CCollider* pDest, float fTime);

	// 애니메이션 변경하는 함수. left right 순서대로 출력 되도록.

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

