#pragma once

#include "../Game.h"

class CPathManager
{
	DECLARE_SINGLE(CPathManager);

private:
	unordered_map<string, TCHAR*> m_mapPath;	// 경로를 담아둘 map
	char		m_strMultibyte[MAX_PATH];		// 경로를 멀티바이트를 기준으로 반환하는데 사용할 배열이다.

public:
	bool Init();					// RootPath와 Texture Path를 추가해주고 있다.
	void AddPath(const string& strName,	
		const TCHAR* pPath,
		const string& strBasePath = ROOT_PATH);

	// strBasePath의 값을 가지고, m_mapPath에서 경로를 찾아와서 
	// 이것을 기본 경로로 삼는다.
	// 여기에, pPath를 덧붙여서, strName를 Key 로 가지는 경로를 만들어
	// map에 저장한다.

	// TCHAR* 로 반환하지 않고 const TCHAR*로 받는 이유는 해당 포인터를 가져와서 값을 바꾸지 못하도록 할 것이기 때문이다.
	// 이미 넣은 값을 보호하기 위함이다.
	const TCHAR* FindPath(const string& strName);
	const char* FindPathMultibyte(const string& strName);

};

