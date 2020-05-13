#pragma once



typedef struct _tagResolution
{
	int iWidth;
	int iHeight;

}Resolution, *PResolution;

typedef struct _tagPosition
{
	float x, y;

	_tagPosition() :
		x(0.f),
		y(0.f)
	{}

	_tagPosition(float x, float y) :
		x(x),
		y(y)
	{}

	_tagPosition(int _x, int _y)
	{
		x = (float)_x;
		y = (float)_y;
	}

	_tagPosition(const _tagPosition& tPos)
	{
		// 복사 생성자.

		x = tPos.x;
		y = tPos.y;
	}

	_tagPosition(const POINT& pt)
	{
		x = (float)pt.x;
		y = (float)pt.y;
	}

	// 연산자.

	void operator = (const _tagPosition& tPos)
	{
		x = tPos.x;
		y = tPos.y;
	}

	void operator = (const POINT& pt)
	{
		x = (float)pt.x;
		y = (float)pt.y;
	}

	void operator = (float f)
	{
		x = f;
		y = f;
	}

	void operator = (int i)
	{
		x = (float)i;
		y = (float)y;
	}

	// + 
	_tagPosition operator + (const _tagPosition& tPos) const
	{
		_tagPosition m_tPos;

		m_tPos.x = x + tPos.x;
		m_tPos.y = y + tPos.y;

		return m_tPos;
	}

	_tagPosition operator + (const POINT& pt) const
	{

		_tagPosition m_tPos;

		m_tPos.x = x + (float)pt.x;
		m_tPos.y = y + (float)pt.y;

		return m_tPos;
	}

	_tagPosition operator + (float f) const
	{
		_tagPosition m_tPos;

		m_tPos.x = x + f;
		m_tPos.y = y + f;

		return m_tPos;
	}

	_tagPosition operator + (int i) const
	{
		_tagPosition m_tPos;

		m_tPos.x = x +(float)i;
		m_tPos.y = y +(float)i;

		return m_tPos;
	}

	void operator += (const _tagPosition& tPos)
	{
		x += tPos.x;
		y += tPos.y;
	}

	void operator += (const POINT& pt)
	{
		x += (float)pt.x;
		y += (float)pt.y;
	}

	void operator += (float f)
	{
		x += f;
		y += f;
	}

	void operator += (int i)
	{
		x += (float)i;
		y += (float)i;
	}


	// - 
	_tagPosition operator - (const _tagPosition& tPos) const
	{
		_tagPosition m_tPos;

		m_tPos.x = x - tPos.x;
		m_tPos.y = y - tPos.y;

		return m_tPos;
	}

	_tagPosition operator - (const POINT& pt) const
	{

		_tagPosition m_tPos;

		m_tPos.x = x - (float)pt.x;
		m_tPos.y = y - (float)pt.y;

		return m_tPos;
	}

	_tagPosition operator - (float f) const
	{
		_tagPosition m_tPos;

		m_tPos.x = x - f;
		m_tPos.y = y - f;

		return m_tPos;
	}

	_tagPosition operator - (int i) const
	{
		_tagPosition m_tPos;

		m_tPos.x = x - (float)i;
		m_tPos.y = y - (float)i;

		return m_tPos;
	}

	void operator -= (const _tagPosition& tPos)
	{
		x -= tPos.x;
		y -= tPos.y;
	}

	void operator -= (const POINT& pt)
	{
		x -= (float)pt.x;
		y -= (float)pt.y;
	}

	void operator -= (float f)
	{
		x -= f;
		y -= f;
	}

	void operator -= (int i)
	{
		x -= (float)i;
		y -= (float)i;
	}



	// * 
	_tagPosition operator * (const _tagPosition& tPos) const
	{
		_tagPosition m_tPos;

		m_tPos.x = x * tPos.x;
		m_tPos.y = y * tPos.y;

		return m_tPos;
	}

	_tagPosition operator * (const POINT& pt) const
	{

		_tagPosition m_tPos;

		m_tPos.x = x * (float)pt.x;
		m_tPos.y = y * (float)pt.y;

		return m_tPos;
	}

	_tagPosition operator * (float f) const
	{
		_tagPosition m_tPos;

		m_tPos.x = x * f;
		m_tPos.y = y * f;

		return m_tPos;
	}

	_tagPosition operator * (int i) const
	{
		_tagPosition m_tPos;

		m_tPos.x = x * (float)i;
		m_tPos.y = y * (float)i;

		return m_tPos;
	}

	void operator *= (const _tagPosition& tPos)
	{
		x *= tPos.x;
		y *= tPos.y;
	}

	void operator *= (const POINT& pt)
	{
		x *= (float)pt.x;
		y *= (float)pt.y;
	}

	void operator *= (float f)
	{
		x *= f;
		y *= f;
	}

	void operator *= (int i)
	{
		x *= (float)i;
		y *= (float)i;
	}


	// / 
	_tagPosition operator / (const _tagPosition& tPos) const
	{
		_tagPosition m_tPos;

		m_tPos.x = x / tPos.x;
		m_tPos.y = y / tPos.y;

		return m_tPos;
	}

	_tagPosition operator / (const POINT& pt) const
	{

		_tagPosition m_tPos;

		m_tPos.x = x / (float)pt.x;
		m_tPos.y = y / (float)pt.y;

		return m_tPos;
	}

	_tagPosition operator / (float f) const
	{
		_tagPosition m_tPos;

		m_tPos.x = x / f;
		m_tPos.y = y / f;

		return m_tPos;
	}

	_tagPosition operator / (int i) const
	{
		_tagPosition m_tPos;

		m_tPos.x = x / (float)i;
		m_tPos.y = y / (float)i;

		return m_tPos;
	}

	void operator /= (const _tagPosition& tPos)
	{
		x /= tPos.x;
		y /= tPos.y;
	}

	void operator /= (const POINT& pt)
	{
		x /= (float)pt.x;
		y /= (float)pt.y;
	}

	void operator /= (float f)
	{
		x /= f;
		y /= f;
	}

	void operator /= (int i)
	{
		x /= (float)i;
		y /= (float)i;
	}

	void operator ++()
	{
		x += 1.f;
		y += 1.f;
	}

	void operator ++(int)
	{
		x += 1.f;
		y += 1.f;
	}

	void operator --()
	{
		x -= 1.f;
		y -= 1.f;
	}

	void operator --(int)
	{
		x -= 1.f;
		y -= 1.f;
	}


}Position, *PPosition, Size, *PSize;



