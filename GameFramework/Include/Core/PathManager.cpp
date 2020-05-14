#include "PathManager.h"

DEFINITION_SINGLE(CPathManager);

CPathManager::CPathManager()
{
}


CPathManager::~CPathManager()
{
	// ��ο� �����Ҵ��� �迭�� ���� ���̱� ������,
	// �� ������ ������ �Ѵ�.
	Safe_Delete_Array_Map(m_mapPath);
}

bool CPathManager::Init()
{
	// Init������ ���ο� ���ڿ��� ����, �ش� ���ڿ��� �������ϱ����� ��θ� �о�� �Ŀ�,
	// ���������� ������ �����ϰ� �� ��θ� RootPath�� �����Ѵ�.
	// �� ��ο� Texture Ű���带 �ٿ���, Texture���������� ��θ� �߰����ش�.

	TCHAR* pPath = new TCHAR[MAX_PATH];
	memset(pPath, 0, sizeof(TCHAR) * MAX_PATH);

	GetModuleFileName(nullptr, pPath, MAX_PATH);

	// ���� ������ �ڿ������� \\�� ã�Ƽ�
	// �� ������ ���ڿ��� ���־� �Ѵ�.
	// bbb\\aaa.exe
	// 0123 45678910
	// ������ �ϴ� ���ڿ��� (i = 3) 4���� 10���� size = 11 ���־� �ϴ� ������ ��
	// 7��.
	// ��, size - i + 1���� ���־� �Ѵ�.
	// ���̸� �޾ƿ´�.
	int iLength = lstrlen(pPath);

	for (int i = iLength - 1; i >= 0; --i)
	{
		if (pPath[i] == '\\')
		{
			memset(&pPath[i + 1], 0, sizeof(TCHAR) * (iLength - i + 1));
			break;
		}
	}

	// �̷��� �Ͽ�, pPath���� ���������� ������ ���ڿ��� ���̰� ����ȴ�.
	m_mapPath.insert(make_pair(ROOT_PATH, pPath));

	AddPath(TEXTURE_PATH, L"Texture/");
	// RootPath + Texture�� ���ؼ�, ��θ� ���� TEXTURE_PATH�� �̸����� �־��ش�.

	// SoundPath ����.
	AddPath(SOUND_PATH, L"Sound/");


	return true;
}

void CPathManager::AddPath(const string & strName, const TCHAR * pPath, const string & strBasePath)
{
	// ����, strName���� ���� ��ΰ� �̹� �ִٸ�, Ű���� �ߺ��̹Ƿ� �߰����� �ʴ´�.
	if (FindPath(strName))
		return;

	// �׷� ��찡 �ƴ϶��, �迭�� �����Ҵ� �޾Ƽ�, ��θ� �������ش�.
	TCHAR* pFullPath = new TCHAR[MAX_PATH];
	memset(pFullPath, 0, sizeof(TCHAR) * MAX_PATH);

	// �ϴ�, �⺻��θ� �޾ƿ´�.
	const TCHAR* pBasePath = FindPath(strBasePath);

	if (pBasePath)
		lstrcpy(pFullPath, pBasePath);
	// FullPath�� BasePath�� ���� �����Ѵ�.

	lstrcat(pFullPath, pPath);
	// �⺻ ��θ� �߰��� �迭�� pPath�� �ٿ��� ��ü ��θ� ����� �ش�.

	// �̷��� ������ Ǯ ��θ� strName �� Ű������ �������� �Ͽ� �߰��Ѵ�.

	m_mapPath.insert(make_pair(strName, pFullPath));
	
}

const TCHAR * CPathManager::FindPath(const string & strName)
{
	unordered_map<string, TCHAR*>::iterator iter = m_mapPath.find(strName);

	if (iter == m_mapPath.end())
		return nullptr;

	return iter->second;
}

const char * CPathManager::FindPathMultibyte(const string & strName)
{
	const TCHAR* pPath = FindPath(strName);
	// strName�� Ű������ ���� ��θ� ã�ƿͼ�, �ش� ��θ� ��Ƽ����Ʈ�� �ٲ㼭 ��ȯ���ش�.
	
	if (!pPath)
		return nullptr;

	memset(m_strMultibyte, 0, sizeof(char) * MAX_PATH);
	// MultiByte�� ��ȯ�ϱ� ����, �̸� �ش� �迭�� �������ش�.

	// ���̸� �о�´�.
	int iLength = WideCharToMultiByte(CP_ACP, 0, pPath, -1, nullptr, 0,
		nullptr, nullptr);

	// ������ ���� ���̸� �̿��ؼ�, ���ڿ��� ��ȯ�Ѵ�.
	WideCharToMultiByte(CP_ACP, 0, pPath, -1, m_strMultibyte,
		iLength, nullptr, nullptr);

	return m_strMultibyte;

}
