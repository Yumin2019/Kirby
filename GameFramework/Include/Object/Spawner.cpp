#include "Spawner.h"
#include "../Core/SceneManager.h"
#include "../Object/Monster.h"
#include "../Core/GameManager.h"
#include "../Collision/Collider.h"
#include "../Math.h"
#include "../Object/Player.h"

// ������ ���Ϳ� ���� ������ �޾Ƴ��� �̰Ϳ� ���� �ش� Ŭ������ ������ ����� �Ѵ�.(�� �ƴ���, �ֳĸ�,
// �ش� Ŭ�������� �Լ��� ����� ���̶�� �ݵ�� �ش� Ŭ������ h�� �ʿ��ѵ�, �װ��� �ƴ϶��
// �ڽ��� ��ü�� ��Ƴ��� ����ص� ������ ����.
// ���������� ���� ���ε��� �� ���̴�...


CSpawner::CSpawner()
{
	m_pTarget = nullptr;
	m_eSpawnDir = SD_NONE;
	m_bCameraCollision = false;
	m_bSpawn = false;
	m_iCount = INT_MAX;
	m_bCount = false;
}


CSpawner::CSpawner(const CSpawner & obj) :
	CObj(obj)
{
	// �����ʸ� ������ ���� �ִ�.
	m_eSpawnDir = obj.m_eSpawnDir;
	m_bCameraCollision = false;
	m_bSpawn = false;
	m_strSpawnMonsterTag = obj.m_strSpawnMonsterTag;

	m_iCount = obj.m_iCount;
	m_bCount = obj.m_bCount;

}

CSpawner::~CSpawner()
{
	SAFE_RELEASE(m_pTarget);
}

void CSpawner::SetSpawner(float x, float y, 
	SIDE_DIR eSpawnDir, const string & strSpawnMonsterTag,
	bool bCount, int iCount)
{
	m_tPos = Position(x, y);
	m_eSpawnDir = eSpawnDir;
	m_strSpawnMonsterTag = strSpawnMonsterTag;
	m_iCount = iCount;
	m_bCount = bCount;

}


void CSpawner::Start()
{
	CObj::Start();

	// TileCollision false�� ���� ���� �ʴ´�.
	// �ٵ�, �̰� ����� ���°� Stage���� ���� Update�� ���� ���̴�.


}

bool CSpawner::Init()
{
	if (!CObj::Init())
		return false;

	return true;
}

