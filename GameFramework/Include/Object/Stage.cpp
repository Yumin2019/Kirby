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
	// TileCount��ŭ�� Tile* �� �Ҵ��� �Ǿ� �ִ�.
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

	// ��ġ����.
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

	// ��ġ����.
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

		// Ÿ���� ������, �ð��� ���߰�, Ÿ���� ������, �ð��� �ٽ� �帣�� .. �ٵ�..
		// �� ó���� �������� playMode������ ����ϴ� ���̴�.

		if (!bRender)
		{
			// �ٵ� �� ó���� Play ��忡���� ������ �ϵ��� �Ѵ�.
			GET_SINGLE(CSceneManager)->GravityTimeClear("Default");

			GET_SINGLE(CSoundManager)->Stop(CT_BGM);

			// �뷡 �ٽ� ����.
			GET_SINGLE(CSoundManager)->Play(m_pScene->GetBgmName(), CT_BGM);
		}
		else
		{
			// Render = true ����ٴ� �Ҹ�.
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

	// ������ ���� ���ɼ��� ������ ������.

	m_iNumX = iNumX;
	m_iNumY = iNumY;
	m_tTileSize = tTileSize;
	m_iTileCount = iNumX * iNumY;
	SetSize(iNumX * tTileSize.x, iNumY * tTileSize.y);
	// Ÿ���� ������ ���� ����� �̸� ���صд�.


	m_pTileList = new CTile*[m_iTileCount];

	for (int y = 0; y < m_iNumY; ++y)
	{
		for (int x = 0; x < m_iNumX; ++x)
		{
			int iIndex = y * m_iNumX + x;

			m_pTileList[iIndex] = new CTile;
			// �ϳ��� Ÿ���� ��ȸ�ϸ鼭 �����Ҵ��� ���ش�.

			m_pTileList[iIndex]->Init();
			// ���� ���� Ÿ�Ͽ� ���Ͽ� �ʱ�ȭ�Լ��� ȣ�����ְ�.

			m_pTileList[iIndex]->SetSize(m_tTileSize);
			// ������ ���� �������ָ�...

			m_pTileList[iIndex]->SetPos(x * m_tTileSize.x,
				y * m_tTileSize.y);
			// ��ġ�� �����Ѵ�.
			// ��ġ�� �����ϴ� ����� 2�����迭�� �����ϸ� �����ϴ�.

			// �̰� �������� �����ε�, ������������ Ÿ���� ��ƾ� �Ѵٰ� �����ϸ�.
			// ���������� ���� ��ġ���� Ÿ���� ����ִ� ���� ���� ������ ?
			// (�翬����.. �ٵ� 0, 0���� ������ ����Ұ��ݾ�)

			if(pBaseTile)
				m_pTileList[iIndex]->SetTexture(strTileTexKey, pBaseTile, strPath);
			// �⺻ Ÿ���� �̹����� �����ϴ� ����̴�.
			// Load�ؼ� ����� Ÿ���� �̸��� ������ �̸�, �ؽ�Ʈ�� ��θ� ������ 
			// �⺻ Ÿ�Ϸ� �������ְ� �ִ�.
			// �⺻Ÿ�Ͽ� ���ؼ� �ѹ� Load�� �Ͼ ��쿡 �� ���ĺ��ʹ�
			// ���� �̸��� ���� �ؽ�ó�� ���� ������ �Ͼ�Ƿ� ���۷��� ���� �����ϴ� �����̴�.
			
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

	// �ش� ��ġ�� Ÿ���� ���ͼ� ����.

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

	// ��ã���� null ã���� �ش� �ε����� �´� Tile*�� ��ȯ.
}

int CStage::GetTileIndexX(float x)
{
	x -= m_tPos.x;

	// ���������� ������ǥ�� 0�� �ƴ� ��츦 �����ؼ� ���� ��ġ�� ���ִ� ���̴�.
	// ���� ��ġ�� ������� �Ͽ� ������� ��ġ�� Ȯ���ϱ� �����̴�.
	// ���� ��ġ�� 50, 50�϶�, �� ��ǥ�� ������� ��ġ�� ����ϱ� ���ؼ�
	// 0, 0���� �����.
	// �׷��� �ٸ� ��ǥ�鵵 -50, -50 ��ŭ �̵��� �����ִ� ���̴�.

	// �̷��� ���� ������� ��ġ x�� Ÿ�� ������ x��ŭ �����ָ� �ε����� ���� �� �ִ�.
	x /= m_tTileSize.x;

	int iX = (int)x;

	if (iX < 0 || iX >= m_iNumX)
		return -1;

	// �̷��� ���� �ε����� 0���� �۰ų�, Ȥ�� NumX�̻��̶�� �߸��� ���̴�. (0���� ����)
	// ���� ���, ���������� ���� ��ġ ������ Ŭ���ߴٸ�, �ε����� �߸����� ���̴�.
	// ���������� �Ѿ�� ���� x ��ǥ�� ���� ��쿡�� ������ x �ε����� �Ѿ�� �� ���̴�.
	// -1����.

	return iX;
}

int CStage::GetTileIndexY(float y)
{
	// y�� �����ϴ�.
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

	// ���������� �������� ������� ��ġ�� ���ϰ�, �ش� ��ġ�� Ÿ�� ������� ����������.
	// �츮�� ����� x, y�� �ε����� ��� �ȴ�.

	int iX = (int)tPos.x;
	int iY = (int)tPos.y;

	if (iX < 0 || iY < 0 || iX >= m_iNumX || iY >= m_iNumY)
		return -1;

	// ����� �ε����� ������ �� ��쿡 1���� �迭�� ���·� �ٲ㼭 return.

	return iY * m_iNumX + iX;
}

bool CStage::CheckWallW(float & fResult, RectInfo & tTileRC,
	CObj * pObj, int iDir)
{
	if (iDir == 0)
		return false;

	// �Ѱ��� ���⿡�� Move.x = 0��, �������� ���� ��Ȳ�̶�� return.

	Position tPos = pObj->GetPos();
	Position tPivot = pObj->GetPivot();
	Size tSize = pObj->GetStaticSize();

	// �Ѱ��� ������Ʈ�� ��ġ��, Pivot����, StaticSize�� ���ؼ� �簢�� ������ �����.

	Position tStart, tEnd;

	// �������� �̵��ϴ� ��� : �簢���� ������ Ȯ���ϸ� �ȴ�.
	if (iDir == -1)
	{
		tStart = tPos - tPivot * tSize; // ������Ʈ�� ���� ����� ���Ѵ�.
		tEnd = tPos + (Position(1.f, 1.f) - tPivot) * tSize; // ������Ʈ�� ������ ����� ���Ѵ�.
		tEnd.x = tStart.x;
		//tStart�� ���� ���
		//tEnd�� ���� �ϴ�
	}

	else
	{
		// ���������� �̵��ϴ� ��� : �簢���� �������� Ȯ���ϸ� �ȴ�.

		tEnd = tPos + (Position(1.f, 1.f) - tPivot) * tSize; // ������ ��ܺ��� �����ش�.
		tStart = tPos - tPivot * tSize;
		tStart.x = tEnd.x;

		// tStart�� ������ ���
		// tEnd�� ������ �ϴ�
	}

	// �̷��� ������ ������ 3������ �̾Ƴ� �� �ִµ�,
	// x��ġ�� �ش��ϴ� �ε���.(Start, End ����)
	// ��ܿ��� ���� Y tStart.y
	// �ϴܿ��� ���� Y tEnd.y

	// ��ܿ� �ִ� ���� �ε����� ����������, �ϴܿ� �ִ� ���� ���߿� ���´�.
	// tStart.y -> startY // tEnd.y -> endY

	int iX, iStartY, iEndY;
	iX = GetTileIndexX(tStart.x);
	iStartY = GetTileIndexY(tStart.y);
	iEndY = GetTileIndexY(tEnd.y);

	// �ϴ� ���� ó���κ�.

	if (iX < 0 || iX >= m_iNumX)
		return false;
	// y�࿡ �����ϴ� ������ �׾��µ� X��ǥ�� �ش��ϴ� �ε����� �߸����� ����̴�.
	// ������Ʈ�� �������������� ������ �Ѿ�鼭(x) �浹�� �Ͼ�� ����� �� �� �ִ�. -1-1-1-1-1

	
	if (iStartY < 0)
		iStartY = 0;

	// StartY��� �ϸ�, ���� �ε����� ���ϴ� ���ε�, �� ��ǥ�� ���������� 0.f ��ǥ���� �۾��� ���
	// �� ��쿡�� y. 0�ε������� �浹�˻縦 �غ��� ���� �´�.

	else if (iStartY >= m_iNumY)
		return false;
	
	// �ٵ� �̰� GetTileIndex�� ���ؼ� ���ϱ� ������ ���� ����� -1�� ���ͼ� �ᱹ���� else if�� Ÿ���� ���� �� ����.

	if (iEndY < 0)
		return false;
	// End�� �߸� ���� ����ε�... �̰� ������ ���� ���� �� ����.
	// �̹� �ε����� ���ϴ� �������� �߸��Ǵ� ��츦 -1�� ó���ϴµ�, ���⼭ iEndY�� ���� ����ؼ� ������ ó���ϰ� �ִ�.

	// �ƹ�ư. ������ -1�� ��ȯ���� �ʴ´ٰ� �ϸ�...

	// End�� 0�ε������� �۰� ���� ���� �׳� ���� �������� ���̴�. return false;

	else if (iEndY >= m_iNumY)
		iEndY = m_iNumY - 1;

	// end�� �Ʒ��ʿ��� ���� �Ѿ ����.. ó�����ش�.



	//=================================== X�ε����� YStart YEnd�� ���� ���Ŀ� 
	// ��, ������Ʈ�� ���� �� / ������ ���� ������ �ش� �κ��� Ÿ�ϰ� �浹ó���� �غ��� ���̴�.

	for (int y = iStartY; y <= iEndY; ++y)
	{
		int iIndex = y * m_iNumX + iX;

		// ������ ���� ������ �ε����� ���ؼ�...
		// �� ��ġ�� Ÿ���� ������ Ȯ���غ���.

		if (m_pTileList[iIndex]->GetTileOption() == TO_WALL)
		{


			Position tTilePos = m_pTileList[iIndex]->GetPos();
			// �ش� Ÿ���� ��ġ�� ���´�.

			Size tTileSize = m_pTileList[iIndex]->GetSize();
			// Ÿ���� Size�� �޾ƿ´�.

			// Ÿ���� ��ġ�� Size������ ������ ���۷��� ���·� �޾ƿ� RectInfo tTileRC�� �ش� 
			//Ÿ���� ������ �־��ش�.

			tTileRC.l = tTilePos.x;
			tTileRC.r = tTilePos.x + tTileSize.x;
			tTileRC.t = tTilePos.y;
			tTileRC.b = tTilePos.y + tTileSize.y;

			// *x ������ �ִ� ���� return.

			if (iDir == 1)
			{
				// �������̵�.
				fResult = tTileRC.l - tStart.x - 0.1f;

				// �����̵��� �ݴ��� ����̴�. 

				// ������ ��ܺ��� ������ �ϴ��� ������ �˻��Ͽ� �ش� ��ġ�� Ÿ�� �߿��� ���� �ִ��� Ȯ���Ѵ�.
				// ���� �ִٸ� �ش� ������Ʈ�� Ÿ�Ͽ� �İ�� x ���� ��ŭ ����� �ϴ� ��Ȳ�̴�.'
				
				// �̶� fResult���� ���� ������ ���;� �ϹǷ� 
				// tTileRC. l - tStart.x ������ ���� ���� ���´�.
				// ���⼭ �ܼ��� �پ� �ִ� ���·� ������ �ʱ� ���� 0.1f -
			}
			else
			{
				// ���� �̵�.
				fResult = tTileRC.r - tStart.x + 0.1f;

				// ������Ʈ�� ���� ��ܺ��� ���� �ϴ� �׸��� �� ������ �ش��ϴ� x�� ��ǥ�� ������
				// ���⿡ ��ġ�� Ÿ���� �ε����� ���س°�, �ϳ��ϳ� ������ ���� �˻��ϴ� �߿�
				// ���� ���� ��Ȳ�̴�.

				// ���ڰ� ��� ���� �ִ� ��Ȳ�� �ƴ϶�, �� �߿� �ϳ��� ���� ���� ��Ȳ.

				// �� ��쿡 ������Ʈ�� Ÿ�ϰ� �浹�� ��ŭ�� X��ŭ�� �ٽ� ���� �������� �̵����Ѿ� �Ѵ�.

				// Ÿ���� r���� Start.x��ǥ�� ���� �İ� �� ���� ��ŭ�� ���´�. 0.1f +
			}

			// Result���� ���� Ȥ�� �������� �̵��ϴ� ��쿡 �������� �ϴ� �̵����� ���ϴ� ó���̴�.
			

			return true;
		}

		// else ���� �ƴ϶�� ���� ���� �˻��Ѵ�.
		
	}

	// ������� �����Դٴ� �ǹ̴� ������Ʈ�� �������� �˻������� ���� Ÿ���� �����ٴ� ���̴�.

	return false;
}

bool CStage::CheckWallH(float & fResult, RectInfo & tTileRC, CObj * pObj, int iDir)
{

	// CheckWallW ���� x��ġ�� �����ϰ� �ΰ� �ش� �κ��� �¿��� �̵��� �Ǵ��ߴٸ�.
	// CheckWallH ������ y��ġ�� �����ϰ� �ΰ� ������ �̵��� �Ǵ��Ѵ�.

	// Ÿ���� ���ο��� �̵��� �Ͼ����
	// �ٱ����� �� ������ �İ� �������..

	if (iDir == 0)
		return false;

	// Y�࿡ ���ؼ� �������� ���� ��쿡 false

	Position tPos = pObj->GetPos();
	Position tPivot = pObj->GetPivot();
	Size tSize = pObj->GetStaticSize();

	Position tStart, tEnd;

	if (iDir == -1)
	{
		// ���� �ö󰡴� ���.

		tStart = tPos - tPivot * tSize; // ���� ����� ��ġ�� ���Ѵ�.
		tEnd = tPos + (Position(1.f, 1.f) - tPivot) * tSize; // ������ �ϴ��� ��ġ�� ���Ѵ�.

		tEnd.y = tStart.y;
		// tStart : ���� ���
		// tEnd : ������ ���
	}

	else
	{
		// �Ʒ��� �������� ���.

		tEnd = tPos + (Position(1.f, 1.f) - tPivot) * tSize; // ������ �ϴ��� ��ġ�� ���Ѵ�.
		tStart = tPos - tPivot * tSize; // ���� ����� ��ġ�� ���Ѵ�.

		tStart.y = tEnd.y;

		// tStart : ���� �ϴ�
		// tEnd : ������ �ϴ�
	}

	// Y�� ��ġ�� ������,
	// X�� ����, X�� ��

	int iY, iStartX, iEndX;

	iY = GetTileIndexY(tStart.y);
	iStartX = GetTileIndexX(tStart.x);
	iEndX = GetTileIndexX(tEnd.x);

	if (iY < 0 || iY >= m_iNumY)
		return false;

	// Y�ε����� �߸��� ��� false.

	if (iStartX < 0)
		iStartX = 0;

	// StartX �� ���� ��� ��� 0����

	else if (iStartX >= m_iNumX)
		return false;
	// StartX �� ���� ũ�⸦ ��� ���� return

	if (iEndX < 0)
		return false;

	else if (iEndX >= m_iNumX)
		iEndX = m_iNumX - 1;

	// ���� ó���� ������, Index�� �θ��� �Լ����� �̹� ����ó���� �ϱ� ������ -1�� ������ ���� ����.

	// �̷��� ���� X�࿡ ������ ������ �浹�ϴ� Ÿ���� ���Ѵ�.

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
				// �Ʒ��� �������� ���.

				// ���� ���� �������� �ؾ� �Ѵ�.
				fResult = tTileRC.t - tStart.y - 0.1f;
			}

			else
			{
				// ���� �ö󰡴� ���.

				// ���� ���� �������� .
				fResult = tTileRC.b - tStart.y + 0.1f;
			}


			return true;
		}
		// ELSE �������� �˻��Ѵ�.
	}


	// �ش� �κ��� ��� Ÿ���� �˻�������, ���� ���� ��� RETURN FALSE
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

	// ���������� ���� ��ǥ�� �� ��ǥ�� ���ߴٸ�, SceneManager(ī�޶� �ִ� ��)�� �ش� ���������� ����� �����Ѵ�.

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

	// ī�޶� BackGround�� src�� Dest�� ��ġ�� �˰� ���� �ʿ䰡 �ִ°� ?
	// ��, ī�޶� �����̴µ� ������ �α� ���� Size���� �˰� ���� �ʿ䰡 �ִ°� ?
	// ī�޶� BackGround�� ũ�⸦ �������� �����̴°��� �����غ��� �ȴ�.
	// ���� �ƴϴ�.(�ʿ����)

	m_tBackGroundSize = m_tBackGroundDestPos - m_tBackGroundSrcPos;
	// Stage������ BackGround��  Size�� ���ؼ� ���� ��ũ���� �����ؾ� �Ѵ�.
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
	// Background������ ����� foreground�� �����ϴ� �Լ��̴�.
	SAFE_RELEASE(m_pForeGroundTexture);


	// �ش� �̸��� ������ ResourceManager�� ���ؼ� �ش� �Լ��� �о�´�.
	// ���Ŀ� �� ���� map�� �����ϰ� (strName����)
	// �̷��� �߰��� �ؽ�ó�� m_pForeGround�� �����Ѵ�.
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

	// BackGround ��ũ�ѿ��� y ���� �ϴ��� �Ű������ �ʱ� ���ؼ� (����� �Ǿ� �ϴϱ�)
	// �⺻���� �ش�.

	m_tFrontPos.tStart.y = 0.f;
	m_tFrontPos.tEnd.y = RS_HEIGHT;


	m_tBackPos.tStart.y = 0.f;
	m_tBackPos.tEnd.y = RS_HEIGHT;

	// y���� 0 ~ RS_HEIGHT�� ������ ����ϴ� ������ ������.
	
	return true;
}

