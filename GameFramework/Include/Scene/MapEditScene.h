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


// ��忡 ���� �޶����� �ɼ� ����...
// ��尡 ���Ѵ� -> �ش� �ɼ��� NONE���� �ʱ�ȭ�� �صδ� ���� ���� ������ ? �׳� �ֵ� ������ ����

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
	EDIT_MODE	m_eMode;			// ������ ���� ���. �̹���, �� ������ �ִ�.
	TILE_OPTION m_eTileOption;			// ������ �ɼ����� ������ Ÿ���� �����ϱ� �����̴�.
	class CInputComponent* m_pInputComponent;	// �Լ��� ����Ͽ� ����ϱ� ���ؼ� Inputcomponent�� ������.
	class		CStage* m_pStage;			// Ÿ�� ������ ���� �������� ��ü ���.

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

	// ������ ���� ���ýÿ� �̿��� ���̴�.
	Position		m_tPos;


	// BackGround / ForeGround
	vector<ClipCoord> m_vecBackGroundPos;
	vector<ClipCoord> m_vecForeGroundPos;
	unsigned int m_iBack; 
	unsigned int m_iFore;

	 // Tile�� ������ ������ �ʿ������� ���� ���θ� Ȯ���ϱ� ���ؼ�, �̸� Size�� �޾Ƶα� ���� ����ü.
	Size	m_tPrevStageSize;
	Size	m_tCurrentStageSize;

	// Render On/Off
	bool	m_bTileRender;
	bool	m_bLineRender;
	bool	m_bSpawnerRender;
	// ������ BackGround��, ForeGround ����� �������� ���� ����... 

	


	Position m_tMousePos;

	EDIT_STATE m_eState; // �ϰ������� ����ϴ� flag
	Position	m_tClickPos;

	Size		m_tTileSize;

	Position m_tRenderOffset; // ��Ʈ�� Ű�� ���� ������ ��.

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
	// ���� �Լ�.  : ��ȿ�� �˻�� ���� �ʴ´�. Very Dangerous 
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

