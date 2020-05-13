#include "Stage.h"
#include "../Texture.h"
#include "../Core/ResourceManager.h"
#include "../Core/SceneManager.h"
#include "../Core/Input.h"
#include "../Object/Player.h"
#include "../Object/Spawner.h"
#include "../Core/GameManager.h"
#include "../Sound/SoundManager.h"

// Tile
#include "../Tile.h"

CStage::CStage()
{
	m_pForeGroundTexture = nullptr;
	m_fBackGroundSpeed = 0.f;
	m_pPlayer = nullptr;
	m_bOutOfRange = false;

	// Tile
	m_pTileList = nullptr;
	m_iNumX = 0;
	m_iNumY = 0;
	m_iTileCount = 0;
	m_iStartX = 0;
	m_iStartY = 0;
	m_iEndX = 0;
	m_iEndY = 0;
	m_bTileRender = false;
	m_bBackGround = true;

	m_eObjectType = OT_STAGE;

}


CStage::CStage(const CStage & obj) :
	CCharacter(obj)
{
}

CStage::~CStage()
{
	// TileCount만큼의 Tile* 가 할당이 되어 있다.
	for (int i = 0; i < m_iTileCount; ++i)
	{
		SAFE_RELEASE(m_pTileList[i]);
	}

	SAFE_DELETE_ARRAY(m_pTileList);


	SAFE_RELEASE(m_pForeGroundTexture);
	SAFE_RELEASE(m_pPlayer);

	Safe_Release_VecList(m_vecSpawnerList);
}

void CStage::Save(const char * pFileName)
{
	FILE* pFile = nullptr;

	fopen_s(&pFile, pFileName, "wb");

	fwrite(&m_iNumX, sizeof(int), 1, pFile);
	fwrite(&m_iNumY, sizeof(int), 1, pFile);
	fwrite(&m_iTileCount, sizeof(int), 1, pFile);
	fwrite(&m_iStartX, sizeof(int), 1, pFile);
	fwrite(&m_iStartY, sizeof(int), 1, pFile);
	fwrite(&m_iEndX, sizeof(int), 1, pFile);
	fwrite(&m_iEndY, sizeof(int), 1, pFile);
	fwrite(&m_tTileSize, sizeof(Size), 1, pFile);

	// 위치정보.
	fwrite(&m_tPos, sizeof(Position), 1, pFile);


	for (int i = 0; i < m_iTileCount; ++i)
	{
		m_pTileList[i]->Save(pFile);
	}

	fclose(pFile);
}

void CStage::Load(const char * pFileName)
{

	for (int i = 0; i < m_iTileCount; ++i)
	{
		SAFE_RELEASE(m_pTileList[i]);
	}

	SAFE_DELETE_ARRAY(m_pTileList);


	FILE* pFile = nullptr;

	fopen_s(&pFile, pFileName, "rb");

	fread(&m_iNumX, sizeof(int), 1, pFile);
	fread(&m_iNumY, sizeof(int), 1, pFile);
	fread(&m_iTileCount, sizeof(int), 1, pFile);
	fread(&m_iStartX, sizeof(int), 1, pFile);
	fread(&m_iStartY, sizeof(int), 1, pFile);
	fread(&m_iEndX, sizeof(int), 1, pFile);
	fread(&m_iEndY, sizeof(int), 1, pFile);
	fread(&m_tTileSize, sizeof(Size), 1, pFile);

	// 위치정보.
	fread(&m_tPos, sizeof(Position), 1, pFile);

	

	m_pTileList = new CTile*[m_iTileCount];

	for (int i = 0; i < m_iTileCount; ++i)
	{
		m_pTileList[i] = new CTile;
		m_pTileList[i]->Load(pFile);
	}

	fclose(pFile);
}





void CStage::AddSpawner(const string & strSpawnerTag, float PosX, float PosY,
	SIDE_DIR eSpawnDir, const string & strSpawnMonsterTag, bool bCount, int iCount)
{
	CSpawner* pSpawner = new CSpawner;

	pSpawner->SetTag(strSpawnerTag);
	pSpawner->SetSpawner(PosX, PosY, eSpawnDir, strSpawnMonsterTag, bCount, iCount);
	pSpawner->m_pScene = m_pScene;

	m_vecSpawnerList.push_back(pSpawner);
}



void CStage::SetTileRender(bool bRender)
{
	m_bTileRender = bRender;


	if (GET_SINGLE(CGameManager)->GetGameMode() == GM_PLAY)
	{
		GET_SINGLE(CGameManager)->SetPause(bRender);

		// 타일이 켜지면, 시간도 멈추고, 타일이 꺼지면, 시간도 다시 흐르고 .. 근데..
		// 이 처리는 전적으로 playMode에서만 사용하는 것이다.

		if (!bRender)
		{
			// 근데 이 처리는 Play 모드에서만 셋팅을 하도록 한다.
			GET_SINGLE(CSceneManager)->GravityTimeClear("Default");

			GET_SINGLE(CSoundManager)->Stop(CT_BGM);

			// 노래 다시 시작.
			GET_SINGLE(CSoundManager)->Play(m_pScene->GetBgmName(), CT_BGM);
		}
		else
		{
			// Render = true 멈춘다는 소리.
			GET_SINGLE(CSoundManager)->Stop(CT_BGM);
			GET_SINGLE(CSoundManager)->Stop(CT_ATTACK);
			GET_SINGLE(CSoundManager)->Stop(CT_STATE);

			GET_SINGLE(CSoundManager)->Play("Pause", CT_EFFECT);

		}
	}
}

