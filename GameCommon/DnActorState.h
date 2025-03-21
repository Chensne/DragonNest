#pragma once

#include "DnState.h"


class CDnActor;
class CDnActorState : public CDnState
{
public:
	CDnActorState();
	virtual ~CDnActorState();

	static float s_fMaxStiffTime;
	static float s_fCriticalDamageProb;

	enum ActorStateEnum {
		None = 0,
		Stay = 0x0001,
		Move = 0x0002,
		Attack = 0x0004,
		Hit = 0x0008,
		Air = 0x0010,
		Down = 0x0020,
		Stun = 0x0040,
		Stiff = 0x0080,
		IgnoreCantAction = 0x0100,
		IgnoreBackMoveSpeed = 0x0200, // #31110 탑스피닝 처럼 Move State 인데 뒤로 이동하는 경우 이동속도 절반으로 처리하지 않도록 함.

		ActorStateEnum_Amount = 11,
	};

	enum ActorCustomStateEnum {
		Custom_None = 0,
		Custom_Ground = 0x0001,
		Custom_Fly = 0x0002,
		Custom_UnderGround = 0x0004,
		Custom_LifeSkill = 0x0008,
		Custom_Fly2 = 0x0010,
		ActorCustomStateEnum_Amount = 5,
	};


	// 상태효과로부터 발동된 액터의 특수 상태(지속시간이 있는 버프/디버프)
	enum ActorStateEffectEnum
	{
		StateEffect_None = 0,
		Cant_Move = 0x00000001,
		Cant_Use_Skill = 0x00000002,
		Cant_Use_Item = 0x00000004,
		Cant_AttackAction = 0x00000008,
		
		ActorStateEffectEnum_Amount = 5,
	};

	enum ActorMaterialEnum {
		Flesh,
		Metal,
		Solid,
		Wood,
		ActorMaterialEnum_Amount = 4,
	};

	enum ActorTypeEnum {
		Warrior =  0,
		Archer,
		Soceress,
		Cleric,
#if defined(PRE_ADD_ACADEMIC)
		Academic,
#else
		Reserved1,
#endif	// #if defined(PRE_ADD_ACADEMIC)
#ifdef PRE_ADD_KALI
		Kali,
#else
		Reserved2,
#endif // #ifdef PRE_ADD_KALI
#ifdef PRE_ADD_ASSASSIN
		Assassin,
#else
		Reserved3,
#endif	// #ifdef PRE_ADD_ASSASSIN
		Lencea,
		Machina,
		Reserved6,
		SimpleRush,
		Normal,
		Defense,
		Disturbance,
		Range,
		Rush,
		Giant,
		Magic,
		Trap,
		Cannon,				// 플레이어가 쏘는 대포 몹의 액터 타입.
		NoAggro,
		NoAggroTrap,
		Crocodile = 23,
		// 여기부터 보스
		NESTBOSS = 69,
		Basilisk = 70,
		Beholder,

		// 드래곤
		BlackDragon, 
		GoldDragon,
		Manticore,
		Cerberos,
		SeaDragon,
		LotusGolem,
		GreenDragon,
		EndOfMonster,
//
		Vehicle = 80, // Rotha 탈것 시스템
		Pet,

		// 여기부터 npc
		Npc = 100,
		
		// 여기서부터 Prop용 액터
		PropActor = 150,
		ActorTypeEnum_Amount,

	};

	enum RefreshStateExtentEnum {
		RefreshBase = 0x01,
		RefreshEquip = 0x02,
		RefreshSkill = 0x04,
		RefreshAll = 0xFF,
	};

	enum StateTypeEnum {
		ST_Strength				= 0x00000001,
		ST_Agility				= 0x00000002,
		ST_Intelligence			= 0x00000004,
		ST_Stamina				= 0x00000008,
		ST_HP					= 0x00000010,
		ST_SP					= 0x00000020,
		ST_AttackP				= 0x00000040,
		ST_AttackM				= 0x00000080,
		ST_ElementAttack		= 0x00000100,
		ST_DefenseP				= 0x00000200,
		ST_DefenseM				= 0x00000400,
		ST_ElementDefense		= 0x00000800,
		ST_MoveSpeed			= 0x00001000,
		ST_DownDelay			= 0x00002000,
		ST_Stiff				= 0x00004000,
		ST_StiffResistance		= 0x00008000,
		ST_Critical				= 0x00010000,
		ST_CriticalResistance	= 0x00020000,
		ST_Stun					= 0x00040000,
		ST_StunResistance		= 0x00080000,
		ST_SuperAmmor			= 0x00100000,
		ST_RecoverySP			= 0x00200000,
		ST_FinalDamage			= 0x00400000,
		ST_Spirit				= 0x00800000,
		ST_SafeZoneMoveSpeed	= 0x01000000,
		ST_AddExp				= 0x02000000,
		ST_All					= 0xFFFFFFFF,
	};

