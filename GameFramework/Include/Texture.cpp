#include "Texture.h"
#include "Core/PathManager.h"
#include "Core/GameManager.h"

CTexture::CTexture()
{
	m_bColorKey = false;
	m_iColorKey = 0;
}


CTexture::~CTexture()
{
	for (size_t i = 0; i < m_vecDC.size(); ++i)
	{
		// DC에 담긴 사이즈만큼, 각 DC에서 기존에 설정해두었던, 비트맵 정보를 다시 복구하고,
		// DC를 날리며 생성했던 비트맵 정보를 날려주는 처리를 한다.

		// 기존에 설정해둔 비트맵 정보를 다시 셋팅한다.
		SelectObject(m_vecDC[i], m_vecPrevBmp[i]);

		// 비트맵 정보를 날린다.
		DeleteObject(m_vecBmp[i]);
		
		// DC정보를 날려준다.
		DeleteObject(m_vecDC[i]);

	}

}

bool CTexture::LoadTexture(const string & strName, 
	const TCHAR * pFileName, 
	const string & strPathName)
{
	SetTag(strName);
	// 태그 설정.

	// 풀경로를 만들어준다.
	TCHAR pFullPath[MAX_PATH] = {};

	const TCHAR* pBasePath = GET_SINGLE(CPathManager)->FindPath(strPathName);

	// 기존의 경로를 붙인다.
	if (pBasePath)
		lstrcpy(pFullPath, pBasePath);

	// 파일의 이름을 붙여준다.
	lstrcat(pFullPath, pFileName);

	m_vecFileName.push_back(pFullPath);

	m_strPathName = strPathName;

	// 해당 경로를 가지고 비트맵 정보를 읽어온다.
	
	// 화면 DC와 호환이 되는 메모리 DC를 생성한다.
	HDC hDC = CreateCompatibleDC(WINDOWDC);

	// 비트맵 정보를 읽어온다.
	HBITMAP m_hBmp = (HBITMAP)LoadImage(WINDOWINSTANCE, pFullPath, IMAGE_BITMAP,
		0, 0, LR_LOADFROMFILE);

	// 새로운 비트맵을 설정하고 이전 비트맵을 받아둔다.
	HBITMAP m_hPrevBmp = (HBITMAP)SelectObject(hDC, m_hBmp);

	BITMAP tInfo;

	// 비트맵Info를 저장한다.
	GetObject(m_hBmp, sizeof(BITMAP), &tInfo);

	// Info 정보와, 생성한 DC, 이전 비트맵 정보와, 설정한 비트맵 정보를 만든 후에
	// 벡터에 넣어준다.

	m_vecDC.push_back(hDC);
	m_vecBmp.push_back(m_hBmp);
	m_vecPrevBmp.push_back(m_hPrevBmp);
	m_vecBmpInfo.push_back(tInfo);

	return true;
}

bool CTexture::LoadTexture(const string & strName, const vector<const TCHAR*>& vecFileName,
	const string & strPathName)
{
	for (size_t i = 0; i < vecFileName.size(); ++i)
	{
		if (!LoadTexture(strName, vecFileName[i], strPathName))
			return false;
		
	}

	// 이렇게 vecFileName에 넣어둔 경로를 읽어와서 여러개의 DC 를 생성해주는 작업을 하고 있다.
	// 이 함수는 프레임 애니메이션 전용인 것이다.

	return true;
}