TILE_OPTION CStage::GetTileOption(int iIndex)
{
	if (iIndex < 0 || iIndex >= m_iTileCount)
		return TO_NORMAL;

	return m_pTileList[iIndex]->GetTileOption();
}

bool CStage::CreateTile(int iNumX, int iNumY, const Size & tTileSize,
	const string & strTileTexKey, 
	const TCHAR * pBaseTile, const string & strPath)
{

	if (m_pTileList)
	{
		for (int i = 0; i < m_iTileCount; ++i)
		{
			SAFE_RELEASE(m_pTileList[i]);
		}
	}

	SAFE_DELETE_ARRAY(m_pTileList);

	// 기존에 있을 가능성이 있으니 날린다.

	m_iNumX = iNumX;
	m_iNumY = iNumY;
	m_tTileSize = tTileSize;
	m_iTileCount = iNumX * iNumY;
	SetSize(iNumX * tTileSize.x, iNumY * tTileSize.y);
	// 타일의 개수에 따른 사이즈를 미리 구해둔다.


	m_pTileList = new CTile*[m_iTileCount];

	for (int y = 0; y < m_iNumY; ++y)
	{
		for (int x = 0; x < m_iNumX; ++x)
		{
			int iIndex = y * m_iNumX + x;

			m_pTileList[iIndex] = new CTile;
			// 하나씩 타일을 순회하면서 동적할당을 해준다.

			m_pTileList[iIndex]->Init();
			// 새로 생긴 타일에 대하여 초기화함수를 호출해주고.

			m_pTileList[iIndex]->SetSize(m_tTileSize);
			// 사이즈 또한 설정해주며...

			m_pTileList[iIndex]->SetPos(x * m_tTileSize.x,
				y * m_tTileSize.y);
			// 위치를 설정한다.
			// 위치를 설정하는 방식은 2차원배열을 생각하면 동일하다.

			// 이건 개인적인 생각인데, 스테이지에만 타일을 깔아야 한다고 가정하면.
			// 스테이지의 시작 위치부터 타일을 깔아주는 것이 맞지 않을까 ?
			// (당연하지.. 근데 0, 0으로 보통은 사용할거잖아)

			if(pBaseTile)
				m_pTileList[iIndex]->SetTexture(strTileTexKey, pBaseTile, strPath);
			// 기본 타일의 이미지를 설정하는 기능이다.
			// Load해서 사용할 타일의 이름과 파일의 이름, 텍스트의 경로를 받으면 
			// 기본 타일로 지정해주고 있다.
			// 기본타일에 대해서 한번 Load가 일어난 경우에 그 이후부터는
			// 같은 이름에 대한 텍스처에 대한 참조가 일어나므로 레퍼런스 값만 증가하는 형태이다.
			
		}
	}

	return true;
}

int CStage::ChangeTileOption(Position tPos, TILE_OPTION eOption)
{
	int iIndex = GetTileIndex(tPos);

	if (iIndex == -1)
		return -1;

	m_pTileList[iIndex]->SetTileOption(eOption);

	// 해당 위치의 타일을 얻어와서 셋팅.

	return iIndex;
}

void CStage::ChangeTileOption(int iIndex, TILE_OPTION eOption)
{
	if (iIndex <= -1 || iIndex >= m_iTileCount)
		return;


	m_pTileList[iIndex]->SetTileOption(eOption);

}

CTile * CStage::GetTile(const Position & tPos)
{
	int iIndex = GetTileIndex(tPos);

	if (iIndex == -1)
		return nullptr;

	return m_pTileList[iIndex];

	// 못찾으면 null 찾으면 해당 인덱스에 맞는 Tile*을 반환.
}

int CStage::GetTileIndexX(float x)
{
	x -= m_tPos.x;

	// 스테이지의 시작좌표가 0이 아닌 경우를 생각해서 시작 위치를 빼주는 것이다.
	// 시작 위치를 기반으로 하여 상대적인 위치를 확인하기 위함이다.
	// 시작 위치가 50, 50일때, 이 좌표를 상대적인 위치로 사용하기 위해서
	// 0, 0으로 만든다.
	// 그러면 다른 좌표들도 -50, -50 만큼 이동을 시켜주는 것이다.

	// 이렇게 구한 상대적인 위치 x를 타일 사이즈 x만큼 나눠주면 인덱스를 구할 수 있다.
	x /= m_tTileSize.x;

	int iX = (int)x;

	if (iX < 0 || iX >= m_iNumX)
		return -1;

	// 이렇게 구한 인덱스가 0보다 작거나, 혹은 NumX이상이라면 잘못된 것이다. (0부터 시작)
	// 예를 들면, 스테이지의 시작 위치 이전을 클릭했다면, 인덱스가 잘못나올 것이다.
	// 스테이지를 넘어가는 곳의 x 좌표가 들어온 경우에도 마지막 x 인덱스를 넘어가게 될 것이다.
	// -1리턴.

	return iX;
}