	enum ActorPressEnum {
		Press_Circle,	// 기본타입
		Press_Capsule,	// 켈베로스
		//Press_Ellipse,	// 현재 지원 안함.
		Press_NoneSameCircle,	// 다른 타입일 때만 밀어내기
		ActorPressEnum_Amount = 3,
	};

	// AddBlowState 함수에서 사용하는 타입. 정해주는 타입에 따라 리스트에 나눠서 들어간다.
	enum AddBlowStateType
	{
		Equip_Buff_Level,	 // State[0] : 장비와 버프가 계산되는 레벨
		Equip_Skill_Level,   // State[0] : 장비와 버프를 계산하고 나서 특수한 상태효과를 계산하는 레벨
		Skill_Level,		 // state[1] : 마지막으로 스킬을 계산하는 레벨
	};

	static int s_nActorStateIndex[ActorStateEnum_Amount];
	static char *s_szActorStateString[ActorStateEnum_Amount];
	static ActorStateEnum String2ActorStateEnum( const char *szStr );

	static int s_nActorCustomStateIndex[ActorCustomStateEnum_Amount];
	static char *s_szActorCustomStateString[ActorCustomStateEnum_Amount];
	static ActorStateEnum String2ActorCustomStateEnum( const char *szStr );

	CDnActor *m_pActor;
protected:
	tstring m_szName;

#ifdef PRE_ADD_BESTFRIEND
	INT64 m_BFserial;
	tstring m_szBestfriendName;
#endif
#if defined(PRE_ADD_MULTILANGUAGE)
	int m_nNameUiStringIndex;
#endif		//#if defined(PRE_ADD_MULTILANGUAGE)

	CDnState m_BaseState;
	CDnState m_StateStep[2];

	// 고정 상태
	int m_nActorTableID;
	MSDT_DECL(int) m_nLevel;		// Level
	ActorTypeEnum m_ActorType;	// 액터 타입
	ActorMaterialEnum m_Material;// 액터 제질
	MSDT_DECL(float) m_fWeight;	// 무게
	MSDT_DECL(int) m_nHeight;	// 키
	MSDT_DECL(int) m_nUnitSize;	// 유닛 사이즈
	ActorPressEnum m_Press;
	int m_nUnitSizeParam1;	// 캡슐이라면, xz평면에서의 가로반지름
	int m_nUnitSizeParam2;	// 캡슐이라면, xz평면에서의 세로반지름

#ifndef _GAMESERVER
#define RENDER_PRESS_hayannal2009
#ifdef RENDER_PRESS_hayannal2009
public:
	// 렌더용버퍼. 원의 경우엔 1만 사용하고, 캡슐의 경우엔 다 사용한다.
	SPrimitiveDraw3D m_PressVertices1[10];
	SPrimitiveDraw3D m_PressVertices2[10];
	SPrimitiveDraw3D m_PressVertices3[4];
protected:
#endif
#endif

	MSDT_DECL(float) m_fRotateAngleSpeed; // 회전속도
	MSDT_DECL(int) m_nPressLevel;		// 밀어내기 레벨

	MSDT_DECL(INT64) m_nHP;		// HP	직접 수정하지 말고 SetHP() 를 사용하세요
	MSDT_DECL(int) m_nSP;		// SP	직접 수정하지 말고 SetSP() 를 사용하세요

	ActorStateEnum m_State;		// 액터 상태
	ActorCustomStateEnum	m_CustomState;

	ActorStateEffectEnum m_StateEffect;				// 액터 상태효과로 인한 현 상태.

	MSDT_DECL(bool) m_bHittable;
	MSDT_DECL(bool) m_bMovable;
	MSDT_DECL(bool) m_bRotatable;

	typedef std::list<CDnState*>		BLOW_STATE_LIST;
	typedef BLOW_STATE_LIST::iterator	BLOW_STATE_LIST_ITER;
	BLOW_STATE_LIST m_listBlowState;
	BLOW_STATE_LIST m_listBeforePostBlowState;
	BLOW_STATE_LIST m_listPostBlowState;

#ifdef PRE_ADD_SKILLBUF_RENEW
	bool m_bCopiedFromSummoner;
	BLOW_STATE_LIST m_listBuffBlowState;
#endif // #ifdef PRE_ADD_SKILLBUF_RENEW

