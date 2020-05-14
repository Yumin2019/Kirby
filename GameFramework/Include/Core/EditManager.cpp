#include "EditManager.h"
#include "../Core/Input.h"
#include "../Object/Obj.h"
#include "../Core/SceneManager.h"
#include "../Core/GameManager.h"
#include "../Collision/Collider.h"
#include "../Math.h"
#include "../Animation/Animation.h"
#include "../Texture.h"
#include <algorithm>

DEFINITION_SINGLE(CEditManager);

CEditManager::CEditManager() :
	m_pEditObject(nullptr),
	m_pMouseCollider(nullptr),
	m_eState(CS_NONE),
	m_bOverlapCollision(false),
	m_bFrameSize(false),
	m_fMoveScale(1.f),
	m_eChange(CCS_MOVE),
	m_eSaveOption(AO_LOOP),
	m_pAnimation(nullptr),
	m_pCutClip(nullptr),
	m_eStatic(SS_NONE),
	m_bFrameAdj(false),
	m_bCut(false),
	m_tFrameAdj(10, 10),
	m_eAniOption(AO_LOOP),
	m_iCurClipIndex(0),
	m_bOffset(false),
	m_bSaveOption(false)
{
	m_eBrushType = BT_RED;
}


CEditManager::~CEditManager()
{
	//Save("Auto/"); // �޼��� �ڽ��� �ѱ��, �׳� �ڵ����� ������ Ŭ���� �����ϴ� ����̴�. �ڿ� auto�±װ� ����

	SAFE_RELEASE(m_pEditObject);
	SAFE_RELEASE(m_pMouseCollider);
}

bool CEditManager::Init()
{
	

	// ������ ����� �о�´�. 
	m_pEditObject = GET_SINGLE(CSceneManager)->FindNextSceneObject("Kirby");

	m_strCurTag = m_pEditObject->GetTag();

	m_pAnimation = m_pEditObject->GetAnimation();

	PAnimationClip pClip = m_pAnimation->GetClip();

	// �������� �о�´�.
	m_tOffset = pClip->tOffset * -1.f;

	// ��Ʈ���� ����� �о�´�. ���⼭ ������Ʈ�� ��Ʋ�� �̹����� �Բ� ����ϰ� ���� ���̹Ƿ�
	// �ѹ��� ��Ʈ���� ����� �о���� �ȴ�.
	m_tBmpSize = m_pAnimation->GetBmpSize();

	GET_SINGLE(CSceneManager)->LimitCameraDisEnable();

	if (m_tBmpSize.x >= RS_WIDTH && m_tBmpSize.y >= RS_HEIGHT)
		GET_SINGLE(CSceneManager)->SetLimitCamera(m_tBmpSize);
	
	// ���� ����� Ŭ���� ���� ������ �Ǿ� ���� ���ɼ��� �ִ�.
	m_vecCoord = m_pEditObject->GetvecCoord();


	m_iPrevFrameNumber = m_vecCoord.size();

	// LOOP�� �����ϱ� ���� �ִϸ��̼� �ɼ��� �޾Ƶд� (����� �ɼǰ�)
	m_eAniOption = pClip->eOption;

	// ������ ���ϴ� �ɼ��� ���߿� �ְ� �ϴ�, ������忡���� loopó���� �ؾ��Ѵ�.
	if (pClip->eOption != AO_ONCE_LOOP)
		pClip->eOption = AO_LOOP;

	// ClipName�� �޾ƿ´�.
	m_vecClipName.clear();

	m_pEditObject->AddClipName(m_vecClipName);

	m_vecCoord.reserve(30);
	m_vecEditCoordIndex.reserve(30);
	m_vecPrevCoord.reserve(30);


	// ������忡�� ����� Ű�� �߰��Ѵ�.
	AddInput();

	m_pMouseCollider = GET_SINGLE(CInput)->GetMouseCollider();
	m_pMouseCollider->AddRef();

	return true;
}

void CEditManager::Update(float fTime)
{

	// Edit �Է�ó�� : �����ϰ� �ִ���� �ϴ���, ī�޶�� �������� �Ѵ�.
	Input(fTime);


	if (GET_SINGLE(CGameManager)->GetPause())
		fTime = 0.f;

	// ���콺 ����
	MouseUpdate(fTime);
	
	// �߰��� ������ ������ ���ٸ�, �ִϸ��̼��� ������ �ʴ´�.
	if (!m_vecCoord.empty())
		EditObjectUpdate(fTime);




	// Frame�� ������ ���ߴ°� ? 
	size_t iSize = m_vecCoord.size();

	if (m_iPrevFrameNumber != iSize)
	{
		// �ٲ� ��쿡�� ���� �ִϸ��̼� Ŭ���� ������ �����ؾ� �Ѵ�.
		PAnimationClip pClip = m_pAnimation->GetClip();


		if (!GET_SINGLE(CGameManager)->GetPause())
		{
			// �������� ���� ����� �����ϰ� 0�����Ӻ��� ����.
			// �����ִ� ����� �׳� �д�.
			pClip->iFrame = 0;
			pClip->fTime = 0.f;
		}
		m_iPrevFrameNumber = m_vecCoord.size();

		// �������� ������ �޶����ٴ� �Ҹ���, ��ü �÷��̽ð��� �޶����ٴ� ���� �ǹ��Ѵ�.
		// Ȥ��, Ŭ�� ��ü�� �ٲ���ų�.
		// �ƹ�ư �̰Ͱ� ������� �ϴ� �������� ��ȭ�� ����ٸ�, fPlayTime�� �������� �ʿ䰡 �ִ�.

		pClip->fPlayTime = m_vecCoord.size() * pClip->fFrameTime;

		// �ɼ� �ð��� ��쿡�� Ư�� �ð��� ������ ������ų�, Ư�� �ð��� ������ return �ǰų� �ϴ� ó���̱� ������ ������ �ʿ����.
	}


}

void CEditManager::Render(HDC hDC, float fTime)
{
	// �ҽ� �̹����� �ִϸ��̼� ������ ���.
	EditObjectRender(hDC, fTime);

	/*
		vacant ���¿����� �Լ��� ȣ���� �Ǵµ�, �ִϸ��̼ǿ� �߰��� �������� ��� �ε����� ������ ���� ���Ѵ�.
		����, �ִϸ��̼��� ����ó���� ����, ������ �ҽ��� Ŭ�������� ����Ѵ�.
	*/

	// StaticX, StaticY�� ���� �� ���
	StaticLineRender(hDC, fTime);

	// �� ���� ���� ������ ���.
	EditRender(hDC, fTime);

}

void CEditManager::ClipCut()
{
	// ���� Ŭ���� ������ �����صд�. 
	// �̸� ������ �̸� �޾Ƶд�.

	if (m_vecCoord.empty())
	{
		MessageBox(nullptr, TEXT("NO COORD"), TEXT("Error"), MB_OK);
		return;
	}



	m_pCutClip = m_pAnimation->GetClip();

	m_strCut = m_pEditObject->GetTag();

	// �̰� ���� �����ص� ������ �������̱� ������ vector�� ���� �ִ� (������ ������ ������� ������ �ؾ� �Ѵ�.)

	m_tCutFrameSize = m_vecCoord[0].tEnd - m_vecCoord[0].tStart;

	// ������ ����� �� �����ִ� ���¶��,(���� ��� Load ����)
	// Coord������ �̿��ؼ� �����Ѵ�.
	// Coord ������ ���µ� Ű�� ���� ���� ����ó���ϰ� ������ �ݵ�� �ϳ��� �������̶� �����Ѵ�.

}

void CEditManager::ClipConversion(bool bChange)
{
	ShowCursor(TRUE);

	if (!m_pCutClip)
	{
		MessageBox(nullptr, TEXT("�ڸ� Ŭ���� �����ϴ�."), TEXT("Error"), MB_OK);

		return;

		// �ڸ� Ŭ���� ���� ��� �׳� ��� �޼���.
	}

	// �߶�� Ŭ���� ������Ʈ�� ���� �����ϴ� ������Ʈ�� �ٸ� �� �ִ�.
	//else if (m_strCurTag != m_strCut)
	//{
	//	// �߶�� ������Ʈ�� �±׿� ���� �����ϰ� �ִ� ������Ʈ�� �±װ� �ٸ��� �߸��� ���̴�.

	//	MessageBox(nullptr, TEXT("Ŭ���� ������Ʈ�� �ٸ��ϴ�."), TEXT("Error"), MB_OK);

	//	// ex : Kirby / KirbyWalkLeft Clip -> �����Ϸ��� ������Ʈ�� Burt .
	//	// -> Error ��Ȳ�̶�� �� �� �ִ�.

	//	return;
	//}


	// �ڸ� Ŭ���� �ִ� ��쿡�� ���������� ��ȯ���� �����. (���� ������ ���󰡴� ó��)
	if (MessageBox(nullptr, TEXT("Ŭ���� ������ ��ȯ �Ͻðڽ��ϱ� ?"), TEXT("Ŭ�� ��ȯ"), MB_YESNO) == IDYES)
	{
		// CutClip�� ������ �����ؿ´�.
		

		// ���� Ŭ���� ������ ���´�.
		PAnimationClip pClip = m_pAnimation->GetClip();

		pClip->eOption = m_pCutClip->eOption;
		pClip->iOnceLoopFrame = m_pCutClip->iOnceLoopFrame;
		pClip->eType = m_pCutClip->eType;
		// ������ �ؽ�ó�� Cut�� �ؽ�ó�� ��ü�ϸ� �� ����.
		// right������ Texture�� �� ���� Ŭ���� ��������.
		pClip->iColorKey = m_pCutClip->iColorKey;
		pClip->bColorKey = m_pCutClip->bColorKey;
		pClip->iFrame = 0;
		pClip->vecCoord = m_pCutClip->vecCoord; // src�� �Ǵ� vector�� �ϴ� �޾Ƽ� ���縦 �ϰ�,
		pClip->tOffset = m_pCutClip->tOffset;  // �ٽ� ������ �ϴ��� �ϴ� �����ϴ� ���� �´�.

		pClip->fTime = 0.f;
		pClip->fPlayTime = m_pCutClip->fPlayTime;
		pClip->fFrameTime = m_pCutClip->fFrameTime;
		pClip->fOptionTime = m_pCutClip->fOptionTime;
		pClip->fOptionPlayTime = m_pCutClip->fOptionPlayTime;

		// �̸��� ������ Ŭ���� �⺻���� ����, �÷��� �ð�, �ɼ�, ���� ���� ��Ȳ ���� ������ �����Ѵ�.

		// ��ǥ�� ��ȯ�ϴ� �۾��� �����Ѵ�.
		CoordConversion(pClip->vecCoord, bChange); // �ش� src������ �̿��Ͽ� ��ǥ�� ��ȯ.

		// ����.
		m_vecCoord = pClip->vecCoord;

		m_iPrevFrameNumber = m_vecCoord.size();

		m_eAniOption = m_pCutClip->eOption;

		m_tFrameSize = m_tCutFrameSize;

		m_pEditObject->SetSize(m_tFrameSize);

		m_bFrameSize = true;

		m_tOffset = m_pCutClip->tOffset * -1.f;

		// ����� �ɼ��� ������ �޾ƿ´�.
		// (�ٵ� �ᱹ���� Save�� �������� ó���ϰ�����, �ƹ�ư ������ �ɼ��� �����ִ� ó���� �ȴٴ� ���̴�)

		if (m_eAniOption != AO_ONCE_LOOP)
			pClip->eOption = AO_LOOP;


		// ����� ���Ŀ��� Ǯ���ֵ��� �Ѵ�.
		//m_pCutClip = nullptr;

	}

	// ������ �ʴ� ��� �׳� ����.


	ShowCursor(FALSE);

}

