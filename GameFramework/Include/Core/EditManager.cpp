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
	//Save("Auto/"); // 메세지 박스는 넘기고, 그냥 자동으로 마지막 클립을 저장하는 기능이다. 뒤에 auto태그가 붙음

	SAFE_RELEASE(m_pEditObject);
	SAFE_RELEASE(m_pMouseCollider);
}

bool CEditManager::Init()
{
	

	// 편집할 대상을 읽어온다. 
	m_pEditObject = GET_SINGLE(CSceneManager)->FindNextSceneObject("Kirby");

	m_strCurTag = m_pEditObject->GetTag();

	m_pAnimation = m_pEditObject->GetAnimation();

	PAnimationClip pClip = m_pAnimation->GetClip();

	// 오프셋을 읽어온다.
	m_tOffset = pClip->tOffset * -1.f;

	// 비트맵의 사이즈를 읽어온다. 여기서 오브젝트는 아틀라스 이미지를 함께 사용하고 있을 것이므로
	// 한번만 비트맵의 사이즈를 읽어오면 된다.
	m_tBmpSize = m_pAnimation->GetBmpSize();

	GET_SINGLE(CSceneManager)->LimitCameraDisEnable();

	if (m_tBmpSize.x >= RS_WIDTH && m_tBmpSize.y >= RS_HEIGHT)
		GET_SINGLE(CSceneManager)->SetLimitCamera(m_tBmpSize);
	
	// 현재 대상의 클립에 뭔가 저장이 되어 있을 가능성이 있다.
	m_vecCoord = m_pEditObject->GetvecCoord();


	m_iPrevFrameNumber = m_vecCoord.size();

	// LOOP로 변경하기 전의 애니메이션 옵션을 받아둔다 (저장된 옵션값)
	m_eAniOption = pClip->eOption;

	// 저장을 원하는 옵션은 나중에 주고 일단, 편집모드에서는 loop처리를 해야한다.
	if (pClip->eOption != AO_ONCE_LOOP)
		pClip->eOption = AO_LOOP;

	// ClipName을 받아온다.
	m_vecClipName.clear();

	m_pEditObject->AddClipName(m_vecClipName);

	m_vecCoord.reserve(30);
	m_vecEditCoordIndex.reserve(30);
	m_vecPrevCoord.reserve(30);


	// 편집모드에서 사용할 키를 추가한다.
	AddInput();

	m_pMouseCollider = GET_SINGLE(CInput)->GetMouseCollider();
	m_pMouseCollider->AddRef();

	return true;
}

void CEditManager::Update(float fTime)
{

	// Edit 입력처리 : 정지하고 있더라고 하더라도, 카메라는 움직여야 한다.
	Input(fTime);


	if (GET_SINGLE(CGameManager)->GetPause())
		fTime = 0.f;

	// 마우스 정보
	MouseUpdate(fTime);
	
	// 추가한 프레임 정보가 없다면, 애니메이션을 돌리지 않는다.
	if (!m_vecCoord.empty())
		EditObjectUpdate(fTime);




	// Frame의 개수가 변했는가 ? 
	size_t iSize = m_vecCoord.size();

	if (m_iPrevFrameNumber != iSize)
	{
		// 바뀐 경우에는 현재 애니메이션 클립의 정보를 수정해야 한다.
		PAnimationClip pClip = m_pAnimation->GetClip();


		if (!GET_SINGLE(CGameManager)->GetPause())
		{
			// 멈춰있지 않은 경우라면 안전하게 0프레임부터 시작.
			// 멈춰있는 경우라면 그냥 둔다.
			pClip->iFrame = 0;
			pClip->fTime = 0.f;
		}
		m_iPrevFrameNumber = m_vecCoord.size();

		// 프레임의 개수가 달라졌다는 소리는, 전체 플레이시간이 달라졌다는 것을 의미한다.
		// 혹은, 클립 자체가 바뀌었거나.
		// 아무튼 이것과 상관없이 일단 프레임의 변화가 생겼다면, fPlayTime을 갱신해줄 필요가 있다.

		pClip->fPlayTime = m_vecCoord.size() * pClip->fFrameTime;

		// 옵션 시간의 경우에는 특정 시간이 지나면 사라지거나, 특정 시간이 지나면 return 되거나 하는 처리이기 때문에 갱신이 필요없다.
	}


}

void CEditManager::Render(HDC hDC, float fTime)
{
	// 소스 이미지와 애니메이션 정보를 출력.
	EditObjectRender(hDC, fTime);

	/*
		vacant 상태에서도 함수가 호출이 되는데, 애니메이션에 추가된 프레임이 없어서 인덱스에 접근을 하지 못한다.
		따라서, 애니메이션을 예외처리로 빼고, 나머지 소스와 클립정보는 출력한다.
	*/

	// StaticX, StaticY에 대한 선 출력
	StaticLineRender(hDC, fTime);

	// 그 위에 편집 정보를 출력.
	EditRender(hDC, fTime);

}

void CEditManager::ClipCut()
{
	// 현재 클립의 정보를 저장해둔다. 
	// 이름 정보도 미리 받아둔다.

	if (m_vecCoord.empty())
	{
		MessageBox(nullptr, TEXT("NO COORD"), TEXT("Error"), MB_OK);
		return;
	}



	m_pCutClip = m_pAnimation->GetClip();

	m_strCut = m_pEditObject->GetTag();

	// 이건 지금 선택해둔 프레임 사이즈이기 때문에 vector에 들어와 있는 (설정된 프레임 사이즈로 설정을 해야 한다.)

	m_tCutFrameSize = m_vecCoord[0].tEnd - m_vecCoord[0].tStart;

	// 프레임 사이즈가 안 잡혀있는 상태라면,(예들 들면 Load 상태)
	// Coord정보를 이용해서 셋팅한다.
	// Coord 정보가 없는데 키를 누른 경우는 예외처리하고 있으니 반드시 하나의 프레임이라도 존재한다.

}

