#include "DDD.h"
#include "../Collision/Collider.h"
#include "../Animation/Animation.h"
#include "../Object/Player.h"
#include "../Core/GameManager.h"
#include "../Core/SceneManager.h"
#include "../Core/Input.h"
#include "../Math.h"
#include "../Object/MoveStar.h"
#include "../Collision/ColliderRect.h"
#include "../Sound/SoundManager.h"
#include "../Scene/MainScene.h"

enum DDD_CLIP
{
	D_IDLE,
	D_WALK,
	D_JUMP,
	D_BALLOON,
	D_BALLOON_ATTACK,
	D_ATTACK,
	D_HIT,
	D_VOMIT,
	D_RUN,
	D_END
};

const char* g_pDDDAnimation[SD_END][D_END] =
{
	{"DDDIdleLeft", "DDDWalkLeft", "DDDJumpLeft", "DDDBalloonLeft", "DDDBalloonAttackLeft", 
	"DDDAttackLeft", "DDDHitLeft", "DDDVomitLeft", "DDDRunLeft"},

	{"DDDIdleRight", "DDDWalkRight", "DDDJumpRight", "DDDBalloonRight", "DDDBalloonAttackRight",
	 "DDDAttackRight", "DDDHitRight", "DDDVomitRight", "DDDRunRight"}

};

CDDD::CDDD()
{
	m_fWalkTime = 0.f;

	m_iJumpCount = 0;
	m_bJump = false;
	m_fPower = 0.f;
	m_fTime = 0.f;
	m_fJumpSpeedX = 0.f;


	m_pPlayer = nullptr;

	m_bJumpTime = false;
	m_bFalling = false;
	m_fPowerTime = 0.f;
	m_bJumpTimeEnable = false;
	m_fJumpTime = 0.f;
	m_bDDD = false;
}


CDDD::CDDD(const CDDD & obj) :
	CMonster(obj)
{
}

CDDD::~CDDD()
{
	SAFE_RELEASE(m_pPlayer);
}


void CDDD::InitIdle()
{
	
	m_eState = DS_IDLE;
	m_fTime = 0.f;
	m_fPlayTime = 1.f;
	m_bAttack = false;

	ChangeDDDClip(D_IDLE);
	DefaultDDDClip(D_IDLE);

}

void CDDD::JumpAttackMoveStar()
{
	// 왼쪽에 대한 MoveStar를 생성한다.	// Blue Ball
	CEffect* pBlueBall = CObj::CreateCloneObject<CEffect>(BLUE_BALL, "BlueBall", "Default", m_pScene);

	pBlueBall->SetBlend(0.5f, 100, 200, 150); 

	CMoveStar* pMoveLeft = CObj::CreateCloneObject<CMoveStar>(MOVE_STAR_LEFT, MOVE_STAR_LEFT, "Default", m_pScene);
	pMoveLeft->SetMoveTime(1.f);
	pMoveLeft->SetPos(m_tPos + Position(-170.f, 30.f));
	pBlueBall->SetTarget(pMoveLeft);

	pBlueBall = CObj::CreateCloneObject<CEffect>(BLUE_BALL, "BlueBall", "Default", m_pScene);

	pBlueBall->SetBlend(0.5f, 100, 200, 150);
	CMoveStar* pMoveRight = CObj::CreateCloneObject<CMoveStar>(MOVE_STAR_RIGHT, MOVE_STAR_RIGHT, "Default", m_pScene);
	pMoveRight->SetMoveTime(1.f);
	pMoveRight->SetPos(m_tPos + Position(170.f, 30.f));
	pBlueBall->SetTarget(pMoveRight);

}

