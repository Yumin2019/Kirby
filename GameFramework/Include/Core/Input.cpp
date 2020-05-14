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


	// �����Ҵ��� �ؼ� �߰��ص� map������ ������.
	Safe_Delete_Map(m_mapBindAxis);
	Safe_Delete_Map(m_mapBindAction);
	

	// BindActionInfo BindAxisInfo�� �����ʹ� heap�޸𸮸� ������� �ʰ� �ִ�.
	// �� �� KeyList, InputKey�� ���ÿ� ������ �޸𸮴�.
	// ���� map�� ����ָ� �ȴ�.

	Safe_Delete_Map(m_mapNormalKey);
}

bool CInput::Init(HWND hWnd)
{
	// ���⼭��, Ű���带 ���� �Է��� �� �ִ� �⺻Ű�� ���ؼ�, �߰����ְ� �׿� Ư��Ű
	// SHIFT, ALT, CTRL�� ���� ������ �߰����ش�.

	m_hWnd = hWnd;

	m_KeyList.resize(256); 

	for (int i = 0; i < 256; ++i)
	{
		m_KeyList[i].cKey = (unsigned char)i;
		// Enable �ǵ����� ����, ���� ���� �ǵ����� ����, �ܼ��� �ش� Ű�� �־��ش�.
	}

	// Ư��Ű�� ���� ������ �߰��Ѵ�.

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
	// ���콺�� ��ġ�� ���°���  Position������ ����.

	m_tMousePrevPos = m_tMousePos;
	// ���� ���콺�� ��ġ�� �ش�.

	CreateMouse();

	return true;
}

void CInput::Update(float fTime)
{
	// ���콺 ������Ʈ.
	MouseUpdate(fTime);



	if (m_bIsScreen)
	{
		// NormalKey.
		NormalKeyInput();

		// ��� Ű�� ���� ó���� ���ְ�,


		if (!GET_SINGLE(CGameManager)->GetPause())
		{
			SKeyInput();
			KeyInput();
			AxisInput(fTime);
			ActionInput(fTime);
		}
		// �ش� ������ �������� Action������ Axis������ �ν��Ѵ�.
	}
}

void CInput::Render(HDC hDC, float fTime)
{
	// ���콺�� ������ش�.
	m_pMouse->Render(hDC, fTime);
}

void CInput::SKeyInput()
{
	// Ư��Ű�� ���� �Է��� Ȯ���ϴ� �Լ��̴�.

	for (int i = 0; i < SKEY_END; ++i)
	{
		if (GetAsyncKeyState(m_tSKey[i].cKey) & 0x8000)
		{
			// Ư��Ű�� �����ٸ�...
			// Ű�� ���� ������ Ȯ���Ѵ�.
			if (!m_tSKey[i].bDown && !m_tSKey[i].bHold)
			{
				// ������ ���� ������ �ƿ� ���� ���. ó�� ���� ���̴�.
				m_tSKey[i].bDown = true;
				m_tSKey[i].bHold = true;
			}
			else if (m_tSKey[i].bDown)
			{
				// ������ Down�� true�� ��쿡�� ������ �ִ� ���� ���̴�.
				m_tSKey[i].bDown = false;
				// Down�� ���� ó���� �ѹ��� ���ֱ� ���ؼ� Down�� false�� �ش�.
				// �� ��Ȳ���� ��� ������ �ִ� ��쿡��, �ƹ� ó���� �� �ϰ�, ����������.
				// �̰��� bHold�� ��Ȳ�� �����ϴ� ���̴�. (������ ��)
			}
			
		}
		else
		{
			// ������ �ʾҴٸ�,

			if (m_tSKey[i].bDown || m_tSKey[i].bHold)
			{
				// ������ ������ �ʾ����� ������ ���� ������ �ִٴ� ���� ��� �� ����̴�.
				m_tSKey[i].bUp = true;
				m_tSKey[i].bHold = false;
				m_tSKey[i].bDown = false;
			}
			else if (m_tSKey[i].bUp)
				m_tSKey[i].bUp = false;
			// Up ���¸� ó���� ���Ŀ��� false�� ������ش�.
			
			// �̰����� ���´ٸ�, ������ ���� ������ ����, ���ݵ� ������ ���� ��쿡�� �ƹ��͵� �ƴϴ�.
		}
	}

	// �̷��� Ư��Ű�� ���� ������ ó���Ѵ�.
}

