#include "MapEditScene.h"
#include "../Object/Stage.h"
#include "../Core/Input.h"
#include "../Core/SceneManager.h"
#include "../InputComponent.h"
#include "../Core/GameManager.h"


CMapEditScene::CMapEditScene()
{
	m_eTileOption = TO_NORMAL;
	m_eMode = EM_TILE_OPTION;
	m_eState = ES_NONE;
	m_eSpawnerOption = SO_BURT;
	m_strMonsterTag = "Burt";
	m_eSpawnerDir = SD_LEFT;
	m_iSpawnCount = 1;
	m_bSpawnCount = true;

	m_ePositionOption = PO_START;

	m_pInputComponent = nullptr;

	m_bTileRender = true;
	m_bLineRender = true;
	m_bSpawnerRender = true;


	m_tDoorPos = Position(200.f, RS_HEIGHT - 200.f);
	m_tStartPos = Position(50.f, RS_HEIGHT - 200.f);

	m_iBack = 0;
	m_iFore = 0;
}


CMapEditScene::~CMapEditScene()
{
	SAFE_RELEASE(m_pInputComponent);
}

void CMapEditScene::Start()
{
	CScene::Start();
}



bool CMapEditScene::Init()
{

	CScene::Init();

	ShowCursor(TRUE);
	

	// BackGround와 ForeGround의 정보를 가져온다. (레퍼런스를 읽어와서, 그대로 복사)
	m_vecBackGroundPos = GET_SINGLE(CSceneManager)->GetBackGroundVec();
	m_vecForeGroundPos = GET_SINGLE(CSceneManager)->GetForeGroundVec();

	CStage* pStage = CObj::CreateObj<CStage>("Stage", "BackGround", this);


	// ForeGround를 설정해준다.
	pStage->SetForeGroundTexture("ForeGround", TEXT("ForeGround.bmp"));
	pStage->SetForeGroundTextureColorKey(0, 18, 127);

	pStage->SetStagePos(GetForePos(m_iFore).tStart,
		GetForePos(m_iFore).tEnd);

	// ForeGround와 연관된 정보인, 스테이지 사이즈를 받아둔다.
	m_tCurrentStageSize = GET_SINGLE(CSceneManager)->GetStageSize();

	// 새로운 스테이지의 사이즈로 카메라의 움직임의 범위를 제한한다.
	GET_SINGLE(CSceneManager)->SetLimitCamera(m_tCurrentStageSize);


	// 스테이지 사이즈를 기반으로 타일을 생성한다.
	pStage->CreateTile((int)m_tCurrentStageSize.x / 25, (int)m_tCurrentStageSize.y / 25, Size(25.f, 25.f));


	// BackGround를 설정해준다.
	pStage->SetTexture("BackGround", TEXT("BackGround.bmp"));
	pStage->SetBackGroundPos(GetBackPos(m_iBack).tStart,
		GetBackPos(m_iBack).tEnd);


	m_pStage = pStage;

	m_pStage->SetTileRender(true);

	m_tPrevStageSize = m_tCurrentStageSize;

	AddKey();


	// 고정된 값.
	m_tTileSize = Size(25.f, 25.f);


	return true;
}

void CMapEditScene::Update(float fTime)
{
	// 마우스가 화면밖으로 나가면, 처리하지 않는다.
	if (!GET_SINGLE(CInput)->MouseIsScreen())
	{
		m_eState = ES_NONE;
		return;
	}

	m_tMousePos = GET_SINGLE(CInput)->GetMousePos();


	if(m_eState != ES_DRAG)
		CameraUpdate(fTime);

	// 드래그중인 경우 카메라의 위치를 고정.

	Position tCameraPos = GET_SINGLE(CSceneManager)->GetCameraPos();

	if (KEYDOWN("MouseLButton"))
	{
		m_eState = ES_CLICK;

		if (m_eMode == EM_TILE_OPTION && m_bTileRender)
		{

			// 마우스를 누른 순간에 해당 마우스의 위치를 기준으로 타일의 위치를 알아낸다.

			// 이렇게 알아온 위치는 클라이언트 영역의 마우스 좌표이다.

			int iIndex = m_pStage->GetTileIndex(m_tMousePos + tCameraPos);

			// 현재 옵션 값에 따른 처리를 한다.

			if (m_eTileOption == TO_NORMAL)
			{
				m_pStage->ChangeTileOption(iIndex, TO_NORMAL);
			}
			else
			{
				if (m_pStage->GetTileOption(iIndex) == TO_WALL)
				{
					// 블럭 옵션인 상태로 기존에 블럭인 타일을 눌렀다.
					m_pStage->ChangeTileOption(iIndex, TO_NORMAL);

				}
				else
				{
					// 기존 블럭이 Normal 상태이다.
					m_pStage->ChangeTileOption(iIndex, TO_WALL);

				}
			}
		}
		else if (m_eMode == EM_LINE && m_bLineRender)
		{
			// SRC의 위치를 받는다.
			m_tSrc = tCameraPos + m_tMousePos;
		}
		else if (m_eMode == EM_SPAWNER && m_bSpawnerRender)
		{
			m_tSpawnerPos = tCameraPos + m_tMousePos;
		}
		else if (m_eMode == EM_POSITION)
		{
			m_tPos = tCameraPos + m_tMousePos;
		}
		
		// BACKGROUND, FOREGROUND 의 경우에는  마우스 처리를 생각했는데, 이것은 통일성을 해치며,
		// 쓸데없이 추가적인 기능이다. 오히려 이것을 추가함으로써 생기는 잡다한 문제점이 훨씬 많아진다..
		//else if (m_eMode == EM_BACKGROUND)
		//{
		//	// Mouse의 위치를 가지고 넘어갈지 이전으로 갈지 판단한다. 
		//	// 다른 옵션 정보에서는 사용하지 못하는 기능인데, 배경과 연관이 있는 두 모드에서는 충분히 사용하는 것이 자연스럽다.
		//	// 예를 들어 타일 옵션을 변경하는데, 여기서 화면을 이용해서 옵션값을 바꾸는 것은 부자연스럽다..

		//	if (m_tMousePos.x <= RS_WIDTH / 2.f)
		//	{
		//		// 왼쪽 : 감소
		//		BackPrev();
		//	}
		//	else
		//	{
		//		// 오른쪽 : 증가
		//		BackNext();
		//	}
		//}

		//else if (m_eMode == EM_FOREGROUND)
		//{
		//	if (m_tMousePos.x <= RS_WIDTH / 2.f)
		//	{
		//		// 왼쪽 : 감소
		//		ForePrev();
		//	}
		//	else
		//	{
		//		// 오른쪽 : 증가
		//		ForeNext();
		//	}
		//}
	
	}
	else if (KEYPRESS("MouseLButton") && m_eMode == EM_LINE && m_bLineRender)
	{
		m_tDest = tCameraPos + m_tMousePos;

	}
	else if (KEYUP("MouseLButton"))
	{
		switch (m_eMode)
		{
		case EM_POSITION:
		case EM_LINE:
		case EM_SPAWNER:
			m_eState = ES_NONE;

			break;
		}
	}


	// RButton.
	if (KEYDOWN("MouseRButton"))
	{
		switch (m_eMode)
		{
		case EM_FOREGROUND:
			ShowCursor(TRUE);

			if (MessageBox(nullptr, TEXT("Primary ForeGround"), TEXT("ForeGround Reset"), MB_YESNO) == IDYES)
			{
				SetFore(0);
			}

			ShowCursor(FALSE);
			break;

		case EM_BACKGROUND:

			ShowCursor(TRUE);

			if (MessageBox(nullptr, TEXT("Primary BackGround"), TEXT("BackGround Reset"), MB_YESNO) == IDYES)
			{
				SetBack(0);
			}

			ShowCursor(FALSE);

			break;

		case EM_POSITION:
			m_tPos = Position(0.f, 0.f);

			break;
		case EM_TILE_OPTION:
			if (m_bTileRender)
			{
				m_eState = ES_DRAG;
				m_tClickPos = m_tMousePos;
			}
			break;

		case EM_LINE:
			if (m_bLineRender)
			{
				m_eState = ES_NONE;
				m_tSrc = Position(0.f, 0.f);
				m_tDest = m_tSrc;
			}
			break;

		case EM_SPAWNER:

			if (m_bSpawnerRender)
			{
				m_eState = ES_NONE;
				m_tSpawnerPos = Position(0.f, 0.f);
			}
			break;

		}

	}

	else if (KEYUP("MouseRButton") )
	{

		// 만약에 마우스가 화면밖으로 나가서 UP을 하는 경우에 인덱스의 범위를 벗어날 것이다.
		// 이런 경우가 있을 가능성이 있는가 ?
		// 중간에 Drag상태에서 나가버리는 경우. 이전에 처리가 되기 때문에 처리하지 않는다.
		// 즉, 인덱스 Out의 경우는 없다.

		// 드래그하던 중에 뗀 경우.
		
		if (m_eMode == EM_TILE_OPTION && m_eState == ES_DRAG && m_bTileRender)
		{

			// Src와 Dest의 World좌표를 얻어낸다.
			Position tSrcPos = m_tClickPos + tCameraPos;
			Position tDestPos = m_tMousePos + tCameraPos;


			// Rect의 충돌영역에 해당하는 인덱스를 구해낸다.
			int iStartX = m_pStage->GetTileIndexX(tSrcPos.x);
			int iEndX = m_pStage->GetTileIndexX(tDestPos.x);
			
			int iStartY = m_pStage->GetTileIndexY(tSrcPos.y);
			int iEndY = m_pStage->GetTileIndexY(tDestPos.y);

			int iNumX = (int)m_pStage->GetTileNumber().x;

			for (int y = iStartY; y <= iEndY; ++y)
			{
				for (int x = iStartX; x <= iEndX; ++x)
				{
					int iIndex = y * iNumX + x;

					// Rect의 인덱스를 구해서 모든 부분에 Option을 똑같이 준다.
					m_pStage->ChangeTileOption(iIndex, m_eTileOption);

				}
			}

			
		}

		m_eState = ES_NONE;

	}

	NormalInput(fTime);
		
	


	CScene::Update(fTime);
}