void CDDD::AttackMoveStar()
{
	
	CEffect* pBlueBall = CObj::CreateCloneObject<CEffect>(BLUE_BALL, "BlueBall", "Default", m_pScene);

	pBlueBall->SetBlend(0.5f, 100, 200, 150);
	CMoveStar* pStar = nullptr;

	switch (m_eDir)
	{
	case SD_RIGHT:
		pStar = CObj::CreateCloneObject<CMoveStar>(MOVE_STAR_RIGHT, MOVE_STAR_RIGHT, "Default", m_pScene);
		pStar->SetPos(m_tPos + Position(300.f, 30.f));
		break;

	case SD_LEFT:
		pStar = CObj::CreateCloneObject<CMoveStar>(MOVE_STAR_LEFT, MOVE_STAR_LEFT, "Default", m_pScene);
		pStar->SetPos(m_tPos + Position(-300.f, 30.f));

		break;
	}
	pStar->SetMoveTime(0.5f);

	pBlueBall->SetTarget(pStar);
}

void CDDD::SetPlayer(CPlayer * pPlayer)
{
	m_pPlayer = pPlayer;

	if (m_pPlayer)
	{
		if (m_pPlayer->GetPos().x > m_tPos.x)
		{
			m_eDir = SD_RIGHT;
		}
		else
			m_eDir = SD_LEFT;

		m_pPlayer->AddRef();

	}


}

void CDDD::ChangeDDDClip(DDD_CLIP eClip)
{
	m_pAnimation->ChangeClip(g_pDDDAnimation[m_eDir][eClip]);
}

void CDDD::DefaultDDDClip(DDD_CLIP eClip)
{
	m_pAnimation->SetDefaultClip(g_pDDDAnimation[m_eDir][eClip]);

}

void CDDD::Damage(int iDamage)
{
	CCharacter::Damage(iDamage);

	AttackColliderDisEnable();

	if (IsDie())
	{
		// 죽은 이후에 어떻게 할 건지 생각해보자.
		m_bGravity = false;
		SetDieTime(30.f);
		m_bTileCollisionEnable = false; // 타일 충돌을 빼버린다.  타일은 충돌체랑은 별개..
		SetColliderStateAll(false); // 모든 충돌체를 꺼버린다. (처리를 막아버림)
		m_bDDD = true;
		// 쭉 날라가게 할 것이다.
		GET_SINGLE(CSoundManager)->Play("BossDie", CT_EFFECT);

		m_tDiePos = m_tPos;

	}
}

void CDDD::Start()
{
	CMonster::Start();


	CCollider* pCollider = FindCollider("DDDBody");

	if (pCollider)
	{
		pCollider->SetCollisionCallback<CDDD>(CS_BLOCK, &CDDD::HitStart, this);
	}

	SAFE_RELEASE(pCollider);


	

}

