#include "Stage4Scene.h"
#include "../Object/Player.h"
#include "../Object/Stage.h"
#include "../Object/Grass.h"
#include "../Object/Door.h"
#include "../Sound/SoundManager.h"

CStage4Scene::CStage4Scene()
{
}


CStage4Scene::~CStage4Scene()
{
}

void CStage4Scene::Start()
{
	CMainScene::Start();
}

bool CStage4Scene::Init()
{
	if (!CMainScene::Init())
		return false;

	GET_SINGLE(CSoundManager)->Play("Stage4", CT_BGM);
	m_strBgm = "Stage4";


	CStage* pStage = CObj::CreateObj<CStage>("Stage", "BackGround", this);
	pStage->Load("../Bin/Stage/Stage4.stg");  // Tile
	LoadObjectPos(pStage, "../Bin/Stage/Stage4.txt"); // Other Info

	KirbyInit(pStage);


	CDoor* pDoor = CreateDoor(m_tDoorPos, ST_SCENE5); // '' Door를 가져와서 연결할 장면의 flag를 넣어준다.
	// 첫 장면을 제외하고는 이 함수를 호출해야 한다.
	PrototypeSave();


	return true;
}

void CStage4Scene::Update(float fTime)
{
	CMainScene::Update(fTime);

}

void CStage4Scene::CreatePrototype()
{
	CMainScene::CreatePrototype();

}

void CStage4Scene::EffectPrototype()
{
	CMainScene::EffectPrototype();
}

void CStage4Scene::StageLineCollision(CCollider * pSrc, CCollider * pDest, float fTime)
{
	CMainScene::StageLineCollision(pSrc, pDest, fTime);
}

void CStage4Scene::CreateMonster()
{
	CMainScene::CreateMonster();

}

bool CStage4Scene::SoundInit()
{
	GET_SINGLE(CSoundManager)->LoadSound("Stage4", true, "Music/Stage4.mp3");

	return true;
}


