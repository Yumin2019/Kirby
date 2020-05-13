#include "Input.h"
#include "../Object/Obj.h"
#include "../Collision/Collider.h"
#include "../Animation/Animation.h"
#include "../UI/UIImage.h"
#include "../Collision/ColliderPoint.h"
#include "../Core/GameManager.h"

DEFINITION_SINGLE(CInput);

CInput::CInput() :
	m_pNormalKey(nullptr),
	m_pMouse(nullptr),
	m_pMouseCollider(nullptr),
	m_pMouseAnimation(nullptr),
	m_bIsScreen(false)
{
}


CInput::~CInput()
{
	SAFE_RELEASE(m_pMouse);
	SAFE_RELEASE(m_pMouseCollider);
	SAFE_RELEASE(m_pMouseAnimation);


	// 동적할당을 해서 추가해둔 map정보를 날린다.
	Safe_Delete_Map(m_mapBindAxis);
	Safe_Delete_Map(m_mapBindAction);
	

	// BindActionInfo BindAxisInfo의 데이터는 heap메모리를 사용하지 않고 있다.
	// 그 외 KeyList, InputKey도 스택에 잡히는 메모리다.
	// 따라서 map만 비워주면 된다.

	Safe_Delete_Map(m_mapNormalKey);
}

bool CInput::Init(HWND hWnd)
{
	// 여기서는, 키보드를 통해 입력할 수 있는 기본키에 대해서, 추가해주고 그외 특수키
	// SHIFT, ALT, CTRL에 대한 정보도 추가해준다.

	m_hWnd = hWnd;

	m_KeyList.resize(256); 

	for (int i = 0; i < 256; ++i)
	{
		m_KeyList[i].cKey = (unsigned char)i;
		// Enable 건들이지 말고, 상태 정보 건들이지 말고, 단순히 해당 키만 넣어준다.
	}

	// 특수키에 대한 정보를 추가한다.

	m_tSKey[SKEY_CTRL].cKey = VK_CONTROL;
	m_tSKey[SKEY_SHIFT].cKey = VK_SHIFT;
	m_tSKey[SKEY_ALT].cKey = VK_MENU;

	memset(&m_tMousePos, 0, sizeof(POINT));
	memset(&m_tMousePrevPos, 0, sizeof(POINT));
	memset(&m_tMouseMove, 0, sizeof(POINT));

	POINT ptMousePos;

	GetCursorPos(&ptMousePos);

	ScreenToClient(m_hWnd, &ptMousePos);

	m_tMousePos.x = (float)ptMousePos.x;
	m_tMousePos.y = (float)ptMousePos.y;
	// 마우스의 위치를 얻어온것을  Position정보에 저장.

	m_tMousePrevPos = m_tMousePos;
	// 이전 마우스의 위치로 준다.

	CreateMouse();

	return true;
}

void CInput::Update(float fTime)
{
	// 마우스 업데이트.
	MouseUpdate(fTime);



	if (m_bIsScreen)
	{
		// NormalKey.
		NormalKeyInput();

		// 모든 키에 대한 처리를 해주고,


		if (!GET_SINGLE(CGameManager)->GetPause())
		{
			SKeyInput();
			KeyInput();
			AxisInput(fTime);
			ActionInput(fTime);
		}
		// 해당 정보를 바탕으로 Action정보와 Axis정보를 인식한다.
	}
}

void CInput::Render(HDC hDC, float fTime)
{
	// 마우스를 출력해준다.
	m_pMouse->Render(hDC, fTime);
}

