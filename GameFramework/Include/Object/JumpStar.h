#pragma once
#include "Star.h"
class CJumpStar :
	public CStar
{

	DECLARE_OBJECT

protected:
	CJumpStar();
	CJumpStar(const CJumpStar& obj);
	virtual ~CJumpStar();


protected:
	// ¸â¹ö º¯¼ö.
	bool m_bJump;
	float m_fPower;
	float m_fPowerTime;

	int		m_iJumpCount;
	int		m_iJumpCountMax;

	bool m_bMove;

public:
	void SetPower(float fPower, int iJumpCountMax = 6)
	{
		m_fPower = fPower;
		m_bJump = true;
		m_fPowerTime = 0.f;
		m_iJumpCount = 0;
		m_iJumpCountMax = iJumpCountMax;
	}

	void JumpDisEnable()
	{
		m_bJump = false;
	}

	bool JumpDown() const
	{
		return m_fPower < 1000.f;
	}

	void MoveDisEnable()
	{
		m_bMove = false;
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
	virtual CJumpStar* Clone()	const;
	virtual void AddClipName(vector<string>& vecClipName);
};