void CEditManager::CoordConversion(vector<ClipCoord>& vecSrcCoord, bool bChange)
{
	// �̰����� ��ǥ�� ������ �츮�� �� Ŭ�� ���� �ϳ��ϳ� ���� ��ǥ�� �����ش�.
	// �¿� �̹����� ���ؼ� ����� ���̹Ƿ� �߽ɸ� ���ϸ� �ȴ�.

	if (bChange)
	{
		float fMidX = m_tBmpSize.x / 2.f;

		size_t iSize = vecSrcCoord.size();

		for (size_t i = 0; i < iSize; ++i)
		{
			CoordConversion(vecSrcCoord[i], fMidX);
		}

	}

	// �̷��� �ϸ�  vector�� ������� (�߰��� �������) �״�� ��ġ�� �����Ͽ� 
	// �������� ����ְ� �ȴ�.
}

void CEditManager::CoordConversion(ClipCoord & tCoord, float fMidX)
{
	// �̰��� ���� Coord�� ��ǥ�� �̹����� �߽� Y���� �������� ��ȯ�ϴ� �Լ��̴�.
	// A -> A'�� ��ȯ�ϰ� �̰��� B'�� �����.
	// D -> D'�� ��ȯ�ϰ� �̰��� C'�� �����.

	// �̰��� LR��ȯ�̵� RL��ȯ�̵� �����ϴ�.

	// fMid�� X�� �Ÿ��� ���Ѵ�.
	// A'�� DestA��� ǥ���ϰڴ�.

	Position tBegin = tCoord.tStart;

	if(tCoord.tStart.x < fMidX)
		tBegin.x = fMidX + abs(fMidX - tBegin.x);
	else
		tBegin.x = fMidX - abs(fMidX - tBegin.x);
	// y �� �״��.

	// �̷��� ��ȯ�� ��ǥ�� ���ʻ�ܰ� ������ �ϴ����� �����.

	tBegin.x -= m_tCutFrameSize.x;


	tCoord.tStart = tBegin;
	tCoord.tEnd = tBegin + m_tCutFrameSize;
}



void CEditManager::CameraInput(float fTime)
{
	GET_SINGLE(CSceneManager)->CameraUpdate(fTime);
}

void CEditManager::OptionChange()
{
	if (!m_bSaveOption) // saved�� ������ �Ѵٴ� �Ҹ�
	{
		m_bSaveOption = true; // �̰��� Ǯ���ش�.
		// ���̺� �ɼ��� false�� �ȴٴ� �Ҹ��� ... �ɼ��� ����Ѵٴ� �Ҹ���.
		// 0������ �����Ѵ�.
	}
	else
	{
		// saved �ɼ��� Ǯ���� ���¶�� �ɼ��� �ٲ� �� �ֵ��� �Ѵ�.
		int iOption = m_eSaveOption + 1;

		if (iOption >= AO_END)
		{
			// ���������� ����, SaveOption���� ����.
			iOption = 0;
			m_eSaveOption = (ANIMATION_OPTION)iOption;
			m_bSaveOption = false;

			return;
		}

		m_eSaveOption = (ANIMATION_OPTION)iOption;

	}
}

void CEditManager::StaticStateChange()
{

	int iStatic = m_eStatic + 1;

	if (iStatic >= SS_END)
	{	
		iStatic = 0;
	}

	m_eStatic = (STATIC_STATE)iStatic;


}

void CEditManager::EditObjectRender(HDC hDC, float fTime)
{
	m_pEditObject->EditRender(hDC, fTime, m_bOffset, m_tOffset);
}

void CEditManager::EditRender(HDC hDC, float fTime)
{
	// ���� ������ ǥ���� �ؽ�Ʈ�� ����Ѵ�.
	TextRender(hDC, fTime);

	// �������� �߰��ߴ� ��� Coord ���� ���.
	CoordRender(hDC, fTime);

}

void CEditManager::EditObjectUpdate(float fTime)
{
	m_pAnimation->Update(fTime);
}

void CEditManager::MouseUpdate(float fTime)
{
	// ������ ��ܿ� ���� ���콺�� src������ ��ġ�� ����Ѵ�.
	m_tMouseScreenPos = GET_SINGLE(CInput)->GetMousePos();
	m_tMouseSrcPos = m_tMouseScreenPos + GET_SINGLE(CSceneManager)->GetCameraPos();

}

void CEditManager::AddInput()
{
	

	// �ִϸ��̼��� �ӵ��� �����ϴ� Ű�� �����.
	GET_SINGLE(CInput)->AddNormalKey("--FrameTime", VK_OEM_MINUS);
	GET_SINGLE(CInput)->AddNormalKey("++FrameTime", VK_OEM_PLUS);

	// �ִϸ��̼� ��ü.
	GET_SINGLE(CInput)->AddNormalKey("++Animation", VK_F5);
	GET_SINGLE(CInput)->AddNormalKey("--Animation", VK_F6);


	// Camera Move 
	GET_SINGLE(CInput)->AddNormalKey("D", 'D');
	GET_SINGLE(CInput)->AddNormalKey("W", 'W');
	GET_SINGLE(CInput)->AddNormalKey("A", 'A');
	GET_SINGLE(CInput)->AddNormalKey("S", 'S');

	// �ӵ� ���� ����.
	GET_SINGLE(CInput)->AddNormalKey("F1", VK_F1);
	GET_SINGLE(CInput)->AddNormalKey("F2", VK_F2);
	// ���� Ȱ��ȭ�� ���������� ũ�⸦ ������Ŵ.  �������� �����ִ� ��쿡�� ó������ ����. 
	// �̷��� ������ ���Ŀ� ������ ��Ƶ� ��� ������ ������ �����ӿ� �°� ������.
	GET_SINGLE(CInput)->AddNormalKey("F3", VK_F3);

	// Save Option Change
	GET_SINGLE(CInput)->AddNormalKey("F4", VK_F4);

	// Once Loop Begin Frame
	GET_SINGLE(CInput)->AddNormalKey("F7", VK_F7);

	// reset
	GET_SINGLE(CInput)->AddNormalKey("F8", VK_F8);

	// Offset Copy & paste
	GET_SINGLE(CInput)->AddNormalKey("5", '5');
	GET_SINGLE(CInput)->AddNormalKey("6", '6');

	// Brush Color
	GET_SINGLE(CInput)->AddNormalKey("7", '7');

	// Offset
	GET_SINGLE(CInput)->AddNormalKey("8", '8');

	// Static State
	GET_SINGLE(CInput)->AddNormalKey("9", '9');

	// Frame Adj Enable
	GET_SINGLE(CInput)->AddNormalKey("0", '0');



	// Collision Enable
	GET_SINGLE(CInput)->AddNormalKey("Q", 'Q');
	// all Delete
	GET_SINGLE(CInput)->AddNormalKey("ESC", VK_ESCAPE);


	// COORD KEY
	GET_SINGLE(CInput)->AddNormalKey("CoordDelete", VK_BACK);

	// Coord Move & Resize Keyboard
	GET_SINGLE(CInput)->AddNormalKey("Right", VK_RIGHT);
	GET_SINGLE(CInput)->AddNormalKey("Left", VK_LEFT);
	GET_SINGLE(CInput)->AddNormalKey("Top", VK_UP);
	GET_SINGLE(CInput)->AddNormalKey("Bottom", VK_DOWN);

	// Coord Move Speed
	GET_SINGLE(CInput)->AddNormalKey("++MoveSpeed", '1');
	GET_SINGLE(CInput)->AddNormalKey("--MoveSpeed", '2');

	// Coord Move Mouse
	GET_SINGLE(CInput)->AddNormalKey("E", 'E'); // Pos
	GET_SINGLE(CInput)->AddNormalKey("R", 'R'); // Size

	// EditCoord clear
	GET_SINGLE(CInput)->AddNormalKey("F", 'F'); 

	// ex : E + Ű���� ��ġ�̵�.
	//		E + ���콺 ��ġ�̵�
	//		R + Ű���� ũ�⺯��
	//		R + ���콺	ũ�⺯��


	// Most Newly Added Coord   Revive
	GET_SINGLE(CInput)->AddNormalKey("CtrlZ", 'Z', VK_LCONTROL); // DELETE : ���� �ֱ� ��ǥ�� ���ش�.
	// �ٵ� �ش� ��ǥ�� �������� ���ɼ��� �����Ƿ� vector�� �޾Ƶд�.

	// �����ϱ�
	GET_SINGLE(CInput)->AddNormalKey("CtrlS", 'S', VK_LCONTROL);
	
	// �ҷ�����
	GET_SINGLE(CInput)->AddNormalKey("CtrlL", 'L', VK_LCONTROL);

	// ���� ������ �ٿ��ֱ�
	GET_SINGLE(CInput)->AddNormalKey("CtrlV", 'V', VK_LCONTROL);

	// ��� ������ ������� ����.
	GET_SINGLE(CInput)->AddNormalKey("CtrlA", 'A', VK_LCONTROL);

	// Frame ���� ����.
	GET_SINGLE(CInput)->AddNormalKey("++FrameAdjX", VK_NEXT); // x ����
	GET_SINGLE(CInput)->AddNormalKey("--FrameAdjX", VK_DELETE); // x ����
	GET_SINGLE(CInput)->AddNormalKey("++FrameAdjY", VK_HOME); // y ����
	GET_SINGLE(CInput)->AddNormalKey("--FrameAdjY", VK_END); // y ����

	// �ִϸ��̼� ������ �ѱ��. ���� P(����) ���·� ����� ����
	GET_SINGLE(CInput)->AddNormalKey("3", '3'); // Next 
	GET_SINGLE(CInput)->AddNormalKey("4", '4'); // Prev


	// ��ȯ ��� Ȱ��ȭ.
	GET_SINGLE(CInput)->AddNormalKey("ConversionMode", 'Y');

	// �纻����.
	GET_SINGLE(CInput)->AddNormalKey("ClipCut", 'U'); 

	// �纻 �ٿ� �ֱ�. Reverse
	GET_SINGLE(CInput)->AddNormalKey("ClipCopy", 'I');

	// �纻 �ٿ� �ֱ� NOT Reverse
	GET_SINGLE(CInput)->AddNormalKey("ClipCopy2", 'O');

	// �����ϴ� ������Ʈ ����.
	GET_SINGLE(CInput)->AddNormalKey("ObjectChange", VK_TAB);

	// Size ����
	GET_SINGLE(CInput)->AddNormalKey("Shift", VK_SHIFT);

	// ��� ��ġ ����
	GET_SINGLE(CInput)->AddNormalKey("Ctrl", VK_LCONTROL);



}