typedef struct _tagCollisionChannel
{
	string strName;
	COLLISION_CHANNEL_STATE		eState;

	// 이름과 상태정보를 저장할 수 있도록한다.
	// 채널은 프로파일이 각각 가지고 있는 정보로서, 자신의 프로파일에 해당하는 객체가
	// 상대 채널의 객체를 만난 경우에 어떤 식으로 처리할지를 가지고 있는 정보라고 할 수 있다.
	// 어떤 채널과 만나서, 충돌 처리를 할지 말지만 결정하기 때문에 별다른 정보가 필요가 없다.

	_tagCollisionChannel()
	{
		eState = CCS_IGNORE;
	}

}CollisionChannel, *PCollisionChannel;

typedef struct _tagCollisionProfile
{
	string	strName;					// 프로파일을 찾아오기 위한 문자열
	PCollisionChannel	pChannelList; // 프로파일은 채널을 여러개 가지고 다닌다.
	int					m_iChannelSize; // 채널이 몇 개가 있는가.
	
	_tagCollisionProfile() :
		pChannelList(nullptr),
		m_iChannelSize(0)
	{

	}

	~_tagCollisionProfile()
	{
		// 구조체의 소멸자이다.
		// 우리는 해당 구조체에 동적할당하여 사용할 예정이기 때문에
		// 반드시 메모리를 반환해줘야 한다.
		// 이것을 프로파일에게 자동으로 맞긴다.

		SAFE_DELETE_ARRAY(pChannelList);
	}

	// 나중에 프로파일을 정리하는 경우에 프로파일 할당 부분을 한번에 날리면 각 프로파일에 속해있는
	// 채널의 정보도 모두 날리는 것이다.
	
}CollisionProfile, *PCollisionProfile;