void CMapEditScene::Render(HDC hDC, float fTime)
{

	CScene::Render(hDC, fTime);

	if (m_eState == ES_DRAG && m_bTileRender)
	{
		// 현재 마우스의 위치를 얻어온다. m_tMousePos
		// 처음 클릭한 위치는 m_tClickPos  둘다 클라이언트 영역의 좌표.
		// 그대로 사용하면 된다.

		RECT tRC = { (int)m_tClickPos.x, (int)m_tClickPos.y, (int)m_tMousePos.x, (int)m_tMousePos.y };

		FrameRect(hDC, &tRC, MAGENTABRUSH);
	}

	ObjectPosRender(hDC);

	TextRender(hDC);



}

void CMapEditScene::EdgeSetBlock()
{
	int iSizeX = (int)m_pStage->GetTileNumber().x;
	int iSizeY = (int)m_pStage->GetTileNumber().y;

	// 가장 상단
	for (int i = 0; i < iSizeX; ++i)
	{
		m_pStage->ChangeTileOption(i, TO_WALL);
	}

	// 가장 하단
	for (int i = 0; i < iSizeX; ++i)
	{
		int iIndex = iSizeX * (iSizeY - 1) + i;

		m_pStage->ChangeTileOption(iIndex, TO_WALL);
	}

	// 왼쪽
	for (int y = 0; y < iSizeY; ++y)
	{
		int iIndex = y * iSizeX;
		m_pStage->ChangeTileOption(iIndex, TO_WALL);

	}

	// 오른쪽
	for (int y = 0; y < iSizeY; ++y)
	{
		int iIndex = y * iSizeX + (iSizeX - 1);
		m_pStage->ChangeTileOption(iIndex, TO_WALL);

	}
}

void CMapEditScene::NormalInput(float fTime)
{

	// Spawner Option Key

	if (m_eMode == EM_SPAWNER && m_bSpawnerRender) // 보인다는 조건하에 바꾼다.
	{
		if (KEYDOWN("3")) // dir
		{
			switch (m_eSpawnerDir)
			{
			case SD_LEFT:
				m_eSpawnerDir = SD_RIGHT;
				break;

			case SD_RIGHT:
				m_eSpawnerDir = SD_LEFT;

				break;
			}
		}

		if (KEYDOWN("4")) // Count Switch
		{
			m_bSpawnCount = !m_bSpawnCount;

			if (!m_bSpawnCount)
			{
				// 마리수 무한대로 설정이 되었다면, 이것의 저장을 위해서 0으로 셋팅한다.
				m_iSpawnCount = 0;
			}
			else
				m_iSpawnCount = 1;


			
		}


		if (m_bSpawnCount)
		{
			if (KEYDOWN("5")) // ++
			{
				++m_iSpawnCount;

				if (m_iSpawnCount > 10)
					m_iSpawnCount = 10;
			}


			else if (KEYDOWN("6")) // --
			{
				--m_iSpawnCount;

				if (m_iSpawnCount < 1)
					m_iSpawnCount = 1;
			}

		}
	}
	else if (m_eMode == EM_FOREGROUND)
	{
		if (KEYDOWN("3"))
		{
			ForeNext();

		}

		if (KEYDOWN("4")) 
		{
			ForePrev();

		}
	}
	else if (m_eMode == EM_BACKGROUND)
	{
		if (KEYDOWN("3"))
		{
			BackNext();

		}

		if (KEYDOWN("4"))
		{
			BackPrev();

		}
	}

	// 타일 옵션 / 타일 보이고, f6키. 둘러치기.
	if (m_eMode == EM_TILE_OPTION && m_bTileRender &&
		KEYDOWN("F6"))
	{
		EdgeSetBlock();
	}

	// Render Key
	if (KEYDOWN("F3"))
	{
		// Tile
		m_bTileRender = !m_bTileRender;

		// 현재 타일은 생성된 Stage에서 출력해주고 있다.
		m_pStage->SetTileRender(m_bTileRender);
	}
	else if (KEYDOWN("F4"))
	{
		// Spawner
		m_bSpawnerRender = !m_bSpawnerRender;

	}
	else if (KEYDOWN("F5"))
	{
		// Line
		m_bLineRender = !m_bLineRender;

	}
	

	// 저장하는 키 / 삭제하는 키 ( 내부 처리는 모드에 따라서 달라진다. )

	if (KEYDOWN("SavePos"))
	{
		ObjectPosSave();

		switch (m_eMode)
		{
		case EM_TILE_OPTION: // 옵션 정보라면, 처리하지 않음.
			break;

		case EM_LINE:
			if (m_bLineRender)
			{
				m_tSrc = Position(0.f, 0.f);
				m_tDest = m_tSrc;
			}
			break;

		case EM_SPAWNER:
			if(m_bSpawnerRender)
				m_tSpawnerPos = Position(0.f, 0.f);

			break;

		}

	}

	else if (KEYDOWN("DeletePos"))
	{
		ObjectPosDelete();
		// 상태에 따라서 우리가 최근에 추가한 것을 정리한다.
	}
}

void CMapEditScene::ModeChangeNext(float fTime)
{
	if (!InnerMouse())
		return;

	int iMode = m_eMode + 1;

	if (iMode >= EM_END)
		iMode = EM_NONE + 1;

	m_eMode = (EDIT_MODE)iMode;
}


