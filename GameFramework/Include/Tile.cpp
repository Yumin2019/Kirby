#include "Tile.h"
#include "Core/GameManager.h"
#include "Core/SceneManager.h"

CTile::CTile()
{
	m_eOption = TO_NORMAL;
	
}

CTile::CTile(const CTile & obj) :
	CObj(obj)
{

}


CTile::~CTile()
{
}

void CTile::Start()
{
}

bool CTile::Init()
{
	if (!CObj::Init())
		return false;

	return true;
}

void CTile::Update(float fTime)
{
	CObj::Update(fTime);
}

void CTile::LateUpdate(float fTime)
{
	CObj::LateUpdate(fTime);

}

void CTile::Collision(float fTime)
{
	CObj::Collision(fTime);

}

void CTile::PrevRender(HDC hDC, float fTime)
{
	CObj::PrevRender(hDC, fTime);

}

void CTile::Render(HDC hDC, float fTime)
{
	CObj::Render(hDC, fTime);


	Position tCameraPos = GET_SINGLE(CSceneManager)->GetCameraPos();

	RECT tRC = { (int)(m_tPos.x - tCameraPos.x), (int)(m_tPos.y - tCameraPos.y)
		, (int)(m_tPos.x + m_tSize.x - tCameraPos.x), (int)(m_tPos.y + m_tSize.y - tCameraPos.y) };

	switch (m_eOption)
	{
	case TO_NORMAL:

		FrameRect(hDC, &tRC, BLACKBRUSH);


		break;

	case TO_WALL:

		Ellipse(hDC, tRC.left, tRC.top, tRC.right, tRC.bottom);



		break;
	}


	// �⺻������ ��� ������ Ÿ���� ����ϰ� ���� ��쿡 ����Ÿ �������� ����Ѵ�.
}

void CTile::PostRender(HDC hDC, float fTime)
{
	CObj::PostRender(hDC, fTime);

}

CTile * CTile::Clone() const
{
	return new CTile(*this);
}

void CTile::AddClipName(vector<string>& vecClipName)
{
}

void CTile::Save(FILE * pFile)
{

	// ������ ������Ʈ SaveLoad ����°� ���� ����� ���δ�.
	// �ʱ���¸� �����صδ� ���� ���� �ʾ� ���̰�.. �׷��� Ÿ�� ������ ������ �صΰ� ����Ϸ��� �Ѵ�.

	fwrite(&m_tPos, sizeof(Position), 1, pFile);
	fwrite(&m_tSize, sizeof(Size), 1, pFile);
	fwrite(&m_eOption, sizeof(TILE_OPTION), 1, pFile);
}

void CTile::Load(FILE * pFile)
{
	fread(&m_tPos, sizeof(Position), 1, pFile);
	fread(&m_tSize, sizeof(Size), 1, pFile);
	fread(&m_eOption, sizeof(TILE_OPTION), 1, pFile);
}
