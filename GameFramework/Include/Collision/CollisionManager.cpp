#include "CollisionManager.h"
#include "../Core/SceneManager.h"
#include "../Core/GameManager.h"

#include "Collider.h"
#include "../Object/Obj.h"
#include "../Core/Input.h"
#include "../Object/Stage.h"
#include "../Object/Player.h"
#include "../Animation/Animation.h"

#include "../Object/Grizoo.h"
#include "../Object/Burt.h"
#include "../Object/Effect.h"
#include "../Object/JumpStar.h"
#include "../Object/DDD.h"
#include "../Sound/SoundManager.h"


DEFINITION_SINGLE(CCollisionManager);

CCollisionManager::CCollisionManager()
{
	m_pPrevCollisionAdj = nullptr;
	m_pMousePrevCollision = nullptr;
}


CCollisionManager::~CCollisionManager()
{

}

unsigned int CCollisionManager::CreateSerialNumber()
{
	return m_SerialNumber.CreateSerialNumber();
}

void CCollisionManager::ReturnSerialNumber(unsigned int iNumber)
{
	m_SerialNumber.ReturnSerialNumber(iNumber);
}

void CCollisionManager::Clear()
{
	// �浹ü �����ڿ����� UI�浹ü��, ������Ʈ �浹ü�� �����ִ� ó���� �ϰ� �ִ�.
	// �ش� �浹ü�� ������� �������� �ڽŰ� �浹�ߴ� �浹ü���� �ڽ��� ������ �����ִ� ó���� �ϰ� �ְ�
	// �浹ü �����ڿ��� �����Ͽ� ��������� �������ִ� ó���� ���ְ� �ִ�.
	// �ٵ�, ���⼭ �߿��� ����, CollisionManager�� �̹� ������� ������ ��ü��� ���̴�.
	// ������ ��ü�� �ٽ� ȣ���Ͽ� ����ϰ� �ִ�. (��Ȯ�ϰԴ� ����� ��ü�� �ٽ� �����Ͽ� ����ϴ� �����̴�)
	// �翬�� �̷� ó���� �����ϴ�.
	// �浹ü�� ������� �������� �浹 ������ Ŭ������ �ʿ��ѵ�,
	// �浹 ������ Ŭ������ ������ �翬�� ������ ���� ���̴�.
	// ����, �浹 �����ڰ� ������� ������ �浹ü�� ���� �����ϴ� ������ ��ģ�ٸ� �ƹ��� ������ ������ ���� ���̴�.
	// �̷� ������ Clear�Լ��� �ʿ��ϴ�.
	// �浹�����ڰ� �縮���� ������ �̸� �浹ü�� ������ �ý����� �ʿ��ϴϱ�.

	// ��������� ������. ��������� �浹ü�� ������� �������� ����� ���̴�.
	// ���� ��������� ������ �浹ü�� ���� ���� �ƴ϶� �浹ü�� ������, ��������� �̿��ϵ��� �ϰ�
	// �� ���Ŀ� �����ϰ� ��������� ������ ���� ���� �� ����.


	// �Ҽ� �浹ü�� �޾Ƴ��Ҵ� ColliderList�� UIColliderList�� ������.
	Safe_Release_VecList(m_ColliderList);
	Safe_Release_VecList(m_UIColliderList);

	// �ش� List�� ��쿡�� Ÿ�� �浹ó���� �ϱ� ���� �޾Ƶ�  List �̴�.
	// �߰��� �� AddRef�� �Ͼ���Ƿ� release�� ���ش�.
	
	// �浹ü�� ������鼭, ��������� ������ ���ְ�, �ڽŰ� �浹�ߴ� �浹ü�� ���ؼ�
	// �Լ��� ȣ��, �浹 ���踦 �����ִ� ó���� �ϰ� �ִ�.
	// �̷��� ��� �浹ü�� �����ְ�, ���Ŀ� ��������� ���ش�.
	for (unsigned int i = 0; i < m_iPrevCollisionCount; ++i)
	{
		SAFE_DELETE_ARRAY(m_pPrevCollisionAdj[i]);
	}

	SAFE_DELETE_ARRAY(m_pPrevCollisionAdj);

	// �Ҵ�ް� �ִ� Channel�� Profile������ ������.
	SAFE_DELETE_ARRAY(m_pChannel);
	SAFE_DELETE_ARRAY(m_pProfile);


}

void CCollisionManager::WorldObjListClear()
{
	Safe_Release_VecList(m_WorldObjList);
}

void CCollisionManager::AddPrevCollisionAdj(unsigned int iSrc, unsigned int iDest)
{
	unsigned int iSrcIdx = iSrc / 32;
	unsigned int iDestIdx = iDest / 32;

	// �� SerialNumber�� ����Ű�� �� �ε����� ���Ѵ�.
	// �� ��° ���� �ִ��� ���ϴ� ���̴�.

	// �� ��° ��Ʈ�� �߰��� �ؾ��ϴ��� ���ؾ��Ѵ�.
	unsigned int iSrcBit = 31 - iSrc % 32;
	unsigned int iDestBit = 31 - iDest % 32;

	// SerialNumber % 32�� ���ؼ� �� ��°�� �ε������� ���� �� �ִ�.
	// Src�ε����� ���ص� �����̰�, �� ��ü�� Dest��ü�� Bit�� ���ϴ� �����̴�.
	// ���࿡ 33���̶��, ù��° ��Ʈ �ڸ��� 1�� ����� ���̴�.
	// �׷��� 1�� �̿��ؼ� bit�� �ű�� �Ǵµ� 
	// ��Ʈ�� Ư���� 0000000... 1 �̷� ������ ������ �Ǿ��ִ�.
	// ��, �츮�� �ڿ��� ���� ��Ʈ�� �о 1���� �ڸ��� ���� ������ �ϴ� ���̴�.
	// ����, iSrcBit�� iDestBit�� �Űܾ��� ��Ʈ ���� �ǹ��ϵ��� �Ͽ�,
	// 31 - x % 32�� ���ؼ�,
	// x�� 32���, ���� ù��° �ڸ��� �־�� �Ѵ�.
	// ����, 31 - 0��ŭ �����ʿ� �ִ� 1�� �������� �о 0��°¥���� true�� ���� �� �ִ�.
	
	// x = 2���, 30���� bit�� �Űܼ�, 
	// 1��° �ڸ��� bit�� true�� ���� �� �ִ� ���̴�.

	unsigned int iSrcAdd = 1 << iSrcBit;
	unsigned int iDestAdd = 1 << iDestBit;

	// �̷��� �Ͽ� �츮�� ���ϴ�  bit (Dest�� �ش��ϴ� ��Ʈ)�� true�� �ϰ� �������� false�� �������.

	// �̰��� OR�����Ͽ� ������Ŀ��� �ش� ������ �߰��ϴ� ���̴�.
	// SRC ������ DESTBIT
	// DEST ������ SRCBIT

	// Src��ü�� �������� Dest�� Bit�� �ִ� ������ �����Ͽ� . . .
	// Dest�� Bit�� �ش��ϴ� �ڸ��� true�� ���� unsigned int �� ������ OR����.
	// ����, Src��ü��, Dest�� ��ü�� �浹�ߴٴ� ���� ����.
	m_pPrevCollisionAdj[iSrc][iDestIdx] |= iDestAdd;
	m_pPrevCollisionAdj[iDest][iSrcIdx] |= iSrcAdd;
	// Dest�� ���������Ͽ� SrcBit�� �ִ� ������ ����.
	// Src�ڸ��� bit�� true�� ���� unsigned int ���� OR�����Ͽ�,
	// Src�� bit�ڸ��� true
}

