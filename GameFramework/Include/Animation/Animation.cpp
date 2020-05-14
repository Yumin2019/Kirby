#include "Animation.h"
#include "../Core/ResourceManager.h"
#include "../Texture.h"
#include "../Object/Obj.h"
#include "../Core/SceneManager.h"
#include <stack>


CAnimation::CAnimation() :
	m_pObj(nullptr),
	m_pClip(nullptr)
{
}

CAnimation::CAnimation(const CAnimation & ani)
{
	// �ִϸ��̼��� ����.
	// ������Ÿ���� ��ü���� �����ص� ��ü�� �ִϸ��̼��� �����Ѵ�.

	m_pObj = nullptr;

	m_strDefaultClip = ani.m_strDefaultClip;

	m_mapClip.clear();

	unordered_map<string, PAnimationClip>::const_iterator iter;
	unordered_map<string, PAnimationClip>::const_iterator iterEnd = ani.m_mapClip.end();

	for (iter = ani.m_mapClip.begin(); iter != iterEnd; ++iter)
	{
		PAnimationClip pClip = new AnimationClip;

		*pClip = *iter->second;
		
		// ���͸� �����ϸ� ��� �Ǵ°� ?

		// ������ �����Ϳ� ������ �������°� �ƴϸ�, �ܼ��� �����͸� ���簡 �Ǵ°� ?

		pClip->pTexture->AddRef();

		m_mapClip.insert(make_pair(iter->first, pClip));
		// ���� �������ְ�, �ؽ�ó�� ���ؼ��� ����Ƚ���� ������Ų��.
	}

	// �ִϸ��̼ǿ� �ִ� ��� Ŭ���� �߰����ְ�, �⺻Ŭ���� �������ش�.

	m_pClip = FindClip(ani.m_pClip->strName);
	// �����Ϸ��� ��ü�� �⺻������ ��� �ִ� Ŭ������ ����.
}


CAnimation::~CAnimation()
{
	// map���� ������ �ִ� clip�������� Texture�� �ҷ����� �������� ���۷��� ī��Ʈ�� ���� �Ǿ���.
	// ���� �ش� �ִϸ��̼��� ���ִ� �������� ���۷���  ī���͸� �ٽ� ���ҽ�Ų��.

	unordered_map<string, PAnimationClip>::iterator iter;
	unordered_map<string, PAnimationClip>::iterator iterEnd = m_mapClip.end();

	for (iter = m_mapClip.begin(); iter != iterEnd; ++iter)
	{
		SAFE_RELEASE(iter->second->pTexture);
		SAFE_DELETE(iter->second);
	}
	
	m_mapClip.clear();
}


// ������ �ߴµ�, �����غ���, �̷� �Լ��� ���¸� ���� �� �ֵ��� ������ �Ǿ� ���� �ʴ�....
// �������� Ư��Ű�� ������, �ش� Ű�� �ν��ؼ� �� �Լ��� ȣ���� �� �ֵ��� �Ǿ� ���� �ʴ�.
// �̷��� �Ϸ��� �Լ��� ���¸� ���߾�� �ϴµ�, �Ѱ谡 ��Ȯ�ϴ�.
// ������ �Է� ������, ȣ���Ϸ��� �ϴ� �Լ��� ���°� �и��ϰ� �������ְ�, ���� �̰��� ���ؼ� 
// �츮�� ���ϴ� �Լ��� ȣ���� �� ������ �ǹ��Ѵ�.
// ���ʿ� Ű �Է� �ý��� ��ü�� �Լ��� �����ϱ� ����� ���� ��Ȯ�ϰ� �巯����.
// �׷��� ����, �������� ���ø��� �̿��� �� �ٸ� �Է� �ý����� ���� ���⼭ �ش� Ű�� �������� �Ǵ��ϴ� �ý����� ������ ���̴�.
// OK ?
// �׷��� �̷��� �̿��� Ű �Է� �ý����� �ܼ��� Debug�� ó�������� �� ���ΰ� ? 
// �ƴϸ� �̰��� Ȱ�뵵�� �� �̿��� �� �ֵ��� �� 2�� InputSystem���μ� ����� ���ΰ� ?
// ���ڰ� ���� �� ����. ¥�� ������ ����� ���´�.
// �׸��� ����ϱ� ������ ���� ������ �� ���� �κ�.
// �׷���, �׳� CInputŬ������ �����ϴ� ���� � ?

