#pragma once 

#include "../Ref.h"

class CScene :
	public CRef
{

	friend class CObj;
	friend class CSceneManager;

public:
	CScene();
	virtual ~CScene() = 0;

protected:
	list<class CLayer*>*		m_pLayerList;			// Scene은 LayerList를 간접적으로 관리한다.
	list<class CObj*>			m_ObjList;				// Scene은 자신의 모든 오브젝트 리스트, ObjList를 가지고 있는다.
	list<class CObj*>			m_WorldObjList;			// Scene은 새로 추가할 오브젝트를 담아둘 공간인 WorldObjList를 가지고 있는다.
	unordered_map<string, class CObj*>		m_mapPrototype;		// Scene은 자신이 사용할 Prototype을 가지고 있도록 한다.
	class CPlayer*							m_pPlayer;
	string									m_strBgm;

public:
	string GetBgmName() const
	{
		return m_strBgm;
	}

private:
	void AddPrototype(class CObj* pObj);		// 프로토타입을 추가하는 함수이다.
	class CObj* ClonePrototype(const string& strProtoName,
		const string& strName);						// 프로토타입을 불러와서 복사하고, 해당 함수에 Tag를 붙여주고 반환하는 기능이다.

	class CObj* FindPrototype(const string& strName);
	bool DeletePrototype(const string& strName);

public:
	void SetLayerList(list<class CLayer*>* pLayerList);
	void AddObject(class CObj* pObj);
	void DeleteObject(class CObj* pObj);
	class CObj* FindObject(const string& strTag);
	class CObj* FindNextObject(const string& strTag);




protected:
	void SetWorld();

public:
	virtual bool SoundInit();
	virtual void Start();
	virtual bool Init();
	virtual void Update(float fTime);
	virtual void LateUpdate(float fTime);
	virtual void Collision(float fTime);
	virtual void PrevRender(HDC hDC, float fTime);
	virtual void Render(HDC hDC, float fTime);
	virtual void PostRender(HDC hDC, float fTime);
	int GetObjectCount(const string& strTag);
	void TimeStop(float fTime);
	


};
