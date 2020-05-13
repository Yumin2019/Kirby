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
	// 충돌체 관리자에서는 UI충돌체와, 오브젝트 충돌체를 날려주는 처리를 하고 있다.
	// 해당 충돌체가 사라지는 과정에서 자신과 충돌했던 충돌체에게 자신의 정보를 날려주는 처리를 하고 있고
	// 충돌체 관리자에게 접근하여 인접행렬을 정리해주는 처리를 해주고 있다.
	// 근데, 여기서 중요한 점은, CollisionManager가 이미 사라져서 없어진 객체라는 것이다.
	// 없어진 객체를 다시 호출하여 사용하고 있다. (정확하게는 사라진 객체를 다시 생성하여 사용하는 개념이다)
	// 당연히 이런 처리는 위험하다.
	// 충돌체가 사라지는 과정에서 충돌 관리자 클래스가 필요한데,
	// 충돌 관리자 클래스가 없으니 당연히 문제가 생길 것이다.
	// 따라서, 충돌 관리자가 사라지기 이전에 충돌체를 먼저 정리하는 과정을 거친다면 아무런 문제도 생기지 않을 것이다.
	// 이런 이유로 Clear함수가 필요하다.
	// 충돌관리자가 사리지기 이전에 미리 충돌체를 날리는 시스템이 필요하니까.

	// 인접행렬을 날린다. 인접행렬은 충돌체가 사라지는 과정에서 사용할 것이다.
	// 따라서 인접행렬을 날리고 충돌체를 날릴 것이 아니라 충돌체를 날리고, 인접행렬을 이용하도록 하고
	// 그 이후에 안전하게 인접행렬을 날리는 것이 맞을 것 같다.


	// 소속 충돌체를 받아놓았던 ColliderList와 UIColliderList를 날린다.
	Safe_Release_VecList(m_ColliderList);
	Safe_Release_VecList(m_UIColliderList);

	// 해당 List의 경우에는 타일 충돌처리를 하기 위해 받아둔  List 이다.
	// 추가할 때 AddRef가 일어났으므로 release를 해준다.
	
	// 충돌체는 사라지면서, 인접행렬의 정보를 없애고, 자신과 충돌했던 충돌체에 대해서
	// 함수를 호출, 충돌 관계를 없애주는 처리를 하고 있다.
	// 이렇게 모든 충돌체를 없애주고, 이후에 인접행렬을 없앤다.
	for (unsigned int i = 0; i < m_iPrevCollisionCount; ++i)
	{
		SAFE_DELETE_ARRAY(m_pPrevCollisionAdj[i]);
	}

	SAFE_DELETE_ARRAY(m_pPrevCollisionAdj);

	// 할당받고 있던 Channel과 Profile정보를 날린다.
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

	// 각 SerialNumber가 가리키는 열 인덱스를 구한다.
	// 몇 번째 블럭에 있는지 구하는 것이다.

	// 몇 번째 비트에 추가를 해야하는지 구해야한다.
	unsigned int iSrcBit = 31 - iSrc % 32;
	unsigned int iDestBit = 31 - iDest % 32;

	// SerialNumber % 32를 통해서 몇 번째의 인덱스인지 구할 수 있다.
	// Src인덱스는 구해둔 상태이고, 이 객체와 Dest객체의 Bit를 구하는 과정이다.
	// 만약에 33번이라면, 첫번째 비트 자리에 1을 줘야할 것이다.
	// 그래서 1을 이용해서 bit를 옮기면 되는데 
	// 비트의 특성상 0000000... 1 이런 식으로 구성이 되어있다.
	// 즉, 우리는 뒤에서 부터 비트를 밀어서 1번의 자리로 오게 만들어야 하는 것이다.
	// 따라서, iSrcBit와 iDestBit는 옮겨야할 비트 수를 의미하도록 하여,
	// 31 - x % 32를 통해서,
	// x가 32라면, 블럭의 첫번째 자리에 있어야 한다.
	// 따라서, 31 - 0만큼 오른쪽에 있는 1을 왼쪽으로 밀어서 0번째짜리를 true로 만들 수 있다.
	
	// x = 2라면, 30개의 bit를 옮겨서, 
	// 1번째 자리의 bit만 true로 만들 수 있는 것이다.

	unsigned int iSrcAdd = 1 << iSrcBit;
	unsigned int iDestAdd = 1 << iDestBit;

	// 이렇게 하여 우리가 원하는  bit (Dest에 해당하는 비트)만 true로 하고 나머지는 false로 만들었다.

	// 이것을 OR연산하여 인접행렬에서 해당 정보를 추가하는 것이다.
	// SRC 기준의 DESTBIT
	// DEST 기준의 SRCBIT

	// Src객체를 기준으로 Dest의 Bit가 있는 곳으로 접근하여 . . .
	// Dest의 Bit에 해당하는 자리만 true로 해준 unsigned int 형 변수와 OR연산.
	// 따라서, Src객체가, Dest의 객체와 충돌했다는 것을 저장.
	m_pPrevCollisionAdj[iSrc][iDestIdx] |= iDestAdd;
	m_pPrevCollisionAdj[iDest][iSrcIdx] |= iSrcAdd;
	// Dest를 기준으로하여 SrcBit가 있는 곳으로 접근.
	// Src자리의 bit만 true로 해준 unsigned int 형을 OR연산하여,
	// Src의 bit자리를 true
}

