#include "UIButton.h"
#include "../Collision/ColliderRect.h"
#include "../Core/Input.h"
#include "../Texture.h"
#include "../Core/ResourceManager.h"


CUIButton::CUIButton()
{
	m_eState = BS_NORMAL;
	m_bMouseOn = false;
	memset(m_pStateTexture, 0, sizeof(CTexture*) * BS_END);
}


CUIButton::CUIButton(const CUIButton & obj) :
	CUI(obj)
{
	m_bMouseOn = false;
}

CUIButton::~CUIButton()
{
	for (int i = 0; i < BS_END; ++i)
	{
		SAFE_RELEASE(m_pStateTexture[i]);
	}
}

void CUIButton::Start()
{
	CUI::Start();
}

bool CUIButton::Init()
{
	if (!CUI::Init())
		return false;

	SetSize(700, 100);
	// 마우스 배치를 하는 경우에는 중심을 기준으로 하는 것이 쉬울 것 같다.

	// 기본텍스처 지정 X
	m_eState = BS_NORMAL;
	m_ePrevState = BS_NORMAL;

	// 충돌의 상태에 따라서, 호출할 함수를 등록하는 시스템이다.
	// 따라서, 등록할 대상체인 Collider를 가져와야 한다.

	CColliderRect* pBody = AddCollider<CColliderRect>("ButtonBody");
	//  충돌체를 Obj의 충돌체 추가 함수를 통해서 추가하면, 일단, 오브젝트의 ColliderList에 넣어주고,
	// ref = 3가 된다. (내부적으로 ref = 2, 나중에 CollisionManager에 해당 충돌체가 들어가게 되고, (Start함수) 1)
	// 소멸시킬 때는, 오브젝트에서 한번, CollisionManager에서 한번.
	// 남은 하나는 이곳에서 처리해야 한다.

	pBody->SetRelativeInfo(0.f, 0.f, 700.f, 100.f);
	pBody->SetProfile("ButtonProfile");
	pBody->SetChannel("Button");
	pBody->SetCollisionCallback<CUIButton>(CS_BLOCK, &CUIButton::Hit, this); // Block(초기 충돌)에서 호출할 함수로 
	// Hit을 등록.

	pBody->SetCollisionCallback<CUIButton>(CS_RELEASE, &CUIButton::HitRelease, this);

	SAFE_RELEASE(pBody);

	/*
	정리. 버튼은 내부적으로 Hit, HitRelease함수를 이용하여 마우스의 충돌체와 소통할 것이다.
	이것은 이제 상관쓰지 않아도 되고(쓸 필요도 없는 독립적인 사용이니까) 우리는 버튼의 상태에 따른
	텍스처와 상태에 따른 호출 함수만 지정하면 되는 것이다. float 형 하나를 가지는 함수의 형태로.
	상태에 따른 텍스처, 상태에 따른 호출함수(주로 click에서 처리하겠지만), 버튼의 좌표.
	*/

	return true;
}

void CUIButton::Update(float fTime)
{
	CUI::Update(fTime);
}

