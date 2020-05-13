#include "Stage3Scene.h"
#include "../Object/Player.h"
#include "../Object/Stage.h"
#include "../Object/Grass.h"
#include "../Object/Door.h"
#include "../Sound/SoundManager.h"

CStage3Scene::CStage3Scene()
{
}


CStage3Scene::~CStage3Scene()
{
}

void CStage3Scene::Start()
{
	CMainScene::Start();
}

bool CStage3Scene::Init()
{
	if (!CMainScene::Init())
		return false;

	GET_SINGLE(CSoundManager)->Play("Stage3", CT_BGM);
	m_strBgm = "Stage3";


	CStage* pStage = CObj::CreateObj<CStage>("Stage", "BackGround", this);
	pStage->Load("../Bin/Stage/Stage3.stg");  // Tile
	LoadObjectPos(pStage, "../Bin/Stage/Stage3.txt"); // Other Info

	KirbyInit(pStage);


	CDoor* pDoor = CreateDoor(m_tDoorPos, ST_SCENE4); // '' Door를 가져와서 연결할 장면의 flag를 넣어준다.
	// 첫 장면을 제외하고는 이 함수를 호출해야 한다.
	PrototypeSave();


	return true;
}

void CStage3Scene::Update(float fTime)
{
	CMainScene::Update(fTime);

}

void CStage3Scene::CreatePrototype()
{
	CMainScene::CreatePrototype();

}

void CStage3Scene::EffectPrototype()
{
	CMainScene::EffectPrototype();
}

void CStage3Scene::StageLineCollision(CCollider * pSrc, CCollider * pDest, float fTime)
{
	CMainScene::StageLineCollision(pSrc, pDest, fTime);
}

void CStage3Scene::CreateMonster()
{
	CMainScene::CreateMonster();

}

bool CStage3Scene::SoundInit()
{
	GET_SINGLE(CSoundManager)->LoadSound("Stage3", true, "Music/Stage3.mp3");

	return true;
}

