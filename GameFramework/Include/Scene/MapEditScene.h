#pragma once
#include "Scene.h"


enum EDIT_MODE
{
	EM_NONE = -1,
	EM_FOREGROUND,
	EM_BACKGROUND,
	EM_TILE_OPTION,
	EM_SPAWNER, 
	EM_LINE,
	EM_POSITION,
	EM_END
};


// 모드에 따라서 달라지는 옵션 값들...
// 모드가 변한다 -> 해당 옵션을 NONE으로 초기화를 해두는 것이 좋지 않을까 ? 그냥 둬도 괜찮고 ㅋㅋ

/*
enum TILE_OPTION
{
	TO_NONE,
	TO_NORMAL,
	TO_WALL,
	TO_END
};

*/

enum POSITION_OPTION
{
	PO_NONE,
	PO_START,
	PO_DOOR,
	PO_END
};

enum SPAWNER_OPTION
{
	SO_NONE,
	SO_BURT,
	SO_TWIZZY,
	SO_TWIZZY_FLY,
	SO_GRIZOO,
	SO_MIKE,
	SO_BLADE,
	SO_ROCKY,
	SO_SLEEP,
	SO_END
};

enum EDIT_STATE
{
	ES_NONE,
	ES_CLICK,
	ES_DRAG
};


class CMapEditScene :
	public CScene
{
public:
	CMapEditScene();
	~CMapEditScene();


private:

	// Tile
	EDIT_MODE	m_eMode;			// 수정을 위한 모드. 이미지, 블럭 설정이 있다.
	TILE_OPTION m_eTileOption;			// 설정된 옵션으로 선택한 타일을 수정하기 위함이다.
	class CInputComponent* m_pInputComponent;	// 함수를 등록하여 사용하기 위해서 Inputcomponent를 가져옴.
	class		CStage* m_pStage;			// 타일 편집을 위한 스테이지 객체 등록.

	// Spanwer 
	SPAWNER_OPTION m_eSpawnerOption;
	SIDE_DIR m_eSpawnerDir;
	bool		m_bSpawnCount;
	int			m_iSpawnCount;
	Position	 m_tSpawnerPos;
	string		m_strMonsterTag;

	// Position 
	POSITION_OPTION m_ePositionOption;
	Position		m_tStartPos;
	Position		m_tDoorPos;

	// 포지션 정보 셋팅시에 이용할 값이다.
	Position		m_tPos;


	// BackGround / ForeGround
	vector<ClipCoord> m_vecBackGroundPos;
	vector<ClipCoord> m_vecForeGroundPos;
	unsigned int m_iBack; 
	unsigned int m_iFore;

	 // Tile의 정보의 변경이 필요한지에 대한 여부를 확인하기 위해서, 미리 Size를 받아두기 위한 구조체.
	Size	m_tPrevStageSize;
	Size	m_tCurrentStageSize;

	// Render On/Off
	bool	m_bTileRender;
	bool	m_bLineRender;
	bool	m_bSpawnerRender;
	// 솔직히 BackGround랑, ForeGround 출력을 빼버리면 뭐가 남냐... 

	


	Position m_tMousePos;

	EDIT_STATE m_eState; // 일괄편집에 사용하는 flag
	Position	m_tClickPos;

	Size		m_tTileSize;

	Position m_tRenderOffset; // 컨트롤 키와 연계 구현할 것.

	Position m_tSrc;
	Position m_tDest;

	vector<ClipCoord> m_vecCoord;
	vector<SPAWNERINFO> m_vecSpawner;



public:
	virtual void Start();
	virtual bool Init();
	virtual void Update(float fTime);
	virtual void Render(HDC hDC, float fTime);

private:
	// 매핑 함수.  : 유효성 검사는 하지 않는다. Very Dangerous 
	ClipCoord GetBackPos(int iIndex)
	{
		return m_vecBackGroundPos[iIndex];
	}

	ClipCoord GetForePos(int iIndex)
	{
		return m_vecForeGroundPos[iIndex];
	}
private:
	void EdgeSetBlock();
	void NormalInput(float fTime);
	void ModeChangeNext(float fTime);
	void OptionChangeNext(float fTime);
	void Save(float fTime);
	void Load(float fTime);
	void SaveObjectPos(float fTime);
	void LoadObjectPos(float fTime);
	void SaveObjectPosText(const char* pFileName);
	void LoadObjectPosText(const char* pFileName);

	void DeleteEditIndex(int iIndex);

	void ClearWallTile(float fTime);

	void TextRender(HDC hDC);

	void ObjectPosSave();
	void ObjectPosDelete();

	void ObjectPosRender(HDC hDC);

	wstring StringToWstring(const string& s)
	{
		int len;
		int slength = (int)s.length() + 1;
		len = MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, 0, 0);
		wchar_t* buf = new wchar_t[len];
		MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, buf, len);
		wstring r(buf);
		delete[] buf;

		return r;
	}

	void ForeNext();
	void ForePrev();
	void BackNext();
	void BackPrev();

	void SetFore(unsigned int iIndex);
	void SetBack(unsigned int iIndex);

private:
	void AddKey();
	void CameraUpdate(float fTime);

	bool InnerMouse() const;

};