void CCollisionManager::KirbyCollision(CObj* pObj, float fTime)
{
	GET_SINGLE(CSceneManager)->CameraUpdate(fTime);

	CPlayer* pPlayer = (CPlayer*)pObj;

	if (pPlayer->GetJump())
	{
		pPlayer->JumpDisEnable();

		CAnimation* pAni = pPlayer->GetAnimation();

		// �����߿� ���� �ִϸ��̼��� KirbyAttackRight / AttackLeft�� ��쿡��
		// ����� �ٲ��� �ʴ´�. (�Ϲ� Ŀ��)

		switch (pPlayer->GetKirbyType())
		{
		case KIRBY_NORMAL:
			if ((pAni->GetCurrentAnimation() != "KirbyAttackLeft") &&
				(pAni->GetCurrentAnimation() != "KirbyAttackRight"))
				pPlayer->ChangeDefaultClip();

		case KIRBY_MIKE:
			// ���� �ð��� �ƴ϶�� �ٲ۴�.
			if (!pPlayer->GetAttack())
			{
				pPlayer->ChangeDefaultClip();
			}
			break;

	

		case KIRBY_STONE:

			if(!pPlayer->GetStone())
				pPlayer->ChangeDefaultClip();

			break;

		case KIRBY_SWORD:

			if ( (pAni->GetCurrentAnimation() == "SwordJumpAttackLeft") ||
				(pAni->GetCurrentAnimation() == "SwordJumpAttackRight") )
				pPlayer->RunDisEnable();
			else
				pPlayer->ChangeDefaultClip();

			// ������ Ŀ���ε�, ���� ������ �ϰ� ���� ���� ��쿡 Ŭ���� ��ü�Ѵ�.

			break;
		}
			

		if (pPlayer->GetRun())
		{
			if (!KEYPRESS("MoveRight") && !KEYPRESS("MoveLeft"))
				pPlayer->RunDisEnable();
		}

		// �����߿� Run �� ��� �¿� Ű�� ������ �ִ� ��쿡 Run���¸� �����ϸ� �ǰ�,
		// �¿�Ű�� ������ ���� ���� ��쿡 Run�� Ǯ������� �ȴ�.
	}

	else if (pPlayer->GetBallon())
	{
		// �ϴ��� Ballon DisEnable
		// ǳ�� ���¿��� �ٴ����� ������ ��쿡�� 
		// �Ա� �ִϸ��̼��� ȣ���� �Ǹ鼭 �⺻ �ִϸ��̼����� ���ư��� �Ǿ� �Ѵ�.
		pPlayer->BallonDisEnable();
		pPlayer->BalloonStageCollision();

	}

	pPlayer->FallingDisEanble();


	if (pPlayer->GetKirbyType() == KIRBY_SLEEP)
	{
		pPlayer->SleepEnable();
	}
}

void CCollisionManager::DeletePrevCollisionAdj(unsigned int iSrc, unsigned int iDest)
{
	unsigned int	iSrcIdx = iSrc / 32;
	unsigned int	iDestIdx = iDest / 32;

	unsigned int	iSrcBit = 31 - iSrc % 32;
	unsigned int	iDestBit = 31 - iDest % 32;

	unsigned int	iSrcAdd = 1 << iSrcBit;
	unsigned int	iDestAdd = 1 << iDestBit;

	// �߰��� �����ϰ�, Src�� Dest�� bit�� true�� ���� unsigned int���� �����.
	// Src�������� Dest�� ���� ����, Dest�� bit��ġ�� �����ϰ� 0���� ä�� unsigned int�� ������
	// AND �����Ͽ�, DestBit���� Ȯ���Ͽ� �浹�̶��, �� �κ��� �����ش�.

	if (m_pPrevCollisionAdj[iSrc][iDestIdx] & iDestAdd)
		m_pPrevCollisionAdj[iSrc][iDestIdx] ^= iDestAdd;
	// XOR ������ false���� �ֵ���.

	// & ���꿡�� true�� ���´ٴ� ���� 0�� �ƴ� ���� ���´ٴ� �ǹ��̴�.
	// ���ڸ��� true�� unsinged int���� &������ ���ؼ� 0�� �ƴ� ���� ���´ٴ� ��.
	// ���࿡ ������ �̷� ������ �ϰ� �ȴٸ�..
	// 11110011
	// 00010000
	//=00010000 4��° �ڸ����� 1�� ������ �Ǿ� 0�� �ƴϰ� �ȴ�.
	// �츮�� 4��° �ڸ��� bit�� Ȯ���ϸ� �Ǵµ�, ������ ������ bit�� &������ �����ϴ� ����.
	// 0���� ä������, ���� 4��° �ڸ��� bit�� ���ؼ� 0�̰ų� 0�� �ƴ� ���� ������ �ȴ�.

	// 11110011
	// 00001000 �̶��.
	//=00000000 �� �ȴ�.
	// 5��° �ڸ��̿ܿ��� 0�̹Ƿ� &������ ���ؼ� 5��° �ڸ� �̿ܿ��� �ݵ�� 0�� �ȴ�.
	// ���� 5��° �ڸ��� ���ؼ� 0�̰ų� 0�� �ƴ� ���� ���´�.
	// ���� 5��° �ڸ��� true��� 0�� �ƴ� ���� ���ͼ� true�� �ǰ�
	// 5��° �ڸ��� 0�̶�� &������ ����� false�� �Ǵ� ���̴�.

	// ��, &������ ���ؼ� �츮�� �츮�� ���ϴ� bit�� Ȯ���� ����� ���� �� �ִ� ���̴�.
	// �ش� bit�� true�� true.
	// �ش� bit�� false�� false.
	if (m_pPrevCollisionAdj[iDest][iSrcIdx] & iSrcAdd)
		m_pPrevCollisionAdj[iDest][iSrcIdx] ^= iSrcAdd;

	//  Dest�� �������� Src�� bit�� Ȯ���Ͽ� true���,
	// XOR������ ���ؼ�, �츮�� ���ϴ� bit���� 0���� ����� ó���� �����ϴ�.

	// ex : 100100
	//	    100000 xor
	//resul	000100

	// �� �츮�� ���ϴ� ù��° ��Ʈ���� false�� ���� �浹���¸� Ǯ����� ���̴�.
	// XOR ������ Ư����, �ڽŰ� �ٸ� ��쿡�� true�� ��´�.
	// ��, �츮�� ���ϴ� ��Ʈ�� �����ϰ�� 0�� XOR ������ �ϰ� �Ǵµ�,
	// ���� 0�� �ڸ��� �������� 0�� ���� ���̸� 1�� �ڸ��� �޶� 1�� ������ ���̴�.

	// 10101
	// 00000
	// 10101 �̴� ��, ������ ����� �״�� ������ �Ǵ� ������ ��Ÿ����.

	// �ݸ� �츮�� ���ϴ� ��Ʈ�� �ڸ��� ��쿡�� 1�̱� ������
	// �ش� �ڸ��� ������ false�� ���Ѵ�.
	
	// ��� : Ȯ���ϴ� bit�� false(�浹���¸� ���ְ�) ������ bit�� �״�� ���ΰ� �ִ�.



}

bool CCollisionManager::CheckPrevCollisionAdj(unsigned int iSrc, unsigned int iDest)
{
	unsigned int	iDestIdx = iDest / 32;

	unsigned int	iDestBit = 31 - iDest % 32;

	unsigned int	iDestAdd = 1 << iDestBit;

	if (m_pPrevCollisionAdj[iSrc][iDestIdx] & iDestAdd)
		return true;

	// �浹ó���� Ȯ���ϴ� ���� ���ʸ� Ȯ���غ��� �ȴ�.
	// ����, �ش� �κа� &������ �Ͽ�, �ش� bit�� ���°� true���� ���Ѵ�.
	// ���⼭ true�� ������, �浹 ���¶�� ���̰� false�� �浹���°� �ƴ϶�� ���̴�.

	return false;
}