void CTexture::Render(HDC hDC,
	int iDestX, int iDestY,
	int iSrcX, int iSrcY,
	int iSizeX, int iSizeY, int iFrame)
{

	if (!m_bColorKey)
		BitBlt(hDC, iDestX, iDestY, iSizeX, iSizeY,
			m_vecDC[iFrame], iSrcX, iSrcY, SRCCOPY);

	else
		TransparentBlt(hDC, iDestX, iDestY, iSizeX, iSizeY,
			m_vecDC[iFrame], iSrcX, iSrcY, iSizeX, iSizeY,
			m_iColorKey);
		// 컬러키가 지정되어 있다면,  TransparentBlt 함수를 이용한다.

	// 첫번째 인자는 출력할 DC를 입력한다.
	// 2,3 번째 인자는 출력할 DC에서의 X, Y 좌표이다.
	// 4, 5번째 인자는 출력할 대상의 Size를 의미한다. 
	// 6번째 인자는 출력할 대상의 DC를 넣어준다
	// 7, 8 번째 인자는 출력할 대상 이미지에서의 시작 좌표이다.

	// BitBlt함수는 인자가 이렇게 구성이 되어 있지만, TransparentBlt함수의 경우에는 더 많은 인자를 필요로 한다.

	/*
	BOOL TransparentBlt(
    HDC hdcDest,				// 출력 DC
    int xoriginDest,			// 출력 X좌표
    int yoriginDest,			// 출력 Y좌표
    int wDest,					// 출력 너비
    int hDest,					// 출력 높이   hdcSrc로부터 받은 이미지를 이 너비와 높이에 맞게 이미지 크기를 변경합니다.
    HDC hdcSrc,					// 그림 DC
    int xoriginSrc,				// 그림에서의 X
    int yoriginSrc,				// 그림에서의 Y 
    int wSrc,					// 그림 너비 이 크기만큼 원본 이미지에서 잘라내어 wDest, hDest의 크기에 맞게 이미지 크기를 변경합니다.
    int hSrc,					// 그림 높이 
    UINT crTransparent);		// ColorKey RGB매크로를 이용한다.
	*/


}

void CTexture::Render(HDC hDC,
	const Position & tDestPos,
	const Position & tSrcPos,
	const Size & tSize, int iFrame)
{

	if(!m_bColorKey)
		BitBlt(hDC, (int)tDestPos.x, (int)tDestPos.y,
		(int)tSize.x, (int)tSize.y, m_vecDC[iFrame], (int)tSrcPos.x, (int)tSrcPos.y, SRCCOPY);

	else
		TransparentBlt(hDC, (int)tDestPos.x, (int)tDestPos.y, (int)tSize.x, (int)tSize.y,
		m_vecDC[iFrame], (int)tSrcPos.x, (int)tSrcPos.y, (int)tSize.x, (int)tSize.y,
		m_iColorKey);

	// src에서 크기를 출력해주는 부분이 있는데, 이 부분은 건드리지 않는다.
	// 픽셀을 늘리는 순간 깨진다.

}


bool CTexture::Load(FILE * pFile)
{

	//// 현재 텍스처의 이름을 읽어온다.
	//int iLength = 0;
	//fread(&iLength, sizeof(int), 1, pFile);
	//fread(m_strTag.c_str(), sizeof(char), iLength, pFile);

	//// 파일 경로를 저장한다.
	//size_t iSize = m_vecFileName.size();

	//fread(&iSize, sizeof(size_t), 1, pFile);

	//for (size_t i = 0; i < iSize; ++i)
	//{
	//	iLength = lstrlen(m_vecFileName[i]);
	//	fread(&iLength, sizeof(int), 1, pFile);
	//	fread(m_vecFileName[i], sizeof(char), iLength, pFile);
	//}

	//// PathKey를 저장한다
	//iLength = m_strPathName.length();
	//fread(&iLength, sizeof(int), 1, pFile);
	//fread(m_strPathName.c_str(), sizeof(char), iLength, pFile);

	fread(&m_iColorKey, sizeof(unsigned int), 1, pFile);
	fread(&m_bColorKey, sizeof(bool), 1, pFile);


	return true;
;
}

bool CTexture::Save(FILE * pFile)
{


	// 현재 텍스처의 이름을 저장한다.
	int iLength = m_strTag.length();
	fwrite(&iLength, sizeof(int), 1, pFile);
	fwrite(m_strTag.c_str(), sizeof(char), iLength, pFile);

	// 파일 경로를 저장한다.
	size_t iSize = m_vecFileName.size();

	fwrite(&iSize, sizeof(size_t), 1, pFile);

	for (size_t i = 0; i < iSize; ++i)
	{
		iLength = lstrlen(m_vecFileName[i]);
		fwrite(&iLength, sizeof(int), 1, pFile);
		fwrite(m_vecFileName[i], sizeof(char), iLength, pFile);
	}

	// PathKey를 저장한다
	iLength = m_strPathName.length();
	fwrite(&iLength, sizeof(int), 1, pFile);
	fwrite(m_strPathName.c_str(), sizeof(char), iLength, pFile);

	fwrite(&m_iColorKey, sizeof(unsigned int), 1, pFile);
	fwrite(&m_bColorKey, sizeof(bool), 1, pFile);


	return true;
}
