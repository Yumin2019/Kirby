#include "GameManager.h"
#include "PathManager.h"
#include "ResourceManager.h"
#include "SceneManager.h"
#include "../Collision/CollisionManager.h"
#include "../Core/EditManager.h"
#include "../Timer.h"
#include "Input.h"
#include "../Texture.h"
#include "../Object/Stage.h"
#include "../Sound/SoundManager.h"

DEFINITION_SINGLE(CGameManager);


bool CGameManager::m_bLoop = true;

CGameManager::CGameManager() :
	m_pTimer(nullptr),
	m_fTimeScale(1.f),
	m_pBackBuffer(nullptr),
	m_eMode(GM_START)
{
#ifdef _DEBUG
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	_CrtSetBreakAlloc(112);

#endif
}


CGameManager::~CGameManager()
{
	DESTROY_SINGLE(CEditManager); // 1 1 1s
	
	
	DESTROY_SINGLE(CSceneManager);

	DESTROY_SINGLE(CInput);

	GET_SINGLE(CCollisionManager)->Clear();
	// �浹�����ڰ� ������� ������ �浹ü��, �������, ä��, �������� ���� ���ش�.

	DESTROY_SINGLE(CCollisionManager);
		// ���Ŀ� �浹�����ڸ� ����.

	DESTROY_SINGLE(CSoundManager);


	SAFE_RELEASE(m_pBackBuffer);
	DESTROY_SINGLE(CResourceManager);
	DESTROY_SINGLE(CPathManager);


	


	// Ÿ�̸Ӹ� ���ش�.
	SAFE_DELETE(m_pTimer);



	// Brush �� ���ش�.
	DeleteObject(m_hGreenBrush);
	DeleteObject(m_hRedBrush);
	DeleteObject(m_hYellowBrush);
	DeleteObject(m_hBlackBrush);
	DeleteObject(m_hWhiteBrush);
	DeleteObject(m_hMagenta);

	// Pen �� ���ش�.
	DeleteObject(m_hRedPen);
	DeleteObject(m_hGreenPen);
	DeleteObject(m_hPurplePen);
	DeleteObject(m_hWhitePen);


	// ȭ�� DC�� ��ȯ�Ѵ�.
	ReleaseDC(m_hWnd, m_hDC);
}


void CGameManager::TimeStop(float fTime)
{
	m_pTimer->TimeStop(fTime);
}

float CGameManager::GetDeltaTime() const
{
	return m_pTimer->m_fDeltaTime;
}

void CGameManager::AddInput()
{
	// CInput Ŭ������ �̿��ؼ�, Ű�� �߰��ϴ� �Լ��̴�.
	// Ű�� �߰��ϴ� �������� �̸� BindActionInfo�� BindAxisInfo  ������ �߰��ϰ� �ȴ�.

	// ��մ� ����, �Ϲ�Ű�� ���ؼ��� ���� ������ ó���� �ϰ� ���� �ʾƼ�
	// a shift ����ϸ�, a shift b Ű�� ������, bŰ�� ���� ó���� ���� �ʴ´�.
	// ����, a shift�� �ش��ϴ� �Լ��� ȣ��Ǵ� ���̴�.



	// Player Move Key
	GET_SINGLE(CInput)->AddNormalKey("MoveLeft", VK_LEFT);
	GET_SINGLE(CInput)->AddNormalKey("MoveRight", VK_RIGHT);
	GET_SINGLE(CInput)->AddNormalKey("MoveUp", VK_UP);
	GET_SINGLE(CInput)->AddNormalKey("MoveDown", VK_DOWN);

	// MouseKey
	GET_SINGLE(CInput)->AddNormalKey("MouseLButton", VK_LBUTTON);
	GET_SINGLE(CInput)->AddNormalKey("MouseRButton", VK_RBUTTON);



	// ����
	GET_SINGLE(CInput)->AddNormalKey("Pause", VK_LCONTROL, 'P');




	
}

void CGameManager::SetTimeScale(float fScale)
{
	m_fTimeScale = fScale;
}