void CMapEditScene::OptionChangeNext(float fTime)
{
	if (!InnerMouse())
		return;

	int iOption = 0;

	switch (m_eMode)
	{
	case EM_TILE_OPTION:
		if (m_bTileRender)
		{

			iOption = m_eTileOption + 1;

			if (iOption >= TO_END)
				iOption = TO_NONE + 1;

			m_eTileOption = (TILE_OPTION)iOption;
		}
		break;

	case EM_POSITION:


		iOption = m_ePositionOption + 1;

		if (iOption >= PO_END)
			iOption = PO_NONE + 1;

		m_ePositionOption = (POSITION_OPTION)iOption;

		break;

	case EM_BACKGROUND: // 2 경우에는 이전과 다음 키에 대한 처리를 하기 위해서 이곳에서 처리를 하지 않는다.
	case EM_FOREGROUND:
	case EM_LINE: // Line모드에서는 무시.
		break;

	case EM_SPAWNER:

		if (m_bSpawnerRender)
		{
			iOption = m_eSpawnerOption + 1;

			if (iOption >= SO_END)
				iOption = SO_NONE + 1;

			m_eSpawnerOption = (SPAWNER_OPTION)iOption;

			switch (m_eSpawnerOption)
			{
			case SO_BURT:
				m_strMonsterTag = "Burt";
				break;

			case SO_TWIZZY:
				m_strMonsterTag = "Twizzy";
				break;

			case SO_TWIZZY_FLY:
				m_strMonsterTag = "TwizzyFly";
				break;

			case SO_GRIZOO:
				m_strMonsterTag = "Grizoo";
				break;

			case SO_BLADE:
				m_strMonsterTag = "Blade";
				break;

			case SO_ROCKY:
				m_strMonsterTag = "Rocky";
				break;

			case SO_MIKE:
				m_strMonsterTag = "Mike";
				break;

			case SO_SLEEP:
				m_strMonsterTag = "Sleep";
				break;
			}
		}

		break;
	}
	
}


void CMapEditScene::Save(float fTime)
{
	if (!InnerMouse())
		return;

	else if (m_eMode != EM_TILE_OPTION)
		return;

	OPENFILENAME	tOFN;

	memset(&tOFN, 0, sizeof(OPENFILENAME));

	TCHAR strPath[MAX_PATH] = {};

	tOFN.lStructSize = sizeof(OPENFILENAME); // 구조체의 사이즈를 원한다.
	tOFN.hwndOwner = WINDOWHANDLE; // 주인이 되는 핸들을 넘긴다.
	tOFN.lpstrFilter = TEXT("Stage File(*.stg)\0*.stg\0All Files(*.*)\0*.*\0");
	// 파일 형식의 콤보 박스에 나타낼 필터들을 나타낸다. 널 문자를 기준으로 하기에 
	// 파일형식\0필터와 같이 사용한다.
	// 콤보박스의 순서대로 나타난다. 여러개를 지정할 수 있다.

	tOFN.lpstrFile = strPath;
	// 파일 이름 에디트에서 처음 나타날 파일명을 지정하는 용도로 사용하며, 최종적으로는
	// 사용자가 선택한 파일의 이름을 반환하는 용도로 사용된다.
	// 즉, GetSaveFileName 을 거쳐서 제대로 파일의 경로를 알아온 이후에
	// 이것(strPath)를 사용하는 것이다.


	// lpstrFileTile은 파일의 이름을 돌려받기 위한 버퍼인데, 이것의 길이를 maxPath로 한다.
	tOFN.nMaxFile = MAX_PATH;

	tOFN.lpstrInitialDir = TEXT("..\\Bin\\");
	// 파일 찾기를 시작할 디렉토리를 지정한다. 현재 프로젝트의 위치에서 ../Bin(출력폴더)/

	ShowCursor(TRUE);

	if (GetSaveFileName(&tOFN) != 0)
	{
		// 0이 아니면 풀경로가 들어오는데, 이 경로를 이용해서 파일을 만들어낸다.
		// Unicode -> Multibyte

		char strFullPath[MAX_PATH] = {};

		// 바꿀 길이를 얻어온다.
		int iLength = WideCharToMultiByte(CP_ACP, 0, strPath, 
			-1, nullptr, 0, nullptr, nullptr);

		WideCharToMultiByte(CP_ACP, 0, strPath, -1, strFullPath, iLength, nullptr, nullptr);
		// 만들어진 경로를 이용해서 Stage를 저장한다.

		m_pStage->Save(strFullPath);
	}

	ShowCursor(FALSE);
	
}

void CMapEditScene::Load(float fTime)
{
	if (!InnerMouse())
		return;

	else if (m_eMode != EM_TILE_OPTION)
		return;

	OPENFILENAME	tOFN;

	memset(&tOFN, 0, sizeof(OPENFILENAME));

	TCHAR strPath[MAX_PATH] = {};

	tOFN.lStructSize = sizeof(OPENFILENAME); // 구조체의 사이즈를 원한다.
	tOFN.hwndOwner = WINDOWHANDLE; // 주인이 되는 핸들을 넘긴다.
	tOFN.lpstrFilter = TEXT("Stage File(*.stg)\0*.stg\0All Files(*.*)\0*.*\0");
	// 파일 형식의 콤보 박스에 나타낼 필터들을 나타낸다. 널 문자를 기준으로 하기에 
	// 파일형식\0필터와 같이 사용한다.
	// 콤보박스의 순서대로 나타난다. 여러개를 지정할 수 있다.

	tOFN.lpstrFile = strPath;
	tOFN.nMaxFile = MAX_PATH;
	tOFN.lpstrInitialDir = TEXT("..\\Bin\\");

	ShowCursor(TRUE);

	if (GetOpenFileName(&tOFN) != 0)
	{
		// 0이 아니면 풀경로가 들어오는데, 이 경로를 이용해서 파일을 만들어낸다.
		// Unicode -> Multibyte

		char strFullPath[MAX_PATH] = {};

		// 바꿀 길이를 얻어온다.
		int iLength = WideCharToMultiByte(CP_ACP, 0, strPath,
			-1, nullptr, 0, nullptr, nullptr);

		WideCharToMultiByte(CP_ACP, 0, strPath, -1, strFullPath, iLength, nullptr, nullptr);
		// 만들어진 경로를 이용해서 Stage를 저장한다.

		m_pStage->Load(strFullPath);

	}

	ShowCursor(FALSE);
}

void CMapEditScene::SaveObjectPos(float fTime)
{
	// LineCollider와 Spawner의 정보를 저장한다.
	if (!InnerMouse())
		return;

	else if (m_eMode == EM_TILE_OPTION)
		return;

	OPENFILENAME	tOFN;

	memset(&tOFN, 0, sizeof(OPENFILENAME));

	TCHAR strPath[MAX_PATH] = {};

	tOFN.lStructSize = sizeof(OPENFILENAME); 
	tOFN.hwndOwner = WINDOWHANDLE; 
	tOFN.lpstrFilter = TEXT("Object Pos(*.txt)\0*.txt\0All Files(*.*)\0*.*\0");

	tOFN.lpstrFile = strPath;
	tOFN.nMaxFile = MAX_PATH;
	tOFN.lpstrInitialDir = TEXT("..\\Bin\\");


	ShowCursor(TRUE);

	if (GetSaveFileName(&tOFN) != 0)
	{
		char strFullPath[MAX_PATH] = {};

		int iLength = WideCharToMultiByte(CP_ACP, 0, strPath,
			-1, nullptr, 0, nullptr, nullptr);

		WideCharToMultiByte(CP_ACP, 0, strPath, -1, strFullPath, iLength, nullptr, nullptr);

		SaveObjectPosText(strFullPath);

	}

	ShowCursor(FALSE);
}