int CStage::GetTileIndexY(float y)
{
	// y도 동일하다.
	y -= m_tPos.y;

	y /= m_tTileSize.y;

	int iY = (int)y;

	if (iY < 0 || iY >= m_iNumY)
		return -1;


	return iY;
}

int CStage::GetTileIndex(Position tPos)
{

	tPos -= m_tPos;

	tPos /= m_tTileSize;

	// 스테이지를 기준으로 상대적인 위치를 구하고, 해당 위치를 타일 사이즈로 나눠버린다.
	// 우리는 결과로 x, y의 인덱스를 얻게 된다.

	int iX = (int)tPos.x;
	int iY = (int)tPos.y;

	if (iX < 0 || iY < 0 || iX >= m_iNumX || iY >= m_iNumY)
		return -1;

	// 제대로 인덱스가 잡히게 된 경우에 1차원 배열의 형태로 바꿔서 return.

	return iY * m_iNumX + iX;
}

bool CStage::CheckWallW(float & fResult, RectInfo & tTileRC,
	CObj * pObj, int iDir)
{
	if (iDir == 0)
		return false;

	// 넘겨진 방향에서 Move.x = 0즉, 움직이지 않은 상황이라면 return.

	Position tPos = pObj->GetPos();
	Position tPivot = pObj->GetPivot();
	Size tSize = pObj->GetStaticSize();

	// 넘겨진 오브젝트의 위치와, Pivot정보, StaticSize를 통해서 사각형 정보를 만든다.

	Position tStart, tEnd;

	// 왼쪽으로 이동하는 경우 : 사각형의 왼쪽을 확인하면 된다.
	if (iDir == -1)
	{
		tStart = tPos - tPivot * tSize; // 오브젝트의 왼쪽 상단을 구한다.
		tEnd = tPos + (Position(1.f, 1.f) - tPivot) * tSize; // 오브젝트의 오른쪽 상단을 구한다.
		tEnd.x = tStart.x;
		//tStart는 왼쪽 상단
		//tEnd는 왼쪽 하단
	}

	else
	{
		// 오른쪽으로 이동하는 경우 : 사각형의 오른쪽을 확인하면 된다.

		tEnd = tPos + (Position(1.f, 1.f) - tPivot) * tSize; // 오른쪽 상단부터 구해준다.
		tStart = tPos - tPivot * tSize;
		tStart.x = tEnd.x;

		// tStart는 오른쪽 상단
		// tEnd는 오른쪽 하단
	}

	// 이렇게 정리한 정보로 3가지를 뽑아낼 수 있는데,
	// x위치에 해당하는 인덱스.(Start, End 동일)
	// 상단에서 뽑을 Y tStart.y
	// 하단에서 뽑을 Y tEnd.y

	// 상단에 있는 것이 인덱스가 먼저나오고, 하단에 있는 것이 나중에 나온다.
	// tStart.y -> startY // tEnd.y -> endY

	int iX, iStartY, iEndY;
	iX = GetTileIndexX(tStart.x);
	iStartY = GetTileIndexY(tStart.y);
	iEndY = GetTileIndexY(tEnd.y);

	// 일단 예외 처리부분.

	if (iX < 0 || iX >= m_iNumX)
		return false;
	// y축에 평행하는 직선을 그었는데 X좌표에 해당하는 인덱스가 잘못나온 경우이다.
	// 오브젝트가 스테이지밖으로 밖으로 넘어가면서(x) 충돌이 일어나는 경우라고 볼 수 있다. -1-1-1-1-1

	
	if (iStartY < 0)
		iStartY = 0;

	// StartY라고 하면, 위쪽 인덱스를 말하는 것인데, 들어간 좌표가 스테이지의 0.f 좌표보다 작아진 경우
	// 이 경우에는 y. 0인덱스부터 충돌검사를 해보는 것이 맞다.

	else if (iStartY >= m_iNumY)
		return false;
	
	// 근데 이거 GetTileIndex를 통해서 구하기 때문에 위의 경우라면 -1이 나와서 결국에서 else if를 타지는 않을 것 같다.

	if (iEndY < 0)
		return false;
	// End가 잘못 나온 경우인데... 이거 생각이 닿지 않은 것 같다.
	// 이미 인덱스를 구하는 순간부터 잘못되는 경우를 -1로 처리하는데, 여기서 iEndY의 값을 사용해서 조건을 처리하고 있다.

	// 아무튼. 위에서 -1을 반환하지 않는다고 하면...

	// End가 0인덱스보다 작게 나온 경우는 그냥 맵을 빠져나간 것이다. return false;

	else if (iEndY >= m_iNumY)
		iEndY = m_iNumY - 1;

	// end가 아래쪽에서 조금 넘어간 경우면.. 처리해준다.



	//=================================== X인덱스와 YStart YEnd를 구한 이후에 
	// 즉, 오브젝트의 왼쪽 선 / 오른쪽 선을 가지고서 해당 부분의 타일과 충돌처리를 해보는 것이다.

	for (int y = iStartY; y <= iEndY; ++y)
	{
		int iIndex = y * m_iNumX + iX;

		// 위에서 세로 방향의 인덱스를 구해서...
		// 그 위치의 타일이 벽인지 확인해본다.

		if (m_pTileList[iIndex]->GetTileOption() == TO_WALL)
		{


			Position tTilePos = m_pTileList[iIndex]->GetPos();
			// 해당 타일의 위치를 얻어온다.

			Size tTileSize = m_pTileList[iIndex]->GetSize();
			// 타일의 Size를 받아온다.

			// 타일의 위치와 Size정보를 가지고 레퍼런스 형태로 받아온 RectInfo tTileRC에 해당 
			//타일의 정보를 넣어준다.

			tTileRC.l = tTilePos.x;
			tTileRC.r = tTilePos.x + tTileSize.x;
			tTileRC.t = tTilePos.y;
			tTileRC.b = tTilePos.y + tTileSize.y;

			// *x 가만히 있는 경우는 return.

			if (iDir == 1)
			{
				// 오른쪽이동.
				fResult = tTileRC.l - tStart.x - 0.1f;

				// 왼쪽이동의 반대의 경우이다. 

				// 오른쪽 상단부터 오른쪽 하단의 직선을 검사하여 해당 위치의 타일 중에서 벽이 있는지 확인한다.
				// 벽이 있다면 해당 오브젝트를 타일에 파고든 x 길이 만큼 빼줘야 하는 상황이다.'
				
				// 이때 fResult값은 음의 방향이 나와야 하므로 
				// tTileRC. l - tStart.x 길이의 음의 값이 나온다.
				// 여기서 단순히 붙어 있는 상태로 만들지 않기 위해 0.1f -
			}
			else
			{
				// 왼쪽 이동.
				fResult = tTileRC.r - tStart.x + 0.1f;

				// 오브젝트의 왼쪽 상단부터 왼쪽 하단 그리고 이 직선에 해당하는 x의 좌표를 가지고서
				// 여기에 걸치는 타일의 인덱스를 구해냈고, 하나하나 위에서 부터 검사하던 중에
				// 벽이 나온 상황이다.

				// 일자가 모두 벽이 있는 상황이 아니라, 그 중에 하나라도 벽이 나온 상황.

				// 이 경우에 오브젝트가 타일과 충돌한 만큼의 X만큼을 다시 양의 방향으로 이동시켜야 한다.

				// 타일의 r에서 Start.x좌표를 빼면 파고 들어간 길이 만큼이 나온다. 0.1f +
			}

			// Result값은 왼쪽 혹은 오른쪽을 이동하는 경우에 움직여야 하는 이동값을 구하는 처리이다.
			

			return true;
		}

		// else 벽이 아니라면 다음 것을 검사한다.
		
	}

	// 여기까지 내려왔다는 의미는 오브젝트의 수직선을 검사했지만 벽인 타일이 없었다는 것이다.

	return false;
}