void CCollisionManager::AddWorldObjList(CObj * pObj)
{
	// �ٵ� �ش� ������Ʈ ����Ʈ�� ��쿡�� Ÿ�ϰ� �浹�� �Ͼ�� ������ UI�� ó���ϸ� �� �ȴ�.
	if (pObj->IsUI())
		return;

	else if (pObj->GetTag() == "Stage")
		return;

	else if (!pObj->GetTileCollisionEnable())
		return;


	pObj->AddRef();

	m_WorldObjList.push_back(pObj);
}

void CCollisionManager::AddCollider(CCollider * pCollider)
{
	// �浹ü�� CollisionManager�� ������ �޴� ���̴�.
	// �浹ü�� �⺻������ ������Ʈ�� CollisionManager�� ������ �ް� �Ǿ� ���۷��� ī���Ͱ� 2�� �ȴ�.
	
	if (pCollider->GetTag() == "MouseUI")
		return;
	// ���콺 �浹ü�� �־����� �ʴ´�. ���� ó���ϱ� �����̴�. (�ߺ�ó���� ��������)

	pCollider->AddRef();

	if (pCollider->m_bIsUI)
	{
		m_UIColliderList.push_back(pCollider);
	}

	else
		m_ColliderList.push_back(pCollider);
}

void CCollisionManager::DeleteCollider(CScene * pScene)
{
	// pScene�� �Ҽ����� �ϴ� �浹ü�� ��� ������.

	list<CCollider*>::iterator iter;
	list<CCollider*>::iterator iterEnd = m_ColliderList.end();

	for (iter = m_ColliderList.begin(); iter != iterEnd;)
	{
		if ((*iter)->GetScene() == pScene)
		{
			SAFE_RELEASE((*iter));
			iter = m_ColliderList.erase(iter);
			iterEnd = m_ColliderList.end();
			continue;
		}

		++iter;
	}
}

void CCollisionManager::AddProfile(const string & strName)
{
	// ���������� �߰��ϴ� �Լ��̴�.

	// ���������� �Ҵ緮�� �Ѿ���� üũ�Ѵ�.
	if (m_iProfileCapacity == m_iProfileSize)
		return;

	else if (FindProfile(strName))
		return;

	// �̹� �ش� �̸��� ���� ���������� �ִ� ��쵵 ����

	m_pProfile[m_iProfileSize].strName = strName;
	// �߰��� �ε����� �ش��ϴ� �Ҵ�� ������ �̸��� ����.

	// ���� ������ ä�ο� ä�� �Ҵ緮 ��ŭ ä���� ������ �Ҵ��Ѵ�.
	m_pProfile[m_iProfileSize].pChannelList = new CollisionChannel[m_iChannelCapacity];
	// ���⼭ ä���� �����ŭ �Ҵ��� �ϴ� ���� �ƴ϶�, �Ҵ緮��ŭ �ְ� �ִٴ� ���� �ָ�����.
	// �׷��ϱ�, ���߿� �߰��Ǵ� ä�ο��� ������ �� �ִ�.

	// ���������� �� ä���� ������ ���ε��� ������ �ִ�.
	for (int i = 0; i < m_iChannelSize; ++i)
	{
		m_pProfile[m_iProfileSize].pChannelList[i] = m_pChannel[i];
		// �浹 �����ڴ� ���������� ä�ΰ��� ������� ä���� �Ҵ��Ͽ� � ä���� �ִ��� 
		// ������ Ȯ���� �� �ֵ��� �Ѵ�.
		// ����, �� �⺻������ ����ִ� ä���� �ϳ��� ��ȸ�ϸ鼭 ���� �Ҵ��� �κп�
		// ���� �������ش�.

		// �ܼ��� �̸��� ���� �������� �Ѱ��ִ� ó���� �ϰ� �ִ�.
		// ��� : ���� �Ҵ��� ä�ο� ������ ä���ش�.
	}
	
	// ���θ��� ���������� ä���� ������ 0�� ���̴�.
	m_pProfile[m_iProfileSize].m_iChannelSize = m_iChannelSize;

	// ���������� ���� �����Ѵ�.
	++m_iProfileSize;
}

void CCollisionManager::AddChannel(const string & strName,
	COLLISION_CHANNEL_STATE eDefaultState)
{
	// ä���� �߰��ϴ� �����̴�.

	// �ش� �̸��� ���� ä���� �ִ� ���, �̹� �Ҵ緮�� �� �� ��츦 ����.

	if (m_iChannelCapacity == m_iChannelSize)
		return;

	else if (GetChannelIndex(strName) != -1) // -1�� �ƴ϶�� ���� �̹� �ش��̸��� ���� ä���� �����Ѵٴ� ��.
		return;

	// ä���� �����ϰ�, ��� ���������� ��ȸ�ϸ鼭 �� ä���� ������ �߰��Ѵ�.
	// �浹ü �����ڿ��� �ش� ä���� ������ �߰��ϰ�, 
	// �� ä�� ���� ���� �Ͽ�, �������Ͽ� �ش� ���� �������ش�.

	m_pChannel[m_iChannelSize].strName = strName;
	m_pChannel[m_iChannelSize].eState = eDefaultState;

	for (int i = 0; i < m_iProfileSize; ++i)
	{
		m_pProfile[i].pChannelList[m_iChannelSize] = m_pChannel[m_iChannelSize];
		++m_pProfile[i].m_iChannelSize;
	}

	++m_iChannelSize;
}

void CCollisionManager::SetProfileChannelState(const string & strProfile,
	const string & strChannel, COLLISION_CHANNEL_STATE eState)
{
	// �ش� �̸��� ���� Profile���� ä���� ã�Ƽ�, �ش� ä���� ���¸� �����ϴ� �Լ��̴�.

	for (int i = 0; i < m_iProfileSize; ++i)
	{
		if (m_pProfile[i].strName == strProfile)
		{
			// ä���� ã�ƶ�.
			int iChannelIndex = GetChannelIndex(strChannel);

			// ���⼭ ä�� �̸��� ���� -1�� ���� ���ɼ��� �ִ�.

			if (iChannelIndex == -1)
				return;

			// -1�� �ƴѰ�쿡 �ش� ä���� ���� �ٲ������.

			m_pProfile[i].pChannelList[iChannelIndex].eState = eState;

			return;
		}
	}
}

void CCollisionManager::SetProfileChannelStateAll(const string & strProfile, 
	COLLISION_CHANNEL_STATE eState)
{
	// �ش� �������Ͽ� �ִ� ��� ä���� ���¸� eState�� �����Ѵ�.

	for (int i = 0; i < m_iProfileSize; ++i)
	{
		if (m_pProfile[i].strName == strProfile)
		{
			for (int j = 0; j < m_iChannelSize; ++i)
			{
				m_pProfile[i].pChannelList[i].eState = eState;
			}

			break;
		}
	}
}

PCollisionProfile CCollisionManager::FindProfile(const string & strName)
{
	for (int i = 0; i < m_iProfileSize; ++i)
	{
		if (m_pProfile[i].strName == strName)
		{
			return &m_pProfile[i];
		}
	}

	return nullptr;
}

int CCollisionManager::GetChannelIndex(const string & strName) const
{
	for (int i = 0; i < m_iChannelSize; ++i)
	{
		if (m_pChannel[i].strName == strName)
		{
			return i;
		}
	}

	// Collision���� ä���� ������ �ִ� ������ ���� �ε����� ã�ƿ��� �����̶�� �� �� �ִ�.

	return -1;
}

