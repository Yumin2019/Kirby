#pragma once

#include "../Game.h"
#include "../SerialNumber.h"

// 인접행렬 하다가 말았다.

class CCollisionManager
{
	DECLARE_SINGLE(CCollisionManager);

	// 충돌체와 충돌을 관리하는 Manager이다.

private:
	list<class CCollider*>	m_ColliderList; // UI가 아닌 충돌체를 받아두기 위한 충돌체 리스트이다.
	list<class CCollider*>	m_UIColliderList; // UI 충돌체를 담아두기 위한 충돌체 리스트 이다.
	// 이들 두 충돌체 리스트의 경우에는 해당 Scene에 속하는 모든 충돌체를 받아두는 개념이다
	list<class CCollider*>	m_SectionColliderList; // 충돌처리 영역을 분할하는데 사용할 충돌체 리스트이다.
	list<class CCollider*>	m_SectionUIColliderList; // 충돌영역을 따져서 UI충돌체를 담아둘 충돌체 리스트이다.
	// 이들 두 충돌체 리스트의 경우에는 실제 처리를 할 충돌체들을 담아놓고 충돌처리를 한 번 한 이후에는 
	// clear를 통해서 깔끔하게 날려주고 다음 프레임에서 다시 넣어주는 작업을 거치고 있다.
	// 새로운 충돌체의 추가 -> 처리 -> clear 이런 과정이 매 프레임 마다 일어나는 것이다.


	list<class CObj*>		m_WorldObjList;
	// WorldObjList의 경우에는 Stage 즉, 타일과 충돌을 하는 경우에
	// 타일과 모든 오브젝트를 충돌처리해주기 위해서 가지고 있는 List이다.

	// 마우스의 중복 충돌을 막기위해서 이전에 충돌한 충돌체를 가지고 있는다.
	class CCollider*		m_pMousePrevCollision;

	CSerialNumber	m_SerialNumber; // SerialNumber를 관리하기 위한 객체이다.

	PCollisionProfile		m_pProfile; // 프로파일을 동적으로 할당하여 관리한다.
	int						m_iProfileSize;
	int						m_iProfileCapacity; 

	PCollisionChannel		m_pChannel; // 채널을 동적으로 할당하여 관리한다.
	int						m_iChannelSize;
	int						m_iChannelCapacity;


	// CollisionManager에서 충돌 인접행렬을 가지고 있는다.
private:
	unsigned int**	m_pPrevCollisionAdj; // 이전 충돌 인접행렬 unsinged int* 을 묶어서 가지고 다니기 위한 정보
	unsigned int	m_iPrevCollisionCount; // 인접행렬을 만드는 과정에서 행으로 사용할 값이다.

public:
	unsigned int CreateSerialNumber(); 
	void ReturnSerialNumber(unsigned int iNumber);
	// CollisionManager를 통해서 SerialNumber의 함수를 호출하기 위해서 만든 함수이다. 

	void Clear();
	void WorldObjListClear();

private:
	void AddPrevCollisionAdj(unsigned int iSrc, unsigned int iDest); 
	// iSrc, iDest와의 충돌을 인접행렬에 추가하는 함수이다.


	void KirbyCollision(CObj* pObj, float fTime);

public:
	void DeletePrevCollisionAdj(unsigned int iSrc, unsigned int iDest);
	// iSrc, iDest와의 충돌을 없애주는 함수이다.

	bool CheckPrevCollisionAdj(unsigned int iSrc, unsigned int iDest);
	// iSrc, iDest와의 충돌을 확인하는 함수이다.
	

public:
	void AddWorldObjList(class CObj* pObj);
	void AddCollider(class CCollider* pCollider);
	//  충돌리스트에 충돌체를 추가하는 함수이다. UI인지 아닌지에 따라서 추가하는 List에 차이가 있다.
	void DeleteCollider(class CScene* pScene); 
	// 장면을 넣어주면, 이 장면에 소속되는 충돌체를 모두 충돌체리스트에서 제거한다.

private:
	void AddProfile(const string& strName); // strName의 이름을 가진 프로파일을 생성한다.
	void AddChannel(const string& strName,
		COLLISION_CHANNEL_STATE eDefaultState = CCS_IGNORE); // strName의 이름을 가진 채널을 생성한다.
	void SetProfileChannelState(const string& strProfile, 
		const string& strChannel, COLLISION_CHANNEL_STATE eState);

	// 해당이름을 가진 프로파일의 정보에서 strChannel의 이름을 가진 채널을 eState의 상태로 만든다.

	void SetProfileChannelStateAll(const string& strProfile,
		COLLISION_CHANNEL_STATE eState); // 해당 프로파일의 모든 채널을 eState상태로 만들어주는 함수이다.


public:
	PCollisionProfile FindProfile(const string& strName); // 해당 이름을 가진 프로파일을 찾아서 반환한다.
	int GetChannelIndex(const string& strName) const; // 해당 이름을 가진 채널을 찾아서 인덱스를 반환한다.

public:
	bool Init(); // 인접행렬 정보를 만들어주고, 채널을 생성하며, 프로파일의 정보를 생성한다.
	void Collision(float fTime); // 실질적인 충돌처리를 하는 과정이다.
	// 충돌 영역에 맞추어 충돌체를 추가하고, 해당 충돌체들 끼리 채널 정보를 확인하면서 충돌처리를 진행해주고 있다.
	void ChangeScene();

private:
	void CollisionWorld(float fTime); // 월드의 정보에 해당하는 충돌처리
	void CollisionUI(float fTime); // UI의 정보에 해당하는 충돌처리.

	void CollisionMouseUI(float fTime);
	// 내 게임에서 오브젝트를 선택하는 경우가 없다. 오직 UI


	void TileCollision(float fTime);
};

