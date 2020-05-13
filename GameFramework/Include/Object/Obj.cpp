#include "Obj.h"
#include "../Texture.h"
#include "../Collision/Collider.h"
#include "../Animation/Animation.h"
#include "../Core/ResourceManager.h"
#include "../Core/SceneManager.h"
#include "../Core/EditManager.h"
#include "../Core/GameManager.h"
#include "../Core/SceneManager.h"
#include "../Collision/CollisionManager.h"
#include "../Object/Effect.h"
#include "../Collision/ColliderRect.h"
#include "../Object/Player.h"

CSerialNumber CObj::m_SerialNumber;

CObj::CObj() :
	m_pTexture(nullptr),
	m_iSerialNumber(0),
	m_bIsUI(false),
	m_pScene(nullptr),
	m_pAnimation(nullptr),
	m_fGravityTime(0.f),
	m_bAngle(false),
	m_fAngle(0.f),
	m_bPrototype(false),
	m_bDieEffect(false)
{
	m_bStageCollision = false;
	m_bLineCollision = false;
	m_bPrevLineCollision = false;

	m_eObjectType = OT_NONE;


	// 세 개의 설정값
	m_bLineCollisionEnable = false;
	m_bTileCollisionEnable = true;
	m_bGravity = false;

	m_fPlayLimitTime = 0.f;
	m_fPlayTime = 0.f;
	m_bPlayTime = false;
	m_bDie = false;

	// Blend
	m_bBlend = false;
	m_fBlendTime = 0.f;
	m_fBlendLimitTime = 0.f;
	m_iBlend = 0;

	// 넉백.


	m_bKnockBack = false;
	m_bKnockUp = false;

	m_fKnockBackTime = 0.f;
	m_fKnockUpTime = 0.f;

	m_fKnockBackLimitTime = 0.f;
	m_fKnockUpLimitTime = 0.f;
	m_fKnockUpEndLimitTime = 0.f;


	m_fKnockBackPower = 0.f;
	m_fKnockUpPower = 0.f;

	m_fKnockBackPowerTime = 0.f;
	m_fKnockUpPowerTime = 0.f;

	m_eCollisionDir = SD_NONE;

	m_bKnockUpEnd = false;
	m_fKnockUpTime = 0.f;


	m_bInputDisEnable = false;
	m_fInputDisEnableTime = 0.f;
	m_fInputDisEnableLimitTime = 0.f;
}

CObj::CObj(const CObj & obj)
{
	*this = obj;

	// 모든 변수에 대하여 얇은 복사를 수행한다.


	//unsigned int		m_iSerialNumber; 복사를 한다는 의미는 프로토타입을 의미한다.
	// 따라서, SerialNumber값이 0일 것이다.

	if (m_pTexture)
	{
		m_pTexture->AddRef();
		// 새로운 객체가 해당 포인터를 가지게 된다. 참조횟수 증가
	}



	// 충돌체 리스트

	m_ColliderList.clear();

	list<CCollider*>::const_iterator iter;
	list<CCollider*>::const_iterator iterEnd = obj.m_ColliderList.end();

	for (iter = obj.m_ColliderList.begin(); iter != iterEnd; ++iter)
	{
		CCollider* pCollider = (*iter)->Clone();

		m_ColliderList.push_back(pCollider);

		pCollider->m_pObj = this;
		pCollider->SetScene(m_pScene); // 현재 장면을 선택해준다.
	}

	// 모든 충돌체에 대해서 복사해준다.
	// 이후에 이 충돌체는 Obj의 Start함수에 의해서 충돌관리자에 등록이 될 것이다.

	if (obj.m_pAnimation)
	{
		m_pAnimation = obj.m_pAnimation->Clone();
		m_pAnimation->SetObj(this);
	}

}


CObj::~CObj()
{
	// 자신이 가지고 있던 충돌체를 날려준다.


	list<CCollider*>::iterator iter;
	list<CCollider*>::iterator iterEnd = m_ColliderList.end();

	for (iter = m_ColliderList.begin(); iter != iterEnd; ++iter)
	{
		(*iter)->SetActive(false);
	}

	// 사라지는 충돌체에 대해서 처리하면 안 되니.. 죽게 만든다.

	// 구체적으로는 충돌 관리자에서 장면의 모든 충돌체를 가지고 있게 되는데
	// 이렇게 사라지는 오브젝트의 충돌체 또한 당연히 충돌처리를 검사하게 된다. (오브젝트와는 상관없이 모든 충돌체를 들고 있는 개념이니까)
	// 따라서, Active값을 false로 만들어서 이들이 살아 있더라도, 처리가 안 되게 막는다.

	Safe_Release_VecList(m_ColliderList); // 여기서 없앤다. 참조횟수 감소.

	// 충돌체는 사라지면서 자신과 충돌한 충돌체에 접근하여 자신의 정보를 없애주는 처리를 하고 있고, 해당 정보를 인접행렬에서도 없애주는 처리를 
	// 해주고 있다. 마지막으로 Release상태에 해당하는 함수를 호출하면서 충돌체는 사라진다.

	m_ColliderList.clear();

	// 오브젝트가 사라지는 경우에 SerialNumber를 반환해준다.

	m_SerialNumber.ReturnSerialNumber(m_iSerialNumber);


	// 사용하던 텍스처와 Animation을 날려준다.
     	SAFE_RELEASE(m_pTexture);
	// 그렇다면, 미리 하나 지정하고 있던 애니메이션의 텍스처는 다른 텍스처보다 하나만큼 더 감소되어 개수를 맞춰주고,

		SAFE_RELEASE(m_pAnimation);
	// 여기서 오브젝트의 애니메이션이 날라가고 AnimationClip정보를 정리하면서 각 텍스처의 참조횟수를 줄인다.

	// ResourceManager 1 애니메이션 Clip의 텍스처로 셋팅 2 
	// 대부분의 텍스처 2 2 2 2 2 현재 돌아가고 있는 텍스처의 Ref = 3
	// Scene을 생성하는 과정이라면, ref 값을 하나 더 증가시키는데, 이 경우에는 장면 생성 이후에 ClearScene함수 호출을 통해서
	// 다시 감소시키는 부분이니 생각하지 않아도 된다.

	// 그렇다면, 현재 돌리고 있던 텍스처의 참조값을 줄여서 222222
	// 여기서 애니메이션 날리면서 111111
	// 마지막으로 ResourceManager에서 해당 리소스를 정리 00000 delete

	if (m_pScene)
		m_pScene->DeleteObject(this);
	// ObjList에서 해당 오브젝트의 정보를 없애준다.
	// 참조값은 상관없고, 단순히 오브젝트의 검색을 위해서 가지고 있는 List이다.
	
}



