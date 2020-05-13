#include "Spawner.h"
#include "../Core/SceneManager.h"
#include "../Object/Monster.h"
#include "../Core/GameManager.h"
#include "../Collision/Collider.h"
#include "../Math.h"
#include "../Object/Player.h"

// 생성할 몬스터에 대한 정보를 받아놓고 이것에 따라서 해당 클래스로 생성을 해줘야 한다.(는 아니지, 왜냐면,
// 해당 클래스만의 함수를 사용할 것이라면 반드시 해당 클래스의 h가 필요한데, 그것이 아니라면
// 자식의 객체를 담아놓고 사용해도 문제는 없지.
// 내부적으로 동적 바인딩을 할 것이니...


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
	// 스포너를 복사할 일은 있다.
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

	// TileCollision false로 따로 들어가지 않는다.
	// 근데, 이게 상관이 없는게 Stage에서 따로 Update를 돌릴 것이다.


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

	// 카메라의 내부에 들어와 있는지 확인한다.

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

	// 플레이어와 너무 가까이에 존재하는 경우에는 충돌되지 않았다고 인식한다.
	/*else if (CMath::GetDist(m_tPos, m_pTarget->GetPos()) < 900.f)
		IsCollision = false;*/


	//// x좌표와 너무 가까이 존재하지 않는지..
	//else if (CMath::GetDist(m_tPos.x, tPlayerPos.x) < 300.f)
	//	IsCollision = false;

	//// y좌표와 너무 가까이 존재하지 않는지..
	//else if (CMath::GetDist(m_tPos.y, tPlayerPos.y) < 500.f)
	//	IsCollision = false;

	

	

	// 만약에 충돌이 일어난 경우에는 true일 것이다.
	m_bCameraCollision = IsCollision;

	if (m_bCameraCollision)
	{
		if (!m_bSpawn)
		{

			// 왼쪽으로 가는 몬스터라면 왼쪽에서 플레이어가 오고 있는 경우에 생성하고,
			// 오른쪽으로 가는 몬스터라면 오른쪽에서 오고 있는 경우에 생성하도록 한다.
			// 즉, 방향이 한쪽으로 정해져 있어서 해당 방향에 반하는 방향으로 오고 있는 경우에 카메라, Spawn의 상대와는 상관없이 생성하지 않는 것이다.

			bool IsDir = false;

			// 플레이어의 위치는 카메라를 기준으로 하기 때문에 카메라의 중심위치만 보아도 확인해볼 수 있다.
			// 왼쪽에서 오고 있는 경우라면, 현재 스포너의 위치보다 카메라의 중심이 왼쪽에 있을 것.
			// 오른쪽에서 오고 있는 경우라면, 현재 스포너의 위치보다 카메라의 중심이 오른쪽에 있을 것이다.
			float fX = tSrc.x + RS_WIDTH / 2.f + 200.f;

			switch (m_eSpawnDir)
			{
			case SD_LEFT:
				// 카메라의 좌표가 왼쪽에 있는 경우만 생성한다.

				if (fX < m_tPos.x)
					IsDir = true;
				else
					m_bSpawn = true;
				break;

			case SD_RIGHT:

				// 카메라의 좌표가 오른쪽에 있는 경우만 생성한다.
				
				if (fX > m_tPos.x)
					IsDir = true;
				else
					m_bSpawn = true;

				break;
			}

			//  만약 반대의 경우로 오고 있는 경우라면, 조건이 true가 되는 시점(즉 플레이어가 중간에 오는 상황에 true값이 들어와서 오브젝트가 생성이 된다.
			// 따라서, 처음부터 오브젝트를 생성하지 않도록 방향이 다르다면 이미 생성을 했다고 true를 줘버리자.
			// 그러면 조건이 맞아도 처리하지 않을 것이다.

			// 카메라와 막 충돌이 된 경우이다.
			// 몬스터를 생성해준다.

			if (IsDir)
			{

				bool IsCreate = true;

				if (m_bCount)
				{
					// 사용하는 경우라면 지금 생성해야하는 개수를 확인하고 한계치라면 넘긴다.
					if (m_iCount <= m_pScene->GetObjectCount(m_strSpawnMonsterTag))
					{
						IsCreate = false;
					}

				}

				// 사용하지 않는 경우라면 그냥 생성한다.
				// 생성을 하는데, 카메라가 흔들리는 경우에 막 생기는 상황을 막는다.
				Position tPlayerPos = m_pTarget->GetPos();



				if (IsCreate && !GET_SINGLE(CSceneManager)->GetSway())
				{

					CMonster* pMonster = CObj::CreateCloneObject<CMonster>(m_strSpawnMonsterTag, m_strSpawnMonsterTag,
						"Default", m_pScene);
					pMonster->SetDir(m_eSpawnDir);
					pMonster->SetPos(m_tPos);


					// 몬스터가 잘 생성이 되어 배치가 될 것이다.
					m_bSpawn = true;
				
				}



			}
		}
		//else
		//{
		//	// 카메라와 충돌이 된 상태로 한번의 spawn을 거쳤다는 의미이다. 아무 것도 안 하면 된다.
		//}
	}
	else
	{
		// 충돌이 나지 않았다면 spawn을 false로 준다.
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
