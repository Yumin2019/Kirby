#include "Stage2Scene.h"
#include "../Object/Player.h"
#include "../Object/Stage.h"
#include "../Object/Grass.h"
#include "../Object/Door.h"
#include "../Sound/SoundManager.h"

CStage2Scene::CStage2Scene()
{
}


CStage2Scene::~CStage2Scene()
{
}

void CStage2Scene::Start()
{
	CMainScene::Start();
}

bool CStage2Scene::Init()
{
	if (!CMainScene::Init())
		return false;


	GET_SINGLE(CSoundManager)->Play("Stage2", CT_BGM);
	m_strBgm = "Stage2";

	// -3 923  2315 928 5196 924 6800 767



	CObj* pGrass = CObj::CreateObj<CGrass>("Grass1", "Grass", this);
	pGrass->SetStaticSize(555.f, 45.f);
	pGrass->SetPos(-3, 923);
	pGrass->LoadClip("../Bin/Save/AniObject/Stage2Grass1.clip");

	pGrass = CObj::CreateObj<CGrass>("Grass2", "Grass", this);
	pGrass->SetStaticSize(280.f, 47.f);
	pGrass->SetPos(2315, 928);
	pGrass->LoadClip("../Bin/Save/AniObject/Stage2Grass2.clip");


	pGrass = CObj::CreateObj<CGrass>("Grass3", "Grass", this);
	pGrass->SetStaticSize(531.f, 43.f);
	pGrass->SetPos(5196, 924);
	pGrass->LoadClip("../Bin/Save/AniObject/Stage2Grass3.clip");

	pGrass = CObj::CreateObj<CGrass>("Grass4", "Grass", this);
	pGrass->SetStaticSize(531.f, 43.f);
	pGrass->SetPos(6800, 767);
	pGrass->LoadClip("../Bin/Save/AniObject/Stage2Grass4.clip");





	CStage* pStage = CObj::CreateObj<CStage>("Stage", "BackGround", this);
	pStage->Load("../Bin/Stage/Stage2.stg");  // Tile
	LoadObjectPos(pStage, "../Bin/Stage/Stage2.txt"); // Other Info
	pStage->SetBackGroundSpeed(500.f); // 스피드 지정 지정하지 않는다면 0.f ..

	KirbyInit(pStage);


	CDoor* pDoor = CreateDoor(m_tDoorPos, ST_SCENE3); // '' Door를 가져와서 연결할 장면의 flag를 넣어준다.
	// 첫 장면을 제외하고는 이 함수를 호출해야 한다.
	PrototypeSave();
	

	return true;

}

void CStage2Scene::Update(float fTime)
{
	CMainScene::Update(fTime);

}

bool CStage2Scene::SoundInit()
{
	GET_SINGLE(CSoundManager)->LoadSound("Stage2", true, "Music/Stage2.mp3");

	return true;
}

void CStage2Scene::CreatePrototype()
{
	CMainScene::CreatePrototype();

}

void CStage2Scene::EffectPrototype()
{
	CMainScene::EffectPrototype();
}

void CStage2Scene::StageLineCollision(CCollider * pSrc, CCollider * pDest, float fTime)
{
	CMainScene::StageLineCollision(pSrc, pDest, fTime);
}

void CStage2Scene::CreateMonster()
{
	CMainScene::CreateMonster();

}