void CInput::SKeyInput()
{
	// 특수키에 대한 입력을 확인하는 함수이다.

	for (int i = 0; i < SKEY_END; ++i)
	{
		if (GetAsyncKeyState(m_tSKey[i].cKey) & 0x8000)
		{
			// 특수키를 눌렀다면...
			// 키를 누른 정보를 확인한다.
			if (!m_tSKey[i].bDown && !m_tSKey[i].bHold)
			{
				// 이전에 누른 흔적이 아예 없는 경우. 처음 누른 것이다.
				m_tSKey[i].bDown = true;
				m_tSKey[i].bHold = true;
			}
			else if (m_tSKey[i].bDown)
			{
				// 기존에 Down이 true인 경우에는 누르고 있는 중인 것이다.
				m_tSKey[i].bDown = false;
				// Down에 대한 처리를 한번만 해주기 위해서 Down을 false로 준다.
				// 이 상황에서 계속 누르고 있는 경우에는, 아무 처리도 안 하고, 빠져나간다.
				// 이것은 bHold인 상황을 유지하는 것이다. (누르는 중)
			}
			
		}
		else
		{
			// 누르지 않았다면,

			if (m_tSKey[i].bDown || m_tSKey[i].bHold)
			{
				// 지금은 누르지 않았지만 이전에 누른 흔적이 있다는 것은 방금 뗀 경우이다.
				m_tSKey[i].bUp = true;
				m_tSKey[i].bHold = false;
				m_tSKey[i].bDown = false;
			}
			else if (m_tSKey[i].bUp)
				m_tSKey[i].bUp = false;
			// Up 상태를 처리한 이후에는 false로 만들어준다.
			
			// 이곳으로 들어온다면, 이전에 누른 흔적이 없고, 지금도 누르지 않은 경우에는 아무것도 아니다.
		}
	}

	// 이렇게 특수키에 대한 정보를 처리한다.
}

void CInput::KeyInput()
{
	// 일반키 256개에 대한 입력처리를 한다.
	
	for (int i = 0; i < 256; ++i)
	{
		if (!m_KeyList[i].bEnable)
			continue;
		// 만약, 해당 일반키에 활성화되지 않았다면, (해당 키에 대한 어떠한 입력도 필요로 하지 않는 경우)
		// 해당 키를 처리하지 않고 넘긴다.

		// 처리해야 하는 키인 경우에는 해당키에 대한 입력처리를 한다.


		if (GetAsyncKeyState(m_KeyList[i].cKey) & 0x8000)
		{
			if (!m_KeyList[i].bDown && !m_KeyList[i].bHold)
			{
				m_KeyList[i].bDown = true;
				m_KeyList[i].bHold = true;
			}
			else if (m_KeyList[i].bDown)
			{
				m_KeyList[i].bDown = false;
			}

		}
		else
		{

			if (m_KeyList[i].bDown || m_KeyList[i].bHold)
			{
				m_KeyList[i].bUp = true;
				m_KeyList[i].bHold = false;
				m_KeyList[i].bDown = false;
			}
			else if (m_KeyList[i].bUp)
				m_KeyList[i].bUp = false;
		}

	}
}

