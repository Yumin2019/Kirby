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
	

	// BackGround�� ForeGround�� ������ �����´�. (���۷����� �о�ͼ�, �״�� ����)
	m_vecBackGroundPos = GET_SINGLE(CSceneManager)->GetBackGroundVec();
	m_vecForeGroundPos = GET_SINGLE(CSceneManager)->GetForeGroundVec();

	CStage* pStage = CObj::CreateObj<CStage>("Stage", "BackGround", this);


	// ForeGround�� �������ش�.
	pStage->SetForeGroundTexture("ForeGround", TEXT("ForeGround.bmp"));
	pStage->SetForeGroundTextureColorKey(0, 18, 127);

	pStage->SetStagePos(GetForePos(m_iFore).tStart,
		GetForePos(m_iFore).tEnd);

	// ForeGround�� ������ ������, �������� ����� �޾Ƶд�.
	m_tCurrentStageSize = GET_SINGLE(CSceneManager)->GetStageSize();

	// ���ο� ���������� ������� ī�޶��� �������� ������ �����Ѵ�.
	GET_SINGLE(CSceneManager)->SetLimitCamera(m_tCurrentStageSize);


	// �������� ����� ������� Ÿ���� �����Ѵ�.
	pStage->CreateTile((int)m_tCurrentStageSize.x / 25, (int)m_tCurrentStageSize.y / 25, Size(25.f, 25.f));


	// BackGround�� �������ش�.
	pStage->SetTexture("BackGround", TEXT("BackGround.bmp"));
	pStage->SetBackGroundPos(GetBackPos(m_iBack).tStart,
		GetBackPos(m_iBack).tEnd);


	m_pStage = pStage;

	m_pStage->SetTileRender(true);

	m_tPrevStageSize = m_tCurrentStageSize;

	AddKey();


	// ������ ��.
	m_tTileSize = Size(25.f, 25.f);


	return true;
}