string CAnimation::GetCurrentAnimation() const
{
	return m_pClip->strName;
}

void CAnimation::ClipPosUpdate(bool bHorizon, float fScale)
{
	if (!m_pClip)
		return;

	// ���� Ŭ���� �����ġ�� ����ϴ� ���� �ִϸ��̼��� �����ϴ� �������� �ϴ� ��� ũ�� ����.
	// ����� ��� �̷�������� Ȯ���غ���.
	
	// begin�� end�� ��ġ�� ��� �����Ѵ�.
	// begin�� �ٲٴ� ������ �������� ��¿� ������ �ִ� ���̱� �����̴�.
	// begin + m_tSize �� ���ؼ� �ش� �κ��� ������ְ� �ִ�.
	// ���� begin���� ����/���ҿ� ����, �����ġ�� �����ų �� �ֵ�.
	// �׷��ٰ� end�� ��ġ�� �������� ������, ���߿� ChangeClip�� �ϴ� �������� 
	// ��»���� ���ϰ� �Ǵ� ������ �Ͼ ���̴�.(����) �츮�� �ܼ��� �����ġ�� �������� ���ε� ����� �ٲ�� ������ ����(����)����
	// ����, ��°��� ����� ���� end��� �ϴ��� �ݵ�� ������Ʈ ����� �Ѵ�. (���߿� ��� ����� ��� ��Ȳ�� ���ؼ�)

	vector<ClipCoord>& vecCoord = m_pClip->vecCoord;

	for (size_t i = 0; i < vecCoord.size(); ++i)
	{
		if (bHorizon)
		{
			vecCoord[i].tEnd.x += fScale;
			vecCoord[i].tStart.x += fScale;
		}
		else
		{
			vecCoord[i].tEnd.y += fScale;
			vecCoord[i].tStart.y += fScale;
		}
	}
	
	// ��� ��ġ�� �ٲپ���, �� ����� begin�� ���ϸ鼭 ������Ʈ���� �ٷ� ������ �ȴ�.
	// ������� �״�� �̿��ϴϱ�.
	
}

void CAnimation::ClipSizeUpdate(bool bHorizon, float fScale)
{
	if (!m_pClip)
		return;

	// ����� �����ϴ� �Լ��̴�.
	// ���� Ŭ���� end��ǥ�� �����Ű��,
	// ������Ʈ�� size�� ��ȭ�� �ش�.


	vector<ClipCoord>& vecCoord = m_pClip->vecCoord;

	// �ִϸ��̼��� srcPos�� ���´�.
	Position tSrcPos = vecCoord[0].tStart;

	// ���� �������� Size�� ���´�.
	Position tFrameSize = vecCoord[0].tEnd - tSrcPos;

	// fScale���� ��ø�Ͽ� �����̵� ������ ����� ���Ѵ�.
	if (bHorizon)
		tFrameSize.x += fScale;
	else
		tFrameSize.y += fScale;
	
	Position tDestPos = tSrcPos + tFrameSize;

	for (size_t i = 0; i < vecCoord.size(); ++i)
	{

		vecCoord[i].tStart = tSrcPos;
		vecCoord[i].tEnd = tDestPos;


		if (bHorizon)
		{
			tSrcPos.x += tFrameSize.x;
			tDestPos.x += tFrameSize.x;
		}
		else
		{
			tSrcPos.y += tFrameSize.y;
			tDestPos.y += tFrameSize.y;
		}
	}

	// Size�� �����Ѵ�.
	m_pObj->SetSize(tFrameSize);
	

	/*
		if (bHorizon)
		{
			vecCoord[i].tEnd.x += fScale;
		}
		else
		{
			vecCoord[i].tEnd.y += fScale;
		}*/

		// ������. End�� ��ġ�� �ٲ۴�.
		// Size�� �ٲ۴�. 
		// �̰��� �� �����Ӵ� ����ϴ� ����� �ٲ���ٴ� ���� �ǹ��Ѵ�.
		// �ٵ�, �ܼ��� end���� �ٲ��, ó�� ������ �þ ũ��� ����� ���Ŀ�
		// ���� �������� ����ϴ� ��Ȳ������ ������ begin�� ��ġ�� �״�ζ� ������ ���� �������� end ��ǥ�� ���� �������� begin�� ������
		// ħ���ϴ� ��Ȳ�� �߻��Ѵ�.
		// ��, �����Ӵ� ũ��� �����ߴٰ� �� �� ������, �������� ��� ������ ��ġ�� ��Ȳ�� ����� �ȴ�.
		// �������� ��ġ�� �ʵ��� �ٽ� ������ ¥���� �Ѵ�.

}