bool CCollisionManager::Init()
{
	m_iPrevCollisionCount = 4096;

	m_pPrevCollisionAdj = new unsigned int*[m_iPrevCollisionCount];
	// 4096 ���� ��ü�� �����ϱ� ���ؼ� 4096���� ���� ������ش�.

	unsigned int iWidth = m_iPrevCollisionCount / 32;
	// ���� �ش��ϴ� ���� ��Ʈ������ ����ϱ� ���ؼ� 4096 / 32 = 128 unsigned int(32bit)�� �ɰ���.
	// �׷��� 128���� unsigned int��ŭ�� �Ҵ��ϸ� �ȴ�.

	for (unsigned int i = 0; i < m_iPrevCollisionCount; ++i)
	{
		m_pPrevCollisionAdj[i] = new unsigned int[iWidth];
		memset(m_pPrevCollisionAdj[i], 0, sizeof(unsigned int) * iWidth);
		// & ?????? & ???? 
		// [i]��ü�� 2���� �迭�̶� i���� �ּҸ� ��Ÿ���µ� ���⿡ &�� ���̸� ���� �ǹ� ?
		// ���⼭ ������ �� �� ����.
	}

	// �̷��� �ϸ�, �� ��� unsigned int ���� 128���� �Ҵ��Ͽ�, 
	// ������ 4096���� ��ü�� �����ϰ�, �� �ึ�� 4096���� bit�� ���� ��������� ������� ���̴�.

	m_iProfileSize = 0;
	m_iProfileCapacity = 64;
	m_pProfile = new CollisionProfile[m_iProfileCapacity];

	m_iChannelSize = 0;
	m_iChannelCapacity = 64;
	m_pChannel = new CollisionChannel[m_iChannelCapacity];

	// ä�ΰ� ���������� �����Ѵ�.
	
	// �ϴ�, �����ϰ� �÷��̾�, ���͸� ����.

	// �⺻������ Ignore
	AddChannel("Player");
	AddChannel("Stage");
	AddChannel("Monster");
	AddChannel("Mouse");
	AddChannel("Button");
	AddChannel("PlayerAttack");
	AddChannel("MonsterAttack");
	AddChannel("Star");
	AddChannel("Door");

	AddProfile("PlayerProfile");
	SetProfileChannelState("PlayerProfile", "Monster", CCS_BLOCK);
	SetProfileChannelState("PlayerProfile", "Stage", CCS_BLOCK);
	SetProfileChannelState("PlayerProfile", "MonsterAttack", CCS_BLOCK);
	SetProfileChannelState("PlayerProfile", "Star", CCS_BLOCK);
	SetProfileChannelState("PlayerProfile", "Door", CCS_BLOCK);

	// ��Ÿ�� �浹.

	// ���Ϳ� �浹.
	// ���������� �浹.


	AddProfile("MonsterProfile");
	SetProfileChannelState("MonsterProfile", "Player", CCS_BLOCK);
	SetProfileChannelState("MonsterProfile", "Stage", CCS_BLOCK);
	SetProfileChannelState("MonsterProfile", "PlayerAttack", CCS_BLOCK);
	// �÷��̾�� �浹. 
	// ���������� �浹.

	AddProfile("StageProfile");
	SetProfileChannelState("StageProfile", "Player", CCS_BLOCK);
	SetProfileChannelState("StageProfile", "Monster", CCS_BLOCK);
	SetProfileChannelState("StageProfile", "PlayerAttack", CCS_BLOCK);
	SetProfileChannelState("StageProfile", "MonsterAttack", CCS_BLOCK);
	SetProfileChannelState("StageProfile", "Star", CCS_BLOCK);
	// �÷��̾� ���� �浹.
	// ���������� ���� �浹.


	AddProfile("MouseProfile");
	SetProfileChannelState("MouseProfile", "Button", CCS_BLOCK);
	// ��ư�� �浹

	AddProfile("ButtonProfile");
	SetProfileChannelState("ButtonProfile", "Mouse", CCS_BLOCK);
	// ���콺�� �浹

	// �÷��̾� ����
	AddProfile("PlayerAttackProfile");
	SetProfileChannelState("PlayerAttackProfile", "Monster", CCS_BLOCK);
	SetProfileChannelState("PlayerAttackProfile", "Stage", CCS_BLOCK);


	// ���� ����
	AddProfile("MonsterAttackProfile");
	SetProfileChannelState("MonsterAttackProfile", "Player", CCS_BLOCK);
	SetProfileChannelState("MonsterAttackProfile", "Stage", CCS_BLOCK);


	// Star Profile
	AddProfile("StarProfile");
	SetProfileChannelState("StarProfile", "Player", CCS_BLOCK);
	SetProfileChannelState("StarProfile", "Stage", CCS_BLOCK);
	// ��Ÿ�� �÷��̾�� �浹.
	// ���������� �浹.

	// Door Profile
	AddProfile("DoorProfile");
	SetProfileChannelState("DoorProfile", "Player", CCS_BLOCK);


	return true;
}

void CCollisionManager::Collision(float fTime)
{
	m_SectionColliderList.clear();

	// ���� �߰��� �浹ü�� ���ؼ� �浹ó���� �ϴ� �Լ��̴�.
	// �ϴ�, ��� �浹ü�� ��ȸ�ϸ鼭 ȭ�鿡 ���̴� �浹ü�� SectionColliderList�� �߰��Ѵ�.

	list<CCollider*>::iterator iter;
	list<CCollider*>::iterator iterEnd = m_ColliderList.end();

	for (iter = m_ColliderList.begin(); iter != iterEnd; )
	{
		if (!(*iter)->IsActive())
		{
			// ����, �ش� �浹ü�� �׾��ִٸ�,
			SAFE_RELEASE((*iter));
			iter = m_ColliderList.erase(iter);
			iterEnd = m_ColliderList.end();
			continue;
			

			// �浹ü�� �浹ü �����ڿ��� �߰��Ǿ� �������� �����ȴ�.
			// ���� �浹ü ����Ʈ���� �縮���� �������� Release�Լ��� ȣ���ؾ� �Ѵ�.
		}

		else if (!(*iter)->IsEnable())
		{
			// Ȱ��ȭ off
			++iter;
			continue;
		}

		// ���⼭ ȭ�鿡 ���̴� ������ ������ SectionColliderList�� �浹ü�� �߰��ϴ� �ڵ尡
		// �ۼ��Ǿ�� ������, ���� ī�޶� ����.

		// �׷��� �ϴ��� ��� �浹ü�� �߰��Ѵ�.

		m_SectionColliderList.push_back(*iter);

		++iter;
	}

	// UIColliderList. �� ���ӿ����� UI�κ��� �ſ� ������ �ϴ� �־��ش�.

	m_SectionUIColliderList.clear();

	iterEnd = m_UIColliderList.end();

	for (iter = m_UIColliderList.begin(); iter != iterEnd;)
	{
		if (!(*iter)->IsActive())
		{
			SAFE_RELEASE((*iter));
			iter = m_ColliderList.erase(iter);
			iterEnd = m_ColliderList.end();
			continue;
		}

		else if (!(*iter)->IsEnable())
		{
			++iter;
			continue;
		}

		// UI�浹ü List���� Ȯ���Ͽ� UI�浹ü SectionColliderList �� �߰�.
		m_SectionUIColliderList.push_back(*iter);

		++iter;
	}



	// ���콺�� UI �浹ó��.
	CollisionMouseUI(fTime);


	CollisionUI(fTime); // UI������ �浹ó��.
	CollisionWorld(fTime); // ������Ʈ������ �浹ó��.


	/////// Stage�� ���� Ÿ�ϸ� ó���� �� ���.

	switch (GET_SINGLE(CGameManager)->GetGameMode())
	{
	case GM_PLAY:

			// Ÿ�ϸ��� ����ϴ� ���.
			TileCollision(fTime);
		break;


		// Ÿ�ϸ��� ������� �ʴ� ���.
	case GM_ANIMATION_EDIT:
	case GM_START:
		break;
	}

	//////



	// ����� �������� SectionColliderList�� ������ִ� �۾��� �ϰ� �ֵ�.
	// WorldObjList������ SectionObjList�� �̾Ƴ���
	// UIObjList������ SectionUIObjList�� �̾Ƴ��� ���̴�.

	// ���⿡ ����ȭ �κ��� ���� ������, �����ӿ�ũ���� ���ܵǾ���.
	// �ƹ�ư, �׳� ��� ��ǥ�� ��ü�� �浹��Ű�� �ִ� �����̴�.

	// ��� : ������ �׾��ִ� �浹ü, ��Ȱ��ȭ �浹ü�� �Ÿ��� ������ ������  List�� ������ָ�.
	// �Ʒ��� �Լ����� ó���ϴ� ����̴�.


	// ���� : �浹ü�� �ڽ��� ä�ΰ� ���������� ������ ������ �ִµ�,
	// �ƹ�ư ���� �ϰ� ���� ���� �浹ó���� ���ϴ� �浹ü ���� �浹�� �Ͼ�ٴ� ���̴�.

	// �浹ü���� �浹ó���� �ϰ�, Mouse�� UI�� �浹ó���� �����Ѵ�.
}

