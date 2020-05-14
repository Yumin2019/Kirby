#pragma once

#include "../Ref.h"
#include "../SerialNumber.h"
#include "../Scene/Scene.h"


enum OBJECT_TYPE
{
	OT_NONE,
	OT_NORMAL_MONSTER,
	OT_PLAYER,
	OT_PLAYER_ATTACK,
	OT_MONSTER_ATTACK,
	OT_BOSS_DDD,
	OT_ABILITY_MONSTER,
	OT_JUMP_STAR,
	OT_MOVE_STAR,
	OT_UI,
	OT_DOOR,
	OT_STAGE,
	OT_IMAGE
};



// Obj Position���� �ϰ� ����.
class CObj :
	public CRef
{

	friend class CEditManager;

protected:
	CObj();
	CObj(const CObj& obj);
	virtual ~CObj() = 0;

	// ������ Ÿ�� ���� Ȥ�� ������Ʈ ������
	// CObjŬ������ ��ü�� �����ϴ� ���̶�� = 0 �� Ǯ���� ����.

	// �ƹ�����.. release�� ������� ��ü�� �������ִµ�, 
	// �̰Ͱ��� ������. �ڽ��� ��ü���� �θ��� ��������ڸ� ȣ���ؾ� �ϴ�...
	// protected �� ������ ������ ���δ�.

private:
	static CSerialNumber	m_SerialNumber; // CObj���ο��� ����� SerialNumber���� �������ִ� static ����̴�.

protected:
	string		m_strLayerName;	 // �Ҽ� Layer�� �̸��� ������ �ֵ��� �Ѵ�.
	// CLayer* �� �ƴ� Layer�� �̸��� ������ �ִ� ������ �ƹ�����, ���������̶�� �����Ѵ�.
	// Layer*�� �־��ִ� ��쿡�� �ݵ�� Layer�� ������ �ǰ�, �ش� �����͸� ������ �־�� �Ѵٴ� ���̴�.
	// �ݸ鿡 string�� ���ؼ� ������ �����Ѵٸ�,
	// Layer, Obj�� ���� �����ʹ� ������� �Ϸ������� Layer�� Obj�� �־�αⰡ ���ϴ�.
	// Layer�� ������ �Ǿ� ���� �ʴ��� �̸� string ���� �־� ���� �ʱ�ȭ�� �ϴ� ������ ��ٸ��⸸ �ϸ� �Ǵ� ���̴�.
	//	1. �ݵ�� Layer�� ���� ������ �Ǿ� ���� �ʴ��� �̸� �ʱ�ȭ�� �� �� �ִ�.
	//  2. Layer*�� ������ �Ѵٸ�, �� ������ �ҷ����� ���ؼ� �ڵ尡 �߰��Ǿ�� �Ѵ�.
	// ���� ���, CreateLayer �Լ� ���� ���� �߰��Ͽ�, Layer* �� ��ȯ�ް� ���⼭
	// �������� ������Ʈ �߰��Լ��� ȣ���Ѵٵ���... �׷��� �ڵ尡 ������ �ȴ�.
	// string�̶�� �̸� ���ø� �ϰ�, �Ϸ��� �۾��� ���� �ڽ��� Layer�� ã�Ƽ� �� Layer���� �ش� ��ü�� �־��ִ� �������
	// ó���ϸ�, ����� �����ϱ⿡ �ſ� ���ϴ�.
	// �Դٰ� Layer��ü�� �׸� ���� ���� �ƴϴϱ� �׸� ���� �۾��� �ƴϴ�.
	
	class CScene*		m_pScene; // �ڽ��� �Ҽ� Scene�� ��Ƶ� ����.
	bool				m_bIsUI; // UI������Ʈ���� �ƴ���.
	unsigned int		m_iSerialNumber;	

public:
	bool IsUI() const
	{
		return m_bIsUI;
	}

	bool GetInputEnable() const
	{
		return !m_bInputDisEnable;
	}
	
	bool GetDieEnable() const
	{
		return m_bDie;
	}

protected:
	Position	m_tPos; // ��ü�� ��ġ
	Position	m_tPrevPos; // ��ü�� ���� ��ġ
	Position		m_tMove; // ���� �����Ӱ� ���� �������� �̵���.

	Position	m_tPivot; // ��ü�� Pivot
	Size		m_tSize; // ��ü�� ũ��
	float		m_fMoveSpeed; // ��ü�� �ʴ� �ӵ�
	class CTexture*		m_pTexture; // ��ü�� �ؽ�ó
	list<class CCollider*> m_ColliderList; // �浹ü ����Ʈ
	float					m_fAngle;
	class CAnimation*		m_pAnimation;
	Size			m_tStaticSize; // Ÿ�ϰ� �浹�� �� / �浹ü�� ������� ��ġ�� ���� �� ���.
	bool					m_bGravity;
	float					m_fGravityTime;
	SIDE_DIR			 m_ePrevDir;
	bool				m_bAngle;
	
	// ���� �浹�� �̿�
	bool	m_bStageCollision;
	bool	m_bLineCollision;
	bool	m_bPrevLineCollision;
	bool	m_bMiddleCollision;

	// Ÿ�� �浹 ����
	bool	m_bTileCollisionEnable;

	// ���� �浹 ����
	bool	m_bLineCollisionEnable;

	// ������Ÿ�� ����
	bool	m_bPrototype;

	// ObjectType
	OBJECT_TYPE		m_eObjectType;


	// Die Time
	float		m_fPlayTime;
	float		m_fPlayLimitTime;
	bool		m_bPlayTime;
	bool		m_bDie;
	bool		m_bDieEffect;


	// Blend
	bool			m_bBlend; // Blend ���
	float			m_fBlendTime; // ���忡 ����� �ð���
	float			m_fBlendLimitTime; // �����ӿ� ����� �����̴�.
	int				m_iBlendCount; // ���� Ƚ��
	int				m_iBlendLimitCount; // ���ѵ� Ƚ��
	unsigned char			m_iBlend; // ���� ��ġ
	unsigned char			m_iBlendMin;
	unsigned char			m_iBlendMax;

	// �˹�.
	bool			m_bKnockBack; // Is or Not
	bool			m_bKnockUp;

	float			m_fKnockBackTime; // �ð� ��ø��.
	float			m_fKnockUpTime;

	float			m_fKnockBackLimitTime; // ���� �ð� üũ
	float			m_fKnockUpLimitTime;


	float			m_fKnockBackPower; // Power��.
	float			m_fKnockUpPower;

	float			m_fKnockBackPowerTime; // �Ŀ� �ð� ��ø��.
	float			m_fKnockUpPowerTime;


	// �浹�� �� ������ ���Ⱚ
	SIDE_DIR		m_eCollisionDir;

	// �˾��� ������ �� �ʰ�, õõ�� ���������� �Ѵ�.
	bool			m_bKnockUpEnd;
	float			m_fKnockUpEndTime;
	float			m_fKnockUpEndLimitTime;

	// �˹� / �˾��� �������� �Է��� ���� �ð��� �����Ѵ�.
	bool			m_bInputDisEnable;
	float			m_fInputDisEnableTime;
	float			m_fInputDisEnableLimitTime;

	

public:
	
	void SetInputDisEnable(float fDisEnableTime);

	void InputEnable()
	{
		m_bInputDisEnable = false;
	}

	void KnockUpEndDisEnable()
	{
		m_bKnockUpEnd = false;
	}

	void DieEffectDisEnable()
	{
		m_bDieEffect = false;
	}
	
	void DisEnableKnockBack()
	{
		m_bKnockBack = false;

	}

	void DisEnableKnockUp()
	{
		m_bKnockUp = false;

	}


	void SetKnockBack(float fPlayTime, float fPower, SIDE_DIR eCollisionDir);
	void SetKnockUp(float fPlayTime, float fPower, float fKnockUpEndTime = 1.f);

	bool GetKnockBack() const
	{
		return m_bKnockBack;
	}

	bool GetKnockUp() const
	{
		return m_bKnockUp;
	}

	bool GetBlend() const
	{
		return m_bBlend;
	}

	void SetBlend(float fIntervalTime, int iLimitCount, unsigned char iBlendMin = 180, unsigned char iBlendMax = 255);
	

	static void StarEffectCreate(Position tStartPos, float fPlayTime = 1.f,
		float fMoveSpeed = 50.f, const string& strLayer = "Default");

	void SetPlayTime(float fLimit)
	{
		m_fPlayLimitTime = fLimit;
		m_bPlayTime = true;
	}

	void SetObjectType(OBJECT_TYPE eType)
	{
		m_eObjectType = eType;
	}

	void SetTileCollisionEnable(bool bEnable)
	{
		m_bTileCollisionEnable = bEnable;
	}

	void SetLineCollisionEnable(bool bEnable)
	{
		m_bLineCollisionEnable = bEnable;
	}


	virtual void SetGravity(bool bEnable)
	{
		m_bGravity = bEnable;
	}

	void MiddleCollisionEnable()
	{
		m_bMiddleCollision = true;
	}

	void StageCollisionEnable()
	{
		m_bStageCollision = true;
	}


	void LineCollisionEnable()
	{
		m_bLineCollision = true;
	}

	


public:
	void GravityTimeDisEnable()
	{
		m_fGravityTime = 0.f;
	}


public:

	void SetStaticSize(float x, float y)
	{
		m_tStaticSize = Size(x, y);
	}

	void SetStaticSize(const Size& tSize)
	{
		m_tStaticSize = tSize;
	}

	void SetPos(float x, float y)
	{
		m_tPos.x = x;
		m_tPos.y = y;
	}

	void SetPos(int x, int y)
	{
		m_tPos.x = (float)x;
		m_tPos.y = (float)y;
	}
	
	void SetPos(const Position& tPos)
	{
		m_tPos = tPos;
	}

	void SetPivot(float x, float y)
	{
		m_tPivot.x = x;
		m_tPivot.y = y;
	}

	void SetPivot(const Position& tPivot)
	{
		m_tPivot = tPivot;
	}

	void SetSize(float x, float y)
	{
		m_tSize.x = x;
		m_tSize.y = y;

	}

	void SetSize(const Size& tSize)
	{
		m_tSize = tSize;
	}

	void SetMoveSpeed(float fSpeed)
	{
		m_fMoveSpeed = fSpeed;
	}

public:

	OBJECT_TYPE GetObjectType() const
	{
		return m_eObjectType;
	}

	bool GetLineCollisionEnable() const
	{
		return m_bLineCollisionEnable;
	}

	bool GetTileCollisionEnable() const
	{
		return m_bTileCollisionEnable;
	}

	bool GetMiddleCollision() const
	{
		return m_bMiddleCollision;
	}

	bool GetStageCollision() const
	{
		return m_bStageCollision;
	}

	bool GetLineCollision() const
	{
		return m_bLineCollision;
	}

	bool GetPrevLineCollision() const
	{
		return m_bPrevLineCollision;
	}

	Position GetMove() const
	{
		return m_tPos - m_tPrevPos;
		// ������ ó���� ���ؼ� ���ϴ� ������ Move���� ������ �Ǿ� �ִ�.
	}

	Position GetPrevPos() const
	{
		return m_tPrevPos;
	}

	Size GetStaticSize() const
	{
		return m_tStaticSize;
	}

	Position GetPos() const
	{
		return m_tPos;
	}

	Size GetSize() const
	{
		return m_tSize;
	}

	float GetAngle() const
	{
		return m_fAngle;
	}

	class CAnimation* GetAnimation() const
	{
		return m_pAnimation;
	}

	Position GetPivot() const
	{
		return m_tPivot;
	}

	float GetMoveSpeed() const
	{
		return m_fMoveSpeed;
	}

	string GetLayerName() const
	{
		return m_strLayerName;
	}

	class CScene* GetScene() const
	{
		return m_pScene;
	}
	
	bool GetEffectEnable() const
	{
		return m_bDieEffect;
	}

public:
	void SetLayerName(const string& strName)
	{
		m_strLayerName = strName;
	}

	void SetScene(class CScene* pScene)
	{
		m_pScene = pScene;
	}

	void UpdatePos(const Position& tPos);
	void PrototypeEnable()
	{
		m_bPrototype = true;
	}


public:
	void SetTexture(const string& strName, const TCHAR* pFileName = nullptr,
		const string& strPathName = TEXTURE_PATH);
	// ��ü�� Texture�� �����ϴ� �Լ��̴�.
	// ResourceManager�� ���ؼ� �ؽ�ó�� �ҷ����� �Լ��� ȣ���Ѵ�.
	// strPathName ���� pFileName�� �ٿ��� �̰��� ��η� �Ͽ� �ؽ�ó�� �о�´�.
	// �̷��� �о�� ������ strName �� Ű�������Ͽ� Texture Map�� �־�д�.
	// �̷��� ������ Texture�� �ش� ��ü���� �ش�.

	void SetTexture(class CTexture* pTexture);

	void SetColorKey(unsigned char r, unsigned char g, unsigned char b);
	// �ڽ��� �ؽ�ó�� �ش� ColorKey�� �������ش�.

	CObj* FindObject(const string& strName); 
	// Scene���� ��� ������Ʈ�� �߰��صξ���. 
	// Ư�� ��ü�� ã�ƿ� �� �ֵ��� �����ϱ� ���ؼ�
	// �� �Լ��� ���ؼ� Scene�� �����ϰ� Scene���� ���ϴ� ��ü�� ã���ش�.

	void CreateAnimation(const string& strName);

public:
	virtual void Start();
	virtual bool Init();
	virtual void Update(float fTime);
	virtual void LateUpdate(float fTime);
	virtual void Collision(float fTime);
	virtual void PrevRender(HDC hDC, float fTime);
	virtual void Render(HDC hDC, float fTime);
	virtual void PostRender(HDC hDC, float fTime);
	virtual void EditRender(HDC hDC, float fTime, bool bOffset, Position tOffset);
	virtual CObj* Clone() const;
	virtual void AddClipName(vector<string>& vecClipName) = 0;

	void ColliderRender(HDC hDC, float fTime);

	//  SceneManager�������� �ش� �Լ��� ������ ���缭,
	// ��������� ������ �����Ѵ�.
	// ��ü�� �ٰ��� �Ǵ� CObj�Լ������� �ڽĿ��� �������Ͽ� ����� �� �ֵ��� �����Լ��� �����Ѵ�.

	void ChangeDefaultClip();

public:
	void Move(const Position& tDir);			// ������ �������ָ�, ������ ���ؼ� ���ǵ常ŭ�̵�
	void Move(const Position& tDir, float fTime); // ����� �ð��� �����ָ� �ð��� ������� ���ǵ� ��ŭ�̵�
	void Move(DIR eDir);	// �ܹ����� ���ǵ�� �̵�
	void Move(DIR eDir, float fTime);	// �ܹ����� ���ǵ�� �̵��ϴµ� �ð��� �������.
	void MoveAngle();
	void MoveAngle(float fTime);
	void MoveAngle(float fAngle, float fSpeed, float fTime); // �־��� ������ ���ǵ带 �̿��ؼ� �����̴� �Լ��̴�.
	void Move(const Position& tDir, float fSpeed, float fTime);




	// ������
	vector<ClipCoord>& GetvecCoord() const;
	
public:

	void SetAngle(float fAngle)
	{
		m_fAngle = fAngle;
		m_bAngle = true;
	}

	bool GetPrototype() const
	{
		return m_bPrototype;
	}


	void SetDieTime(float fLimit, bool bEffect)
	{
		// �̹� ������ �� ��츦 ���´�.
		SetDieTime(fLimit);
		m_bDieEffect = bEffect;
	}

	void SetDieTime(float fLimit)
	{
		m_bDie = true;
		m_fPlayTime = 0.f;
		m_fPlayLimitTime = fLimit;
	}

	void LoadClip(const char* pFileName);


public:
	void SetColliderStateAll(bool bEnable);

public:
	// ��� ��ȯ�ÿ� �浹ü�� CollisionManager�� ������ �־��ִ� �Լ��̴�.
	// (Tile�浹�� ���� ó���� �̹� �ص� �����̴�)
	// �츮�� �浹ü�� ��´�.

	void AddPrevSceneCollider();
	// �浹ü ���� �Լ�

	// Static ����� �������� Body �浹ü�� ������ִ� �Լ��̴�.
	void AddBodyCollision(Size tStaticSize, const string& strColliderTag, const string& strProfile, const string& strChannel);

	CCollider* FindCollider(const string& strName); // �־��� strName ���� Tag�� ���� Collider�� ã�Ƽ� ��ȯ
	CCollider* FindCollider();

	// �浹ü�� �̸��� �˾ƾ߸� ����� �� �ִ� �Լ��� �׷��� ���� �Լ�(�� �浹ü�� �ϳ�)
	void SetColliderState(const string& strName, bool bEnable);
	void SetColliderState(bool bState = false);

	//  CObj�� ����� �޴� �������� Ŭ������ �������ְ� ���� ���ø��Լ��� �����.
	// �ܺο��� �ش� Ŭ������ �������� �� �ֵ��� static �Լ��� .

	template <typename T>
	static T* CreateObj(const string& strName, const string& strLayerName,
		CScene* pScene)
	{
		// strTag�� �±װ����� �������ְ�, strLayerName�� �Ҽ� Layer�� �������ָ�,
		// pScene�� �Ҽ����� �Ѵ�.

		CObj* pObj = new T;

		pObj->SetTag(strName);
		pObj->SetLayerName(strLayerName);
		pObj->m_iSerialNumber = m_SerialNumber.CreateSerialNumber(); // ���⼭ �ش� ������Ʈ���� SerialNumber�� �Ҵ��Ѵ�.

		if (!pObj->Init())
		{
			// �� Ŭ�������� Init�� ���ǰ� �Ǿ� �־, �� ��ü���� �ʱ�ȭ�� ���ش�.
			SAFE_RELEASE(pObj);
			return nullptr;
		}

		if(pScene)
			pScene->AddObject(pObj);

		return (T*)pObj;
	}

	// � Ŭ������ ��ü�� ������Ÿ���� ������ ���� �� �� ����.
	// �־��� Scene���� strTag�� Ű�� �����Ͽ� �ش� Scene�� ������Ÿ���� ������ش�.
	// tag�ε� ����ϰ� Ű�ε� ����ϰ�..

	template <typename T>
	static T* CreatePrototype(const string& strTag,
		CScene* pScene)
	{

		// ������Ÿ���� ������ִ� �Լ��̴�.

		CObj* pObj = new T; // ref 1

		pObj->SetTag(strTag);

		if (!pObj->Init())
		{
			// �Ҵ��� ��ü
			SAFE_RELEASE(pObj);
			return false;
		}

		pScene->AddPrototype(pObj);
		pObj->PrototypeEnable();

		return (T*)pObj;

		// ������ Ÿ���� ��ü�� ���� ������� ?

		// SceneManager�� ���ؼ� 1���� ref = 1
		// ������ 1�� ? 

	}

	template <typename T>
	static T* CreateCloneObject(const string& strProtoName,
		const string& strTag, const string& strLayerName,
		CScene* pScene)
	{
		// Scene�� ������Ÿ���� �̿��ؼ� ����� Prototype��ü�� ����� �Լ��̴�.
		// �ܺο��� ������Ʈ�� �������� �� �ֵ��� static �Լ��� �Ǿ� �ִ�.

		CObj* pObj = pScene->ClonePrototype(strProtoName, strTag);
		
		// ������Ÿ�Կ��� strProtoName�� ���� ��ü�� �����ؼ� strName�� �±׸� �����Ͽ�
		// pObj�� ��ȯ���� ���̴�.

		if (!pObj)
			return nullptr; // ��ü�� ����� ������ ���� ���� ���̴�. nullptr return

		// ��ü�� ����� ������ �Ǿ��ٸ� ref = 1�̵ȴ�.

		pObj->SetLayerName(strLayerName);
		pObj->m_iSerialNumber = m_SerialNumber.CreateSerialNumber(); 
		// �ƹ�ư ��ġ�Ǵ� ������Ʈ�̴�. �翬�� SerialNumber�� ����Ѵ�.

		// Scene���� �ش� ��ü������ �ֵ��� �Ѵ�.
		pScene->AddObject(pObj);
		
		// �� ���Ŀ�, ObjList������ �ش� ��ü�� ã�ƿ��� ���� �������� ������ ���� ���̰�
		// WorldObjList�� ��쿡�� ������ map�� ��ġ�ϴ� �������� �ٸ� ������Ʈ�� ����������
		// start�Լ��� ȣ���ϴ� ���� ������ ��ĥ ���̴�.
		// ���⼭ ref 1�� ���°� �����Ǵµ�, Scene's List���� ����Ƚ���� ������Ű�� �ʰ� �����ϱ�..

		// ���߿� WorldObjList���� ��ü�� �ϳ��� ��ġ�ϴ� �������� Layer�� �߰��Ѵ�.
		// Layer �� �� �־ ref = 1.
		// �� �ȴ�.

		// CreateObj�Լ����� �������� ?

		// �ش� �Լ��� Prototype�� �̿����� �ʴ´�.
		// �� �Լ��� �̸������� ������Ÿ���� ��ü�� �����Ͽ� �������µ� �����ϰ� ����� �� �ִ�.

		return (T*)pObj;
	}

	// ���� �浹ü Ÿ���� �ִµ�, �ƹ�ư � Ŭ������ ��ü�� �浹ü�� �������� �� �� ����.

	template <typename T>
	T* AddCollider(const string& strName)
	{
		T* pCollider = new T; // ref = 1


		pCollider->SetTag(strName);
		pCollider->m_pObj = this;
		pCollider->Init();

		// �浹ü�� �ڽ��� �Ҽ�Scene�� �����Ѵ�.
		pCollider->SetScene(m_pScene);

		if (m_bIsUI)
		{
			pCollider->EnableUI();

		}

		pCollider->AddRef(); // ref = 2
		m_ColliderList.push_back(pCollider);

		return pCollider; // ref 2 return
	}

};


