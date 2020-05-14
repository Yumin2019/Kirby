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
		// ���� ������.

		x = tPos.x;
		y = tPos.y;
	}

	_tagPosition(const POINT& pt)
	{
		x = (float)pt.x;
		y = (float)pt.y;
	}

	// ������.

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

	// �̸��� ���������� ������ �� �ֵ����Ѵ�.
	// ä���� ���������� ���� ������ �ִ� �����μ�, �ڽ��� �������Ͽ� �ش��ϴ� ��ü��
	// ��� ä���� ��ü�� ���� ��쿡 � ������ ó�������� ������ �ִ� ������� �� �� �ִ�.
	// � ä�ΰ� ������, �浹 ó���� ���� ������ �����ϱ� ������ ���ٸ� ������ �ʿ䰡 ����.

	_tagCollisionChannel()
	{
		eState = CCS_IGNORE;
	}

}CollisionChannel, *PCollisionChannel;

typedef struct _tagCollisionProfile
{
	string	strName;					// ���������� ã�ƿ��� ���� ���ڿ�
	PCollisionChannel	pChannelList; // ���������� ä���� ������ ������ �ٴѴ�.
	int					m_iChannelSize; // ä���� �� ���� �ִ°�.
	
	_tagCollisionProfile() :
		pChannelList(nullptr),
		m_iChannelSize(0)
	{

	}

	~_tagCollisionProfile()
	{
		// ����ü�� �Ҹ����̴�.
		// �츮�� �ش� ����ü�� �����Ҵ��Ͽ� ����� �����̱� ������
		// �ݵ�� �޸𸮸� ��ȯ����� �Ѵ�.
		// �̰��� �������Ͽ��� �ڵ����� �±��.

		SAFE_DELETE_ARRAY(pChannelList);
	}

	// ���߿� ���������� �����ϴ� ��쿡 �������� �Ҵ� �κ��� �ѹ��� ������ �� �������Ͽ� �����ִ�
	// ä���� ������ ��� ������ ���̴�.
	
}CollisionProfile, *PCollisionProfile;

typedef struct _tagSphereInfo
{
	// ���� ������ �߽��� ��ǥ, �������̸� ����ϴ�

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
	// �ִϸ��̼� Ŭ����, �ϳ��� �ִϸ��̼��� ǥ���ϴ� ������ ������ �ִ�.

	string strName;
	ANIMATION_OPTION eOption;
	ANIMATION_TYPE	 eType;
	class CTexture*	 pTexture;
	unsigned int	 iColorKey; // Ŭ������ �÷�Ű�� �����ؼ�, ����ϴ� �ؽ�ó�� �÷�Ű�� �������� �� �ֵ��� 
									// ������ �Ǿ� �ִ�.

	bool			 bColorKey;
	int				 iFrame; //  ����ǰ� �ִ� ������ ������ �ش��ϴ� Number�̴� Index

	vector<ClipCoord> vecCoord;
	float fTime;	// �ð��� ��ø��Ű�µ� ����� �����̴�.
	float fPlayTime;
	float fFrameTime; // fPlayTime / vecCoord.size() ��ü�ð��� size�� ������ �� �����Ӵ��� �ð��� ���� �� �ִ�.

	float fOptionTime;
	float fOptionPlayTime; // �ɼ� �������� ����� �����̴�.
	// �������� �ð��� �̿��ϴ� ó���� �ϰ� ������, ���������� �ش� ������ �̿��Ͽ� ������ ���ϰ� �ִ�.

	int iOnceLoopFrame;

	Position tOffset;

	// �����̰�
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
		// �⺻������ �ɷ� �ִ� ���Ϳ� ���� �������� �Ѵ�.
	}

}SPAWNERINFO, *PSPAWNERINFO;
