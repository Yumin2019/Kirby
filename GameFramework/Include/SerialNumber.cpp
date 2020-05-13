#include "SerialNumber.h"



CSerialNumber::CSerialNumber()
{
	m_iSize = 0;
	m_iNumber = 0;
	m_iCapacity = 16;
	m_pValidNumber = new unsigned int[m_iCapacity];
	memset(m_pValidNumber, 0, sizeof(unsigned int) * m_iCapacity);
}


CSerialNumber::~CSerialNumber()
{
	// 동적할당하고 있는 m_pValidSerialNumber를 반환해준다.
	SAFE_DELETE_ARRAY(m_pValidNumber);
}

unsigned int CSerialNumber::CreateSerialNumber()
{
	// 일단, 반환된 SerialNumber값이 있는지 확인한다.

	if (m_iSize > 0)
	{
		// 가장 최근에 반환된 숫자부터 넘겨준다.
		--m_iSize;

		return m_pValidNumber[m_iSize];
	}

	// 여유 SerialNumber값이 없다면, 새로운 숫자를 생성해서 넘겨준다.

	unsigned int iNumber = m_iNumber;

	++m_iNumber;

	return iNumber;
}

void CSerialNumber::ReturnSerialNumber(unsigned int iNumber)
{
	// 받은 숫자를 ValidNumber에 추가한다.

	if (m_iSize == m_iCapacity)
	{
		// 새로 할당을 받는다.

		m_iCapacity *= 2;

		unsigned int* pValid = new unsigned int[m_iCapacity];
		memset(pValid, 0, sizeof(unsigned int) * m_iCapacity);

		memcpy(pValid, m_pValidNumber, sizeof(unsigned int) * m_iSize);

		SAFE_DELETE_ARRAY(m_pValidNumber);

		m_pValidNumber = pValid;
		
	}

	m_pValidNumber[m_iSize] = iNumber;
	++m_iSize;
}


// 만약 갑자기 객체가 100개가 생성이 된다면, 0 ~ 99 까지의 SerialNumber가 생성이 되고, 
// 이후에 객체 수가 더 많아진다면 새로운 할당을, 더 적어진다면 기존에 생성했던 SerialNumber를 계속
// 돌려서 사용하는 시스템이다.
// 객체가 소멸이 되면서 자기가 가지고 있던 SerialNumber를 반환하면 새로 생성이 되는 객체가 이것을 다시 
// 가져가서 사용하는 시스템이다.
// 즉, SerialNumber값은 고유한 값이 된다.