void CAnimation::ChangeClip(float fScale)
{
	if (!m_pClip)
		return;

	// ���� ������ �ִ� Ŭ���� ��ġ�� �˾ƿ´�.

	// �ݺ��ڸ� �� ����ϴ� ��쿡�� end�� �����ϰ� �� �� �ִ�.

	// map�� Ư¡�� �̿��ؼ� ó���Ѵ�.

	unordered_map<string, PAnimationClip>::iterator iter = m_mapClip.find(m_pClip->strName);

	if (fScale)
	{
		++iter;

		if (iter == m_mapClip.end())
		{
			// ������ ���ҿ��� ������ ���Ѽ� end�� �����״ٸ�, begin�� �ش�.
			iter = m_mapClip.begin();
		}

		// end�� ����Ű�� ���� ��쿡�� ���д�.
	}
	else
	{
		if (iter == m_mapClip.begin()) // ����Ű�� �ִ� ��ġ�� ó���� ���ҿ� �ش��Ѵٸ�, 
		{
			// ó�� ������ �������� ���������Ҹ� �ش�.

			iter = (--m_mapClip.end());
		}
		else
			--iter; // ó�����Ҹ� ����Ű�� ���� ���� ��� �׳� ����.
	}

	// �̷��� iter�� �ٲ���ϴ� Ŭ���� ����ְ� �ȴ�.

	// Ŭ���� �ٲ��ش�.
	
	ChangeClip(iter->second->strName);
}

void CAnimation::AddFrameTime(float fTime)
{
	if (!m_pClip)
		return;


	m_pClip->fFrameTime += fTime;
	m_pClip->fPlayTime = m_pClip->vecCoord.size() * m_pClip->fFrameTime;

	if (m_pClip->fFrameTime <= 0.f)
	{
		m_pClip->fFrameTime = 0.01f;
		m_pClip->fPlayTime = m_pClip->vecCoord.size() * m_pClip->fFrameTime;
	}
}

bool CAnimation::SaveClip(const char * pFileName, bool bOption, ANIMATION_OPTION eOption)
{
	// ���� Ŭ���� ������ �����Ѵ�.
	FILE* pFile = nullptr;

	fopen_s(&pFile, pFileName, "wb");

	if (!pFile)
		return false;


	char strName[50] = {};
	strcpy_s(strName, sizeof(char) * 50, m_pClip->strName.c_str());

	// �̸��� ũ�⸦ �����Ѵ�.
	int iLength = strlen(strName);
	
	fwrite(&iLength, sizeof(int), 1, pFile);

	// �̸��� ���̸�ŭ �����Ѵ�.
	fwrite(strName, sizeof(char), iLength, pFile);

	// �ɼ��� �����Ѵ�.
	if (bOption) // �ɼ��� ����ϴ� ��� �ش� �ɼǴ��.
		fwrite(&eOption, sizeof(ANIMATION_OPTION), 1, pFile);
	else // ������� �ʴ� ����� �׳� ���� �ɼ��� �״�� ����.
		fwrite(&m_pClip->ePrevOption, sizeof(ANIMATION_OPTION), 1, pFile);

	// ColorKey ���θ�  �����Ѵ�.
	fwrite(&m_pClip->bColorKey, sizeof(bool), 1, pFile);

	// ColorKey�� �ִٸ� �÷�Ű�� �����Ѵ�.
	if (m_pClip->bColorKey)
	{
		fwrite(&m_pClip->iColorKey, sizeof(unsigned int), 1, pFile);
	}

	// PlayTime�� �����Ѵ�.
	fwrite(&m_pClip->fPlayTime, sizeof(float), 1, pFile);

	// FrameTime�� �����Ѵ�.
	fwrite(&m_pClip->fFrameTime, sizeof(float), 1, pFile);

	// OptionPlayTime�� �����Ѵ�.
	fwrite(&m_pClip->fOptionPlayTime, sizeof(float), 1, pFile);

	// iOnceLoopFrame�� �����Ѵ�.
	fwrite(&m_pClip->iOnceLoopFrame, sizeof(int), 1, pFile);

	// tOffset�� �����Ѵ�.
	fwrite(&m_pClip->tOffset, sizeof(Position), 1, pFile);

	// Coord ������ ����� �����Ѵ�.
	vector<ClipCoord>& vecCoord = m_pClip->vecCoord;

	iLength = vecCoord.size();
	fwrite(&iLength, sizeof(int), 1, pFile);

	// ����� ���� ClipCoord������ �����Ѵ�.
	for (int i = 0; i < iLength; ++i)
	{
		fwrite(&vecCoord[i], sizeof(ClipCoord), 1, pFile);
	}

	fclose(pFile);

	return true;
}

