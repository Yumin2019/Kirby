#pragma once
#include "Collider.h"
class CColliderLine :
	public CCollider
{
	friend class CObj;

private:
	CColliderLine();
	CColliderLine(const CColliderLine& collider);
	~CColliderLine();

private:
	
	// Stage �������� ���ȴ�. ��ǥ�� ������� ��ġ�� �ٷ� ��ƹ����� �� �� ����.
	
	Position m_tSrcPos;
	Position m_tDestPos;
	float	 m_fInclination;

	RectInfo	m_tLineRect;

private:
	void SetLineRect();

public:
	
	RectInfo GetLineRect() const
	{
		return m_tLineRect;
	}

	void SetPos(const Position& tSrc, const Position& tDest)
	{
		m_tSrcPos = tSrc;
		m_tDestPos = tDest;

		Position tMove = m_tDestPos - m_tSrcPos;
		m_fInclination = tMove.y / tMove.x;

		SetLineRect();
	}

	void SetPos(int srcX, int srcY, int destX, int destY)
	{
		m_tSrcPos = Position(srcX, srcY);
		m_tDestPos = Position(destX, destY);

		Position tMove = m_tDestPos - m_tSrcPos;
		m_fInclination = tMove.y / tMove.x;

		SetLineRect();
	}

public:
	Position GetSrcPos() const
	{
		return m_tSrcPos;
	}

	Position GetDestPos() const
	{
		return m_tDestPos;
	}

	// ������ �������� �̿��ؼ� x ��ǥ�� �־��ָ� y��
	// y ��ǥ�� �־��ָ� x �� ���� ��ȯ�ϴ� �Լ��̴�.
	float GetX(float y);
	float GetY(float x);
	
public:
	virtual void Start();
	virtual bool Init();
	virtual void Update(float fTime);
	virtual void LateUpdate(float fTime);
	virtual void PrevRender(HDC hDC, float fTime);
	virtual void Render(HDC hDC, float fTime);
	virtual void PostRender(HDC hDC, float fTime);
	virtual CColliderLine* Clone() const;
	virtual bool Collision(CCollider* pCollider);
	virtual void UpdateObjPos(const Position& tPos);
};

