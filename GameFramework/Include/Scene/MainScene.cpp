#include "MainScene.h"
#include "../Core/SceneManager.h"
#include "../Object/Player.h"
#include "../Object/Twizzy.h"
#include "../Object/Burt.h"
#include "../Object/Grizoo.h"
#include "../Object/Stage.h"
#include "../Collision/ColliderRect.h"
#include "../Core/Input.h"
#include "../Collision/ColliderLine.h"
#include "../Object/Player.h"
#include "../Core/Input.h"
#include "../Animation/Animation.h"
#include "../Object/Effect.h"
#include "../Object/Bullet.h"
#include "../Object/Mike.h"
#include "../Object/BladeKnight.h"
#include "../Object/Rocky.h"
#include "../Object/Sleep.h"
#include "../Object/MoveStar.h"
#include "../Object/JumpStar.h"
#include "../Object/Door.h"
#include "../Core/GameManager.h"

#include "../Core/ResourceManager.h"
#include "../Sound/SoundManager.h"
#include "../Core/SceneManager.h"

CMainScene::CMainScene()
{
}


CMainScene::~CMainScene()
{
}

CDoor* CMainScene::CreateDoor(const Position & tPos, SCENE_TYPE eArrivalScene)
{
	CDoor* pDoor = CObj::CreateCloneObject<CDoor>(DOOR, DOOR, "Default", this);
	pDoor->SetSceneType(eArrivalScene); 
	pDoor->SetPos(tPos);

	CEffect* pStar = CObj::CreateCloneObject<CEffect>(STAR, STAR, "Default", this);
	pStar->SetTarget(pDoor);

	return pDoor;
}

void CMainScene::KirbyInit(CStage* pStage)
{
	CScene* pPrevScene = GET_SINGLE(CSceneManager)->GetCurrentScene();
	CPlayer* pPlayer = (CPlayer*)pPrevScene->FindObject("Kirby");
	pPlayer->JumpDisEnable();
	pStage->SetPlayer(pPlayer);
	pPlayer->SetPos(m_tStartPos); // 위에서 읽어온 정보에 포함되어 있다.
	m_pPlayer = pPlayer;
}

void CMainScene::Start()
{
	CScene::Start();
}

bool CMainScene::Init()
{
	CScene::Init();

	GET_SINGLE(CSoundManager)->Stop(CT_BGM);

	CreatePrototype();
	EffectPrototype();
	ObjectPrototype();
	

	return true;
}

void CMainScene::Update(float fTime)
{
	CScene::Update(fTime);

	// 몬스터에 대한 키를 처리한다. (몬스터생성키)
	CreateMonster();

	

	if (KEYDOWN("PlayerPos"))
	{
		CObj* pObj = GET_SINGLE(CSceneManager)->FindObject("Kirby");

		pObj->SetPos(pObj->GetPos().x, 500.f);

		SAFE_RELEASE(pObj);
	}


	/*CObj* pObj = FindObject("Grass4");

	if (KEYPRESS("Shift"))
	{
		if (KEYDOWN("MoveLeft"))
		{
			pObj->SetPos(pObj->GetPos() + Position(-50.f, 0.f));
		}

		if (KEYDOWN("MoveRight"))
		{
			pObj->SetPos(pObj->GetPos() + Position(50.f, 0.f));
		}

		if (KEYDOWN("MoveUp"))
		{
			pObj->SetPos(pObj->GetPos() + Position(0.f, -50.f));
		}

		if (KEYDOWN("MoveDown"))
		{
			pObj->SetPos(pObj->GetPos() + Position(0.f, 50.f));

		}
	}
	else
	{
		if (KEYDOWN("MoveLeft"))
		{
			pObj->SetPos(pObj->GetPos() + Position(-1.f, 0.f));
		}

		if (KEYDOWN("MoveRight"))
		{
			pObj->SetPos(pObj->GetPos() + Position(1.f, 0.f));
		}

		if (KEYDOWN("MoveUp"))
		{
			pObj->SetPos(pObj->GetPos() + Position(0.f, -1.f));
		}

		if (KEYDOWN("MoveDown"))
		{
			pObj->SetPos(pObj->GetPos() + Position(0.f, 1.f));

		}
	}

	if (KEYDOWN("Spacebar"))
	{
		Position tPos = pObj->GetPos();

			int i = 0; 

	}

	SAFE_RELEASE(pObj);*/

}

void CMainScene::PrototypeSave()
{
	GET_SINGLE(CResourceManager)->SavePrototype();
	GET_SINGLE(CSoundManager)->PrototypeSound();

}

void CMainScene::AddLineCollider(CStage* pStage, int iSrcX, int iSrcY, int iDestX, int iDestY)
{
	CColliderLine* pLine = pStage->AddCollider<CColliderLine>("StageLine");

	pLine->SetCollisionCallback<CMainScene>(CS_OVERLAP, &CMainScene::StageLineCollision,
		this);

	pLine->SetPos(iSrcX, iSrcY, iDestX, iDestY);
	pLine->SetProfile("StageProfile");
	pLine->SetChannel("Stage");

	SAFE_RELEASE(pLine);
}