typedef struct _tagSphereInfo
{
	// 원의 정보는 중심의 좌표, 반지름이면 충분하다

	Position	tCenter;
	float		fRadius;

	_tagSphereInfo() :
		fRadius(0.f)
	{}

	_tagSphereInfo operator + (const Position& tPos) const
	{
		_tagSphereInfo result;
		result.fRadius = fRadius;
		result.tCenter = tCenter + tPos;

		return result;
	}

	void operator += (const Position& tPos)
	{
		tCenter += tPos;
	}

	_tagSphereInfo operator - (const Position& tPos) const
	{
		_tagSphereInfo result;
		result.fRadius = fRadius;
		result.tCenter = tCenter - tPos;

		return result;
	}

	void operator -= (const Position& tPos)
	{
		tCenter -= tPos;
	}
}SphereInfo, *PSphereInfo;

typedef struct _tagClipCoord
{
	Position tStart;
	Position tEnd;

	_tagClipCoord()
	{

	}

	_tagClipCoord(const _tagPosition& tSrc, const _tagPosition& tDest)
	{
		tStart = tSrc;
		tEnd = tDest;
	}

	_tagClipCoord(int x, int y, int _x, int _y)
	{
		tStart = Position(x, y);
		tEnd = Position(_x, _y);
	}

	void operator = (const _tagClipCoord& tCoord)
	{
		tStart = tCoord.tStart;
		tEnd = tCoord.tEnd;
	}

}ClipCoord, *PClidCoord;

typedef struct _tagRectInfo
{
	float l, t, r, b;

	_tagRectInfo() :
		l(0.f),
		t(0.f),
		r(0.f),
		b(0.f)
	{}

	_tagRectInfo(float l, float t, float r, float b) :
		l(l),
		t(t),
		r(r),
		b(b)
	{
	}

	_tagRectInfo(const _tagClipCoord& tCoord)
	{
		l = tCoord.tStart.x;
		t = tCoord.tStart.y;
		r = tCoord.tEnd.x;
		b = tCoord.tEnd.y;
	}

}RectInfo, *PRectInfo;

typedef struct _tagAnimationClip
{
	// 애니메이션 클립은, 하나의 애니메이션을 표현하는 정보를 가지고 있다.

	string strName;
	ANIMATION_OPTION eOption;
	ANIMATION_TYPE	 eType;
	class CTexture*	 pTexture;
	unsigned int	 iColorKey; // 클립에서 컬러키를 설정해서, 출력하는 텍스처에 컬러키를 지정해줄 수 있도록 
									// 구현이 되어 있다.

	bool			 bColorKey;
	int				 iFrame; //  진행되고 있는 프레임 정보에 해당하는 Number이다 Index

	vector<ClipCoord> vecCoord;
	float fTime;	// 시간을 중첩시키는데 사용할 정보이다.
	float fPlayTime;
	float fFrameTime; // fPlayTime / vecCoord.size() 전체시간을 size로 나눠서 한 프레임당의 시간을 구할 수 있다.

	float fOptionTime;
	float fOptionPlayTime; // 옵션 정보에서 사용할 변수이다.
	// 프레임의 시간을 이용하는 처리를 하고 있지만, 실질적으로 해당 변수를 이용하여 동작을 취하고 있다.

	int iOnceLoopFrame;

	Position tOffset;

	// 지워이거
	ANIMATION_OPTION ePrevOption;

	_tagAnimationClip()
	{
		iOnceLoopFrame = 0;
		eType = AT_ATLAS;
		eOption = AO_LOOP;
		ePrevOption = AO_LOOP;
		pTexture = nullptr;
		iFrame = 0;
		iColorKey = 0;
		bColorKey = false;
		fTime = 0.f;
		fPlayTime = 1.f;
		fFrameTime = 1.f;
		fOptionTime = 0.f;
		fOptionPlayTime = 0.f;

		vecCoord.push_back(ClipCoord(0, 0, 50, 50));
	}


}AnimationClip, *PAnimationClip;


typedef struct _tagSpawner
{
	Position tPos;
	SIDE_DIR eSpawnDir;
	string strMonsterTag;
	bool	bCount;
	int		iCount;

	_tagSpawner() :
		eSpawnDir(SD_LEFT),
		bCount(false),
		iCount(0)
	{}

	_tagSpawner(const string& strMonsterTag, const Position& tPos,
		SIDE_DIR eDir = SD_LEFT,
		bool bCount = true, int iCount = 1) :
		tPos(tPos),
		eSpawnDir(eDir),
		bCount(bCount),
		iCount(iCount),
		strMonsterTag(strMonsterTag)
	{
		// 기본적으로 능력 있는 몬스터에 대한 설정으로 한다.
	}

}SPAWNERINFO, *PSPAWNERINFO;
