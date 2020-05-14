#include "Ref.h"



CRef::CRef() :
	m_iRef(1),
	m_bEnable(true),
	m_bActive(true)
{
	// �ʱ� ref�� 1�� �ְ�,
	// Ȱ��ȭ, ��ȿ�ϰ� �ش�.
}


CRef::~CRef()
{
}

void CRef::AddRef()
{
	++m_iRef;
}

int CRef::Release()
{
	--m_iRef;

	if (m_iRef == 0)
	{
		delete this;
		return 0;
	}

	return m_iRef;
}