void CMainScene::LoadObjectPos(CStage * pStage, const char * pFileName)
{

	FILE* pFile = nullptr;

	fopen_s(&pFile, pFileName, "rt");

	if (!pFile)
		return;

	int srcX = 0, srcY = 0, destX = 0, destY = 0;

	int iBack = 0, iFore = 0;

	fscanf_s(pFile, "BackGround : %d / ForeGround : %d\n", &iBack, &iFore);

	--iBack;
	--iFore;
	// 얻어낸 인덱스를 토대로 스테이지의 정보를 설정한다.

	ClipCoord tBack = GET_SINGLE(CSceneManager)->GetBackGroundPos(iBack);
	ClipCoord tFore = GET_SINGLE(CSceneManager)->GetForeGroundPos(iFore);

	// ForeGround를 설정해준다.
	pStage->SetForeGroundTexture("ForeGround", TEXT("ForeGround.bmp"));
	pStage->SetForeGroundTextureColorKey(0, 18, 127);
	pStage->SetStagePos(tFore.tStart, tFore.tEnd); 
	// 내부적으로 Stage사이즈가 잡히는 방식이고, 이 크기를 기반으로 하여, 플레이어의 위치를 막아버림.


	// BackGround를 설정해준다. 
	pStage->SetTexture("BackGround", TEXT("BackGround.bmp"));
	pStage->SetBackGroundPos(tBack.tStart, tBack.tEnd);




	// 시작 지점과 문의 위치를 읽어온다.
	fscanf_s(pFile, "Start : (%d, %d) / Door : (%d, %d)\n", &srcX, &srcY,
		&destX, &destY);

	m_tStartPos = Position(srcX, srcY);
	m_tDoorPos = Position(destX, destY);

	char strFileName[100] = {};

	fgets(strFileName, 100, pFile); // == Line ==

	int iSize = 0;
	// 개수를 읽어온다.

	fscanf_s(pFile, "%d\n", &iSize);

	// 개수 만큼 LineCollider를 생성한다.
	for (int i = 0; i < iSize; ++i)
	{
		srcX = 0, srcY = 0, destX = 0, destY = 0;

		fscanf_s(pFile, "src : (%d, %d) / dest : (%d, %d)\n", &srcX, &srcY,
			&destX, &destY);

		AddLineCollider(pStage, srcX, srcY, destX, destY);
	}

	

	// 스포너를 생성한다.

	fgets(strFileName, 100, pFile); // == Spawner ==

	// 스포너의 개수를 읽어온다.
	iSize = 0;
	fscanf_s(pFile, "%d\n", &iSize);


	 //스포너의 개수 만큼 읽어온다.

	for (int i = 0; i < iSize; ++i)
	{
		SPAWNERINFO tInfo;

		int X, Y;
		int iDir = 0;

		int strSize = 0;
		char strLine[100] = {};

		//몬스터의 이름의 개수를 읽는다.
			fscanf_s(pFile, "Length : %d\n", &strSize);

		//이름의 개수만큼 읽는다
			fread(strLine, sizeof(char), strSize, pFile);

			int iCount = 0;

		fscanf_s(pFile, "\nSpawnerPos : (%d, %d) / DirLeft : %d / bCount : %d / iCount : %d\n",
			&X, &Y,
			&iDir, &iCount,
			&tInfo.iCount);
		
		tInfo.bCount = iCount;
		tInfo.strMonsterTag = strLine;
		tInfo.tPos = Position(X, Y);
		tInfo.eSpawnDir = (iDir != 0) ? SD_LEFT : SD_RIGHT;

		// 읽어온 정보를 토대로 스포너의 설정을 한다.

		/*
		pStage->AddSpawner("BurtSpawner", 2050.f, 500.f, SD_LEFT, "Burt");
	*/

		pStage->AddSpawner(tInfo.strMonsterTag + "Spawner", tInfo.tPos.x, tInfo.tPos.y, 
			tInfo.eSpawnDir, tInfo.strMonsterTag, tInfo.bCount, tInfo.iCount);
		
	}
	
	fclose(pFile);

}



