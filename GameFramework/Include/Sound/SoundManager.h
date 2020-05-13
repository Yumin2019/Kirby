#pragma once

#include "../Game.h"
#include "fmod.hpp"

using namespace FMOD;

// _WIN64 는 64비트용 개발을 할 경우 자동으로 define 되어 있다.
#ifdef _WIN64
#pragma comment(lib, "fmod_vc.lib") 
#else
#pragma comment(lib, "fmod_vc.lib")
#endif // _WIN64

typedef struct _tagSoundInfo
{
	Sound*	pSound;
	int		iRef;

	_tagSoundInfo() :
		pSound(nullptr),
		iRef(1)
	{
	}

	void AddRef()
	{
		++iRef;
	}

	int Release()
	{
		--iRef;

		if (iRef == 0)
		{
			delete	this;
			return 0;
		}

		return iRef;
	}
}SoundInfo, *PSoundInfo;

enum CHANNEL_TYPE
{
	CT_BGM,
	CT_EFFECT,
	CT_STATE,
	CT_ATTACK,
	CT_END
};

class CSoundManager
{
	DECLARE_SINGLE(CSoundManager);

private:
	System*		m_pSystem;
	unordered_map<string, PSoundInfo>	m_mapSound;
	vector<Channel*>		m_vecChannel;

public:
	bool Init();

	// 사운드의 Loop여부를 받아둔다.
	bool LoadSound(const string& strName, bool bLoop, const char* pFileName,
		const string& strPathName = SOUND_PATH);
	void ClearScene();
	void Play(const string& strName, CHANNEL_TYPE eType, bool bPause = false); // Play를 하는 상황에서 채널의 타입을 받도록 한다.
	void Stop(CHANNEL_TYPE eType);
	bool IsPlaying(CHANNEL_TYPE eType);
	void Update(float fTime);
	void PrototypeSound();

private:
	PSoundInfo FindSound(const string& strName);

};

