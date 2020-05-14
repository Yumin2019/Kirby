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


	// �� ���������� ���� ó���� �Ѵ�.
	// Stage��ü�� ���ҽ��� �ش� ������Ʈ�� ������� ����(�����ȯ�� ����)���� �ش� ������Ʈ�� �Ҹ��ϸ鼭
	// ������ ���ִ� ������� �ʾƵ� �Ǵ� �κ��̴�.

	// �׸��� BackGround��, ForeGround�� ��쿡�� �̹����� ��ġ�� ������� �̿��� �� �ִ� �ý�����
	// ������ �Ǿ� �ִ� ���¶�, �̹����� ������ �ʿ���� ��ġ�� �˾ƿ��� ����� ����.
	// �������� ���� ��Ȳ�� �ϳ� �ִµ�, BackGround�� �ִϸ��̼��� �ִ� ����̴�.
	// �ܼ��� �÷��̾��� ��ġ�� �������� ��ũ���ϴ� �͸� �����ϰ� ������ �ߴµ�,
	// �ִϸ��̼� ����� �ֳ�. �׷��� ���������� �ִϸ��̼��� �����ϴ� ����� ������� �ƴϸ�, �ش� ���ҽ��� ������� �ʴ���
	// �� �߿� �ϳ��̴�.

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

	// ForeGround�� �������ش�.
	pStage->SetForeGroundTexture("DDDScene", TEXT("DDDScene.bmp"));
	pStage->SetForeGroundTextureColorKey(176, 160, 208);
	pStage->SetStagePos(29, 2645, 2639, 3827);

	// BackGround�� �������ش�.

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



	// ����� �������Ƿ� ���� ����� ���ؼ� Ŀ�������Ʈ�� ã�ƿ´�. ++ 1
	CPlayer* pPlayer = (CPlayer*)pPrevScene->FindObject("Kirby");
	pPlayer->JumpDisEnable();


	// ���� Scene�� �ʱ�ȭ���̶�, SceneManager���� Scene�� �������� �ʴ�(FindObject�� �̿��� �� ����)
	pStage->SetPlayer(pPlayer); // ++ 2

	pPlayer->SetPos(900.f, 600.f);

	m_pPlayer = pPlayer; 

	// ù ����� �����ϰ�� �� �Լ��� ȣ���ؾ� �Ѵ�.
	PrototypeSave();

	CDDD* pDDD = CObj::CreateObj<CDDD>("DDD", "Default", this);
	pDDD->SetPlayer(m_pPlayer);
	pDDD->SetPos(1300.f, 600.f);

	// �浹ü�� �ִϸ��̼ǿ����� �ʿ�����Ƿ� �̰����� ���ֵ��� �ϰڴ�.
	pDDD->AddBodyCollision(pDDD->GetStaticSize(), "DDDBody", "MonsterProfile", "Monster");

	// DDD�� Ÿ������ �����Ѵ�.
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

	// ���⼭, 
	CMainScene::Update(fTime);


	// ���� Ű�� �߰��Ѵ�.
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
	// ������������ ����� ��� ���͸� �����Ѵ�.
	CMainScene::CreatePrototype();

}

void CDDDScene::EffectPrototype()
{
	// ������������ ����� ��� ����Ʈ�� �����Ѵ�.
	CMainScene::EffectPrototype();
}

void CDDDScene::StageLineCollision(CCollider * pSrc, CCollider * pDest, float fTime)
{
	CMainScene::StageLineCollision(pSrc, pDest, fTime);
}

void CDDDScene::CreateMonster()
{
	CMainScene::CreateMonster();

	// MainScene�� �κ��� ȣ���ϴ� ������ �����Լ��� ��Ģ�� ����, �� �Լ��� ȣ���� �� ���ε�, �⺻���� ó����
	// MainScene�� CreateMonster�Լ����� ���ְ� �����Ƿ� �ش� �Լ��� ȣ�����ִ� ������ �Ѵ�.
}

