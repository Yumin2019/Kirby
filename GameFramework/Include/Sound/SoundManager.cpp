#include "SoundManager.h"
#include "../Core/PathManager.h"
#include "../Core/SceneManager.h"

DEFINITION_SINGLE(CSoundManager);

CSoundManager::CSoundManager()	:
	m_pSystem(nullptr)
{
	m_vecChannel.resize(CT_END);
}

CSoundManager::~CSoundManager()
{
	unordered_map<string, PSoundInfo>::iterator	iter;
	unordered_map<string, PSoundInfo>::iterator	iterEnd = m_mapSound.end();

	for (iter = m_mapSound.begin(); iter != iterEnd; ++iter)
	{
		iter->second->pSound->release(); //  내부적으로 Counting 방식이라서 release..
		SAFE_RELEASE(iter->second);
	}

	m_mapSound.clear();

	if (m_pSystem)
	{
		m_pSystem->close(); // 안 쓰고 닫겠다.
		m_pSystem->release(); // 카운팅 방식을 통해서 더 이상 사용하지 않겠다고 말하는 것.
		// Fmod에서 직접 사운드를 생성하는 시스템을 제공한다.
	}
}

bool CSoundManager::Init()
{
	// System을 생성한다.
	System_Create(&m_pSystem);
	// 이중 포인터를 넣어서, 이곳에 동적할당을 하여, 다시 넘겨준다.

	// System을 초기화한다.
	m_pSystem->init(FMOD_MAX_CHANNEL_WIDTH, FMOD_INIT_NORMAL, nullptr);

	return true;
}

bool CSoundManager::LoadSound(const string & strName, bool bLoop, const char * pFileName,
	const string & strPathName)
{
	// 이미지 생성시 카운트를 2로 해놓고 장면 전환시
	// 원하는 이미지들만 지워지도록 처리한다.
	PSoundInfo	pInfo = FindSound(strName);

	if (pInfo)
	{
		SAFE_RELEASE(pInfo);
		return false;

		// 텍스처와 동일한 방식으로 같은 이름의 사운드가 존재한다면 참조를 줄이고 false
	}

	pInfo = new SoundInfo;
	// 없다면 할당이 일어난다.

	const char* pPath = GET_SINGLE(CPathManager)->FindPathMultibyte(strPathName);
	// Path이름을 기준으로 MultiByte로 받아온다.

	string	strFullPath;

	if (pPath)
		strFullPath = pPath;

	// 받아온 것이 있다면 기본적을 데입해놓고

	strFullPath += pFileName;
	// 파일이름을 붙여서 최종 경로를 구한다.

	FMOD_MODE	eMode = FMOD_DEFAULT;
	// 기본 재생 :1번.

	if (bLoop)
		eMode = FMOD_LOOP_NORMAL;
	// 루프라면 루프 옵션을 준다.

	FMOD_RESULT result = m_pSystem->createSound(strFullPath.c_str(), eMode, nullptr, &pInfo->pSound);

	if (FMOD_OK != result)
		int i = 0;
	// 시스템을 이용해서 사운드를 생성하는 작업이다. 
	// strFullPath.c_str()을 이용해서 const char*  형을 넣어주고 모드, 사운드의 정보를 넣을 곳의 주소를 넣어준다.
	// 이곳에 할당을 하겠다는 소리겟지.

	if (GET_SINGLE(CSceneManager)->CreateSceneEnable())
		pInfo->AddRef();
	// clearscene에서 남도록. 미리 증가.

	m_mapSound.insert(make_pair(strName, pInfo));
	// 넣어준 이름으로 Sound를 생성해둔다.

	return true;
}

void CSoundManager::ClearScene()
{
	unordered_map<string, PSoundInfo>::iterator	iter;
	unordered_map<string, PSoundInfo>::iterator	iterEnd = m_mapSound.end();

	for (iter = m_mapSound.begin(); iter != iterEnd;)
	{
		if (iter->second->Release() == 0)
		{
			iter = m_mapSound.erase(iter);
			iterEnd = m_mapSound.end();
			continue;
		}

		++iter;
	}
}