void CObj::SetInputDisEnable(float fDisEnableTime)
{
	m_bInputDisEnable = true;
	m_fInputDisEnableTime = 0.f;
	m_fInputDisEnableLimitTime = fDisEnableTime;
}

void CObj::SetKnockBack(float fPlayTime, float fPower, SIDE_DIR eCollisionDir)
{


	m_bKnockBack = true;
	m_fKnockBackTime = 0.f;
	m_fKnockBackLimitTime = fPlayTime;
	m_fKnockBackPower = fPower;
	m_fKnockBackPowerTime = 0.f;
	m_eCollisionDir = eCollisionDir;

	/*switch (m_eObjectType)
	{
	case OT_PLAYER:
		break;
	}*/
}

void CObj::SetKnockUp(float fPlayTime, float fPower, float fKnockUpEndTime)
{

	m_bKnockUp = true;
	m_fKnockUpTime = 0.f;
	m_fKnockUpLimitTime = fPlayTime;
	m_fKnockUpPower = fPower;
	m_fKnockUpPowerTime = 0.f;
	m_bKnockUpEnd = false;
	m_fKnockUpEndTime = 0.f;
	m_fKnockUpEndTime = fKnockUpEndTime;
}

void CObj::SetBlend(float fIntervalTime, int iLimitCount, 
	unsigned char iBlendMin, unsigned char iBlendMax)
{
		if (m_bBlend)
			return;

		m_bBlend = true;
		m_fBlendTime = 0.f;
		m_iBlendCount = 0;
		m_fBlendLimitTime = fIntervalTime / 2.f;
		m_iBlendLimitCount = iLimitCount * 2;
		m_iBlend = iBlendMin;
		m_iBlendMin = iBlendMin;
		m_iBlendMax = iBlendMax;

}

void CObj::StarEffectCreate(Position tStartPos, float fPlayTime,
	float fMoveSpeed, const string& strLayer)
{
	// 사방으로 이동하는 StartEffect를 생성한다.

	// 60도 6개.

	CScene* pScene = GET_SINGLE(CSceneManager)->GetCurrentScene();

	float fAngle = 0.f;

	for (int i = 0; i < 6; ++i)
	{
		CEffect* pEffect = CObj::CreateCloneObject<CEffect>(SMALL_STAR, SMALL_STAR, strLayer, pScene);

		pEffect->SetAngle(fAngle);

		pEffect->SetPos(tStartPos);

		pEffect->SetPlayTime(fPlayTime);

		pEffect->SetMoveSpeed(fMoveSpeed);


		fAngle += PI / 3.f;
	}
}

void CObj::UpdatePos(const Position & tPos)
{
	m_tPos = tPos;

	// 넣어준 좌표로 물체를 이동시키고,

	list<CCollider*>::iterator iter;
	list<CCollider*>::iterator iterEnd = m_ColliderList.end();

	for (iter = m_ColliderList.begin(); iter != iterEnd; ++iter)
	{
		(*iter)->UpdateObjPos(m_tPos);
		// 해당 오브젝트가 가지고 있는 충돌체에 대하여 모두 Update처리를 해준다.
	}

	// 물체를 이동시키고, 충돌체도 이동시키고.
}

void CObj::SetTexture(const string & strName, const TCHAR * pFileName, const string & strPathName)
{
	SAFE_RELEASE(m_pTexture);

	if (pFileName)
		GET_SINGLE(CResourceManager)->LoadTexture(strName, pFileName, strPathName);

	m_pTexture = GET_SINGLE(CResourceManager)->FindTexture(strName);
}

void CObj::SetTexture(CTexture * pTexture)
{
	// 새로 텍스처를 적용한다는 의미는 기존에 쓰던 것을 참조해제하고, 새로운 것을 받아들인다는 의미.
	SAFE_RELEASE(m_pTexture);
	
	m_pTexture = pTexture; 

	if(pTexture)
		pTexture->AddRef();

	// ChangeClip으로 예시를 들어보면, Start부분에서 기본 애니메이션에서 텍스처를 받아서 셋팅하면서 참조횟수를 증가시켰다.

	// 따라서, 다음 ChangeClip으로 인해 새로운 텍스처를 셋팅해야 한다면, 이렇게 증가시킨 값을 하나 줄이고서
	// 새로운 텍스처를 받고 새로운 텍스처의 참조횟수를 증가시켜야 한다.
}

void CObj::SetColorKey(unsigned char r, unsigned char g, unsigned char b)
{
	if (!m_pTexture)
		return;

	m_pTexture->SetColorKey(r, g, b);
}

CObj * CObj::FindObject(const string & strName)
{
	return m_pScene->FindObject(strName);
	// 누군가의 참조. ref ++
}

void CObj::CreateAnimation(const string & strName)
{
	// 그럴리는 없겠지만. 
	SAFE_RELEASE(m_pAnimation);

	// 애니메이션을 생성해서 strName의 태그로 설정해주는 함수이다.

	m_pAnimation = new CAnimation;

	m_pAnimation->m_pObj = this;

	m_pAnimation->SetTag(strName);

}



