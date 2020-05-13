#pragma once

#include "../Game.h"

// Input 끝.

enum SKEY
{
	SKEY_CTRL,
	SKEY_ALT,
	SKEY_SHIFT,
	SKEY_END
};

enum SKEY_TYPE
{
	ST_CTRL = 0x1,
	ST_ALT = 0x2,
	ST_SHIFT = 0x4
};

typedef struct _tagKeyFunction
{
	function<void(float, float)>	func;
	void*	pObj;
	string	strFuncName;

	_tagKeyFunction()
	{
		pObj = nullptr;
	}


	// KeyFunction은 AxisKeyInfo에 들어가는 정보이다.
	// 따라서 fScale값을 받는다.

}KeyFunction, *PKeyFunction;


typedef struct _tagActionKeyFunction
{
	function<void(float)>	func;
	void*	pObj;
	string	strFuncName;
	INPUT_EVENT eEvent;

	_tagActionKeyFunction()
	{
		pObj = nullptr;
	}

}ActionKeyFunction, *PActionKeyFunction;

typedef struct _tagKeyInfo
{
	unsigned char	cKey;
	bool	bSKey[SKEY_END];
	bool	bDown;
	bool	bHold;
	bool	bUp;

	_tagKeyInfo()
	{
		cKey = 0;
		bDown = false;
		bHold = false;
		bUp = false;

		for (int i = 0; i < SKEY_END; ++i)
		{
			bSKey[i] = false;
		}
	}

	// KeyInfo에 들어가는 정보는, 일반키와 특수키의 배열, 그리고, 여기서 상태정보는 일반키를 위한 정보이다.
	// 특수키에 대한 정보는, 해당키를 눌러야 하는지에 대한 정보만을 가지고 있도록 하고,
	// 이 특수키를 눌러야 하는지에 대한 여부를 가지고 제대로 눌렀는지 확인하는 것은 함수에서 미리 처리를 해주고 있다.
	// (따로 정보를 가지고 있으니 할 수 있다.)
	
}KeyInfo, *PKeyInfo;

typedef struct _tagAxisInfo
{
	KeyInfo	tKey;
	float	fScale;
	// AxisInfo 정보에서는 fScale 값과 KeyInfo 정보를 가지고 있다.
	// 즉, 일반키와 특수키의 조합과 입력상태, 그리고  Scale값을 가지고 있는 것이다.
}AxisInfo, *PAxisInfo;

typedef struct _tagBindAxisInfo
{
	string		strName;
	list<AxisInfo>	KeyList;
	list<KeyFunction>	FunctionList;
	// BindAxisInfo의 정보에는 해당 BindAxisInfo를 나타내기 위한 string 
	// BindAxisInfo 소속의 KeyList와 KeyFunctionList 를 포함하고 있다.
}BindAxisInfo, *PBindAxisInfo;

typedef struct _tagBindActionInfo
{
	// BindActionInfo 에는 Event 정보가 들어가고,
	// 더블클릭을 확인하기 위한 bDoubleStart와 fDoubleTime이 들어간다.
	// fScale값이 필요가 없으므로, AxisKeyInfo가 아닌, KeyInfo List를 사용하고 있다.
	// ActionKeyFunction은 KeyFunction과 function객체에서만 차이가 난다.
	string		strName;
	bool		bDoubleStart;
	float		fDoubleTime;
	list<KeyInfo>	KeyList;
	list<ActionKeyFunction>	FunctionList;

	_tagBindActionInfo() :
		fDoubleTime(0.f),
		bDoubleStart(false)
	{
	}
}BindActionInfo, *PBindActionInfo;

typedef struct _tagCommandInfo
{

}CommandInfo, *PCommandInfo;

typedef struct _tagInputKey
{
	bool	bEnable;
	unsigned char	cKey;
	bool	bDown;
	bool	bHold;
	bool	bUp;

	_tagInputKey()
	{
		cKey = 0;
		bDown = false;
		bHold = false;
		bUp = false;
		bEnable = false;
	}

	// 실제로 해당키가 눌렸는지 확인할때 사용하는 정보이다.
	// 여러 키의 조합에 대한 상태 정보가 아닌(AxisInfo나 ActionInfo에 저장하는 상태정보가 아니라, 
	// 키 하나하나에 대한 상태정보를 의미한다.)
}InputKey, *PInputKey;


// BindAction과는 상관없는 키 입력 시스템이다.
// 단순히 어떤 상태인지 비교해서 처리를 하는 시스템을 만든다.

