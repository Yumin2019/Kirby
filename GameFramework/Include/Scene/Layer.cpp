#include "Layer.h"
#include "../Object/Obj.h"
#include "../Core/EditManager.h"

CLayer::CLayer()
{
}


CLayer::~CLayer()
{
	// Object의 경우에는 이것을 가지고 있는 Layer's ObjList
	// Scene에 있는 자신의 모든 오브젝트를 담은 Scene's ObjList
	// 이렇게 하여 Obj는 ref = 2가 된다.
	// 따라서, Scene에서 한번 Release를 해주고,
	// Layer에서 한번 Release를 한다면, 깔끔하게 처리된다.

	Safe_Release_VecList(m_ObjList);
}

void CLayer::AddObject(CObj * pObj)
{
	m_ObjList.push_back(pObj);
	// 단순히 ObjList에 추가해주는 작업만 해주고 있다.
	// Scene의 WorldObjList의 경우에는 단순히 Layer 에 추가해주는데 사용할  List이고,
	// Scene의 ObjList의 경우에는 특정 오브젝트의 정보가 필요한 순간에 사용하기 위한
	//  List이다. (그러니까 참조횟수는 증가시키지 않고 있다.)

	// 오브젝트가 생성이 되고나서  ref = 1이 된다. 이후에 Scene의 WorldObjList와
	// ObjList에 들어가게 되는데, WorldObjList의 성격상 단순히 Layer 에 추가하기 위한 중개자 역할을 
	// 하고 있어서 따로 참조값을 증가시키지 않는다. (그냥 떼어놓고 생각하면 된다. ref값이랑은 상관없다고)
	// 더불어 ObjList의 경우에도 단순히 오브젝트 검색용도로 모든 오브젝트를 담아놓은 List일 뿐이다.
	// 여기서도 참조횟수를 증가하고 있지 않다. 따라서, 이것도 ref값이랑은 상관이 없는 List가 된다.
	// 그리고 여기서 중요한데,
	// Ref = 1인 상태에서 Layer's ObjList에 들어가게 된다.
	// 아무런 참조횟수 증가 없이.

	// 그러니까. . . Layer 가 소멸되는 순간에 ObjList의 객체를  release함으로서 Object를 날려주기 위해서
	// ObjList에 아무런 참조횟수 증가없이 넣어준 것이다.

	// 혹은, Scene's ObjList, WorldObjList와는 별개로 Layer의 ObjList에 넣어주었기 때문에
	// ref = 1이라고 봐도 맞다.
	// 즉, 직접적으로 오브젝트를 날리는 처리는 Layer 에서 하고 있는 것이며, Scene의 WorldObjList와
	// ObjList의 경우에는 참조값과는 상관없이 사용하는 List이다.
	// Scene이 사라짐과 동시에 아라서 날라가는 List로 생각할 필요가 없다.

	// 아무튼 Layer's ObjList에 추가함으로서 Obj는 참조값이 1이된다.
	// Layer가 날라가는 과정에서 모두 날려버리는 개념.
	// Scene이 날라가면 단순히 자신이 사용하는 Prototype의 객체를 날려주고,
	// 그 이후에 SceneManager가 날라가면, LayerList를 날려주는데
	// 여기에 들어있는 오브젝트가 날라간다. (ref = 1이니까)


}

void CLayer::DeleteObject(CScene * pScene)
{
	list<CObj*>::iterator iter;
	list<CObj*>::iterator iterEnd = m_ObjList.end();

	for (iter = m_ObjList.begin(); iter != iterEnd;)
	{
		if ((*iter)->GetScene() == pScene)
		{
		
			SAFE_RELEASE((*iter));
			iter = m_ObjList.erase(iter);
			iterEnd = m_ObjList.end();
			continue;
		}

		++iter;

	}
}

void CLayer::Start()
{
}

bool CLayer::Init()
{
	return true;
}