bool CStage::CheckWallH(float & fResult, RectInfo & tTileRC, CObj * pObj, int iDir)
{

	// CheckWallW 에서 x위치를 동일하게 두고 해당 부분의 좌우의 이동을 판단했다면.
	// CheckWallH 에서는 y위치를 동일하게 두고 상하의 이동을 판단한다.

	// 타일의 내부에서 이동이 일어났는지
	// 바깥에서 안 쪽으로 파고 들었는지..

	if (iDir == 0)
		return false;

	// Y축에 대해서 움직이지 않은 경우에 false

	Position tPos = pObj->GetPos();
	Position tPivot = pObj->GetPivot();
	Size tSize = pObj->GetStaticSize();

	Position tStart, tEnd;

	if (iDir == -1)
	{
		// 위로 올라가는 경우.

		tStart = tPos - tPivot * tSize; // 왼쪽 상단의 위치를 구한다.
		tEnd = tPos + (Position(1.f, 1.f) - tPivot) * tSize; // 오른쪽 하단의 위치를 구한다.

		tEnd.y = tStart.y;
		// tStart : 왼쪽 상단
		// tEnd : 오른쪽 상단
	}

	else
	{
		// 아래로 내려가는 경우.

		tEnd = tPos + (Position(1.f, 1.f) - tPivot) * tSize; // 오른쪽 하단의 위치를 구한다.
		tStart = tPos - tPivot * tSize; // 왼쪽 상단의 위치를 구한다.

		tStart.y = tEnd.y;

		// tStart : 왼쪽 하단
		// tEnd : 오른쪽 하단
	}

	// Y의 위치는 같으며,
	// X의 시작, X의 끝

	int iY, iStartX, iEndX;

	iY = GetTileIndexY(tStart.y);
	iStartX = GetTileIndexX(tStart.x);
	iEndX = GetTileIndexX(tEnd.x);

	if (iY < 0 || iY >= m_iNumY)
		return false;

	// Y인덱스가 잘못된 경우 false.

	if (iStartX < 0)
		iStartX = 0;

	// StartX 가 맵을 벗어난 경우 0부터

	else if (iStartX >= m_iNumX)
		return false;
	// StartX 가 맵의 크기를 벗어난 경우는 return

	if (iEndX < 0)
		return false;

	else if (iEndX >= m_iNumX)
		iEndX = m_iNumX - 1;

	// 예외 처리는 좋지만, Index를 부르는 함수에서 이미 예외처리를 하기 때문에 -1이 나오는 것이 문제.

	// 이렇게 구한 X축에 평행한 직선에 충돌하는 타일을 구한다.

	for (int x = iStartX; x <= iEndX; ++x)
	{
		int iIndex = iY * m_iNumX + x;

		if (m_pTileList[iIndex]->GetTileOption() == TO_WALL)
		{
			Position tTilePos = m_pTileList[iIndex]->GetPos();

			Size tTileSize = m_pTileList[iIndex]->GetSize();


			tTileRC.l = tTilePos.x;
			tTileRC.r = tTilePos.x + tTileSize.x;
			tTileRC.t = tTilePos.y;
			tTileRC.b = tTilePos.y + tTileSize.y;


			if (iDir == 1)
			{
				// 아래로 내려가는 경우.

				// 음의 값이 나오도록 해야 한다.
				fResult = tTileRC.t - tStart.y - 0.1f;
			}

			else
			{
				// 위로 올라가는 경우.

				// 양의 값이 나오도록 .
				fResult = tTileRC.b - tStart.y + 0.1f;
			}


			return true;
		}
		// ELSE 다음것을 검사한다.
	}


	// 해당 부분의 모든 타일을 검사했지만, 벽은 없는 경우 RETURN FALSE
	return false;
}