void CMainScene::CreatePrototype()
{
	// 몬스터에 대한 프로토타입을 만든다.
	
	// 아이템에 대한 프로토타입을 만든다.

	// 그 외 특수 오브젝트에 대한 프로토타입을 만든다. (이펙트, 별 등)

	// 몬스터의 충돌체를 넣기 편하게 하기 위해서 피봇을 0.5f 0.5f 로 두고 있다.


	// 몬스터
	CMonster* pMonster = CObj::CreatePrototype<CTwizzy>("Twizzy", this);

	pMonster->SetPivot(0.5f, 0.5f);
	pMonster->SetMoveSpeed(100.f);
	pMonster->SetChangeTime(0.5f);
	pMonster->SetGravity(true);
	pMonster->SetLineCollisionEnable(true);
	pMonster->AddBodyCollision(Size(60.f, 60.f), "TwizzyBody", "MonsterProfile", "Monster");
	pMonster->SetHP(1);


/*
	void CObj::AddBodyCollision(Size tStaticSize, const string & strColliderTag,
		const string & strProfile, const string & strChannel)*/

	// Twizzy Fly
	pMonster = CObj::CreatePrototype<CTwizzy>("TwizzyFly", this);

	pMonster->SetMoveSpeed(400.f);
	pMonster->SetPivot(0.5f, 0.5f);
	pMonster->SetChangeTime(0.f);
	((CTwizzy*)pMonster)->SetFlyEnable();
	pMonster->SetGravity(false);
	pMonster->SetTileCollisionEnable(false);
	pMonster->AddBodyCollision(Size(60.f, 60.f), "TwizzyBody", "MonsterProfile", "Monster");
	pMonster->SetHP(1);



	pMonster = CObj::CreatePrototype<CBurt>("Burt", this);

	pMonster->SetPivot(0.5f, 0.5f);
	pMonster->SetChangeTime(0.f);
	pMonster->SetMoveSpeed(300.f);
	pMonster->SetGravity(false);
	pMonster->SetLineCollisionEnable(false);
	pMonster->SetTileCollisionEnable(false);
	pMonster->AddBodyCollision(Size(70.f, 70.f), "BurtBody", "MonsterProfile", "Monster");
	pMonster->SetHP(2);


	pMonster = CObj::CreatePrototype<CGrizoo>("Grizoo", this);

	pMonster->SetPivot(0.5f, 0.5f);
	pMonster->SetChangeTime(4.f);
	pMonster->SetMoveSpeed(200.f);
	pMonster->SetGravity(true);
	pMonster->SetLineCollisionEnable(true);
	pMonster->SetHP(5);

	pMonster->AddBodyCollision(Size(130.f, 120.f), "GrizooBody", "MonsterProfile", "Monster");



	// Ability Monster


	// Mike
	pMonster = CObj::CreatePrototype<CMike>("Mike", this);

	pMonster->SetPivot(0.5f, 0.5f);
	pMonster->SetChangeTime(5.f);
	pMonster->SetMoveSpeed(200.f);
	pMonster->SetGravity(true);
	pMonster->SetLineCollisionEnable(true);

	pMonster->AddBodyCollision(pMonster->GetStaticSize(), "MikeBody", "MonsterProfile", "Monster");
	pMonster->SetMonsterType(MT_MIKE);
	pMonster->SetHP(3);

	// Blade Knight
	pMonster = CObj::CreatePrototype<CBladeKnight>("Blade", this);

	pMonster->SetPivot(0.5f, 0.5f);
	pMonster->SetChangeTime(5.f);
	pMonster->SetMoveSpeed(200.f);
	pMonster->SetGravity(true);
	pMonster->SetLineCollisionEnable(true);

	pMonster->AddBodyCollision(pMonster->GetStaticSize(), "BladeBody", "MonsterProfile", "Monster");
	pMonster->SetMonsterType(MT_SWORD);
	pMonster->SetHP(3);

	// Rocky 
	pMonster = CObj::CreatePrototype<CRocky>("Rocky", this);

	pMonster->SetPivot(0.5f, 0.5f);
	pMonster->SetChangeTime(5.f);
	pMonster->SetMoveSpeed(25.f);
	pMonster->SetGravity(true);
	pMonster->SetLineCollisionEnable(true);

	pMonster->AddBodyCollision(pMonster->GetStaticSize(), "RockyBody", "MonsterProfile", "Monster");
	pMonster->SetMonsterType(MT_STONE);
	pMonster->SetHP(4);


	// Sleep
	pMonster = CObj::CreatePrototype<CSleep>("Sleep", this);

	pMonster->SetPivot(0.5f, 0.5f);
	pMonster->SetChangeTime(3.f);
	pMonster->SetMoveSpeed(100.f);
	pMonster->SetGravity(true);
	pMonster->SetLineCollisionEnable(true);

	pMonster->AddBodyCollision(pMonster->GetStaticSize(), "SleepBody", "MonsterProfile", "Monster");
	pMonster->SetMonsterType(MT_SLEEP);
	pMonster->SetHP(3);


}

