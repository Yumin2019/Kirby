#pragma once
#include "UIImage.h"
class CTargetUI :
	public CUIImage
{
	DECLARE_OBJECT

protected:
	CObj* m_pTarget;

public:
	void SetTarget(CObj* pTarget)
	{
		if (pTarget)
		{
			m_pTarget = pTarget;
			m_pTarget->AddRef();

			// 타겟을 잡는다는 소리는 이것을 사용하겠다는 의미 아닌가 ? 
			// Enable = true;

			m_bEnable = true;
		}
	}

	void TargetDisEnable()
	{
		SAFE_RELEASE(m_pTarget);

		// 장면이 전환이 되면서 해당 UI를 계속 이용해야 한다. 이 상황에서 TargetUI가 참조하는 오브젝트가 사라져야 하는 경우가
		// 분명 존재하는데, 이쪽에서 참조를 하고 있으면 해당오브젝트는 사라지지 않는다.
		// 따라서 원하는 순간에 이것을 풀어주는 처리를 할 수 있는 함수를 만들어 둔 것이다.

		// DDD 를 셋팅하고 다음 장면으로 넘어간다. 
	}

protected:
	CTargetUI();
	CTargetUI(const CTargetUI& obj);
	~CTargetUI();

public:
	virtual void Start();
	virtual bool Init();
	virtual void Update(float fTime);
	virtual void LateUpdate(float fTime);
	virtual void Collision(float fTime);
	virtual void PrevRender(HDC hDC, float fTime);
	virtual void Render(HDC hDC, float fTime);
	virtual void PostRender(HDC hDC, float fTime);
	virtual CTargetUI* Clone()	const;
	virtual void AddClipName(vector<string>& vecClipName);
};

