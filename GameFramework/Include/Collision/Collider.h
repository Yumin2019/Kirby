#pragma once

#include "../Ref.h"

// CollisionManager���� �ϰ� �߰���

class CCollider :
	public CRef
{
	friend class CObj;
	friend class CCollisionManager;
	friend class CEditManager;

protected: // �ڽĿ��� ������ �� �ֵ���.
	CCollider();
	CCollider(const CCollider& collider);
	virtual ~CCollider() = 0;

protected:
	COLLIDER_TYPE	m_eColliderType; // �浹ü�� Ÿ���� �˷��ֱ� ���� Tag
	class CObj*		m_pObj; // �Ҽ� ������Ʈ
	bool			m_bCollision; // �浹�� ������ �Ǵ��ϱ� ���� bool
	class CScene*	m_pScene; // �浹ü�� �Ҽ��� �Ǵ� Scene�� �ǹ��Ѵ�.
	bool			m_bIsUI;	// UI���� �ƴ����� �Ǵ��ϱ� ���� ����
	Position		m_tIntersect; // �ڽ��� �浹 ��ġ�� ǥ���ϱ� ���� Position �����̴�
	unsigned int	m_iSerialNumber;	// �浹ü���� ������ ���� ���� �ֱ� ���� SerialNumber�̴�.
	PCollisionProfile m_pProfile; // �ڽ��� �Ҽ� ���������� ����Ű�� ���� �����̴�.
	int					m_iChannelIndex; // �ڽ��� �Ҽ� ä���� ����Ű�� �ε����̴�.

	 // A �浹ü�� B�浹ü�� �ִ� ��쿡 A�浹ü������ B�浹ü�� ä�� �ε����� ������ �ͼ�
	// �ڽ��� ����Ű�� ���������� �������� �ش� ä���� Ȯ���Ͽ� �ش� ä�ΰ��� ó���� ��� ���� Ȯ���ϰ�
	// B �浹ü������ A�浹ü�� ä���ε����� �����ͼ� �ڽ��� �������Ͽ��� �ش� ä���� ������ Ȯ���ϰ�
	// �ش� ä�ΰ��� ó���� ��� ���� ���ϴ� ���̴�.

	list<CCollider*>	m_PrevCollisionList; // ���� �浹ü ����Ʈ�̴�.
	// ���� �ش� List�� ���ٰ� �غ���.
	// ���࿡ ó���ϰ� �ִ� �浹ü�� ���ڱ� ����� ��쿡 �� �浹ü�� ���õ� �浹���¸� false�� ����� �־��
	// �� ���̴�. (������ �浹 ���¸� �����ϰ� ������ �Ѵ�.) �׷��� �ش� �浹ü�� ������ �ִ� 
	// SerialNumber�� �������� �Ͽ� 4096���� bit�� ��� ã�Ƽ� ���������� reset�� ������, �ƴϸ�
	// ���� �浹 ����Ʈ�� ������ �־ �ش� �浹ü�� ����� ��쿡 �ڽŰ� �浹�ߴ� �ٸ� �浹ü�� ����
	// �ڽ��� �浹����Ʈ���� ���ִ޶�� ��û�� �ϰ� �̷� �������� ����� SerialNumber���� ������
	// ������Ŀ����� �̵��� ������ �����ϰ� ���������� ó���� �ϱ� ���ؼ� �ʿ��� ���̴�.

	// ������� �浹ü�� �������� 4096�� ������ �ϴ� �ͺ��ٴ� ����Ž���� ���ؼ� �ڽŰ� �浹�� �� �浹ü����
	// ã�ƺ��� ���� �ξ� ����� ����.

	function<void(CCollider*, CCollider*, float)>		m_Callback[CS_END]; 
	// �� �浹ü�� Ư�� �浹ü�� �浹�� �Ǿ��ٸ�, ȣ������ �Լ��̴�.
	// �� ���¿� ���� �Լ��� �ϳ��� ����� �� �ִ� �����̴�.
	// ���¸��� �������� ����� �� �ֵ��� �Ǿ� ������ �ʴ�.

public:
	void PrevCollisionListClear();

public:
	void SetProfile(const string& strName);
	void SetChannel(const string& strName); // �̸��� strName�� ä���� ã�Ƽ� �ε����� �����´�.

	// �浹ü�� �ڽ��� �Ҽ��� Profile�� �������־�� �ϰ�, ���� �ڽ��� ä���� �����ص־� �Ѵ�.

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
		// UI �浹ü Ȱ��ȭ �Լ�
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
	// �ڽ��� ��üŸ���� ���������� ���� �޶� �� �� �ִ� �κ��̴�.
	// ���� �θ��� �Լ����� �ѹ��� ó������ ���� ������, �ƹ�ư �ڽĿ��� �������̽��� ���������� ��
	// �ڽ��� Ÿ�Կ� ���� ó���� �����Ϸ��� �Ѵ�.

protected:
	bool CollisionRectToRect(CCollider* pSrc, CCollider* pDest);
	bool CollisionSphereToSphere(CCollider* pSrc, CCollider* pDest);
	bool CollisionRectToSphere(CCollider* pSrc, CCollider* pDest);
	bool CollisionRectToPoint(CCollider* pSrc, CCollider* pDest);
	bool CollisionSphereToPoint(CCollider* pSrc, CCollider* pDest);
	bool CollisionPointToPoint(CCollider* pSrc, CCollider* pDest);
	bool CollisionRectToLine(CCollider* pSrc, CCollider* pDest);

	// math�� ���� ���� �Լ��� ���� Ŭ������ �浹ü�� �־��ָ� �� ������ ������ �浹���¸� �Ǵ����ִ� �ý�����
	// �ƴ϶�, �浹ü���� ���� �ش� �浹�� Ȯ���ϴ� �Լ��� ������ �ִ�.




public:
	void DisEnable()
	{
		m_bEnable = false;
	}

	void AddPrevCollisionList(class CCollider* pCollider);
	void DeletePrevCollisionList(class CCollider* pCollider);

	void Call(COLLISION_STATE eState, CCollider* pDest, float fTime)
	{
		// this, pDest, fTime�� ���ڷ� �޵��� �Ͽ� �Լ������͸� �̿��Ͽ� �Լ��� ȣ���Ѵ�.

		if (m_Callback[eState]) // �ش� �Լ������Ϳ� �ּҰ� �� �ִ� ��쿡
			m_Callback[eState](this, pDest, fTime);

		// ���� ���� ȣ������ �ʴ� ����ó���� ���ԵǾ� �ִ�.
	}	

	// �����Լ� ������
	void SetCollisionCallback(COLLISION_STATE eState,
		void(*pFunc)(CCollider*, CCollider*, float))
	{
		m_Callback[eState] = bind(pFunc, placeholders::_1, placeholders::_2, placeholders::_3);
	}

	// ����Լ� ������
	template <typename T>
	void SetCollisionCallback(COLLISION_STATE eState,
		void(T::*pFunc)(CCollider*, CCollider*, float), T* pObj)
	{
		m_Callback[eState] = bind(pFunc, pObj, placeholders
			::_1, placeholders::_2, placeholders::_3);
	}
};
