#include "InputComponent.h"



CInputComponent::CInputComponent()
{
}


CInputComponent::~CInputComponent()
{

	// Axis부터 날려볼까요.
	list<AddFunction>::iterator iter;
	list<AddFunction>::iterator iterEnd = m_FunctionAxisList.end();

	for (iter = m_FunctionAxisList.begin(); iter != iterEnd; ++iter)
	{
		GET_SINGLE(CInput)->DeleteBindAxis((*iter).strName, (*iter).pObj, (*iter).strFuncName);

		// 이 과정을 거치면, 삭제된 오브젝트의 멤버함수가 사라져서
		// 해당 함수를 호출하는 과정이 있을 수가 없다.

		// 구체적으로는 객체가 사라져서 InputComponent가 사라지는 경우에
		// 자신이 사라지면서 Input에 등록된 키 정보를 찾고,
		// 해당 키의 함수를 찾는다.
		// 그리고 여기서 가리키는 정보를 가지고 해당 함수를 지워버린다.
		//  InputComponent를 통해서 없애주는 처리를 하고 있는 것이다.
		// 그렇게 지워진 객체의 멤버함수를 지워주는 처리를 한다.
	}

	// Axis를 지웠으면 Action도 지운다.

	iterEnd = m_FunctionActionList.end();

	for (iter = m_FunctionActionList.begin(); iter != iterEnd; ++iter)
	{
		GET_SINGLE(CInput)->DeleteBindAction((*iter).strName, (*iter).pObj, (*iter).strFuncName);
	}

	// InputComponent는 자연스럽게 자신의 리소스를 정리하고,
	// 동시에 Input에 등록된 함수도 지워지는 처리를 하고 있다.
}
