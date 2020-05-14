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
	// ����Ǵ� ��ǻ�͸� �������� 1�ʿ� ��� ��ŭ�� Count�ϴ��� ���Ѵ�(�ֱ�)
	QueryPerformanceFrequency(&m_tSecond);
	
	// ó�� �ð��� ƽ�� �޾Ƶд�.
	QueryPerformanceCounter(&m_tTime);

	m_fDeltaTime = 0.f;
	m_fFPSTime = 0.f;
	m_iFrameCount = 0;

	m_iLimitFrame = 60;

	// 1������ �� ó���� �ð��� ���Ѵ�.
	m_fLimitFramePlayTime = 1.f / (float)m_iLimitFrame;

	return true;
}

float CTimer::Update()
{

	// ���� ƽ�� ���� ƽ�� ���̸� ���ؼ� ��� ��ŭ�� ��ġ��ŭ �������� ���Ѵ�.
	LARGE_INTEGER tTime;
	QueryPerformanceCounter(&tTime);

	// ���� ���������κ��� ������ �ð��� ���Ѵ�.
	// ���� : ���� ƽ - ���� ƽ�� ���ؼ� ���� ���� 1���̰�,
	// 1�ʿ� �������� ƽ�� 60���̶�� ...
	// ���� �����Ӱ��� ���ݵ��� 1/60 �ʸ� ���� ���̴�.

	m_fDeltaTime = (tTime.QuadPart - m_tTime.QuadPart) / (float)m_tSecond.QuadPart;

	// ������ ƽ�� �����Ѵ�.
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
			// 1�������� �ð��� ���� ���.

			m_fLimitFrameTime -= m_fLimitFramePlayTime;

			m_fDeltaTime = m_fLimitFramePlayTime;

			++m_iFrameCount;
		}
		else
		{
			// �� �������� �ð��� ������ ���� ���.
			m_fDeltaTime = 0.f;
		}

		

	}
	else
	{
		// FPS�� ���Ѵ�.
		++m_iFrameCount;
	}



	if (m_fFPSTime >= 1.f)
	{
		// 1.2�� -> 100��
		// 1�� -> 100 / 1.2 -> fps

		float fFPS = m_iFrameCount / m_fFPSTime;

		// ���� FPS �� ������ش�.

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
