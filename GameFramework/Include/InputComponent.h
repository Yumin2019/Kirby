#pragma once
#include "Ref.h"
#include "Core/Input.h"

typedef struct _tagAddFunction
{
	string strName;
	void* pObj;
	string strFuncName;

	_tagAddFunction()
	{
		pObj = nullptr;
	}

}AddFunction, *PAddFunction;


class CInputComponent :
	public CRef
{

	friend class CInput;
	friend class CCharacter;
	friend class CMapEditScene;


private:
	CInputComponent();
	~CInputComponent();

private:
	list<AddFunction>	m_FunctionAxisList;
	list<AddFunction>	m_FunctionActionList;

public:

	// BindAxisInfo�� �Լ��� ����԰� ���ÿ�.
	// �̿� �ش��ϴ� ������ �����ϴ� �뵵�� ����Ѵ�.
	// ��ϵǴ� �Լ����� ��� ����Լ��� ����ϴ� 
	// �Լ����̴�.

	template <typename T>
	void BindAxis(const string& strName, T* pObj,
		void(T::*pFunc)(float, float),
		const string& strFuncName)
	{
		if (GET_SINGLE(CInput)->BindAxis(strName,
			pObj, pFunc, strFuncName))
		{
			// ����� ������ �Ǿ��ٸ� FunctionAxisList�� ������
			// �߰��ؾ��Ѵ�.

			AddFunction tFunc;

			tFunc.pObj = pObj;
			tFunc.strFuncName = strFuncName;
			tFunc.strName = strName;

			m_FunctionAxisList.push_back(tFunc);
		}
	}

	template <typename T>
	void BindAction(const string& strName,
		INPUT_EVENT eEvent, T* pObj,
		void (T::*pFunc)(float),
		const string& strFuncName)
	{
		// Action�� Ư�� ���ۿ����� ó���� �ؾ��ϴµ�,
		// �̰��� BindActionInfo���� ������ �� �ְ�,
		// Ű�� �߰��Ҷ� �ش� event�� �����ϴ� ���� 
		// �ƴ϶�, �Լ��� �߰��ϴ� �������� 
		// �̺�Ʈ�� �������ش�.

		if (GET_SINGLE(CInput)->BindAction(strName, eEvent,
			pObj ,pFunc, strFuncName))
		{
			AddFunction tFunc;

			tFunc.pObj = pObj;
			tFunc.strFuncName = strFuncName;
			tFunc.strName = strName;

			m_FunctionActionList.push_back(tFunc);
		}
	}

	// Input�� ���ؼ� �Լ��� ����ϴ� ���� �ƴϴ�.
	// m_pInputcomponent�� ���ؼ� �ش� ��ü��
	// ����Լ��� ����Ѵ�.
	// �׷��� �ڵ����� Input���� �ش� Action, Axis�� ã�Ƽ�
	// �Լ��� �߰����ִ� ������ ��ġ�� �ִ�.

	// �ƹ�ư �츮�� InputComponent�� ������ ����ϸ� �ȴ�.
};

