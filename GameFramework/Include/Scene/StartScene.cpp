#include "StartScene.h"
#include "../UI/UIButton.h"
#include "../Core/SceneManager.h"
#include "../Core/EditManager.h"
#include "AnimationEditScene.h"
#include "MapEditScene.h"
#include "../Core/GameManager.h"
#include "../UI/UIImage.h"
#include "../Core/Input.h"
#include "../Sound/SoundManager.h"

#include "Stage1Scene.h"
//#include "Stage2Scene.h"
#include "DDDScene.h"

CStartScene::CStartScene()
{
}


CStartScene::~CStartScene()
{
}

void CStartScene::Start()
{
	CScene::Start();
}

bool CStartScene::Init()
{
	if (!CScene::Init())
		return false;

	// ���콺�� �̹� ��������, �浹ü, ä�α��� ��� ������ �Ǿ� �����̰� ������ ���⿡���� 
	// ��ư�� �������ش�.
	

	GET_SINGLE(CSoundManager)->Play("StartScene", CT_BGM);


	//���¿� ���� �ؽ�ó, ���¿� ���� ȣ���Լ�(�ַ� click���� ó���ϰ�����), ��ư�� ��ǥ.
	// ����� ��ư���� ������ y��ǥ�� x��ǥ(����)�� ���ؾ� �Ѵ�.
	// x : 14 y���� : 90
	CUIButton* pStartButton = CObj::CreateObj<CUIButton>("StartButton", "UI", this);

	pStartButton->SetPos(14.f, 114.f);

	pStartButton->SetButtonStateCallback(BS_CLICK, this, &CStartScene::StartButton); 
	
	pStartButton->SetButtonStateTexture(BS_NORMAL, "StartButtonNormal", TEXT("StartButton.bmp"));




	CUIButton* pMapEditorButton = CObj::CreateObj<CUIButton>("MapEditorButton", "UI", this);

	pMapEditorButton->SetPos(14.f, 304.f);

	pMapEditorButton->SetButtonStateCallback(BS_CLICK, this, &CStartScene::MapEditorButton);

	pMapEditorButton->SetButtonStateTexture(BS_NORMAL, "MapEditorButtonNormal", TEXT("MapEditorButton.bmp"));




	CUIButton* pAnimationEditorButton = CObj::CreateObj<CUIButton>("AnimationEditorButton", "UI", this);

	pAnimationEditorButton->SetPos(14.f, 494.f);

	pAnimationEditorButton->SetButtonStateCallback(BS_CLICK, this, &CStartScene::AnimationEditorButton);

	pAnimationEditorButton->SetButtonStateTexture(BS_NORMAL, "AnimationEditorButtonNormal", TEXT("AnimationEditorButton.bmp"));


	// ���� �̹����� �ϳ� �����Ѵ�.
	CUIImage* pBackGround = CObj::CreateObj<CUIImage>("StartSceneBackGround", "BackGround", this);

	pBackGround->SetTexture("StartSceneBackGround", TEXT("StartScene.bmp"));

	pBackGround->SetSize(RS_WIDTH, RS_HEIGHT);
	// �ؽ�ó�� �����ϴ� ��쿡�� �ִϸ��̼�ó�� ��»���� �������� �ʴ´�.



	return true;

}

bool CStartScene::SoundInit()
{
	GET_SINGLE(CSoundManager)->LoadSound("StartScene", true, "Music/StartScene.mp3");
	GET_SINGLE(CSoundManager)->LoadSound("ButtonChoice", false, "Effect/ButtonChoice.wav");

	return true;
}

