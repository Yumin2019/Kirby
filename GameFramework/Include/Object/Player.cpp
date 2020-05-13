#include "Player.h"
#include "../InputComponent.h"
#include "../Animation/Animation.h"
#include "../Core/Input.h"
#include "../Collision/ColliderRect.h"
#include "../Core/GameManager.h"
#include "../Collision/CollisionManager.h"
#include "../Core/SceneManager.h"
#include "../Object/Monster.h"
#include "../Math.h"
#include "../Object/Effect.h"
#include "../Object/Bullet.h"
#include "../Collision/ColliderSphere.h"
#include "../MikeObject.h"

#include "../Object/Twizzy.h"
#include "../Object/Burt.h"
#include "../Object/Grizoo.h"

#include "../Object/MoveStar.h"
#include "../Object/JumpStar.h"
#include "../Object/Door.h"
#include "../Object/DDD.h"
#include "../Sound/SoundManager.h"
#include "../Scene/MainScene.h"

enum STONE_CLIP
{
	ST_IDLE,
	ST_DOWN,
	ST_WALK,
	ST_RUN,
	ST_FLY,
	ST_PREVENT,
	ST_SLIDE,
	ST_ATTACK,
	ST_VOMIT,
	ST_HIT,
	ST_FLY_UP,
	ST_JUMP,
	ST_BIG_HIT,
	ST_END
};

// AttackLeft AttackRight 근육질의 아저씨 OR 마리오로 고정.

const char* g_pStoneAnimation[SD_END][ST_END] =
{
	{"StoneIdleLeft", "StoneDownLeft", "StoneWalkLeft", "StoneRunLeft", "StoneFlyLeft",
	"StonePreventLeft", "StoneSlideLeft", "StoneAttackLeft", "StoneVomitLeft", "StoneHitLeft",
	"StoneFlyUpLeft", "StoneJumpLeft", "StoneBigHitLeft"},

	{"StoneIdleRight", "StoneDownRight", "StoneWalkRight", "StoneRunRight", "StoneFlyRight",
	"StonePreventRight", "StoneSlideRight", "StoneAttackRight", "StoneVomitRight", "StoneHitRight",
	"StoneFlyUpRight", "StoneJumpRight", "StoneBigHitRight"},
};

enum SWORD_CLIP
{
	SW_IDLE,
	SW_DOWN,
	SW_WALK,
	SW_RUN,
	SW_FLY,

	SW_PREVENT,
	SW_SLIDE,
	SW_ATTACK1, // 공격에 콤보 시스템 있는 거 아심 ? 여기에 이펙트 따로 한바닥 나올 것 같음.. ㅎㄷㄷ ㅠㅠ
	SW_ATTACK2,
	SW_ATTACK3,
	SW_DASH_ATTACK,

	SW_JUMP_ATTACK,
	SW_VOMIT,
	SW_HIT,
	SW_BIG_HIT,
	SW_FLY_UP,

	SW_JUMP,
	SW_END
};

const char* g_pSwordAnimation[SD_END][SW_END] = {

	{"SwordIdleLeft", "SwordDownLeft", "SwordWalkLeft", "SwordRunLeft", "SwordFlyLeft",
	"SwordPreventLeft", "SwordSlideLeft", "SwordAttack1Left", "SwordAttack2Left", "SwordAttack3Left",
	"SwordDashAttackLeft","SwordJumpAttackLeft", "SwordVomitLeft", "SwordHitLeft", "SwordBigHitLeft", 
	"SwordFlyUpLeft","SwordJumpLeft"},

	{"SwordIdleRight", "SwordDownRight", "SwordWalkRight", "SwordRunRight", "SwordFlyRight",
	"SwordPreventRight", "SwordSlideRight", "SwordAttack1Right", "SwordAttack2Right", "SwordAttack3Right",
	"SwordDashAttackRight", "SwordJumpAttackRight", "SwordVomitRight", "SwordHitRight", "SwordBigHitRight", 
	"SwordFlyUpRight","SwordJumpRight"}
};

enum PLAYER_NORMAL_CLIP
{
	PL_IDLE,
	PL_DOWN,
	PL_WALK,
	PL_RUN,
	PL_FLY,
	PL_PREVENT,
	PL_SLIDE,
	PL_ATTACK,
	PL_TAKE,
	PL_VOMIT,
	PL_EAT,
	//PL_DIE, Die는 왼쪽 오른쪽의 구분이 없으므로 그냥 따로 해준다.
	PL_HIT,
	PL_BIG_WALK,
	PL_BIG_HIT,
	PL_FLY_UP,
	PL_JUMP,
	PL_BIG_JUMP,
	PL_BIG_IDLE,
	PL_BIG_RUN,
	PL_END

};

// 능력에 따른 구분은 나중에....
const char* g_pPlayerAnimationName[SD_END][PL_END] =
{
	{"KirbyIdleLeft", "KirbyDownLeft", "KirbyWalkLeft", "KirbyRunLeft",	"KirbyFlyLeft", 
	"KirbyPreventLeft", "KirbySlideLeft", "KirbyAttackLeft", "KirbyTakeLeft", "KirbyVomitLeft", 
	"KirbyEatLeft", "KirbyHitLeft", "KirbyBigWalkLeft", "KirbyBigHitLeft" , "KirbyFlyUpLeft",
	"KirbyJumpLeft", "KirbyBigJumpLeft", "KirbyBigIdleLeft", "KirbyBigRunLeft"},

	{"KirbyIdleRight", "KirbyDownRight", "KirbyWalkRight", "KirbyRunRight", "KirbyFlyRight", 
	"KirbyPreventRight", "KirbySlideRight", "KirbyAttackRight", "KirbyTakeRight", "KirbyVomitRight",
	"KirbyEatRight", "KirbyHitRight", "KirbyBigWalkRight", "KirbyBigHitRight", "KirbyFlyUpRight",
	"KirbyJumpRight", "KirbyBigJumpRight", "KirbyBigIdleRight", "KirbyBigRunRight"}
	
};

enum EASY_CLIP
{
	// 요리를 빼던가.. 아니면 좌우 다 넣어서 1차원배열의 형태로 사용.

	E_MIKE1,
	E_MIKE2,
	E_MIKE3,
	E_SLEEP,
	E_END
};

const char* g_pEasyAnimation[SD_END][E_END] =
{
	{"Mike1Left", "Mike2Left", "Mike3Left", "SleepLeft"},
	{"Mike1Right", "Mike2Right", "Mike3Right", "SleepRight"},
};



CPlayer::CPlayer()
{
	m_bIsDie = false;
	m_eMikeAttack = MA_NONE;
	m_pTornadoEffect = nullptr;
	m_bVomit = false;
	m_bBalloon = false;
	m_bRun = false;
	m_bStop = false;
	m_bBreath = false;
	m_bAttack = false;
	m_bFalling = false;
	m_fAttackTime = 0.f;
	m_fJumpTime = 0.f;
	m_fEatTime = 0.f;
	m_fSlideTime = 0.f;
	m_fSlidePixel = 0.f;
	m_fPower = 0.f;
	m_fTime = 0.f;
	m_fPowerTime = 0.f;
	m_fTakeTime = 0.f;
	m_fBreathTime = 0.f;
	m_fVomitTime = 0.f;
	m_fAttackEndLimitTime = 0.f;
	m_bPrevent = false;
	m_bTake = false;
	m_bEat = false;
	m_bBig = false;
	m_bDown = false;
	m_bJump = false;
	m_bSlide = false;
	m_eType = KIRBY_NORMAL;
	m_fRunTime = 0.f;
	m_bDashAttack = false;
	m_bStone = false;
	m_bSleep = false;
	m_bSwordEffect = true;
	m_bStoneEffect = true;
	m_eMonsterType = MT_NORMAL;

	m_eSwordAttack = SA_NONE;
	m_eSwordState = SA_NONE;

	m_bSwordAttack = false;
	m_fSwordAttackTime = 0.f;
	m_fSlideLimitTime = 0.f;
	m_fSlideLimitPixel = 0.f;

	m_bLineCollisionEnable = true;
	m_bTileCollisionEnable = true;
	m_bGravity = true;
	m_bAttackEnd = false;
	m_bJumpAttack = false;
	m_fAttackEndTime = 0.f;

	m_fDeathTime = 0.f;
	m_pMikeObject = nullptr;

	m_iHPMax = 6;
	m_iHP = 6;

	m_eObjectType = OT_PLAYER;

	m_bAttackColliderEnable = false;

	m_iAttackedCount = 0;
}

CPlayer::CPlayer(const CPlayer & obj) :
	CCharacter(obj)
{
	m_bRun = false;
	m_iHPMax = 6;
	m_iHP = 6;
}

CPlayer::~CPlayer()
{
	SAFE_RELEASE(m_pMikeObject);

}




void CPlayer::SwordAniBulletCreate()
{
	CBullet* pBullet = nullptr;

	switch (m_eDir)
	{
	case SD_LEFT:
		pBullet = CObj::CreateCloneObject<CBullet>(SWORD_BULLET_ANI_LEFT, "SwordBulletAniL", "Default", m_pScene);
		pBullet->SetPos(GetPos() + Position(-100.f, 0.f));
		break;

	case SD_RIGHT:
		pBullet = CObj::CreateCloneObject<CBullet>(SWORD_BULLET_ANI_RIGHT, "SwordBulletAniR", "Default", m_pScene);
		pBullet->SetPos(GetPos() + Position(100.f, 0.f));
		break;
	}

	pBullet->SetPlayTime(0.4f);
	pBullet->SetMoveSpeed(1200.f);

}

void CPlayer::SwordBulletCreate()
{
	CBullet* pBullet = nullptr;


	switch (m_eDir)
	{
	case SD_LEFT:
		pBullet = CObj::CreateCloneObject<CBullet>(SWORD_BULLET_LEFT, "SwordBulletL", "Default", m_pScene);
		pBullet->SetPos(GetPos() + Position(-150.f, 20.f * sinf(m_fSwordAttackTime * 20.f)));


		pBullet->SetPlayTime(0.3f);
		pBullet->SetMoveSpeed(1000.f);

		break;

	case SD_RIGHT:
		pBullet = CObj::CreateCloneObject<CBullet>(SWORD_BULLET_RIGHT, "SwordBulletR", "Default", m_pScene);
		pBullet->SetPos(GetPos() + Position(150.f, 20.f * sinf(m_fSwordAttackTime * 20.f)));


		pBullet->SetPlayTime(0.3f);
		pBullet->SetMoveSpeed(1000.f);

		

		break;
	}

}

void CPlayer::KirbyConversionKey()
{
	if (!m_bJump && !m_bSlide && !m_bAttack && !m_bBalloon && !m_bBig && !m_bStop && m_vecMonsterList.empty())
		// 몬스터 vector가 빈 경우에만 처리한다.
	{
		if (KEYDOWN("Kirby"))
		{
			m_eType = KIRBY_NORMAL;
			KirbyConversion();

			m_eMikeAttack = MA_NONE;
			
		}

		else if (KEYDOWN("StoneKirby"))
		{
			m_eType = KIRBY_STONE;
			KirbyConversion();


		}

		else if (KEYDOWN("SwordKirby"))
		{
			m_eType = KIRBY_SWORD;
			KirbyConversion();

			
			m_eSwordAttack = SA_NONE;
			m_eSwordState = SA_NONE;
		}

		else if (KEYDOWN("MikeKirby"))
		{
			m_eType = KIRBY_MIKE;
			KirbyConversion();

			m_eMikeAttack = MA_NONE;
		}

		else if (KEYDOWN("SleepKirby"))
		{
			m_eType = KIRBY_SLEEP;
			KirbyConversion();

			m_bSleep = false;
			RunDisEnable();

		}
	}

}

void CPlayer::KirbyConversion()
{
	switch (m_eType)
	{
	case KIRBY_NORMAL:
	case KIRBY_MIKE:
	case KIRBY_SLEEP:
		

		m_pAnimation->ChangeClip(g_pPlayerAnimationName[m_eDir][PL_IDLE]);
		m_pAnimation->SetDefaultClip(g_pPlayerAnimationName[m_eDir][PL_IDLE]);

		break;

	case KIRBY_STONE:


		m_pAnimation->ChangeClip(g_pStoneAnimation[m_eDir][PL_IDLE]);
		m_pAnimation->SetDefaultClip(g_pStoneAnimation[m_eDir][PL_IDLE]);

		m_bStone = false;

		break;

	case KIRBY_SWORD:

		m_bJumpAttack = false;
		m_bDashAttack = false;
		SetColliderState("SwordJumpAttack", false);

		m_pAnimation->ChangeClip(g_pSwordAnimation[m_eDir][PL_IDLE]);
		m_pAnimation->SetDefaultClip(g_pSwordAnimation[m_eDir][PL_IDLE]);

		break;
	}

	m_iAttackedCount = 0;

	if (m_eType != KIRBY_NORMAL)
	{
		GET_SINGLE(CSoundManager)->Play("AbilityGet", CT_EFFECT);
		CObj::StarEffectCreate(m_tPos);
	}
}

