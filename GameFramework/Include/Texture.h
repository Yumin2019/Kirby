#pragma once
#include "Ref.h"
class CTexture :
	public CRef
{


	friend class CObj;
	friend class CResourceManager;
	// �ܺ��� ������ ����, ResourceManager�� ���ؼ��� ������ �� �ֵ��� �Ѵ�.

private:
	CTexture();
	~CTexture();

private:
	vector<HDC>		m_vecDC; // �޸� DC ������.
	vector<HBITMAP> m_vecBmp; // ���� ����� ��Ʈ�� ���� �翬�� ������
	vector<HBITMAP>	m_vecPrevBmp; // ������ ����� ��Ʈ�� �翬�� ������
	vector<BITMAP>	m_vecBmpInfo; // ��Ʈ�� �������� ����ü �翬�� ������
	unsigned int		m_iColorKey; // �÷�Ű ����
	bool				m_bColorKey; // �÷�Ű�� ��뿩��

	vector<const TCHAR*>	m_vecFileName;
	string			m_strPathName;


public:
	HDC GetDC(int iFrame = 0) const
	{
		// �⺻������ ��Ʋ�󽺶�� �����ϰ�, ������ �Ǿ� �ִ�.
		return m_vecDC[iFrame];
	}



	HBITMAP GetBmp(int iFrame = 0) const
	{
		// ���� iFrame�� �ش��ϴ� ��Ʈ���ڵ��� ��ȯ
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
	// �ؽ�ó�� �о���� �Լ��̴�.
	//  strPathName�� ��� + pFileName�� ��θ� ���� ��θ� ������, Texture�� �о�´�.
	// ���⼭ strPathName�� �̿��ϱ� ���ؼ�, PathManager�� �̿��Ѵ�.

	bool LoadTexture(const string& strName,
		const vector<const TCHAR*>& vecFileName,
		const string& strPathName = TEXTURE_PATH);

	// vecFileName �� �־��ָ�, ���⿡ �ش��ϴ� �̹��� ������ ��� �о��ִ� �Լ��̴�.
	// Texture�� �̸��� ���Ƶ� �ߺ��ν��� ������ �ʴ´�.
	// ���� Tag�� �ߺ����� �ִ� ��. ���� �ش� �Լ��� ȣ���ϸ�, 
	// �ؽ�ó�� �������� DC�� ������ �ְ� �ȴ�.
	
	void Render(HDC hDC, int iDestX, int iDestY, int iSrcX, int iSrcY,
		int iSizeX, int iSizeY, int iFrame = 0);
	void Render(HDC hDC, const Position& tDestPos,
		const Position& tSrcPos, const Size& tSize, int iFrame = 0);

	// DC�� �������̹Ƿ� �⺻���� 0 �ε����� ����.

	// �÷�Ű ��������� ���ְ� ���� �ʴٴ� ����
	// �ؽ�ó�� �ܼ��� ��� ���� ���ְ� �ִٴ� ���̴�.
	// TransparentBlt�Լ��� ���ؼ� ColorKey�� ����ϴ� �Լ��� �߰��� �����̸�
	// ����� BitBlt�Լ��� ���� ��� ���縸�� �ϰ� �ִ�.

	bool Load(FILE* pFile);
	bool Save(FILE* pFile);
};