void CInput::AxisInput(float fTime)
{
	// AxisInfo에 대한 입력처리를 한다
	unordered_map<string, PBindAxisInfo>::iterator iter;
	unordered_map<string, PBindAxisInfo>::iterator iterEnd = m_mapBindAxis.end();

	for (iter = m_mapBindAxis.begin(); iter != iterEnd; ++iter)
	{
		// BindAxisInfo정보는 여러 키에 대한 묶음에 대한 정보이다.
		// 해당 묶음을 하나씩 처리하여 조합을 만족하는 경우에 등록된 함수를 모두 호출하는 개념이다.
		// A와 D키로 각각 좌 우로 이동하는 함수가 등록이 되어있다면,
		// A와 D키를 누른 경우에 이 함수가 각각 호출되는 것이다.

		// 아무튼, AxisKey List를 돌려줘야한다.

		list<AxisInfo>::iterator iterKey;
		list<AxisInfo>::iterator iterKeyEnd = iter->second->KeyList.end();

		for (iterKey = iter->second->KeyList.begin(); iterKey != iterKeyEnd; ++iterKey)
		{
			// 모든 키 정보에 대해서 하나씩 돌려준다.

			// 이러한 키 묶음은 일반키와 특수키로 이루어져있는데, 일단 특수키에 대한 정보부터 처리한다.

			// iterKey가 가리키는 정보 -> AxisInfo
			// (*IterKey).tKey.tSkey[i] 를 통해서 일반키에 접근할 수 있다.
			// 여기서 생각해볼 점은, 이미 SKeyInput함수와 KeyInput함수를 통해서 미리 해당 키에 대한 입력처리를 해줬다는
			// 점이다. 그러니까 우리는 이 map에 있는 정보를 갱신만 하면 된다.

			
			bool bSKey = true;

			for (int i = 0; i < SKEY_END; ++i)
			{
				// 일단 해당 키를 눌러야 하는지에 대한 여부를 판단한다.
				if ((*iterKey).tKey.bSKey[i])
				{
					// 키를 눌러야 한다.
					if (!m_tSKey[i].bHold)
					{
						bSKey = false;
						break;
					}
				}
				else
				{
					// 눌러서는 안 된다.
					if (m_tSKey[i].bHold)
					{
						bSKey = false;
						break;
					}
				}
			}

			if (!bSKey)
				continue;
			// 특수키에서 틀렸다면, 이미 해당 AxisInfo를 통해서 함수를 호출할 가능성은 없다.
			// 따라서 다음으로 넘긴다.
			// 이후에 특수키를 제대로 입력한 경우에, 일반키의 상태정보를 바탕으로 함수를 호출하면 문제없다.

			// 일반키에 대한 처리이다.
			// 해당 일반키의 상태를 m_KeyList에서 가져온다.
			(*iterKey).tKey.bDown = m_KeyList[(*iterKey).tKey.cKey].bDown;
			(*iterKey).tKey.bHold = m_KeyList[(*iterKey).tKey.cKey].bHold;
			(*iterKey).tKey.bUp = m_KeyList[(*iterKey).tKey.cKey].bUp;

			// 특수키의 경우. 만약 눌러야 하는 키를 제대로 누르지 않은 경우.
			// 누르지 않아야 하는 키를 누른 경우를 따져서, 맞지 않는다면, false값이 들어가 있을 것이다.

			// 반면에 모든 특수키를 제대로 입력을 했다면, false를 주지 않았을 것이다.
			// 이런 상황에서 false인 경우에는 처리를 안 하면 되는 것이다(특수키에 대한 조합이 틀렸으므로)
			// true 인 경우에는 처리하면 된다.

			// * 일반키는 map에 있는 키 정보를 직접적으로 갱신해서(일반키에 대한 상태정보) 사용하고 있고,
			// 특수키에 대해서는 단지 해당 키에대한 사용여부만을 가지고 있도록 하여,
			// 실질적인 특수키에 대한 처리는 SKeyInput에게 맡기고 있다.

			// 아무튼 이렇게 일반키와 특수키에 대한 처리를 마치고 나서는, 함수를 호출해준다.

			// 위에서 특수키를 제대로 입력하지 않은 경우 넘겨주고 있기 때문에 특수키를 제대로 입력한 경우에만 들어온다.

			list<KeyFunction>::iterator iterFunc;
			list<KeyFunction>::iterator iterFuncEnd = iter->second->FunctionList.end();

			// 특수키를 제대로 눌렀고, Down상태 Hold상태, Up 상태에서 함수를 호출하는 경우가 있는데,
			// 3가지에서 함수를 호출하는 경우는 제외하고, 어느 한 상태에서 계속 함수를 호출해주는 식으로 해야 한다.
			// 예를 들어서 S ctrl키라고 하면, ctrl키와 S키를 계속 누르고 있는 경우에 함수 호출이 계속 된다.

			if ((*iterKey).tKey.bDown)
			{
			}

			if ((*iterKey).tKey.bHold)
			{
				for (iterFunc = iter->second->FunctionList.begin(); iterFunc != iterFuncEnd; ++iterFunc)
				{
					// 첫번째 인자로, 해당 AxisInfo의 scale정보를 넣어주고, 두번째 인자로 fTime을 넣어준다.
					(*iterFunc).func((*iterKey).fScale, fTime);

				}

			}

			if ((*iterKey).tKey.bUp)
			{
			}

			// 키 상태를 확인하고, 특수키를 입력했고, 일반키에 대한 상태 정보가 일치하는 경우에 등록된 모든 함수를 호출해준다.
			// 이렇게 처리를 하고 다음 AxisInfo정보를 확인하는 것이다.
		}
	}
}

