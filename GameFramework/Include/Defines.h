#pragma once

#if defined(_DEBUG)
#define _CRTDBG_MAP_ALLOC
#define new new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif


#define SAFE_DELETE(p) if(p) {delete p; p = nullptr;}
#define SAFE_DELETE_ARRAY(p) if(p) {delete[] p; p = nullptr;}
#define SAFE_RELEASE(p) if(p) {p->Release(); p = nullptr;}

#define DECLARE_SINGLE(Type) \
private: \
	static Type* m_pInst; \
public:	\
	static Type* GetInst() \
	{\
		if(!m_pInst) m_pInst = new Type; \
			return m_pInst; \
	}\
	static void DestroyInst()\
	{\
			SAFE_DELETE(m_pInst); \
	}\
private: \
	Type();\
	~Type();

#define GET_SINGLE(Type) Type::GetInst()
#define DESTROY_SINGLE(Type) Type::DestroyInst()
#define DEFINITION_SINGLE(Type) Type* Type::m_pInst = nullptr

#define RS_WIDTH 1280
#define RS_HEIGHT 720

#define PI 3.141592f

#define WINDOWHANDLE		CGameManager::GetInst()->GetWindowHandle()
#define WINDOWINSTANCE		CGameManager::GetInst()->GetWindowInstance()
#define WINDOWDC			CGameManager::GetInst()->GetWindowDC()
#define WINDOWRESOLUTION	CGameManager::GetInst()->GetResolution()
#define REDBRUSH			CGameManager::GetInst()->GetRedBrush()
#define GREENBRUSH			CGameManager::GetInst()->GetGreenBrush()
#define YELLOWBRUSH			CGameManager::GetInst()->GetYellowBrush()
#define WHITEBRUSH			CGameManager::GetInst()->GetWhiteBrush()
#define MAGENTABRUSH		CGameManager::GetInst()->GetMagentaBrush()
#define BLACKBRUSH			CGameManager::GetInst()->GetBlackBrush()
#define REDPEN				CGameManager::GetInst()->GetRedPen()
#define GREENPEN			CGameManager::GetInst()->GetGreenPen()
#define PURPLEPEN			CGameManager::GetInst()->GetPurplePen()
#define WHITEPEN			CGameManager::GetInst()->GetWhitePen()

#define ROOT_PATH		"RootPath"
#define TEXTURE_PATH	"TexturePath"
#define SOUND_PATH		"SoundPath"

#define DECLARE_OBJECT friend class CObj;

#define KEYDOWN(Key)	CInput::GetInst()->NormalKeyDown(Key)
#define KEYPRESS(Key)	CInput::GetInst()->NormalKeyPress(Key)
#define KEYUP(Key)		CInput::GetInst()->NormalKeyUp(Key)



#define ATTACK_RIGHT "KirbyAttackEffectRight"
#define ATTACK_LEFT "KirbyAttackEffectLeft"
#define BREATH_RIGHT "KirbyBreathAttackEffectRight"
#define BREATH_LEFT "KirbyBreathAttackEffectLeft"
#define RUN_LEFT "KirbyRunEffectLeft"
#define RUN_RIGHT "KirbyRunEffectRight"
#define STAR_LEFT "KirbyStarEffectLeft"
#define STAR_RIGHT "KirbyStarEffectRight"
#define PINKSTONE "PinkStoneEffect"
#define SWORD_BULLET_LEFT "SwordBulletLeft"
#define SWORD_BULLET_RIGHT "SwordBulletRight"
#define SWORD_BULLET_ANI_LEFT "SwordBulletAniLeft"
#define SWORD_BULLET_ANI_RIGHT "SwordBulletAniRight"
#define SMALL_STAR "SmallStar"
#define BLUE_BALL "BlueBall"
#define BLADE_BULLET_LEFT "BladeBulletLeft"
#define BLADE_BULLET_RIGHT "BladeBulletRight"
#define MIKE_BULLET "MikeBullet"
#define MOVE_STAR_RIGHT "MoveStarRight"
#define MOVE_STAR_LEFT "MoveStarLeft"
#define JUMP_STAR_RIGHT "JumpStarRight"
#define JUMP_STAR_LEFT "JumpStarLeft"
#define DOOR "Door"
#define STAR "Star"


template <typename T>
void Safe_Delete_VecList(T& container)
{
	typename T::iterator iter;
	typename T::iterator iterEnd = container.end();

	for (iter = container.begin(); iter != iterEnd; ++iter)
	{
		SAFE_DELETE(*iter);
	}

	container.clear();
}


template <typename T>
void Safe_Delete_Array_VecList(T& container)
{
	typename T::iterator iter;
	typename T::iterator iterEnd = container.end();

	for (iter = container.begin(); iter != iterEnd; ++iter)
	{
		SAFE_DELETE_ARRAY(*iter);
	}

	container.clear();
}


template <typename T>
void Safe_Delete_Map(T& container)
{
	typename T::iterator iter;
	typename T::iterator iterEnd = container.end();

	for (iter = container.begin(); iter != iterEnd; ++iter)
	{
		SAFE_DELETE(iter->second);
	}

	container.clear();
}


template <typename T>
void Safe_Delete_Array_Map(T& container)
{
	typename T::iterator iter;
	typename T::iterator iterEnd = container.end();

	for (iter = container.begin(); iter != iterEnd; ++iter)
	{
		SAFE_DELETE_ARRAY(iter->second);
	}

	container.clear();
}

template <typename T>
void Safe_Release_Map(T& container)
{
	typename T::iterator iter;
	typename T::iterator iterEnd = container.end();

	for (iter = container.begin(); iter != iterEnd; ++iter)
	{
		SAFE_RELEASE(iter->second);
	}

	container.clear();
}


template <typename T>
void Safe_Release_VecList(T& container)
{
	typename T::iterator iter;
	typename T::iterator iterEnd = container.end();

	for (iter = container.begin(); iter != iterEnd; ++iter)
	{
		SAFE_RELEASE((*iter));
	}

	container.clear();
}