#pragma once

#include "Game.h"

// SerialNumber는 객체들마다 고유한 숫자 정보를 넘겨주기 위해 필요한 시스템을 관리하는 역할을 하는 객체이다.
// 주요 역할은 SerialNumber를 생성하고, 이것을 사용하고 반환한 숫자를 저장하는 역할.
// 그리고 그것을 다시 필요한 객체들에게 넘겨주는 역할을 해주고 있다.
// 해당 객체는 CollisionManager에 배치가 되는데 CollisionManager에서는 
// 해당 객체를 이용하여 SerialNumber값을 관리하는 함수까지 포함하고 있다.

class CSerialNumber
{
public:
	CSerialNumber();
	~CSerialNumber();

private:
	unsigned int	m_iNumber; // 넘겨줄 값을 나타낸다. 미리 가리키는 값이라고 볼 수 있다.
	unsigned int*	m_pValidNumber; // 유효한 넘버값이다. 구체적으로는 객체가 사용하던 숫자를 반환하면
	// 이 동적할당 배열에 저장이 되고 필요한 객체가 생겼을 때 여기에 저장된 숫자를 우선적으로 반환한다.

	unsigned int	m_iSize; // return SerialNumber 의 개수이다.
	unsigned int	m_iCapacity; // 할당량

public:
	unsigned int	CreateSerialNumber(); // 반환된 값이 우선적으로 할당이 되고, 없다면 숫자를 새로 생성하여 넘겨준다.
	void ReturnSerialNumber(unsigned int iNumber); // 해당 숫자를 받아서 return SerialNumber Array에 추가하는 함수이다.
};

