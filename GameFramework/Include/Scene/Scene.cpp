#include "Scene.h"  // Scene�� �����ϰ�
#include "../Object/Obj.h"  // ���⼭�� Scene�� �����Ѵ�. �ߺ� include�� ������ ���� �� ������,
								// SceneŬ�������� pragma once Ű���带 �������༭ ������ ������ �ʴ´�.
#include "Layer.h"
#include "../Core/SceneManager.h"
#include "../Core/ResourceManager.h"
#include "../Object/Player.h"
#include "../Core/EditManager.h"
#include "../Core/GameManager.h"
#include "../Sound/SoundManager.h"

CScene::CScene()
{
	m_pLayerList = nullptr;
	m_pPlayer = nullptr;
}


CScene::~CScene()
{
	list<CLayer*>::iterator iter;
	list<CLayer*>::iterator iterEnd = m_pLayerList->end();

	for (iter = m_pLayerList->begin(); iter != iterEnd; ++iter)
	{
		(*iter)->DeleteObject(this);
	}

	// Layer�� ��ȸ�ϸ鼭, ������ Scene �Ҽ��� ������Ʈ�� �����ִ� �۾��̴�.
	// ���� �� StartScene �� ��ư���� MainScene���� ���̸� �� �Ǵµ� �̷� ���� ���ִ� ���� �ϰ� �ִ�.


	// �ڽ��� ������ Ÿ�� ��ü�� ��� ������.
	Safe_Release_Map(m_mapPrototype);

	SAFE_RELEASE(m_pPlayer);
	
}

void CScene::AddPrototype(CObj * pObj)
{
	pObj->SetScene(this);
	m_mapPrototype.insert(make_pair(pObj->GetTag(), pObj));
	// �־��� ��ü�� ������Ÿ�Կ� �ְ�, ���۷��� ī���� ����.
	// ���Ŀ� Scene�� ������� �������� release
}

CObj * CScene::ClonePrototype(const string & strProtoName, const string & strName)
{
	// strProtoName�� �̸��� ���� ������Ÿ�� ��ü�� ã�Ƽ� Clone�Լ��� ȣ���ؼ� return�ϴ� �Լ��̴�
	// strName�� Tag�� �������ش�.

	CObj* pFind = FindPrototype(strProtoName);

	if (!pFind)
		return nullptr;

	CObj* pObj = pFind->Clone();

	pObj->SetTag(strName);

	return pObj;
	// ref 1 return
}

CObj * CScene::FindPrototype(const string & strName)
{
	unordered_map<string, CObj*>::iterator iter = m_mapPrototype.find(strName);

	if (iter == m_mapPrototype.end())
		return nullptr;

	return iter->second;
}

bool CScene::DeletePrototype(const string & strName)
{
	unordered_map<string, CObj*>::iterator iter = m_mapPrototype.find(strName);


	if (iter == m_mapPrototype.end())
		return false;

	// ������Ÿ�Կ� �ִ� ��ü�� ������ Ǫ�� �����̴�, release�ѹ� ���ְ� �̰����� ����.
	SAFE_RELEASE(iter->second);
	m_mapPrototype.erase(iter);

	return true;
}

void CScene::SetLayerList(list<class CLayer*>* pLayerList)
{
	// �Ѱ���, List*�� �����Ѵ�.

	m_pLayerList = pLayerList;

	// ��� Layer �� ���Ͽ� �Ҽ� Scene�� �������ش�.

	list<CLayer*>::iterator iter;
	list<CLayer*>::iterator iterEnd = pLayerList->end();

	for (iter = pLayerList->begin(); iter != iterEnd; ++iter)
	{
		(*iter)->SetScene(this);
	}

}

void CScene::AddObject(CObj * pObj)
{
	// �ش� ������Ʈ�� �� ������Ʈ ����Ʈ�� �߰��ϰ�,
	// �ش� ������Ʈ�� �Ҽ����� �ش� ����� �ش�.

	pObj->SetScene(this);

	m_ObjList.push_back(pObj);
	m_WorldObjList.push_back(pObj);

}