void CPlayer::CreateClip()
{


	CreateAnimation("PlayerAnimation"); // 플레이어 애니메이션을 생성한다.


	// Big Jump left 
	m_pAnimation->AddClip(g_pPlayerAnimationName[SD_LEFT][PL_BIG_JUMP]); // 클립을 생성한다. 왼쪽 아이들.
	m_pAnimation->SetClipTexture(g_pPlayerAnimationName[SD_LEFT][PL_BIG_JUMP], "KirbyLeft", TEXT("KirbyLeft.bmp"));
	m_pAnimation->LoadClip("../Bin/Save/Kirby/KirbyBigJumpLeft.clip");

	// Big Jump Right 

	m_pAnimation->AddClip(g_pPlayerAnimationName[SD_RIGHT][PL_BIG_JUMP]);
	m_pAnimation->SetClipTexture(g_pPlayerAnimationName[SD_RIGHT][PL_BIG_JUMP], "KirbyRight", TEXT("KirbyRight.bmp"));
	m_pAnimation->LoadClip("../Bin/Save/Kirby/KirbyBigJumpRight.clip");



	// Jump left 
	m_pAnimation->AddClip(g_pPlayerAnimationName[SD_LEFT][PL_JUMP]); // 클립을 생성한다. 왼쪽 아이들.
	m_pAnimation->SetClipTexture(g_pPlayerAnimationName[SD_LEFT][PL_JUMP], "KirbyLeft", TEXT("KirbyLeft.bmp"));
	m_pAnimation->LoadClip("../Bin/Save/Kirby/KirbyJumpLeft.clip");

	// Jump Right 

	m_pAnimation->AddClip(g_pPlayerAnimationName[SD_RIGHT][PL_JUMP]);
	m_pAnimation->SetClipTexture(g_pPlayerAnimationName[SD_RIGHT][PL_JUMP], "KirbyRight", TEXT("KirbyRight.bmp"));
	m_pAnimation->LoadClip("../Bin/Save/Kirby/KirbyJumpRight.clip");






		// Idle left 
	m_pAnimation->AddClip(g_pPlayerAnimationName[SD_LEFT][PL_IDLE]); // 클립을 생성한다. 왼쪽 아이들.
	m_pAnimation->SetClipTexture(g_pPlayerAnimationName[SD_LEFT][PL_IDLE], "KirbyLeft", TEXT("KirbyLeft.bmp"));
	m_pAnimation->LoadClip("../Bin/Save/Kirby/KirbyIdleLeft.clip");

	// Idle Right 

	m_pAnimation->AddClip(g_pPlayerAnimationName[SD_RIGHT][PL_IDLE]);
	m_pAnimation->SetClipTexture(g_pPlayerAnimationName[SD_RIGHT][PL_IDLE], "KirbyRight", TEXT("KirbyRight.bmp"));
	m_pAnimation->LoadClip("../Bin/Save/Kirby/KirbyIdleRight.clip");


	// Down Left  

	m_pAnimation->AddClip(g_pPlayerAnimationName[SD_LEFT][PL_DOWN]);
	m_pAnimation->SetClipTexture(g_pPlayerAnimationName[SD_LEFT][PL_DOWN], "KirbyLeft", TEXT("KirbyLeft.bmp"));
	m_pAnimation->LoadClip("../Bin/Save/Kirby/KirbyDownLeft.clip");


	// Down Right 

	m_pAnimation->AddClip(g_pPlayerAnimationName[SD_RIGHT][PL_DOWN]);
	m_pAnimation->SetClipTexture(g_pPlayerAnimationName[SD_RIGHT][PL_DOWN], "KirbyRight", TEXT("KirbyRight.bmp"));
	m_pAnimation->LoadClip("../Bin/Save/Kirby/KirbyDownRight.clip");



	// Walk Left 

	m_pAnimation->AddClip(g_pPlayerAnimationName[SD_LEFT][PL_WALK]);
	m_pAnimation->SetClipTexture(g_pPlayerAnimationName[SD_LEFT][PL_WALK], "KirbyLeft", TEXT("KirbyLeft.bmp"));
	m_pAnimation->LoadClip("../Bin/Save/Kirby/KirbyWalkLeft.clip");


	// Walk Right 

	m_pAnimation->AddClip(g_pPlayerAnimationName[SD_RIGHT][PL_WALK]);
	m_pAnimation->SetClipTexture(g_pPlayerAnimationName[SD_RIGHT][PL_WALK], "KirbyRight", TEXT("KirbyRight.bmp"));
	m_pAnimation->LoadClip("../Bin/Save/Kirby/KirbyWalkRight.clip");


	// Fly UP left 
	m_pAnimation->AddClip(g_pPlayerAnimationName[SD_LEFT][PL_FLY_UP]);
	m_pAnimation->SetClipTexture(g_pPlayerAnimationName[SD_LEFT][PL_FLY_UP], "KirbyLeft", TEXT("KirbyLeft.bmp"));
	m_pAnimation->LoadClip("../Bin/Save/Kirby/KirbyFlyUpLeft.clip");

	// Fly Up Right 

	m_pAnimation->AddClip(g_pPlayerAnimationName[SD_RIGHT][PL_FLY_UP]);
	m_pAnimation->SetClipTexture(g_pPlayerAnimationName[SD_RIGHT][PL_FLY_UP], "KirbyRight", TEXT("KirbyRight.bmp"));
	m_pAnimation->LoadClip("../Bin/Save/Kirby/KirbyFlyUpRight.clip");


	// Run Left 

	m_pAnimation->AddClip(g_pPlayerAnimationName[SD_LEFT][PL_RUN]);
	m_pAnimation->SetClipTexture(g_pPlayerAnimationName[SD_LEFT][PL_RUN], "KirbyLeft", TEXT("KirbyLeft.bmp"));
	m_pAnimation->LoadClip("../Bin/Save/Kirby/KirbyRunLeft.clip");



	// Run Right 

	m_pAnimation->AddClip(g_pPlayerAnimationName[SD_RIGHT][PL_RUN]);
	m_pAnimation->SetClipTexture(g_pPlayerAnimationName[SD_RIGHT][PL_RUN], "KirbyRight", TEXT("KirbyRight.bmp"));
	m_pAnimation->LoadClip("../Bin/Save/Kirby/KirbyRunRight.clip");




	// Fly Left ㅇㅇ

	m_pAnimation->AddClip(g_pPlayerAnimationName[SD_LEFT][PL_FLY]);
	m_pAnimation->SetClipTexture(g_pPlayerAnimationName[SD_LEFT][PL_FLY], "KirbyLeft", TEXT("KirbyLeft.bmp"));
	m_pAnimation->LoadClip("../Bin/Save/Kirby/KirbyFlyLeft.clip");


	// Fly Right 

	m_pAnimation->AddClip(g_pPlayerAnimationName[SD_RIGHT][PL_FLY]);
	m_pAnimation->SetClipTexture(g_pPlayerAnimationName[SD_RIGHT][PL_FLY], "KirbyRight", TEXT("KirbyRight.bmp"));
	m_pAnimation->LoadClip("../Bin/Save/Kirby/KirbyFlyRight.clip");


	// Prevent Left 

	m_pAnimation->AddClip(g_pPlayerAnimationName[SD_LEFT][PL_PREVENT]);
	m_pAnimation->SetClipTexture(g_pPlayerAnimationName[SD_LEFT][PL_PREVENT], "KirbyLeft", TEXT("KirbyLeft.bmp"));
	m_pAnimation->LoadClip("../Bin/Save/Kirby/KirbyPreventLeft.clip");



	// Prevent Right

	m_pAnimation->AddClip(g_pPlayerAnimationName[SD_RIGHT][PL_PREVENT]);
	m_pAnimation->SetClipTexture(g_pPlayerAnimationName[SD_RIGHT][PL_PREVENT], "KirbyRight", TEXT("KirbyRight.bmp"));

	m_pAnimation->LoadClip("../Bin/Save/Kirby/KirbyPreventRight.clip");




	// Silde Left 

	m_pAnimation->AddClip(g_pPlayerAnimationName[SD_LEFT][PL_SLIDE]);
	m_pAnimation->SetClipTexture(g_pPlayerAnimationName[SD_LEFT][PL_SLIDE], "KirbyLeft", TEXT("KirbyLeft.bmp"));

	m_pAnimation->LoadClip("../Bin/Save/Kirby/KirbySlideLeft.clip");



	// Silde Right 

	m_pAnimation->AddClip(g_pPlayerAnimationName[SD_RIGHT][PL_SLIDE]);
	m_pAnimation->SetClipTexture(g_pPlayerAnimationName[SD_RIGHT][PL_SLIDE], "KirbyRight", TEXT("KirbyRight.bmp"));
	m_pAnimation->LoadClip("../Bin/Save/Kirby/KirbySlideRight.clip");






	// Attack Left .

	m_pAnimation->AddClip(g_pPlayerAnimationName[SD_LEFT][PL_ATTACK]);
	m_pAnimation->SetClipTexture(g_pPlayerAnimationName[SD_LEFT][PL_ATTACK], "KirbyLeft", TEXT("KirbyLeft.bmp"));
	m_pAnimation->LoadClip("../Bin/Save/Kirby/KirbyAttackLeft.clip");



	// Attack Right 

	m_pAnimation->AddClip(g_pPlayerAnimationName[SD_RIGHT][PL_ATTACK]);
	m_pAnimation->SetClipTexture(g_pPlayerAnimationName[SD_RIGHT][PL_ATTACK], "KirbyRight", TEXT("KirbyRight.bmp"));
	m_pAnimation->LoadClip("../Bin/Save/Kirby/KirbyAttackRight.clip");


	// Attack애니메이션을 보면, 처음에 빨아들이는 애니메이션이후에 공격 키를 계속 누르고 있는 경우에 입을 벌리고 있는 모션이 반복된다.
	// 이 부분의 예외처리가 필요하다.




	// Take Left 

	m_pAnimation->AddClip(g_pPlayerAnimationName[SD_LEFT][PL_TAKE]);
	m_pAnimation->SetClipTexture(g_pPlayerAnimationName[SD_LEFT][PL_TAKE], "KirbyLeft", TEXT("KirbyLeft.bmp"));
	m_pAnimation->LoadClip("../Bin/Save/Kirby/KirbyTakeLeft.clip");



	// Take Right 

	m_pAnimation->AddClip(g_pPlayerAnimationName[SD_RIGHT][PL_TAKE]);
	m_pAnimation->SetClipTexture(g_pPlayerAnimationName[SD_RIGHT][PL_TAKE], "KirbyRight", TEXT("KirbyRight.bmp"));
	m_pAnimation->LoadClip("../Bin/Save/Kirby/KirbyTakeRight.clip");






	// Vomit Left 

	m_pAnimation->AddClip(g_pPlayerAnimationName[SD_LEFT][PL_VOMIT]);
	m_pAnimation->SetClipTexture(g_pPlayerAnimationName[SD_LEFT][PL_VOMIT], "KirbyLeft", TEXT("KirbyLeft.bmp"));

	m_pAnimation->LoadClip("../Bin/Save/Kirby/KirbyVomitLeft.clip");



	// Vomit Right 

	m_pAnimation->AddClip(g_pPlayerAnimationName[SD_RIGHT][PL_VOMIT]);
	m_pAnimation->SetClipTexture(g_pPlayerAnimationName[SD_RIGHT][PL_VOMIT], "KirbyRight", TEXT("KirbyRight.bmp"));
	m_pAnimation->LoadClip("../Bin/Save/Kirby/KirbyVomitRight.clip");



	// Eat Left 

	m_pAnimation->AddClip(g_pPlayerAnimationName[SD_LEFT][PL_EAT]);
	m_pAnimation->SetClipTexture(g_pPlayerAnimationName[SD_LEFT][PL_EAT], "KirbyLeft", TEXT("KirbyLeft.bmp"));
	m_pAnimation->LoadClip("../Bin/Save/Kirby/KirbyEatLeft.clip");


	// Eat Right 

	m_pAnimation->AddClip(g_pPlayerAnimationName[SD_RIGHT][PL_EAT]);
	m_pAnimation->SetClipTexture(g_pPlayerAnimationName[SD_RIGHT][PL_EAT], "KirbyRight", TEXT("KirbyRight.bmp"));
	m_pAnimation->LoadClip("../Bin/Save/Kirby/KirbyEatRight.clip");





	// HIT Left 

	m_pAnimation->AddClip(g_pPlayerAnimationName[SD_LEFT][PL_HIT]);
	m_pAnimation->SetClipTexture(g_pPlayerAnimationName[SD_LEFT][PL_HIT], "KirbyLeft", TEXT("KirbyLeft.bmp"));

	m_pAnimation->LoadClip("../Bin/Save/Kirby/KirbyHitLeft.clip");


	// HIT Right 

	m_pAnimation->AddClip(g_pPlayerAnimationName[SD_RIGHT][PL_HIT]);
	m_pAnimation->SetClipTexture(g_pPlayerAnimationName[SD_RIGHT][PL_HIT], "KirbyRight", TEXT("KirbyRight.bmp"));

	m_pAnimation->LoadClip("../Bin/Save/Kirby/KirbyHitRight.clip");




	// BIG_WALK Left 

	m_pAnimation->AddClip(g_pPlayerAnimationName[SD_LEFT][PL_BIG_WALK]);
	m_pAnimation->SetClipTexture(g_pPlayerAnimationName[SD_LEFT][PL_BIG_WALK], "KirbyLeft", TEXT("KirbyLeft.bmp"));

	m_pAnimation->LoadClip("../Bin/Save/Kirby/KirbyBigWalkLeft.clip");


	// BIG_WALK Right 

	m_pAnimation->AddClip(g_pPlayerAnimationName[SD_RIGHT][PL_BIG_WALK]);
	m_pAnimation->SetClipTexture(g_pPlayerAnimationName[SD_RIGHT][PL_BIG_WALK], "KirbyRight", TEXT("KirbyRight.bmp"));
	m_pAnimation->LoadClip("../Bin/Save/Kirby/KirbyBigWalkRight.clip");




	// Big Hit Left 

	m_pAnimation->AddClip(g_pPlayerAnimationName[SD_LEFT][PL_BIG_HIT]);
	m_pAnimation->SetClipTexture(g_pPlayerAnimationName[SD_LEFT][PL_BIG_HIT], "KirbyLeft", TEXT("KirbyLeft.bmp"));
	m_pAnimation->LoadClip("../Bin/Save/Kirby/KirbyBigHitLeft.clip");


	// Big Hit Right 

	m_pAnimation->AddClip(g_pPlayerAnimationName[SD_RIGHT][PL_BIG_HIT]);
	m_pAnimation->SetClipTexture(g_pPlayerAnimationName[SD_RIGHT][PL_BIG_HIT], "KirbyRight", TEXT("KirbyRight.bmp"));
	m_pAnimation->LoadClip("../Bin/Save/Kirby/KirbyBigHitRight.clip");


	// Big Idle Left
	m_pAnimation->AddClip(g_pPlayerAnimationName[SD_LEFT][PL_BIG_IDLE]);
	m_pAnimation->SetClipTexture(g_pPlayerAnimationName[SD_LEFT][PL_BIG_IDLE], "KirbyLeft", TEXT("KirbyLeft.bmp"));
	m_pAnimation->LoadClip("../Bin/Save/Kirby/KirbyBigIdleLeft.clip");


	// Big Idle Right
	m_pAnimation->AddClip(g_pPlayerAnimationName[SD_RIGHT][PL_BIG_IDLE]);
	m_pAnimation->SetClipTexture(g_pPlayerAnimationName[SD_RIGHT][PL_BIG_IDLE], "KirbyRight", TEXT("KirbyRight.bmp"));
	m_pAnimation->LoadClip("../Bin/Save/Kirby/KirbyBigIdleRight.clip");




	// Big Run Left
	m_pAnimation->AddClip(g_pPlayerAnimationName[SD_LEFT][PL_BIG_RUN]);
	m_pAnimation->SetClipTexture(g_pPlayerAnimationName[SD_LEFT][PL_BIG_RUN], "KirbyLeft", TEXT("KirbyLeft.bmp"));
	m_pAnimation->LoadClip("../Bin/Save/Kirby/KirbyBigRunLeft.clip");


	// Big Run Right
	m_pAnimation->AddClip(g_pPlayerAnimationName[SD_RIGHT][PL_BIG_RUN]);
	m_pAnimation->SetClipTexture(g_pPlayerAnimationName[SD_RIGHT][PL_BIG_RUN], "KirbyRight", TEXT("KirbyRight.bmp"));
	m_pAnimation->LoadClip("../Bin/Save/Kirby/KirbyBigRunRight.clip");


	m_pAnimation->SetTextureColorKeyAll(72, 104, 112);


	EasyKirbySource();
	StoneKirbySource();
	SwordKirbySource();





}

void CPlayer::RunDisEnable()
{
	m_bRun = false;
	SetMoveSpeed(350.f);
}

void CPlayer::SleepEnable()
{
	if (m_bSleep)
		return;

	m_bStop = true;
	m_bSleep = true;
	m_fTime = 0.f;
	EasyKirbyChangeClip(E_SLEEP);
	KirbyDefaultClip(PL_IDLE);
	RunDisEnable();
}

void CPlayer::AddAttacked()
{
	// 이것이 호출되기 이전에 일반 커비가 아니라는 예외처리가 이루어 져야 한다.

	switch (m_eType)
	{
	case KIRBY_NORMAL:
	case KIRBY_MIKE:
	case KIRBY_SLEEP:
		return;

	case KIRBY_SWORD:
	case KIRBY_STONE:
		break;
	}

	++m_iAttackedCount;

	if (m_iAttackedCount >= 3) // 3
	{
		// 능력이 풀린다.

		ThrowAbility();

		// 일단 Idle로 셋팅이 되는데 이것은 나중에 고치도록 하자.
	}
}

void CPlayer::ThrowAbility()
{
	

		// 능력을 가지고 있는 상태라면 능력을 벗을 준비를 한다.
		// 아래에 먼저 깔기 위한 Blue Ball을 생성한다.


		// Blue Ball
		CEffect* pBlueBall = CObj::CreateCloneObject<CEffect>(BLUE_BALL, "BlueBall", "Default", m_pScene);


		CJumpStar* pJumpStar = nullptr;

		switch (m_eDir)
		{
		case SD_LEFT:
			pJumpStar = CObj::CreateCloneObject<CJumpStar>(JUMP_STAR_RIGHT, "JumpStar", "Default", m_pScene);
			pJumpStar->SetPos(GetPos() + Position(60.f, 0.f));
			break;

		case SD_RIGHT:
			pJumpStar = CObj::CreateCloneObject<CJumpStar>(JUMP_STAR_LEFT, "JumpStar", "Default", m_pScene);
			pJumpStar->SetPos(GetPos() + Position(-60.f, 0.f));

			break;
		}

		pBlueBall->SetBlend(0.5f, 100, 200, 150); // 영구 Blend 같이 Blend 파티.

		pBlueBall->SetTarget(pJumpStar);

		// 자신이 바라보고 있는 방향에 따라서 어느 위치로 나아가는지가 정해지며, 어떤 위치에서 생성이 되는지 정해지는 부분이다.\


		// 자신의 능력을 JumpStar에게 준다. 자신은 커비상태로 돌린다.

		switch (m_eType)
		{
		case KIRBY_STONE:
			pJumpStar->SetAbility(MT_STONE);
			break;

		case KIRBY_SWORD:
			pJumpStar->SetAbility(MT_SWORD);
			break;
		}

		// 이렇게 능력을 주면, 이후에 해당 별을 먹었을 때 이 능력이 담겨져 있는 것이다.
		// 먹는 것에 대한 처리는 먹는 경우에 처리가 된다.

		// 이후에 기본 커비로 바뀐다.
		m_eType = KIRBY_NORMAL;
		KirbyConversion();


	
}

void CPlayer::MonsterCollision(CObj * pObj)
{
	// 몬스터가 공격을 하는 경우.

	switch (pObj->GetObjectType())
	{

		case OT_MONSTER_ATTACK:
		case OT_NORMAL_MONSTER:
		case OT_BOSS_DDD:
		{
			SIDE_DIR eCollisionDir = SD_LEFT;

			// 몬스터와 충돌이 일어난 경우.
			SetBlend(1.f, 2);
			GET_SINGLE(CSoundManager)->Play("KirbyHit", CT_EFFECT);
			SetInputDisEnable(5.f);
			Damage(1);

			// 날라가야 하는 방향을 정의한다.
			if (pObj->GetPos().x < m_tPos.x)
				eCollisionDir = SD_RIGHT;

			// 점프를 한 상태라면 넉업.
			// 점프를 하지 않은 상태라면 넉백만.

			SetKnockBack(0.8f, 600.f, eCollisionDir);
			GET_SINGLE(CSoundManager)->Stop(CT_ATTACK);


			// 날라가야 하는 상황이라면, 점프나, Run상태, 다양한 상태들을 풀어주는 것이 맞다.
			// 근데 이걸 풀어주면 아마 중력에 의해서 값이 이상할 수도 있는데 이거는 파워값으로 조정해보자.

			// 애니메이션을 교체해준다.

			// 기본애니메이션 셋팅.
			switch (m_eType)
			{
			case KIRBY_NORMAL:
			case KIRBY_MIKE:
			case KIRBY_SLEEP:
				KirbyChangeClip(PL_HIT);
				KirbyDefaultClip(PL_IDLE);

				break;

			case KIRBY_STONE:
				StoneKirbyChangeClip(ST_HIT);
				StoneKirbyDefaultClip(ST_IDLE);

				break;

			case KIRBY_SWORD:
				SwordKirbyChangeClip(SW_HIT);
				SwordKirbyDefaultClip(SW_IDLE);

				m_eSwordState = SA_NONE;
				m_eSwordAttack = SA_NONE;

				m_bSwordAttack = false;
				m_bStop = false;
				m_bAttack = false;
				// 공격을 맞았다면 콤보가 끊겨야 한다.

				break;
			}


			m_bTake = false;


			switch (m_eType)
			{
			case KIRBY_NORMAL:

				// Big 상태에서 맞더라도 먹은 것이 사라지지는 않는다. (원작)
				// 나는 사라지도록 할 것이다.

				if (m_bBig)
				{
					m_bBig = false;
				}
				

				if (m_bJump)
				{
					SetKnockUp(0.8f, 300.f);
					SetKnockBack(0.8f, 700.f, eCollisionDir);

					// Star
				}



				else
				{


					if (m_bBalloon)
					{
						SetKnockUp(0.8f, 150.f);
						InputEnable();

						KirbyChangeClip(PL_BIG_HIT);
						KirbyDefaultClip(PL_FLY);

					}
					else if (m_bJump)
					{
						SetKnockUp(0.8f, 300.f);
						SetKnockBack(0.8f, 700.f, eCollisionDir);


					}


				}


				SetColliderState("KirbyAttackRight", false);
				SetColliderState("KirbyAttackLeft", false);
				m_bAttackColliderEnable = false;
				m_bAttackEnd = true;
				m_fAttackEndTime = 0.f;
				m_fAttackEndLimitTime = 0.5f;
				DisEnableTornadoEffect();


				break;

			case KIRBY_MIKE:

				if (m_bJump)
				{
					SetKnockUp(0.8f, 300.f);
					SetKnockBack(0.8f, 700.f, eCollisionDir);

				}

				break;


			case KIRBY_SLEEP: // 아마 무적 상태인 걸로 알음.
				break;

			case KIRBY_STONE:


				if (m_bBalloon)
				{
					SetKnockUp(0.8f, 150.f, 0.5f);
					InputEnable();

					StoneKirbyChangeClip(ST_BIG_HIT);
					StoneKirbyDefaultClip(ST_FLY);

				}
				else if (m_bJump)
				{
					SetKnockUp(0.8f, 300.f);
					SetKnockBack(0.8f, 700.f, eCollisionDir);


				}

				break;

			case KIRBY_SWORD:

				if (m_bBalloon)
				{
					SetKnockUp(0.8f, 150.f);
					InputEnable();

					SwordKirbyChangeClip(SW_BIG_HIT);
					SwordKirbyDefaultClip(SW_FLY);

				}
				else if (m_bJump)
				{
					SetKnockUp(0.8f, 300.f);
					SetKnockBack(0.8f, 700.f, eCollisionDir);


				}


				break;
			}

			RunDisEnable();
			JumpDisEnable();
			m_bAttack = false;

			// 누르고 있다가, 맞는 경우 바로 처리를 끊어버려야 한다.
			m_bStop = false; // 맞은 이후에 Stop = false인 상태여야 한다.
			

		}
		break;
	}

}

void CPlayer::ChangeIdleClip(KIRBY_TYPE eType)
{
	switch (eType)
	{
	case KIRBY_NORMAL:
		KirbyChangeClip(PL_IDLE);
		break;

	case KIRBY_STONE:
		StoneKirbyChangeClip(ST_IDLE);
		break;

	case KIRBY_SWORD:
		SwordKirbyChangeClip(SW_IDLE);
		break;
	}
}

void CPlayer::CreateTonadoEffect()
{
	// 기존에 있던 것이 있으면 죽이고, .. 이후에 아라서 삭제.
	if (m_pTornadoEffect)
		return;

	// 새로운 이펙트를 생성하여 받아둔다.

	m_pTornadoEffect = nullptr;

	switch (m_eDir)
	{
	case SD_LEFT:
		m_pTornadoEffect = CObj::CreateCloneObject<CEffect>(ATTACK_LEFT, "Tornado", "Default", m_pScene);
		m_pTornadoEffect->SetRelative(-250.f, 0.f);

		break;

	case SD_RIGHT:
		m_pTornadoEffect = CObj::CreateCloneObject<CEffect>(ATTACK_RIGHT, "Tornado", "Default", m_pScene);
		m_pTornadoEffect->SetRelative(250.f, 0.f);

		break;
	}

	m_pTornadoEffect->SetTarget(this);
	// 빨아들이는 오브젝트의 경우에는 플레이어를 따라다닌다.
	// Offset은 미리 설정해둔 값이 들어간다.
}

void CPlayer::DisEnableTornadoEffect()
{
	if (m_pTornadoEffect)
	{
		m_pTornadoEffect->SetActive(false);
		m_pTornadoEffect = nullptr;
	}
}

void CPlayer::CreateBreathEffect()
{
	CEffect* pEffect = nullptr;




	switch (m_eDir)
	{
	case SD_LEFT:
		pEffect = CObj::CreateCloneObject<CEffect>(BREATH_LEFT, "Breath", "Default", m_pScene);
		pEffect->SetPos(GetPos() + Position(-100.f, 0.f));

		if (m_bFalling)
			pEffect->SetPos(GetPos() + Position(-100.f, 30.f));

		break;

	case SD_RIGHT:
		pEffect = CObj::CreateCloneObject<CEffect>(BREATH_RIGHT, "Breath", "Default", m_pScene);
		pEffect->SetPos(GetPos() + Position(100.f, 0.f));

		if (m_bFalling)
			pEffect->SetPos(GetPos() + Position(100.f, 30.f));

		break;
	}


	

	// Falling 상태인 경우에는 위에서 입김이 생성된 경우이고,
	// 아니라면 바닥에서 입김이 생성된 경우를 말한다.

	// 일정 시간의 텀을 줘서 동작을 막는다.
	m_bStop = true;
	m_bBreath = true;
	m_fBreathTime = 0.f;

	// 입김을 버리는 경우에는 breath로 stop처리를 할 것이다. 따라서, vomit을 false로 만든다.
	m_bVomit = false;

	switch (m_eType)
	{
	case KIRBY_NORMAL:
	case KIRBY_MIKE:
	case KIRBY_SLEEP:
		// 입김이 생성이 된 이후에 공격의 대기시간을 줘서 바로 공격을 시작할 수 없도록 한다.
		m_fAttackEndLimitTime = 0.4f;
		break;

	case KIRBY_STONE:

	
		m_fAttackEndLimitTime = 0.1f;
		break;

	case KIRBY_SWORD:
		break;
	}

	m_bAttackEnd = true;
	m_fAttackEndTime = 0.f;

	GET_SINGLE(CSoundManager)->Play("Breath", CT_EFFECT);
}

