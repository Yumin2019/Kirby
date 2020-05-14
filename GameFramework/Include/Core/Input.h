#pragma once

#include "../Game.h"

// Input ��.

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


	// KeyFunction�� AxisKeyInfo�� ���� �����̴�.
	// ���� fScale���� �޴´�.

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

	// KeyInfo�� ���� ������, �Ϲ�Ű�� Ư��Ű�� �迭, �׸���, ���⼭ ���������� �Ϲ�Ű�� ���� �����̴�.
	// Ư��Ű�� ���� ������, �ش�Ű�� ������ �ϴ����� ���� �������� ������ �ֵ��� �ϰ�,
	// �� Ư��Ű�� ������ �ϴ����� ���� ���θ� ������ ����� �������� Ȯ���ϴ� ���� �Լ����� �̸� ó���� ���ְ� �ִ�.
	// (���� ������ ������ ������ �� �� �ִ�.)
	
}KeyInfo, *PKeyInfo;

typedef struct _tagAxisInfo
{
	KeyInfo	tKey;
	float	fScale;
	// AxisInfo ���������� fScale ���� KeyInfo ������ ������ �ִ�.
	// ��, �Ϲ�Ű�� Ư��Ű�� ���հ� �Է»���, �׸���  Scale���� ������ �ִ� ���̴�.
}AxisInfo, *PAxisInfo;

typedef struct _tagBindAxisInfo
{
	string		strName;
	list<AxisInfo>	KeyList;
	list<KeyFunction>	FunctionList;
	// BindAxisInfo�� �������� �ش� BindAxisInfo�� ��Ÿ���� ���� string 
	// BindAxisInfo �Ҽ��� KeyList�� KeyFunctionList �� �����ϰ� �ִ�.
}BindAxisInfo, *PBindAxisInfo;

typedef struct _tagBindActionInfo
{
	// BindActionInfo ���� Event ������ ����,
	// ����Ŭ���� Ȯ���ϱ� ���� bDoubleStart�� fDoubleTime�� ����.
	// fScale���� �ʿ䰡 �����Ƿ�, AxisKeyInfo�� �ƴ�, KeyInfo List�� ����ϰ� �ִ�.
	// ActionKeyFunction�� KeyFunction�� function��ü������ ���̰� ����.
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

	// ������ �ش�Ű�� ���ȴ��� Ȯ���Ҷ� ����ϴ� �����̴�.
	// ���� Ű�� ���տ� ���� ���� ������ �ƴ�(AxisInfo�� ActionInfo�� �����ϴ� ���������� �ƴ϶�, 
	// Ű �ϳ��ϳ��� ���� ���������� �ǹ��Ѵ�.)
}InputKey, *PInputKey;


// BindAction���� ������� Ű �Է� �ý����̴�.
// �ܼ��� � �������� ���ؼ� ó���� �ϴ� �ý����� �����.

