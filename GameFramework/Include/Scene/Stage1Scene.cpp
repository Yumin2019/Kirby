#include "Stage1Scene.h"
#include "../Core/SceneManager.h"
#include "../Object/Player.h"
#include "../Object/Twizzy.h"
#include "../Object/Burt.h"
#include "../Object/Grizoo.h"
#include "../Object/Stage.h"
#include "../Collision/ColliderRect.h"
#include "../Collision/ColliderLine.h"
#include "../Core/Input.h"
#include "../Animation/Animation.h"
#include "../Object/Effect.h"
#include "../Object/Bullet.h"
#include "../Object/Grass.h"
#include "../UI/AbilityName.h"
#include "../UI/AbilityImage.h"
#include "../UI/HPHud.h"
#include "../Object/Door.h"
#include "../UI/BossHP.h"
#include "../Sound/SoundManager.h"

CStage1Scene::CStage1Scene()
{
}


CStage1Scene::~CStage1Scene()
{
}

void CStage1Scene::Start()
{
	CMainScene::Start();
}

bool CStage1Scene::Init()
{
	if (!CMainScene::Init())
		return false;

	GET_SINGLE(CSoundManager)->Play("Stage1", CT_BGM);
	m_strBgm = "Stage1";


	CObj* pGrass = CObj::CreateObj<CGrass>("Grass1", "Grass", this);
	pGrass->SetStaticSize(555.f, 45.f);
	pGrass->SetPos(805.f, 840.f);
	pGrass->LoadClip("../Bin/Save/AniObject/Stage1Grass1.clip");

	pGrass = CObj::CreateObj<CGrass>("Grass2", "Grass", this);
	pGrass->SetStaticSize(280.f, 47.f);
	pGrass->SetPos(4055.f, 535.f);
	pGrass->LoadClip("../Bin/Save/AniObject/Stage1Grass2.clip");


	pGrass = CObj::CreateObj<CGrass>("Grass3", "Grass", this);
	pGrass->SetStaticSize(531.f, 43.f);
	pGrass->SetPos(6420, 755);
	pGrass->LoadClip("../Bin/Save/AniObject/Stage1Grass3.clip");



	//CStage* pStage = CObj::CreateObj<CStage>("Stage", "BackGround", this);

	//// ForeGround를 설정해준다.
	//pStage->SetForeGroundTexture("ForeGround", TEXT("ForeGround.bmp"));
	//pStage->SetForeGroundTextureColorKey(0, 18, 127);
	//pStage->SetStagePos(0, 0, 7920, 1100);
	////GET_SINGLE(CSceneManager)->SetTargetPivot()

	//// BackGround를 설정해준다.
	//pStage->SetTexture("BackGround", TEXT("BackGround.bmp"));
	//pStage->SetBackGroundPos(15, 85, 3376, 1163);
	//pStage->SetBackGroundSpeed(300.f);

	//pStage->Load("../Bin/Stage/Stage1.stg"); // 타일 읽어오기.

	
	CStage* pStage = CObj::CreateObj<CStage>("Stage", "BackGround", this);
	pStage->Load("../Bin/Stage/Stage1.stg");  // Tile
	LoadObjectPos(pStage, "../Bin/Stage/Stage1.txt"); // Other Info
	pStage->SetBackGroundSpeed(300.f); // 스피드 지정 지정하지 않는다면 0.f ..



	CPlayer* pPlayer = CObj::CreateObj<CPlayer>("Kirby", "Default", this);


	// 현재 Scene이 초기화중이라서, SceneManager에서 Scene이 잡혀있지 않다(FindObject를 이용할 수 없다)
	pStage->SetPlayer(pPlayer);

	pPlayer->SetPos(m_tStartPos);

	m_pPlayer = pPlayer;
	pPlayer->AddRef(); // ++ 씬 삭제시 감소.


	GET_SINGLE(CInput)->AddNormalKey("Spacebar", VK_SPACE);
	GET_SINGLE(CInput)->AddNormalKey("Shift", VK_LSHIFT);


	// UI
	
	CTargetUI* pTarget = CObj::CreateObj<CAbilityName>("AbilityName", "UI", this);
	pTarget->SetPos(10.f, 10.f);

	//pTarget = CObj::CreateObj<CAbilityImage>("AbilityImage", "UI", this);
	//pTarget->SetPos(0.f, 10.f);
	// 너무 별로라서 뺀다...

	pTarget = CObj::CreateObj<CHPHud>("HPHud", "UI", this);
	pTarget->SetPos(10.f + 5.f, 105.f);

	// Boss Border
	CUIImage* pImage = CObj::CreateObj<CUIImage>("BossHPBorder", "UI", this);
	pImage->SetTexture("BossHPBorder", TEXT("UI/BossHPBorder.bmp"));
	pImage->SetColorKey(255, 0, 255);
	pImage->SetPos(RS_WIDTH - 450.f, 10.f);
	pImage->SetEnable(false);

	// Boss HP 
	pTarget = CObj::CreateObj<CBossHP>("BossHP", "UI", this);
	pTarget->SetPos(RS_WIDTH - 450.f + 10.f, 10.f + 45.f);
	pImage->SetEnable(false);



	// 이 문을 통해서 옮겨야할 다음 장면을 연결할 것인데, 이 정보를 여기서 셋팅하는 것으로 하자.
	// 내부적으로 장면을 정리하는 처리와 어떤 장면에 대한 처리를 할 것인지 함수로 관리하고, 어떤 함수를 등록하는지에 따라서 처리가 달라지
	// 는 것이다.

	CreateDoor(Position(m_tDoorPos), ST_SCENE2);


	return true;
}