void CEditManager::CoordRender(HDC hDC, float fTime)
{
	// ���� �����ִ� coord������ ��� ����Ѵ�.

	Position tCameraPos = GET_SINGLE(CSceneManager)->GetCameraPos();

	size_t iSize = m_vecCoord.size();

	TCHAR strPos[32] = {};

	for (size_t i = 0; i < iSize; ++i)
	{
		RECT rc;

		rc.left = (int)(m_vecCoord[i].tStart.x - tCameraPos.x);
		rc.right = (int)(m_vecCoord[i].tEnd.x - tCameraPos.x);
		rc.top = (int)(m_vecCoord[i].tStart.y  - tCameraPos.y);
		rc.bottom = (int)(m_vecCoord[i].tEnd.y - tCameraPos.y);

		if(IsIndex(i))
			FrameRect(hDC, &rc, REDBRUSH);
		else
			FrameRect(hDC, &rc, GREENBRUSH);


		// ������ ��� �� �Ʒ��� �ش� tStart�� ��ǥ�� ������ش�.
		wsprintf(strPos, TEXT("%d. (%d, %d)"), i, (int)m_vecCoord[i].tStart.x, (int)m_vecCoord[i].tStart.y);

		int iRenderX = RS_WIDTH / 2 - 450 + (i / 5) * 100;
		int iRenderY = (i % 5) * 20 + 20;

		TextOut(hDC, iRenderX, iRenderY, strPos, lstrlen(strPos));
		
	}



	// ���� �׸��� �ִ� Coord������ ������ش�.

	if (m_eState == CS_FRAME_DOING)
	{
		// SRC�� ��ǥ�� ��ũ�� ��ǥ�� �ٲٷ��� �Ѵ�.
		// SRC TO SCREEN  (��ũ�� ��ǥ + ī�޶� ��ǥ) - ī�޶� ��ǥ = ��ũ�� ��ǥ.
		// SCREEN TO SRC  ��ũ�� ��ǥ + ī�޶� ��ǥ.

		RECT rc;

		rc.left = (int)(m_tBegin.x - tCameraPos.x);
		rc.top =  (int)(m_tBegin.y - tCameraPos.y);
		rc.right = (int)(m_tEnd.x - tCameraPos.x);
		rc.bottom = (int)(m_tEnd.y - tCameraPos.y);

		FrameRect(hDC, &rc, YELLOWBRUSH);
	}
}

void CEditManager::CoordDelete(unsigned int iIndex)
{
	size_t iSize = m_vecCoord.size();

	if (iIndex >= iSize)
		return;

	// ������ ������ �������� �ӽ÷� ��Ƶд�.
	m_vecPrevCoord.push_back(m_vecCoord[iIndex]);

	for (size_t i = iIndex; i < iSize - 1; ++i)
	{
		// �� ĭ�� �ű�� ����
		m_vecCoord[i] = m_vecCoord[i + 1];
	}

	m_vecCoord.pop_back();

}

void CEditManager::EditCoordDelete(int iIndex)
{
	size_t iSize = m_vecEditCoordIndex.size();

	for (size_t i = 0; i < iSize; ++i)
	{
		if (m_vecEditCoordIndex[i] == iIndex)
		{
			// ���ַ��� �ε����� ������.
			// �ڿ� �ִ� �ε����� ����� ������.

			for (size_t j = i; j < iSize - 1; ++j)
			{
				m_vecEditCoordIndex[j] = m_vecEditCoordIndex[j + 1];
			}

			m_vecEditCoordIndex.pop_back();

			break;
		}
	}
}

