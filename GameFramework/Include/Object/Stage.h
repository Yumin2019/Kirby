#pragma once
#include "Character.h"
class CStage :
	public CCharacter
{
	DECLARE_OBJECT

public:
	CStage();
	CStage(const CStage& obj);
	~CStage();

private:
	Position	m_tStageSrcPos;
	Position	m_tStageDestPos;

	Position	m_tBackGroundSrcPos; // BackGround�� src�� �������� �� ���� ��ǥ, �� ��ǥ�� ������ �ִ´�.
	Position	m_tBackGroundDestPos;
	Size		m_tBackGroundSize;		// �׸��� �̰��� ����� ������ ���� �����̴�.

	// ���������� �����Ѵٴ� ���� �÷��̾ Ÿ������ ��� �ִٴ� ���̴�.
	class CPlayer* m_pPlayer;


	// �������� ��ü�� �ش� ����� ����浹ü�� ������ ������, background��µ� ����ϰ� �ִ�.
	// ���⼭ �� �ڿ� ������ background�� foreground �� �� ������ ó���ؾ� �Ѵ�.
	// �ִϸ��̼��� ����, foreground�� �ش��ϴ� �ؽ�ó�� ������ �ִ� ������ �Ѵ�.

	class CTexture*		m_pForeGroundTexture;

	float m_fBackGroundSpeed; // ���ȭ���� ���ǵ��̴�.

	ClipCoord		m_tFrontPos; // ��ũ���� ���� ����ϴ� ��ǥ�̴�.
	ClipCoord		m_tBackPos; // ��ũ���� �ڸ� ����ϴ� ��ǥ�̴�. 
	// �� ���� ��� src��ġ�� ����ֱ� ���� �����̴�.
	// �ٵ�, BackGroundPos(�����ǥ)�� �������� �ִ� ������
	// �̵鵵 �����ǥ�� �������� ó���� �̷�� ����.

	Position		m_tBackGroundPos;
	// src������ ��ġ�� ���������� �����ǥ.

	 // ������ �Ѿ�� ���� ��쿡 ����ϴµ� ����� �����̴�.
	// �׸��� �̰��� ������ ���� ��ǥ�� �����̴µ� ����� ���̴�. 
	// �� ������ �����̴ٰ� ������ ����� ��쿡 OutOfRange������ true�� �� �����̴�.
	// �� ������ ����ϴٰ� ������ �ʹ� ����� OutOfRange �� Ǯ���� ��쵵 �Ǵ����ش�.
	// �ƹ�ư OutOfRange�� ������ ������� �����δ�. ��� !
	// �ٵ� �ش� ������ �츮��, src�� �������� �ϰ� �ִ��� �ƴϸ� 0,0 size,size �������� ���� �ִ����� �Ǵ��ؾ� �Ѵ�.
	// src�� �������� �ϸ�, ������ �Ѿ���� �Ǵ��� �� ������ ? (���� �翬��)
	// src�� �������� ���� �ʰ�, �ش� �̹����� ������� ��ǥ�� �������� �ϸ� ������ �Ѿ���� Ȯ���ϱ� ����,
	// �̰��� �������� ����ϴ� ���������� src�� ���� ��ġ�� �̿��ϸ�ȴ�.
	// �����ϴ� ��� �����ε�, ���� ������� ��ǥ�� ó���ϵ��� �Ѵ�. (��, �ش� ��ǥ�� 0, 0 ~ size, size)�� ��ǥ�̴�.
	bool			m_bOutOfRange;	// ������ ������� Ȯ���� bool.


	// Tile �� ���õ� ����

private:
	int		m_iNumX; // Ÿ���� ���� ����
	int		m_iNumY; // Ÿ���� ���� ����
	int		m_iTileCount; // ��ü Ÿ���� ��
	Size	m_tTileSize; // Ÿ���� ������
	class CTile** m_pTileList; // Ÿ���� �����ϱ� ���� List TILE**�� �̿��ؼ�
	// TILE*�� ������ ������ �ֱ� �����̴�.

	int			m_iStartX; // �����ϴ� Ÿ�� X
	int			m_iStartY; // �����ϴ� Ÿ�� Y

	int			m_iEndX; // �� Ÿ�� X
	int			m_iEndY; // ������ Ÿ�� Y

	bool	m_bTileRender;
	bool	m_bBackGround;

	// Spawner
	list<class CSpawner*> m_vecSpawnerList;

public:
	void Save(const char* pFileName);
	void Load(const char* pFileName);
	void AddSpawner(const string& strSpawnerTag, float PosX, float PosY,
		SIDE_DIR eSpawnDir, const string& strSpawnMonsterTag, bool bCount = false, int iCount = INT_MAX);
	

public:
	void BackGroundStop()
	{
		m_bBackGround = false;
	}

	bool IsTile() const
	{
		return m_iTileCount != 0;
		// 0�� �ƴϿ��� ����ϸ� true
	}

	int GetTileCount() const
	{
		return m_iTileCount;
	}

	void SetTileRender(bool bRender);


	bool GetTileRender() const
	{
		return m_bTileRender;
	}

	TILE_OPTION GetTileOption(int iIndex);


public:
	bool CreateTile(int iNumX, int iNumY, const Size& tTileSize,
		const string& strTileTexKey = "",
		const TCHAR* pBaseTile = nullptr,
		const string& strPath = TEXTURE_PATH);
	// Ÿ���� �����ϴ� �Լ��̴�. Ÿ���� X, Y ����, ����� �޾��ְ� �ִ�.
	// Ÿ�Ͽ� �̹����� �� �� �ִ� ��ɱ��� �����ϰ� �ִµ� 
	// ���࿡ �⺻ Ÿ���� �����ϰ� �ʹٸ�, pBaseTile�� �⺻ Ÿ�Ͽ� �ش��ϴ� Ÿ���� �̹��� ������ �ָ� �ȴ�.
	// ResourceManager�� �̿��ؼ� �о���� strTileTexKey�� �ش� Ÿ���� �߰��ϴ� ���̴�.

	int ChangeTileOption(Position tPos,
		TILE_OPTION eOption);
	// �ش� ��ġ�� Ÿ���� �ɼ��� �ٲ��ִ� �Լ��̴�.

	void ChangeTileOption(int iIndex, TILE_OPTION eOption);
	// �ε����� �ָ� �ɼ� �ٲٴ� �Լ�.


	class CTile* GetTile(const Position& tPos);
	// �ش� ��ġ�� �ִ� Ÿ���� �������� �Լ��̴�.

	int GetTileIndexX(float x); // X��ǥ�� �־��ָ� X�ε����� ������
	int GetTileIndexY(float y); // Y��ǥ�� �־��ָ� Y�ε����� ����ִ� �Լ��̴�.

	int GetTileIndex(Position tPos); // X, Y �ε����� ��� ���ؼ� �ε����� ��ȯ���ִ� �Լ�.

	bool CheckWallW(float& fResult, RectInfo& tTileRC, CObj* pObj, int iDir);
	bool CheckWallH(float& fResult, RectInfo& tTileRC, CObj* pObj, int iDir);
	// Ÿ�� �浹�� �̿��ϴ� �Լ��μ�, fResult�� ���۷��� ���·� �޾Ƽ� �̵����� ���ϰ�,
	// Collision�ʿ��� .. ���� ������ ����.

	// Ÿ�� ����.
	Size GetTileNumber() const
	{
		return Size(m_iNumX, m_iNumY);
	}

	void AllTileOptionChange(TILE_OPTION eOption);

public:

	void SetPlayer(class CPlayer* pPlayer);
	void SetBackGroundStartPos(const Position& tStartPos);
	void SetBackGroundStartPos(int x, int y);

	void SetBackGroundSpeed(float fSpeed)
	{
		m_fBackGroundSpeed = fSpeed;
	}

	void SetForeGroundTextureColorKey(unsigned char r, unsigned char g, unsigned char b);

	// �츮�� Scene�� ���ؼ� �ش� ���� �����ؾ� �Ѵ�.
	void SetStagePos(int srcX, int srcY, int destX, int destY);
	void SetStagePos(const Position& tSrcPos, const Position& tDestPos);
	void SetBackGroundPos(int srcX, int srcY, int destX, int destY);
	void SetBackGroundPos(const Position& tSrcPos, const Position& tDestPos);
	

public:
	void SetForeGroundTexture(const string& strName,
		const TCHAR* pFileName, const string& strPathName = TEXTURE_PATH);


	virtual void Start();
	virtual bool Init();
	virtual void Update(float fTime);
	virtual void LateUpdate(float fTime);
	virtual void Collision(float fTime);
	virtual void PrevRender(HDC hDC, float fTime);
	virtual void Render(HDC hDC, float fTime);
	virtual void PostRender(HDC hDC, float fTime);
	virtual CStage* Clone()	const;
	virtual void AddClipName(vector<string>& vecClipName);

	void SpawnerUpdate(float fTime);
	void SpawnerRender(HDC hDC, float fTime);

private:
	void BackGroundRender(HDC hDC, float fTime, const Position& tCameraPos);
	void BackGroundUpdate(float fTime);
};