bool CAnimation::LoadClip(const char * pFileName)
{

	// ���� Ŭ���� ������ �����Ѵ�.
	FILE* pFile = nullptr;

	fopen_s(&pFile, pFileName, "rb");

	if (!pFile)
		return false;

	AnimationClip tInfo = {};

	char strName[50] = {};

	// �̸��� ũ�⸦ �о�´�..
	int iLength = 0;

	fread(&iLength, sizeof(int), 1, pFile);

	// �̸��� ���̸�ŭ �о�´�.
	fread(strName, sizeof(char), iLength, pFile);

	// �о�� ���̸� string�� �����Ѵ�.
	tInfo.strName = strName;

	// �ɼ��� �о�´�.
	fread(&tInfo.eOption, sizeof(ANIMATION_OPTION), 1, pFile);

	// ColorKey ���θ�  �о�´�.
	fread(&tInfo.bColorKey, sizeof(bool), 1, pFile);

	// ColorKey�� �ִٸ� �÷�Ű�� �����Ѵ�.
	if (tInfo.bColorKey)
	{
		fread(&tInfo.iColorKey, sizeof(unsigned int), 1, pFile);
	}

	// PlayTime�� �����Ѵ�.
	fread(&tInfo.fPlayTime, sizeof(float), 1, pFile);

	// FrameTime�� �����Ѵ�.
	fread(&tInfo.fFrameTime, sizeof(float), 1, pFile);

	// OptionPlayTime�� �����Ѵ�.
	fread(&tInfo.fOptionPlayTime, sizeof(float), 1, pFile);


	// iOnceLoopFrame�� �о�´�.
	fread(&tInfo.iOnceLoopFrame, sizeof(int), 1, pFile);

	//// Offset ������ �о�´�.
	fread(&tInfo.tOffset, sizeof(Position), 1, pFile);


	// Coord ������ ����� �о�´�.

	iLength = 0;
	fread(&iLength, sizeof(int), 1, pFile);



	tInfo.vecCoord.clear();

	// ����� ���� ClipCoord������ �����Ѵ�.
	for (int i = 0; i < iLength; ++i)
	{
		ClipCoord tCoord = {};

		fread(&tCoord, sizeof(ClipCoord), 1, pFile);

		// �о�� ������ vector�� ����.
		tInfo.vecCoord.push_back(tCoord);
	}

	fclose(pFile);

	// �߰��� ������ map�� �������ش�.
	// �ش� Ŭ���� ����, coord ����, �ؽ�ó �׿�...
	// ������ �ؽ�ó�� �̹� �Ǿ� �ְ�, �츮�� coord������ �׿��� ������ ó���ϸ� �ȴ�. (�����ϸ� �ȴ�)

	PAnimationClip pClip = FindClip(tInfo.strName); 

	// ������ Ŭ���� ���ٸ�, false;
	if (!pClip)
		return false;

	// ������ Ŭ���� �ִٸ� ������ �����Ѵ�. ���⼭ �ؽ�ó�� ���� ������ �����Ѵ�.
	pClip->eOption = tInfo.eOption;
	pClip->ePrevOption = tInfo.eOption;
	pClip->eType = AT_ATLAS;
	pClip->bColorKey = tInfo.bColorKey;
	if (tInfo.bColorKey)
	{
		pClip->iColorKey = tInfo.iColorKey;
		
		// �ҷ����� �������� �ؽ��Ͱ� �����Ѵ�.
		pClip->pTexture->SetColorKey(tInfo.iColorKey);
	}
	else
		pClip->iColorKey = 0;
		
	pClip->iFrame = 0;
	pClip->fTime = 0.f;
	pClip->fPlayTime = tInfo.fPlayTime;
	pClip->fFrameTime = tInfo.fFrameTime;
	pClip->fOptionTime = 0.f;
	pClip->fOptionPlayTime = tInfo.fOptionPlayTime;
	pClip->iOnceLoopFrame = tInfo.iOnceLoopFrame;
	pClip->tOffset = tInfo.tOffset;
	// vector
	pClip->vecCoord = tInfo.vecCoord;

	return true;
}