void CPlayer::BalloonStageCollision()
{
	// Ballon을 풀어둔 상태로. 
	// Falling을 둘 필요가 없는게, Falling의 경우에는 입김을 뱉고 떨어지는 경우에
	// 천천히 떨어지도록 하기 위함이다.
	// Falling은 생각하지 않아도 되고, 애니메이션만.

	
	switch (m_eType)
	{
	case KIRBY_MIKE:
	case KIRBY_SLEEP:
	case KIRBY_NORMAL:
		KirbyChangeClip(PL_VOMIT);
		KirbyDefaultClip(PL_IDLE);
		break;

	case KIRBY_STONE:
		StoneKirbyChangeClip(ST_VOMIT);
		StoneKirbyDefaultClip(ST_IDLE);
		break;

	case KIRBY_SWORD:
		SwordKirbyChangeClip(SW_VOMIT);
		SwordKirbyDefaultClip(SW_IDLE);
		break;

	}

	m_bStop = true;
	m_bVomit = true;
	m_fVomitTime = 0.f;

	// 이 상태로 한번 입김 뱉어주고, idle모션으로 변한다.

	// 입김 생성.

	CreateBreathEffect();

	
}

void CPlayer::Start()
{
	CCharacter::Start();


	
	if (GET_SINGLE(CGameManager)->GetGameMode() == GM_ANIMATION_EDIT)
		return;


	CCollider* pCollider = FindCollider("KirbyBody");

	pCollider->SetCollisionCallback<CPlayer>(CS_BLOCK, &CPlayer::BodyCollisionStart, this);
	pCollider->SetCollisionCallback<CPlayer>(CS_OVERLAP, &CPlayer::BodyCollisionStay, this);
	pCollider->SetCollisionCallback<CPlayer>(CS_RELEASE, &CPlayer::BodyCollisionEnd, this);
	SAFE_RELEASE(pCollider);



	pCollider = FindCollider("KirbyAttackRight");

	pCollider->SetCollisionCallback<CPlayer>(CS_OVERLAP, &CPlayer::PlayerAttack, this);
	pCollider->SetCollisionCallback<CPlayer>(CS_RELEASE, &CPlayer::PlayerAttackRelease, this);
	SAFE_RELEASE(pCollider);



	pCollider = FindCollider("KirbyAttackLeft");
	pCollider->SetCollisionCallback<CPlayer>(CS_OVERLAP, &CPlayer::PlayerAttack, this);
	pCollider->SetCollisionCallback<CPlayer>(CS_RELEASE, &CPlayer::PlayerAttackRelease, this);
	SAFE_RELEASE(pCollider);


	pCollider = FindCollider("SwordJumpAttack");
	pCollider->SetCollisionCallback<CPlayer>(CS_BLOCK, &CPlayer::SwordJumpAttackColliderStart, this);
	SAFE_RELEASE(pCollider);




	m_pInputComponent->BindAction<CPlayer>("Jump", IE_PRESS, this, &CPlayer::Jump, "Jump");
	m_pInputComponent->BindAction<CPlayer>("Down", IE_HOLD, this, &CPlayer::Down, "Down");
	m_pInputComponent->BindAction<CPlayer>("Down", IE_UP, this, &CPlayer::DownUp, "DownUp");
	m_pInputComponent->BindAction<CPlayer>("Run", IE_DOUBLE, this, &CPlayer::Run, "Run");
	m_pInputComponent->BindAction<CPlayer>("Run", IE_UP, this, &CPlayer::MoveUp, "MoveUp");
	m_pInputComponent->BindAction<CPlayer>("Throw", IE_PRESS, this, &CPlayer::Throw, "Throw");
	m_pInputComponent->BindAction<CPlayer>("Prevent", IE_HOLD, this, &CPlayer::Prevent, "Prevent");
	m_pInputComponent->BindAction<CPlayer>("Prevent", IE_UP, this, &CPlayer::PreventUp, "PreventUp");
	m_pInputComponent->BindAction<CPlayer>("Attack", IE_HOLD, this, &CPlayer::Attack, "Attack");
	m_pInputComponent->BindAction<CPlayer>("Attack", IE_UP, this, &CPlayer::AttackUp, "AttackUp");

	// Stone
	m_pInputComponent->BindAction<CPlayer>("Attack", IE_PRESS, this, &CPlayer::StoneAttack, "StoneAttack");
	m_pInputComponent->BindAction<CPlayer>("Attack", IE_UP, this, &CPlayer::StoneAttackUp, "StoneAttackUp");

	// Sword
	m_pInputComponent->BindAction<CPlayer>("Attack", IE_PRESS, this, &CPlayer::SwordAttack, "SwordAttack");
	m_pInputComponent->BindAction<CPlayer>("Attack", IE_UP, this, &CPlayer::SwordAttackUp, "SwordAttackUp");

	// Mike 
	m_pInputComponent->BindAction<CPlayer>("Attack", IE_PRESS, this, &CPlayer::MikeAttack, "MikeAttack");

	// Sleep은 공격이 필요가 없다.

	m_pInputComponent->BindAxis<CPlayer>("Move", this, &CPlayer::MoveSide, "MoveSide");

}

bool CPlayer::Init()
{
	if (!CCharacter::Init())
		return false;


	CreateClip();




	m_pAnimation->SetDefaultClip(g_pPlayerAnimationName[m_eDir][PL_IDLE]);
	//m_pAnimation->ChangeClip(g_pPlayerAnimationName[m_eDir][PL_IDLE]);
	//m_pAnimation->GetClip()->tOffset.x = -14.f;
	//m_pAnimation->GetClip()->tOffset.y = -19.f;
	
	// DefaultClip은 return 애니메이션 이후에 돌아갈 클립을 미리 정해두는 개념이지.
	// 현재 클립을 바꾸는 함수가 아니다.
	// 즉, 현재 클립은 IdleLeft로 되어 있다. (처음에 넣어서 강제 셋팅)
	// 따라서 이것을 오른쪽으로 보이게 하고 싶다면, ChangeClip을 통해서 해당 클립을 설정하면 된다.

	KirbyChangeClip(PL_IDLE);

	m_fMoveSpeed = 350.f;



	// 기본 커비.
	SetPivot(0.5f, 0.5f);
	SetStaticSize(100.f, 100.f); // 크기가 변하는 것을 생각하여 잡아준다.
	// 편집모드에서는 함수를 등록하지 않는다. (처리가 되지 않도록)

	if (GET_SINGLE(CGameManager)->GetGameMode() == GM_ANIMATION_EDIT)
		return true;

		
	

		// 일단 노말키로 처리하고 복잡한 것을 하나씩 바꿔나가는 것이 조건 처리에 좋을 듯.


		

	/*
		GET_SINGLE(CInput)->AddAxisKey("Move", VK_LEFT, -1.f);
	GET_SINGLE(CInput)->AddAxisKey("Move", VK_RIGHT, 1.f); //
	
	GET_SINGLE(CInput)->AddActionKey("Down", VK_DOWN); //
	GET_SINGLE(CInput)->AddActionKey("Jump", 'S'); //

	GET_SINGLE(CInput)->AddActionKey("Run", VK_LEFT);//
	GET_SINGLE(CInput)->AddActionKey("Run", VK_RIGHT);
	// 보고 있는 방향으로 처리.

	GET_SINGLE(CInput)->AddActionKey("Throw", 'F');
	GET_SINGLE(CInput)->AddActionKey("Shield", 'D');
	//  자신의 능력 풀기
	// 혹은 헬퍼 해제.
	

	// 커비 공격
	GET_SINGLE(CInput)->AddActionKey("Attack", 'A');
	*/

	// 플레이어의 충돌체를 상대적인 위치에서 값을 쉽게 구할 수 있도록 Size값을 잡아둔다.
	// 근데 이 Size값은 현재 애니메이션에 따라 달라질 수 있는 값이기 때문에 고정된 값을 하나 준다.

	// 마이크 오브젝트를 생성한다.

		SAFE_RELEASE(m_pMikeObject);

	m_pMikeObject = new CMikeObject;

	if (!m_pMikeObject->Init())
		return false;




	Size tStaticSize = GetStaticSize();
	Position tLeftTop = Position(tStaticSize.x / -2.f, tStaticSize.y / -2.f);
	Position tRightBottom = Position(tStaticSize.x / 2.f, tStaticSize.y / 2.f);

	CColliderRect* pCollider = AddCollider<CColliderRect>("KirbyBody");

	pCollider->SetProfile("PlayerProfile");
	pCollider->SetChannel("Player");
	pCollider->SetRelativeInfo(tLeftTop.x, tLeftTop.y, tRightBottom.x, tRightBottom.y);
	SAFE_RELEASE(pCollider);





	// 오른쪽 공격범위
	pCollider = AddCollider<CColliderRect>("KirbyAttackRight");

	pCollider->SetProfile("PlayerProfile");
	pCollider->SetChannel("Player");
	pCollider->SetRelativeInfo(tRightBottom.x, -80.f, tRightBottom.x + 290.f, 80.f);
	pCollider->SetEnable(false);
	SAFE_RELEASE(pCollider);

	// 왼쪽 공격 범위
	pCollider = AddCollider<CColliderRect>("KirbyAttackLeft");

	pCollider->SetProfile("PlayerProfile");
	pCollider->SetChannel("Player");
	pCollider->SetRelativeInfo(tLeftTop.x - 290.f, -80.f, tLeftTop.x, 80.f);
	pCollider->SetEnable(false);
	SAFE_RELEASE(pCollider);

	// 세로 160 가로 290 좌우끝 기준.


	// Stone Kirby : 그냥 Body를 이용하면 되겠다.

	// SwordKirby 

	CColliderSphere* pSwordJumpAttack = CObj::AddCollider<CColliderSphere>("SwordJumpAttack");

	pSwordJumpAttack->SetRelativeInfo(0.f, 0.f, 120.f);
	// 플레이어의 중심위치를 기준으로 하여, 100 의 반지름을 가진 구체를 충돌체로 한다.
	pSwordJumpAttack->SetProfile("PlayerProfile");
	pSwordJumpAttack->SetChannel("Player");
	pSwordJumpAttack->SetEnable(false);

	SAFE_RELEASE(pSwordJumpAttack);


	return true;
}

void CPlayer::Update(float fTime)
{
	if (IsDie())
	{
		// 플레이어의 체력이 0인 경우.

		m_bBlend = false;
		// Blend가 적용이 되어있는 경우에 적용이 되지 않으므로 처리를 풀어버리고 새로 적용한다.

		SetBlend(1.f, 5, 0, 255);

		m_iHP = m_iHPMax;

		m_iAttackedCount = 0;

		m_tPos = ((CMainScene*)m_pScene)->GetStartPos();

		GET_SINGLE(CSoundManager)->Play("Die", CT_STATE);

		// 위치를 바꾼다.
		// 스테이지에 저장을 해두는 시스템 빨리 구현해 ㅜㅜ

		// 생략하고 일단은 .. 
		// 노래를 다시 시작한다.

		GET_SINGLE(CSoundManager)->Stop(CT_BGM);

		m_bKnockBack = false;
		m_bKnockUp = false;
		m_bJump = false;
		m_bBig = false;
		m_bAttack = false;
		m_bKnockUpEnd = false;


		m_bIsDie = true;
		
	}

	if (m_bIsDie && !GET_SINGLE(CSoundManager)->IsPlaying(CT_STATE))
	{
		// 죽은 이후로, Die노래가 꺼진 상태라면, 다시 노래를 틀도록 한다.
		m_bIsDie = false;

		GET_SINGLE(CSoundManager)->Play(m_pScene->GetBgmName(), CT_BGM);

	}


	CCharacter::Update(fTime);


	if (KEYPRESS("MoveLeft") && !m_bStop)
	{
		m_eDir = SD_LEFT;
	}

	else if (KEYPRESS("MoveRight") && !m_bStop)
	{
		m_eDir = SD_RIGHT;
	}


	// 달리는 이펙트 생성.
	// 칼잡이가 Dash공격하는 경우에는 생성하지 않는다.

	if (((m_bRun && !m_bJump) || m_bSlide) && !m_bDashAttack)
	{

		// 슬라이드의 경우에는 바닥에 붙어 있는 상황이라서 그냥 해주면 되고,
		// Run상태에서 Jump가 아닌 경우에만 처리한다.
		m_fRunTime += fTime;

		if (m_fRunTime >= 0.7f)
		{
			m_fRunTime -= 0.7f;
			CEffect* pEffect = nullptr;

			switch (m_eDir)
			{
			case SD_LEFT:
				pEffect = CObj::CreateCloneObject<CEffect>(RUN_LEFT, "RunEffectL", "Default", m_pScene);
				pEffect->SetRelative(250.f, 0.f);
				break;

			case SD_RIGHT:
				pEffect = CObj::CreateCloneObject<CEffect>(RUN_RIGHT, "RunEffectR", "Default", m_pScene);
				pEffect->SetRelative(-250.f, 0.f);

				break;
			}

			pEffect->SetTarget(this);

		}
	}

	// 커비 교체키.
	KirbyConversionKey();
	
	//if (m_bTake && m_bStop && !m_bBig)
	//{
	//	// 먹고서 뱉은 경우에는 false가 들어온다.
	//	// 여기서 true로 남아있다는 것은 먹고서 아직 남아있는 상태인 것이다.
	//	// 아직 뱉어 있는 상태로 Down과 Stop이 같이 눌렸다는 것은 Eat애니메이션을 호출하고 기다리는 순간이다.
	//	// 일정 시간 이후에 풀어준다.

	//	//  Big상태가 풀려있는데, Take와 관련이 되어 있다는 것은 기본 상태는 아니라는 것.
	//	// 즉, 뭔가를 먹었고 그 이후에 Big이 풀렸다(먹었다)
	//	// 이 상태에서 Stop이 true. 100 %네.


	//	m_fEatTime += fTime;

	////// 변함없는 부분. 상단.

	
		// 넉백, 넉업, 넉업 끝 처리가 없는 경우에만 처리한다.

		if (m_bJump)
		{
			// 점프중에 A 공격키를 누르고 있지 않다면 점프의 처리를 한다.
			m_fJumpTime += fTime;
			m_fPowerTime += fTime;

			if (m_fPowerTime >= 0.1f)
			{
				m_fPowerTime -= 0.1f;
				m_fPower -= 200.f;
			}

			Move(Position(0.f, -1.f), m_fPower, fTime);

			// Jump Update 세분화.
			switch (m_eType)
			{
			case KIRBY_NORMAL:
				JumpUpdate(fTime);
				break;

			case KIRBY_STONE:
				StoneJumpUpdate(fTime);

				break;

			case KIRBY_SWORD:
				SwordJumpUpdate(fTime);
				break;

			case KIRBY_MIKE:
				break;

			case KIRBY_SLEEP:
				break;
			}


			// 점프를 하면서 vomit을 처리해줘야 하는 경우가 있음.
			if (m_bVomit)
			{
				// 동일 별 뱉을 때 사용하고 안 씀.
				m_fVomitTime += fTime;

				if (m_fVomitTime >= 0.3f)
				{
					m_bStop = false;
					m_bVomit = false;
				}
			}

		}

		else if (m_bBalloon && !m_bKnockUp && !m_bKnockBack) // Jump와 Ballon은 따로 처리된다.
		{
			// Balloon은 동일.
			m_fJumpTime += fTime;
			m_fPowerTime += fTime;

			if (m_fPowerTime >= 0.1f)
			{
				m_fPowerTime -= 0.1f;
				m_fPower -= 120.f;
			}

			Move(Position(0.f, -1.f), m_fPower, fTime);




			// Ballon 상태에서는 GravityTime의 영향을 받아서는 안 된다.
			// 충접되는 중력은 아니지만, 기본적인 속도유지를 위해서  200.f더 밀어준다.
			m_fGravityTime = 0.f;


			Move(Position(0.f, 1.f), 700.f, fTime);


		}

		// 그냥 점프하다가 떨어진 경우에는 빠른 속도로 떨어지지만, 입김불고 Falling이 true인 상태라면 천천히 떨어져야 한다.
		else if (m_bFalling)
			Move(Position(0.f, -1.f), 400.f, fTime);

		// 동일.


		else if (m_bVomit)
		{
			// 동일 별 뱉을 때 사용하고 안 씀.
			m_fVomitTime += fTime;

			if (m_fVomitTime >= 0.3f)
			{
				m_bStop = false;
				m_bVomit = false;
			}
		}




		if (m_bEat && m_eType == KIRBY_NORMAL) // 일반 커비 전용.
		{
			// 삼킴이 일어난 경우에 삼키는 애니메이션 시간을 보장해주기 위해서 0.5s의 텀을 둔다.

			m_fEatTime += fTime;

			if (m_fEatTime >= 0.5f)
			{
				m_bEat = false;
				m_bStop = false;
			}
		}



		else if (m_bSlide)
		{
			// 동일.
			m_fSlideTime += fTime;
			m_fPowerTime += fTime;

			if (m_fPowerTime >= 0.1f)
			{
				m_fPowerTime -= 0.1f;
				m_fPower -= 70.f;
			}

			// 처음에는 파워값이 가장 큰 상태여서 빠른 속도로 이동.
			// 이후에는 점점 작아지고 속도의 영향을 받아서 적은 속도로 이동.

			if (m_fSlidePixel < m_fSlideLimitPixel)
			{
				switch (m_eDir)
				{
				case SD_LEFT:
					Move(Position(-1.f, 0.f), 700.f + m_fPower, fTime);
					break;

				case SD_RIGHT:
					Move(Position(1.f, 0.f), 700.f + m_fPower, fTime);

					break;
				}

				m_fSlidePixel += fTime * (700.f + m_fPower);

			}



			if (m_fSlideTime >= m_fSlideLimitTime)
			{
				m_bStop = false;
				m_bSlide = false;
				m_bDashAttack = false;

				RunDisEnable();
				ChangeDefaultClip();
			}

		}

		else if (m_bBreath)
		{
			// 동일.
			m_fBreathTime += fTime;

			if (m_fBreathTime >= 0.25f)
			{
				// 숨을 내뱉은 다음에는 일정시간 움직일 수 없도록 하여 입김을 난사하는 것을 막는다.
				m_bStop = false;
				m_bBreath = false;
			}

			if (m_bFalling && (KEYPRESS("MoveRight") || KEYPRESS("MoveLeft")))
			{
				// 입김을 불고서 떨어지고 있는 상태라면, 해당 방향을 향해서 살짝 움직여주자.
				// 그리고 누르지 않는 경우를 판단해주기 위해서, 어느 한 쪽이라도 누르고 있는 경우에만 처리.
				// 만약 바닥에 붙어서 Falling이 false인 경우에는 해당 처리를 하지 않는다.
				switch (m_eDir)
				{
				case SD_LEFT:
					Move(Position(-1.f, 0.f), 300.f, fTime);

					break;

				case SD_RIGHT:
					Move(Position(1.f, 0.f), 300.f, fTime);

					break;
				}
			}
		}


		else if ((m_eType == KIRBY_NORMAL) && m_bStop && !m_bVomit)  // 일반 커비 전용이다.
		{
			// 공격을 시작한 이후로 Take는 좀 다른 처리를 하기 위한 변수이다.
			m_fAttackTime += fTime;


			if (m_fAttackTime >= 0.3f) // 0.3s가 지나고
			{
				// 공격키를 뗀 상태라면, 공격키를 떼지 않은 상태라면, 계속 Stop = true
				if (!m_bAttack)
				{
					
					ChangeDefaultClip();

					// 공격이 끊어졌으므로 공격용 충돌체를 꺼버린다.
					SetColliderState("KirbyAttackRight", false);
					SetColliderState("KirbyAttackLeft", false);

					GET_SINGLE(CSoundManager)->Stop(CT_ATTACK);
					m_bAttackColliderEnable = false;
					m_bAttackEnd = true;
					m_fAttackEndTime = 0.f;
					m_fAttackEndLimitTime = 0.5f;
					m_bStop = false; // = 입력을 받음
					DisEnableTornadoEffect();


					m_fTime = 0.f;

					// 공격 모션이 끝나는 순간은 직접적으로 이곳에서 처리가 된다.
					// 강제시간이든, 계속 누르고 있는 상황이든.
				}
			}
		}

		
		
		if (m_bAttackEnd)
		{
			m_fAttackEndTime += fTime;

			if (m_fAttackEndTime >= m_fAttackEndLimitTime)
			{
				m_bAttackEnd = false;
			}
		}


	if (!m_bKnockUp && !m_bKnockBack && !m_bKnockUpEnd)
	{

		// 기본 커비는 위의 처리를 가지고 다 처리하고, 능력이 있는 커비들은 각각의 처리 함수에 따라서 처리한다.
			switch (m_eType)
			{
			case KIRBY_NORMAL:
				KirbyUpdate(fTime);
				break;
			case KIRBY_STONE:
				StoneUpdate(fTime);
				break;

			case KIRBY_SWORD:
				SwordUpdate(fTime);
				break;

			case KIRBY_MIKE:
				MikeUpdate(fTime);
				break;

			case KIRBY_SLEEP:
				SleepUpdate(fTime);
				break;
			}

	}


	// ALL
	
}

void CPlayer::LateUpdate(float fTime)
{
	CCharacter::LateUpdate(fTime);

	m_fStopTime += fTime;


}

