#include "EndingScene.h"
#include "../Object/Player.h"
#include "../Object/Stage.h"
#include "../Object/Grass.h"
#include "../Object/Door.h"
#include "../Sound/SoundManager.h"

CEndingScene::CEndingScene()
{
	m_fStarTime = 0.f;
	m_fStarLimitTime = 0.5f;
}


CEndingScene::~CEndingScene()
{
}

void CEndingScene::Start()
{
	CMainScene::Start();
}

bool CEndingScene::Init()
{
	if (!CMainScene::Init())
		return false;

	GET_SINGLE(CSoundManager)->Play("Ending", CT_BGM);
	m_strBgm = "Ending";


	CStage* pStage = CObj::CreateObj<CStage>("Stage", "BackGround", this);
	pStage->Load("../Bin/Stage/Ending.stg");  // Tile
	LoadObjectPos(pStage, "../Bin/Stage/Ending.txt"); // Other Info

	KirbyInit(pStage);

	PrototypeSave();

	

	return true;
}

void CEndingScene::Update(float fTime)
{
	CMainScene::Update(fTime);

}

void CEndingScene::Render(HDC hDC, float fTime)
{
	m_fStarTime += fTime;

	if (m_fStarTime >= m_fStarLimitTime)
	{
		m_fStarTime -= m_fStarLimitTime;

		float fSpeed = 100.f * (rand() % 5 + 1);
		CObj::StarEffectCreate(m_pPlayer->GetPos(), 1.f, fSpeed, "BackGround");

	}

	CScene::Render(hDC, fTime);
}

void CEndingScene::CreatePrototype()
{
	CMainScene::CreatePrototype();

}

void CEndingScene::EffectPrototype()
{
	CMainScene::EffectPrototype();
}

void CEndingScene::StageLineCollision(CCollider * pSrc, CCollider * pDest, float fTime)
{
	CMainScene::StageLineCollision(pSrc, pDest, fTime);
}

void CEndingScene::CreateMonster()
{
	CMainScene::CreateMonster();

}

bool CEndingScene::SoundInit()
{
	GET_SINGLE(CSoundManager)->LoadSound("Ending", false, "Music/Ending.mp3");

	return true;
}


