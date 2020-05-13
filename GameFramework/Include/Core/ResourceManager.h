#pragma once

#include "../Game.h"

class CResourceManager
{
	DECLARE_SINGLE(CResourceManager);

	// 리소스 매니저는 Texture를 관리하는 클래스이다.
	// 텍스처의 정보를 읽어오고, 그 정보를 저장하며, 해당 Texture의 정보가 필요한 
	// 해당 Texture를 찾아주는 기능을 포함하고 있다.

private:
	HINSTANCE	m_hInst;
	HDC			m_hDC;
	HWND		m_hWnd;

private:
	unordered_map<string, class CTexture*> m_mapTexture;	// Texture의 정보를 저장하고 있는 map

public:
	bool Init();					// 멤버변수의 값을 셋팅하고 있고,  BackBuffer를 Load하고 있다.
	bool LoadTexture(const string& strName,		
		const TCHAR* pFileName,
		const string& strPathName = TEXTURE_PATH);

	bool LoadTexture(const string& strName,
		const vector<const TCHAR*>& vecFileName,
		const string& strPathName = TEXTURE_PATH);
	// 이미지 경로를 여러개를 넣어주면, 여기 들어있는 경로를 이용하여 이미지를 불러오고,
	// 그 이미지들을 텍스처에 추가하는 함수이다.

	// PathManager와 연합하여, strPathName + pFileName을 통해서 해당 경로로부터 이미지를 읽어온다.
	// 그 이후에, 이 정보를 CTexture* 에 셋팅.
	// 이렇게 만든 클래스를 map에 저장한다.

	void ClearScene();
	void SavePrototype();
	// 구조상, 미리 Scene에 배치가 될 객체들은 ref 값을 3으로 유지하고 있다.
	// 하나는 ClearScene함수를 통해서 감소시킬 값 (실행 하니까)
	// 하나는 Obj객체가 소멸되는 순간에 사라질 값 Obj의 경우에는 Scene에서 한번, Layer에서 한번
	// Scene과 Layer의 경우에는 SceneManager가 Scene을 터트려주면, Layer도 바로 작살남.
	// 따라서 Obj가 사라지는 것은 절대적.
	// 하나는 이 값을 가지고 있는 ResourceManager에서 소멸시킬 값이다. (마지막 정리 용도 + 다시 사용할 용도)

	// 결국에는 다른 관계를 제외하고는 ResourceManager가 ref = 1인 상태로 들고 있는 개념이다.
	// 모든 연결관계가 끈어진 이후에는 Safe_Release_Map을 통한 메모리 정리.

public:
	class CTexture* FindTexture(const string& strName); // strName을 Key로하는 Texture를 찾아오는 함수이다.
};