void CObj::Start()
{
	// Obj가 Layer에 배치(즉, 월드에 배치되는 과정에서 호출하는 함수이다)

	list<CCollider*>::const_iterator iter;
	list<CCollider*>::const_iterator iterEnd = m_ColliderList.end();

	for (iter = m_ColliderList.begin(); iter != iterEnd; ++iter)
	{
		(*iter)->Start(); 
		// 자신에게 붙어있는 Collider의 Start함수를 호출해준다.
		// 충돌체에서는 SerialNumber를 할당받고 있으며,
		// 자신을 CollisionManager에 등록해주고 있다.

	}

	GET_SINGLE(CCollisionManager)->AddWorldObjList(this);

	if (m_pAnimation)
	{
		// 애니메이션이 추가가 되어 있는 경우에는...
		// 텍스처를 셋팅할 것이므로 기존에 있었다면 release한번 해주고.
		SAFE_RELEASE(m_pTexture);

		/*
		애니메이션 클래스는 여러개의 클립 정보를 가지고 있을 수 있는데,
		여기서 클립 정보에는 각 애니메이션을 돌리기 위한 정보와 Texture* 의 정보를 포함하고 있다.
		즉, Animation의 여러개의 Texture를 가지고 있을 수 있다.
		하나의 클립은 아틀라스, 프레임 상관없이 여러개의 DC를 가지고 있을 수 있는 텍스처를 하나 가지고 있는다.
		
		아무튼 애니메이션을 돌려줘야 하기 때문에 기본값으로 설정된 클립이 들고 있는 텍스처 정보를
		Obj의 텍스처 정보로 셋팅하는 것이다.

		*/

		PAnimationClip pClip = m_pAnimation->GetClip();

		if (!pClip)
			return;
		// 애니메이션은 있지만 클립은 없는 경우 return.

			m_pTexture = pClip->pTexture;

			if(m_pTexture)
				m_pTexture->AddRef();
		// 또한 이렇게 텍스처를 주는 것은 참조값이 증가하는 것이다.

		m_pAnimation->m_pObj = this;

		if(!pClip->vecCoord.empty())
			m_tSize = pClip->vecCoord[0].tEnd - pClip->vecCoord[0].tStart;

		// 현재 Texture클래스는 Render를 하는 과정에서
		// Obj의 Size를 받아서 처리해주고 있다.
		// 즉, Obj의 Size는 출력하는 Size가 된다.
		// 따라서, 이미지의 크기에 맞는 Size를 설정해주어야 이미지가 제대로 나올 수 있다.
		// Obj의  Size는 이미지의 출력전용이고, 충돌에 이용하는 충돌체는 다른 Size를 가지고 있다.

		// 클립이 가지고 있는 Texture정보에 대해서 생각해보자.
		// 일단, 오브젝트에서 애니메이션을 추가한다.
		// 그리고 해당 애니메이션에 Clip을 생성하고
		// 해당 클립을 찾아와서 Texture를 생성해주는데,

		// 여기서 인자값에 따라서 Texture는 여러개의 DC를 가질 수도 있고,
		// 하나의  DC를 가지게 될 수도 있다.
		
		// 아무튼 이렇게 읽어오는 과정에서 Texture가 생성되어 ResourceManager에서 해당 정보를 가지고
		// 있어서 ref = 1이다.
		// 이 상태에서 클립에서 해당 텍스처를 설정해준다. ref = 2
		// 이렇게 2인 상태에서 Obj가 배치되는 과정에서 Start함수가 호출이 되고,
		// 자연스럽게 현재 클립에 대한 텍스처가 Obj의 정보로 셋팅이 된다.
		// 여기서 참조가 일어나서 자연스럽게 ref = 3이 된다.
		// 만약에 여기서 참조가 일어나지 않는 다른 클립이라면 ref = 2이다.
		
		
	}

	else if (m_pTexture)
	{
		// 애니메이션의 경우에는 현재 클립의 텍스처를 오브젝트가 잡도록 하고, 여기서 참조횟수를 증가시켜 두었다. 
		// 동시에 출력하는 사이즈를 설정해주고 있다. 

		// 문제는 단순히 텍스처만 있는 경우에는 출력사이즈를 설정하고 있지 않다는 것이다.
		// 참조 횟수랑은 상관이 없으며, 우리는 출력 사이즈만 생각해주면 된다.

		if (m_tSize.x == 0.f)
		{
			// 출력 사이즈가 기존에 설정이 되어 있지 않은 경우는 이미지의 크기대로 출력하도록 한다.
			m_tSize = Size(m_pTexture->GetBitmapInfo().bmWidth, m_pTexture->GetBitmapInfo().bmHeight);
		}
	}
}

bool CObj::Init()
{
	return true;
}

void CObj::Update(float fTime)
{

	if (m_bDie)
	{
		m_fPlayTime += fTime;

		if (m_fPlayTime >= m_fPlayLimitTime)
		{
			m_bActive = false;

			if(m_bDieEffect)
				CObj::StarEffectCreate(m_tPos, 1.f, 100.f);
		}
	}


	// InputDisEnableTime
	if (m_bInputDisEnable)
	{
		m_fInputDisEnableTime += fTime;

		if (m_fInputDisEnableTime >= m_fInputDisEnableLimitTime)
			m_bInputDisEnable = false;
	}

	/*

		m_bKnockBack; // Is or Not
		m_bKnockUp;

		m_fKnockBackTime; // 시간 중첩용.
		m_fKnockUpTime;

		m_fKnockBackLimitTime; // 끄는 시간 체크
		m_fKnockUpLimitTime;


		m_fKnockBackPower; // Power값.
		m_fKnockUpPower;

		m_fKnockBackPowerTime; // 파워 시간 중첩용.
		m_fKnockUpPowerTime;
*/

	// Knock Back
	if (m_bKnockBack)
	{
		m_fKnockBackTime += fTime;
		m_fKnockBackPowerTime += fTime;
		


		switch (m_eCollisionDir)
		{
		case SD_LEFT:
			Move(Position(-1.f, 0.f), m_fKnockBackPower, fTime);
			break;

		case SD_RIGHT:
			Move(Position(1.f, 0.f), m_fKnockBackPower, fTime);

			break;
		}

		if (m_fKnockBackTime >= m_fKnockBackLimitTime)
		{
			m_bKnockBack = false;
	

		}

		if (m_fKnockBackPowerTime >= 0.1f)
		{
			m_fKnockBackPower -= 5.f;


			if (m_fKnockBackPower < 0.f)
			{
				m_fKnockBackPower = 0.f;
			}
		}


	}

	// Knock Up
	if (m_bKnockUp)
	{
		
		m_fKnockUpTime += fTime;
		m_fKnockUpPowerTime += fTime;

		Move(Position(0.f, -1.f), m_fKnockUpPower, fTime);


		if (m_fKnockUpTime >= m_fKnockUpLimitTime)
		{
			m_bKnockUp = false;
			m_bKnockUpEnd = true;
			m_fKnockUpEndTime = 0.f;
		}

		if (m_fKnockUpPowerTime >= 0.1f)
		{
			m_fKnockUpPowerTime -= 0.1f;
			m_fKnockUpPower -= 20.f;

			if (m_fKnockUpPower < 0.f)
			{
				m_bKnockUp = false;
				m_bKnockUpEnd = true;
				m_fKnockUpEndTime = 0.f;

			}
		}

	
	}

	else if (m_bKnockUpEnd)
	{

		Move(Position(0.f, -1.f), 100.f, fTime);

		m_fKnockUpEndTime += fTime;

		if (m_fKnockUpEndTime >= m_fKnockUpEndLimitTime)
			m_bKnockUpEnd = false;


		// 넉업이 끝나고 1초간은 천천히 떨어지도록 한다.

	}

	
	

	// Blend Update
	m_fBlendTime += fTime;

	if (m_fBlendTime >= m_fBlendLimitTime)
	{
		m_fBlendTime -= m_fBlendLimitTime;
		++m_iBlendCount;


		if (m_iBlendCount == m_iBlendLimitCount)
		{
			m_bBlend = false;


			//list<CCollider*>::iterator iter;
			//list<CCollider*>::iterator iterEnd = m_ColliderList.end();

			//for (iter = m_ColliderList.begin(); iter != iterEnd; ++iter)
			//{
			//	// 모든 충돌체를 순회하면서 이전에 충돌했던 물체가 있는 경우에는 충돌 관계를 없던 것으로 처리한다.
			//	(*iter)->PrevCollisionListClear();
			//}
			
		}


		if (m_iBlend == m_iBlendMin)
		{
			m_iBlend = m_iBlendMax;
		}
		else
			m_iBlend = m_iBlendMin;

	
	
	}

	// 중력 처리. 바닥과 충돌이 아닌 경우에만 중력의 처리를 한다.
	if (m_bGravity && !m_bLineCollision && !m_bStageCollision && !m_bKnockBack && !m_bKnockUp)
	{
		// 상수값 하나. 10.f

			
		// 들어온 시간이. 0.05f보다 작은 경우(즉, 정상적인 프레임인 경우에만 중력에 대한 처리를 
		// 하도록 한다. 
		// 갑자기 들어오는 상황을 막는 처리이다.
		if (fTime < 0.05f)
		{

			if (m_fGravityTime != 0.f)
			{
				//m_tPos.y += 0.5f * m_fGravityTime; // *m_fGravityTime;

				Move(Position(0.f, 1.f), 730.f + m_fGravityTime * 800.f, fTime);

				// 미세한 증가.
			}

			m_fGravityTime += fTime;

			if (m_fGravityTime > 1.f)
				m_fGravityTime = 1.f;

		}

		// 중력이 켜져있는 경우.
		// y축의 양의 방향으로 초당 1000.f 의 속도로 이동.

	}


	if (m_pAnimation)
		m_pAnimation->Update(fTime);

	list<CCollider*>::iterator iter;
	list<CCollider*>::iterator iterEnd = m_ColliderList.end();

	for (iter = m_ColliderList.begin(); iter != iterEnd; )
	{
		// 자신의 모든 충돌체에 대하여 Update함수를 호출해준다.
		// 근데 여기서 활성화, 비활성화의 개념과 사망 개념을 도입한다.
		// 객체가 살아있는데 해당 객체의 충돌체가 사라지는 경우가 있을 까 ?

		// 충분히 있을 가능성이 있다.
		// 내가 만들려고 하는 커비의 경우에도 어떤 상태가 되냐에 따라서 능력이 달라지는데
		// 이러면 모션 자체가 달라지게 된다. 이런 과정에서 충돌체의 차이가 생기게 되고
		// 기존에 가지고 있던 충돌체를 날려줘야 하는 상황이 생길 가능성이 충분히 있다.

		if (!(*iter)->IsActive())
		{
			// 해당 객체가 살아있지 않은 경우에는...

			// 충돌체를 추가하면서 참조과정이 일어났으므로

			SAFE_RELEASE((*iter));
			iter = m_ColliderList.erase(iter);
			iterEnd = m_ColliderList.end();
			continue;
		}

		//else if (!(*iter)->IsEnable())
		//{
		//	// 살아는 있지만 활성화가 되어 있지 않은 경우.
		//	// 그냥 넘겨준다.
		//	++iter;
		//	continue;
		//}

		// 활성화까지 되어있는 경우에는 Update처리를 해준다.
		(*iter)->Update(fTime);
		++iter;
	}


	// Angle Move
	if (m_bAngle)
	{
		switch (m_eObjectType)
		{
			case OT_PLAYER_ATTACK:
			case OT_MONSTER_ATTACK:
			{
				CEffect* pEffect = (CEffect*)this;

				if (!pEffect->GetDie())
					MoveAngle(fTime);

			}
			break;

			default:
				MoveAngle(fTime);

		}

		// 기본적으로 MoveAngle을 이용하며. Effect오브젝트의 경우에는 사라지고 있는 경우에 처리하지 않는다.
	}							 	
								 	
								 	
}								 	

