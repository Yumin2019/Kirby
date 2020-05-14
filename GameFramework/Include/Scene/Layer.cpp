#include "Layer.h"
#include "../Object/Obj.h"
#include "../Core/EditManager.h"

CLayer::CLayer()
{
}


CLayer::~CLayer()
{
	// Object�� ��쿡�� �̰��� ������ �ִ� Layer's ObjList
	// Scene�� �ִ� �ڽ��� ��� ������Ʈ�� ���� Scene's ObjList
	// �̷��� �Ͽ� Obj�� ref = 2�� �ȴ�.
	// ����, Scene���� �ѹ� Release�� ���ְ�,
	// Layer���� �ѹ� Release�� �Ѵٸ�, ����ϰ� ó���ȴ�.

	Safe_Release_VecList(m_ObjList);
}

void CLayer::AddObject(CObj * pObj)
{
	m_ObjList.push_back(pObj);
	// �ܼ��� ObjList�� �߰����ִ� �۾��� ���ְ� �ִ�.
	// Scene�� WorldObjList�� ��쿡�� �ܼ��� Layer �� �߰����ִµ� �����  List�̰�,
	// Scene�� ObjList�� ��쿡�� Ư�� ������Ʈ�� ������ �ʿ��� ������ ����ϱ� ����
	//  List�̴�. (�׷��ϱ� ����Ƚ���� ������Ű�� �ʰ� �ִ�.)

	// ������Ʈ�� ������ �ǰ���  ref = 1�� �ȴ�. ���Ŀ� Scene�� WorldObjList��
	// ObjList�� ���� �Ǵµ�, WorldObjList�� ���ݻ� �ܼ��� Layer �� �߰��ϱ� ���� �߰��� ������ 
	// �ϰ� �־ ���� �������� ������Ű�� �ʴ´�. (�׳� ������� �����ϸ� �ȴ�. ref���̶��� ������ٰ�)
	// ���Ҿ� ObjList�� ��쿡�� �ܼ��� ������Ʈ �˻��뵵�� ��� ������Ʈ�� ��Ƴ��� List�� ���̴�.
	// ���⼭�� ����Ƚ���� �����ϰ� ���� �ʴ�. ����, �̰͵� ref���̶��� ����� ���� List�� �ȴ�.
	// �׸��� ���⼭ �߿��ѵ�,
	// Ref = 1�� ���¿��� Layer's ObjList�� ���� �ȴ�.
	// �ƹ��� ����Ƚ�� ���� ����.

	// �׷��ϱ�. . . Layer �� �Ҹ�Ǵ� ������ ObjList�� ��ü��  release�����μ� Object�� �����ֱ� ���ؼ�
	// ObjList�� �ƹ��� ����Ƚ�� �������� �־��� ���̴�.

	// Ȥ��, Scene's ObjList, WorldObjList�ʹ� ������ Layer�� ObjList�� �־��־��� ������
	// ref = 1�̶�� ���� �´�.
	// ��, ���������� ������Ʈ�� ������ ó���� Layer ���� �ϰ� �ִ� ���̸�, Scene�� WorldObjList��
	// ObjList�� ��쿡�� ���������� ������� ����ϴ� List�̴�.
	// Scene�� ������� ���ÿ� �ƶ� ���󰡴� List�� ������ �ʿ䰡 ����.

	// �ƹ�ư Layer's ObjList�� �߰������μ� Obj�� �������� 1�̵ȴ�.
	// Layer�� ���󰡴� �������� ��� ���������� ����.
	// Scene�� ���󰡸� �ܼ��� �ڽ��� ����ϴ� Prototype�� ��ü�� �����ְ�,
	// �� ���Ŀ� SceneManager�� ���󰡸�, LayerList�� �����ִµ�
	// ���⿡ ����ִ� ������Ʈ�� ���󰣴�. (ref = 1�̴ϱ�)


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
	// Obj�� ���¸� Ȯ���Ͽ�, Ȱ��ȭ�� �� ��ü���� ���ο�, ���� ���������� Ȯ���ϸ�
	// ObjList�� ��ȸ�Ѵ�.

	list<CObj*>::iterator iter;
	list<CObj*>::iterator iterEnd = m_ObjList.end();

	for (iter = m_ObjList.begin(); iter != iterEnd; )
	{	
		if (!(*iter)->IsActive())
		{
			// �׾� �ִ� ��ü�� Release���ְ�, �ش� List���� �����Ѵ�.

			SAFE_RELEASE((*iter));
			iter = m_ObjList.erase(iter);
			iterEnd = m_ObjList.end();
			continue;
		}

		else if (!(*iter)->IsEnable())
		{
			// ��� ������, Ȱ��ȭ�� �Ǿ� ���� ���� ��쿡�� ó������ �ʰ� �ѱ��.
			++iter;
			continue;
		}

		// ����ְ�, Ȱ��ȭ�� �Ǿ� �ִ� ������ ��ü�� ó���Ѵ�.
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
