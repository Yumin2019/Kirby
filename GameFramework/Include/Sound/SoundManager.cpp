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
		iter->second->pSound->release(); //  ���������� Counting ����̶� release..
		SAFE_RELEASE(iter->second);
	}

	m_mapSound.clear();

	if (m_pSystem)
	{
		m_pSystem->close(); // �� ���� �ݰڴ�.
		m_pSystem->release(); // ī���� ����� ���ؼ� �� �̻� ������� �ʰڴٰ� ���ϴ� ��.
		// Fmod���� ���� ���带 �����ϴ� �ý����� �����Ѵ�.
	}
}

bool CSoundManager::Init()
{
	// System�� �����Ѵ�.
	System_Create(&m_pSystem);
	// ���� �����͸� �־, �̰��� �����Ҵ��� �Ͽ�, �ٽ� �Ѱ��ش�.

	// System�� �ʱ�ȭ�Ѵ�.
	m_pSystem->init(FMOD_MAX_CHANNEL_WIDTH, FMOD_INIT_NORMAL, nullptr);

	return true;
}

bool CSoundManager::LoadSound(const string & strName, bool bLoop, const char * pFileName,
	const string & strPathName)
{
	// �̹��� ������ ī��Ʈ�� 2�� �س��� ��� ��ȯ��
	// ���ϴ� �̹����鸸 ���������� ó���Ѵ�.
	PSoundInfo	pInfo = FindSound(strName);

	if (pInfo)
	{
		SAFE_RELEASE(pInfo);
		return false;

		// �ؽ�ó�� ������ ������� ���� �̸��� ���尡 �����Ѵٸ� ������ ���̰� false
	}

	pInfo = new SoundInfo;
	// ���ٸ� �Ҵ��� �Ͼ��.

	const char* pPath = GET_SINGLE(CPathManager)->FindPathMultibyte(strPathName);
	// Path�̸��� �������� MultiByte�� �޾ƿ´�.

	string	strFullPath;

	if (pPath)
		strFullPath = pPath;

	// �޾ƿ� ���� �ִٸ� �⺻���� �����س���

	strFullPath += pFileName;
	// �����̸��� �ٿ��� ���� ��θ� ���Ѵ�.

	FMOD_MODE	eMode = FMOD_DEFAULT;
	// �⺻ ��� :1��.

	if (bLoop)
		eMode = FMOD_LOOP_NORMAL;
	// ������� ���� �ɼ��� �ش�.

	FMOD_RESULT result = m_pSystem->createSound(strFullPath.c_str(), eMode, nullptr, &pInfo->pSound);

	if (FMOD_OK != result)
		int i = 0;
	// �ý����� �̿��ؼ� ���带 �����ϴ� �۾��̴�. 
	// strFullPath.c_str()�� �̿��ؼ� const char*  ���� �־��ְ� ���, ������ ������ ���� ���� �ּҸ� �־��ش�.
	// �̰��� �Ҵ��� �ϰڴٴ� �Ҹ�����.

	if (GET_SINGLE(CSceneManager)->CreateSceneEnable())
		pInfo->AddRef();
	// clearscene���� ������. �̸� ����.

	m_mapSound.insert(make_pair(strName, pInfo));
	// �־��� �̸����� Sound�� �����صд�.

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
	// �ش� �̸��� ���� �Ҹ��� ���� ��� return.

	Channel*	pChannel = nullptr;
	m_pSystem->playSound(pInfo->pSound, nullptr, bPause, &m_vecChannel[eType]);
	// Sound ����� ��û���ִ� ���̴�. �� ���� ������ ���带 ����ش޶�. ä�� �׷��� ������� �ʴ´ٴ� �Ҹ���,
	// �Ͻ����� �� �Ѵ�. 
	// ä���� 2�� �����͸� �Ѱ��༭, ���⿡ ä���� �ּҸ� �Ҵ��Ѵ�. ä���� ����ϵ��� ����� �Ѵ�.

	// �̸� ä�� vector(Effect, Bgm)�� ������ ���¿��� �װ��� ä���ּҸ� �־ ȣ���� ��û�Ѵ�.
	// ��, Effect ä�ΰ� Bgmä���� ���� �ִ� ��.

	SAFE_RELEASE(pInfo);

	// FindSound. �����Ͽ� ���带 ����ϰ� ������� �ʴ´�.
}

void CSoundManager::Stop(CHANNEL_TYPE eType)
{
	// Stop�� ä�δ����� �̷�� ���µ�, ���� ä���� ���ļ� ���带 ����ϰ� �ִٰ��� �� �������� �ϴ� ����̴�.

	if (m_vecChannel[eType])
		// �ش� flag�� �Ҵ���� ä���� �ִٸ�, �� ó���� ���ش�.
	{
		// �÷��� ���� ��쿡�� ó���� �ؾ� �Ѵ�.
		bool	bPlay = false;
		m_vecChannel[eType]->isPlaying(&bPlay);

		// �⺻������ false�� �ְ� playing ���¶�� true�� �� ���̴�.
		// �ش� ä�ο��� ���尡 ������̶��, stop�Լ��� ���ؼ� ä���� ��������.

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
	// SoundManager��  Update�� ��� ���ư��� �Ѵ�.
}

void CSoundManager::PrototypeSound()
{
	// ����� ������Ÿ�԰� ���� ��Ƽ� ���ư��� �ϴ� �Ҹ����� �ǹ��Ѵ�.
	// ClearScene�� �鿪�� �������� �Ѵ�.

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
	// ������ �Ͼ��.

	return iter->second;
}
