#include "SceneManager.h"
#include "../Scene/Layer.h"
#include "../Core/GameManager.h"
#include "../Core/Input.h"
#include "../Object/Player.h"
#include "../Collision/CollisionManager.h"
#include "../Core/ResourceManager.h"
#include "../Sound/SoundManager.h"

#include "../Scene/MainScene.h"
#include "../Scene/AnimationEditScene.h"
#include "../Scene/StartScene.h"
#include "../Object/Stage.h"
#include "../Math.h"
#include "../Object/Monster.h"

DEFINITION_SINGLE(CSceneManager);

CSceneManager::CSceneManager() :
	m_bCreateScene(false),
	m_pScene(nullptr),
	m_pPlayer(nullptr),
	m_pNextScene(nullptr),
	m_pStage(nullptr),
	m_bLimitCamera(false)
{
	m_fCameraSpeed = 700.f;
	m_fSwayScale = 2.f;
}


CSceneManager::~CSceneManager()
{

	GET_SINGLE(CCollisionManager)->WorldObjListClear();
	SAFE_RELEASE(m_pPlayer); // ref --
	SAFE_RELEASE(m_pStage); 

	// Layer�� �������ִ� �������� ����.
	Safe_Release_VecList(m_LayerList);

	// ����� ��� ������.
	SAFE_RELEASE(m_pScene);
	SAFE_RELEASE(m_pNextScene);
}


void CSceneManager::LoadScreenPos(const char * pFileName)
{
	FILE* pFile = nullptr;

	fopen_s(&pFile, pFileName, "rt"); 

	if (!pFile)
		return;

	m_vecBackGroundPos.clear();
	m_vecForeGroundPos.clear();

	
	char strFileName[100] = {};

	fgets(strFileName, 100, pFile); // == Line ==

	// BackGround�� ������ �о�´�.
	int iSize = 0;

	fscanf_s(pFile, "%d\n", &iSize);

	// ���� ��ŭ �д´�.

	for (int i = 0; i < iSize; ++i)
	{
		int srcX = 0, srcY = 0, destX = 0, destY = 0;
		int iIndex = 0;

		fscanf_s(pFile, "%d src : (%d, %d) / dest : (%d, %d)\n", &iIndex, &srcX, &srcY,
			&destX, &destY);
		
		m_vecBackGroundPos.push_back(ClipCoord(srcX, srcY, destX, destY));
	}

	// ForeGround�� �д´�.

	fgets(strFileName, 100, pFile); // == Line ==

	iSize = 0;

	fscanf_s(pFile, "%d\n", &iSize);

	// ���� ��ŭ �д´�.

	for (int i = 0; i < iSize; ++i)
	{
		int srcX = 0, srcY = 0, destX = 0, destY = 0;
		int iIndex = 0;

		fscanf_s(pFile, "%d src : (%d, %d) / dest : (%d, %d)\n", &iIndex, &srcX, &srcY,
			&destX, &destY);

		m_vecForeGroundPos.push_back(ClipCoord(srcX, srcY, destX, destY));
	}



	fclose(pFile);
}

ClipCoord CSceneManager::GetForeGroundPos(unsigned int iIndex)
{
	if(iIndex >= m_vecForeGroundPos.size())
		return ClipCoord(0, 0, 0, 0);

	return m_vecForeGroundPos[iIndex];
}

ClipCoord CSceneManager::GetBackGroundPos(unsigned int iIndex)
{
	if (iIndex >= m_vecBackGroundPos.size())
		return ClipCoord(0, 0, 0, 0);

	return m_vecBackGroundPos[iIndex];
}

vector<ClipCoord>& CSceneManager::GetForeGroundVec()
{
	return m_vecForeGroundPos;
}

vector<ClipCoord>& CSceneManager::GetBackGroundVec()
{
	return m_vecBackGroundPos;
}