void CSoundManager::Play(const string & strName, CHANNEL_TYPE eType, bool bPause)
{
	PSoundInfo	pInfo = FindSound(strName);

	if (!pInfo)
		return;
	// 해당 이름을 가진 소리가 없는 경우 return.

	Channel*	pChannel = nullptr;
	m_pSystem->playSound(pInfo->pSound, nullptr, bPause, &m_vecChannel[eType]);
	// Sound 재생을 요청해주는 것이다. 이 사운드 정보의 사운드를 재생해달라. 채널 그룹은 사용하지 않는다는 소리고,
	// 일시정지 안 한다. 
	// 채널의 2중 포인터를 넘겨줘서, 여기에 채널의 주소를 할당한다. 채널을 재생하도록 명령을 한다.

	// 미리 채널 vector(Effect, Bgm)을 만들어둔 상태에서 그것의 채널주소를 넣어서 호출을 요청한다.
	// 즉, Effect 채널과 Bgm채널이 따로 있는 것.

	SAFE_RELEASE(pInfo);

	// FindSound. 참조하여 사운드를 재생하고서 사용하지 않는다.
}

void CSoundManager::Stop(CHANNEL_TYPE eType)
{
	// Stop은 채널단위로 이루어 지는데, 여러 채널을 거쳐서 사운드를 재생하고 있다가도 다 끊어지게 하는 방식이다.

	if (m_vecChannel[eType])
		// 해당 flag로 할당받은 채널이 있다면, 이 처리를 해준다.
	{
		// 플레이 중인 경우에만 처리를 해야 한다.
		bool	bPlay = false;
		m_vecChannel[eType]->isPlaying(&bPlay);

		// 기본적으로 false를 주고 playing 상태라면 true가 될 것이다.
		// 해당 채널에서 사운드가 재생중이라면, stop함수를 통해서 채널을 꺼버린다.

		if(bPlay)
			m_vecChannel[eType]->stop();
	}
}

bool CSoundManager::IsPlaying(CHANNEL_TYPE eType)
{
	bool	bPlay = false;

	if (m_vecChannel[eType])
	{
		m_vecChannel[eType]->isPlaying(&bPlay);
	}

	return bPlay;
}

void CSoundManager::Update(float fTime)
{
	m_pSystem->update();
	// SoundManager의  Update는 계속 돌아가야 한다.
}

void CSoundManager::PrototypeSound()
{
	// 장면의 프로토타입과 같이 살아서 나아가야 하는 소리들을 의미한다.
	// ClearScene에 면역을 만들어줘야 한다.

	FindSound("AttackStarHit");
	FindSound("GoodSleep");
	FindSound("Door");
	FindSound("JumpStarBounce");
	FindSound("Pause");
	FindSound("AbilityGet");
	FindSound("Breath");
	FindSound("Die");
	FindSound("FlyUp");
	FindSound("Jump");
	FindSound("KirbyAttack");
	FindSound("KirbyHit");
	FindSound("RunStart");
	FindSound("Slide");
	FindSound("StarBullet");
	FindSound("Sing1");
	FindSound("Sing2");
	FindSound("Sing3");
	FindSound("MikeAttack");
	FindSound("MonsterHit1");
	FindSound("MonsterHit2");
	FindSound("MonsterHit3");
	FindSound("Stone1");

	FindSound("Attack1");
	FindSound("Attack3");
	FindSound("DashAttack");
	FindSound("JumpAttackHit");
	FindSound("JumpAttack");
	
}

PSoundInfo CSoundManager::FindSound(const string & strName)
{
	unordered_map<string, PSoundInfo>::iterator	iter = m_mapSound.find(strName);

	if (iter == m_mapSound.end())
		return nullptr;

	iter->second->AddRef();
	// 참조가 일어난다.

	return iter->second;
}