void CMainScene::EffectPrototype()
{
	CColliderRect* pCollider = nullptr;

	// Kirby Attack Left Effect
	CEffect* pEffect = CObj::CreatePrototype<CEffect>("KirbyAttackEffectLeft", this);
	pEffect->SetRelative(-270.f, 0.f);
	pEffect->CreateAnimation("Effect");


	CAnimation* pAni = pEffect->GetAnimation();


	pAni->AddClip("KirbyAttackEffectLeft");
	pAni->SetClipTexture("KirbyAttackEffectLeft", "KirbyAttackEffectLeft", TEXT("Effect/KirbyAttackLeft.bmp"));
	pAni->LoadClip("../Bin/Save/Effect/KirbyAttackEffectLeft.clip");

	

	// Attack Right Effect
	pEffect = CObj::CreatePrototype<CEffect>("KirbyAttackEffectRight", this);
	pEffect->SetRelative(270.f, 0.f);

	pEffect->CreateAnimation("Effect");

	pAni = pEffect->GetAnimation();


	pAni->AddClip("KirbyAttackEffectRight");
	pAni->SetClipTexture("KirbyAttackEffectRight", "KirbyAttackEffectRight", TEXT("Effect/KirbyAttackRight.bmp"));
	pAni->LoadClip("../Bin/Save/Effect/KirbyAttackEffectRight.clip");




	// Breath Attack Left

	pEffect = CObj::CreatePrototype<CEffect>("KirbyBreathAttackEffectLeft", this);
	pEffect->SetAngle(PI);
	pEffect->SetPlayTime(1.f);
	pEffect->SetMoveSpeed(700.f);
	pEffect->SetObjectType(OT_PLAYER_ATTACK);
	pEffect->AddBodyCollision(Size(60.f, 60.f), "BreathCol", "PlayerAttackProfile", "PlayerAttack");
	pEffect->SetKnockBackPower(500.f);
	pEffect->SetDamage(2);



	pEffect->CreateAnimation("Effect");

	pAni = pEffect->GetAnimation();


	pAni->AddClip("KirbyBreathAttackEffectLeft");
	pAni->SetClipTexture("KirbyBreathAttackEffectLeft", "KirbyBreathAttackEffectLeft", TEXT("Effect/KirbyBreathAttackLeft.bmp"));
	pAni->LoadClip("../Bin/Save/Effect/KirbyBreathAttackEffectLeft.clip");



	// Breath Attack Right
	pEffect = CObj::CreatePrototype<CEffect>("KirbyBreathAttackEffectRight", this);
	pEffect->SetAngle(0.f);
	pEffect->SetSize(130.f, 130.f);
	pEffect->SetPlayTime(1.f);
	pEffect->SetMoveSpeed(700.f);
	pEffect->SetObjectType(OT_PLAYER_ATTACK);
	pEffect->AddBodyCollision(Size(60.f, 60.f), "BreathCol", "PlayerAttackProfile", "PlayerAttack");
	pEffect->SetKnockBackPower(500.f);
	pEffect->SetDamage(2);



	pEffect->CreateAnimation("Effect");

	pAni = pEffect->GetAnimation();


	pAni->AddClip("KirbyBreathAttackEffectRight");
	pAni->SetClipTexture("KirbyBreathAttackEffectRight", "KirbyBreathAttackEffectRight", TEXT("Effect/KirbyBreathAttackRight.bmp"));
	pAni->LoadClip("../Bin/Save/Effect/KirbyBreathAttackEffectRight.clip");




	// Run Left

	pEffect = CObj::CreatePrototype<CEffect>("KirbyRunEffectLeft", this);
	pEffect->SetRelative(300.f, 0.f);
	pEffect->CreateAnimation("Effect");
	pEffect->SetPlayTime(0.5f);



	pAni = pEffect->GetAnimation();


	pAni->AddClip("KirbyRunEffectLeft");
	pAni->SetClipTexture("KirbyRunEffectLeft", "KirbyRunEffectLeft", TEXT("Effect/KirbyRunLeft.bmp"));
	pAni->LoadClip("../Bin/Save/Effect/KirbyRunEffectLeft.clip");


	// Run Right
	pEffect = CObj::CreatePrototype<CEffect>("KirbyRunEffectRight", this);
	pEffect->SetRelative(0.f, 0.f);
	pEffect->SetPlayTime(0.5f);

	pEffect->CreateAnimation("Effect");

	pAni = pEffect->GetAnimation();


	pAni->AddClip("KirbyRunEffectRight");
	pAni->SetClipTexture("KirbyRunEffectRight", "KirbyRunEffectRight", TEXT("Effect/KirbyRunRight.bmp"));
	pAni->LoadClip("../Bin/Save/Effect/KirbyRunEffectRight.clip");




	// Vomit Left

	pEffect = CObj::CreatePrototype<CEffect>("KirbyStarEffectLeft", this);
	pEffect->SetAngle(PI);
	pEffect->SetPlayTime(10.f);
	pEffect->SetMoveSpeed(1000.f);
	pEffect->AddBodyCollision(Size(60.f, 60.f), "Star", "PlayerAttackProfile", "PlayerAttack");
	pEffect->SetObjectType(OT_PLAYER_ATTACK);
	pEffect->SetKnockBackPower(500.f);
	pEffect->DeplexCollisionEnable();
	pEffect->SetDamage(5);


	pEffect->CreateAnimation("Effect");

	pAni = pEffect->GetAnimation();


	pAni->AddClip("KirbyStarEffectLeft");
	pAni->SetClipTexture("KirbyStarEffectLeft", "KirbyStarEffectLeft", TEXT("Effect/KirbyVomitLeft.bmp"));
	pAni->LoadClip("../Bin/Save/Effect/KirbyStarEffectLeft.clip");


	// Vomit Right
	pEffect = CObj::CreatePrototype<CEffect>("KirbyStarEffectRight", this);
	pEffect->SetAngle(0.f);
	pEffect->SetPlayTime(10.f);
	pEffect->SetMoveSpeed(1000.f);
	pEffect->SetObjectType(OT_PLAYER_ATTACK);
	pEffect->AddBodyCollision(Size(60.f, 60.f), "Star", "PlayerAttackProfile", "PlayerAttack");
	pEffect->SetKnockBackPower(500.f);
	pEffect->DeplexCollisionEnable();
	pEffect->SetDamage(5);



	pEffect->CreateAnimation("Effect");

	pAni = pEffect->GetAnimation();


	pAni->AddClip("KirbyStarEffectRight");
	pAni->SetClipTexture("KirbyStarEffectRight", "KirbyStarEffectRight", TEXT("Effect/KirbyVomitRight.bmp"));
	pAni->LoadClip("../Bin/Save/Effect/KirbyStarEffectRight.clip");



	// 상태 설정 나중에 DisEnable
	// 몬스터 죽는 기능에서 나중에 추가할 것임.
	// Blue Ball

	pEffect = CObj::CreatePrototype<CEffect>("BlueBall", this);
	pEffect->SetRelative(0.f, 0.f);
	pEffect->SetPlayTime(10.f);
	pEffect->SetStaticSize(100.f, 100.f);

	pEffect->CreateAnimation("Effect");

	pAni = pEffect->GetAnimation();


	pAni->AddClip("BlueBall");
	pAni->SetClipTexture("BlueBall", "BlueBall", TEXT("Effect/BlueBallEffect.bmp"));
	pAni->LoadClip("../Bin/Save/Effect/BlueBall.clip");

	// Small Star

	pEffect = CObj::CreatePrototype<CEffect>("SmallStar", this);
	pEffect->SetRelative(-300.f, 0.f);
	pEffect->SetPlayTime(0.5f);
	pEffect->SetMoveSpeed(50.f);
	pEffect->SetTileCollisionEnable(false);

	pEffect->CreateAnimation("Effect");

	pAni = pEffect->GetAnimation();


	pAni->AddClip("SmallStar");
	pAni->SetClipTexture("SmallStar", "SmallStar", TEXT("Effect/SmallStarEffect.bmp"));
	pAni->LoadClip("../Bin/Save/Effect/SmallStar.clip");

	// PinkEffect
	pEffect = CObj::CreatePrototype<CEffect>("PinkStoneEffect", this);
	pEffect->SetRelative(-100.f, -100.f);
	pEffect->CreateAnimation("Effect");
	pEffect->SetTileCollisionEnable(false);

	pAni = pEffect->GetAnimation();


	pAni->AddClip("PinkStoneEffect");
	pAni->SetClipTexture("PinkStoneEffect", "StoneKirbyLeft", TEXT("StoneKirbyLeft.bmp"));
	pAni->LoadClip("../Bin/Save/Effect/PinkStoneEffect.clip");



	// Sword Bullet Left

	CBullet* pBullet = CObj::CreatePrototype<CBullet>("SwordBulletLeft", this);
	pBullet->SetAngle(PI);
	pBullet->SetPlayTime(10.f);
	pBullet->SetMoveSpeed(100.f);
	pBullet->AddBodyCollision(Size(80.f, 40.f), "SwordBullet", "PlayerAttackProfile", "PlayerAttack");
	pBullet->SetObjectType(OT_PLAYER_ATTACK);	
	pBullet->SetKnockBackPower(500.f);
	pBullet->SetDamage(1);

	pBullet->CreateAnimation("Effect");

	pAni = pBullet->GetAnimation();



	pAni->AddClip("SwordBulletLeft");
	pAni->SetClipTexture("SwordBulletLeft", "SwordKirbyLeft", TEXT("SwordKirbyLeft.bmp"));
	pAni->LoadClip("../Bin/Save/Bullet/SwordBulletLeft.clip");




	// Sword Bullet Right
	pBullet = CObj::CreatePrototype<CBullet>("SwordBulletRight", this);
	pBullet->SetAngle(0.f);
	pBullet->SetPlayTime(10.f);
	pBullet->SetMoveSpeed(1000.f);
	pBullet->AddBodyCollision(Size(80.f, 40.f), "SwordBullet", "PlayerAttackProfile", "PlayerAttack");
	pBullet->SetKnockBackPower(100.f);
	pBullet->SetDamage(1);

	pBullet->SetObjectType(OT_PLAYER_ATTACK);

	pBullet->CreateAnimation("Effect");
	pAni = pBullet->GetAnimation();


	pAni->AddClip("SwordBulletRight");
	pAni->SetClipTexture("SwordBulletRight", "SwordKirbyRight", TEXT("SwordKirbyRight.bmp"));
	pAni->LoadClip("../Bin/Save/Bullet/SwordBulletRight.clip");

	// Ani Left
	pBullet = CObj::CreatePrototype<CBullet>("SwordBulletAniLeft", this);
	pBullet->SetAngle(PI);
	pBullet->SetPlayTime(10.f);
	pBullet->SetMoveSpeed(1000.f);
	pBullet->AddBodyCollision(Size(76.f, 32.f), "SwordBullet", "PlayerAttackProfile", "PlayerAttack");
	pBullet->SetObjectType(OT_PLAYER_ATTACK);
	pBullet->SetKnockBackPower(500.f);
	pBullet->SetDamage(3);

	pBullet->CreateAnimation("Effect");
	pAni = pBullet->GetAnimation();


	pAni->AddClip("SwordBulletAniLeft");
	pAni->SetClipTexture("SwordBulletAniLeft", "SwordKirbyLeft", TEXT("SwordKirbyLeft.bmp"));
	pAni->LoadClip("../Bin/Save/Bullet/SwordBulletAniLeft.clip");



	// Ani Right
	pBullet = CObj::CreatePrototype<CBullet>("SwordBulletAniRight", this);
	pBullet->SetAngle(0.f);
	pBullet->SetSize(230.f, 70.f);
	pBullet->SetKnockBackPower(500.f);
	pBullet->SetPlayTime(10.f);
	pBullet->SetMoveSpeed(1000.f);
	pBullet->SetObjectType(OT_PLAYER_ATTACK);
	pBullet->AddBodyCollision(Size(76.f, 32.f), "SwordBullet", "PlayerAttackProfile", "PlayerAttack");
	pBullet->SetDamage(3);

	pBullet->CreateAnimation("Effect");
	pAni = pBullet->GetAnimation();


	pAni->AddClip("SwordBulletAniRight");
	pAni->SetClipTexture("SwordBulletAniRight", "SwordKirbyRight", TEXT("SwordKirbyRight.bmp"));
	pAni->LoadClip("../Bin/Save/Bullet/SwordBulletAniRight.clip");


	// Monster Bullet

	// Mike Bullet

	pBullet = CObj::CreatePrototype<CBullet>("MikeBullet", this);
	pBullet->SetStaticSize(Size(50.f, 50.f));
	pBullet->SetKnockBackPower(200.f);
	pBullet->SetPlayTime(10.f);
	pBullet->SetMoveSpeed(400.f);
	pBullet->SetTileCollisionEnable(false);
	pBullet->SetObjectType(OT_MONSTER_ATTACK);
	pBullet->AddBodyCollision(pBullet->GetStaticSize(), "MikeBullet", "MonsterAttackProfile", "MonsterAttack");
	pBullet->SetDamage(1);

	pBullet->CreateAnimation("Effect");

	pAni = pBullet->GetAnimation();


	pAni->AddClip("MikeBullet");
	pAni->SetClipTexture("MikeBullet", "AbilityMonsterRight", TEXT("AbilityMonsterRight.bmp"));
	pAni->LoadClip("../Bin/Save/Bullet/MikeBullet.clip");




	// Blade Bullet Left

	pBullet = CObj::CreatePrototype<CBullet>("BladeBulletLeft", this);
	pBullet->SetStaticSize(Size(100.f, 90.f));

	pBullet->SetKnockBackPower(200.f);
	pBullet->SetPlayTime(1.f);
	pBullet->SetMoveSpeed(300.f);
	pBullet->SetObjectType(OT_MONSTER_ATTACK);
	pBullet->AddBodyCollision(pBullet->GetStaticSize(), "BladeBullet", "MonsterAttackProfile", "MonsterAttack");
	pBullet->SetDamage(1);

	pBullet->CreateAnimation("Effect");

	pAni = pBullet->GetAnimation();


	pAni->AddClip("BladeBulletLeft");
	pAni->SetClipTexture("BladeBulletLeft", "AbilityMonsterLeft", TEXT("AbilityMonsterLeft.bmp"));
	pAni->LoadClip("../Bin/Save/Bullet/BladeBulletLeft.clip");


	// Blade Bullet Right

	pBullet = CObj::CreatePrototype<CBullet>("BladeBulletRight", this);
	pBullet->SetStaticSize(Size(100.f, 90.f));


	pBullet->SetKnockBackPower(200.f);
	pBullet->SetPlayTime(1.f);
	pBullet->SetMoveSpeed(300.f);
	pBullet->SetObjectType(OT_MONSTER_ATTACK);
	pBullet->AddBodyCollision(pBullet->GetStaticSize(), "BladeBullet", "MonsterAttackProfile", "MonsterAttack");
	pBullet->SetDamage(1);
	pBullet->CreateAnimation("Effect");

	pAni = pBullet->GetAnimation();


	pAni->AddClip("BladeBulletRight");
	pAni->SetClipTexture("BladeBulletRight", "AbilityMonsterRight", TEXT("AbilityMonsterRight.bmp"));
	pAni->LoadClip("../Bin/Save/Bullet/BladeBulletRight.clip");





	// Move Star Right
	CMoveStar* pMoveStar = CObj::CreatePrototype<CMoveStar>("MoveStarRight", this);
	pMoveStar->AddBodyCollision(Size(60.f, 60.f), "Star", "StarProfile", "Star");
	pMoveStar->SetPlayTime(1.f);
	pMoveStar->SetMoveSpeed(100.f);
	pMoveStar->SetObjectType(OT_MOVE_STAR);
	pMoveStar->SetAngle(0.f);
	pMoveStar->SetMoveTime(1.f);
	pMoveStar->LineCollisionEnable();

	pMoveStar->CreateAnimation("Effect");

	pAni = pMoveStar->GetAnimation();


	pAni->AddClip("KirbyStarEffectRight");
	pAni->SetClipTexture("KirbyStarEffectRight", "KirbyStarEffectRight", TEXT("Effect/KirbyVomitRight.bmp"));
	pAni->LoadClip("../Bin/Save/Effect/KirbyStarEffectRight.clip");

	// Move Star Left

	pMoveStar = CObj::CreatePrototype<CMoveStar>("MoveStarLeft", this);
	pMoveStar->AddBodyCollision(Size(60.f, 60.f), "Star", "StarProfile", "Star");
	pMoveStar->SetPlayTime(1.f);
	pMoveStar->SetMoveSpeed(100.f);
	pMoveStar->SetObjectType(OT_MOVE_STAR);
	pMoveStar->SetAngle(PI);
	pMoveStar->SetMoveTime(1.f);
	pMoveStar->LineCollisionEnable();

	pMoveStar->CreateAnimation("Effect");

	pAni = pMoveStar->GetAnimation();


	pAni->AddClip("KirbyStarEffectLeft");
	pAni->SetClipTexture("KirbyStarEffectLeft", "KirbyStarEffectLeft", TEXT("Effect/KirbyVomitLeft.bmp"));
	pAni->LoadClip("../Bin/Save/Effect/KirbyStarEffectLeft.clip");



	// Jump Star Right

	CJumpStar* pJumpStar = CObj::CreatePrototype<CJumpStar>("JumpStarRight", this);
	pJumpStar->AddBodyCollision(Size(60.f, 60.f), "Star", "StarProfile", "Star");
	pJumpStar->SetPlayTime(5.f);
	pJumpStar->SetMoveSpeed(450.f);
	pJumpStar->SetObjectType(OT_JUMP_STAR);
	pJumpStar->SetAngle(0.f);
	pJumpStar->SetPower(500.f);
	pJumpStar->SetGravity(true);
	pJumpStar->SetLineCollisionEnable(true);

	pJumpStar->CreateAnimation("Effect");

	pAni = pJumpStar->GetAnimation();


	pAni->AddClip("KirbyStarEffectRight");
	pAni->SetClipTexture("KirbyStarEffectRight", "KirbyStarEffectRight", TEXT("Effect/KirbyVomitRight.bmp"));
	pAni->LoadClip("../Bin/Save/Effect/KirbyStarEffectRight.clip");;


	// Jump Star Left

	pJumpStar = CObj::CreatePrototype<CJumpStar>("JumpStarLeft", this);
	pJumpStar->AddBodyCollision(Size(60.f, 60.f), "Star", "StarProfile", "Star");
	pJumpStar->SetPlayTime(5.f);
	pJumpStar->SetMoveSpeed(450.f);
	pJumpStar->SetObjectType(OT_JUMP_STAR);
	pJumpStar->SetAngle(PI);
	pJumpStar->SetPower(500.f);
	pJumpStar->SetGravity(true);
	pJumpStar->SetLineCollisionEnable(true);

	pJumpStar->CreateAnimation("Effect");

	pAni = pJumpStar->GetAnimation();


	pAni->AddClip("KirbyStarEffectLeft");
	pAni->SetClipTexture("KirbyStarEffectLeft", "KirbyStarEffectLeft", TEXT("Effect/KirbyVomitLeft.bmp"));
	pAni->LoadClip("../Bin/Save/Effect/KirbyStarEffectLeft.clip");


}

