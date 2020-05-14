#pragma once

#include "Game.h"

class CRef
{
public:
	CRef();
	virtual ~CRef();

protected:
	int			m_iRef; // Ref����
	string		m_strTag;	// Tag����
	bool		m_bEnable;	 // Enable�� Ȱ��ȭ�� �ǹ��Ѵ�.
	bool		m_bActive;	// Active�� ��ȿ�� ������������ �ǹ��Ѵ�.

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