void CStage::AllTileOptionChange(TILE_OPTION eOption)
{
	for (int y = 0; y < m_iNumY; ++y)
	{
		for (int x = 0; x < m_iNumX; ++x)
		{
			int iIndex = y * m_iNumX + x;

			m_pTileList[iIndex]->SetTileOption(eOption);
		}
	}
}

void CStage::SetPlayer(CPlayer * pPlayer)
{
	m_pPlayer = pPlayer;

	if (m_pPlayer)
	{
		m_pPlayer->AddRef();

		list<CSpawner*>::iterator iter;
		list<CSpawner*>::iterator iterEnd = m_vecSpawnerList.end();

		for (iter = m_vecSpawnerList.begin(); iter != iterEnd; ++iter)
		{
			(*iter)->m_pTarget = m_pPlayer;
			m_pPlayer->AddRef();
		}
	}
}

void CStage::SetBackGroundStartPos(const Position & tStartPos)
{
	m_tBackGroundPos = tStartPos;
}

void CStage::SetBackGroundStartPos(int x, int y)
{
	m_tBackGroundPos = Position(x, y);
}

void CStage::SetForeGroundTextureColorKey(unsigned char r, unsigned char g, unsigned char b)
{
	m_pForeGroundTexture->SetColorKey(r, g, b);
}

void CStage::SetStagePos(int srcX, int srcY, int destX, int destY)
{
	m_tStageSrcPos = Position(srcX, srcY);
	m_tStageDestPos = Position(destX, destY);

	// 스테이지의 시작 좌표와 끝 좌표를 정했다면, SceneManager(카메라 있는 곳)에 해당 스테이지의 사이즈를 설정한다.

	GET_SINGLE(CSceneManager)->SetStageSize(m_tStageDestPos - m_tStageSrcPos);
}

void CStage::SetStagePos(const Position & tSrcPos, const Position & tDestPos)
{
	m_tStageSrcPos = tSrcPos;
	m_tStageDestPos = tDestPos;

	GET_SINGLE(CSceneManager)->SetStageSize(m_tStageDestPos - m_tStageSrcPos);

}
void CStage::SetBackGroundPos(int srcX, int srcY, int destX, int destY)
{
	m_tBackGroundSrcPos = Position(srcX, srcY);
	m_tBackGroundDestPos = Position(destX, destY);

	// 카메라가 BackGround의 src와 Dest의 위치를 알고 있을 필요가 있는가 ?
	// 즉, 카메라를 움직이는데 제한을 두기 위한 Size값을 알고 있을 필요가 있는가 ?
	// 카메라가 BackGround의 크기를 기준으로 움직이는가를 생각해보면 된다.
	// 전혀 아니다.(필요없다)

	m_tBackGroundSize = m_tBackGroundDestPos - m_tBackGroundSrcPos;
	// Stage에서는 BackGround의  Size를 통해서 무한 스크롤을 구현해야 한다.
}

void CStage::SetBackGroundPos(const Position & tSrcPos, const Position & tDestPos)
{
	m_tBackGroundSrcPos = tSrcPos;
	m_tBackGroundDestPos = tDestPos;

	m_tBackGroundSize = m_tBackGroundDestPos - m_tBackGroundSrcPos;

}

