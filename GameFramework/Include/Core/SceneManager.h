#pragma once

#include "../Game.h"

// ���� �ٸ� Ŭ�������� ���� ���� ������ ���� ���� �κ��� ����.

class CSceneManager
{
	DECLARE_SINGLE(CSceneManager);

private:
	class CScene*				m_pScene;		// ���� Scene
	class CScene*				m_pNextScene;	// ���� Scene
	list<class CLayer*>			m_LayerList;	// �������� Layer�� SceneManager���� �����Ѵ�.
	bool						m_bCreateScene;	// Ref�� �����ؼ� ����� �����̴�.
	// ����� �߰� �Ǵ� ���� ��쿡�� Start �Լ��� ���ؼ� ���ҽ��� Release�ϱ� ������, 
	// AddRef�� �ѹ� �� ���ִ� ó���� �ϰ� �ְ�, �ƴ϶�� ���� ���� �ʴ´�.


	// ī�޶��� ��ǥ�� �����ϴµ� ����ϴ� ��. 
	Position m_tCameraBegin;
	
	float	 m_fCameraSpeed;
	class CPlayer* m_pPlayer;

	Position			m_tStageSize; // ī�޶� ����ϴ� ������ ���������� ����� �˰� �־��.
	// �÷��̾��� ��ġ�� ��������� ī�޶��� ��ǥ�� x,y ���� 0�� ������ ������, Ȥ�� size������ ū���� ��Ȯ�ϰ�
	// ���س� �� �ִ� ���̴�.

	Position			m_tPivot; // �÷��̾ ����� ��ũ���� ������� �����̴�.


	bool	m_bLimitCamera;
	Size	m_tLimitSize;

	bool	m_bSwayCamera;
	float	m_fSwayTime;
	float	m_fSwayPlayTime;

	class CStage* m_pStage;

	list<class CObj*> m_PrevSceneObject;
	float m_fSwayScale;

	// Screen�� Pos
	vector<ClipCoord> m_vecForeGroundPos;
	vector<ClipCoord> m_vecBackGroundPos;

private:
	// BackGround�� ForeGround�� ��ġ�� �о���� �Լ��� �����.
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

	// ���� SwayTime�� ������ �Լ��̴�.
	float GetLeftSwayTime()
	{
		if (!m_bSwayCamera)
			return 0.f;

		// ī�޶� ��鸮�� ���̶�� ���� �ð��� �����ش�.

		return m_fSwayPlayTime - m_fSwayTime;
	}

public:

	void SetSway(float fSwayPlayTime, float fScale = 2.f)
	{
		// �̹� ���� �ִ� ��Ȳ���� �ش� �Լ��� ���´ٸ� �����Ѵ�.

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
	// CreateScene������ �����ϴ� �Լ�
	void SetCreateScene(bool bScene)
	{
		m_bCreateScene = bScene;
	}

	// �������� Ȱ��ȭ�� ���� ������ return �ϴ� �Լ��̴�.
	bool CreateSceneEnable() const
	{
		return m_bCreateScene;
	}


	bool GetSway() const
	{
		return m_bSwayCamera;
	}

public:
	bool Init();								// SceneManager�� �����ϴ� LayerList�� ���ο� Layer�� �߰��ϴ� �������Ѵ�.
												// �⺻ ����� �����ϰ� �ִ�.
	int Update(float fTime);
	int LateUpdate(float fTime);
	int Collision(float fTime);
	void PrevRender(HDC hDC, float fTime);
	void Render(HDC hDC, float fTime);
	void PostRender(HDC hDC, float fTime);
	void CameraUpdate(float fTime); // �ΰ��ӿ����� ����ϴ� �Լ���, ī�޶��� ��ǥ�� �ű�� �Լ��̴�.
	// �ٸ�, edit��忡���� update�� play��忡���� ������Ʈ���� ���̰� �ִ�.

	// SceneManager�� ȣ���Ͽ� ���� Scene�� Update, LateUpdate �Լ����� ȣ���ϸ�,
	// ���� ��������� �������� �����̴�.

	// SceneManager�� ���ؼ� ���� Scene�� �����ϰ�,
	// �ش� ����� ������Ʈ�� ã�ƿ��� �Լ��� �����.
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
	void AddLayer(const string& strName, int iZOrder = 0); // Layer�� �߰��ϴ� �Լ��̴�.
	void SetLayerZOrder(const string& strName, int iZOrder = 0); // strName�� �̸��� ����, Layer�� ã�Ƽ� �ش� Layer�� 
	// ZOrder ���� �ٲ��ִ� �Լ��̴�.
	class CLayer* FindLayer(const string& strName); // strName�� ���� Layer�� ��ȯ�ϴ� ������ �Ѵ�.

public:
	template <typename T>
	bool CreateScene(const string & strName, bool bCurrent = false)
	{
		// strName�� �޴� ������ Ref�� ����� �ޱ⿡ Tag�� �������ֱ� �����̴�.
		// ���ø��Լ��� ����ϴ� ������ Scene�� ��ӹ޴� �������� Ÿ���� Ŭ������ �����ϱ� �����̴�.

		T* pScene = new T;

		m_bCreateScene = true;

		if (!pScene->Init())
		{
			SAFE_DELETE(pScene); // Scene�� SceneManager�� friend ó���� �Ǿ� �ִ�. ���� �������� ������ ������ ���� �ʴ´�.
			// �׿� ���� Release�Լ��� ��쿡�� ���������� ������ �ϴ� ���� �ƴϴ�.
			// public �Լ��� �̿��ؼ� �ش� ��ü�� ref���� ���ҽ�Ű�� �����̴�.
			// �̰��� private �Ҹ����� ����ʹ� ����� ����.
			// �ֳĸ� ref == 0�� �� delete�� �Ͼ�� ���� ��ü�� ���ο��� �Ͼ�� �����̴�.
			// ��, �ܺο��� �����ϴ� �Ͱ��� ���� ����� ����.
			// �ƹ�ư ���⼭�� delete�� �����.
			return false;
		}

		// ����� �ʱ�ȭ�� �̷���� ��쿡�� ������ �����Ѵ�.

		pScene->SetTag(strName);
		pScene->SetLayerList(&m_LayerList); // LayerList�� �ּҸ� �־��ָ� �ش� Layer�� ��ȸ�ϸ鼭 
		// �� Layer�� �Ҽ� Scene�� ȣ���� ��ü��(������ �������) �ִ� �Լ��̴�.

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
	// Layer�� �����ϱ� ���� �Լ��̴�.
	// 

	SCENE_CHANGE ChangeScene();
};