void CObj::LateUpdate(float fTime)
{
	list<CCollider*>::iterator iter;
	list<CCollider*>::iterator iterEnd = m_ColliderList.end();

	for (iter = m_ColliderList.begin(); iter != iterEnd; )
	{
		if (!(*iter)->IsActive())
		{
			SAFE_RELEASE((*iter));
			iter = m_ColliderList.erase(iter);
			iterEnd = m_ColliderList.end();
			continue;
		}

		//else if (!(*iter)->IsEnable())
		//{
		//	++iter;
		//	continue;
		//}

		(*iter)->LateUpdate(fTime);
		++iter;
	}
	
	m_bPrevLineCollision = m_bLineCollision;
	m_bStageCollision = false;
	m_bLineCollision = false;
	m_bMiddleCollision = false;

}

void CObj::Collision(float fTime)
{

}

void CObj::PrevRender(HDC hDC, float fTime)
{

	// Update를 통해서 물체가 이동한 이후에.
	// 움직인 양을 구하고.
	// 이전 위치를 갱신.
	m_tMove = m_tPos - m_tPrevPos;

	m_tPrevPos = m_tPos;

	list<CCollider*>::iterator iter;
	list<CCollider*>::iterator iterEnd = m_ColliderList.end();

	for (iter = m_ColliderList.begin(); iter != iterEnd; )
	{
		if (!(*iter)->IsActive())
		{
			SAFE_RELEASE((*iter));
			iter = m_ColliderList.erase(iter);
			iterEnd = m_ColliderList.end();
			continue;
		}

		/*else if (!(*iter)->IsEnable())
		{
			++iter;
			continue;
		}*/

		(*iter)->PrevRender(hDC, fTime);
		++iter;
	}
}