bool CAnimation::SaveClipOption(const char * pFileName)
{
	// �ڽ��� �̸����� ������ �ϴ� ������ ����. (������Animation.op)
	FILE* pFile = nullptr;

	fopen_s(&pFile, pFileName, "wb");

	if (!pFile)
		return false;

	// Ŭ���� ������ �����Ѵ�.
	int iCount = m_mapClip.size();

	fwrite(&iCount, sizeof(int), 1, pFile);

	unordered_map<string, PAnimationClip>::iterator iter;
	unordered_map<string, PAnimationClip>::iterator iterEnd = m_mapClip.end();

	for (iter = m_mapClip.begin(); iter != iterEnd; ++iter)
	{
		PAnimationClip pCilp = iter->second;

		char strName[50] = {};
		strcpy_s(strName, sizeof(char) * 50, pCilp->strName.c_str());

		// �̸��� ũ�⸦ �����Ѵ�.
		int iLength = strlen(strName);

		fwrite(&iLength, sizeof(int), 1, pFile);

		// �̸��� ���̸�ŭ �����Ѵ�.
		fwrite(strName, sizeof(char), iLength, pFile);

		// �ִϸ��̼� �ɼ��� ����.
		fwrite(&pCilp->ePrevOption, sizeof(ANIMATION_OPTION), 1, pFile);

	}

	fclose(pFile);

	return true;
}

bool CAnimation::LoadClipOption(const char * pFileName)
{
	FILE* pFile = nullptr;

	fopen_s(&pFile, pFileName, "rb");

	if (!pFile)
		return false;

	// ������ �д´�.
	int iCount = 0;
	fread(&iCount, sizeof(int), 1, pFile);

	// ���� ��ŭ ������ �д´�.
	for (int i = 0; i < iCount; ++i)
	{
		char strName[50] = {};

		// �̸��� ũ�⸦ �д´�.
		int iLength = 0;

		fread(&iLength, sizeof(int), 1, pFile);

		// �̸��� ���̸�ŭ �д´�..
		fread(strName, sizeof(char), iLength, pFile);

		// �ɼ������� �д´�.
		ANIMATION_OPTION eOption = AO_LOOP;

		fread(&eOption, sizeof(ANIMATION_OPTION), 1, pFile);


		// �̸��� ������ Ŭ���� ã�ƿ´�.
		PAnimationClip pClip = FindClip(strName);

		
		if (pClip)
		{
			pClip->eOption = eOption;
			pClip->ePrevOption = eOption;
		}
	}


	fclose(pFile);

	return true;
}




void CAnimation::CurrentClipClear()
{
	m_pClip->iFrame = 0;
	m_pClip->fTime = 0.f;
	m_pClip->fOptionTime = 0.f;
}

bool CAnimation::AddClip(const string & strName, ANIMATION_OPTION eOption,
	float fPlayTime, float fOptionPlayTime)
{
	PAnimationClip pClip = FindClip(strName);

	if (pClip)
		return false;
	// �̹� strName�� �̸����� ���� Clip�� �����ϴ� ��쿡�� �߰����� �ʴ´�.

	pClip = new AnimationClip;

	pClip->strName = strName;
	pClip->eOption = eOption;
	pClip->fPlayTime = fPlayTime;
	pClip->fOptionTime = fOptionPlayTime;

	m_mapClip.insert(make_pair(strName, pClip));

	if (!m_pClip)
	{
		m_pClip = pClip;
		m_strDefaultClip = strName;
		// ���࿡ ó���� �߰��� �� Ŭ���̶��, �⺻���� �����Ѵ�.
	}
	
	return true;
}