void CSceneManager::LayerUp(const string& strLayerName, const string& strTag)
{
	// �ش� Layer���� �ش� ������Ʈ�� ã�� �� ���������� �ڸ��� �ű��. (��� �켱����)

	CLayer* pLayer = FindLayer(strLayerName);

	if (pLayer)
	{
		// Kirby�� ã�´�. List���� ã�ƾ� �Ѵ�.
		list<CObj*>::iterator iter;
		list<CObj*>::iterator iterEnd = pLayer->m_ObjList.end();

		CObj* pObj = nullptr;

		for (iter = pLayer->m_ObjList.begin(); iter != iterEnd; ++iter)
		{
			if ((*iter)->GetTag() == strTag)
			{
				// Ŀ�� ã�Ƽ� �޾Ƶд�.
				// �׸��� �ش� �ڸ��� ����������.
				pObj = *iter;
				pLayer->m_ObjList.erase(iter);
				break;
			}
		}

		if(pObj)
			pLayer->m_ObjList.push_back(pObj);

	}

	SAFE_RELEASE(pLayer);

}

void CSceneManager::PrevObjectAdd()
{
	// ���� ��鿡 �����ϴ� ������Ʈ�� �߰��ϴ� �Լ��̴�.
	// �� �Լ��� ���� ����� ������ ���Ŀ� ȣ���� �Ǿ�� �Ѵ�.
	// �׷���, ObjList, CollisionM�� �ִ� �ǹ̰� �ִ�.

	list<CObj*>::iterator iter;
	list<CObj*>::iterator iterEnd = m_PrevSceneObject.end();

	for (iter = m_PrevSceneObject.begin(); iter != iterEnd; ++iter)
	{
		GET_SINGLE(CCollisionManager)->AddWorldObjList((*iter));
		(*iter)->AddPrevSceneCollider();

		// ���� ����� ���� ����� �� ���¿��� ..
		m_pScene->m_ObjList.push_back((*iter));

	}
	
	m_PrevSceneObject.clear();
}

void CSceneManager::SetStage()
{
	if (m_pNextScene)
	{
		SAFE_RELEASE(m_pStage);
		m_pStage = (CStage*)m_pNextScene->FindObject("Stage");
	}
}

void CSceneManager::SetPlayer()
{
	m_pPlayer = (CPlayer*)m_pNextScene->FindObject("Kirby");
}

bool CSceneManager::Init()
{
	// �⺻���� Layer�� �߰��Ѵ�.
	AddLayer("Default", 2);
	AddLayer("Grass", 1);
	AddLayer("BackGround");
	AddLayer("UI", INT_MAX - 1);
	AddLayer("HUD", INT_MAX);

	m_tPivot.x = 0.5f;
	m_tPivot.y = 0.3f;
	
	// ù��° ���ڴ� Layer�� �̸��� �������ִ� ���̰�,
	// �ι�° ���ڴ� �ش� Layer�� �켱������ ������ �ִ� ���̴�.
	// ������������ ������ �ȴ�. (��� �켱����)

	// ó���� SceneManager Init�κ��̴�.
	// StartScene ����.

	LoadScreenPos("../Bin/Stage/Screen.txt");


	CreateScene<CStartScene>("StartScene", true);


	// ���⼭ ������Ʈ�� ��ġ�ϰ�, ��ġ�� ������Ʈ���� �ʿ� ��ġ�� �غ� �ϵ��� �Ѵ�.
	// Start�Լ��� ȣ���Ͽ�, WorldObjList�� �߰��� ������Ʈ���� �Ҽ� Layer���� ��ġ�����ְ�, ResourceManager�� �ִ� Texture������ release�ѹ�.

	// ���⼭ �߰��� ������Ʈ�鿡 ���� Start�Լ��� ȣ���� �ȴ�.
	// �浹ü �ø��� �ѹ�, �ִϸ��̼��� ȣ���ϱ� ���� �⺻ ó��(�⺻ �ִϸ��̼� ���� ��, ������Ʈ Size����)
	// �̷��� ������ �غ� �ϰ� true�� �Ǿ� Init�� ó���� ������.

	m_pScene->Start();


	return true;
}