bool CDDD::Init()
{
	if (!CMonster::Init())
		return false;

	// 애니메이션의 클립을 추가한다.
	CreateAnimation("DDDAnimation");

	/*
	enum DDD_CLIP
{
	D_IDLE,
	D_WALK,
	D_JUMP,
	D_BALLOON,
	D_BALLOON_ATTACK,
	D_HOLD,
	D_ATTACK,
	D_HIT,
	D_VOMIT,
	D_END
};

	*/

	// DDD D_IDLE
	m_pAnimation->AddClip(g_pDDDAnimation[SD_LEFT][D_IDLE]);
	m_pAnimation->SetClipTexture(g_pDDDAnimation[SD_LEFT][D_IDLE], "DDDLeft", TEXT("DDDLeft.bmp"));

	m_pAnimation->AddClip(g_pDDDAnimation[SD_RIGHT][D_IDLE]);
	m_pAnimation->SetClipTexture(g_pDDDAnimation[SD_RIGHT][D_IDLE], "DDDRight", TEXT("DDDRight.bmp"));

	// DDD D_WALK
	m_pAnimation->AddClip(g_pDDDAnimation[SD_LEFT][D_WALK]);
	m_pAnimation->SetClipTexture(g_pDDDAnimation[SD_LEFT][D_WALK], "DDDLeft", TEXT("DDDLeft.bmp"));

	m_pAnimation->AddClip(g_pDDDAnimation[SD_RIGHT][D_WALK]);
	m_pAnimation->SetClipTexture(g_pDDDAnimation[SD_RIGHT][D_WALK], "DDDRight", TEXT("DDDRight.bmp"));

	// DDD D_JUMP
	m_pAnimation->AddClip(g_pDDDAnimation[SD_LEFT][D_JUMP]);
	m_pAnimation->SetClipTexture(g_pDDDAnimation[SD_LEFT][D_JUMP], "DDDLeft", TEXT("DDDLeft.bmp"));

	m_pAnimation->AddClip(g_pDDDAnimation[SD_RIGHT][D_JUMP]);
	m_pAnimation->SetClipTexture(g_pDDDAnimation[SD_RIGHT][D_JUMP], "DDDRight", TEXT("DDDRight.bmp"));

	// DDD D_BALLOON
	m_pAnimation->AddClip(g_pDDDAnimation[SD_LEFT][D_BALLOON]);
	m_pAnimation->SetClipTexture(g_pDDDAnimation[SD_LEFT][D_BALLOON], "DDDLeft", TEXT("DDDLeft.bmp"));

	m_pAnimation->AddClip(g_pDDDAnimation[SD_RIGHT][D_BALLOON]);
	m_pAnimation->SetClipTexture(g_pDDDAnimation[SD_RIGHT][D_BALLOON], "DDDRight", TEXT("DDDRight.bmp"));

	// DDD D_BALLOON_ATTACK
	m_pAnimation->AddClip(g_pDDDAnimation[SD_LEFT][D_BALLOON_ATTACK]);
	m_pAnimation->SetClipTexture(g_pDDDAnimation[SD_LEFT][D_BALLOON_ATTACK], "DDDLeft", TEXT("DDDLeft.bmp"));

	m_pAnimation->AddClip(g_pDDDAnimation[SD_RIGHT][D_BALLOON_ATTACK]);
	m_pAnimation->SetClipTexture(g_pDDDAnimation[SD_RIGHT][D_BALLOON_ATTACK], "DDDRight", TEXT("DDDRight.bmp"));



	// DDD D_ATTACK
	m_pAnimation->AddClip(g_pDDDAnimation[SD_LEFT][D_ATTACK]);
	m_pAnimation->SetClipTexture(g_pDDDAnimation[SD_LEFT][D_ATTACK], "DDDLeft", TEXT("DDDLeft.bmp"));

	m_pAnimation->AddClip(g_pDDDAnimation[SD_RIGHT][D_ATTACK]);
	m_pAnimation->SetClipTexture(g_pDDDAnimation[SD_RIGHT][D_ATTACK], "DDDRight", TEXT("DDDRight.bmp"));


	// DDD D_HIT
	m_pAnimation->AddClip(g_pDDDAnimation[SD_LEFT][D_HIT]);
	m_pAnimation->SetClipTexture(g_pDDDAnimation[SD_LEFT][D_HIT], "DDDLeft", TEXT("DDDLeft.bmp"));

	m_pAnimation->AddClip(g_pDDDAnimation[SD_RIGHT][D_HIT]);
	m_pAnimation->SetClipTexture(g_pDDDAnimation[SD_RIGHT][D_HIT], "DDDRight", TEXT("DDDRight.bmp"));


	// DDD D_VOMIT
	m_pAnimation->AddClip(g_pDDDAnimation[SD_LEFT][D_VOMIT]);
	m_pAnimation->SetClipTexture(g_pDDDAnimation[SD_LEFT][D_VOMIT], "DDDLeft", TEXT("DDDLeft.bmp"));

	m_pAnimation->AddClip(g_pDDDAnimation[SD_RIGHT][D_VOMIT]);
	m_pAnimation->SetClipTexture(g_pDDDAnimation[SD_RIGHT][D_VOMIT], "DDDRight", TEXT("DDDRight.bmp"));

	// DDD RUN
	m_pAnimation->AddClip(g_pDDDAnimation[SD_LEFT][D_RUN]);
	m_pAnimation->SetClipTexture(g_pDDDAnimation[SD_LEFT][D_RUN], "DDDLeft", TEXT("DDDLeft.bmp"));

	m_pAnimation->AddClip(g_pDDDAnimation[SD_RIGHT][D_RUN]);
	m_pAnimation->SetClipTexture(g_pDDDAnimation[SD_RIGHT][D_RUN], "DDDRight", TEXT("DDDRight.bmp"));



	m_pAnimation->LoadClip("../Bin/Save/DDD/DDDAttackLeft.clip");
	m_pAnimation->LoadClip("../Bin/Save/DDD/DDDAttackRight.clip");

	m_pAnimation->LoadClip("../Bin/Save/DDD/DDDHitLeft.clip");
	m_pAnimation->LoadClip("../Bin/Save/DDD/DDDHitRight.clip");

	m_pAnimation->LoadClip("../Bin/Save/DDD/DDDBalloonAttackLeft.clip");
	m_pAnimation->LoadClip("../Bin/Save/DDD/DDDBalloonAttackRight.clip");

	m_pAnimation->LoadClip("../Bin/Save/DDD/DDDBalloonLeft.clip");
	m_pAnimation->LoadClip("../Bin/Save/DDD/DDDBalloonRight.clip");

	m_pAnimation->LoadClip("../Bin/Save/DDD/DDDIdleLeft.clip");
	m_pAnimation->LoadClip("../Bin/Save/DDD/DDDIdleRight.clip");

	m_pAnimation->LoadClip("../Bin/Save/DDD/DDDJumpLeft.clip");
	m_pAnimation->LoadClip("../Bin/Save/DDD/DDDJumpRight.clip");

	m_pAnimation->LoadClip("../Bin/Save/DDD/DDDWalkLeft.clip");
	m_pAnimation->LoadClip("../Bin/Save/DDD/DDDWalkRight.clip");

	m_pAnimation->LoadClip("../Bin/Save/DDD/DDDVomitRight.clip");
	m_pAnimation->LoadClip("../Bin/Save/DDD/DDDVomitLeft.clip");

	m_pAnimation->LoadClip("../Bin/Save/DDD/DDDRunRight.clip");
	m_pAnimation->LoadClip("../Bin/Save/DDD/DDDRunLeft.clip");



	SetStaticSize(150.f, 200.f);

	m_pAnimation->SetTextureColorKeyAll(192, 160, 192);




	// 게임 플레이 모드인 경우에는 플레이어를 찾아서 주도록 한다. 그것이 아니라면 (애니메이션 편집모드)
	// 이것에 대한 처리를 하지 않는다.
	// Animation Mode에서는 Update함수를 호출하지 않으므로 다른 것은 걱정할 필요가 없다.
	// 단순히 Animation의 업데이트 함수만 호출해주고 있다. (게임의 로직과 관련된 부분은 필요가 없으니...)

	
	SetPivot(0.5f, 0.5f);

	m_eState = DS_IDLE;
	m_fPlayTime = 3.f;

	m_bGravity = true;
	m_bPrevGravity = m_bGravity;
	m_bLineCollision = true;
	m_bStageCollision = true;

	m_eObjectType = OT_BOSS_DDD;


	GET_SINGLE(CInput)->AddNormalKey("DDDPos", VK_LCONTROL, 'D');

	m_iHPMax = 25; 
	m_iHP = m_iHPMax;


	// 충돌체를 생성해준다.
	CColliderRect* pCollider = AddCollider<CColliderRect>("DDDAttackRight");

	pCollider->SetProfile("MonsterProfile");
	pCollider->SetChannel("Monster");
	pCollider->SetRelativeInfo(130.f, -250.f, 240.f, 150.f);
	pCollider->DisEnable();
	SAFE_RELEASE(pCollider);


	pCollider = AddCollider<CColliderRect>("DDDAttackLeft");

	pCollider->SetProfile("MonsterProfile");
	pCollider->SetChannel("Monster");
	pCollider->SetRelativeInfo(-240.f, -250.f, -130.f, 150.f);
	pCollider->DisEnable();
	SAFE_RELEASE(pCollider);

	return true;
}

