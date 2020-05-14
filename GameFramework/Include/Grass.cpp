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

	// �ܵ� ���� �ִϸ��̼��� �����.
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
	// �ش� �ִϸ��̼��� 4���� ������ �Ǿ� �ִµ�, �̰��� ������Ʈ�� �浹�� �Ǵ� ������ ������� �Ѵ�.
	// ClipName���� ���������� ���� �ƽ�����, �� �κ��� ���� �̸����� ��ü�ϸ� �ȴ�.
	// ��, �ִϸ��̼��� �����صδ� ���� �̰����� �ϰ�����, �� �ִϸ��̼��� Ŭ���� �����ϰ� ����ϴ� ���� 
	// �������� �ܺο��� ó�����ִ� ���̴�.
	// �ܺο��� �ش� Ŭ������ �����ؼ� LoadClip�Լ��� ȣ�����ֵ��� �ϰڴ�.
	// �̸��� ���� ����ϰ�,

	vecClipName.push_back("Grass");
	
}
