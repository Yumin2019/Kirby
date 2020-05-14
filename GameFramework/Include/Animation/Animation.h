#pragma once
#include "../Ref.h"

class CAnimation :
	public CRef
{

	friend class CEditManager;
	friend class CObj; 
	// Obj���� �ִϸ��̼��� �������� ���̴�


private:
	CAnimation();
	CAnimation(const CAnimation& ani);
	~CAnimation();

	// �ִϸ��̼��� �� ������Ÿ�Կ� ������ �Ǿ�, �ʿ� ��ġ�ϴ� �������� ��ü���� ���ε��� ��������ϴ� 
	// ���ҽ��̴�.

private:
	unordered_map<string, PAnimationClip>	m_mapClip;			// Ŭ�� ������ ��Ƶ� map
	PAnimationClip							m_pClip;				// ���� ������ �ִ� Ŭ�������̴�.
	class CObj*								m_pObj;					// �ִϸ��̼��� ������ �ִ� ������Ʈ�� ����
	string									m_strDefaultClip;		// Return�� �ش��ϴ� �ɼǿ��� ����� �⺻ Ŭ���� �̸�

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


	// ���� ������ �ִ� Ŭ���� vector ��ġ�� (=�����ġ)�������ְ�, �̰��� ���� �ٷ� ���̵��� �����ϴ� �Լ��̴�.
	void ClipPosUpdate(bool bHorizon, float fScale);
	void ClipSizeUpdate(bool bHorizon, float fScale);
	void ChangeClip(float fScale);
	void AddFrameTime(float fTime);

	// Ŭ�� ���� �����Լ�.
	bool SaveClip(const char* pFileName, bool bOption = false, ANIMATION_OPTION eOption = AO_LOOP); // ���������� ���.
	bool LoadClip(const char* pFileName);							// ��� ���


	bool SaveClipOption(const char* pFileName); // Ŭ�� �̸� + �ɼ��� ������ �����Ѵ�.
	bool LoadClipOption(const char* pFileName);


public:

	void CurrentClipClear();

	bool AddClip(const string& strName, ANIMATION_OPTION eOption = AO_LOOP,
		float fPlayTime = 1.f, float fOptionPlayTime = 0.f);
	// Ŭ�� ������ �߰��ϴ� �Լ��̴�.
	// strName�� Ű���ϴ� Ŭ�� ������ �߰��ϰ�, option, PlayTime, �ɼǿ� ���� optionPlayTime�� ���� �־��ְ� �ִ�.
	// ��� ��ǥ��, �ؽ�ó�� ���� ������ �ٸ� �Լ����� ���ְ� �ִ�.
	
	bool SetClipTexture(const string& strClipName, const string& strTexName,
		const TCHAR* pFileName, const string& strPathName = TEXTURE_PATH);
	// �ϴ� �� �Լ��� �����, ��Ʋ�󽺿� �ش��ϴ� �ؽ�ó�� Clip�� �߰����ִ� ����̴�.
	// ResourceManager�� ���ؼ�, LoadTexture�Լ��� ȣ���ϰ�, LoadTexture�Լ��� ���� ���ڸ� �����Ͽ�
	// �ؽ�ó Ŭ������ LoadTexture�Լ��� ȣ���Ѵ�.
	// �׷��� �ؽ�ó Ŭ������ �о�� ��Ʈ�� ������ �����ϰ� �Ǵµ�, ���⼭ ResourceManager�� �̰��� map������ �����ϰ�
	// ref = 1
	// �̷��� �߰��� ������ ResourceManager�� ���ؼ� ã�ƿ� Clip�� �����ϰ� �Ǵ� ���̴�.
	// ���⼭ ������ �Ͼ��. ref = 2
	// �Ҹ��ų�� �ٿ���� �Ѵ�.
	// ���⼭ �ѹ� �ٿ��ְ� ���߿� ResourceManager �� �Ҹ��ڿ��� �ѹ� �ٿ��ָ�, ���ҽ��� ��ȯ�ȴ�.

	bool SetClipTexture(const string& strClipName, const string& strTexName,
		const vector<const TCHAR*>& vecFileName, const string& strPathName = TEXTURE_PATH);
	// ���� ���� �̹����� �о���� �Լ��̴�. ��, ������ �ִϸ��̼��� �߰��ϴµ� �̿��ϴ� �Լ���� �� �� �ִ�.

	void SetTextureColorKey(const string& strName,
		unsigned char r, unsigned char g, unsigned char b);
	// �ش��̸��� ���� Clip�� ã�ƿͼ� �ش� Clip�� �ִ� �ؽ�ó ������ ColorKey�� �������ش�.

	// �������� Clip������ ColorKey�� �����ϴ� �Լ��̴�.
	void SetTextureColorKeyAll(unsigned char r, unsigned char g, unsigned char b);


	bool AddClipCoord(const string& strName, const vector<ClipCoord>& vecCoord);
	// cood vec�� ������� ��� ��ǥ�� �������ִ� �Լ��̴�.

	bool AddClipCoord(const string& strName, const ClipCoord& tCoord,
		int iCount);
	// tCoord�� ��ǥ�� iCount��ŭ �ִ� �����ε�, ������ �ִϸ��̼ǿ��� ����ϴ� �����̴�.
	// �̹����� ����� �����ϱ�, ���� �����ġ�� ������ �ʿ��� ���̴�.

	Size GetBmpSize() const;





	// �ִϸ��̼��� Coord�� �־��ִ� �Լ��� �ϳ� �� �����. 
	// �ֿ� ���. ���� ��ǥ left top�� �־��ְ�,  ������ ������, ������ �־��ָ� ��Ʋ�� �̹����� ��ǥ�� �ν��Ͽ� �߰��ϴ� �Լ�.
	// const vector<ClipCoord>�� �޴� �Լ��� ���� ȣȯ. 
	// Coord�Լ��� ����Ѵٴ� �ǹ̴� �̹� SetClipTexture�� ���ؼ� ���ҽ��� �޾ƿԴٴ� ���� �ǹ��Ѵ�.
	// ��Ʋ���̹����� �޾ƿ� ���¿��� ȣ���ϴ� �Լ�.
	bool AddClipCoord(const string& strName, const Position& tSrcStartPos, int iFrameSizeX, int iFrameSizeY, 
		int iCount, bool bRight = true, bool bHorizon = true);







	// �ڽ��� Ŭ���� ������ �ִ� ��� �ؽ�ó�� ���Ͽ� ����Ƚ���� ������Ű�� ó��.
	void AddRefAllClipTexture();


	void SetDefaultClip(const string& strName);	// DefaultClip�� Key���� �����ϴ� �Լ��ε� return option���� �̿��ϴ� ���̴�
	void SetCurrentClip(const string& strName); // strName �� �ش��ϴ� �ִϸ��̼��� ������ Ŭ������ �����ϴ� �Լ��̴�.
	void ChangeClip(const string& strName); // strName�� ���� Ŭ������ �ִϸ��̼��� �����ϴ� �Լ��̴�.

	PAnimationClip FindClip(const string& strName);
	//void ClearOptionTime(); // ���࿡ ������ ���� ������ ���� �� �̰��� �ɼǰ��� �ٲ��ִ� ����̴�.
	// �ٵ� �� ���ӿ��� �� ����.

public:
	void Start();
	bool Init();
	void Update(float fTime);
	CAnimation* Clone() const;
	

public:
	void SetObj(class CObj* pObj);



};