void CMapEditScene::LoadObjectPos(float fTime)
{
	if (!InnerMouse())
		return;

	else if (m_eMode == EM_TILE_OPTION)
		return;

	OPENFILENAME	tOFN;

	memset(&tOFN, 0, sizeof(OPENFILENAME));

	TCHAR strPath[MAX_PATH] = {};

	tOFN.lStructSize = sizeof(OPENFILENAME);
	tOFN.hwndOwner = WINDOWHANDLE;
	tOFN.lpstrFilter = TEXT("Object Pos(*.txt)\0*.txt\0All Files(*.*)\0*.*\0");

	tOFN.lpstrFile = strPath;
	tOFN.nMaxFile = MAX_PATH;
	tOFN.lpstrInitialDir = TEXT("..\\Bin\\");



	ShowCursor(TRUE);

	if (GetOpenFileName(&tOFN) != 0)
	{
		char strFullPath[MAX_PATH] = {};

		int iLength = WideCharToMultiByte(CP_ACP, 0, strPath,
			-1, nullptr, 0, nullptr, nullptr);

		WideCharToMultiByte(CP_ACP, 0, strPath, -1, strFullPath, iLength, nullptr, nullptr);

		LoadObjectPosText(strFullPath);

	}

	ShowCursor(FALSE);
}

void CMapEditScene::SaveObjectPosText(const char * pFileName)
{
	FILE* pFile = nullptr;

	fopen_s(&pFile, pFileName, "wt"); // 수정을 편리하게 하기 위해서 txt파일로 한다.

	if (!pFile)
		return;

	// BackGround의 인덱스와 ForeGround의 인덱스를 저장한다. 
	fprintf(pFile, "BackGround : %d / ForeGround : %d\n", m_iBack + 1, m_iFore + 1);

	// 시작 위치와 Door의 위치를 저장한다.
	fprintf(pFile, "Start : (%d, %d) / Door : (%d, %d)\n", (int)m_tStartPos.x, (int)m_tStartPos.y,
		(int)m_tDoorPos.x, (int)m_tDoorPos.y);

	fprintf(pFile, " == Line Collider == \n");
	int iSize = m_vecCoord.size();
	fprintf(pFile, "%d\n", iSize);

	// 개수 만큼 저장한다.
	for (int i = 0; i < iSize; ++i)
	{
		fprintf(pFile, "src : (%d, %d) / dest : (%d, %d)\n",
			(int)m_vecCoord[i].tStart.x, (int)m_vecCoord[i].tStart.y,
			(int)m_vecCoord[i].tEnd.x, (int)m_vecCoord[i].tEnd.y);
	}
	// 스포너를 저장한다.
	fprintf(pFile, " == Spawner == \n");
	iSize = m_vecSpawner.size();
	fprintf(pFile, "%d\n", iSize);

	// 스포너의 개수 만큼 저장한다.
	for (int i = 0; i < iSize; ++i)
	{
		int strSize = m_vecSpawner[i].strMonsterTag.length();
		// 몬스터의 이름의 개수를 저장한다.
		fprintf(pFile, "Length : %d\n", strSize);

		// 이름의 개수만큼 쓴다.
		fwrite(m_vecSpawner[i].strMonsterTag.c_str(), sizeof(char), strSize, pFile);

		fprintf(pFile, "\nSpawnerPos : (%d, %d) / DirLeft : %d / bCount : %d / iCount : %d\n",
			(int)m_vecSpawner[i].tPos.x, (int)m_vecSpawner[i].tPos.y,
			(m_vecSpawner[i].eSpawnDir == SD_LEFT), m_vecSpawner[i].bCount,
			m_vecSpawner[i].iCount);

	}

	fclose(pFile);
}

void CMapEditScene::LoadObjectPosText(const char * pFileName)
{

	FILE* pFile = nullptr;

	fopen_s(&pFile, pFileName, "rt"); // 수정을 편리하게 하기 위해서 txt파일로 한다.

	if (!pFile)
		return;

	// BackGround의 값과 ForeGround의 값을 읽어온다.
	fscanf_s(pFile, "BackGround : %d / ForeGround : %d\n", &m_iBack, &m_iFore);

	--m_iBack;
	--m_iFore;
	// 여기서 읽어온 값은 인덱스는 아니다.

	SetBack(m_iBack);
	SetFore(m_iFore);

	int srcX = 0, srcY = 0, destX = 0, destY = 0;

	// 시작 지점과 문의 위치를 읽어온다.
	fscanf_s(pFile, "Start : (%d, %d) / Door : (%d, %d)\n", &srcX, &srcY,
		&destX, &destY);

	m_tStartPos = Position(srcX, srcY);
	m_tDoorPos = Position(destX, destY);

	char strFileName[100] = {};

	fgets(strFileName, 100, pFile); // == Line ==

	int iSize = 0;
	// 개수를 읽어온다.

	fscanf_s(pFile, "%d\n", &iSize);

	m_vecCoord.clear();

	// 개수 만큼 읽어온다..
	for (int i = 0; i < iSize; ++i)
	{
		srcX = 0, srcY = 0, destX = 0, destY = 0;

		fscanf_s(pFile, "src : (%d, %d) / dest : (%d, %d)\n", &srcX, &srcY,
			&destX, &destY);

		m_vecCoord.push_back(ClipCoord(srcX, srcY, destX, destY));
	}

	// 스포너를 읽어온다.
	fgets(strFileName, 100, pFile); // == Spawner == 

	// 사이즈를 읽는다.
	iSize = 0;
	fscanf_s(pFile, "%d\n", &iSize);

	m_vecSpawner.clear();

	// 스포너의 개수 만큼 읽어온다.

	for (int i = 0; i < iSize; ++i)
	{
		SPAWNERINFO tInfo;

		int X, Y;
		int iDir = 0;
		
		int strSize = 0;
		char strLine[100] = {};

		// 몬스터의 이름의 개수를 읽는다.
		fscanf_s(pFile, "Length : %d\n", &strSize);

		// 이름의 개수만큼 읽는다
		fread(strLine, sizeof(char), strSize, pFile);

		int iCount = 0;

		fscanf_s(pFile, "\nSpawnerPos : (%d, %d) / DirLeft : %d / bCount : %d / iCount : %d\n",
			&X, &Y,
			&iDir, &iCount,
			&tInfo.iCount);
		
		tInfo.bCount = iCount;
		tInfo.strMonsterTag = strLine;
		tInfo.tPos = Position(X, Y);
		tInfo.eSpawnDir = (iDir != 0) ? SD_LEFT : SD_RIGHT;

		m_vecSpawner.push_back(tInfo);
		/*
		_tagSpawner(const string& strMonsterTag, const Position& tPos,
		SIDE_DIR eDir = SD_LEFT,
		bool bCount = true, int iCount = 1) :
		tPos(tPos),
		eSpawnDir(eDir),
		bCount(bCount),
		iCount(iCount),
		strMonsterTag(strMonsterTag)
		*/
	}

	fclose(pFile);
}


void CMapEditScene::ClearWallTile(float fTime)
{
	if (!InnerMouse())
		return;

	ShowCursor(TRUE);


	switch (m_eMode)
	{
	case EM_FOREGROUND:
		//  현재 출력하고 있는 텍스처를 초기상태로 바꾸는데, 이것이 좀 위험한 처리 아닌가 ? 
		// 보험하나 걸어두자.

		ShowCursor(TRUE);

		if (MessageBox(nullptr, TEXT("Primary ForeGround"), TEXT("ForeGround Reset"), MB_YESNO) == IDYES)
		{
			SetFore(0);
		}

		ShowCursor(FALSE);
		break;

	case EM_BACKGROUND:

		ShowCursor(TRUE);

		if (MessageBox(nullptr, TEXT("Primary BackGround"), TEXT("BackGround Reset"), MB_YESNO) == IDYES)
		{
			SetBack(0);
		}

		ShowCursor(FALSE);


		break;
	case EM_TILE_OPTION:

		if (m_bTileRender)
		{
			if (MessageBox(nullptr, TEXT("정말 삭제하시겠습니까 ?"),
				TEXT("Wall Tile"), MB_YESNO) == IDYES)
			{
				m_pStage->AllTileOptionChange(TO_NORMAL);
			}
		}
		break;

	case EM_POSITION:

		if (MessageBox(nullptr, TEXT("초기 위치로 옮기시겠습니까 ?"),
			TEXT("Start/Door Pos"), MB_YESNO) == IDYES)
		{
			m_tDoorPos = Position(200.f, RS_HEIGHT - 200.f);
			m_tStartPos = Position(50.f, RS_HEIGHT - 200.f);


			m_tPos = Position(0.f, 0.f);
		}
		break;

	case EM_LINE:

		if (m_bLineRender)
		{
			if (MessageBox(nullptr, TEXT("정말 삭제하시겠습니까 ?"),
				TEXT("Line Collider"), MB_YESNO) == IDYES)
			{
				m_vecCoord.clear();
				m_tSrc = Position(0.f, 0.f);
				m_tDest = m_tSrc;
			}
		}
	
		break;

	case EM_SPAWNER:

		if (m_bSpawnerRender)
		{
			if (MessageBox(nullptr, TEXT("정말 삭제하시겠습니까 ?"),
				TEXT("Spawner"), MB_YESNO) == IDYES)
			{
				m_vecSpawner.clear();
				m_tSpawnerPos = Position(0.f, 0.f);
			}
		}
		break;
	}
	
	

	ShowCursor(FALSE);

}

