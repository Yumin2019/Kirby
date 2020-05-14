#pragma once

#include "../Game.h"

class CPathManager
{
	DECLARE_SINGLE(CPathManager);

private:
	unordered_map<string, TCHAR*> m_mapPath;	// ��θ� ��Ƶ� map
	char		m_strMultibyte[MAX_PATH];		// ��θ� ��Ƽ����Ʈ�� �������� ��ȯ�ϴµ� ����� �迭�̴�.

public:
	bool Init();					// RootPath�� Texture Path�� �߰����ְ� �ִ�.
	void AddPath(const string& strName,	
		const TCHAR* pPath,
		const string& strBasePath = ROOT_PATH);

	// strBasePath�� ���� ������, m_mapPath���� ��θ� ã�ƿͼ� 
	// �̰��� �⺻ ��η� ��´�.
	// ���⿡, pPath�� ���ٿ���, strName�� Key �� ������ ��θ� �����
	// map�� �����Ѵ�.

	// TCHAR* �� ��ȯ���� �ʰ� const TCHAR*�� �޴� ������ �ش� �����͸� �����ͼ� ���� �ٲ��� ���ϵ��� �� ���̱� �����̴�.
	// �̹� ���� ���� ��ȣ�ϱ� �����̴�.
	const TCHAR* FindPath(const string& strName);
	const char* FindPathMultibyte(const string& strName);

};