void CEditManager::Input(float fTime)
{
	// ���콺�� �ٱ����� �����ٸ�, �ش� �۾��� �ϰ� ���� ���� ������ �ν��ϰ�
	// �Է��� ���´�.
	// (ī�޶� �̵��ϴ� Ű�� ���ƹ����� ó�� + ����Ű ����)
	if (!GET_SINGLE(CInput)->MouseIsScreen())
	{
		m_eState = CS_NONE;
		return;
	}



	// CameraInput
	CameraInput(fTime);

	// �⺻���� Ȱ��ȭ/��Ȱ��ȭ, ũ�� ������ ���� ó���� �ϴ� �Լ� (���� �ܼ��� Ű�Է�)
	BasicInput(fTime);

	// Coord Move ������ �̵�.   ���콺�� �Ʒ����� ó��.

	if (!m_bOffset)
	{
		if (!m_vecEditCoordIndex.empty())
		{
			if (m_eChange == CCS_MOVE) // E + Ű���� : Ű����� ��ġ�̵�.
			{



				if (KEYDOWN("Right"))
				{
					MoveEditCoord(m_fMoveScale);
				}
				else if (KEYDOWN("Left"))
				{
					MoveEditCoord(-m_fMoveScale);
				}


				if (KEYDOWN("Top"))
				{
					MoveEditCoord(-m_fMoveScale, false);
				}
				else if (KEYDOWN("Bottom"))
				{
					MoveEditCoord(m_fMoveScale, false);

				}
				// E, R�� PRESS�� ���¶�� ���� �����ʹ�ư�� ������ �ʴ´ٴ� ���̴�.

			}

			else if (m_eChange == CCS_RESIZE) // R + Ű���� : Ű����� ������ ����
			{



				if (KEYDOWN("Right"))
				{
					ResizeEditCoord(m_fMoveScale);
				}
				else if (KEYDOWN("Left"))
				{
					ResizeEditCoord(-m_fMoveScale);
				}


				if (KEYDOWN("Top"))
				{
					ResizeEditCoord(-m_fMoveScale, false);
				}
				else if (KEYDOWN("Bottom"))
				{
					ResizeEditCoord(m_fMoveScale, false);

				}



			}


			// �ϳ��� ���� �������� ����.
			if (m_vecEditCoordIndex.size() == 1)
			{

				// Once loop Frame Set
				if (KEYDOWN("F7") && (m_eSaveOption == AO_ONCE_LOOP))
				{

					// �ϳ��� �������� ���� ���¿��� F7�� ���� ���
					// �ش� �������� iOnceFrame���� �ִ� ���̴�.
					// Once Loop�ɼ��� �����ص� ���¿��� �ش� �������� OnceLoopFrame���� �����ϴ� ���.

					m_pAnimation->GetClip()->iOnceLoopFrame = m_vecEditCoordIndex[0];

					m_pAnimation->GetClip()->eOption = AO_ONCE_LOOP;

				}




				if (KEYDOWN("CtrlV"))
				{

					CtrlV();

				}



			}
		}
	}

	else
	{
		// Ű���带 ���� ������ ����.

		if (KEYDOWN("Right"))
		{
			m_tOffset.x += m_fMoveScale;
			m_pAnimation->GetClip()->tOffset = m_tOffset * -1.f;


		}
		else if (KEYDOWN("Left"))
		{
			m_tOffset.x -= m_fMoveScale;
			m_pAnimation->GetClip()->tOffset = m_tOffset * -1.f;


		}


		if (KEYDOWN("Top"))
		{
			m_tOffset.y -= m_fMoveScale;
			m_pAnimation->GetClip()->tOffset = m_tOffset * -1.f;


		}
		else if (KEYDOWN("Bottom"))
		{
			m_tOffset.y += m_fMoveScale;
			m_pAnimation->GetClip()->tOffset = m_tOffset * -1.f;


		}


	}


	// ���콺 Coord Change
	// Mouse + E : ��ġ ����

	if (!m_bOffset) // Offset Off 
	{


		if (KEYPRESS("MouseRButton"))
		{
			Position m_tMouseMove = GET_SINGLE(CInput)->GetMouseMove();

			if (KEYPRESS("Shift"))
			{
				// ��� ������
				m_pEditObject->SetSize(m_pEditObject->GetSize() + m_tMouseMove);
			}

			else if (KEYPRESS("Ctrl"))
			{
				// ��� ��ġ����.
				m_tPrintOffset += m_tMouseMove;

				if (m_tPrintOffset.x < -1600.f || m_tPrintOffset.x > 1600.f ||
					m_tPrintOffset.y > 1600.f || m_tPrintOffset.y < -1600.f)
					m_tPrintOffset = Position(0.f, 0.f);
				

			}
			else
			{
				if (m_eChange == CCS_MOVE)
				{
					// Ȱ��ȭ�� Coord������ ũ�⸦ �ٲٰų� ��ġ�� �ٲٴ� ó���� �Ѵ�.


					// ũ�⸦ �ٲٰų�, Size�� �ٲٴ� ������ �Ѵ�. (�ߺ��� �� �ǵ���)

					MoveEditCoord(m_tMouseMove);
				}
				// Mouse + R : ũ�� ����
				else if (m_eChange == CCS_RESIZE)
				{

					ResizeEditCoord(m_tMouseMove);
				}
			}


		}
	}
	else
	{
		if (KEYPRESS("MouseRButton"))
		{
			Position m_tMouseMove = GET_SINGLE(CInput)->GetMouseMove();

			m_tOffset += m_tMouseMove;
			m_pAnimation->GetClip()->tOffset = m_tOffset * -1.f;
		}
	}

	// FrameSize
	if (KEYDOWN("F3"))
	{
		if (m_vecEditCoordIndex.size() == 1 && !m_bFrameSize)
		{
			int iIndex = m_vecEditCoordIndex[0];
			// �ϳ��� ����������, ó���Ѵ�.

			m_tFrameSize = m_vecCoord[iIndex].tEnd - m_vecCoord[iIndex].tStart;
			// ������ �ε����� �ش��ϴ� ������ũ�⸦ �����Ѵ�.

			m_bFrameSize = true;

			// ������ Size�� �������ٸ�, ���� �������� ������ ����� �°� ��ģ��.  
			// begin + frameSize


			size_t iSize = m_vecCoord.size();

			for (size_t i = 0; i < iSize; ++i)
			{
				m_vecCoord[i].tEnd = m_vecCoord[i].tStart + m_tFrameSize;
			}

			// ������ ����� �������� ����. �ش� ������ ������� Object�� ��»������ �ش�.
			m_pEditObject->SetSize(m_tFrameSize);


		}
		else if (m_bFrameSize)
			m_bFrameSize = !m_bFrameSize;
		
		// F3 Ű�� ���ؼ�, ������ �ϳ��� �� ���Ŀ� ������ ����� ������ �� �ִ�.
		// ���Ŀ� �� ������ ����� Ǯ�� �ʹٸ� �ٽ� F3Ű�� ������.
		// ���� �������� ������ �Ŀ� F3Ű�� ���� ��� ���� ó������ �ʴ´�.
	}

	// ���Ű ��ü : �浹Ȱ��ȭ(�� ���..) -> ��ģ���� �ߺ� �浹 Ȱ��ȭ.
	if (KEYDOWN("Q"))
	{
		m_bOverlapCollision = !m_bOverlapCollision;

	}

	// ��� ���� ����.
	if (KEYDOWN("ESC"))
	{

		ShowCursor(TRUE);

		if (m_bOffset)
		{
			if (MessageBox(nullptr, TEXT("������ �ʱ���·�"), TEXT("(0.0)"), MB_YESNO) == IDYES)
			{
				m_tOffset = Position(0.f, 0.f);
			}
		}
		else
		{
			// ������ �ӻ���
			if (MessageBox(nullptr, TEXT("��� ������ ����"), TEXT("Warning"), MB_YESNO) == IDYES)
			{
				if (!m_vecPrevCoord.empty())
					m_vecPrevCoord.clear();

				m_vecEditCoordIndex.clear();
				m_vecPrevCoord = m_vecCoord;
				m_vecCoord.clear();
				m_eState = CS_NONE;
				m_vecCollisionCoordIndex.clear();
			}

		}
		ShowCursor(FALSE);

	}





	// ���콺�� ���¿� ����, coord�� ������ �߰��Ѵ�.	

	if (!m_bOffset)
	{
		if (KEYDOWN("MouseLButton"))
		{


			// CoordȰ��ȭ�� ���� ó��. 
			if (!m_vecCollisionCoordIndex.empty())
			{
				// ��Ȳ�� ���� ����. �������� �浹 �������� �����Ѵ�.
				// ���⼭ ��ġ�� ������ ���� ��쿡 �� �� �������� ��� EditIndex�� �ִ� ���� ������
				// Ȥ�� �ϳ��� EditIndex�� �ִ� ���� ������...
				// => �浹�� �������� ��� �ִ� ���� �´� �� �ϴ�.

				// ��� �� 7�� ���� �����ߴ� �� ������ ���� ���ο� ����� �߰��� �ž�.

				// ���� �浹 Ȱ��ȭ ��� !
				// �� ����� Ȱ��ȭ�� �Ǿ� �ִٸ�, ������ ���������� �ѹ��� �������� ������ �� �־�

				// ������ ��Ȱ��ȭ�� ��쿡�� �ѹ��� �ϳ����� ���� �ε����� �߰��� �� �ִ� ����.
				// �ֳĸ� �װ͸� �浹�� �Ǵϱ�.

				// �ƹ�ư �̰��� �浹������ ó���ϴ� �κ��̰� ���� �ε����� �ִ� �Ͱ��� ���� ����� ����.
				// ���� �浹�� ���ؼ� �������� �ѹ��� �߰��ϵ�, �ϳ��� ������ �߰��ϵ� �װ� ����� �����̰�,
				// �ƹ�ư �浹�� Ƚ���� �޶����� �κ��̶�,
				// �װ� �����Ͽ� Edit�� �ִ� ���� �ϳ��� �浹�� �Ǿ �ϳ��� �ִ���
				// Ȥ�� �������� �ѹ��� ó���� �Ǿ �������� �ѹ��� �ִ��Ŀ� ���̾�.
				// �ֳĸ� vector�� ����ִ� size������ ���̰� �ִ°��ݾ�.

				size_t iSize = m_vecCollisionCoordIndex.size();

				// ��Ȱ��ȭ ���¶�� 1�̰���. �׸��� �̰��� �־��ٰž�.
				// Ȱ��ȭ ���¶�� �������� ���� ���ɼ��� �ְ� �̰��� �ѹ��� Edit���� �߰��ϰ���.

				for (size_t i = 0; i < iSize; ++i)
				{
					bool bIsIndex = IsIndex(m_vecCollisionCoordIndex[i]);
					// �̹� EditCoordIndex�� ��ϵ� �ε����� �־��ַ��� ����� ���� �ִ�.

					if (!bIsIndex)
					{
						m_vecEditCoordIndex.push_back(m_vecCollisionCoordIndex[i]);


						// ���� ���¸� �ش�.
						m_eState = CS_CHANGE_DOING;


						if (!m_bOverlapCollision)
						{
							// 2�� �̻� ���� �� �ִ� ���� ���� �浹�� ���Ǵ� �������� �̴�.
							sort(m_vecEditCoordIndex.begin(), m_vecEditCoordIndex.end());
							vector<int>::iterator iter = unique(m_vecEditCoordIndex.begin(), m_vecEditCoordIndex.end());
							m_vecEditCoordIndex.resize(distance(m_vecEditCoordIndex.begin(), iter));

							// �ߺ����� ���Ұ� �߰��Ǵ� ��찡 ����.
							// �� ��츦 �� �𸣰����� �ϴ��� ... ���ܷ� ���ƹ���.
						}

						//if (m_vecEditCoordIndex.size() > m_vecCoord.size())
						//	int i = 0; // ERROR


						// ������ �ϴ� ������ �ε����� ���� ������ ���� ������ ����� ���� �ε����� �ִ� ����(��ĭ�� ���)�� �±� �����̴�.
						// 3 5 7 �̶��,
						// 3 �ε��� ���ְ� 5, 7�� �� �ε����� ���ܼ� 4 6 .. 4���ְ� ���ܼ� 5 . 5 ���ְ� ��.
						// 5 7 3 �̶��, 5���ְ� ���ܼ� 6 2(!) 5�ڿ� �ִ� �ε����� ������ ������, �տ� �ִ� �ͱ��� ������ ������ �ʴ´�.
						// ���� �ε��� - 1 �� ó���� �ϱ� ���ؼ��� ������ �ʿ��ϴ�.
					}

					else
					{
						// �ε����� �־��ָ�, �ش� Indexvec���� �ش� ���� ã�´�.
						// �׸��� �ش� ���� �ִ� ���� �ε����κ��� ��ĭ�� ����� ������ ����.



						EditCoordDelete(m_vecCollisionCoordIndex[i]);
					}

				}

				if (m_vecEditCoordIndex.empty())
				{
					m_eState = CS_NONE;
				}
			}

			else if (m_eState == CS_NONE)
			{
				// begin�� ��ǥ�� ����.
				m_tBegin = m_tMouseSrcPos;
				m_eState = CS_FRAME_DOING;
			}


		}

		else if (KEYPRESS("MouseLButton"))
		{
			if (m_eState == CS_FRAME_DOING)
			{
				m_tEnd = m_tMouseSrcPos;

				//  �������� �����̶�� �ϴ��� �ϴ� �׸��� �׷��ش�.
			}

		}

		else if (KEYUP("MouseLButton"))
		{
			if (m_eState == CS_FRAME_DOING)
			{
				// ��ǥ�� �־��� ��, �ش� ũ�Ⱑ �͹��Ͼ��� �۰ų�, �͹��� ���� ū ��츦 ���´�.
				if (!m_bFrameSize)
				{
					Size tSize = m_tEnd - m_tBegin;
					if (tSize.x >= 10.f && tSize.y >= 10.f)
						m_vecCoord.push_back(ClipCoord(m_tBegin, m_tEnd));

					// 10 * 10 �̻��� ��쿡�� �־��ش�.
				}
				else
				{
					// �����ӻ���� ������Ű�� �������� dest�� ��ġ�� �����ϰ�, �������� ����� �����.
					if (m_bFrameSize)
						m_tEnd = m_tBegin + m_tFrameSize;

					m_vecCoord.push_back(ClipCoord(m_tBegin, m_tEnd));
				}

				m_eState = CS_NONE;
			}

		}

	}


}