void CMapEditScene::TextRender(HDC hDC)
{

	// 마우스의 위치를 출력해주는 것으로 한다.
	// 일단, 한 가지 확인하고 갈 점. MapEditScene은 카메라 제한이 스테이지 크기로 잡혀있어서
	// 스테이지 크기 만큼만 이동할 수 있다. 즉, 우리가 움직이는 좌표는 스테이지의 월드좌표인 것이다.

	Position tCameraPos = GET_SINGLE(CSceneManager)->GetCameraPos();
	Position tMousePos = GET_SINGLE(CInput)->GetMousePos();
	Position tMouseWorldPos = tCameraPos + tMousePos;

	TCHAR strPos[40] = {};
	Position tRenderPos(20, 20);

	if (InnerMouse())
	{
		// 화면 안에 있는 마우스 좌표만 출력해준다.
		wsprintf(strPos, TEXT("Mouse : (%d, %d)"), (int)tMouseWorldPos.x, (int)tMouseWorldPos.y);
		TextOut(hDC, (int)tRenderPos.x, (int)tRenderPos.y, strPos, lstrlen(strPos));

	}




	switch (m_eMode)
	{
	case EM_LINE:
		TextOut(hDC, (int)tRenderPos.x, (int)tRenderPos.y + 20, TEXT("LINE MODE"), lstrlen(TEXT("LINE MODE")));
		break;

	case EM_TILE_OPTION:
		TextOut(hDC, (int)tRenderPos.x, (int)tRenderPos.y + 20, TEXT("TILE MODE"), lstrlen(TEXT("TILE MODE")));

		break;

	case EM_SPAWNER:
		TextOut(hDC, (int)tRenderPos.x, (int)tRenderPos.y + 20, TEXT("SPAWNER MODE"), lstrlen(TEXT("SPAWNER MODE")));

		break;

	case EM_POSITION:
		TextOut(hDC, (int)tRenderPos.x, (int)tRenderPos.y + 20, TEXT("POSITION MODE"), lstrlen(TEXT("POSITION MODE")));

		break;

	case EM_BACKGROUND:
		TextOut(hDC, (int)tRenderPos.x, (int)tRenderPos.y + 20, TEXT("BACKGROUND MODE"), lstrlen(TEXT("BACKGROUND MODE")));

		break;

	case EM_FOREGROUND:
		TextOut(hDC, (int)tRenderPos.x, (int)tRenderPos.y + 20, TEXT("FOREGROUND MODE"), lstrlen(TEXT("FOREGROUND MODE")));

		break;
	}



	switch (m_eMode)
	{

		// 아래의 두 경우는 옵션을 변경하여 출력하는 개념으로 하지 않고, 그냥 일반적으로 출력해주는 것으로 한다.
		// 타일처럼 옵션을 변경하는 개념이 아니다. ㅇㅇ 
	case EM_BACKGROUND:
	case EM_FOREGROUND:
		break;

	case EM_TILE_OPTION:
		switch (m_eTileOption)
		{
		case TO_NORMAL:
			TextOut(hDC, (int)tRenderPos.x, (int)tRenderPos.y + 40, TEXT("NORMAL"), lstrlen(TEXT("NORMAL")));
			break;

		case TO_WALL:
			TextOut(hDC, (int)tRenderPos.x, (int)tRenderPos.y + 40, TEXT("WALL"), lstrlen(TEXT("WALL")));

			break;
		}
		break;

	case EM_POSITION:
		switch (m_ePositionOption)
		{
		case PO_START:
			TextOut(hDC, (int)tRenderPos.x, (int)tRenderPos.y + 40, TEXT("START"), lstrlen(TEXT("START")));
			break;

		case PO_DOOR:
			TextOut(hDC, (int)tRenderPos.x, (int)tRenderPos.y + 40, TEXT("DOOR"), lstrlen(TEXT("DOOR")));

			break;
		}
		break;
		

	case EM_LINE:

		// Src Dest
		wsprintf(strPos, TEXT("Src : (%d, %d)"), (int)m_tSrc.x, (int)m_tSrc.y);
		TextOut(hDC, (int)tRenderPos.x, (int)tRenderPos.y + 40, strPos, lstrlen(strPos));

		wsprintf(strPos, TEXT("Dest : (%d, %d)"), (int)m_tDest.x, (int)m_tDest.y);
		TextOut(hDC, (int)tRenderPos.x, (int)tRenderPos.y + 60, strPos, lstrlen(strPos));

		break;

	case EM_SPAWNER:

		// 몬스터 태그
		switch (m_eSpawnerOption)
		{
		case SO_BURT:
			TextOut(hDC, (int)tRenderPos.x, (int)tRenderPos.y + 40, 
				TEXT("Monster : Burt"), lstrlen(TEXT("Monster : Burt")));

			break;

		case SO_TWIZZY:
			TextOut(hDC, (int)tRenderPos.x, (int)tRenderPos.y + 40,
				TEXT("Monster : Twizzy"), lstrlen(TEXT("Monster : Twizzy")));
			break;

		case SO_TWIZZY_FLY:
			TextOut(hDC, (int)tRenderPos.x, (int)tRenderPos.y + 40,
				TEXT("Monster : TwizzyFly"), lstrlen(TEXT("Monster : TwizzyFly")));
			break;

		case SO_GRIZOO:
			TextOut(hDC, (int)tRenderPos.x, (int)tRenderPos.y + 40,
				TEXT("Monster : Grizoo"), lstrlen(TEXT("Monster : Grizoo")));
			break;

		case SO_BLADE:
			TextOut(hDC, (int)tRenderPos.x, (int)tRenderPos.y + 40,
				TEXT("Monster : Blade"), lstrlen(TEXT("Monster : Blade")));
			break;

		case SO_ROCKY:
			TextOut(hDC, (int)tRenderPos.x, (int)tRenderPos.y + 40,
				TEXT("Monster : Rocky"), lstrlen(TEXT("Monster : Rocky")));
			break;

		case SO_MIKE:
			TextOut(hDC, (int)tRenderPos.x, (int)tRenderPos.y + 40,
				TEXT("Monster : Mike"), lstrlen(TEXT("Monster : Mike")));
			break;

		case SO_SLEEP:
			TextOut(hDC, (int)tRenderPos.x, (int)tRenderPos.y + 40,
				TEXT("Monster : Sleep"), lstrlen(TEXT("Monster : Sleep")));
			break;
		}
		
		// 스폰 방향
		switch (m_eSpawnerDir)
		{
		case SD_LEFT:
			TextOut(hDC, (int)tRenderPos.x, (int)tRenderPos.y + 60, TEXT("SpawnDir : Left"), lstrlen(TEXT("SpawnDir : Left")));

			break;

		case SD_RIGHT:
			TextOut(hDC, (int)tRenderPos.x, (int)tRenderPos.y + 60, TEXT("SpawnDir : Right"), lstrlen(TEXT("SpawnDir : Right")));

			break;
		}

		// 몬스터의 개수를 설정하는 기능이며, 이것을 보여준다.
		if (m_bSpawnCount)
		{
			wsprintf(strPos, TEXT("Limit Count : %d"), m_iSpawnCount);
			TextOut(hDC, (int)tRenderPos.x, (int)tRenderPos.y + 80, strPos, lstrlen(strPos));

			// 몬스터의 개수가 잡혀 있는 경우에는 이것을 보여주도록 한다.
		}
		else
		{
			// 잡혀있지 않다면 max
			TextOut(hDC, (int)tRenderPos.x, (int)tRenderPos.y + 80, 
				TEXT("Limit Count : Max"), lstrlen(TEXT("Limit Count : Max")));

		}
		
		break;
	}


	if (m_bTileRender)
	{
		TextOut(hDC, (int)tRenderPos.x, (int)tRenderPos.y + 120,
			TEXT("TileRender"), lstrlen(TEXT("TileRender")));
	}

	if (m_bSpawnerRender)
	{
		TextOut(hDC, (int)tRenderPos.x, (int)tRenderPos.y + 140,
			TEXT("SpawnerRender"), lstrlen(TEXT("SpawnerRender")));
	}

	if (m_bLineRender)
	{
		TextOut(hDC, (int)tRenderPos.x, (int)tRenderPos.y + 160,
			TEXT("LineRender"), lstrlen(TEXT("LineRender")));
	}


	// StartPos
	wsprintf(strPos, TEXT("Start Pos : (%d, %d)"), (int)m_tStartPos.x, (int)m_tStartPos.y);
	TextOut(hDC, (int)tRenderPos.x, (int)tRenderPos.y + 180, strPos, lstrlen(strPos));
	
	// DoorPos
	wsprintf(strPos, TEXT("Door Pos : (%d, %d)"), (int)m_tDoorPos.x, (int)m_tDoorPos.y);
	TextOut(hDC, (int)tRenderPos.x, (int)tRenderPos.y + 200, strPos, lstrlen(strPos));

	// ForeGround Number
	wsprintf(strPos, TEXT("ForeGround : %d"), m_iFore + 1);
	TextOut(hDC, (int)tRenderPos.x, (int)tRenderPos.y + 220, strPos, lstrlen(strPos));

	// BackGround Number
	wsprintf(strPos, TEXT("BackGround : %d"), m_iBack + 1);
	TextOut(hDC, (int)tRenderPos.x, (int)tRenderPos.y + 240, strPos, lstrlen(strPos));
}

