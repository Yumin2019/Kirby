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

// AttackLeft AttackRight �������� ������ OR �������� ����.

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
	SW_ATTACK1, // ���ݿ� �޺� �ý��� �ִ� �� �ƽ� ? ���⿡ ����Ʈ ���� �ѹٴ� ���� �� ����.. ������ �Ф�
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
	//PL_DIE, Die�� ���� �������� ������ �����Ƿ� �׳� ���� ���ش�.
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

// �ɷ¿� ���� ������ ���߿�....
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
	// �丮�� ������.. �ƴϸ� �¿� �� �־ 1�����迭�� ���·� ���.

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
		// ���� vector�� �� ��쿡�� ó���Ѵ�.
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


	CreateAnimation("PlayerAnimation"); // �÷��̾� �ִϸ��̼��� �����Ѵ�.


	// Big Jump left 
	m_pAnimation->AddClip(g_pPlayerAnimationName[SD_LEFT][PL_BIG_JUMP]); // Ŭ���� �����Ѵ�. ���� ���̵�.
	m_pAnimation->SetClipTexture(g_pPlayerAnimationName[SD_LEFT][PL_BIG_JUMP], "KirbyLeft", TEXT("KirbyLeft.bmp"));
	m_pAnimation->LoadClip("../Bin/Save/Kirby/KirbyBigJumpLeft.clip");

	// Big Jump Right 

	m_pAnimation->AddClip(g_pPlayerAnimationName[SD_RIGHT][PL_BIG_JUMP]);
	m_pAnimation->SetClipTexture(g_pPlayerAnimationName[SD_RIGHT][PL_BIG_JUMP], "KirbyRight", TEXT("KirbyRight.bmp"));
	m_pAnimation->LoadClip("../Bin/Save/Kirby/KirbyBigJumpRight.clip");



	// Jump left 
	m_pAnimation->AddClip(g_pPlayerAnimationName[SD_LEFT][PL_JUMP]); // Ŭ���� �����Ѵ�. ���� ���̵�.
	m_pAnimation->SetClipTexture(g_pPlayerAnimationName[SD_LEFT][PL_JUMP], "KirbyLeft", TEXT("KirbyLeft.bmp"));
	m_pAnimation->LoadClip("../Bin/Save/Kirby/KirbyJumpLeft.clip");

	// Jump Right 

	m_pAnimation->AddClip(g_pPlayerAnimationName[SD_RIGHT][PL_JUMP]);
	m_pAnimation->SetClipTexture(g_pPlayerAnimationName[SD_RIGHT][PL_JUMP], "KirbyRight", TEXT("KirbyRight.bmp"));
	m_pAnimation->LoadClip("../Bin/Save/Kirby/KirbyJumpRight.clip");






		// Idle left 
	m_pAnimation->AddClip(g_pPlayerAnimationName[SD_LEFT][PL_IDLE]); // Ŭ���� �����Ѵ�. ���� ���̵�.
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




	// Fly Left ����

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


	// Attack�ִϸ��̼��� ����, ó���� ���Ƶ��̴� �ִϸ��̼����Ŀ� ���� Ű�� ��� ������ �ִ� ��쿡 ���� ������ �ִ� ����� �ݺ��ȴ�.
	// �� �κ��� ����ó���� �ʿ��ϴ�.




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
	// �̰��� ȣ��Ǳ� ������ �Ϲ� Ŀ�� �ƴ϶�� ����ó���� �̷�� ���� �Ѵ�.

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
		// �ɷ��� Ǯ����.

		ThrowAbility();

		// �ϴ� Idle�� ������ �Ǵµ� �̰��� ���߿� ��ġ���� ����.
	}
}

void CPlayer::ThrowAbility()
{
	

		// �ɷ��� ������ �ִ� ���¶�� �ɷ��� ���� �غ� �Ѵ�.
		// �Ʒ��� ���� ��� ���� Blue Ball�� �����Ѵ�.


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

		pBlueBall->SetBlend(0.5f, 100, 200, 150); // ���� Blend ���� Blend ��Ƽ.

		pBlueBall->SetTarget(pJumpStar);

		// �ڽ��� �ٶ󺸰� �ִ� ���⿡ ���� ��� ��ġ�� ���ư������� ��������, � ��ġ���� ������ �Ǵ��� �������� �κ��̴�.\


		// �ڽ��� �ɷ��� JumpStar���� �ش�. �ڽ��� Ŀ����·� ������.

		switch (m_eType)
		{
		case KIRBY_STONE:
			pJumpStar->SetAbility(MT_STONE);
			break;

		case KIRBY_SWORD:
			pJumpStar->SetAbility(MT_SWORD);
			break;
		}

		// �̷��� �ɷ��� �ָ�, ���Ŀ� �ش� ���� �Ծ��� �� �� �ɷ��� ����� �ִ� ���̴�.
		// �Դ� �Ϳ� ���� ó���� �Դ� ��쿡 ó���� �ȴ�.

		// ���Ŀ� �⺻ Ŀ��� �ٲ��.
		m_eType = KIRBY_NORMAL;
		KirbyConversion();


	
}

