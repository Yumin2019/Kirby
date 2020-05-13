#pragma once

#include "Game.h"

class CRef
{
public:
	CRef();
	virtual ~CRef();

protected:
	int			m_iRef; // Ref변수
	string		m_strTag;	// Tag설정
	bool		m_bEnable;	 // Enable은 활성화를 의미한다.
	bool		m_bActive;	// Active는 유효한 데이터인지를 의미한다.

public:
	void AddRef();
	int Release();

	void SetTag(const string& strTag)
	{
		m_strTag = strTag;
	}

	string GetTag() const
	{
		return m_strTag;
	}

	void SetActive(bool bActive)
	{
		m_bActive = bActive;
	}

	bool IsActive() const
	{
		return m_bActive;
	}

	bool IsEnable() const
	{
		return m_bEnable;
	}

	void SetEnable(bool bEnable)
	{
		m_bEnable = bEnable;
	}
};

