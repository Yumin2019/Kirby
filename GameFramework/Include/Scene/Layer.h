#pragma once

// Obj�߰��Ǹ� �����.

#include "../Ref.h"


class CLayer :
	public CRef
{
	// Layer���� Ref�� ����� �޴� ��ü�μ�, SAFE_RELEASE�Լ��� ���� Ȱ���Ѵ�.
	
	friend class CSceneManager;
	// SceneManager�� ���� ������ �Ǵ� ��ü�̴�.
	// friend ó��.

private:
	CLayer();
	~CLayer();

private:
	class CScene*		m_pScene; // �ڽ��� �Ҽ� ����� ������ ����
	list<class CObj*>	m_ObjList; // �ڽ��� ������ �ִ� ObjList�� �ǹ��Ѵ�.
	// Layer�� �ش� ������ ������ Object�� �����Ͽ�, ����� ���踦 �ϴ� ���̴�.
	int				m_iZOrder;
	// Layer�� �켱������ ������ �־, (�ᱹ���� ��¿켱���Ǹ� �����ϱ� ����)
	// �̰��� �����ϱ� ���� ���ذ��� �ʿ��ϴ�.

public:
	void AddObject(class CObj* pObj); // �ش� ������Ʈ�� ObjList �� �־��ִ� ������ �ϴ� �Լ��̴�.
	void DeleteObject(class CScene* pScene);
	// ����� �־��ָ� �ش� ��鿡 ���ϴ� ������Ʈ���� �����ִ� �Լ��̴�.


public:
	void SetScene(class CScene* pScene) // ����� �������ش�.
	{
		m_pScene = pScene;
	}

	void SetZOrder(int iZOrder)
	{
		m_iZOrder = iZOrder;
	}

	int GetZOrder() const
	{
		return m_iZOrder;
	}

public:
	// SceneManager->Scene->Layer->Obj�� ����� ���������. . . ������ ������ �ݺ��ȴ�.

	void Start(); 
	bool Init();
	void Update(float fTime);
	void LateUpdate(float fTime);
	void Collision(float fTime);
	void PrevRender(HDC hDC, float fTime);
	void Render(HDC hDC, float fTime);
	void PostRender(HDC hDC, float fTime);
	void EditRender(HDC hDC, float fTime);
	
	
};