void CEditManager::MouseOverCoord()
{
	// ���콺�� �浹�ϰ� �ִ� ��� �������� �ε����� CollisionIndex�� �־��ش�.

	m_vecCollisionCoordIndex.clear();


	size_t iSize = m_vecCoord.size();

	for (size_t i = 0; i < iSize; ++i)
	{
		// ��� �������� ��ȸ�ϸ鼭 �浹ó���� �Ǿ����� Ȯ���Ѵ�.
		// ����ȭ������ �ʿ����, �ܼ��� �浹 or NOT���� �����Ѵ�.

		RectInfo tRect(m_vecCoord[i]);
				
		if (CMath::CollisionRectToPoint(tRect, m_tMouseSrcPos, m_tIntersect))
		{
			// ���콺�� �浹�� �Ͼ ��쿡�� �浹�ε����� �߰��Ѵ�.
			m_vecCollisionCoordIndex.push_back(i);
		
		}
			
	}

}

void CEditManager::MouseOverSoleCoord()
{
	// ���콺�� �浹�ϰ� �ִ� ��� �������� �ε����� CollisionIndex�� �־��ش�.

	m_vecCollisionCoordIndex.clear();


	int iSize = (int)m_vecCoord.size();

	for (int i = iSize - 1; i >= 0; --i)
	{
		// ��� �������� ��ȸ�ϸ鼭 �浹ó���� �Ǿ����� Ȯ���Ѵ�.
		// Z���� ���� �������� ���󿡼� ���� ���� �ִ� �������� ������ �ִ°� ?
		// �׳� �ܼ��� �����ؼ� ���� �ֱٿ� �߰��� �������� ���� ���� ���� ���̹Ƿ� ���� �ֱ��� ������
		// (������� �����ϱ� ���� ���ʿ� �ִ� �ͺ��� ���� �ȴ�)
		// �ϳ��� �浹��Ű�ڴ� ����.

		RectInfo tRect(m_vecCoord[i]);

		if (CMath::CollisionRectToPoint(tRect, m_tMouseSrcPos, m_tIntersect))
		{
			// ���콺�� �浹�� �Ͼ ��쿡�� ���� �ֱ� �߰��� �浹�ε����� �߰��ϸ� return.
			m_vecCollisionCoordIndex.push_back(i);

			return;

		}

	}

	// �浹�� �ε����� ���� ���� �ִ�.
	// �ƹ�ư �浹�̶�� üũ�ϴ� �ε����� (�浹�� �����ӵ� �߿��� ���� �ֱ� ���� �ε����� �ִ� ���̴�.)

	
}

void CEditManager::StaticLineRender(HDC hDC, float fTime)
{
	Position tCameraPos = GET_SINGLE(CSceneManager)->GetCameraPos();

	RECT tRC;

	switch (m_eStatic)
	{
	case SS_X:
		tRC = { 30, 290, 80, 390 };
		break;

	case SS_Y:
		tRC = { 30, 290, 130, 340 };

	}

	FrameRect(hDC, &tRC, YELLOWBRUSH);
	
}

void CEditManager::Collision(float fTime)
{

	if (m_bOffset)
		return;

	if (m_bOverlapCollision)
	{
		// �� �����Ӹ���, �浹�� �� �������� �˻��Ͽ�, CollisionIndex�� �־��ش�.
		MouseOverCoord();
	}
	else
	{
		// ���콺�� �浹�ϴ� �������� �ϳ��� �����ϵ� �� �ϳ��� �������� ���� ���� ���������� �Ѵ�.
		MouseOverSoleCoord();
	}
}

int CEditManager::GetClipNameIndex(bool bScale)
{
	if (m_vecClipName.empty())
	{
		MessageBox(nullptr, TEXT("Not Found String"), TEXT("Error"), MB_OK);

		return -1;
	}

	if(bScale)
	{
		++m_iCurClipIndex;

		if (m_iCurClipIndex >= m_vecClipName.size())
			m_iCurClipIndex = 0;
	}

	else
	{
		--m_iCurClipIndex;

		if ((int)m_iCurClipIndex <= -1)
			m_iCurClipIndex = m_vecClipName.size() - 1;
	}


	return m_iCurClipIndex;

}


bool CEditManager::IsIndex(int iIndex)
{

	size_t iSize = m_vecEditCoordIndex.size();

	for (size_t i = 0; i < iSize; ++i)
	{
		if (m_vecEditCoordIndex[i] == iIndex)
			return true;
	}

	return false;
}

void CEditManager::MoveEditCoord(float fScale, bool bHorizon)
{
	// XY Static�� �׳�. CtrlV�� �����ϱ� ���� ������ �д�. 
	// ������ Ǯ���ֱ� ����.

	//// X �� �̵��ε�, X���� ���� ���
	//// Y�� �̵��ε�, Y���� ���� ���
	//if (bHorizon && m_eStatic == SS_X ||
	//	!bHorizon && m_eStatic == SS_Y)
	//	return;

	//// ������ ������ ����ε�, ���� ������ �����ִ� ��쿡�� ��� �����ӿ� ���ؼ� ó���Ѵ�.
	//if (m_bFrameAdj)
	//	CtrlA();

	size_t iSize = m_vecEditCoordIndex.size();

	for (size_t i = 0; i < iSize; ++i)
	{
		// ������ �������� �ε����� �����´�.
		int iIndex = m_vecEditCoordIndex[i];

		// �ش� �������� ������ �����Ͽ� ��ġ�� �����Ѵ�.
		if (bHorizon)
		{
			m_vecCoord[iIndex].tStart.x += fScale;
			m_vecCoord[iIndex].tEnd.x += fScale;
		}
		else
		{
			m_vecCoord[iIndex].tStart.y += fScale;
			m_vecCoord[iIndex].tEnd.y += fScale;
		}

	}

}

void CEditManager::MoveEditCoord(const Position & tMouseMove)
{
	size_t iSize = m_vecEditCoordIndex.size();

	for (size_t i = 0; i < iSize; ++i)
	{
		// ������ �������� �ε����� �����´�.
		int iIndex = m_vecEditCoordIndex[i];

		// �ش� �������� ������ �����Ͽ� ��ġ�� �����Ѵ�.
		m_vecCoord[iIndex].tStart += tMouseMove;
		m_vecCoord[iIndex].tEnd += tMouseMove;

	}
}

void CEditManager::ResizeEditCoord(float fScale, bool bHorizon)
{
	// ������ ����� ������ ��쿡�� ũ�⸦ �ٲ� �� ����.
	if (m_bFrameSize)
		return;

	// ������ Size�� ������ �� ��쿡�� �������� �ʴ´�.
	// XY Static�� �׳�. CtrlV�� �����ϱ� ���� ������ �д�. 
	// ������ Ǯ���ֱ� ����.

	/*if (bHorizon && m_eStatic == SS_X ||
		!bHorizon && m_eStatic == SS_Y)
		return;

	if (m_bFrameAdj)
		CtrlA();*/

	size_t iSize = m_vecEditCoordIndex.size();

	for (size_t i = 0; i < iSize; ++i)
	{
		// ������ �������� �ε����� �����´�.
		int iIndex = m_vecEditCoordIndex[i];

		// �ش� �������� ������ �����Ͽ� ũ�⸦ �����Ѵ�.
		if (bHorizon)
		{
			m_vecCoord[iIndex].tEnd.x += fScale;
		}
		else
		{
			m_vecCoord[iIndex].tEnd.y += fScale;
		}

	}
}

void CEditManager::ResizeEditCoord(const Position & tMouseMove)
{
	if (m_bFrameSize)
		return;

	/*if (m_eStatic == SS_X ||
		m_eStatic == SS_Y)
		return;*/

	size_t iSize = m_vecEditCoordIndex.size();

	for (size_t i = 0; i < iSize; ++i)
	{
		// ������ �������� �ε����� �����´�.
		int iIndex = m_vecEditCoordIndex[i];

		m_vecCoord[iIndex].tEnd += tMouseMove;

	}
}

