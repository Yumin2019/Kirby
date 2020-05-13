#include "AnimationEditScene.h"
#include "../Animation/Animation.h"
// Monster
#include "../Object/Burt.h"
#include "../Object/Grizoo.h"
#include "../Object/Twizzy.h"
#include "../Object/BladeKnight.h"
#include "../Object/Mike.h"
#include "../Object/Rocky.h"
#include "../Object/Sleep.h"

// DDD
#include "../Object/DDD.h"

// Player
#include "../Object/Player.h"

// Effect
#include "../Object/Effect.h"

// Bullet
#include "../Object/Bullet.h"

// Grass
#include "../Object/Grass.h"

// DDDBackGround
#include "../Object/DDDBackGround.h"



CAnimationEditScene::CAnimationEditScene()
{
}


CAnimationEditScene::~CAnimationEditScene()
{
}

void CAnimationEditScene::Start()
{
	CScene::Start();
}

bool CAnimationEditScene::Init()
{
	if (!CScene::Init())
		return false;



	/*CreatePrototype();*/

	CObj* pObj = CObj::CreateObj<CPlayer>("Kirby", "Default", this);

	// Monster
	CObj::CreateObj<CBurt>("Burt", "Default", this);
	
	CObj::CreateObj<CGrizoo>("Grizoo", "Default", this);

	CObj::CreateObj<CTwizzy>("Twizzy", "Default", this);

	// Boss
	CObj::CreateObj<CDDD>("DDD", "Default", this);

	// Abil Mon
	CObj::CreateObj<CBladeKnight>("BladeKnight", "Default", this);
	CObj::CreateObj<CMike>("Mike", "Default", this);
	CObj::CreateObj<CRocky>("Rocky", "Default", this);
	CObj::CreateObj<CSleep>("Sleep", "Default", this);

	

	// Grass »ý¼º 7°³.
	pObj = CObj::CreateObj<CGrass>("Stage1Grass1", "Default", this); pObj->LoadClip("../Bin/Save/AniObject/Stage1Grass1.clip");
	pObj = CObj::CreateObj<CGrass>("Stage1Grass2", "Default", this); pObj->LoadClip("../Bin/Save/AniObject/Stage1Grass2.clip");
	pObj = CObj::CreateObj<CGrass>("Stage1Grass3", "Default", this); pObj->LoadClip("../Bin/Save/AniObject/Stage1Grass3.clip");
	pObj = CObj::CreateObj<CGrass>("Stage2Grass1", "Default", this); pObj->LoadClip("../Bin/Save/AniObject/Stage2Grass1.clip");
	pObj = CObj::CreateObj<CGrass>("Stage2Grass2", "Default", this); pObj->LoadClip("../Bin/Save/AniObject/Stage2Grass2.clip");
	pObj = CObj::CreateObj<CGrass>("Stage2Grass3", "Default", this); pObj->LoadClip("../Bin/Save/AniObject/Stage2Grass3.clip");
	pObj = CObj::CreateObj<CGrass>("Stage2Grass4", "Default", this); pObj->LoadClip("../Bin/Save/AniObject/Stage2Grass4.clip");
	


	// Kirby Attack Left Effect
	CEffect* pEffect = CObj::CreateObj<CEffect>("KirbyAttackEffectLeft", "Default", this);

	pEffect->CreateAnimation("Effect");
	
	CAnimation* pAni = pEffect->GetAnimation();
	

	pAni->AddClip("KirbyAttackEffectLeft", AO_ONCE_DESTROY, 1.f);
	pAni->SetClipTexture("KirbyAttackEffectLeft", "KirbyAttackEffectLeft", TEXT("Effect/KirbyAttackLeft.bmp"));
	pAni->LoadClip("../Bin/Save/Effect/KirbyAttackEffectLeft.clip");



	// Attack Right Effect
	pEffect = CObj::CreateObj<CEffect>("KirbyAttackEffectRight", "Default", this);

	pEffect->CreateAnimation("Effect");

	pAni = pEffect->GetAnimation();


	pAni->AddClip("KirbyAttackEffectRight", AO_ONCE_DESTROY, 1.f);
	pAni->SetClipTexture("KirbyAttackEffectRight", "KirbyAttackEffectRight", TEXT("Effect/KirbyAttackRight.bmp"));
	pAni->LoadClip("../Bin/Save/Effect/KirbyAttackEffectRight.clip");




	// Breath Attack Left

	pEffect = CObj::CreateObj<CEffect>("KirbyBreathAttackEffectLeft", "Default", this);

	pEffect->CreateAnimation("Effect");
	pEffect->SetStaticSize(60.f, 60.f);

	pAni = pEffect->GetAnimation();


	pAni->AddClip("KirbyBreathAttackEffectLeft", AO_ONCE_DESTROY, 1.f);
	pAni->SetClipTexture("KirbyBreathAttackEffectLeft", "KirbyBreathAttackEffectLeft", TEXT("Effect/KirbyBreathAttackLeft.bmp"));
	pAni->LoadClip("../Bin/Save/Effect/KirbyBreathAttackEffectLeft.clip");



	// Breath Attack Right
	pEffect = CObj::CreateObj<CEffect>("KirbyBreathAttackEffectRight", "Default", this);
	pEffect->SetStaticSize(60.f, 60.f);

	pEffect->CreateAnimation("Effect");

	pAni = pEffect->GetAnimation();


	pAni->AddClip("KirbyBreathAttackEffectRight", AO_ONCE_DESTROY, 1.f);
	pAni->SetClipTexture("KirbyBreathAttackEffectRight", "KirbyBreathAttackEffectRight", TEXT("Effect/KirbyBreathAttackRight.bmp"));
	pAni->LoadClip("../Bin/Save/Effect/KirbyBreathAttackEffectRight.clip");



	
	// Run Left

	pEffect = CObj::CreateObj<CEffect>("KirbyRunEffectLeft", "Default", this);

	pEffect->CreateAnimation("Effect");

	pAni = pEffect->GetAnimation();


	pAni->AddClip("KirbyRunEffectLeft", AO_ONCE_DESTROY, 1.f);
	pAni->SetClipTexture("KirbyRunEffectLeft", "KirbyRunEffectLeft", TEXT("Effect/KirbyRunLeft.bmp"));
	pAni->LoadClip("../Bin/Save/Effect/KirbyRunEffectLeft.clip");


	// Run Right
	pEffect = CObj::CreateObj<CEffect>("KirbyRunEffectRight", "Default", this);

	pEffect->CreateAnimation("Effect");

	pAni = pEffect->GetAnimation();


	pAni->AddClip("KirbyRunEffectRight", AO_ONCE_DESTROY, 1.f);
	pAni->SetClipTexture("KirbyRunEffectRight", "KirbyRunEffectRight", TEXT("Effect/KirbyRunRight.bmp"));
	pAni->LoadClip("../Bin/Save/Effect/KirbyRunEffectRight.clip");



	// Vomit Left

	pEffect = CObj::CreateObj<CEffect>("KirbyStarEffectLeft", "Default", this);
	pEffect->SetStaticSize(60.f, 60.f);

	pEffect->CreateAnimation("Effect");

	pAni = pEffect->GetAnimation();


	pAni->AddClip("KirbyStarEffectLeft", AO_ONCE_DESTROY, 1.f);
	pAni->SetClipTexture("KirbyStarEffectLeft", "KirbyStarEffectLeft", TEXT("Effect/KirbyVomitLeft.bmp"));
	pAni->LoadClip("../Bin/Save/Effect/KirbyStarEffectLeft.clip");


	// Vomit Right
	pEffect = CObj::CreateObj<CEffect>("KirbyStarEffectRight", "Default", this);
	pEffect->SetStaticSize(60.f, 60.f);

	pEffect->CreateAnimation("Effect");

	pAni = pEffect->GetAnimation();


	pAni->AddClip("KirbyStarEffectRight", AO_ONCE_DESTROY, 1.f);
	pAni->SetClipTexture("KirbyStarEffectRight", "KirbyStarEffectRight", TEXT("Effect/KirbyVomitRight.bmp"));
	pAni->LoadClip("../Bin/Save/Effect/KirbyStarEffectRight.clip");
	



	// Small Star Effect
	pEffect = CObj::CreateObj<CEffect>("SmallStar", "Default", this);

	pEffect->CreateAnimation("Effect");

	pAni = pEffect->GetAnimation();


	pAni->AddClip("SmallStar", AO_LOOP, 1.f);
	pAni->SetClipTexture("SmallStar", "SmallStar", TEXT("Effect/SmallStarEffect.bmp"));
	pAni->LoadClip("../Bin/Save/Effect/SmallStar.clip");

	pAni->SetTextureColorKeyAll(255, 0, 255);


		// blue Ball Effect
	pEffect = CObj::CreateObj<CEffect>("BlueBall", "Default", this);
	pEffect->SetStaticSize(100.f, 100.f);

	pEffect->CreateAnimation("Effect");

	pAni = pEffect->GetAnimation();


	pAni->AddClip("BlueBall", AO_LOOP, 1.f);
	pAni->SetClipTexture("BlueBall", "BlueBall", TEXT("Effect/BlueBallEffect.bmp"));
	pAni->SetTextureColorKeyAll(255, 0, 255);
	pAni->LoadClip("../Bin/Save/Effect/BlueBall.clip");



	// PinkStoneEffect
	pEffect = CObj::CreateObj<CEffect>("PinkStoneEffect", "Default", this);

	pEffect->CreateAnimation("Effect");

	pAni = pEffect->GetAnimation();


	pAni->AddClip("PinkStoneEffect", AO_LOOP, 1.f);
	pAni->SetClipTexture("PinkStoneEffect", "StoneKirbyLeft", TEXT("StoneKirbyLeft.bmp"));
	pAni->SetTextureColorKeyAll(96, 160, 128);
	pAni->LoadClip("../Bin/Save/Effect/PinkStoneEffect.clip");


	// Sword Bullet Left
	CBullet* pBullet = CObj::CreateObj<CBullet>("SwordBulletLeft", "Default", this);
	pBullet->SetStaticSize(80.f, 40.f);
	pBullet->CreateAnimation("Effect");

	pAni = pBullet->GetAnimation();


	pAni->AddClip("SwordBulletLeft", AO_LOOP, 1.f);
	pAni->SetClipTexture("SwordBulletLeft", "SwordKirbyLeft", TEXT("SwordKirbyLeft.bmp"));
	pAni->LoadClip("../Bin/Save/Bullet/SwordBulletLeft.clip");

	pAni->SetTextureColorKeyAll(160, 224, 128);



	// Sword Bullet Right
	pBullet = CObj::CreateObj<CBullet>("SwordBulletRight", "Default", this);
	pBullet->SetStaticSize(80.f, 40.f);

	pBullet->CreateAnimation("Effect");

	pAni = pBullet->GetAnimation();


	pAni->AddClip("SwordBulletRight", AO_LOOP, 1.f);
	pAni->SetClipTexture("SwordBulletRight", "SwordKirbyRight", TEXT("SwordKirbyRight.bmp"));
	pAni->LoadClip("../Bin/Save/Bullet/SwordBulletRight.clip");
	pAni->SetTextureColorKeyAll(160, 224, 128);



	// Ani Left
	pBullet = CObj::CreateObj<CBullet>("SwordBulletAniLeft", "Default", this);
	pBullet->SetStaticSize(Size(76.f, 32.f));
	pBullet->CreateAnimation("Effect");

	pAni = pBullet->GetAnimation();


	pAni->AddClip("SwordBulletAniLeft", AO_LOOP, 1.f);
	pAni->SetClipTexture("SwordBulletAniLeft", "SwordKirbyLeft", TEXT("SwordKirbyLeft.bmp"));
	pAni->LoadClip("../Bin/Save/Bullet/SwordBulletAniLeft.clip");
	pAni->SetTextureColorKeyAll(160, 224, 128);



	// Ani Right
	pBullet = CObj::CreateObj<CBullet>("SwordBulletAniRight", "Default", this);
	pBullet->SetStaticSize(Size(76.f, 32.f));

	pBullet->CreateAnimation("Effect");

	pAni = pBullet->GetAnimation();


	pAni->AddClip("SwordBulletAniRight", AO_LOOP, 1.f);
	pAni->SetClipTexture("SwordBulletAniRight", "SwordKirbyRight", TEXT("SwordKirbyRight.bmp"));
	pAni->LoadClip("../Bin/Save/Bullet/SwordBulletAniRight.clip");
	pAni->SetTextureColorKeyAll(160, 224, 128);




	// Mike Bullet
	pBullet = CObj::CreateObj<CBullet>("MikeBullet", "Default", this);
	pBullet->SetStaticSize(Size(50.f, 50.f ));

	pBullet->CreateAnimation("Effect");

	pAni = pBullet->GetAnimation();


	pAni->AddClip("MikeBullet");
	pAni->SetClipTexture("MikeBullet", "AbilityMonsterRight", TEXT("AbilityMonsterRight.bmp"));
	pAni->LoadClip("../Bin/Save/Bullet/MikeBullet.clip");
	pAni->SetTextureColorKeyAll(0, 72, 80);


	// Blade Bullet
	pBullet = CObj::CreateObj<CBullet>("BladeBulletLeft", "Default", this);
	pBullet->SetStaticSize(Size(100.f, 90.f));


	pBullet->CreateAnimation("Effect");

	pAni = pBullet->GetAnimation();


	pAni->AddClip("BladeBulletLeft");
	pAni->SetClipTexture("BladeBulletLeft", "AbilityMonsterLeft", TEXT("AbilityMonsterLeft.bmp"));
	pAni->LoadClip("../Bin/Save/Bullet/BladeBulletLeft.clip");
	pAni->SetTextureColorKeyAll(0, 72, 80);

	pBullet = CObj::CreateObj<CBullet>("BladeBulletRight", "Default", this);
	pBullet->SetStaticSize(Size(100.f, 90.f));


	pBullet->CreateAnimation("Effect");

	pAni = pBullet->GetAnimation();


	pAni->AddClip("BladeBulletRight");
	pAni->SetClipTexture("BladeBulletRight", "AbilityMonsterRight", TEXT("AbilityMonsterRight.bmp"));
	pAni->LoadClip("../Bin/Save/Bullet/BladeBulletRight.clip");

	pAni->SetTextureColorKeyAll(0, 72, 80);



	// DDD Background Ani

	pEffect = CObj::CreateObj<CEffect>("DDDBackGround", "Default", this);
	pEffect->SetStaticSize(60.f, 60.f);

	pEffect->CreateAnimation("Ani");

	pAni = pEffect->GetAnimation();


	pAni->AddClip("DDDBackGround", AO_ONCE_DESTROY, 1.f);
	pAni->SetClipTexture("DDDBackGround", "DDDScene", TEXT("DDDScene.bmp"));
	pAni->LoadClip("../Bin/Save/AniObject/DDDBackGround.clip");


	// BackGruond Left

	pEffect = CObj::CreateObj<CEffect>("DDDBackGroundLeft", "Default", this);
	pEffect->SetStaticSize(60.f, 60.f);

	pEffect->CreateAnimation("Ani");

	pAni = pEffect->GetAnimation();


	pAni->AddClip("DDDBackGroundLeft", AO_ONCE_DESTROY, 1.f);
	pAni->SetClipTexture("DDDBackGroundLeft", "DDDScene", TEXT("DDDScene.bmp"));

	pAni->LoadClip("../Bin/Save/AniObject/DDDBackGroundLeft.clip");

	// BackGruond Right

	pEffect = CObj::CreateObj<CEffect>("DDDBackGroundRight", "Default", this);
	pEffect->SetStaticSize(60.f, 60.f);

	pEffect->CreateAnimation("Ani");

	pAni = pEffect->GetAnimation();


	pAni->AddClip("DDDBackGroundRight", AO_ONCE_DESTROY, 1.f);
	pAni->SetClipTexture("DDDBackGroundRight", "DDDScene", TEXT("DDDScene.bmp"));
	pAni->LoadClip("../Bin/Save/AniObject/DDDBackGroundRight.clip");



	SetWorld();

	return true;
}

void CAnimationEditScene::CreatePrototype()
{
}