void CStage::SetForeGroundTexture(const string & strName,
	const TCHAR * pFileName, const string & strPathName)
{
	// Background다음에 출력할 foreground를 설정하는 함수이다.
	SAFE_RELEASE(m_pForeGroundTexture);


	// 해당 이름을 가지고 ResourceManager를 통해서 해당 함수를 읽어온다.
	// 이후에 이 값을 map에 저장하고 (strName으로)
	// 이렇게 추가한 텍스처를 m_pForeGround로 설정한다.
	if (pFileName)
		GET_SINGLE(CResourceManager)->LoadTexture(strName, pFileName, strPathName);

	m_pForeGroundTexture = GET_SINGLE(CResourceManager)->FindTexture(strName);

}

void CStage::Start()
{
	CCharacter::Start();
}

bool CStage::Init()
{
	if (!CObj::Init())
		return false;

	m_tSize = Size(RS_WIDTH, RS_HEIGHT);

	// BackGround 스크롤에서 y 축을 일단은 신경써주지 않기 위해서 (출력은 되야 하니까)
	// 기본값을 준다.

	m_tFrontPos.tStart.y = 0.f;
	m_tFrontPos.tEnd.y = RS_HEIGHT;


	m_tBackPos.tStart.y = 0.f;
	m_tBackPos.tEnd.y = RS_HEIGHT;

	// y축은 0 ~ RS_HEIGHT의 범위를 출력하는 것으로 고정됨.
	
	return true;
}

void CStage::Update(float fTime)
{
	CObj::Update(fTime);

	



	// 카메라의 좌표에 맞는 타일만 출력해주는 시스템을 구현한다.
	// 근데 이 카메라의 좌표
	Position tCameraPos = GET_SINGLE(CSceneManager)->GetCameraPos();
	Position tRightBottom = tCameraPos + Position(RS_WIDTH, RS_HEIGHT);

	m_iStartX = GetTileIndexX(tCameraPos.x);
	m_iStartY = GetTileIndexY(tCameraPos.y);

	tRightBottom -= m_tPos;

	tRightBottom /= m_tTileSize; // x 인덱스 y 인덱스

	m_iEndX = (int)tRightBottom.x;
	m_iEndY = (int)tRightBottom.y;

	if (m_iStartX == -1)
		m_iStartX = 0;

	if (m_iStartY == -1)
		m_iStartY = 0;

	if (m_iEndX >= m_iNumX)
		m_iEndX = m_iNumX - 1;

	if(m_iEndY >= m_iNumY)
		m_iEndY = m_iNumY - 1;
	
	// 스포너가 있다면 스포너를 Update한다.
	SpawnerUpdate(fTime);
}

void CStage::LateUpdate(float fTime)
{
	CObj::LateUpdate(fTime);

}

void CStage::Collision(float fTime)
{
	CObj::Collision(fTime);


}

void CStage::PrevRender(HDC hDC, float fTime)
{
	// BackGround의 출력 좌표를 움직이는 역할을 한다.
	if(m_bBackGround)
		BackGroundUpdate(fTime);

	CObj::PrevRender(hDC, fTime);

	m_bBackGround = true;

}

void CStage::Render(HDC hDC, float fTime)
{

	Position tCameraPos = GET_SINGLE(CSceneManager)->GetCameraPos();

	// BackGround를 출력한다. 적어도 화면보다는 크기 때문에 사이즈는 resolution으로 주면 된다.
	//CObj::Render(hDC, fTime); 

	// 애니메이션은 전용 오브젝트를 잡아서 돌려도 되니까 그냥 따로 처리하도록 하자.
	// UI활성화를 시켜놓고 화면좌표로 처리하기에는 너무 심하다. (다른 충돌체가 들어갈 것을 생각하면...)
	// 그리고, 배경의 속도를 지정할 예정이라 조금 다른 처리가 필요하다고 판단된다.
	// 애초에 배경이, 현재 카메라의 기준(src)도 아닌 것도 문제이다. ( src = foreground )



	// BackGround를 출력한다.
	// 플레이어의 입력에 따라서 처리를 할 것이다. 
	// 플레이어가 있다면, Left Right 키도 등록이 되있을 것이다.

	// 플레이어가 왼쪽 혹은 오른쪽 키를 누르고 있는 상태 + bStop || bPrevent 상태가 아니라면 
	// 움직이는 처리를 하는 것이다.

	// BackGround의 Render처리를 하고 있다.
	BackGroundRender(hDC, fTime, tCameraPos);

	// foreground를 출력한다.
	// StageSrcPos + tCameraPos 를 통해서 src의 시작위치로 접근한다.
	m_pForeGroundTexture->Render(hDC, Position(0.f, 0.f), m_tStageSrcPos + tCameraPos, Size(RS_WIDTH, RS_HEIGHT));







	// TileRender 

	if (m_bTileRender)
	{
		for (int y = m_iStartY; y <= m_iEndY; ++y)
		{
			for (int x = m_iStartX; x <= m_iEndX; ++x)
			{
				int	idx = y * m_iNumX + x;

				m_pTileList[idx]->SetPos(m_tPos.x + x * m_tTileSize.x,
					m_tPos.y + y * m_tTileSize.y);

				m_pTileList[idx]->Render(hDC, fTime);
			}
		}


		// Stage ColliderRender
		ColliderRender(hDC, fTime);
	}


#ifdef _DEBUG



	// 스포너가 있다면 스포너를 Render한다.
	SpawnerRender(hDC, fTime);
#endif



}

