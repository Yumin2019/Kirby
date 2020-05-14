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
	// �� �浹ü��, �÷��̾��� Body�� �浹�� �� ���¿��� upŰ�� ���� ��쿡 ó���� �Ǿ� �Ѵ�.
	// �̰��� �÷��̾��� �浹�߿� �ش��ϴ� �Լ����� ó���� �ϵ��� ����.
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
	// �츱 ������Ʈ�� �츮�� ó���� �Ѵ�.

	// ������Ʈ�� ������� ������ �� �����غ��ƾ� �Ѵ�. Scene�� �������, �ش� Scene�� �Ҽ��̾���
	// ������Ʈ�� ��� ������ Ǯ��, �����ִ� ó���� ������ �ǰ� �ִ�.
	// �ٵ�, Scene�� ������� ó���� ChangeScene���� ������ �ǹǷ�(SceneManager) �츮�� �� ������ 
	// ������ ���ְ�, ���� ��鿡 ���ؼ� �̸� ������Ʈ�� �߰��� �صθ� �Ǵ� ���̴�.
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

	// Door �ؽ�ó�� �о����, �̰��� �����Ѵ�.
	// ���Ŀ� �ٽ� Init�Լ��� ȣ���ϴ� ������ �̹� �޾Ƶ� ���ҽ��� Ȱ���Ͽ� ����Ƚ���� ������Ű�� ó���� �ϰ� �ִ�.

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