void CCollisionManager::CollisionWorld(float fTime)
{
	if (m_SectionColliderList.size() < 2)
		return;

	// �浹ó���� �����Ѵ�.

	list<CCollider*>::iterator iter;
	list<CCollider*>::iterator iterEnd = m_SectionColliderList.end();

	--iterEnd;

	for (iter = m_SectionColliderList.begin(); iter != iterEnd; ++iter)
	{

		// src�� �������ϰ� ä���� �޾Ƶд�.

		PCollisionProfile pSrcProfile = (*iter)->GetProfile();
		int iSrcChannelIndex = (*iter)->GetChannelIndex();
		CCollider* pSrc = (*iter);

		// �⺻���� �浹ó���� ����.
		list<CCollider*>::iterator iter1 = iter;

		++iter1;
		list<CCollider*>::iterator iter1End = m_SectionColliderList.end();

		for (; iter1 != iter1End; ++iter1)
		{
			// iter�� iter1�� ����Ű�� ���� �浹ó���� �غ���.
			// �ٵ� ���⼭ ������ �������Ͽ��� ������ ä�� ���¸� Ȯ���Ͽ� ó���� ���ش�.

			// Dest�� �������ϰ� ä���ε����� �޾Ƶд�.
			PCollisionProfile pDestProfile = (*iter1)->GetProfile();
			int	iDestChannelIndex = (*iter1)->GetChannelIndex();
			CCollider* pDest = (*iter1);

			// ���⼭ �浹ó���� ���� ������ ���Ѵ�.
			// �ϴ�, ä���� ������ �ִ� state�� ó���ϰų� ���ų�
			// �� �߿� �ϳ��ε�, �츮��, �Ѵ� ó���� ���ϴ� ��쿡�� ó���� �� ���̴�.

			// src�� �������Ͽ��� dest�� �ش��ϴ� ä���� ���¸� Ȯ���ϰ�
			// dest�� �������Ͽ��� src�� �ش��ϴ� ä���� ���¸� Ȯ���Ͽ� �浹ó���� ���� ������ ���ϴ� ���̴�.

			if (pSrcProfile->pChannelList[iDestChannelIndex].eState == CS_BLOCK &&
				pDestProfile->pChannelList[iSrcChannelIndex].eState == CS_BLOCK)
			{
				// �� �ý����� �ڽ��� ä���ε����ʹ� ������� �ڽ��� ���������� ������ �����ͼ�
				// ����� ä���ε����� ������ �ڽ��� �������Ͽ��� �� ������ Ȯ���Ͽ� 
				// ä���� ���¿� ���� ó���� ���� ���� ���ϴ� �ý����̴�.
				// ���� ������ �ִ� ä���� ���� �ڽ��� �Ҽ�ä���� �ǹ��ϱ�� ������,
				// ������ �������Ͽ��� �̰��� Ȯ�����ֱ� ���� ���̴�.

				// ��ü�����δ� src�� �������Ͽ� ��� ä���� �ε����� ������ 
				// �������Ͽ� �ִ� ä�� ������ Ȯ���Ѵ�. (���⼭ �ش� ä�ο� �ش��ϴ� ������Ʈ��
				// ������ ��� ó���� ���� ������ ���� ó���� ����ִ� �����̴�.)
				// ���� ������ Dest������ Ȯ�����ָ� �ȴ�.
				

				// �ƹ�ư �Ѵ� ó���� ���ϴ� ��쿡�� �浹ó���� ���ش�.

				
				if (pSrc->Collision(pDest))
				{
					// �浹�� �Ͼ ���


					// �浹ó���� Ȯ���ϴ� �Լ� Collision
					// Src�� �������� �ϵ� Dest�� �������� �ϵ� �ѹ��� ����ϸ� �׸��̴�.
					// �ƹ�ư, Src�� �浹 ���� Dest�� �浹ü�� ���Ͽ� Ư����Ȳ�� �´� �Լ��� ȣ���ϰ�
					// �浹�� �Ͼ ��쿡 true�� ��ȯ, �ƴ϶�� false�� ��ȯ.

					// ������ �浹�� �Ͼ������ Ȯ���ϴ�.
					if (!CheckPrevCollisionAdj(pSrc->GetSerialNumber(), pDest->GetSerialNumber()))
					{
						// ó�� �浹�� �Ͼ ����̴�.

						// ó�� �浹�� �Ͼ ��쿡�� ���ο��� �浹List�� �־��ش�.
						// + ������Ŀ� �ش� �浹������ ���.

						AddPrevCollisionAdj(pSrc->GetSerialNumber(), pDest->GetSerialNumber());
						pSrc->AddPrevCollisionList(pDest);
						pDest->AddPrevCollisionList(pSrc);

						// �浹���ۿ� �ش��ϴ� �Լ��� ȣ���Ѵ�.
						
						pSrc->Call(CS_BLOCK, pDest, fTime);
						pDest->Call(CS_BLOCK, pSrc, fTime);

					}
					else
					{
						// �浹 ���� �����̴�.

						pSrc->Call(CS_OVERLAP, pDest, fTime);
						pDest->Call(CS_OVERLAP, pSrc, fTime);
					}
				}

				// �浹�� �ƴ� ��쿡 ������ �浹�� �Ͼ���� Ȯ���Ѵ�.
				else if (CheckPrevCollisionAdj(pSrc->GetSerialNumber(), pDest->GetSerialNumber()))
				{
					// �浹�� �ƴѵ� ������ �浹�� �Ͼ���ٸ� ���� �� ������ �����̴�.

					// ��������� �������� ������, ���θ� ������� �浹����Ʈ���� �����ϸ�
					// release���¿� �ش��ϴ� �Լ��� ȣ�����ش�.
					DeletePrevCollisionAdj(pSrc->GetSerialNumber(), pDest->GetSerialNumber());
					pSrc->DeletePrevCollisionList(pDest);
					pDest->DeletePrevCollisionList(pSrc);

					pSrc->Call(CS_RELEASE, pDest, fTime);
					pDest->Call(CS_RELEASE, pSrc, fTime);
				}

				// else ������ �浹�� ���� �ʾҰ�, ���絵 �浹�� �ƴ� ��쿡�� 
				// �ƹ� �͵� �ƴϴ�.



			}
		}
	}
}