bool CAnimation::SetClipTexture(const string & strClipName, 
	const string & strTexName, const TCHAR * pFileName,
	const string & strPathName)
{
	// strClipName�� �ش��ϴ� Ŭ���� ã�ƿͼ� �� Ŭ���� �ؽ�ó�� �����ϴ� ����̴�.
	// ���⼭ �ؽ�ó�� Load���ְ�, ������ �Ͼ��.
	// ��Ȯ�ϰԴ� ResourceManager���� �ѹ��� ������ �Ͼ��,
	// �� ��°�� AnimationClip���� ������ �����鼭 ������ �Ͼ��.
	

	PAnimationClip pClip = FindClip(strClipName);

	if (!pClip)
		return false;

	// �̹��� �� ���� �ε��ϴ� �Լ��� ����Ѵٴ� ���� ��Ʋ�� �̹����� ����ϴ� ������ �� �� �ִ�.
	pClip->eType = AT_ATLAS;

	// ���ҽ� �Ŵ����� �ش� �ؽ�ó�� �ε��ϵ��� �Ѵ�.
	GET_SINGLE(CResourceManager)->LoadTexture(strTexName, pFileName, strPathName);

	// �̷��� �Ͽ� TexName�� Ű������ �Ͽ� ResourceManager�� ����ִ� map�� �ش� ������ �߰��ϰ�,
	// ���⼭ ���� �ѹ�
	
	// �̷��� �߰��� Texture�� ã�Ƽ� pClip���� �ش�.

	pClip->pTexture = GET_SINGLE(CResourceManager)->FindTexture(strTexName);
	// ���� 

	return true;
}

bool CAnimation::SetClipTexture(const string & strClipName,
	const string & strTexName, const vector<const TCHAR*>& vecFileName,
	const string & strPathName)
{
	PAnimationClip pClip = FindClip(strClipName);

	if (!pClip)
		return false;

	pClip->eType = AT_FRAME;

	GET_SINGLE(CResourceManager)->LoadTexture(strTexName, vecFileName, strPathName);

	pClip->pTexture = GET_SINGLE(CResourceManager)->FindTexture(strTexName);

	return true;
}


void CAnimation::SetTextureColorKey(const string & strName, unsigned char r, unsigned char g, unsigned char b)
{
	// strName�� �̸��� ���� Ŭ���� ã�ƿͼ� �ش� Ŭ���� �ؽ�ó�� �÷�Ű�� �ٲ��ش�.
	PAnimationClip pClip = FindClip(strName);

	if (!pClip)
		return;

	pClip->pTexture->SetColorKey(r, g, b);
	// Ŭ���� �÷�Ű�� �����Ѵ�.
	pClip->iColorKey = RGB(r, g, b);
	pClip->bColorKey = true;
}

void CAnimation::SetTextureColorKeyAll(unsigned char r, unsigned char g, unsigned char b)
{
	unordered_map<string, PAnimationClip>::iterator iter;
	unordered_map<string, PAnimationClip>::iterator iterEnd = m_mapClip.end();

	for (iter = m_mapClip.begin(); iter != iterEnd; ++iter)
	{
		iter->second->pTexture->SetColorKey(r, g, b);
		iter->second->iColorKey = RGB(r, g, b);
		iter->second->bColorKey = true;
	}
}



bool CAnimation::AddClipCoord(const string & strName,
	const vector<ClipCoord>& vecCoord)
{
	// Ŭ���� Coord�� �����ϴ� �Լ��̴�. 
	// �������� ��ǥ�� �����ϴ� �Լ���, ��Ʋ�� �����̴�.

	PAnimationClip pClip = FindClip(strName);

	if (!pClip)
		return false;

	pClip->vecCoord = vecCoord;

	pClip->fFrameTime = pClip->fPlayTime / vecCoord.size();
	// �� �������� �ð���, ��ü �ð� / ���� 

	return true;
}

bool CAnimation::AddClipCoord(const string & strName,
	const ClipCoord & tCoord, int iCount)
{
	// �ϳ��� ��ǥ�� ������ �߰��ϴ� �Լ��̴�.
	// ������ �Լ��̴�.

	PAnimationClip pClip = FindClip(strName);

	if (!pClip)
		return false;

	pClip->vecCoord.clear();

	for (int i = 0; i < iCount; ++i)
	{
		pClip->vecCoord.push_back(tCoord);
	}

	pClip->fFrameTime = pClip->fPlayTime / iCount;

	return true;
}

Size CAnimation::GetBmpSize() const
{
	BITMAP tInfo = m_pClip->pTexture->GetBitmapInfo();
	return Size(tInfo.bmWidth, tInfo.bmHeight);
}

