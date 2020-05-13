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
	// 애니메이션의 복사.
	// 프로토타입의 객체에서 생성해둔 객체의 애니메이션을 복사한다.

	m_pObj = nullptr;

	m_strDefaultClip = ani.m_strDefaultClip;

	m_mapClip.clear();

	unordered_map<string, PAnimationClip>::const_iterator iter;
	unordered_map<string, PAnimationClip>::const_iterator iterEnd = ani.m_mapClip.end();

	for (iter = ani.m_mapClip.begin(); iter != iterEnd; ++iter)
	{
		PAnimationClip pClip = new AnimationClip;

		*pClip = *iter->second;
		
		// 벡터를 데입하면 어떻게 되는가 ?

		// 기존의 데이터와 완전히 같아지는가 아니면, 단순히 데이터만 복사가 되는가 ?

		pClip->pTexture->AddRef();

		m_mapClip.insert(make_pair(iter->first, pClip));
		// 값을 복사해주고, 텍스처에 대해서는 참조횟수를 증가시킨다.
	}

	// 애니메이션에 있는 모든 클립을 추가해주고, 기본클립을 설정해준다.

	m_pClip = FindClip(ani.m_pClip->strName);
	// 복사하려는 객체가 기본적으록 잡고 있던 클립으로 설정.
}


CAnimation::~CAnimation()
{
	// map에서 가지고 있는 clip정보마다 Texture를 불러오는 과정에서 레퍼런스 카운트가 증가 되었다.
	// 따라서 해당 애니메이션을 없애는 과정에서 레퍼런스  카운터를 다시 감소시킨다.

	unordered_map<string, PAnimationClip>::iterator iter;
	unordered_map<string, PAnimationClip>::iterator iterEnd = m_mapClip.end();

	for (iter = m_mapClip.begin(); iter != iterEnd; ++iter)
	{
		SAFE_RELEASE(iter->second->pTexture);
		SAFE_DELETE(iter->second);
	}
	
	m_mapClip.clear();
}


// 만들기는 했는데, 생각해보니, 이런 함수의 형태를 받을 수 있도록 구현이 되어 있지 않다....
// ㅎㄷㄷㄷ 특정키를 누르면, 해당 키를 인식해서 이 함수를 호출할 수 있도록 되어 있지 않다.
// 이렇게 하려면 함수의 형태를 맞추어야 하는데, 한계가 명확하다.
// 현재의 입력 구조상, 호출하려고 하는 함수의 형태가 분명하게 정해져있고, 따라서 이것을 통해서 
// 우리가 원하는 함수롤 호출할 수 없음을 의미한다.
// 애초에 키 입력 시스템 자체가 함수와 연결하기 어려운 점이 명확하게 드러난다.
// 그래서 나는, 가변인자 템플릿을 이용한 또 다른 입력 시스템을 만들어서 여기서 해당 키를 눌렀는지 판단하는 시스템을 구현할 것이다.
// OK ?
// 그러면 이렇게 이용한 키 입력 시스템을 단순히 Debug용 처리에서만 할 것인가 ? 
// 아니면 이것의 활용도를 더 이용할 수 있도록 제 2의 InputSystem으로서 사용할 것인가 ?
// 후자가 좋을 것 같다. 짜피 프레임 충분히 나온다.
// 그리고 사용하기 용이한 점도 무시할 수 없는 부분.
// 그러면, 그냥 CInput클래스에 구현하는 것은 어때 ?

string CAnimation::GetCurrentAnimation() const
{
	return m_pClip->strName;
}