void CCollisionManager::CollisionUI(float fTime)
{

	// UIColliderList�� ���� ó���� �Ѵ�.

	if (m_SectionUIColliderList.size() >= 2)
	{
		list<CCollider*>::iterator	iter;
		list<CCollider*>::iterator	iterEnd = m_SectionUIColliderList.end();
		--iterEnd;

		for (iter = m_SectionUIColliderList.begin(); iter != iterEnd; ++iter)
		{
			list<CCollider*>::iterator	iter1 = iter;
			++iter1;
			list<CCollider*>::iterator	iter1End = m_SectionUIColliderList.end();

			CCollider*	pSrc = *iter;

			PCollisionProfile	pSrcProfile = pSrc->GetProfile();
			int	iSrcChannel = pSrc->GetChannelIndex();

			for (; iter1 != iter1End; ++iter1)
			{
				CCollider*	pDest = *iter1;

				PCollisionProfile	pDestProfile = pDest->GetProfile();
				int	iDestChannel = pDest->GetChannelIndex();

				if (pSrcProfile->pChannelList[iDestChannel].eState != CCS_IGNORE &&
					pDestProfile->pChannelList[iSrcChannel].eState != CCS_IGNORE)
				{
					// �浹 �Ǿ��� ���
					if (pSrc->Collision(pDest))
					{
						// ���� �����ӿ� �浹�� �Ǿ����� �Ǵ��Ѵ�.
						if (!CheckPrevCollisionAdj(pSrc->GetSerialNumber(),
							pDest->GetSerialNumber()))
						{
							// �� �浹ü�� ���� �浹���·� ����Ѵ�.
							AddPrevCollisionAdj(pSrc->GetSerialNumber(),
								pDest->GetSerialNumber());

							pSrc->AddPrevCollisionList(pDest);
							pDest->AddPrevCollisionList(pSrc);

							pSrc->Call(CS_BLOCK, pDest, fTime);
							pDest->Call(CS_BLOCK, pSrc, fTime);
						}

						else
						{
							pSrc->Call(CS_OVERLAP, pDest, fTime);
							pDest->Call(CS_OVERLAP, pSrc, fTime);
						}
					}

					// ���� �����ӿ��� �浹�� �ȵǰ� ���������ӿ�����
					// �浹���¶�� �浹�Ǵ� �浹ü�� ��������
					// ���¶�� ���̴�.
					else if (CheckPrevCollisionAdj(pSrc->GetSerialNumber(),
						pDest->GetSerialNumber()))
					{
						// �� �浹ü�� ���� �浹���¿��� �����Ѵ�.
						DeletePrevCollisionAdj(pSrc->GetSerialNumber(),
							pDest->GetSerialNumber());

						pSrc->DeletePrevCollisionList(pDest);
						pDest->DeletePrevCollisionList(pSrc);

						pSrc->Call(CS_RELEASE, pDest, fTime);
						pDest->Call(CS_RELEASE, pSrc, fTime);
					}
				}
			}
		}
	}
}

void CCollisionManager::ChangeScene()
{
	// ��� ��ȯ�� �Ͼ ��쿡�� �ش� ��鿡�� Ÿ�ϰ� �浹�� �����ߴ� ģ������ �����־�� �Ѵ�.
	// �ֳ��ϸ� ����� ���󰡱� ������ ������ Ÿ�ϰ� �浹�ߴ� ģ����� ������ �ʿ䰡 ���� ���̴�.
	Safe_Release_VecList(m_WorldObjList);
	Safe_Release_VecList(m_ColliderList);
	Safe_Release_VecList(m_UIColliderList);
	m_pMousePrevCollision = nullptr;
}

void CCollisionManager::CollisionMouseUI(float fTime)
{
	if (m_SectionUIColliderList.empty())
		return;

	CCollider*	pSrc = GET_SINGLE(CInput)->GetMouseCollider();
	PCollisionProfile	pSrcProfile = pSrc->GetProfile();
	int	iSrcChannel = pSrc->GetChannelIndex();

	list<CCollider*>::iterator	iter;
	list<CCollider*>::iterator	iterEnd = m_SectionUIColliderList.end();

	for (iter = m_SectionUIColliderList.begin(); iter != iterEnd; ++iter)
	{
		CCollider*	pDest = *iter;

		PCollisionProfile	pDestProfile = pDest->GetProfile();
		int	iDestChannel = pDest->GetChannelIndex();

		if (pSrcProfile->pChannelList[iDestChannel].eState != CCS_IGNORE &&
			pDestProfile->pChannelList[iSrcChannel].eState != CCS_IGNORE)
		{
			// �浹 �Ǿ��� ���
			if (pSrc->Collision(pDest))
			{
				// ���� �����ӿ� �浹�� �Ǿ����� �Ǵ��Ѵ�.
				if (!CheckPrevCollisionAdj(pSrc->GetSerialNumber(),
					pDest->GetSerialNumber()))
				{
					// ó�� �浹�� �Ͼ ����̴�.


					// ������ �����͵� �浹�� ���� ���� ���¶�� �׳� �ְ�.
					if (!m_pMousePrevCollision)
						m_pMousePrevCollision = pDest;

					else if (m_pMousePrevCollision != pDest)
					{
						// ������ �������� �浹�� �̷������ �ִ� ����� ������ �浹�ߴ� �浹ü����
						// ���踦 ���� ������ ����� ���ο� �浹ü�� �־��ش�.
						// �̶� Release�Լ��� ȣ�����ش�.

						// �� �浹ü�� ���� �浹���¿��� �����Ѵ�.
						DeletePrevCollisionAdj(pSrc->GetSerialNumber(),
							m_pMousePrevCollision->GetSerialNumber());

						pSrc->DeletePrevCollisionList(m_pMousePrevCollision);
						m_pMousePrevCollision->DeletePrevCollisionList(pSrc);

						pSrc->Call(CS_RELEASE, m_pMousePrevCollision, fTime);
						m_pMousePrevCollision->Call(CS_RELEASE, pSrc, fTime);

						m_pMousePrevCollision = pDest;
					}

					// �浹�� �Ͼ ��쿡�� ������ �浹�� �� ���¿� �׷��� ���� ���¸� �������
					// ���� �浹�� �� ���·� �浹ü�� �־��־�����...
					// ���� �浹���¸� ������ش�.

					// �� �浹ü�� ���� �浹���·� ����Ѵ�.
					AddPrevCollisionAdj(pSrc->GetSerialNumber(),
						pDest->GetSerialNumber());

					pSrc->AddPrevCollisionList(pDest);
					pDest->AddPrevCollisionList(pSrc);

					pSrc->Call(CS_BLOCK, pDest, fTime);
					pDest->Call(CS_BLOCK, pSrc, fTime);
				}

				else // �浹�� �Ǿ���, �������� �浹�̶��..
				{
					pSrc->Call(CS_OVERLAP, pDest, fTime);
					pDest->Call(CS_OVERLAP, pSrc, fTime);
				}

				return;

				// �������� �浹�� �� ��쿡�� ���Ŀ� �浹ó���� ���� �ʴ´�.
			}
		}
	}

	// �ƹ��� ��ü�͵� �浹�� �Ͼ�� ���� ���.
	// �׷���, ������ �浹�� ��ü�� �ִ� ���.
	// �浹���¿��� ����Ʈ���� ó���� ���ο� ��ü�� �浹�� �Ͼ ��쿡�� ���ְ� ������,
	// �̰��� �ٸ� ��ü���� �浹�� �Ͼ ��쿡 ������ ����̴�.
	// ����, �ٸ� �浹ü�� �浹�� ���� ���� ��쿡�� ������ �浹�� �Ǿ��� ��ü�� �ִٸ� ó���� ���־�� �Ѵ�.
	// Release �Լ� ȣ�����ְ�, �浹���� �������ִ� ó���� �ʿ��ϴ�(�ֳĸ� �ƹ����� �浹�� �� �ȰŴϱ�)

	if (m_pMousePrevCollision)
	{
		// �� �浹ü�� ���� �浹���¿��� �����Ѵ�.
		DeletePrevCollisionAdj(pSrc->GetSerialNumber(),
			m_pMousePrevCollision->GetSerialNumber());

		pSrc->DeletePrevCollisionList(m_pMousePrevCollision);
		m_pMousePrevCollision->DeletePrevCollisionList(pSrc);

		pSrc->Call(CS_RELEASE, m_pMousePrevCollision, fTime);
		m_pMousePrevCollision->Call(CS_RELEASE, pSrc, fTime);

		m_pMousePrevCollision = nullptr;
	}

	return;
}