void CObj::Render(HDC hDC, float fTime)
{

	//  오브젝트 출력.


	Position tWorldPos = m_tPos;

	if (m_tStaticSize.x == 0.f)
	{
		tWorldPos -= m_tPivot * m_tSize;
		
		// 아무것도 설정이 되어 있지 않은 경우에는 출력 사이즈를 기준으로 피봇을 잡는다.

	}
	else
		tWorldPos -= m_tPivot * m_tStaticSize;

	Position tCameraPos;

	if (!m_bIsUI)
	{
		tCameraPos = GET_SINGLE(CSceneManager)->GetCameraPos();
		tWorldPos -= tCameraPos;
	}

	// UI가 아니면, 카메라의 영향을 받도록 한다.
	// UI면 영향을 받지 않는다.


	// 0, 0 이라면, 아무런 처리도 하지 않은 체 그대로.
	// 0.5f, 0.5f라면, 우리가 설정한 좌표는 물체의 중심을 의미하는 것이다.
	// 따라서, 윈도우 출력의 특성상 왼쪽 상단을 출력위치로 잡아주어야 한다.

	// m_tSize * m_tPivot 을 통해서, 왼쪽 상단으로 옮겨야할 상대적인 위치를 잡는다.
	// 그리고 그것을 WorldPos에 적용시키면 Pivot을 설정한대로 출력이 이루어 지는 것이다.

	if (m_pTexture)
	{
		Position tImagePos;

		int iFrame = 0;


		if (m_pAnimation)
		{
			PAnimationClip pClip = m_pAnimation->GetClip();

			tImagePos = pClip->vecCoord[pClip->iFrame].tStart;

			if (pClip->eType == AT_FRAME)
				iFrame = pClip->iFrame;

			// 오프셋을 지정한다.

			tWorldPos += pClip->tOffset;
		}



		if (!m_bBlend)
		{
			m_pTexture->Render(hDC, tWorldPos, tImagePos, m_tSize, iFrame);
		}
		else
		{
			// AlphaBlend

			if (m_pTexture->m_bColorKey)
			{
				
				HDC m_Alpha = CreateCompatibleDC(hDC);
				HBITMAP m_AlphaBit = CreateCompatibleBitmap(hDC, (int)m_tSize.x, (int)m_tSize.y);
				HBITMAP m_OldAlphaBit = (HBITMAP)SelectObject(m_Alpha, m_AlphaBit);


				BitBlt(m_Alpha, 0, 0, (int)m_tSize.x, (int)m_tSize.y,
					hDC, (int)tWorldPos.x, (int)tWorldPos.y, SRCCOPY);
				// 배경 복사.

				TransparentBlt(m_Alpha, 0, 0, (int)m_tSize.x, (int)m_tSize.y,
					m_pTexture->GetDC(), (int)tImagePos.x, (int)tImagePos.y,
					(int)m_tSize.x, (int)m_tSize.y, m_pTexture->m_iColorKey);

				BLENDFUNCTION bf;
				bf.BlendOp = 0;
				bf.BlendFlags = 0;
				bf.SourceConstantAlpha = m_iBlend;
				bf.AlphaFormat = 0;

				// 배경 색깔을 뺸 것을, 투명하게 하여 backDC에 넘김
				AlphaBlend(hDC, (int)tWorldPos.x, (int)tWorldPos.y, (int)m_tSize.x, (int)m_tSize.y,
					m_Alpha, 0, 0, (int)m_tSize.x, (int)m_tSize.y, bf);

				// 이전에 쓰던 것을 장착
				SelectObject(m_Alpha, m_OldAlphaBit);
				DeleteObject(m_AlphaBit);
				DeleteDC(m_Alpha);

			}

			else
			{

				BLENDFUNCTION bf;
				bf.BlendOp = 0;
				bf.BlendFlags = 0;
				bf.SourceConstantAlpha = m_iBlend;
				bf.AlphaFormat = 0;

				AlphaBlend(hDC, (int)tWorldPos.x, (int)tWorldPos.y, (int)m_tSize.x, (int)m_tSize.y,
					m_pTexture->GetDC(), 0, 0, (int)m_tSize.x, (int)m_tSize.y, bf);
			}
		}

	}



	ColliderRender(hDC, fTime);



	//  아래 주석.

	//#ifdef _DEBUG
//
//		// 오브젝트의 출력 크기를 표현한다.
//		// 오브젝트는 텍스처에 의해서 출력이 되는데 현재 위치기준으로 Size만큼 출력을 하고 있다.
//
//		HBRUSH hBrush = GREENBRUSH;
//
//		RECT tRC = { tWorldPos.x, tWorldPos.y, tWorldPos.x + m_tSize.x, tWorldPos.y + m_tSize.y };
//		FrameRect(hDC, &tRC, hBrush);
//
//#endif

		// 현재 출력을 보면, 처음에 Start를 하는 과정이나, ChangeClip함수를 통해서 애니메이션의 클립을 변경해주는 부분이나.
		// 출력하려고 하는 DC의 크기(즉, vector에 있는 Coord정보의 크기)를 하나하나 설정하고 있지는 않다.
		// 상식적으로 생각해서 애니메이션의 프레임당 크기가 가변적으로 바뀐다.
		// 그러면 그거 애니메이션 잘못 만든거라고 할 수 있다.
		// 실제로 그런 부분은 고려하지 않고 있고 고려할 필요도 없다. 애니메이션을 돌리는데 매 프레임마다 사이즈가 바뀐다.
		// 아무튼 그런 애니메이션은 없다.
		// 따라서, 현재 설정하는 정보를 보면, 애니메이션의 클립을 설정하는 순간에서 0번째 프레임의 Size를 기준으로하여 오브젝트의
		// 출력 사이즈로 설정하는데, 이것이 가능한 이유는 모든 프레임이 같은 크기의 출력을 보장한다고 가정하기 때문이다.
		// 그래서 매 Frame마다 Size를 다시 데입받는 그런 처리가 없는 것이다.
		// 단지, 매 프레임당  SrcPos의 위치는 다시 받아주고 있다. (이건 당연히 받아야 하는 정보니까)
		// 그렇게 출력해야하는 SrcPos의 위치를 알면, 우리는 동일한 크기를 가지는 프레임 정보를 가지고 
		// 출력해야하는 다음 부분을 알아낼 수 있는 것이다.

		// 결론 : 출력은, 클립을 변경하는 과정에서(start, changeclip) 정해진 출력크기(Size)와 Src의 Pos에 따라서 변경이 된다.
		// 이 정보를 애니메이션의 출력위치를 바꾸거나, 출력크기를 바꾸는 것에 적용하여 본다면...

		// 일단, 각각 따로 처리하니까 하나씩 생각해보자.
		// 출력 위치를 변경하는 부분은... Size가 바뀌는가 ? No No
		// 단지 바뀌는 것은 출력하는 영역일 뿐이다.
		// 그렇다면 출력 영역의 정보인 Size값에 손을 댈 필요는 없다. ( 사이즈 변경이 아니니까)
		// 돌리고 있는 클립의 vector정보에 접근하여 출력위치를 변경한다면 ...
		// 출력해야하는 FrameSizeX와 FrameSizeY의 값을 알고 있는 경우에 LeftTop의 좌표만 알고 있으면
		// 출력하는데에는 문제가 없다.
		// 결론 : Size는 변경할 필요가 없다. 단순히 vector에서 Start의 값만 변경하면 된다.
		// 근데 Start값만 바꿔버리면, 나중에 ChangeClip을 통해서 해당 애니메이션 클립으로 변경이 일어나는 경우에
		// 정확한 사이즈를 잡지 못할 가능성이 크다. ex : 0 0 50 50 이었는데
		// -1 -1 50 50 으로 바뀐다면, 나중에 이 클립을 잡아서 출력위치를 잡아줄때의 크기는 ...
		// 51 51 로 잡히게 된다. ( Size를 바꾸려고 한 것이 아닌데, Size가 바뀌어버림.. 이는 오류)
		// 아무튼 나중에 클립정보를 바꿀 것을 생각해서 end의 좌표까지 모두 변경한다.

		// 출력 좌표의 변경. --> 현재 돌리고 있는 클립의 vector정보에 접근하여 좌표를 다르게 설정한다.
		// (프레임을 땡기는 개념이라고 볼 수 있다)


		// 출력 크기의 변경은, Size당연히 바뀌어야 하고, 출력 영역이 바뀐다는 것은 당연히 Begin과 End의 정보도 변하는 것이다.
		// 왜냐면, Begin과 End는 단순히 좌표만을 의미하는 것이 아닌 Size의 개념도 포함되어 있기 때문이다.
		// 출력 크기의 변경. --> 현재 돌리고 있는 클립의 vector정보에 접근하여 좌표를 다르게 설정하고,
		// 이것을 Size에 직접 적용시킨다.
		// 자연스럽게 자연스럽게 자연스럽게하려면 ...
		// size를 증가. lefttop이 변경되지 않았다면, 단순히 right bottom쪽에서 값이 증가.

		// 아니 근데, 이것을 굳이 이렇게 복잡하게 생각할 필요도 없이 그냥,
		// 늘린 좌표를, 그대로 오브젝트에게 size로 넘겨주고 오브젝트는 그대로 출력하도록 하면 되는거 아닌가 ?
		// ex : size를 end쪽에서 1증가. size 에 바로 적용됨.  ㅇㅇㅇ
		//		size를 begin쪽에서 1증가. (-1) size바로 적용.  
		// 결론. end의 좌표만, 변경해주자. 그리고, size에 직접적용을 해주는 걸로.

	//  우리는 출력하는 방식이 Texture를 통한 간접적인 호출을 해주고 있다.
// Texture의 Render에서는 출력좌표와 이미지에서의 위치, 출력 Size 를 받아주고 있는데, 여기서 어떤 애니메이션의 텍스처를 출력하느냐에 따라서
// 출력하는 크기가 당연히 변한다.
// 따라서, 출력을 할 때 마다, 현재 클립을 얻어와서, 현재 돌려야 하는 프레임에서의 좌표를 받아온다.
// 애니매이션의 클립을 바꾸는 과정, 혹은 처음 받아서 사용할 때는 출력해야하는 프레임의 크기로 Size를 설정해주고 있다.
// 즉, 여기서  Size 는 출력을 위한 크기이다.
// tWorldPos(출력위치)에 출력하고, tImagePos에서 m_tSize만큼(애니메이션의 출력 크기) 출력한다.
// iFrame에 해당하는 DC 를 사용하여, 프레임 애니메이션이라면 DC의 순서를 따져주고, 아틀라스 애니메이션이라면 
// 상관없이 0을 준다.


	// 충돌체의 상태를 확인하고 없애주는 처리를 포함하여 Render를 해주고 있는데,
	// 이는 Debug 모드에서만 보인다.

//
//#ifdef _DEBUG
//
//
//	if (m_pAnimation)
//	{
//		// 출력중인 애니메이션의 이름을 보여준다. 오브젝트의 좌표에서 y축으로 150 증가 시킨 위치에
//		
//		PAnimationClip pClip = m_pAnimation->GetClip();
//		const string& s = pClip->strName;
//
//		int len;
//		int slength = (int)s.length() + 1;
//		len = MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, 0, 0);
//		wchar_t* buf = new wchar_t[len];
//		MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, buf, len);
//		wstring r(buf);
//		delete[] buf;
//		
//
//		TextOut(hDC, m_tPos.x + 50.f, m_tPos.y + 230.f, r.c_str(), r.size());
//
//
//		// 애니메이션을 셋팅하는 과정에서 2가지 정보가 필요하다.
//		// 일단, SRC에서의 시작 좌표와, 프레임의 사이즈 정보.
//		// 이것을 출력해주는 부분을 만든다.
//
//
//
//		vector<ClipCoord>& vecCoord = pClip->vecCoord;
//
//		// 출력중인 애니메이션의 Src 좌표를 출력한다.
//		TCHAR strPos[32] = {};
//
//		wsprintf(strPos, TEXT("SrcPos : (%d, %d)"), (int)vecCoord[0].tStart.x, (int)vecCoord[0].tStart.y);
//		
//		TextOut(hDC, m_tPos.x + 50.f, m_tPos.y + 250.f, strPos, lstrlen(strPos));
//
//
//
//		// 출력 중인 애니메이션의 프레임 사이즈를 출력한다.
//
//		wsprintf(strPos, TEXT("FrameSize : (%d, %d)"), (int)m_tSize.x, (int)m_tSize.y);
//
//		TextOut(hDC, m_tPos.x + 50.f, m_tPos.y + 270.f, strPos, lstrlen(strPos));
//
//		// 현재 프레임을 출력해준다
//
//		wsprintf(strPos, TEXT("Frame : %d / %d"), pClip->iFrame, vecCoord.size());
//
//		TextOut(hDC, m_tPos.x + 50.f, m_tPos.y + 290.f, strPos, lstrlen(strPos));
//
//
//
//		// 애니매이션의 한 프레임 타임을 출력해준다.
//
//		swprintf_s(strPos, 32, TEXT("Frame PlayTime : %.2f / %.2f"), pClip->fTime, pClip->fFrameTime);
//
//		TextOut(hDC, m_tPos.x + 50.f, m_tPos.y + 310.f, strPos, lstrlen(strPos));
//
//
//		// 전체 플레이 시간을 출력한다.
//
//		swprintf_s(strPos, 32, TEXT("PlayTime : %.2f"), pClip->fPlayTime);
//
//		TextOut(hDC, m_tPos.x + 50.f, m_tPos.y + 330.f, strPos, lstrlen(strPos));
//	}
//
//
//#endif
}