void CInput::ActionInput(float fTime)
{
	// BindActionInfo 정보를 순회하면서, Action키를 눌렀는지 판단하는 함수이다.

	unordered_map<string, PBindActionInfo>::iterator iter;
	unordered_map<string, PBindActionInfo>::iterator iterEnd = m_mapBindAction.end();

	for (iter = m_mapBindAction.begin(); iter != iterEnd; ++iter)
	{
		// BindAxisInfo와 동일하게,  BindActionInfo가 가지고 있는,
		// KeyInfo List를 순회한다.

		list<KeyInfo>::iterator iterKey;
		list<KeyInfo>::iterator iterKeyEnd = iter->second->KeyList.end();

		for (iterKey = iter->second->KeyList.begin(); iterKey != iterKeyEnd; ++iterKey)
		{
			// BindAxisInfo와 다른 점을 생각해보자.
			// BindActionInfo에서는 INPUT_EVENT 라는 값이 존재하여,
			// 우리가 이 값을 기준을 함수를 호출해 주어야 한다는 것이다.
			// 예를 들어 S + ALT 키 DoubleClick이면, 해당 키를 alt는 계속 누르고 있고,
			// s키를 두 번 눌러도 인식을 하게 된다.
			// ( 특수키는 누르고 있는지만 판단하기 때문이다.)
			// 혹은 S + ALT키를 두번 연속으로 누른다고 하더라도,
			// 첫 스타트 이후의 시간동안 굉장히 많이 함수를 호출하기 때문에
			// 더블클릭으로 인식이 될 것이다.

			// 특수키에 대한 처리를 한다.

			bool bSKey = true;

			for (int i = 0; i < SKEY_END; ++i)
			{
				// 해당 키를 눌러야 하는지 확인한다.
				if ((*iterKey).bSKey[i])
				{
					// 키를 눌러야한다.

					if (!m_tSKey[i].bHold)
					{
						bSKey = false;
						break;
					}
				}
				else
				{
					// 키를 눌러서는 안 된다.
					if (m_tSKey[i].bHold)
					{
						bSKey = false;
						break;
					}
				}
			}

			// 키를 눌러서는 안 되는데 누른 경우, 눌러야 하는데 누르지 않은 경우는 false값이 들어올 것이다.
			// 반면 특수키에 대한 조건을 모두 잘 지킨 경우에는 true가 된다.
			// true가 아닌 경우에는 함수호출을 하지 않는다.

			if (!bSKey)
				continue;

			// 특수키를 제대로 입력한 경우에는 일반키를 업데이트해준다.

			(*iterKey).bDown = m_KeyList[(*iterKey).cKey].bDown;
			(*iterKey).bHold = m_KeyList[(*iterKey).cKey].bHold;
			(*iterKey).bUp = m_KeyList[(*iterKey).cKey].bUp;

			// 일반키와 특수키를 받은 이후에는 INPUT_EVENT에 따른 처리를 해준다.

			// 여기까지 내려왔다는 소리는 특수키를 제대로 입력했다는 의미이다.

			list<ActionKeyFunction>::iterator iterFunc;
			list<ActionKeyFunction>::iterator iterFuncEnd = iter->second->FunctionList.end();


			for (iterFunc = iter->second->FunctionList.begin(); iterFunc != iterFuncEnd; ++iterFunc)
			{

				switch ((*iterFunc).eEvent)
				{
				case IE_PRESS:
					if ((*iterKey).bDown)
					{
						// 특수키를 제대로 입력했고, 해당 일반키를 누른 상태라면 함수를 호출해준다.

							(*iterFunc).func(fTime);
					}
					break;

				case IE_HOLD:
					if ((*iterKey).bHold)
					{
						// 특수키를 제대로 입력했고, 해당 일반키를 누르고 있는 상태라면 함수를 호출해준다.
							(*iterFunc).func(fTime);
					}
					break;

				case IE_UP:
					if ((*iterKey).bUp)
					{
						// 특수키를 제대로 입력했고, 해당 일반키를 뗀 상태라면 함수를 호출해준다.
							(*iterFunc).func(fTime);
					}
					break;

				case IE_DOUBLE:

					// DoubleClick의 경우에는 처음 입력한 경우를 생각해줘야한다.

					if (iter->second->bDoubleStart)
					{
						iter->second->fDoubleTime += fTime;

						if (iter->second->fDoubleTime > 0.5f)
						{
							//처음 누른 이후로 0.5s가 지났다는 것은.
							//초기 상태로 돌려야 한다는 것을 의미한다.
							iter->second->bDoubleStart = false;
							iter->second->fDoubleTime = 0.f;
						}
					}

					// 우리는 처음 클릭한 순간을 기준으로 얼마만큼의 시간이 흘렀는지 판단하여,
					// 0.5s 이전에 다시 입력한 경우에는 함수를 호출해주고
					// 그게 아닌 경우에는 처음 입력한 것으로 처리할 것이다.
					// 즉, 이 과정에서 0.5s가 지난 경우에는 초기 상태로 돌아가고,
					// 이후에 (0.5s가 지나서) 클릭을 하므로 처음 입력을 한 상태가 되는 것이다.

					if ((*iterKey).bDown)
					{
						// 키를 입력한 경우.

						if (!iter->second->bDoubleStart)
						{
							// 처음 해당키를 입력한 경우.
							iter->second->bDoubleStart = true;
							iter->second->fDoubleTime = 0.f;
							// 누른 이후에 시간을 정확하게 .
						}
						else
						{
							// 처음 누른 경우가 아닌 경우에는 함수를 호출해준다.
							// 0.5초가 지나서 이곳으로 들어온 경우와 그렇지 않은 경우가 있는데
							// 그렇지 않은 경우에는 정상적인 처리이다.
							// 0.5이후에 들어온 경우는 있을 수 없다.
							// 왜냐면 0.5s가 지난 경우에는 초기상태로 돌려버리기 때문이다.
							// 따라서 이후에 click을 했다면, doubleStart가 false라서 처음 입력한 상태로 인식한다.

								(*iterFunc).func(fTime);

							// 더블클릭을 통해서 함수를 호출했으니 다시 초기상태로 돌린다.
							iter->second->bDoubleStart = false;
							iter->second->fDoubleTime = 0.f;
						}
					}
					break;
				}

				// 이렇게하여, 기본키와 특수키를 제대로 입력하고, Event에 맞는 경우 함수를 호출해주고 있다.
				// BindActionInfo에 있는 모든 KeyInfo를 순회하면서 해당 키를 누르고 + Event에 맞다면,
				// 함수를 호출해준다.
				// 이렇게 모든 KeyInfo를 처리하고서, 다음 BindActionInfo를 처리한다.
			}
		}
	}
}

