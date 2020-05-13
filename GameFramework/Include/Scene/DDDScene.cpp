#include "DDDScene.h"
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
#include "../Object/DDD.h"
#include "../UI/TargetUI.h"
#include "../Sound/SoundManager.h"


CDDDScene::CDDDScene()
{
}


CDDDScene::~CDDDScene()
{
}

void CDDDScene::Start()
{
	CMainScene::Start();
}

bool CDDDScene::Init()
{
	if (!CMainScene::Init())
		return false;

	GET_SINGLE(CSoundManager)->Play("DDDScene", CT_BGM);
	m_strBgm = "DDDScene";


	// 각 스테이지에 따른 처리를 한다.
	// Stage객체의 리소스는 해당 오브젝트가 사라지는 순간(장면전환의 순간)에서 해당 오브젝트를 소멸하면서
	// 정리를 해주니 상관하지 않아도 되는 부분이다.

	// 그리고 BackGround나, ForeGround의 경우에는 이미지의 위치에 상관없이 이용할 수 있는 시스템이
	// 구현이 되어 있는 상태라서, 이미지를 수정할 필요없이 위치만 알아오면 상관이 없다.
	// 예상하지 못한 상황이 하나 있는데, BackGround에 애니메이션이 있는 경우이다.
	// 단순히 플레이어의 위치를 기준으로 스크롤하는 것만 생각하고 구현을 했는데,
	// 애니메이션 출력이 있네. 그러면 스테이지에 애니메이션을 적용하는 기능을 만들던가 아니면, 해당 리소스를 사용하지 않던가
	// 둘 중에 하나이다.

	// BackGround
	// DDD Background Ani

	CEffect* pEffect = CObj::CreateObj<CEffect>("DDDBackGround1", "BackGround", this);
	pEffect->SetPivot(0.f, 0.f);
	pEffect->CreateAnimation("Ani");
	pEffect->SetPos(0.f, -135.f);
	pEffect->SetTileCollisionEnable(false);

	CAnimation* pAni = pEffect->GetAnimation();


	pAni->AddClip("DDDBackGround", AO_ONCE_DESTROY, 1.f);
	pAni->SetClipTexture("DDDBackGround", "DDDScene", TEXT("DDDScene.bmp"));
	pAni->LoadClip("../Bin/Save/AniObject/DDDBackGround.clip");



	pEffect = CObj::CreateObj<CEffect>("DDDBackGround2", "BackGround", this);
	pEffect->SetPivot(0.f, 0.f);
	pEffect->CreateAnimation("Ani");
	pEffect->SetPos(1592.f - 150.f, -135.f);
	pEffect->SetTileCollisionEnable(false);

	pAni = pEffect->GetAnimation();


	pAni->AddClip("DDDBackGround", AO_ONCE_DESTROY, 1.f);
	pAni->SetClipTexture("DDDBackGround", "DDDScene", TEXT("DDDScene.bmp"));
	pAni->LoadClip("../Bin/Save/AniObject/DDDBackGround.clip");






	CStage* pStage = CObj::CreateObj<CStage>("Stage", "BackGround", this);

	// ForeGround를 설정해준다.
	pStage->SetForeGroundTexture("DDDScene", TEXT("DDDScene.bmp"));
	pStage->SetForeGroundTextureColorKey(176, 160, 208);
	pStage->SetStagePos(29, 2645, 2639, 3827);

	// BackGround를 설정해준다.

	pStage->Load("../Bin/Stage/DDD.stg"); 


	

	// BackGruond Left

	pEffect = CObj::CreateObj<CEffect>("DDDBackGroundLeft", "BackGround", this);
	pEffect->SetPivot(0.f, 0.f);
	pEffect->SetPos(-4.f, 568.f);
	pEffect->SetTileCollisionEnable(false);

	pEffect->CreateAnimation("Ani");

	pAni = pEffect->GetAnimation();


	pAni->AddClip("DDDBackGroundLeft", AO_ONCE_DESTROY, 1.f);
	pAni->SetClipTexture("DDDBackGroundLeft", "DDDScene", TEXT("DDDScene.bmp"));

	pAni->LoadClip("../Bin/Save/AniObject/DDDBackGroundLeft.clip");

	// BackGruond Right

	pEffect = CObj::CreateObj<CEffect>("DDDBackGroundRight", "BackGround", this);
	pEffect->SetPivot(0.f, 0.f);
	pEffect->SetPos(2397.f, 568.f);
	//pEffect->SetPos(800.f, 557.f);
	pEffect->SetTileCollisionEnable(false);

	pEffect->CreateAnimation("Ani");

	pAni = pEffect->GetAnimation();


	pAni->AddClip("DDDBackGroundRight", AO_ONCE_DESTROY, 1.f);
	pAni->SetClipTexture("DDDBackGroundRight", "DDDScene", TEXT("DDDScene.bmp"));
	pAni->LoadClip("../Bin/Save/AniObject/DDDBackGroundRight.clip");





	CScene* pPrevScene = GET_SINGLE(CSceneManager)->GetCurrentScene();



	// 장면이 변했으므로 이전 장면을 통해서 커비오브젝트를 찾아온다. ++ 1
	CPlayer* pPlayer = (CPlayer*)pPrevScene->FindObject("Kirby");
	pPlayer->JumpDisEnable();


	// 현재 Scene이 초기화중이라서, SceneManager에서 Scene이 잡혀있지 않다(FindObject를 이용할 수 없다)
	pStage->SetPlayer(pPlayer); // ++ 2

	pPlayer->SetPos(900.f, 600.f);

	m_pPlayer = pPlayer; 

	// 첫 장면을 제외하고는 이 함수를 호출해야 한다.
	PrototypeSave();

	CDDD* pDDD = CObj::CreateObj<CDDD>("DDD", "Default", this);
	pDDD->SetPlayer(m_pPlayer);
	pDDD->SetPos(1300.f, 600.f);

	// 충돌체는 애니메이션에서는 필요없으므로 이곳에서 해주도록 하겠다.
	pDDD->AddBodyCollision(pDDD->GetStaticSize(), "DDDBody", "MonsterProfile", "Monster");

	// DDD를 타겟으로 설정한다.
	CObj* pUI = pPrevScene->FindObject("BossHP");
	((CTargetUI*)pUI)->SetTarget(pDDD);
	SAFE_RELEASE(pUI);

	pUI = pPrevScene->FindObject("BossHPBorder");
	pUI->SetEnable(true);
	SAFE_RELEASE(pUI);

	GET_SINGLE(CSceneManager)->SetTargetPivot(0.5f, 0.7f);

	CreateDoor(m_tDoorPos, ST_SCENE5);


	return true;
}

