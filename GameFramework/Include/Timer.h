#pragma once

#include "Game.h"

// ³¡

class CTimer
{
	friend class CGameManager;

private:
	CTimer();
	~CTimer();

public:
	bool Init();
	float Update();
	

	void SetLimitFrame(int iLimitFrame)
	{
		m_bLimitFrame = true;
		m_iLimitFrame = iLimitFrame;
	}
	
	void TimeStop(float fTime)
	{
		m_bTimeStop = true;
		m_fStopTime = 0.f;
		m_fStopPlayTime = fTime;
	}

	bool GetTimeStop() const
	{
		return m_bTimeStop;
	}


private:
	LARGE_INTEGER m_tTime;
	LARGE_INTEGER m_tSecond;
	float		  m_fDeltaTime;
	float		  m_fFPSTime;
	int			  m_iFrameCount;

	int				m_iLimitFrame;
	bool			m_bLimitFrame;
	float			m_fLimitFrameTime;
	float			m_fLimitFramePlayTime;

	bool m_bTimeStop;
	float m_fStopTime;
	float m_fStopPlayTime;
};