// AxisInfo 정보를 추가하는 함수이다.
// 첫 인자로 BindAxisInfo의 이름을 넣어주고 있고, 일반키와 특수키의 배열, fScale값을 받아주고 있다.
// KeyInfo의 변수로는 단순히 키 정보만을 입력받으면 되는 상황이고, 여기에 fScale값이 추가.
// BindAxisInfo 정보를 찾아오기 위한 strName

void CInput::MouseUpdate(float fTime)
{
	m_bIsScreen = true;
	m_tMousePrevPos = m_tMousePos;

	// 현재 위치를 받아서 셋팅한다.

	POINT tPoint;
	GetCursorPos(&tPoint);
	ScreenToClient(m_hWnd, &tPoint);

	m_tMousePos = tPoint;

	if (m_tMousePos.x < 0.f || m_tMousePos.x > RS_WIDTH ||
		m_tMousePos.y < 0.f || m_tMousePos.y > RS_HEIGHT)
		m_bIsScreen = false;

	m_tMouseMove = m_tMousePos - m_tMousePrevPos;

	// 이렇게 구한 좌표를 마우스에게 셋팅한다(이렇게 해야 Render를 통해서, 해당 위치에 출력이 된다.)
	m_pMouse->SetPos(m_tMousePos);

	// Layer에 소속된 오브젝트가 아니므로, 따로 함수를 돌려줘야 한다.
	m_pMouse->Update(fTime);
	m_pMouse->LateUpdate(fTime);
}