void CEditManager::Save(const char* strFrontTag)
{
	ShowCursor(TRUE);

	if (MessageBox(nullptr, TEXT("���� �����Ͻðڽ��ϱ� ?"), TEXT("Save Clip"), MB_YESNO) == IDYES)
	{
		// ���� ó���ϰ� �ִ� Ŭ���� ������ �����´�.
		PAnimationClip pClip = m_pAnimation->m_pClip;

		// �����̸��� ���� �ð������� �����ϱ� ���ؼ�, LocalTime�� �о�´�.
		

		// �����̸��� �����.
		char strName[100] = {};

		// FrontTag �� �����Ѵ�.
		strcpy_s(strName, sizeof(char) * 100, strFrontTag);

		// ���� Ŭ���� �̸��� ���δ�.
		strcat_s(strName, sizeof(char) * 100, pClip->strName.c_str());
		// Kirby/KirbyIdleRight

	/*
		SYSTEMTIME st;
		GetLocalTime(&st);	char* strLocal = new char[32];
		memset(strLocal, 0, sizeof(char) * 32);

		sprintf_s(strLocal, sizeof(char) * 32, " %d.%d.%d %dh %dm.clip", st.wYear,
			st.wMonth, st.wDay, st.wHour, st.wMinute);

		strcat_s(strName, sizeof(char) * 100, strLocal);*/
		// �����ϴ� ��쿡�� "�����̸�/Ŭ���̸� ��.��-�ð�-��.clip" �� ���·� �����Ѵ�.

		strcat_s(strName, sizeof(char) * 100, ".clip"); // �ϴ��� �׳� Ŭ������ ����.

		// �ش� �̸��� ���·� ���� Ŭ�������� �����Ѵ�.
		// ���̺� �ɼ��� ����ϴ� ��쿡�� �װ����� ����. ������� �ʴ� ��쿡�� ����� �ɼ��� ����Ͽ� �����Ѵ�.
		if (!m_pAnimation->SaveClip(strName, m_bSaveOption, m_eSaveOption))
		{
			MessageBox(nullptr, TEXT("�����ϱ� ����"), TEXT("SAVE ERROR"), MB_OK);
		}

		//delete[] strLocal;

	}

	ShowCursor(FALSE);

}

void CEditManager::SaveAll(const char * strFrontTag)
{
	ShowCursor(TRUE);

	if (MessageBox(nullptr, TEXT("���� ��� �����Ͻðڽ��ϱ� ?"), TEXT("Save All Clip"), MB_YESNO) == IDYES)
	{
		unordered_map<string, PAnimationClip>::iterator iter;
		unordered_map<string, PAnimationClip>::iterator iterEnd = m_pAnimation->m_mapClip.end();

		for (iter = m_pAnimation->m_mapClip.begin(); iter != iterEnd; ++iter)
		{
			// Ŭ���� ������ �����´�.
			PAnimationClip pClip = iter->second;

			char strName[100] = {};

			// FrontTag �� �����Ѵ�.
			strcpy_s(strName, sizeof(char) * 100, strFrontTag);

			// ���� Ŭ���� �̸��� ���δ�.
			strcat_s(strName, sizeof(char) * 100, pClip->strName.c_str());
			// Kirby/KirbyIdleRight.clip

			strcat_s(strName, sizeof(char) * 100, ".clip");

			// �ش� �̸��� ���·� ���� Ŭ�������� �����Ѵ�.

			if (!m_pAnimation->SaveClip(strName, false))
			{
				MessageBox(nullptr, TEXT("�����ϱ� ����"), TEXT("SAVE ERROR"), MB_OK);
			}
		}
	}

	ShowCursor(FALSE);

}

void CEditManager::Load(const char* strFrontTag)
{

	ShowCursor(TRUE);

	if (MessageBox(nullptr, TEXT("���� �ҷ����ðڽ��ϱ� ?"), TEXT("Load Clip"), MB_YESNO) == IDYES)
	{
		PAnimationClip pClip = m_pAnimation->m_pClip;

		// �о�� ������ �̸��� �����.
		// �о���� ��쿡�� "�����̸�/Ŭ���̸�.clip" ���� �о�´�.

		char strName[100] = {};

		// ���� Ŭ���� �̸��� �����Ѵ�.
		strcpy_s(strName, sizeof(char) * 100, strFrontTag);
		strcat_s(strName, sizeof(char) * 100, pClip->strName.c_str());

		strcat_s(strName, sizeof(char) * 100, ".clip");

		if (m_pAnimation->LoadClip(strName))
		{

			// ���� Ŭ���� ������ �ٽ� ������ �Ǿ��� ���̴�. �������� ���� ������� �ʱⰪ ����.

			PAnimationClip pClip = m_pAnimation->GetClip();

			m_iPrevFrameNumber = pClip->vecCoord.size();


			m_eAniOption = pClip->eOption;

			if (m_eAniOption != AO_ONCE_LOOP)
			{
				// ������忡���� �׻� LOOP
				pClip->eOption = AO_LOOP;
			}

			

			// ������忡 �ִ� ������ ����������, ���� �о�� ���� �����Ѵ�.

			m_vecCoord = pClip->vecCoord;
			m_eState = CS_NONE;
			m_vecCollisionCoordIndex.clear();
			m_vecEditCoordIndex.clear();
			m_vecPrevCoord.clear();
			m_eChange = CCS_NONE;
			m_tOffset = m_pCutClip->tOffset * -1.f;


			if (!m_vecCoord.empty())
				m_pEditObject->SetSize(m_vecCoord[0].tEnd - m_vecCoord[0].tStart);
		}
		else
		{
			MessageBox(nullptr, TEXT("�ҷ����� ����"), TEXT("LOAD ERROR"), MB_OK);
		}

	}

	ShowCursor(FALSE);

}

void CEditManager::AnimationChange(bool bScale)
{
	// ������ Ŭ������ loop�� �����ص� �ɼ��� �����Ѵ�.
	PAnimationClip pClip = m_pAnimation->GetClip();
	pClip->eOption = m_eAniOption;

	// Ŭ���̸��� �ƹ��͵� ���� ���� ��� return.
	if (m_vecClipName.empty())
		return;

	m_pAnimation->ChangeClip(m_vecClipName[GetClipNameIndex(bScale)]);

	// ���� ������Ʈ ���̿����� �ؽ�ó�� �ٸ� ���ɼ��� �����Ƿ� Bmp����� �ٽù޾Ƶд�.
	m_tBmpSize = m_pAnimation->GetBmpSize();


	// ��Ʈ�� ����� ȭ�� ũ�⸦ �Ѿ�� ��쿡�� �װ�����, �ƴ� ��쿡�� ȭ��ũ��� ����.
	if (m_tBmpSize.x >= RS_WIDTH && m_tBmpSize.y >= RS_HEIGHT)
		GET_SINGLE(CSceneManager)->SetLimitCamera(m_tBmpSize);
	else
		GET_SINGLE(CSceneManager)->SetLimitCamera(Size(RS_WIDTH, RS_HEIGHT));

	// Ŭ���� �ٲٰ� ���ο� Ŭ���� �޾ƿ´�.
	pClip = m_pAnimation->GetClip();
	m_vecCoord = pClip->vecCoord;
	m_tOffset = pClip->tOffset * -1.f;

	
	// ���ο� Ŭ���� ���� �ɼ��� �޾Ƶд�.
	m_eAniOption = pClip->eOption;
	m_iPrevFrameNumber = m_vecCoord.size();
	if (pClip->eOption != AO_ONCE_LOOP)
		pClip->eOption = AO_LOOP;
	m_vecEditCoordIndex.clear();
	m_vecCollisionCoordIndex.clear();

	// ������ ����� ���缭 ����� �ؾ��ϹǷ�, Object�� Size�� �ٲ��ش�.

	if(!m_vecCoord.empty())
		m_pEditObject->SetSize(m_vecCoord[0].tEnd - m_vecCoord[0].tStart);
}

