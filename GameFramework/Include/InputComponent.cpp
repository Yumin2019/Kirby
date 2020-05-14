#include "InputComponent.h"



CInputComponent::CInputComponent()
{
}


CInputComponent::~CInputComponent()
{

	// Axis���� ���������.
	list<AddFunction>::iterator iter;
	list<AddFunction>::iterator iterEnd = m_FunctionAxisList.end();

	for (iter = m_FunctionAxisList.begin(); iter != iterEnd; ++iter)
	{
		GET_SINGLE(CInput)->DeleteBindAxis((*iter).strName, (*iter).pObj, (*iter).strFuncName);

		// �� ������ ��ġ��, ������ ������Ʈ�� ����Լ��� �������
		// �ش� �Լ��� ȣ���ϴ� ������ ���� ���� ����.

		// ��ü�����δ� ��ü�� ������� InputComponent�� ������� ��쿡
		// �ڽ��� ������鼭 Input�� ��ϵ� Ű ������ ã��,
		// �ش� Ű�� �Լ��� ã�´�.
		// �׸��� ���⼭ ����Ű�� ������ ������ �ش� �Լ��� ����������.
		//  InputComponent�� ���ؼ� �����ִ� ó���� �ϰ� �ִ� ���̴�.
		// �׷��� ������ ��ü�� ����Լ��� �����ִ� ó���� �Ѵ�.
	}

	// Axis�� �������� Action�� �����.

	iterEnd = m_FunctionActionList.end();

	for (iter = m_FunctionActionList.begin(); iter != iterEnd; ++iter)
	{
		GET_SINGLE(CInput)->DeleteBindAction((*iter).strName, (*iter).pObj, (*iter).strFuncName);
	}

	// InputComponent�� �ڿ������� �ڽ��� ���ҽ��� �����ϰ�,
	// ���ÿ� Input�� ��ϵ� �Լ��� �������� ó���� �ϰ� �ִ�.
}