void CStage1Scene::Update(float fTime)
{

	

	// 여기서, 
	CMainScene::Update(fTime);
}

bool CStage1Scene::SoundInit()
{
	

	// StageScene에서 사용할 사운드를 추가한다. 
	// Effect
	GET_SINGLE(CSoundManager)->LoadSound("AttackStarHit", false, "Effect/AttackStarHit.wav");
	GET_SINGLE(CSoundManager)->LoadSound("Door", false, "Effect/Door.wav");
	GET_SINGLE(CSoundManager)->LoadSound("JumpStarBounce", false, "Effect/JumpStarBounce.wav");
	GET_SINGLE(CSoundManager)->LoadSound("Pause", false, "Effect/Pause.wav");
	GET_SINGLE(CSoundManager)->LoadSound("GoodSleep", false, "Effect/GoodSleep.wav");
	GET_SINGLE(CSoundManager)->LoadSound("Food", false, "Effect/Food.wav");

	// Kirby
	GET_SINGLE(CSoundManager)->LoadSound("AbilityGet", false, "Kirby/AbilityGet.wav");
	GET_SINGLE(CSoundManager)->LoadSound("Breath", false, "Kirby/Breath.wav");
	GET_SINGLE(CSoundManager)->LoadSound("Die", false, "Kirby/Die.wav");
	GET_SINGLE(CSoundManager)->LoadSound("FlyUp", false, "Kirby/FlyUp.wav");
	GET_SINGLE(CSoundManager)->LoadSound("Jump", false, "Kirby/Jump.wav");
	GET_SINGLE(CSoundManager)->LoadSound("KirbyAttack", false, "Kirby/KirbyAttack.mp3");
	GET_SINGLE(CSoundManager)->LoadSound("KirbyHit", false, "Kirby/KirbyHit.wav");
	GET_SINGLE(CSoundManager)->LoadSound("RunStart", false, "Kirby/RunStart.mp3");
	GET_SINGLE(CSoundManager)->LoadSound("Slide", false, "Kirby/Slide.wav");
	GET_SINGLE(CSoundManager)->LoadSound("StarBullet", false, "Kirby/StarBullet.wav");

	// Mike Kirby
	GET_SINGLE(CSoundManager)->LoadSound("Sing1", false, "MikeKirby/Sing1.wav");
	GET_SINGLE(CSoundManager)->LoadSound("Sing2", false, "MikeKirby/Sing2.wav");
	GET_SINGLE(CSoundManager)->LoadSound("Sing3", false, "MikeKirby/Sing3.wav");

	// Monster
	GET_SINGLE(CSoundManager)->LoadSound("MikeAttack", false, "Monster/MikeAttack.wav");
	GET_SINGLE(CSoundManager)->LoadSound("MonsterHit1", false, "Monster/MonsterHit1.wav");
	GET_SINGLE(CSoundManager)->LoadSound("MonsterHit2", false, "Monster/MonsterHit2.wav");
	GET_SINGLE(CSoundManager)->LoadSound("MonsterHit3", false, "Monster/MonsterHit3.wav");

	// Music은 Stage1만 읽는다.
	GET_SINGLE(CSoundManager)->LoadSound("Stage1", true, "Music/Stage1.mp3");

	// Stone Kirby
	GET_SINGLE(CSoundManager)->LoadSound("Stone1", false, "StoneKirby/Stone1.wav");


	// Sword Kirby
	GET_SINGLE(CSoundManager)->LoadSound("Attack1", false, "SwordKirby/Attack1.wav");
	GET_SINGLE(CSoundManager)->LoadSound("Attack3", false, "SwordKirby/Attack3.wav");
	GET_SINGLE(CSoundManager)->LoadSound("DashAttack", false, "SwordKirby/DashAttack.wav");
	GET_SINGLE(CSoundManager)->LoadSound("JumpAttackHit", false, "SwordKirby/JumpAttackHit.wav");
	GET_SINGLE(CSoundManager)->LoadSound("JumpAttack", false, "SwordKirby/JumpAttack.mp3");




	return true;
}

void CStage1Scene::CreatePrototype()
{
	// 스테이지에서 사용할 모든 몬스터를 생성한다.
	CMainScene::CreatePrototype();

}

void CStage1Scene::EffectPrototype()
{
	// 스테이지에서 사용할 모든 이펙트를 생성한다.
	CMainScene::EffectPrototype();
}

void CStage1Scene::StageLineCollision(CCollider * pSrc, CCollider * pDest, float fTime)
{

	CMainScene::StageLineCollision(pSrc, pDest, fTime);
}

void CStage1Scene::CreateMonster()
{
	CMainScene::CreateMonster();

	// MainScene의 부분을 호출하는 순간에 가상함수의 규칙에 따라서, 이 함수가 호출이 될 것인데, 기본적인 처리를
	// MainScene의 CreateMonster함수에서 해주고 있으므로 해당 함수를 호출해주는 것으로 한다.
}
