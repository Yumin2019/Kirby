#pragma once

// Obj추가되면 계속함.

#include "../Ref.h"


class CLayer :
	public CRef
{
	// Layer또한 Ref의 상속을 받는 객체로서, SAFE_RELEASE함수를 적극 활용한다.
	
	friend class CSceneManager;
	// SceneManager에 의해 생성이 되는 객체이다.
	// friend 처리.

private:
	CLayer();
	~CLayer();

private:
	class CScene*		m_pScene; // 자신의 소속 장면을 저장할 변수
	list<class CObj*>	m_ObjList; // 자신이 가지고 있는 ObjList를 의미한다.
	// Layer는 해당 정보를 가지고 Object에 접근하여, 하향식 설계를 하는 것이다.
	int				m_iZOrder;
	// Layer는 우선순위의 개념이 있어서, (결국에는 출력우선순의를 설정하기 위함)
	// 이것을 조절하기 위한 기준값이 필요하다.

public:
	void AddObject(class CObj* pObj); // 해당 오브젝트를 ObjList 에 넣어주는 역할을 하는 함수이다.
	void DeleteObject(class CScene* pScene);
	// 장면을 넣어주면 해당 장면에 속하는 오브젝트들을 날려주는 함수이다.


public:
	void SetScene(class CScene* pScene) // 장면을 설정해준다.
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
	// SceneManager->Scene->Layer->Obj의 하향식 구조설계는. . . 유사한 구조가 반복된다.

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