void CScene::DeleteObject(CObj * pObj)
{
	list<CObj*>::iterator iter;
	list<CObj*>::iterator iterEnd = m_ObjList.end();

	for (iter = m_ObjList.begin(); iter != iterEnd; ++iter)
	{
		if ((*iter) == pObj)
		{
			m_ObjList.erase(iter);
			return;
		}
	}

	// ObjList���� �����ִ� ������ ���� release�� ������ �ʴ´�.
	// �������� �ʾ����ϱ�.
}

CObj * CScene::FindObject(const string & strTag)
{
	list<CObj*>::iterator iter;
	list<CObj*>::iterator iterEnd = m_ObjList.end();

	for (iter = m_ObjList.begin(); iter != iterEnd; ++iter)
	{
		if ((*iter)->GetTag() == strTag)
		{
			(*iter)->AddRef();
			return *iter;

			// ������ �ش� �±׸� ���� ������Ʈ�� �����Ϸ��� �Ѵ�.
			// add ref

			// ObjList�� ��쿡�� Scene�� ���ؼ� Ư�� ������Ʈ�� ã�ƿ��� �������� ����ϱ� ����
			// ObjList�̰�, WorldObjList�� ������Ʈ �߰��� ���� List�̴�.
			// WorldObj�� ������� ���� ��쿡 �ش� List�� �߰��� ��ü��
			// Layer�� ObjList�� ��ġ���ְ� WorldObjList�� ����ְ� �ִ�.
		}

	}

	return nullptr;
}

CObj * CScene::FindNextObject(const string & strTag)
{
	list<CObj*>::iterator iter;
	list<CObj*>::iterator iterEnd = m_ObjList.end();

	for (iter = m_ObjList.begin(); iter != iterEnd; ++iter)
	{
		if ((*iter)->GetTag() == strTag)
		{
			++iter;

			if (iter == m_ObjList.end())
				return *(m_ObjList.begin());

			return *iter;
		}
	}

	return nullptr;
}

void CScene::SetWorld()
{
	// WorldObjList�� �߰��� ������Ʈ�� Layer�� ��ġ���ִ� �Լ��̴�.

	list<CObj*>::iterator iter;
	list<CObj*>::iterator iterEnd = m_WorldObjList.end();

	for (iter = m_WorldObjList.begin(); iter != iterEnd; ++iter)
	{
		CLayer* pLayer = GET_SINGLE(CSceneManager)->FindLayer((*iter)->GetLayerName());

		if (pLayer)
		{
			(*iter)->Start();

			pLayer->AddObject((*iter));

			// �浹ü�� �ڽ��� SerialNumber�� ������ ��ġ�� �غ� �� ���̸�,
			// �ִϸ��̼��� ������ �Ǿ� �ִٸ�, �⺻ �ִϸ��̼��� ����� �޾Ƽ�, �̰��� ���� ��� ����� �����ϸ�,
			// �ִϸ��̼��� Ŭ���� ������ �Ǿ� �ִ� Texture�� ������Ʈ���� �Ѱ��ִ� ���̹Ƿ�, ���⼭ ����Ƚ���� ���������ش�.
			// �ƹ�ư, �ִϸ��̼��� ������ ���� �⺻ �غ� ���ְ� �ִ�.
		}

		SAFE_RELEASE(pLayer);
	}

	m_WorldObjList.clear();
	// �߰��� ������Ʈ�� �ڽ���  Layer �� �°� ��ġ���ְ�, ����.
}

bool CScene::SoundInit()
{
	return true;
}

void CScene::Start()
{

	SetWorld();

	GET_SINGLE(CResourceManager)->ClearScene();

	// Sound 
	GET_SINGLE(CSoundManager)->ClearScene();
}

bool CScene::Init()
{
	SoundInit();

	return true;
}

void CScene::Update(float fTime)
{
	if (!m_WorldObjList.empty())
		SetWorld();

	list<CLayer*>::iterator iter;
	list<CLayer*>::iterator iterEnd = m_pLayerList->end();

	for (iter = m_pLayerList->begin(); iter != iterEnd;)
	{
		if (!(*iter)->IsActive())
		{
			SAFE_RELEASE((*iter));
			iter = m_pLayerList->erase(iter);
			iterEnd = m_pLayerList->end();
			continue;
		}
		else if (!(*iter)->IsEnable())
		{
			++iter;
			continue;
		}

		(*iter)->Update(fTime);
		++iter;
	}
}