void CEditManager::CtrlV()
{
	// �⺻����. ������ ����� �������ѵξ���.
	if (!m_bFrameSize)
		return;

	// 1���� EditFrame�� ���� ��쿡�� CtrlV�� ������.
	// ���Ⱑ �ſ� �߿��� �κ�.

	// �ϴ�, ��� ���. ���콺 ��ǥ�� ������������ �浹�ߴ��� Ȯ���Ѵ�.

	int iIndex = m_vecEditCoordIndex[0];

	ClipCoord tCoord = m_vecCoord[iIndex];

	bool bCollision = IsCollision(iIndex);

	if (bCollision || m_eStatic == SS_NONE)
	{
		// ����, �浹�̰ų� NONE ���¶�� ���� �����ӿ� �ش��ϴ� �������� �ϳ� �߰��Ѵ�.
		// ������ ���� �������� �����ְ�, ���� ������ �������� ���� �����ӿ� �ִ´�.

		m_vecEditCoordIndex.clear();

		m_vecEditCoordIndex.push_back(m_vecCoord.size());

		m_vecCoord.push_back(tCoord);
		
		return;
	}

	// ������ none �����̰ų�, �浹�� �Ͼ ��쿡 ���ؼ� ó���ߴ�.
	// �Ʒ��� ���� ���콺�� ���� ��ǥ�� ���� ó���� �Ͼ�� �Ѵ�.
	// ���� vecCoord�� �ִ� ������ ��� ���� ��ǥ�̴�. ����, ���콺�� ���� ��ǥ�� �����´�.  m_tMouseSrcPos


		// ����.  y���� ������ �Ǿ� �ִ� ���·� x�� ������ �������״�.
		// �̷� ��Ȳ���� �� �������� �������� �Ͽ� CtrlVŰ�� ���� ��� ���콺�� ��ġ ����(�¿����) �߿���
		// y�� �����̹Ƿ� �� �浹 ��, �̷��� 3������ �ִµ� �浹�̸� �� �ڸ� �״�� ����
		// ���� ��쿡�� �����ϴ� �������� begin��ǥ�� �������� Adj.x + fFrameX��ŭ ����, ���ο� �������� begin���� ��´�
		// ���Ŀ� Frame���� �̿��Ͽ� end�� ���� ���̴�.

		// �����ʿ� �ִ� ��쿡�� fFrameX + Adj.x ��ŭ ���ؼ� �װ��� begin���� ��Ƽ� ó���Ѵ�.
		// ���� ���� Y�� ������ ���ؼ��� �����ϰ� �ȴ�.

	// ���� �������� ��ǥ�κ��� ���콺�� ��ġ�� ���س���.

	ClipCoord tNewCoord;

	switch (m_eStatic)
	{
	case SS_X:
		// X��ǥ�� ������ ��쿡�� ������ �Ʒ������� �Ǵ��Ѵ�.
		if (tCoord.tEnd.y < m_tMouseSrcPos.y)
		{
			// �Ʒ��ʿ� �ִ� ��� �Ʒ��� ���ο� �������� �߰��Ѵ�.
			// ���� ������ �ִٸ�, Y������ ����ϰ� �ȴ�.

			Position tSrcPos = tCoord.tStart;
		
			// src��ġ���� ������ y��ŭ�� ũ�⸦ ���Ѵ�.
			tSrcPos.y += m_tFrameSize.y;

			// ������ �ִٸ� �߰��Ѵ�.
			if (m_bFrameAdj)
				tSrcPos.y += m_tFrameAdj.y;

			// �̷��� ���� ���� begin���� �Ͽ� end�� ��� coord�� �־��ش�.
			tNewCoord = ClipCoord(tSrcPos, tSrcPos + m_tFrameSize);
		}
		else
		{
			// ������ ��쿡�� y������ ����, FrameY������ ���ش�.
			Position tSrcPos = tCoord.tStart;

			tSrcPos.y -= m_tFrameSize.y;

			if (m_bFrameAdj)
				tSrcPos.y -= m_tFrameAdj.y;

			tNewCoord = ClipCoord(tSrcPos, tSrcPos + m_tFrameSize);

		}
		break;

	case SS_Y:
		// Y��ǥ�� ������ ��쿡�� �����ʰ� ������ �Ǵ��Ѵ�.
		if (tCoord.tEnd.x < m_tMouseSrcPos.x)
		{
			// ������
			Position tSrcPos = tCoord.tStart;

			tSrcPos.x += m_tFrameSize.x;

			if (m_bFrameAdj)
				tSrcPos.x += m_tFrameAdj.x;

			tNewCoord = ClipCoord(tSrcPos, tSrcPos + m_tFrameSize);
		}
		else
		{
			// ����
			Position tSrcPos = tCoord.tStart;

			tSrcPos.x -= m_tFrameSize.x;

			if (m_bFrameAdj)
				tSrcPos.x -= m_tFrameAdj.x;

			tNewCoord = ClipCoord(tSrcPos, tSrcPos + m_tFrameSize);

		}
		break;

	}

	m_vecEditCoordIndex.clear();

	m_vecEditCoordIndex.push_back(m_vecCoord.size());

	m_vecCoord.push_back(tNewCoord);

}

void CEditManager::CtrlA()
{
	m_vecEditCoordIndex.clear();

	size_t iSize = m_vecCoord.size();

	for (unsigned int i = 0; i < iSize; ++i)
	{
		m_vecEditCoordIndex.push_back(i);
	}
	// ��� �ε����� �߰��Ѵ�.(��ü ����)
}

void CEditManager::BasicInput(float fTime)
{

	//  Frame Adj Enable/DisEnable
	if (KEYDOWN("0"))
	{
		m_bFrameAdj = !m_bFrameAdj;
	}

	// Frame Adj X

	if (m_bFrameAdj && m_eStatic != SS_NONE)
	{
		// ������ ������ Ȱ��ȭ�Ǿ� �ִ� ��쿡�� ó��.
		// ������ �Ǿ����� ���� ��쿡�� ����Ȥ�� ���η� ������ �ִ� ��찡 �ƴ� ���̴�.
		// ���� ������ ����ִ� ���ۿ� ���� ��. ( ���� ���� �ʿ䰡 ���� )

		switch (m_eStatic)
		{
		case SS_X:
			// StaticX ���¸�, Y����
			if (KEYDOWN("++FrameAdjY"))
			{
				m_tFrameAdj.y += m_fMoveScale;

			}
			else if (KEYDOWN("--FrameAdjY"))
			{
				m_tFrameAdj.y -= m_fMoveScale;

			}
			break;

		case SS_Y:
			// StaticY ���¸�, X����
			if (KEYDOWN("++FrameAdjX"))
			{
				m_tFrameAdj.x += m_fMoveScale;


			}
			else if (KEYDOWN("--FrameAdjX"))
			{
				m_tFrameAdj.x -= m_fMoveScale;

			}
			break;
		}
		
	

	}
	
	// SaveOption Change 
	if (KEYDOWN("F4"))
	{
		OptionChange();
	}


	// reset
	if (KEYDOWN("F8"))
	{
		PAnimationClip pClip = m_pAnimation->GetClip();

		pClip->fTime = 0.f;
		pClip->iFrame = 0;
	}

	// Save
	if (KEYDOWN("CtrlS"))
	{

		string s = "../Bin/Save/New/";
		/*SaveAll(s.c_str());*/ // ��� Ŭ���� ������ �����Ѵ�.
		Save(s.c_str());
	}
	else if (KEYDOWN("CtrlL"))
	{
		string s = "../Bin/Save/"; // ������ Tag�ε�, ����� ���� �α� ������ �������� �ʴ´�.
		// �ƹ�ư �׷��� Tag�� �ٿ�����. OK 
		s += m_pEditObject->GetTag();
		Load(s.c_str());
	}

	// CoordMoveSpeed (Key board)
	if (KEYDOWN("++MoveSpeed"))
	{
		m_fMoveScale += 1.f;

		if (m_fMoveScale > 20.f)
			m_fMoveScale = 20.f;
	}
	else if (KEYDOWN("--MoveSpeed"))
	{
		m_fMoveScale -= 1.f;

		if (m_fMoveScale <= 0.f)
			m_fMoveScale = 1.f;
	}

	// ��ü ����(�����ε����� ����)
	if (KEYDOWN("CtrlA") && !m_bOffset)
	{
		CtrlA();
	}

	// Animation Speed
	if (KEYDOWN("++FrameTime"))
	{
		m_pAnimation->AddFrameTime(0.03f);
	}

	else if (KEYDOWN("--FrameTime"))
	{
		m_pAnimation->AddFrameTime(-0.03f);
	}

	// Animtion Change
	if (KEYDOWN("++Animation")) // f5
		AnimationChange(true);


	else if (KEYDOWN("--Animation")) // f6
		AnimationChange(false);


	// EditCoord Clear
	if (KEYDOWN("F"))
	{
		m_eState = CS_NONE;
		m_vecEditCoordIndex.clear();
	}


	// �۾� ��� ����
	// E, R�� ���� ���Ŀ� ��Ŭ���� ���ؼ� �۾��� �� �� �ֵ��� �Ѵ�.

	if (!m_bOffset)
	{
		// �������� �����ִ� ��� ��ü �Ұ�.
		if (KEYUP("E"))
		{
			m_eChange = CCS_MOVE;
		}
		else if (KEYUP("R"))
		{
			m_eChange = CCS_RESIZE;
		}

	}

	// Ctrl Z  : Lately Coord Revive
	if (KEYDOWN("CtrlZ") && !m_bOffset)
	{
		if (!m_vecPrevCoord.empty())
		{

			size_t iSize = m_vecPrevCoord.size();

			for (size_t i = 0; i < iSize; ++i)
			{
				m_vecCoord.push_back(m_vecPrevCoord[i]);
			}

			m_vecPrevCoord.clear();

			m_eState = CS_NONE;
		}
	}


	// BackSpace Key
	if (KEYDOWN("CoordDelete"))
	{
		if (!m_vecEditCoordIndex.empty())
		{
			if (!m_vecPrevCoord.empty())
				m_vecPrevCoord.clear();

			// �迭�� �� �ִ� �ε����� �ϳ��� �����ش�.

			for (size_t i = 0; i < m_vecEditCoordIndex.size(); ++i)
			{
				CoordDelete(m_vecEditCoordIndex[i]);

				// �ѹ� ó���� �ϰ� �� ���Ŀ��� �ϳ��� ������ ���ָ鼭, �ε����� �ϳ��� �������µ�,
				// ���⿡�� size������ �ɸ���, �߸��� �ε����� �Ǵ��ϰ� ������ �ʴ� ó���� �ȴ�.

				for (size_t j = 0; j < m_vecEditCoordIndex.size(); ++j)
					--m_vecEditCoordIndex[j];
			}

			m_vecEditCoordIndex.clear();

			m_eState = CS_NONE;

		}

	}

	if (KEYDOWN("5"))
	{
		// Offset Copy

		m_tSaveOffset = m_tOffset;

		// �ܼ� ����.
	}

	else if (KEYDOWN("6"))
	{
		// Paste
		m_tOffset = m_tSaveOffset;

		m_pAnimation->GetClip()->tOffset = m_tSaveOffset * -1.f;
	}

	// Offset Color
	if (KEYDOWN("7"))
	{
		int iBrushType = m_eBrushType;

		++iBrushType;

		if (iBrushType >= BT_END)
			iBrushType = 0;

		m_eBrushType = (BRUSH_TYPE)iBrushType;
	}

	if (KEYDOWN("8"))
	{

		m_bOffset = !m_bOffset;

		if (m_bOffset)
		{
			m_eChange = CCS_NONE;
			m_eState = CS_NONE;
			m_vecEditCoordIndex.clear();
		}
		// Offset �������
		// ������ ó���� �ִٸ�, Ǯ�������.


	}

	// Static State
	if (KEYDOWN("9"))
	{
		StaticStateChange();
	}

	if (KEYDOWN("3"))
	{
		// ���� ���������� �ѱ��.
		PAnimationClip pClip = m_pAnimation->GetClip();
		++pClip->iFrame;

		if (pClip->iFrame == pClip->vecCoord.size())
			pClip->iFrame = 0;
	}

	else if (KEYDOWN("4"))
	{
		// ���� ���������� ���ư���.
		PAnimationClip pClip = m_pAnimation->GetClip();
		--pClip->iFrame;

		if (pClip->iFrame == -1)
			pClip->iFrame = pClip->vecCoord.size() - 1;
	}


	if (KEYDOWN("ConversionMode")) // Y
	{
		m_bCut = !m_bCut;

		if (!m_bCut)
		{
			m_pCutClip = nullptr;
			m_strCut = "";
		}
	}



	if (KEYDOWN("ClipCut") && m_bCut) // U
	{
		ClipCut();
	}

	if (KEYDOWN("ClipCopy") && m_bCut) // I
	{
		ClipConversion();
	}

	if (KEYDOWN("ClipCopy2") && m_bCut) // O
	{
		ClipConversion(false);
	}

	// ObjectChange
	if (KEYDOWN("ObjectChange"))
	{
		ObjectChange();
	}


	// KeyInput  ���� �۾��ϴٰ� p������  pause���°� ���� �ʵ��� ���ش�.
	if (KEYDOWN("Pause"))
	{
		GET_SINGLE(CGameManager)->Pause();
	}
}