void CEditManager::ClipConversion(bool bChange)
{
	ShowCursor(TRUE);

	if (!m_pCutClip)
	{
		MessageBox(nullptr, TEXT("자른 클립이 없습니다."), TEXT("Error"), MB_OK);

		return;

		// 자른 클립이 없는 경우 그냥 경고 메세지.
	}

	// 잘라온 클립의 오브젝트와 현재 편집하는 오브젝트가 다를 수 있다.
	//else if (m_strCurTag != m_strCut)
	//{
	//	// 잘라온 오브젝트의 태그와 현재 편집하고 있는 오브젝트의 태그가 다르면 잘못된 것이다.

	//	MessageBox(nullptr, TEXT("클립의 오브젝트가 다릅니다."), TEXT("Error"), MB_OK);

	//	// ex : Kirby / KirbyWalkLeft Clip -> 적용하려는 오브젝트가 Burt .
	//	// -> Error 상황이라고 볼 수 있다.

	//	return;
	//}


	// 자른 클립이 있는 경우에는 마지막으로 변환할지 물어본다. (기존 정보가 날라가는 처리)
	if (MessageBox(nullptr, TEXT("클립의 정보를 변환 하시겠습니까 ?"), TEXT("클립 변환"), MB_YESNO) == IDYES)
	{
		// CutClip의 정보를 복사해온다.
		

		// 현재 클립의 정보를 얻어온다.
		PAnimationClip pClip = m_pAnimation->GetClip();

		pClip->eOption = m_pCutClip->eOption;
		pClip->iOnceLoopFrame = m_pCutClip->iOnceLoopFrame;
		pClip->eType = m_pCutClip->eType;
		// 현재의 텍스처를 Cut의 텍스처로 교체하면 안 되지.
		// right기준의 Texture를 왜 왼쪽 클립에 적용을해.
		pClip->iColorKey = m_pCutClip->iColorKey;
		pClip->bColorKey = m_pCutClip->bColorKey;
		pClip->iFrame = 0;
		pClip->vecCoord = m_pCutClip->vecCoord; // src가 되는 vector를 일단 받아서 복사를 하고,
		pClip->tOffset = m_pCutClip->tOffset;  // 다시 조정을 하더라도 일단 복사하는 것이 맞다.

		pClip->fTime = 0.f;
		pClip->fPlayTime = m_pCutClip->fPlayTime;
		pClip->fFrameTime = m_pCutClip->fFrameTime;
		pClip->fOptionTime = m_pCutClip->fOptionTime;
		pClip->fOptionPlayTime = m_pCutClip->fOptionPlayTime;

		// 이름을 제외한 클립의 기본적인 정보, 플레이 시간, 옵션, 현재 진행 상황 등의 정보를 복사한다.

		// 좌표를 변환하는 작업을 진행한다.
		CoordConversion(pClip->vecCoord, bChange); // 해당 src정보를 이용하여 좌표를 변환.

		// 셋팅.
		m_vecCoord = pClip->vecCoord;

		m_iPrevFrameNumber = m_vecCoord.size();

		m_eAniOption = m_pCutClip->eOption;

		m_tFrameSize = m_tCutFrameSize;

		m_pEditObject->SetSize(m_tFrameSize);

		m_bFrameSize = true;

		m_tOffset = m_pCutClip->tOffset * -1.f;

		// 저장된 옵션의 정보를 받아온다.
		// (근데 결국에는 Save를 기준으로 처리하겠지만, 아무튼 기존의 옵션을 보여주는 처리가 된다는 것이다)

		if (m_eAniOption != AO_ONCE_LOOP)
			pClip->eOption = AO_LOOP;


		// 사용한 이후에는 풀어주도록 한다.
		//m_pCutClip = nullptr;

	}

	// 원하지 않는 경우 그냥 종료.


	ShowCursor(FALSE);

}

void CEditManager::CoordConversion(vector<ClipCoord>& vecSrcCoord, bool bChange)
{
	// 이곳으로 좌표가 들어오면 우리는 한 클립 정보 하나하나 마다 좌표를 구해준다.
	// 좌우 이미지에 대해서 사용할 것이므로 중심만 구하면 된다.

	if (bChange)
	{
		float fMidX = m_tBmpSize.x / 2.f;

		size_t iSize = vecSrcCoord.size();

		for (size_t i = 0; i < iSize; ++i)
		{
			CoordConversion(vecSrcCoord[i], fMidX);
		}

	}

	// 이렇게 하면  vector의 순서대로 (추가한 순서대로) 그대로 위치만 변경하여 
	// 프레임을 잡아주게 된다.
}