typedef struct _tagNormalKeyInfo
{
	string			strName;
	vector<DWORD>	vecKey;
	bool			bDown;
	bool			bPress;
	bool			bUp;

	// 단순히 KeyNormalInfo를 구분하기 위한 strName정보와
	// 여러키를 동시에 적용하기 위한 vector
	// 그리고 해당 키를 눌렀는지 확인하기 위한 상태정보.

	_tagNormalKeyInfo() :
		bDown(false),
		bPress(false),
		bUp(false)
	{

	}

}NormalKeyInfo, *PNormalKeyInfo;

class CInput
{

	DECLARE_SINGLE(CInput)



	friend class CInputComponent;	

private:
	unordered_map<string, PBindAxisInfo>	m_mapBindAxis;		// BindAxisInfo에 대한 정보를 담아둘  map
	unordered_map<string, PBindActionInfo>	m_mapBindAction;	// BindActionInfo 에 대한 정보를 담아둘 map
																// 이것을 동적할당하여 담아둘 것이다.

	vector<InputKey>	m_KeyList;		// 일반키에 대한 입력 여부를 저장할 vector
	InputKey			m_tSKey[SKEY_END];	// 특수키에 대한 입력 여부를 저장할 배열.


	// NormalKey에 대한 처리.
	PNormalKeyInfo		m_pNormalKey;
	unordered_map<string, PNormalKeyInfo>	m_mapNormalKey;


	// Mouse : 마우스를 Scene의 소속으로 두는 것이 아니라, 따로 가지고서 처리하는 시스템이다.
	Position			m_tMousePos;
	Position			m_tMousePrevPos;
	Position			m_tMouseMove;
	class	CObj*		m_pMouse;
	class	CCollider*  m_pMouseCollider;
	class	CAnimation* m_pMouseAnimation;
	HWND				m_hWnd;

	bool				m_bIsScreen;

public:
	// Mouse

	bool MouseIsScreen() const
	{
		return m_bIsScreen;
	}

	class CCollider* GetMouseCollider() const
	{
		return m_pMouseCollider;
	}

	Position GetMousePos() const
	{
		return m_tMousePos;
	}

	Position GetMouseMove() const
	{
		return m_tMouseMove;
	}

	Position GetMousePrevPos() const
	{
		return m_tMousePrevPos;
	}

public:
	bool Init(HWND hWnd);		// Init 기본키에 대한 정보와 특수키에 대한 정보를 추가한다.
	void Update(float fTime);  // 기본키에 대한 입력 처리 + 특수키에 대한 입력처리 
								// 모든 키를 입력처리한 후에 이 키에 대한 정보를 조합하여, map에 존재하는
								// AxisInfo 정보와 ActionInfo 정보를 만족하는지에 대한 여부 검사.
	void Render(HDC hDC, float fTime);

private:
	void SKeyInput();	// 특수키에 대한 입력 정보.
	void KeyInput();	// 일반키에 대한 입력 정보
	void AxisInput(float fTime);	// 입력 정보를 바탕으로 한 AxisInfo 정보 인식
	void ActionInput(float fTime);	// 입력 정보를 바탕으로 한 ActionInfo 정보 인식
						// 입력을 확인하는 과정에서 해당 함수를 호출할 것이다.
						// 근데 이 함수의 인자로 fTime변수를 받아주고 있으니 인자로 델타타임을 받도록 해야 한다.
						// ActionInfo의 경우에는 DoubleClick에 대한 처리도 해줘야 하므로 더더욱 그렇다.

	void MouseUpdate(float fTime);

	// NormalKey
	void NormalKeyInput();
	
	void CreateMouse();

public:
	void AddAxisKey(const string& strName, unsigned char cKey,
		float fScale, int iSKey = 0); // AxisInfo정보를 추가하는 함수로서,
	// AxisInfo 정보를 추가하기 위한 키들의 조합 + fScale값
	// BindAxisInfo 정보를 찾아오기 위한 strName 을 인자로 받고 있다.
	void AddActionKey(const string& strName, unsigned char cKey,
		int iSKey = 0); // KeyInfo를 추가하기위한 함수로서,
	// BindActionInfo정보를 찾아오기 위한 strName을 인자로 받고 있다.
	// AxisInfo 와는 다르게 fScale값을 받지 않는다. 그외 InputEvent, bDoubleStart, fDoubleTime과 같은 변수들은
	// 여기서 사용하는 변수가 아니다.

public:
	// NormalKey Function
	
	bool NormalKeyDown(const string& strName);
	bool NormalKeyPress(const string& strName);
	bool NormalKeyUp(const string& strName);


private:
	PBindAxisInfo FindAxis(const string& strName);		// Key를 넘겨주면 BindAxisInfo정보를 찾아오는 함수.
	PBindActionInfo FindAction(const string& strName);	// Key를 넘겨주면 BindActionInfo 정보를 찾아오는 함수.
	PNormalKeyInfo FindNormalKey(const string strName); // NormalKey

private:
	bool DeleteBindAxis(const string& strName, void* pObj,
		const string& strFuncName);
	// BindAxisInfo 정보에 접근하여, 해당 오브젝트와, strFuncName의 이름이 같은 함수를 찾아와서 지우는 함수이다.

