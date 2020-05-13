#pragma once

#include "../Game.h"

// 현재 다른 클래스와의 연결 관계 때문에 아직 못한 부분이 많음.

class CSceneManager
{
	DECLARE_SINGLE(CSceneManager);

private:
	class CScene*				m_pScene;		// 현재 Scene
	class CScene*				m_pNextScene;	// 다음 Scene
	list<class CLayer*>			m_LayerList;	// 실질적인 Layer는 SceneManager에서 관리한다.
	bool						m_bCreateScene;	// Ref와 연동해서 사용할 정보이다.
	// 장면이 추가 되는 중인 경우에는 Start 함수를 통해서 리소스를 Release하기 때문에, 
	// AddRef를 한번 더 해주는 처리를 하고 있고, 아니라면 따로 하지 않는다.


	// 카메라의 좌표를 설정하는데 사용하는 값. 
	Position m_tCameraBegin;
	
	float	 m_fCameraSpeed;
	class CPlayer* m_pPlayer;

	Position			m_tStageSize; // 카메라에 사용하는 값으로 스테이지의 사이즈를 알고 있어야.
	// 플레이어의 위치를 기반으로한 카메라의 좌표가 x,y 각각 0의 값보다 작은지, 혹은 size값보다 큰지를 명확하게
	// 구해낼 수 있는 것이다.

	Position			m_tPivot; // 플레이어를 출력할 스크린의 상대적인 비율이다.


	bool	m_bLimitCamera;
	Size	m_tLimitSize;

	bool	m_bSwayCamera;
	float	m_fSwayTime;
	float	m_fSwayPlayTime;

	class CStage* m_pStage;

	list<class CObj*> m_PrevSceneObject;
	float m_fSwayScale;

	// Screen의 Pos
	vector<ClipCoord> m_vecForeGroundPos;
	vector<ClipCoord> m_vecBackGroundPos;

private:
	// BackGround와 ForeGround의 위치를 읽어오는 함수를 만든다.
	void LoadScreenPos(const char* pFileName);

public:

	ClipCoord GetForeGroundPos(unsigned int iIndex);
	ClipCoord GetBackGroundPos(unsigned int iIndex);

	vector<ClipCoord>& GetForeGroundVec();
	vector<ClipCoord>& GetBackGroundVec();
	



	void LayerUp(const string& strLayerName, const string& strTag);
	class CStage* GetStage() const
	{
		return m_pStage;
	}

	// 남은 SwayTime을 얻어오는 함수이다.
	float GetLeftSwayTime()
	{
		if (!m_bSwayCamera)
			return 0.f;

		// 카메라가 흔들리는 중이라면 남은 시간을 구해준다.

		return m_fSwayPlayTime - m_fSwayTime;
	}

public:

	void SetSway(float fSwayPlayTime, float fScale = 2.f)
	{
		// 이미 흔들고 있는 상황에서 해당 함수가 들어온다면 무시한다.

		m_fSwayPlayTime = fSwayPlayTime;
		m_fSwayTime = 0.f;
		m_bSwayCamera = true;
		m_fSwayScale = fScale;
	}

	void SwayDisEnable()
	{
		m_bSwayCamera = false;
	}

public:

	void PrevObjectAdd();

	void SetStage();

	class CScene* GetCurrentScene() const
	{
		return m_pScene;
	}


	void SetLimitCamera(Size tSize)
	{
		m_tLimitSize = tSize;

		m_bLimitCamera = true;
	}
	
	void LimitCameraDisEnable()
	{
		m_bLimitCamera = false;
	}

	void SetLimitCamera(int x, int y)
	{
		m_tLimitSize = Size(x, y);

		m_bLimitCamera = true;
	}

	void SetTargetPivot(float x, float y)
	{
		m_tPivot = Position(x, y);
	}

	void SetStageSize(int x, int y)
	{
		m_tStageSize = Size(x, y);
	}

	void SetStageSize(const Position& tSize)
	{
		m_tStageSize = tSize;
	}

	Size GetStageSize() const
	{
		return m_tStageSize;
	}

	Position GetCameraPos() const
	{
		return m_tCameraBegin;
	}

	void SetCameraPos(float x, float y)
	{
		m_tCameraBegin = Position(x, y);
	}