void CMapEditScene::Update(float fTime)
{
	// ���콺�� ȭ������� ������, ó������ �ʴ´�.
	if (!GET_SINGLE(CInput)->MouseIsScreen())
	{
		m_eState = ES_NONE;
		return;
	}

	m_tMousePos = GET_SINGLE(CInput)->GetMousePos();


	if(m_eState != ES_DRAG)
		CameraUpdate(fTime);

	// �巡������ ��� ī�޶��� ��ġ�� ����.

	Position tCameraPos = GET_SINGLE(CSceneManager)->GetCameraPos();

	if (KEYDOWN("MouseLButton"))
	{
		m_eState = ES_CLICK;

		if (m_eMode == EM_TILE_OPTION && m_bTileRender)
		{

			// ���콺�� ���� ������ �ش� ���콺�� ��ġ�� �������� Ÿ���� ��ġ�� �˾Ƴ���.

			// �̷��� �˾ƿ� ��ġ�� Ŭ���̾�Ʈ ������ ���콺 ��ǥ�̴�.

			int iIndex = m_pStage->GetTileIndex(m_tMousePos + tCameraPos);

			// ���� �ɼ� ���� ���� ó���� �Ѵ�.

			if (m_eTileOption == TO_NORMAL)
			{
				m_pStage->ChangeTileOption(iIndex, TO_NORMAL);
			}
			else
			{
				if (m_pStage->GetTileOption(iIndex) == TO_WALL)
				{
					// �� �ɼ��� ���·� ������ ���� Ÿ���� ������.
					m_pStage->ChangeTileOption(iIndex, TO_NORMAL);

				}
				else
				{
					// ���� ���� Normal �����̴�.
					m_pStage->ChangeTileOption(iIndex, TO_WALL);

				}
			}
		}
		else if (m_eMode == EM_LINE && m_bLineRender)
		{
			// SRC�� ��ġ�� �޴´�.
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
		
		// BACKGROUND, FOREGROUND �� ��쿡��  ���콺 ó���� �����ߴµ�, �̰��� ���ϼ��� ��ġ��,
		// �������� �߰����� ����̴�. ������ �̰��� �߰������ν� ����� ����� �������� �ξ� ��������..
		//else if (m_eMode == EM_BACKGROUND)
		//{
		//	// Mouse�� ��ġ�� ������ �Ѿ�� �������� ���� �Ǵ��Ѵ�. 
		//	// �ٸ� �ɼ� ���������� ������� ���ϴ� ����ε�, ���� ������ �ִ� �� ��忡���� ����� ����ϴ� ���� �ڿ�������.
		//	// ���� ��� Ÿ�� �ɼ��� �����ϴµ�, ���⼭ ȭ���� �̿��ؼ� �ɼǰ��� �ٲٴ� ���� ���ڿ�������..

		//	if (m_tMousePos.x <= RS_WIDTH / 2.f)
		//	{
		//		// ���� : ����
		//		BackPrev();
		//	}
		//	else
		//	{
		//		// ������ : ����
		//		BackNext();
		//	}
		//}

		//else if (m_eMode == EM_FOREGROUND)
		//{
		//	if (m_tMousePos.x <= RS_WIDTH / 2.f)
		//	{
		//		// ���� : ����
		//		ForePrev();
		//	}
		//	else
		//	{
		//		// ������ : ����
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

		// ���࿡ ���콺�� ȭ������� ������ UP�� �ϴ� ��쿡 �ε����� ������ ��� ���̴�.
		// �̷� ��찡 ���� ���ɼ��� �ִ°� ?
		// �߰��� Drag���¿��� ���������� ���. ������ ó���� �Ǳ� ������ ó������ �ʴ´�.
		// ��, �ε��� Out�� ���� ����.

		// �巡���ϴ� �߿� �� ���.
		
		if (m_eMode == EM_TILE_OPTION && m_eState == ES_DRAG && m_bTileRender)
		{

			// Src�� Dest�� World��ǥ�� ����.
			Position tSrcPos = m_tClickPos + tCameraPos;
			Position tDestPos = m_tMousePos + tCameraPos;


			// Rect�� �浹������ �ش��ϴ� �ε����� ���س���.
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

					// Rect�� �ε����� ���ؼ� ��� �κп� Option�� �Ȱ��� �ش�.
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
		// ���� ���콺�� ��ġ�� ���´�. m_tMousePos
		// ó�� Ŭ���� ��ġ�� m_tClickPos  �Ѵ� Ŭ���̾�Ʈ ������ ��ǥ.
		// �״�� ����ϸ� �ȴ�.

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

	// ���� ���
	for (int i = 0; i < iSizeX; ++i)
	{
		m_pStage->ChangeTileOption(i, TO_WALL);
	}

	// ���� �ϴ�
	for (int i = 0; i < iSizeX; ++i)
	{
		int iIndex = iSizeX * (iSizeY - 1) + i;

		m_pStage->ChangeTileOption(iIndex, TO_WALL);
	}

	// ����
	for (int y = 0; y < iSizeY; ++y)
	{
		int iIndex = y * iSizeX;
		m_pStage->ChangeTileOption(iIndex, TO_WALL);

	}

	// ������
	for (int y = 0; y < iSizeY; ++y)
	{
		int iIndex = y * iSizeX + (iSizeX - 1);
		m_pStage->ChangeTileOption(iIndex, TO_WALL);

	}
}

void CMapEditScene::NormalInput(float fTime)
{

	// Spawner Option Key

	if (m_eMode == EM_SPAWNER && m_bSpawnerRender) // ���δٴ� �����Ͽ� �ٲ۴�.
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
				// ������ ���Ѵ�� ������ �Ǿ��ٸ�, �̰��� ������ ���ؼ� 0���� �����Ѵ�.
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

	// Ÿ�� �ɼ� / Ÿ�� ���̰�, f6Ű. �ѷ�ġ��.
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

		// ���� Ÿ���� ������ Stage���� ������ְ� �ִ�.
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
	

	// �����ϴ� Ű / �����ϴ� Ű ( ���� ó���� ��忡 ���� �޶�����. )

	if (KEYDOWN("SavePos"))
	{
		ObjectPosSave();

		switch (m_eMode)
		{
		case EM_TILE_OPTION: // �ɼ� �������, ó������ ����.
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
		// ���¿� ���� �츮�� �ֱٿ� �߰��� ���� �����Ѵ�.
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

	case EM_BACKGROUND: // 2 ��쿡�� ������ ���� Ű�� ���� ó���� �ϱ� ���ؼ� �̰����� ó���� ���� �ʴ´�.
	case EM_FOREGROUND:
	case EM_LINE: // Line��忡���� ����.
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

	tOFN.lStructSize = sizeof(OPENFILENAME); // ����ü�� ����� ���Ѵ�.
	tOFN.hwndOwner = WINDOWHANDLE; // ������ �Ǵ� �ڵ��� �ѱ��.
	tOFN.lpstrFilter = TEXT("Stage File(*.stg)\0*.stg\0All Files(*.*)\0*.*\0");
	// ���� ������ �޺� �ڽ��� ��Ÿ�� ���͵��� ��Ÿ����. �� ���ڸ� �������� �ϱ⿡ 
	// ��������\0���Ϳ� ���� ����Ѵ�.
	// �޺��ڽ��� ������� ��Ÿ����. �������� ������ �� �ִ�.

	tOFN.lpstrFile = strPath;
	// ���� �̸� ����Ʈ���� ó�� ��Ÿ�� ���ϸ��� �����ϴ� �뵵�� ����ϸ�, ���������δ�
	// ����ڰ� ������ ������ �̸��� ��ȯ�ϴ� �뵵�� ���ȴ�.
	// ��, GetSaveFileName �� ���ļ� ����� ������ ��θ� �˾ƿ� ���Ŀ�
	// �̰�(strPath)�� ����ϴ� ���̴�.


	// lpstrFileTile�� ������ �̸��� �����ޱ� ���� �����ε�, �̰��� ���̸� maxPath�� �Ѵ�.
	tOFN.nMaxFile = MAX_PATH;

	tOFN.lpstrInitialDir = TEXT("..\\Bin\\");
	// ���� ã�⸦ ������ ���丮�� �����Ѵ�. ���� ������Ʈ�� ��ġ���� ../Bin(�������)/

	ShowCursor(TRUE);

	if (GetSaveFileName(&tOFN) != 0)
	{
		// 0�� �ƴϸ� Ǯ��ΰ� �����µ�, �� ��θ� �̿��ؼ� ������ ������.
		// Unicode -> Multibyte

		char strFullPath[MAX_PATH] = {};

		// �ٲ� ���̸� ���´�.
		int iLength = WideCharToMultiByte(CP_ACP, 0, strPath, 
			-1, nullptr, 0, nullptr, nullptr);

		WideCharToMultiByte(CP_ACP, 0, strPath, -1, strFullPath, iLength, nullptr, nullptr);
		// ������� ��θ� �̿��ؼ� Stage�� �����Ѵ�.

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

	tOFN.lStructSize = sizeof(OPENFILENAME); // ����ü�� ����� ���Ѵ�.
	tOFN.hwndOwner = WINDOWHANDLE; // ������ �Ǵ� �ڵ��� �ѱ��.
	tOFN.lpstrFilter = TEXT("Stage File(*.stg)\0*.stg\0All Files(*.*)\0*.*\0");
	// ���� ������ �޺� �ڽ��� ��Ÿ�� ���͵��� ��Ÿ����. �� ���ڸ� �������� �ϱ⿡ 
	// ��������\0���Ϳ� ���� ����Ѵ�.
	// �޺��ڽ��� ������� ��Ÿ����. �������� ������ �� �ִ�.

	tOFN.lpstrFile = strPath;
	tOFN.nMaxFile = MAX_PATH;
	tOFN.lpstrInitialDir = TEXT("..\\Bin\\");

	ShowCursor(TRUE);

	if (GetOpenFileName(&tOFN) != 0)
	{
		// 0�� �ƴϸ� Ǯ��ΰ� �����µ�, �� ��θ� �̿��ؼ� ������ ������.
		// Unicode -> Multibyte

		char strFullPath[MAX_PATH] = {};

		// �ٲ� ���̸� ���´�.
		int iLength = WideCharToMultiByte(CP_ACP, 0, strPath,
			-1, nullptr, 0, nullptr, nullptr);

		WideCharToMultiByte(CP_ACP, 0, strPath, -1, strFullPath, iLength, nullptr, nullptr);
		// ������� ��θ� �̿��ؼ� Stage�� �����Ѵ�.

		m_pStage->Load(strFullPath);

	}

	ShowCursor(FALSE);
}

void CMapEditScene::SaveObjectPos(float fTime)
{
	// LineCollider�� Spawner�� ������ �����Ѵ�.
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

	fopen_s(&pFile, pFileName, "wt"); // ������ ���ϰ� �ϱ� ���ؼ� txt���Ϸ� �Ѵ�.

	if (!pFile)
		return;

	// BackGround�� �ε����� ForeGround�� �ε����� �����Ѵ�. 
	fprintf(pFile, "BackGround : %d / ForeGround : %d\n", m_iBack + 1, m_iFore + 1);

	// ���� ��ġ�� Door�� ��ġ�� �����Ѵ�.
	fprintf(pFile, "Start : (%d, %d) / Door : (%d, %d)\n", (int)m_tStartPos.x, (int)m_tStartPos.y,
		(int)m_tDoorPos.x, (int)m_tDoorPos.y);

	fprintf(pFile, " == Line Collider == \n");
	int iSize = m_vecCoord.size();
	fprintf(pFile, "%d\n", iSize);

	// ���� ��ŭ �����Ѵ�.
	for (int i = 0; i < iSize; ++i)
	{
		fprintf(pFile, "src : (%d, %d) / dest : (%d, %d)\n",
			(int)m_vecCoord[i].tStart.x, (int)m_vecCoord[i].tStart.y,
			(int)m_vecCoord[i].tEnd.x, (int)m_vecCoord[i].tEnd.y);
	}
	// �����ʸ� �����Ѵ�.
	fprintf(pFile, " == Spawner == \n");
	iSize = m_vecSpawner.size();
	fprintf(pFile, "%d\n", iSize);

	// �������� ���� ��ŭ �����Ѵ�.
	for (int i = 0; i < iSize; ++i)
	{
		int strSize = m_vecSpawner[i].strMonsterTag.length();
		// ������ �̸��� ������ �����Ѵ�.
		fprintf(pFile, "Length : %d\n", strSize);

		// �̸��� ������ŭ ����.
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

	fopen_s(&pFile, pFileName, "rt"); // ������ ���ϰ� �ϱ� ���ؼ� txt���Ϸ� �Ѵ�.

	if (!pFile)
		return;

	// BackGround�� ���� ForeGround�� ���� �о�´�.
	fscanf_s(pFile, "BackGround : %d / ForeGround : %d\n", &m_iBack, &m_iFore);

	--m_iBack;
	--m_iFore;
	// ���⼭ �о�� ���� �ε����� �ƴϴ�.

	SetBack(m_iBack);
	SetFore(m_iFore);

	int srcX = 0, srcY = 0, destX = 0, destY = 0;

	// ���� ������ ���� ��ġ�� �о�´�.
	fscanf_s(pFile, "Start : (%d, %d) / Door : (%d, %d)\n", &srcX, &srcY,
		&destX, &destY);

	m_tStartPos = Position(srcX, srcY);
	m_tDoorPos = Position(destX, destY);

	char strFileName[100] = {};

	fgets(strFileName, 100, pFile); // == Line ==

	int iSize = 0;
	// ������ �о�´�.

	fscanf_s(pFile, "%d\n", &iSize);

	m_vecCoord.clear();

	// ���� ��ŭ �о�´�..
	for (int i = 0; i < iSize; ++i)
	{
		srcX = 0, srcY = 0, destX = 0, destY = 0;

		fscanf_s(pFile, "src : (%d, %d) / dest : (%d, %d)\n", &srcX, &srcY,
			&destX, &destY);

		m_vecCoord.push_back(ClipCoord(srcX, srcY, destX, destY));
	}

	// �����ʸ� �о�´�.
	fgets(strFileName, 100, pFile); // == Spawner == 

	// ����� �д´�.
	iSize = 0;
	fscanf_s(pFile, "%d\n", &iSize);

	m_vecSpawner.clear();

	// �������� ���� ��ŭ �о�´�.

	for (int i = 0; i < iSize; ++i)
	{
		SPAWNERINFO tInfo;

		int X, Y;
		int iDir = 0;
		
		int strSize = 0;
		char strLine[100] = {};

		// ������ �̸��� ������ �д´�.
		fscanf_s(pFile, "Length : %d\n", &strSize);

		// �̸��� ������ŭ �д´�
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
		//  ���� ����ϰ� �ִ� �ؽ�ó�� �ʱ���·� �ٲٴµ�, �̰��� �� ������ ó�� �ƴѰ� ? 
		// �����ϳ� �ɾ����.

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
			if (MessageBox(nullptr, TEXT("���� �����Ͻðڽ��ϱ� ?"),
				TEXT("Wall Tile"), MB_YESNO) == IDYES)
			{
				m_pStage->AllTileOptionChange(TO_NORMAL);
			}
		}
		break;

	case EM_POSITION:

		if (MessageBox(nullptr, TEXT("�ʱ� ��ġ�� �ű�ðڽ��ϱ� ?"),
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
			if (MessageBox(nullptr, TEXT("���� �����Ͻðڽ��ϱ� ?"),
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
			if (MessageBox(nullptr, TEXT("���� �����Ͻðڽ��ϱ� ?"),
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

	// ���콺�� ��ġ�� ������ִ� ������ �Ѵ�.
	// �ϴ�, �� ���� Ȯ���ϰ� �� ��. MapEditScene�� ī�޶� ������ �������� ũ��� �����־
	// �������� ũ�� ��ŭ�� �̵��� �� �ִ�. ��, �츮�� �����̴� ��ǥ�� ���������� ������ǥ�� ���̴�.

	Position tCameraPos = GET_SINGLE(CSceneManager)->GetCameraPos();
	Position tMousePos = GET_SINGLE(CInput)->GetMousePos();
	Position tMouseWorldPos = tCameraPos + tMousePos;

	TCHAR strPos[40] = {};
	Position tRenderPos(20, 20);

	if (InnerMouse())
	{
		// ȭ�� �ȿ� �ִ� ���콺 ��ǥ�� ������ش�.
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

		// �Ʒ��� �� ���� �ɼ��� �����Ͽ� ����ϴ� �������� ���� �ʰ�, �׳� �Ϲ������� ������ִ� ������ �Ѵ�.
		// Ÿ��ó�� �ɼ��� �����ϴ� ������ �ƴϴ�. ���� 
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

		// ���� �±�
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
		
		// ���� ����
		switch (m_eSpawnerDir)
		{
		case SD_LEFT:
			TextOut(hDC, (int)tRenderPos.x, (int)tRenderPos.y + 60, TEXT("SpawnDir : Left"), lstrlen(TEXT("SpawnDir : Left")));

			break;

		case SD_RIGHT:
			TextOut(hDC, (int)tRenderPos.x, (int)tRenderPos.y + 60, TEXT("SpawnDir : Right"), lstrlen(TEXT("SpawnDir : Right")));

			break;
		}

		// ������ ������ �����ϴ� ����̸�, �̰��� �����ش�.
		if (m_bSpawnCount)
		{
			wsprintf(strPos, TEXT("Limit Count : %d"), m_iSpawnCount);
			TextOut(hDC, (int)tRenderPos.x, (int)tRenderPos.y + 80, strPos, lstrlen(strPos));

			// ������ ������ ���� �ִ� ��쿡�� �̰��� �����ֵ��� �Ѵ�.
		}
		else
		{
			// �������� �ʴٸ� max
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
	// ���� �ֱٿ� �߰��� LineSave�� �����Ѵ�.
	// �� �Լ��� ȣ���ϱ� ������ ���콺�� �ܺο� �����Ѵٸ� 
	// return�� �ȴ�.

	// �޼��� �ڽ��� ��ġ ������ ����� ��ġ�� ��찡 �ִ�. ����, �̰��� ���� ���� �ʱⰪ������ Ȯ���ϸ� �ȴ�.

	switch (m_eMode)
	{
	case EM_FOREGROUND:
		// ���� �������� �ε����� �������� ����� �����ϴ� ���̴�.

		// ������ ó���̹Ƿ� MessageBox�� ���θ� �޴´�.
		if (MessageBox(nullptr, TEXT("���� ForeGround�� �����Ͻðڽ��ϱ� ? (Tile Clear)"), TEXT("New Tile Set"), MB_YESNO) == IDYES)
		{
			// ���� ������ �������� ������� (ForeGround�� ��ġ�� �ٲٸ鼭 �������� ����� �ٲ��ִ� �ý����� �Ǿ��ִ�)
			// Tile�� �����ϴ� �۾��� ��ģ��.

			// �׳� CreateTile�Լ��� ����ϸ� �ȴ�. (���������� �����ϴ� ������ ��ģ��.)

			// �������� ����� ������� Ÿ���� �����Ѵ�.
			m_pStage->CreateTile((int)m_tCurrentStageSize.x / 25,
				(int)m_tCurrentStageSize.y / 25, Size(25.f, 25.f));

			// �ʱⰪ���� Ÿ���� ������ ���缭 ������ �ȴ�.
			// �� ������ �ȴ� ! 

		}

		break;

	case EM_BACKGROUND:
		// �ƹ� ó���� ���� �ʴ´�. (�׳� �ε����� �ٲٸ� �װ� ������...)
		break;

	case EM_TILE_OPTION:
		break;

	case EM_POSITION:

		// Pos�� ���� ��ġ�� Start Or Door���� �ش�.

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
			// Pos�� �ʱⰪ���� .
		}

		

		break;

	case EM_LINE:
		if(m_bLineRender)
			m_vecCoord.push_back(ClipCoord(m_tSrc, m_tDest));

		break;

	case EM_SPAWNER:
		// ���� ������ �ɼǿ� ���� �޶����� �κ��̴�.
		// ���� ������ �������� ���⵵ �޴´�.

		// �츮�� ������ �ɼǿ� ����, �̹� ������ �±� ���� �����ִ�.

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
		// �⺻������ �ɷ� �ִ� ���Ϳ� ���� �������� �Ѵ�.
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
		//  ���� ����ϰ� �ִ� �ؽ�ó�� �ʱ���·� �ٲٴµ�, �̰��� �� ������ ó�� �ƴѰ� ? 
		// �����ϳ� �ɾ����.

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

		// ���� �����ִ� ��ġ�� ���ֹ�����. (����� ���� Pos ����)
		// ��Ȯ�ϰԴ� �ʱ���ġ��... 

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
	

	// �̸� �׸��� ��� ! 
	switch(m_eMode)
	{
		// �� ��쿡�� ����� ���� ����.
	case EM_FOREGROUND:
	case EM_BACKGROUND:
	case EM_TILE_OPTION:
		break;


		
	case EM_LINE:

		if (m_bLineRender)
		{
			// ��尡 ������ ��쿡�� ���� �׸��� �ִ� ... src�� dest�� Render���ش�.
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
			// ������ �÷��� �ϴ� ��Ȳ���� ������ �Ͱ� �����ϰ� 0.5 0.5 �Ǻ����� 50px��.

			// 0.f 0.f�� ���� ���� ������ ģ��. = �׳� �׸��� �ʴ� ��ġ�� ���ص� ���̴�. 
			// �ʱ�ȭ������..
			if (m_tSpawnerPos.x != 0.f && m_tSpawnerPos.y != 0.f)
			{
				// �ش� ��ġ�� �������� �Ͽ� 50px�� �׷��ش�.
				RECT tRect = { (int)(m_tSpawnerPos.x - 50.f - tCameraPos.x), 
					(int)(m_tSpawnerPos.y - 50.f - tCameraPos.y),
					(int)(m_tSpawnerPos.x + 50.f - tCameraPos.x),
					(int)(m_tSpawnerPos.y + 50.f - tCameraPos.y)
				};

				FrameRect(hDC, &tRect, MAGENTABRUSH);

				// ��ġ�� ����ϵ� Ȯ���� �ƴϴϱ�, ������ ������� �ʴ´�. (���ʿ� ���� ���� ���ݾ�)
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

		// DOOR ��ġ�� ����Ѵ�.
		tRect.left =		(int)(m_tDoorPos.x - tCameraPos.x - 60.f);
		tRect.right =		(int)(m_tDoorPos.x - tCameraPos.x + 60.f);
		tRect.top =			(int)(m_tDoorPos.y - tCameraPos.y - 77.f);
		tRect.bottom =		(int)(m_tDoorPos.y - tCameraPos.y + 77.f);

		FrameRect(hDC, &tRect, REDBRUSH);
		TextOut(hDC, (int)(m_tDoorPos.x - tCameraPos.x - 20.f), (int)(m_tDoorPos.y - tCameraPos.y), TEXT("Door"), lstrlen(TEXT("Door")));
	}

	
	// ������ vector�� ������ �� �ִٸ�...
	if (!m_vecSpawner.empty() && m_bSpawnerRender)
	{
		// ����ؾ� �ϴ� ���� : �������� ��ġ(�⺻����) dd  ���� ����... dd
		// ������ ���� ���� dd Count�� ������ش�. dd

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

			// ���� ���͸� ����Ѵ�.
			wsprintf(strPos, TEXT("Monster : %s"),
				StringToWstring(m_vecSpawner[i].strMonsterTag).c_str());
			TextOut(hDC, (int)(tPos.x + 60.f), (int)(tPos.y - 50.f), strPos, lstrlen(strPos));


			// ��ġ�� ����Ѵ�.
			wsprintf(strPos, TEXT("Pos : (%d, %d)"), (int)m_vecSpawner[i].tPos.x, (int)m_vecSpawner[i].tPos.y);
			TextOut(hDC, (int)(tPos.x + 60.f), (int)(tPos.y -30.f), strPos, lstrlen(strPos));

			// ���� ������ ����Ѵ�.
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

			// Count�� ����Ѵ�.
			if (m_vecSpawner[i].bCount)
			{
				// ����ϴ� ��쿡�� �ش� ���ڸ�.

				wsprintf(strPos, TEXT("Count : %d"), m_vecSpawner[i].iCount);
				TextOut(hDC, (int)(tPos.x + 60.f), (int)(tPos.y + 10.f), strPos, lstrlen(strPos));
			}
			else
			{
				// ������� �ʴ� ��쿡�� Max���� ����Ѵ�.
				TextOut(hDC, (int)(tPos.x + 60.f), (int)(tPos.y + 10.f),
					TEXT("Count : Max"), lstrlen(TEXT("Count : Max")));

			}

			RECT tRect = { (int)tPos.x - 50, (int)tPos.y - 50, (int)tPos.x + 50, (int)tPos.y + 50 };
			FrameRect(hDC, &tRect, REDBRUSH);
		}
			

	}


	// ���� vector�� ������ �ִٸ�...
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

			// �ش� ��ġ�� Line�� ������ְ�, ��ġ�� ǥ���Ѵ�.

			Position tRenderPos = m_vecCoord[i].tEnd;

			if (tRenderPos.y > m_vecCoord[i].tStart.y)
			{
				// ���࿡ ����ϰ��� �ϴ� Y��ǥ(End)�� Start ���� �ؿ� �ִٸ�,
				// Start�� �������� ����Ѵ�.
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

	// ForeGround�� ������ ������, �������� ����� �޾Ƶд�.
	m_tCurrentStageSize = GET_SINGLE(CSceneManager)->GetStageSize();

	// ���ο� ���������� ������� ī�޶��� �������� ������ �����Ѵ�.
	GET_SINGLE(CSceneManager)->SetLimitCamera(m_tCurrentStageSize);

	// Ÿ���� ���߿� ...


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

	// �ε����� �ٲٴ� ��쿡 ������ ���ִ� ������ �ʿ��ϴ�. 
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

	// �ӵ� ���� ����.
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

	// AddActionKey �Լ��� ���ؼ� BindActionInfo�� �����ϰ� 
	// Ű�� ����Ѵ�.

	// ���⿡ m_pInputComponent �� �̿��ؼ� Input�� BindActionInfo�� ã�Ƽ� �ش� �Լ� ������ ����ϰ�,
	// �̰��� InputComponent���� ������ �ֵ��� �Ѵ�.

	// InputComponent�� �Ҹ��� �Ǵ� ������ �ڽ��� ������ �̿��Ͽ� Input�� �Լ��� �����ְ� �ִ�.
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
