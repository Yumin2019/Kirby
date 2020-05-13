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
	// 충돌관리자가 사라지기 이전에 충돌체와, 인접행렬, 채널, 프로파일 등을 없앤다.

	DESTROY_SINGLE(CCollisionManager);
		// 이후에 충돌관리자를 삭제.

	DESTROY_SINGLE(CSoundManager);


	SAFE_RELEASE(m_pBackBuffer);
	DESTROY_SINGLE(CResourceManager);
	DESTROY_SINGLE(CPathManager);


	


	// 타이머를 없앤다.
	SAFE_DELETE(m_pTimer);



	// Brush 를 없앤다.
	DeleteObject(m_hGreenBrush);
	DeleteObject(m_hRedBrush);
	DeleteObject(m_hYellowBrush);
	DeleteObject(m_hBlackBrush);
	DeleteObject(m_hWhiteBrush);
	DeleteObject(m_hMagenta);

	// Pen 을 없앤다.
	DeleteObject(m_hRedPen);
	DeleteObject(m_hGreenPen);
	DeleteObject(m_hPurplePen);
	DeleteObject(m_hWhitePen);


	// 화면 DC를 반환한다.
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
	// CInput 클래스를 이용해서, 키를 추가하는 함수이다.
	// 키를 추가하는 과정에서 미리 BindActionInfo와 BindAxisInfo  정보를 추가하게 된다.

	// 재밌는 점은, 일반키에 대해서는 따로 정밀한 처리를 하고 있지 않아서
	// a shift 라고하면, a shift b 키를 눌러도, b키에 대한 처리는 하지 않는다.
	// 따라서, a shift에 해당하는 함수가 호출되는 것이다.



	// Player Move Key
	GET_SINGLE(CInput)->AddNormalKey("MoveLeft", VK_LEFT);
	GET_SINGLE(CInput)->AddNormalKey("MoveRight", VK_RIGHT);
	GET_SINGLE(CInput)->AddNormalKey("MoveUp", VK_UP);
	GET_SINGLE(CInput)->AddNormalKey("MoveDown", VK_DOWN);

	// MouseKey
	GET_SINGLE(CInput)->AddNormalKey("MouseLButton", VK_LBUTTON);
	GET_SINGLE(CInput)->AddNormalKey("MouseRButton", VK_RBUTTON);



	// 정지
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

	// Brush를 생성한다
	m_hGreenBrush = CreateSolidBrush(RGB(0, 255, 0));
	m_hRedBrush = CreateSolidBrush(RGB(255, 0, 0));
	m_hYellowBrush = CreateSolidBrush(RGB(255, 255, 0));
	m_hBlackBrush = CreateSolidBrush(RGB(0, 0, 0));
	m_hWhiteBrush = CreateSolidBrush(RGB(255, 255, 255));
	m_hMagenta = CreateSolidBrush(RGB(255, 0, 255));

	// Pen을 생성한다.
	m_hRedPen = CreatePen(PS_SOLID, 3, RGB(255, 0, 0));
	m_hGreenPen = CreatePen(PS_SOLID, 3, RGB(0, 255, 0));
	m_hPurplePen = CreatePen(PS_SOLID, 3, RGB(128, 0, 255));
	m_hWhitePen = CreatePen(PS_SOLID, 3, RGB(255, 255, 255));

	// 화면 DC를 생성한다
	m_hDC = GetDC(m_hWnd);

	// 타이머를 생성한다.
	m_pTimer = new CTimer;

	m_pTimer->Init();

	// 정지 상태 false
	m_bPause = false; 



	// 경로 관리자 초기화
	if (!GET_SINGLE(CPathManager)->Init())
		return false;

	// 자원 관리자 초기화
	if (!GET_SINGLE(CResourceManager)->Init())
		return false;

	// 소리 관리자 초기화
	if (!GET_SINGLE(CSoundManager)->Init())
		return false;

	// 충돌 관리자 초기화
	if (!GET_SINGLE(CCollisionManager)->Init())
		return false;

	// 입력 관리자 초기화.
	if (!GET_SINGLE(CInput)->Init(m_hWnd))
		return false;

	// 키를 추가한다.
	AddInput();


	// 장면 관리자 초기화
	if (!GET_SINGLE(CSceneManager)->Init())
		return false;



	// 미리 생성해둔 백버퍼를 얻어온다.
	m_pBackBuffer = GET_SINGLE(CResourceManager)->FindTexture("BackBuffer");
	// ref = 2

	/*
	
	애니메이션 편집모드로 실행하는 경우에는 src에 해당하는 애니메이션을 전체 출력하고,
	이것 위에서 프레임의 크기에 해당하는 사각형이 움직인다.
	그러니까, src의 좌표대로 그대로 출력을 하는 것이다. src를 배경처럼 출력하고, 이 위에서
	프레임을 움직이면서 상태를 볼 수 있도록 한다.

	오른쪽 상단에 애니메이션을 현재 위치에서 src와 FrameSize를 가지고 출력하면 어떤 애니메이션이 보이게 되는지 
	보여주는 기능을 만든다.

	애니메이션을 바꾸는 경우. 해당 src에서 애니메이션의 src로 이동(화면에서 이것이 바로 보이도록 구성한다)

	게임 시작시 애니메이션 편집모드로 실행할지 물어보는 창을 만든다.
	
	*/

	ShowCursor(FALSE);


	return true;
}