void CDDD::Update(float fTime)
{
	CMonster::Update(fTime);

	if (m_bDDD)
	{
		// DieTime이 켜지면, 중력을 끄고, 모든 처리를 막은 상태로 날라가도록 한다. 2초 동안 날라가고 별이 생긴다.
		// 날라가야 한다.

		Size tStageSize = GET_SINGLE(CSceneManager)->GetStageSize();

		// 그러면 Hit애니메이션이 호출이 되면서, 자신이 바라보는 방향과 반대로 움직이면서 날라가고, 사라지게 된다.
		switch (m_eDir)
		{
		case SD_LEFT:
			Move(Position(1.f, 0.f), 300.f, fTime);

			// 왼쪽이 화면밖인가..
			{
				float fLeft = m_tPos.x - m_tPivot.x * m_tStaticSize.x;

				if (fLeft > tStageSize.x)
					m_bActive = false;

				
			}

			break;

		case SD_RIGHT:
			Move(Position(-1.f, 0.f), 300.f, fTime);

			// 오른쪽이 화면밖인가 
			{
				float fRight = m_tPos.x + (1.f - m_tPivot.x ) * m_tStaticSize.x;

				if (fRight < 0.f)
					m_bActive = false;
			}
			break;
		}

		Move(Position(0.f, -1.f), 300.f, fTime);



		if (!m_bActive)
		{
			((CMainScene*)m_pScene)->CreateDoor(m_tDiePos, ST_ENDINGSCENE);
		}

	}
	else
	{

		if (!m_bMove)
			return;

		else if (m_bFalling)
			return;


		float fDist = CMath::GetDist(m_tPos, m_pPlayer->GetPos());
		float fDistX = fabsf(m_tPos.x - m_pPlayer->GetPos().x);

		m_fTime += fTime;

		switch (m_eState)
		{
		case DS_IDLE:

			if (m_tPos.x < m_pPlayer->GetPos().x)
			{
				// 오른쪽으로 점프한다.
				m_eDir = SD_RIGHT;
			}
			else
			{
				// 왼쪽으로 점프한다.
				m_eDir = SD_LEFT;
			}

			// 점프를 하는 상황에서는 자신의 방향대로 처리를 하는 것이 맞는데, 
			// Idle 상태(정지)에서는 플레이어를 보고 있도록 하는 것이 맞다.


			if (m_fTime >= m_fPlayTime)
			{
				m_fTime -= m_fPlayTime;



				// 플레이어와의 거리를 구한다.


				if (fDistX < 400.f)
				{
					// 거리가 300픽셀보다 적다면, 점프 공격을 진행한다.
						// 플레이어의 위치에 따라서 방향이 달라지는 부분이다.
					m_eState = DS_JUMP_ATTACK;

					m_fJumpSpeedX = fDistX; // 300 이하의 범위에서 점프 공격을 시도하며, 그곳까지 가기 위한 속도로 지정.
					m_iJumpCount = 0;
					m_bJump = true;
					m_fPower = 2100.f;
					m_fPowerTime = 0.f;

					m_fTime = 0.f;
					m_fPlayTime = 0.2f; // 1

					ChangeDDDClip(D_JUMP);
				}

				else if (fDist < 700.f)
				{
					// 거리가 500픽셀 정도된다면, 걸어가서 거리를 좁히는 것으로 한다.
					m_fTime = 0.f;
					m_fPlayTime = 0.2f; // 1

					ChangeDDDClip(D_WALK);
					DefaultDDDClip(D_WALK);
					m_eState = DS_WALK;

					// 움직이는 처리를 하는 시간을 구한다.
					m_fWalkTime = (fDist - 230.f) / 400.f;

				}
				else // 달려가서 때리는 처리를 할 것이다.
				{
					m_fTime = 0.f;
					m_fPlayTime = 0.2f;

					ChangeDDDClip(D_RUN);
					DefaultDDDClip(D_IDLE);
					m_eState = DS_RUN;

					// 움직이는 처리를 하는 시간을 구한다.
					m_fWalkTime = (fDist - 230.f) / 600.f;
				}





			}
			break;


		case DS_JUMP_ATTACK:



			// 점프 대기 시간이 없는 경우에는 바로 점프 처리를 해준다.
			if (m_bJump)
			{
				m_fPowerTime += fTime;

				if (m_fPowerTime >= 0.1f)
				{
					m_fPower -= 200.f;
					m_fPowerTime -= 0.1f;
				}

				Move(Position(0.f, -1.f), m_fPower, fTime);

				MoveX(m_fJumpSpeedX, fTime);
			}
			else
			{
				// 바닥에 닿은 경우.

				GET_SINGLE(CSoundManager)->Play("BossJump", CT_EFFECT);

				++m_iJumpCount;

				GET_SINGLE(CSceneManager)->SetSway(0.05f, 10.f);

				if (m_iJumpCount == 2)
				{
					// 두 번째 점프에서 MoveStar를 생성하고, 점프 상태를 풀어버린다.

					InitIdle();

					JumpAttackMoveStar();

					return;
				}

				// 다시 점프할 수 있도록 처리를 한다.
				m_bJump = true;
				m_fPower = 2100.f;
				m_fPowerTime = 0.f;
				m_fJumpSpeedX = fDistX;

				if (m_tPos.x < m_pPlayer->GetPos().x)
				{
					// 오른쪽으로 점프한다.
					m_eDir = SD_RIGHT;
				}
				else
				{
					// 왼쪽으로 점프한다.
					m_eDir = SD_LEFT;

				}

				ChangeDDDClip(D_JUMP);

				m_pAnimation->CurrentClipClear();




			}




			// 점프 애니메이션을 설정한다.
			// 기본 애니메이션으로 idle을 설정한다.



			// 한번 점프를 끝낸 다음에는 JumpTime 셋팅해서 이것이 true인 동안은 다시 점프를 하지 않도록
			// 하고 이것이 다시 풀린 상황에서 점프 처리를 진행해서 다시 점프를 하는 것이다.
			// 점프를 시작하자 마자 처리가 점프처리를 안 해버렸다. ㅋㅋㅋ

			break;

		case DS_WALK:

			// 이곳에 들어왔다면, 자신의 방향과 시간만큼 이동을 시킨다.

			// m_fTime은 이미 업데이트를 하고 있는 상황이다.
			if (m_fTime >= m_fWalkTime)
			{
				ChangeDDDClip(D_ATTACK);
				m_eState = DS_ATTACK;
				// 충돌체 활성화. 
				m_bAttack = false;

			}
			else
				MoveX(400.f, fTime);

			break;

		case DS_RUN:



			// 공격의 경우에는 2가지 case로 나뉘는데, 첫번째로 달려가는 모션.
			// 두 번째로 내리찍는 모션이다.
			// WalkTime의 보다 작은 시간에서는 움직이고, 
			// 이후에는 찍는 처리를 하도록 하자.

			if (m_fTime >= m_fWalkTime)
			{

				ChangeDDDClip(D_ATTACK);
				m_eState = DS_ATTACK;
				// 충돌체 활성화. 

				m_bAttack = false;
			}
			else
				MoveX(600.f, fTime);

			break;

		case DS_ATTACK:

			// 공격 모션을 취하고, 잠시후에 Idle상태로 돌아가도록 한다.
			// 현재 애니메이션이 Attack이 아니라면, Idle상태로 돌아가도록 한다.
			if ((m_pAnimation->GetCurrentAnimation() != "DDDAttackLeft") &&
				(m_pAnimation->GetCurrentAnimation() != "DDDAttackRight"))
			{
				SetColliderState("DDDAttackLeft", false);
				SetColliderState("DDDAttackRight", false);

				InitIdle();
				return;
			}

			// 찍는 모션에서 화면을 흔들게 한다.

			if (!m_bAttack)
			{
				if (m_pAnimation->GetClip()->iFrame == 5)
				{
					m_bAttack = true;
					GET_SINGLE(CSceneManager)->SetSway(0.05f, 10.f);
					AttackMoveStar();

					GET_SINGLE(CSoundManager)->Play("BossAttack", CT_EFFECT);
				}
				else if (m_pAnimation->GetClip()->iFrame == 4)
				{
					switch (m_eDir)
					{
					case SD_LEFT:
						SetColliderState("DDDAttackLeft", true);
						break;


					case SD_RIGHT:
						SetColliderState("DDDAttackRight", true);
						break;
					}
				}

				// 공격 이전 상태에서 검사를 하면서 4프레임에서 충돌체를 활성화. 5ㅍ레임에서 화면 흔들기 시작.
				// 끝.

			}

			break;

		case DS_BALLOON_ATTACK:
			break;

		case DS_VOMIT:
			break;
		}
	}
}