typedef struct _tagNormalKeyInfo
{
	string			strName;
	vector<DWORD>	vecKey;
	bool			bDown;
	bool			bPress;
	bool			bUp;

	// �ܼ��� KeyNormalInfo�� �����ϱ� ���� strName������
	// ����Ű�� ���ÿ� �����ϱ� ���� vector
	// �׸��� �ش� Ű�� �������� Ȯ���ϱ� ���� ��������.

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
	unordered_map<string, PBindAxisInfo>	m_mapBindAxis;		// BindAxisInfo�� ���� ������ ��Ƶ�  map
	unordered_map<string, PBindActionInfo>	m_mapBindAction;	// BindActionInfo �� ���� ������ ��Ƶ� map
																// �̰��� �����Ҵ��Ͽ� ��Ƶ� ���̴�.

	vector<InputKey>	m_KeyList;		// �Ϲ�Ű�� ���� �Է� ���θ� ������ vector
	InputKey			m_tSKey[SKEY_END];	// Ư��Ű�� ���� �Է� ���θ� ������ �迭.


	// NormalKey�� ���� ó��.
	PNormalKeyInfo		m_pNormalKey;
	unordered_map<string, PNormalKeyInfo>	m_mapNormalKey;


	// Mouse : ���콺�� Scene�� �Ҽ����� �δ� ���� �ƴ϶�, ���� ������ ó���ϴ� �ý����̴�.
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
	bool Init(HWND hWnd);		// Init �⺻Ű�� ���� ������ Ư��Ű�� ���� ������ �߰��Ѵ�.
	void Update(float fTime);  // �⺻Ű�� ���� �Է� ó�� + Ư��Ű�� ���� �Է�ó�� 
								// ��� Ű�� �Է�ó���� �Ŀ� �� Ű�� ���� ������ �����Ͽ�, map�� �����ϴ�
								// AxisInfo ������ ActionInfo ������ �����ϴ����� ���� ���� �˻�.
	void Render(HDC hDC, float fTime);

private:
	void SKeyInput();	// Ư��Ű�� ���� �Է� ����.
	void KeyInput();	// �Ϲ�Ű�� ���� �Է� ����
	void AxisInput(float fTime);	// �Է� ������ �������� �� AxisInfo ���� �ν�
	void ActionInput(float fTime);	// �Է� ������ �������� �� ActionInfo ���� �ν�
						// �Է��� Ȯ���ϴ� �������� �ش� �Լ��� ȣ���� ���̴�.
						// �ٵ� �� �Լ��� ���ڷ� fTime������ �޾��ְ� ������ ���ڷ� ��ŸŸ���� �޵��� �ؾ� �Ѵ�.
						// ActionInfo�� ��쿡�� DoubleClick�� ���� ó���� ����� �ϹǷ� ������ �׷���.

	void MouseUpdate(float fTime);

	// NormalKey
	void NormalKeyInput();
	
	void CreateMouse();

public:
	void AddAxisKey(const string& strName, unsigned char cKey,
		float fScale, int iSKey = 0); // AxisInfo������ �߰��ϴ� �Լ��μ�,
	// AxisInfo ������ �߰��ϱ� ���� Ű���� ���� + fScale��
	// BindAxisInfo ������ ã�ƿ��� ���� strName �� ���ڷ� �ް� �ִ�.
	void AddActionKey(const string& strName, unsigned char cKey,
		int iSKey = 0); // KeyInfo�� �߰��ϱ����� �Լ��μ�,
	// BindActionInfo������ ã�ƿ��� ���� strName�� ���ڷ� �ް� �ִ�.
	// AxisInfo �ʹ� �ٸ��� fScale���� ���� �ʴ´�. �׿� InputEvent, bDoubleStart, fDoubleTime�� ���� ��������
	// ���⼭ ����ϴ� ������ �ƴϴ�.

public:
	// NormalKey Function
	
	bool NormalKeyDown(const string& strName);
	bool NormalKeyPress(const string& strName);
	bool NormalKeyUp(const string& strName);


private:
	PBindAxisInfo FindAxis(const string& strName);		// Key�� �Ѱ��ָ� BindAxisInfo������ ã�ƿ��� �Լ�.
	PBindActionInfo FindAction(const string& strName);	// Key�� �Ѱ��ָ� BindActionInfo ������ ã�ƿ��� �Լ�.
	PNormalKeyInfo FindNormalKey(const string strName); // NormalKey

private:
	bool DeleteBindAxis(const string& strName, void* pObj,
		const string& strFuncName);
	// BindAxisInfo ������ �����Ͽ�, �ش� ������Ʈ��, strFuncName�� �̸��� ���� �Լ��� ã�ƿͼ� ����� �Լ��̴�.

	bool DeleteBindAction(const string& strName, void* pObj,
		const string& strFuncName);
	// DeleteBindAxis�Լ��� ������ �����ϸ�, �ٸ�, BindActionInfo�� ���� ���̶�� �������� ���̰� ����.


	// BindAxis BindAxisInfo�� �Լ��� �߰����ִ� �Լ��̴�.
	// ����� �������� Ŭ����Ÿ���� ���� ���ɼ��� �����Ƿ�, ���ø��Լ��� ������ �Ǿ��ִ�.
	// �ʿ��� ������, BindAxisInfo�� ������ strName 
	// �Լ� ������ �߰��ϱ� ����, ���, �Լ�������, �Լ��� �̸�
	// �� �Է¹����� �ȴ�.
	template <typename T>
	bool BindAxis(const string& strName, T* pObj,
		void(T::*pFunc)(float, float),
		const string & strFuncName)
	{
		PBindAxisInfo pAxis = FindAxis(strName);

		if (!pAxis)
			return false;

		// ����� ã�ƿ� ��쿡�� �ش� �Լ��� �߰��Ѵ�.
		KeyFunction tFunc;

		
		tFunc.strFuncName = strFuncName;
		tFunc.func = bind(pFunc, pObj, placeholders::_1, placeholders::_2);
		tFunc.pObj = pObj;

		pAxis->FunctionList.push_back(tFunc);

		return true;
	}

	// BindAction �Լ��� BindAction�� ������ �Լ��� �߰��ϴ� �Լ��̴�
	// ���⼭�� BindAxis�ʹ� �ٸ��� ���� ������ ���� �Ǵµ�,
	// �⺻������ BindAction�� ã�ƿ��� ����, strName,
	// �Լ� ȣ���� ���� �Լ������Ϳ� ���, �Լ��� �̸� ������ �������� �߰��̰�
	// ���⼭ BindActionInfo���� �߰����� �ʾҴ� ������ INPUT_EVENT������ �߰����ش�.

	template <typename T>
	bool BindAction(const string& strName, INPUT_EVENT eEvent,
		T* pObj,
		void(T::*pFunc)(float),
		const string& strFuncName )
	{
		// strName �� �̿��ؼ�, BindActionInfo�� ã�ƿ´�.
		PBindActionInfo pAction = FindAction(strName);

		if (!pAction)
			return false;
		// �ش� BindAction������ ã�ƿ��� ���ߴٸ� return false;


		// ã�Ҵٸ� �Լ��� �߰��Ѵ�.
		ActionKeyFunction tFunc;

		tFunc.eEvent = eEvent;
		tFunc.func = bind(pFunc, pObj, placeholders::_1);
		tFunc.pObj = pObj;
		tFunc.strFuncName = strFuncName;

		pAction->FunctionList.push_back(tFunc);

		return true;
	}
	

	// �Ϲ�Ű�� �߰��ϴ� �Լ��̴�.
	// ���� ���ڸ� ���� �� �ֵ��� �����������ø��� Ȱ���Ѵ�.


public:
	template <typename T>
	bool AddNormalKey(const T& data)
	{

		// �־��� Ÿ���� ���ڿ� ���� �ٸ� ó���� �Ѵ�.
		const char* m_Type = typeid(T).name();

		if (strcmp(m_Type, "char") == 0 ||
			strcmp(m_Type, "int") == 0)
		{
			// int�� char���� �߰��� ���.
			m_pNormalKey->vecKey.push_back((DWORD)data);
		}

		else
		{
			// �̸����� �ش�.
			m_pNormalKey->strName = data;
			// �ش� Ű�� map�� �߰��Ѵ�.
			m_mapNormalKey.insert(make_pair(m_pNormalKey->strName, m_pNormalKey));
		}


		return true;
	}


	template <typename T, typename ... Types>
	bool AddNormalKey(const T& data, const Types& ... arg)
	{

		if (!m_pNormalKey)
			m_pNormalKey = new NormalKeyInfo;

		// �־��� Ÿ���� ���ڿ� ���� �ٸ� ó���� �Ѵ�.
		const char* m_Type = typeid(T).name();

		if (strcmp(m_Type, "char") == 0 ||
			strcmp(m_Type, "int") == 0)
		{
			// int�� char���� �߰��� ���.
			m_pNormalKey->vecKey.push_back((DWORD)data);
		}

		else
		{
			// �̸����� �ش�.
			m_pNormalKey->strName = data;
			// �ش� Ű�� map�� �߰��Ѵ�.


			m_mapNormalKey.insert(make_pair(m_pNormalKey->strName, m_pNormalKey));
		}

		AddNormalKey(arg...);

		if (m_pNormalKey)
			m_pNormalKey = nullptr;

		return true;
	}

};