void CAnimation::ClipPosUpdate(bool bHorizon, float fScale)
{
	if (!m_pClip)
		return;

	// 현재 클립의 출력위치를 담당하는 것은 애니메이션을 셋팅하는 과정에서 하는 출력 크기 설정.
	// 출력이 어떻게 이루어지는지 확인해보자.
	
	// begin과 end의 위치를 모두 변경한다.
	// begin을 바꾸는 이유는 직접적인 출력에 영향을 주는 값이기 때문이다.
	// begin + m_tSize 를 통해서 해당 부분을 출력해주고 있다.
	// 따라서 begin값을 증가/감소에 따라서, 출력위치를 변경시킬 수 있따.
	// 그렇다고 end의 위치를 변경하지 않으면, 나중에 ChangeClip을 하는 과정에서 
	// 출력사이즈가 변하게 되는 현상이 일어날 것이다.(오류) 우리는 단순히 출력위치를 변경했을 뿐인데 사이즈가 바뀌는 현상이 생김(오류)오류
	// 따라서, 출력과는 상관이 없는 end라고 하더라도 반드시 업데이트 해줘야 한다. (나중에 출력 사이즈를 잡는 상황을 위해서)

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
	
	// 출력 위치를 바꾸었고, 이 결과는 begin이 변하면서 오브젝트에서 바로 적용이 된다.
	// 사이즈는 그대로 이용하니까.
	
}

void CAnimation::ClipSizeUpdate(bool bHorizon, float fScale)
{
	if (!m_pClip)
		return;

	// 사이즈를 변경하는 함수이다.
	// 현재 클립의 end좌표를 변경시키고,
	// 오브젝트의 size에 변화를 준다.


	vector<ClipCoord>& vecCoord = m_pClip->vecCoord;

	// 애니메이션의 srcPos를 얻어온다.
	Position tSrcPos = vecCoord[0].tStart;

	// 기존 프레임의 Size를 얻어온다.
	Position tFrameSize = vecCoord[0].tEnd - tSrcPos;

	// fScale값을 중첩하여 적용이될 프레임 사이즈를 구한다.
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

	// Size를 변경한다.
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

		// 문제점. End의 위치를 바꾼다.
		// Size를 바꾼다. 
		// 이것은 한 프레임당 출력하는 사이즈가 바뀌었다는 것을 의미한다.
		// 근데, 단순히 end값만 바뀌면, 처음 프레임 늘어난 크기로 출력한 이후에
		// 다음 프레임을 출력하는 상황에서는 기존의 begin의 위치가 그대로라서 증가된 이전 프레임의 end 좌표가 다음 프레임의 begin의 영역을
		// 침범하는 상황이 발생한다.
		// 즉, 프레임당 크기는 증가했다고 볼 수 있지만, 프레임의 출력 영역이 겹치는 상황이 생기게 된다.
		// 프레임이 겹치지 않도록 다시 로직을 짜도록 한다.

}

void CAnimation::ChangeClip(float fScale)
{
	if (!m_pClip)
		return;

	// 현재 돌리고 있는 클립의 위치를 알아온다.

	// 반복자를 막 사용하는 경우에는 end에 도달하게 될 수 있다.

	// map의 특징을 이용해서 처리한다.

	unordered_map<string, PAnimationClip>::iterator iter = m_mapClip.find(m_pClip->strName);

	if (fScale)
	{
		++iter;

		if (iter == m_mapClip.end())
		{
			// 마지막 원소에서 증가를 시켜서 end를 가리켰다면, begin을 준다.
			iter = m_mapClip.begin();
		}

		// end를 가리키지 않은 경우에는 냅둔다.
	}
	else
	{
		if (iter == m_mapClip.begin()) // 가리키고 있던 위치가 처음에 원소에 해당한다면, 
		{
			// 처음 원소의 이전으로 마지막원소를 준다.

			iter = (--m_mapClip.end());
		}
		else
			--iter; // 처음원소를 가리키고 있지 않은 경우 그냥 감소.
	}

	// 이렇게 iter는 바꿔야하는 클립을 들고있게 된다.

	// 클립을 바꿔준다.
	
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
	// 현재 클립의 정보를 저장한다.
	FILE* pFile = nullptr;

	fopen_s(&pFile, pFileName, "wb");

	if (!pFile)
		return false;


	char strName[50] = {};
	strcpy_s(strName, sizeof(char) * 50, m_pClip->strName.c_str());

	// 이름의 크기를 저장한다.
	int iLength = strlen(strName);
	
	fwrite(&iLength, sizeof(int), 1, pFile);

	// 이름의 길이만큼 저장한다.
	fwrite(strName, sizeof(char), iLength, pFile);

	// 옵션을 저장한다.
	if (bOption) // 옵션을 사용하는 경우 해당 옵션대로.
		fwrite(&eOption, sizeof(ANIMATION_OPTION), 1, pFile);
	else // 사용하지 않는 경우라면 그냥 이전 옵션을 그대로 적용.
		fwrite(&m_pClip->ePrevOption, sizeof(ANIMATION_OPTION), 1, pFile);

	// ColorKey 여부를  저장한다.
	fwrite(&m_pClip->bColorKey, sizeof(bool), 1, pFile);

	// ColorKey가 있다면 컬러키를 지정한다.
	if (m_pClip->bColorKey)
	{
		fwrite(&m_pClip->iColorKey, sizeof(unsigned int), 1, pFile);
	}

	// PlayTime을 저장한다.
	fwrite(&m_pClip->fPlayTime, sizeof(float), 1, pFile);

	// FrameTime을 저장한다.
	fwrite(&m_pClip->fFrameTime, sizeof(float), 1, pFile);

	// OptionPlayTime을 저장한다.
	fwrite(&m_pClip->fOptionPlayTime, sizeof(float), 1, pFile);

	// iOnceLoopFrame을 저장한다.
	fwrite(&m_pClip->iOnceLoopFrame, sizeof(int), 1, pFile);

	// tOffset을 저장한다.
	fwrite(&m_pClip->tOffset, sizeof(Position), 1, pFile);

	// Coord 정보의 사이즈를 저장한다.
	vector<ClipCoord>& vecCoord = m_pClip->vecCoord;

	iLength = vecCoord.size();
	fwrite(&iLength, sizeof(int), 1, pFile);

	// 사이즈를 토대로 ClipCoord정보를 저장한다.
	for (int i = 0; i < iLength; ++i)
	{
		fwrite(&vecCoord[i], sizeof(ClipCoord), 1, pFile);
	}

	fclose(pFile);

	return true;
}

