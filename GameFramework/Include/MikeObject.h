#pragma once
#include "Object/Obj.h"

class CMikeObject :
	public CObj
{
	DECLARE_OBJECT

	friend class CPlayer;

protected:
	CMikeObject();
	CMikeObject(const CMikeObject& obj);
	virtual ~CMikeObject();



public:
	// 일단, 진동의 정보를 셋팅한다. 그리고 여기에 추가적으로 Enable을 사용하여 셋팅을 해주는 처리를 하면 좋겠다
	
	void SetMikeObject(float fIntervalTime, int iLimitCount, int iBlendMin = 180, int iBlendMax = 255);


	
public:
	virtual void Start();
	virtual bool Init();
	virtual void Update(float fTime);
	virtual void LateUpdate(float fTime);
	virtual void Collision(float fTime);
	virtual void PrevRender(HDC hDC, float fTime);
	virtual void Render(HDC hDC, float fTime);
	virtual void PostRender(HDC hDC, float fTime);
	virtual CMikeObject* Clone() const;
	virtual void AddClipName(vector<string>& vecClipName);
	
	void MikeObjectCollisionStart(CCollider* pSrc, CCollider* pDest, float fTime);
};