int CSceneManager::Update(float fTime)
{
	m_pScene->Update(fTime);

	return ChangeScene();

	// Update�� LateUpdate�Լ��� ��ġ�鼭 ���� ����� �����ִ� ��쿡 ����� ��ü�ϴ� �۾��� ���ְ� ������
	// (�׷��ϱ� ���� ����� �����ִ����� Ȯ���Ͽ� ������ none�� �ִٸ� ��ü�� change���� ��ȯ�ϴ� �׷�..
	// Ȯ�ο� �Լ��̴�.)
	// �̷��� �Ѱ��� ���� ������ GameManager ���� �Ǵ��Ͽ� ����� ��ü�� �Ǿ����� Ȯ���ϰ� ��ü��
	// �Ͼ ��쿡�� ���ο� ��鿡 ���Ͽ� Collision , Renderó���� ���ڿ������� ������ Logic�Լ���
	// ó������ ȣ���� �� �� �ֵ��� return�� ���ִ� �۾��� �ϰ� �ִ�.
}

int CSceneManager::LateUpdate(float fTime)
{
	m_pScene->LateUpdate(fTime);

	return ChangeScene();
}

int CSceneManager::Collision(float fTime)
{
	m_pScene->Collision(fTime);

	return 0;
}

void CSceneManager::PrevRender(HDC hDC, float fTime)
{
	m_pScene->PrevRender(hDC, fTime);

}

void CSceneManager::Render(HDC hDC, float fTime)
{
	m_pScene->Render(hDC, fTime);

}

void CSceneManager::PostRender(HDC hDC, float fTime)
{
	m_pScene->PostRender(hDC, fTime);

	
}