int CGameManager::Run()
{
	
	MSG msg;

	// 기본 메시지 루프입니다:
	while (m_bLoop)
	{
		if(PeekMessage(&msg, nullptr, NULL, NULL, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		else
		{
			// 윈도우의 데드타임
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

	// 윈도우 크기를 설정한다. 
	m_tResolution.iWidth = RS_WIDTH;
	m_tResolution.iHeight = RS_HEIGHT;

	// 실제 클라이언트를 해상도에 맞추기 위해서 설정해야하는 크기를 받아온다.
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

	// 시간을 얻어온다.
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
		// Stage TileRender를 가지고 멈추는 처리와, 타일 렌더링을 같이 하는 것으로 한다. (PlayMode에서는)

		// 기능 키에 대한 처리를 한다.
		if (KEYDOWN("TileRender"))
		{
			CStage* pStage = (CStage*)GET_SINGLE(CSceneManager)->FindObject("Stage");

			pStage->SetTileRender(!pStage->GetTileRender());

			// 이 키는 MainScene에서만 호출이 되는데, 이것을 끄면 Tile의 Render를 하지 않는다.

			SAFE_RELEASE(pStage);
		}

		if (m_bPause)
			break;

		// Pause 상태라면, Collision과 Render에 대한 처리를 한다. 
		// 그러면, 업데이트 처리를 하지 않으니 Tile충돌 혹은, LineCollision에 대한 함수를 호출하며
		// 갑자기 프레임이 상승하는 상황에서도 기존의 처리대로 올려주는 처리가 진행이 되고 있어서
		// 빠지는 상황을 없을 것이다.

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
	// SceneManager의 Collision을 통해서 따로 하는 처리가 없다.
	// 호출하지 않고, 직접적인 처리를 위해 CollisionManager의 Collision함수를 호출해준다.

	switch (m_eMode)
	{
	case GM_PLAY:
	case GM_START:
	case GM_MAP_EDIT: // 마우스와 충돌.
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

	// 마우스 Render
	GET_SINGLE(CInput)->Render(m_pBackBuffer->GetDC(), fTime);

	// 백버퍼에서 그린 그림을 현재 화면DC에 복사한다.

	m_pBackBuffer->Render(m_hDC, 0, 0, 0, 0, 
		m_tResolution.iWidth, m_tResolution.iHeight);
	// 백버퍼의 그림을 전체화면크기로 화면 DC에 그려준다.
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

