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


	// �� ���� ������
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

	// �˹�.


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

	// ��� ������ ���Ͽ� ���� ���縦 �����Ѵ�.


	//unsigned int		m_iSerialNumber; ���縦 �Ѵٴ� �ǹ̴� ������Ÿ���� �ǹ��Ѵ�.
	// ����, SerialNumber���� 0�� ���̴�.

	if (m_pTexture)
	{
		m_pTexture->AddRef();
		// ���ο� ��ü�� �ش� �����͸� ������ �ȴ�. ����Ƚ�� ����
	}



	// �浹ü ����Ʈ

	m_ColliderList.clear();

	list<CCollider*>::const_iterator iter;
	list<CCollider*>::const_iterator iterEnd = obj.m_ColliderList.end();

	for (iter = obj.m_ColliderList.begin(); iter != iterEnd; ++iter)
	{
		CCollider* pCollider = (*iter)->Clone();

		m_ColliderList.push_back(pCollider);

		pCollider->m_pObj = this;
		pCollider->SetScene(m_pScene); // ���� ����� �������ش�.
	}

	// ��� �浹ü�� ���ؼ� �������ش�.
	// ���Ŀ� �� �浹ü�� Obj�� Start�Լ��� ���ؼ� �浹�����ڿ� ����� �� ���̴�.

	if (obj.m_pAnimation)
	{
		m_pAnimation = obj.m_pAnimation->Clone();
		m_pAnimation->SetObj(this);
	}

}


CObj::~CObj()
{
	// �ڽ��� ������ �ִ� �浹ü�� �����ش�.


	list<CCollider*>::iterator iter;
	list<CCollider*>::iterator iterEnd = m_ColliderList.end();

	for (iter = m_ColliderList.begin(); iter != iterEnd; ++iter)
	{
		(*iter)->SetActive(false);
	}

	// ������� �浹ü�� ���ؼ� ó���ϸ� �� �Ǵ�.. �װ� �����.

	// ��ü�����δ� �浹 �����ڿ��� ����� ��� �浹ü�� ������ �ְ� �Ǵµ�
	// �̷��� ������� ������Ʈ�� �浹ü ���� �翬�� �浹ó���� �˻��ϰ� �ȴ�. (������Ʈ�ʹ� ������� ��� �浹ü�� ��� �ִ� �����̴ϱ�)
	// ����, Active���� false�� ���� �̵��� ��� �ִ���, ó���� �� �ǰ� ���´�.

	Safe_Release_VecList(m_ColliderList); // ���⼭ ���ش�. ����Ƚ�� ����.

	// �浹ü�� ������鼭 �ڽŰ� �浹�� �浹ü�� �����Ͽ� �ڽ��� ������ �����ִ� ó���� �ϰ� �ְ�, �ش� ������ ������Ŀ����� �����ִ� ó���� 
	// ���ְ� �ִ�. ���������� Release���¿� �ش��ϴ� �Լ��� ȣ���ϸ鼭 �浹ü�� �������.

	m_ColliderList.clear();

	// ������Ʈ�� ������� ��쿡 SerialNumber�� ��ȯ���ش�.

	m_SerialNumber.ReturnSerialNumber(m_iSerialNumber);


	// ����ϴ� �ؽ�ó�� Animation�� �����ش�.
     	SAFE_RELEASE(m_pTexture);
	// �׷��ٸ�, �̸� �ϳ� �����ϰ� �ִ� �ִϸ��̼��� �ؽ�ó�� �ٸ� �ؽ�ó���� �ϳ���ŭ �� ���ҵǾ� ������ �����ְ�,

		SAFE_RELEASE(m_pAnimation);
	// ���⼭ ������Ʈ�� �ִϸ��̼��� ���󰡰� AnimationClip������ �����ϸ鼭 �� �ؽ�ó�� ����Ƚ���� ���δ�.

	// ResourceManager 1 �ִϸ��̼� Clip�� �ؽ�ó�� ���� 2 
	// ��κ��� �ؽ�ó 2 2 2 2 2 ���� ���ư��� �ִ� �ؽ�ó�� Ref = 3
	// Scene�� �����ϴ� �����̶��, ref ���� �ϳ� �� ������Ű�µ�, �� ��쿡�� ��� ���� ���Ŀ� ClearScene�Լ� ȣ���� ���ؼ�
	// �ٽ� ���ҽ�Ű�� �κ��̴� �������� �ʾƵ� �ȴ�.

	// �׷��ٸ�, ���� ������ �ִ� �ؽ�ó�� �������� �ٿ��� 222222
	// ���⼭ �ִϸ��̼� �����鼭 111111
	// ���������� ResourceManager���� �ش� ���ҽ��� ���� 00000 delete

	if (m_pScene)
		m_pScene->DeleteObject(this);
	// ObjList���� �ش� ������Ʈ�� ������ �����ش�.
	// �������� �������, �ܼ��� ������Ʈ�� �˻��� ���ؼ� ������ �ִ� List�̴�.
	
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
	// ������� �̵��ϴ� StartEffect�� �����Ѵ�.

	// 60�� 6��.

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

	// �־��� ��ǥ�� ��ü�� �̵���Ű��,

	list<CCollider*>::iterator iter;
	list<CCollider*>::iterator iterEnd = m_ColliderList.end();

	for (iter = m_ColliderList.begin(); iter != iterEnd; ++iter)
	{
		(*iter)->UpdateObjPos(m_tPos);
		// �ش� ������Ʈ�� ������ �ִ� �浹ü�� ���Ͽ� ��� Updateó���� ���ش�.
	}

	// ��ü�� �̵���Ű��, �浹ü�� �̵���Ű��.
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
	// ���� �ؽ�ó�� �����Ѵٴ� �ǹ̴� ������ ���� ���� ���������ϰ�, ���ο� ���� �޾Ƶ��δٴ� �ǹ�.
	SAFE_RELEASE(m_pTexture);
	
	m_pTexture = pTexture; 

	if(pTexture)
		pTexture->AddRef();

	// ChangeClip���� ���ø� ����, Start�κп��� �⺻ �ִϸ��̼ǿ��� �ؽ�ó�� �޾Ƽ� �����ϸ鼭 ����Ƚ���� �������״�.

	// ����, ���� ChangeClip���� ���� ���ο� �ؽ�ó�� �����ؾ� �Ѵٸ�, �̷��� ������Ų ���� �ϳ� ���̰�
	// ���ο� �ؽ�ó�� �ް� ���ο� �ؽ�ó�� ����Ƚ���� �������Ѿ� �Ѵ�.
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
	// �������� ����. ref ++
}

