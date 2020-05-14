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
		// DC�� ��� �����ŭ, �� DC���� ������ �����صξ���, ��Ʈ�� ������ �ٽ� �����ϰ�,
		// DC�� ������ �����ߴ� ��Ʈ�� ������ �����ִ� ó���� �Ѵ�.

		// ������ �����ص� ��Ʈ�� ������ �ٽ� �����Ѵ�.
		SelectObject(m_vecDC[i], m_vecPrevBmp[i]);

		// ��Ʈ�� ������ ������.
		DeleteObject(m_vecBmp[i]);
		
		// DC������ �����ش�.
		DeleteObject(m_vecDC[i]);

	}

}

bool CTexture::LoadTexture(const string & strName, 
	const TCHAR * pFileName, 
	const string & strPathName)
{
	SetTag(strName);
	// �±� ����.

	// Ǯ��θ� ������ش�.
	TCHAR pFullPath[MAX_PATH] = {};

	const TCHAR* pBasePath = GET_SINGLE(CPathManager)->FindPath(strPathName);

	// ������ ��θ� ���δ�.
	if (pBasePath)
		lstrcpy(pFullPath, pBasePath);

	// ������ �̸��� �ٿ��ش�.
	lstrcat(pFullPath, pFileName);

	m_vecFileName.push_back(pFullPath);

	m_strPathName = strPathName;

	// �ش� ��θ� ������ ��Ʈ�� ������ �о�´�.
	
	// ȭ�� DC�� ȣȯ�� �Ǵ� �޸� DC�� �����Ѵ�.
	HDC hDC = CreateCompatibleDC(WINDOWDC);

	// ��Ʈ�� ������ �о�´�.
	HBITMAP m_hBmp = (HBITMAP)LoadImage(WINDOWINSTANCE, pFullPath, IMAGE_BITMAP,
		0, 0, LR_LOADFROMFILE);

	// ���ο� ��Ʈ���� �����ϰ� ���� ��Ʈ���� �޾Ƶд�.
	HBITMAP m_hPrevBmp = (HBITMAP)SelectObject(hDC, m_hBmp);

	BITMAP tInfo;

	// ��Ʈ��Info�� �����Ѵ�.
	GetObject(m_hBmp, sizeof(BITMAP), &tInfo);

	// Info ������, ������ DC, ���� ��Ʈ�� ������, ������ ��Ʈ�� ������ ���� �Ŀ�
	// ���Ϳ� �־��ش�.

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

	// �̷��� vecFileName�� �־�� ��θ� �о�ͼ� �������� DC �� �������ִ� �۾��� �ϰ� �ִ�.
	// �� �Լ��� ������ �ִϸ��̼� ������ ���̴�.

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
		// �÷�Ű�� �����Ǿ� �ִٸ�,  TransparentBlt �Լ��� �̿��Ѵ�.

	// ù��° ���ڴ� ����� DC�� �Է��Ѵ�.
	// 2,3 ��° ���ڴ� ����� DC������ X, Y ��ǥ�̴�.
	// 4, 5��° ���ڴ� ����� ����� Size�� �ǹ��Ѵ�. 
	// 6��° ���ڴ� ����� ����� DC�� �־��ش�
	// 7, 8 ��° ���ڴ� ����� ��� �̹��������� ���� ��ǥ�̴�.

	// BitBlt�Լ��� ���ڰ� �̷��� ������ �Ǿ� ������, TransparentBlt�Լ��� ��쿡�� �� ���� ���ڸ� �ʿ�� �Ѵ�.

	/*
	BOOL TransparentBlt(
    HDC hdcDest,				// ��� DC
    int xoriginDest,			// ��� X��ǥ
    int yoriginDest,			// ��� Y��ǥ
    int wDest,					// ��� �ʺ�
    int hDest,					// ��� ����   hdcSrc�κ��� ���� �̹����� �� �ʺ�� ���̿� �°� �̹��� ũ�⸦ �����մϴ�.
    HDC hdcSrc,					// �׸� DC
    int xoriginSrc,				// �׸������� X
    int yoriginSrc,				// �׸������� Y 
    int wSrc,					// �׸� �ʺ� �� ũ�⸸ŭ ���� �̹������� �߶󳻾� wDest, hDest�� ũ�⿡ �°� �̹��� ũ�⸦ �����մϴ�.
    int hSrc,					// �׸� ���� 
    UINT crTransparent);		// ColorKey RGB��ũ�θ� �̿��Ѵ�.
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

	// src���� ũ�⸦ ������ִ� �κ��� �ִµ�, �� �κ��� �ǵ帮�� �ʴ´�.
	// �ȼ��� �ø��� ���� ������.

}


bool CTexture::Load(FILE * pFile)
{

	//// ���� �ؽ�ó�� �̸��� �о�´�.
	//int iLength = 0;
	//fread(&iLength, sizeof(int), 1, pFile);
	//fread(m_strTag.c_str(), sizeof(char), iLength, pFile);

	//// ���� ��θ� �����Ѵ�.
	//size_t iSize = m_vecFileName.size();

	//fread(&iSize, sizeof(size_t), 1, pFile);

	//for (size_t i = 0; i < iSize; ++i)
	//{
	//	iLength = lstrlen(m_vecFileName[i]);
	//	fread(&iLength, sizeof(int), 1, pFile);
	//	fread(m_vecFileName[i], sizeof(char), iLength, pFile);
	//}

	//// PathKey�� �����Ѵ�
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


	// ���� �ؽ�ó�� �̸��� �����Ѵ�.
	int iLength = m_strTag.length();
	fwrite(&iLength, sizeof(int), 1, pFile);
	fwrite(m_strTag.c_str(), sizeof(char), iLength, pFile);

	// ���� ��θ� �����Ѵ�.
	size_t iSize = m_vecFileName.size();

	fwrite(&iSize, sizeof(size_t), 1, pFile);

	for (size_t i = 0; i < iSize; ++i)
	{
		iLength = lstrlen(m_vecFileName[i]);
		fwrite(&iLength, sizeof(int), 1, pFile);
		fwrite(m_vecFileName[i], sizeof(char), iLength, pFile);
	}

	// PathKey�� �����Ѵ�
	iLength = m_strPathName.length();
	fwrite(&iLength, sizeof(int), 1, pFile);
	fwrite(m_strPathName.c_str(), sizeof(char), iLength, pFile);

	fwrite(&m_iColorKey, sizeof(unsigned int), 1, pFile);
	fwrite(&m_bColorKey, sizeof(bool), 1, pFile);


	return true;
}