	enum StateListTypeEnum {
		BaseList = 0x0001,
		WeaponList = 0x0002,
		DefenseList = 0x0004,
		AccessoryList = 0x0008,
		StateEffectList = 0x0010,
		PostStateEffectList = 0x0020,
		SetItemList = 0x0040,
		AppellationList = 0x0080,
		GlyphList = 0x0100,
		// #32220 아이템에서 사용할 몇가지 특수한 상태효과인 경우 post state effect 계산 되기 전에 먼저 계산된다.
		// 현재 물리 공격력, 마법 공격력에서만 사용된다.
		BeforePostStateEffectList = 0x0200,
		PetAbility = 0x0400,
		TalismanList = 0x0800,

		AllList = 0xFFFF,
	};

public:
	void AddBlowState( CDnState *pState, AddBlowStateType eAddBlowStateType = Equip_Buff_Level , bool bIsBuff = false );
	void DelBlowState( CDnState *pState );

protected:
	virtual void OnLevelUp( int nNewLevel, int iLevelUpAmount ) {}
	virtual void OnAddExperience( int nAddExperience, int nLogCode, INT64 biFKey ) {}	// -> nLogCode에 따라 biFKey값도 넣어준다 (eCharacterExpChangeCode참조)
	virtual void OnRefreshState() {}

	virtual void CalcBaseState( StateTypeEnum Type = ST_All ) {}
	virtual void CalcState( RefreshStateExtentEnum Extent, StateTypeEnum Type ) {}
	virtual void GetStateList( int ListType, CDnState::ValueType Type, std::vector<CDnState *> &VecList );

#if defined (PRE_ADD_BESTFRIEND)
	bool IsApplyState(CDnActor *pActor, CDnWeapon* pWeapon);
#endif

#ifdef PRE_ADD_SKILLBUF_RENEW
	virtual void GetBuffStateList( CDnState::ValueType Type, std::vector<CDnState *> &VecList );
#endif // #ifdef PRE_ADD_SKILLBUF_RENEW

	virtual void RefreshAdditionalState( RefreshStateExtentEnum &Extent, StateTypeEnum &Type ) {}

	void Initialize( CDnActor *pActor ) { m_pActor = pActor; }

	virtual void OnCalcEquipStep( StateTypeEnum Type, CDnState *pState ) {}

public:
	virtual void Initialize( int nClassID );
	void RefreshState( RefreshStateExtentEnum Extent = RefreshAll, StateTypeEnum Type = ST_All );
	virtual void ProcessState( LOCAL_TIME LocalTime, float fDelta ) {}

	void SetName( const TCHAR *szName ) { m_szName = szName; }
	TCHAR *GetName() const { return (TCHAR*)m_szName.c_str(); }
#ifdef PRE_ADD_BESTFRIEND
	void SetBFserial( INT64 serial ) { m_BFserial = serial; }
	INT64 GetBFserial() { return m_BFserial; }
	void SetBestfriendName( const TCHAR * szBFname ) { m_szBestfriendName.assign( szBFname ); }
	TCHAR *GetBestfriendName() const { return (TCHAR*)m_szBestfriendName.c_str(); }
#endif

#if defined(PRE_ADD_MULTILANGUAGE)
	void SetNameUIStringIndex(int nNameUiStringIndex) { m_nNameUiStringIndex = nNameUiStringIndex; }
	int GetNameUIStringIndex() { return m_nNameUiStringIndex; }
#endif		//#if defined(PRE_ADD_MULTILANGUAGE)

	void SetLevel( int nValue );
	__forceinline int GetLevel() { return m_nLevel; }
	virtual int GetCalcLevel() { return m_nLevel; }

	__forceinline INT64 GetHP() { return m_nHP; }
	__forceinline int GetHPPercent() 
	{ 
		float fMaxHP = static_cast<float>(GetMaxHP());
		float fCurHP = static_cast<float>(GetHP());

		if( fMaxHP == 0.f )
			return 0;

		return static_cast<int>((fCurHP/fMaxHP)*100.0f);
	}

	__forceinline float GetHPPercentFloat()
	{ 
		float fMaxHP = static_cast<float>(GetMaxHP());
		float fCurHP = static_cast<float>(GetHP());

		if( fMaxHP == 0.f )
			return 0.f;

		return static_cast<float>(fCurHP/fMaxHP);
	}

#if defined(PRE_ADD_MISSION_COUPON) && (_GAMESERVER)
	void SetHP( INT64 nValue );
#else
	__forceinline void SetHP( INT64 nValue ) 
	{
		//---------------------------------------
		//[_debug]  솰柬HP뵨SP돨긴뺏

		if (nValue > 0)
		{
		   m_nHP = nValue;
		}
		//---------------------------------------


		m_nHP = nValue;
		m_nHP = (m_nHP < 0) ? 0 : m_nHP;
	}	
#endif
		