void CObj::CreateAnimation(const string & strName)
{
	// �׷����� ��������. 
	SAFE_RELEASE(m_pAnimation);

	// �ִϸ��̼��� �����ؼ� strName�� �±׷� �������ִ� �Լ��̴�.

	m_pAnimation = new CAnimation;

	m_pAnimation->m_pObj = this;

	m_pAnimation->SetTag(strName);

}



void CObj::Start()
{
	// Obj�� Layer�� ��ġ(��, ���忡 ��ġ�Ǵ� �������� ȣ���ϴ� �Լ��̴�)

	list<CCollider*>::const_iterator iter;
	list<CCollider*>::const_iterator iterEnd = m_ColliderList.end();

	for (iter = m_ColliderList.begin(); iter != iterEnd; ++iter)
	{
		(*iter)->Start(); 
		// �ڽſ��� �پ��ִ� Collider�� Start�Լ��� ȣ�����ش�.
		// �浹ü������ SerialNumber�� �Ҵ�ް� ������,
		// �ڽ��� CollisionManager�� ������ְ� �ִ�.

	}

	GET_SINGLE(CCollisionManager)->AddWorldObjList(this);

	if (m_pAnimation)
	{
		// �ִϸ��̼��� �߰��� �Ǿ� �ִ� ��쿡��...
		// �ؽ�ó�� ������ ���̹Ƿ� ������ �־��ٸ� release�ѹ� ���ְ�.
		SAFE_RELEASE(m_pTexture);

		/*
		�ִϸ��̼� Ŭ������ �������� Ŭ�� ������ ������ ���� �� �ִµ�,
		���⼭ Ŭ�� �������� �� �ִϸ��̼��� ������ ���� ������ Texture* �� ������ �����ϰ� �ִ�.
		��, Animation�� �������� Texture�� ������ ���� �� �ִ�.
		�ϳ��� Ŭ���� ��Ʋ��, ������ ������� �������� DC�� ������ ���� �� �ִ� �ؽ�ó�� �ϳ� ������ �ִ´�.
		
		�ƹ�ư �ִϸ��̼��� ������� �ϱ� ������ �⺻������ ������ Ŭ���� ��� �ִ� �ؽ�ó ������
		Obj�� �ؽ�ó ������ �����ϴ� ���̴�.

		*/

		PAnimationClip pClip = m_pAnimation->GetClip();

		if (!pClip)
			return;
		// �ִϸ��̼��� ������ Ŭ���� ���� ��� return.

			m_pTexture = pClip->pTexture;

			if(m_pTexture)
				m_pTexture->AddRef();
		// ���� �̷��� �ؽ�ó�� �ִ� ���� �������� �����ϴ� ���̴�.

		m_pAnimation->m_pObj = this;

		if(!pClip->vecCoord.empty())
			m_tSize = pClip->vecCoord[0].tEnd - pClip->vecCoord[0].tStart;

		// ���� TextureŬ������ Render�� �ϴ� ��������
		// Obj�� Size�� �޾Ƽ� ó�����ְ� �ִ�.
		// ��, Obj�� Size�� ����ϴ� Size�� �ȴ�.
		// ����, �̹����� ũ�⿡ �´� Size�� �������־�� �̹����� ����� ���� �� �ִ�.
		// Obj��  Size�� �̹����� ��������̰�, �浹�� �̿��ϴ� �浹ü�� �ٸ� Size�� ������ �ִ�.

		// Ŭ���� ������ �ִ� Texture������ ���ؼ� �����غ���.
		// �ϴ�, ������Ʈ���� �ִϸ��̼��� �߰��Ѵ�.
		// �׸��� �ش� �ִϸ��̼ǿ� Clip�� �����ϰ�
		// �ش� Ŭ���� ã�ƿͼ� Texture�� �������ִµ�,

		// ���⼭ ���ڰ��� ���� Texture�� �������� DC�� ���� ���� �ְ�,
		// �ϳ���  DC�� ������ �� ���� �ִ�.
		
		// �ƹ�ư �̷��� �о���� �������� Texture�� �����Ǿ� ResourceManager���� �ش� ������ ������
		// �־ ref = 1�̴�.
		// �� ���¿��� Ŭ������ �ش� �ؽ�ó�� �������ش�. ref = 2
		// �̷��� 2�� ���¿��� Obj�� ��ġ�Ǵ� �������� Start�Լ��� ȣ���� �ǰ�,
		// �ڿ������� ���� Ŭ���� ���� �ؽ�ó�� Obj�� ������ ������ �ȴ�.
		// ���⼭ ������ �Ͼ�� �ڿ������� ref = 3�� �ȴ�.
		// ���࿡ ���⼭ ������ �Ͼ�� �ʴ� �ٸ� Ŭ���̶�� ref = 2�̴�.
		
		
	}

	else if (m_pTexture)
	{
		// �ִϸ��̼��� ��쿡�� ���� Ŭ���� �ؽ�ó�� ������Ʈ�� �⵵�� �ϰ�, ���⼭ ����Ƚ���� �������� �ξ���. 
		// ���ÿ� ����ϴ� ����� �������ְ� �ִ�. 

		// ������ �ܼ��� �ؽ�ó�� �ִ� ��쿡�� ��»���� �����ϰ� ���� �ʴٴ� ���̴�.
		// ���� Ƚ������ ����� ������, �츮�� ��� ����� �������ָ� �ȴ�.

		if (m_tSize.x == 0.f)
		{
			// ��� ����� ������ ������ �Ǿ� ���� ���� ���� �̹����� ũ���� ����ϵ��� �Ѵ�.
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

		m_fKnockBackTime; // �ð� ��ø��.
		m_fKnockUpTime;

		m_fKnockBackLimitTime; // ���� �ð� üũ
		m_fKnockUpLimitTime;


		m_fKnockBackPower; // Power��.
		m_fKnockUpPower;

		m_fKnockBackPowerTime; // �Ŀ� �ð� ��ø��.
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


		// �˾��� ������ 1�ʰ��� õõ�� ���������� �Ѵ�.

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
			//	// ��� �浹ü�� ��ȸ�ϸ鼭 ������ �浹�ߴ� ��ü�� �ִ� ��쿡�� �浹 ���踦 ���� ������ ó���Ѵ�.
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

	// �߷� ó��. �ٴڰ� �浹�� �ƴ� ��쿡�� �߷��� ó���� �Ѵ�.
	if (m_bGravity && !m_bLineCollision && !m_bStageCollision && !m_bKnockBack && !m_bKnockUp)
	{
		// ����� �ϳ�. 10.f

			
		// ���� �ð���. 0.05f���� ���� ���(��, �������� �������� ��쿡�� �߷¿� ���� ó���� 
		// �ϵ��� �Ѵ�. 
		// ���ڱ� ������ ��Ȳ�� ���� ó���̴�.
		if (fTime < 0.05f)
		{

			if (m_fGravityTime != 0.f)
			{
				//m_tPos.y += 0.5f * m_fGravityTime; // *m_fGravityTime;

				Move(Position(0.f, 1.f), 730.f + m_fGravityTime * 800.f, fTime);

				// �̼��� ����.
			}

			m_fGravityTime += fTime;

			if (m_fGravityTime > 1.f)
				m_fGravityTime = 1.f;

		}

		// �߷��� �����ִ� ���.
		// y���� ���� �������� �ʴ� 1000.f �� �ӵ��� �̵�.

	}


	if (m_pAnimation)
		m_pAnimation->Update(fTime);

	list<CCollider*>::iterator iter;
	list<CCollider*>::iterator iterEnd = m_ColliderList.end();

	for (iter = m_ColliderList.begin(); iter != iterEnd; )
	{
		// �ڽ��� ��� �浹ü�� ���Ͽ� Update�Լ��� ȣ�����ش�.
		// �ٵ� ���⼭ Ȱ��ȭ, ��Ȱ��ȭ�� ����� ��� ������ �����Ѵ�.
		// ��ü�� ����ִµ� �ش� ��ü�� �浹ü�� ������� ��찡 ���� �� ?

		// ����� ���� ���ɼ��� �ִ�.
		// ���� ������� �ϴ� Ŀ���� ��쿡�� � ���°� �ǳĿ� ���� �ɷ��� �޶����µ�
		// �̷��� ��� ��ü�� �޶����� �ȴ�. �̷� �������� �浹ü�� ���̰� ����� �ǰ�
		// ������ ������ �ִ� �浹ü�� ������� �ϴ� ��Ȳ�� ���� ���ɼ��� ����� �ִ�.

		if (!(*iter)->IsActive())
		{
			// �ش� ��ü�� ������� ���� ��쿡��...

			// �浹ü�� �߰��ϸ鼭 ���������� �Ͼ���Ƿ�

			SAFE_RELEASE((*iter));
			iter = m_ColliderList.erase(iter);
			iterEnd = m_ColliderList.end();
			continue;
		}

		//else if (!(*iter)->IsEnable())
		//{
		//	// ��ƴ� ������ Ȱ��ȭ�� �Ǿ� ���� ���� ���.
		//	// �׳� �Ѱ��ش�.
		//	++iter;
		//	continue;
		//}

		// Ȱ��ȭ���� �Ǿ��ִ� ��쿡�� Updateó���� ���ش�.
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

		// �⺻������ MoveAngle�� �̿��ϸ�. Effect������Ʈ�� ��쿡�� ������� �ִ� ��쿡 ó������ �ʴ´�.
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

	// Update�� ���ؼ� ��ü�� �̵��� ���Ŀ�.
	// ������ ���� ���ϰ�.
	// ���� ��ġ�� ����.
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

	//  ������Ʈ ���.


	Position tWorldPos = m_tPos;

	if (m_tStaticSize.x == 0.f)
	{
		tWorldPos -= m_tPivot * m_tSize;
		
		// �ƹ��͵� ������ �Ǿ� ���� ���� ��쿡�� ��� ����� �������� �Ǻ��� ��´�.

	}
	else
		tWorldPos -= m_tPivot * m_tStaticSize;

	Position tCameraPos;

	if (!m_bIsUI)
	{
		tCameraPos = GET_SINGLE(CSceneManager)->GetCameraPos();
		tWorldPos -= tCameraPos;
	}

	// UI�� �ƴϸ�, ī�޶��� ������ �޵��� �Ѵ�.
	// UI�� ������ ���� �ʴ´�.


	// 0, 0 �̶��, �ƹ��� ó���� ���� ���� ü �״��.
	// 0.5f, 0.5f���, �츮�� ������ ��ǥ�� ��ü�� �߽��� �ǹ��ϴ� ���̴�.
	// ����, ������ ����� Ư���� ���� ����� �����ġ�� ����־�� �Ѵ�.

	// m_tSize * m_tPivot �� ���ؼ�, ���� ������� �Űܾ��� ������� ��ġ�� ��´�.
	// �׸��� �װ��� WorldPos�� �����Ű�� Pivot�� �����Ѵ�� ����� �̷�� ���� ���̴�.

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

			// �������� �����Ѵ�.

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
				// ��� ����.

				TransparentBlt(m_Alpha, 0, 0, (int)m_tSize.x, (int)m_tSize.y,
					m_pTexture->GetDC(), (int)tImagePos.x, (int)tImagePos.y,
					(int)m_tSize.x, (int)m_tSize.y, m_pTexture->m_iColorKey);

				BLENDFUNCTION bf;
				bf.BlendOp = 0;
				bf.BlendFlags = 0;
				bf.SourceConstantAlpha = m_iBlend;
				bf.AlphaFormat = 0;

				// ��� ������ �A ����, �����ϰ� �Ͽ� backDC�� �ѱ�
				AlphaBlend(hDC, (int)tWorldPos.x, (int)tWorldPos.y, (int)m_tSize.x, (int)m_tSize.y,
					m_Alpha, 0, 0, (int)m_tSize.x, (int)m_tSize.y, bf);

				// ������ ���� ���� ����
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



	//  �Ʒ� �ּ�.

	//#ifdef _DEBUG
//
//		// ������Ʈ�� ��� ũ�⸦ ǥ���Ѵ�.
//		// ������Ʈ�� �ؽ�ó�� ���ؼ� ����� �Ǵµ� ���� ��ġ�������� Size��ŭ ����� �ϰ� �ִ�.
//
//		HBRUSH hBrush = GREENBRUSH;
//
//		RECT tRC = { tWorldPos.x, tWorldPos.y, tWorldPos.x + m_tSize.x, tWorldPos.y + m_tSize.y };
//		FrameRect(hDC, &tRC, hBrush);
//
//#endif

		// ���� ����� ����, ó���� Start�� �ϴ� �����̳�, ChangeClip�Լ��� ���ؼ� �ִϸ��̼��� Ŭ���� �������ִ� �κ��̳�.
		// ����Ϸ��� �ϴ� DC�� ũ��(��, vector�� �ִ� Coord������ ũ��)�� �ϳ��ϳ� �����ϰ� ������ �ʴ�.
		// ��������� �����ؼ� �ִϸ��̼��� �����Ӵ� ũ�Ⱑ ���������� �ٲ��.
		// �׷��� �װ� �ִϸ��̼� �߸� ����Ŷ�� �� �� �ִ�.
		// ������ �׷� �κ��� ������� �ʰ� �ְ� ����� �ʿ䵵 ����. �ִϸ��̼��� �����µ� �� �����Ӹ��� ����� �ٲ��.
		// �ƹ�ư �׷� �ִϸ��̼��� ����.
		// ����, ���� �����ϴ� ������ ����, �ִϸ��̼��� Ŭ���� �����ϴ� �������� 0��° �������� Size�� ���������Ͽ� ������Ʈ��
		// ��� ������� �����ϴµ�, �̰��� ������ ������ ��� �������� ���� ũ���� ����� �����Ѵٰ� �����ϱ� �����̴�.
		// �׷��� �� Frame���� Size�� �ٽ� ���Թ޴� �׷� ó���� ���� ���̴�.
		// ����, �� �����Ӵ�  SrcPos�� ��ġ�� �ٽ� �޾��ְ� �ִ�. (�̰� �翬�� �޾ƾ� �ϴ� �����ϱ�)
		// �׷��� ����ؾ��ϴ� SrcPos�� ��ġ�� �˸�, �츮�� ������ ũ�⸦ ������ ������ ������ ������ 
		// ����ؾ��ϴ� ���� �κ��� �˾Ƴ� �� �ִ� ���̴�.

		// ��� : �����, Ŭ���� �����ϴ� ��������(start, changeclip) ������ ���ũ��(Size)�� Src�� Pos�� ���� ������ �ȴ�.
		// �� ������ �ִϸ��̼��� �����ġ�� �ٲٰų�, ���ũ�⸦ �ٲٴ� �Ϳ� �����Ͽ� ���ٸ�...

		// �ϴ�, ���� ���� ó���ϴϱ� �ϳ��� �����غ���.
		// ��� ��ġ�� �����ϴ� �κ���... Size�� �ٲ�°� ? No No
		// ���� �ٲ�� ���� ����ϴ� ������ ���̴�.
		// �׷��ٸ� ��� ������ ������ Size���� ���� �� �ʿ�� ����. ( ������ ������ �ƴϴϱ�)
		// ������ �ִ� Ŭ���� vector������ �����Ͽ� �����ġ�� �����Ѵٸ� ...
		// ����ؾ��ϴ� FrameSizeX�� FrameSizeY�� ���� �˰� �ִ� ��쿡 LeftTop�� ��ǥ�� �˰� ������
		// ����ϴµ����� ������ ����.
		// ��� : Size�� ������ �ʿ䰡 ����. �ܼ��� vector���� Start�� ���� �����ϸ� �ȴ�.
		// �ٵ� Start���� �ٲ������, ���߿� ChangeClip�� ���ؼ� �ش� �ִϸ��̼� Ŭ������ ������ �Ͼ�� ��쿡
		// ��Ȯ�� ����� ���� ���� ���ɼ��� ũ��. ex : 0 0 50 50 �̾��µ�
		// -1 -1 50 50 ���� �ٲ�ٸ�, ���߿� �� Ŭ���� ��Ƽ� �����ġ�� ����ٶ��� ũ��� ...
		// 51 51 �� ������ �ȴ�. ( Size�� �ٲٷ��� �� ���� �ƴѵ�, Size�� �ٲ�����.. �̴� ����)
		// �ƹ�ư ���߿� Ŭ�������� �ٲ� ���� �����ؼ� end�� ��ǥ���� ��� �����Ѵ�.

		// ��� ��ǥ�� ����. --> ���� ������ �ִ� Ŭ���� vector������ �����Ͽ� ��ǥ�� �ٸ��� �����Ѵ�.
		// (�������� ����� �����̶�� �� �� �ִ�)


		// ��� ũ���� ������, Size�翬�� �ٲ��� �ϰ�, ��� ������ �ٲ�ٴ� ���� �翬�� Begin�� End�� ������ ���ϴ� ���̴�.
		// �ֳĸ�, Begin�� End�� �ܼ��� ��ǥ���� �ǹ��ϴ� ���� �ƴ� Size�� ���䵵 ���ԵǾ� �ֱ� �����̴�.
		// ��� ũ���� ����. --> ���� ������ �ִ� Ŭ���� vector������ �����Ͽ� ��ǥ�� �ٸ��� �����ϰ�,
		// �̰��� Size�� ���� �����Ų��.
		// �ڿ������� �ڿ������� �ڿ��������Ϸ��� ...
		// size�� ����. lefttop�� ������� �ʾҴٸ�, �ܼ��� right bottom�ʿ��� ���� ����.

		// �ƴ� �ٵ�, �̰��� ���� �̷��� �����ϰ� ������ �ʿ䵵 ���� �׳�,
		// �ø� ��ǥ��, �״�� ������Ʈ���� size�� �Ѱ��ְ� ������Ʈ�� �״�� ����ϵ��� �ϸ� �Ǵ°� �ƴѰ� ?
		// ex : size�� end�ʿ��� 1����. size �� �ٷ� �����.  ������
		//		size�� begin�ʿ��� 1����. (-1) size�ٷ� ����.  
		// ���. end�� ��ǥ��, ����������. �׸���, size�� ���������� ���ִ� �ɷ�.

	//  �츮�� ����ϴ� ����� Texture�� ���� �������� ȣ���� ���ְ� �ִ�.
// Texture�� Render������ �����ǥ�� �̹��������� ��ġ, ��� Size �� �޾��ְ� �ִµ�, ���⼭ � �ִϸ��̼��� �ؽ�ó�� ����ϴ��Ŀ� ����
// ����ϴ� ũ�Ⱑ �翬�� ���Ѵ�.
// ����, ����� �� �� ����, ���� Ŭ���� ���ͼ�, ���� ������ �ϴ� �����ӿ����� ��ǥ�� �޾ƿ´�.
// �ִϸ��̼��� Ŭ���� �ٲٴ� ����, Ȥ�� ó�� �޾Ƽ� ����� ���� ����ؾ��ϴ� �������� ũ��� Size�� �������ְ� �ִ�.
// ��, ���⼭  Size �� ����� ���� ũ���̴�.
// tWorldPos(�����ġ)�� ����ϰ�, tImagePos���� m_tSize��ŭ(�ִϸ��̼��� ��� ũ��) ����Ѵ�.
// iFrame�� �ش��ϴ� DC �� ����Ͽ�, ������ �ִϸ��̼��̶�� DC�� ������ �����ְ�, ��Ʋ�� �ִϸ��̼��̶�� 
// ������� 0�� �ش�.


	// �浹ü�� ���¸� Ȯ���ϰ� �����ִ� ó���� �����Ͽ� Render�� ���ְ� �ִµ�,
	// �̴� Debug ��忡���� ���δ�.

//
//#ifdef _DEBUG
//
//
//	if (m_pAnimation)
//	{
//		// ������� �ִϸ��̼��� �̸��� �����ش�. ������Ʈ�� ��ǥ���� y������ 150 ���� ��Ų ��ġ��
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
//		// �ִϸ��̼��� �����ϴ� �������� 2���� ������ �ʿ��ϴ�.
//		// �ϴ�, SRC������ ���� ��ǥ��, �������� ������ ����.
//		// �̰��� ������ִ� �κ��� �����.
//
//
//
//		vector<ClipCoord>& vecCoord = pClip->vecCoord;
//
//		// ������� �ִϸ��̼��� Src ��ǥ�� ����Ѵ�.
//		TCHAR strPos[32] = {};
//
//		wsprintf(strPos, TEXT("SrcPos : (%d, %d)"), (int)vecCoord[0].tStart.x, (int)vecCoord[0].tStart.y);
//		
//		TextOut(hDC, m_tPos.x + 50.f, m_tPos.y + 250.f, strPos, lstrlen(strPos));
//
//
//
//		// ��� ���� �ִϸ��̼��� ������ ����� ����Ѵ�.
//
//		wsprintf(strPos, TEXT("FrameSize : (%d, %d)"), (int)m_tSize.x, (int)m_tSize.y);
//
//		TextOut(hDC, m_tPos.x + 50.f, m_tPos.y + 270.f, strPos, lstrlen(strPos));
//
//		// ���� �������� ������ش�
//
//		wsprintf(strPos, TEXT("Frame : %d / %d"), pClip->iFrame, vecCoord.size());
//
//		TextOut(hDC, m_tPos.x + 50.f, m_tPos.y + 290.f, strPos, lstrlen(strPos));
//
//
//
//		// �ִϸ��̼��� �� ������ Ÿ���� ������ش�.
//
//		swprintf_s(strPos, 32, TEXT("Frame PlayTime : %.2f / %.2f"), pClip->fTime, pClip->fFrameTime);
//
//		TextOut(hDC, m_tPos.x + 50.f, m_tPos.y + 310.f, strPos, lstrlen(strPos));
//
//
//		// ��ü �÷��� �ð��� ����Ѵ�.
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
	// ��¥�� ������ �Ѵٴ� �ǹ̴� ��Ʋ�󽺶�� �ǹ��̴�.
	// src�� ��ǥ�� map�̶�� �����ϰ� ����ϴ� �����̴�.
	// �߰��� ũ�⸦ ����� ��찡 �����ٵ� �� �κ��� ī�޶��� �������� ó���ؾ� �Ѵ�.
	// ī�޶��� ��ǥ�� �ش��ϴ� ���� Scene���� ���� �ε��� �Ѵ�.
	// UI���� Render�� �ٸ��� ������ ����. ��¶�� ������Ʈ�� ��쿡�� gameManager�� ī�޶��� ��ǥ�� ������ �޾ƾ� �Ѵ�.

	// ���� ���⿡�� �ϳ��� ������Ʈ�� ���� ������ ����ϰ� �� ���̴�. �ֳĸ�, ���콺�� ���� Update�� ���ư��� �ֱ⿡...

	// �츮�� src������ ī�޶� ��ġ�κ��� RS_WIDTH, RS_HEIGHT ��ŭ ����� �ؾ� �Ѵ�.



	Position tImagePos = GET_SINGLE(CSceneManager)->GetCameraPos();
	// ������ ī�޶� ��ǥ�� �̿��Ͽ� src�� ��ǥ�� �Űܹ����٤�.

	if (m_pTexture)
	{
		// �����ġ�� 0,0 ���� RS_WIDTH, RS_HEIGHT
		BitBlt(hDC, 0, 0, RS_WIDTH, RS_HEIGHT, m_pTexture->m_vecDC[0],
			(int)tImagePos.x, (int)tImagePos.y, SRCCOPY);
	}

	Position tPrintOffset = GET_SINGLE(CEditManager)->GetPrintOffset();

	// SIZE�� �ִϸ��̼��� ������ ũ���̴�.
	Position tRenderPos((int)(tPrintOffset.x + RS_WIDTH - m_tSize.x - 100.f), (int)tPrintOffset.y +  30);

	// ������ ��ܿ� �ִϸ��̼��� ��� �κ��� ���μ� �����ش�.
	RECT tRC = {  (int)tRenderPos.x, (int)tRenderPos.y,
		(int)(tRenderPos.x + m_tSize.x),  (int)(tRenderPos.y + m_tSize.y) };

	

	Rectangle(hDC, tRC.left, tRC.top, tRC.right, tRC.bottom);
	// �ִϸ��̼��� �κп� �������� �ʵ���, ���߿� ����Ѵ�.
	// �̷μ� �÷�Ű���� ���������� �ʴ���, ������ �˾ƺ� �� �ִ�.

	{
		// ���� ó���ϰ� �ִ� ������Ʈ�� �±׸� ����Ѵ�.
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

		// �� �ٴ��� �����ġ�� ȭ���� ����� ��쿡�� �������� �༭ 100 + 60 ��ŭ �ö� �ڸ��� ����� �����ϵ��� �Ѵ�.
		if (tRC.bottom + 130 > RS_HEIGHT)
			iBottom = RS_HEIGHT - 160;
	
		// �����ġ�� �� �ٴ��� �Ѿ�� �ʴ� �����쿡�� �׳� �ش� ��ġ�� ����� �ϴ� ������ �Ѵ�.

		// �ִϸ��̼��� �̸��� �����ش�.
		TextOut(hDC, (int)tRenderPos.x, iBottom + 20, r.c_str(), r.size());


		vector<ClipCoord>& vecCoord = pClip->vecCoord;
		vector<ClipCoord>& vecEditCoord = GET_SINGLE(CEditManager)->GetClipCoord();

		vecCoord = vecEditCoord;
		// Edit�� Coord������ ���ͼ�, �� �����Ӹ��� �̰��� �����Ų��.
		// ���⼭ EditCoord ������ ������ �ް� �ȴ�. (��� ��ǥ & ������ ������)

	

		// ��� ���� �ִϸ��̼��� ������ ����� ����Ѵ�.
		TCHAR strPos[32] = {};

		wsprintf(strPos, TEXT("FrameSize : (%d, %d)"), (int)m_tSize.x, (int)m_tSize.y);

		TextOut(hDC, (int)tRenderPos.x, iBottom + 40, strPos, lstrlen(strPos));

		// ���� �������� ������ش�

		wsprintf(strPos, TEXT("Frame : %d / %d"), pClip->iFrame, vecCoord.size());

		TextOut(hDC, (int)tRenderPos.x, iBottom + 60, strPos, lstrlen(strPos));



		// �ִϸ��̼��� �� ������ Ÿ���� ������ش�.

		swprintf_s(strPos, 32, TEXT("Frame PlayTime : %.2f / %.2f"), pClip->fTime, pClip->fFrameTime);

		TextOut(hDC, (int)tRenderPos.x, iBottom + 80, strPos, lstrlen(strPos));


		// ��ü �÷��� �ð��� ����Ѵ�.

		swprintf_s(strPos, 32, TEXT("PlayTime : %.2f"), pClip->fPlayTime);

		TextOut(hDC, (int)tRenderPos.x, iBottom + 100, strPos, lstrlen(strPos));

		if (vecCoord.empty())
			return;


		Position tImage;

		// src������ ��ġ�� �����´�.
		if ((size_t)pClip->iFrame < vecCoord.size())
			tImagePos = vecCoord[pClip->iFrame].tStart;

		// �ִϸ��̼��� ����Ѵ�.
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
		// ������ �ִϸ��̼��� ��ġ�� �ű�� ���� ������, �ش� ������ �����ִ� ��Ȳ������ Static�� �ű�� ���� �´�.
		// ������Ʈ�� pivot�� �������� �����ϴ� ���� ���� �� ����.

		// StaticSize�� ���Ѵ�. ���⼭ offset�� �������ִ� ���̴�.
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

		// �ϴ� �浹ü�� Ȱ��ȭ���� ���� ����� �ص� ���.

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
	// ���� ��� ������ x = 1 y = 1�̸� 
	//  x��� y���� ��� ���� �������� �����̸� m_fMoveSpeed ��ŭ ��������� �����δ�.

}

void CObj::Move(const Position & tDir, float fTime)
{
	// ���� ������ �Լ��ε�, �ð��� ������ �߰��ߴ�.
	m_tPos += tDir * m_fMoveSpeed * fTime;
	// tDir�� ����Ű�� �������� m_fMoveSpeed ��ŭ �̵��ϴµ�, ���⿡ �ð��� ���Ͽ� �������� �����Ѵ�.
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
	// ������ ����Ű�� �������� ���ǵ常ŭ �̵�.
}
void CObj::MoveAngle(float fTime)
{
	m_tPos.x += cosf(m_fAngle) * m_fMoveSpeed * fTime;
	m_tPos.y += sinf(m_fAngle) * m_fMoveSpeed * fTime;

	// �ð��� ������� �̵�.
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
	// �̶�� �ϸ�, x ��������θ� �ð��� ���� �̵�����ŭ �̵�
	// -1.f ���� �̿��ؼ� ���� �������� �����̰� �ϴ� �͵� ����.
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

	// �ڽ��� �浹ü����Ʈ���� strName�� Tag�� �������� ã�Ƽ� ��ȯ.
	// ���⼭ �����͸� ��ȯ�ϹǷ� Ref���� �ϳ� ����.
}

CCollider * CObj::FindCollider()
{
	if (m_ColliderList.size() == 1)
	{
		return *m_ColliderList.begin();
	}

	// �ϳ��� �ִ� ��� �̰��� ��ȯ.
	// �ϳ��� ���� ��� nullptr.
	return nullptr;
}

void  CObj::SetColliderState(const string & strName, bool bEnable)
{
	// �ش� �̸��� ���� Collider�� ã�ƿͼ� ���¸� �������ִ� �Լ��̴�.
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
