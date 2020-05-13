#pragma once
#include "../Ref.h"

class CAnimation :
	public CRef
{

	friend class CEditManager;
	friend class CObj; 
	// Obj에서 애니메이션을 설정해줄 것이다


private:
	CAnimation();
	CAnimation(const CAnimation& ani);
	~CAnimation();

	// 애니메이션은 각 프로토타입에 생성이 되어, 맵에 배치하는 과정에서 객체마다 따로따로 돌려줘야하는 
	// 리소스이다.

private:
	unordered_map<string, PAnimationClip>	m_mapClip;			// 클립 정보를 담아둘 map
	PAnimationClip							m_pClip;				// 현재 돌리고 있는 클립정보이다.
	class CObj*								m_pObj;					// 애니메이션을 가지고 있는 오브젝트의 정보
	string									m_strDefaultClip;		// Return에 해당하는 옵션에서 사용할 기본 클립의 이름

public:
	PAnimationClip GetClip() const
	{
		return m_pClip;
	}

	string GetDefaultClip() const
	{
		return m_strDefaultClip;
	}

	string GetCurrentAnimation() const;


	// 현재 돌리고 있는 클립의 vector 위치를 (=출력위치)변경해주고, 이것이 눈에 바로 보이도록 적용하는 함수이다.
	void ClipPosUpdate(bool bHorizon, float fScale);
	void ClipSizeUpdate(bool bHorizon, float fScale);
	void ChangeClip(float fScale);
	void AddFrameTime(float fTime);

	// 클립 정보 저장함수.
	bool SaveClip(const char* pFileName, bool bOption = false, ANIMATION_OPTION eOption = AO_LOOP); // 편집용으로 사용.
	bool LoadClip(const char* pFileName);							// 모두 사용


	bool SaveClipOption(const char* pFileName); // 클립 이름 + 옵션의 정볼를 저장한다.
	bool LoadClipOption(const char* pFileName);


public:

	void CurrentClipClear();

	bool AddClip(const string& strName, ANIMATION_OPTION eOption = AO_LOOP,
		float fPlayTime = 1.f, float fOptionPlayTime = 0.f);
	// 클립 정보를 추가하는 함수이다.
	// strName을 키로하는 클립 정보를 추가하고, option, PlayTime, 옵션에 따른 optionPlayTime의 값을 넣어주고 있다.
	// 출력 좌표나, 텍스처에 대한 셋팅은 다른 함수에서 해주고 있다.
	
	bool SetClipTexture(const string& strClipName, const string& strTexName,
		const TCHAR* pFileName, const string& strPathName = TEXTURE_PATH);
	// 일단 이 함수의 기능은, 아틀라스에 해당하는 텍스처를 Clip에 추가해주는 기능이다.
	// ResourceManager를 통해서, LoadTexture함수를 호출하고, LoadTexture함수는 들어온 인자를 토대로하여
	// 텍스처 클래스의 LoadTexture함수를 호출한다.
	// 그러면 텍스처 클래스는 읽어온 비트맵 정보를 저장하게 되는데, 여기서 ResourceManager는 이것을 map정보에 저장하고
	// ref = 1
	// 이렇게 추가한 정보를 ResourceManager를 통해서 찾아와 Clip에 셋팅하게 되는 것이다.
	// 여기서 참조가 일어난다. ref = 2
	// 소멸시킬때 줄여줘야 한다.
	// 여기서 한번 줄여주고 나중에 ResourceManager 의 소멸자에서 한번 줄여주면, 리소스가 반환된다.

	bool SetClipTexture(const string& strClipName, const string& strTexName,
		const vector<const TCHAR*>& vecFileName, const string& strPathName = TEXTURE_PATH);
	// 여러 장의 이미지를 읽어오는 함수이다. 즉, 프레임 애니메이션을 추가하는데 이용하는 함수라고 할 수 있다.

	void SetTextureColorKey(const string& strName,
		unsigned char r, unsigned char g, unsigned char b);
	// 해당이름을 가진 Clip을 찾아와서 해당 Clip에 있는 텍스처 정보의 ColorKey를 설정해준다.

	// 여러개의 Clip정보에 ColorKey를 지정하는 함수이다.
	void SetTextureColorKeyAll(unsigned char r, unsigned char g, unsigned char b);


	bool AddClipCoord(const string& strName, const vector<ClipCoord>& vecCoord);
	// cood vec의 정보대로 출력 좌표를 셋팅해주는 함수이다.

	bool AddClipCoord(const string& strName, const ClipCoord& tCoord,
		int iCount);
	// tCoord의 좌표를 iCount만큼 넣는 개념인데, 프레임 애니메이션에서 사용하는 변수이다.
	// 이미지의 사이즈가 같으니까, 같은 출력위치가 여러개 필요할 것이다.

	Size GetBmpSize() const;





	// 애니메이션의 Coord를 넣어주는 함수를 하나 더 만든다. 
	// 주요 기능. 시작 좌표 left top을 넣어주고,  프레임 사이즈, 장면수를 넣어주면 아틀라스 이미지의 좌표를 인식하여 추가하는 함수.
	// const vector<ClipCoord>를 받는 함수의 상위 호환. 
	// Coord함수를 사용한다는 의미는 이미 SetClipTexture를 통해서 리소스를 받아왔다는 것을 의미한다.
	// 아틀라스이미지를 받아온 상태에서 호출하는 함수.
	bool AddClipCoord(const string& strName, const Position& tSrcStartPos, int iFrameSizeX, int iFrameSizeY, 
		int iCount, bool bRight = true, bool bHorizon = true);







	// 자신의 클립이 가지고 있는 모든 텍스처에 대하여 참조횟수를 증가시키는 처리.
	void AddRefAllClipTexture();


	void SetDefaultClip(const string& strName);	// DefaultClip의 Key값을 설정하는 함수인데 return option에서 이용하는 값이다
	void SetCurrentClip(const string& strName); // strName 에 해당하는 애니메이션을 현재의 클립으로 설정하는 함수이다.
	void ChangeClip(const string& strName); // strName을 가진 클립으로 애니메이션을 변경하는 함수이다.

	PAnimationClip FindClip(const string& strName);
	//void ClearOptionTime(); // 만약에 버프와 같은 정보가 있을 때 이것의 옵션값만 바꿔주는 기능이다.
	// 근데 내 게임에서 안 쓴다.

public:
	void Start();
	bool Init();
	void Update(float fTime);
	CAnimation* Clone() const;
	

public:
	void SetObj(class CObj* pObj);



};