	__forceinline int GetSP() { return m_nSP; }
	__forceinline int GetSPPercent() 
	{ 
		float fMaxSP = static_cast<float>(GetMaxSP());
		float fCurSP = static_cast<float>(GetSP());

		if( fMaxSP == 0.f )
			return 0;

		return static_cast<int>((fCurSP/fMaxSP)*100.0f);
	}

#if defined(PRE_ADD_MISSION_COUPON) && (_GAMESERVER)
	void SetSP( int nValue );
#else
	
	__forceinline void SetSP( int nValue ) 
	{ 
		//--------------------------------------
		//[_debug] 댔丹SP딧桿斤口
		//WCHAR wszBuf[MAX_PATH];
		//wsprintf( wszBuf, L"SetSP SP:%d", nValue );
		//CDnChatTabDlg::AddChatMessage( CHATTYPE_NORMAL, L"", wszBuf  );
		//GetInterface().AddChatMessage( CHATTYPE_NORMAL, L"", wszBuf  );
		//CDnInterface::GetInstance().AddChatMessage(CHATTYPE_NORMAL, L"", wszBuf  );   //CHATTYPE_SYSTEM

		//--------------------------------------

		//---------------------------------------
		//[_debug]  솰柬HP뵨SP돨긴뺏
		if (nValue <= 0)
		{
			m_nSP = nValue;
		}

        if (nValue > 0)
        {
			m_nSP = nValue;
        }
		//---------------------------------------

		m_nSP = nValue; 
	}

#endif

	virtual float GetWeight() { return m_fWeight; }
	virtual void SetWeight( float fWeight ) { m_fWeight = fWeight; };
	__forceinline int GetHeight() { return m_nHeight; }
	__forceinline void SetHeight(int height) { m_nHeight = height; }

	__inline ActorPressEnum GetPress() { return m_Press; }
	__inline void SetPress(ActorPressEnum _pressType) { m_Press = _pressType; }
	__inline int GetPressLevel() { return m_nPressLevel; }
	__inline void SetPressLevel( int nPressLevel ) { m_nPressLevel = nPressLevel; };		// 소환 몬스터에게 능력치 복사할 때 따로 백업했다가 복구해줄 때만 쓰임.

	__forceinline virtual void SetState( ActorStateEnum State ) { m_State = State; SetStateChangeTime(); }
	__forceinline int GetState() { return m_State; }

	__forceinline void SetCustomState( ActorCustomStateEnum State ) { m_CustomState = State; }
	__forceinline int GetCustomState() { return m_CustomState; }

	__forceinline void SetStateEffect( int State ) { m_StateEffect = (ActorStateEffectEnum)State; }
	__forceinline int GetStateEffect() { return m_StateEffect; }

	__forceinline ActorMaterialEnum GetMaterial() { return m_Material; }
	__forceinline void SetMaterial(ActorMaterialEnum _material) { m_Material = _material; }

	__forceinline ActorTypeEnum GetActorType() { return m_ActorType; }
	__forceinline void SetActorType( ActorTypeEnum Type ) { m_ActorType = Type; }

	__forceinline void SetHittable( bool bFlag ) { m_bHittable = bFlag; }
	__forceinline bool IsHittable() { return m_bHittable; }

	__forceinline void SetMovable( bool bFlag ) { m_bMovable = bFlag; }
	__forceinline bool IsMovable() 
	{ 
		if( (m_StateEffect & Cant_Move) )
			return false;
		else
			return m_bMovable;
	}

	__forceinline void SetRotatable( bool bFlag ) { m_bRotatable = bFlag; }
	__forceinline bool IsRotatable() { return m_bRotatable; }

	__forceinline int GetUnitSize() { return m_nUnitSize; }
	__forceinline int GetUnitSizeParam1() { return m_nUnitSizeParam1; }
	__forceinline int GetUnitSizeParam2() { return m_nUnitSizeParam2; }
	
	__forceinline void SetUnitSize(int nSize) { m_nUnitSize = nSize; }
	__forceinline void SetUnitSizeParam1(int nSize) { m_nUnitSizeParam1 = nSize; }
	__forceinline void SetUnitSizeParam2(int nSize) { m_nUnitSizeParam2 = nSize; }

	virtual void SetRotateAngleSpeed( float fRotateAngleSpeed ) { m_fRotateAngleSpeed = fRotateAngleSpeed; };
	virtual float GetRotateAngleSpeed() { return m_fRotateAngleSpeed; }