void CStage::PostRender(HDC hDC, float fTime)
{
	CObj::PostRender(hDC, fTime);

}

CStage * CStage::Clone() const
{
	return new CStage(*this);
}

void CStage::AddClipName(vector<string>& vecClipName)
{
}

void CStage::SpawnerUpdate(float fTime)
{
	list<CSpawner*>::iterator iter;
	list<CSpawner*>::iterator iterEnd = m_vecSpawnerList.end();

	for (iter = m_vecSpawnerList.begin(); iter != iterEnd; )
	{
		// 중간에 없어지는 경우는 없다. 
		// 비활성화는 가능성이 있음. ( 거의 없긴 한데)
		if (!(*iter)->IsEnable())
		{
			++iter;
			continue;
		}

		(*iter)->Update(fTime);
		++iter;
	}
}

void CStage::SpawnerRender(HDC hDC, float fTime)
{
	list<CSpawner*>::iterator iter;
	list<CSpawner*>::iterator iterEnd = m_vecSpawnerList.end();

	for (iter = m_vecSpawnerList.begin(); iter != iterEnd; )
	{
		// 중간에 없어지는 경우는 없다. 
		// 비활성화는 가능성이 있음. ( 거의 없긴 한데)
		if (!(*iter)->IsEnable())
		{
			++iter;
			continue;
		}

		(*iter)->Render(hDC, fTime);
		++iter;
	}
}

void CStage::BackGroundRender(HDC hDC, float fTime, const Position & tCameraPos)
{
	// 실질적인, BackGround의 출력좌표대로 출력이 이루어지는 함수이다. 
	// 이미 BackGroundUpdate에서 OutOfRange에 대한 처리, 그 외 좌표잡아두는 것 하나하나를
	// 다 처리해둔 상태이기 때문에 이제 출력만 이곳에서 하면 된다.

	// BackGroundPos , BackPos, FrontPos 모두 이미지에서의 위치를 기준으로한 상대좌표이다.

	if (!m_pTexture)
		return;
	// 만약 BackGround를 사용하지 않는다면, return.
	// Foreground를 사용하지 않는 경우는 없다.

	if (!m_bOutOfRange)
	{
		// src의 좌표만으로 처리할 수 있는 경우
		m_pTexture->Render(hDC, Position(0.f, 0.f), m_tBackGroundSrcPos + m_tBackGroundPos,
			Size(RS_WIDTH, RS_HEIGHT));
	}
	else
	{
		// 두 개의 출력으로 이루어진 경우.

		Size tFrontSize = m_tFrontPos.tEnd - m_tFrontPos.tStart;

		// FRONT를 출력한다.
		m_pTexture->Render(hDC, Position(0.f, 0.f), m_tBackGroundSrcPos + m_tFrontPos.tStart,
			tFrontSize);

		// BACK을 출력한다.
		// Back을 출력하는 경우에는 Front의 뒤에 출력해야 한다.
		m_pTexture->Render(hDC, Position(tFrontSize.x, 0.f), m_tBackGroundSrcPos + m_tBackPos.tStart,
			Size(RS_WIDTH - tFrontSize.x, tFrontSize.y));
	}
}

