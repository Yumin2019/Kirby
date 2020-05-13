#include "Ref.h"



CRef::CRef() :
	m_iRef(1),
	m_bEnable(true),
	m_bActive(true)
{
	// 초기 ref는 1로 주고,
	// 활성화, 유효하게 준다.
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
