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



// Obj Position까지 하고 말음.
class CObj :
	public CRef
{

	friend class CEditManager;

protected:
	CObj();
	CObj(const CObj& obj);
	virtual ~CObj() = 0;

	// 프로토 타입 생성 혹은 오브젝트 생성이
	// CObj클래스의 객체를 생성하는 것이라면 = 0 을 풀도록 하자.

	// 아무래도.. release를 기반으로 객체를 삭제해주는데, 
	// 이것과는 별개로. 자식의 객체에서 부모의 복사생성자를 호출해야 하니...
	// protected 로 설정한 것으로 보인다.

private:
	static CSerialNumber	m_SerialNumber; // CObj내부에서 사용할 SerialNumber값을 생성해주는 static 멤버이다.

protected:
	string		m_strLayerName;	 // 소속 Layer의 이름을 가지고 있도록 한다.
	// CLayer* 가 아닌 Layer의 이름을 가지고 있는 이유는 아무래도, 시차때문이라고 생각한다.
	// Layer*를 넣어주는 경우에는 반드시 Layer가 생성이 되고, 해당 포인터를 가지고 있어야 한다는 점이다.
	// 반면에 string을 통해서 정보를 저장한다면,
	// Layer, Obj의 생성 순서와는 상관없이 일련적으로 Layer에 Obj를 넣어두기가 편하다.
	// Layer가 생성이 되어 있지 않더라도 미리 string 값을 넣어 놓고 초기화를 하는 순간을 기다리기만 하면 되는 것이다.
	//	1. 반드시 Layer가 먼저 생성이 되어 있지 않더라도 미리 초기화를 할 수 있다.
	//  2. Layer*로 셋팅을 한다면, 이 정보를 불러오기 위해서 코드가 추가되어야 한다.
	// 예를 들면, CreateLayer 함수 같은 것을 추가하여, Layer* 를 반환받고 여기서
	// 직접적인 오브젝트 추가함수를 호출한다든지... 그러면 코드가 귀찮게 된다.
	// string이라면 미리 셋팅만 하고서, 일련의 작업을 통해 자신의 Layer를 찾아서 그 Layer에서 해당 객체를 넣어주는 방식으로
	// 처리하면, 사람이 관리하기에 매우 편리하다.
	// 게다가 Layer자체가 그리 많은 편이 아니니까 그리 느린 작업은 아니다.
	
	class CScene*		m_pScene; // 자신의 소속 Scene을 담아둘 변수.
	bool				m_bIsUI; // UI오브젝트인지 아닌지.
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
	Position	m_tPos; // 객체의 위치
	Position	m_tPrevPos; // 객체의 이전 위치
	Position		m_tMove; // 이전 프레임과 현재 프레임의 이동량.

	Position	m_tPivot; // 객체의 Pivot
	Size		m_tSize; // 객체의 크기
	float		m_fMoveSpeed; // 객체의 초당 속도
	class CTexture*		m_pTexture; // 객체의 텍스처
	list<class CCollider*> m_ColliderList; // 충돌체 리스트
	float					m_fAngle;
	class CAnimation*		m_pAnimation;
	Size			m_tStaticSize; // 타일과 충돌할 때 / 충돌체의 상대적인 위치를 잡을 때 사용.
	bool					m_bGravity;
	float					m_fGravityTime;
	SIDE_DIR			 m_ePrevDir;
	bool				m_bAngle;
	
	// 직선 충돌에 이용
	bool	m_bStageCollision;
	bool	m_bLineCollision;
	bool	m_bPrevLineCollision;
	bool	m_bMiddleCollision;

	// 타일 충돌 여부
	bool	m_bTileCollisionEnable;

	// 직선 충돌 여부
	bool	m_bLineCollisionEnable;

	// 프로토타입 여부
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
	bool			m_bBlend; // Blend 사용
	float			m_fBlendTime; // 블렌드에 사용할 시간값
	float			m_fBlendLimitTime; // 깜빡임에 사용할 변수이다.
	int				m_iBlendCount; // 현재 횟수
	int				m_iBlendLimitCount; // 제한된 횟수
	unsigned char			m_iBlend; // 블렌딩 수치
	unsigned char			m_iBlendMin;
	unsigned char			m_iBlendMax;

	// 넉백.
	bool			m_bKnockBack; // Is or Not
	bool			m_bKnockUp;

	float			m_fKnockBackTime; // 시간 중첩용.
	float			m_fKnockUpTime;

	float			m_fKnockBackLimitTime; // 끄는 시간 체크
	float			m_fKnockUpLimitTime;


	float			m_fKnockBackPower; // Power값.
	float			m_fKnockUpPower;

	float			m_fKnockBackPowerTime; // 파워 시간 중첩용.
	float			m_fKnockUpPowerTime;


	// 충돌을 한 시점의 방향값
	SIDE_DIR		m_eCollisionDir;

	// 넉업이 끝나고 몇 초간, 천천히 떨어지도록 한다.
	bool			m_bKnockUpEnd;
	float			m_fKnockUpEndTime;
	float			m_fKnockUpEndLimitTime;

	// 넉백 / 넉업의 영향으로 입력을 막는 시간을 정의한다.
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
		// 정밀한 처리를 위해서 원하는 순간에 Move값을 얻어오게 되어 있다.
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
	// 객체의 Texture를 셋팅하는 함수이다.
	// ResourceManager를 통해서 텍스처를 불러오는 함수를 호출한다.
	// strPathName 에서 pFileName을 붙여서 이것을 경로로 하여 텍스처를 읽어온다.
	// 이렇게 읽어온 정보를 strName 을 키값으로하여 Texture Map에 넣어둔다.
	// 이렇게 생성한 Texture를 해당 객체에게 준다.

	void SetTexture(class CTexture* pTexture);

	void SetColorKey(unsigned char r, unsigned char g, unsigned char b);
	// 자신의 텍스처에 해당 ColorKey를 설정해준다.

	CObj* FindObject(const string& strName); 
	// Scene에서 모든 오브젝트를 추가해두었다. 
	// 특정 객체를 찾아올 수 있돌고 구현하기 위해서
	// 이 함수를 통해서 Scene에 접근하고 Scene에서 원하는 객체를 찾아준다.

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

	//  SceneManager에서부터 해당 함수의 구조를 맞춰서,
	// 하향식으로 구조를 설계한다.
	// 객체의 근간이 되는 CObj함수에서는 자식에서 재정의하여 사용할 수 있도록 가상함수로 정의한다.

	void ChangeDefaultClip();

public:
	void Move(const Position& tDir);			// 방향을 정의해주면, 방향을 향해서 스피드만큼이동
	void Move(const Position& tDir, float fTime); // 방향과 시간을 정해주면 시간을 기반으로 스피드 만큼이동
	void Move(DIR eDir);	// 단방향을 스피드로 이동
	void Move(DIR eDir, float fTime);	// 단방향을 스피드로 이동하는데 시간을 기반으로.
	void MoveAngle();
	void MoveAngle(float fTime);
	void MoveAngle(float fAngle, float fSpeed, float fTime); // 주어진 각도와 스피드를 이용해서 움직이는 함수이다.
	void Move(const Position& tDir, float fSpeed, float fTime);




	// 편집용
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
		// 이미 지정이 된 경우를 막는다.
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
	// 장면 전환시에 충돌체를 CollisionManager에 정보로 넣어주는 함수이다.
	// (Tile충돌을 위한 처리는 이미 해둔 상태이다)
	// 우리는 충돌체를 담는다.

	void AddPrevSceneCollider();
	// 충돌체 생성 함수

	// Static 사이즈를 기준으로 Body 충돌체를 만들어주는 함수이다.
	void AddBodyCollision(Size tStaticSize, const string& strColliderTag, const string& strProfile, const string& strChannel);

	CCollider* FindCollider(const string& strName); // 넣어준 strName 값을 Tag로 가진 Collider를 찾아서 반환
	CCollider* FindCollider();

	// 충돌체의 이름을 알아야만 사용할 수 있는 함수와 그렇지 않은 함수(단 충돌체는 하나)
	void SetColliderState(const string& strName, bool bEnable);
	void SetColliderState(bool bState = false);

	//  CObj의 상속을 받는 가변적인 클래스를 생성해주고 위한 템플릿함수를 만든다.
	// 외부에서 해당 클래스를 생성해줄 수 있도록 static 함수로 .

	template <typename T>
	static T* CreateObj(const string& strName, const string& strLayerName,
		CScene* pScene)
	{
		// strTag를 태그값으로 설정해주고, strLayerName을 소속 Layer로 설정해주며,
		// pScene의 소속으로 한다.

		CObj* pObj = new T;

		pObj->SetTag(strName);
		pObj->SetLayerName(strLayerName);
		pObj->m_iSerialNumber = m_SerialNumber.CreateSerialNumber(); // 여기서 해당 오브젝트에게 SerialNumber를 할당한다.

		if (!pObj->Init())
		{
			// 각 클래스마다 Init이 정의가 되어 있어서, 각 객체마다 초기화를 해준다.
			SAFE_RELEASE(pObj);
			return nullptr;
		}

		if(pScene)
			pScene->AddObject(pObj);

		return (T*)pObj;
	}

	// 어떤 클래스의 객체로 프로토타입이 생성이 될지 알 수 없다.
	// 넣어준 Scene에서 strTag를 키로 갖게하여 해당 Scene의 프로토타입을 만들어준다.
	// tag로도 사용하고 키로도 사용하고..

	template <typename T>
	static T* CreatePrototype(const string& strTag,
		CScene* pScene)
	{

		// 프로토타입을 만들어주는 함수이다.

		CObj* pObj = new T; // ref 1

		pObj->SetTag(strTag);

		if (!pObj->Init())
		{
			// 할당한 객체
			SAFE_RELEASE(pObj);
			return false;
		}

		pScene->AddPrototype(pObj);
		pObj->PrototypeEnable();

		return (T*)pObj;

		// 프로토 타입의 객체는 언제 사라질까 ?

		// SceneManager를 통해서 1감소 ref = 1
		// 나머지 1은 ? 

	}

	template <typename T>
	static T* CreateCloneObject(const string& strProtoName,
		const string& strTag, const string& strLayerName,
		CScene* pScene)
	{
		// Scene의 프로토타입을 이용해서 복사된 Prototype객체를 만드는 함수이다.
		// 외부에서 오브젝트를 생성해줄 수 있도록 static 함수로 되어 있다.

		CObj* pObj = pScene->ClonePrototype(strProtoName, strTag);
		
		// 프로포타입에서 strProtoName을 가진 객체를 복사해서 strName의 태그를 설정하여
		// pObj에 반환했을 것이다.

		if (!pObj)
			return nullptr; // 객체가 제대로 생성이 되지 않은 것이다. nullptr return

		// 객체가 제대로 생성이 되었다면 ref = 1이된다.

		pObj->SetLayerName(strLayerName);
		pObj->m_iSerialNumber = m_SerialNumber.CreateSerialNumber(); 
		// 아무튼 배치되는 오브젝트이다. 당연히 SerialNumber를 줘야한다.

		// Scene에서 해당 객체가지고 있도록 한다.
		pScene->AddObject(pObj);
		
		// 이 이후에, ObjList에서는 해당 객체를 찾아오기 위한 보따리로 가지고 있을 것이고
		// WorldObjList의 경우에는 실제로 map에 배치하는 과정에서 다른 오브젝트와 마찬가지고
		// start함수를 호출하는 등의 과정을 거칠 것이다.
		// 여기서 ref 1인 상태가 유지되는데, Scene's List에서 참조횟수를 증가시키지 않고 있으니까..

		// 나중에 WorldObjList에서 객체를 하나씩 배치하는 과정에서 Layer에 추가한다.
		// Layer 에 들어가 있어서 ref = 1.
		// 잘 된다.

		// CreateObj함수와의 차이점은 ?

		// 해당 함수는 Prototype을 이용하지 않는다.
		// 이 함수는 미리생성한 프로토타입의 객체를 복사하여 가져오는데 유용하게 사용할 수 있다.

		return (T*)pObj;
	}

	// 여러 충돌체 타입이 있는데, 아무튼 어떤 클래스의 객체가 충돌체로 생성될지 알 수 없다.

	template <typename T>
	T* AddCollider(const string& strName)
	{
		T* pCollider = new T; // ref = 1


		pCollider->SetTag(strName);
		pCollider->m_pObj = this;
		pCollider->Init();

		// 충돌체도 자신의 소속Scene이 존재한다.
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


