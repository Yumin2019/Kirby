#include "ResourceManager.h"
#include "GameManager.h"
#include "SceneManager.h"
#include "../Texture.h"

DEFINITION_SINGLE(CResourceManager);


CResourceManager::CResourceManager()
{
}


CResourceManager::~CResourceManager()
{
	Safe_Release_Map(m_mapTexture);
	// ��� ���谡 ������ ���Ŀ� ���� �ؽ�ó�� �����Ѵ�.
}

bool CResourceManager::Init()
{
	m_hInst = WINDOWINSTANCE;
	m_hWnd = WINDOWHANDLE;
	m_hDC = WINDOWDC;

	LoadTexture("BackBuffer", TEXT("BackBuffer.bmp"));
	// ���⼭ BackBuffer�� ������ �ִ�.
	// BackBuffer�� map�� �� �ְ� ref = 1�̴�.

	return true;
}

bool CResourceManager::LoadTexture(const string & strName, 
	const TCHAR * pFileName, 
	const string & strPathName)
{
	CTexture* pTexture = FindTexture(strName);

	if (pTexture)
	{

		// �̹� strName�� ���� �ؽ�ó�� �ִٴ� �ǹ��̴�.
		// �̹����� �ε����� �ʴ´�.
		SAFE_RELEASE(pTexture);
		// ���۷��� ī���͸� �ٽ� �����д�.
		return false;
	}

	// �ش� �̸��� ���� Texture�� �������� �ʴ´�.
	// ���� ������ ���ش�.

	pTexture = new CTexture;

	if (!pTexture->LoadTexture(strName, pFileName))
	{
		// TextureŬ������ ���������� PathManager�� ���ؼ� �̹����� �о�� ��θ� �����,
		// �о�� ������ Texture�� �����Ѵ�.

		// �о�� Texture�� ������ ���� ���..
		SAFE_RELEASE(pTexture); // = SAFE_DELETE
		return false;
	}

	if (GET_SINGLE(CSceneManager)->CreateSceneEnable())
	{
		// Scene�� �����ϰ� �ִ� ��쿡 �ش� LoadTexture�Լ��� ȣ���� �� ��쿡��
		// Scene�� Start�Լ��� ���ؼ� map�� �ִ� Texture������ release (��Ȯ���� ClearScene) ���ְ� �ִ�
		// ����, �̸� �������� �д�.
		pTexture->AddRef();
	}

	// Texture�� LoadTexture�Լ��� ���ؼ� �������� ȣ���� �ؼ�, �ؽ�ó�� ������ �о�� ���� ������, 
	// �׷���, ���� ResourceManager�� �־�� �Ѵٴ� ���ŷο��� �ִ�.

	// ����, �̷� �Ϸ��� ������ ResourceManager���� ó���ϰ��� �ϴ� ���̴�.
	// LoadTexture���� �ʿ��� ������ ResourceManager�� LoadTexture�Լ��� �־������ν�
	// Texture�� �Լ��� �������� ������ �� �� �ֵ��� �ϰ�, ResourceManager���� �ش� �ؽ�ó�� �־��ִ� �������� �ѹ��� ó���ϴ� ���̴�.

	// ����� Texture�� ������ �� ��쿡��, strName���� Ű�� ���� �ؽ�ó Ŭ������  map�� �߰��Ѵ�.

	m_mapTexture.insert(make_pair(strName, pTexture));

	// ����� ���� ������ �Ǵ� Texture�� ������ ������ (�̸���)
	// ref = 1�� �����̴�. ��, m_mapTexture�� �ִ�.

	return true;
}

bool CResourceManager::LoadTexture(const string & strName,
	const vector<const TCHAR*>& vecFileName, const string & strPathName)
{
	// �������� DC�� ���� �ؽ�ó�� �߰��ϴ� �Լ��̴�.

	CTexture* pTexture = FindTexture(strName);

	if (pTexture)
	{
		SAFE_RELEASE(pTexture);
		return false;
	}

	// �̹� ���� �̸��� ���� �ؽ�ó�� �ִ� ���.

	pTexture = new CTexture;

	if (!pTexture->LoadTexture(strName, vecFileName, strPathName))
	{
		// ����������, �Ҵ�� �ؽ�ó�̹���.
		SAFE_RELEASE(pTexture);
		return false;
	}

	// vecFileName�� �޴� LoadTexture�Լ��� ����Ͽ�, �������� DC�� ���� �ؽ�ó�� �����.

	if (GET_SINGLE(CSceneManager)->CreateSceneEnable())
		pTexture->AddRef();

	
	m_mapTexture.insert(make_pair(strName, pTexture));
	// �ؽ�ó �ʿ� �߰��Ѵ�.

	return true;
}

void CResourceManager::ClearScene()
{
	FindTexture("BackBuffer");
	FindTexture("MouseDefault");


	// BackBuffer�� ���  ref���� �������Ѿ� �Ѵ�.
	// ���� �� ����

	unordered_map<string, CTexture*>::iterator iter;
	unordered_map<string, CTexture*>::iterator iterEnd = m_mapTexture.end();

	for (iter = m_mapTexture.begin(); iter != iterEnd;)
	{
		if (iter->second->Release() == 0)
		{
			// map�� �ִ� ��� ������ release�ϴ� �ش� �ؽ�ó�� �Ҹ��� �Ǿ��ٸ�. �����.
			iter = m_mapTexture.erase(iter);
			iterEnd = m_mapTexture.end();
			continue;
		}

		++iter;
		// �������� ���� ��쿡�� ���� .
	}
}

void CResourceManager::SavePrototype()
{

	// ������Ÿ���� ������� ���� �����ϱ� ���� �Լ��̴�.
	// ������Ÿ������ ����� ��ü�� ���ҽ��� �̰����� ������ ����� �Ѵ�.

	FindTexture("BlueBall");
	FindTexture("AbilityMonsterLeft");
	FindTexture("AbilityMonsterRight");
	FindTexture("KirbyRunEffectLeft");
	FindTexture("KirbyRunEffectRight");
	FindTexture("NormalMonsterLeft");
	FindTexture("NormalMonsterRight");

	FindTexture("KirbyAttackEffectLeft");
	FindTexture("KirbyAttackEffectRight");
	FindTexture("KirbyBreathAttackEffectLeft");
	FindTexture("KirbyBreathAttackEffectRight");
	FindTexture("KirbyStarEffectLeft");
	FindTexture("KirbyStarEffectRight");

	FindTexture("SmallStar");
	FindTexture("StoneKirbyLeft");
	FindTexture("StoneKirbyRight");
	FindTexture("SwordKirbyLeft");
	FindTexture("SwordKirbyRight");
	FindTexture("Door");
	FindTexture("KirbyLeft");
	FindTexture("KirbyRight");
	FindTexture("EasyKirbyLeft");
	FindTexture("EasyKirbyRight");
	FindTexture("MikeObject");
	FindTexture("AbilityName");
	FindTexture("HPHud");
	FindTexture("Star");

	// ������Ÿ���� �ƴ����� ��� ��� �ϴ� ������Ʈ
	FindTexture("BossHP");
	FindTexture("BossHPBorder");
	FindTexture("BackGround");
	FindTexture("ForeGround"); // ��� ���� ��ü��.
}


CTexture * CResourceManager::FindTexture(const string & strName)
{
	unordered_map<string, CTexture*>::iterator iter = m_mapTexture.find(strName);

	if (iter == m_mapTexture.end())
		return nullptr;

	iter->second->AddRef();
	// ����

	return iter->second;
}