void CObj::PostRender(HDC hDC, float fTime)
{

	list<CCollider*>::iterator iter;
	list<CCollider*>::iterator iterEnd = m_ColliderList.end();

	for (iter = m_ColliderList.begin(); iter != iterEnd; )
	{
		if (!(*iter)->IsActive())
		{
			SAFE_RELEASE((*iter));
			iter = m_ColliderList.erase(iter);
			iterEnd = m_ColliderList.end();
			continue;
		}

	/*	else if (!(*iter)->IsEnable())
		{
			++iter;
			continue;
		}*/

		

		(*iter)->PostRender(hDC, fTime);
		++iter;
	}

	

}

void CObj::EditRender(HDC hDC, float fTime, bool bOffset, Position tOffset)
{
	// 어짜피 편집을 한다는 의미는 아틀라스라는 의미이다.
	// src의 좌표를 map이라고 생각하고 출력하는 개념이다.
	// 중간에 크기를 벗어나는 경우가 생길텐데 이 부분은 카메라의 개념으로 처리해야 한다.
	// 카메라의 좌표에 해당하는 값을 Scene마다 따로 두도록 한다.
	// UI쪽의 Render는 다르게 구현할 예정. 어쨋든 오브젝트의 경우에는 gameManager의 카메라의 좌표에 영향을 받아야 한다.

	// 현재 여기에는 하나의 오브젝트에 대한 정보만 출력하게 될 것이다. 왜냐면, 마우스는 따로 Update가 돌아가고 있기에...

	// 우리는 src에서의 카메라 위치로부터 RS_WIDTH, RS_HEIGHT 만큼 출력을 해야 한다.



	Position tImagePos = GET_SINGLE(CSceneManager)->GetCameraPos();
	// 움직인 카메라 좌표를 이용하여 src의 좌표를 옮겨버린다ㅏ.

	if (m_pTexture)
	{
		// 출력위치는 0,0 부터 RS_WIDTH, RS_HEIGHT
		BitBlt(hDC, 0, 0, RS_WIDTH, RS_HEIGHT, m_pTexture->m_vecDC[0],
			(int)tImagePos.x, (int)tImagePos.y, SRCCOPY);
	}

	Position tPrintOffset = GET_SINGLE(CEditManager)->GetPrintOffset();

	// SIZE는 애니메이션의 프레임 크기이다.
	Position tRenderPos((int)(tPrintOffset.x + RS_WIDTH - m_tSize.x - 100.f), (int)tPrintOffset.y +  30);

	// 오른쪽 상단에 애니메이션의 출력 부분을 감싸서 보여준다.
	RECT tRC = {  (int)tRenderPos.x, (int)tRenderPos.y,
		(int)(tRenderPos.x + m_tSize.x),  (int)(tRenderPos.y + m_tSize.y) };

	

	Rectangle(hDC, tRC.left, tRC.top, tRC.right, tRC.bottom);
	// 애니메이션의 부분에 덮어지지 않도록, 나중에 출력한다.
	// 이로서 컬러키값이 정해져있지 않더라도, 간격을 알아볼 수 있다.

	{
		// 현재 처리하고 있는 오브젝트의 태그를 출력한다.
		int len;
		int slength = (int)m_strTag.length() + 1;
		len = MultiByteToWideChar(CP_ACP, 0, m_strTag.c_str(), slength, 0, 0);
		wchar_t* buf = new wchar_t[len];
		MultiByteToWideChar(CP_ACP, 0, m_strTag.c_str(), slength, buf, len);
		wstring r(buf);
		delete[] buf;

		TextOut(hDC, tRC.left, tRC.top - 20, r.c_str(), r.size());
	}
	
	if (m_pAnimation)
	{
		PAnimationClip pClip = m_pAnimation->GetClip();
		const string &s = pClip->strName;

		int len;
		int slength = (int)s.length() + 1;
		len = MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, 0, 0);
		wchar_t* buf = new wchar_t[len];
		MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, buf, len);
		wstring r(buf);
		delete[] buf;

	


		int iBottom = tRC.bottom;

		// 맨 바닥의 출력위치가 화면을 벗어나는 경우에는 여유값을 줘서 100 + 60 만큼 올라간 자리에 출력을 시작하도록 한다.
		if (tRC.bottom + 130 > RS_HEIGHT)
			iBottom = RS_HEIGHT - 160;
	
		// 출력위치가 맨 바닥을 넘어가지 않는 ㄱ여우에는 그냥 해당 위치에 출력을 하는 것으로 한다.

		// 애니메이션의 이름을 보여준다.
		TextOut(hDC, (int)tRenderPos.x, iBottom + 20, r.c_str(), r.size());


		vector<ClipCoord>& vecCoord = pClip->vecCoord;
		vector<ClipCoord>& vecEditCoord = GET_SINGLE(CEditManager)->GetClipCoord();

		vecCoord = vecEditCoord;
		// Edit의 Coord정보를 얻어와서, 매 프레임마다 이것을 적용시킨다.
		// 여기서 EditCoord 정보의 갱신을 받게 된다. (출력 좌표 & 프레임 사이즈)

	

		// 출력 중인 애니메이션의 프레임 사이즈를 출력한다.
		TCHAR strPos[32] = {};

		wsprintf(strPos, TEXT("FrameSize : (%d, %d)"), (int)m_tSize.x, (int)m_tSize.y);

		TextOut(hDC, (int)tRenderPos.x, iBottom + 40, strPos, lstrlen(strPos));

		// 현재 프레임을 출력해준다

		wsprintf(strPos, TEXT("Frame : %d / %d"), pClip->iFrame, vecCoord.size());

		TextOut(hDC, (int)tRenderPos.x, iBottom + 60, strPos, lstrlen(strPos));



		// 애니매이션의 한 프레임 타임을 출력해준다.

		swprintf_s(strPos, 32, TEXT("Frame PlayTime : %.2f / %.2f"), pClip->fTime, pClip->fFrameTime);

		TextOut(hDC, (int)tRenderPos.x, iBottom + 80, strPos, lstrlen(strPos));


		// 전체 플레이 시간을 출력한다.

		swprintf_s(strPos, 32, TEXT("PlayTime : %.2f"), pClip->fPlayTime);

		TextOut(hDC, (int)tRenderPos.x, iBottom + 100, strPos, lstrlen(strPos));

		if (vecCoord.empty())
			return;


		Position tImage;

		// src에서의 위치를 가져온다.
		if ((size_t)pClip->iFrame < vecCoord.size())
			tImagePos = vecCoord[pClip->iFrame].tStart;

		// 애니메이션을 출력한다.
		m_pTexture->Render(hDC, tRC.left, tRC.top,
			(int)tImagePos.x, (int)tImagePos.y, (int)m_tSize.x, (int)m_tSize.y);

	}

	if (bOffset)
	{

		HBRUSH hBrush;

		switch (GET_SINGLE(CEditManager)->GetBrushType())
		{
		case BT_BLACK:
			hBrush = BLACKBRUSH;
			break;

		case BT_RED:
			hBrush = REDBRUSH;
			break;

		case BT_YELLOW:
			hBrush = YELLOWBRUSH;
			break;

		case BT_WHITE:
			hBrush = WHITEBRUSH;
			break;

		case BT_MAGENTA:
			hBrush = MAGENTABRUSH;
			break;

		case BT_GREEN:
			hBrush = GREENBRUSH;
			break;
		}
		// 원래는 애니메이션의 위치를 옮기는 것이 맞지만, 해당 툴에서 보여주는 상황에서는 Static을 옮기는 것이 맞다.
		// 오브젝트의 pivot을 기준으로 생각하는 것이 좋을 것 같다.

		// StaticSize를 구한다. 여기서 offset을 지정해주는 것이다.
		RECT tStaticRC = { (int)(tRenderPos.x + tOffset.x), (int)(tRenderPos.y + tOffset.y),
			(int)(tRenderPos.x + m_tStaticSize.x + tOffset.x), (int)(tRenderPos.y + m_tStaticSize.y + tOffset.y) };

		FrameRect(hDC, &tStaticRC, hBrush);
	}



}