	virtual int GetMoveSpeed() { return CDnState::GetMoveSpeed(); }


	float GetLevelWeightValue();
	
	bool IsNeedPvPLevelWeight();
	float GetDefenseConstant();
	float GetCriticalConstant();
	float GetFinalDamageConstant();

	virtual bool IsDie();

	const CDnState *GetBaseState() { return &m_BaseState; }
	CDnState *GetStateStep( int nStep ) { return &m_StateStep[nStep]; }

	void CopyStateFromThis( DnActorHandle hActor, bool bDontCopySkillStateEffect = false );

	int GetActorTableID() const { return m_nActorTableID; }
	void SetActorTableID(int nTableID) { m_nActorTableID = nTableID; }
	int GetAttackPMaxWithoutSkill() { return m_StateStep[0].GetAttackPMax(); }
	int GetAttackMMaxWithoutSkill() { return m_StateStep[0].GetAttackMMax(); }
	int GetAttackMMinWithoutSkill() { return m_StateStep[0].GetAttackMMin(); }


protected:
	LOCAL_TIME m_StateChangeTime;
public:
	void SetStateChangeTime();// { if (m_pActor) m_StateChangeTime = m_pActor->GetLocalTime(); }
	LOCAL_TIME GetStateChangeTime() { return m_StateChangeTime;}

#if defined(PRE_ADD_TOTAL_LEVEL_SKILL)
	virtual void OnLevelChange() {};
#endif // PRE_ADD_TOTAL_LEVEL_SKILL
};

#define CALC_STATE_VALUE_ABSOLUTE( itemvector, value, func ) \
	value = 0; \
	for( DWORD ADD_STATE_VALUE_I = 0; ADD_STATE_VALUE_I<itemvector.size(); ADD_STATE_VALUE_I++ ) { \
	value += itemvector[ADD_STATE_VALUE_I]->func;	\
	}

#define CALC_STATE_VALUE_RATIO( itemvector, standardvalue, value, func ) { \
	float FLOAT_STATE_RATIO = 0.f; \
	value = 0; \
	for( DWORD ADD_STATE_VALUE_I = 0; ADD_STATE_VALUE_I<itemvector.size(); ADD_STATE_VALUE_I++ ) { \
	FLOAT_STATE_RATIO += itemvector[ADD_STATE_VALUE_I]->func; \
	} \
	value = ( standardvalue * FLOAT_STATE_RATIO ); \
}

#define MODIFY_STATE_VALUE_LIMIT_RATIO_INTEGER( itemvector, standardvalue, value, func , minlimit, maxlimit ) { \
	float FLOAT_STATE_RATIO = 0.f; \
	for( DWORD ADD_STATE_VALUE_I = 0; ADD_STATE_VALUE_I<itemvector.size(); ADD_STATE_VALUE_I++ ) { \
	FLOAT_STATE_RATIO += itemvector[ADD_STATE_VALUE_I]->func; \
	} \
	if(FLOAT_STATE_RATIO < minlimit) { \
	value += (int)( -(standardvalue * FLOAT_STATE_RATIO) + (standardvalue * minlimit) ); \
	} \
	if(FLOAT_STATE_RATIO > maxlimit ) { \
	value += (int)( -(standardvalue * FLOAT_STATE_RATIO) + (standardvalue * maxlimit) ); \
	} \
}

#define MODIFY_STATE_VALUE_LIMIT_ABSOLUTE( itemvector, value, func , minlimit, maxlimit ) { \
	float FLOAT_STATE_VALUE = 0.f; \
	for( DWORD ADD_STATE_VALUE_I = 0; ADD_STATE_VALUE_I<itemvector.size(); ADD_STATE_VALUE_I++ ) { \
	FLOAT_STATE_VALUE += itemvector[ADD_STATE_VALUE_I]->func; \
	} \
	if( FLOAT_STATE_VALUE < minlimit ) { \
	value += ( -FLOAT_STATE_VALUE + minlimit ); \
	} \
	if( FLOAT_STATE_VALUE > maxlimit ) { \
	value += ( -FLOAT_STATE_VALUE + maxlimit ); \
	} \
}

#define GATHER_RATIO( itemvector, value, func ) \
	value = 0.f; \
	for( DWORD ADD_STATE_VALUE_I = 0; ADD_STATE_VALUE_I<itemvector.size(); ADD_STATE_VALUE_I++ ) { \
	value += itemvector[ADD_STATE_VALUE_I]->func; \
	} \

template < class T, class T2 >
void CALC_STATE_LIMIT( T &Value, T2 Min, T2 Max )
{
	if( Value < Min ) Value = Min;
	else if( Value > Max ) Value = Max;
}