bool CEditManager::IsCollision(int iIndex)
{
	size_t iSize = m_vecCollisionCoordIndex.size();

	for (size_t i = 0; i < iSize; ++i)
	{
		if (m_vecCollisionCoordIndex[i] == iIndex)
			return true;
	}

	return false;
}

void CEditManager::TextRender(HDC hDC, float fTime)
{
	// ���콺�� src������ ��ġ�� ����Ѵ�.

	TCHAR strPos[40] = {};

	wsprintf(strPos, TEXT("Mouse : (%d, %d)"), (int)m_tMouseSrcPos.x, (int)m_tMouseSrcPos.y);

	TextOut(hDC, 20, 20, strPos, lstrlen(strPos));

	// �ߺ� �浹 Ȱ��ȭ ���� ���
	if (m_bOverlapCollision)
	{
		TextOut(hDC, 20, 40, TEXT("OverlapCollision"), lstrlen(TEXT("OverlapCollision")));
	}

	if (m_bFrameSize)
	{
		// FrameSize ������ �Ǿ� �ִٸ� ���.
		wsprintf(strPos, TEXT("Frame : (%d, %d)"), (int)m_tFrameSize.x, (int)m_tFrameSize.y);

		TextOut(hDC, 20, 60, strPos, lstrlen(strPos));
	}

	switch (m_eChange)
	{
	case CCS_MOVE:
		TextOut(hDC, 20, 80, TEXT("MOVE"), lstrlen(TEXT("MOVE")));

		break;

	case CCS_RESIZE:
		TextOut(hDC, 20, 80, TEXT("RESIZE"), lstrlen(TEXT("RESIZE")));

		break;
	}


	if (m_bSaveOption)
	{
		switch (m_eSaveOption)
		{
		case AO_LOOP:
			TextOut(hDC, 20, 100, TEXT("Save : Loop"), lstrlen(TEXT("Save : Loop")));

			break;

		case AO_ONCE_DESTROY:
			TextOut(hDC, 20, 100, TEXT("Save : Once Destroy"), lstrlen(TEXT("Save : Once Destroy")));

			break;

		case AO_ONCE_RETURN:
			TextOut(hDC, 20, 100, TEXT("Save : Once Return"), lstrlen(TEXT("Save : Once Return")));

			break;

		case AO_ONCE_LOOP:
			TextOut(hDC, 20, 100, TEXT("Save : Once Loop"), lstrlen(TEXT("Save : Once Loop")));

			break;

		case AO_TIME_DESTROY:
			TextOut(hDC, 20, 100, TEXT("Save : Time Destroy"), lstrlen(TEXT("Save : Time Destroy")));

			break;
		}
	}
	else
	{
		// Save Option �� ���ٸ�
		TextOut(hDC, 20, 100, TEXT("Save : Saved"), lstrlen(TEXT("Save : Saved")));

	}


	// Once Loop Frame�� ������ش�. Once Loop�� �ɼ��� ��쿡�� �ǹ̰� �ִ�.
	wsprintf(strPos, TEXT("Once Loop Frame : %d"), m_pAnimation->GetClip()->iOnceLoopFrame);
	TextOut(hDC, 20, 120, strPos, lstrlen(strPos));



	switch (m_eStatic)
	{
	case SS_X:
		TextOut(hDC, 20, 140, TEXT("StaticX"), lstrlen(TEXT("StaticX")));

		break;

	case SS_Y:
		TextOut(hDC, 20, 140, TEXT("StaticY"), lstrlen(TEXT("StaticY")));

		break;
	}

	if (m_bFrameAdj)
	{
		wsprintf(strPos, TEXT("FrameAdj: (%d, %d)"), (int)m_tFrameAdj.x, (int)m_tFrameAdj.y);
		TextOut(hDC, 20, 160, strPos, lstrlen(strPos));
	}


	switch (m_eAniOption)
	{
	case AO_LOOP:
		TextOut(hDC, 20, 180, TEXT("Saved : Loop"), lstrlen(TEXT("Saved : Loop")));

		break;

	case AO_ONCE_DESTROY:
		TextOut(hDC, 20, 180, TEXT("Saved : Once Destroy"), lstrlen(TEXT("Saved : Once Destroy")));

		break;

	case AO_ONCE_RETURN:
		TextOut(hDC, 20, 180, TEXT("Saved : Once Return"), lstrlen(TEXT("Saved : Once Return")));

		break;

	case AO_ONCE_LOOP:
		TextOut(hDC, 20, 180, TEXT("Saved : Once Loop"), lstrlen(TEXT("Saved : Once Loop")));

		break;

	case AO_TIME_DESTROY:
		TextOut(hDC, 20, 180, TEXT("Saved : Time Destroy"), lstrlen(TEXT("Saved : Time Destroy")));

		break;
	}

	if (m_bCut)
	{
		TextOut(hDC, 20, 200, TEXT("Conversion Mode"), lstrlen(TEXT("Conversion Mode")));

		if (m_pCutClip)
		{
			string s = "Cut : " + m_pCutClip->strName;


			int len;
			int slength = (int)s.length() + 1;
			len = MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, 0, 0);
			wchar_t* buf = new wchar_t[len];
			MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, buf, len);
			wstring r(buf);
			delete[] buf;

			// �ڸ� �ִϸ��̼��� �̸��� �����ش�.
			TextOut(hDC, 20, 220, r.c_str(), r.size());

		}

	}

	if (m_bOffset)
	{
		TextOut(hDC, 20, 240, TEXT("Offset Mode On"), lstrlen(TEXT("Offset Mode On")));
	}
	
	wsprintf(strPos, TEXT("Offset : (%d, %d)"), (int)m_tOffset.x, (int)m_tOffset.y);
	TextOut(hDC, 20, 260, strPos, lstrlen(strPos));

	wsprintf(strPos, TEXT("Save Offset : (%d, %d)"), (int)m_tSaveOffset.x, (int)m_tSaveOffset.y);
	TextOut(hDC, 20, 380, strPos, lstrlen(strPos));
}


void CEditManager::ObjectChange()
{
	// ������Ʈ�� �ٲٱ� ���� ���� ������ 
	PAnimationClip pClip = m_pAnimation->GetClip();
	pClip->eOption = m_eAniOption;



	// ���� ���õ� ������Ʈ �����ϰ�, ���ο� ������Ʈ�� ��Ƽ� �����ϴ� ����̴�.

	SAFE_RELEASE(m_pEditObject);

	m_pEditObject = GET_SINGLE(CSceneManager)->FindNextObject(m_strCurTag);

	m_strCurTag = m_pEditObject->GetTag();

	m_pAnimation = m_pEditObject->GetAnimation();

	pClip = m_pAnimation->GetClip();
	

	// ��Ʈ���� ����� �о�´�. ���⼭ ������Ʈ�� ��Ʋ�� �̹����� �Բ� ����ϰ� ���� ���̹Ƿ�
	// �ѹ��� ��Ʈ���� ����� �о���� �ȴ�.
	m_tBmpSize = m_pAnimation->GetBmpSize();

	GET_SINGLE(CSceneManager)->LimitCameraDisEnable();

	if (m_tBmpSize.x >= RS_WIDTH && m_tBmpSize.y >= RS_HEIGHT)
		GET_SINGLE(CSceneManager)->SetLimitCamera(m_tBmpSize);

	// ���� ����� Ŭ���� ���� ������ �Ǿ� ���� ���ɼ��� �ִ�.
	m_vecCoord = m_pEditObject->GetvecCoord();

	// ������ �翬�� �޾ƿ´�.
	m_tOffset = pClip->tOffset * -1.f;


	m_iPrevFrameNumber = m_vecCoord.size();

	// LOOP�� �����ϱ� ���� �ִϸ��̼� �ɼ��� �޾Ƶд� (����� �ɼǰ�)
	m_eAniOption = pClip->eOption;

	// ������ ���ϴ� �ɼ��� ���߿� �ְ� �ϴ�, ������忡���� loopó���� �ؾ��Ѵ�.
	if (pClip->eOption != AO_ONCE_LOOP)
		pClip->eOption = AO_LOOP;


	m_vecEditCoordIndex.clear();
	m_vecCollisionCoordIndex.clear();


	if (!m_vecCoord.empty())
		m_pEditObject->SetSize(m_vecCoord[0].tEnd - m_vecCoord[0].tStart);
	
	// ClipName�� �޾ƿ´�.
	m_vecClipName.clear();

	m_pEditObject->AddClipName(m_vecClipName);
}