void CSpawner::Update(float fTime)
{
	CObj::Update(fTime);

	// ī�޶��� ���ο� ���� �ִ��� Ȯ���Ѵ�.

	m_tCameraSrc = GET_SINGLE(CSceneManager)->GetCameraPos();

	Position tSrc = m_tCameraSrc - Position(200.f, 200.f);
	Position tDest = tSrc + Position(RS_WIDTH + 400.f, RS_HEIGHT + 400.f);

	bool IsCollision = true;

	if (tSrc.x > m_tPos.x)
		IsCollision = false;
	else if (tDest.x < m_tPos.x)
		IsCollision = false;
	else if (tSrc.y > m_tPos.y)
		IsCollision = false;
	else if (tDest.y < m_tPos.y)
		IsCollision = false;

	// �÷��̾�� �ʹ� �����̿� �����ϴ� ��쿡�� �浹���� �ʾҴٰ� �ν��Ѵ�.
	/*else if (CMath::GetDist(m_tPos, m_pTarget->GetPos()) < 900.f)
		IsCollision = false;*/


	//// x��ǥ�� �ʹ� ������ �������� �ʴ���..
	//else if (CMath::GetDist(m_tPos.x, tPlayerPos.x) < 300.f)
	//	IsCollision = false;

	//// y��ǥ�� �ʹ� ������ �������� �ʴ���..
	//else if (CMath::GetDist(m_tPos.y, tPlayerPos.y) < 500.f)
	//	IsCollision = false;

	

	

	// ���࿡ �浹�� �Ͼ ��쿡�� true�� ���̴�.
	m_bCameraCollision = IsCollision;

	if (m_bCameraCollision)
	{
		if (!m_bSpawn)
		{

			// �������� ���� ���Ͷ�� ���ʿ��� �÷��̾ ���� �ִ� ��쿡 �����ϰ�,
			// ���������� ���� ���Ͷ�� �����ʿ��� ���� �ִ� ��쿡 �����ϵ��� �Ѵ�.
			// ��, ������ �������� ������ �־ �ش� ���⿡ ���ϴ� �������� ���� �ִ� ��쿡 ī�޶�, Spawn�� ���ʹ� ������� �������� �ʴ� ���̴�.

			bool IsDir = false;

			// �÷��̾��� ��ġ�� ī�޶� �������� �ϱ� ������ ī�޶��� �߽���ġ�� ���Ƶ� Ȯ���غ� �� �ִ�.
			// ���ʿ��� ���� �ִ� �����, ���� �������� ��ġ���� ī�޶��� �߽��� ���ʿ� ���� ��.
			// �����ʿ��� ���� �ִ� �����, ���� �������� ��ġ���� ī�޶��� �߽��� �����ʿ� ���� ���̴�.
			float fX = tSrc.x + RS_WIDTH / 2.f + 200.f;

			switch (m_eSpawnDir)
			{
			case SD_LEFT:
				// ī�޶��� ��ǥ�� ���ʿ� �ִ� ��츸 �����Ѵ�.

				if (fX < m_tPos.x)
					IsDir = true;
				else
					m_bSpawn = true;
				break;

			case SD_RIGHT:

				// ī�޶��� ��ǥ�� �����ʿ� �ִ� ��츸 �����Ѵ�.
				
				if (fX > m_tPos.x)
					IsDir = true;
				else
					m_bSpawn = true;

				break;
			}

			//  ���� �ݴ��� ���� ���� �ִ� �����, ������ true�� �Ǵ� ����(�� �÷��̾ �߰��� ���� ��Ȳ�� true���� ���ͼ� ������Ʈ�� ������ �ȴ�.
			// ����, ó������ ������Ʈ�� �������� �ʵ��� ������ �ٸ��ٸ� �̹� ������ �ߴٰ� true�� �������.
			// �׷��� ������ �¾Ƶ� ó������ ���� ���̴�.

			// ī�޶�� �� �浹�� �� ����̴�.
			// ���͸� �������ش�.

			if (IsDir)
			{

				bool IsCreate = true;

				if (m_bCount)
				{
					// ����ϴ� ����� ���� �����ؾ��ϴ� ������ Ȯ���ϰ� �Ѱ�ġ��� �ѱ��.
					if (m_iCount <= m_pScene->GetObjectCount(m_strSpawnMonsterTag))
					{
						IsCreate = false;
					}

				}

				// ������� �ʴ� ����� �׳� �����Ѵ�.
				// ������ �ϴµ�, ī�޶� ��鸮�� ��쿡 �� ����� ��Ȳ�� ���´�.
				Position tPlayerPos = m_pTarget->GetPos();



				if (IsCreate && !GET_SINGLE(CSceneManager)->GetSway())
				{

					CMonster* pMonster = CObj::CreateCloneObject<CMonster>(m_strSpawnMonsterTag, m_strSpawnMonsterTag,
						"Default", m_pScene);
					pMonster->SetDir(m_eSpawnDir);
					pMonster->SetPos(m_tPos);


					// ���Ͱ� �� ������ �Ǿ� ��ġ�� �� ���̴�.
					m_bSpawn = true;
				
				}



			}
		}
		//else
		//{
		//	// ī�޶�� �浹�� �� ���·� �ѹ��� spawn�� ���ƴٴ� �ǹ��̴�. �ƹ� �͵� �� �ϸ� �ȴ�.
		//}
	}
	else
	{
		// �浹�� ���� �ʾҴٸ� spawn�� false�� �ش�.
		m_bSpawn = false;
	}
}

void CSpawner::LateUpdate(float fTime)
{
	CObj::LateUpdate(fTime);

}

void CSpawner::Collision(float fTime)
{
	CObj::Collision(fTime);

}

void CSpawner::PrevRender(HDC hDC, float fTime)
{
	CObj::PrevRender(hDC, fTime);

}

void CSpawner::Render(HDC hDC, float fTime)
{
	CObj::Render(hDC, fTime);

#ifdef _DEBUG

	int iX = (int)(m_tPos.x - m_tCameraSrc.x);
	int iY = (int)(m_tPos.y - m_tCameraSrc.y);
	RECT tRect = { iX - 50, iY - 50, iX + 50, iY + 50 };

	FrameRect(hDC, &tRect, MAGENTABRUSH);
#endif

}

void CSpawner::PostRender(HDC hDC, float fTime)
{
	CObj::PostRender(hDC, fTime);

}

CSpawner * CSpawner::Clone() const
{
	return new CSpawner(*this);
}

void CSpawner::AddClipName(vector<string>& vecClipName)
{
}