void CPlayer::Collision(float fTime)
{
	CCharacter::Collision(fTime);

}

void CPlayer::PrevRender(HDC hDC, float fTime)
{
	CCharacter::PrevRender(hDC, fTime);

}

void CPlayer::Render(HDC hDC, float fTime)
{
	switch (m_eType)
	{
	case KIRBY_MIKE:
		if (m_pMikeObject->IsEnable())
			m_pMikeObject->Render(hDC, fTime);
		break;
	}

	CCharacter::Render(hDC, fTime);

	
}

void CPlayer::PostRender(HDC hDC, float fTime)
{
	CCharacter::PostRender(hDC, fTime);


}

CPlayer * CPlayer::Clone() const
{
	return new CPlayer(*this);
}

void CPlayer::AddClipName(vector<string>& vecClipName)
{
	// 플레이어 기본 SRC
	for (int j = 0; j < PL_END; ++j)
	{
		for (int i = 0; i < SD_END; ++i)
		{
			vecClipName.push_back(g_pPlayerAnimationName[i][j]);
		}
	}

	// Stone Src
	for (int j = 0; j < ST_END; ++j)
	{
		for (int i = 0; i < SD_END; ++i)
		{
			vecClipName.push_back(g_pStoneAnimation[i][j]);
		}
	}

	// Sword Src
	for (int j = 0; j < SW_END; ++j)
	{
		for (int i = 0; i < SD_END; ++i)
		{
			vecClipName.push_back(g_pSwordAnimation[i][j]);
		}
	}

	// EasyKirby
	for (int j = 0; j < E_END; ++j)
	{
		for (int i = 0; i < SD_END; ++i)
		{
			vecClipName.push_back(g_pEasyAnimation[i][j]);
		}
	}
}

void CPlayer::AddMonsterVector(CObj * pObj)
{
	list<CObj*>::iterator iter;
	list<CObj*>::iterator iterEnd = m_vecMonsterList.end();

	for (iter = m_vecMonsterList.begin(); iter != iterEnd; ++iter)
	{
		if ((*iter) == pObj)
			return;
	}

	// 여기 까지 내려왔다는 소리는 없다는 것.

	m_vecMonsterList.push_back(pObj);
	pObj->AddRef();

	// 2개를 먹은 경우 상황을 제어 하기 위해서 Take = true
	if (m_vecMonsterList.size() == 2)
		m_bTake = true;

	// 먹은 것에 대한 처리.
	pObj->SetLineCollisionEnable(false);
	pObj->SetTileCollisionEnable(false);
	pObj->SetGravity(false);

	switch (pObj->GetObjectType())
	{
	case OT_NORMAL_MONSTER:

		((CMonster*)pObj)->MoveDisEnable();

		break;

	case OT_JUMP_STAR:
		((CJumpStar*)pObj)->MoveDisEnable();

		break;

	case OT_MOVE_STAR:

		((CMoveStar*)pObj)->MoveDisEnable();

		break;
	}
}

void CPlayer::KirbyChangeClip(PLAYER_NORMAL_CLIP eClip)
{
	m_pAnimation->ChangeClip(g_pPlayerAnimationName[m_eDir][eClip]);

}

void CPlayer::KirbyDefaultClip(PLAYER_NORMAL_CLIP eClip)
{
	m_pAnimation->SetDefaultClip(g_pPlayerAnimationName[m_eDir][eClip]);

}

void CPlayer::StoneKirbyChangeClip(STONE_CLIP eClip)
{
	m_pAnimation->ChangeClip(g_pStoneAnimation[m_eDir][eClip]);

}

void CPlayer::StoneKirbyDefaultClip(STONE_CLIP eClip)
{
	m_pAnimation->SetDefaultClip(g_pStoneAnimation[m_eDir][eClip]);

}

void CPlayer::SwordKirbyChangeClip(SWORD_CLIP eClip)
{
	m_pAnimation->ChangeClip(g_pSwordAnimation[m_eDir][eClip]);

}

void CPlayer::SwordKirbyDefaultClip(SWORD_CLIP eClip)
{
	m_pAnimation->SetDefaultClip(g_pSwordAnimation[m_eDir][eClip]);

}

void CPlayer::EasyKirbyChangeClip(EASY_CLIP eClip)
{
	m_pAnimation->ChangeClip(g_pEasyAnimation[m_eDir][eClip]);
}

void CPlayer::EasyKirbyDefaultClip(EASY_CLIP eClip)
{
	m_pAnimation->SetDefaultClip(g_pEasyAnimation[m_eDir][eClip]);
}


void CPlayer::MoveSide(float fScale, float fTime)
{
	if (m_bStop || m_bPrevent || KEYPRESS("MoveDown") || m_bKnockBack || m_bKnockUp || m_bInputDisEnable)
		return;

	

	if (KEYPRESS("MoveLeft") && KEYPRESS("MoveRight") && !m_bJump && !m_bBalloon && !m_bFalling && !m_bKnockUpEnd)
	{
		if (m_eType == KIRBY_SWORD )
		{
			if(!m_bJumpAttack)
				ChangeDefaultClip();

			// 스워드 커비이고, 점프 애니메이션이 돌아가고 있지 않는 경우에만 애니메이션을 바꾼다.

		}
		else
		{
			ChangeDefaultClip();
		}

		RunDisEnable();
		return;
	}



	Move(Position(1.f, 0.f), fTime * fScale);


	m_fStopTime = 0.f;
	m_bMove = true;
	m_bDown = false;
	// 움직이는 동작을 하고 있는 상황에서는 절대 Down일 수 없다.

	if (!m_bJump && !m_bBalloon && !m_bFalling && !m_bKnockUpEnd)
	{
		// 점프중에는 움직일 수 있지만, 모션이 바뀌지는 않는다.

		if (!m_bBig)
		{
			// Big상태인 경우를 나눠서 처리한다.

			switch (m_eType)
			{
			case KIRBY_MIKE:
			case KIRBY_SLEEP:
			case KIRBY_NORMAL:
				if (!m_bRun)
					KirbyChangeClip(PL_WALK);
				else
					KirbyChangeClip(PL_RUN);


				KirbyDefaultClip(PL_IDLE);

				break;

			case KIRBY_STONE:
				if (!m_bRun)
					StoneKirbyChangeClip(ST_WALK);
				else
					StoneKirbyChangeClip(ST_RUN);


				StoneKirbyDefaultClip(ST_IDLE);
				break;

			case KIRBY_SWORD:

				// 스워드 커비가 점프공격을 하고 있는 상태이고, 그 상태로 움직이는 경우에는 STopTime을 건들이지 않는다.

				
				/*if (m_bAttackEnd)
					return;*/
				// AttackEnd 의 경우에는 점프 이후에 해당 FALG가 켜진 경우이다.
				// 다른 경우에서는 중첩되는 시스템이기 때문에 해당 키입력과는 상관이 없다.
				// 아무튼, 점프 공격이 끝난 이후 몇 초까지는 처리를 막는다.
				
			// 스워드 커비가 점프공격을 하고 있는 경우라면 움직이는 처리만 진행한다.
				if (m_bJumpAttack)
					return;

				if (!m_bRun)
					SwordKirbyChangeClip(SW_WALK);
				else
					SwordKirbyChangeClip(SW_RUN);


				SwordKirbyDefaultClip(SW_IDLE);
				break;
			}

			
		}
		else if(m_eType == KIRBY_NORMAL)
		{
			// BIg 상태는 기본 커비만 존재함.
			if (!m_bRun)
				KirbyChangeClip(PL_BIG_WALK);
			else
				KirbyChangeClip(PL_BIG_RUN);


			KirbyDefaultClip(PL_BIG_IDLE);
		}
	}
}

void CPlayer::MoveUp(float fTime)
{
	if (m_bStop || m_bPrevent || KEYPRESS("MoveDown") || m_bJump || m_bBalloon || m_bKnockBack || m_bKnockUp || m_bKnockUpEnd)
		return;

	// 더블 클릭이후에 UP이 된 경우를 의미한다.

	switch (m_eType)
	{
	case KIRBY_NORMAL:
	case KIRBY_MIKE:
	case KIRBY_SLEEP:

		if (m_bRun)
		{
			RunDisEnable();

			ChangeDefaultClip();

		}
		else
		{
			ChangeDefaultClip();
		}


		break;

	case KIRBY_STONE:


		if (m_bRun)
		{
			RunDisEnable();

			ChangeDefaultClip();

		}
		else
		{
			ChangeDefaultClip();
		}


		break;

	case KIRBY_SWORD:



		if (m_bRun)
		{
			RunDisEnable();

			if (m_eSwordState != SA_JUMP_ATTACK)
				ChangeDefaultClip();

		}
		else if(m_eSwordState != SA_JUMP_ATTACK)
		{
			ChangeDefaultClip();
		}


		break;
	}
	
	
}

void CPlayer::Down(float fTime)
{
	if (m_bStop || m_bPrevent || m_bJump || m_bBalloon || m_bKnockUpEnd || m_bInputDisEnable)
		return;


	if (m_eType == KIRBY_NORMAL)
	{
		if (!m_bBig)
		{
			KirbyChangeClip(PL_DOWN);

			// 다운 상태로 만든다.
			m_bDown = true;

			RunDisEnable();
		}
		else // 커진 상태에서 아직 뱉지 않은 상태라면, (동의어 반복으로 들리는데 ?)
		{
			// Big상태로 다운을 누른 경우에는 먹은 것이다.
			m_bBig = false;

			

			/*
			(!m_bJump && !m_bSlide && !m_bAttack && !m_bBalloon && !m_bBig && !m_bStop 
			*/


			// 능력이 있는 몬스터를 먹었는지 확인하고 해당 몬스터를 먹었다면 Conversion

			if (m_eMonsterType != MT_NORMAL)
			{
				switch (m_eMonsterType)
				{
				case MT_MIKE:
					m_eType = KIRBY_MIKE;
					m_eMikeAttack = MA_NONE;

					break;

				case MT_SLEEP:
					m_eType = KIRBY_SLEEP;
					m_bSleep = false;
					RunDisEnable();
					break;

				case MT_STONE:
					m_eType = KIRBY_STONE;
					break;

				case MT_SWORD:
					m_eType = KIRBY_SWORD;

					m_eSwordAttack = SA_NONE;
					m_eSwordState = SA_NONE;
					break;
				}

				KirbyConversion();

				m_eMonsterType = MT_NORMAL;

				// 능력 있는 몬스터를 먹은 경우에는 아래의 처리를 하지 않는다.

			}
			else
			{
				// 일반 몬스터를 먹은 경우에는 기존의 커비대로 처리를 해준다.
				KirbyChangeClip(PL_EAT);
				// Eat Animation -> RETURN

				KirbyDefaultClip(PL_IDLE);


				m_bStop = true;

				// 가지고 있던 물체를 먹었다. 가지고 있지 않다.


				m_bEat = true;
				m_fEatTime = 0.f;

				GET_SINGLE(CSoundManager)->Play("Food", CT_EFFECT);
			}

			m_bTake = false;

		}


	}
	else
	{
		// 일반적인 커비가 아닌 경우.
		// 이 경우에는 Big상태에 해당하는 먹는 모션이 없다.
		// 따라서 그냥 Down으로 처리한다.


		switch (m_eType)
		{
		case KIRBY_MIKE:
		case KIRBY_SLEEP:
			KirbyChangeClip(PL_DOWN);
			break;

		case KIRBY_STONE:
			StoneKirbyChangeClip(ST_DOWN);
			break;

		case KIRBY_SWORD:

			if (m_bJumpAttack)
				return;

			SwordKirbyChangeClip(SW_DOWN);

			break;
		}

		m_bDown = true;

		RunDisEnable();

	}

	// 슬라이드
	if (KEYDOWN("Jump"))
	{
		RunDisEnable();
		m_bDown = false;
		m_bSlide = true;
		m_fSlideLimitTime = 1.f;
		m_fRunTime = 0.f;
		m_bStop = true;
		m_fSlideTime = 0.f;
		m_fSlidePixel = 0.f;
		m_fPower = 300.f;
		m_fSlideLimitPixel = 600.f;

		GET_SINGLE(CSoundManager)->Play("Slide", CT_EFFECT);


		switch (m_eType)
		{
		case KIRBY_SLEEP:
		case KIRBY_MIKE:
		case KIRBY_NORMAL:
			KirbyChangeClip(PL_SLIDE);

			KirbyDefaultClip(PL_IDLE);
			break;

		case KIRBY_STONE:
			StoneKirbyChangeClip(ST_SLIDE);

			StoneKirbyDefaultClip(ST_IDLE);
			break;

		case KIRBY_SWORD:
			SwordKirbyChangeClip(SW_SLIDE);

			SwordKirbyDefaultClip(SW_IDLE);
			break;
		}
	
	}
	
}

void CPlayer::DownUp(float fTime)
{
	if (m_bStop || m_bPrevent || m_bBig || m_bKnockUpEnd || m_bInputDisEnable)
		return;

	switch (m_eType)
	{
	case KIRBY_SWORD:
		if (m_bJumpAttack)
			return;

	default:
		break;

	}

	m_bDown = false;

	if(!m_bJump && !m_bBalloon)
		ChangeDefaultClip();

}

void CPlayer::Jump(float fTime)
{
	if (m_bStop || m_bPrevent || m_bDown || m_bKnockBack || m_bKnockUp || m_bInputDisEnable) // 이미 점프중.
		return;

	// 점프의 경우에도 DisEnable이면 처리를 하지 않는다. 다만, 풍선 상태로 날라가는 경우에는 이것을 처리하지 않으니 상관없다.
	// 그 경우는 예외로 두고 나머지는 그냥 맞고 떨어지도록 구성하는 것이다.(키를 입력할 수 없어서)

	// 점프를 처음 시작하는 경우에
	if (!m_bJump && !m_bBalloon && !m_bFalling && !m_bKnockUpEnd)
	{
		// Down중인 상태에서 Jump를 한다고 해서 진짜 Jump로 처리하면 안 된다.(Slide모션)


		// 여기 순서 바꿔서 점프 처리 안 되게 바꾸면 된다.
		// 스워드 점프 공격 상태에서 점프하면 풀리는 현상.

			switch (m_eType)
			{
			case KIRBY_NORMAL:
				if (!m_bBig)
				{
					KirbyChangeClip(PL_JUMP);
					KirbyDefaultClip(PL_IDLE);
				}
				else
				{
					KirbyChangeClip(PL_BIG_JUMP);
					KirbyDefaultClip(PL_BIG_IDLE);
				}
				break;

			case KIRBY_MIKE:
			case KIRBY_SLEEP:
				KirbyChangeClip(PL_JUMP);
				KirbyDefaultClip(PL_IDLE);
				break;

				// 능력이 있는 커비의 경우에는 Big상태가 없으므로 그냥 점프로 처리해준다.
			case KIRBY_STONE:
				StoneKirbyChangeClip(ST_JUMP);
				StoneKirbyDefaultClip(ST_IDLE);
				break;

			case KIRBY_SWORD:

				if (m_eSwordAttack == SA_JUMP_ATTACK)
					return;

				SwordKirbyChangeClip(SW_JUMP);
				SwordKirbyDefaultClip(SW_IDLE);
				break;
			}


		m_bJump = true;
		m_fJumpTime = 0.f;
		m_fPower = 2060.f;
		m_fPowerTime = 0.f;

		GET_SINGLE(CSoundManager)->Play("Jump", CT_EFFECT);


		
	}

	else 
	{
			// 점프를 한 이후에 올라가고 있는 경우. (다시 누름)을 인식하지 않도록 한다.
			if (m_bJump && !JumpDown())
				return;

			// 뭔가를 먹은 상태라면 날 수 없다. 기본커비일시.
			if (m_eType == KIRBY_NORMAL && m_bBig)
				return;

			// 넉백업end 이면서 풍선상태가 아닌 경우에 처리하지 않는다.
			if (m_bKnockUpEnd && !m_bBalloon)
				return;

			// FLY 상태를 기본으로 설정하고, FLYUP 애니메이션을 실행.

			switch (m_eType)
			{
			case KIRBY_NORMAL:
			case KIRBY_MIKE:
			case KIRBY_SLEEP:
				KirbyChangeClip(PL_FLY_UP);
				KirbyDefaultClip(PL_FLY);
				break;

			case KIRBY_STONE:
				StoneKirbyChangeClip(ST_FLY_UP);
				StoneKirbyDefaultClip(ST_FLY);
				break;

			case KIRBY_SWORD:

				if (m_eSwordAttack == SA_JUMP_ATTACK)
					return;

				SwordKirbyChangeClip(SW_FLY_UP);
				SwordKirbyDefaultClip(SW_FLY);
				break;
			}


			// 점프를 통해서 최대 높이까지 올라간 이후에 처리가 되도록 JumpDown을 이용한다.
			// 점프 도중에 다시 점프키를 누른 경우에 풍선 상태가 되어야 한다.
			m_bFalling = false;
			m_bJump = false;
			m_bBalloon = true;
			m_fJumpTime = 0.f;
			m_fPower = 1200.f;

			RunDisEnable();
			InputEnable();
			
			GET_SINGLE(CSoundManager)->Play("FlyUp", CT_EFFECT);
		

	}
}

void CPlayer::Run(float fTime)
{
	// 점프중에 Run을 시작할 수는 없다.
	
	if (m_bStop || m_bPrevent || m_bJump || m_bBalloon || m_bKnockBack || m_bKnockUp || m_bKnockUpEnd || m_bInputDisEnable)
		return;
	// 처음에 왼쪽 혹은 오른쪽을 눌렀을 때
	// 방향을 체크를 한다.
	// 여기서 두번 누른 키를 인식해주는 것이다.
	// 왼쪽을 누르고, 오른쪽을 2번 누른 경우. 오른쪽으로 설정 되어있다.
	// 오른쪽 누르고 왼쪽 2번 눌르면 왼쪽으로 설정.
	// 왼 오 왼 오 흐음 최근 것으로 아무튼 처리한다.

	m_bRun = true;
	m_fRunTime = 0.f;

	SetMoveSpeed(350.f + 200.f);

	GET_SINGLE(CSoundManager)->Play("RunStart", CT_EFFECT);


	switch (m_eType)
	{
	case KIRBY_NORMAL:
	case KIRBY_SLEEP:
	case KIRBY_MIKE:
		
		KirbyDefaultClip(PL_IDLE);

		break;

	case KIRBY_STONE:
		StoneKirbyDefaultClip(ST_IDLE);
		break;

	case KIRBY_SWORD:

		if (m_bJumpAttack)
			return;

		SwordKirbyDefaultClip(SW_IDLE);
		break;
	}

	// 만약에 두번 누르고 나서 처리를 하는데,
	// 두번 누른 처리 이후에 달리는 중이 아니다. 에 대한 처리를
	// 어떤 식으로 할지 생각해보자. !!
}

void CPlayer::Throw(float fTime)
{
		// F키를 누른 경우이다.
	

		switch (m_eType)
		{
		case KIRBY_NORMAL:
		case KIRBY_MIKE:
		case KIRBY_SLEEP:


			break;

		case KIRBY_STONE:
		case KIRBY_SWORD:


			if (!m_bJump && !m_bSlide && !m_bAttack && !m_bBalloon && !m_bBig && !m_bStop)
				ThrowAbility();

			break;


		}

	

}

void CPlayer::Prevent(float fTime)
{
	if (m_bStop || m_bBig || m_bJump || m_bBalloon || m_bKnockBack || m_bKnockUp || m_bKnockUpEnd || m_bInputDisEnable)
		
		// 점프중에 방어 불가.
		// 무언가를 먹고 있는 상태에서 방어를 할 수 없다.
		return;

	// 쉴드모드
	m_bPrevent = true;


	switch (m_eType)
	{
	case KIRBY_NORMAL:
	case KIRBY_MIKE:
	case KIRBY_SLEEP:
		KirbyChangeClip(PL_PREVENT);
		break;

	case KIRBY_STONE:
		StoneKirbyChangeClip(ST_PREVENT);
		break;

	case KIRBY_SWORD:
		SwordKirbyChangeClip(SW_PREVENT);
		break;
	}

	RunDisEnable();
}