void CEditManager::CoordConversion(ClipCoord & tCoord, float fMidX)
{
	// 이곳에 들어온 Coord의 좌표를 이미지의 중심 Y축을 기준으로 변환하는 함수이다.
	// A -> A'로 변환하고 이것을 B'로 만든다.
	// D -> D'로 변환하고 이것을 C'로 만든다.

	// 이것은 LR변환이든 RL변환이든 동일하다.

	// fMid와 X의 거리를 구한다.
	// A'를 DestA라고 표현하겠다.

	Position tBegin = tCoord.tStart;

	if(tCoord.tStart.x < fMidX)
		tBegin.x = fMidX + abs(fMidX - tBegin.x);
	else
		tBegin.x = fMidX - abs(fMidX - tBegin.x);
	// y 는 그대로.

	// 이렇게 변환한 좌표를 왼쪽상단과 오른쪽 하단으로 만든다.

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
	if (!m_bSaveOption) // saved로 저장을 한다는 소리
	{
		m_bSaveOption = true; // 이것을 풀어준다.
		// 세이브 옵션이 false가 된다는 소리는 ... 옵션을 사용한다는 소리로.
		// 0번부터 시작한다.
	}
	else
	{
		// saved 옵션이 풀어진 상태라면 옵션을 바꿀 수 있도록 한다.
		int iOption = m_eSaveOption + 1;

		if (iOption >= AO_END)
		{
			// 마지막까지 가면, SaveOption으로 설정.
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
	// 각종 정보를 표시할 텍스트를 출력한다.
	TextRender(hDC, fTime);

	// 기존까지 추가했던 모든 Coord 정보 출력.
	CoordRender(hDC, fTime);

}

void CEditManager::EditObjectUpdate(float fTime)
{
	m_pAnimation->Update(fTime);
}

void CEditManager::MouseUpdate(float fTime)
{
	// 오른쪽 상단에 현재 마우스의 src에서의 위치를 출력한다.
	m_tMouseScreenPos = GET_SINGLE(CInput)->GetMousePos();
	m_tMouseSrcPos = m_tMouseScreenPos + GET_SINGLE(CSceneManager)->GetCameraPos();

}

void CEditManager::AddInput()
{
	

	// 애니메이션의 속도를 변경하는 키를 만든다.
	GET_SINGLE(CInput)->AddNormalKey("--FrameTime", VK_OEM_MINUS);
	GET_SINGLE(CInput)->AddNormalKey("++FrameTime", VK_OEM_PLUS);

	// 애니메이션 교체.
	GET_SINGLE(CInput)->AddNormalKey("++Animation", VK_F5);
	GET_SINGLE(CInput)->AddNormalKey("--Animation", VK_F6);


	// Camera Move 
	GET_SINGLE(CInput)->AddNormalKey("D", 'D');
	GET_SINGLE(CInput)->AddNormalKey("W", 'W');
	GET_SINGLE(CInput)->AddNormalKey("A", 'A');
	GET_SINGLE(CInput)->AddNormalKey("S", 'S');

	// 속도 증가 감소.
	GET_SINGLE(CInput)->AddNormalKey("F1", VK_F1);
	GET_SINGLE(CInput)->AddNormalKey("F2", VK_F2);
	// 현재 활성화된 프레임으로 크기를 고정시킴.  여러개가 잡혀있는 경우에는 처리하지 않음. 
	// 이렇게 설정한 이후에 이전에 잡아둔 모든 프레임 정보가 프레임에 맞게 수정됨.
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

	// ex : E + 키보드 위치이동.
	//		E + 마우스 위치이동
	//		R + 키보드 크기변경
	//		R + 마우스	크기변경


	// Most Newly Added Coord   Revive
	GET_SINGLE(CInput)->AddNormalKey("CtrlZ", 'Z', VK_LCONTROL); // DELETE : 가장 최근 좌표를 없앤다.
	// 근데 해당 좌표가 여러개일 가능성이 있으므로 vector로 받아둔다.

	// 저장하기
	GET_SINGLE(CInput)->AddNormalKey("CtrlS", 'S', VK_LCONTROL);
	
	// 불러오기
	GET_SINGLE(CInput)->AddNormalKey("CtrlL", 'L', VK_LCONTROL);

	// 편집 프레임 붙여넣기
	GET_SINGLE(CInput)->AddNormalKey("CtrlV", 'V', VK_LCONTROL);

	// 모든 프레임 편집모드 선택.
	GET_SINGLE(CInput)->AddNormalKey("CtrlA", 'A', VK_LCONTROL);

	// Frame 간격 설정.
	GET_SINGLE(CInput)->AddNormalKey("++FrameAdjX", VK_NEXT); // x 증가
	GET_SINGLE(CInput)->AddNormalKey("--FrameAdjX", VK_DELETE); // x 감소
	GET_SINGLE(CInput)->AddNormalKey("++FrameAdjY", VK_HOME); // y 증가
	GET_SINGLE(CInput)->AddNormalKey("--FrameAdjY", VK_END); // y 감소

	// 애니메이션 프레임 넘기기. 보통 P(정지) 상태로 사용할 예정
	GET_SINGLE(CInput)->AddNormalKey("3", '3'); // Next 
	GET_SINGLE(CInput)->AddNormalKey("4", '4'); // Prev


	// 변환 모드 활성화.
	GET_SINGLE(CInput)->AddNormalKey("ConversionMode", 'Y');

	// 사본저장.
	GET_SINGLE(CInput)->AddNormalKey("ClipCut", 'U'); 

	// 사본 붙여 넣기. Reverse
	GET_SINGLE(CInput)->AddNormalKey("ClipCopy", 'I');

	// 사본 붙여 넣기 NOT Reverse
	GET_SINGLE(CInput)->AddNormalKey("ClipCopy2", 'O');

	// 편집하는 오브젝트 변경.
	GET_SINGLE(CInput)->AddNormalKey("ObjectChange", VK_TAB);

	// Size 변경
	GET_SINGLE(CInput)->AddNormalKey("Shift", VK_SHIFT);

	// 출력 위치 변경
	GET_SINGLE(CInput)->AddNormalKey("Ctrl", VK_LCONTROL);



}

void CEditManager::CoordRender(HDC hDC, float fTime)
{
	// 현재 잡혀있는 coord정보를 모두 출력한다.

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


		// 오른쪽 상단 좀 아래에 해당 tStart의 좌표를 출력해준다.
		wsprintf(strPos, TEXT("%d. (%d, %d)"), i, (int)m_vecCoord[i].tStart.x, (int)m_vecCoord[i].tStart.y);

		int iRenderX = RS_WIDTH / 2 - 450 + (i / 5) * 100;
		int iRenderY = (i % 5) * 20 + 20;

		TextOut(hDC, iRenderX, iRenderY, strPos, lstrlen(strPos));
		
	}



	// 현재 그리고 있는 Coord정보를 출력해준다.

	if (m_eState == CS_FRAME_DOING)
	{
		// SRC의 좌표를 스크린 좌표로 바꾸려고 한다.
		// SRC TO SCREEN  (스크린 좌표 + 카메라 좌표) - 카메라 좌표 = 스크린 좌표.
		// SCREEN TO SRC  스크린 좌표 + 카메라 좌표.

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

	// 이전에 삭제된 프레임을 임시로 담아둔다.
	m_vecPrevCoord.push_back(m_vecCoord[iIndex]);

	for (size_t i = iIndex; i < iSize - 1; ++i)
	{
		// 한 칸씩 옮기는 과정
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
			// 없애려는 인덱스를 만났다.
			// 뒤에 있는 인덱스를 땡기고 끝낸다.

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
	// 마우스가 바깥으로 빠졌다면, 해당 작업을 하고 있지 않은 것으로 인식하고
	// 입력을 막는다.
	// (카메라 이동하는 키를 막아버리는 처리 + 단축키 무시)
	if (!GET_SINGLE(CInput)->MouseIsScreen())
	{
		m_eState = CS_NONE;
		return;
	}



	// CameraInput
	CameraInput(fTime);

	// 기본적인 활성화/비활성화, 크기 증감에 대한 처리를 하는 함수 (비교적 단순한 키입력)
	BasicInput(fTime);

	// Coord Move 정교한 이동.   마우스는 아래에서 처리.

	if (!m_bOffset)
	{
		if (!m_vecEditCoordIndex.empty())
		{
			if (m_eChange == CCS_MOVE) // E + 키보드 : 키보드로 위치이동.
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
				// E, R을 PRESS한 상태라는 것은 오른쪽버튼을 누르지 않는다는 말이다.

			}

			else if (m_eChange == CCS_RESIZE) // R + 키보드 : 키보드로 사이즈 변경
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


			// 하나의 편집 프레임을 선택.
			if (m_vecEditCoordIndex.size() == 1)
			{

				// Once loop Frame Set
				if (KEYDOWN("F7") && (m_eSaveOption == AO_ONCE_LOOP))
				{

					// 하나의 프레임을 누른 상태에서 F7을 누른 경우
					// 해당 프레임을 iOnceFrame으로 주는 것이다.
					// Once Loop옵션을 설정해둔 상태에서 해당 프레임을 OnceLoopFrame으로 설정하는 경우.

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
		// 키보드를 통한 오프셋 설정.

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


	// 마우스 Coord Change
	// Mouse + E : 위치 조정

	if (!m_bOffset) // Offset Off 
	{


		if (KEYPRESS("MouseRButton"))
		{
			Position m_tMouseMove = GET_SINGLE(CInput)->GetMouseMove();

			if (KEYPRESS("Shift"))
			{
				// 출력 사이즈
				m_pEditObject->SetSize(m_pEditObject->GetSize() + m_tMouseMove);
			}

			else if (KEYPRESS("Ctrl"))
			{
				// 출력 위치조절.
				m_tPrintOffset += m_tMouseMove;

				if (m_tPrintOffset.x < -1600.f || m_tPrintOffset.x > 1600.f ||
					m_tPrintOffset.y > 1600.f || m_tPrintOffset.y < -1600.f)
					m_tPrintOffset = Position(0.f, 0.f);
				

			}
			else
			{
				if (m_eChange == CCS_MOVE)
				{
					// 활성화된 Coord정보의 크기를 바꾸거나 위치를 바꾸는 처리를 한다.


					// 크기를 바꾸거나, Size를 바꾸는 것으로 한다. (중복은 안 되도록)

					MoveEditCoord(m_tMouseMove);
				}
				// Mouse + R : 크기 조정
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
			// 하나의 설정에서만, 처리한다.

			m_tFrameSize = m_vecCoord[iIndex].tEnd - m_vecCoord[iIndex].tStart;
			// 설정된 인덱스에 해당하는 프레임크기를 고정한다.

			m_bFrameSize = true;

			// 프레임 Size가 정해졌다면, 이전 프레임을 프레임 사이즈에 맞게 고친다.  
			// begin + frameSize


			size_t iSize = m_vecCoord.size();

			for (size_t i = 0; i < iSize; ++i)
			{
				m_vecCoord[i].tEnd = m_vecCoord[i].tStart + m_tFrameSize;
			}

			// 프레임 사이즈가 정해지는 순간. 해당 프레임 사이즈로 Object의 출력사이즈로 준다.
			m_pEditObject->SetSize(m_tFrameSize);


		}
		else if (m_bFrameSize)
			m_bFrameSize = !m_bFrameSize;
		
		// F3 키를 통해서, 프레임 하나를 고른 이후에 프레임 사이즈를 설정할 수 있다.
		// 이후에 이 프레임 사이즈를 풀고 싶다면 다시 F3키를 누른다.
		// 여러 프레임을 선택한 후에 F3키를 누른 경우 따로 처리하지 않는다.
	}

	// 기능키 교체 : 충돌활성화(안 써요..) -> 겹친영역 중복 충돌 활성화.
	if (KEYDOWN("Q"))
	{
		m_bOverlapCollision = !m_bOverlapCollision;

	}

	// 모든 정보 정리.
	if (KEYDOWN("ESC"))
	{

		ShowCursor(TRUE);

		if (m_bOffset)
		{
			if (MessageBox(nullptr, TEXT("오프셋 초기상태로"), TEXT("(0.0)"), MB_YESNO) == IDYES)
			{
				m_tOffset = Position(0.f, 0.f);
			}
		}
		else
		{
			// 최후의 속삭임
			if (MessageBox(nullptr, TEXT("모든 프레임 삭제"), TEXT("Warning"), MB_YESNO) == IDYES)
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





	// 마우스의 상태에 따라서, coord의 정보를 추가한다.	

	if (!m_bOffset)
	{
		if (KEYDOWN("MouseLButton"))
		{


			// Coord활성화에 대한 처리. 
			if (!m_vecCollisionCoordIndex.empty())
			{
				// 상황을 예로 들어보자. 여러개의 충돌 프레임이 존재한다.
				// 여기서 겹치는 영역을 누른 경우에 이 두 프레임을 모두 EditIndex에 넣는 것이 맞을까
				// 혹은 하나만 EditIndex에 넣는 것이 맞을까...
				// => 충돌된 프레임을 모두 넣는 것이 맞는 듯 하다.

				// 라고 한 7일 전에 생각했던 것 같은데 이제 새로운 기능을 추가할 거야.

				// 다중 충돌 활성화 기능 !
				// 이 기능이 활성화가 되어 있다면, 기존과 마찬가지로 한번에 여러개를 선택할 수 있어

				// 하지만 비활성화인 경우에는 한번에 하나씩만 편집 인덱스로 추가할 수 있는 거지.
				// 왜냐면 그것만 충돌이 되니까.

				// 아무튼 이것은 충돌에서만 처리하는 부분이고 편집 인덱스로 넣는 것과는 전혀 상관이 없어.
				// 다중 충돌을 통해서 여러개를 한번에 추가하든, 하나씩 여러번 추가하든 그건 사용자 마음이고,
				// 아무튼 충돌의 횟수가 달라지는 부분이라,
				// 그걸 선택하여 Edit에 넣는 것은 하나가 충돌이 되어서 하나만 넣느냐
				// 혹은 여러개가 한번에 처리가 되어서 여러개를 한번에 넣느냐에 차이야.
				// 왜냐면 vector에 들어있는 size에서만 차이가 있는거잖아.

				size_t iSize = m_vecCollisionCoordIndex.size();

				// 비활성화 상태라면 1이겠지. 그리고 이것을 넣어줄거야.
				// 활성화 상태라면 여러개가 들어올 가능성이 있고 이것을 한번에 Edit으로 추가하겠지.

				for (size_t i = 0; i < iSize; ++i)
				{
					bool bIsIndex = IsIndex(m_vecCollisionCoordIndex[i]);
					// 이미 EditCoordIndex로 등록된 인덱스를 넣어주려는 경우일 수도 있다.

					if (!bIsIndex)
					{
						m_vecEditCoordIndex.push_back(m_vecCollisionCoordIndex[i]);


						// 편집 상태를 준다.
						m_eState = CS_CHANGE_DOING;


						if (!m_bOverlapCollision)
						{
							// 2개 이상 나올 수 있는 경우는 다중 충돌이 허용되는 시점에서 이다.
							sort(m_vecEditCoordIndex.begin(), m_vecEditCoordIndex.end());
							vector<int>::iterator iter = unique(m_vecEditCoordIndex.begin(), m_vecEditCoordIndex.end());
							m_vecEditCoordIndex.resize(distance(m_vecEditCoordIndex.begin(), iter));

							// 중복으로 원소가 추가되는 경우가 있음.
							// 이 경우를 잘 모르겠으니 일단은 ... 예외로 막아버림.
						}

						//if (m_vecEditCoordIndex.size() > m_vecCoord.size())
						//	int i = 0; // ERROR


						// 정렬을 하는 이유는 인덱스가 작은 순부터 들어가서 삭제를 해줘야 남은 인덱스에 주는 영향(한칸씩 당김)이 맞기 때문이다.
						// 3 5 7 이라면,
						// 3 인덱스 없애고 5, 7은 한 인덱스씩 땡겨서 4 6 .. 4없애고 땡겨서 5 . 5 없애고 끝.
						// 5 7 3 이라면, 5없애고 땡겨서 6 2(!) 5뒤에 있던 인덱스는 영향을 받지만, 앞에 있던 것까지 영향을 받지는 않는다.
						// 남은 인덱스 - 1 의 처리를 하기 위해서는 정렬이 필요하다.
					}

					else
					{
						// 인덱스를 넣어주면, 해당 Indexvec에서 해당 값을 찾는다.
						// 그리고 해당 값이 있는 곳의 인덱스로부터 한칸씩 땡기고 사이즈 감소.



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
				// begin의 좌표로 설정.
				m_tBegin = m_tMouseSrcPos;
				m_eState = CS_FRAME_DOING;
			}


		}

		else if (KEYPRESS("MouseLButton"))
		{
			if (m_eState == CS_FRAME_DOING)
			{
				m_tEnd = m_tMouseSrcPos;

				//  프레임이 고정이라고 하더라도 일단 그림은 그려준다.
			}

		}

		else if (KEYUP("MouseLButton"))
		{
			if (m_eState == CS_FRAME_DOING)
			{
				// 좌표를 넣어줄 때, 해당 크기가 터무니없이 작거나, 터무니 없이 큰 경우를 막는다.
				if (!m_bFrameSize)
				{
					Size tSize = m_tEnd - m_tBegin;
					if (tSize.x >= 10.f && tSize.y >= 10.f)
						m_vecCoord.push_back(ClipCoord(m_tBegin, m_tEnd));

					// 10 * 10 이상인 경우에만 넣어준다.
				}
				else
				{
					// 프레임사이즈를 고정시키는 순간부터 dest의 위치는 무시하고, 프레임의 사이즈를 맞춘다.
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
	// 마우스와 충돌하고 있는 모든 프레임의 인덱스를 CollisionIndex에 넣어준다.

	m_vecCollisionCoordIndex.clear();


	size_t iSize = m_vecCoord.size();

	for (size_t i = 0; i < iSize; ++i)
	{
		// 모든 프레임을 순회하면서 충돌처리가 되었는지 확인한다.
		// 세분화까지는 필요없고, 단순히 충돌 or NOT으로 구분한다.

		RectInfo tRect(m_vecCoord[i]);
				
		if (CMath::CollisionRectToPoint(tRect, m_tMouseSrcPos, m_tIntersect))
		{
			// 마우스와 충돌이 일어난 경우에는 충돌인덱스로 추가한다.
			m_vecCollisionCoordIndex.push_back(i);
		
		}
			
	}

}

void CEditManager::MouseOverSoleCoord()
{
	// 마우스와 충돌하고 있는 모든 프레임의 인덱스를 CollisionIndex에 넣어준다.

	m_vecCollisionCoordIndex.clear();


	int iSize = (int)m_vecCoord.size();

	for (int i = iSize - 1; i >= 0; --i)
	{
		// 모든 프레임을 순회하면서 충돌처리가 되었는지 확인한다.
		// Z축이 없는 프레임의 세상에서 가장 위에 있는 프레임의 기준이 있는가 ?
		// 그냥 단순히 생각해서 가장 최근에 추가한 프레임이 가장 위에 있을 것이므로 가장 최근의 프레임
		// (순서대로 넣으니까 가장 뒤쪽에 있는 것부터 보면 된다)
		// 하나를 충돌시키자는 거지.

		RectInfo tRect(m_vecCoord[i]);

		if (CMath::CollisionRectToPoint(tRect, m_tMouseSrcPos, m_tIntersect))
		{
			// 마우스와 충돌이 일어난 경우에는 가장 최근 추가한 충돌인덱스를 추가하며 return.
			m_vecCollisionCoordIndex.push_back(i);

			return;

		}

	}

	// 충돌한 인덱스가 없을 수도 있다.
	// 아무튼 충돌이라고 체크하는 인덱스가 (충돌된 프레임들 중에서 가장 최근 것을 인덱스로 넣는 것이다.)

	
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
		// 매 프레임마다, 충돌이 된 프레임을 검사하여, CollisionIndex에 넣어준다.
		MouseOverCoord();
	}
	else
	{
		// 마우스와 충돌하는 프레임을 하나로 고정하되 이 하나의 프레임은 가장 위의 프레임으로 한다.
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
	// XY Static은 그냥. CtrlV와 연계하기 위한 값으로 둔다. 
	// 제약을 풀어주기 위함.

	//// X 축 이동인데, X축이 막힌 경우
	//// Y축 이동인데, Y축이 막힌 경우
	//if (bHorizon && m_eStatic == SS_X ||
	//	!bHorizon && m_eStatic == SS_Y)
	//	return;

	//// 수정이 가능한 경우인데, 만약 간격이 켜져있는 경우에는 모든 프레임에 대해서 처리한다.
	//if (m_bFrameAdj)
	//	CtrlA();

	size_t iSize = m_vecEditCoordIndex.size();

	for (size_t i = 0; i < iSize; ++i)
	{
		// 선택한 프레임의 인덱스를 가져온다.
		int iIndex = m_vecEditCoordIndex[i];

		// 해당 프레임의 정보에 접근하여 위치를 변경한다.
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
		// 선택한 프레임의 인덱스를 가져온다.
		int iIndex = m_vecEditCoordIndex[i];

		// 해당 프레임의 정보에 접근하여 위치를 변경한다.
		m_vecCoord[iIndex].tStart += tMouseMove;
		m_vecCoord[iIndex].tEnd += tMouseMove;

	}
}

void CEditManager::ResizeEditCoord(float fScale, bool bHorizon)
{
	// 프레임 사이즈가 고정인 경우에는 크기를 바꿀 수 없다.
	if (m_bFrameSize)
		return;

	// 프레임 Size에 고정이 된 경우에는 변경하지 않는다.
	// XY Static은 그냥. CtrlV와 연계하기 위한 값으로 둔다. 
	// 제약을 풀어주기 위함.

	/*if (bHorizon && m_eStatic == SS_X ||
		!bHorizon && m_eStatic == SS_Y)
		return;

	if (m_bFrameAdj)
		CtrlA();*/

	size_t iSize = m_vecEditCoordIndex.size();

	for (size_t i = 0; i < iSize; ++i)
	{
		// 선택한 프레임의 인덱스를 가져온다.
		int iIndex = m_vecEditCoordIndex[i];

		// 해당 프레임의 정보에 접근하여 크기를 변경한다.
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
		// 선택한 프레임의 인덱스를 가져온다.
		int iIndex = m_vecEditCoordIndex[i];

		m_vecCoord[iIndex].tEnd += tMouseMove;

	}
}

void CEditManager::Save(const char* strFrontTag)
{
	ShowCursor(TRUE);

	if (MessageBox(nullptr, TEXT("정말 저장하시겠습니까 ?"), TEXT("Save Clip"), MB_YESNO) == IDYES)
	{
		// 현재 처리하고 있는 클립의 정보를 가져온다.
		PAnimationClip pClip = m_pAnimation->m_pClip;

		// 파일이름에 현재 시간정보를 포함하기 위해서, LocalTime을 읽어온다.
		

		// 파일이름을 만든다.
		char strName[100] = {};

		// FrontTag 를 복사한다.
		strcpy_s(strName, sizeof(char) * 100, strFrontTag);

		// 현재 클립의 이름을 붙인다.
		strcat_s(strName, sizeof(char) * 100, pClip->strName.c_str());
		// Kirby/KirbyIdleRight

	/*
		SYSTEMTIME st;
		GetLocalTime(&st);	char* strLocal = new char[32];
		memset(strLocal, 0, sizeof(char) * 32);

		sprintf_s(strLocal, sizeof(char) * 32, " %d.%d.%d %dh %dm.clip", st.wYear,
			st.wMonth, st.wDay, st.wHour, st.wMinute);

		strcat_s(strName, sizeof(char) * 100, strLocal);*/
		// 저장하는 경우에는 "파일이름/클립이름 월.일-시간-분.clip" 의 형태로 저장한다.

		strcat_s(strName, sizeof(char) * 100, ".clip"); // 일단은 그냥 클립으로 설정.

		// 해당 이름의 형태로 현재 클립정보를 저장한다.
		// 세이브 옵션을 사용하는 경우에는 그것으로 저장. 사용하지 않는 경우에는 저장된 옵션을 사용하여 저장한다.
		if (!m_pAnimation->SaveClip(strName, m_bSaveOption, m_eSaveOption))
		{
			MessageBox(nullptr, TEXT("저장하기 실패"), TEXT("SAVE ERROR"), MB_OK);
		}

		//delete[] strLocal;

	}

	ShowCursor(FALSE);

}

void CEditManager::SaveAll(const char * strFrontTag)
{
	ShowCursor(TRUE);

	if (MessageBox(nullptr, TEXT("정말 모두 저장하시겠습니까 ?"), TEXT("Save All Clip"), MB_YESNO) == IDYES)
	{
		unordered_map<string, PAnimationClip>::iterator iter;
		unordered_map<string, PAnimationClip>::iterator iterEnd = m_pAnimation->m_mapClip.end();

		for (iter = m_pAnimation->m_mapClip.begin(); iter != iterEnd; ++iter)
		{
			// 클립의 정보를 가져온다.
			PAnimationClip pClip = iter->second;

			char strName[100] = {};

			// FrontTag 를 복사한다.
			strcpy_s(strName, sizeof(char) * 100, strFrontTag);

			// 현재 클립의 이름을 붙인다.
			strcat_s(strName, sizeof(char) * 100, pClip->strName.c_str());
			// Kirby/KirbyIdleRight.clip

			strcat_s(strName, sizeof(char) * 100, ".clip");

			// 해당 이름의 형태로 현재 클립정보를 저장한다.

			if (!m_pAnimation->SaveClip(strName, false))
			{
				MessageBox(nullptr, TEXT("저장하기 실패"), TEXT("SAVE ERROR"), MB_OK);
			}
		}
	}

	ShowCursor(FALSE);

}

void CEditManager::Load(const char* strFrontTag)
{

	ShowCursor(TRUE);

	if (MessageBox(nullptr, TEXT("정말 불러오시겠습니까 ?"), TEXT("Load Clip"), MB_YESNO) == IDYES)
	{
		PAnimationClip pClip = m_pAnimation->m_pClip;

		// 읽어올 파일의 이름을 만든다.
		// 읽어오는 경우에는 "파일이름/클립이름.clip" 으로 읽어온다.

		char strName[100] = {};

		// 현재 클립의 이름을 저장한다.
		strcpy_s(strName, sizeof(char) * 100, strFrontTag);
		strcat_s(strName, sizeof(char) * 100, pClip->strName.c_str());

		strcat_s(strName, sizeof(char) * 100, ".clip");

		if (m_pAnimation->LoadClip(strName))
		{

			// 현재 클립의 정보가 다시 셋팅이 되었을 것이다. 프레임의 수와 상관없이 초기값 설정.

			PAnimationClip pClip = m_pAnimation->GetClip();

			m_iPrevFrameNumber = pClip->vecCoord.size();


			m_eAniOption = pClip->eOption;

			if (m_eAniOption != AO_ONCE_LOOP)
			{
				// 편집모드에서는 항상 LOOP
				pClip->eOption = AO_LOOP;
			}

			

			// 편집모드에 있던 정보를 날려버리고, 현재 읽어온 값을 셋팅한다.

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
			MessageBox(nullptr, TEXT("불러오기 실패"), TEXT("LOAD ERROR"), MB_OK);
		}

	}

	ShowCursor(FALSE);

}

void CEditManager::AnimationChange(bool bScale)
{
	// 기존의 클립에서 loop로 설정해둔 옵션을 복구한다.
	PAnimationClip pClip = m_pAnimation->GetClip();
	pClip->eOption = m_eAniOption;

	// 클립이름에 아무것도 들어가지 않은 경우 return.
	if (m_vecClipName.empty())
		return;

	m_pAnimation->ChangeClip(m_vecClipName[GetClipNameIndex(bScale)]);

	// 같은 오브젝트 사이에서도 텍스처가 다를 가능성이 있으므로 Bmp사이즈를 다시받아둔다.
	m_tBmpSize = m_pAnimation->GetBmpSize();


	// 비트맵 사이즈가 화면 크기를 넘어가는 경우에는 그것으로, 아닌 경우에는 화면크기로 설정.
	if (m_tBmpSize.x >= RS_WIDTH && m_tBmpSize.y >= RS_HEIGHT)
		GET_SINGLE(CSceneManager)->SetLimitCamera(m_tBmpSize);
	else
		GET_SINGLE(CSceneManager)->SetLimitCamera(Size(RS_WIDTH, RS_HEIGHT));

	// 클립을 바꾸고 새로운 클립을 받아온다.
	pClip = m_pAnimation->GetClip();
	m_vecCoord = pClip->vecCoord;
	m_tOffset = pClip->tOffset * -1.f;

	
	// 새로운 클립의 기존 옵션을 받아둔다.
	m_eAniOption = pClip->eOption;
	m_iPrevFrameNumber = m_vecCoord.size();
	if (pClip->eOption != AO_ONCE_LOOP)
		pClip->eOption = AO_LOOP;
	m_vecEditCoordIndex.clear();
	m_vecCollisionCoordIndex.clear();

	// 프레임 사이즈에 맞춰서 출력을 해야하므로, Object의 Size를 바꿔준다.

	if(!m_vecCoord.empty())
		m_pEditObject->SetSize(m_vecCoord[0].tEnd - m_vecCoord[0].tStart);
}

void CEditManager::CtrlV()
{
	// 기본조건. 프레임 사이즈를 고정시켜두었다.
	if (!m_bFrameSize)
		return;

	// 1개의 EditFrame이 들어온 경우에만 CtrlV가 써진다.
	// 여기가 매우 중요한 부분.

	// 일단, 모든 경우. 마우스 좌표와 편집프레임이 충돌했는지 확인한다.

	int iIndex = m_vecEditCoordIndex[0];

	ClipCoord tCoord = m_vecCoord[iIndex];

	bool bCollision = IsCollision(iIndex);

	if (bCollision || m_eStatic == SS_NONE)
	{
		// 만약, 충돌이거나 NONE 상태라면 편집 프레임에 해당하는 프레임을 하나 추가한다.
		// 기존의 편집 프레임을 날려주고, 새로 생성한 프레임을 편집 프레임에 넣는다.

		m_vecEditCoordIndex.clear();

		m_vecEditCoordIndex.push_back(m_vecCoord.size());

		m_vecCoord.push_back(tCoord);
		
		return;
	}

	// 위에서 none 상태이거나, 충돌이 일어난 경우에 대해서 처리했다.
	// 아래의 경우는 마우스의 현재 좌표에 따른 처리가 일어나야 한다.
	// 현재 vecCoord에 있는 정보는 모두 월드 좌표이다. 따라서, 마우스의 월드 좌표를 가져온다.  m_tMouseSrcPos


		// 예시.  y축이 고정이 되어 있는 상태로 x의 간격을 증가시켰다.
		// 이런 상황에서 한 프레임을 기준으로 하여 CtrlV키를 누른 경우 마우스의 위치 기준(좌우상하) 중에서
		// y축 고정이므로 좌 충돌 우, 이렇게 3가지가 있는데 충돌이면 그 자리 그대로 복사
		// 좌인 경우에는 편집하는 프레임의 begin좌표를 기준으로 Adj.x + fFrameX만큼 빼서, 새로운 프레임의 begin으로 삼는다
		// 이후에 Frame값을 이용하여 end를 구할 것이다.

		// 오른쪽에 있던 경우에는 fFrameX + Adj.x 만큼 더해서 그곳을 begin으로 삼아서 처리한다.
		// 같은 논리로 Y축 고정에 대해서도 진행하게 된다.

	// 편집 프레임의 좌표로부터 마우스의 위치를 구해낸다.

	ClipCoord tNewCoord;

	switch (m_eStatic)
	{
	case SS_X:
		// X좌표가 고정인 경우에는 위인지 아래인지를 판단한다.
		if (tCoord.tEnd.y < m_tMouseSrcPos.y)
		{
			// 아래쪽에 있는 경우 아래에 새로운 프레임을 추가한다.
			// 간격 정보가 있다면, Y정보를 사용하게 된다.

			Position tSrcPos = tCoord.tStart;
		
			// src위치에서 프레임 y만큼의 크기를 더한다.
			tSrcPos.y += m_tFrameSize.y;

			// 간격이 있다면 추가한다.
			if (m_bFrameAdj)
				tSrcPos.y += m_tFrameAdj.y;

			// 이렇게 잡힌 값은 begin으로 하여 end를 잡고 coord를 넣어준다.
			tNewCoord = ClipCoord(tSrcPos, tSrcPos + m_tFrameSize);
		}
		else
		{
			// 위쪽인 경우에는 y간격을 빼고, FrameY간격을 빼준다.
			Position tSrcPos = tCoord.tStart;

			tSrcPos.y -= m_tFrameSize.y;

			if (m_bFrameAdj)
				tSrcPos.y -= m_tFrameAdj.y;

			tNewCoord = ClipCoord(tSrcPos, tSrcPos + m_tFrameSize);

		}
		break;

	case SS_Y:
		// Y좌표가 고정인 경우에는 오른쪽과 왼쪽을 판단한다.
		if (tCoord.tEnd.x < m_tMouseSrcPos.x)
		{
			// 오른쪽
			Position tSrcPos = tCoord.tStart;

			tSrcPos.x += m_tFrameSize.x;

			if (m_bFrameAdj)
				tSrcPos.x += m_tFrameAdj.x;

			tNewCoord = ClipCoord(tSrcPos, tSrcPos + m_tFrameSize);
		}
		else
		{
			// 왼쪽
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
	// 모든 인덱스를 추가한다.(전체 선택)
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
		// 프레임 간격이 활성화되어 있는 경우에만 처리.
		// 고정이 되어있지 않은 경우에는 세로혹은 가로로 맞춰져 있는 경우가 아닌 것이다.
		// 따라서 일일이 잡아주는 수밖에 없을 것. ( 간격 맞출 필요가 없음 )

		switch (m_eStatic)
		{
		case SS_X:
			// StaticX 상태면, Y간격
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
			// StaticY 상태면, X간격
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
		/*SaveAll(s.c_str());*/ // 모든 클립의 정보를 저장한다.
		Save(s.c_str());
	}
	else if (KEYDOWN("CtrlL"))
	{
		string s = "../Bin/Save/"; // 보통은 Tag인데, 몇몇은 따로 두기 떄문에 통용되지는 않는다.
		// 아무튼 그래도 Tag를 붙여주자. OK 
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

	// 전체 선택(편집인덱스로 설정)
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


	// 작업 모드 변경
	// E, R을 누른 이후에 우클릭을 통해서 작업을 할 수 있도록 한다.

	if (!m_bOffset)
	{
		// 오프셋이 켜져있는 경우 교체 불가.
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

			// 배열에 들어가 있는 인덱스를 하나씩 날려준다.

			for (size_t i = 0; i < m_vecEditCoordIndex.size(); ++i)
			{
				CoordDelete(m_vecEditCoordIndex[i]);

				// 한번 처리를 하고 난 이후에는 하나의 정보를 없애면서, 인덱스가 하나씩 땡겨지는데,
				// 여기에서 size값에서 걸리면, 잘못된 인덱스로 판단하고 없애지 않는 처리가 된다.

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

		// 단순 복사.
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
		// Offset 설정모드
		// 기존의 처리가 있다면, 풀어버린다.


	}

	// Static State
	if (KEYDOWN("9"))
	{
		StaticStateChange();
	}

	if (KEYDOWN("3"))
	{
		// 다음 프레임으로 넘긴다.
		PAnimationClip pClip = m_pAnimation->GetClip();
		++pClip->iFrame;

		if (pClip->iFrame == pClip->vecCoord.size())
			pClip->iFrame = 0;
	}

	else if (KEYDOWN("4"))
	{
		// 이전 프레임으로 돌아간다.
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


	// KeyInput  문서 작업하다가 p눌러서  pause상태가 되지 않도록 해준다.
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
	// 마우스의 src에서의 위치를 출력한다.

	TCHAR strPos[40] = {};

	wsprintf(strPos, TEXT("Mouse : (%d, %d)"), (int)m_tMouseSrcPos.x, (int)m_tMouseSrcPos.y);

	TextOut(hDC, 20, 20, strPos, lstrlen(strPos));

	// 중복 충돌 활성화 정보 출력
	if (m_bOverlapCollision)
	{
		TextOut(hDC, 20, 40, TEXT("OverlapCollision"), lstrlen(TEXT("OverlapCollision")));
	}

	if (m_bFrameSize)
	{
		// FrameSize 고정이 되어 있다면 출력.
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
		// Save Option 이 없다면
		TextOut(hDC, 20, 100, TEXT("Save : Saved"), lstrlen(TEXT("Save : Saved")));

	}


	// Once Loop Frame을 출력해준다. Once Loop의 옵션인 경우에만 의미가 있다.
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

			// 자른 애니메이션의 이름을 보여준다.
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
	// 오브젝트를 바꾸기 전에 기존 셋팅을 
	PAnimationClip pClip = m_pAnimation->GetClip();
	pClip->eOption = m_eAniOption;



	// 현재 선택된 오브젝트 해제하고, 새로운 오브젝트를 잡아서 편집하는 기능이다.

	SAFE_RELEASE(m_pEditObject);

	m_pEditObject = GET_SINGLE(CSceneManager)->FindNextObject(m_strCurTag);

	m_strCurTag = m_pEditObject->GetTag();

	m_pAnimation = m_pEditObject->GetAnimation();

	pClip = m_pAnimation->GetClip();
	

	// 비트맵의 사이즈를 읽어온다. 여기서 오브젝트는 아틀라스 이미지를 함께 사용하고 있을 것이므로
	// 한번만 비트맵의 사이즈를 읽어오면 된다.
	m_tBmpSize = m_pAnimation->GetBmpSize();

	GET_SINGLE(CSceneManager)->LimitCameraDisEnable();

	if (m_tBmpSize.x >= RS_WIDTH && m_tBmpSize.y >= RS_HEIGHT)
		GET_SINGLE(CSceneManager)->SetLimitCamera(m_tBmpSize);

	// 현재 대상의 클립에 뭔가 저장이 되어 있을 가능성이 있다.
	m_vecCoord = m_pEditObject->GetvecCoord();

	// 오프셋 당연히 받아온다.
	m_tOffset = pClip->tOffset * -1.f;


	m_iPrevFrameNumber = m_vecCoord.size();

	// LOOP로 변경하기 전의 애니메이션 옵션을 받아둔다 (저장된 옵션값)
	m_eAniOption = pClip->eOption;

	// 저장을 원하는 옵션은 나중에 주고 일단, 편집모드에서는 loop처리를 해야한다.
	if (pClip->eOption != AO_ONCE_LOOP)
		pClip->eOption = AO_LOOP;


	m_vecEditCoordIndex.clear();
	m_vecCollisionCoordIndex.clear();


	if (!m_vecCoord.empty())
		m_pEditObject->SetSize(m_vecCoord[0].tEnd - m_vecCoord[0].tStart);
	
	// ClipName을 받아온다.
	m_vecClipName.clear();

	m_pEditObject->AddClipName(m_vecClipName);
}
