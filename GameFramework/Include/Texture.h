#pragma once
#include "Ref.h"
class CTexture :
	public CRef
{


	friend class CObj;
	friend class CResourceManager;
	// 외부의 생성을 막고, ResourceManager를 통해서만 생성할 수 있도록 한다.

private:
	CTexture();
	~CTexture();

private:
	vector<HDC>		m_vecDC; // 메모리 DC 여러개.
	vector<HBITMAP> m_vecBmp; // 현재 사용할 비트맵 정보 당연히 여러개
	vector<HBITMAP>	m_vecPrevBmp; // 이전에 사용한 비트맵 당연히 여러개
	vector<BITMAP>	m_vecBmpInfo; // 비트맵 정보저장 구조체 당연히 여러개
	unsigned int		m_iColorKey; // 컬러키 지정
	bool				m_bColorKey; // 컬러키의 사용여부

	vector<const TCHAR*>	m_vecFileName;
	string			m_strPathName;


public:
	HDC GetDC(int iFrame = 0) const
	{
		// 기본적으로 아틀라스라고 가정하고, 구현이 되어 있다.
		return m_vecDC[iFrame];
	}



	HBITMAP GetBmp(int iFrame = 0) const
	{
		// 현재 iFrame에 해당하는 비트맵핸들을 반환
		return m_vecBmp[iFrame];
	}

	BITMAP GetBitmapInfo(int iFrame = 0) const
	{
		return m_vecBmpInfo[iFrame];
	}

	void SetColorKey(unsigned char r, unsigned char g, unsigned char b)
	{
		m_iColorKey = RGB(r, g, b);
		m_bColorKey = true;
	}
	
	void SetColorKey(unsigned int iColorKey)
	{
		m_iColorKey = iColorKey;
		m_bColorKey = true;
	}




public:
	bool LoadTexture(const string& strName,
		const TCHAR* pFileName,
		const string& strPathName = TEXTURE_PATH);
	// 텍스처를 읽어오는 함수이다.
	//  strPathName의 경로 + pFileName의 경로를 합한 경로를 가지고, Texture를 읽어온다.
	// 여기서 strPathName을 이용하기 위해서, PathManager를 이용한다.

	bool LoadTexture(const string& strName,
		const vector<const TCHAR*>& vecFileName,
		const string& strPathName = TEXTURE_PATH);

	// vecFileName 을 넣어주면, 여기에 해당하는 이미지 파일을 모두 읽어주는 함수이다.
	// Texture는 이름이 같아도 중복인식을 하지는 않는다.
	// 단지 Tag만 중복으로 주는 것. 따라서 해당 함수를 호출하면, 
	// 텍스처는 여러개의 DC를 가지고 있게 된다.
	
	void Render(HDC hDC, int iDestX, int iDestY, int iSrcX, int iSrcY,
		int iSizeX, int iSizeY, int iFrame = 0);
	void Render(HDC hDC, const Position& tDestPos,
		const Position& tSrcPos, const Size& tSize, int iFrame = 0);

	// DC가 여러개이므로 기본적은 0 인덱스로 설정.

	// 컬러키 지정방식을 해주고 있지 않다는 것은
	// 텍스처를 단순히 고속 복사 해주고 있다는 것이다.
	// TransparentBlt함수를 통해서 ColorKey를 사용하는 함수는 추가될 예정이며
	// 현재는 BitBlt함수를 통한 고속 복사만을 하고 있다.

	bool Load(FILE* pFile);
	bool Save(FILE* pFile);
};