void CMainScene::ObjectPrototype()
{
	// Door를 생성한다.

	CDoor* pDoor = CObj::CreatePrototype<CDoor>(DOOR, this);
	pDoor->SetPivot(0.5f, 0.5f);
	pDoor->SetColorKey(255, 0, 255);

	pDoor->AddBodyCollision(Size(120.f, 154.f), "DoorCollider", "DoorProfile", "Door");
	CCollider* pCollider = pDoor->FindCollider();


	// Door를 추가하고 여기에 아래쪽에 충돌체를 담아두엇다.

	CEffect* pEffect = CObj::CreatePrototype<CEffect>(STAR, this);
	pEffect->SetRelative(Position(0.f, -70.f));
	pEffect->SetMoveSpeed(0.f);

	pEffect->CreateAnimation("Effect");

	CAnimation* pAni = pEffect->GetAnimation();


	pAni->AddClip("KirbyStarEffectRight");
	pAni->SetClipTexture("KirbyStarEffectRight", "KirbyStarEffectRight", TEXT("Effect/KirbyVomitRight.bmp"));
	pAni->LoadClip("../Bin/Save/Effect/KirbyStarEffectRight.clip");
	pAni->GetClip()->tOffset = Position(0.f, 0.f);


	// 플레이어의 UI는 한번 생성하고 계속 이어서 사용하지만, 나머지 보스 UI의 경우에는 보스 -> 보스 보스 보스 이런 식이면 상관없는데,
	// 그런 방식이 아니고 중간에 Stage를 사용한다면, 나중에 따로따로 생성을 해야 한다.
	// 아니면, 활성화를 풀든가.

	
}

