#pragma once
#include "UI.h"

enum BUTTON_STATE
{
	BS_DISENABLE, // ���ɷ��ϰ� �ϴ�
	BS_NORMAL,
	BS_MOUSEON,
	BS_CLICK,
	BS_END
};

class CUIButton :
	public CUI
{

	DECLARE_OBJECT

protected:
	CUIButton();
	CUIButton(const CUIButton& obj);
	virtual ~CUIButton();

protected:
	BUTTON_STATE	m_eState;
	BUTTON_STATE	m_ePrevState;
	class CTexture*		m_pStateTexture[BS_END];
	bool					m_bMouseOn;
	function<void(float)>	m_StateCallback[BS_END];

public:
	void DisEnable(bool bDisEnable)
	{
		if (bDisEnable)
			m_eState = BS_DISENABLE;
		else
			m_eState = BS_NORMAL;

		if (m_pStateTexture[m_eState])
			SetTexture(m_pStateTexture[m_eState]);

		// normal disenable�� �ش��ϴ� �̹����� ������ �̹����� �����Ѵ�.
		// ������ normal�̹����� ����� ����.
	}

public:
	virtual void Start();
	virtual bool Init();
	virtual void Update(float fTime);
	virtual void LateUpdate(float fTime);
	virtual void Collision(float fTime);
	virtual void PrevRender(HDC hDC, float fTime);
	virtual void Render(HDC hDC, float fTime);
	virtual void PostRender(HDC hDC, float fTime);
	virtual CUIButton* Clone() const;
	virtual void AddClipName(vector<string>& vecClipName);

public:
	void Hit(CCollider* pSrc, CCollider* pDest, float fTime);
	void HitRelease(CCollider* pSrc, CCollider* pDest, float fTime);
	

public:
	template <typename T>
	void SetButtonStateCallback(BUTTON_STATE eState,
		T* pObj, void(T::*pFunc)(float))
	{
		// ���콺�� Ư�� ���¿��� ȣ��� �Լ��� ����Ѵ�.
		m_StateCallback[eState] = bind(pFunc, pObj, placeholders::_1);
	}

	// ��ư�� ���¿� ���� �ؽ�ó�� �ٸ��� ������ �� �ִ� ����� �����.
	void SetButtonStateTexture(BUTTON_STATE eState, 
		const string& strName, const TCHAR* pFileName = nullptr, 
		const string& strPathName = TEXTURE_PATH);

		/*
		void SetTexture(const string& strName, const TCHAR* pFileName = nullptr,
		const string& strPathName = TEXTURE_PATH);
		*/
};