void CPlayer::Attack(float fTime)
{
	// Normal Kirby 전용 함수.
	if (m_eType != KIRBY_NORMAL)
		return;

	// 공격이 끝난 시점을 기준으로 다시 공격을 사용하기 위한 시간을 준다.
	if (m_bPrevent || m_bVomit || m_bKnockBack || m_bKnockUp || m_bAttackEnd || m_bInputDisEnable)
		return;


	if (m_bBig && KEYDOWN("Attack") && !m_bKnockUpEnd)
	{

		// 점프를 하고 있으면서, 위로 위로 올라가고 있는 경우에는 처리하지 않는다.
		// 적어도 어느 정도 올라간 후에 뱉을 수 있도록 하는 것이다.
		if (m_bJump && !JumpDown())
			return;

		RunDisEnable();
		m_bTake = false;
		m_bBig = false;
		m_bVomit = true;
		m_fVomitTime = 0.f;
		//m_bFalling = true;

		// 뱉는 모션 (Stop) -> idle

		KirbyChangeClip(PL_VOMIT);
		KirbyDefaultClip(PL_IDLE);

		m_eMonsterType = MT_NORMAL;


		// 입김 시간처럼 잠시 정지를 준다.
		m_bStop = true;
		m_bBreath = true;
		m_fBreathTime = 0.f;


		// 해당 방향을 향해 움직이는 별을 생성한다.

	
		CObj* pEffect = nullptr;
		switch (m_eDir)
		{
		case  SD_RIGHT:
			pEffect = CObj::CreateCloneObject<CEffect>(STAR_RIGHT, "VomitStar", "Default", m_pScene);
			pEffect->SetPos(GetPos() + Position(100.f, 0.f));
			break;

		case SD_LEFT:
			pEffect = CObj::CreateCloneObject<CEffect>(STAR_LEFT, "VomitStar", "Default", m_pScene);
			pEffect->SetPos(GetPos() + Position(-100.f, 0.f));

			break;
		}

		GET_SINGLE(CSoundManager)->Play("StarBullet", CT_EFFECT);


		return;
	}
	else if (m_bBalloon && !m_bKnockUpEnd)
	{

		KirbyChangeClip(PL_VOMIT);
		KirbyDefaultClip(PL_IDLE);
		

		// 입김 생성.
		m_bFalling = true;
		m_bBalloon = false;
		m_bAttack = false;

		CreateBreathEffect();

		return;
	}
	

	// 누르는 중에 대한 처리. 위의 경우는 모두 return.

	if (m_bBig || m_bSlide || m_bKnockUpEnd)
		return;
	// Big에 대한 처리는 위에서 해주고 있다. (뱉는 것)
	// Big  상태로 아래에 대한 처리는 하지 않는다.
	// 빨아들이는 모션에 대한 처리를 하기 위한 전제조건은 기본커비.



	// Stop false라는 것은 처음 공격을 시작한 것이다.
	if (!m_bStop)
		m_fAttackTime = 0.f;




	m_bStop = true;
	m_bAttack = true;


	RunDisEnable();

	if (m_bJump)
		m_ePrevDir = m_eDir;

	
	KirbyChangeClip(PL_ATTACK);
	KirbyDefaultClip(PL_IDLE);

	// 플레이어가 바로보는 방향의 공격 충돌체를 활성화.
	switch (m_eDir)
	{
	case SD_LEFT:
		SetColliderState("KirbyAttackLeft", true);
		break;

	case SD_RIGHT:
		SetColliderState("KirbyAttackRight", true);
		break;
	}

	if (!GET_SINGLE(CSoundManager)->IsPlaying(CT_ATTACK))
	{
		GET_SINGLE(CSoundManager)->Play("KirbyAttack", CT_ATTACK);
		// 만약 돌고 있지 않은 경우에는.. 한번만 돌린다.
	}

	m_bAttackColliderEnable = true;

	CreateTonadoEffect();
}

void CPlayer::AttackUp(float fTime)
{
	if (m_eType != KIRBY_NORMAL)
		return;

	if (m_bPrevent || m_bKnockBack || m_bKnockUp || m_bInputDisEnable)
		return;

	// UP
	m_bAttack = false;

}

void CPlayer::JumpUpdate(float fTime)
{
	// 점프중에 A키를 누른 경우. 함수를 통해 받아놓은 위치를 가지고 일정 속도로 움직이게 한다.
		// 만약 처리를 하다가 점프가 끝난 경우(바닥에 닿은 순간) 점프가 아니므로 해당 처리는 종료된다.
		// Jump의 경우에는 방향키의 입력을 받지 않는다. 그래서 속도가 갑자기 줄어들게 된다.

	

	if (m_bKnockBack || m_bKnockUp || m_bKnockUpEnd)
		return;
	// 공격을 한 이후에 대기 시간이라면 공격키에 대한 처리를 막아야 한다.
	// 지금은 단순히 키를 눌렀는지에 대한 여부만 판단하기 때문이다.

	if (KEYPRESS("Attack") && !m_bBig && !m_bVomit)
	{
		// Big 상태인 경우에는 빨라지면 매우 이상하다.// 

		if (KEYPRESS("MoveLeft") || KEYPRESS("MoveRight"))
		{
			if (m_ePrevDir == SD_LEFT)
				Move(Position(-1.f, 0.f), 300.f, fTime);

			else
				Move(Position(1.f, 0.f), 300.f, fTime);
		}

		// 둘다 누르고 있는 경우에는 둘 중에 하나로 처리하고,
		// 누르지 않은 경우에는 그 자리에서.

	}
}

void CPlayer::KirbyUpdate(float fTime)
{

	if (m_bTake)
		m_fTakeTime += fTime;

	if (m_vecMonsterList.empty())
		return;

	m_fTime += fTime;

	list<CObj*>::iterator iter;
	list<CObj*>::iterator iterEnd = m_vecMonsterList.end();

	for (iter = m_vecMonsterList.begin(); iter != iterEnd; ++iter)
	{
		m_bStop = true;


		switch ((*iter)->GetObjectType())
		{
		case OT_NORMAL_MONSTER:
		case OT_ABILITY_MONSTER:
			((CMonster*)(*iter))->HitAnimation();
			break;

		case OT_JUMP_STAR:
		case OT_MOVE_STAR:
			((CStar*)(*iter))->SetMoveSpeed(0.f);
			break;

		}

		// destoy인 것도 있는데 일단은 생각하지 않는다.
		// 몬스터의 위치를 옮긴다.

		// 몬스터에서 커비까지의 각도를 구한다.
		float fAngle = CMath::GetAngle((*iter)->GetPos(), GetPos());
		// 두 물체 모두 Center를 기준으로 하고 있으므로 상관 없다.

		(*iter)->MoveAngle(fAngle, 700.f + (m_fTime * 1000.f), fTime);

		float fDist = CMath::GetDist((*iter)->GetPos(), GetPos());

		if (fDist < 50.f)
		{

			(*iter)->SetActive(false);


			KirbyChangeClip(PL_TAKE); // 섭취 애니메이션.
			KirbyDefaultClip(PL_BIG_IDLE); // 이후에 기본은 BIG IDLE

			switch ((*iter)->GetObjectType())
			{
				case OT_NORMAL_MONSTER:
				{
					// 먹은 것이 몬스터라면 먹은 몬스터의 타입에 따라서 처리.
					// 노말이든, 능력이든..

					CMonster* pMonster = (CMonster*)(*iter);
					// 이전에 먹은 것이 능력이 아니다. (일단 받아서 저장)
					if (m_eMonsterType == MT_NORMAL)
						m_eMonsterType = pMonster->GetMonsterType();
					else
					{
						//  이전에 먹은 것이 능력이다.
						// 현재 먹는 것에 따라서 다르게 처리한다.
						// 현재 먹은 것이 능력이라면 랜덤으로 능력을 주고, 
						// 현재 먹은 것이 능력이 아니라면 이번에 먹은 것은 넘긴다.
						if (pMonster->GetMonsterType() != MT_NORMAL)
						{
							// 랜덤.

							m_eMonsterType = (MONSTER_TYPE)(rand() % 4 + 1);
							// 1 ~ 4 능력중 랜덤으로 저장.
						}

						// 이전에 먹은 것이 능력이고, 현재 먹는 것이 능력이 아닌 것이라면
						// 덮어씌울 이유가 없다.
					}

					// 몬스터를 먹은 경우에는 해당 몬스터를 죽이고
					// 그리고 다음 것을 처리한다.
					// 만약에 거리가 되지 않은 경우에는 다음 몬스터를 처리한다.
					// 이렇게 하나하나씩 처리를 해보고서, 확인을 하는데
				}
				break;

				case OT_MOVE_STAR:
					break;

				case OT_JUMP_STAR:
				{

					CStar* pStar = (CStar*)(*iter);

					if (m_eMonsterType == MT_NORMAL)
						m_eMonsterType = pStar->GetAbilityType();
					else
					{
						//  이전에 먹은 것이 능력이다.
						// 현재 먹는 것에 따라서 다르게 처리한다.
						// 현재 먹은 것이 능력이라면 랜덤으로 능력을 주고, 
						// 현재 먹은 것이 능력이 아니라면 이번에 먹은 것은 넘긴다.
						if (pStar->GetAbilityType() != MT_NORMAL)
						{
							// 랜덤.

							m_eMonsterType = (MONSTER_TYPE)(rand() % 4 + 1);
							// 1 ~ 4 능력중 랜덤으로 저장.
						}

						// 이전에 먹은 것이 능력이고, 현재 먹는 것이 능력이 아닌 것이라면
						// 덮어씌울 이유가 없다.
					}
				}

				// Star도 가진 능력이 Normal일 수도 있고, Normal이 아닐 수 도 있다.

				break;

			}

			
			
		}
	}

	bool MonsterDie = true;

	for (iter = m_vecMonsterList.begin(); iter != iterEnd; ++iter)
	{
		if ((*iter)->IsActive())
		{
			MonsterDie = false;
			break;
		}

	}
	
	// 몬스터가 모두 죽은 경우에는 true상태이다.

	if (MonsterDie)
	{
		// 넣어둔 리스트에 해당하는 몬스터가 모두 죽은 상태인 경우에는 먹은 상태에 대해서 처리하고 List를 날린다.
		// 이렇게 처리를 하면, 흡수한 모든 몬스터 최대 2마리가 모두 커비가 먹은 상태가 되기 전 까지는 리스트에서 삭제하지 않아서
		// 여러개를 순차적으로 먹는 현상을 피할 수 있을 것이다.
		// 더군다나 레퍼런스 카운터를 이용하고 있어서 터지는 일도 없을 것.
		// 먹은 이후에 vomit 이면 잠깐 동안 막힘
		m_bBig = true;
		RunDisEnable();
		m_bStop = false;

		DisEnableTornadoEffect();

		Safe_Release_VecList(m_vecMonsterList);

		// 공격이 끊어졌으므로 공격용 충돌체를 꺼버린다.
		SetColliderState("KirbyAttackRight", false);
		SetColliderState("KirbyAttackLeft", false);

		GET_SINGLE(CSoundManager)->Stop(CT_ATTACK);

		m_bAttackColliderEnable = false;


		GET_SINGLE(CSoundManager)->Stop(CT_ATTACK);

		m_fTime = 0.f;
	}
}

void CPlayer::PreventUp(float fTime)
{
	// 일반 몬스터의 경우에는 true가 들어와서 여기서 막히는 것이고
	// 보스의 경우에는 false가 들어와서 이곳을 넘는다.
	if (m_bInputDisEnable)
	{
		if (m_bKnockBack || m_bKnockUp || m_bKnockUpEnd)
			return;
	}

	if (m_bStop || m_bBig || m_bJump || m_bBalloon)
		return;

	m_bPrevent = false;

	ChangeDefaultClip();

}

void CPlayer::StoneAttack(float fTime)
{
	if (m_eType != KIRBY_STONE)
		return;

	if (m_bAttackEnd || m_bPrevent || m_bSlide)
		return;

	if (m_bKnockBack || m_bKnockUp || m_bKnockUpEnd || m_bInputDisEnable)
		return;

	if (m_bBalloon)
	{

		StoneKirbyChangeClip(ST_VOMIT);
		StoneKirbyDefaultClip(ST_IDLE);


		// 입김 생성.
		m_bFalling = true;
		m_bBalloon = false;

		CreateBreathEffect();

		return;
	}

	// 풍선 상태가 아닌데, 공격키를 누른 경우.
	// 즉, 대시 공격, 점프 공격, 기본공격의 경우 돌이 된다. 돌!

	m_bStone = !m_bStone;
	RunDisEnable();


	if (m_bStone)
	{
		m_bAttack = true;
		m_fAttackTime = 0.f;
		// 돌이 되는 경우. 
		m_bStop = true;

		StoneKirbyChangeClip(ST_ATTACK);

		// 돌 상태가 된 이후에 모든 처리가 막히다가, 다시 공격키를 푼다면 idle모션이 되면서 움직일 수 있게 된다.

		CEffect* pEffect = CObj::CreateCloneObject<CEffect>(PINKSTONE, "PinkStone", "Default", m_pScene);
		pEffect->SetTarget(this);
		pEffect->SetRelative(0.f, 0.f);

		// 카메라를 1초 동안 흔든다.
		GET_SINGLE(CSceneManager)->SetSway(1.f);

		m_bStoneEffect = false;
	}

	// 0.3s 

	else if (m_fAttackTime >= 0.5f)
	{
		m_bAttack = false;

		m_bStop = false;
		// 돌이 되었다가 돌 상태가 풀린 경우.

		m_bJump = true;
		m_fPower = 1800.f;

		StoneKirbyChangeClip(ST_HIT);
		StoneKirbyDefaultClip(ST_IDLE);

		GET_SINGLE(CSceneManager)->SwayDisEnable();

		SetBlend(0.3f, 1); // 1초간 무적시간을 준다.
		// 만약에 켜져 있었다면 꺼버린다.

	}
	else
		m_bStone = true;

	// Stone = false인 상태가 된 이후에 (풀리는 상태) 공격시간을 확인해서 0.3s 이후라면
	// 해당 처리를 진행하고 이전이라면 진행하지 않는다.
	// 이때 Stone = false인 상태이므로 stone = true로 준다 (끝내는 처리를 하지 않았으니까)

	GET_SINGLE(CSceneManager)->LayerUp("Default", "Kirby");
}

void CPlayer::StoneAttackUp(float fTime)
{
	if (m_eType != KIRBY_STONE)
		return;

	if (m_bKnockBack || m_bKnockUp || m_bKnockUpEnd)
		return;

	if (m_bPrevent)
		return;



}

void CPlayer::StoneJumpUpdate(float fTime)
{
}

void CPlayer::StoneUpdate(float fTime)
{

	// 스테이지의 바닥과 충돌이 되지 않은 경우에만.
	if (m_bStone && !m_bStageCollision && !m_bLineCollision )
		Move(Position(0.f, 1.f), 500.f, fTime);

	if (m_bStone && !m_bStoneEffect && (m_bStageCollision || m_bLineCollision))
	{
		// 스톤 상태로, 이펙트가 false인 상태의 지형 충돌인 경우에 (즉, 스톤상태가 된 이후에
		// 지형과 충돌한 첫번째 경우를 의미한다.
		m_bStoneEffect = true;


		GET_SINGLE(CSoundManager)->Play("Stone1", CT_EFFECT);

		
	}

	if (m_bAttack)
	{
		m_fAttackTime += fTime;
	}
}

void CPlayer::SwordAttack(float fTime)
{
	if (m_eType != KIRBY_SWORD)
		return;

	if (m_bPrevent || m_bAttackEnd || m_bSlide)
		return;


	if (m_bKnockBack || m_bKnockUp || m_bKnockUpEnd || m_bInputDisEnable)
		return;

	if (m_bBalloon)
	{

		SwordKirbyChangeClip(SW_VOMIT);
		SwordKirbyDefaultClip(SW_IDLE);


		// 입김 생성.
		m_bFalling = true;
		m_bBalloon = false;

		CreateBreathEffect();

		return;
	}
	else if (m_bJump || m_bFalling)
	{
		// 점프 공격.

		// 현재의 공격 상태가 none이 아니라면 처리하지 않는다.
		if (m_eSwordState != SA_NONE)
			return;

		SwordKirbyChangeClip(SW_JUMP_ATTACK);
		SwordKirbyDefaultClip(SW_IDLE);

		// 점프 공격 충돌체를 활성화.
		SetColliderState("SwordJumpAttack", true);

		m_eSwordAttack = SA_JUMP_ATTACK;
		m_eSwordState = SA_JUMP_ATTACK;

		m_bJumpAttack = true;

		GET_SINGLE(CSoundManager)->Play("JumpAttack", CT_EFFECT);

		

		return;
	}
	else if (m_bRun)
	{
		// Dash 공격을 시작한 경우이다.

		SwordKirbyChangeClip(SW_DASH_ATTACK);
		SwordKirbyDefaultClip(SW_IDLE);

		// 대쉬는 솔직히 Slide와 처리가 다를 수가 없다. 그냥 같은 로직이용하면 된다.

		RunDisEnable(); 
		m_bDown = false;
		m_bSlide = true;
		m_bDashAttack = true;
		m_fSlideLimitTime = 1.f;
		m_fRunTime = 0.f;
		m_bStop = true;
		m_fSlideTime = 0.f;
		m_fSlidePixel = 0.f;
		m_fPower = 550.f;
		m_fSlideLimitPixel = 800.f;
		
		GET_SINGLE(CSoundManager)->Play("DashAttack", CT_EFFECT);

		return;

	}

	// 대시 공격이 진행중인 상태로 기본 공격을 할 수 없도록 한다(캔슬)
	if (m_bDashAttack)
		return;

	RunDisEnable();


	// 기본 공격에 대한 처리이다.

	/*m_bAttack = true;
	m_bStop = true;
	m_fStopTime = 0.f;
	m_fAttackTime = 0.f;*/

	m_bStop = true;

	switch (m_eSwordAttack)
	{
	case SA_NONE:
		m_eSwordAttack = SA_ONE;
		m_eSwordState = SA_ONE;

		SwordKirbyChangeClip(SW_ATTACK1);
		SwordKirbyDefaultClip(SW_IDLE);


		// Ani Bullet 이펙트 생성.
		SwordAniBulletCreate();

		GET_SINGLE(CSoundManager)->Play("Attack1", CT_EFFECT);

		break;

	case SA_ONE:
		m_eSwordAttack = SA_TWO;
		m_bSwordEffect = false;

		break;

	case SA_TWO:
		m_eSwordAttack = SA_THREE;
		m_bSwordAttack = true;
		m_fSwordAttackTime = 0.f;
		m_bAttack = true;
		m_fAttackTime = 0.f;
		

		break;
	}

	GET_SINGLE(CSceneManager)->LayerUp("Default", "Kirby");

}

void CPlayer::SwordAttackUp(float fTime)
{
	if (m_eType != KIRBY_SWORD)
		return;

	if (m_bPrevent)
		return;

}

void CPlayer::SwordJumpUpdate(float fTime)
{
}