bool CAnimation::LoadClip(const char * pFileName)
{

	// 현재 클립의 정보를 저장한다.
	FILE* pFile = nullptr;

	fopen_s(&pFile, pFileName, "rb");

	if (!pFile)
		return false;

	AnimationClip tInfo = {};

	char strName[50] = {};

	// 이름의 크기를 읽어온다..
	int iLength = 0;

	fread(&iLength, sizeof(int), 1, pFile);

	// 이름의 길이만큼 읽어온다.
	fread(strName, sizeof(char), iLength, pFile);

	// 읽어온 길이를 string에 저장한다.
	tInfo.strName = strName;

	// 옵션을 읽어온다.
	fread(&tInfo.eOption, sizeof(ANIMATION_OPTION), 1, pFile);

	// ColorKey 여부를  읽어온다.
	fread(&tInfo.bColorKey, sizeof(bool), 1, pFile);

	// ColorKey가 있다면 컬러키를 지정한다.
	if (tInfo.bColorKey)
	{
		fread(&tInfo.iColorKey, sizeof(unsigned int), 1, pFile);
	}

	// PlayTime을 저장한다.
	fread(&tInfo.fPlayTime, sizeof(float), 1, pFile);

	// FrameTime을 저장한다.
	fread(&tInfo.fFrameTime, sizeof(float), 1, pFile);

	// OptionPlayTime을 저장한다.
	fread(&tInfo.fOptionPlayTime, sizeof(float), 1, pFile);


	// iOnceLoopFrame을 읽어온다.
	fread(&tInfo.iOnceLoopFrame, sizeof(int), 1, pFile);

	//// Offset 정보를 읽어온다.
	fread(&tInfo.tOffset, sizeof(Position), 1, pFile);


	// Coord 정보의 사이즈를 읽어온다.

	iLength = 0;
	fread(&iLength, sizeof(int), 1, pFile);



	tInfo.vecCoord.clear();

	// 사이즈를 토대로 ClipCoord정보를 저장한다.
	for (int i = 0; i < iLength; ++i)
	{
		ClipCoord tCoord = {};

		fread(&tCoord, sizeof(ClipCoord), 1, pFile);

		// 읽어온 정보를 vector에 데입.
		tInfo.vecCoord.push_back(tCoord);
	}

	fclose(pFile);

	// 추가한 정보를 map에 셋팅해준다.
	// 해당 클립은 생성, coord 정보, 텍스처 그외...
	// 생성과 텍스처는 이미 되어 있고, 우리는 coord정보와 그외의 정보만 처리하면 된다. (데입하면 된다)

	PAnimationClip pClip = FindClip(tInfo.strName); 

	// 저장할 클립이 없다면, false;
	if (!pClip)
		return false;

	// 저장할 클립이 있다면 정보를 복사한다. 여기서 텍스처와 같은 정보는 제외한다.
	pClip->eOption = tInfo.eOption;
	pClip->ePrevOption = tInfo.eOption;
	pClip->eType = AT_ATLAS;
	pClip->bColorKey = tInfo.bColorKey;
	if (tInfo.bColorKey)
	{
		pClip->iColorKey = tInfo.iColorKey;
		
		// 불러오는 시점에서 텍스터가 존재한다.
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
	// 자신의 이름으로 저장을 하는 것으로 하자. (ㅇㅇㅇAnimation.op)
	FILE* pFile = nullptr;

	fopen_s(&pFile, pFileName, "wb");

	if (!pFile)
		return false;

	// 클립의 개수를 저장한다.
	int iCount = m_mapClip.size();

	fwrite(&iCount, sizeof(int), 1, pFile);

	unordered_map<string, PAnimationClip>::iterator iter;
	unordered_map<string, PAnimationClip>::iterator iterEnd = m_mapClip.end();

	for (iter = m_mapClip.begin(); iter != iterEnd; ++iter)
	{
		PAnimationClip pCilp = iter->second;

		char strName[50] = {};
		strcpy_s(strName, sizeof(char) * 50, pCilp->strName.c_str());

		// 이름의 크기를 저장한다.
		int iLength = strlen(strName);

		fwrite(&iLength, sizeof(int), 1, pFile);

		// 이름의 길이만큼 저장한다.
		fwrite(strName, sizeof(char), iLength, pFile);

		// 애니메이션 옵션을 저장.
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

	// 개수를 읽는다.
	int iCount = 0;
	fread(&iCount, sizeof(int), 1, pFile);

	// 개수 만큼 파일을 읽는다.
	for (int i = 0; i < iCount; ++i)
	{
		char strName[50] = {};

		// 이름의 크기를 읽는다.
		int iLength = 0;

		fread(&iLength, sizeof(int), 1, pFile);

		// 이름의 길이만큼 읽는다..
		fread(strName, sizeof(char), iLength, pFile);

		// 옵션정보를 읽는다.
		ANIMATION_OPTION eOption = AO_LOOP;

		fread(&eOption, sizeof(ANIMATION_OPTION), 1, pFile);


		// 이름을 가지고 클립을 찾아온다.
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
	// 이미 strName을 이름으로 가진 Clip이 존재하는 경우에는 추가하지 않는다.

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
		// 만약에 처음에 추가가 된 클립이라면, 기본으로 셋팅한다.
	}
	
	return true;
}

bool CAnimation::SetClipTexture(const string & strClipName, 
	const string & strTexName, const TCHAR * pFileName,
	const string & strPathName)
{
	// strClipName에 해당하는 클립을 찾아와서 그 클립의 텍스처를 지정하는 기능이다.
	// 여기서 텍스처를 Load해주고, 참조가 일어난다.
	// 정확하게는 ResourceManager에서 한번의 참조가 일어나고,
	// 두 번째로 AnimationClip에서 가지고 있으면서 참조가 일어난다.
	

	PAnimationClip pClip = FindClip(strClipName);

	if (!pClip)
		return false;

	// 이미지 한 장을 로드하는 함수를 사용한다는 것은 아틀라스 이미지를 사용하는 것으로 볼 수 있다.
	pClip->eType = AT_ATLAS;

	// 리소스 매니저에 해당 텍스처를 로드하도록 한다.
	GET_SINGLE(CResourceManager)->LoadTexture(strTexName, pFileName, strPathName);

	// 이렇게 하여 TexName을 키값으로 하여 ResourceManager가 들고있는 map에 해당 정보를 추가하고,
	// 여기서 참조 한번
	
	// 이렇게 추가된 Texture를 찾아서 pClip에게 준다.

	pClip->pTexture = GET_SINGLE(CResourceManager)->FindTexture(strTexName);
	// 참조 

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
	// strName의 이름을 가진 클립을 찾아와서 해당 클립의 텍스처의 컬러키를 바꿔준다.
	PAnimationClip pClip = FindClip(strName);

	if (!pClip)
		return;

	pClip->pTexture->SetColorKey(r, g, b);
	// 클립의 컬러키를 설정한다.
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
	// 클립의 Coord를 설정하는 함수이다. 
	// 여러개의 좌표를 설정하는 함수로, 아틀라스 전용이다.

	PAnimationClip pClip = FindClip(strName);

	if (!pClip)
		return false;

	pClip->vecCoord = vecCoord;

	pClip->fFrameTime = pClip->fPlayTime / vecCoord.size();
	// 한 프레임의 시간은, 전체 시간 / 장면수 

	return true;
}

bool CAnimation::AddClipCoord(const string & strName,
	const ClipCoord & tCoord, int iCount)
{
	// 하나의 좌표를 여러번 추가하는 함수이다.
	// 프레임 함수이다.

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
	// bHorizon은, 아틀라스 이미지가 가로로 있는 상황을 가정하는 것이다.
	// 만약 이 값이 false라면, 인덱스 * iFrameY를 통해서 세로로 좌표를 추가할 것이다.
	// bRight은 좌우 애니메이션을 판단하는 과정에서 사용하는 변수이다.
	// 기본적으로 리소스가 Right를 기준으로 구성이 되어 있어서
	// Right이미지를 넣는 경우에는 시작 좌표와 프레임 X, Y의 크기를 넣어주면, 자동으로 애니메이션을 추가한다.
	// 하지만, Left의 이미지를 넣는 경우에는 시작 좌표를 넣으면, 이미지의 순서가 거꾸로 들어가게 된다.
	// Right : A B C Left(Right의 반전) : C B A 
	// 같은 위치를 넣는다고 할때, Left와 Right의 시작 이미지가 달리지게 된다.
	
	// 따라서, Left의 이미지를 넣는 경우에는 마지막 이미지부터 순서대로 넣는 시스템을 구현하도록 한다.
	// 일단, 넣어주는 시작 좌표는 그대로 사용할 수 있도록 한다. (애니메이션의 추가 용이성을 위해서)
	// 시작 좌표를 오른쪽과 똑같이 넣어주면, 왼쪽을 기준으로 추가하는 과정에서는 자동으로 마지막 프레임부터 넣어주는 시스템.
	// 우리는 이것을 스택으로 구현한다.
	

	// 프레임의 이미지 개수 만큼 for문을 돌려서 좌표를 넣어주는 처리를 한다.

	vector<ClipCoord> vecCoord;
	// 좌표를 넣어서 처리해줄 vector


		if (bHorizon)
		{
			// 수평선으로 추가하는 경우.

				// 가로로 좌표를 추가한다.
				for (int i = 0; i < iCount; ++i)
				{
					ClipCoord tCoord;
					// 시작좌표부터 첫 프레임의 좌표를 넣어준다.

					tCoord.tStart = tSrcStartPos;

					tCoord.tStart.x += iFrameSizeX * i;
					// begin의 y좌표는 바뀌지 않는다. startPos와 동일하다. horizon이니까.

					tCoord.tEnd = tCoord.tStart + Position(iFrameSizeX, iFrameSizeY);

					vecCoord.push_back(tCoord);
						
				}

		}
		else
		{
			// 수직선으로 추가하는 경우

				for (int i = 0; i < iCount; ++i)
				{
					ClipCoord tCoord;
					// 시작좌표부터 첫 프레임의 좌표를 넣어준다.

					tCoord.tStart = tSrcStartPos;

					tCoord.tStart.y += iFrameSizeY * i;
					// begin의 y좌표는 바뀌지 않는다. startPos와 동일하다. horizon이니까.

					tCoord.tEnd = tCoord.tStart + Position(iFrameSizeX, iFrameSizeY);

					vecCoord.push_back(tCoord);
				}
		}

		// 이렇게 추가할 좌표를 생성했다면, vector를 받는 함수를 호출하여 한번에 처리할 수 있도록 한다.

		if(!bRight)
		{
			stack<ClipCoord> stackCoord;

			// 왼쪽을 기준으로 하는 경우라면, 추가된 것을 반대로 넣어줘야 한다.
			// 따라서, vector에 추가된 것을 순서대로 stack에 넣고,  vector에 다시 이것을 넣어서 덮어씌운다.

			for (int i = 0; i < iCount; ++i)
			{
				stackCoord.push(vecCoord[i]);
			}

			for (int i = 0; i < iCount; ++i)
			{
				vecCoord[i] = stackCoord.top();
				stackCoord.pop();
			}

			// 이렇게 하여 우리가 원하는 순서대로 들어간다.

		}

		return AddClipCoord(strName, vecCoord);

		// 오른쪽을 기준으로 하면 그대로 vector를 넣어주고
		// 왼쪽을 기준으로 한다면, 스택을 이용해서 반대로 vector에 넣어주고 (제대로 순서를 맞춰서) 이것을 vector로 넣어준다.

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

	// 기존에 사용하던 클립의 정보를 초기상태로.

	m_pClip->iFrame = 0;
	m_pClip->fTime = 0.f;
	m_pClip->fOptionTime = 0.f;
	m_pClip = FindClip(strName);

	m_pObj->SetTexture(m_pClip->pTexture);  // 기존 텍스처 참조 풀고, 현재 텍스처 참조 증가.

	if(!m_pClip->vecCoord.empty())
		m_pObj->SetSize(m_pClip->vecCoord[0].tEnd - m_pClip->vecCoord[0].tStart); 
	
	// 출력 사이즈에 맞게 설정.
	// 오브젝트의 사이즈는 이미지를 출력하는데 사용하는 크기이다.
	// 텍스처는, 크기를 가지고 있지 않는다.
	// 단지 오브젝트의 출력사이즈를 넘겨줘서 대신 텍스처에서 출력하도록 만들어져 있을뿐.
	// 따라서, 출력하는 이미지가 달라졌다는 의미는 출력하는 크기를 변경시켜 줘야함을 의미하고,
	// 이것은 오브젝트의 사이즈를 변경해야 한다는 것이다.
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

	// 시간을 기반으로 하여 다음 프레임에 해당하는 장면을 설정해야 하는 경우에
	// 프레임을 증가시키며, 텍스처에서는 이 프레임 정보를 이용하여, 자신이 출력해야하는 DC를 알아낸다.

	m_pClip->fTime += fTime;

	if (m_pClip->eOption == AO_TIME_DESTROY)
	{
		m_pClip->fOptionTime += fTime;

		if (m_pClip->fOptionTime >= m_pClip->fOptionPlayTime)
		{
			m_pObj->SetActive(false);


			
			// 오브젝트가 죽으면, 자신의 애니메이션 정보는 모두 날라가므로 신경써줄 필요는 없다.
		}
	}

	// 프레임이 끊기더라도 시간에 따라서 여러장의 프레임을 넘어갈 수 있도록 한다.

	while (m_pClip->fTime >= m_pClip->fFrameTime)
	{
		m_pClip->fTime -= m_pClip->fFrameTime;

		++m_pClip->iFrame;
		
		if (m_pClip->iFrame == m_pClip->vecCoord.size())
		{
			// 한번을 모두 돌린 경우에..

			m_pClip->iFrame = 0;

			switch (m_pClip->eOption)
			{
			case AO_ONCE_DESTROY:


				if (!m_pObj->GetDieEnable())
					m_pObj->SetActive(false);

				m_pClip->iFrame = m_pClip->vecCoord.size() - 1;

				// 이곳에서 죽이는 처리를 하지 않고 once_loop처럼 처리한다.
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
	
	// ao_loop의 경우에는 계속 돌아가는 것이다. 프레임 0부터 다시.
}

CAnimation * CAnimation::Clone() const
{
	return new CAnimation(*this);
}

void CAnimation::SetObj(CObj * pObj)
{
	m_pObj = pObj;
}