	void SetPlayer();

public:
	// CreateScene변수를 셋팅하는 함수
	void SetCreateScene(bool bScene)
	{
		m_bCreateScene = bScene;
	}

	// 장면생성의 활성화에 대한 정보를 return 하는 함수이다.
	bool CreateSceneEnable() const
	{
		return m_bCreateScene;
	}


	bool GetSway() const
	{
		return m_bSwayCamera;
	}

public:
	bool Init();								// SceneManager가 관리하는 LayerList에 새로운 Layer를 추가하는 역할을한다.
												// 기본 장면을 생성하고 있다.
	int Update(float fTime);
	int LateUpdate(float fTime);
	int Collision(float fTime);
	void PrevRender(HDC hDC, float fTime);
	void Render(HDC hDC, float fTime);
	void PostRender(HDC hDC, float fTime);
	void CameraUpdate(float fTime); // 인게임에서도 사용하는 함수로, 카메라의 좌표를 옮기는 함수이다.
	// 다만, edit모드에서의 update와 play모드에서의 업데이트에서 차이가 있다.

	// SceneManager를 호출하여 현재 Scene의 Update, LateUpdate 함수등을 호출하며,
	// 점점 하향식으로 내려가는 설계이다.

	// SceneManager를 통해서 현재 Scene에 접근하고,
	// 해당 장면의 오브젝트를 찾아오는 함수를 만든다.
	class CObj* FindObject(const string strName);
	class CObj* FindNextObject(const string& strName);

	class CObj* FindNextSceneObject(const string& strName);

	// Mike Attack
	void MikeAttackCollision();

	// Scene Change
	void ObjectSave();

	// GravityTime Clear
	void GravityTimeClear(const string& strLayerName);


public:
	void AddLayer(const string& strName, int iZOrder = 0); // Layer를 추가하는 함수이다.
	void SetLayerZOrder(const string& strName, int iZOrder = 0); // strName의 이름을 가진, Layer를 찾아서 해당 Layer의 
	// ZOrder 값을 바꿔주는 함수이다.
	class CLayer* FindLayer(const string& strName); // strName을 가진 Layer를 반환하는 역할을 한다.

public:
	template <typename T>
	bool CreateScene(const string & strName, bool bCurrent = false)
	{
		// strName을 받는 이유는 Ref의 상속을 받기에 Tag를 설정해주기 위함이다.
		// 템플릿함수를 사용하는 이유는 Scene을 상속받는 가변적인 타입의 클래스를 생성하기 때문이다.

		T* pScene = new T;

		m_bCreateScene = true;

		if (!pScene->Init())
		{
			SAFE_DELETE(pScene); // Scene은 SceneManager에 friend 처리가 되어 있다. 따라서 직접적인 삭제가 문제가 되지 않는다.
			// 그에 비해 Release함수의 경우에는 직접적으로 삭제를 하는 것이 아니다.
			// public 함수를 이용해서 해당 객체의 ref값만 감소시키는 역할이다.
			// 이것은 private 소멸자의 관계와는 상관이 없다.
			// 왜냐면 ref == 0일 때 delete가 일어나는 것은 객체의 내부에서 일어나기 때문이다.
			// 즉, 외부에서 삭제하는 것과는 전혀 상관이 없다.
			// 아무튼 여기서는 delete를 해줬다.
			return false;
		}

		// 제대로 초기화가 이루어진 경우에는 정보를 셋팅한다.

		pScene->SetTag(strName);
		pScene->SetLayerList(&m_LayerList); // LayerList의 주소를 넣어주면 해당 Layer를 순회하면서 
		// 그 Layer의 소속 Scene을 호출한 객체로(생성한 장면으로) 주는 함수이다.

		if (bCurrent)
		{
			SAFE_DELETE(m_pScene);
			m_pScene = pScene;
		}
		else
		{
			SAFE_DELETE(m_pNextScene);
			m_pNextScene = pScene;
		}

		m_bCreateScene = false;

		return true;
	}

private:
	static bool SortLayer(const CLayer* pL1, const CLayer* pL2);
	// Layer를 정렬하기 위한 함수이다.
	// 

	SCENE_CHANGE ChangeScene();
};