void CDDDScene::Update(float fTime)
{

	// 여기서, 
	CMainScene::Update(fTime);


	// 보스 키를 추가한다.
	if (KEYDOWN("DDD"))
	{
		CDDD* pDDD = (CDDD*)FindObject("DDD");

		if (pDDD)
		{
			pDDD->SetHP(pDDD->GetHPMax());
			SAFE_RELEASE(pDDD);
		}

	}

}

bool CDDDScene::SoundInit()
{
	GET_SINGLE(CSoundManager)->LoadSound("DDDScene", true, "Music/DDDScene.mp3");

	GET_SINGLE(CSoundManager)->LoadSound("BossAttack", false , "DDD/BossAttack.wav");
	GET_SINGLE(CSoundManager)->LoadSound("BossDie", false, "DDD/BossDie.wav");
	GET_SINGLE(CSoundManager)->LoadSound("BossJump", false, "DDD/BossJump.wav");
	GET_SINGLE(CSoundManager)->LoadSound("BossStarHit", false, "DDD/BossStarHit.wav");

	return false;
}

void CDDDScene::CreatePrototype()
{
	// 스테이지에서 사용할 모든 몬스터를 생성한다.
	CMainScene::CreatePrototype();

}

void CDDDScene::EffectPrototype()
{
	// 스테이지에서 사용할 모든 이펙트를 생성한다.
	CMainScene::EffectPrototype();
}

void CDDDScene::StageLineCollision(CCollider * pSrc, CCollider * pDest, float fTime)
{
	CMainScene::StageLineCollision(pSrc, pDest, fTime);
}

void CDDDScene::CreateMonster()
{
	CMainScene::CreateMonster();

	// MainScene의 부분을 호출하는 순간에 가상함수의 규칙에 따라서, 이 함수가 호출이 될 것인데, 기본적인 처리를
	// MainScene의 CreateMonster함수에서 해주고 있으므로 해당 함수를 호출해주는 것으로 한다.
}