bool CAnimation::AddClipCoord(const string & strName, const Position & tSrcStartPos,
	int iFrameSizeX, int iFrameSizeY, int iCount, bool bRight, bool bHorizon)
{
	// bHorizon��, ��Ʋ�� �̹����� ���η� �ִ� ��Ȳ�� �����ϴ� ���̴�.
	// ���� �� ���� false���, �ε��� * iFrameY�� ���ؼ� ���η� ��ǥ�� �߰��� ���̴�.
	// bRight�� �¿� �ִϸ��̼��� �Ǵ��ϴ� �������� ����ϴ� �����̴�.
	// �⺻������ ���ҽ��� Right�� �������� ������ �Ǿ� �־
	// Right�̹����� �ִ� ��쿡�� ���� ��ǥ�� ������ X, Y�� ũ�⸦ �־��ָ�, �ڵ����� �ִϸ��̼��� �߰��Ѵ�.
	// ������, Left�� �̹����� �ִ� ��쿡�� ���� ��ǥ�� ������, �̹����� ������ �Ųٷ� ���� �ȴ�.
	// Right : A B C Left(Right�� ����) : C B A 
	// ���� ��ġ�� �ִ´ٰ� �Ҷ�, Left�� Right�� ���� �̹����� �޸����� �ȴ�.
	
	// ����, Left�� �̹����� �ִ� ��쿡�� ������ �̹������� ������� �ִ� �ý����� �����ϵ��� �Ѵ�.
	// �ϴ�, �־��ִ� ���� ��ǥ�� �״�� ����� �� �ֵ��� �Ѵ�. (�ִϸ��̼��� �߰� ���̼��� ���ؼ�)
	// ���� ��ǥ�� �����ʰ� �Ȱ��� �־��ָ�, ������ �������� �߰��ϴ� ���������� �ڵ����� ������ �����Ӻ��� �־��ִ� �ý���.
	// �츮�� �̰��� �������� �����Ѵ�.
	

	// �������� �̹��� ���� ��ŭ for���� ������ ��ǥ�� �־��ִ� ó���� �Ѵ�.

	vector<ClipCoord> vecCoord;
	// ��ǥ�� �־ ó������ vector


		if (bHorizon)
		{
			// �������� �߰��ϴ� ���.

				// ���η� ��ǥ�� �߰��Ѵ�.
				for (int i = 0; i < iCount; ++i)
				{
					ClipCoord tCoord;
					// ������ǥ���� ù �������� ��ǥ�� �־��ش�.

					tCoord.tStart = tSrcStartPos;

					tCoord.tStart.x += iFrameSizeX * i;
					// begin�� y��ǥ�� �ٲ��� �ʴ´�. startPos�� �����ϴ�. horizon�̴ϱ�.

					tCoord.tEnd = tCoord.tStart + Position(iFrameSizeX, iFrameSizeY);

					vecCoord.push_back(tCoord);
						
				}

		}
		else
		{
			// ���������� �߰��ϴ� ���

				for (int i = 0; i < iCount; ++i)
				{
					ClipCoord tCoord;
					// ������ǥ���� ù �������� ��ǥ�� �־��ش�.

					tCoord.tStart = tSrcStartPos;

					tCoord.tStart.y += iFrameSizeY * i;
					// begin�� y��ǥ�� �ٲ��� �ʴ´�. startPos�� �����ϴ�. horizon�̴ϱ�.

					tCoord.tEnd = tCoord.tStart + Position(iFrameSizeX, iFrameSizeY);

					vecCoord.push_back(tCoord);
				}
		}

		// �̷��� �߰��� ��ǥ�� �����ߴٸ�, vector�� �޴� �Լ��� ȣ���Ͽ� �ѹ��� ó���� �� �ֵ��� �Ѵ�.

		if(!bRight)
		{
			stack<ClipCoord> stackCoord;

			// ������ �������� �ϴ� �����, �߰��� ���� �ݴ�� �־���� �Ѵ�.
			// ����, vector�� �߰��� ���� ������� stack�� �ְ�,  vector�� �ٽ� �̰��� �־ ������.

			for (int i = 0; i < iCount; ++i)
			{
				stackCoord.push(vecCoord[i]);
			}

			for (int i = 0; i < iCount; ++i)
			{
				vecCoord[i] = stackCoord.top();
				stackCoord.pop();
			}

			// �̷��� �Ͽ� �츮�� ���ϴ� ������� ����.

		}

		return AddClipCoord(strName, vecCoord);

		// �������� �������� �ϸ� �״�� vector�� �־��ְ�
		// ������ �������� �Ѵٸ�, ������ �̿��ؼ� �ݴ�� vector�� �־��ְ� (����� ������ ���缭) �̰��� vector�� �־��ش�.

}

