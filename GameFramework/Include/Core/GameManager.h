#pragma once

#include "../Game.h"

class CGameManager
{
	DECLARE_SINGLE(CGameManager);

private:
	HWND		m_hWnd;
	HINSTANCE	m_hInst;
	HDC			m_hDC;
	Resolution	m_tResolution;
	class CTimer* m_pTimer;
	float		m_fTimeScale;
	HBRUSH		m_hGreenBrush;
	HBRUSH		m_hRedBrush;
	HBRUSH		m_hYellowBrush;
	HBRUSH		m_hBlackBrush;
	HBRUSH		m_hWhiteBrush;
	HBRUSH		m_hMagenta;
	
	HPEN		m_hWhitePen;
	HPEN		m_hRedPen;
	HPEN		m_hGreenPen;
	HPEN		m_hPurplePen;
	class CTexture* m_pBackBuffer;
	bool			m_bPause;

	GAME_MODE		m_eMode;

private:
	static bool		m_bLoop;

public:

	void TimeStop(float fTime);
	void Pause()
	{
		m_bPause = !m_bPause;
	}

	void SetPause(bool bPause)
	{
		m_bPause = bPause;

	}

	GAME_MODE GetGameMode() const
	{
		return m_eMode;
	}

	HWND GetWindowHandle() const
	{
		return m_hWnd;
	}

	HINSTANCE GetWindowInstance() const
	{
		return m_hInst;
	}

	HDC GetWindowDC() const
	{
		return m_hDC;
	}

	Resolution GetWindowResolution() const
	{
		return m_tResolution;
	}

	float GetTimeScale() const
	{
		return m_fTimeScale;
	}


	HBRUSH GetBlackBrush() const
	{
		return m_hBlackBrush;
	}

	HBRUSH GetWhiteBrush() const
	{
		return m_hWhiteBrush;
	}

	HBRUSH GetMagentaBrush() const
	{
		return m_hMagenta;
	}


	HBRUSH GetRedBrush() const
	{
		return m_hRedBrush;
	}

	HBRUSH GetGreenBrush() const
	{
		return m_hGreenBrush;
	}

	HBRUSH GetYellowBrush() const
	{
		return m_hYellowBrush;
	}

	HPEN GetRedPen() const
	{
		return m_hRedPen;
	}

	HPEN GetGreenPen() const
	{
		return m_hGreenPen;
	}

	HPEN GetPurplePen() const
	{
		return m_hPurplePen;
	}

	HPEN GetWhitePen() const
	{
		return m_hWhitePen;
	}


	float GetDeltaTime() const;

	bool GetPause() const
	{
		return m_bPause;
	}

	void SetGameMode(GAME_MODE eMode)
	{
		m_eMode = eMode;
	}

private:
	void AddInput();



public:
	void SetTimeScale(float fScale);


public:
	bool Init(HINSTANCE hInst);
	int Run();

private:
	bool WindowCreate(const TCHAR* pClassName, const TCHAR* pTitleName);
	ATOM MyRegisterClass(const TCHAR* pClassName);
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

private:
	void Logic();
	SCENE_CHANGE Update(float fTime);
	SCENE_CHANGE LateUpdate(float fTime);
	int Collision(float fTime);
	void Render(float fTime);
	void CameraUpdate(float fTime);
	void EditUpdate(float fTime);
	void Input(float fTime);
};

