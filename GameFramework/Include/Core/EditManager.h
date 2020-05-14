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

	// EditManager�� ������ ���� ó���� �ϴ� �������̴�.

private:
	class CObj*		m_pEditObject;
	vector<ClipCoord>	m_vecCoord;
	Position			m_tBegin;
	Position			m_tEnd;
	Position			m_tMouseSrcPos;
	Position			m_tMouseScreenPos;
	COORD_STATE			m_eState; // ���콺��, �����Ӱ��� �������� 

	vector<int>			m_vecCollisionCoordIndex; // ���� �浹�� vecCoord�� �ε���
	vector<int>			m_vecEditCoordIndex;
	class CCollider*	m_pMouseCollider;
	Position			m_tIntersect; // ���콺�� �������� �浹 ��ġ�� �����Ѵ�.

	bool				m_bOverlapCollision;
	// ���� �浹�̶�� �̸����ٴ� OverlapCollision  �ߺ� �浹�� ���ڴ�.
	// �����ִ� ��Ȳ���� ����ϴ� �Ŵϱ�.


	Position			m_tFrameSize;
	bool				m_bFrameSize;

	float				m_fMoveScale;

	vector<ClipCoord>	m_vecPrevCoord; // ������� �߰��ϰų�, ������ Coord������ ����ȴ�.

	COORD_CHANGE_STATE	m_eChange;

	int					m_iPrevFrameNumber;

	bool					m_bSaveOption;
	ANIMATION_OPTION		m_eSaveOption;

	class CAnimation*	m_pAnimation;
	
	// ���� ������ ����
	bool				m_bFrameAdj;
	Position			m_tFrameAdj;

	// Ư�� �������� �������� �Ͽ� ������ �� ���Ŀ�
	// �������� ���� �����ӿ� ���Ͽ� ������ ���Ѵ�.
	STATIC_STATE			m_eStatic;

	
	Size					m_tBmpSize;

	// �о�� �ִϸ��̼��� �ɼ��� �����ϴ� ����
	ANIMATION_OPTION		m_eAniOption;

	string					m_strCurTag;

	// �ڸ��� ����� �Ǵ� Ŭ���̴�.
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

	// �ִϸ��̼� ���� ����.
	void ClipCut();

	// �¿� ��ȯ�Ͽ� �ٿ��ֱ�.
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
	bool IsIndex(int iIndex); // vecCoordIndex�� �ش� �ε����� �ִ��� Ȯ�����ش�.

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

	// �ܼ� Enableó��.
	void BasicInput(float fTime);

	bool IsCollision(int iIndex);
	void TextRender(HDC hDC, float fTime);
	void ObjectChange();
};