bool CGameManager::Init(HINSTANCE hInst)
{
	m_hInst = hInst;

	MyRegisterClass(TEXT("Kirby"));

	WindowCreate(TEXT("Kirby"), TEXT("Kirby"));

	// Brush�� �����Ѵ�
	m_hGreenBrush = CreateSolidBrush(RGB(0, 255, 0));
	m_hRedBrush = CreateSolidBrush(RGB(255, 0, 0));
	m_hYellowBrush = CreateSolidBrush(RGB(255, 255, 0));
	m_hBlackBrush = CreateSolidBrush(RGB(0, 0, 0));
	m_hWhiteBrush = CreateSolidBrush(RGB(255, 255, 255));
	m_hMagenta = CreateSolidBrush(RGB(255, 0, 255));

	// Pen�� �����Ѵ�.
	m_hRedPen = CreatePen(PS_SOLID, 3, RGB(255, 0, 0));
	m_hGreenPen = CreatePen(PS_SOLID, 3, RGB(0, 255, 0));
	m_hPurplePen = CreatePen(PS_SOLID, 3, RGB(128, 0, 255));
	m_hWhitePen = CreatePen(PS_SOLID, 3, RGB(255, 255, 255));

	// ȭ�� DC�� �����Ѵ�
	m_hDC = GetDC(m_hWnd);

	// Ÿ�̸Ӹ� �����Ѵ�.
	m_pTimer = new CTimer;

	m_pTimer->Init();

	// ���� ���� false
	m_bPause = false; 



	// ��� ������ �ʱ�ȭ
	if (!GET_SINGLE(CPathManager)->Init())
		return false;

	// �ڿ� ������ �ʱ�ȭ
	if (!GET_SINGLE(CResourceManager)->Init())
		return false;

	// �Ҹ� ������ �ʱ�ȭ
	if (!GET_SINGLE(CSoundManager)->Init())
		return false;

	// �浹 ������ �ʱ�ȭ
	if (!GET_SINGLE(CCollisionManager)->Init())
		return false;

	// �Է� ������ �ʱ�ȭ.
	if (!GET_SINGLE(CInput)->Init(m_hWnd))
		return false;

	// Ű�� �߰��Ѵ�.
	AddInput();


	// ��� ������ �ʱ�ȭ
	if (!GET_SINGLE(CSceneManager)->Init())
		return false;



	// �̸� �����ص� ����۸� ���´�.
	m_pBackBuffer = GET_SINGLE(CResourceManager)->FindTexture("BackBuffer");
	// ref = 2

	/*
	
	�ִϸ��̼� �������� �����ϴ� ��쿡�� src�� �ش��ϴ� �ִϸ��̼��� ��ü ����ϰ�,
	�̰� ������ �������� ũ�⿡ �ش��ϴ� �簢���� �����δ�.
	�׷��ϱ�, src�� ��ǥ��� �״�� ����� �ϴ� ���̴�. src�� ���ó�� ����ϰ�, �� ������
	�������� �����̸鼭 ���¸� �� �� �ֵ��� �Ѵ�.

	������ ��ܿ� �ִϸ��̼��� ���� ��ġ���� src�� FrameSize�� ������ ����ϸ� � �ִϸ��̼��� ���̰� �Ǵ��� 
	�����ִ� ����� �����.

	�ִϸ��̼��� �ٲٴ� ���. �ش� src���� �ִϸ��̼��� src�� �̵�(ȭ�鿡�� �̰��� �ٷ� ���̵��� �����Ѵ�)

	���� ���۽� �ִϸ��̼� �������� �������� ����� â�� �����.
	
	*/

	ShowCursor(FALSE);


	return true;
}

int CGameManager::Run()
{
	
	MSG msg;

	// �⺻ �޽��� �����Դϴ�:
	while (m_bLoop)
	{
		if(PeekMessage(&msg, nullptr, NULL, NULL, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		else
		{
			// �������� ����Ÿ��
			Logic();
		}
	}

	return (int)msg.wParam;
}

bool CGameManager::WindowCreate(const TCHAR* pClassName, const TCHAR* pTitleName)
{
	m_hWnd = CreateWindow(pClassName, pTitleName, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, m_hInst, nullptr);

	if (!m_hWnd)
	{
		return FALSE;
	}

	// ������ ũ�⸦ �����Ѵ�. 
	m_tResolution.iWidth = RS_WIDTH;
	m_tResolution.iHeight = RS_HEIGHT;

	// ���� Ŭ���̾�Ʈ�� �ػ󵵿� ���߱� ���ؼ� �����ؾ��ϴ� ũ�⸦ �޾ƿ´�.
	RECT rect = { 0, 0, RS_WIDTH, RS_HEIGHT };

	AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, NULL);
	SetWindowPos(m_hWnd, HWND_TOPMOST, 0, 0, rect.right - rect.left, rect.bottom - rect.top,
		SWP_NOZORDER | SWP_NOMOVE);

	ShowWindow(m_hWnd, SW_SHOW);
	UpdateWindow(m_hWnd);

	return true;
}

ATOM CGameManager::MyRegisterClass(const TCHAR* pClassName)
{
	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = CGameManager::WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = m_hInst;
	wcex.hIcon = LoadIcon(m_hInst, MAKEINTRESOURCE(IDI_ICON1));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = pClassName;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_ICON1));

	return RegisterClassExW(&wcex);
}

LRESULT CGameManager::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_DESTROY:
		m_bLoop = false;
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