void CPlayer::SwordUpdate(float fTime)
{

	if (m_bSwordAttack)
	{
		m_fSwordAttackTime += fTime;
		m_fAttackTime += fTime;
	}
	//if (m_bAttack)
	//{
	//	m_fAttackTime += fTime;

	//	if (m_fAttackTime >= 0.5f)
	//	{
	//		m_eSwordAttack = SA_NONE;

	//		// 콤보가 끊어진 경우.
	//		SwordKirbyChangeClip(SW_IDLE);
	//		SwordKirbyDefaultClip(SW_IDLE);
	//	}
	//}

	/*if (m_bStop)
	{
		m_fStopTime += fTime;

		if (m_fStopTime >= 0.3f)
			m_bStop = false;
	}*/

	// 애니메이션의 처리를 하는 공간이다. 예를 들어서, SA_ONE 상태라면 ONE 애니메이션까지 호출하라는 의미이고,
	// two라면 two까지 계속 호출해야 한다.

	if (m_eSwordAttack != SA_NONE)
	{
		// NONE 상태가 아니라는 소리는 일단 호출할 애니메이션이 존재한다는 의미이다.

		switch (m_eSwordState)
		{

			case SA_JUMP_ATTACK:
			{
				string strCurrent = m_pAnimation->GetCurrentAnimation();

				if (strCurrent == g_pSwordAnimation[SD_LEFT][SW_JUMP_ATTACK] ||
					strCurrent == g_pSwordAnimation[SD_RIGHT][SW_JUMP_ATTACK])
				{
					// 현재 해당 애니메이션을 돌리고 있다는 의미이다.
				}
				else
				{
					// 점프 공격 애니메이션이 끝난 상황이다.
					SetColliderState("SwordJumpAttack", false);
					// 충돌체 비활성화.

					m_eSwordAttack = SA_NONE;
					m_eSwordState = SA_NONE;

					//SetAttackEndTime(0.5f);

					m_bJumpAttack = false;
					m_bStop = false;
				}
			}
			break;

			case SA_ONE:
			{// 현재 ONE 애니메이션을 호출하고 있는 상태다.
				// 이 애니메이션이 끝난 경우에 다음 애니메이션으로 연결이 되야 하는지 확인하는 처리가 필요하다.

				string strCurrent = m_pAnimation->GetCurrentAnimation();

				if (strCurrent == g_pSwordAnimation[SD_LEFT][SW_ATTACK1] ||
					strCurrent == g_pSwordAnimation[SD_RIGHT][SW_ATTACK1])
				{
					// 현재 해당 애니메이션을 돌리고 있다는 의미이다.
				}
				else
				{
					// 현재 해당 애니메이션이 끝난 것이다.
					// 이 상황에서 호출해야하는 애니메이션이 SA_ONE을 넘는지 확인하면 된다.
					if (m_eSwordAttack > SA_ONE)
					{
						// 적어도 two까지는 애니메이션을 호출하라고 하는 상황인 것이다.
						// 그 애니메이션을 셋팅해준다.
						SwordKirbyChangeClip(SW_ATTACK2);
						SwordKirbyDefaultClip(SW_IDLE);

						m_eSwordState = SA_TWO;


					}
					else
					{
						// 현재 돌리고 있는 애니메이션이 끝났는데, 여기까지 호출을 하라는 상황인 것이다.
						ChangeDefaultClip();
						m_eSwordState = SA_NONE;
						m_eSwordAttack = SA_NONE;
						m_bStop = false;

					}
				}
			}
			break;

		case SA_TWO:
			{

				string strCurrent = m_pAnimation->GetCurrentAnimation();

				if (strCurrent == g_pSwordAnimation[SD_LEFT][SW_ATTACK2] ||
					strCurrent == g_pSwordAnimation[SD_RIGHT][SW_ATTACK2])
				{
					if (m_pAnimation->GetClip()->iFrame == 1 && !m_bSwordEffect)
					{
						GET_SINGLE(CSoundManager)->Play("Attack1", CT_EFFECT);
						m_bSwordEffect = true;
					}
				}
				else
				{
					if (m_eSwordAttack > SA_TWO)
					{
						SwordKirbyChangeClip(SW_ATTACK3);
						SwordKirbyDefaultClip(SW_IDLE);

						m_eSwordState = SA_THREE;


					}
					else
					{
						// 현재 돌리고 있는 애니메이션이 끝났는데, 여기까지 호출을 하라는 상황인 것이다.
						ChangeDefaultClip();
						m_eSwordAttack = SA_NONE;
						m_eSwordState = SA_NONE;

						m_bStop = false;

					}
				}
			}
			break;

		case SA_THREE:
			{

				m_fAttackTime += fTime;

				while (m_fAttackTime >= 0.2f)
				{
					m_fAttackTime -= 0.2f;
					SwordBulletCreate();

					GET_SINGLE(CSoundManager)->Play("Attack3", CT_EFFECT);

				}


				if(m_fSwordAttackTime >= 1.6f)
				{
						ChangeDefaultClip();
						m_eSwordAttack = SA_NONE;
						m_eSwordState = SA_NONE;

						m_bSwordAttack = false;
						m_bStop = false;
						m_bAttack = false;
				}
			}


			break;
		}
	}

}

void CPlayer::MikeAttack(float fTime)
{
	if (m_eType != KIRBY_MIKE)
		return;

	if (m_bAttackEnd || m_bPrevent || m_bVomit || m_bAttack) // 마이크의 경우에는 공격하고 있는 중에 다시 공격을 할 수 없다.
		return;


	if (m_bKnockBack || m_bKnockUp || m_bKnockUpEnd || m_bInputDisEnable)
		return;

	
	if (m_bBalloon)
	{

		KirbyChangeClip(PL_VOMIT);
		KirbyDefaultClip(PL_IDLE);


		// 입김 생성.
		m_bFalling = true;
		m_bBalloon = false;

		CreateBreathEffect();

		return;
	}


	// 누르는 중에 대한 처리. 위의 경우는 모두 return.

	if (m_bSlide)
		return;




	// 공격이 시작된 시점에서 false 를 주고...
	SetColliderState("KirbyBody", false);
	m_bStop = true;
	m_bAttack = true;
	m_fAttackTime = 0.f;
	m_bGravity = false;

	RunDisEnable();

	if (m_bJump)
		m_ePrevDir = m_eDir;

	switch (m_eMikeAttack)
	{
	case MA_NONE:
		// 처음 공격을 시작하는 경우이다.
		EasyKirbyChangeClip(E_MIKE1);

		m_eMikeAttack = MA_1;

		//m_pMikeObject->SetMikeObject(1.f, 2, 100, 50);
		m_pMikeObject->SetMikeObject(0.6f, 3, 120, 50);

		GET_SINGLE(CSoundManager)->Play("Sing1", CT_EFFECT);
		break;

	case MA_1:
		EasyKirbyChangeClip(E_MIKE2);

		m_eMikeAttack = MA_2;

		//m_pMikeObject->SetMikeObject(0.8f, 2, 100, 50);
		m_pMikeObject->SetMikeObject(0.6f, 3, 120, 50);

		GET_SINGLE(CSoundManager)->Play("Sing2", CT_EFFECT);


		break;

	case MA_2:
		EasyKirbyChangeClip(E_MIKE3);

		m_eMikeAttack = MA_END;

		m_pMikeObject->SetMikeObject(0.6f, 3, 120, 50);

		GET_SINGLE(CSoundManager)->Play("Sing3", CT_EFFECT);



		// KIRBY로 전환하는 과정이 필요하다.
		break;
	}

	// 마이크 공격의 특징 : 진동.
	GET_SINGLE(CSceneManager)->SetSway(2.0f);


	// 마이크 공격이 시작되므로, Kirby를 맨 위로 올려준다.
	GET_SINGLE(CSceneManager)->LayerUp("Default", "Kirby");

	
	// 마이크 오브젝트를 활성화 시킨다.

	// 마이크의 범위 (카메라)와 몬스터의 Rect를 충돌시켜, 충돌이라면 죽이는 처리를 하고 아닌 경우에는 그냥 넘긴다.
	GET_SINGLE(CSceneManager)->MikeAttackCollision();

	

	//E_MIKE1_LEFT,
	//	E_MIKE1_RIGHT,
	//	E_MIKE2_LEFT,
	//	E_MIKE2_RIGHT,
	//	E_MIKE3_LEFT,
	//	E_MIKE3_RIGHT,
	//	E_SLEEP_LEFT,
	//	E_SLEEP_RIGHT,
	//	E_END

	// 플레이어가 바로보는 방향의 공격 충돌체를 활성화. 마이크 충돌체 활성화 ?! 
	//switch (m_eDir)
	//{
	//case SD_LEFT:
	//	SetColliderState("KirbyAttackLeft", true);
	//	break;

	//case SD_RIGHT:
	//	SetColliderState("KirbyAttackRight", true);
	//	break;
	//}

	//CreateTonadoEffect(); 마이크 이펙트 생성. ?!
}

void CPlayer::MikeAttackUp(float fTime)
{
}

void CPlayer::MikeJumpUpdate(float fTime)
{
}

void CPlayer::MikeUpdate(float fTime)
{

	if (m_pMikeObject->IsEnable())
	{
		m_pMikeObject->Update(fTime);
	}

	// 공격을 하고 있는 경우에는 MikeUpdate를 호출해준다.


	if (m_bAttack)
	{
		// 공격을 시작했다면, 잠시 화면에서 멈추도록 한다.
		
		m_bJump = false;
		m_bRun = false;
		m_bFalling = false;

		

		// 기본적으로 스탑 상태이기 때문에 왠만한 동작은 못하도록 되어 있으며, 중력시간을 매 프레임마다 없애준다.
		// 당연히 점프가 on 상태라면 꺼주는 처리가 필요하다.

		m_fAttackTime += fTime;

		if (m_fAttackTime >= 2.2f)
		{
			// 공격이 끝난 시점에서 다시 충돌처리를 한다.
			// 그러면 중력이 다시 처리가 되는 시점에서 다시 충돌처리를 하는 것이다.
			SetColliderState("KirbyBody", true);
			m_bStop = false;
			m_bAttack = false;
			m_bGravity = true;
			// 공격을 시작한 이후로 충분한 시간이 지난 경우에 다시 공격 및, 다른 동작을 할 수 있도록 한다.
		}
	}

	else if (m_eMikeAttack == MA_END)
	{
		// To Kirby

		m_eType = KIRBY_NORMAL;

		
	}
}

void CPlayer::SleepAttack(float fTime)
{
}

void CPlayer::SleepAttackUp(float fTime)
{
}

void CPlayer::SleepJumpUpdate(float fTime)
{
}

void CPlayer::SleepUpdate(float fTime)
{
	if (m_bSleep)
	{
		m_fTime += fTime;

		if (m_fTime >= 7.f)
		{
			m_bStop = false;
			m_bSleep = false;
			m_eType = KIRBY_NORMAL;

			SetBlend(1.f, 1);

			// 체력 회복 효과.

			m_iHP = m_iHPMax;
			CObj::StarEffectCreate(GetPos());

			m_fTime = 0.f;

			GET_SINGLE(CSoundManager)->Play("GoodSleep", CT_EFFECT);
		}
	}
}


void CPlayer::PlayerAttack(CCollider * pSrc, CCollider * pDest, float fTime)
{

	// 해당 처리는 오직 기본 커비와 관련된 것이다.
	if (m_eType != KIRBY_NORMAL)
		return;

	else if (m_bVomit)
		return;


	if (m_bKnockBack || m_bKnockUp || m_bKnockUpEnd || m_bInputDisEnable)
		return;

	// Vomit 상태라면, 몬스터를 먹고서 뱉은 상태를 말한다.
	// 이 상태일 때 다른 오브젝트를 또 먹어대면 곤란하다..

	// 공격이 일어나면 Stop상태. 동시에 Prevent일 수는 없다. Take를 조건으로 걸어주는 순간
	// 한번 밖에 들어오지 못함.


	if (pDest->GetTag() == "Stage")
		return;

	else if (pDest->GetTag() == "StageLine")
		return;

	else if (m_bBig || m_bTake)
		return;

	CObj* pDestObj = pDest->GetObj();

	switch (pDestObj->GetObjectType())
	{
		case OT_MOVE_STAR:
		case OT_JUMP_STAR:
		case OT_NORMAL_MONSTER:
		{

			// 해당 함수가 호출된다는 것은 플레이어가 몬스터를 먹은 경우이다. 
			// 중복 없이 / 살아있는 경우에만.
			// 2개까지만 먹기 가능.
			if (pDestObj->IsActive() && m_vecMonsterList.size() < 3)
			{
				AddMonsterVector(pDestObj);

			}
		}

		// 보스는 먹을 수 없다.
		break;

		

	}

	

}

void CPlayer::PlayerAttackRelease(CCollider * pSrc, CCollider * pDest, float fTime)
{


	
}

void CPlayer::BodyCollisionStart(CCollider * pSrc, CCollider * pDest, float fTime)
{
	if (pDest->GetTag() == "Stage" || pDest->GetTag() == "StageLine")
		return;


	if (m_bKnockBack || m_bKnockUp || !m_vecMonsterList.empty()  || m_bKnockUpEnd)
		return;

	// 공격에 대한 처리를 할 것이기 때문에 이곳까지 들어오는 물체는 몬스터이다.

	CObj* pDestObj = pDest->GetObj();

	

	if (pDestObj->GetBlend())
	{
		pSrc->DeletePrevCollisionList(pDest);
		pDest->DeletePrevCollisionList(pSrc);
		GET_SINGLE(CCollisionManager)->DeletePrevCollisionAdj(pSrc->GetSerialNumber(), pDest->GetSerialNumber());
		return;
	}



	switch (pDestObj->GetObjectType())
	{
	case OT_NORMAL_MONSTER:
	case OT_BOSS_DDD:
		break;

	case OT_MOVE_STAR:
	case OT_JUMP_STAR:
	case OT_DOOR:
		// Star의 경우에는 해당 충돌체와 충돌처리를 하지 않는다.
		return;
		// 몬스터가 직접 충돌이 일어나는 경우는.... 아래에서 처리한다. 
		// 따라서 이곳에서는 처리하지 않는다.

	case OT_MONSTER_ATTACK:
		// 이곳에서 몬스터 총알에 대한 처리를 한다.
		

		// 플레이어가 몬스터의 공격을 막을 수 있는 경우.
		if (m_bPrevent || m_bSlide)
		{
			pDestObj->SetActive(false);
			CObj::StarEffectCreate(pDestObj->GetPos(), 1.f, 100.f);
			return;

		}

		switch (m_eType)
		{

		case KIRBY_NORMAL:

			// 이 경우를 따로 빼는 것으로 한다.

			//if (GetBlend())
			//	return;
			//// 무적이면 처리 X

			//if (m_bBig)
			//{
			//	m_bBig = false;
			//}

		

			break;


			// 자고 있는 상태로는 몬스터의 공격을 맞도록 하지 않는다.
		case KIRBY_SLEEP:

			pDestObj->SetActive(false);
			CObj::StarEffectCreate(pDestObj->GetPos(), 1.f, 100.f);

			return;


		case KIRBY_STONE:

			if (m_bStone)
			{
				pDestObj->SetActive(false);
				CObj::StarEffectCreate(pDestObj->GetPos(), 1.f, 100.f);
				return;

			}

			break;

		case KIRBY_SWORD:

			if (m_bJumpAttack)
			{
				pDestObj->SetActive(false);
				CObj::StarEffectCreate(pDestObj->GetPos(), 1.f, 100.f);

				return;
			}

			break;

		}

		// 모든 커비에 대하여 Prevent  상태이면 맞는 처리를 없애고, 스톤 커비에 대해서는 돌 상태라면 맞지 않도록 한다.

		// 커비의 각 상태에 따라서, 총알을 없애는 처리를 해야하는 경우에 대해서 처리해주고, 그 이후에는 맞아야 하는 상황에서 
		// 플레이어가 무적 상태인지 확인하고 무적 상태라면 처리를 하지 않는다.

		// 커비가 맞아야 하는 상황인데 커비가 무적 상태에 해당하는 경우에는 처리하지 않는다.
		// 참고로 몬스터와 직접 충돌이 일어나는 경우와 몬스터의 공격에 맞는 경우가 있는데,
		// 몬스터와 충돌이 일어나는 경우에는 미리 아래에서 Blend에 대한 처리를 해주고 있다.
		// 아예 조건을 분리했기 때문에 이곳에서 예외처리를 해줘야 한다.
		if (GetBlend())
			return;


		MonsterCollision(pDestObj);
		
		return;
	}




	// 몬스터가 무적이 아닌 경우..

	CMonster* pMonster = (CMonster*)pDestObj;



	// 커비가 공격하는 경우. 무적 상태

	// 모든 커비의 공통

	if (m_bPrevent)
	{

		switch (pDestObj->GetObjectType())
		{
			case OT_NORMAL_MONSTER:
			{
				SIDE_DIR eCollisionDir = SD_LEFT;

				if (m_tPos.x < pDestObj->GetPos().x)
					eCollisionDir = SD_RIGHT;



				// 몬스터에 대한 넉백처리가 필요할 것 같다. 
				// 날아다니는 물체와 날아다니지 않는 물체.


				if (pMonster->GetTag() == "Twizzy")
				{
					pMonster->SetKnockBack(1.f, 550.f, eCollisionDir);
					if (((CTwizzy*)pMonster)->GetJump())
					{
						// 중력이 없는 상태에서 점프를 하니까 슈퍼 점프가 된다.
						// 점프를 꺼버리고 떨어지도록 구성한다.
						((CTwizzy*)pMonster)->JumpDisEnable();
						pMonster->SetKnockUp(0.8f, 100.f, 0.f);

						pMonster->MoveDisEnable(1.f, eCollisionDir);

					}
				}
				else if (pDestObj->GetTag() == "TwizzyFly" || pDestObj->GetTag() == "Burt")
				{
					pMonster->SetKnockBack(0.8f, 300.f, eCollisionDir);
					pMonster->SetKnockUp(0.8f, 200.f, 0.f);
					pMonster->MoveDisEnable(1.f, eCollisionDir);

				}
				else
				{
					// Ability Monster 
					pMonster->SetKnockBack(0.8f, 450.f, eCollisionDir);
					pMonster->MoveDisEnable(1.f, eCollisionDir);
				}


			}
			
			// 기존에 하던 처리를 보스가 아닌 몬스터의 경우에만 처리를 한다.

			break;

			case OT_BOSS_DDD:
			{

				// 근데 이 처리를 하는 상황은 공격 상황에서 해버리면 안 된다. 즉,
				// 보스가 점프 공격을 하고 있는 상태에서 이것이 통하면 안 된다는 것이다.
				// 그때는 충돌이 나서 날라가는 처리를 해야한다.

				CDDD* pDDD = (CDDD*)pMonster;

				switch (pDDD->GetState())
				{
					case DS_IDLE:
					case DS_WALK:
					case DS_RUN:
					case DS_JUMP_ATTACK:
						// 이 경우에는 넉백 처리를 해야 한다. (데미지는 주지 않고 아래의 처리)
					{
						SIDE_DIR eCollisionDir = SD_LEFT;

						if (pMonster->GetPos().x < m_tPos.x)
							eCollisionDir = SD_RIGHT;

						// 보스가 오른쪽에 있는 경우 왼쪽으로 날라가야 한다.
						// 보스가 왼쪽에 있는 경우 오른쪽으로 날라가야 한다.


						SetKnockBack(0.8f, 300.f, eCollisionDir);
						// 이렇게 해서, 데미지를 먹고 끝.
					}
					break;


					case DS_ATTACK:

						// 역으로 공격을 먹는 상황이다.
						MonsterCollision(pMonster);

						m_bPrevent = false;

						return;
				
				}

			
			
				// 보스한테는 데미지도 안 먹힌다.
				
			}
			// 보스의 경우에는 날라가는 처리를 하지 않는다.
			// 보스가 날라가지 않고 커비가 날라간다.

			break;
		}
		

		pMonster->SetBlend(0.7f, 2);

		pMonster->Damage(1);



		return;
	}

	else if (m_bSlide && !m_bDashAttack) // Slide 기능을 같이 사용하고 있어서, 따로 경우를 처리해야 한다.
	{

		switch (pDestObj->GetObjectType())
		{
			case OT_NORMAL_MONSTER:
			{
				SIDE_DIR eCollisionDir = SD_LEFT;

				if (m_tPos.x < pDestObj->GetPos().x)
					eCollisionDir = SD_RIGHT;



				pMonster->SetKnockBack(0.8f, 1000.f, eCollisionDir);
				pMonster->MoveDisEnable(1.f, eCollisionDir);


				if (pMonster->GetTag() == "Grizoo")
				{
				}
				else if (pMonster->GetTag() == "Twizzy")
				{
					if (((CTwizzy*)pMonster)->GetJump())
					{
						// 중력이 없는 상태에서 점프를 하니까 슈퍼 점프가 된다.
						// 점프를 꺼버리고 떨어지도록 구성한다.
						((CTwizzy*)pMonster)->JumpDisEnable();
						/*	pDestObj->SetKnockUp(0.8f, 100.f, 0.f);

							((CMonster*)pDestObj)->MoveDisEnable(1.f, eCollisionDir);*/

					}
				}
				else if (pMonster->GetTag() == "TwizzyFly" || pMonster->GetTag() == "Burt")
				{
					//pDestObj->SetKnockBack(0.8f, 300.f, eCollisionDir);
					////pDestObj->SetKnockUp(0.8f, 200.f, 0.f);
					//((CMonster*)pDestObj)->MoveDisEnable(1.f, eCollisionDir);

				}



				pMonster->SetBlend(0.7f, 2);
				pMonster->Damage(2);


			}
			break;

		case OT_BOSS_DDD:
			// 슬라이드를 통해서 보스는 데미지를 입지 않는다.


			{


				CDDD* pDDD = (CDDD*)pMonster;

				switch (pDDD->GetState())
				{
					case DS_IDLE:
					case DS_WALK:
					case DS_RUN:

						// 이 경우에는 플레이어의 넉백처리를 하고 끝낸다.
						{
							SIDE_DIR eCollisionDir = SD_LEFT;

							if (pMonster->GetPos().x < m_tPos.x)
								eCollisionDir = SD_RIGHT;

							SetKnockBack(0.8f, 600.f, eCollisionDir);

							m_bSlide = false;
							m_bStop = false;
						}

						break;


					case DS_JUMP_ATTACK:
					case DS_ATTACK:
						// 점프 공격 중간에 Slide로 맞춘 경우이다.
						// Attack중에 Slide로 맞춘 경우. 
						// 충돌 처리를 해버린다.
						MonsterCollision(pMonster);
						m_bSlide = false;

						break;
				}
	
					
				

			}
			break;

			/*

			*/

		}

		

		return;
	}
	///// 

	switch (m_eType)
	{

	case KIRBY_NORMAL:

		


		break;

		case KIRBY_SLEEP: // 자고 있는 경우는 Stone 상태인 것 처럼 처리한다.
		{

			switch (pDestObj->GetObjectType())
			{
				case OT_NORMAL_MONSTER:
				{
					SIDE_DIR eCollisionDir = SD_LEFT;

					if (m_tPos.x < pDestObj->GetPos().x)
						eCollisionDir = SD_RIGHT;



					pMonster->SetKnockBack(0.8f, 400.f, eCollisionDir);
					pMonster->MoveDisEnable(1.f, eCollisionDir);
				}
				break;

			case OT_BOSS_DDD:
				break;

			}
			

			pMonster->SetBlend(0.7f, 2);
			pMonster->Damage(2);


			return;
		}
		break;

	case KIRBY_STONE:

		if (m_bStone)
		{
			SIDE_DIR eCollisionDir = SD_LEFT;

			if (m_tPos.x < pDestObj->GetPos().x)
				eCollisionDir = SD_RIGHT;

			switch (pDestObj->GetObjectType())
			{
				case OT_NORMAL_MONSTER:
				{
					// 몬스터의 넉백처리.

					pMonster->SetKnockBack(0.8f, 400.f, eCollisionDir);
					pMonster->MoveDisEnable(1.f, eCollisionDir);

					// 그냥 맞은 경우에 충돌.
					pMonster->SetBlend(0.7f, 2);
					pMonster->Damage(3);
				}
				break;

			case OT_BOSS_DDD:

				if (m_bJump)
				{
					// Stone 상태로 떨어지고 있는 경우에만 처리를 하도록 한다.
					// 그 외에 그냥 가만히 있는다고 해서 충돌처리가 되지는 않는다.
					pMonster->MoveDisEnable(1.f, eCollisionDir);
					pMonster->SetBlend(0.7f, 2);
					pMonster->Damage(3);
				}
				break;

			}
		

			


			return;
		}

		break;

	case KIRBY_SWORD:

		if (m_bDashAttack)
		{
			SIDE_DIR eCollisionDir = SD_LEFT;

			if (m_tPos.x < pDestObj->GetPos().x)
				eCollisionDir = SD_RIGHT;

			switch (pDestObj->GetObjectType())
			{
				case OT_NORMAL_MONSTER:
				{
					pMonster->SetKnockUp(0.8f, 150.f, 0.f);
					pMonster->SetKnockBack(0.8f, 400.f, eCollisionDir);
					pMonster->MoveDisEnable(1.f, eCollisionDir);
				}
				break;

			case OT_BOSS_DDD:
				pMonster->MoveDisEnable(1.f, eCollisionDir);

				break;

			}
		

			pMonster->SetBlend(0.7f, 2);
			pMonster->Damage(3);

			return;
		}
		else if (m_bJumpAttack)
		{

			switch (pDestObj->GetObjectType())
			{
			case OT_NORMAL_MONSTER:
				return;
				// 점프 공격인데, 일반  몬스터와 충돌이 난 경우에는 이곳에서 처리를 하지 않는다. ( 충돌이 나더라도 무적이다. )

			case OT_BOSS_DDD:

				// 점프 공격을 하다가, 중심과 보스가 충돌이 난 경우에는 충돌처리를 해버린다.
				m_bJumpAttack = false;
				MonsterCollision(pDestObj);

				return;


			}
		}
	

		break;
	}




	// 몬스터에 의해 날라가야 하는 경우.

	// 만약 커비가 무적시간이라면, 처리하지 않는다.
	if (m_bBlend)
	{
		pSrc->DeletePrevCollisionList(pDest);
		pDest->DeletePrevCollisionList(pSrc);
		GET_SINGLE(CCollisionManager)->DeletePrevCollisionAdj(pSrc->GetSerialNumber(), pDest->GetSerialNumber());

		return;
	}

	MonsterCollision(pDestObj);

	


}