void CMapEditScene::ObjectPosSave()
{
	// 가장 최근에 추가된 LineSave를 삭제한다.
	// 이 함수를 호출하기 이전에 마우스가 외부에 존재한다면 
	// return이 된다.

	// 메세지 박스와 위치 설정이 모션이 겹치는 경우가 있다. 따라서, 이것을 막기 위해 초기값인지를 확인하면 된다.

	switch (m_eMode)
	{
	case EM_FOREGROUND:
		// 현재 보여지는 인덱스의 스테이지 사이즈를 적용하는 것이다.

		// 위험한 처리이므로 MessageBox의 여부를 받는다.
		if (MessageBox(nullptr, TEXT("현재 ForeGround를 적용하시겠습니까 ? (Tile Clear)"), TEXT("New Tile Set"), MB_YESNO) == IDYES)
		{
			// 현재 설정된 스테이지 사이즈로 (ForeGround의 위치를 바꾸면서 스테이지 사이즈도 바꿔주는 시스템이 되어있다)
			// Tile을 생성하는 작업을 거친다.

			// 그냥 CreateTile함수를 사용하면 된다. (내부적으로 삭제하는 과정을 거친다.)

			// 스테이지 사이즈를 기반으로 타일을 생성한다.
			m_pStage->CreateTile((int)m_tCurrentStageSize.x / 25,
				(int)m_tCurrentStageSize.y / 25, Size(25.f, 25.f));

			// 초기값으로 타일의 개수만 맞춰서 생성이 된다.
			// 재 생성이 된다 ! 

		}

		break;

	case EM_BACKGROUND:
		// 아무 처리도 하지 않는다. (그냥 인덱스만 바꾸면 그게 적용임...)
		break;

	case EM_TILE_OPTION:
		break;

	case EM_POSITION:

		// Pos에 잡힌 위치를 Start Or Door에게 준다.

		if (m_tPos.x != 0.f)
		{
			switch (m_ePositionOption)
			{
			case PO_START:

				m_tStartPos = m_tPos;
				break;

			case PO_DOOR:
				m_tDoorPos = m_tPos;

				break;
			}

			m_tPos = Position(0.f, 0.f);
			// Pos는 초기값으로 .
		}

		

		break;

	case EM_LINE:
		if(m_bLineRender)
			m_vecCoord.push_back(ClipCoord(m_tSrc, m_tDest));

		break;

	case EM_SPAWNER:
		// 현재 설정한 옵션에 따라서 달라지는 부분이다.
		// 현재 설정한 포지션의 영향도 받는다.

		// 우리가 설정한 옵션에 따라서, 이미 몬스터의 태그 값이 잡혀있다.

		if (m_bSpawnerRender && m_tSpawnerPos.x != 0.f)
			m_vecSpawner.push_back(SPAWNERINFO(m_strMonsterTag, m_tSpawnerPos,
				m_eSpawnerDir, m_bSpawnCount, m_iSpawnCount));
		
		/*
		_tagSpawner(const string& strMonsterTagValue, const Position& tPos,
		SIDE_DIR eDir = SD_LEFT,
		bool bCount = true, int iCount = 1) :
		tPos(tPos),
		eSpawnDir(eDir),
		bCount(bCount),
		iCount(iCount)
	{
		strMonsterTag = strMonsterTagValue + "Spawner";
		// 기본적으로 능력 있는 몬스터에 대한 설정으로 한다.
	}
		*/

		break;
	}
}

void CMapEditScene::ObjectPosDelete()
{

	switch (m_eMode)
	{
	case EM_FOREGROUND:
		//  현재 출력하고 있는 텍스처를 초기상태로 바꾸는데, 이것이 좀 위험한 처리 아닌가 ? 
		// 보험하나 걸어두자.

		ShowCursor(TRUE);
		
		if (MessageBox(nullptr, TEXT("Primary ForeGround"), TEXT("ForeGround Reset"), MB_YESNO) == IDYES)
		{
			SetFore(0);
		}

		ShowCursor(FALSE);
		break;

	case EM_BACKGROUND:

		ShowCursor(TRUE);

		if (MessageBox(nullptr, TEXT("Primary BackGround"), TEXT("BackGround Reset"), MB_YESNO) == IDYES)
		{
			SetBack(0);
		}

		ShowCursor(FALSE);

		break;

	case EM_TILE_OPTION:
		break;

	case EM_POSITION:

		// 현재 잡혀있는 위치를 없애버린다. (저장된 값을 Pos 말고)
		// 정확하게는 초기위치로... 

		switch (m_ePositionOption)
		{
		case PO_START:

			m_tStartPos = Position(50.f, RS_HEIGHT - 200.f);
			break;
			
		case PO_DOOR:
			m_tDoorPos = Position(200.f, RS_HEIGHT - 200.f);

			break;
		}

		break;

	case EM_LINE:
		if (m_vecCoord.empty() && m_bLineRender)
			return;

		m_vecCoord.pop_back();
		break;

	case EM_SPAWNER:
		if (m_vecSpawner.empty() && m_bSpawnerRender)
			return;

		m_vecSpawner.pop_back();

		break;
	}


}

