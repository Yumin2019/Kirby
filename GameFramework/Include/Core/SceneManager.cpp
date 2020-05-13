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

	// Layer를 생성해주는 과정에서 참조.
	Safe_Release_VecList(m_LayerList);

	// 장면을 모두 날린다.
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

	// BackGround의 개수를 읽어온다.
	int iSize = 0;

	fscanf_s(pFile, "%d\n", &iSize);

	// 개수 만큼 읽는다.

	for (int i = 0; i < iSize; ++i)
	{
		int srcX = 0, srcY = 0, destX = 0, destY = 0;
		int iIndex = 0;

		fscanf_s(pFile, "%d src : (%d, %d) / dest : (%d, %d)\n", &iIndex, &srcX, &srcY,
			&destX, &destY);
		
		m_vecBackGroundPos.push_back(ClipCoord(srcX, srcY, destX, destY));
	}

	// ForeGround를 읽는다.

	fgets(strFileName, 100, pFile); // == Line ==

	iSize = 0;

	fscanf_s(pFile, "%d\n", &iSize);

	// 개수 만큼 읽는다.

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
	// 해당 Layer에서 해당 오브젝트를 찾아 맨 마지막으로 자리를 옮긴다. (출력 우선순위)

	CLayer* pLayer = FindLayer(strLayerName);

	if (pLayer)
	{
		// Kirby를 찾는다. List에서 찾아야 한다.
		list<CObj*>::iterator iter;
		list<CObj*>::iterator iterEnd = pLayer->m_ObjList.end();

		CObj* pObj = nullptr;

		for (iter = pLayer->m_ObjList.begin(); iter != iterEnd; ++iter)
		{
			if ((*iter)->GetTag() == strTag)
			{
				// 커비를 찾아서 받아둔다.
				// 그리고 해당 자리를 지워버린다.
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
	// 이전 장면에 존재하던 오브젝트를 추가하는 함수이다.
	// 이 함수는 이전 장면이 삭제된 이후에 호출이 되어야 한다.
	// 그래야, ObjList, CollisionM에 넣는 의미가 있다.

	list<CObj*>::iterator iter;
	list<CObj*>::iterator iterEnd = m_PrevSceneObject.end();

	for (iter = m_PrevSceneObject.begin(); iter != iterEnd; ++iter)
	{
		GET_SINGLE(CCollisionManager)->AddWorldObjList((*iter));
		(*iter)->AddPrevSceneCollider();

		// 다음 장면이 현재 장면이 된 상태에서 ..
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
	// 기본적인 Layer를 추가한다.
	AddLayer("Default", 2);
	AddLayer("Grass", 1);
	AddLayer("BackGround");
	AddLayer("UI", INT_MAX - 1);
	AddLayer("HUD", INT_MAX);

	m_tPivot.x = 0.5f;
	m_tPivot.y = 0.3f;
	
	// 첫번째 인자는 Layer의 이름을 설정해주는 것이고,
	// 두번째 인자는 해당 Layer의 우선순위를 설정해 주는 것이다.
	// 오름차순으로 정렬이 된다. (출력 우선순위)

	// 처음에 SceneManager Init부분이다.
	// StartScene 생성.

	LoadScreenPos("../Bin/Stage/Screen.txt");


	CreateScene<CStartScene>("StartScene", true);


	// 여기서 오브젝트를 배치하고, 배치된 오브젝트들이 맵에 배치될 준비를 하도록 한다.
	// Start함수를 호출하여, WorldObjList에 추가된 오브젝트들을 소속 Layer에게 배치시켜주고, ResourceManager에 있는 Texture정보를 release한번.

	// 여기서 추가된 오브젝트들에 대한 Start함수가 호출이 된다.
	// 충돌체 시리얼 넘버, 애니메이션을 호출하기 위한 기본 처리(기본 애니메이션 설정 및, 오브젝트 Size설정)
	// 이렇게 시작할 준비를 하고 true가 되어 Init이 처리가 끝난다.

	m_pScene->Start();


	return true;
}

int CSceneManager::Update(float fTime)
{
	m_pScene->Update(fTime);

	return ChangeScene();

	// Update와 LateUpdate함수를 거치면서 다음 장면이 잡혀있는 경우에 장면을 교체하는 작업을 해주고 있으며
	// (그러니까 다음 장면이 잡혀있는지를 확인하여 없으면 none을 있다면 교체후 change값을 반환하는 그런..
	// 확인용 함수이다.)
	// 이렇게 넘겨준 값을 가지고 GameManager 에서 판단하여 장면이 교체가 되었는지 확인하고 교체가
	// 일어난 경우에는 새로운 장면에 대하여 Collision , Render처리가 부자연스럽기 때문에 Logic함수의
	// 처음부터 호출이 될 수 있도록 return을 해주는 작업을 하고 있다.
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
	// 카메라의 업데이트
	// Play, Edit모드의 처리를 다르게 한다.

	switch (GET_SINGLE(CGameManager)->GetGameMode())
	{
		case GM_PLAY:
		{

			if (!m_bSwayCamera)
			{
				// 카메라를 흔들지 않는 경우.
				// Player의 좌표를 토대로 스크롤을 진행한다. 여기서 0,0 ~ size.x, size.y가 넘어가는지를 확인한다.

				// 플레이어의 중심 좌표를 얻어온다.
				Position tCenter = m_pPlayer->GetPos();
				Position tResolution = Position(RS_WIDTH, RS_HEIGHT);

				Position tLeftTop = tCenter - tResolution * m_tPivot;
				Position tRightBottom = tCenter + tResolution * (Position(1.f, 1.f) - m_tPivot);

				// 비율에 맞게 플레이어의 위치가 있도록 카메라가 구성이 되었다.
				// 카메라의 위치가 x,y 각각 0보다 작거나 size보다 큰 경우에는 
				// 비율을 맞추다가는 카메라가 화면밖으로 나가는 상황임을 의미한다.
				// 이 경우에는 예외처리로 카메라가 밖으로 나가지 못하도록 한다.

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

				// 이렇게 설정한 LeftTop을 카메라의 좌표로 준다.
				m_tCameraBegin = tLeftTop;
			

			}
			else
			{
				// 카메라를 흔드는 경우.

				m_fSwayTime += fTime;

				if (m_fSwayTime >= m_fSwayPlayTime)
				{
					m_bSwayCamera = false;
				}



				// 플레이어의 중심 위치를 가져온다.
				Position tCenter = m_pPlayer->GetPos();

				float tTop = tCenter.y - RS_HEIGHT * m_tPivot.y;
				float tBottom = tCenter.y + RS_HEIGHT * (1.f - m_tPivot.y);

				// 카메라를 흔들어준다.
				// sin 그래프를 활용하여 카메라를 흔들어줄 것이다.
				// sin 그래프의 주기를 빠르게 적용하기 위해서 각도에 n을 곱하고,
				// 시간의 중첩에 따라서 처리를 할 것이다.
				// 급격한 변화를 주고 싶다면 시간에 몇 n 만큼을 곱해줄 수도 있겠지.

				m_tCameraBegin.x += m_fSwayScale * sinf(m_fSwayTime * 20.f);
				m_tCameraBegin.y = tTop;
				
				// 이렇게 구성을 한다면, 시간의 값에 따라서, 그래프는 계속 흐르고,
				// 여기에 심한 변화를 주기위해서 10.f를 곱해서 처리했다.
				// sin 그래프의 특성상 1 ~ -1 이 반복되는데 일단 해보도록 하자.


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

		// 카메라 속도 증가 감소
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

		// 초기상태에서는 NONE 카메라 업데이트할 필요가 없다.
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
	// Default Layer에서 List에서 오브젝트들을 검사하면서, 몬스터에 해당하는 오브젝트라면 충돌처리를 해보고,
	// 충돌이라면, 처리를 진행한다.

	// 카메라의 좌표를 가져온다.
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
		// 옵션을 확인한다.
		switch ((*iter)->GetObjectType())
		{
			case OT_NORMAL_MONSTER:
			case OT_BOSS_DDD:
			{
				// 몬스터의 Rect를 구한다.
				Position tSrcPos = (*iter)->GetPos() - (*iter)->GetPivot() * (*iter)->GetStaticSize();
				Position tDestPos = tSrcPos + (*iter)->GetStaticSize();

				if (CMath::CollisionRectToRect(tCameraInfo,
					RectInfo(tSrcPos.x, tSrcPos.y, tDestPos.x, tDestPos.y), tIntersect))
				{
					// 몬스터와 카메라의 위치가 충돌했다는 것이다.
					// 마이크의 공격을 하고 있는 동안에는 HitAnimation을 호출해야 한다. (공통)
					// 그리고, 일반 몬스터의 경우에는 바로 죽도록 처리를 하고, 보스의 경우에는 죽지 않도록 처리를
					// 해줘야 한다.
					// 체력의 개념으로 죽은 경우에만 없애는 처리를 하도록 하거나, 아니면 보스만 따로 처리를 하거나.
					// Boss만 따로 처리하자 귀찮다.

					CMonster* pMonster = (CMonster*)(*iter);

					if (pMonster->GetObjectType() != OT_BOSS_DDD)
					{
						// 보스가 아닌 경우에는 잠시 후에 죽이는 처리를 하도록 한다.
						pMonster->SetDieTime(fSwayTime, true);
					}
					else
					{
						// 보스의 경우에는 데미지 처리를 하도록 한다.
						pMonster->Damage(10);
					}

					// 공통 사항 : 마이크 공격 동안에 HitAnimation이 호출이 되어야 한다.
					// 이 시간 동안에 HitAnimation이 계속 출력되며, 중력의 처리를 막아버렸다가 다시 백업한다.
					pMonster->SetHitAnimation(fSwayTime); 
					
				}

				// 충돌하지 않은 경우에는 그냥 넘긴다.
			}
			break;

		case OT_PLAYER_ATTACK:
		case OT_MONSTER_ATTACK:

			// 공격에 해당하는 친구들이 마이크 공격에 맞는다면, 움직이지 않도록 하고, 같이 사라지도록 한다.
			(*iter)->SetGravity(false);
			(*iter)->SetMoveSpeed(0.f);
			(*iter)->SetDieTime(fSwayTime, true);

			break;
		}
	}

	// 이러면, 카메라의 위치에 있는 몬스터를 잡아서 해당 몬스터가 죽는 처리를 한 것이다. 
	// 처음 한번만 처리하므로 이런 식으로 구현해도 상관없다.

	SAFE_RELEASE(pLayer);
}

void CSceneManager::ObjectSave()
{
	// 이곳에서 이전 장면에서 살아있어야 하는 오브젝트에 대한 Ref 값을 조절한다.
	// 일단, 커비 / UI는 살아 있어야만 한다.
	// 이곳에서는 다음 장면의 AddObject함수를 사용할 수 있어야 하므로, 다음 장면이 생긴 이후에 호출이 되어야 한다.

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
		// 해당 오브젝트가 Stage or Line에 충돌된 상태라면, GravityTime 을 초기화한다.
	}

	SAFE_RELEASE(pLayer);
}

void CSceneManager::AddLayer(const string & strName, int iZOrder)
{
	// 일단, 같은 이름을 가진 Layer가 존재하는지 확인한다.

	list<CLayer*>::iterator iter;
	list<CLayer*>::iterator iterEnd = m_LayerList.end();

	for (iter = m_LayerList.begin(); iter != iterEnd; ++iter)
	{
		if ((*iter)->GetTag() == strName)
			return;
	}

	// 해당 이름을 가진 Layer 가 존재하지 않는다.

	// Layer를 추가해준다.

	CLayer* pLayer = new CLayer;

	if (!pLayer->Init())
	{
		SAFE_RELEASE(pLayer);
		return;
	}

	pLayer->SetTag(strName);
	pLayer->SetZOrder(iZOrder);

	// 여기서 Layer의 경우에는 Layer의 리스트에 들어가서 ref = 1이다.

	m_LayerList.push_back(pLayer);

	if (m_LayerList.size() >= 2)
		m_LayerList.sort(CSceneManager::SortLayer);
}

void CSceneManager::SetLayerZOrder(const string & strName, int iZOrder)
{
	// strName을 가진 Layer를 찾아와서 해당 Layer의 우선순위를 iZorder로 교체하고
	// 다시 sort

	CLayer* pLayer = FindLayer(strName);

	if (!pLayer)
		return;

	// Layer를 제대로 찾아왔다.
	pLayer->SetZOrder(iZOrder);


	if (m_LayerList.size() >= 2)
		m_LayerList.sort(CSceneManager::SortLayer);

	SAFE_RELEASE(pLayer);
	// FindLayer를 통해서 증가시킨 ref값을 감소시킨다.
}

CLayer * CSceneManager::FindLayer(const string & strName)
{

	list<CLayer*>::iterator iter;
	list<CLayer*>::iterator iterEnd = m_LayerList.end();

	for (iter = m_LayerList.begin(); iter != iterEnd; ++iter)
	{
		if ((*iter)->GetTag() == strName)
		{
			// 해당 Layer 에 대한 참조.
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
	// 장면 전환이 일어났는지 확인하고 장면 전환이 일어난 경우에 장면전환을 처리해주는 함수이다.
	// 이전의 장면을 날리고 현재의 장면으로 교체하는 처리가 일어나며 해당 반환값에 따라서
	// LateUpdate혹은 , Update에서 인식하고 이후의 처리를 막는다.
	// 장면전환이 일어나면 중간에 return이 되어서 현재 다시 설정한 장면을 토대로 Logic의 처음부터 처리한다

	if (m_pNextScene)
	{
		GET_SINGLE(CCollisionManager)->ChangeScene(); 
		// CollisionManager가 가지고 있던 WorldObjList를 날려주는 작업(Scene이 변경되니까)
		// 기존에 사용하고 있던 Resource를 순회하면서 release를 한번씩 호출해주고 있다.
		// 만약에 어떤 텍스처가 ref = 2라면, resourceManager1, 오브젝트 1
		// 이런 경우에는 자연스럽게 소멸될 것이다. (더 이상 사용하는 곳이 없기 때문에)
		// 프로토타입의 객체가 남아있다고 하더라도, 프로토타입까지 한번에 날리기 때문에 
		// 아마 리소스 자체가 날라가지 않을까 ..
		
		// 무튼, 필요한 리소스의 경우에는 다시 읽는 작업을 거칠테니 문제는 없다.

		// 기존 장면을 없애고 새로운 장면으로 덮에 씌우는 처리를 한다.
		SAFE_RELEASE(m_pScene);

		GET_SINGLE(CResourceManager)->ClearScene();
		GET_SINGLE(CSoundManager)->ClearScene();


		m_pScene = m_pNextScene;
		m_pNextScene = nullptr;

		PrevObjectAdd();

		// 다음 Scene이 잡혀있다는 소리는 해당 Scene에 대한 초기화가 끝났다는 것을 의미한다.
		// ! 그러면 만약에 이전에 ResourceManager에서 담겨있던 텍스처에 대한 정보가 있는데
		// 이것을 다음 장면에서도 프로토타입을 만들어서 굳혀버린다면 ... ?
		// 새로운 참조값이 생기기 때문에 결국 해당 리소스는 사라지지 않고 계속 재활용되는 것이다.
		// 오좋네 좋네

	


		return SC_CHANGE; // 장면전환이 일어난 경우 sc_change return
	}

	return SC_NONE; // 장면전환 X
}
