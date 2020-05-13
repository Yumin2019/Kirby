#include "ResourceManager.h"
#include "GameManager.h"
#include "SceneManager.h"
#include "../Texture.h"

DEFINITION_SINGLE(CResourceManager);


CResourceManager::CResourceManager()
{
}


CResourceManager::~CResourceManager()
{
	Safe_Release_Map(m_mapTexture);
	// 모든 관계가 끈어진 이후에 남은 텍스처를 정리한다.
}

bool CResourceManager::Init()
{
	m_hInst = WINDOWINSTANCE;
	m_hWnd = WINDOWHANDLE;
	m_hDC = WINDOWDC;

	LoadTexture("BackBuffer", TEXT("BackBuffer.bmp"));
	// 여기서 BackBuffer를 얻어오고 있다.
	// BackBuffer는 map에 들어가 있고 ref = 1이다.

	return true;
}

bool CResourceManager::LoadTexture(const string & strName, 
	const TCHAR * pFileName, 
	const string & strPathName)
{
	CTexture* pTexture = FindTexture(strName);

	if (pTexture)
	{

		// 이미 strName을 가진 텍스처가 있다는 의미이다.
		// 이미지를 로드하지 않는다.
		SAFE_RELEASE(pTexture);
		// 레퍼런스 카운터를 다시 돌려둔다.
		return false;
	}

	// 해당 이름을 가진 Texture가 존재하지 않는다.
	// 새로 생성을 해준다.

	pTexture = new CTexture;

	if (!pTexture->LoadTexture(strName, pFileName))
	{
		// Texture클래스는 내부적으로 PathManager를 통해서 이미지를 읽어올 경로를 만들고,
		// 읽어온 정보를 Texture에 저장한다.

		// 읽어온 Texture가 문제가 있을 경우..
		SAFE_RELEASE(pTexture); // = SAFE_DELETE
		return false;
	}

	if (GET_SINGLE(CSceneManager)->CreateSceneEnable())
	{
		// Scene을 생성하고 있는 경우에 해당 LoadTexture함수가 호출이 된 경우에는
		// Scene의 Start함수를 통해서 map에 있는 Texture정보를 release (정확히는 ClearScene) 해주고 있다
		// 따라서, 미리 증가시켜 둔다.
		pTexture->AddRef();
	}

	// Texture의 LoadTexture함수를 통해서 직접적인 호출을 해서, 텍스처의 정보를 읽어올 수도 있지만, 
	// 그러면, 따로 ResourceManager에 넣어야 한다는 번거로움이 있다.

	// 따라서, 이런 일련의 과정을 ResourceManager에서 처리하고자 하는 것이다.
	// LoadTexture에서 필요한 정보를 ResourceManager의 LoadTexture함수에 넣어줌으로써
	// Texture의 함수에 간접적인 접근을 할 수 있도록 하고, ResourceManager에서 해당 텍스처를 넣어주는 과정까지 한번에 처리하는 것이다.

	// 제대로 Texture가 생성이 된 경우에는, strName값을 키로 갖는 텍스처 클래스를  map에 추가한다.

	m_mapTexture.insert(make_pair(strName, pTexture));

	// 여기는 새로 생성이 되는 Texture만 들어오기 때문에 (이름상)
	// ref = 1인 상태이다. 즉, m_mapTexture에 있다.

	return true;
}

bool CResourceManager::LoadTexture(const string & strName,
	const vector<const TCHAR*>& vecFileName, const string & strPathName)
{
	// 여러개의 DC를 가진 텍스처를 추가하는 함수이다.

	CTexture* pTexture = FindTexture(strName);

	if (pTexture)
	{
		SAFE_RELEASE(pTexture);
		return false;
	}

	// 이미 같은 이름을 가진 텍스처가 있는 경우.

	pTexture = new CTexture;

	if (!pTexture->LoadTexture(strName, vecFileName, strPathName))
	{
		// 실패했지만, 할당된 텍스처이미지.
		SAFE_RELEASE(pTexture);
		return false;
	}

	// vecFileName을 받는 LoadTexture함수를 사용하여, 여려개의 DC를 가진 텍스처를 만든다.

	if (GET_SINGLE(CSceneManager)->CreateSceneEnable())
		pTexture->AddRef();

	
	m_mapTexture.insert(make_pair(strName, pTexture));
	// 텍스처 맵에 추가한다.

	return true;
}

void CResourceManager::ClearScene()
{
	FindTexture("BackBuffer");
	FindTexture("MouseDefault");


	// BackBuffer는 계속  ref값을 유지시켜야 한다.
	// 증가 후 감소

	unordered_map<string, CTexture*>::iterator iter;
	unordered_map<string, CTexture*>::iterator iterEnd = m_mapTexture.end();

	for (iter = m_mapTexture.begin(); iter != iterEnd;)
	{
		if (iter->second->Release() == 0)
		{
			// map에 있는 모든 정보를 release하니 해당 텍스처가 소멸이 되었다면. 지운다.
			iter = m_mapTexture.erase(iter);
			iterEnd = m_mapTexture.end();
			continue;
		}

		++iter;
		// 지워지지 않은 경우에는 다음 .
	}
}

void CResourceManager::SavePrototype()
{

	// 프로토타입이 사라지는 것을 방지하기 위한 함수이다.
	// 프로토타입으로 사용할 객체의 리소스는 이곳에서 참조를 해줘야 한다.

	FindTexture("BlueBall");
	FindTexture("AbilityMonsterLeft");
	FindTexture("AbilityMonsterRight");
	FindTexture("KirbyRunEffectLeft");
	FindTexture("KirbyRunEffectRight");
	FindTexture("NormalMonsterLeft");
	FindTexture("NormalMonsterRight");

	FindTexture("KirbyAttackEffectLeft");
	FindTexture("KirbyAttackEffectRight");
	FindTexture("KirbyBreathAttackEffectLeft");
	FindTexture("KirbyBreathAttackEffectRight");
	FindTexture("KirbyStarEffectLeft");
	FindTexture("KirbyStarEffectRight");

	FindTexture("SmallStar");
	FindTexture("StoneKirbyLeft");
	FindTexture("StoneKirbyRight");
	FindTexture("SwordKirbyLeft");
	FindTexture("SwordKirbyRight");
	FindTexture("Door");
	FindTexture("KirbyLeft");
	FindTexture("KirbyRight");
	FindTexture("EasyKirbyLeft");
	FindTexture("EasyKirbyRight");
	FindTexture("MikeObject");
	FindTexture("AbilityName");
	FindTexture("HPHud");
	FindTexture("Star");

	// 프로토타입은 아니지만 계속 써야 하는 오브젝트
	FindTexture("BossHP");
	FindTexture("BossHPBorder");
	FindTexture("BackGround");
	FindTexture("ForeGround"); // 계속 쓰는 물체들.
}


CTexture * CResourceManager::FindTexture(const string & strName)
{
	unordered_map<string, CTexture*>::iterator iter = m_mapTexture.find(strName);

	if (iter == m_mapTexture.end())
		return nullptr;

	iter->second->AddRef();
	// 참조

	return iter->second;
}