void CSceneManager::CameraUpdate(float fTime)
{
	// ī�޶��� ������Ʈ
	// Play, Edit����� ó���� �ٸ��� �Ѵ�.

	switch (GET_SINGLE(CGameManager)->GetGameMode())
	{
		case GM_PLAY:
		{

			if (!m_bSwayCamera)
			{
				// ī�޶� ����� �ʴ� ���.
				// Player�� ��ǥ�� ���� ��ũ���� �����Ѵ�. ���⼭ 0,0 ~ size.x, size.y�� �Ѿ������ Ȯ���Ѵ�.

				// �÷��̾��� �߽� ��ǥ�� ���´�.
				Position tCenter = m_pPlayer->GetPos();
				Position tResolution = Position(RS_WIDTH, RS_HEIGHT);

				Position tLeftTop = tCenter - tResolution * m_tPivot;
				Position tRightBottom = tCenter + tResolution * (Position(1.f, 1.f) - m_tPivot);

				// ������ �°� �÷��̾��� ��ġ�� �ֵ��� ī�޶� ������ �Ǿ���.
				// ī�޶��� ��ġ�� x,y ���� 0���� �۰ų� size���� ū ��쿡�� 
				// ������ ���ߴٰ��� ī�޶� ȭ������� ������ ��Ȳ���� �ǹ��Ѵ�.
				// �� ��쿡�� ����ó���� ī�޶� ������ ������ ���ϵ��� �Ѵ�.

				if (tLeftTop.x < 0.f)
				{
					tLeftTop.x = 0.f;
					tRightBottom.x = RS_WIDTH;
				}
				else if (tRightBottom.x > m_tStageSize.x)
				{
					tRightBottom.x = m_tStageSize.x;
					tLeftTop.x = m_tStageSize.x - RS_WIDTH;
				}

				if (tLeftTop.y < 0.f)
				{
					tLeftTop.y = 0.f;
					tRightBottom.y = RS_HEIGHT;
				}
				else if (tRightBottom.y > m_tStageSize.y)
				{
					tRightBottom.y = m_tStageSize.y;
					tLeftTop.y = m_tStageSize.y - RS_HEIGHT;
				}

				// �̷��� ������ LeftTop�� ī�޶��� ��ǥ�� �ش�.
				m_tCameraBegin = tLeftTop;
			

			}
			else
			{
				// ī�޶� ���� ���.

				m_fSwayTime += fTime;

				if (m_fSwayTime >= m_fSwayPlayTime)
				{
					m_bSwayCamera = false;
				}



				// �÷��̾��� �߽� ��ġ�� �����´�.
				Position tCenter = m_pPlayer->GetPos();

				float tTop = tCenter.y - RS_HEIGHT * m_tPivot.y;
				float tBottom = tCenter.y + RS_HEIGHT * (1.f - m_tPivot.y);

				// ī�޶� �����ش�.
				// sin �׷����� Ȱ���Ͽ� ī�޶� ������ ���̴�.
				// sin �׷����� �ֱ⸦ ������ �����ϱ� ���ؼ� ������ n�� ���ϰ�,
				// �ð��� ��ø�� ���� ó���� �� ���̴�.
				// �ް��� ��ȭ�� �ְ� �ʹٸ� �ð��� �� n ��ŭ�� ������ ���� �ְ���.

				m_tCameraBegin.x += m_fSwayScale * sinf(m_fSwayTime * 20.f);
				m_tCameraBegin.y = tTop;
				
				// �̷��� ������ �Ѵٸ�, �ð��� ���� ����, �׷����� ��� �帣��,
				// ���⿡ ���� ��ȭ�� �ֱ����ؼ� 10.f�� ���ؼ� ó���ߴ�.
				// sin �׷����� Ư���� 1 ~ -1 �� �ݺ��Ǵµ� �ϴ� �غ����� ����.


				Position tRightBottom = m_tCameraBegin + Position(RS_WIDTH, RS_HEIGHT);

				if (m_tCameraBegin.x < 0.f)
				{
					m_tCameraBegin.x = 0.f;
					tRightBottom.x = RS_WIDTH;
				}
				else if (tRightBottom.x > m_tStageSize.x)
				{
					tRightBottom.x = m_tStageSize.x;
					m_tCameraBegin.x = m_tStageSize.x - RS_WIDTH;
				}


				if (m_tCameraBegin.y < 0.f)
				{
					m_tCameraBegin.y = 0.f;
					tBottom = RS_HEIGHT;
				}
				else if (tBottom > m_tStageSize.y)
				{
					tBottom = m_tStageSize.y;
					m_tCameraBegin.y = m_tStageSize.y - RS_HEIGHT;
				}

				

			}

		

		}


		break;

		case GM_ANIMATION_EDIT:
		case GM_MAP_EDIT:

			

		// CameraMove
		if (KEYPRESS("A"))
		{
			m_tCameraBegin.x -= fTime * m_fCameraSpeed;
		}
		else if (KEYPRESS("D"))
		{
			m_tCameraBegin.x += fTime * m_fCameraSpeed;

		}

		if (KEYPRESS("W"))
		{
			m_tCameraBegin.y -= fTime * m_fCameraSpeed;

		}
		else if (KEYPRESS("S"))
		{
			m_tCameraBegin.y += fTime * m_fCameraSpeed;

		}

		// ī�޶� �ӵ� ���� ����
		if (KEYDOWN("F1"))
		{
			m_fCameraSpeed += 50.f;
		}

		if (KEYDOWN("F2"))
		{
			m_fCameraSpeed -= 50.f;
		}


		if (m_bLimitCamera)
		{
			Position tRB = m_tCameraBegin + Position(RS_WIDTH, RS_HEIGHT);


			if (m_tCameraBegin.x < 0.f)
				m_tCameraBegin.x = 0.f;

			else if (tRB.x > m_tLimitSize.x)
				m_tCameraBegin.x = m_tLimitSize.x - RS_WIDTH;


			if (m_tCameraBegin.y < 0.f)
				m_tCameraBegin.y = 0.f;

			else if (tRB.y > m_tLimitSize.y)
				m_tCameraBegin.y = m_tLimitSize.y - RS_HEIGHT;

		}

		break;

		// �ʱ���¿����� NONE ī�޶� ������Ʈ�� �ʿ䰡 ����.
	case GM_START:
		break;
	}



}

