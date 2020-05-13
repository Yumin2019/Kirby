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

	// BindAxisInfo의 함수를 등록함과 동시에.
	// 이에 해당하는 정보를 저장하는 용도로 사용한다.
	// 등록되는 함수들은 모두 멤버함수로 등록하는 
	// 함수들이다.

	template <typename T>
	void BindAxis(const string& strName, T* pObj,
		void(T::*pFunc)(float, float),
		const string& strFuncName)
	{
		if (GET_SINGLE(CInput)->BindAxis(strName,
			pObj, pFunc, strFuncName))
		{
			// 제대로 생성이 되었다면 FunctionAxisList에 정보를
			// 추가해야한다.

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
		// Action은 특정 동작에대한 처리를 해야하는데,
		// 이것을 BindActionInfo에서 설정할 수 있고,
		// 키를 추가할때 해당 event를 데입하는 것이 
		// 아니라, 함수를 추가하는 과정에서 
		// 이벤트를 설정해준다.

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

	// Input을 통해서 함수를 등록하는 것이 아니다.
	// m_pInputcomponent를 통해서 해당 객체의
	// 멤버함수를 등록한다.
	// 그러면 자동으로 Input에서 해당 Action, Axis를 찾아서
	// 함수를 추가해주는 과정을 거치고 있다.

	// 아무튼 우리는 InputComponent를 가지고 등록하면 된다.
};

