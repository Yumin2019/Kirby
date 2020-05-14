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
	// ���ʿ� ���� MoveStar�� �����Ѵ�.	// Blue Ball
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
		// ���� ���Ŀ� ��� �� ���� �����غ���.
		m_bGravity = false;
		SetDieTime(30.f);
		m_bTileCollisionEnable = false; // Ÿ�� �浹�� ��������.  Ÿ���� �浹ü���� ����..
		SetColliderStateAll(false); // ��� �浹ü�� ��������. (ó���� ���ƹ���)
		m_bDDD = true;
		// �� ���󰡰� �� ���̴�.
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

	// �ִϸ��̼��� Ŭ���� �߰��Ѵ�.
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




	// ���� �÷��� ����� ��쿡�� �÷��̾ ã�Ƽ� �ֵ��� �Ѵ�. �װ��� �ƴ϶�� (�ִϸ��̼� �������)
	// �̰Ϳ� ���� ó���� ���� �ʴ´�.
	// Animation Mode������ Update�Լ��� ȣ������ �����Ƿ� �ٸ� ���� ������ �ʿ䰡 ����.
	// �ܼ��� Animation�� ������Ʈ �Լ��� ȣ�����ְ� �ִ�. (������ ������ ���õ� �κ��� �ʿ䰡 ������...)

	
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


	// �浹ü�� �������ش�.
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
		// DieTime�� ������, �߷��� ����, ��� ó���� ���� ���·� ���󰡵��� �Ѵ�. 2�� ���� ���󰡰� ���� �����.
		// ���󰡾� �Ѵ�.

		Size tStageSize = GET_SINGLE(CSceneManager)->GetStageSize();

		// �׷��� Hit�ִϸ��̼��� ȣ���� �Ǹ鼭, �ڽ��� �ٶ󺸴� ����� �ݴ�� �����̸鼭 ���󰡰�, ������� �ȴ�.
		switch (m_eDir)
		{
		case SD_LEFT:
			Move(Position(1.f, 0.f), 300.f, fTime);

			// ������ ȭ����ΰ�..
			{
				float fLeft = m_tPos.x - m_tPivot.x * m_tStaticSize.x;

				if (fLeft > tStageSize.x)
					m_bActive = false;

				
			}

			break;

		case SD_RIGHT:
			Move(Position(-1.f, 0.f), 300.f, fTime);

			// �������� ȭ����ΰ� 
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
				// ���������� �����Ѵ�.
				m_eDir = SD_RIGHT;
			}
			else
			{
				// �������� �����Ѵ�.
				m_eDir = SD_LEFT;
			}

			// ������ �ϴ� ��Ȳ������ �ڽ��� ������ ó���� �ϴ� ���� �´µ�, 
			// Idle ����(����)������ �÷��̾ ���� �ֵ��� �ϴ� ���� �´�.


			if (m_fTime >= m_fPlayTime)
			{
				m_fTime -= m_fPlayTime;



				// �÷��̾���� �Ÿ��� ���Ѵ�.


				if (fDistX < 400.f)
				{
					// �Ÿ��� 300�ȼ����� ���ٸ�, ���� ������ �����Ѵ�.
						// �÷��̾��� ��ġ�� ���� ������ �޶����� �κ��̴�.
					m_eState = DS_JUMP_ATTACK;

					m_fJumpSpeedX = fDistX; // 300 ������ �������� ���� ������ �õ��ϸ�, �װ����� ���� ���� �ӵ��� ����.
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
					// �Ÿ��� 500�ȼ� �����ȴٸ�, �ɾ�� �Ÿ��� ������ ������ �Ѵ�.
					m_fTime = 0.f;
					m_fPlayTime = 0.2f; // 1

					ChangeDDDClip(D_WALK);
					DefaultDDDClip(D_WALK);
					m_eState = DS_WALK;

					// �����̴� ó���� �ϴ� �ð��� ���Ѵ�.
					m_fWalkTime = (fDist - 230.f) / 400.f;

				}
				else // �޷����� ������ ó���� �� ���̴�.
				{
					m_fTime = 0.f;
					m_fPlayTime = 0.2f;

					ChangeDDDClip(D_RUN);
					DefaultDDDClip(D_IDLE);
					m_eState = DS_RUN;

					// �����̴� ó���� �ϴ� �ð��� ���Ѵ�.
					m_fWalkTime = (fDist - 230.f) / 600.f;
				}





			}
			break;


		case DS_JUMP_ATTACK:



			// ���� ��� �ð��� ���� ��쿡�� �ٷ� ���� ó���� ���ش�.
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
				// �ٴڿ� ���� ���.

				GET_SINGLE(CSoundManager)->Play("BossJump", CT_EFFECT);

				++m_iJumpCount;

				GET_SINGLE(CSceneManager)->SetSway(0.05f, 10.f);

				if (m_iJumpCount == 2)
				{
					// �� ��° �������� MoveStar�� �����ϰ�, ���� ���¸� Ǯ�������.

					InitIdle();

					JumpAttackMoveStar();

					return;
				}

				// �ٽ� ������ �� �ֵ��� ó���� �Ѵ�.
				m_bJump = true;
				m_fPower = 2100.f;
				m_fPowerTime = 0.f;
				m_fJumpSpeedX = fDistX;

				if (m_tPos.x < m_pPlayer->GetPos().x)
				{
					// ���������� �����Ѵ�.
					m_eDir = SD_RIGHT;
				}
				else
				{
					// �������� �����Ѵ�.
					m_eDir = SD_LEFT;

				}

				ChangeDDDClip(D_JUMP);

				m_pAnimation->CurrentClipClear();




			}




			// ���� �ִϸ��̼��� �����Ѵ�.
			// �⺻ �ִϸ��̼����� idle�� �����Ѵ�.



			// �ѹ� ������ ���� �������� JumpTime �����ؼ� �̰��� true�� ������ �ٽ� ������ ���� �ʵ���
			// �ϰ� �̰��� �ٽ� Ǯ�� ��Ȳ���� ���� ó���� �����ؼ� �ٽ� ������ �ϴ� ���̴�.
			// ������ �������� ���� ó���� ����ó���� �� �ع��ȴ�. ������

			break;

		case DS_WALK:

			// �̰��� ���Դٸ�, �ڽ��� ����� �ð���ŭ �̵��� ��Ų��.

			// m_fTime�� �̹� ������Ʈ�� �ϰ� �ִ� ��Ȳ�̴�.
			if (m_fTime >= m_fWalkTime)
			{
				ChangeDDDClip(D_ATTACK);
				m_eState = DS_ATTACK;
				// �浹ü Ȱ��ȭ. 
				m_bAttack = false;

			}
			else
				MoveX(400.f, fTime);

			break;

		case DS_RUN:



			// ������ ��쿡�� 2���� case�� �����µ�, ù��°�� �޷����� ���.
			// �� ��°�� ������� ����̴�.
			// WalkTime�� ���� ���� �ð������� �����̰�, 
			// ���Ŀ��� ��� ó���� �ϵ��� ����.

			if (m_fTime >= m_fWalkTime)
			{

				ChangeDDDClip(D_ATTACK);
				m_eState = DS_ATTACK;
				// �浹ü Ȱ��ȭ. 

				m_bAttack = false;
			}
			else
				MoveX(600.f, fTime);

			break;

		case DS_ATTACK:

			// ���� ����� ���ϰ�, ����Ŀ� Idle���·� ���ư����� �Ѵ�.
			// ���� �ִϸ��̼��� Attack�� �ƴ϶��, Idle���·� ���ư����� �Ѵ�.
			if ((m_pAnimation->GetCurrentAnimation() != "DDDAttackLeft") &&
				(m_pAnimation->GetCurrentAnimation() != "DDDAttackRight"))
			{
				SetColliderState("DDDAttackLeft", false);
				SetColliderState("DDDAttackRight", false);

				InitIdle();
				return;
			}

			// ��� ��ǿ��� ȭ���� ���� �Ѵ�.

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

				// ���� ���� ���¿��� �˻縦 �ϸ鼭 4�����ӿ��� �浹ü�� Ȱ��ȭ. 5�����ӿ��� ȭ�� ���� ����.
				// ��.

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

		// �� �Լ����� �������� ���� ó���� �������� �� �Լ��� ó������ �ʵ��� �Ѵ�.

		m_bKnockBack = false;
		// �˹鿡 ���� ó���� �׳� ���ƹ�����. ������ �˹���� �ʴ´�.

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