void CCollisionManager::KirbyCollision(CObj* pObj, float fTime)
{
	GET_SINGLE(CSceneManager)->CameraUpdate(fTime);

	CPlayer* pPlayer = (CPlayer*)pObj;

	if (pPlayer->GetJump())
	{
		pPlayer->JumpDisEnable();

		CAnimation* pAni = pPlayer->GetAnimation();

		// 점프중에 현재 애니메이션이 KirbyAttackRight / AttackLeft인 경우에는
		// 모션을 바꾸지 않는다. (일반 커비)

		switch (pPlayer->GetKirbyType())
		{
		case KIRBY_NORMAL:
			if ((pAni->GetCurrentAnimation() != "KirbyAttackLeft") &&
				(pAni->GetCurrentAnimation() != "KirbyAttackRight"))
				pPlayer->ChangeDefaultClip();

		case KIRBY_MIKE:
			// 공격 시간이 아니라면 바꾼다.
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

			// 스워드 커비인데, 점프 공격을 하고 있지 않은 경우에 클립을 교체한다.

			break;
		}
			

		if (pPlayer->GetRun())
		{
			if (!KEYPRESS("MoveRight") && !KEYPRESS("MoveLeft"))
				pPlayer->RunDisEnable();
		}

		// 점프중에 Run 인 경우 좌우 키를 누르고 있는 경우에 Run상태를 유지하면 되고,
		// 좌우키를 누르고 있지 않은 경우에 Run을 풀어버리면 된다.
	}

	else if (pPlayer->GetBallon())
	{
		// 일단은 Ballon DisEnable
		// 풍선 상태에서 바닥으로 떨어진 경우에는 
		// 입김 애니메이션이 호출이 되면서 기본 애니메이션으로 돌아가게 되야 한다.
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

	// 추가와 동일하게, Src와 Dest의 bit만 true로 해준 unsigned int형을 만든다.
	// Src기준으로 Dest의 블럭에 접근, Dest의 bit위치를 제외하고 0으로 채운 unsigned int형 변수를
	// AND 연산하여, DestBit만을 확인하여 충돌이라면, 그 부분을 없애준다.

	if (m_pPrevCollisionAdj[iSrc][iDestIdx] & iDestAdd)
		m_pPrevCollisionAdj[iSrc][iDestIdx] ^= iDestAdd;
	// XOR 같으면 false값을 주도록.

	// & 연산에서 true가 나온다는 것은 0이 아닌 값이 나온다는 의미이다.
	// 한자리만 true인 unsinged int형과 &연산을 통해서 0이 아닌 값이 나온다는 것.
	// 만약에 연산을 이런 식으로 하게 된다면..
	// 11110011
	// 00010000
	//=00010000 4번째 자리에서 1이 나오게 되어 0이 아니게 된다.
	// 우리는 4번째 자리의 bit만 확인하면 되는데, 실제로 나머지 bit는 &연산을 진행하는 순간.
	// 0으로 채워지고, 오직 4번째 자리의 bit에 의해서 0이거나 0이 아닌 값이 나오게 된다.

	// 11110011
	// 00001000 이라면.
	//=00000000 이 된다.
	// 5번째 자리이외에는 0이므로 &연산을 통해서 5번째 자리 이외에는 반드시 0이 된다.
	// 따라서 5번째 자리에 의해서 0이거나 0이 아닌 값이 나온다.
	// 만약 5번째 자리가 true라면 0이 아닌 값이 나와서 true가 되고
	// 5번째 자리가 0이라면 &연산의 결과는 false가 되는 것이다.

	// 즉, &연산을 통해서 우리는 우리가 원하는 bit만 확인한 결과를 얻을 수 있는 것이다.
	// 해당 bit가 true면 true.
	// 해당 bit가 false면 false.
	if (m_pPrevCollisionAdj[iDest][iSrcIdx] & iSrcAdd)
		m_pPrevCollisionAdj[iDest][iSrcIdx] ^= iSrcAdd;

	//  Dest를 기준으로 Src의 bit를 확인하여 true라면,
	// XOR연산을 통해서, 우리가 원하는 bit만을 0으로 만드는 처리가 가능하다.

	// ex : 100100
	//	    100000 xor
	//resul	000100

	// 즉 우리가 원하는 첫번째 비트만을 false로 만들어서 충돌상태를 풀어버린 것이다.
	// XOR 연산의 특성상, 자신과 다른 경우에만 true를 뱉는다.
	// 즉, 우리가 원하는 비트를 제외하고는 0과 XOR 연산을 하게 되는데,
	// 원래 0인 자리는 같아져서 0이 나올 것이며 1인 자리는 달라서 1이 나오는 것이다.

	// 10101
	// 00000
	// 10101 이는 곧, 본래의 결과가 그대로 유지가 되는 현상이 나타난다.

	// 반면 우리가 원하는 비트의 자리의 경우에는 1이기 때문에
	// 해당 자리와 만나서 false로 변한다.
	
	// 결론 : 확인하는 bit는 false(충돌상태를 없애고) 기존의 bit는 그대로 냅두고 있다.



}

bool CCollisionManager::CheckPrevCollisionAdj(unsigned int iSrc, unsigned int iDest)
{
	unsigned int	iDestIdx = iDest / 32;

	unsigned int	iDestBit = 31 - iDest % 32;

	unsigned int	iDestAdd = 1 << iDestBit;

	if (m_pPrevCollisionAdj[iSrc][iDestIdx] & iDestAdd)
		return true;

	// 충돌처리를 확인하는 것은 한쪽만 확인해보면 된다.
	// 따라서, 해당 부분과 &연산을 하여, 해당 bit의 상태가 true인지 구한다.
	// 여기서 true가 나오면, 충돌 상태라는 것이고 false면 충돌상태가 아니라는 것이다.

	return false;
}

void CCollisionManager::AddWorldObjList(CObj * pObj)
{
	// 근데 해당 오브젝트 리스트의 경우에는 타일과 충돌이 일어나기 때문에 UI는 처리하면 안 된다.
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
	// 충돌체는 CollisionManager의 참조를 받는 것이다.
	// 충돌체는 기본적으로 오브젝트와 CollisionManager의 참조를 받게 되어 레퍼런스 카운터가 2가 된다.
	
	if (pCollider->GetTag() == "MouseUI")
		return;
	// 마우스 충돌체는 넣어주지 않는다. 따로 처리하기 위함이다. (중복처리를 막기위함)

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
	// pScene을 소속으로 하는 충돌체를 모두 날린다.

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
	// 프로파일을 추가하는 함수이다.

	// 프로파일의 할당량을 넘어가는지 체크한다.
	if (m_iProfileCapacity == m_iProfileSize)
		return;

	else if (FindProfile(strName))
		return;

	// 이미 해당 이름을 가진 프로파일이 있는 경우도 제외

	m_pProfile[m_iProfileSize].strName = strName;
	// 추가할 인덱스에 해당하는 할당된 공간에 이름을 지정.

	// 새로 생성한 채널에 채널 할당량 만큼 채널의 공간을 할당한다.
	m_pProfile[m_iProfileSize].pChannelList = new CollisionChannel[m_iChannelCapacity];
	// 여기서 채널의 사이즈만큼 할당을 하는 것이 아니라, 할당량만큼 주고 있다는 것을 주목하자.
	// 그러니까, 나중에 추가되는 채널에도 대응할 수 있다.

	// 프로파일은 각 채널의 정보를 따로따로 가지고 있다.
	for (int i = 0; i < m_iChannelSize; ++i)
	{
		m_pProfile[m_iProfileSize].pChannelList[i] = m_pChannel[i];
		// 충돌 관리자는 프로파일의 채널과는 상관없이 채널을 할당하여 어떤 채널이 있는지 
		// 정보를 확인할 수 있도록 한다.
		// 따라서, 이 기본적으로 들고있는 채널을 하나씩 순회하면서 새로 할당한 부분에
		// 값을 복사해준다.

		// 단순히 이름과 상태 정보만을 넘겨주는 처리를 하고 있다.
		// 요약 : 새로 할당한 채널에 정보를 채워준다.
	}
	
	// 새로만든 프로파일은 채널의 개수가 0일 것이다.
	m_pProfile[m_iProfileSize].m_iChannelSize = m_iChannelSize;

	// 프로파일의 수를 증가한다.
	++m_iProfileSize;
}

void CCollisionManager::AddChannel(const string & strName,
	COLLISION_CHANNEL_STATE eDefaultState)
{
	// 채널을 추가하는 과정이다.

	// 해당 이름을 가진 채널이 있는 경우, 이미 할당량이 꽉 찬 경우를 제외.

	if (m_iChannelCapacity == m_iChannelSize)
		return;

	else if (GetChannelIndex(strName) != -1) // -1이 아니라는 것은 이미 해당이름을 가진 채널이 존재한다는 것.
		return;

	// 채널을 생성하고, 모든 프로파일을 순회하면서 이 채널의 정보를 추가한다.
	// 충돌체 관리자에서 해당 채널의 정보를 추가하고, 
	// 이 채널 값을 토대로 하여, 프로파일에 해당 값을 복사해준다.

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
	// 해당 이름을 가진 Profile에서 채널을 찾아서, 해당 채널의 상태를 변경하는 함수이다.

	for (int i = 0; i < m_iProfileSize; ++i)
	{
		if (m_pProfile[i].strName == strProfile)
		{
			// 채널을 찾아라.
			int iChannelIndex = GetChannelIndex(strChannel);

			// 여기서 채널 이름에 따라서 -1이 나올 가능성이 있다.

			if (iChannelIndex == -1)
				return;

			// -1이 아닌경우에 해당 채널의 값을 바꿔버린다.

			m_pProfile[i].pChannelList[iChannelIndex].eState = eState;

			return;
		}
	}
}

void CCollisionManager::SetProfileChannelStateAll(const string & strProfile, 
	COLLISION_CHANNEL_STATE eState)
{
	// 해당 프로파일에 있는 모든 채널의 상태를 eState로 변경한다.

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

	// Collision에서 채널을 가지고 있는 이유는 단지 인덱스를 찾아오기 위함이라고 할 수 있다.

	return -1;
}

bool CCollisionManager::Init()
{
	m_iPrevCollisionCount = 4096;

	m_pPrevCollisionAdj = new unsigned int*[m_iPrevCollisionCount];
	// 4096 개의 객체를 지원하기 위해서 4096개의 행을 만들어준다.

	unsigned int iWidth = m_iPrevCollisionCount / 32;
	// 열에 해당하는 값은 비트단위로 사용하기 위해서 4096 / 32 = 128 unsigned int(32bit)로 쪼갠다.
	// 그러면 128개의 unsigned int만큼을 할당하면 된다.

	for (unsigned int i = 0; i < m_iPrevCollisionCount; ++i)
	{
		m_pPrevCollisionAdj[i] = new unsigned int[iWidth];
		memset(m_pPrevCollisionAdj[i], 0, sizeof(unsigned int) * iWidth);
		// & ?????? & ???? 
		// [i]자체가 2차원 배열이라서 i행의 주소를 나타내는데 여기에 &를 붙이면 무슨 의미 ?
		// 여기서 오류가 난 것 같다.
	}

	// 이렇게 하면, 각 행당 unsigned int 형을 128개씩 할당하여, 
	// 행으로 4096개의 객체에 접근하고, 각 행마다 4096개의 bit를 가진 인접행렬이 만들어진 것이다.

	m_iProfileSize = 0;
	m_iProfileCapacity = 64;
	m_pProfile = new CollisionProfile[m_iProfileCapacity];

	m_iChannelSize = 0;
	m_iChannelCapacity = 64;
	m_pChannel = new CollisionChannel[m_iChannelCapacity];

	// 채널과 프로파일을 생성한다.
	
	// 일단, 간단하게 플레이어, 몬스터만 생성.

	// 기본적으로 Ignore
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

	// 스타와 충돌.

	// 몬스터와 충돌.
	// 스테이지와 충돌.


	AddProfile("MonsterProfile");
	SetProfileChannelState("MonsterProfile", "Player", CCS_BLOCK);
	SetProfileChannelState("MonsterProfile", "Stage", CCS_BLOCK);
	SetProfileChannelState("MonsterProfile", "PlayerAttack", CCS_BLOCK);
	// 플레이어와 충돌. 
	// 스테이지와 충돌.

	AddProfile("StageProfile");
	SetProfileChannelState("StageProfile", "Player", CCS_BLOCK);
	SetProfileChannelState("StageProfile", "Monster", CCS_BLOCK);
	SetProfileChannelState("StageProfile", "PlayerAttack", CCS_BLOCK);
	SetProfileChannelState("StageProfile", "MonsterAttack", CCS_BLOCK);
	SetProfileChannelState("StageProfile", "Star", CCS_BLOCK);
	// 플레이어 몬스터 충돌.
	// 스테이지는 별과 충돌.


	AddProfile("MouseProfile");
	SetProfileChannelState("MouseProfile", "Button", CCS_BLOCK);
	// 버튼과 충돌

	AddProfile("ButtonProfile");
	SetProfileChannelState("ButtonProfile", "Mouse", CCS_BLOCK);
	// 마우스와 충돌

	// 플레이어 공격
	AddProfile("PlayerAttackProfile");
	SetProfileChannelState("PlayerAttackProfile", "Monster", CCS_BLOCK);
	SetProfileChannelState("PlayerAttackProfile", "Stage", CCS_BLOCK);


	// 몬스터 공격
	AddProfile("MonsterAttackProfile");
	SetProfileChannelState("MonsterAttackProfile", "Player", CCS_BLOCK);
	SetProfileChannelState("MonsterAttackProfile", "Stage", CCS_BLOCK);


	// Star Profile
	AddProfile("StarProfile");
	SetProfileChannelState("StarProfile", "Player", CCS_BLOCK);
	SetProfileChannelState("StarProfile", "Stage", CCS_BLOCK);
	// 스타는 플레이어와 충돌.
	// 스테이지와 충돌.

	// Door Profile
	AddProfile("DoorProfile");
	SetProfileChannelState("DoorProfile", "Player", CCS_BLOCK);


	return true;
}

void CCollisionManager::Collision(float fTime)
{
	m_SectionColliderList.clear();

	// 현재 추가된 충돌체에 대해서 충돌처리를 하는 함수이다.
	// 일단, 모든 충돌체를 순회하면서 화면에 보이는 충돌체만 SectionColliderList에 추가한다.

	list<CCollider*>::iterator iter;
	list<CCollider*>::iterator iterEnd = m_ColliderList.end();

	for (iter = m_ColliderList.begin(); iter != iterEnd; )
	{
		if (!(*iter)->IsActive())
		{
			// 만약, 해당 충돌체가 죽어있다면,
			SAFE_RELEASE((*iter));
			iter = m_ColliderList.erase(iter);
			iterEnd = m_ColliderList.end();
			continue;
			

			// 충돌체는 충돌체 관리자에게 추가되어 참조값이 증가된다.
			// 따라서 충돌체 리스트에서 사리지는 과정에서 Release함수를 호출해야 한다.
		}

		else if (!(*iter)->IsEnable())
		{
			// 활성화 off
			++iter;
			continue;
		}

		// 여기서 화면에 보이는 영역을 가지고 SectionColliderList에 충돌체를 추가하는 코드가
		// 작성되어야 하지만, 아직 카메라가 없다.

		// 그래서 일단은 모든 충돌체를 추가한다.

		m_SectionColliderList.push_back(*iter);

		++iter;
	}

	// UIColliderList. 내 게임에서는 UI부분이 매우 없지만 일단 넣어준다.

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

		// UI충돌체 List에서 확인하여 UI충돌체 SectionColliderList 에 추가.
		m_SectionUIColliderList.push_back(*iter);

		++iter;
	}



	// 마우스와 UI 충돌처리.
	CollisionMouseUI(fTime);


	CollisionUI(fTime); // UI끼리의 충돌처리.
	CollisionWorld(fTime); // 오브젝트끼리의 충돌처리.


	/////// Stage를 통한 타일맵 처리가 들어갈 장소.

	switch (GET_SINGLE(CGameManager)->GetGameMode())
	{
	case GM_PLAY:

			// 타일맵을 사용하는 경우.
			TileCollision(fTime);
		break;


		// 타일맵을 사용하지 않는 경우.
	case GM_ANIMATION_EDIT:
	case GM_START:
		break;
	}

	//////



	// 참고로 위에서는 SectionColliderList를 만들어주는 작업을 하고 있따.
	// WorldObjList에서는 SectionObjList를 뽑아내고
	// UIObjList에서는 SectionUIObjList를 뽑아내는 중이다.

	// 여기에 최적화 부분이 들어가야 하지만, 프레임워크에서 제외되었다.
	// 아무튼, 그냥 모든 좌표의 물체를 충돌시키고 있는 상태이다.

	// 결론 : 위에서 죽어있는 충돌체, 비활성화 충돌체를 거르고 깨끗한 상태의  List를 만들어주면.
	// 아래의 함수에서 처리하는 방식이다.


	// 참고 : 충돌체는 자신의 채널과 프로파일의 개념을 가지고 있는데,
	// 아무튼 내가 하고 싶은 말은 충돌처리를 원하는 충돌체 끼리 충돌이 일어난다는 것이다.

	// 충돌체끼리 충돌처리를 하고, Mouse와 UI의 충돌처리를 진행한다.
}

