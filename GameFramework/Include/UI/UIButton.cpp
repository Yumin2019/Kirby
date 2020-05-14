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
	// ���콺 ��ġ�� �ϴ� ��쿡�� �߽��� �������� �ϴ� ���� ���� �� ����.

	// �⺻�ؽ�ó ���� X
	m_eState = BS_NORMAL;
	m_ePrevState = BS_NORMAL;

	// �浹�� ���¿� ����, ȣ���� �Լ��� ����ϴ� �ý����̴�.
	// ����, ����� ���ü�� Collider�� �����;� �Ѵ�.

	CColliderRect* pBody = AddCollider<CColliderRect>("ButtonBody");
	//  �浹ü�� Obj�� �浹ü �߰� �Լ��� ���ؼ� �߰��ϸ�, �ϴ�, ������Ʈ�� ColliderList�� �־��ְ�,
	// ref = 3�� �ȴ�. (���������� ref = 2, ���߿� CollisionManager�� �ش� �浹ü�� ���� �ǰ�, (Start�Լ�) 1)
	// �Ҹ��ų ����, ������Ʈ���� �ѹ�, CollisionManager���� �ѹ�.
	// ���� �ϳ��� �̰����� ó���ؾ� �Ѵ�.

	pBody->SetRelativeInfo(0.f, 0.f, 700.f, 100.f);
	pBody->SetProfile("ButtonProfile");
	pBody->SetChannel("Button");
	pBody->SetCollisionCallback<CUIButton>(CS_BLOCK, &CUIButton::Hit, this); // Block(�ʱ� �浹)���� ȣ���� �Լ��� 
	// Hit�� ���.

	pBody->SetCollisionCallback<CUIButton>(CS_RELEASE, &CUIButton::HitRelease, this);

	SAFE_RELEASE(pBody);

	/*
	����. ��ư�� ���������� Hit, HitRelease�Լ��� �̿��Ͽ� ���콺�� �浹ü�� ������ ���̴�.
	�̰��� ���� ������� �ʾƵ� �ǰ�(�� �ʿ䵵 ���� �������� ����̴ϱ�) �츮�� ��ư�� ���¿� ����
	�ؽ�ó�� ���¿� ���� ȣ�� �Լ��� �����ϸ� �Ǵ� ���̴�. float �� �ϳ��� ������ �Լ��� ���·�.
	���¿� ���� �ؽ�ó, ���¿� ���� ȣ���Լ�(�ַ� click���� ó���ϰ�����), ��ư�� ��ǥ.
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
			// �浹�� �� ���¿��� DOWN�� ������.
			m_eState = BS_CLICK;
		}
		else if (KEYUP("MouseLButton") && m_eState == BS_CLICK)
		{
			// ���콺�� ��ư�� �� �����ε�, �� ��ġ���� �浹�� �Ͼ�� �־���, 
			// ���� ���°� click.

			// Ŭ���� ������ ������ ���� ���۱���.
			// ������ �ٱ����� �̵��� ������ up�� �� ��쿡�� ���� ó���� �� �ϴ� ���� ��κ�.
			// �� ó���� �ϱ� ���� ������ ����Ǿ� �ִ�.
			// �ϴ� �浹���� ���¿��� click������ Ȯ���ϰ�,
			// Ȯ���ϰ� �� ��쿡. (�� (��Ȯ�ϰԴ� ����)���°� click�̰�, UP�̸鼭 �浹�̶�� ������ Ŭ���� �̷�� �� ���̴�)
			// �� ��쿡 Ŭ���� ���¿� �ش��ϴ� �Լ��� ȣ�����ִ� �����̴�.

			// �ٸ� ���÷� ���࿡ ���ο��� ���콺�� Ŭ���ϰ�, �ٱ����� �̵��� �ߴٰ� �غ���.
			// �׷���, ���°� MOUSE_NORMAL �� �ǰ�, ��������, ���콺���� �浹���°� Ǯ���� �Ǿ
			// �̰����� �������� �ʴ´�. ��, �Լ� ȣ���� ����.

			// �̰��� ���� �����, DOWN�� �� ���Ŀ�, �浹�߿� UP�� �� ����̴�. (��, �Ϻ��� Ŭ��. �Լ� ȣ���Ȳ)

			if (m_StateCallback[BS_CLICK] != nullptr)
				m_StateCallback[BS_CLICK](fTime);
		}
	}


	if (m_ePrevState != m_eState)
	{
		m_ePrevState = m_eState;


		if(m_pStateTexture[m_eState] != nullptr)
			SetTexture(m_pStateTexture[m_eState]);

		// ���� ���¿� ���� ���°� �ٸ� ��쿡�� ���� ���¿� ���� ���¸� ������Ʈ ���ְ�
		// �ٲ� ������ ���·� �ؽ�ó�� ��ü���ش�. (�ִٸ�.)
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

// �� �� �Լ��� (hit, hitRelease) ���콺�� �浹ü��
// �浹�� �Ͼ���� Ȯ���ϴ� �뵵�� ����ϴ� �Լ��ε�,
// �ʱ� �浹�� �Ͼ ��쿡 TRUE / �浹���� ��� ��쿡 FALSE�� �ֱ� �����̴�.


void CUIButton::Hit(CCollider * pSrc, CCollider * pDest, float fTime)
{
	// ������, DISENABLE �� ������ ���콺�� �������� �ǹ��̱� �ѵ�, Ȱ�뵵�� ���� �ٸ� �κ��ΰ� ?
	// �ƹ�ư ��Ȱ��ȭ��� RETURN
	if (m_eState == BS_DISENABLE)
	{
		return;
	}

	if (pDest->GetTag() == "MouseUI")
	{
		// ���콺�� ó�� �浹�� �Ͼ ���
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
		// �浹�� ������ ���
		m_bMouseOn = false;
		m_eState = BS_NORMAL;
	}
}

void CUIButton::SetButtonStateTexture(BUTTON_STATE eState,
	const string & strName, const TCHAR * pFileName,
	const string & strPathName)
{
	SAFE_RELEASE(m_pStateTexture[eState]);
	// �Ƹ� ���� �ǵ� ���� �� �����ϰ�..

	if (pFileName)
		GET_SINGLE(CResourceManager)->LoadTexture(strName, pFileName, strPathName);
	// �־��� �̸��� ������� �ؽ�ó�� �������ְ�.

	m_pStateTexture[eState] = GET_SINGLE(CResourceManager)->FindTexture(strName);
	// �ش��ؽ�ó�� �־��ֵ��� �Ѵ�.


	if (eState == BS_NORMAL)
	{
		m_pTexture = m_pStateTexture[eState];
		m_pTexture->AddRef();
	}

	// resource 1 Obj 1
	// Obj rel resource rel OK
}
