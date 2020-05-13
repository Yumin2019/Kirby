#pragma once
#include "UI.h"

enum BUTTON_STATE
{
	BS_DISENABLE, // 무능력하게 하다
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

		// normal disenable에 해당하는 이미지를 현재의 이미지로 셋팅한다.
		// 보통은 normal이미지를 사용할 것임.
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
		// 마우스에 특정 상태에서 호출될 함수를 등록한다.
		m_StateCallback[eState] = bind(pFunc, pObj, placeholders::_1);
	}

	// 버튼에 상태에 따라 텍스처를 다르게 지정할 수 있는 기능을 만든다.
	void SetButtonStateTexture(BUTTON_STATE eState, 
		const string& strName, const TCHAR* pFileName = nullptr, 
		const string& strPathName = TEXTURE_PATH);

		/*
		void SetTexture(const string& strName, const TCHAR* pFileName = nullptr,
		const string& strPathName = TEXTURE_PATH);
		*/
};

