#pragma once
#include "Character.h"
class CMonster :
	public CCharacter
{
	DECLARE_OBJECT

public:
	CMonster();
	CMonster(const CMonster& obj);
	virtual ~CMonster() = 0;

protected:
	float m_fChangeTime;
	float m_fChangePlayTime;
	bool m_bMove;

	bool m_bStop;
	float m_fStopTime;
	float m_fStopLimitTime;

	float m_fInitChangeTime;
	
	bool m_bChange;

	MONSTER_MODE m_eMode;

	MONSTER_TYPE m_eMonsterType;

	float		m_fHitTime;
	float		m_fHitPlayTime;
	bool		m_bHitAnimation;

	bool		m_bPrevGravity;

protected:
	

	virtual void IdleAnimation();
	virtual void DefaultIdleAnimation();




public:

	void SetMonsterType(MONSTER_TYPE eType)
	{
		m_eMonsterType = eType;
	}

	MONSTER_TYPE GetMonsterType() const
	{
		return m_eMonsterType;
	}


	void MoveDisEnable()
	{
		m_bMove = false;
		// 넉백 등에 대한 처리가 아닌, 단순히 Move =false 로 만들기 위한 함수이다.
		// 즉, 아예 안 움직이게 처리하는 상황에서 쓴다.
	}
	void MoveDisEnable(float fStopLimitTime, SIDE_DIR eCollisionDir);
	

	void SetChangeTime(float fLimitTime)
	{
		m_bMove = true;
		m_fInitChangeTime = fLimitTime;
		m_fChangePlayTime = fLimitTime;
	}



	virtual void Damage(int iDamage);




protected:
	void MoveX(float fTime);
	void MoveX(float fSpeed, float fTime);
	void DirChange();

public:

	virtual void Start();
	virtual bool Init();
	virtual void Update(float fTime);
	virtual void LateUpdate(float fTime);
	virtual void Collision(float fTime);
	virtual void PrevRender(HDC hDC, float fTime);
	virtual void Render(HDC hDC, float fTime);
	virtual void PostRender(HDC hDC, float fTime);
	virtual CMonster* Clone() 	const;
	virtual void HitAnimation();
	virtual void AddClipName(vector<string>& vecClipName) = 0;

	virtual void HitStart(CCollider* pSrc, CCollider* pDest, float fTime);
	// 이 함수는 Stop 시간을 설정하는 함수와는 별개이다.
	virtual void SetHitAnimation(float fPlayTime);

	void SetDir(SIDE_DIR eDir);
};

