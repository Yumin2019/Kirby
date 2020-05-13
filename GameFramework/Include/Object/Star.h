#pragma once
#include "Effect.h"
class CStar :
	public CEffect
{
protected:
	CStar();
	CStar(const CStar& obj);
	virtual ~CStar();


protected:
	// ¸â¹ö º¯¼ö.
	MONSTER_TYPE m_eAbility;

public:
	MONSTER_TYPE GetAbilityType() const
	{
		return m_eAbility;
	}

	void SetAbility(MONSTER_TYPE eType)
	{
		m_eAbility = eType;
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
	virtual CStar* Clone()	const;
	virtual void AddClipName(vector<string>& vecClipName);
};