void CInput::NormalKeyInput()
{
	// 기본키에 대한 입력 처리이다.

	unordered_map<string, PNormalKeyInfo>::iterator iter;
	unordered_map<string, PNormalKeyInfo>::iterator iterEnd = m_mapNormalKey.end();
	
	for (iter = m_mapNormalKey.begin(); iter != iterEnd; ++iter)
	{
		// 일단, 해당 키를 눌렀는지 확인한다.
		// 단순히 키를 GetAsncKeyState함수를 이용해서 입력을 했는지 확인하고,
		// 해당 키를 모두 눌렀는지에 대한 처리를 누른 키의 수를 세서 체크하는 것이다.
		// ctrl a 키와 ctrl키가 있는 경우에 ctrl a 키를 누르면 ctrl에 해당하는 함수도 호출이 되는 구조이다.

		int iCount = 0;

		for (size_t i = 0; i < iter->second->vecKey.size(); ++i)
		{
			if (GetAsyncKeyState(iter->second->vecKey[i]) & 0x8000)
				++iCount;
		}

		
		if (iter->second->vecKey.size() == iCount)
		{
			// 키를 입력한 경우.

			// 처음 키를 입력했는지 판단한다.

			if (!iter->second->bDown && !iter->second->bPress)
			{
				// 처음 키를 누른 경우.
				iter->second->bDown = true;
				iter->second->bPress = true;
			}

			else if (iter->second->bDown)
			{
				// 키를 누르고 있는 경우이다.
				iter->second->bDown = false;
			}

		}

		else
		{
			// 키를 입력하지 않은 경우.

			// 이전에 키를 누른 흔적이 있는지 판단한다.
			if (iter->second->bDown || iter->second->bPress)
			{
				// 이전 프레임까지 누르다가 지금 막 키를 뗀 것이다.
				iter->second->bDown = false;
				iter->second->bPress = false;
				iter->second->bUp = true;
			}
			else if (iter->second->bUp)
				iter->second->bUp = false;
		}
	}
}

void CInput::CreateMouse()
{
	m_pMouse = CObj::CreateObj<CUIImage>("Mouse", "", nullptr);
	// 어느 위치에도 소속되지 않은 MouseObject생성.

	// 애니메이션 생성후 클립추가, 텍스처 추가, 좌표셋팅 컬러키까지...
	m_pMouse->CreateAnimation("MouseAnimation");
	m_pMouseAnimation = m_pMouse->GetAnimation();

	m_pMouseAnimation->AddRef();

	m_pMouseAnimation->AddClip("MouseDefault", AO_LOOP, 1.f);


	vector<const TCHAR*>	vecFileName;
	for (int i = 0; i <= 10; ++i)
	{
		TCHAR*	pFileName = new TCHAR[MAX_PATH];

		memset(pFileName, 0, sizeof(TCHAR) * MAX_PATH);

		wsprintf(pFileName, TEXT("Mouse/%d.bmp"), i);

		vecFileName.push_back(pFileName);
	}

	ClipCoord	tCoord = {};
	tCoord.tEnd.x = 32.f;
	tCoord.tEnd.y = 31.f;


	m_pMouseAnimation->AddClipCoord("MouseDefault", tCoord, 11);

	m_pMouseAnimation->SetClipTexture("MouseDefault", "MouseDefault",
		vecFileName);

	Safe_Delete_Array_VecList(vecFileName);

	m_pMouseAnimation->SetTextureColorKey("MouseDefault", 255, 0, 255);


	// 마우스 충돌체 추가
	m_pMouse->SetEnable(true);
	m_pMouseCollider = m_pMouse->AddCollider<CColliderPoint>("MouseUI");


	// 충돌 관계 설정.
	m_pMouseCollider->SetChannel("Mouse");
	m_pMouseCollider->SetProfile("MouseProfile");

	m_pMouse->Start();
	
}

