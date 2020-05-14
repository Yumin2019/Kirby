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

			// Ÿ���� ��´ٴ� �Ҹ��� �̰��� ����ϰڴٴ� �ǹ� �ƴѰ� ? 
			// Enable = true;

			m_bEnable = true;
		}
	}

	void TargetDisEnable()
	{
		SAFE_RELEASE(m_pTarget);

		// ����� ��ȯ�� �Ǹ鼭 �ش� UI�� ��� �̿��ؾ� �Ѵ�. �� ��Ȳ���� TargetUI�� �����ϴ� ������Ʈ�� ������� �ϴ� ��찡
		// �и� �����ϴµ�, ���ʿ��� ������ �ϰ� ������ �ش������Ʈ�� ������� �ʴ´�.
		// ���� ���ϴ� ������ �̰��� Ǯ���ִ� ó���� �� �� �ִ� �Լ��� ����� �� ���̴�.

		// DDD �� �����ϰ� ���� ������� �Ѿ��. 
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

