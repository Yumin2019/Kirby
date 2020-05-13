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

	// 마우스는 이미 프로파일, 충돌체, 채널까지 모두 설정이 되어 움직이고 있으니 여기에서는 
	// 버튼을 생성해준다.
	

	GET_SINGLE(CSoundManager)->Play("StartScene", CT_BGM);


	//상태에 따른 텍스처, 상태에 따른 호출함수(주로 click에서 처리하겠지만), 버튼의 좌표.
	// 등록할 버튼간의 적절한 y좌표와 x좌표(시작)을 구해야 한다.
	// x : 14 y간격 : 90
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


	// 배경용 이미지를 하나 생성한다.
	CUIImage* pBackGround = CObj::CreateObj<CUIImage>("StartSceneBackGround", "BackGround", this);

	pBackGround->SetTexture("StartSceneBackGround", TEXT("StartScene.bmp"));

	pBackGround->SetSize(RS_WIDTH, RS_HEIGHT);
	// 텍스처를 지정하는 경우에는 애니메이션처럼 출력사이즈를 정해주지 않는다.



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

	// 실제로 게임을 시작하는 경우에 사용할 키를 추가한다. (모든 Stage에서 사용하는 키)

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

	// 커비 움직임에 이용할 함수.
	GET_SINGLE(CInput)->AddAxisKey("Move", VK_LEFT, -1.f);
	GET_SINGLE(CInput)->AddAxisKey("Move", VK_RIGHT, 1.f);

	GET_SINGLE(CInput)->AddActionKey("Down", VK_DOWN);
	GET_SINGLE(CInput)->AddActionKey("Jump", 'S');

	GET_SINGLE(CInput)->AddActionKey("Run", VK_LEFT);
	GET_SINGLE(CInput)->AddActionKey("Run", VK_RIGHT);
	// 보고 있는 방향으로 처리.

	GET_SINGLE(CInput)->AddActionKey("Throw", 'F');
	//  자신의 능력 풀기
	// 혹은 헬퍼 해제.

	GET_SINGLE(CInput)->AddActionKey("Prevent", 'D');


	// 커비 공격
	GET_SINGLE(CInput)->AddActionKey("Attack", 'A');




	// MainScene GOGO
	GET_SINGLE(CGameManager)->SetGameMode(GM_PLAY);
	//GET_SINGLE(CSceneManager)->CreateScene<CDDDScene>("DDDScene");
	GET_SINGLE(CSceneManager)->CreateScene<CStage1Scene>("Stage1Scene");
	//GET_SINGLE(CSceneManager)->CreateScene<CStage2Scene>("Stage2Scene");
	GET_SINGLE(CSceneManager)->SetPlayer();
	GET_SINGLE(CSceneManager)->SetStage();
	GET_SINGLE(CSceneManager)->SetLimitCamera(GET_SINGLE(CSceneManager)->GetStageSize());




	// CreateScene-> Scene Init -> CreateObj-> Object Init 이후에 -> Scene->AddObject(새로 생성한 장면에
	// 오브젝트가 들어감) 그러면 우리는 NextScene(생긴 것)을 통해서 FindObject함수를 사용할 수 있음.

}

void CStartScene::MapEditorButton(float fTime)
{
	GET_SINGLE(CSoundManager)->Play("ButtonChoice", CT_EFFECT);

	GET_SINGLE(CGameManager)->SetGameMode(GM_MAP_EDIT);
	GET_SINGLE(CSceneManager)->CreateScene<CMapEditScene>("MapEditScene");
	GET_SINGLE(CSceneManager)->SetStage();
	GET_SINGLE(CGameManager)->SetPause(false);

	// 스테이지를 생성하여 보여주는 시스템 제작.

	ShowCursor(FALSE);

}

void CStartScene::AnimationEditorButton(float fTime)
{
	GET_SINGLE(CSoundManager)->Play("ButtonChoice", CT_EFFECT);

	// AnimationScene GOGO 장면 생성에서 Mode의 영향을 받아서 미리 처리.
	GET_SINGLE(CGameManager)->SetGameMode(GM_ANIMATION_EDIT);
	GET_SINGLE(CSceneManager)->CreateScene<CAnimationEditScene>("AnimationEditScene");


		// 애니메이션 편집에 관련된 정보를 초기화.
		if (!GET_SINGLE(CEditManager)->Init())
			return;

}