void CInput::AddAxisKey(const string & strName, unsigned char cKey, float fScale, int iSKey)
{
	// 일단, 해당 이름을 가진 BindAxisInfo 정보가 있는지 확인한다.
	// 키를 추가하는 과정에서 해당 이름을 가진 BindAxisInfo정보가 없다면 새로 생성해준다.
	// Action키도 마찬가지이다.

	// 즉, 키를 추가하는 과정에서 새로운 BindAxisInfo, BindActionInfo를 추가하게 되고,
	// 나중에 이 정보를 string 을 키 값으로 찾아와서, FunctionList에 함수를 등록하는 개념이다.

	// 따라서 초기화의 순서는 AddKey -> AddFunction 순서이다.

	PBindAxisInfo pAxis = FindAxis(strName);

	if (!pAxis)
	{
		// BindAxisInfo 정보가 없다면 하나 생성한다.
		pAxis = new BindAxisInfo;
		// 생성한 BindAxisInfo정보를 map에 넣어준다.
		pAxis->strName = strName;
		// 키는 나중에 추가할 것이고, FunctionList는 건들일 필요가 없다.
		m_mapBindAxis.insert(make_pair(strName, pAxis));
		// strName 을 키값으로 BindAxisInfo 정보를 넣어준다.
	}

	// 새로운 키를 추가하는 과정에서 이전에 추가된 일반키인지 확인해준다.

	list<AxisInfo>::iterator iter;
	list<AxisInfo>::iterator iterEnd = pAxis->KeyList.end();

	for (iter = pAxis->KeyList.begin(); iter != iterEnd; ++iter)
	{
		if ((*iter).tKey.cKey == cKey)
			return;
		// 해당 BindAxisInfo 정보를 순회하면서 이미 추가가 된 일반키를 다시 추가하려고 한 경우라면 return;
	}

	// 없으면 새로 만들고 있으면 있는 것을 받아온다.

	// 새로운 키를 추가한다.

	AxisInfo tInfo = {};

	tInfo.fScale = fScale;
	tInfo.tKey.cKey = cKey;
	
	// 예를 들어 SKey로 ST_CTRL | ST_ALT 이런 식으로 들어올 것인데,
	// 이러면, 아래 부분에서 CTRL 부분과 ALT 부분을 & 연산 했을때 TRUE 가 들어올 것이다.
	// 따라서 ALT키와 CTRL키를 추가한다.

	if (iSKey & ST_CTRL)
		tInfo.tKey.bSKey[SKEY_CTRL] = true;


	if (iSKey & ST_ALT)
		tInfo.tKey.bSKey[SKEY_ALT] = true;


	if (iSKey & ST_SHIFT)
		tInfo.tKey.bSKey[SKEY_SHIFT] = true;

	// 설정하지 않으면 FALSE

	// 이렇게 하여, AxisInfo 정보를 채워주고, 해당 정보를 BindAxisInfo에 추가한다.

	pAxis->KeyList.push_back(tInfo);

	// 해당 키에 대해서 활성화를 해준다.
	m_KeyList[cKey].bEnable = true;
	
}