void CStage::Update(float fTime)
{
	CObj::Update(fTime);

	



	// ī�޶��� ��ǥ�� �´� Ÿ�ϸ� ������ִ� �ý����� �����Ѵ�.
	// �ٵ� �� ī�޶��� ��ǥ
	Position tCameraPos = GET_SINGLE(CSceneManager)->GetCameraPos();
	Position tRightBottom = tCameraPos + Position(RS_WIDTH, RS_HEIGHT);

	m_iStartX = GetTileIndexX(tCameraPos.x);
	m_iStartY = GetTileIndexY(tCameraPos.y);

	tRightBottom -= m_tPos;

	tRightBottom /= m_tTileSize; // x �ε��� y �ε���

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
	
	// �����ʰ� �ִٸ� �����ʸ� Update�Ѵ�.
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
	// BackGround�� ��� ��ǥ�� �����̴� ������ �Ѵ�.
	if(m_bBackGround)
		BackGroundUpdate(fTime);

	CObj::PrevRender(hDC, fTime);

	m_bBackGround = true;

}

void CStage::Render(HDC hDC, float fTime)
{

	Position tCameraPos = GET_SINGLE(CSceneManager)->GetCameraPos();

	// BackGround�� ����Ѵ�. ��� ȭ�麸�ٴ� ũ�� ������ ������� resolution���� �ָ� �ȴ�.
	//CObj::Render(hDC, fTime); 

	// �ִϸ��̼��� ���� ������Ʈ�� ��Ƽ� ������ �Ǵϱ� �׳� ���� ó���ϵ��� ����.
	// UIȰ��ȭ�� ���ѳ��� ȭ����ǥ�� ó���ϱ⿡�� �ʹ� ���ϴ�. (�ٸ� �浹ü�� �� ���� �����ϸ�...)
	// �׸���, ����� �ӵ��� ������ �����̶� ���� �ٸ� ó���� �ʿ��ϴٰ� �Ǵܵȴ�.
	// ���ʿ� �����, ���� ī�޶��� ����(src)�� �ƴ� �͵� �����̴�. ( src = foreground )



	// BackGround�� ����Ѵ�.
	// �÷��̾��� �Է¿� ���� ó���� �� ���̴�. 
	// �÷��̾ �ִٸ�, Left Right Ű�� ����� ������ ���̴�.

	// �÷��̾ ���� Ȥ�� ������ Ű�� ������ �ִ� ���� + bStop || bPrevent ���°� �ƴ϶�� 
	// �����̴� ó���� �ϴ� ���̴�.

	// BackGround�� Renderó���� �ϰ� �ִ�.
	BackGroundRender(hDC, fTime, tCameraPos);

	// foreground�� ����Ѵ�.
	// StageSrcPos + tCameraPos �� ���ؼ� src�� ������ġ�� �����Ѵ�.
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



	// �����ʰ� �ִٸ� �����ʸ� Render�Ѵ�.
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
		// �߰��� �������� ���� ����. 
		// ��Ȱ��ȭ�� ���ɼ��� ����. ( ���� ���� �ѵ�)
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
		// �߰��� �������� ���� ����. 
		// ��Ȱ��ȭ�� ���ɼ��� ����. ( ���� ���� �ѵ�)
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
	// ��������, BackGround�� �����ǥ��� ����� �̷������ �Լ��̴�. 
	// �̹� BackGroundUpdate���� OutOfRange�� ���� ó��, �� �� ��ǥ��Ƶδ� �� �ϳ��ϳ���
	// �� ó���ص� �����̱� ������ ���� ��¸� �̰����� �ϸ� �ȴ�.

	// BackGroundPos , BackPos, FrontPos ��� �̹��������� ��ġ�� ���������� �����ǥ�̴�.

	if (!m_pTexture)
		return;
	// ���� BackGround�� ������� �ʴ´ٸ�, return.
	// Foreground�� ������� �ʴ� ���� ����.

	if (!m_bOutOfRange)
	{
		// src�� ��ǥ������ ó���� �� �ִ� ���
		m_pTexture->Render(hDC, Position(0.f, 0.f), m_tBackGroundSrcPos + m_tBackGroundPos,
			Size(RS_WIDTH, RS_HEIGHT));
	}
	else
	{
		// �� ���� ������� �̷���� ���.

		Size tFrontSize = m_tFrontPos.tEnd - m_tFrontPos.tStart;

		// FRONT�� ����Ѵ�.
		m_pTexture->Render(hDC, Position(0.f, 0.f), m_tBackGroundSrcPos + m_tFrontPos.tStart,
			tFrontSize);

		// BACK�� ����Ѵ�.
		// Back�� ����ϴ� ��쿡�� Front�� �ڿ� ����ؾ� �Ѵ�.
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

	// BackGround�� �̹����� ���ٴ� ���� ������� �ʴ´ٴ� �ǹ��̴�.

	if (!m_pPlayer->GetStop() && !m_pPlayer->GetPrevent() && 
		!m_pPlayer->GetDown() && !m_pPlayer->GetKnockBack() && !m_pPlayer->GetKnockUp() &&
		m_pPlayer->GetInputEnable() && !GET_SINGLE(CGameManager)->GetPause())
		// �÷��̾��� �Է��� Ȱ��ȭ �� ��쿡�� ó���� �Ѵ�
	{
		// �÷��̾ ���ݰ� ���� ���� ������ �ϰ� ���� �ʰ�, ������� �͵� �ƴϴ�.
		// ���, ����Ű�� ����� ������ �ִ��� Ȯ���Ѵ�.

		

		if (KEYPRESS("MoveLeft") && !KEYPRESS("MoveRight"))
		{
			// �� ����Ű �߿��� ���ʸ� ������ �ִ�.
			m_tBackGroundPos.x -= m_fBackGroundSpeed * fTime;
		}

		else if (KEYPRESS("MoveRight") && !KEYPRESS("MoveLeft"))
		{
			// �����ʸ� ������ �ִ�.

			m_tBackGroundPos.x += m_fBackGroundSpeed * fTime;

		}

		// BackGroundPos�� 0, 0���� �۰ų�, size size���� ū ��쿡 ������ ��� ���̴�.
		// �ٵ� ���⼭ ����ó���� �ؾ� �ϴ� �κ���. ��� �׷��� ������ ����ٰ� ������ ��¹������� ����� ��쿡��
		// OutOfRange�� Ǯ���ְ�, ���ʿ��� ��� ��� / �����ʿ��� ��� ��츦 ������ ó�����ش�.
		// y���� �Ű澲�� ���� !! �����.

		// ������ ������� Ȯ���Ѵ�. �ٵ�, ������ ȭ�� ũ�⺸�� ���� ��� ��츦 �Ǵ����ش�.
		// �츮�� �ϴ� X�� ���ؼ��� ó���� �� �����̴ϱ�. y�� �������� �ʴ´�.
		

		float fRight = m_tBackGroundPos.x + RS_WIDTH;



		if (m_tBackGroundPos.x < 0.f)
		{
			// ���ʿ��� ��� ���

			// ������ ȭ�� ũ�⺸�� �� �Ѿ ����� ���������� ��ǥ�� �ְ�, OutOfRange�� Ǯ�������.
			if ((-m_tBackGroundPos.x) >= RS_WIDTH)
			{
				// ��� ������ ��Ȯ�� �Ǵ��ؾ� �Ѵ�. �׳� ���������� �����̸� �� �ǰ�,
				float fWidth = -m_tBackGroundPos.x;
				// ��Ȯ�ϰ� ��� ������ width�� �������� ���Ѵ�.
				// ���⼭ width�� RS_WIDTH �̻��̴�.
				m_tBackGroundPos.x = m_tBackGroundSize.x - fWidth;
				m_bOutOfRange = false;
			}
			else
			{
				// ������ ������ ��� ��찡 �ƴ϶��, ���ʿ� �����ִ� ����̴�.
				// �� ��쿡�� tFront �� tBack �� �̿��ؼ� ����� ��ġ�� ������ ó���Ѵ�.
				// �Ѿ�� ���� ����� �κ��� 1��(front)
				// ���ʿ��� �����ִ� �κ��� 2��(back)
				m_bOutOfRange = true;

				// �Ѿ ������ ���Ѵ�.
				float fWidth = -m_tBackGroundPos.x;
				// ����� fWidth�� ��쿡�� RS_WIDTH�̻����� �Ѿ ���� �ƴϴ�.


				m_tFrontPos.tStart.x = m_tBackGroundSize.x - fWidth;

				// y ����.
				m_tFrontPos.tEnd.x = m_tBackGroundSize.x;

				fWidth = RS_WIDTH - fWidth;

				m_tBackPos.tStart.x = 0.f;
				m_tBackPos.tEnd.x = fWidth;
			}
		}

		else if (fRight > m_tBackGroundSize.x)
		{
			// �����ʿ��� ��� ���

			// ������ ���� ��� ���. �� �κ��� �������� �ű��, OutOfRange�� false�� �ش�.
			if (m_tBackGroundPos.x >= m_tBackGroundSize.x)
			{
				// �Ѿ ������ ���Ѵ�.
				float fWidth = m_tBackGroundPos.x - m_tBackGroundSize.x;
				// fWidth�� 0�̻��̴�.
				// 0�϶�, ���ʿ��� ����.
				// 5�϶�, ���� + 5���� ����.

				m_tBackGroundPos.x = fWidth;
				m_bOutOfRange = false;
			}
			else
			{
				// �����ʿ��� ������ ����µ�, RS_WIDTH��ŭ�� ����� ���� ���.
				m_bOutOfRange = true;

				// �����ʿ� �����ִ� �κ��� front
				// ���ʿ��� ���� ����κ��� back

				// �Ѿ ������ ���Ѵ�.
				float fWidth = fRight - m_tBackGroundSize.x;
				// �̷��� �Ѿ ������ RS_WIDTH ���� �۴�.
				// ������ RS_WIDTH�̻��� ��쿡�� Src�� ��ġ�� �ش��ϴ� 
				// BackGroundPos.x �� �׻� size�̻��̱� �����̴�.

				m_tBackPos.tStart.x = 0.f;
				m_tBackPos.tEnd.x = fWidth;

				fWidth = RS_WIDTH - fWidth;

				m_tFrontPos.tStart.x = m_tBackGroundSize.x - fWidth;
				m_tFrontPos.tEnd.x = m_tBackGroundSize.x;
			}


		}

		// ���� ���� ���¿����� ���ʿ��� �Ѿ�� ����� ���� �Ѿ�� Ǯ���� ó��
		// Ȥ�� �߰��� �����ִ� ���¿� ���� ó���� �ϰ� �ִ�.
		// �����ʵ� ���������� ó���̴�.
		// �� ��츦 ����, ���ʿ��� �� �ִ� ���¿��� �������� �� ���ư� ���(���ʿ��� �������� ���� ó��)
		// �����ʿ����� ���������� �� �Ƴ��� ��쿡 ���� ó���� �ִ�.

		// ��, ���ʿ��� ���ִٰ� ���������� �����, ���� ���� ��쿡 ���� ó���� ����
		// �����ʿ��� ���ֵ��� �������� �����, ���� �ʰ� �Ǵ� ��쿡 ���� ó���� ����.
		// ���� ���ÿ����� OutOfRange�� false�� �Ǿ�� �ϴ� ��Ȳ�ε��� ��� true(���� ������Ʈ������)�� �Ǿ� ó���� �� ���̴�.
		// (�ϴ� ��ǥ�� ������ ����� �ʾҰ�, ���Ŀ� OutOfRange�� üũ�Ͽ� ���� ó������ �ʴ´�)

		else
			m_bOutOfRange = false;

		// ���ʿ��� ��� ��찡 �ƴϰ� �����ʿ��� ��� ��찡 �ƴ϶��, Src�� ������ ����ϸ� �ǹǷ�
		// OutOfRange�� false �� �ش�.

	}
}