void CMapEditScene::ObjectPosRender(HDC hDC)
{
	Position tCameraPos = GET_SINGLE(CSceneManager)->GetCameraPos();
	HPEN hPen;
	HPEN hPrevPen;
	TCHAR strPos[200] = {};
	

	// 미리 그리기 모드 ! 
	switch(m_eMode)
	{
		// 이 경우에는 출력할 것이 없다.
	case EM_FOREGROUND:
	case EM_BACKGROUND:
	case EM_TILE_OPTION:
		break;


		
	case EM_LINE:

		if (m_bLineRender)
		{
			// 모드가 라인인 경우에만 현재 그리고 있는 ... src와 dest를 Render해준다.
			hPen = REDPEN;
			hPrevPen = (HPEN)SelectObject(hDC, hPen);

			MoveToEx(hDC,(int)(m_tSrc.x - tCameraPos.x), (int)(m_tSrc.y - tCameraPos.y ), nullptr);
			LineTo(hDC,	 (int)(m_tDest.x - tCameraPos.x),(int)( m_tDest.y - tCameraPos.y ) );

			SelectObject(hDC, hPrevPen);
		}

		break;

	case EM_SPAWNER:

		if (m_bSpawnerRender)
		{
			// 게임을 플레이 하는 상황에서 나오는 것과 동일하게 0.5 0.5 피봇으로 50px씩.

			// 0.f 0.f의 경우는 없는 것으로 친다. = 그냥 그리지 않는 수치를 정해둔 것이다. 
			// 초기화값으로..
			if (m_tSpawnerPos.x != 0.f && m_tSpawnerPos.y != 0.f)
			{
				// 해당 위치를 기준으로 하여 50px씩 그려준다.
				RECT tRect = { (int)(m_tSpawnerPos.x - 50.f - tCameraPos.x), 
					(int)(m_tSpawnerPos.y - 50.f - tCameraPos.y),
					(int)(m_tSpawnerPos.x + 50.f - tCameraPos.x),
					(int)(m_tSpawnerPos.y + 50.f - tCameraPos.y)
				};

				FrameRect(hDC, &tRect, MAGENTABRUSH);

				// 위치는 출력하되 확정이 아니니까, 정보는 출력하지 않는다. (애초에 옆에 정보 있잖아)
			}
		}

		break;

	case EM_POSITION:
		
		if (m_tPos.x != 0.f && m_tPos.y != 0.f)
		{
			switch (m_ePositionOption)
			{
			case PO_START:
			{
				RECT tRect = {
					(int)(m_tPos.x - tCameraPos.x - 30.f),
					(int)(m_tPos.y - tCameraPos.y - 30.f),
					(int)(m_tPos.x - tCameraPos.x + 30.f), 
					(int)(m_tPos.y - tCameraPos.y + 30.f) };

				FrameRect(hDC, &tRect, MAGENTABRUSH);
			}
			break;

			case PO_DOOR:
			{
				RECT tRect = { 
					(int)(m_tPos.x - tCameraPos.x - 60.f),
					(int)(m_tPos.y - tCameraPos.y - 70.f),
					(int)(m_tPos.x - tCameraPos.x + 60.f),
					(int)(m_tPos.y - tCameraPos.y + 70.f)
				};

				FrameRect(hDC, &tRect, MAGENTABRUSH);
			}
			break;
			}
		}
		break;
		
	}

	
	{
		// START POS 
		RECT tRect = {
			(int)(m_tStartPos.x - tCameraPos.x - 30.f), 
			(int)(m_tStartPos.y - tCameraPos.y - 30.f),
			(int)(m_tStartPos.x - tCameraPos.x + 30.f), 
			(int)(m_tStartPos.y - tCameraPos.y + 30.f) 
		};

		FrameRect(hDC, &tRect, REDBRUSH);
		TextOut(hDC,(int)(m_tStartPos.x - tCameraPos.x - 27.f),
					(int)(m_tStartPos.y - tCameraPos.y ),
			TEXT("StartPos"), lstrlen(TEXT("StartPos")));

		// DOOR 위치를 출력한다.
		tRect.left =		(int)(m_tDoorPos.x - tCameraPos.x - 60.f);
		tRect.right =		(int)(m_tDoorPos.x - tCameraPos.x + 60.f);
		tRect.top =			(int)(m_tDoorPos.y - tCameraPos.y - 77.f);
		tRect.bottom =		(int)(m_tDoorPos.y - tCameraPos.y + 77.f);

		FrameRect(hDC, &tRect, REDBRUSH);
		TextOut(hDC, (int)(m_tDoorPos.x - tCameraPos.x - 20.f), (int)(m_tDoorPos.y - tCameraPos.y), TEXT("Door"), lstrlen(TEXT("Door")));
	}

	
	// 스포너 vector에 정보가 들어가 있다면...
	if (!m_vecSpawner.empty() && m_bSpawnerRender)
	{
		// 출력해야 하는 정보 : 스포너의 위치(기본이지) dd  생성 몬스터... dd
		// 몬스터의 생성 방향 dd Count를 출력해준다. dd

		/*
		
		Position tPos;
		SIDE_DIR eSpawnDir;
		string strMonsterTag;
		bool	bCount;
		int		iCount;
		*/

		size_t iSize = m_vecSpawner.size();

		for (size_t i = 0; i < iSize; ++i)
		{

			Position tPos = m_vecSpawner[i].tPos;
			tPos -= tCameraPos;

			// 생성 몬스터를 출력한다.
			wsprintf(strPos, TEXT("Monster : %s"),
				StringToWstring(m_vecSpawner[i].strMonsterTag).c_str());
			TextOut(hDC, (int)(tPos.x + 60.f), (int)(tPos.y - 50.f), strPos, lstrlen(strPos));


			// 위치를 출력한다.
			wsprintf(strPos, TEXT("Pos : (%d, %d)"), (int)m_vecSpawner[i].tPos.x, (int)m_vecSpawner[i].tPos.y);
			TextOut(hDC, (int)(tPos.x + 60.f), (int)(tPos.y -30.f), strPos, lstrlen(strPos));

			// 생성 방향을 출력한다.
			switch (m_vecSpawner[i].eSpawnDir)
			{
			case SD_LEFT:
				TextOut(hDC, (int)(tPos.x + 60.f), (int)(tPos.y - 10.f),
					TEXT("Dir : Left"), lstrlen(TEXT("Dir : Left")));
				break;

			case SD_RIGHT:
				TextOut(hDC, (int)(tPos.x + 60.f), (int)(tPos.y - 10.f),
					TEXT("Dir : Right"), lstrlen(TEXT("Dir : Right")));
				break;
			}

			// Count를 출력한다.
			if (m_vecSpawner[i].bCount)
			{
				// 사용하는 경우에는 해당 숫자를.

				wsprintf(strPos, TEXT("Count : %d"), m_vecSpawner[i].iCount);
				TextOut(hDC, (int)(tPos.x + 60.f), (int)(tPos.y + 10.f), strPos, lstrlen(strPos));
			}
			else
			{
				// 사용하지 않는 경우에는 Max값을 출력한다.
				TextOut(hDC, (int)(tPos.x + 60.f), (int)(tPos.y + 10.f),
					TEXT("Count : Max"), lstrlen(TEXT("Count : Max")));

			}

			RECT tRect = { (int)tPos.x - 50, (int)tPos.y - 50, (int)tPos.x + 50, (int)tPos.y + 50 };
			FrameRect(hDC, &tRect, REDBRUSH);
		}
			

	}


	// 직선 vector에 뭔가가 있다면...
	if (!m_vecCoord.empty() && m_bLineRender)
	{
		// RedPen
		hPen = PURPLEPEN;
		hPrevPen = (HPEN)SelectObject(hDC, hPen);

		

		size_t iSize = m_vecCoord.size();

		for (size_t i = 0; i < iSize; ++i)
		{
			wsprintf(strPos, TEXT("Src : (%d, %d) / Dest : (%d, %d)"),
				(int)m_vecCoord[i].tStart.x, (int)m_vecCoord[i].tStart.y,
				(int)m_vecCoord[i].tEnd.x, (int)m_vecCoord[i].tEnd.y);

			// 해당 위치에 Line을 출력해주고, 위치를 표시한다.

			Position tRenderPos = m_vecCoord[i].tEnd;

			if (tRenderPos.y > m_vecCoord[i].tStart.y)
			{
				// 만약에 출력하고자 하는 Y좌표(End)가 Start 보다 밑에 있다면,
				// Start를 기준으로 출력한다.
				tRenderPos = m_vecCoord[i].tStart;
			}

			TextOut(hDC, (int)(tRenderPos.x - tCameraPos.x),
						 (int)(tRenderPos.y - 40.f - tCameraPos.y)
				, strPos, lstrlen(strPos));


			MoveToEx(hDC, (int)(m_vecCoord[i].tStart.x - tCameraPos.x),
						  (int)(m_vecCoord[i].tStart.y - tCameraPos.y), nullptr);

			LineTo(hDC, (int)(m_vecCoord[i].tEnd.x - tCameraPos.x),
						(int)(m_vecCoord[i].tEnd.y - tCameraPos.y));
		}

		SelectObject(hDC, hPrevPen);

	}

}

