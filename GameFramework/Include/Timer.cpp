#include "Timer.h"
#include "Core/GameManager.h"


CTimer::CTimer()
{


	m_iLimitFrame = 120;
	m_bLimitFrame = false;
	m_fLimitFrameTime = 0.f;
	m_fLimitFramePlayTime = 0.f;
}


CTimer::~CTimer()
{
}

bool CTimer::Init()
{
	// 실행되는 컴퓨터를 기준으로 1초에 어느 만큼을 Count하는지 구한다(주기)
	QueryPerformanceFrequency(&m_tSecond);
	
	// 처음 시간의 틱을 받아둔다.
	QueryPerformanceCounter(&m_tTime);

	m_fDeltaTime = 0.f;
	m_fFPSTime = 0.f;
	m_iFrameCount = 0;

	m_iLimitFrame = 60;

	// 1프레임 당 처리할 시간을 구한다.
	m_fLimitFramePlayTime = 1.f / (float)m_iLimitFrame;

	return true;
}

float CTimer::Update()
{

	// 현재 틱과 이전 틱의 차이를 구해서 어느 만큼의 수치만큼 지났는지 구한다.
	LARGE_INTEGER tTime;
	QueryPerformanceCounter(&tTime);

	// 이전 프레임으로부터 지나간 시간을 구한다.
	// 공식 : 현재 틱 - 이전 틱을 통해서 구한 값이 1만이고,
	// 1초에 지나가는 틱이 60만이라면 ...
	// 이전 프레임과의 간격동안 1/60 초를 지난 것이다.

	m_fDeltaTime = (tTime.QuadPart - m_tTime.QuadPart) / (float)m_tSecond.QuadPart;

	// 현재의 틱을 갱신한다.
	m_tTime = tTime;

	m_fFPSTime += m_fDeltaTime;

	if (m_bTimeStop)
	{
		m_fStopTime += m_fDeltaTime;
		if (m_fStopTime >= m_fStopPlayTime)
		{
			m_bTimeStop = false;
		}

		return 0.f;
	}


	if (m_bLimitFrame)
	{
		m_fLimitFrameTime += m_fDeltaTime;
		
		if (m_fLimitFrameTime >= m_fLimitFramePlayTime)
		{
			// 1프레임의 시간이 넘은 경우.

			m_fLimitFrameTime -= m_fLimitFramePlayTime;

			m_fDeltaTime = m_fLimitFramePlayTime;

			++m_iFrameCount;
		}
		else
		{
			// 한 프레임의 시간이 지나지 않은 경우.
			m_fDeltaTime = 0.f;
		}

		

	}
	else
	{
		// FPS를 구한다.
		++m_iFrameCount;
	}



	if (m_fFPSTime >= 1.f)
	{
		// 1.2초 -> 100번
		// 1초 -> 100 / 1.2 -> fps

		float fFPS = m_iFrameCount / m_fFPSTime;

		// 구한 FPS 를 출력해준다.

#ifdef _DEBUG
		char strFPS[64] = {};

		sprintf_s(strFPS, "FPS : %d", (int)fFPS);

		SetWindowTextA(WINDOWHANDLE, strFPS);

#endif // _DEBUG

		m_fFPSTime -= 1.f;
		m_iFrameCount = 0;

	}


	return m_fDeltaTime;

}
