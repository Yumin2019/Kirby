#pragma once

#include "../Ref.h"

// CollisionManager부터 하고 추가함

class CCollider :
	public CRef
{
	friend class CObj;
	friend class CCollisionManager;
	friend class CEditManager;

protected: // 자식에서 생성할 수 있도록.
	CCollider();
	CCollider(const CCollider& collider);
	virtual ~CCollider() = 0;

protected:
	COLLIDER_TYPE	m_eColliderType; // 충돌체의 타입을 알려주기 위한 Tag
	class CObj*		m_pObj; // 소속 오브젝트
	bool			m_bCollision; // 충돌이 났는지 판단하기 위한 bool
	class CScene*	m_pScene; // 충돌체가 소속이 되는 Scene을 의미한다.
	bool			m_bIsUI;	// UI인지 아닌지를 판단하기 위한 변수
	Position		m_tIntersect; // 자신의 충돌 위치를 표현하기 위한 Position 정보이다
	unsigned int	m_iSerialNumber;	// 충돌체마다 고유한 값을 갖고 있기 위한 SerialNumber이다.
	PCollisionProfile m_pProfile; // 자신의 소속 프로파일을 가리키기 위한 정보이다.
	int					m_iChannelIndex; // 자신의 소속 채널을 가리키는 인덱스이다.

	 // A 충돌체와 B충돌체가 있는 경우에 A충돌체에서는 B충돌체의 채널 인덱스를 가지고 와서
	// 자신이 가리키는 프로파일의 정보에서 해당 채널을 확인하여 해당 채널과의 처리를 어떻게 할지 확인하고
	// B 충돌체에서는 A충돌체의 채널인덱스를 가져와서 자신의 프로파일에서 해당 채널의 정보를 확인하고
	// 해당 채널과의 처리를 어떻게 할지 정하는 것이다.

	list<CCollider*>	m_PrevCollisionList; // 이전 충돌체 리스트이다.
	// 만약 해당 List가 없다고 해보자.
	// 만약에 처리하고 있던 충돌체가 갑자기 사라진 경우에 이 충돌체와 관련된 충돌상태를 false로 만들어 주어야
	// 할 것이다. (이전의 충돌 상태를 깨끗하게 만들어야 한다.) 그러면 해당 충돌체가 가지고 있던 
	// SerialNumber를 기준으로 하여 4096개의 bit를 모두 찾아서 상태정보를 reset할 것인지, 아니면
	// 이전 충돌 리스트를 가지고 있어서 해당 충돌체가 사라진 경우에 자신과 충돌했던 다른 충돌체로 부터
	// 자신을 충돌리스트에서 없애달라고 요청을 하고 이런 과정에서 상대의 SerialNumber값을 가지고
	// 인접행렬에서도 이들의 정보를 깨끗하게 날려버리는 처리를 하기 위해서 필요한 것이다.

	// 사라지는 충돌체를 기준으로 4096번 연산을 하는 것보다는 선형탐색을 통해서 자신과 충돌이 된 충돌체들을
	// 찾아보는 것이 훨씬 비용이 적다.

	function<void(CCollider*, CCollider*, float)>		m_Callback[CS_END]; 
	// 이 충돌체가 특정 충돌체와 충돌이 되었다면, 호출해줄 함수이다.
	// 각 상태에 따른 함수를 하나씩 등록할 수 있는 형태이다.
	// 상태마다 여러개를 등록할 수 있도록 되어 있지는 않다.

public:
	void PrevCollisionListClear();

public:
	void SetProfile(const string& strName);
	void SetChannel(const string& strName); // 이름이 strName인 채널을 찾아서 인덱스를 가져온다.

	// 충돌체는 자신의 소속인 Profile을 설정해주어야 하고, 또한 자신의 채널을 설정해둬야 한다.

	PCollisionProfile GetProfile() const
	{
		return m_pProfile;
	}

	int GetChannelIndex() const
	{
		return m_iChannelIndex;
	}

	Position GetIntersect() const
	{
		return m_tIntersect;
	}

public:

	class CObj* GetObj() const
	{
		return m_pObj;
	}

	unsigned int GetSerialNumber() const
	{
		return m_iSerialNumber;
	}



	COLLIDER_TYPE GetColliderType() const
	{
		return m_eColliderType;
	}

	class CScene* GetScene() const
	{
		return m_pScene;
	}

	void SetScene(class CScene* pScene)
	{
		m_pScene = pScene;
	}

	bool IsUI() const
	{
		return m_bIsUI;
	}

	void EnableUI()
	{
		// UI 충돌체 활성화 함수
		m_bIsUI = true;
	}

public:
	virtual void Start();
	virtual bool Init();
	virtual void Update(float fTime);
	virtual void LateUpdate(float fTime);
	virtual void PrevRender(HDC hDC, float fTime);
	virtual void Render(HDC hDC, float fTime);
	virtual void PostRender(HDC hDC, float fTime);
	virtual CCollider* Clone() const;
	virtual bool Collision(CCollider* pCollider) = 0;
	virtual void UpdateObjPos(const Position& tPos);
	// 자신의 객체타입이 무엇인지에 따라서 달라 질 수 있는 부분이다.
	// 물론 부모의 함수에서 한번에 처리해줄 수도 있지만, 아무튼 자식에게 인터페이스를 물려줌으로 써
	// 자신의 타입에 따른 처리를 각각하려고 한다.

protected:
	bool CollisionRectToRect(CCollider* pSrc, CCollider* pDest);
	bool CollisionSphereToSphere(CCollider* pSrc, CCollider* pDest);
	bool CollisionRectToSphere(CCollider* pSrc, CCollider* pDest);
	bool CollisionRectToPoint(CCollider* pSrc, CCollider* pDest);
	bool CollisionSphereToPoint(CCollider* pSrc, CCollider* pDest);
	bool CollisionPointToPoint(CCollider* pSrc, CCollider* pDest);
	bool CollisionRectToLine(CCollider* pSrc, CCollider* pDest);

	// math와 같은 정적 함수를 가진 클래스에 충돌체를 넣어주면 이 정보를 가지고 충돌상태를 판단해주는 시스템이
	// 아니라, 충돌체에서 직접 해당 충돌을 확인하는 함수를 가지고 있다.




public:
	void DisEnable()
	{
		m_bEnable = false;
	}

	void AddPrevCollisionList(class CCollider* pCollider);
	void DeletePrevCollisionList(class CCollider* pCollider);

	void Call(COLLISION_STATE eState, CCollider* pDest, float fTime)
	{
		// this, pDest, fTime을 인자로 받도록 하여 함수포인터를 이용하여 함수를 호출한다.

		if (m_Callback[eState]) // 해당 함수포인터에 주소가 들어가 있는 경우에
			m_Callback[eState](this, pDest, fTime);

		// 없을 때는 호출하지 않는 예외처리가 포함되어 있다.
	}	

	// 전역함수 포인터
	void SetCollisionCallback(COLLISION_STATE eState,
		void(*pFunc)(CCollider*, CCollider*, float))
	{
		m_Callback[eState] = bind(pFunc, placeholders::_1, placeholders::_2, placeholders::_3);
	}

	// 멤버함수 포인터
	template <typename T>
	void SetCollisionCallback(COLLISION_STATE eState,
		void(T::*pFunc)(CCollider*, CCollider*, float), T* pObj)
	{
		m_Callback[eState] = bind(pFunc, pObj, placeholders
			::_1, placeholders::_2, placeholders::_3);
	}
};