void CPlayer::BodyCollisionStay(CCollider * pSrc, CCollider * pDest, float fTime)
{
	if (pDest->GetTag() == "Stage" || pDest->GetTag() == "StageLine")
		return;
	
	CObj* pDestObj = pDest->GetObj();

	switch (pDestObj->GetObjectType())
	{
	case OT_DOOR:

		GET_SINGLE(CSceneManager)->LayerUp("Default", "Kirby");

		if (!m_bAttack && !m_bStop && m_vecMonsterList.empty() && !m_bJumpAttack && !m_bDashAttack )
		{
			if (KEYPRESS("MoveUp"))
			{

				CDoor* pDoor = (CDoor*)pDestObj;
				// 위로 가는 키를 누른 경우에 다른 장면으로 전환을 해버린다.
				switch (pDoor->GetSceneType())
				{
					// TO Scene
				case ST_SCENE2:
					pDoor->Stage2Scene();
					break;

				case ST_SCENE3:
					pDoor->Stage3Scene();

					break;

				case ST_SCENE4:
					pDoor->Stage4Scene();

					break;

				case ST_SCENE5:
					pDoor->Stage5Scene();

					break;

				case ST_DDDSCENE:
					pDoor->DDDScene();
	
					break;

				case ST_ENDINGSCENE:
					pDoor->EndingScene();

					break;
				}

				GET_SINGLE(CSoundManager)->Play("Door", CT_EFFECT);

			}
		}

		break;
	}
}

void CPlayer::BodyCollisionEnd(CCollider * pSrc, CCollider * pDest, float fTime)
{
	if (pDest->GetTag() == "Stage" || pDest->GetTag() == "StageLine")
		return;
}

void CPlayer::SwordJumpAttackColliderStart(CCollider * pSrc, CCollider * pDest, float fTime)
{
	// 해당 충돌체 함수가 호출이 되었다는 소리는 스워드 커비로 점프 공격을 하고 있는 상황을 의미한다.
	
	CObj* pDestObj = pDest->GetObj();

	switch (pDestObj->GetObjectType())
	{
	case OT_NORMAL_MONSTER:
	case OT_BOSS_DDD:
		break;
		
	case OT_NONE:
	case OT_MOVE_STAR:
	case OT_JUMP_STAR:
	case OT_DOOR:
		return; // 할 필요가 없다.

	case OT_MONSTER_ATTACK:
		// 이곳에서 몬스터 총알에 대한 처리를 한다.
		pDestObj->SetActive(false);
		CObj::StarEffectCreate(pDestObj->GetPos(), 1.f, 100.f);

		return;
	}


	if (pDestObj->GetBlend())
	{
		// 해당 오브젝트가 무적 시간이라면...
		pSrc->DeletePrevCollisionList(pDest);
		pDest->DeletePrevCollisionList(pSrc);
		GET_SINGLE(CCollisionManager)->DeletePrevCollisionAdj(pSrc->GetSerialNumber(), pDest->GetSerialNumber());

		return;
	}


	SIDE_DIR eCollisionDir = SD_LEFT;

	if (m_tPos.x < pDestObj->GetPos().x)
		eCollisionDir = SD_RIGHT;

	switch (pDestObj->GetObjectType())
	{
		case OT_NORMAL_MONSTER:
		{


			pDestObj->SetKnockUp(0.8f, 300.f, 0.f);
			pDestObj->SetKnockBack(0.8f, 500.f, eCollisionDir);
			((CMonster*)pDestObj)->MoveDisEnable(1.f, eCollisionDir);

			pDestObj->SetBlend(0.7f, 2);
			((CMonster*)pDestObj)->Damage(2);

		}
		break;

		case OT_BOSS_DDD:
		{
			CDDD* pDDD = (CDDD*)pDestObj;

			switch (pDDD->GetState())
			{
			case DS_IDLE:
			case DS_WALK:
			case DS_RUN:
			{
				((CMonster*)pDestObj)->MoveDisEnable(1.f, eCollisionDir);

				pDestObj->SetBlend(0.7f, 2);
				((CMonster*)pDestObj)->Damage(2);

			}

			break;


			case DS_JUMP_ATTACK:
			case DS_ATTACK:

				// 처리를 하지 않는다. Hit
				// 무적은 키지 않으므로, 데미지가 들어가고 나서, 그 상태로 중심에 충돌이면 날라간다. 로 하려고 했는데
				// 무적 시간을 주지 않으면 바로 충돌이 나서 너무 쎄다.
				
				((CMonster*)pDestObj)->Damage(2);
				pDestObj->SetBlend(0.2f, 1);

				break;
			}

		

		}
		break;
	}

	
	GET_SINGLE(CSoundManager)->Play("JumpAttackHit", CT_EFFECT);

	// 몬스터와 충돌이 일어난 경우
	// 몬스터의 공격 or 몬스터가 들어오는 상황이다.
	
}

void CPlayer::SetAttackEndTime(float fTime)
{
	m_bAttackEnd = true;
	m_fAttackEndTime = 0.f;
	m_fAttackEndLimitTime = fTime;
}

void CPlayer::EasyKirbySource()
{
	/*
	E_MIKE1_LEFT,
	E_MIKE1_RIGHT,
	E_MIKE2_LEFT,
	E_MIKE2_RIGHT,
	E_MIKE3_LEFT,
	E_MIKE3_RIGHT,
	E_SLEEP_LEFT,
	E_SLEEP_RIGHT,
	E_END
};

const char* g_pEasyAnimation[E_END] =
{
	"Mike1Left", "Mike1Right", "Mike2Left", "Mike2Right", "Mike3Left", "Mike3Right",
	"SleepLeft", "SleepRight"
};


	*/

	// 리소스는 같이 사용하는 부분이므로, 텍스처 이름은 EasyKirbyLeft Right로 통일한다.
	// 소스 완성.


	// Mike 1 
	m_pAnimation->AddClip(g_pEasyAnimation[SD_LEFT][E_MIKE1]);
	m_pAnimation->SetClipTexture(g_pEasyAnimation[SD_LEFT][E_MIKE1], "EasyKirbyLeft", TEXT("EasyKirbyLeft.bmp"));

	m_pAnimation->AddClip(g_pEasyAnimation[SD_RIGHT][E_MIKE1]);
	m_pAnimation->SetClipTexture(g_pEasyAnimation[SD_RIGHT][E_MIKE1], "EasyKirbyRight", TEXT("EasyKirbyRight.bmp"));

	// Mike 2 

	m_pAnimation->AddClip(g_pEasyAnimation[SD_LEFT][E_MIKE2]);
	m_pAnimation->SetClipTexture(g_pEasyAnimation[SD_LEFT][E_MIKE2], "EasyKirbyLeft", TEXT("EasyKirbyLeft.bmp"));

	m_pAnimation->AddClip(g_pEasyAnimation[SD_RIGHT][E_MIKE2]);
	m_pAnimation->SetClipTexture(g_pEasyAnimation[SD_RIGHT][E_MIKE2], "EasyKirbyRight", TEXT("EasyKirbyRight.bmp"));


	// Mike 3 

	m_pAnimation->AddClip(g_pEasyAnimation[SD_LEFT][E_MIKE3]);
	m_pAnimation->SetClipTexture(g_pEasyAnimation[SD_LEFT][E_MIKE3], "EasyKirbyLeft", TEXT("EasyKirbyLeft.bmp"));

	m_pAnimation->AddClip(g_pEasyAnimation[SD_RIGHT][E_MIKE3]);
	m_pAnimation->SetClipTexture(g_pEasyAnimation[SD_RIGHT][E_MIKE3], "EasyKirbyRight", TEXT("EasyKirbyRight.bmp"));

	// Sleep 
	m_pAnimation->AddClip(g_pEasyAnimation[SD_LEFT][E_SLEEP]);
	m_pAnimation->SetClipTexture(g_pEasyAnimation[SD_LEFT][E_SLEEP], "EasyKirbyLeft", TEXT("EasyKirbyLeft.bmp"));

	m_pAnimation->AddClip(g_pEasyAnimation[SD_RIGHT][E_SLEEP]);
	m_pAnimation->SetClipTexture(g_pEasyAnimation[SD_RIGHT][E_SLEEP], "EasyKirbyRight", TEXT("EasyKirbyRight.bmp"));

	m_pAnimation->LoadClip("../Bin/Save/EasyKirby/Mike1Left.clip");
	m_pAnimation->LoadClip("../Bin/Save/EasyKirby/Mike2Left.clip");
	m_pAnimation->LoadClip("../Bin/Save/EasyKirby/Mike3Left.clip");
	m_pAnimation->LoadClip("../Bin/Save/EasyKirby/SleepLeft.clip");

	m_pAnimation->LoadClip("../Bin/Save/EasyKirby/Mike1Right.clip");
	m_pAnimation->LoadClip("../Bin/Save/EasyKirby/Mike2Right.clip");
	m_pAnimation->LoadClip("../Bin/Save/EasyKirby/Mike3Right.clip");
	m_pAnimation->LoadClip("../Bin/Save/EasyKirby/SleepRight.clip");

}

void CPlayer::StoneKirbySource()
{
	// Stone Kirby Idle
	m_pAnimation->AddClip(g_pStoneAnimation[SD_LEFT][ST_IDLE]); 
	m_pAnimation->SetClipTexture(g_pStoneAnimation[SD_LEFT][ST_IDLE], "StoneKirbyLeft", TEXT("StoneKirbyLeft.bmp"));


	m_pAnimation->AddClip(g_pStoneAnimation[SD_RIGHT][ST_IDLE]);
	m_pAnimation->SetClipTexture(g_pStoneAnimation[SD_RIGHT][ST_IDLE], "StoneKirbyRight", TEXT("StoneKirbyRight.bmp"));


	// Stone Kirby Down
	m_pAnimation->AddClip(g_pStoneAnimation[SD_LEFT][ST_DOWN]);
	m_pAnimation->SetClipTexture(g_pStoneAnimation[SD_LEFT][ST_DOWN], "StoneKirbyLeft", TEXT("StoneKirbyLeft.bmp"));


	m_pAnimation->AddClip(g_pStoneAnimation[SD_RIGHT][ST_DOWN]);
	m_pAnimation->SetClipTexture(g_pStoneAnimation[SD_RIGHT][ST_DOWN], "StoneKirbyRight", TEXT("StoneKirbyRight.bmp"));


	// Stone Kirby Walk
	m_pAnimation->AddClip(g_pStoneAnimation[SD_LEFT][ST_WALK]);
	m_pAnimation->SetClipTexture(g_pStoneAnimation[SD_LEFT][ST_WALK], "StoneKirbyLeft", TEXT("StoneKirbyLeft.bmp"));


	m_pAnimation->AddClip(g_pStoneAnimation[SD_RIGHT][ST_WALK]);
	m_pAnimation->SetClipTexture(g_pStoneAnimation[SD_RIGHT][ST_WALK], "StoneKirbyRight", TEXT("StoneKirbyRight.bmp"));


	// Stone Kirby Run
	m_pAnimation->AddClip(g_pStoneAnimation[SD_LEFT][ST_RUN]);
	m_pAnimation->SetClipTexture(g_pStoneAnimation[SD_LEFT][ST_RUN], "StoneKirbyLeft", TEXT("StoneKirbyLeft.bmp"));


	m_pAnimation->AddClip(g_pStoneAnimation[SD_RIGHT][ST_RUN]);
	m_pAnimation->SetClipTexture(g_pStoneAnimation[SD_RIGHT][ST_RUN], "StoneKirbyRight", TEXT("StoneKirbyRight.bmp"));


	// Stone Kirby Fly
	m_pAnimation->AddClip(g_pStoneAnimation[SD_LEFT][ST_FLY]);
	m_pAnimation->SetClipTexture(g_pStoneAnimation[SD_LEFT][ST_FLY], "StoneKirbyLeft", TEXT("StoneKirbyLeft.bmp"));


	m_pAnimation->AddClip(g_pStoneAnimation[SD_RIGHT][ST_FLY]);
	m_pAnimation->SetClipTexture(g_pStoneAnimation[SD_RIGHT][ST_FLY], "StoneKirbyRight", TEXT("StoneKirbyRight.bmp"));



	// Stone Kirby Prevent
	m_pAnimation->AddClip(g_pStoneAnimation[SD_LEFT][ST_PREVENT]);
	m_pAnimation->SetClipTexture(g_pStoneAnimation[SD_LEFT][ST_PREVENT], "StoneKirbyLeft", TEXT("StoneKirbyLeft.bmp"));


	m_pAnimation->AddClip(g_pStoneAnimation[SD_RIGHT][ST_PREVENT]);
	m_pAnimation->SetClipTexture(g_pStoneAnimation[SD_RIGHT][ST_PREVENT], "StoneKirbyRight", TEXT("StoneKirbyRight.bmp"));



	// Stone Kirby Slide
	m_pAnimation->AddClip(g_pStoneAnimation[SD_LEFT][ST_SLIDE]);
	m_pAnimation->SetClipTexture(g_pStoneAnimation[SD_LEFT][ST_SLIDE], "StoneKirbyLeft", TEXT("StoneKirbyLeft.bmp"));


	m_pAnimation->AddClip(g_pStoneAnimation[SD_RIGHT][ST_SLIDE]);
	m_pAnimation->SetClipTexture(g_pStoneAnimation[SD_RIGHT][ST_SLIDE], "StoneKirbyRight", TEXT("StoneKirbyRight.bmp"));


	// Stone Kirby Attack
	m_pAnimation->AddClip(g_pStoneAnimation[SD_LEFT][ST_ATTACK]);
	m_pAnimation->SetClipTexture(g_pStoneAnimation[SD_LEFT][ST_ATTACK], "StoneKirbyLeft", TEXT("StoneKirbyLeft.bmp"));


	m_pAnimation->AddClip(g_pStoneAnimation[SD_RIGHT][ST_ATTACK]);
	m_pAnimation->SetClipTexture(g_pStoneAnimation[SD_RIGHT][ST_ATTACK], "StoneKirbyRight", TEXT("StoneKirbyRight.bmp"));



	// Stone Kirby Vomit
	m_pAnimation->AddClip(g_pStoneAnimation[SD_LEFT][ST_VOMIT]);
	m_pAnimation->SetClipTexture(g_pStoneAnimation[SD_LEFT][ST_VOMIT], "StoneKirbyLeft", TEXT("StoneKirbyLeft.bmp"));


	m_pAnimation->AddClip(g_pStoneAnimation[SD_RIGHT][ST_VOMIT]);
	m_pAnimation->SetClipTexture(g_pStoneAnimation[SD_RIGHT][ST_VOMIT], "StoneKirbyRight", TEXT("StoneKirbyRight.bmp"));


	// Stone Kirby Hit
	m_pAnimation->AddClip(g_pStoneAnimation[SD_LEFT][ST_HIT]);
	m_pAnimation->SetClipTexture(g_pStoneAnimation[SD_LEFT][ST_HIT], "StoneKirbyLeft", TEXT("StoneKirbyLeft.bmp"));


	m_pAnimation->AddClip(g_pStoneAnimation[SD_RIGHT][ST_HIT]);
	m_pAnimation->SetClipTexture(g_pStoneAnimation[SD_RIGHT][ST_HIT], "StoneKirbyRight", TEXT("StoneKirbyRight.bmp"));



	// Stone Kirby FlyUp
	m_pAnimation->AddClip(g_pStoneAnimation[SD_LEFT][ST_FLY_UP]);
	m_pAnimation->SetClipTexture(g_pStoneAnimation[SD_LEFT][ST_FLY_UP], "StoneKirbyLeft", TEXT("StoneKirbyLeft.bmp"));


	m_pAnimation->AddClip(g_pStoneAnimation[SD_RIGHT][ST_FLY_UP]);
	m_pAnimation->SetClipTexture(g_pStoneAnimation[SD_RIGHT][ST_FLY_UP], "StoneKirbyRight", TEXT("StoneKirbyRight.bmp"));

	

	// Stone Kirby Jump
	m_pAnimation->AddClip(g_pStoneAnimation[SD_LEFT][ST_JUMP]);
	m_pAnimation->SetClipTexture(g_pStoneAnimation[SD_LEFT][ST_JUMP], "StoneKirbyLeft", TEXT("StoneKirbyLeft.bmp"));


	m_pAnimation->AddClip(g_pStoneAnimation[SD_RIGHT][ST_JUMP]);
	m_pAnimation->SetClipTexture(g_pStoneAnimation[SD_RIGHT][ST_JUMP], "StoneKirbyRight", TEXT("StoneKirbyRight.bmp"));



	// Stone Kirby Big Hit
	m_pAnimation->AddClip(g_pStoneAnimation[SD_LEFT][ST_BIG_HIT]);
	m_pAnimation->SetClipTexture(g_pStoneAnimation[SD_LEFT][ST_BIG_HIT], "StoneKirbyLeft", TEXT("StoneKirbyLeft.bmp"));


	m_pAnimation->AddClip(g_pStoneAnimation[SD_RIGHT][ST_BIG_HIT]);
	m_pAnimation->SetClipTexture(g_pStoneAnimation[SD_RIGHT][ST_BIG_HIT], "StoneKirbyRight", TEXT("StoneKirbyRight.bmp"));




	m_pAnimation->LoadClip("../Bin/Save/StoneKirby/StoneDownLeft.clip");
	m_pAnimation->LoadClip("../Bin/Save/StoneKirby/StoneDownRight.clip");

	m_pAnimation->LoadClip("../Bin/Save/StoneKirby/StoneFlyUpLeft.clip");
	m_pAnimation->LoadClip("../Bin/Save/StoneKirby/StoneFlyUpRight.clip");

	m_pAnimation->LoadClip("../Bin/Save/StoneKirby/StoneIdleLeft.clip");
	m_pAnimation->LoadClip("../Bin/Save/StoneKirby/StoneIdleRight.clip");

	m_pAnimation->LoadClip("../Bin/Save/StoneKirby/StoneVomitLeft.clip");
	m_pAnimation->LoadClip("../Bin/Save/StoneKirby/StoneVomitRight.clip");

	m_pAnimation->LoadClip("../Bin/Save/StoneKirby/StoneWalkLeft.clip");
	m_pAnimation->LoadClip("../Bin/Save/StoneKirby/StoneWalkRight.clip");

	m_pAnimation->LoadClip("../Bin/Save/StoneKirby/StoneBigHitLeft.clip");
	m_pAnimation->LoadClip("../Bin/Save/StoneKirby/StoneBigHitRight.clip");


	m_pAnimation->LoadClip("../Bin/Save/StoneKirby/StoneHitLeft.clip");
	m_pAnimation->LoadClip("../Bin/Save/StoneKirby/StoneHitRight.clip");

	m_pAnimation->LoadClip("../Bin/Save/StoneKirby/StoneJumpLeft.clip");
	m_pAnimation->LoadClip("../Bin/Save/StoneKirby/StoneJumpRight.clip");

	m_pAnimation->LoadClip("../Bin/Save/StoneKirby/StonePreventLeft.clip");
	m_pAnimation->LoadClip("../Bin/Save/StoneKirby/StonePreventRight.clip");

	m_pAnimation->LoadClip("../Bin/Save/StoneKirby/StoneSlideLeft.clip");
	m_pAnimation->LoadClip("../Bin/Save/StoneKirby/StoneSlideRight.clip");

	m_pAnimation->LoadClip("../Bin/Save/StoneKirby/StoneAttackRight.clip");
	m_pAnimation->LoadClip("../Bin/Save/StoneKirby/StoneAttackLeft.clip");

	m_pAnimation->LoadClip("../Bin/Save/StoneKirby/StoneFlyLeft.clip");
	m_pAnimation->LoadClip("../Bin/Save/StoneKirby/StoneFlyRight.clip");

	m_pAnimation->LoadClip("../Bin/Save/StoneKirby/StoneRunLeft.clip");
	m_pAnimation->LoadClip("../Bin/Save/StoneKirby/StoneRunRight.clip");

	for (int i = 0; i < SD_END; ++i)
	{
		for (int j = 0; j < ST_END; ++j)
		{
			m_pAnimation->SetTextureColorKey(g_pStoneAnimation[i][j], 96, 160, 128);
		}
	}

	// 컬러키가 지정이 안 된 Clip들이 있는 것 같으니 미리 설정해두는 것으로 하자.

	/*
	enum STONE_CLIP
	{
		ST_IDLE,
		ST_DOWN,
		ST_WALK,
		ST_RUN,
		ST_FLY,
		ST_PREVENT,
		ST_SLIDE,
		ST_ATTACK,
		ST_VOMIT,
		ST_HIT,
		ST_FLY_UP,
		ST_JUMP,
		ST_BIG_HIT,
		ST_END
	};

	// AttackLeft AttackRight 근육질의 아저씨 OR 마리오로 고정.

	const char* g_pStoneAnimation[SD_END][ST_END] =
	{
		{"StoneIdleLeft", "StoneDownLeft", "StoneWalkLeft", "StoneRunLeft", "StoneFlyLeft"
		"StonePreventLeft", "StoneSlideLeft", "StoneAttackLeft", "StoneVomitLeft", "StoneHitLeft",
		"StoneFlyUpLeft", "StoneJumpLeft", "StoneBigHitLeft"},

		{"StoneIdleRight", "StoneDownRight", "StoneWalkRight", "StoneRunRight", "StoneFlyRight"
		"StonePreventRight", "StoneSlideRight", "StoneAttackRight", "StoneVomitRight", "StoneHitRight",
		"StoneFlyUpRight", "StoneJumpRight", "StoneBigHitRight"},
	};

	*/
}