void CInput::AddActionKey(const string & strName, unsigned char cKey, int iSKey)
{
	/*
	string		strName;
	INPUT_EVENT	eEvent;
	bool		bDoubleStart;
	float		fDoubleTime;
	list<KeyInfo>	KeyList;
	list<ActionKeyFunction>	FunctionList;
	*/

	// eEvent는 함수를 추가할 때 넣어주고 있고, bDoubleStart와 fDoubleTime은 여기서 이용하는 변수가 아니다.
	// FunctionList도 생각할 필요가 없다.
	// KeyList에 추가할 정보. 특수키와 일반키의 배열 정보를 통해서 KeyInfo를 추가하고,
	// strName을 이용해서 추가할 BindActionInfo를 받아온다.


	PBindActionInfo pAction = FindAction(strName);

	if (!pAction)
	{
		pAction = new BindActionInfo;
		pAction->strName = strName;
		m_mapBindAction.insert(make_pair(strName, pAction));
	}

	// 새로운 키를 추가하는 과정에서 이전에 추가된 일반키인지 확인해준다.

	list<KeyInfo>::iterator iter;
	list<KeyInfo>::iterator iterEnd = pAction->KeyList.end();

	for (iter = pAction->KeyList.begin(); iter != iterEnd; ++iter)
	{
		if ((*iter).cKey == cKey)
			return;
	}

	KeyInfo tInfo = {};

	tInfo.cKey = cKey;

	if (iSKey & ST_CTRL)
		tInfo.bSKey[SKEY_CTRL] = true;


	if (iSKey & ST_ALT)
		tInfo.bSKey[SKEY_ALT] = true;


	if (iSKey & ST_SHIFT)
		tInfo.bSKey[SKEY_SHIFT] = true;

	// 설정하지 않으면 FALSE

	// 이렇게 하여, AxisInfo 정보를 채워주고, 해당 정보를 BindAxisInfo에 추가한다.

	pAction->KeyList.push_back(tInfo);

	// 해당 키에 대해서 활성화를 해준다.
	m_KeyList[cKey].bEnable = true;
}

PNormalKeyInfo CInput::FindNormalKey(const string strName)
{
	unordered_map<string, PNormalKeyInfo>::iterator iter = m_mapNormalKey.find(strName);

	if (iter == m_mapNormalKey.end())
		return nullptr;

	return iter->second;
}

bool CInput::NormalKeyDown(const string & strName)
{
	PNormalKeyInfo pKey = FindNormalKey(strName);

	return pKey->bDown;
}

bool CInput::NormalKeyPress(const string & strName)
{
	PNormalKeyInfo pKey = FindNormalKey(strName);

	return pKey->bPress;
}

bool CInput::NormalKeyUp(const string & strName)
{
	PNormalKeyInfo pKey = FindNormalKey(strName);

	return pKey->bUp;
}

PBindAxisInfo CInput::FindAxis(const string & strName)
{
	unordered_map<string, PBindAxisInfo>::iterator iter = m_mapBindAxis.find(strName);

	if (iter == m_mapBindAxis.end())
		return nullptr;

	return iter->second;
}

PBindActionInfo CInput::FindAction(const string & strName)
{
	unordered_map<string, PBindActionInfo>::iterator iter = m_mapBindAction.find(strName);

	if (iter == m_mapBindAction.end())
		return nullptr;

	return iter->second;
}

bool CInput::DeleteBindAxis(const string & strName, void * pObj, const string & strFuncName)
{
	// 일단 strName의 값을 가진 BindAxisInfo를 찾아온다.

	PBindAxisInfo pAxis = FindAxis(strName);

	if (!pAxis)
		return false;
	// 해당 이름을 가진 BindAxisInfo가 없다면 함수를 제거할 수 없다.

	// 있다면 조건을 돌려서 찾아준다.

	list<KeyFunction>::iterator iter;
	list<KeyFunction>::iterator iterEnd = pAxis->FunctionList.end();

	for (iter = pAxis->FunctionList.begin(); iter != iterEnd; ++iter)
	{
		if ((*iter).pObj == pObj && (*iter).strFuncName == strFuncName)
		{
			// 오브젝트가 같고, 함수이름이 동일하다면 해당 함수를 찾은 것이다.
			pAxis->FunctionList.erase(iter);
			return true;

			// 해당 함수를 지워버리고 return true;
		}
	}

	// 여기 까지 내려왔다는 소리는 해당 조건의 함수를 찾지 못했다는 것이다.
	// return false;
	return false;
}

bool CInput::DeleteBindAction(const string & strName, void * pObj, const string & strFuncName)
{

	PBindActionInfo pAction = FindAction(strName);

	if (!pAction)
		return false;

	list<ActionKeyFunction>::iterator iter;
	list<ActionKeyFunction>::iterator iterEnd = pAction->FunctionList.end();

	for (iter = pAction->FunctionList.begin(); iter != iterEnd; ++iter)
	{
		if ((*iter).pObj == pObj && (*iter).strFuncName == strFuncName)
		{
			pAction->FunctionList.erase(iter);
			return true;
		}
	}

	return false;
}
