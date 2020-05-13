#include "Door.h"
#include "../Core/SceneManager.h"
#include "../Scene/DDDScene.h"
#include "../Scene/Stage2Scene.h"
#include "../Scene/Stage3Scene.h"
#include "../Scene/Stage4Scene.h"
#include "../Scene/Stage5Scene.h"
#include "../Scene/EndingScene.h"

CDoor::CDoor()
{
	m_eSceneType = ST_NONE;
	m_eObjectType = OT_DOOR;
}


CDoor::CDoor(const CDoor & obj) :
	CObj(obj)
{
	m_eObjectType = OT_DOOR;
	m_eSceneType = ST_NONE;

}

CDoor::~CDoor()
{
}

void CDoor::DoorColliderStay(CCollider * pSrc, CCollider * pDest, float fTime)
{
	// 이 충돌체와, 플레이어의 Body와 충돌이 된 상태에서 up키를 눌른 경우에 처리가 되야 한다.
	// 이것을 플레이어의 충돌중에 해당하는 함수에서 처리를 하도록 하자.
}

void CDoor::Stage3Scene()
{

	GET_SINGLE(CSceneManager)->CreateScene<CStage3Scene>("Stage3Scene");
	SceneChange();
}

void CDoor::Stage4Scene()
{
	GET_SINGLE(CSceneManager)->CreateScene<CStage4Scene>("Stage4Scene");
	SceneChange();
}

void CDoor::Stage5Scene()
{
	GET_SINGLE(CSceneManager)->CreateScene<CStage5Scene>("Stage5Scene");
	SceneChange();
}

void CDoor::Stage2Scene()
{
	
	GET_SINGLE(CSceneManager)->CreateScene<CStage2Scene>("Stage2Scene");
	SceneChange();

}

void CDoor::DDDScene()
{
	GET_SINGLE(CSceneManager)->CreateScene<CDDDScene>("DDDScene");
	SceneChange();
	// 살릴 오브젝트를 살리는 처리를 한다.

	// 오브젝트가 사라지는 시점을 잘 생각해보아야 한다. Scene이 사라지면, 해당 Scene의 소속이었던
	// 오브젝트를 모두 참조를 풀고, 날려주는 처리가 진행이 되고 있다.
	// 근데, Scene이 사라지는 처리는 ChangeScene에서 진행이 되므로(SceneManager) 우리는 그 전에만 
	// 참조를 해주고, 다음 장면에 대해서 미리 오브젝트만 추가를 해두면 되는 것이다.
}

void CDoor::EndingScene()
{
	GET_SINGLE(CSceneManager)->CreateScene<CEndingScene>("EndingScene");
	SceneChange();
}

void CDoor::SceneChange()
{
	GET_SINGLE(CSceneManager)->SetStage();
	GET_SINGLE(CSceneManager)->SetLimitCamera(GET_SINGLE(CSceneManager)->GetStageSize());


	GET_SINGLE(CSceneManager)->ObjectSave();
}


void CDoor::Start()
{
	CObj::Start();
}

bool CDoor::Init()
{
	if (!CObj::Init())
		return false;

	SetTexture("Door", TEXT("Door.bmp"));

	// Door 텍스처를 읽어오고, 이것을 설정한다.
	// 이후에 다시 Init함수를 호출하는 순간에 이미 받아둔 리소스를 활용하여 참조횟수를 증가시키는 처리를 하고 있다.

	return true;
}

void CDoor::Update(float fTime)
{
	CObj::Update(fTime);
}

void CDoor::LateUpdate(float fTime)
{
	CObj::LateUpdate(fTime);

}

void CDoor::Collision(float fTime)
{
	CObj::Collision(fTime);

}

void CDoor::PrevRender(HDC hDC, float fTime)
{
	CObj::PrevRender(hDC, fTime);

}

void CDoor::Render(HDC hDC, float fTime)
{
	CObj::Render(hDC, fTime);

}

void CDoor::PostRender(HDC hDC, float fTime)
{
	CObj::PostRender(hDC, fTime);

}

CDoor * CDoor::Clone() const
{
	return new CDoor(*this);
}

void CDoor::AddClipName(vector<string>& vecClipName)
{
}