void CGameManager::Logic()
{

	// �ð��� ���´�.
	float fDeltaTime = m_pTimer->Update() * m_fTimeScale;


	//if (fDeltaTime == 0.f)
	//	return;

	GET_SINGLE(CSoundManager)->Update(fDeltaTime);

	


	Input(fDeltaTime);


	switch (m_eMode)
	{
	case GM_MAP_EDIT:

		// KeyInput
		if (KEYDOWN("Pause"))
		{
			m_bPause = !m_bPause;
		}

		if (m_bPause)
			return;


		if (Update(fDeltaTime) == SC_CHANGE)
			return;

		if (LateUpdate(fDeltaTime) == SC_CHANGE)
			return;


	

		break;

	case GM_PLAY:
		SetBkMode(m_pBackBuffer->GetDC(), TRANSPARENT);


		// KeyInput
		// Stage TileRender�� ������ ���ߴ� ó����, Ÿ�� �������� ���� �ϴ� ������ �Ѵ�. (PlayMode������)

		// ��� Ű�� ���� ó���� �Ѵ�.
		if (KEYDOWN("TileRender"))
		{
			CStage* pStage = (CStage*)GET_SINGLE(CSceneManager)->FindObject("Stage");

			pStage->SetTileRender(!pStage->GetTileRender());

			// �� Ű�� MainScene������ ȣ���� �Ǵµ�, �̰��� ���� Tile�� Render�� ���� �ʴ´�.

			SAFE_RELEASE(pStage);
		}

		if (m_bPause)
			break;

		// Pause ���¶��, Collision�� Render�� ���� ó���� �Ѵ�. 
		// �׷���, ������Ʈ ó���� ���� ������ Tile�浹 Ȥ��, LineCollision�� ���� �Լ��� ȣ���ϸ�
		// ���ڱ� �������� ����ϴ� ��Ȳ������ ������ ó����� �÷��ִ� ó���� ������ �ǰ� �־
		// ������ ��Ȳ�� ���� ���̴�.

	case GM_START:


		

		if (Update(fDeltaTime) == SC_CHANGE)
			return;

		if (LateUpdate(fDeltaTime) == SC_CHANGE)
			return;

		CameraUpdate(fDeltaTime);


	
		break;

	case GM_ANIMATION_EDIT:

		EditUpdate(fDeltaTime);

		break;

	}




	Collision(fDeltaTime);
	Render(fDeltaTime);







}

SCENE_CHANGE CGameManager::Update(float fTime)
{
	SCENE_CHANGE sc = (SCENE_CHANGE)GET_SINGLE(CSceneManager)->Update(fTime);

	return sc;
}

SCENE_CHANGE CGameManager::LateUpdate(float fTime)
{
	SCENE_CHANGE sc = (SCENE_CHANGE)GET_SINGLE(CSceneManager)->LateUpdate(fTime);

	return sc;

}

int CGameManager::Collision(float fTime)
{
	// SceneManager�� Collision�� ���ؼ� ���� �ϴ� ó���� ����.
	// ȣ������ �ʰ�, �������� ó���� ���� CollisionManager�� Collision�Լ��� ȣ�����ش�.

	switch (m_eMode)
	{
	case GM_PLAY:
	case GM_START:
	case GM_MAP_EDIT: // ���콺�� �浹.
		GET_SINGLE(CCollisionManager)->Collision(fTime);

		break;

	case GM_ANIMATION_EDIT:
		GET_SINGLE(CEditManager)->Collision(fTime);

		break;
	}

	return 0;

}

void CGameManager::Render(float fTime)
{
	Rectangle(m_pBackBuffer->GetDC(), 0, 0, m_tResolution.iWidth, m_tResolution.iHeight);
	

	switch (m_eMode)
	{
	case GM_PLAY:
	case GM_START:
	case GM_MAP_EDIT:

		GET_SINGLE(CSceneManager)->PrevRender(m_pBackBuffer->GetDC(), fTime);
		GET_SINGLE(CSceneManager)->Render(m_pBackBuffer->GetDC(), fTime);
		GET_SINGLE(CSceneManager)->PostRender(m_pBackBuffer->GetDC(), fTime);

		break;

	case GM_ANIMATION_EDIT:

		GET_SINGLE(CEditManager)->Render(m_pBackBuffer->GetDC(), fTime);

		break;
	}

	// ���콺 Render
	GET_SINGLE(CInput)->Render(m_pBackBuffer->GetDC(), fTime);

	// ����ۿ��� �׸� �׸��� ���� ȭ��DC�� �����Ѵ�.

	m_pBackBuffer->Render(m_hDC, 0, 0, 0, 0, 
		m_tResolution.iWidth, m_tResolution.iHeight);
	// ������� �׸��� ��üȭ��ũ��� ȭ�� DC�� �׷��ش�.
}

void CGameManager::CameraUpdate(float fTime)
{
	GET_SINGLE(CSceneManager)->CameraUpdate(fTime);
}

void CGameManager::EditUpdate(float fTime)
{
	GET_SINGLE(CEditManager)->Update(fTime);
}

void CGameManager::Input(float fTime)
{
	GET_SINGLE(CInput)->Update(fTime);
}