CObj * CObj::Clone() const
{
	return nullptr;
}

void CObj::ColliderRender(HDC hDC, float fTime)
{
	list<CCollider*>::iterator iter;
	list<CCollider*>::iterator iterEnd = m_ColliderList.end();

	for (iter = m_ColliderList.begin(); iter != iterEnd; )
	{
		if (!(*iter)->IsActive())
		{
			SAFE_RELEASE((*iter));
			iter = m_ColliderList.erase(iter);
			iterEnd = m_ColliderList.end();
			continue;
		}

		/*else if (!(*iter)->IsEnable())
		{
			++iter;
			continue;
		}*/

		// 일단 충돌체가 활성화되지 않은 경우라고 해도 출력.

		(*iter)->Render(hDC, fTime);
		++iter;
	}
}

void CObj::ChangeDefaultClip()
{
	m_pAnimation->ChangeClip(m_pAnimation->GetDefaultClip());
}

void CObj::Move(const Position & tDir)
{
	m_tPos += tDir * m_fMoveSpeed;
	// 예를 들어 방향이 x = 1 y = 1이면 
	//  x축과 y축이 모두 양의 방향으로 움직이며 m_fMoveSpeed 만큼 즉시적으로 움직인다.

}

void CObj::Move(const Position & tDir, float fTime)
{
	// 위와 동일한 함수인데, 시간의 개념을 추가했다.
	m_tPos += tDir * m_fMoveSpeed * fTime;
	// tDir이 가리키는 방향으로 m_fMoveSpeed 만큼 이동하는데, 여기에 시간을 곱하여 움직임을 제어한다.
}


