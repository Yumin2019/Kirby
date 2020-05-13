#include "Scene.h"  // Scene을 포함하고
#include "../Object/Obj.h"  // 여기서도 Scene을 포함한다. 중복 include라서 문제가 생길 수 있지만,
								// Scene클래스에서 pragma once 키워드를 정의해줘서 문제가 생기지 않는다.
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

	// Layer를 순회하면서, 이전의 Scene 소속인 오브젝트를 날려주는 작업이다.
	// 예를 들어서 StartScene 의 버튼들은 MainScene에서 보이면 안 되는데 이런 것을 없애는 것을 하고 있다.


	// 자신의 프로토 타입 객체를 모두 날린다.
	Safe_Release_Map(m_mapPrototype);

	SAFE_RELEASE(m_pPlayer);
	
}

void CScene::AddPrototype(CObj * pObj)
{
	pObj->SetScene(this);
	m_mapPrototype.insert(make_pair(pObj->GetTag(), pObj));
	// 넣어준 객체를 프로토타입에 넣고, 레퍼런스 카운터 증가.
	// 이후에 Scene이 사라지는 과정에서 release
}

CObj * CScene::ClonePrototype(const string & strProtoName, const string & strName)
{
	// strProtoName의 이름을 가진 프로토타입 객체를 찾아서 Clone함수를 호출해서 return하는 함수이다
	// strName은 Tag로 설정해준다.

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

	// 프로토타입에 있던 객체를 참조를 푸는 과정이니, release한번 해주고 이곳에서 삭제.
	SAFE_RELEASE(iter->second);
	m_mapPrototype.erase(iter);

	return true;
}

void CScene::SetLayerList(list<class CLayer*>* pLayerList)
{
	// 넘겨준, List*를 설정한다.

	m_pLayerList = pLayerList;

	// 모든 Layer 에 대하여 소속 Scene을 설정해준다.

	list<CLayer*>::iterator iter;
	list<CLayer*>::iterator iterEnd = pLayerList->end();

	for (iter = pLayerList->begin(); iter != iterEnd; ++iter)
	{
		(*iter)->SetScene(this);
	}

}

void CScene::AddObject(CObj * pObj)
{
	// 해당 오브젝트를 두 오브젝트 리스트에 추가하고,
	// 해당 오브젝트의 소속으로 해당 장면을 준다.

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

	// ObjList에서 없애주는 과정도 따로 release를 해주지 않는다.
	// 참조하지 않았으니까.
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

			// 누군가 해당 태그를 가진 오브젝트를 참조하려고 한다.
			// add ref

			// ObjList의 경우에는 Scene을 통해서 특정 오브젝트를 찾아오는 과정에서 사용하기 위한
			// ObjList이고, WorldObjList는 오브젝트 추가를 위한 List이다.
			// WorldObj가 비어있지 않은 경우에 해당 List에 추가된 객체를
			// Layer의 ObjList에 배치해주고 WorldObjList는 비워주고 있다.
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
	// WorldObjList에 추가된 오브젝트를 Layer에 배치해주는 함수이다.

	list<CObj*>::iterator iter;
	list<CObj*>::iterator iterEnd = m_WorldObjList.end();

	for (iter = m_WorldObjList.begin(); iter != iterEnd; ++iter)
	{
		CLayer* pLayer = GET_SINGLE(CSceneManager)->FindLayer((*iter)->GetLayerName());

		if (pLayer)
		{
			(*iter)->Start();

			pLayer->AddObject((*iter));

			// 충돌체는 자신의 SerialNumber를 받으며 배치될 준비를 할 것이며,
			// 애니메이션이 적용이 되어 있다면, 기본 애니메이션의 사이즈를 받아서, 이것을 토대로 출력 사이즈를 설정하며,
			// 애니메이션의 클립에 설정이 되어 있던 Texture를 오브젝트에게 넘겨주는 것이므로, 여기서 참조횟수를 증가시켜준다.
			// 아무튼, 애니메이션을 돌리기 위한 기본 준비를 해주고 있다.
		}

		SAFE_RELEASE(pLayer);
	}

	m_WorldObjList.clear();
	// 추가된 오브젝트를 자신의  Layer 에 맞게 배치해주고, 정리.
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