void CAnimation::AddRefAllClipTexture()
{
	unordered_map<string, PAnimationClip>::iterator iter;
	unordered_map<string, PAnimationClip>::iterator iterEnd = m_mapClip.end();

	for (iter = m_mapClip.begin(); iter != iterEnd; ++iter)
	{
		iter->second->pTexture->AddRef();
	}
}

void CAnimation::SetDefaultClip(const string & strName)
{
	m_strDefaultClip = strName;
}

void CAnimation::SetCurrentClip(const string & strName)
{
	m_pClip = FindClip(strName);
}

void CAnimation::ChangeClip(const string & strName)
{
	if (m_pClip->strName == strName)
		return;

	// ������ ����ϴ� Ŭ���� ������ �ʱ���·�.

	m_pClip->iFrame = 0;
	m_pClip->fTime = 0.f;
	m_pClip->fOptionTime = 0.f;
	m_pClip = FindClip(strName);

	m_pObj->SetTexture(m_pClip->pTexture);  // ���� �ؽ�ó ���� Ǯ��, ���� �ؽ�ó ���� ����.

	if(!m_pClip->vecCoord.empty())
		m_pObj->SetSize(m_pClip->vecCoord[0].tEnd - m_pClip->vecCoord[0].tStart); 
	
	// ��� ����� �°� ����.
	// ������Ʈ�� ������� �̹����� ����ϴµ� ����ϴ� ũ���̴�.
	// �ؽ�ó��, ũ�⸦ ������ ���� �ʴ´�.
	// ���� ������Ʈ�� ��»���� �Ѱ��༭ ��� �ؽ�ó���� ����ϵ��� ������� ������.
	// ����, ����ϴ� �̹����� �޶����ٴ� �ǹ̴� ����ϴ� ũ�⸦ ������� ������� �ǹ��ϰ�,
	// �̰��� ������Ʈ�� ����� �����ؾ� �Ѵٴ� ���̴�.
}

PAnimationClip CAnimation::FindClip(const string & strName)
{
	unordered_map<string, PAnimationClip>::iterator iter = m_mapClip.find(strName);

	if (iter == m_mapClip.end())
		return nullptr;

	return iter->second;
}

void CAnimation::Start()
{
}

bool CAnimation::Init()
{
	return true;
}

void CAnimation::Update(float fTime)
{

	// �ð��� ������� �Ͽ� ���� �����ӿ� �ش��ϴ� ����� �����ؾ� �ϴ� ��쿡
	// �������� ������Ű��, �ؽ�ó������ �� ������ ������ �̿��Ͽ�, �ڽ��� ����ؾ��ϴ� DC�� �˾Ƴ���.

	m_pClip->fTime += fTime;

	if (m_pClip->eOption == AO_TIME_DESTROY)
	{
		m_pClip->fOptionTime += fTime;

		if (m_pClip->fOptionTime >= m_pClip->fOptionPlayTime)
		{
			m_pObj->SetActive(false);


			
			// ������Ʈ�� ������, �ڽ��� �ִϸ��̼� ������ ��� ���󰡹Ƿ� �Ű���� �ʿ�� ����.
		}
	}

	// �������� ������� �ð��� ���� �������� �������� �Ѿ �� �ֵ��� �Ѵ�.

	while (m_pClip->fTime >= m_pClip->fFrameTime)
	{
		m_pClip->fTime -= m_pClip->fFrameTime;

		++m_pClip->iFrame;
		
		if (m_pClip->iFrame == m_pClip->vecCoord.size())
		{
			// �ѹ��� ��� ���� ��쿡..

			m_pClip->iFrame = 0;

			switch (m_pClip->eOption)
			{
			case AO_ONCE_DESTROY:


				if (!m_pObj->GetDieEnable())
					m_pObj->SetActive(false);

				m_pClip->iFrame = m_pClip->vecCoord.size() - 1;

				// �̰����� ���̴� ó���� ���� �ʰ� once_loopó�� ó���Ѵ�.
				break;

			case AO_ONCE_RETURN:
				ChangeClip(m_strDefaultClip);
				break;

			case AO_ONCE_LOOP:
				m_pClip->iFrame = m_pClip->iOnceLoopFrame;
				break;
			}
		}
	}
	
	// ao_loop�� ��쿡�� ��� ���ư��� ���̴�. ������ 0���� �ٽ�.
}

CAnimation * CAnimation::Clone() const
{
	return new CAnimation(*this);
}

void CAnimation::SetObj(CObj * pObj)
{
	m_pObj = pObj;
}