CObj * CSceneManager::FindObject(const string strName)
{
	if (!m_pScene)
		nullptr;

	return m_pScene->FindObject(strName);
}

CObj * CSceneManager::FindNextObject(const string & strName)
{
	if (!m_pScene)
		nullptr;

	CObj* pObj = m_pScene->FindNextObject(strName);

	pObj->AddRef();

	return pObj;
}

CObj * CSceneManager::FindNextSceneObject(const string & strName)
{
	return m_pNextScene->FindObject(strName);
}

void CSceneManager::MikeAttackCollision()
{
	// Default Layer���� List���� ������Ʈ���� �˻��ϸ鼭, ���Ϳ� �ش��ϴ� ������Ʈ��� �浹ó���� �غ���,
	// �浹�̶��, ó���� �����Ѵ�.

	// ī�޶��� ��ǥ�� �����´�.
	Position tCameraPos = GET_SINGLE(CSceneManager)->GetCameraPos();

	RectInfo tCameraInfo = { tCameraPos.x, tCameraPos.y,
		tCameraPos.x + RS_WIDTH, tCameraPos.y + RS_HEIGHT };

	Position tIntersect;

	float fSwayTime = GET_SINGLE(CSceneManager)->GetLeftSwayTime();

	CLayer* pLayer = FindLayer("Default");

	list<CObj*>::iterator iter;
	list<CObj*>::iterator iterEnd = pLayer->m_ObjList.end();

	for (iter = pLayer->m_ObjList.begin(); iter != iterEnd; ++iter)
	{
		// �ɼ��� Ȯ���Ѵ�.
		switch ((*iter)->GetObjectType())
		{
			case OT_NORMAL_MONSTER:
			case OT_BOSS_DDD:
			{
				// ������ Rect�� ���Ѵ�.
				Position tSrcPos = (*iter)->GetPos() - (*iter)->GetPivot() * (*iter)->GetStaticSize();
				Position tDestPos = tSrcPos + (*iter)->GetStaticSize();

				if (CMath::CollisionRectToRect(tCameraInfo,
					RectInfo(tSrcPos.x, tSrcPos.y, tDestPos.x, tDestPos.y), tIntersect))
				{
					// ���Ϳ� ī�޶��� ��ġ�� �浹�ߴٴ� ���̴�.
					// ����ũ�� ������ �ϰ� �ִ� ���ȿ��� HitAnimation�� ȣ���ؾ� �Ѵ�. (����)
					// �׸���, �Ϲ� ������ ��쿡�� �ٷ� �׵��� ó���� �ϰ�, ������ ��쿡�� ���� �ʵ��� ó����
					// ����� �Ѵ�.
					// ü���� �������� ���� ��쿡�� ���ִ� ó���� �ϵ��� �ϰų�, �ƴϸ� ������ ���� ó���� �ϰų�.
					// Boss�� ���� ó������ ������.

					CMonster* pMonster = (CMonster*)(*iter);

					if (pMonster->GetObjectType() != OT_BOSS_DDD)
					{
						// ������ �ƴ� ��쿡�� ��� �Ŀ� ���̴� ó���� �ϵ��� �Ѵ�.
						pMonster->SetDieTime(fSwayTime, true);
					}
					else
					{
						// ������ ��쿡�� ������ ó���� �ϵ��� �Ѵ�.
						pMonster->Damage(10);
					}

					// ���� ���� : ����ũ ���� ���ȿ� HitAnimation�� ȣ���� �Ǿ�� �Ѵ�.
					// �� �ð� ���ȿ� HitAnimation�� ��� ��µǸ�, �߷��� ó���� ���ƹ��ȴٰ� �ٽ� ����Ѵ�.
					pMonster->SetHitAnimation(fSwayTime); 
					
				}

				// �浹���� ���� ��쿡�� �׳� �ѱ��.
			}
			break;

		case OT_PLAYER_ATTACK:
		case OT_MONSTER_ATTACK:

			// ���ݿ� �ش��ϴ� ģ������ ����ũ ���ݿ� �´´ٸ�, �������� �ʵ��� �ϰ�, ���� ��������� �Ѵ�.
			(*iter)->SetGravity(false);
			(*iter)->SetMoveSpeed(0.f);
			(*iter)->SetDieTime(fSwayTime, true);

			break;
		}
	}

	// �̷���, ī�޶��� ��ġ�� �ִ� ���͸� ��Ƽ� �ش� ���Ͱ� �״� ó���� �� ���̴�. 
	// ó�� �ѹ��� ó���ϹǷ� �̷� ������ �����ص� �������.

	SAFE_RELEASE(pLayer);
}