void CCollisionManager::CollisionWorld(float fTime)
{
	if (m_SectionColliderList.size() < 2)
		return;

	// 충돌처리를 진행한다.

	list<CCollider*>::iterator iter;
	list<CCollider*>::iterator iterEnd = m_SectionColliderList.end();

	--iterEnd;

	for (iter = m_SectionColliderList.begin(); iter != iterEnd; ++iter)
	{

		// src의 프로파일과 채널을 받아둔다.

		PCollisionProfile pSrcProfile = (*iter)->GetProfile();
		int iSrcChannelIndex = (*iter)->GetChannelIndex();
		CCollider* pSrc = (*iter);

		// 기본적인 충돌처리와 동일.
		list<CCollider*>::iterator iter1 = iter;

		++iter1;
		list<CCollider*>::iterator iter1End = m_SectionColliderList.end();

		for (; iter1 != iter1End; ++iter1)
		{
			// iter와 iter1이 가리키는 값을 충돌처리를 해본다.
			// 근데 여기서 서로의 프로파일에서 상대방의 채널 상태를 확인하여 처리를 해준다.

			// Dest의 프로파일과 채널인덱스를 받아둔다.
			PCollisionProfile pDestProfile = (*iter1)->GetProfile();
			int	iDestChannelIndex = (*iter1)->GetChannelIndex();
			CCollider* pDest = (*iter1);

			// 여기서 충돌처리를 할지 말지를 정한다.
			// 일단, 채널이 가지고 있는 state는 처리하거나 말거나
			// 둘 중에 하나인데, 우리는, 둘다 처리를 원하는 경우에만 처리를 할 것이다.

			// src의 프로파일에서 dest에 해당하는 채널의 상태를 확인하고
			// dest의 프로파일에서 src에 해당하는 채널의 상태를 확인하여 충돌처리를 할지 말지를 정하는 것이다.

			if (pSrcProfile->pChannelList[iDestChannelIndex].eState == CS_BLOCK &&
				pDestProfile->pChannelList[iSrcChannelIndex].eState == CS_BLOCK)
			{
				// 이 시스템은 자신의 채널인덱스와는 상관없이 자신의 프로파일의 정보를 가져와서
				// 상대의 채널인덱스를 가져와 자신의 프로파일에서 이 정보를 확인하여 
				// 채널의 상태에 따라 처리를 할지 말지 정하는 시스템이다.
				// 내가 가지고 있는 채널의 값은 자신의 소속채널을 의미하기는 하지만,
				// 상대방의 프로파일에서 이것을 확인해주기 위한 값이다.

				// 구체적으로는 src의 프로파일에 상대 채널의 인덱스를 가지고 
				// 프로파일에 있는 채널 정보를 확인한다. (여기서 해당 채널에 해당하는 오브젝트를
				// 만났을 경우 처리를 할지 말지에 대한 처리가 들어있는 개념이다.)
				// 같은 식으로 Dest에서도 확인해주면 된다.
				

				// 아무튼 둘다 처리를 원하는 경우에만 충돌처리를 해준다.

				
				if (pSrc->Collision(pDest))
				{
					// 충돌이 일어난 경우


					// 충돌처리를 확인하는 함수 Collision
					// Src를 기준으로 하든 Dest를 기준으로 하든 한번만 출력하면 그만이다.
					// 아무튼, Src의 충돌 모양과 Dest의 충돌체를 비교하여 특정상황에 맞는 함수를 호출하고
					// 충돌이 일어난 경우에 true를 반환, 아니라면 false를 반환.

					// 이전에 충돌이 일어났었는지 확인하다.
					if (!CheckPrevCollisionAdj(pSrc->GetSerialNumber(), pDest->GetSerialNumber()))
					{
						// 처음 충돌이 일어난 경우이다.

						// 처음 충돌이 일어난 경우에는 서로에게 충돌List로 넣어준다.
						// + 인접행렬에 해당 충돌정보를 기록.

						AddPrevCollisionAdj(pSrc->GetSerialNumber(), pDest->GetSerialNumber());
						pSrc->AddPrevCollisionList(pDest);
						pDest->AddPrevCollisionList(pSrc);

						// 충돌시작에 해당하는 함수를 호출한다.
						
						pSrc->Call(CS_BLOCK, pDest, fTime);
						pDest->Call(CS_BLOCK, pSrc, fTime);

					}
					else
					{
						// 충돌 중인 상태이다.

						pSrc->Call(CS_OVERLAP, pDest, fTime);
						pDest->Call(CS_OVERLAP, pSrc, fTime);
					}
				}

				// 충돌이 아닌 경우에 이전에 충돌이 일어났는지 확인한다.
				else if (CheckPrevCollisionAdj(pSrc->GetSerialNumber(), pDest->GetSerialNumber()))
				{
					// 충돌이 아닌데 이전에 충돌이 일어났었다면 지금 막 떨어진 상태이다.

					// 인접행렬의 정보에서 날리고, 서로를 대상으로 충돌리스트에서 삭제하며
					// release상태에 해당하는 함수를 호출해준다.
					DeletePrevCollisionAdj(pSrc->GetSerialNumber(), pDest->GetSerialNumber());
					pSrc->DeletePrevCollisionList(pDest);
					pDest->DeletePrevCollisionList(pSrc);

					pSrc->Call(CS_RELEASE, pDest, fTime);
					pDest->Call(CS_RELEASE, pSrc, fTime);
				}

				// else 이전에 충돌이 나지 않았고, 현재도 충돌이 아닌 경우에는 
				// 아무 것도 아니다.



			}
		}
	}
}

