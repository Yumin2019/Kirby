#include "PathManager.h"

DEFINITION_SINGLE(CPathManager);

CPathManager::CPathManager()
{
}


CPathManager::~CPathManager()
{
	// 경로에 동적할당한 배열을 넣을 것이기 때문에,
	// 이 정보를 날려야 한다.
	Safe_Delete_Array_Map(m_mapPath);
}

bool CPathManager::Init()
{
	// Init에서는 새로운 문자열을 만들어서, 해당 문자열에 실행파일까지의 경로를 읽어온 후에,
	// 실행파일의 정보를 제거하고 이 경로를 RootPath로 설정한다.
	// 이 경로에 Texture 키워드를 붙여서, Texture폴더까지의 경로를 추가해준다.

	TCHAR* pPath = new TCHAR[MAX_PATH];
	memset(pPath, 0, sizeof(TCHAR) * MAX_PATH);

	GetModuleFileName(nullptr, pPath, MAX_PATH);

	// 실행 파일의 뒤에서부터 \\를 찾아서
	// 그 이후의 문자열을 없애야 한다.
	// bbb\\aaa.exe
	// 0123 45678910
	// 날려야 하는 문자열은 (i = 3) 4부터 10까지 size = 11 없애야 하는 문자의 수
	// 7개.
	// 즉, size - i + 1개를 없애야 한다.
	// 길이를 받아온다.
	int iLength = lstrlen(pPath);

	for (int i = iLength - 1; i >= 0; --i)
	{
		if (pPath[i] == '\\')
		{
			memset(&pPath[i + 1], 0, sizeof(TCHAR) * (iLength - i + 1));
			break;
		}
	}

	// 이렇게 하여, pPath에는 실행파일을 제외한 문자열의 길이가 저장된다.
	m_mapPath.insert(make_pair(ROOT_PATH, pPath));

	AddPath(TEXTURE_PATH, L"Texture/");
	// RootPath + Texture를 통해서, 경로를 만들어서 TEXTURE_PATH의 이름으로 넣어준다.

	// SoundPath 생성.
	AddPath(SOUND_PATH, L"Sound/");


	return true;
}

void CPathManager::AddPath(const string & strName, const TCHAR * pPath, const string & strBasePath)
{
	// 만약, strName값을 가진 경로가 이미 있다면, 키값이 중복이므로 추가하지 않는다.
	if (FindPath(strName))
		return;

	// 그런 경우가 아니라면, 배열을 동적할당 받아서, 경로를 저장해준다.
	TCHAR* pFullPath = new TCHAR[MAX_PATH];
	memset(pFullPath, 0, sizeof(TCHAR) * MAX_PATH);

	// 일단, 기본경로를 받아온다.
	const TCHAR* pBasePath = FindPath(strBasePath);

	if (pBasePath)
		lstrcpy(pFullPath, pBasePath);
	// FullPath로 BasePath의 값을 복사한다.

	lstrcat(pFullPath, pPath);
	// 기본 경로를 추가한 배열에 pPath를 붙여서 전체 경로를 만들어 준다.

	// 이렇게 생성한 풀 경로를 strName 을 키값으로 가지도록 하여 추가한다.

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
	// strName을 키값으로 가진 경로를 찾아와서, 해당 경로를 멀티바이트로 바꿔서 반환해준다.
	
	if (!pPath)
		return nullptr;

	memset(m_strMultibyte, 0, sizeof(char) * MAX_PATH);
	// MultiByte로 반환하기 전에, 미리 해당 배열을 정리해준다.

	// 길이를 읽어온다.
	int iLength = WideCharToMultiByte(CP_ACP, 0, pPath, -1, nullptr, 0,
		nullptr, nullptr);

	// 위에서 얻어온 길이를 이용해서, 문자열을 변환한다.
	WideCharToMultiByte(CP_ACP, 0, pPath, -1, m_strMultibyte,
		iLength, nullptr, nullptr);

	return m_strMultibyte;

}