void CSceneManager::ObjectSave()
{
	// �̰����� ���� ��鿡�� ����־�� �ϴ� ������Ʈ�� ���� Ref ���� �����Ѵ�.
	// �ϴ�, Ŀ�� / UI�� ��� �־�߸� �Ѵ�.
	// �̰������� ���� ����� AddObject�Լ��� ����� �� �־�� �ϹǷ�, ���� ����� ���� ���Ŀ� ȣ���� �Ǿ�� �Ѵ�.

	m_PrevSceneObject.clear();

	CLayer* pLayer = FindLayer("Default");

	list<CObj*>::iterator iter;
	list<CObj*>::iterator iterEnd = pLayer->m_ObjList.end();

	for (iter = pLayer->m_ObjList.begin(); iter != iterEnd; ++iter)
	{
		if ((*iter)->GetTag() == "Kirby")
		{
			(*iter)->SetScene(m_pNextScene);
			m_PrevSceneObject.push_back((*iter));
			break;
		}
	}

	SAFE_RELEASE(pLayer);


	// UI Save.
	pLayer = FindLayer("UI");

	iterEnd = pLayer->m_ObjList.end();

	for (iter = pLayer->m_ObjList.begin(); iter != iterEnd; ++iter)
	{
		(*iter)->SetScene(m_pNextScene);
		m_PrevSceneObject.push_back((*iter));

	}


	SAFE_RELEASE(pLayer);

}

void CSceneManager::GravityTimeClear(const string& strLayerName)
{
	CLayer* pLayer = FindLayer(strLayerName);

	list<CObj*>::iterator iter;
	list<CObj*>::iterator iterEnd = pLayer->m_ObjList.end();

	for (iter = pLayer->m_ObjList.begin(); iter != iterEnd; ++iter)
	{
		if((*iter)->GetStageCollision() || ((*iter)->GetLineCollision()))
			(*iter)->GravityTimeDisEnable();
		// �ش� ������Ʈ�� Stage or Line�� �浹�� ���¶��, GravityTime �� �ʱ�ȭ�Ѵ�.
	}

	SAFE_RELEASE(pLayer);
}

void CSceneManager::AddLayer(const string & strName, int iZOrder)
{
	// �ϴ�, ���� �̸��� ���� Layer�� �����ϴ��� Ȯ���Ѵ�.

	list<CLayer*>::iterator iter;
	list<CLayer*>::iterator iterEnd = m_LayerList.end();

	for (iter = m_LayerList.begin(); iter != iterEnd; ++iter)
	{
		if ((*iter)->GetTag() == strName)
			return;
	}

	// �ش� �̸��� ���� Layer �� �������� �ʴ´�.

	// Layer�� �߰����ش�.

	CLayer* pLayer = new CLayer;

	if (!pLayer->Init())
	{
		SAFE_RELEASE(pLayer);
		return;
	}

	pLayer->SetTag(strName);
	pLayer->SetZOrder(iZOrder);

	// ���⼭ Layer�� ��쿡�� Layer�� ����Ʈ�� ���� ref = 1�̴�.

	m_LayerList.push_back(pLayer);

	if (m_LayerList.size() >= 2)
		m_LayerList.sort(CSceneManager::SortLayer);
}