void CPlayer::SwordKirbySource()
{
	// Sword Kirby SW_IDLE
	m_pAnimation->AddClip(g_pSwordAnimation[SD_RIGHT][SW_IDLE]);
	m_pAnimation->SetClipTexture(g_pSwordAnimation[SD_RIGHT][SW_IDLE], "SwordKirbyRight", TEXT("SwordKirbyRight.bmp"));

	m_pAnimation->AddClip(g_pSwordAnimation[SD_LEFT][SW_IDLE]);
	m_pAnimation->SetClipTexture(g_pSwordAnimation[SD_LEFT][SW_IDLE], "SwordKirbyLeft", TEXT("SwordKirbyLeft.bmp"));

	// Sword Kirby SW_DOWN
	m_pAnimation->AddClip(g_pSwordAnimation[SD_RIGHT][SW_DOWN]);
	m_pAnimation->SetClipTexture(g_pSwordAnimation[SD_RIGHT][SW_DOWN], "SwordKirbyRight", TEXT("SwordKirbyRight.bmp"));

	m_pAnimation->AddClip(g_pSwordAnimation[SD_LEFT][SW_DOWN]);
	m_pAnimation->SetClipTexture(g_pSwordAnimation[SD_LEFT][SW_DOWN], "SwordKirbyLeft", TEXT("SwordKirbyLeft.bmp"));
	

	// Sword Kirby SW_WALK
	m_pAnimation->AddClip(g_pSwordAnimation[SD_RIGHT][SW_WALK]);
	m_pAnimation->SetClipTexture(g_pSwordAnimation[SD_RIGHT][SW_WALK], "SwordKirbyRight", TEXT("SwordKirbyRight.bmp"));

	m_pAnimation->AddClip(g_pSwordAnimation[SD_LEFT][SW_WALK]);
	m_pAnimation->SetClipTexture(g_pSwordAnimation[SD_LEFT][SW_WALK], "SwordKirbyLeft", TEXT("SwordKirbyLeft.bmp"));


	// Sword Kirby SW_RUN
	m_pAnimation->AddClip(g_pSwordAnimation[SD_RIGHT][SW_RUN]);
	m_pAnimation->SetClipTexture(g_pSwordAnimation[SD_RIGHT][SW_RUN], "SwordKirbyRight", TEXT("SwordKirbyRight.bmp"));

	m_pAnimation->AddClip(g_pSwordAnimation[SD_LEFT][SW_RUN]);
	m_pAnimation->SetClipTexture(g_pSwordAnimation[SD_LEFT][SW_RUN], "SwordKirbyLeft", TEXT("SwordKirbyLeft.bmp"));

	// Sword Kirby SW_FLY
	m_pAnimation->AddClip(g_pSwordAnimation[SD_RIGHT][SW_FLY]);
	m_pAnimation->SetClipTexture(g_pSwordAnimation[SD_RIGHT][SW_FLY], "SwordKirbyRight", TEXT("SwordKirbyRight.bmp"));

	m_pAnimation->AddClip(g_pSwordAnimation[SD_LEFT][SW_FLY]);
	m_pAnimation->SetClipTexture(g_pSwordAnimation[SD_LEFT][SW_FLY], "SwordKirbyLeft", TEXT("SwordKirbyLeft.bmp"));


	// Sword Kirby SW_PREVENT
	m_pAnimation->AddClip(g_pSwordAnimation[SD_RIGHT][SW_PREVENT]);
	m_pAnimation->SetClipTexture(g_pSwordAnimation[SD_RIGHT][SW_PREVENT], "SwordKirbyRight", TEXT("SwordKirbyRight.bmp"));

	m_pAnimation->AddClip(g_pSwordAnimation[SD_LEFT][SW_PREVENT]);
	m_pAnimation->SetClipTexture(g_pSwordAnimation[SD_LEFT][SW_PREVENT], "SwordKirbyLeft", TEXT("SwordKirbyLeft.bmp"));


	// Sword Kirby SW_SLIDE
	m_pAnimation->AddClip(g_pSwordAnimation[SD_RIGHT][SW_SLIDE]);
	m_pAnimation->SetClipTexture(g_pSwordAnimation[SD_RIGHT][SW_SLIDE], "SwordKirbyRight", TEXT("SwordKirbyRight.bmp"));

	m_pAnimation->AddClip(g_pSwordAnimation[SD_LEFT][SW_SLIDE]);
	m_pAnimation->SetClipTexture(g_pSwordAnimation[SD_LEFT][SW_SLIDE], "SwordKirbyLeft", TEXT("SwordKirbyLeft.bmp"));


	// Sword Kirby SW_ATTACK1
	m_pAnimation->AddClip(g_pSwordAnimation[SD_RIGHT][SW_ATTACK1]);
	m_pAnimation->SetClipTexture(g_pSwordAnimation[SD_RIGHT][SW_ATTACK1], "SwordKirbyRight", TEXT("SwordKirbyRight.bmp"));

	m_pAnimation->AddClip(g_pSwordAnimation[SD_LEFT][SW_ATTACK1]);
	m_pAnimation->SetClipTexture(g_pSwordAnimation[SD_LEFT][SW_ATTACK1], "SwordKirbyLeft", TEXT("SwordKirbyLeft.bmp"));


	// Sword Kirby SW_ATTACK2
	m_pAnimation->AddClip(g_pSwordAnimation[SD_RIGHT][SW_ATTACK2]);
	m_pAnimation->SetClipTexture(g_pSwordAnimation[SD_RIGHT][SW_ATTACK2], "SwordKirbyRight", TEXT("SwordKirbyRight.bmp"));

	m_pAnimation->AddClip(g_pSwordAnimation[SD_LEFT][SW_ATTACK2]);
	m_pAnimation->SetClipTexture(g_pSwordAnimation[SD_LEFT][SW_ATTACK2], "SwordKirbyLeft", TEXT("SwordKirbyLeft.bmp"));


	// Sword Kirby SW_ATTACK3
	m_pAnimation->AddClip(g_pSwordAnimation[SD_RIGHT][SW_ATTACK3]);
	m_pAnimation->SetClipTexture(g_pSwordAnimation[SD_RIGHT][SW_ATTACK3], "SwordKirbyRight", TEXT("SwordKirbyRight.bmp"));

	m_pAnimation->AddClip(g_pSwordAnimation[SD_LEFT][SW_ATTACK3]);
	m_pAnimation->SetClipTexture(g_pSwordAnimation[SD_LEFT][SW_ATTACK3], "SwordKirbyLeft", TEXT("SwordKirbyLeft.bmp"));


	// Sword Kirby SW_DASH_ATTACK
	m_pAnimation->AddClip(g_pSwordAnimation[SD_RIGHT][SW_DASH_ATTACK]);
	m_pAnimation->SetClipTexture(g_pSwordAnimation[SD_RIGHT][SW_DASH_ATTACK], "SwordKirbyRight", TEXT("SwordKirbyRight.bmp"));

	m_pAnimation->AddClip(g_pSwordAnimation[SD_LEFT][SW_DASH_ATTACK]);
	m_pAnimation->SetClipTexture(g_pSwordAnimation[SD_LEFT][SW_DASH_ATTACK], "SwordKirbyLeft", TEXT("SwordKirbyLeft.bmp"));


	// Sword Kirby SW_JUMP_ATTACK
	m_pAnimation->AddClip(g_pSwordAnimation[SD_RIGHT][SW_JUMP_ATTACK]);
	m_pAnimation->SetClipTexture(g_pSwordAnimation[SD_RIGHT][SW_JUMP_ATTACK], "SwordKirbyRight", TEXT("SwordKirbyRight.bmp"));

	m_pAnimation->AddClip(g_pSwordAnimation[SD_LEFT][SW_JUMP_ATTACK]);
	m_pAnimation->SetClipTexture(g_pSwordAnimation[SD_LEFT][SW_JUMP_ATTACK], "SwordKirbyLeft", TEXT("SwordKirbyLeft.bmp"));


	// Sword Kirby SW_VOMIT
	m_pAnimation->AddClip(g_pSwordAnimation[SD_RIGHT][SW_VOMIT]);
	m_pAnimation->SetClipTexture(g_pSwordAnimation[SD_RIGHT][SW_VOMIT], "SwordKirbyRight", TEXT("SwordKirbyRight.bmp"));

	m_pAnimation->AddClip(g_pSwordAnimation[SD_LEFT][SW_VOMIT]);
	m_pAnimation->SetClipTexture(g_pSwordAnimation[SD_LEFT][SW_VOMIT], "SwordKirbyLeft", TEXT("SwordKirbyLeft.bmp"));


	// Sword Kirby SW_HIT
	m_pAnimation->AddClip(g_pSwordAnimation[SD_RIGHT][SW_HIT]);
	m_pAnimation->SetClipTexture(g_pSwordAnimation[SD_RIGHT][SW_HIT], "SwordKirbyRight", TEXT("SwordKirbyRight.bmp"));

	m_pAnimation->AddClip(g_pSwordAnimation[SD_LEFT][SW_HIT]);
	m_pAnimation->SetClipTexture(g_pSwordAnimation[SD_LEFT][SW_HIT], "SwordKirbyLeft", TEXT("SwordKirbyLeft.bmp"));



	// Sword Kirby SW_BIG_HIT
	m_pAnimation->AddClip(g_pSwordAnimation[SD_RIGHT][SW_BIG_HIT]);
	m_pAnimation->SetClipTexture(g_pSwordAnimation[SD_RIGHT][SW_BIG_HIT], "SwordKirbyRight", TEXT("SwordKirbyRight.bmp"));

	m_pAnimation->AddClip(g_pSwordAnimation[SD_LEFT][SW_BIG_HIT]);
	m_pAnimation->SetClipTexture(g_pSwordAnimation[SD_LEFT][SW_BIG_HIT], "SwordKirbyLeft", TEXT("SwordKirbyLeft.bmp"));


	// Sword Kirby SW_FLY_UP
	m_pAnimation->AddClip(g_pSwordAnimation[SD_RIGHT][SW_FLY_UP]);
	m_pAnimation->SetClipTexture(g_pSwordAnimation[SD_RIGHT][SW_FLY_UP], "SwordKirbyRight", TEXT("SwordKirbyRight.bmp"));

	m_pAnimation->AddClip(g_pSwordAnimation[SD_LEFT][SW_FLY_UP]);
	m_pAnimation->SetClipTexture(g_pSwordAnimation[SD_LEFT][SW_FLY_UP], "SwordKirbyLeft", TEXT("SwordKirbyLeft.bmp"));


	// Sword Kirby SW_JUMP
	m_pAnimation->AddClip(g_pSwordAnimation[SD_RIGHT][SW_JUMP]);
	m_pAnimation->SetClipTexture(g_pSwordAnimation[SD_RIGHT][SW_JUMP], "SwordKirbyRight", TEXT("SwordKirbyRight.bmp"));

	m_pAnimation->AddClip(g_pSwordAnimation[SD_LEFT][SW_JUMP]);
	m_pAnimation->SetClipTexture(g_pSwordAnimation[SD_LEFT][SW_JUMP], "SwordKirbyLeft", TEXT("SwordKirbyLeft.bmp"));




	m_pAnimation->LoadClip("../Bin/Save/SwordKirby/SwordWalkRight.clip");
	m_pAnimation->LoadClip("../Bin/Save/SwordKirby/SwordWalkLeft.clip");

	m_pAnimation->LoadClip("../Bin/Save/SwordKirby/SwordBigHitRight.clip");
	m_pAnimation->LoadClip("../Bin/Save/SwordKirby/SwordBigHitLeft.clip");

	m_pAnimation->LoadClip("../Bin/Save/SwordKirby/SwordIdleRight.clip");
	m_pAnimation->LoadClip("../Bin/Save/SwordKirby/SwordIdleLeft.clip");

	m_pAnimation->LoadClip("../Bin/Save/SwordKirby/SwordDownRight.clip");
	m_pAnimation->LoadClip("../Bin/Save/SwordKirby/SwordDownLeft.clip");

	m_pAnimation->LoadClip("../Bin/Save/SwordKirby/SwordJumpRight.clip");
	m_pAnimation->LoadClip("../Bin/Save/SwordKirby/SwordJumpLeft.clip");

	m_pAnimation->LoadClip("../Bin/Save/SwordKirby/SwordDashAttackRight.clip");
	m_pAnimation->LoadClip("../Bin/Save/SwordKirby/SwordDashAttackLeft.clip");

	m_pAnimation->LoadClip("../Bin/Save/SwordKirby/SwordFlyRight.clip");
	m_pAnimation->LoadClip("../Bin/Save/SwordKirby/SwordFlyLeft.clip");

	m_pAnimation->LoadClip("../Bin/Save/SwordKirby/SwordFlyUpRight.clip");
	m_pAnimation->LoadClip("../Bin/Save/SwordKirby/SwordFlyUpLeft.clip");

	m_pAnimation->LoadClip("../Bin/Save/SwordKirby/SwordJumpAttackRight.clip");
	m_pAnimation->LoadClip("../Bin/Save/SwordKirby/SwordJumpAttackLeft.clip");

	m_pAnimation->LoadClip("../Bin/Save/SwordKirby/SwordPreventRight.clip");
	m_pAnimation->LoadClip("../Bin/Save/SwordKirby/SwordPreventLeft.clip");

	m_pAnimation->LoadClip("../Bin/Save/SwordKirby/SwordSlideRight.clip");
	m_pAnimation->LoadClip("../Bin/Save/SwordKirby/SwordSlideLeft.clip");

	m_pAnimation->LoadClip("../Bin/Save/SwordKirby/SwordVomitRight.clip");
	m_pAnimation->LoadClip("../Bin/Save/SwordKirby/SwordVomitLeft.clip");

	m_pAnimation->LoadClip("../Bin/Save/SwordKirby/SwordAttack1Left.clip");
	m_pAnimation->LoadClip("../Bin/Save/SwordKirby/SwordAttack1Right.clip");

	m_pAnimation->LoadClip("../Bin/Save/SwordKirby/SwordAttack2Left.clip");
	m_pAnimation->LoadClip("../Bin/Save/SwordKirby/SwordAttack2Right.clip");

	m_pAnimation->LoadClip("../Bin/Save/SwordKirby/SwordAttack3Left.clip");
	m_pAnimation->LoadClip("../Bin/Save/SwordKirby/SwordAttack3Right.clip");

	m_pAnimation->LoadClip("../Bin/Save/SwordKirby/SwordHitLeft.clip");
	m_pAnimation->LoadClip("../Bin/Save/SwordKirby/SwordHitRight.clip");

	m_pAnimation->LoadClip("../Bin/Save/SwordKirby/SwordRunLeft.clip");
	m_pAnimation->LoadClip("../Bin/Save/SwordKirby/SwordRunRight.clip");



	/*
	enum SWORD_CLIP
	{
		SW_IDLE,
		SW_DOWN,
		SW_WALK,
		SW_RUN,
		SW_FLY,

		SW_PREVENT,
		SW_SLIDE,
		SW_ATTACK1, // 공격에 콤보 시스템 있는 거 아심 ? 여기에 이펙트 따로 한바닥 나올 것 같음.. ㅎㄷㄷ ㅠㅠ
		SW_ATTACK2,
		SW_ATTACK3,
		SW_DASH_ATTACK,

		SW_JUMP_ATTACK,
		SW_VOMIT,
		SW_HIT,
		SW_BIG_HIT,
		SW_FLY_UP,

		SW_JUMP,
		SW_END
	};

	const char* g_pSwordAnimation[SD_END][SW_END] = {

		{"SwordIdleLeft", "SwordDownLeft", "SwordWalkLeft", "SwordRunLeft", "SwordFlyLeft",
		"SwordPreventLeft", "SwordSlideLeft", "SwordAttack1Left", "SwordAttack2Left", "SwordAttack3Left"
		"SwordJumpAttackLeft", "SwordVomitLeft", "SwordHitLeft", "SwordBigHitLeft", "SwordFlyUpLeft",
		"SwordJumpLeft"},

		{"SwordIdleRight", "SwordDownRight", "SwordWalkRight", "SwordRunRight", "SwordFlyRight",
		"SwordPreventRight", "SwordSlideRight", "SwordAttack1Right", "SwordAttack2Right", "SwordAttack3Right"
		"SwordJumpAttackRight", "SwordVomitRight", "SwordHitRight", "SwordBigHitRight", "SwordFlyUpRight",
		"SwordJumpRight"}
	};
	*/

	for (int i = 0; i < SD_END; ++i)
	{
		for (int j = 0; j < SW_END; ++j)
		{
			m_pAnimation->SetTextureColorKey(g_pSwordAnimation[i][j], 160, 224, 128);
		}
	}
}






