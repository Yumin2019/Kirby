#pragma once

#include "../Game.h"

enum STATIC_STATE
{
	SS_NONE,
	SS_X,
	SS_Y,
	SS_END
};

enum COORD_STATE
{
	CS_NONE,
	CS_CHANGE_DOING,
	CS_FRAME_DOING
};

enum COORD_CHANGE_STATE
{
	CCS_NONE,
	CCS_MOVE,
	CCS_RESIZE
};

enum BRUSH_TYPE
{
	BT_RED,
	BT_GREEN,
	BT_YELLOW,
	BT_WHITE,
	BT_MAGENTA,
	BT_BLACK,
	BT_END
};


class CEditManager
{
	DECLARE_SINGLE(CEditManager);

	// EditManager는 편집에 대한 처리를 하는 관리자이다.

private:
	class CObj*		m_pEditObject;
	vector<ClipCoord>	m_vecCoord;
	Position			m_tBegin;
	Position			m_tEnd;
	Position			m_tMouseSrcPos;
	Position			m_tMouseScreenPos;
	COORD_STATE			m_eState; // 마우스와, 프레임간의 상태정보 

	vector<int>			m_vecCollisionCoordIndex; // 현재 충돌한 vecCoord의 인덱스
	vector<int>			m_vecEditCoordIndex;
	class CCollider*	m_pMouseCollider;
	Position			m_tIntersect; // 마우스와 프레임의 충돌 위치를 저장한다.

	bool				m_bOverlapCollision;
	// 다중 충돌이라는 이름보다는 OverlapCollision  중복 충돌이 낫겠다.
	// 겹쳐있는 상황에서 사용하는 거니까.


	Position			m_tFrameSize;
	bool				m_bFrameSize;

	float				m_fMoveScale;

	vector<ClipCoord>	m_vecPrevCoord; // 방금전에 추가하거나, 삭제한 Coord정보가 저장된다.

	COORD_CHANGE_STATE	m_eChange;

	int					m_iPrevFrameNumber;

	bool					m_bSaveOption;
	ANIMATION_OPTION		m_eSaveOption;

	class CAnimation*	m_pAnimation;
	
	// 고정 프레임 간격
	bool				m_bFrameAdj;
	Position			m_tFrameAdj;

	// 특정 프레임을 기준으로 하여 고정을 한 이후에
	// 다음번에 생길 프레임에 대하여 제한을 가한다.
	STATIC_STATE			m_eStatic;

	
	Size					m_tBmpSize;

	// 읽어온 애니메이션을 옵션을 저장하는 변수
	ANIMATION_OPTION		m_eAniOption;

	string					m_strCurTag;

	// 자르는 대상이 되는 클립이다.
	PAnimationClip			m_pCutClip;

	bool					m_bCut;
	string					m_strCut;
	Position				m_tCutFrameSize;


	vector<string>			m_vecClipName;
	unsigned int			m_iCurClipIndex;
	
	// Offset Change
	bool					m_bOffset;
	Position				m_tOffset;


	// BRUSH
	BRUSH_TYPE			m_eBrushType;


	// static offset
	Position	m_tSaveOffset;

	
	Position	m_tPrintOffset;

public:
	bool Init();
	void Update(float fTime);
	void Render(HDC hDC, float fTime);
	void Collision(float fTime);

	int GetClipNameIndex(bool bScale);

	Position GetPrintOffset() const
	{
		return m_tPrintOffset;
	}

	vector<ClipCoord>& GetClipCoord() 
	{
		return m_vecCoord;
	}

	Position GetOffset() const
	{
		return m_tOffset;
	}

	bool GetOffsetEnable() const
	{
		return m_bOffset;
	}

	BRUSH_TYPE GetBrushType() const
	{
		return m_eBrushType;
	}

private:

	// 애니메이션 정보 저장.
	void ClipCut();

	// 좌우 변환하여 붙여넣기.
	void ClipConversion(bool bChange = true);


	void CoordConversion(vector<ClipCoord>& vecSrcCoord, bool bChange = true);
	void CoordConversion(ClipCoord& tCoord, float fMidX);
	void CoordConversion(Position& tPos, float fMidX);
	

	void CameraInput(float fTime);
	void OptionChange();
	void StaticStateChange();
	void EditObjectRender(HDC hDC, float fTime);
	void EditRender(HDC hDC, float fTime);
	void EditObjectUpdate(float fTime);
	void MouseUpdate(float fTime);
	void AddInput();
	void CoordRender(HDC hDC, float fTime);
	void CoordDelete(unsigned int iIndex);
	void EditCoordDelete(int iIndex);

	void Input(float fTime);
	void MouseOverCoord();
	void MouseOverSoleCoord();
	void StaticLineRender(HDC hDC, float fTime);
	bool IsIndex(int iIndex); // vecCoordIndex에 해당 인덱스가 있는지 확인해준다.

	void MoveEditCoord(float fScale, bool bHorizon = true);
	void MoveEditCoord(const Position& tMouseMove);
	void ResizeEditCoord(float fScale, bool bHorizon = true);
	void ResizeEditCoord(const Position& tMouseMove);
	
	void Save(const char* strFrontTag = "");
	void SaveAll(const char* strFrontTag = "");
	void Load(const char* strFrontTag = "");
	
	void AnimationChange(bool bScale);
	void CtrlV();
	void CtrlA();

	// 단순 Enable처리.
	void BasicInput(float fTime);

	bool IsCollision(int iIndex);
	void TextRender(HDC hDC, float fTime);
	void ObjectChange();
};

