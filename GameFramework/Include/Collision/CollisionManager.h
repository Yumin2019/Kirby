#pragma once

#include "../Game.h"
#include "../SerialNumber.h"

// ������� �ϴٰ� ���Ҵ�.

class CCollisionManager
{
	DECLARE_SINGLE(CCollisionManager);

	// �浹ü�� �浹�� �����ϴ� Manager�̴�.

private:
	list<class CCollider*>	m_ColliderList; // UI�� �ƴ� �浹ü�� �޾Ƶα� ���� �浹ü ����Ʈ�̴�.
	list<class CCollider*>	m_UIColliderList; // UI �浹ü�� ��Ƶα� ���� �浹ü ����Ʈ �̴�.
	// �̵� �� �浹ü ����Ʈ�� ��쿡�� �ش� Scene�� ���ϴ� ��� �浹ü�� �޾Ƶδ� �����̴�
	list<class CCollider*>	m_SectionColliderList; // �浹ó�� ������ �����ϴµ� ����� �浹ü ����Ʈ�̴�.
	list<class CCollider*>	m_SectionUIColliderList; // �浹������ ������ UI�浹ü�� ��Ƶ� �浹ü ����Ʈ�̴�.
	// �̵� �� �浹ü ����Ʈ�� ��쿡�� ���� ó���� �� �浹ü���� ��Ƴ��� �浹ó���� �� �� �� ���Ŀ��� 
	// clear�� ���ؼ� ����ϰ� �����ְ� ���� �����ӿ��� �ٽ� �־��ִ� �۾��� ��ġ�� �ִ�.
	// ���ο� �浹ü�� �߰� -> ó�� -> clear �̷� ������ �� ������ ���� �Ͼ�� ���̴�.


	list<class CObj*>		m_WorldObjList;
	// WorldObjList�� ��쿡�� Stage ��, Ÿ�ϰ� �浹�� �ϴ� ��쿡
	// Ÿ�ϰ� ��� ������Ʈ�� �浹ó�����ֱ� ���ؼ� ������ �ִ� List�̴�.

	// ���콺�� �ߺ� �浹�� �������ؼ� ������ �浹�� �浹ü�� ������ �ִ´�.
	class CCollider*		m_pMousePrevCollision;

	CSerialNumber	m_SerialNumber; // SerialNumber�� �����ϱ� ���� ��ü�̴�.

	PCollisionProfile		m_pProfile; // ���������� �������� �Ҵ��Ͽ� �����Ѵ�.
	int						m_iProfileSize;
	int						m_iProfileCapacity; 

	PCollisionChannel		m_pChannel; // ä���� �������� �Ҵ��Ͽ� �����Ѵ�.
	int						m_iChannelSize;
	int						m_iChannelCapacity;


	// CollisionManager���� �浹 ��������� ������ �ִ´�.
private:
	unsigned int**	m_pPrevCollisionAdj; // ���� �浹 ������� unsinged int* �� ��� ������ �ٴϱ� ���� ����
	unsigned int	m_iPrevCollisionCount; // ��������� ����� �������� ������ ����� ���̴�.

public:
	unsigned int CreateSerialNumber(); 
	void ReturnSerialNumber(unsigned int iNumber);
	// CollisionManager�� ���ؼ� SerialNumber�� �Լ��� ȣ���ϱ� ���ؼ� ���� �Լ��̴�. 

	void Clear();
	void WorldObjListClear();

private:
	void AddPrevCollisionAdj(unsigned int iSrc, unsigned int iDest); 
	// iSrc, iDest���� �浹�� ������Ŀ� �߰��ϴ� �Լ��̴�.


	void KirbyCollision(CObj* pObj, float fTime);

public:
	void DeletePrevCollisionAdj(unsigned int iSrc, unsigned int iDest);
	// iSrc, iDest���� �浹�� �����ִ� �Լ��̴�.

	bool CheckPrevCollisionAdj(unsigned int iSrc, unsigned int iDest);
	// iSrc, iDest���� �浹�� Ȯ���ϴ� �Լ��̴�.
	

public:
	void AddWorldObjList(class CObj* pObj);
	void AddCollider(class CCollider* pCollider);
	//  �浹����Ʈ�� �浹ü�� �߰��ϴ� �Լ��̴�. UI���� �ƴ����� ���� �߰��ϴ� List�� ���̰� �ִ�.
	void DeleteCollider(class CScene* pScene); 
	// ����� �־��ָ�, �� ��鿡 �ҼӵǴ� �浹ü�� ��� �浹ü����Ʈ���� �����Ѵ�.

private:
	void AddProfile(const string& strName); // strName�� �̸��� ���� ���������� �����Ѵ�.
	void AddChannel(const string& strName,
		COLLISION_CHANNEL_STATE eDefaultState = CCS_IGNORE); // strName�� �̸��� ���� ä���� �����Ѵ�.
	void SetProfileChannelState(const string& strProfile, 
		const string& strChannel, COLLISION_CHANNEL_STATE eState);

	// �ش��̸��� ���� ���������� �������� strChannel�� �̸��� ���� ä���� eState�� ���·� �����.

	void SetProfileChannelStateAll(const string& strProfile,
		COLLISION_CHANNEL_STATE eState); // �ش� ���������� ��� ä���� eState���·� ������ִ� �Լ��̴�.


public:
	PCollisionProfile FindProfile(const string& strName); // �ش� �̸��� ���� ���������� ã�Ƽ� ��ȯ�Ѵ�.
	int GetChannelIndex(const string& strName) const; // �ش� �̸��� ���� ä���� ã�Ƽ� �ε����� ��ȯ�Ѵ�.

public:
	bool Init(); // ������� ������ ������ְ�, ä���� �����ϸ�, ���������� ������ �����Ѵ�.
	void Collision(float fTime); // �������� �浹ó���� �ϴ� �����̴�.
	// �浹 ������ ���߾� �浹ü�� �߰��ϰ�, �ش� �浹ü�� ���� ä�� ������ Ȯ���ϸ鼭 �浹ó���� �������ְ� �ִ�.
	void ChangeScene();

private:
	void CollisionWorld(float fTime); // ������ ������ �ش��ϴ� �浹ó��
	void CollisionUI(float fTime); // UI�� ������ �ش��ϴ� �浹ó��.

	void CollisionMouseUI(float fTime);
	// �� ���ӿ��� ������Ʈ�� �����ϴ� ��찡 ����. ���� UI


	void TileCollision(float fTime);
};

