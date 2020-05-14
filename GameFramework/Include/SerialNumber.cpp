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
	// �����Ҵ��ϰ� �ִ� m_pValidSerialNumber�� ��ȯ���ش�.
	SAFE_DELETE_ARRAY(m_pValidNumber);
}

unsigned int CSerialNumber::CreateSerialNumber()
{
	// �ϴ�, ��ȯ�� SerialNumber���� �ִ��� Ȯ���Ѵ�.

	if (m_iSize > 0)
	{
		// ���� �ֱٿ� ��ȯ�� ���ں��� �Ѱ��ش�.
		--m_iSize;

		return m_pValidNumber[m_iSize];
	}

	// ���� SerialNumber���� ���ٸ�, ���ο� ���ڸ� �����ؼ� �Ѱ��ش�.

	unsigned int iNumber = m_iNumber;

	++m_iNumber;

	return iNumber;
}

void CSerialNumber::ReturnSerialNumber(unsigned int iNumber)
{
	// ���� ���ڸ� ValidNumber�� �߰��Ѵ�.

	if (m_iSize == m_iCapacity)
	{
		// ���� �Ҵ��� �޴´�.

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


// ���� ���ڱ� ��ü�� 100���� ������ �ȴٸ�, 0 ~ 99 ������ SerialNumber�� ������ �ǰ�, 
// ���Ŀ� ��ü ���� �� �������ٸ� ���ο� �Ҵ���, �� �������ٸ� ������ �����ߴ� SerialNumber�� ���
// ������ ����ϴ� �ý����̴�.
// ��ü�� �Ҹ��� �Ǹ鼭 �ڱⰡ ������ �ִ� SerialNumber�� ��ȯ�ϸ� ���� ������ �Ǵ� ��ü�� �̰��� �ٽ� 
// �������� ����ϴ� �ý����̴�.
// ��, SerialNumber���� ������ ���� �ȴ�.