void CPlayer::MonsterCollision(CObj * pObj)
{
	// ���Ͱ� ������ �ϴ� ���.

	switch (pObj->GetObjectType())
	{

		case OT_MONSTER_ATTACK:
		case OT_NORMAL_MONSTER:
		case OT_BOSS_DDD:
		{
			SIDE_DIR eCollisionDir = SD_LEFT;

			// ���Ϳ� �浹�� �Ͼ ���.
			SetBlend(1.f, 2);
			GET_SINGLE(CSoundManager)->Play("KirbyHit", CT_EFFECT);
			SetInputDisEnable(5.f);
			Damage(1);

			// ���󰡾� �ϴ� ������ �����Ѵ�.
			if (pObj->GetPos().x < m_tPos.x)
				eCollisionDir = SD_RIGHT;

			// ������ �� ���¶�� �˾�.
			// ������ ���� ���� ���¶�� �˹鸸.

			SetKnockBack(0.8f, 600.f, eCollisionDir);
			GET_SINGLE(CSoundManager)->Stop(CT_ATTACK);


			// ���󰡾� �ϴ� ��Ȳ�̶��, ������, Run����, �پ��� ���µ��� Ǯ���ִ� ���� �´�.
			// �ٵ� �̰� Ǯ���ָ� �Ƹ� �߷¿� ���ؼ� ���� �̻��� ���� �ִµ� �̰Ŵ� �Ŀ������� �����غ���.

			// �ִϸ��̼��� ��ü���ش�.

			// �⺻�ִϸ��̼� ����.
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
				// ������ �¾Ҵٸ� �޺��� ���ܾ� �Ѵ�.

				break;
			}


			m_bTake = false;


			switch (m_eType)
			{
			case KIRBY_NORMAL:

				// Big ���¿��� �´��� ���� ���� ��������� �ʴ´�. (����)
				// ���� ��������� �� ���̴�.

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


			case KIRBY_SLEEP: // �Ƹ� ���� ������ �ɷ� ����.
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

			// ������ �ִٰ�, �´� ��� �ٷ� ó���� ��������� �Ѵ�.
			m_bStop = false; // ���� ���Ŀ� Stop = false�� ���¿��� �Ѵ�.
			

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
	// ������ �ִ� ���� ������ ���̰�, .. ���Ŀ� �ƶ� ����.
	if (m_pTornadoEffect)
		return;

	// ���ο� ����Ʈ�� �����Ͽ� �޾Ƶд�.

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
	// ���Ƶ��̴� ������Ʈ�� ��쿡�� �÷��̾ ����ٴѴ�.
	// Offset�� �̸� �����ص� ���� ����.
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


	

	// Falling ������ ��쿡�� ������ �Ա��� ������ ����̰�,
	// �ƴ϶�� �ٴڿ��� �Ա��� ������ ��츦 ���Ѵ�.

	// ���� �ð��� ���� �༭ ������ ���´�.
	m_bStop = true;
	m_bBreath = true;
	m_fBreathTime = 0.f;

	// �Ա��� ������ ��쿡�� breath�� stopó���� �� ���̴�. ����, vomit�� false�� �����.
	m_bVomit = false;

	switch (m_eType)
	{
	case KIRBY_NORMAL:
	case KIRBY_MIKE:
	case KIRBY_SLEEP:
		// �Ա��� ������ �� ���Ŀ� ������ ���ð��� �༭ �ٷ� ������ ������ �� ������ �Ѵ�.
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
	// Ballon�� Ǯ��� ���·�. 
	// Falling�� �� �ʿ䰡 ���°�, Falling�� ��쿡�� �Ա��� ��� �������� ��쿡
	// õõ�� ���������� �ϱ� �����̴�.
	// Falling�� �������� �ʾƵ� �ǰ�, �ִϸ��̼Ǹ�.

	
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

	// �� ���·� �ѹ� �Ա� ����ְ�, idle������� ���Ѵ�.

	// �Ա� ����.

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

	// Sleep�� ������ �ʿ䰡 ����.

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
	
	// DefaultClip�� return �ִϸ��̼� ���Ŀ� ���ư� Ŭ���� �̸� ���صδ� ��������.
	// ���� Ŭ���� �ٲٴ� �Լ��� �ƴϴ�.
	// ��, ���� Ŭ���� IdleLeft�� �Ǿ� �ִ�. (ó���� �־ ���� ����)
	// ���� �̰��� ���������� ���̰� �ϰ� �ʹٸ�, ChangeClip�� ���ؼ� �ش� Ŭ���� �����ϸ� �ȴ�.

	KirbyChangeClip(PL_IDLE);

	m_fMoveSpeed = 350.f;



	// �⺻ Ŀ��.
	SetPivot(0.5f, 0.5f);
	SetStaticSize(100.f, 100.f); // ũ�Ⱑ ���ϴ� ���� �����Ͽ� ����ش�.
	// ������忡���� �Լ��� ������� �ʴ´�. (ó���� ���� �ʵ���)

	if (GET_SINGLE(CGameManager)->GetGameMode() == GM_ANIMATION_EDIT)
		return true;

		
	

		// �ϴ� �븻Ű�� ó���ϰ� ������ ���� �ϳ��� �ٲ㳪���� ���� ���� ó���� ���� ��.


		

	/*
		GET_SINGLE(CInput)->AddAxisKey("Move", VK_LEFT, -1.f);
	GET_SINGLE(CInput)->AddAxisKey("Move", VK_RIGHT, 1.f); //
	
	GET_SINGLE(CInput)->AddActionKey("Down", VK_DOWN); //
	GET_SINGLE(CInput)->AddActionKey("Jump", 'S'); //

	GET_SINGLE(CInput)->AddActionKey("Run", VK_LEFT);//
	GET_SINGLE(CInput)->AddActionKey("Run", VK_RIGHT);
	// ���� �ִ� �������� ó��.

	GET_SINGLE(CInput)->AddActionKey("Throw", 'F');
	GET_SINGLE(CInput)->AddActionKey("Shield", 'D');
	//  �ڽ��� �ɷ� Ǯ��
	// Ȥ�� ���� ����.
	

	// Ŀ�� ����
	GET_SINGLE(CInput)->AddActionKey("Attack", 'A');
	*/

	// �÷��̾��� �浹ü�� ������� ��ġ���� ���� ���� ���� �� �ֵ��� Size���� ��Ƶд�.
	// �ٵ� �� Size���� ���� �ִϸ��̼ǿ� ���� �޶��� �� �ִ� ���̱� ������ ������ ���� �ϳ� �ش�.

	// ����ũ ������Ʈ�� �����Ѵ�.

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





	// ������ ���ݹ���
	pCollider = AddCollider<CColliderRect>("KirbyAttackRight");

	pCollider->SetProfile("PlayerProfile");
	pCollider->SetChannel("Player");
	pCollider->SetRelativeInfo(tRightBottom.x, -80.f, tRightBottom.x + 290.f, 80.f);
	pCollider->SetEnable(false);
	SAFE_RELEASE(pCollider);

	// ���� ���� ����
	pCollider = AddCollider<CColliderRect>("KirbyAttackLeft");

	pCollider->SetProfile("PlayerProfile");
	pCollider->SetChannel("Player");
	pCollider->SetRelativeInfo(tLeftTop.x - 290.f, -80.f, tLeftTop.x, 80.f);
	pCollider->SetEnable(false);
	SAFE_RELEASE(pCollider);

	// ���� 160 ���� 290 �¿쳡 ����.


	// Stone Kirby : �׳� Body�� �̿��ϸ� �ǰڴ�.

	// SwordKirby 

	CColliderSphere* pSwordJumpAttack = CObj::AddCollider<CColliderSphere>("SwordJumpAttack");

	pSwordJumpAttack->SetRelativeInfo(0.f, 0.f, 120.f);
	// �÷��̾��� �߽���ġ�� �������� �Ͽ�, 100 �� �������� ���� ��ü�� �浹ü�� �Ѵ�.
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
		// �÷��̾��� ü���� 0�� ���.

		m_bBlend = false;
		// Blend�� ������ �Ǿ��ִ� ��쿡 ������ ���� �����Ƿ� ó���� Ǯ������� ���� �����Ѵ�.

		SetBlend(1.f, 5, 0, 255);

		m_iHP = m_iHPMax;

		m_iAttackedCount = 0;

		m_tPos = ((CMainScene*)m_pScene)->GetStartPos();

		GET_SINGLE(CSoundManager)->Play("Die", CT_STATE);

		// ��ġ�� �ٲ۴�.
		// ���������� ������ �صδ� �ý��� ���� ������ �̤�

		// �����ϰ� �ϴ��� .. 
		// �뷡�� �ٽ� �����Ѵ�.

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
		// ���� ���ķ�, Die�뷡�� ���� ���¶��, �ٽ� �뷡�� Ʋ���� �Ѵ�.
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


	// �޸��� ����Ʈ ����.
	// Į���̰� Dash�����ϴ� ��쿡�� �������� �ʴ´�.

	if (((m_bRun && !m_bJump) || m_bSlide) && !m_bDashAttack)
	{

		// �����̵��� ��쿡�� �ٴڿ� �پ� �ִ� ��Ȳ�̶� �׳� ���ָ� �ǰ�,
		// Run���¿��� Jump�� �ƴ� ��쿡�� ó���Ѵ�.
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

	// Ŀ�� ��üŰ.
	KirbyConversionKey();
	
	//if (m_bTake && m_bStop && !m_bBig)
	//{
	//	// �԰� ���� ��쿡�� false�� ���´�.
	//	// ���⼭ true�� �����ִٴ� ���� �԰� ���� �����ִ� ������ ���̴�.
	//	// ���� ��� �ִ� ���·� Down�� Stop�� ���� ���ȴٴ� ���� Eat�ִϸ��̼��� ȣ���ϰ� ��ٸ��� �����̴�.
	//	// ���� �ð� ���Ŀ� Ǯ���ش�.

	//	//  Big���°� Ǯ���ִµ�, Take�� ������ �Ǿ� �ִٴ� ���� �⺻ ���´� �ƴ϶�� ��.
	//	// ��, ������ �Ծ��� �� ���Ŀ� Big�� Ǯ�ȴ�(�Ծ���)
	//	// �� ���¿��� Stop�� true. 100 %��.


	//	m_fEatTime += fTime;

	////// ���Ծ��� �κ�. ���.

	
		// �˹�, �˾�, �˾� �� ó���� ���� ��쿡�� ó���Ѵ�.

		if (m_bJump)
		{
			// �����߿� A ����Ű�� ������ ���� �ʴٸ� ������ ó���� �Ѵ�.
			m_fJumpTime += fTime;
			m_fPowerTime += fTime;

			if (m_fPowerTime >= 0.1f)
			{
				m_fPowerTime -= 0.1f;
				m_fPower -= 200.f;
			}

			Move(Position(0.f, -1.f), m_fPower, fTime);

			// Jump Update ����ȭ.
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


			// ������ �ϸ鼭 vomit�� ó������� �ϴ� ��찡 ����.
			if (m_bVomit)
			{
				// ���� �� ���� �� ����ϰ� �� ��.
				m_fVomitTime += fTime;

				if (m_fVomitTime >= 0.3f)
				{
					m_bStop = false;
					m_bVomit = false;
				}
			}

		}

		else if (m_bBalloon && !m_bKnockUp && !m_bKnockBack) // Jump�� Ballon�� ���� ó���ȴ�.
		{
			// Balloon�� ����.
			m_fJumpTime += fTime;
			m_fPowerTime += fTime;

			if (m_fPowerTime >= 0.1f)
			{
				m_fPowerTime -= 0.1f;
				m_fPower -= 120.f;
			}

			Move(Position(0.f, -1.f), m_fPower, fTime);




			// Ballon ���¿����� GravityTime�� ������ �޾Ƽ��� �� �ȴ�.
			// �����Ǵ� �߷��� �ƴ�����, �⺻���� �ӵ������� ���ؼ�  200.f�� �о��ش�.
			m_fGravityTime = 0.f;


			Move(Position(0.f, 1.f), 700.f, fTime);


		}

		// �׳� �����ϴٰ� ������ ��쿡�� ���� �ӵ��� ����������, �Ա�Ұ� Falling�� true�� ���¶�� õõ�� �������� �Ѵ�.
		else if (m_bFalling)
			Move(Position(0.f, -1.f), 400.f, fTime);

		// ����.


		else if (m_bVomit)
		{
			// ���� �� ���� �� ����ϰ� �� ��.
			m_fVomitTime += fTime;

			if (m_fVomitTime >= 0.3f)
			{
				m_bStop = false;
				m_bVomit = false;
			}
		}




		if (m_bEat && m_eType == KIRBY_NORMAL) // �Ϲ� Ŀ�� ����.
		{
			// ��Ŵ�� �Ͼ ��쿡 ��Ű�� �ִϸ��̼� �ð��� �������ֱ� ���ؼ� 0.5s�� ���� �д�.

			m_fEatTime += fTime;

			if (m_fEatTime >= 0.5f)
			{
				m_bEat = false;
				m_bStop = false;
			}
		}



		else if (m_bSlide)
		{
			// ����.
			m_fSlideTime += fTime;
			m_fPowerTime += fTime;

			if (m_fPowerTime >= 0.1f)
			{
				m_fPowerTime -= 0.1f;
				m_fPower -= 70.f;
			}

			// ó������ �Ŀ����� ���� ū ���¿��� ���� �ӵ��� �̵�.
			// ���Ŀ��� ���� �۾����� �ӵ��� ������ �޾Ƽ� ���� �ӵ��� �̵�.

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
			// ����.
			m_fBreathTime += fTime;

			if (m_fBreathTime >= 0.25f)
			{
				// ���� ������ �������� �����ð� ������ �� ������ �Ͽ� �Ա��� �����ϴ� ���� ���´�.
				m_bStop = false;
				m_bBreath = false;
			}

			if (m_bFalling && (KEYPRESS("MoveRight") || KEYPRESS("MoveLeft")))
			{
				// �Ա��� �Ұ� �������� �ִ� ���¶��, �ش� ������ ���ؼ� ��¦ ����������.
				// �׸��� ������ �ʴ� ��츦 �Ǵ����ֱ� ���ؼ�, ��� �� ���̶� ������ �ִ� ��쿡�� ó��.
				// ���� �ٴڿ� �پ Falling�� false�� ��쿡�� �ش� ó���� ���� �ʴ´�.
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


		else if ((m_eType == KIRBY_NORMAL) && m_bStop && !m_bVomit)  // �Ϲ� Ŀ�� �����̴�.
		{
			// ������ ������ ���ķ� Take�� �� �ٸ� ó���� �ϱ� ���� �����̴�.
			m_fAttackTime += fTime;


			if (m_fAttackTime >= 0.3f) // 0.3s�� ������
			{
				// ����Ű�� �� ���¶��, ����Ű�� ���� ���� ���¶��, ��� Stop = true
				if (!m_bAttack)
				{
					
					ChangeDefaultClip();

					// ������ ���������Ƿ� ���ݿ� �浹ü�� ��������.
					SetColliderState("KirbyAttackRight", false);
					SetColliderState("KirbyAttackLeft", false);

					GET_SINGLE(CSoundManager)->Stop(CT_ATTACK);
					m_bAttackColliderEnable = false;
					m_bAttackEnd = true;
					m_fAttackEndTime = 0.f;
					m_fAttackEndLimitTime = 0.5f;
					m_bStop = false; // = �Է��� ����
					DisEnableTornadoEffect();


					m_fTime = 0.f;

					// ���� ����� ������ ������ ���������� �̰����� ó���� �ȴ�.
					// �����ð��̵�, ��� ������ �ִ� ��Ȳ�̵�.
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

		// �⺻ Ŀ��� ���� ó���� ������ �� ó���ϰ�, �ɷ��� �ִ� Ŀ����� ������ ó�� �Լ��� ���� ó���Ѵ�.
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
	// �÷��̾� �⺻ SRC
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

	// ���� ���� �����Դٴ� �Ҹ��� ���ٴ� ��.

	m_vecMonsterList.push_back(pObj);
	pObj->AddRef();

	// 2���� ���� ��� ��Ȳ�� ���� �ϱ� ���ؼ� Take = true
	if (m_vecMonsterList.size() == 2)
		m_bTake = true;

	// ���� �Ϳ� ���� ó��.
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

			// ������ Ŀ���̰�, ���� �ִϸ��̼��� ���ư��� ���� �ʴ� ��쿡�� �ִϸ��̼��� �ٲ۴�.

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
	// �����̴� ������ �ϰ� �ִ� ��Ȳ������ ���� Down�� �� ����.

	if (!m_bJump && !m_bBalloon && !m_bFalling && !m_bKnockUpEnd)
	{
		// �����߿��� ������ �� ������, ����� �ٲ����� �ʴ´�.

		if (!m_bBig)
		{
			// Big������ ��츦 ������ ó���Ѵ�.

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

				// ������ Ŀ�� ���������� �ϰ� �ִ� �����̰�, �� ���·� �����̴� ��쿡�� STopTime�� �ǵ����� �ʴ´�.

				
				/*if (m_bAttackEnd)
					return;*/
				// AttackEnd �� ��쿡�� ���� ���Ŀ� �ش� FALG�� ���� ����̴�.
				// �ٸ� ��쿡���� ��ø�Ǵ� �ý����̱� ������ �ش� Ű�Է°��� ����� ����.
				// �ƹ�ư, ���� ������ ���� ���� �� �ʱ����� ó���� ���´�.
				
			// ������ Ŀ�� ���������� �ϰ� �ִ� ����� �����̴� ó���� �����Ѵ�.
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
			// BIg ���´� �⺻ Ŀ�� ������.
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

	// ���� Ŭ�����Ŀ� UP�� �� ��츦 �ǹ��Ѵ�.

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

			// �ٿ� ���·� �����.
			m_bDown = true;

			RunDisEnable();
		}
		else // Ŀ�� ���¿��� ���� ���� ���� ���¶��, (���Ǿ� �ݺ����� �鸮�µ� ?)
		{
			// Big���·� �ٿ��� ���� ��쿡�� ���� ���̴�.
			m_bBig = false;

			

			/*
			(!m_bJump && !m_bSlide && !m_bAttack && !m_bBalloon && !m_bBig && !m_bStop 
			*/


			// �ɷ��� �ִ� ���͸� �Ծ����� Ȯ���ϰ� �ش� ���͸� �Ծ��ٸ� Conversion

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

				// �ɷ� �ִ� ���͸� ���� ��쿡�� �Ʒ��� ó���� ���� �ʴ´�.

			}
			else
			{
				// �Ϲ� ���͸� ���� ��쿡�� ������ Ŀ���� ó���� ���ش�.
				KirbyChangeClip(PL_EAT);
				// Eat Animation -> RETURN

				KirbyDefaultClip(PL_IDLE);


				m_bStop = true;

				// ������ �ִ� ��ü�� �Ծ���. ������ ���� �ʴ�.


				m_bEat = true;
				m_fEatTime = 0.f;

				GET_SINGLE(CSoundManager)->Play("Food", CT_EFFECT);
			}

			m_bTake = false;

		}


	}
	else
	{
		// �Ϲ����� Ŀ�� �ƴ� ���.
		// �� ��쿡�� Big���¿� �ش��ϴ� �Դ� ����� ����.
		// ���� �׳� Down���� ó���Ѵ�.


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

	// �����̵�
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
	if (m_bStop || m_bPrevent || m_bDown || m_bKnockBack || m_bKnockUp || m_bInputDisEnable) // �̹� ������.
		return;

	// ������ ��쿡�� DisEnable�̸� ó���� ���� �ʴ´�. �ٸ�, ǳ�� ���·� ���󰡴� ��쿡�� �̰��� ó������ ������ �������.
	// �� ���� ���ܷ� �ΰ� �������� �׳� �°� ���������� �����ϴ� ���̴�.(Ű�� �Է��� �� ���)

	// ������ ó�� �����ϴ� ��쿡
	if (!m_bJump && !m_bBalloon && !m_bFalling && !m_bKnockUpEnd)
	{
		// Down���� ���¿��� Jump�� �Ѵٰ� �ؼ� ��¥ Jump�� ó���ϸ� �� �ȴ�.(Slide���)


		// ���� ���� �ٲ㼭 ���� ó�� �� �ǰ� �ٲٸ� �ȴ�.
		// ������ ���� ���� ���¿��� �����ϸ� Ǯ���� ����.

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

				// �ɷ��� �ִ� Ŀ���� ��쿡�� Big���°� �����Ƿ� �׳� ������ ó�����ش�.
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
			// ������ �� ���Ŀ� �ö󰡰� �ִ� ���. (�ٽ� ����)�� �ν����� �ʵ��� �Ѵ�.
			if (m_bJump && !JumpDown())
				return;

			// ������ ���� ���¶�� �� �� ����. �⺻Ŀ���Ͻ�.
			if (m_eType == KIRBY_NORMAL && m_bBig)
				return;

			// �˹��end �̸鼭 ǳ�����°� �ƴ� ��쿡 ó������ �ʴ´�.
			if (m_bKnockUpEnd && !m_bBalloon)
				return;

			// FLY ���¸� �⺻���� �����ϰ�, FLYUP �ִϸ��̼��� ����.

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


			// ������ ���ؼ� �ִ� ���̱��� �ö� ���Ŀ� ó���� �ǵ��� JumpDown�� �̿��Ѵ�.
			// ���� ���߿� �ٽ� ����Ű�� ���� ��쿡 ǳ�� ���°� �Ǿ�� �Ѵ�.
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
	// �����߿� Run�� ������ ���� ����.
	
	if (m_bStop || m_bPrevent || m_bJump || m_bBalloon || m_bKnockBack || m_bKnockUp || m_bKnockUpEnd || m_bInputDisEnable)
		return;
	// ó���� ���� Ȥ�� �������� ������ ��
	// ������ üũ�� �Ѵ�.
	// ���⼭ �ι� ���� Ű�� �ν����ִ� ���̴�.
	// ������ ������, �������� 2�� ���� ���. ���������� ���� �Ǿ��ִ�.
	// ������ ������ ���� 2�� ������ �������� ����.
	// �� �� �� �� ���� �ֱ� ������ �ƹ�ư ó���Ѵ�.

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

	// ���࿡ �ι� ������ ���� ó���� �ϴµ�,
	// �ι� ���� ó�� ���Ŀ� �޸��� ���� �ƴϴ�. �� ���� ó����
	// � ������ ���� �����غ���. !!
}

void CPlayer::Throw(float fTime)
{
		// FŰ�� ���� ����̴�.
	

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
		
		// �����߿� ��� �Ұ�.
		// ���𰡸� �԰� �ִ� ���¿��� �� �� �� ����.
		return;

	// ������
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
	// Normal Kirby ���� �Լ�.
	if (m_eType != KIRBY_NORMAL)
		return;

	// ������ ���� ������ �������� �ٽ� ������ ����ϱ� ���� �ð��� �ش�.
	if (m_bPrevent || m_bVomit || m_bKnockBack || m_bKnockUp || m_bAttackEnd || m_bInputDisEnable)
		return;


	if (m_bBig && KEYDOWN("Attack") && !m_bKnockUpEnd)
	{

		// ������ �ϰ� �����鼭, ���� ���� �ö󰡰� �ִ� ��쿡�� ó������ �ʴ´�.
		// ��� ��� ���� �ö� �Ŀ� ���� �� �ֵ��� �ϴ� ���̴�.
		if (m_bJump && !JumpDown())
			return;

		RunDisEnable();
		m_bTake = false;
		m_bBig = false;
		m_bVomit = true;
		m_fVomitTime = 0.f;
		//m_bFalling = true;

		// ��� ��� (Stop) -> idle

		KirbyChangeClip(PL_VOMIT);
		KirbyDefaultClip(PL_IDLE);

		m_eMonsterType = MT_NORMAL;


		// �Ա� �ð�ó�� ��� ������ �ش�.
		m_bStop = true;
		m_bBreath = true;
		m_fBreathTime = 0.f;


		// �ش� ������ ���� �����̴� ���� �����Ѵ�.

	
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
		

		// �Ա� ����.
		m_bFalling = true;
		m_bBalloon = false;
		m_bAttack = false;

		CreateBreathEffect();

		return;
	}
	

	// ������ �߿� ���� ó��. ���� ���� ��� return.

	if (m_bBig || m_bSlide || m_bKnockUpEnd)
		return;
	// Big�� ���� ó���� ������ ���ְ� �ִ�. (��� ��)
	// Big  ���·� �Ʒ��� ���� ó���� ���� �ʴ´�.
	// ���Ƶ��̴� ��ǿ� ���� ó���� �ϱ� ���� ���������� �⺻Ŀ��.



	// Stop false��� ���� ó�� ������ ������ ���̴�.
	if (!m_bStop)
		m_fAttackTime = 0.f;




	m_bStop = true;
	m_bAttack = true;


	RunDisEnable();

	if (m_bJump)
		m_ePrevDir = m_eDir;

	
	KirbyChangeClip(PL_ATTACK);
	KirbyDefaultClip(PL_IDLE);

	// �÷��̾ �ٷκ��� ������ ���� �浹ü�� Ȱ��ȭ.
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
		// ���� ���� ���� ���� ��쿡��.. �ѹ��� ������.
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
	// �����߿� AŰ�� ���� ���. �Լ��� ���� �޾Ƴ��� ��ġ�� ������ ���� �ӵ��� �����̰� �Ѵ�.
		// ���� ó���� �ϴٰ� ������ ���� ���(�ٴڿ� ���� ����) ������ �ƴϹǷ� �ش� ó���� ����ȴ�.
		// Jump�� ��쿡�� ����Ű�� �Է��� ���� �ʴ´�. �׷��� �ӵ��� ���ڱ� �پ��� �ȴ�.

	

	if (m_bKnockBack || m_bKnockUp || m_bKnockUpEnd)
		return;
	// ������ �� ���Ŀ� ��� �ð��̶�� ����Ű�� ���� ó���� ���ƾ� �Ѵ�.
	// ������ �ܼ��� Ű�� ���������� ���� ���θ� �Ǵ��ϱ� �����̴�.

	if (KEYPRESS("Attack") && !m_bBig && !m_bVomit)
	{
		// Big ������ ��쿡�� �������� �ſ� �̻��ϴ�.// 

		if (KEYPRESS("MoveLeft") || KEYPRESS("MoveRight"))
		{
			if (m_ePrevDir == SD_LEFT)
				Move(Position(-1.f, 0.f), 300.f, fTime);

			else
				Move(Position(1.f, 0.f), 300.f, fTime);
		}

		// �Ѵ� ������ �ִ� ��쿡�� �� �߿� �ϳ��� ó���ϰ�,
		// ������ ���� ��쿡�� �� �ڸ�����.

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

		// destoy�� �͵� �ִµ� �ϴ��� �������� �ʴ´�.
		// ������ ��ġ�� �ű��.

		// ���Ϳ��� Ŀ������� ������ ���Ѵ�.
		float fAngle = CMath::GetAngle((*iter)->GetPos(), GetPos());
		// �� ��ü ��� Center�� �������� �ϰ� �����Ƿ� ��� ����.

		(*iter)->MoveAngle(fAngle, 700.f + (m_fTime * 1000.f), fTime);

		float fDist = CMath::GetDist((*iter)->GetPos(), GetPos());

		if (fDist < 50.f)
		{

			(*iter)->SetActive(false);


			KirbyChangeClip(PL_TAKE); // ���� �ִϸ��̼�.
			KirbyDefaultClip(PL_BIG_IDLE); // ���Ŀ� �⺻�� BIG IDLE

			switch ((*iter)->GetObjectType())
			{
				case OT_NORMAL_MONSTER:
				{
					// ���� ���� ���Ͷ�� ���� ������ Ÿ�Կ� ���� ó��.
					// �븻�̵�, �ɷ��̵�..

					CMonster* pMonster = (CMonster*)(*iter);
					// ������ ���� ���� �ɷ��� �ƴϴ�. (�ϴ� �޾Ƽ� ����)
					if (m_eMonsterType == MT_NORMAL)
						m_eMonsterType = pMonster->GetMonsterType();
					else
					{
						//  ������ ���� ���� �ɷ��̴�.
						// ���� �Դ� �Ϳ� ���� �ٸ��� ó���Ѵ�.
						// ���� ���� ���� �ɷ��̶�� �������� �ɷ��� �ְ�, 
						// ���� ���� ���� �ɷ��� �ƴ϶�� �̹��� ���� ���� �ѱ��.
						if (pMonster->GetMonsterType() != MT_NORMAL)
						{
							// ����.

							m_eMonsterType = (MONSTER_TYPE)(rand() % 4 + 1);
							// 1 ~ 4 �ɷ��� �������� ����.
						}

						// ������ ���� ���� �ɷ��̰�, ���� �Դ� ���� �ɷ��� �ƴ� ���̶��
						// ����� ������ ����.
					}

					// ���͸� ���� ��쿡�� �ش� ���͸� ���̰�
					// �׸��� ���� ���� ó���Ѵ�.
					// ���࿡ �Ÿ��� ���� ���� ��쿡�� ���� ���͸� ó���Ѵ�.
					// �̷��� �ϳ��ϳ��� ó���� �غ���, Ȯ���� �ϴµ�
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
						//  ������ ���� ���� �ɷ��̴�.
						// ���� �Դ� �Ϳ� ���� �ٸ��� ó���Ѵ�.
						// ���� ���� ���� �ɷ��̶�� �������� �ɷ��� �ְ�, 
						// ���� ���� ���� �ɷ��� �ƴ϶�� �̹��� ���� ���� �ѱ��.
						if (pStar->GetAbilityType() != MT_NORMAL)
						{
							// ����.

							m_eMonsterType = (MONSTER_TYPE)(rand() % 4 + 1);
							// 1 ~ 4 �ɷ��� �������� ����.
						}

						// ������ ���� ���� �ɷ��̰�, ���� �Դ� ���� �ɷ��� �ƴ� ���̶��
						// ����� ������ ����.
					}
				}

				// Star�� ���� �ɷ��� Normal�� ���� �ְ�, Normal�� �ƴ� �� �� �ִ�.

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
	
	// ���Ͱ� ��� ���� ��쿡�� true�����̴�.

	if (MonsterDie)
	{
		// �־�� ����Ʈ�� �ش��ϴ� ���Ͱ� ��� ���� ������ ��쿡�� ���� ���¿� ���ؼ� ó���ϰ� List�� ������.
		// �̷��� ó���� �ϸ�, ����� ��� ���� �ִ� 2������ ��� Ŀ�� ���� ���°� �Ǳ� �� ������ ����Ʈ���� �������� �ʾƼ�
		// �������� ���������� �Դ� ������ ���� �� ���� ���̴�.
		// �����ٳ� ���۷��� ī���͸� �̿��ϰ� �־ ������ �ϵ� ���� ��.
		// ���� ���Ŀ� vomit �̸� ��� ���� ����
		m_bBig = true;
		RunDisEnable();
		m_bStop = false;

		DisEnableTornadoEffect();

		Safe_Release_VecList(m_vecMonsterList);

		// ������ ���������Ƿ� ���ݿ� �浹ü�� ��������.
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
	// �Ϲ� ������ ��쿡�� true�� ���ͼ� ���⼭ ������ ���̰�
	// ������ ��쿡�� false�� ���ͼ� �̰��� �Ѵ´�.
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


		// �Ա� ����.
		m_bFalling = true;
		m_bBalloon = false;

		CreateBreathEffect();

		return;
	}

	// ǳ�� ���°� �ƴѵ�, ����Ű�� ���� ���.
	// ��, ��� ����, ���� ����, �⺻������ ��� ���� �ȴ�. ��!

	m_bStone = !m_bStone;
	RunDisEnable();


	if (m_bStone)
	{
		m_bAttack = true;
		m_fAttackTime = 0.f;
		// ���� �Ǵ� ���. 
		m_bStop = true;

		StoneKirbyChangeClip(ST_ATTACK);

		// �� ���°� �� ���Ŀ� ��� ó���� �����ٰ�, �ٽ� ����Ű�� Ǭ�ٸ� idle����� �Ǹ鼭 ������ �� �ְ� �ȴ�.

		CEffect* pEffect = CObj::CreateCloneObject<CEffect>(PINKSTONE, "PinkStone", "Default", m_pScene);
		pEffect->SetTarget(this);
		pEffect->SetRelative(0.f, 0.f);

		// ī�޶� 1�� ���� ����.
		GET_SINGLE(CSceneManager)->SetSway(1.f);

		m_bStoneEffect = false;
	}

	// 0.3s 

	else if (m_fAttackTime >= 0.5f)
	{
		m_bAttack = false;

		m_bStop = false;
		// ���� �Ǿ��ٰ� �� ���°� Ǯ�� ���.

		m_bJump = true;
		m_fPower = 1800.f;

		StoneKirbyChangeClip(ST_HIT);
		StoneKirbyDefaultClip(ST_IDLE);

		GET_SINGLE(CSceneManager)->SwayDisEnable();

		SetBlend(0.3f, 1); // 1�ʰ� �����ð��� �ش�.
		// ���࿡ ���� �־��ٸ� ��������.

	}
	else
		m_bStone = true;

	// Stone = false�� ���°� �� ���Ŀ� (Ǯ���� ����) ���ݽð��� Ȯ���ؼ� 0.3s ���Ķ��
	// �ش� ó���� �����ϰ� �����̶�� �������� �ʴ´�.
	// �̶� Stone = false�� �����̹Ƿ� stone = true�� �ش� (������ ó���� ���� �ʾ����ϱ�)

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

	// ���������� �ٴڰ� �浹�� ���� ���� ��쿡��.
	if (m_bStone && !m_bStageCollision && !m_bLineCollision )
		Move(Position(0.f, 1.f), 500.f, fTime);

	if (m_bStone && !m_bStoneEffect && (m_bStageCollision || m_bLineCollision))
	{
		// ���� ���·�, ����Ʈ�� false�� ������ ���� �浹�� ��쿡 (��, ������°� �� ���Ŀ�
		// ������ �浹�� ù��° ��츦 �ǹ��Ѵ�.
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


		// �Ա� ����.
		m_bFalling = true;
		m_bBalloon = false;

		CreateBreathEffect();

		return;
	}
	else if (m_bJump || m_bFalling)
	{
		// ���� ����.

		// ������ ���� ���°� none�� �ƴ϶�� ó������ �ʴ´�.
		if (m_eSwordState != SA_NONE)
			return;

		SwordKirbyChangeClip(SW_JUMP_ATTACK);
		SwordKirbyDefaultClip(SW_IDLE);

		// ���� ���� �浹ü�� Ȱ��ȭ.
		SetColliderState("SwordJumpAttack", true);

		m_eSwordAttack = SA_JUMP_ATTACK;
		m_eSwordState = SA_JUMP_ATTACK;

		m_bJumpAttack = true;

		GET_SINGLE(CSoundManager)->Play("JumpAttack", CT_EFFECT);

		

		return;
	}
	else if (m_bRun)
	{
		// Dash ������ ������ ����̴�.

		SwordKirbyChangeClip(SW_DASH_ATTACK);
		SwordKirbyDefaultClip(SW_IDLE);

		// �뽬�� ������ Slide�� ó���� �ٸ� ���� ����. �׳� ���� �����̿��ϸ� �ȴ�.

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

	// ��� ������ �������� ���·� �⺻ ������ �� �� ������ �Ѵ�(ĵ��)
	if (m_bDashAttack)
		return;

	RunDisEnable();


	// �⺻ ���ݿ� ���� ó���̴�.

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


		// Ani Bullet ����Ʈ ����.
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

	//		// �޺��� ������ ���.
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

	// �ִϸ��̼��� ó���� �ϴ� �����̴�. ���� ��, SA_ONE ���¶�� ONE �ִϸ��̼Ǳ��� ȣ���϶�� �ǹ��̰�,
	// two��� two���� ��� ȣ���ؾ� �Ѵ�.

	if (m_eSwordAttack != SA_NONE)
	{
		// NONE ���°� �ƴ϶�� �Ҹ��� �ϴ� ȣ���� �ִϸ��̼��� �����Ѵٴ� �ǹ��̴�.

		switch (m_eSwordState)
		{

			case SA_JUMP_ATTACK:
			{
				string strCurrent = m_pAnimation->GetCurrentAnimation();

				if (strCurrent == g_pSwordAnimation[SD_LEFT][SW_JUMP_ATTACK] ||
					strCurrent == g_pSwordAnimation[SD_RIGHT][SW_JUMP_ATTACK])
				{
					// ���� �ش� �ִϸ��̼��� ������ �ִٴ� �ǹ��̴�.
				}
				else
				{
					// ���� ���� �ִϸ��̼��� ���� ��Ȳ�̴�.
					SetColliderState("SwordJumpAttack", false);
					// �浹ü ��Ȱ��ȭ.

					m_eSwordAttack = SA_NONE;
					m_eSwordState = SA_NONE;

					//SetAttackEndTime(0.5f);

					m_bJumpAttack = false;
					m_bStop = false;
				}
			}
			break;

			case SA_ONE:
			{// ���� ONE �ִϸ��̼��� ȣ���ϰ� �ִ� ���´�.
				// �� �ִϸ��̼��� ���� ��쿡 ���� �ִϸ��̼����� ������ �Ǿ� �ϴ��� Ȯ���ϴ� ó���� �ʿ��ϴ�.

				string strCurrent = m_pAnimation->GetCurrentAnimation();

				if (strCurrent == g_pSwordAnimation[SD_LEFT][SW_ATTACK1] ||
					strCurrent == g_pSwordAnimation[SD_RIGHT][SW_ATTACK1])
				{
					// ���� �ش� �ִϸ��̼��� ������ �ִٴ� �ǹ��̴�.
				}
				else
				{
					// ���� �ش� �ִϸ��̼��� ���� ���̴�.
					// �� ��Ȳ���� ȣ���ؾ��ϴ� �ִϸ��̼��� SA_ONE�� �Ѵ��� Ȯ���ϸ� �ȴ�.
					if (m_eSwordAttack > SA_ONE)
					{
						// ��� two������ �ִϸ��̼��� ȣ���϶�� �ϴ� ��Ȳ�� ���̴�.
						// �� �ִϸ��̼��� �������ش�.
						SwordKirbyChangeClip(SW_ATTACK2);
						SwordKirbyDefaultClip(SW_IDLE);

						m_eSwordState = SA_TWO;


					}
					else
					{
						// ���� ������ �ִ� �ִϸ��̼��� �����µ�, ������� ȣ���� �϶�� ��Ȳ�� ���̴�.
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
						// ���� ������ �ִ� �ִϸ��̼��� �����µ�, ������� ȣ���� �϶�� ��Ȳ�� ���̴�.
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

	if (m_bAttackEnd || m_bPrevent || m_bVomit || m_bAttack) // ����ũ�� ��쿡�� �����ϰ� �ִ� �߿� �ٽ� ������ �� �� ����.
		return;


	if (m_bKnockBack || m_bKnockUp || m_bKnockUpEnd || m_bInputDisEnable)
		return;

	
	if (m_bBalloon)
	{

		KirbyChangeClip(PL_VOMIT);
		KirbyDefaultClip(PL_IDLE);


		// �Ա� ����.
		m_bFalling = true;
		m_bBalloon = false;

		CreateBreathEffect();

		return;
	}


	// ������ �߿� ���� ó��. ���� ���� ��� return.

	if (m_bSlide)
		return;




	// ������ ���۵� �������� false �� �ְ�...
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
		// ó�� ������ �����ϴ� ����̴�.
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



		// KIRBY�� ��ȯ�ϴ� ������ �ʿ��ϴ�.
		break;
	}

	// ����ũ ������ Ư¡ : ����.
	GET_SINGLE(CSceneManager)->SetSway(2.0f);


	// ����ũ ������ ���۵ǹǷ�, Kirby�� �� ���� �÷��ش�.
	GET_SINGLE(CSceneManager)->LayerUp("Default", "Kirby");

	
	// ����ũ ������Ʈ�� Ȱ��ȭ ��Ų��.

	// ����ũ�� ���� (ī�޶�)�� ������ Rect�� �浹����, �浹�̶�� ���̴� ó���� �ϰ� �ƴ� ��쿡�� �׳� �ѱ��.
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

	// �÷��̾ �ٷκ��� ������ ���� �浹ü�� Ȱ��ȭ. ����ũ �浹ü Ȱ��ȭ ?! 
	//switch (m_eDir)
	//{
	//case SD_LEFT:
	//	SetColliderState("KirbyAttackLeft", true);
	//	break;

	//case SD_RIGHT:
	//	SetColliderState("KirbyAttackRight", true);
	//	break;
	//}

	//CreateTonadoEffect(); ����ũ ����Ʈ ����. ?!
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

	// ������ �ϰ� �ִ� ��쿡�� MikeUpdate�� ȣ�����ش�.


	if (m_bAttack)
	{
		// ������ �����ߴٸ�, ��� ȭ�鿡�� ���ߵ��� �Ѵ�.
		
		m_bJump = false;
		m_bRun = false;
		m_bFalling = false;

		

		// �⺻������ ��ž �����̱� ������ �ظ��� ������ ���ϵ��� �Ǿ� ������, �߷½ð��� �� �����Ӹ��� �����ش�.
		// �翬�� ������ on ���¶�� ���ִ� ó���� �ʿ��ϴ�.

		m_fAttackTime += fTime;

		if (m_fAttackTime >= 2.2f)
		{
			// ������ ���� �������� �ٽ� �浹ó���� �Ѵ�.
			// �׷��� �߷��� �ٽ� ó���� �Ǵ� �������� �ٽ� �浹ó���� �ϴ� ���̴�.
			SetColliderState("KirbyBody", true);
			m_bStop = false;
			m_bAttack = false;
			m_bGravity = true;
			// ������ ������ ���ķ� ����� �ð��� ���� ��쿡 �ٽ� ���� ��, �ٸ� ������ �� �� �ֵ��� �Ѵ�.
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

			// ü�� ȸ�� ȿ��.

			m_iHP = m_iHPMax;
			CObj::StarEffectCreate(GetPos());

			m_fTime = 0.f;

			GET_SINGLE(CSoundManager)->Play("GoodSleep", CT_EFFECT);
		}
	}
}


void CPlayer::PlayerAttack(CCollider * pSrc, CCollider * pDest, float fTime)
{

	// �ش� ó���� ���� �⺻ Ŀ��� ���õ� ���̴�.
	if (m_eType != KIRBY_NORMAL)
		return;

	else if (m_bVomit)
		return;


	if (m_bKnockBack || m_bKnockUp || m_bKnockUpEnd || m_bInputDisEnable)
		return;

	// Vomit ���¶��, ���͸� �԰� ���� ���¸� ���Ѵ�.
	// �� ������ �� �ٸ� ������Ʈ�� �� �Ծ��� ����ϴ�..

	// ������ �Ͼ�� Stop����. ���ÿ� Prevent�� ���� ����. Take�� �������� �ɾ��ִ� ����
	// �ѹ� �ۿ� ������ ����.


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

			// �ش� �Լ��� ȣ��ȴٴ� ���� �÷��̾ ���͸� ���� ����̴�. 
			// �ߺ� ���� / ����ִ� ��쿡��.
			// 2�������� �Ա� ����.
			if (pDestObj->IsActive() && m_vecMonsterList.size() < 3)
			{
				AddMonsterVector(pDestObj);

			}
		}

		// ������ ���� �� ����.
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

	// ���ݿ� ���� ó���� �� ���̱� ������ �̰����� ������ ��ü�� �����̴�.

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
		// Star�� ��쿡�� �ش� �浹ü�� �浹ó���� ���� �ʴ´�.
		return;
		// ���Ͱ� ���� �浹�� �Ͼ�� ����.... �Ʒ����� ó���Ѵ�. 
		// ���� �̰������� ó������ �ʴ´�.

	case OT_MONSTER_ATTACK:
		// �̰����� ���� �Ѿ˿� ���� ó���� �Ѵ�.
		

		// �÷��̾ ������ ������ ���� �� �ִ� ���.
		if (m_bPrevent || m_bSlide)
		{
			pDestObj->SetActive(false);
			CObj::StarEffectCreate(pDestObj->GetPos(), 1.f, 100.f);
			return;

		}

		switch (m_eType)
		{

		case KIRBY_NORMAL:

			// �� ��츦 ���� ���� ������ �Ѵ�.

			//if (GetBlend())
			//	return;
			//// �����̸� ó�� X

			//if (m_bBig)
			//{
			//	m_bBig = false;
			//}

		

			break;


			// �ڰ� �ִ� ���·δ� ������ ������ �µ��� ���� �ʴ´�.
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

		// ��� Ŀ�� ���Ͽ� Prevent  �����̸� �´� ó���� ���ְ�, ���� Ŀ�� ���ؼ��� �� ���¶�� ���� �ʵ��� �Ѵ�.

		// Ŀ���� �� ���¿� ����, �Ѿ��� ���ִ� ó���� �ؾ��ϴ� ��쿡 ���ؼ� ó�����ְ�, �� ���Ŀ��� �¾ƾ� �ϴ� ��Ȳ���� 
		// �÷��̾ ���� �������� Ȯ���ϰ� ���� ���¶�� ó���� ���� �ʴ´�.

		// Ŀ�� �¾ƾ� �ϴ� ��Ȳ�ε� Ŀ�� ���� ���¿� �ش��ϴ� ��쿡�� ó������ �ʴ´�.
		// ����� ���Ϳ� ���� �浹�� �Ͼ�� ���� ������ ���ݿ� �´� ��찡 �ִµ�,
		// ���Ϳ� �浹�� �Ͼ�� ��쿡�� �̸� �Ʒ����� Blend�� ���� ó���� ���ְ� �ִ�.
		// �ƿ� ������ �и��߱� ������ �̰����� ����ó���� ����� �Ѵ�.
		if (GetBlend())
			return;


		MonsterCollision(pDestObj);
		
		return;
	}




	// ���Ͱ� ������ �ƴ� ���..

	CMonster* pMonster = (CMonster*)pDestObj;



	// Ŀ�� �����ϴ� ���. ���� ����

	// ��� Ŀ���� ����

	if (m_bPrevent)
	{

		switch (pDestObj->GetObjectType())
		{
			case OT_NORMAL_MONSTER:
			{
				SIDE_DIR eCollisionDir = SD_LEFT;

				if (m_tPos.x < pDestObj->GetPos().x)
					eCollisionDir = SD_RIGHT;



				// ���Ϳ� ���� �˹�ó���� �ʿ��� �� ����. 
				// ���ƴٴϴ� ��ü�� ���ƴٴ��� �ʴ� ��ü.


				if (pMonster->GetTag() == "Twizzy")
				{
					pMonster->SetKnockBack(1.f, 550.f, eCollisionDir);
					if (((CTwizzy*)pMonster)->GetJump())
					{
						// �߷��� ���� ���¿��� ������ �ϴϱ� ���� ������ �ȴ�.
						// ������ �������� ���������� �����Ѵ�.
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
			
			// ������ �ϴ� ó���� ������ �ƴ� ������ ��쿡�� ó���� �Ѵ�.

			break;

			case OT_BOSS_DDD:
			{

				// �ٵ� �� ó���� �ϴ� ��Ȳ�� ���� ��Ȳ���� �ع����� �� �ȴ�. ��,
				// ������ ���� ������ �ϰ� �ִ� ���¿��� �̰��� ���ϸ� �� �ȴٴ� ���̴�.
				// �׶��� �浹�� ���� ���󰡴� ó���� �ؾ��Ѵ�.

				CDDD* pDDD = (CDDD*)pMonster;

				switch (pDDD->GetState())
				{
					case DS_IDLE:
					case DS_WALK:
					case DS_RUN:
					case DS_JUMP_ATTACK:
						// �� ��쿡�� �˹� ó���� �ؾ� �Ѵ�. (�������� ���� �ʰ� �Ʒ��� ó��)
					{
						SIDE_DIR eCollisionDir = SD_LEFT;

						if (pMonster->GetPos().x < m_tPos.x)
							eCollisionDir = SD_RIGHT;

						// ������ �����ʿ� �ִ� ��� �������� ���󰡾� �Ѵ�.
						// ������ ���ʿ� �ִ� ��� ���������� ���󰡾� �Ѵ�.


						SetKnockBack(0.8f, 300.f, eCollisionDir);
						// �̷��� �ؼ�, �������� �԰� ��.
					}
					break;


					case DS_ATTACK:

						// ������ ������ �Դ� ��Ȳ�̴�.
						MonsterCollision(pMonster);

						m_bPrevent = false;

						return;
				
				}

			
			
				// �������״� �������� �� ������.
				
			}
			// ������ ��쿡�� ���󰡴� ó���� ���� �ʴ´�.
			// ������ ������ �ʰ� Ŀ�� ���󰣴�.

			break;
		}
		

		pMonster->SetBlend(0.7f, 2);

		pMonster->Damage(1);



		return;
	}

	else if (m_bSlide && !m_bDashAttack) // Slide ����� ���� ����ϰ� �־, ���� ��츦 ó���ؾ� �Ѵ�.
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
						// �߷��� ���� ���¿��� ������ �ϴϱ� ���� ������ �ȴ�.
						// ������ �������� ���������� �����Ѵ�.
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
			// �����̵带 ���ؼ� ������ �������� ���� �ʴ´�.


			{


				CDDD* pDDD = (CDDD*)pMonster;

				switch (pDDD->GetState())
				{
					case DS_IDLE:
					case DS_WALK:
					case DS_RUN:

						// �� ��쿡�� �÷��̾��� �˹�ó���� �ϰ� ������.
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
						// ���� ���� �߰��� Slide�� ���� ����̴�.
						// Attack�߿� Slide�� ���� ���. 
						// �浹 ó���� �ع�����.
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

		case KIRBY_SLEEP: // �ڰ� �ִ� ���� Stone ������ �� ó�� ó���Ѵ�.
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
					// ������ �˹�ó��.

					pMonster->SetKnockBack(0.8f, 400.f, eCollisionDir);
					pMonster->MoveDisEnable(1.f, eCollisionDir);

					// �׳� ���� ��쿡 �浹.
					pMonster->SetBlend(0.7f, 2);
					pMonster->Damage(3);
				}
				break;

			case OT_BOSS_DDD:

				if (m_bJump)
				{
					// Stone ���·� �������� �ִ� ��쿡�� ó���� �ϵ��� �Ѵ�.
					// �� �ܿ� �׳� ������ �ִ´ٰ� �ؼ� �浹ó���� ������ �ʴ´�.
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
				// ���� �����ε�, �Ϲ�  ���Ϳ� �浹�� �� ��쿡�� �̰����� ó���� ���� �ʴ´�. ( �浹�� ������ �����̴�. )

			case OT_BOSS_DDD:

				// ���� ������ �ϴٰ�, �߽ɰ� ������ �浹�� �� ��쿡�� �浹ó���� �ع�����.
				m_bJumpAttack = false;
				MonsterCollision(pDestObj);

				return;


			}
		}
	

		break;
	}




	// ���Ϳ� ���� ���󰡾� �ϴ� ���.

	// ���� Ŀ�� �����ð��̶��, ó������ �ʴ´�.
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
				// ���� ���� Ű�� ���� ��쿡 �ٸ� ������� ��ȯ�� �ع�����.
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
	// �ش� �浹ü �Լ��� ȣ���� �Ǿ��ٴ� �Ҹ��� ������ Ŀ��� ���� ������ �ϰ� �ִ� ��Ȳ�� �ǹ��Ѵ�.
	
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
		return; // �� �ʿ䰡 ����.

	case OT_MONSTER_ATTACK:
		// �̰����� ���� �Ѿ˿� ���� ó���� �Ѵ�.
		pDestObj->SetActive(false);
		CObj::StarEffectCreate(pDestObj->GetPos(), 1.f, 100.f);

		return;
	}


	if (pDestObj->GetBlend())
	{
		// �ش� ������Ʈ�� ���� �ð��̶��...
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

				// ó���� ���� �ʴ´�. Hit
				// ������ Ű�� �����Ƿ�, �������� ���� ����, �� ���·� �߽ɿ� �浹�̸� ���󰣴�. �� �Ϸ��� �ߴµ�
				// ���� �ð��� ���� ������ �ٷ� �浹�� ���� �ʹ� ���.
				
				((CMonster*)pDestObj)->Damage(2);
				pDestObj->SetBlend(0.2f, 1);

				break;
			}

		

		}
		break;
	}

	
	GET_SINGLE(CSoundManager)->Play("JumpAttackHit", CT_EFFECT);

	// ���Ϳ� �浹�� �Ͼ ���
	// ������ ���� or ���Ͱ� ������ ��Ȳ�̴�.
	
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

	// ���ҽ��� ���� ����ϴ� �κ��̹Ƿ�, �ؽ�ó �̸��� EasyKirbyLeft Right�� �����Ѵ�.
	// �ҽ� �ϼ�.


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

	// �÷�Ű�� ������ �� �� Clip���� �ִ� �� ������ �̸� �����صδ� ������ ����.

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

	// AttackLeft AttackRight �������� ������ OR �������� ����.

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
		SW_ATTACK1, // ���ݿ� �޺� �ý��� �ִ� �� �ƽ� ? ���⿡ ����Ʈ ���� �ѹٴ� ���� �� ����.. ������ �Ф�
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