void CScene::LateUpdate(float fTime)
{
	if (!m_WorldObjList.empty())
		SetWorld();

	list<CLayer*>::iterator iter;
	list<CLayer*>::iterator iterEnd = m_pLayerList->end();

	for (iter = m_pLayerList->begin(); iter != iterEnd;)
	{
		if (!(*iter)->IsActive())
		{
			SAFE_RELEASE((*iter));
			iter = m_pLayerList->erase(iter);
			iterEnd = m_pLayerList->end();
			continue;
		}
		else if (!(*iter)->IsEnable())
		{
			++iter;
			continue;
		}

		(*iter)->LateUpdate(fTime);
		++iter;
	}
}

void CScene::Collision(float fTime)
{
	if (!m_WorldObjList.empty())
		SetWorld();

	list<CLayer*>::iterator iter;
	list<CLayer*>::iterator iterEnd = m_pLayerList->end();

	for (iter = m_pLayerList->begin(); iter != iterEnd;)
	{
		if (!(*iter)->IsActive())
		{
			SAFE_RELEASE((*iter));
			iter = m_pLayerList->erase(iter);
			iterEnd = m_pLayerList->end();
			continue;
		}
		else if (!(*iter)->IsEnable())
		{
			++iter;
			continue;
		}

		(*iter)->Collision(fTime);
		++iter;
	}
}

void CScene::PrevRender(HDC hDC, float fTime)
{

	if (!m_WorldObjList.empty())
		SetWorld();

	list<CLayer*>::iterator iter;
	list<CLayer*>::iterator iterEnd = m_pLayerList->end();

	for (iter = m_pLayerList->begin(); iter != iterEnd;)
	{
		if (!(*iter)->IsActive())
		{
			SAFE_RELEASE((*iter));
			iter = m_pLayerList->erase(iter);
			iterEnd = m_pLayerList->end();
			continue;
		}
		else if (!(*iter)->IsEnable())
		{
			++iter;
			continue;
		}

		(*iter)->PrevRender(hDC, fTime);
		++iter;
	}
}

void CScene::Render(HDC hDC, float fTime)
{

	if (!m_WorldObjList.empty())
		SetWorld();

	list<CLayer*>::iterator iter;
	list<CLayer*>::iterator iterEnd = m_pLayerList->end();

	for (iter = m_pLayerList->begin(); iter != iterEnd;)
	{
		if (!(*iter)->IsActive())
		{
			SAFE_RELEASE((*iter));
			iter = m_pLayerList->erase(iter);
			iterEnd = m_pLayerList->end();
			continue;
		}
		else if (!(*iter)->IsEnable())
		{
			++iter;
			continue;
		}

		(*iter)->Render(hDC, fTime);
		++iter;
	}
}

void CScene::PostRender(HDC hDC, float fTime)
{

	if (!m_WorldObjList.empty())
		SetWorld();

	list<CLayer*>::iterator iter;
	list<CLayer*>::iterator iterEnd = m_pLayerList->end();

	for (iter = m_pLayerList->begin(); iter != iterEnd;)
	{
		if (!(*iter)->IsActive())
		{
			SAFE_RELEASE((*iter));
			iter = m_pLayerList->erase(iter);
			iterEnd = m_pLayerList->end();
			continue;
		}
		else if (!(*iter)->IsEnable())
		{
			++iter;
			continue;
		}

		(*iter)->PostRender(hDC, fTime);
		++iter;
	}
}

int CScene::GetObjectCount(const string & strTag)
{
	int iCount = 0;
	list<CObj*>::iterator iter;
	list<CObj*>::iterator iterEnd = m_ObjList.end();

	for (iter = m_ObjList.begin(); iter != iterEnd; ++iter)
	{
		if ((*iter)->GetTag() == strTag)
		{
			++iCount;
		}

	}

	return iCount;
}

void CScene::TimeStop(float fTime)
{
	GET_SINGLE(CGameManager)->TimeStop(fTime);
}