void CCollisionManager::CollisionUI(float fTime)
{

	// UIColliderList에 대한 처리를 한다.

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
					// 충돌 되었을 경우
					if (pSrc->Collision(pDest))
					{
						// 이전 프레임에 충돌이 되었는지 판단한다.
						if (!CheckPrevCollisionAdj(pSrc->GetSerialNumber(),
							pDest->GetSerialNumber()))
						{
							// 두 충돌체를 서로 충돌상태로 등록한다.
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

					// 현재 프레임에서 충돌이 안되고 이전프레임에서는
					// 충돌상태라면 충돌되던 충돌체와 떨어지는
					// 상태라는 것이다.
					else if (CheckPrevCollisionAdj(pSrc->GetSerialNumber(),
						pDest->GetSerialNumber()))
					{
						// 두 충돌체를 서로 충돌상태에서 해제한다.
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
	// 장면 전환이 일어난 경우에는 해당 장면에서 타일과 충돌을 도모했던 친구들을 날려주어야 한다.
	// 왜냐하면 장면이 날라가기 때문에 기존에 타일과 충돌했던 친구들과 연락할 필요가 없는 것이다.
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
			// 충돌 되었을 경우
			if (pSrc->Collision(pDest))
			{
				// 이전 프레임에 충돌이 되었는지 판단한다.
				if (!CheckPrevCollisionAdj(pSrc->GetSerialNumber(),
					pDest->GetSerialNumber()))
				{
					// 처음 충돌이 일어난 경우이다.


					// 이전에 누구와도 충돌이 되지 않은 상태라면 그냥 주고.
					if (!m_pMousePrevCollision)
						m_pMousePrevCollision = pDest;

					else if (m_pMousePrevCollision != pDest)
					{
						// 이전에 누군가와 충돌이 이루어지고 있는 경우라면 이전에 충돌했던 충돌체와의
						// 관계를 없던 것으로 만들고 새로운 충돌체를 넣어준다.
						// 이때 Release함수도 호출해준다.

						// 두 충돌체를 서로 충돌상태에서 해제한다.
						DeletePrevCollisionAdj(pSrc->GetSerialNumber(),
							m_pMousePrevCollision->GetSerialNumber());

						pSrc->DeletePrevCollisionList(m_pMousePrevCollision);
						m_pMousePrevCollision->DeletePrevCollisionList(pSrc);

						pSrc->Call(CS_RELEASE, m_pMousePrevCollision, fTime);
						m_pMousePrevCollision->Call(CS_RELEASE, pSrc, fTime);

						m_pMousePrevCollision = pDest;
					}

					// 충돌이 일어난 경우에서 이전에 충돌이 난 상태와 그렇지 않은 상태를 구분지어서
					// 현재 충돌이 된 상태로 충돌체를 넣어주었으니...
					// 이제 충돌상태를 등록해준다.

					// 두 충돌체를 서로 충돌상태로 등록한다.
					AddPrevCollisionAdj(pSrc->GetSerialNumber(),
						pDest->GetSerialNumber());

					pSrc->AddPrevCollisionList(pDest);
					pDest->AddPrevCollisionList(pSrc);

					pSrc->Call(CS_BLOCK, pDest, fTime);
					pDest->Call(CS_BLOCK, pSrc, fTime);
				}

				else // 충돌이 되었고, 이전에도 충돌이라면..
				{
					pSrc->Call(CS_OVERLAP, pDest, fTime);
					pDest->Call(CS_OVERLAP, pSrc, fTime);
				}

				return;

				// 누군가와 충돌이 된 경우에는 이후에 충돌처리를 하지 않는다.
			}
		}
	}

	// 아무런 물체와도 충돌이 일어나지 않은 경우.
	// 그런데, 이전에 충돌한 물체가 있는 경우.
	// 충돌상태에서 떨어트리는 처리는 새로운 물체와 충돌이 일어난 경우에도 해주고 있지만,
	// 이것은 다른 물체와의 충돌이 일어난 경우에 들어오는 경우이다.
	// 따라서, 다른 충돌체와 충돌이 되지 않은 경우에도 기존에 충돌이 되었던 물체가 있다면 처리를 해주어야 한다.
	// Release 함수 호출해주고, 충돌상태 해제해주는 처리가 필요하다(왜냐면 아무랑도 충돌이 안 된거니까)

	if (m_pMousePrevCollision)
	{
		// 두 충돌체를 서로 충돌상태에서 해제한다.
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

	// 스테이지를 가져온다.

	if (pStage && pStage->IsTile())
	{
		// stage && 타일 사용.

		// 타일과 오브젝트들을 충돌시킬 것이다.
		// 모든 오브젝트를 순회하면서 타일과 충돌처리를 한다.
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


			// 타일 충돌을 하지 않는 객체인지 확인하여 맞으면 넘긴다.
			if (!(*iter)->GetTileCollisionEnable())
			{
				++iter;
				continue;
			}

		// 타일 충돌을 하지 않는 물체는 들어오지 않음.

			// 죽어 있는 경우 없애고 땡기고 다음 오브젝트
			// 죽지 않았는데 활성화가 꺼진 경우 넘기고.

			// 죽지 않았고, 활성화인 경우. 충돌처리..

			RectInfo tTileRC;
			Position tPos, tPrevPos;
			tPos = (*iter)->GetPos();
			tPrevPos = (*iter)->GetPrevPos();

			 // 현재위치, 이전위치를 가져온다.

			Position tMove = tPos - tPrevPos;
			// 이동량을 구해온다.

			Position tPivot = (*iter)->GetPivot();
			// 오브젝트의 Rect를 그려내기 위해서 피봇값을 가져온다.

			Size tSize = (*iter)->GetStaticSize();
			// 변하지 않는 Size를 받아온다.
			// m_tSize 의 경우에는 애니메이션의 크기에 따라서 자동으로 바뀌기 때문에

	


			// 방향성을 구해낸다.  왼쪽. 0. 오른쪽.
			int iDir = 0;

			
			if (tMove.x < 0)
				iDir = -1;

			else if (tMove.x > 0)
				iDir = 1;

			if ((*iter)->GetLineCollision())
			{
				iDir = 0;
			}


			// 이곳으로 들어오는 물체가 Line과 충돌이 된 경우에는 X축에 대해서 처리하지 않는다.
			// 동시에 충돌이 나는 경우에 앞으로 나아갈 수 있어야 하는데, 밖에서 파고든 경우로 인식해서 들아가질 못한다.
			
			
			float fResult = 0.f;


			// X이동 처리.

			if (pStage->CheckWallW(fResult, tTileRC, *iter, iDir))
			{
				// Stage의 CheckWallW  함수를 호출하여 왼쪽 이동 / 오른쪽 이동을 처리한다.

				// 이동 방향이 왼쪽으로 이동할때 타일의 오른쪽 면이 
				// 이동 범위 안에 존재하는지 판단하여 존재할 경우에만 밀어준다.
				// -> ?

				if (iDir == -1)
				{

					// 왼쪽으로 이동을 하고 있기 때문에 왼쪽 x좌표를 구한 것이다
					
					Position tPrev = tPrevPos - tPivot * tSize;
					// 이전 위치에서 왼쪽 상단을 구한다.
					Position tNext = tPos - tPivot * tSize;
					// 현재 위치에서 왼쪽 상단을 구한다.

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
							// 다른 물체

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
									// 좌우 예외처리.. Line에도
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

					// 이전 프레임에서의 사각형과 현재 프레임의 사각형에서 왼쪽 상단의  x위치를 비교한다.
					// 이렇게 두 x 좌표를 그어서, 타일의 밖에서 안쪽으로 들어오게 된 경우에만 
					// 다시 밀려나가도록 처리하고 있다.

					// 만약에 중심부쪽에서 왼쪽으로 이동하면서 점프를 했다면.
					// 조건문에서 배제가 되어서 처리하지 않는다.
					// 즉, X축이 아닌 Y축에 대한 처리를 하도록 하는 것이다.
				}

				else if (iDir == 1)
				{
					// 오른쪽 : 오른쪽 상단의 x 좌표를 비교.
					Position tPrev = tPrevPos + (Position(1.f, 1.f) - tPivot) * tSize;
					Position tNext = tPos + (Position(1.f, 1.f) - tPivot) * tSize;
					
					// Pivot의 반대를 이용해서 오른쪽 하단을 구한다. (x좌표만 이용함)

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
							// 다른 물체
							switch ((*iter)->GetObjectType())
							{
							case OT_PLAYER_ATTACK:

								if ((*iter)->GetTag() == "VomitStar")
								{
									((CEffect*)(*iter))->SetDieTime(0.2f, true);

									GET_SINGLE(CSoundManager)->Play("AttackStarHit", CT_EFFECT);
									break;
									// 좌우 예외처리.. Line에도
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

					// 똑같이 타일의 밖에서 파고드는 경우에만 밖으로 튕겨나가도록 구성한다(x)
				}
			}


			// Y 이동 처리

			iDir = 0;

			if (tMove.y < 0)
				iDir = -1;

			else if (tMove.y > 0)
				iDir = 1;

		
			if ((*iter)->GetLineCollision())
			{
				// Line충돌이면서 MiddleCollision이 true라는 소리는 
				// Y축에 대한 처리를 하지 말아야 하는 상황이다.

				if((*iter)->GetMiddleCollision())
					iDir = 0;
				else
				{
					// 반대로 라인 충돌이면서 MiddleCollision이 false라면 이 경우에는 타일충돌 y의 처리가 필요하다.
					// 바꾸지 않는다.
				}
			}

			// 이전에 충돌이 일어났던 경우라면 아래로 내려가는 것으로 인식.
			else if ((*iter)->GetPrevLineCollision())
			{
				iDir = 1;
			}


		
			if (pStage->CheckWallH(fResult, tTileRC, *iter, iDir))
			{
				//* 함수를 호출한 이후에 충돌한 Tile의 위치정보를 가지고서 나오게 된다.
				// 따라서 우리가 이전 직선을 그어가면서 외부에서 들어갔는지를 판단할 수 있는 것이다.
				if (iDir == -1)
				{

					// 위로 올라가는 경우.


					// 이전 프레임의 좌표와 현재 프레임의 좌표를 이용하는데 Top의 좌표를 가지고 판단한다.

					Position tPrev = tPrevPos - tPivot * tSize;
					Position tNext = tPos - tPivot * tSize;

					// 왼쪽 상단의 좌표를 구해서 y좌표만 이용한다.

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
					// 아래로 내려가는 경우.

					// 아래를 확인한다.

					Position tPrev = tPrevPos + (Position(1.f, 1.f) - tPivot) * tSize;
					Position tNext = tPos + (Position(1.f, 1.f) - tPivot) * tSize;

					// 오른쪽 하단의 좌표를 가져온다.

					if ((tPrev.y < tTileRC.t && tNext.y > tTileRC.t) || (*iter)->GetPrevLineCollision())
					{
						// 타일과 직선의 충돌에서, 직선따로 타일따로 충돌처리를 해주고 있는데,
						// 직선과 바로 이전 프레임에서 직선과 충돌이 일어난 경우에는 
						// 그냥 올리는 것으로 한다.
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
									// 점프를 하는 중이며,
									if (pDDD->JumpDown())
										pDDD->JumpDisEnable();

									pDDD->SetFalling(false);
									// 올라가는 상태가 아니라, 떨어지는 상태에서 점프를 꺼버린다.
								}
							}
								break;
							}
						}

						(*iter)->GravityTimeDisEnable();
						(*iter)->KnockUpEndDisEnable();
					



						// 아래로 내려가서 내려간 만큼을 올려주는 상황에서 중력시간을 초기화한다.

					}
				}
			}

			// 이렇게 가로처리와 세로 처리를 따로 하고 다음 오브젝트를 처리한다.

			++iter;


			/*
			좌우의 이동을 검사하는 경우. x의 좌표를 고정시켜둔 상태에서 오브젝트의 y 위치를 가지고
			타일의 인덱스를 구해내고,

			상하의 이동을 검사하는 경우, y의 좌표를 고정시켜둔 상태에서 오브젝트의 x 위치를 가지고 
			타일의 인덱스를 구해내고 있다.


			case 1 : 왼쪽에서 타일의 중간으로 돌진. 오브젝트의 오른쪽 Line을 기준으로 하여 사각형을 파고드는 까닭에
			들어간 만큼 일단 빠져나온다. 그리고 Y를 고정하여 확인하는 순간 해당 부분의 타일이 겹치지 않아서 
			따로 처리하지 않고 끝난다.

			case 2 : 타일의 중간에서 오른쪽을 향하며 점프 오른쪽 Line을 가지고 확인하였으나 사각형 내부에서 놀기 때문에
			따로 처리를 하지 않고 Y축에 대해서는 파고들어서 그 만큼 빠져나오게 된다.
			
			case 3 : 오른쪽을 향하며 타일의 왼쪽으로 돌진. x에 대하여 파고들어서 빠져나온다. Y는 체크 X

			case 4 : 오른쪽을 향하며 타일의 왼쪽으로 돌진. 정확한 대각선. x 빠져나오고 끝.

			case 5 : 위에서 오른쪽을 향하며 타일의 왼쪽에 충돌. 오브젝트의 오른쪽이 충돌이 된 상황에서 이미
			타일의 왼쪽 안으로 들어가있는 상황. X에 대해서 처리하지 않게 된다.
			Y만 처리해서 위로.

			case 6 : 타일의 위에서 떨어지는 경우 x는 상관하지 않는다.
			(왼쪽을 향하든 오른쪽을 향하든 충돌이 난 시점에서 이미 타일의 내부에 존재함. = X축 처리 x)
			이 상황에서 Y축에 대해서만 처리가 된다. 위로 .
			*/
		}
	}

}