void CLayer::Update(float fTime)
{
	// Obj의 상태를 확인하여, 활성화가 된 객체인지 여부와, 죽은 상태인지를 확인하며
	// ObjList를 순회한다.

	list<CObj*>::iterator iter;
	list<CObj*>::iterator iterEnd = m_ObjList.end();

	for (iter = m_ObjList.begin(); iter != iterEnd; )
	{	
		if (!(*iter)->IsActive())
		{
			// 죽어 있는 객체를 Release해주고, 해당 List에서 삭제한다.

			SAFE_RELEASE((*iter));
			iter = m_ObjList.erase(iter);
			iterEnd = m_ObjList.end();
			continue;
		}

		else if (!(*iter)->IsEnable())
		{
			// 살아 있지만, 활성화가 되어 있지 않은 경우에는 처리하지 않고 넘긴다.
			++iter;
			continue;
		}

		// 살아있고, 활성화가 되어 있는 상태인 객체는 처리한다.
		(*iter)->Update(fTime);
		++iter;
	}
}

void CLayer::LateUpdate(float fTime)
{
	list<CObj*>::iterator iter;
	list<CObj*>::iterator iterEnd = m_ObjList.end();

	for (iter = m_ObjList.begin(); iter != iterEnd; )
	{
		if (!(*iter)->IsActive())
		{
			SAFE_RELEASE((*iter));
			iter = m_ObjList.erase(iter);
			iterEnd = m_ObjList.end();
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

void CLayer::Collision(float fTime)
{
	list<CObj*>::iterator iter;
	list<CObj*>::iterator iterEnd = m_ObjList.end();

	for (iter = m_ObjList.begin(); iter != iterEnd; )
	{
		if (!(*iter)->IsActive())
		{
			SAFE_RELEASE((*iter));
			iter = m_ObjList.erase(iter);
			iterEnd = m_ObjList.end();
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

void CLayer::PrevRender(HDC hDC, float fTime)
{
	list<CObj*>::iterator iter;
	list<CObj*>::iterator iterEnd = m_ObjList.end();

	for (iter = m_ObjList.begin(); iter != iterEnd; )
	{
		if (!(*iter)->IsActive())
		{
			SAFE_RELEASE((*iter));
			iter = m_ObjList.erase(iter);
			iterEnd = m_ObjList.end();
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

void CLayer::Render(HDC hDC, float fTime)
{
	list<CObj*>::iterator iter;
	list<CObj*>::iterator iterEnd = m_ObjList.end();

	for (iter = m_ObjList.begin(); iter != iterEnd; )
	{
		if (!(*iter)->IsActive())
		{
			SAFE_RELEASE((*iter));
			iter = m_ObjList.erase(iter);
			iterEnd = m_ObjList.end();
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

void CLayer::PostRender(HDC hDC, float fTime)
{
	list<CObj*>::iterator iter;
	list<CObj*>::iterator iterEnd = m_ObjList.end();

	for (iter = m_ObjList.begin(); iter != iterEnd; )
	{
		if (!(*iter)->IsActive())
		{
			SAFE_RELEASE((*iter));
			iter = m_ObjList.erase(iter);
			iterEnd = m_ObjList.end();
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

void CLayer::EditRender(HDC hDC, float fTime)
{
	list<CObj*>::iterator iter;
	list<CObj*>::iterator iterEnd = m_ObjList.end();

	bool bEnable = GET_SINGLE(CEditManager)->GetOffsetEnable();
	Position tOffset = GET_SINGLE(CEditManager)->GetOffset();

	for (iter = m_ObjList.begin(); iter != iterEnd; )
	{
		if (!(*iter)->IsActive())
		{
			SAFE_RELEASE((*iter));
			iter = m_ObjList.erase(iter);
			iterEnd = m_ObjList.end();
			continue;
		}

		else if (!(*iter)->IsEnable())
		{
			++iter;
			continue;
		}

		(*iter)->EditRender(hDC, fTime, bEnable, tOffset);
		++iter;
	}
}
