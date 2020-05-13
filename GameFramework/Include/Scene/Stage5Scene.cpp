#include "Stage5Scene.h"
#include "../Object/Player.h"
#include "../Object/Stage.h"
#include "../Object/Grass.h"
#include "../Object/Door.h"
#include "../Sound/SoundManager.h"

CStage5Scene::CStage5Scene()
{
}


CStage5Scene::~CStage5Scene()
{
}

void CStage5Scene::Start()
{
	CMainScene::Start();
}

bool CStage5Scene::Init()
{
	if (!CMainScene::Init())
		return false;

	GET_SINGLE(CSoundManager)->Play("Stage5", CT_BGM);
	m_strBgm = "Stage5";


	CStage* pStage = CObj::CreateObj<CStage>("Stage", "BackGround", this);
	pStage->Load("../Bin/Stage/Stage5.stg");  // Tile
	LoadObjectPos(pStage, "../Bin/Stage/Stage5.txt"); // Other Info

	KirbyInit(pStage);


	CDoor* pDoor = CreateDoor(m_tDoorPos, ST_DDDSCENE); // '' Door를 가져와서 연결할 장면의 flag를 넣어준다.
	// 첫 장면을 제외하고는 이 함수를 호출해야 한다.
	PrototypeSave();


	return true;
}

void CStage5Scene::Update(float fTime)
{
	CMainScene::Update(fTime);

}

void CStage5Scene::CreatePrototype()
{
	CMainScene::CreatePrototype();

}

void CStage5Scene::EffectPrototype()
{
	CMainScene::EffectPrototype();
}

void CStage5Scene::StageLineCollision(CCollider * pSrc, CCollider * pDest, float fTime)
{
	CMainScene::StageLineCollision(pSrc, pDest, fTime);
}

void CStage5Scene::CreateMonster()
{
	CMainScene::CreateMonster();

}

bool CStage5Scene::SoundInit()
{
	GET_SINGLE(CSoundManager)->LoadSound("Stage5", true, "Music/Stage5.mp3");

	return true;
}