void CInput::KeyInput()
{
	// �Ϲ�Ű 256���� ���� �Է�ó���� �Ѵ�.
	
	for (int i = 0; i < 256; ++i)
	{
		if (!m_KeyList[i].bEnable)
			continue;
		// ����, �ش� �Ϲ�Ű�� Ȱ��ȭ���� �ʾҴٸ�, (�ش� Ű�� ���� ��� �Էµ� �ʿ�� ���� �ʴ� ���)
		// �ش� Ű�� ó������ �ʰ� �ѱ��.

		// ó���ؾ� �ϴ� Ű�� ��쿡�� �ش�Ű�� ���� �Է�ó���� �Ѵ�.


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
	// AxisInfo�� ���� �Է�ó���� �Ѵ�
	unordered_map<string, PBindAxisInfo>::iterator iter;
	unordered_map<string, PBindAxisInfo>::iterator iterEnd = m_mapBindAxis.end();

	for (iter = m_mapBindAxis.begin(); iter != iterEnd; ++iter)
	{
		// BindAxisInfo������ ���� Ű�� ���� ������ ���� �����̴�.
		// �ش� ������ �ϳ��� ó���Ͽ� ������ �����ϴ� ��쿡 ��ϵ� �Լ��� ��� ȣ���ϴ� �����̴�.
		// A�� DŰ�� ���� �� ��� �̵��ϴ� �Լ��� ����� �Ǿ��ִٸ�,
		// A�� DŰ�� ���� ��쿡 �� �Լ��� ���� ȣ��Ǵ� ���̴�.

		// �ƹ�ư, AxisKey List�� ��������Ѵ�.

		list<AxisInfo>::iterator iterKey;
		list<AxisInfo>::iterator iterKeyEnd = iter->second->KeyList.end();

		for (iterKey = iter->second->KeyList.begin(); iterKey != iterKeyEnd; ++iterKey)
		{
			// ��� Ű ������ ���ؼ� �ϳ��� �����ش�.

			// �̷��� Ű ������ �Ϲ�Ű�� Ư��Ű�� �̷�����ִµ�, �ϴ� Ư��Ű�� ���� �������� ó���Ѵ�.

			// iterKey�� ����Ű�� ���� -> AxisInfo
			// (*IterKey).tKey.tSkey[i] �� ���ؼ� �Ϲ�Ű�� ������ �� �ִ�.
			// ���⼭ �����غ� ����, �̹� SKeyInput�Լ��� KeyInput�Լ��� ���ؼ� �̸� �ش� Ű�� ���� �Է�ó���� ����ٴ�
			// ���̴�. �׷��ϱ� �츮�� �� map�� �ִ� ������ ���Ÿ� �ϸ� �ȴ�.

			
			bool bSKey = true;

			for (int i = 0; i < SKEY_END; ++i)
			{
				// �ϴ� �ش� Ű�� ������ �ϴ����� ���� ���θ� �Ǵ��Ѵ�.
				if ((*iterKey).tKey.bSKey[i])
				{
					// Ű�� ������ �Ѵ�.
					if (!m_tSKey[i].bHold)
					{
						bSKey = false;
						break;
					}
				}
				else
				{
					// �������� �� �ȴ�.
					if (m_tSKey[i].bHold)
					{
						bSKey = false;
						break;
					}
				}
			}

			if (!bSKey)
				continue;
			// Ư��Ű���� Ʋ�ȴٸ�, �̹� �ش� AxisInfo�� ���ؼ� �Լ��� ȣ���� ���ɼ��� ����.
			// ���� �������� �ѱ��.
			// ���Ŀ� Ư��Ű�� ����� �Է��� ��쿡, �Ϲ�Ű�� ���������� �������� �Լ��� ȣ���ϸ� ��������.

			// �Ϲ�Ű�� ���� ó���̴�.
			// �ش� �Ϲ�Ű�� ���¸� m_KeyList���� �����´�.
			(*iterKey).tKey.bDown = m_KeyList[(*iterKey).tKey.cKey].bDown;
			(*iterKey).tKey.bHold = m_KeyList[(*iterKey).tKey.cKey].bHold;
			(*iterKey).tKey.bUp = m_KeyList[(*iterKey).tKey.cKey].bUp;

			// Ư��Ű�� ���. ���� ������ �ϴ� Ű�� ����� ������ ���� ���.
			// ������ �ʾƾ� �ϴ� Ű�� ���� ��츦 ������, ���� �ʴ´ٸ�, false���� �� ���� ���̴�.

			// �ݸ鿡 ��� Ư��Ű�� ����� �Է��� �ߴٸ�, false�� ���� �ʾ��� ���̴�.
			// �̷� ��Ȳ���� false�� ��쿡�� ó���� �� �ϸ� �Ǵ� ���̴�(Ư��Ű�� ���� ������ Ʋ�����Ƿ�)
			// true �� ��쿡�� ó���ϸ� �ȴ�.

			// * �Ϲ�Ű�� map�� �ִ� Ű ������ ���������� �����ؼ�(�Ϲ�Ű�� ���� ��������) ����ϰ� �ְ�,
			// Ư��Ű�� ���ؼ��� ���� �ش� Ű������ ��뿩�θ��� ������ �ֵ��� �Ͽ�,
			// �������� Ư��Ű�� ���� ó���� SKeyInput���� �ñ�� �ִ�.

			// �ƹ�ư �̷��� �Ϲ�Ű�� Ư��Ű�� ���� ó���� ��ġ�� ������, �Լ��� ȣ�����ش�.

			// ������ Ư��Ű�� ����� �Է����� ���� ��� �Ѱ��ְ� �ֱ� ������ Ư��Ű�� ����� �Է��� ��쿡�� ���´�.

			list<KeyFunction>::iterator iterFunc;
			list<KeyFunction>::iterator iterFuncEnd = iter->second->FunctionList.end();

			// Ư��Ű�� ����� ������, Down���� Hold����, Up ���¿��� �Լ��� ȣ���ϴ� ��찡 �ִµ�,
			// 3�������� �Լ��� ȣ���ϴ� ���� �����ϰ�, ��� �� ���¿��� ��� �Լ��� ȣ�����ִ� ������ �ؾ� �Ѵ�.
			// ���� �� S ctrlŰ��� �ϸ�, ctrlŰ�� SŰ�� ��� ������ �ִ� ��쿡 �Լ� ȣ���� ��� �ȴ�.

			if ((*iterKey).tKey.bDown)
			{
			}

			if ((*iterKey).tKey.bHold)
			{
				for (iterFunc = iter->second->FunctionList.begin(); iterFunc != iterFuncEnd; ++iterFunc)
				{
					// ù��° ���ڷ�, �ش� AxisInfo�� scale������ �־��ְ�, �ι�° ���ڷ� fTime�� �־��ش�.
					(*iterFunc).func((*iterKey).fScale, fTime);

				}

			}

			if ((*iterKey).tKey.bUp)
			{
			}

			// Ű ���¸� Ȯ���ϰ�, Ư��Ű�� �Է��߰�, �Ϲ�Ű�� ���� ���� ������ ��ġ�ϴ� ��쿡 ��ϵ� ��� �Լ��� ȣ�����ش�.
			// �̷��� ó���� �ϰ� ���� AxisInfo������ Ȯ���ϴ� ���̴�.
		}
	}
}

void CInput::ActionInput(float fTime)
{
	// BindActionInfo ������ ��ȸ�ϸ鼭, ActionŰ�� �������� �Ǵ��ϴ� �Լ��̴�.

	unordered_map<string, PBindActionInfo>::iterator iter;
	unordered_map<string, PBindActionInfo>::iterator iterEnd = m_mapBindAction.end();

	for (iter = m_mapBindAction.begin(); iter != iterEnd; ++iter)
	{
		// BindAxisInfo�� �����ϰ�,  BindActionInfo�� ������ �ִ�,
		// KeyInfo List�� ��ȸ�Ѵ�.

		list<KeyInfo>::iterator iterKey;
		list<KeyInfo>::iterator iterKeyEnd = iter->second->KeyList.end();

		for (iterKey = iter->second->KeyList.begin(); iterKey != iterKeyEnd; ++iterKey)
		{
			// BindAxisInfo�� �ٸ� ���� �����غ���.
			// BindActionInfo������ INPUT_EVENT ��� ���� �����Ͽ�,
			// �츮�� �� ���� ������ �Լ��� ȣ���� �־�� �Ѵٴ� ���̴�.
			// ���� ��� S + ALT Ű DoubleClick�̸�, �ش� Ű�� alt�� ��� ������ �ְ�,
			// sŰ�� �� �� ������ �ν��� �ϰ� �ȴ�.
			// ( Ư��Ű�� ������ �ִ����� �Ǵ��ϱ� �����̴�.)
			// Ȥ�� S + ALTŰ�� �ι� �������� �����ٰ� �ϴ���,
			// ù ��ŸƮ ������ �ð����� ������ ���� �Լ��� ȣ���ϱ� ������
			// ����Ŭ������ �ν��� �� ���̴�.

			// Ư��Ű�� ���� ó���� �Ѵ�.

			bool bSKey = true;

			for (int i = 0; i < SKEY_END; ++i)
			{
				// �ش� Ű�� ������ �ϴ��� Ȯ���Ѵ�.
				if ((*iterKey).bSKey[i])
				{
					// Ű�� �������Ѵ�.

					if (!m_tSKey[i].bHold)
					{
						bSKey = false;
						break;
					}
				}
				else
				{
					// Ű�� �������� �� �ȴ�.
					if (m_tSKey[i].bHold)
					{
						bSKey = false;
						break;
					}
				}
			}

			// Ű�� �������� �� �Ǵµ� ���� ���, ������ �ϴµ� ������ ���� ���� false���� ���� ���̴�.
			// �ݸ� Ư��Ű�� ���� ������ ��� �� ��Ų ��쿡�� true�� �ȴ�.
			// true�� �ƴ� ��쿡�� �Լ�ȣ���� ���� �ʴ´�.

			if (!bSKey)
				continue;

			// Ư��Ű�� ����� �Է��� ��쿡�� �Ϲ�Ű�� ������Ʈ���ش�.

			(*iterKey).bDown = m_KeyList[(*iterKey).cKey].bDown;
			(*iterKey).bHold = m_KeyList[(*iterKey).cKey].bHold;
			(*iterKey).bUp = m_KeyList[(*iterKey).cKey].bUp;

			// �Ϲ�Ű�� Ư��Ű�� ���� ���Ŀ��� INPUT_EVENT�� ���� ó���� ���ش�.

			// ������� �����Դٴ� �Ҹ��� Ư��Ű�� ����� �Է��ߴٴ� �ǹ��̴�.

			list<ActionKeyFunction>::iterator iterFunc;
			list<ActionKeyFunction>::iterator iterFuncEnd = iter->second->FunctionList.end();


			for (iterFunc = iter->second->FunctionList.begin(); iterFunc != iterFuncEnd; ++iterFunc)
			{

				switch ((*iterFunc).eEvent)
				{
				case IE_PRESS:
					if ((*iterKey).bDown)
					{
						// Ư��Ű�� ����� �Է��߰�, �ش� �Ϲ�Ű�� ���� ���¶�� �Լ��� ȣ�����ش�.

							(*iterFunc).func(fTime);
					}
					break;

				case IE_HOLD:
					if ((*iterKey).bHold)
					{
						// Ư��Ű�� ����� �Է��߰�, �ش� �Ϲ�Ű�� ������ �ִ� ���¶�� �Լ��� ȣ�����ش�.
							(*iterFunc).func(fTime);
					}
					break;

				case IE_UP:
					if ((*iterKey).bUp)
					{
						// Ư��Ű�� ����� �Է��߰�, �ش� �Ϲ�Ű�� �� ���¶�� �Լ��� ȣ�����ش�.
							(*iterFunc).func(fTime);
					}
					break;

				case IE_DOUBLE:

					// DoubleClick�� ��쿡�� ó�� �Է��� ��츦 ����������Ѵ�.

					if (iter->second->bDoubleStart)
					{
						iter->second->fDoubleTime += fTime;

						if (iter->second->fDoubleTime > 0.5f)
						{
							//ó�� ���� ���ķ� 0.5s�� �����ٴ� ����.
							//�ʱ� ���·� ������ �Ѵٴ� ���� �ǹ��Ѵ�.
							iter->second->bDoubleStart = false;
							iter->second->fDoubleTime = 0.f;
						}
					}

					// �츮�� ó�� Ŭ���� ������ �������� �󸶸�ŭ�� �ð��� �귶���� �Ǵ��Ͽ�,
					// 0.5s ������ �ٽ� �Է��� ��쿡�� �Լ��� ȣ�����ְ�
					// �װ� �ƴ� ��쿡�� ó�� �Է��� ������ ó���� ���̴�.
					// ��, �� �������� 0.5s�� ���� ��쿡�� �ʱ� ���·� ���ư���,
					// ���Ŀ� (0.5s�� ������) Ŭ���� �ϹǷ� ó�� �Է��� �� ���°� �Ǵ� ���̴�.

					if ((*iterKey).bDown)
					{
						// Ű�� �Է��� ���.

						if (!iter->second->bDoubleStart)
						{
							// ó�� �ش�Ű�� �Է��� ���.
							iter->second->bDoubleStart = true;
							iter->second->fDoubleTime = 0.f;
							// ���� ���Ŀ� �ð��� ��Ȯ�ϰ� .
						}
						else
						{
							// ó�� ���� ��찡 �ƴ� ��쿡�� �Լ��� ȣ�����ش�.
							// 0.5�ʰ� ������ �̰����� ���� ���� �׷��� ���� ��찡 �ִµ�
							// �׷��� ���� ��쿡�� �������� ó���̴�.
							// 0.5���Ŀ� ���� ���� ���� �� ����.
							// �ֳĸ� 0.5s�� ���� ��쿡�� �ʱ���·� ���������� �����̴�.
							// ���� ���Ŀ� click�� �ߴٸ�, doubleStart�� false�� ó�� �Է��� ���·� �ν��Ѵ�.

								(*iterFunc).func(fTime);

							// ����Ŭ���� ���ؼ� �Լ��� ȣ�������� �ٽ� �ʱ���·� ������.
							iter->second->bDoubleStart = false;
							iter->second->fDoubleTime = 0.f;
						}
					}
					break;
				}

				// �̷����Ͽ�, �⺻Ű�� Ư��Ű�� ����� �Է��ϰ�, Event�� �´� ��� �Լ��� ȣ�����ְ� �ִ�.
				// BindActionInfo�� �ִ� ��� KeyInfo�� ��ȸ�ϸ鼭 �ش� Ű�� ������ + Event�� �´ٸ�,
				// �Լ��� ȣ�����ش�.
				// �̷��� ��� KeyInfo�� ó���ϰ�, ���� BindActionInfo�� ó���Ѵ�.
			}
		}
	}
}

// AxisInfo ������ �߰��ϴ� �Լ��̴�.
// ù ���ڷ� BindAxisInfo�� �̸��� �־��ְ� �ְ�, �Ϲ�Ű�� Ư��Ű�� �迭, fScale���� �޾��ְ� �ִ�.
// KeyInfo�� �����δ� �ܼ��� Ű �������� �Է¹����� �Ǵ� ��Ȳ�̰�, ���⿡ fScale���� �߰�.
// BindAxisInfo ������ ã�ƿ��� ���� strName

void CInput::MouseUpdate(float fTime)
{
	m_bIsScreen = true;
	m_tMousePrevPos = m_tMousePos;

	// ���� ��ġ�� �޾Ƽ� �����Ѵ�.

	POINT tPoint;
	GetCursorPos(&tPoint);
	ScreenToClient(m_hWnd, &tPoint);

	m_tMousePos = tPoint;

	if (m_tMousePos.x < 0.f || m_tMousePos.x > RS_WIDTH ||
		m_tMousePos.y < 0.f || m_tMousePos.y > RS_HEIGHT)
		m_bIsScreen = false;

	m_tMouseMove = m_tMousePos - m_tMousePrevPos;

	// �̷��� ���� ��ǥ�� ���콺���� �����Ѵ�(�̷��� �ؾ� Render�� ���ؼ�, �ش� ��ġ�� ����� �ȴ�.)
	m_pMouse->SetPos(m_tMousePos);

	// Layer�� �Ҽӵ� ������Ʈ�� �ƴϹǷ�, ���� �Լ��� ������� �Ѵ�.
	m_pMouse->Update(fTime);
	m_pMouse->LateUpdate(fTime);
}

void CInput::NormalKeyInput()
{
	// �⺻Ű�� ���� �Է� ó���̴�.

	unordered_map<string, PNormalKeyInfo>::iterator iter;
	unordered_map<string, PNormalKeyInfo>::iterator iterEnd = m_mapNormalKey.end();
	
	for (iter = m_mapNormalKey.begin(); iter != iterEnd; ++iter)
	{
		// �ϴ�, �ش� Ű�� �������� Ȯ���Ѵ�.
		// �ܼ��� Ű�� GetAsncKeyState�Լ��� �̿��ؼ� �Է��� �ߴ��� Ȯ���ϰ�,
		// �ش� Ű�� ��� ���������� ���� ó���� ���� Ű�� ���� ���� üũ�ϴ� ���̴�.
		// ctrl a Ű�� ctrlŰ�� �ִ� ��쿡 ctrl a Ű�� ������ ctrl�� �ش��ϴ� �Լ��� ȣ���� �Ǵ� �����̴�.

		int iCount = 0;

		for (size_t i = 0; i < iter->second->vecKey.size(); ++i)
		{
			if (GetAsyncKeyState(iter->second->vecKey[i]) & 0x8000)
				++iCount;
		}

		
		if (iter->second->vecKey.size() == iCount)
		{
			// Ű�� �Է��� ���.

			// ó�� Ű�� �Է��ߴ��� �Ǵ��Ѵ�.

			if (!iter->second->bDown && !iter->second->bPress)
			{
				// ó�� Ű�� ���� ���.
				iter->second->bDown = true;
				iter->second->bPress = true;
			}

			else if (iter->second->bDown)
			{
				// Ű�� ������ �ִ� ����̴�.
				iter->second->bDown = false;
			}

		}

		else
		{
			// Ű�� �Է����� ���� ���.

			// ������ Ű�� ���� ������ �ִ��� �Ǵ��Ѵ�.
			if (iter->second->bDown || iter->second->bPress)
			{
				// ���� �����ӱ��� �����ٰ� ���� �� Ű�� �� ���̴�.
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
	// ��� ��ġ���� �Ҽӵ��� ���� MouseObject����.

	// �ִϸ��̼� ������ Ŭ���߰�, �ؽ�ó �߰�, ��ǥ���� �÷�Ű����...
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


	// ���콺 �浹ü �߰�
	m_pMouse->SetEnable(true);
	m_pMouseCollider = m_pMouse->AddCollider<CColliderPoint>("MouseUI");


	// �浹 ���� ����.
	m_pMouseCollider->SetChannel("Mouse");
	m_pMouseCollider->SetProfile("MouseProfile");

	m_pMouse->Start();
	
}

void CInput::AddAxisKey(const string & strName, unsigned char cKey, float fScale, int iSKey)
{
	// �ϴ�, �ش� �̸��� ���� BindAxisInfo ������ �ִ��� Ȯ���Ѵ�.
	// Ű�� �߰��ϴ� �������� �ش� �̸��� ���� BindAxisInfo������ ���ٸ� ���� �������ش�.
	// ActionŰ�� ���������̴�.

	// ��, Ű�� �߰��ϴ� �������� ���ο� BindAxisInfo, BindActionInfo�� �߰��ϰ� �ǰ�,
	// ���߿� �� ������ string �� Ű ������ ã�ƿͼ�, FunctionList�� �Լ��� ����ϴ� �����̴�.

	// ���� �ʱ�ȭ�� ������ AddKey -> AddFunction �����̴�.

	PBindAxisInfo pAxis = FindAxis(strName);

	if (!pAxis)
	{
		// BindAxisInfo ������ ���ٸ� �ϳ� �����Ѵ�.
		pAxis = new BindAxisInfo;
		// ������ BindAxisInfo������ map�� �־��ش�.
		pAxis->strName = strName;
		// Ű�� ���߿� �߰��� ���̰�, FunctionList�� �ǵ��� �ʿ䰡 ����.
		m_mapBindAxis.insert(make_pair(strName, pAxis));
		// strName �� Ű������ BindAxisInfo ������ �־��ش�.
	}

	// ���ο� Ű�� �߰��ϴ� �������� ������ �߰��� �Ϲ�Ű���� Ȯ�����ش�.

	list<AxisInfo>::iterator iter;
	list<AxisInfo>::iterator iterEnd = pAxis->KeyList.end();

	for (iter = pAxis->KeyList.begin(); iter != iterEnd; ++iter)
	{
		if ((*iter).tKey.cKey == cKey)
			return;
		// �ش� BindAxisInfo ������ ��ȸ�ϸ鼭 �̹� �߰��� �� �Ϲ�Ű�� �ٽ� �߰��Ϸ��� �� ����� return;
	}

	// ������ ���� ����� ������ �ִ� ���� �޾ƿ´�.

	// ���ο� Ű�� �߰��Ѵ�.

	AxisInfo tInfo = {};

	tInfo.fScale = fScale;
	tInfo.tKey.cKey = cKey;
	
	// ���� ��� SKey�� ST_CTRL | ST_ALT �̷� ������ ���� ���ε�,
	// �̷���, �Ʒ� �κп��� CTRL �κа� ALT �κ��� & ���� ������ TRUE �� ���� ���̴�.
	// ���� ALTŰ�� CTRLŰ�� �߰��Ѵ�.

	if (iSKey & ST_CTRL)
		tInfo.tKey.bSKey[SKEY_CTRL] = true;


	if (iSKey & ST_ALT)
		tInfo.tKey.bSKey[SKEY_ALT] = true;


	if (iSKey & ST_SHIFT)
		tInfo.tKey.bSKey[SKEY_SHIFT] = true;

	// �������� ������ FALSE

	// �̷��� �Ͽ�, AxisInfo ������ ä���ְ�, �ش� ������ BindAxisInfo�� �߰��Ѵ�.

	pAxis->KeyList.push_back(tInfo);

	// �ش� Ű�� ���ؼ� Ȱ��ȭ�� ���ش�.
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

	// eEvent�� �Լ��� �߰��� �� �־��ְ� �ְ�, bDoubleStart�� fDoubleTime�� ���⼭ �̿��ϴ� ������ �ƴϴ�.
	// FunctionList�� ������ �ʿ䰡 ����.
	// KeyList�� �߰��� ����. Ư��Ű�� �Ϲ�Ű�� �迭 ������ ���ؼ� KeyInfo�� �߰��ϰ�,
	// strName�� �̿��ؼ� �߰��� BindActionInfo�� �޾ƿ´�.


	PBindActionInfo pAction = FindAction(strName);

	if (!pAction)
	{
		pAction = new BindActionInfo;
		pAction->strName = strName;
		m_mapBindAction.insert(make_pair(strName, pAction));
	}

	// ���ο� Ű�� �߰��ϴ� �������� ������ �߰��� �Ϲ�Ű���� Ȯ�����ش�.

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

	// �������� ������ FALSE

	// �̷��� �Ͽ�, AxisInfo ������ ä���ְ�, �ش� ������ BindAxisInfo�� �߰��Ѵ�.

	pAction->KeyList.push_back(tInfo);

	// �ش� Ű�� ���ؼ� Ȱ��ȭ�� ���ش�.
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
	// �ϴ� strName�� ���� ���� BindAxisInfo�� ã�ƿ´�.

	PBindAxisInfo pAxis = FindAxis(strName);

	if (!pAxis)
		return false;
	// �ش� �̸��� ���� BindAxisInfo�� ���ٸ� �Լ��� ������ �� ����.

	// �ִٸ� ������ ������ ã���ش�.

	list<KeyFunction>::iterator iter;
	list<KeyFunction>::iterator iterEnd = pAxis->FunctionList.end();

	for (iter = pAxis->FunctionList.begin(); iter != iterEnd; ++iter)
	{
		if ((*iter).pObj == pObj && (*iter).strFuncName == strFuncName)
		{
			// ������Ʈ�� ����, �Լ��̸��� �����ϴٸ� �ش� �Լ��� ã�� ���̴�.
			pAxis->FunctionList.erase(iter);
			return true;

			// �ش� �Լ��� ���������� return true;
		}
	}

	// ���� ���� �����Դٴ� �Ҹ��� �ش� ������ �Լ��� ã�� ���ߴٴ� ���̴�.
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
