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
	list<class CLayer*>*		m_pLayerList;			// Scene�� LayerList�� ���������� �����Ѵ�.
	list<class CObj*>			m_ObjList;				// Scene�� �ڽ��� ��� ������Ʈ ����Ʈ, ObjList�� ������ �ִ´�.
	list<class CObj*>			m_WorldObjList;			// Scene�� ���� �߰��� ������Ʈ�� ��Ƶ� ������ WorldObjList�� ������ �ִ´�.
	unordered_map<string, class CObj*>		m_mapPrototype;		// Scene�� �ڽ��� ����� Prototype�� ������ �ֵ��� �Ѵ�.
	class CPlayer*							m_pPlayer;
	string									m_strBgm;

public:
	string GetBgmName() const
	{
		return m_strBgm;
	}

private:
	void AddPrototype(class CObj* pObj);		// ������Ÿ���� �߰��ϴ� �Լ��̴�.
	class CObj* ClonePrototype(const string& strProtoName,
		const string& strName);						// ������Ÿ���� �ҷ��ͼ� �����ϰ�, �ش� �Լ��� Tag�� �ٿ��ְ� ��ȯ�ϴ� ����̴�.

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