	bool DeleteBindAction(const string& strName, void* pObj,
		const string& strFuncName);
	// DeleteBindAxis함수와 역할이 동일하며, 다만, BindActionInfo에 대한 것이라는 점에서만 차이가 난다.


	// BindAxis BindAxisInfo에 함수를 추가해주는 함수이다.
	// 멤버로 가변적인 클래스타입이 들어올 가능성이 있으므로, 템플릿함수로 구성이 되어있다.
	// 필요한 정보는, BindAxisInfo를 가져올 strName 
	// 함수 정보를 추가하기 위한, 멤버, 함수포인터, 함수의 이름
	// 을 입력받으면 된다.
	template <typename T>
	bool BindAxis(const string& strName, T* pObj,
		void(T::*pFunc)(float, float),
		const string & strFuncName)
	{
		PBindAxisInfo pAxis = FindAxis(strName);

		if (!pAxis)
			return false;

		// 제대로 찾아온 경우에는 해당 함수를 추가한다.
		KeyFunction tFunc;

		
		tFunc.strFuncName = strFuncName;
		tFunc.func = bind(pFunc, pObj, placeholders::_1, placeholders::_2);
		tFunc.pObj = pObj;

		pAxis->FunctionList.push_back(tFunc);

		return true;
	}

	// BindAction 함수는 BindAction의 정보에 함수를 추가하는 함수이다
	// 여기서는 BindAxis와는 다르게 여러 정보가 들어가게 되는데,
	// 기본적으로 BindAction을 찾아오기 위한, strName,
	// 함수 호출을 위한 함수포인터와 멤버, 함수의 이름 까지가 실질적인 추가이고
	// 여기서 BindActionInfo에서 추가하지 않았던 정보인 INPUT_EVENT정보를 추가해준다.

	template <typename T>
	bool BindAction(const string& strName, INPUT_EVENT eEvent,
		T* pObj,
		void(T::*pFunc)(float),
		const string& strFuncName )
	{
		// strName 을 이용해서, BindActionInfo를 찾아온다.
		PBindActionInfo pAction = FindAction(strName);

		if (!pAction)
			return false;
		// 해당 BindAction정보를 찾아오지 못했다면 return false;


		// 찾았다면 함수를 추가한다.
		ActionKeyFunction tFunc;

		tFunc.eEvent = eEvent;
		tFunc.func = bind(pFunc, pObj, placeholders::_1);
		tFunc.pObj = pObj;
		tFunc.strFuncName = strFuncName;

		pAction->FunctionList.push_back(tFunc);

		return true;
	}
	

	// 일반키를 추가하는 함수이다.
	// 여러 인자를 넣을 수 있도록 가변인자템플릿을 활용한다.


public:
	template <typename T>
	bool AddNormalKey(const T& data)
	{

		// 넣어준 타입의 인자에 따라 다른 처리를 한다.
		const char* m_Type = typeid(T).name();

		if (strcmp(m_Type, "char") == 0 ||
			strcmp(m_Type, "int") == 0)
		{
			// int나 char형을 추가한 경우.
			m_pNormalKey->vecKey.push_back((DWORD)data);
		}

		else
		{
			// 이름으로 준다.
			m_pNormalKey->strName = data;
			// 해당 키를 map에 추가한다.
			m_mapNormalKey.insert(make_pair(m_pNormalKey->strName, m_pNormalKey));
		}


		return true;
	}


	template <typename T, typename ... Types>
	bool AddNormalKey(const T& data, const Types& ... arg)
	{

		if (!m_pNormalKey)
			m_pNormalKey = new NormalKeyInfo;

		// 넣어준 타입의 인자에 따라 다른 처리를 한다.
		const char* m_Type = typeid(T).name();

		if (strcmp(m_Type, "char") == 0 ||
			strcmp(m_Type, "int") == 0)
		{
			// int나 char형을 추가한 경우.
			m_pNormalKey->vecKey.push_back((DWORD)data);
		}

		else
		{
			// 이름으로 준다.
			m_pNormalKey->strName = data;
			// 해당 키를 map에 추가한다.


			m_mapNormalKey.insert(make_pair(m_pNormalKey->strName, m_pNormalKey));
		}

		AddNormalKey(arg...);

		if (m_pNormalKey)
			m_pNormalKey = nullptr;

		return true;
	}

};


