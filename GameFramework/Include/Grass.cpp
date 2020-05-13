#include "Object\Grass.h"
#include "Animation/Animation.h"


CGrass::CGrass()
{
	SetTileCollisionEnable(false);

}


CGrass::CGrass(const CGrass & obj) :
	CAniObject(obj)
{
	SetTileCollisionEnable(false);

}

CGrass::~CGrass()
{
}

void CGrass::Start()
{
	CObj::Start();
}

bool CGrass::Init()
{
	if (!CObj::Init())
		return false;

	// 잔디에 대한 애니메이션을 만든다.
	CreateAnimation("GrassAni");

	m_pAnimation->AddClip("Grass");
	m_pAnimation->SetClipTexture("Grass", "ForeGround", TEXT("ForeGround.bmp"));
	m_pAnimation->SetTextureColorKeyAll(0, 18, 127);




	return true;
}

void CGrass::Update(float fTime)
{
	CObj::Update(fTime);
}

void CGrass::LateUpdate(float fTime)
{
	CObj::LateUpdate(fTime);

}

void CGrass::Collision(float fTime)
{
	CObj::Collision(fTime);

}

void CGrass::PrevRender(HDC hDC, float fTime)
{
	CObj::PrevRender(hDC, fTime);

}

void CGrass::Render(HDC hDC, float fTime)
{
	CObj::Render(hDC, fTime);

}

void CGrass::PostRender(HDC hDC, float fTime)
{
	CObj::PostRender(hDC, fTime);

}

CGrass * CGrass::Clone() const
{
	return new CGrass(*this);
}

void CGrass::AddClipName(vector<string>& vecClipName)
{
	// 해당 애니메이션은 4개로 구성이 되어 있는데, 이것은 오브젝트와 충돌이 되는 순간에 돌려줘야 한다.
	// ClipName에서 망설여지는 것이 아쉽지만, 이 부분은 같은 이름으로 대체하면 된다.
	// 즉, 애니메이션을 생성해두는 것은 이곳에서 하겠지만, 그 애니메이션의 클립을 연결하고 사용하는 것은 
	// 전적으로 외부에서 처리해주는 것이다.
	// 외부에서 해당 클래스에 접근해서 LoadClip함수를 호출해주도록 하겠다.
	// 이름을 같이 사용하고,

	vecClipName.push_back("Grass");
	
}