void CMainScene::StageLineCollision(CCollider * pSrc, CCollider * pDest, float fTime)
{
	// Line과 충돌하는 물체는 Monster / Player ...
	CColliderLine* pLine = (CColliderLine*)pSrc;
	CObj* pObj = pDest->GetObj();


	// 이런 오브젝트가 부딪힌 경우에는
	switch (pObj->GetObjectType())
	{
		case OT_PLAYER_ATTACK:

			if (pObj->GetTag() == "VomitStar")
			{
				CEffect* pEffect = (CEffect*)pObj;

				if (!pEffect->GetDie())
				{
					pEffect->SetDieTime(0.2f, true);

					GET_SINGLE(CSoundManager)->Play("AttackStarHit", CT_EFFECT);
				}
				break;
				// 좌우 예외처리.. Line에도
			}

		case OT_MONSTER_ATTACK:
		{
			CEffect* pEffect = (CEffect*)pObj;

			if (!pEffect->GetDie())
				pEffect->SetDieTime(0.4f, true);
		}
		break;

		case OT_PLAYER:

			// 커비인데, 중심 충돌체가 아닌 경우에는 처리하지 않는다
			if (pDest->GetTag() != "KirbyBody")
				return;


			break;


		case OT_MOVE_STAR:

			((CMoveStar*)pObj)->MoveDisEnable(); // 멈추도록 하고, 그 상태에서 Line을 탈 수 있도록 해주자.
			
			break;

			case OT_JUMP_STAR:
			{

				// 점프 스타가 이곳에서 부딪혔다. 그러면 이펙트나 총알처럼 없어지는 처리를 해야 하나 ? 
				// NONO 그냥 점프만 제대로 탈 수 있도록 하면 된다.
			
			}

			break;
	}

	if (!pObj->GetLineCollisionEnable())
		return;

	Position tIntersect = pSrc->GetIntersect();

	Position tPos = pObj->GetPos();
	float PosY = pLine->GetY(tPos.x); // pivot 0.5f


	// 충돌의 비율을 따져서 처리를 할지 말지를 정해야 한다.
	if (PosY == -1.f)
	{
		pObj->LineCollisionEnable();

		return;
	}

	// 범위를 벗어나지 않았다는 것은 일단, 중심의 좌표가 Line의 위에 있다는 것이다.
	// 이것을 이용해서 Y축의 타일과 충돌할지를 결정한다.

	pObj->MiddleCollisionEnable();
	pObj->KnockUpEndDisEnable();
	pObj->InputEnable();


	//// Intersect의 위치가, xPos의 +- 10px 범위에 없다면 rreturn
	//if (tIntersect.x < tPos.x - 30.f || tIntersect.x > tPos.x + 30.f)
	//{

	//	pObj->LineCollisionEnable();
	//	return;
	//}


	
	// 커비의 경우에는 예외 처리를 해줘서 점프하고 있는 경우에 SetPos를 호출하지 않도록 한다.
	if (pObj->GetTag() != "Kirby")
	{
		// Kirby가 아닌 경우 그냥 처리.

		pObj->GravityTimeDisEnable();
		pObj->LineCollisionEnable();

		if (pObj->GetTag() == "Twizzy")
		{
			// 트위지인 경우에
			((CTwizzy*)pObj)->SetLineCollision();

			// 시간 초기화, Jump = false 
			// 그냥 움직이는 처리만 하도록. 
			// 이후 Line을 벗어나면 다시 점프하기 시작함.
			// Line에서는 그냥 움직이는 처리만 하는 것.

			pObj->SetPos(tPos.x, PosY - pObj->GetStaticSize().y * 0.48f);
		}

		else if (pObj->GetObjectType() == OT_JUMP_STAR)
		{
			// 점프 스타의 경우에는 .. 
			CJumpStar* pJumpStar = (CJumpStar*)pObj;

			// 점프는 항상 켜져있기 때문에 따로 처리할 필요는 없다.
			if (pJumpStar->JumpDown())
			{
				// 점프를 하고서 떨어지고 있는 상태라면, Line에 대한 처리를 해준다.
				pJumpStar->SetPos(tPos.x, PosY - pObj->GetStaticSize().y * 0.48f);
				pJumpStar->JumpDisEnable();
			}
			// ELSE 점프를 통해서 올라가고 있는 상황이다.
		}

		else
			pObj->SetPos(tPos.x, PosY - pObj->GetStaticSize().y * 0.48f);
	}

	else
	{
		// Kirby

		CPlayer* pPlayer = (CPlayer*)pObj;

		if (!pPlayer->GetJump())
		{
			pObj->SetPos(tPos.x, PosY - pObj->GetStaticSize().y * 0.4f);
			pObj->GravityTimeDisEnable();
			// Kirby인데, 점프중이 아니면 들어온다.

			// 풍선 상태로 직선과 충돌이 된 경우이다. 이 경우에는 반드시 위에서 아래로 떨어진 경우이다.
			// 따라서, 입김불고 Idle. 
			if (pPlayer->GetBallon())
			{
					// 일단은 Ballon DisEnable
					// 풍선 상태에서 바닥으로 떨어진 경우에는 
					// 입김 애니메이션이 호출이 되면서 기본 애니메이션으로 돌아가게 되야 한다.
					pPlayer->BallonDisEnable();
					pPlayer->BalloonStageCollision();

			}

			else if (pPlayer->GetRun())
			{
				if (!KEYPRESS("MoveRight") && !KEYPRESS("MoveLeft"))
					pPlayer->RunDisEnable();
			}


		}
		else
		{
			// 커비가 점프를 하고 있으면, 위치를 다시 셋팅하지 않아서 위로 올라가게 될 것이다.

			if (pPlayer->JumpDown())
			{
				// 점프를 시작한지 좀 되었다는 의미. (떨어지고 있는 상태이다)

				pObj->SetPos(tPos.x, PosY - pObj->GetStaticSize().y * 0.4f);
				pObj->GravityTimeDisEnable();

				pPlayer->JumpDisEnable();

				CAnimation* pAni = pPlayer->GetAnimation();


				switch (pPlayer->GetKirbyType())
				{
				case KIRBY_NORMAL:
					if ((pAni->GetCurrentAnimation() != "KirbyAttackLeft") &&
						(pAni->GetCurrentAnimation() != "KirbyAttackRight"))
						pPlayer->ChangeDefaultClip();
					break;
				case KIRBY_MIKE:
					pPlayer->ChangeDefaultClip();

					break;


				case KIRBY_STONE:

					if (!pPlayer->GetStone())
						pPlayer->ChangeDefaultClip();

					break;

				case KIRBY_SWORD:

					if ((pAni->GetCurrentAnimation() == "SwordJumpAttackLeft") ||
						(pAni->GetCurrentAnimation() == "SwordJumpAttackRight"))
						pPlayer->RunDisEnable();
					else
						pPlayer->ChangeDefaultClip();


						// 점프 공격을 하고 있는 상태가 아니라면 모션을 바꿔준다. ( 떨어지고 있는 상황에서 )

					break;
				}

			}
			

			// else 점프중인데 점프를 시작한지 별로 안 되었다면 그냥 보내준다.
		}

		pPlayer->FallingDisEanble();
		pPlayer->LineCollisionEnable();
		//pPlayer->GravityTimeDisEnable();
		pPlayer->KnockUpEndDisEnable();
		pPlayer->InputEnable();

		if (pPlayer->GetKirbyType() == KIRBY_SLEEP)
		{
			pPlayer->SleepEnable();
		}

	}
	


}

void CMainScene::CreateMonster()
{
#ifdef _DEBUG

	if (KEYDOWN("BurtCreate"))
	{
		CBurt* pBurt = CObj::CreateCloneObject<CBurt>("Burt", "Burt", "Default", this);
		pBurt->SetDir(m_pPlayer->GetDir());
		pBurt->SetPos(m_pPlayer->GetPos() + Position(0.f, -200.f));

	}
	else if (KEYDOWN("GrizooCreate"))
	{
		CGrizoo* pGrizoo = CObj::CreateCloneObject<CGrizoo>("Grizoo", "Grizoo", "Default", this);
		pGrizoo->SetDir(m_pPlayer->GetDir());
		pGrizoo->SetPos(m_pPlayer->GetPos() + Position(0.f, -200.f));

	}
	else if (KEYDOWN("TwizzyCreate"))
	{
		CTwizzy* pTwizzy = CObj::CreateCloneObject<CTwizzy>("Twizzy", "Twizzy", "Default", this);
		pTwizzy->SetDir(m_pPlayer->GetDir());
		pTwizzy->SetPos(m_pPlayer->GetPos() + Position(0.f, -200.f));
	}

#endif
}