void CMapEditScene::ForeNext()
{
	++m_iFore;

	if (m_iFore >= m_vecForeGroundPos.size())
		m_iFore = 0;

	m_pStage->SetStagePos(GetForePos(m_iFore).tStart,
		GetForePos(m_iFore).tEnd);

	m_tCurrentStageSize = GET_SINGLE(CSceneManager)->GetStageSize();

	GET_SINGLE(CSceneManager)->SetLimitCamera(m_tCurrentStageSize);
}

void CMapEditScene::ForePrev()
{
	--m_iFore;

	if (m_iFore < 0)
		m_iFore = m_vecForeGroundPos.size() - 1;


	m_pStage->SetStagePos(GetForePos(m_iFore).tStart,
		GetForePos(m_iFore).tEnd);

	// ForeGround와 연관된 정보인, 스테이지 사이즈를 받아둔다.
	m_tCurrentStageSize = GET_SINGLE(CSceneManager)->GetStageSize();

	// 새로운 스테이지의 사이즈로 카메라의 움직임의 범위를 제한한다.
	GET_SINGLE(CSceneManager)->SetLimitCamera(m_tCurrentStageSize);

	// 타일은 나중에 ...


}

void CMapEditScene::BackNext()
{
	++m_iBack;

	if (m_iBack >= m_vecBackGroundPos.size())
		m_iBack = 0;

	m_pStage->SetBackGroundPos(GetBackPos(m_iBack).tStart,
		GetBackPos(m_iBack).tEnd);
}

void CMapEditScene::BackPrev()
{
	--m_iBack;

	if (m_iBack < 0)
		m_iBack = m_vecBackGroundPos.size() - 1;

	// 인덱스를 바꾸는 경우에 적용을 해주는 과정이 필요하다. 
	m_pStage->SetBackGroundPos(GetBackPos(m_iBack).tStart,
		GetBackPos(m_iBack).tEnd);
}

void CMapEditScene::SetFore(unsigned int iIndex)
{
	if (iIndex < 0 || iIndex >= m_vecForeGroundPos.size())
		iIndex = 0;

	m_iFore = iIndex;

	m_pStage->SetStagePos(GetForePos(m_iFore).tStart,
		GetForePos(m_iFore).tEnd);

	m_tCurrentStageSize = GET_SINGLE(CSceneManager)->GetStageSize();

	GET_SINGLE(CSceneManager)->SetLimitCamera(m_tCurrentStageSize);

}

void CMapEditScene::SetBack(unsigned int iIndex)
{
	if (iIndex < 0 || iIndex >= m_vecBackGroundPos.size())
		iIndex = 0;

	m_iBack = iIndex;

	m_pStage->SetBackGroundPos(GetBackPos(m_iBack).tStart,
		GetBackPos(m_iBack).tEnd);
}

void CMapEditScene::AddKey()
{
	// Camera Move 
	GET_SINGLE(CInput)->AddNormalKey("D", 'D');
	GET_SINGLE(CInput)->AddNormalKey("W", 'W');
	GET_SINGLE(CInput)->AddNormalKey("A", 'A');
	GET_SINGLE(CInput)->AddNormalKey("S", 'S');

	// 속도 증가 감소.
	GET_SINGLE(CInput)->AddNormalKey("F1", VK_F1);
	GET_SINGLE(CInput)->AddNormalKey("F2", VK_F2);

	// RENDER
	GET_SINGLE(CInput)->AddNormalKey("F3", VK_F3); 
	GET_SINGLE(CInput)->AddNormalKey("F4", VK_F4);
	GET_SINGLE(CInput)->AddNormalKey("F5", VK_F5);

	// Edge Block
	GET_SINGLE(CInput)->AddNormalKey("F6", VK_F6);

	// Line Edit 
	GET_SINGLE(CInput)->AddNormalKey("DeletePos", VK_BACK);
	GET_SINGLE(CInput)->AddNormalKey("SavePos", VK_RETURN);

	// Spawner 
	GET_SINGLE(CInput)->AddNormalKey("3", '3'); // Dir
	GET_SINGLE(CInput)->AddNormalKey("4", '4'); // Count Switch
	GET_SINGLE(CInput)->AddNormalKey("5", '5'); // ++
	GET_SINGLE(CInput)->AddNormalKey("6", '6'); //  --





	// Bind Key
	GET_SINGLE(CInput)->AddActionKey("EditModeNext", '1');
	GET_SINGLE(CInput)->AddActionKey("EditOptionNext", '2');
	GET_SINGLE(CInput)->AddActionKey("Save", 'S', ST_CTRL);
	GET_SINGLE(CInput)->AddActionKey("Load", 'L', ST_CTRL);
	GET_SINGLE(CInput)->AddActionKey("ESC", VK_ESCAPE);


	m_pInputComponent = new CInputComponent;

	m_pInputComponent->BindAction<CMapEditScene>("EditModeNext", IE_PRESS, this, &CMapEditScene::ModeChangeNext, "ModeChangeNext");
	m_pInputComponent->BindAction<CMapEditScene>("EditOptionNext", IE_PRESS, this, &CMapEditScene::OptionChangeNext, "OptionChangeNext");
	m_pInputComponent->BindAction<CMapEditScene>("Save", IE_PRESS, this, &CMapEditScene::Save, "Save");
	m_pInputComponent->BindAction<CMapEditScene>("Load", IE_PRESS, this, &CMapEditScene::Load, "Load");
	m_pInputComponent->BindAction<CMapEditScene>("Save", IE_PRESS, this, &CMapEditScene::SaveObjectPos, "SaveObjectPos");
	m_pInputComponent->BindAction<CMapEditScene>("Load", IE_PRESS, this, &CMapEditScene::LoadObjectPos, "LoadObjectPos");
	m_pInputComponent->BindAction<CMapEditScene>("ESC", IE_PRESS, this, &CMapEditScene::ClearWallTile, "ClearWallTile");

	// AddActionKey 함수를 통해서 BindActionInfo를 생성하고 
	// 키를 등록한다.

	// 여기에 m_pInputComponent 를 이용해서 Input의 BindActionInfo를 찾아서 해당 함수 정보를 등록하고,
	// 이것을 InputComponent에서 가지고 있도록 한다.

	// InputComponent가 소멸이 되는 순간에 자신의 정보를 이용하여 Input의 함수도 날려주고 있다.
}

void CMapEditScene::CameraUpdate(float fTime)
{
	GET_SINGLE(CSceneManager)->CameraUpdate(fTime);

}

bool CMapEditScene::InnerMouse() const
{
	Position tMousePos = GET_SINGLE(CInput)->GetMousePos();

	if (tMousePos.x >= 0.f && tMousePos.y >= 0.f &&
		tMousePos.x <= RS_WIDTH && tMousePos.y <= RS_HEIGHT)
		return true;
	

	return false;
}