void CSceneManager::SetLayerZOrder(const string & strName, int iZOrder)
{
	// strName�� ���� Layer�� ã�ƿͼ� �ش� Layer�� �켱������ iZorder�� ��ü�ϰ�
	// �ٽ� sort

	CLayer* pLayer = FindLayer(strName);

	if (!pLayer)
		return;

	// Layer�� ����� ã�ƿԴ�.
	pLayer->SetZOrder(iZOrder);


	if (m_LayerList.size() >= 2)
		m_LayerList.sort(CSceneManager::SortLayer);

	SAFE_RELEASE(pLayer);
	// FindLayer�� ���ؼ� ������Ų ref���� ���ҽ�Ų��.
}

CLayer * CSceneManager::FindLayer(const string & strName)
{

	list<CLayer*>::iterator iter;
	list<CLayer*>::iterator iterEnd = m_LayerList.end();

	for (iter = m_LayerList.begin(); iter != iterEnd; ++iter)
	{
		if ((*iter)->GetTag() == strName)
		{
			// �ش� Layer �� ���� ����.
			(*iter)->AddRef();
			return *iter;
		}
	}

	return nullptr;
}

bool CSceneManager::SortLayer(const CLayer * pL1, const CLayer * pL2)
{
	return pL1->GetZOrder() < pL2->GetZOrder();
}

SCENE_CHANGE CSceneManager::ChangeScene()
{
	// ��� ��ȯ�� �Ͼ���� Ȯ���ϰ� ��� ��ȯ�� �Ͼ ��쿡 �����ȯ�� ó�����ִ� �Լ��̴�.
	// ������ ����� ������ ������ ������� ��ü�ϴ� ó���� �Ͼ�� �ش� ��ȯ���� ����
	// LateUpdateȤ�� , Update���� �ν��ϰ� ������ ó���� ���´�.
	// �����ȯ�� �Ͼ�� �߰��� return�� �Ǿ ���� �ٽ� ������ ����� ���� Logic�� ó������ ó���Ѵ�

	if (m_pNextScene)
	{
		GET_SINGLE(CCollisionManager)->ChangeScene(); 
		// CollisionManager�� ������ �ִ� WorldObjList�� �����ִ� �۾�(Scene�� ����Ǵϱ�)
		// ������ ����ϰ� �ִ� Resource�� ��ȸ�ϸ鼭 release�� �ѹ��� ȣ�����ְ� �ִ�.
		// ���࿡ � �ؽ�ó�� ref = 2���, resourceManager1, ������Ʈ 1
		// �̷� ��쿡�� �ڿ������� �Ҹ�� ���̴�. (�� �̻� ����ϴ� ���� ���� ������)
		// ������Ÿ���� ��ü�� �����ִٰ� �ϴ���, ������Ÿ�Ա��� �ѹ��� ������ ������ 
		// �Ƹ� ���ҽ� ��ü�� ������ ������ ..
		
		// ��ư, �ʿ��� ���ҽ��� ��쿡�� �ٽ� �д� �۾��� ��ĥ�״� ������ ����.

		// ���� ����� ���ְ� ���ο� ������� ���� ����� ó���� �Ѵ�.
		SAFE_RELEASE(m_pScene);

		GET_SINGLE(CResourceManager)->ClearScene();
		GET_SINGLE(CSoundManager)->ClearScene();


		m_pScene = m_pNextScene;
		m_pNextScene = nullptr;

		PrevObjectAdd();

		// ���� Scene�� �����ִٴ� �Ҹ��� �ش� Scene�� ���� �ʱ�ȭ�� �����ٴ� ���� �ǹ��Ѵ�.
		// ! �׷��� ���࿡ ������ ResourceManager���� ����ִ� �ؽ�ó�� ���� ������ �ִµ�
		// �̰��� ���� ��鿡���� ������Ÿ���� ���� ���������ٸ� ... ?
		// ���ο� �������� ����� ������ �ᱹ �ش� ���ҽ��� ������� �ʰ� ��� ��Ȱ��Ǵ� ���̴�.
		// ������ ����

	


		return SC_CHANGE; // �����ȯ�� �Ͼ ��� sc_change return
	}

	return SC_NONE; // �����ȯ X
}
