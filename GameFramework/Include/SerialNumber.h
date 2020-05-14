#pragma once

#include "Game.h"

// SerialNumber�� ��ü�鸶�� ������ ���� ������ �Ѱ��ֱ� ���� �ʿ��� �ý����� �����ϴ� ������ �ϴ� ��ü�̴�.
// �ֿ� ������ SerialNumber�� �����ϰ�, �̰��� ����ϰ� ��ȯ�� ���ڸ� �����ϴ� ����.
// �׸��� �װ��� �ٽ� �ʿ��� ��ü�鿡�� �Ѱ��ִ� ������ ���ְ� �ִ�.
// �ش� ��ü�� CollisionManager�� ��ġ�� �Ǵµ� CollisionManager������ 
// �ش� ��ü�� �̿��Ͽ� SerialNumber���� �����ϴ� �Լ����� �����ϰ� �ִ�.

class CSerialNumber
{
public:
	CSerialNumber();
	~CSerialNumber();

private:
	unsigned int	m_iNumber; // �Ѱ��� ���� ��Ÿ����. �̸� ����Ű�� ���̶�� �� �� �ִ�.
	unsigned int*	m_pValidNumber; // ��ȿ�� �ѹ����̴�. ��ü�����δ� ��ü�� ����ϴ� ���ڸ� ��ȯ�ϸ�
	// �� �����Ҵ� �迭�� ������ �ǰ� �ʿ��� ��ü�� ������ �� ���⿡ ����� ���ڸ� �켱������ ��ȯ�Ѵ�.

	unsigned int	m_iSize; // return SerialNumber �� �����̴�.
	unsigned int	m_iCapacity; // �Ҵ緮

public:
	unsigned int	CreateSerialNumber(); // ��ȯ�� ���� �켱������ �Ҵ��� �ǰ�, ���ٸ� ���ڸ� ���� �����Ͽ� �Ѱ��ش�.
	void ReturnSerialNumber(unsigned int iNumber); // �ش� ���ڸ� �޾Ƽ� return SerialNumber Array�� �߰��ϴ� �Լ��̴�.
};