void CCollisionManager::TileCollision(float fTime)
{
	CStage* pStage = GET_SINGLE(CSceneManager)->GetStage();

	// ���������� �����´�.

	if (pStage && pStage->IsTile())
	{
		// stage && Ÿ�� ���.

		// Ÿ�ϰ� ������Ʈ���� �浹��ų ���̴�.
		// ��� ������Ʈ�� ��ȸ�ϸ鼭 Ÿ�ϰ� �浹ó���� �Ѵ�.
		list<CObj*>::iterator iter;
		list<CObj*>::iterator iterEnd = m_WorldObjList.end();

		for (iter = m_WorldObjList.begin(); iter != iterEnd; )
		{
			if (!(*iter)->IsActive())
			{
				SAFE_RELEASE((*iter));
				iter = m_WorldObjList.erase(iter);
				iterEnd = m_WorldObjList.end();
				continue;
			}
			
			else if (!(*iter)->IsEnable())
			{
				++iter;
				continue;
			}


			// Ÿ�� �浹�� ���� �ʴ� ��ü���� Ȯ���Ͽ� ������ �ѱ��.
			if (!(*iter)->GetTileCollisionEnable())
			{
				++iter;
				continue;
			}

		// Ÿ�� �浹�� ���� �ʴ� ��ü�� ������ ����.

			// �׾� �ִ� ��� ���ְ� ����� ���� ������Ʈ
			// ���� �ʾҴµ� Ȱ��ȭ�� ���� ��� �ѱ��.

			// ���� �ʾҰ�, Ȱ��ȭ�� ���. �浹ó��..

			RectInfo tTileRC;
			Position tPos, tPrevPos;
			tPos = (*iter)->GetPos();
			tPrevPos = (*iter)->GetPrevPos();

			 // ������ġ, ������ġ�� �����´�.

			Position tMove = tPos - tPrevPos;
			// �̵����� ���ؿ´�.

			Position tPivot = (*iter)->GetPivot();
			// ������Ʈ�� Rect�� �׷����� ���ؼ� �Ǻ����� �����´�.

			Size tSize = (*iter)->GetStaticSize();
			// ������ �ʴ� Size�� �޾ƿ´�.
			// m_tSize �� ��쿡�� �ִϸ��̼��� ũ�⿡ ���� �ڵ����� �ٲ�� ������

	


			// ���⼺�� ���س���.  ����. 0. ������.
			int iDir = 0;

			
			if (tMove.x < 0)
				iDir = -1;

			else if (tMove.x > 0)
				iDir = 1;

			if ((*iter)->GetLineCollision())
			{
				iDir = 0;
			}


			// �̰����� ������ ��ü�� Line�� �浹�� �� ��쿡�� X�࿡ ���ؼ� ó������ �ʴ´�.
			// ���ÿ� �浹�� ���� ��쿡 ������ ���ư� �� �־�� �ϴµ�, �ۿ��� �İ�� ���� �ν��ؼ� ��ư��� ���Ѵ�.
			
			
			float fResult = 0.f;


			// X�̵� ó��.

			if (pStage->CheckWallW(fResult, tTileRC, *iter, iDir))
			{
				// Stage�� CheckWallW  �Լ��� ȣ���Ͽ� ���� �̵� / ������ �̵��� ó���Ѵ�.

				// �̵� ������ �������� �̵��Ҷ� Ÿ���� ������ ���� 
				// �̵� ���� �ȿ� �����ϴ��� �Ǵ��Ͽ� ������ ��쿡�� �о��ش�.
				// -> ?

				if (iDir == -1)
				{

					// �������� �̵��� �ϰ� �ֱ� ������ ���� x��ǥ�� ���� ���̴�
					
					Position tPrev = tPrevPos - tPivot * tSize;
					// ���� ��ġ���� ���� ����� ���Ѵ�.
					Position tNext = tPos - tPivot * tSize;
					// ���� ��ġ���� ���� ����� ���Ѵ�.

					if (tNext.x < tTileRC.r && tTileRC.r < tPrev.x)
					{
						(*iter)->UpdatePos((*iter)->GetPos() + Position(fResult, 0.f));

						if ((*iter)->GetTag() == "Kirby")
						{
							GET_SINGLE(CSceneManager)->CameraUpdate(fTime);

							pStage->BackGroundStop();
						}
						else
						{
							// �ٸ� ��ü

							/*
														enum OBJECT_TYPE
							{
								OT_NONE,
								OT_NORMAL_MONSTER,
								OT_PLAYER,
								OT_PLAYER_ATTACK,,
								OT_MONSTER_ATTACK,
								OT_BOSS_DDD,
								OT_ABILITY_MONSTER,
								OT_UI,
								OT_STAGE
							};

							*/

							switch ((*iter)->GetObjectType())
							{
							case OT_PLAYER_ATTACK:

								if ((*iter)->GetTag() == "VomitStar")
								{
									((CEffect*)(*iter))->SetDieTime(0.2f, true);

									GET_SINGLE(CSoundManager)->Play("AttackStarHit", CT_EFFECT);
									break;
									// �¿� ����ó��.. Line����
								}


								case OT_MONSTER_ATTACK:
								{
									CEffect* pEffect = (CEffect*)(*iter);

									pEffect->SetDieTime(0.4f, true);


								}
								break;
							}
							
						}
					}

					// ���� �����ӿ����� �簢���� ���� �������� �簢������ ���� �����  x��ġ�� ���Ѵ�.
					// �̷��� �� x ��ǥ�� �׾, Ÿ���� �ۿ��� �������� ������ �� ��쿡�� 
					// �ٽ� �з��������� ó���ϰ� �ִ�.

					// ���࿡ �߽ɺ��ʿ��� �������� �̵��ϸ鼭 ������ �ߴٸ�.
					// ���ǹ����� ������ �Ǿ ó������ �ʴ´�.
					// ��, X���� �ƴ� Y�࿡ ���� ó���� �ϵ��� �ϴ� ���̴�.
				}

				else if (iDir == 1)
				{
					// ������ : ������ ����� x ��ǥ�� ��.
					Position tPrev = tPrevPos + (Position(1.f, 1.f) - tPivot) * tSize;
					Position tNext = tPos + (Position(1.f, 1.f) - tPivot) * tSize;
					
					// Pivot�� �ݴ븦 �̿��ؼ� ������ �ϴ��� ���Ѵ�. (x��ǥ�� �̿���)

					if (tPrev.x < tTileRC.l && tNext.x > tTileRC.l)
					{
						(*iter)->UpdatePos((*iter)->GetPos() + Position(fResult, 0.f));

						if ((*iter)->GetTag() == "Kirby")
						{
							GET_SINGLE(CSceneManager)->CameraUpdate(fTime);

							pStage->BackGroundStop();


						}
						else
						{
							// �ٸ� ��ü
							switch ((*iter)->GetObjectType())
							{
							case OT_PLAYER_ATTACK:

								if ((*iter)->GetTag() == "VomitStar")
								{
									((CEffect*)(*iter))->SetDieTime(0.2f, true);

									GET_SINGLE(CSoundManager)->Play("AttackStarHit", CT_EFFECT);
									break;
									// �¿� ����ó��.. Line����
								}


							case OT_MONSTER_ATTACK:
							{
								CEffect* pEffect = (CEffect*)(*iter);

								pEffect->SetDieTime(0.4f, true);


							}
							break;
							}
						}
					}

					// �Ȱ��� Ÿ���� �ۿ��� �İ��� ��쿡�� ������ ƨ�ܳ������� �����Ѵ�(x)
				}
			}


			// Y �̵� ó��

			iDir = 0;

			if (tMove.y < 0)
				iDir = -1;

			else if (tMove.y > 0)
				iDir = 1;

		
			if ((*iter)->GetLineCollision())
			{
				// Line�浹�̸鼭 MiddleCollision�� true��� �Ҹ��� 
				// Y�࿡ ���� ó���� ���� ���ƾ� �ϴ� ��Ȳ�̴�.

				if((*iter)->GetMiddleCollision())
					iDir = 0;
				else
				{
					// �ݴ�� ���� �浹�̸鼭 MiddleCollision�� false��� �� ��쿡�� Ÿ���浹 y�� ó���� �ʿ��ϴ�.
					// �ٲ��� �ʴ´�.
				}
			}

			// ������ �浹�� �Ͼ�� ����� �Ʒ��� �������� ������ �ν�.
			else if ((*iter)->GetPrevLineCollision())
			{
				iDir = 1;
			}


		
			if (pStage->CheckWallH(fResult, tTileRC, *iter, iDir))
			{
				//* �Լ��� ȣ���� ���Ŀ� �浹�� Tile�� ��ġ������ ������ ������ �ȴ�.
				// ���� �츮�� ���� ������ �׾�鼭 �ܺο��� �������� �Ǵ��� �� �ִ� ���̴�.
				if (iDir == -1)
				{

					// ���� �ö󰡴� ���.


					// ���� �������� ��ǥ�� ���� �������� ��ǥ�� �̿��ϴµ� Top�� ��ǥ�� ������ �Ǵ��Ѵ�.

					Position tPrev = tPrevPos - tPivot * tSize;
					Position tNext = tPos - tPivot * tSize;

					// ���� ����� ��ǥ�� ���ؼ� y��ǥ�� �̿��Ѵ�.

					if (tPrev.y > tTileRC.b && tNext.y < tTileRC.b)
					{
						(*iter)->UpdatePos((*iter)->GetPos() + Position(0.f, fResult));

						if ((*iter)->GetTag() == "Kirby")
						{
							GET_SINGLE(CSceneManager)->CameraUpdate(fTime);
						}

					}

				}
				else if (iDir == 1)
				{
					// �Ʒ��� �������� ���.

					// �Ʒ��� Ȯ���Ѵ�.

					Position tPrev = tPrevPos + (Position(1.f, 1.f) - tPivot) * tSize;
					Position tNext = tPos + (Position(1.f, 1.f) - tPivot) * tSize;

					// ������ �ϴ��� ��ǥ�� �����´�.

					if ((tPrev.y < tTileRC.t && tNext.y > tTileRC.t) || (*iter)->GetPrevLineCollision())
					{
						// Ÿ�ϰ� ������ �浹����, �������� Ÿ�ϵ��� �浹ó���� ���ְ� �ִµ�,
						// ������ �ٷ� ���� �����ӿ��� ������ �浹�� �Ͼ ��쿡�� 
						// �׳� �ø��� ������ �Ѵ�.
						(*iter)->UpdatePos((*iter)->GetPos() + Position(0.f, fResult));
						(*iter)->StageCollisionEnable();

						if ((*iter)->GetTag() == "Kirby")
						{
							KirbyCollision(*iter, fTime);

							(*iter)->InputEnable();

						}
						else
						{
							switch ((*iter)->GetObjectType())
							{
							case OT_JUMP_STAR:
								((CJumpStar*)(*iter))->JumpDisEnable();

								break;

							case OT_BOSS_DDD:
							{
								CDDD* pDDD = (CDDD*)(*iter);

								if (pDDD->GetJump())
								{
									// ������ �ϴ� ���̸�,
									if (pDDD->JumpDown())
										pDDD->JumpDisEnable();

									pDDD->SetFalling(false);
									// �ö󰡴� ���°� �ƴ϶�, �������� ���¿��� ������ ��������.
								}
							}
								break;
							}
						}

						(*iter)->GravityTimeDisEnable();
						(*iter)->KnockUpEndDisEnable();
					



						// �Ʒ��� �������� ������ ��ŭ�� �÷��ִ� ��Ȳ���� �߷½ð��� �ʱ�ȭ�Ѵ�.

					}
				}
			}

			// �̷��� ����ó���� ���� ó���� ���� �ϰ� ���� ������Ʈ�� ó���Ѵ�.

			++iter;


			/*
			�¿��� �̵��� �˻��ϴ� ���. x�� ��ǥ�� �������ѵ� ���¿��� ������Ʈ�� y ��ġ�� ������
			Ÿ���� �ε����� ���س���,

			������ �̵��� �˻��ϴ� ���, y�� ��ǥ�� �������ѵ� ���¿��� ������Ʈ�� x ��ġ�� ������ 
			Ÿ���� �ε����� ���س��� �ִ�.


			case 1 : ���ʿ��� Ÿ���� �߰����� ����. ������Ʈ�� ������ Line�� �������� �Ͽ� �簢���� �İ��� ��߿�
			�� ��ŭ �ϴ� �������´�. �׸��� Y�� �����Ͽ� Ȯ���ϴ� ���� �ش� �κ��� Ÿ���� ��ġ�� �ʾƼ� 
			���� ó������ �ʰ� ������.

			case 2 : Ÿ���� �߰����� �������� ���ϸ� ���� ������ Line�� ������ Ȯ���Ͽ����� �簢�� ���ο��� ��� ������
			���� ó���� ���� �ʰ� Y�࿡ ���ؼ��� �İ�� �� ��ŭ ���������� �ȴ�.
			
			case 3 : �������� ���ϸ� Ÿ���� �������� ����. x�� ���Ͽ� �İ�� �������´�. Y�� üũ X

			case 4 : �������� ���ϸ� Ÿ���� �������� ����. ��Ȯ�� �밢��. x ���������� ��.

			case 5 : ������ �������� ���ϸ� Ÿ���� ���ʿ� �浹. ������Ʈ�� �������� �浹�� �� ��Ȳ���� �̹�
			Ÿ���� ���� ������ ���ִ� ��Ȳ. X�� ���ؼ� ó������ �ʰ� �ȴ�.
			Y�� ó���ؼ� ����.

			case 6 : Ÿ���� ������ �������� ��� x�� ������� �ʴ´�.
			(������ ���ϵ� �������� ���ϵ� �浹�� �� �������� �̹� Ÿ���� ���ο� ������. = X�� ó�� x)
			�� ��Ȳ���� Y�࿡ ���ؼ��� ó���� �ȴ�. ���� .
			*/
		}
	}

}