void CStage::BackGroundUpdate(float fTime)
{
	if (!m_pPlayer)
		return;

	else if (!m_pTexture)
		return;

	// BackGround의 이미지가 없다는 것은 사용하지 않는다는 의미이다.

	if (!m_pPlayer->GetStop() && !m_pPlayer->GetPrevent() && 
		!m_pPlayer->GetDown() && !m_pPlayer->GetKnockBack() && !m_pPlayer->GetKnockUp() &&
		m_pPlayer->GetInputEnable() && !GET_SINGLE(CGameManager)->GetPause())
		// 플레이어의 입력이 활성화 된 경우에만 처리를 한다
	{
		// 플레이어가 공격과 같은 정지 동작을 하고 있지 않고, 방어중인 것도 아니다.
		// 라면, 방향키를 제대로 누르고 있는지 확인한다.

		

		if (KEYPRESS("MoveLeft") && !KEYPRESS("MoveRight"))
		{
			// 두 방향키 중에서 왼쪽만 누르고 있다.
			m_tBackGroundPos.x -= m_fBackGroundSpeed * fTime;
		}

		else if (KEYPRESS("MoveRight") && !KEYPRESS("MoveLeft"))
		{
			// 오른쪽만 누르고 있다.

			m_tBackGroundPos.x += m_fBackGroundSpeed * fTime;

		}

		// BackGroundPos가 0, 0보다 작거나, size size보다 큰 경우에 범위를 벗어난 것이다.
		// 근데 여기서 예외처리를 해야 하는 부분이. 계속 그렇게 범위를 벗어나다가 완전히 출력범위까지 벗어나는 경우에는
		// OutOfRange를 풀어주고, 왼쪽에서 벗어난 경우 / 오른쪽에서 벗어난 경우를 나눠서 처리해준다.
		// y축은 신경쓰지 말고 !! 만들어.

		// 범위를 벗어났는지 확인한다. 근데, 범위를 화면 크기보다 많이 벗어난 경우를 판단해준다.
		// 우리는 일단 X에 대해서만 처리를 할 예정이니까. y는 생각하지 않는다.
		

		float fRight = m_tBackGroundPos.x + RS_WIDTH;



		if (m_tBackGroundPos.x < 0.f)
		{
			// 왼쪽에서 벗어난 경우

			// 범위가 화면 크기보다 더 넘어간 경우라면 오른쪽으로 좌표를 주고, OutOfRange를 풀어버린다.
			if ((-m_tBackGroundPos.x) >= RS_WIDTH)
			{
				// 벗어난 범위를 정확히 판단해야 한다. 그냥 오른쪽으로 움직이면 안 되고,
				float fWidth = -m_tBackGroundPos.x;
				// 정확하게 어느 정도의 width를 가지는지 구한다.
				// 여기서 width는 RS_WIDTH 이상이다.
				m_tBackGroundPos.x = m_tBackGroundSize.x - fWidth;
				m_bOutOfRange = false;
			}
			else
			{
				// 완전히 범위를 벗어난 경우가 아니라면, 왼쪽에 걸쳐있는 경우이다.
				// 이 경우에는 tFront 와 tBack 을 이용해서 출력할 위치를 나눠서 처리한다.
				// 넘어가서 새로 생기는 부분이 1번(front)
				// 왼쪽에서 남아있던 부분이 2번(back)
				m_bOutOfRange = true;

				// 넘어간 정도를 구한다.
				float fWidth = -m_tBackGroundPos.x;
				// 참고로 fWidth의 경우에는 RS_WIDTH이상으로 넘어간 것이 아니다.


				m_tFrontPos.tStart.x = m_tBackGroundSize.x - fWidth;

				// y 생략.
				m_tFrontPos.tEnd.x = m_tBackGroundSize.x;

				fWidth = RS_WIDTH - fWidth;

				m_tBackPos.tStart.x = 0.f;
				m_tBackPos.tEnd.x = fWidth;
			}
		}

		else if (fRight > m_tBackGroundSize.x)
		{
			// 오른쪽에서 벗어난 경우

			// 범위를 많이 벗어난 경우. 이 부분을 왼쪽으로 옮기고, OutOfRange를 false로 준다.
			if (m_tBackGroundPos.x >= m_tBackGroundSize.x)
			{
				// 넘어간 범위를 구한다.
				float fWidth = m_tBackGroundPos.x - m_tBackGroundSize.x;
				// fWidth는 0이상이다.
				// 0일때, 왼쪽에서 시작.
				// 5일때, 왼쪽 + 5에서 시작.

				m_tBackGroundPos.x = fWidth;
				m_bOutOfRange = false;
			}
			else
			{
				// 오른쪽에서 범위를 벗어났는데, RS_WIDTH만큼은 벗어나지 않은 경우.
				m_bOutOfRange = true;

				// 오른쪽에 남아있던 부분이 front
				// 왼쪽에서 새로 생긴부분이 back

				// 넘어간 범위를 구한다.
				float fWidth = fRight - m_tBackGroundSize.x;
				// 이렇게 넘어간 정도는 RS_WIDTH 보다 작다.
				// 위에서 RS_WIDTH이상인 경우에는 Src의 위치에 해당하는 
				// BackGroundPos.x 가 항상 size이상이기 때문이다.

				m_tBackPos.tStart.x = 0.f;
				m_tBackPos.tEnd.x = fWidth;

				fWidth = RS_WIDTH - fWidth;

				m_tFrontPos.tStart.x = m_tBackGroundSize.x - fWidth;
				m_tFrontPos.tEnd.x = m_tBackGroundSize.x;
			}


		}

		// 현재 위의 상태에서는 왼쪽에서 넘어가서 충분히 많이 넘어가서 풀리는 처리
		// 혹은 중간에 끼어있는 상태에 대한 처리를 하고 있다.
		// 오른쪽도 마찬가지의 처리이다.
		// 이 경우를 보면, 왼쪽에서 껴 있는 상태에서 왼쪽으로 더 나아간 경우(왼쪽에서 왼쪽으로 가는 처리)
		// 오른쪽에서는 오른쪽으로 더 아나간 경우에 대한 처리가 있다.

		// 즉, 왼쪽에서 껴있다가 오른쪽으로 벗어나서, 끼지 않은 경우에 대한 처리가 없고
		// 오른쪽에서 껴있따가 왼쪽으로 벗어나서, 끼지 않게 되는 경우에 대한 처리가 없다.
		// 위의 예시에서는 OutOfRange가 false가 되어야 하는 상황인데도 계속 true(기존 업데이트로인해)가 되어 처리가 될 것이다.
		// (일단 좌표의 범위가 벗어나지 않았고, 이후에 OutOfRange를 체크하여 값을 처리하지 않는다)

		else
			m_bOutOfRange = false;

		// 왼쪽에서 벗어난 경우가 아니고 오른쪽에서 벗어난 경우가 아니라면, Src를 가지고 출력하면 되므로
		// OutOfRange를 false 로 준다.

	}
}
