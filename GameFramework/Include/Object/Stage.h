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

	Position	m_tBackGroundSrcPos; // BackGround의 src를 기준으로 한 시작 좌표, 끝 좌표를 가지고 있는다.
	Position	m_tBackGroundDestPos;
	Size		m_tBackGroundSize;		// 그리고 이것의 사이즈를 가지고 있을 변수이다.

	// 스테이지가 존재한다는 것은 플레이어를 타겟으로 잡고 있다는 것이다.
	class CPlayer* m_pPlayer;


	// 스테이지 객체는 해당 장면의 배경충돌체를 가지고 있으며, background출력도 담당하고 있다.
	// 여기서 맨 뒤에 까지는 background와 foreground 이 두 가지를 처리해야 한다.
	// 애니메이션이 따로, foreground에 해당하는 텍스처를 가지고 있는 것으로 한다.

	class CTexture*		m_pForeGroundTexture;

	float m_fBackGroundSpeed; // 배경화면의 스피드이다.

	ClipCoord		m_tFrontPos; // 스크롤의 앞을 담당하는 좌표이다.
	ClipCoord		m_tBackPos; // 스크롤의 뒤를 담당하는 좌표이다. 
	// 두 개념 모두 src위치를 잡아주기 위한 정보이다.
	// 근데, BackGroundPos(상대좌표)와 연관성이 있는 정보로
	// 이들도 상대좌표의 개념으로 처리가 이루어 진다.

	Position		m_tBackGroundPos;
	// src에서의 위치를 기준으로한 상대좌표.

	 // 범위를 넘어가지 않은 경우에 출력하는데 사용할 변수이다.
	// 그리고 이것을 가지고 직접 좌표를 움직이는데 사용할 것이다. 
	// 이 변수를 움직이다가 범위를 벗어나는 경우에 OutOfRange변수를 true로 줄 예정이다.
	// 이 변수를 사용하다가 범위를 너무 벗어나서 OutOfRange 가 풀리는 경우도 판단해준다.
	// 아무튼 OutOfRange의 값과는 상관없이 움직인다. 계속 !
	// 근데 해당 변수를 우리가, src를 기준으로 하고 있는지 아니면 0,0 size,size 기준으로 잡혀 있는지를 판단해야 한다.
	// src를 기준으로 하면, 범위를 넘어갔는지 판단할 수 있을까 ? (ㅇㅇ 당연히)
	// src를 기준으로 하지 않고, 해당 이미지의 상대적인 좌표를 기준으로 하면 범위를 넘어갔는지 확인하기 쉽고,
	// 이것을 기준으로 출력하는 과정에서는 src의 시작 위치를 이용하면된다.
	// 구현하는 사람 마음인데, 나는 상대적인 좌표로 처리하도록 한다. (즉, 해당 좌표를 0, 0 ~ size, size)의 좌표이다.
	bool			m_bOutOfRange;	// 범위를 벗어났는지 확인할 bool.


	// Tile 에 관련된 정보

private:
	int		m_iNumX; // 타일의 가로 개수
	int		m_iNumY; // 타일의 세로 개수
	int		m_iTileCount; // 전체 타일의 수
	Size	m_tTileSize; // 타일의 사이즈
	class CTile** m_pTileList; // 타일을 관리하기 위한 List TILE**를 이용해서
	// TILE*를 여러개 가지고 있기 위함이다.

	int			m_iStartX; // 시작하는 타일 X
	int			m_iStartY; // 시작하는 타일 Y

	int			m_iEndX; // 끝 타일 X
	int			m_iEndY; // 마지막 타일 Y

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
		// 0이 아니여서 사용하면 true
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
	// 타일을 생성하는 함수이다. 타일의 X, Y 개수, 사이즈를 받아주고 있다.
	// 타일에 이미지를 줄 수 있는 기능까지 포함하고 있는데 
	// 만약에 기본 타일을 설정하고 싶다면, pBaseTile에 기본 타일에 해당하는 타일의 이미지 정보를 주면 된다.
	// ResourceManager를 이용해서 읽어오고 strTileTexKey로 해당 타일을 추가하는 것이다.

	int ChangeTileOption(Position tPos,
		TILE_OPTION eOption);
	// 해당 위치의 타일의 옵션을 바꿔주는 함수이다.

	void ChangeTileOption(int iIndex, TILE_OPTION eOption);
	// 인덱스를 주면 옵션 바꾸는 함수.


	class CTile* GetTile(const Position& tPos);
	// 해당 위치에 있는 타일을 가져오는 함수이다.

	int GetTileIndexX(float x); // X좌표를 넣어주면 X인덱스를 얻어오고
	int GetTileIndexY(float y); // Y좌표를 넣어주면 Y인덱스를 얻어주는 함수이다.

	int GetTileIndex(Position tPos); // X, Y 인덱스를 모두 구해서 인덱스를 반환해주는 함수.

	bool CheckWallW(float& fResult, RectInfo& tTileRC, CObj* pObj, int iDir);
	bool CheckWallH(float& fResult, RectInfo& tTileRC, CObj* pObj, int iDir);
	// 타일 충돌에 이용하는 함수로서, fResult를 레퍼런스 형태로 받아서 이동량을 구하고,
	// Collision쪽에서 .. 같이 보도록 하자.

	// 타일 개수.
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

	// 우리는 Scene을 통해서 해당 값을 셋팅해야 한다.
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

