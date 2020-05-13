#pragma once
#include "Obj.h"
class CEffect :
	public CObj
{
	DECLARE_OBJECT

protected:
	CEffect();
	CEffect(const CEffect& obj);
	virtual ~CEffect();

protected:
	CObj*		m_pTarget;
	Position	m_tRelative;

	float		m_fKnockBackPower;

	bool		m_bDuplexCollision; // 다중 충돌
	int			m_iDamage;


public:


	SIDE_DIR GetDir() const;
	
	void SetDamage(int iDamage)
	{
		m_iDamage = iDamage;
	}

	void DeplexCollisionEnable()
	{
		m_bDuplexCollision = true;
	}

	void SetTarget(CObj* pObj)
	{
		m_pTarget = pObj;
		
		if(pObj)
			pObj->AddRef();
	}

	void SetKnockBackPower(float fPower)
	{
		m_fKnockBackPower = fPower;
	}


	void SetRelative(float x, float y)
	{
		m_tRelative = Position(x, y);
	}

	void SetRelative(const Position& tRelative)
	{
		m_tRelative = tRelative;
	}

public:
	bool GetDie() const
	{
		return m_bDie;
	}

	int GetDamage() const
	{
		return m_iDamage;
	}

	bool GetDuplexCollision() const
	{
		return m_bDuplexCollision;
	}

	float GetKnockBackPower() const
	{
		return m_fKnockBackPower;
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
	virtual CEffect* Clone()	const;
	virtual void AddClipName(vector<string>& vecClipName);

};