void CObj::Move(DIR eDir)
{
	switch (eDir)
	{
	case LEFT:
		m_tPos.x -= m_fMoveSpeed;
		break;
	case RIGHT:
		m_tPos.x += m_fMoveSpeed;

		break;
	case UP:
		m_tPos.y -= m_fMoveSpeed;

		break;
	case DOWN:
		m_tPos.y += m_fMoveSpeed;

		break;
	}
}

void CObj::Move(DIR eDir, float fTime)
{
	switch (eDir)
	{
	case LEFT:
		m_tPos.x -= m_fMoveSpeed * fTime;
		break;
	case RIGHT:
		m_tPos.x += m_fMoveSpeed * fTime;

		break;
	case UP:
		m_tPos.y -= m_fMoveSpeed * fTime;

		break;
	case DOWN:
		m_tPos.y += m_fMoveSpeed * fTime;

		break;
	}
}
void CObj::MoveAngle()
{
	m_tPos.x += cosf(m_fAngle) * m_fMoveSpeed;
	m_tPos.y += sinf(m_fAngle) * m_fMoveSpeed;
	// 각도가 가리키는 방향으로 스피드만큼 이동.
}
void CObj::MoveAngle(float fTime)
{
	m_tPos.x += cosf(m_fAngle) * m_fMoveSpeed * fTime;
	m_tPos.y += sinf(m_fAngle) * m_fMoveSpeed * fTime;

	// 시간을 기반으로 이동.
}

void CObj::MoveAngle(float fAngle, float fSpeed, float fTime)
{
	m_tPos.x += cosf(fAngle) * fSpeed * fTime;
	m_tPos.y += sinf(fAngle) * fSpeed * fTime;
}

void CObj::Move(const Position & tDir, float fSpeed, float fTime)
{
	m_tPos += tDir * fSpeed * fTime;

	// ex : Dir ( 1.f, 0.f) * fSpeed * fTime
	// 이라고 하면, x 축방향으로만 시간에 따른 이동량만큼 이동
	// -1.f 등을 이용해서 음의 방향으로 움직이게 하는 것도 가능.
}

vector<ClipCoord>& CObj::GetvecCoord() const
{
	return m_pAnimation->GetClip()->vecCoord;
}

void CObj::LoadClip(const char * pFileName)
{
	if (m_pAnimation)
		m_pAnimation->LoadClip(pFileName);
}

void CObj::SetColliderStateAll(bool bEnable)
{

	list<CCollider*>::iterator iter;
	list<CCollider*>::iterator iterEnd = m_ColliderList.end();


	for (iter = m_ColliderList.begin(); iter != iterEnd; ++iter)
	{
		(*iter)->DisEnable();
	}
}

void CObj::AddPrevSceneCollider()
{
	list<CCollider*>::iterator iter;
	list<CCollider*>::iterator iterEnd = m_ColliderList.end();


	for (iter = m_ColliderList.begin(); iter != iterEnd; ++iter)
	{
		GET_SINGLE(CCollisionManager)->AddCollider(*iter);
	}
}

void CObj::AddBodyCollision(Size tStaticSize, const string & strColliderTag,
	const string & strProfile, const string & strChannel)
{
	CColliderRect* pCollider = AddCollider<CColliderRect>(strColliderTag);

	m_tStaticSize = tStaticSize;

	Position tLeftTop = tStaticSize / -2.f;
	Position tRightBottom = tStaticSize / 2.f;

	pCollider->SetRelativeInfo(tLeftTop.x, tLeftTop.y, tRightBottom.x, tRightBottom.y);

	pCollider->SetProfile(strProfile);
	pCollider->SetChannel(strChannel);


	SAFE_RELEASE(pCollider);
}

CCollider * CObj::FindCollider(const string & strName)
{
	list<CCollider*>::iterator iter;
	list<CCollider*>::iterator iterEnd = m_ColliderList.end();

	for (iter = m_ColliderList.begin(); iter != iterEnd; ++iter)
	{
		if ((*iter)->GetTag() == strName)
		{
			(*iter)->AddRef();
			return *iter;
		}
			
	}

	return nullptr;

	// 자신의 충돌체리스트에서 strName의 Tag를 가진것을 찾아서 반환.
	// 여기서 포인터를 반환하므로 Ref값을 하나 증가.
}

CCollider * CObj::FindCollider()
{
	if (m_ColliderList.size() == 1)
	{
		return *m_ColliderList.begin();
	}

	// 하나가 있는 경우 이것을 반환.
	// 하나도 없는 경우 nullptr.
	return nullptr;
}

void  CObj::SetColliderState(const string & strName, bool bEnable)
{
	// 해당 이름을 가진 Collider를 찾아와서 상태를 설정해주는 함수이다.
	CCollider* pCollider = FindCollider(strName);
	pCollider->SetEnable(bEnable);
	SAFE_RELEASE(pCollider);
}

void CObj::SetColliderState(bool bState)
{
	CCollider* pCollider = FindCollider();
	pCollider->SetEnable(bState);
	SAFE_RELEASE(pCollider);
}