void CStartScene::StartButton(float fTime)
{

	// ������ ������ �����ϴ� ��쿡 ����� Ű�� �߰��Ѵ�. (��� Stage���� ����ϴ� Ű)

	GET_SINGLE(CSoundManager)->Play("ButtonChoice", CT_EFFECT);


	GET_SINGLE(CInput)->AddNormalKey("TileRender", VK_F1);
	GET_SINGLE(CInput)->AddNormalKey("PlayerPos", VK_F2);
	GET_SINGLE(CInput)->AddNormalKey("DDD", VK_F3); // Boss HP 

	GET_SINGLE(CInput)->AddNormalKey("BurtCreate", VK_LCONTROL, 'B');
	GET_SINGLE(CInput)->AddNormalKey("GrizooCreate", VK_LCONTROL, 'G');
	GET_SINGLE(CInput)->AddNormalKey("TwizzyCreate", VK_LCONTROL, 'T');

	GET_SINGLE(CInput)->AddNormalKey("Attack", 'A');
	GET_SINGLE(CInput)->AddNormalKey("Jump", 'S');
	GET_SINGLE(CInput)->AddNormalKey("Prevent", 'D');
	GET_SINGLE(CInput)->AddNormalKey("Throw", 'F');

	GET_SINGLE(CInput)->AddNormalKey("Kirby", '1');
	GET_SINGLE(CInput)->AddNormalKey("StoneKirby", '2');
	GET_SINGLE(CInput)->AddNormalKey("SwordKirby", '3');
	GET_SINGLE(CInput)->AddNormalKey("MikeKirby", '4');
	GET_SINGLE(CInput)->AddNormalKey("SleepKirby", '5');

	// Ŀ�� �����ӿ� �̿��� �Լ�.
	GET_SINGLE(CInput)->AddAxisKey("Move", VK_LEFT, -1.f);
	GET_SINGLE(CInput)->AddAxisKey("Move", VK_RIGHT, 1.f);

	GET_SINGLE(CInput)->AddActionKey("Down", VK_DOWN);
	GET_SINGLE(CInput)->AddActionKey("Jump", 'S');

	GET_SINGLE(CInput)->AddActionKey("Run", VK_LEFT);
	GET_SINGLE(CInput)->AddActionKey("Run", VK_RIGHT);
	// ���� �ִ� �������� ó��.

	GET_SINGLE(CInput)->AddActionKey("Throw", 'F');
	//  �ڽ��� �ɷ� Ǯ��
	// Ȥ�� ���� ����.

	GET_SINGLE(CInput)->AddActionKey("Prevent", 'D');


	// Ŀ�� ����
	GET_SINGLE(CInput)->AddActionKey("Attack", 'A');




	// MainScene GOGO
	GET_SINGLE(CGameManager)->SetGameMode(GM_PLAY);
	//GET_SINGLE(CSceneManager)->CreateScene<CDDDScene>("DDDScene");
	GET_SINGLE(CSceneManager)->CreateScene<CStage1Scene>("Stage1Scene");
	//GET_SINGLE(CSceneManager)->CreateScene<CStage2Scene>("Stage2Scene");
	GET_SINGLE(CSceneManager)->SetPlayer();
	GET_SINGLE(CSceneManager)->SetStage();
	GET_SINGLE(CSceneManager)->SetLimitCamera(GET_SINGLE(CSceneManager)->GetStageSize());




	// CreateScene-> Scene Init -> CreateObj-> Object Init ���Ŀ� -> Scene->AddObject(���� ������ ��鿡
	// ������Ʈ�� ��) �׷��� �츮�� NextScene(���� ��)�� ���ؼ� FindObject�Լ��� ����� �� ����.

}

void CStartScene::MapEditorButton(float fTime)
{
	GET_SINGLE(CSoundManager)->Play("ButtonChoice", CT_EFFECT);

	GET_SINGLE(CGameManager)->SetGameMode(GM_MAP_EDIT);
	GET_SINGLE(CSceneManager)->CreateScene<CMapEditScene>("MapEditScene");
	GET_SINGLE(CSceneManager)->SetStage();
	GET_SINGLE(CGameManager)->SetPause(false);

	// ���������� �����Ͽ� �����ִ� �ý��� ����.

	ShowCursor(FALSE);

}

void CStartScene::AnimationEditorButton(float fTime)
{
	GET_SINGLE(CSoundManager)->Play("ButtonChoice", CT_EFFECT);

	// AnimationScene GOGO ��� �������� Mode�� ������ �޾Ƽ� �̸� ó��.
	GET_SINGLE(CGameManager)->SetGameMode(GM_ANIMATION_EDIT);
	GET_SINGLE(CSceneManager)->CreateScene<CAnimationEditScene>("AnimationEditScene");


		// �ִϸ��̼� ������ ���õ� ������ �ʱ�ȭ.
		if (!GET_SINGLE(CEditManager)->Init())
			return;

}

