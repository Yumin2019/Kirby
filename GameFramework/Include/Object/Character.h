#pragma once
#include "Obj.h"

class CCharacter :
	public CObj
{

	DECLARE_OBJECT

protected:
	CCharacter();
	CCharacter(const CCharacter& obj);
	virtual ~CCharacter() = 0;

protected:
	class CInputComponent*	m_pInputComponent;
	SIDE_DIR				m_eDir;
	bool					m_bMove;
	float					m_fStopTime; // 정지시에 사용할 변수.
	bool					m_bStop;	// 정지 상태.
	

	int						m_iHP;
	int						m_iHPMax;

public:
	void SetHP(int iHP)
	{
		m_iHP = iHP;
		m_iHPMax = iHP;
		// 초기 설정으로 이용..
	}

public:

	int GetHP() const
	{
		return m_iHP;
	}

	int GetHPMax() const
	{
		return m_iHPMax;
	}


	bool IsDie() const
	{
		return m_iHP == 0;
	}

	SIDE_DIR GetDir() const
	{
		return m_eDir;
	}


	virtual void Damage(int iDamage)
	{
		m_iHP -= iDamage;

		if (m_iHP < 0)
			m_iHP = 0;
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
	virtual CCharacter* Clone() const;
	virtual void AddClipName(vector<string>& vecClipName) = 0;


};