void CUIButton::LateUpdate(float fTime)
{
	CUI::LateUpdate(fTime);

	if (m_bMouseOn)
	{
		if (KEYDOWN("MouseLButton"))
		{
			// 충돌이 된 상태에서 DOWN을 눌렀다.
			m_eState = BS_CLICK;
		}
		else if (KEYUP("MouseLButton") && m_eState == BS_CLICK)
		{
			// 마우스의 버튼을 뗀 상태인데, 그 위치에서 충돌이 일어나고 있었고, 
			// 현재 상태가 click.

			// 클릭의 동작은 누르고 떼는 동작까지.
			// 누르고서 바깥으로 이동한 다음에 up을 한 경우에는 보통 처리를 안 하는 곳이 대부분.
			// 그 처리를 하기 위한 로직이 설계되어 있다.
			// 일단 충돌중인 상태에서 click인지를 확인하고,
			// 확실하게 뗀 경우에. (즉 (정확하게는 이전)상태가 click이고, UP이면서 충돌이라면 완전한 클릭이 이루어 진 것이다)
			// 이 경우에 클릭의 상태에 해당하는 함수를 호출해주는 형태이다.

			// 다른 예시로 만약에 내부에서 마우스를 클릭하고, 바깥으로 이동을 했다고 해보자.
			// 그러면, 상태가 MOUSE_NORMAL 이 되고, 무엇보다, 마우스와의 충돌상태가 풀리게 되어서
			// 이곳으로 들어오지도 않는다. 즉, 함수 호출은 없다.

			// 이곳에 들어온 경우라면, DOWN을 한 이후에, 충돌중에 UP을 한 경우이다. (즉, 완벽한 클릭. 함수 호출상황)

			if (m_StateCallback[BS_CLICK] != nullptr)
				m_StateCallback[BS_CLICK](fTime);
		}
	}


	if (m_ePrevState != m_eState)
	{
		m_ePrevState = m_eState;


		if(m_pStateTexture[m_eState] != nullptr)
			SetTexture(m_pStateTexture[m_eState]);

		// 이전 상태와 현재 상태가 다른 경우에는 이전 상태에 현재 상태를 업데이트 해주고
		// 바뀐 현재의 상태로 텍스처를 교체해준다. (있다면.)
	}
}

void CUIButton::Collision(float fTime)
{
	CUI::Collision(fTime);

}

void CUIButton::PrevRender(HDC hDC, float fTime)
{
	CUI::PrevRender(hDC, fTime);

}

void CUIButton::Render(HDC hDC, float fTime)
{
	CUI::Render(hDC, fTime);

}

void CUIButton::PostRender(HDC hDC, float fTime)
{
	CUI::PostRender(hDC, fTime);

}

CUIButton * CUIButton::Clone() const
{
	return new CUIButton(*this);
}

void CUIButton::AddClipName(vector<string>& vecClipName)
{
}

// 이 두 함수는 (hit, hitRelease) 마우스의 충돌체와
// 충돌이 일어났는지 확인하는 용도로 사용하는 함수인데,
// 초기 충돌이 일어난 경우에 TRUE / 충돌에서 벗어난 경우에 FALSE를 주기 위함이다.


void CUIButton::Hit(CCollider * pSrc, CCollider * pDest, float fTime)
{
	// 솔직히, DISENABLE 의 상태인 마우스를 만들지는 의문이긴 한데, 활용도에 따라 다른 부분인가 ?
	// 아무튼 비활성화라면 RETURN
	if (m_eState == BS_DISENABLE)
	{
		return;
	}

	if (pDest->GetTag() == "MouseUI")
	{
		// 마우스와 처음 충돌이 일어난 경우
		m_bMouseOn = true;
		m_eState = BS_MOUSEON;
	}
}

void CUIButton::HitRelease(CCollider * pSrc, CCollider * pDest, float fTime)
{
	if (m_eState == BS_DISENABLE)
	{
		return;
	}

	if (pDest->GetTag() == "MouseUI")
	{
		// 충돌이 끊어진 경우
		m_bMouseOn = false;
		m_eState = BS_NORMAL;
	}
}

void CUIButton::SetButtonStateTexture(BUTTON_STATE eState,
	const string & strName, const TCHAR * pFileName,
	const string & strPathName)
{
	SAFE_RELEASE(m_pStateTexture[eState]);
	// 아마 없을 건데 있을 걸 예상하고..

	if (pFileName)
		GET_SINGLE(CResourceManager)->LoadTexture(strName, pFileName, strPathName);
	// 넣어준 이름을 기반으로 텍스처를 생성해주고.

	m_pStateTexture[eState] = GET_SINGLE(CResourceManager)->FindTexture(strName);
	// 해당텍스처를 넣어주도록 한다.


	if (eState == BS_NORMAL)
	{
		m_pTexture = m_pStateTexture[eState];
		m_pTexture->AddRef();
	}

	// resource 1 Obj 1
	// Obj rel resource rel OK
}