void CDDD::LateUpdate(float fTime)
{
	CMonster::LateUpdate(fTime);

}

void CDDD::Collision(float fTime)
{
	CMonster::Collision(fTime);

}

void CDDD::PrevRender(HDC hDC, float fTime)
{
	CMonster::PrevRender(hDC, fTime);

}

void CDDD::Render(HDC hDC, float fTime)
{
	CMonster::Render(hDC, fTime);

}

void CDDD::PostRender(HDC hDC, float fTime)
{
	CMonster::PostRender(hDC, fTime);

}

CDDD * CDDD::Clone() const
{
	return new CDDD(*this);
}

void CDDD::HitAnimation()
{
	CMonster::HitAnimation();

	m_pAnimation->ChangeClip(g_pDDDAnimation[m_eDir][D_HIT]);

}

void CDDD::AddClipName(vector<string>& vecClipName)
{
	for (int j = 0; j < D_END; ++j)
	{
		for (int i = 0; i < SD_END; ++i)
		{
			vecClipName.push_back(g_pDDDAnimation[i][j]);
		}
	}

}

void CDDD::IdleAnimation()
{
	m_pAnimation->ChangeClip(g_pDDDAnimation[m_eDir][D_IDLE]);

}

void CDDD::HitStart(CCollider * pSrc, CCollider * pDest, float fTime)
{
	CMonster::HitStart(pSrc, pDest, fTime);

	CObj* pDestObj = pDest->GetObj();

	switch (pDestObj->GetObjectType())
	{
	case OT_PLAYER_ATTACK:

		if (pDestObj->GetTag() == "Breath")
			return;

		else if (pDestObj->GetTag() == "VomitStar")
			GET_SINGLE(CSoundManager)->Play("BossStarHit", CT_EFFECT);

		// 위 함수에서 데미지에 대한 처리를 막았으니 이 함수도 처리하지 않도록 한다.

		m_bKnockBack = false;
		// 넉백에 대한 처리는 그냥 막아버린다. 보스는 넉백되지 않는다.

		if(!m_bBlend)
			HitAnimation();

		break;
	}
}

void CDDD::AttackColliderDisEnable()
{
	SetColliderState("DDDAttackLeft", false);
	SetColliderState("DDDAttackRight", false);
}

