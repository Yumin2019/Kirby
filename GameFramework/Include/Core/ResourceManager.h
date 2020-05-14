#pragma once

#include "../Game.h"

class CResourceManager
{
	DECLARE_SINGLE(CResourceManager);

	// ���ҽ� �Ŵ����� Texture�� �����ϴ� Ŭ�����̴�.
	// �ؽ�ó�� ������ �о����, �� ������ �����ϸ�, �ش� Texture�� ������ �ʿ��� 
	// �ش� Texture�� ã���ִ� ����� �����ϰ� �ִ�.

private:
	HINSTANCE	m_hInst;
	HDC			m_hDC;
	HWND		m_hWnd;

private:
	unordered_map<string, class CTexture*> m_mapTexture;	// Texture�� ������ �����ϰ� �ִ� map

public:
	bool Init();					// ��������� ���� �����ϰ� �ְ�,  BackBuffer�� Load�ϰ� �ִ�.
	bool LoadTexture(const string& strName,		
		const TCHAR* pFileName,
		const string& strPathName = TEXTURE_PATH);

	bool LoadTexture(const string& strName,
		const vector<const TCHAR*>& vecFileName,
		const string& strPathName = TEXTURE_PATH);
	// �̹��� ��θ� �������� �־��ָ�, ���� ����ִ� ��θ� �̿��Ͽ� �̹����� �ҷ�����,
	// �� �̹������� �ؽ�ó�� �߰��ϴ� �Լ��̴�.

	// PathManager�� �����Ͽ�, strPathName + pFileName�� ���ؼ� �ش� ��ηκ��� �̹����� �о�´�.
	// �� ���Ŀ�, �� ������ CTexture* �� ����.
	// �̷��� ���� Ŭ������ map�� �����Ѵ�.

	void ClearScene();
	void SavePrototype();
	// ������, �̸� Scene�� ��ġ�� �� ��ü���� ref ���� 3���� �����ϰ� �ִ�.
	// �ϳ��� ClearScene�Լ��� ���ؼ� ���ҽ�ų �� (���� �ϴϱ�)
	// �ϳ��� Obj��ü�� �Ҹ�Ǵ� ������ ����� �� Obj�� ��쿡�� Scene���� �ѹ�, Layer���� �ѹ�
	// Scene�� Layer�� ��쿡�� SceneManager�� Scene�� ��Ʈ���ָ�, Layer�� �ٷ� �ۻ쳲.
	// ���� Obj�� ������� ���� ������.
	// �ϳ��� �� ���� ������ �ִ� ResourceManager���� �Ҹ��ų ���̴�. (������ ���� �뵵 + �ٽ� ����� �뵵)

	// �ᱹ���� �ٸ� ���踦 �����ϰ�� ResourceManager�� ref = 1�� ���·� ��� �ִ� �����̴�.
	// ��� ������谡 ������ ���Ŀ��� Safe_Release_Map�� ���� �޸� ����.

public:
	class CTexture* FindTexture(const string& strName); // strName�� Key���ϴ� Texture�� ã�ƿ��� �Լ��̴�.
};

