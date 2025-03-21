#pragma once

//#include "DnActor.h"
#include "DnWeapon.h"
#include "SmartPtr.h"
#include "DnBlowDef.h"
#include "DnObservable.h"


class IDnSkillUsableChecker;
class IDnSkillProcessor;
class CDnProjectile;

const int MAX_SKILL_NEED_EQUIP_COUNT = 2;

const int TIME_ACCELERATION_SKILL_ID = 2415;
const int SPRIT_BOOST_SKILL_ID = 1412;


// 스킬 객체
class CDnSkill : public CMultiSmartPtrBase< CDnSkill, MAX_SESSION_COUNT >, public TBoostMemoryPool< CDnSkill >,
				 public CDnObservable
{
public:
	CDnSkill( DnActorHandle hActor );
	virtual ~CDnSkill(void);

	enum SkillTypeEnum 
	{
		Active,
		Passive,
		AutoPassive,
		EnchantPassive,					// 2차 전직 스킬부터 들어가는 기존 보유 스킬 강화 스킬 타입.
		AutoActive,						// 쿨타임마다 자동으로 사용되는 스킬
	};

	enum DurationTypeEnum 
	{
		Instantly,
		Buff,
		Debuff,
		TimeToggle,
		ActiveToggle,
		Aura,
		ActiveToggleForSummon,		//소환 몬스터에 사용될 토글 스킬
		SummonOnOff,					//소환 몬스터 소환 On/Off용 스킬(알프레도 소환, 소환해제)
		StanceChange,              // 패링스탠스 , 해킹스탠스 , 시즈스탠스 , 데몰리션 등의 스킬 타입
	};

	enum TargetTypeEnum 
	{
		Self,
		Enemy,
		Friend,
		Party,
		All,
	};

	enum StateEffectApplyType 
	{
		ApplySelf,
		ApplyTarget,
		ApplyAll,

		// 이하 적용 대상들은 TargetType 이 All 인 경우에만 유효함.
		ApplyEnemy,
		ApplyFriend,
	};

	// 스킬 사용 실패 이유는 필요한대로 추가.
	enum UsingResult
	{
		Failed = 0,
		Success,
		FailedByCooltime,
		FailedByUsableChecker,
		FailedByInvailedAction,
		NoExistSkill,
		Hack,
	};

	// 현재 pvp/pve 인지 구분. 레벨 테이블과 동일한 값. PVE:0, PVP:1
	enum Mode
	{
		PVE = 0,
		PVP,
		NUM_SKILLLEVEL_APPLY_TYPE,
	};

	// #31013 테이블 레코드 하나 당 상태효과 정의 슬롯 최대 갯수.
	enum MAX_DATA_COUNT
	{	
		MAX_PROCESSOR_COUNT = 5,
		MAX_STATE_EFFECT_COUNT = 7,
		MAX_GLYPH_STATE_EFFECT_COUNT = 5,
	};

	// 상태 이상 발동 관련 데이터
	struct StateEffectStruct
	{
		int nID;
		StateEffectApplyType ApplyType;
		int nDurationTime;
		string szValue;
		int nGlyphID;
		int nFromEnchantPassiveSkillID;
		
		// CDnApplySEWhenActionSetBlowEnabledProcessor 를 갖고 있는 대상에게만 유효함.. 나머진 경우엔 필요없는 플래그
		bool bApplyInProcessor;

		// #40643 핑퐁밤EX 처럼 특수하게 구현된 상태효과의 인자를 건드리는 경우..
		// 어쩔 수 없이 합쳐줘야 한다.
		// 이 플래그가 true 이면 이미 기존에 있는 상태효과에 합쳐진 상태이기 때문에 실제 상태효과 적용할 땐 적용하지 않는다.
		string strEnchantSkillSEParam;

		// ApplyAll 로 쌍으로 들어간 상태효과임. 바로 다음 상태효과와 짝을 이룬다는 의미.
		bool bApplyAllPair;

		// #52905 중첩 추가 효과 여부 
		bool bAddtionalStateInfo;

		StateEffectStruct( void ) : nID( 0 ), ApplyType( ApplySelf ), nDurationTime( 0 ), nGlyphID( 0 ), 
									bApplyInProcessor( false ), nFromEnchantPassiveSkillID( 0 ), bApplyAllPair( false ), bAddtionalStateInfo(false) {}
	};

	enum GLYPH_EFFECT {
		NONE,
		SKILLMP_PRO,
		SKILLDELAY_PRO,
		SKILLDELAY_RESET_PRO,
		SKILLDURATION_PRO,
		SKILLATTACKHEAL_ABSOLUTE,
	};

	struct GlyphEffectStruct
	{
		int		nGlyphID;
		int		eEffectIndex;
		float	fEffectValue;
	};

	struct SkillInfo
	{
		int iSkillID;
		int iSkillLevelID;

#ifdef PRE_FIX_SYNC_ENCHANT_SKILL
		int iAppliedEnchantSkillID;
#endif

		int iLevel;
		int iSkillDuplicateMethod;
		int iDuplicateCount;

		int iDissolvable;
		
		SkillTypeEnum eSkillType;
		DurationTypeEnum eDurationType;
		TargetTypeEnum eTargetType;
		StateEffectApplyType eApplyType;
		CDnState::ElementEnum eSkillElement;
		DnActorHandle hSkillUser;
		tstring strUserName;
		bool bIsItemSkill;

		int nItemID;

		// 60번 상태효과. 체인 스킬에서만 쓰임. 프로젝타일을 쏘는 스킬에서만 유효한 값이 들어있다.
		int iProjectileShootActionIndex;
		int iProjectileSignalArrayIndex;
		DnActorHandle hPrevAttacker;
		int iLeaveCount;			// 남은 체인 공격 카운트. 0 이 되면 더 이상 전파되지 않는다.
		//

		// 파이어/아이스실드 처럼 서버에서 밀어주는 이펙트 테이블 ID 로 이펙트를 붙일 때 셋팅합니다. 디폴트는 -1
		string szEffectOutputIDToClient;

		bool bFromInputHasPassiveSignal;
		bool bFromBuffProp;

		string szEffectOutputIDs;

		int iSkillUserTeam;			// 이 스킬을 사용한 액터의 팀 인덱스.

		// 아이템 접두어 시스템에서 사용하는 스킬. 서버에서는 값이 직접 셋팅되며 클라이언트로는 SC_CMDADDSTATEEFFECT 패킷으로 셋팅된다.
		// 이 플래그가 켜져 있는 접두어 스킬의 상태효과들은 Duplicate 처리를 하면 안됨. 패킷받자마자 곧바로 begin 됨.
		bool bItemPrefixSkill;

		// [2011/04/14 semozz]
		// 길드 스킬 성문에 면역을 무시 하고 적용 하기 위해 추가(트리거에 의해서만 값을 설정..)
		bool bIgnoreImmune;
		bool bIsGuildSkill;

#if defined(PRE_ADD_PREFIX_SYSTE_RENEW)
		int nPrefixSkillType;		//접미사 스킬 그룹ID
#endif // PRE_ADD_PREFIX_SYSTE_RENEW

		LOCAL_TIME projectileSkillStartTime;

		SkillInfo( void ) : iSkillID( 0 ), iSkillLevelID( 0 ),
#ifdef PRE_FIX_SYNC_ENCHANT_SKILL
							iAppliedEnchantSkillID( 0 ),
#endif
							iLevel( 0 ), iSkillDuplicateMethod( 0 ), iDuplicateCount( 0 ),
							eDurationType( Instantly ),
							eTargetType( TargetTypeEnum::Enemy ),
							iDissolvable( 0 ),
							eSkillType( Active ),
							eApplyType(ApplySelf),
							eSkillElement( CDnState::ElementEnum_Amount ),
							bIsItemSkill( false ),
							nItemID(-1),
							iProjectileShootActionIndex( -1 ),
							iProjectileSignalArrayIndex( 0 ),
							iLeaveCount( -1 ),
							bFromInputHasPassiveSignal( false ),
							bFromBuffProp( false ),
							iSkillUserTeam( -1 ),
							bItemPrefixSkill( false ),
							bIgnoreImmune(false)
							, bIsGuildSkill(false)
#if defined(PRE_ADD_PREFIX_SYSTE_RENEW)
							, nPrefixSkillType(-1)
#endif // PRE_ADD_PREFIX_SYSTE_RENEW
							, projectileSkillStartTime(0)
							{}
	};

	enum CanApply
	{
		Fail,
		Apply,
		ApplyDuplicateSameSkill,
	};

protected:

	DnActorHandle m_hActor;

	SkillTypeEnum				m_eSkillType;

	// 현재 pvp/pve 데이터인지
	int m_iSelectedSkillLevelDataApplyType;

	int							m_iSkillID;
	int							m_iSkillLevelID;
	int							m_iNowLevelSkillPoint;
	int							m_iNextLevelSkillPoint;
	tstring						m_strName;
	string						m_strStaticName;
	tstring						m_strDescription;

	DurationTypeEnum			m_eDurationType;
	TargetTypeEnum				m_eTargetType;

	int							m_iNeedJobClassID;
	CDnWeapon::EquipTypeEnum	m_aeNeedEquipType[ MAX_SKILL_NEED_EQUIP_COUNT ];
	int							m_iNeedItemID[ NUM_SKILLLEVEL_APPLY_TYPE ];
	int							m_iNeedItemDecreaseCount[ NUM_SKILLLEVEL_APPLY_TYPE ];

	int							m_iDissolvable;					// 해제 가능 여부. 0: 해제가능, 1: 해제 불가
	int							m_iDuplicateCount;				// 스킬 중복 횟수 
	int							m_iSkillDuplicateMethod;		// 지속효과 구분
	int							m_iEffectDuplicateMethod;		// 이펙트 출력 구분

	float						m_fHPConsumeType[ NUM_SKILLLEVEL_APPLY_TYPE ];
	float						m_fMPConsumeType[ NUM_SKILLLEVEL_APPLY_TYPE ];
	float						m_fResetCooltime;
	int							m_iNeedHP[ NUM_SKILLLEVEL_APPLY_TYPE ];
	int							m_iNeedMP[ NUM_SKILLLEVEL_APPLY_TYPE ];
	int							m_iOriginalNeedMP[ NUM_SKILLLEVEL_APPLY_TYPE ];
	int							m_iCPScore;

	int							m_iLevel;
	int							m_iMaxLevel;
	int							m_iLevelLimit;
	int							m_iIncreaseRange[ NUM_SKILLLEVEL_APPLY_TYPE ];
	int							m_iDecreaseHP[ NUM_SKILLLEVEL_APPLY_TYPE ];
	int							m_iDecreaseMP[ NUM_SKILLLEVEL_APPLY_TYPE ];
	float						m_fOriginalDelayTime[ NUM_SKILLLEVEL_APPLY_TYPE ];
	float						m_fCoolTime;


	//--------------------------------------------------------------
	//[debug_skill]
	int                         m_nMaxUseNum;
	int                         m_nNowNum;
	//--------------------------------------------------------------


	float						m_fDelayTime[ NUM_SKILLLEVEL_APPLY_TYPE ];
	float						m_fLeftDelayTime;
	int							m_iAdditionalThreat;

	int							m_iBaseSkillID;	// 패시브 강화 스킬인 경우에만 유효함. 강화 시킬 대상의 스킬 ID.
	int							m_iAppliedEnchantPassiveSkillID;
	vector<IDnSkillProcessor*> m_vlpProcessorBackup[ NUM_SKILLLEVEL_APPLY_TYPE ];

	LOCAL_TIME					m_LastTimeToggleMPDecreaseTime;
	bool						m_bFinished;

	float						m_fStartSuperArmor;
	bool						m_bStartCanHit;

	// 토글, 오라 스킬인 경우 토글 된 상태인지.
	bool						m_bToggle;
	bool						m_bAura;

	// 이펙트를 미리 읽어놓는다.
	string						m_strEffectSkinName;
	string						m_strEffectAniName;
	string						m_strEffectActionName;
	EtAniObjectHandle			m_hEffect;
	CDnActionBase*				m_pEffectAction;
	
	DNVector(StateEffectStruct)		m_vlStateEffectList[ NUM_SKILLLEVEL_APPLY_TYPE ];
	vector<int>						m_vlApplySelfNoDurationStateEffectList;			// 공격력 2 배 등의 자기 자신에게 한 번만 실행되는 durationtime 이 없는 상태효과 생성 id 모아 둠.
	vector< pair<int, int> >		m_vlStateDurationList[ NUM_SKILLLEVEL_APPLY_TYPE ];

	// 스킬 발동 조건 리스트
	vector<IDnSkillUsableChecker*>	m_vlpUsableCheckers[ NUM_SKILLLEVEL_APPLY_TYPE ];

	// 스킬 프로세스 리스트.. 상태 이상이라던지 기타 등등의 처리해야 하는 상황들의 추상화.
	vector<IDnSkillProcessor*>		m_vlpProcessors[ NUM_SKILLLEVEL_APPLY_TYPE ];

	bool							m_bItemSkill;							// 이 스킬, 아이템 스킬인가.
	
	SkillInfo						m_SkillInfo[ NUM_SKILLLEVEL_APPLY_TYPE ];

	// 오라 스킬을 위해.. 주변에 있는 플레이어 정보 업데이트. (액터의 유니크 아이디)
	LOCAL_TIME						m_SkillStartTime;
	
	// 패시브 액션 스킬을 위해 셋팅된 지속시간
	float							m_fPassiveActionSkillLength;

	CDnState::ElementEnum			m_eElement;

	// 몬스터  AI 에서 사용하기 위해서 스킬에서 액션을 사용하고 있다면 그 액션을 모아 둔다.
	set<string>					m_setUseActionNames;

	// 쿨타임 한번만 돌고나서 원래 쿨타임 값을 사용한다. 장비아이템 스킬 때문에 같은 슬롯의 직전 장비 아이템의 쿨타임을 적용받는다.
	float m_fOnceDelayTime;
	float m_fOnceElapsedDelayTime;
	bool m_bEquipItemSkill;
	int m_iEquipIndex;

	// 쿨타임 조절 상태효과의 값을 스킬 쓸 당시에 가져와서 저장해둔다. 
	float m_fCoolTimeAdjustBlowValue;

	bool m_bChainingPassiveSkill;
	
	bool m_bAppliedPassiveSelfBlows;

	// 마지막으로 스킬이 사용된 시간 (timeGetTime())
	DWORD m_dwLastUseSkillTimeStamp;

	std::vector<GlyphEffectStruct> m_vGlyphEffectList;

	int m_iExclusiveID;				// 스킬 테이블에 있는 같이 배울 수 없는 스킬들의 구분을 위한 id

	bool m_bTempSkill;				// 임시로 생성된 스킬 #26902

	int m_iGlobalSkillGroupID;
	float m_afGlobalCoolTime[ NUM_SKILLLEVEL_APPLY_TYPE ];	// 테이블에선 ms 단위. 갖고 있을 땐 초 단위로 변환.
	float m_fAnotherGlobalSkillCoolTime;					// 다른 글로벌 스킬이 스킬을 사용했을 경우 해당 스킬의 쿨타임으로 한번 돌려준다.
	int m_nAnotherGlobakSkillID;

#if defined( PRE_ADD_ACADEMIC )
	int m_iSummonerDecreaseSP;
	int m_iSummonerDecreaseSPSkillID;
#endif // #if defined( PRE_ADD_ACADEMIC )

	bool m_bEnchantedFromBubble;

protected:
	void _OnInitialize( void );
	void _OnBeginProcessException( void );
	bool _LoadSkillLevelData( int iSkillTableID, int iLevel, int iSkillLevelDataApplyType );
#ifdef PRE_FIX_GAMESERVER_OPTIMIZE
	bool _LoadMonsterSkillLevelData( int iSkillTableID, int iLevel, int iSkillLevelDataApplyType );
#endif // #ifdef PRE_FIX_GAMESERVER_OPTIMIZE
	void _RefreshDecreaseMP( int iSkillLevelDataApplyType );

public:
	DWORD GetStateEffectCount( void );
	StateEffectStruct *GetStateEffectFromIndex( DWORD dwIndex );
	
	bool bIsExtremitySkill(){ return m_iExclusiveID>0; }	// 궁극스킬이냐???

	static CDnSkill::CanApply CanApplySkillStateEffect( DnActorHandle hTargetActor, DnSkillHandle hUsingSkill, map<int, bool>& mapDuplicateResult, bool isHitProcess = false );
	static CDnSkill::CanApply CanApplySkillStateEffect( const SkillInfo* pUsingSkillInfo, DnActorHandle hTargetActor, int iSkillID, int iLevel, int iSkillDuplicateMethod, int iDuplicateCount, map<int, bool>& mapDuplicateResult, bool isHitProcess = false );

	void CheckTargetCount( int iTargetActorCount );

	void SetHasActor( DnActorHandle hActor );

	void CheckAndAddSelfStateEffect( void );
	void CheckAndRemoveInstantApplySelfStateEffect( void );

	void RefreshDecreaseMP( int iSkillLevelDataApplyType = NUM_SKILLLEVEL_APPLY_TYPE );
	
public:
	// 스킬 생성 팩토리 함수
	static DnSkillHandle CreateSkill( DnActorHandle hActor, int iSkillTableID, int iLevel );
#ifdef PRE_FIX_GAMESERVER_OPTIMIZE
	static DnSkillHandle CreateMonsterSkill( DnActorHandle hActor, int iSkillTableID, int iLevel );
#endif // #ifdef PRE_FIX_GAMESERVER_OPTIMIZE
	static SkillTypeEnum GetSkillType( int nSkillID );	
	static void CreateSkillInfo( int nSkillID, int nSkillLevel, CDnSkill::SkillInfo & sSkillInfo, std::vector<CDnSkill::StateEffectStruct> & vecSkillEffect, bool bUseBattleGround = false );
	static void CreateBattleGroundSkillInfo( int nSkillID, int nSkillLevel, CDnSkill::SkillInfo & sSkillInfo, std::vector<CDnSkill::StateEffectStruct> & vecSkillEffect );

	DnActorHandle GetActor( void ) { return m_hActor; };
	int GetClassID( void ) { return m_iSkillID; };
	int GetSkillLevelID( void ) { return m_iSkillLevelID; } 

	//--------------------------------------------------------------
	//[debug_skill]
	int GetSkillMaxUseNum( void ) { return m_nMaxUseNum; }
	int GetSkillNowNum( void ) { return m_nNowNum; }

	int SetSkillMaxUseNum( int nMaxUseNum ) { return m_nMaxUseNum = nMaxUseNum; }
	int SetSkillNowNum( int nNowNum ) { return m_nNowNum = nNowNum; }

	//--------------------------------------------------------------

	bool AddUsableCheckers( IDnSkillUsableChecker* pUsableChecker, int iSelectedLevelData );
	bool AddProcessor( IDnSkillProcessor* pProcessor, int iSelectedLevelData );
	
	// 각종 쿼리 함수들
	SkillTypeEnum GetSkillType( void ) { return m_eSkillType; };
	DurationTypeEnum GetDurationType( void ) { return m_eDurationType; };
	TargetTypeEnum GetTargetType( void ) { return m_eTargetType; };
	
	int GetIncreaseRange( void ) { return m_iIncreaseRange[ m_iSelectedSkillLevelDataApplyType ]; };
	int GetDecreaseHP( void ) { return m_iNeedHP[ m_iSelectedSkillLevelDataApplyType ]; };
	int GetDecreaseMP( void ) { return m_iNeedMP[ m_iSelectedSkillLevelDataApplyType ]; };
	
	TCHAR* GetName( void ) { return const_cast<TCHAR*>(m_strName.c_str()); };
	int GetLevel( void ) { return m_iLevel; };
	int GetNowLevelSkillPoint( void ) { return m_iNowLevelSkillPoint; };
	int GetNextLevelSkillPoint( void ) { return m_iNextLevelSkillPoint; };
	int GetMaxLevel( void ) { return m_iMaxLevel; };
	int GetLevelLimit( void ) { return m_iLevelLimit; };
	
	int GetNeedEquipType( int iIndex ) 
	{ 
		_ASSERT( 0 <= iIndex && iIndex < 2 );
		if( iIndex >= 0 && iIndex < 2 )
		{
			return m_aeNeedEquipType[ iIndex ]; 
		}
		return -1;
	};

	int GetNeedItemID( void ) { return m_iNeedItemID[ m_iSelectedSkillLevelDataApplyType ]; };
	int GetNeedItemDecreaseCount( void ) { return m_iNeedItemDecreaseCount[ m_iSelectedSkillLevelDataApplyType ]; };
	int GetNeedJobClassID( void ) { return m_iNeedJobClassID; };
	int GetAdditionalThreat() { return m_iAdditionalThreat; }

	const char* GetEffectSkinName( void ) { return m_strEffectSkinName.c_str(); };
	const char* GetEffectAniName( void ) { return m_strEffectAniName.c_str(); };
	const char* GetEffectActName( void ) { return m_strEffectActionName.c_str(); };

	bool CanDissolve( void ) { return (m_iDissolvable == 0) ? true : false; };

	int GetMaxDuplicateCount( void ) { return m_iDuplicateCount; };
	int GetDuplicateMethod( void ) { return m_iSkillDuplicateMethod; };
	int	GetEffectDuplicateMethod( void ) { return m_iEffectDuplicateMethod; };

	float GetHPConsumeType( void ) { return m_fHPConsumeType[ m_iSelectedSkillLevelDataApplyType ]; };
	float GetMPConsumeType( void ) { return m_fMPConsumeType[ m_iSelectedSkillLevelDataApplyType ]; };

	int GetCPScore( void ) { return m_iCPScore; }

	CDnState::ElementEnum GetElement( void ) { return m_eElement; };

	bool Initialize( int iSkillTableID, int iLevel );
#ifdef PRE_FIX_GAMESERVER_OPTIMIZE
	bool InitializeMonsterSkill( int iSkillTableID, int iLevel );
#endif // #ifdef PRE_FIX_GAMESERVER_OPTIMIZE

	bool IsSatisfyWeapon( void );
	UsingResult CanExecute( void );
	bool IsFinished( void );

	void OnBeginCoolTime();
	void OnBegin( LOCAL_TIME LocalTime, float fDelta );
	void ProcessExecute( LOCAL_TIME LocalTime, float fDelta );
	void OnEnd( LOCAL_TIME LocalTime, float fDelta );

	// 쿨 타임 등등을 처리
	void Process( LOCAL_TIME LocalTime, float fDelta );

	void EnableToggle( bool bEnable ) { m_bToggle = bEnable; };
	bool IsToggleOn( void ) { return m_bToggle; };

	void EnableAura( bool bEnable ) { m_bAura = bEnable; };
	bool IsAuraOn( void ) { return m_bAura; };

	IDnSkillProcessor* GetProcessor( int iType );
	IDnSkillProcessor* GetProcessor( int iType, int iLevelDataApplyType );

	float GetCoolTime( void ) { return m_fCoolTime; };
	void ResetCoolTime( void ) 
	{ 
		m_fCoolTime = 0.0f; m_fLeftDelayTime = 0.0f;  

		// 같은 글로벌 ID 를 사용하는 다른 글로벌 스킬을 사용하여 셋팅된 쿨타임 값을 실제로 이 스킬 사용될 때는 제거한다.
		m_fAnotherGlobalSkillCoolTime = 0.0f;

#if defined(PRE_ADD_TOTAL_LEVEL_SKILL)
		m_fDeltaGlobalCoolTime = 0.0f;
#endif // PRE_ADD_TOTAL_LEVEL_SKILL
	}
	void IgnoreCoolTime( void ) 
	{ 
		ResetCoolTime();
		for( int i = PVE; i < NUM_SKILLLEVEL_APPLY_TYPE; ++i )
			m_fDelayTime[i] = 0;
	}

	bool IsUseCheckerType( int iCheckerType );
	bool IsUseProcessorType( int iProcessorType );

	int GetNumChecker( void ) { return (int)m_vlpUsableCheckers[ m_iSelectedSkillLevelDataApplyType ].size(); };
	int GetNumProcessor( void ) { return (int)m_vlpProcessors[ m_iSelectedSkillLevelDataApplyType ].size(); };

	void AsItemSkill( void ) 
	{ 
		m_bItemSkill = true; 

		for( int i = PVE; i < NUM_SKILLLEVEL_APPLY_TYPE; ++i )
			m_SkillInfo[ i ].bIsItemSkill = true;
	};
	bool IsItemSkill( void ) { return m_bItemSkill; };

	// CDnStateBlow 에 복사해줄 정보들. 
	const SkillInfo* GetInfo() { return &m_SkillInfo[ m_iSelectedSkillLevelDataApplyType ]; };

	float GetDelayTime( void );
	void SetElapsedDelayTime( float fValue ) { m_fLeftDelayTime = fValue; }
	float GetElapsedDelayTime() { return m_fLeftDelayTime; }
	
	float GetPassiveSkillLength(){ return m_fPassiveActionSkillLength; }
	void SetPassiveSkillLength( float fTimeLength ) { m_fPassiveActionSkillLength = fTimeLength; };
	void SetPassiveSkillActionName( const char* pActionName );
	
	set<string>& GetUseActionNames( void ) { return m_setUseActionNames; }
	void AddUseActionName( const char* pActionName ) { m_setUseActionNames.insert( pActionName ); };
	bool IsUseActionNames( const set<string>& setUseActionNames );

	void AddSignalApplyNoPacketStateEffectHandle( int iBlowID ) { m_vlApplySelfNoDurationStateEffectList.push_back( iBlowID ); };

	void SetOnceCoolTime( float fDelayTime, float fElapsedDelayTime );
	void AsEquipItemSkill( void ) { m_bEquipItemSkill = true; };
	bool IsEquipItemSkill( void ) { return m_bEquipItemSkill; };
	void SetEquipIndex( int iIndex ) { m_iEquipIndex = iIndex; };
	int GetEquipIndex ( void ) { return m_iEquipIndex; };

	// 액션 재생 프로세서가 반드시 있는 액티브 스킬의 동작 계승 요청! (STE_Input 시그널 처리시 호출됨)
	void OnChainInput( const char* pActionName );
	bool IsChainInputAction( const char* pActionName );

	// 패시브 스킬이 체인 입력이 들어왔을 때를 체크한다. 한번 체크되는 순간 체인 입력 플래그는 초기화된다.
	// 체인입력되는 순간 액션의 길이만큼 패시브 스킬 사용 길이가 늘어난다.
	// 이렇게 플래그와 시간 둘 다 같이 사용해야 패시브 스킬의 연속 체인이 가능해진다.
	bool CheckChainingPassiveSkill( void );

	void FromInputHasPassive( void ) 
	{ 
		for( int i = PVE; i < NUM_SKILLLEVEL_APPLY_TYPE; ++i )
			m_SkillInfo[ i ].bFromInputHasPassiveSignal = true; 
	};
	bool IsFromInputHasPassive( void ) { m_SkillInfo[ m_iSelectedSkillLevelDataApplyType ].bFromInputHasPassiveSignal; };

	void SetAppliedPassiveBlows( bool bAppliedPassiveBlows ) { m_bAppliedPassiveSelfBlows = bAppliedPassiveBlows; };
	bool IsAppliedPassiveBlows( void ) { return m_bAppliedPassiveSelfBlows; };

	void AddGlyphStateEffect( int nGlyphID );
	void DelGlyphStateEffect( int nGlyphID );

	// 서버의 쿨타임 오차를 감안해서 0.5초의 여유를 두고 있지만,
	// 존이동할때나 기타 0.5초 이상 클라이언트와 벌어지는 다른 새로운 경우들이 생길 수 있으므로
	// 최종 스킬 사용한 타임 스탬프를 찍어두어 데이터에 지정된 스킬의 쿨타임보타 간격이 크다면 
	// 서버의 스킬 객체에 저장되어있는 쿨타임을 초기화 시켜주도록 한다. (#19737)
	// 그리고 타임 엑셀러레이션 같은 쿨 타임 빨라지게 하는 스킬도 감안해주어야 하긴 하는데 이 함수는 
	// 안전 장치기 때문에 우선 놔둔다.
	void UpdateSkillCoolTimeExactly( void );

	// 스킬 레벨 데이터 값 셋팅. 조건에 맞지 않으면 셋팅 안된다.
	bool SelectLevelDataType( int iSkillLevelDataType, bool bPlayerSummonedMonster = false );
	int GetSelectedLevelDataType( void ) { return m_iSelectedSkillLevelDataApplyType; };

	int GetExclusiveID( void ) { return m_iExclusiveID; };

	// 임시로 얻은 스킬을 위한 함수들..
	void AsTempSkill( void ) { m_bTempSkill = true; };
	bool IsTempSkill( void ) { return m_bTempSkill; };
	void SetLevelLimit( int iLevelLimit ) { m_iLevelLimit = iLevelLimit; };
	void SetNeedJobClassID( int iJobID ) { m_iNeedJobClassID = iJobID; };
	//////////////////////////////////////////////////////////////////////////


	// 지속시간 여부와 관계 없이 무조건 스킬효과가 적용 가능한지 체크해야하는 상태효과인가.
	static bool IsNeedCheckApplyStateBlow( STATE_BLOW::emBLOW_INDEX emBlowIndex );

	// 강화 패시브 스킬 타입 (EnchantPassive) 인 경우 기반이 되는 스킬 아이디를 얻어옴.
	int GetBaseSkillID( void );

	// 강화 패시브 스킬 값을 적용시킴.
	// 강화 패시브 스킬 아이디, 데이터 등은 따로 구조체에 담아서 가지고 있는다.
	// 한번 적용되면 같은 강화 패시브 스킬 ID 는 적용되지 않도록 한다.
	bool ApplyEnchantSkill( DnSkillHandle hEnchantPassiveSkill );
	void ApplyEnchantSkillOnceFromBubble( DnSkillHandle hEnchantPassiveSkill );
	bool CheckAndUnifyStateEffectArgument( int iSkillLevelApplyType, StateEffectStruct* pEnchantSkillSE );
	void ReleaseEnchantSkill( void );
	bool CheckAndDivideStateEffectArgument( StateEffectStruct* pDestSkillSE );
	bool IsEnchantedSkill( void ) { return (0 != m_iAppliedEnchantPassiveSkillID); };
	int GetEnchantedSkillID( void ) { return m_iAppliedEnchantPassiveSkillID; };

	//접두어 스킬 확률값 재 계산을 위해..
	IDnSkillUsableChecker* GetChecker( int iType );
	//접두어 스킬의 우선선위

	bool IsExistUsableChecker( int iType ); // 정의된 UsableChecker을 가지고있느냐?

	void CheckProcessorOnBegin( void );
	void CheckChangeActionStrByBubbleProcessor();
	void CheckStateEffectApplyOnOffByBubbleProcessor();
	void CheckProcessorOnEnd( void );

	int GetGlobalSkillGroupID( void ) { return m_iGlobalSkillGroupID; };
	void OnAnotherGlobalSkillBeginCoolTime( DnSkillHandle hSkill );

protected:
	int m_nPriority;
public:
	void SetPrefixSystemPriority(int nPriority) { m_nPriority = nPriority; }
	int GetPrefixSystemPriority() { return m_nPriority; }

protected:
	int m_nItemID;
public:
	int GetItemID() { return m_nItemID;}
	void SetItemID(int nItemID) 
	{ 
		m_nItemID = nItemID;
		for( int i = PVE; i < NUM_SKILLLEVEL_APPLY_TYPE; ++i )
			m_SkillInfo[ i ].nItemID = m_nItemID;
	}

protected:
	bool m_bIsPrefixTriggerSkill;
public:
	void SetPrefixTriggerSkill(bool bValue) { m_bIsPrefixTriggerSkill = bValue; }
	bool IsPrefixTriggerSkill() { return m_bIsPrefixTriggerSkill; }
	void SetItemPrefixSkill() { m_SkillInfo[ PVP ].bItemPrefixSkill = true; m_SkillInfo[ PVE ].bItemPrefixSkill = true; };
	bool IsItemPrefixSkill() { return m_SkillInfo[ m_iSelectedSkillLevelDataApplyType ].bItemPrefixSkill; };


protected:
	float m_fCoolTimeMultipier;	//스클 쇼타임에 의한 쿨타임 조절용
public:
	float GetCoolTimeAdjustValue() { return m_fCoolTimeMultipier; }
	void SetCoolTimeAdjustValue(float fValue) 
	{ 
		m_fCoolTimeMultipier = fValue; 

		m_fCoolTime *= m_fCoolTimeMultipier;
		m_fLeftDelayTime *= m_fCoolTimeMultipier;
	}

protected:
	bool IsFinishedAuraSkill();

protected:
	bool m_isIgnoreImmuneBackup;
public:
	//트리거에서 hActor->UseSkill(nSkillID)로 호출 되는 부분이 생김.
	//해당 스킬을 찾아서 스킬정보를 수정..OnEnd에서 원래 값으로 변경.
	void SetIgnoreImmnune(bool bIgnore)
	{
		m_isIgnoreImmuneBackup = m_SkillInfo->bIgnoreImmune;
		m_SkillInfo->bIgnoreImmune = bIgnore;
	}

//////////////////////////////////////////////////////////////////////////
// 접두어에서 하나의 스킬이 액터별로 상태효과를(지속시간이 없는) 적용 시킨다.
// 스킬이 끝날때 액터별로 적용한 상태효과를 제거 하기위해 리스트를 만들어 놓고
// 각각을 제거해야 한다..
protected:
	struct ActorHandleComp
	{
		bool operator()(const DnActorHandle& h1, const DnActorHandle& h2) const
		{
			return (h1.GetHandle() < h2.GetHandle());
		}
	};

	typedef vector<int> BLOWLIST;
	typedef std::map<DnActorHandle, BLOWLIST, ActorHandleComp> PREFIX_SKILL_BLOWLIST;

	PREFIX_SKILL_BLOWLIST m_PrefixBlowList;

public:
	void AddPrefixBlow(DnActorHandle hActor, int nBlowID);
	void RemovePrefixBlow();
//////////////////////////////////////////////////////////////////////////

#if defined( PRE_ADD_ACADEMIC )
	void SetSummonerDecreaseSP( int iSkillID, int iSP ){ m_iSummonerDecreaseSPSkillID=iSkillID; m_iSummonerDecreaseSP = iSP; }
	int GetSummonerDecreaseSP(){ return m_iSummonerDecreaseSP; }
	int GetSummonerDecreaseSPSkillID(){ return m_iSummonerDecreaseSPSkillID; }
#endif // #if defined( PRE_ADD_ACADEMIC )

protected:
	int m_nLevelUpValue;
public:
	void SetLevelUpValue(int nLevelUp) { m_nLevelUpValue = nLevelUp; }
	int GetLevelUpValue() { return m_nLevelUpValue; }

#if defined(PRE_ADD_PREFIX_SYSTE_RENEW)
protected:
	int m_nPrefixSkillType;
public:
	void SetPrefixSkillType(int nSkillType) { m_SkillInfo[PVP].nPrefixSkillType = nSkillType; m_SkillInfo[PVE].nPrefixSkillType = nSkillType; }
	int GetPrefixSkillType() { return m_SkillInfo[m_iSelectedSkillLevelDataApplyType].nPrefixSkillType; }
#endif // PRE_ADD_PREFIX_SYSTE_RENEW


protected:
	int m_SummonMonsterID;

	void OnInitializeSummonMonsterInfo();				//소환 몬스터 ID정보를 스킬 액션 시그널에서 찾는다.
	DnActorHandle FindSummonMonster(int nMonsterID);	//설정된 몬스터가 소환 되어 있는지 확인한다.
	
public:
	int GetSummonMonsterID() { return m_SummonMonsterID; }
	bool SummonMonsterOff();				//소혼 몬스터 소환 해제 시킨다.


	//문자열 index;index;index......;index; 에서 blowIndex와 같은 값이 있으면 이 상태효과는 적용되지 않는다.
	static bool IsSkipStateBlow(const char* szSkipStateBlows, STATE_BLOW::emBLOW_INDEX blowIndex);


protected:
	bool m_bAddStateEffectQueue;
public:
	bool IsAddStateEffectQueue() { return m_bAddStateEffectQueue; }
	void SetAddStateEffectQueue(bool bAdded) { m_bAddStateEffectQueue = bAdded; }

	void SetChaningPassiveSkill( const bool bChaningPassiveSkill )	{ m_bChainingPassiveSkill = bChaningPassiveSkill; }

#if defined(PRE_ADD_50903)
	//데미지 분산 상태효과에 사용될 HitSignal에서 히트된 액터수
protected:
	int m_nHitCountForVarianceDamage;
public:
	void SetHitCountForVarianceDamage(int nCount) { m_nHitCountForVarianceDamage = nCount; }
	int GetHitCountForVarianceDamage() { return m_nHitCountForVarianceDamage; }
#endif // PRE_ADD_50903


protected:
	typedef std::list<CDnProjectile*> PROJECTILE_LIST;	//스킬에서 사용된 발사체 리스트(242번 상태효과를 위해서)
	typedef std::map<LOCAL_TIME, PROJECTILE_LIST> SKILL_PROJECTILE_LIST;
	SKILL_PROJECTILE_LIST m_ProjectileList;
public:
	int GetProjectileCount(LOCAL_TIME startTime);// { return (int)m_ProjectileList.size(); }
	void AddProjectile(CDnProjectile* pProjectile);		//액터에서 STE_Projectile로 생성 될때 242번 상태효과를 가지고 있는 스킬이 시전 중인경우 호출됨.
	void RemoveProjectile(CDnProjectile* pProjectile);	//발사체 소멸자에서 발사체에 설정된 스킬 정보로 해당 스킬을 찾아서 호출해서 제거.

	LOCAL_TIME GetSkillStartTime() { return m_SkillStartTime; }

protected:
	void ClearStigmaStateEffect();	//낙인 상태효과를 위한 함수(스킬 사용전에 이미 사용된 낙인 효과가 있으면 제거 한다.)

protected:
	DNVector(StateEffectStruct) m_AddtionalStateInfoList;

	//칼리 중첩 추가 효과 구현을 위해 추가 된 함수..
	void ApplyAddtionalStateInfo();		//추가될 상태효과 추가
	void RemoveAddtionalStateInfo();	//추가된 상태효과 제거

#if defined(PRE_FIX_64312)
protected:
	bool m_isAppliedSummonMonsterEnchantSkill;
	bool m_bIsSummonMonsterSkill;

	DnSkillHandle m_SummonMonsterEnchantSkill;

public:
	bool IsSummonMonsterSkill();

	void AddSummonMonsterEnchantSkill(DnSkillHandle hSkill);
	void RemoveSummonMonsterEnchantSkill();

	DnSkillHandle GetSummonMonsterEnchangeSkill() { return m_SummonMonsterEnchantSkill; }

	bool ApplySummonMonsterEnchantSkill(DnSkillHandle hSkill);
#endif // PRE_FIX_64312

#if defined(PRE_ADD_TOTAL_LEVEL_SKILL)
protected:
	float m_fDeltaGlobalCoolTime;

public:
	float GetOrigDelayTime();
	void UpdateGlobalCoolTime(float fRate);
	void ResetGlobalCoolTime(float fRate);
#endif // PRE_ADD_TOTAL_LEVEL_SKILL

#if defined(PRE_FIX_66175)
protected:
	vector<IDnSkillUsableChecker*> m_vlUsableCheckersBackup[NUM_SKILLLEVEL_APPLY_TYPE];
public:
	vector<IDnSkillUsableChecker*>& GetUsableChecker(int nType) { return m_vlpUsableCheckers[nType]; }
#endif // PRE_FIX_66175

#if defined(PRE_ADD_65808)
public:
	bool IsSummonMonsterRecall(int& monsterID);
#endif // PRE_ADD_65808

protected:
	string m_BackupEffectOutputIDs[NUM_SKILLLEVEL_APPLY_TYPE];
	string m_BackupEffectOutputIDToClient[NUM_SKILLLEVEL_APPLY_TYPE];
public:
	string& GetEffectOutputIDs(int iSkillLevelDataApplyType) { return m_SkillInfo[iSkillLevelDataApplyType].szEffectOutputIDs; }
	string& GetEffectOutputIDToClient(int iSkillLevelDataApplyType) { return m_SkillInfo[iSkillLevelDataApplyType].szEffectOutputIDToClient; }

	string& GetBackupEffectOutputIDs(int iSkillLevelDataApplyType) { return m_BackupEffectOutputIDs[iSkillLevelDataApplyType]; }
	string& GetBackupEffectOutputIDToClient(int iSkillLevelDataApplyType) { return m_BackupEffectOutputIDToClient[iSkillLevelDataApplyType]; }

	int GetAnotherGlobalSkillID() { return m_nAnotherGlobakSkillID; }
};


//////////////////////////////////////////////////////////////////////////

#if defined(_GAMESERVER)
#if defined(PRE_ADD_PREFIX_SYSTE_RENEW)
class CDnPrefixSkill;
#endif // PRE_ADD_PREFIX_SYSTE_RENEW
class CoolTimeInfo
{
protected:
	float m_fDelayTime;

	float m_fCoolTime;
	float m_fCoolTimeAdjustBlowValue;
	float m_fLeftDelayTime;

	CDnSkill::DurationTypeEnum m_eDurationType;

	int m_nSkillID;

	DnActorHandle m_hActor;

public:
	CoolTimeInfo()
	{
		m_fDelayTime = 0.0f;
		m_fCoolTime = 0.0f;
		m_fCoolTimeAdjustBlowValue = 1.0f;
		m_fLeftDelayTime = 0.0f;
		m_eDurationType = CDnSkill::DurationTypeEnum::Instantly;
		m_nSkillID = 0;
	};
	~CoolTimeInfo() {};

	void OnBeginCoolTime();
	bool Process(LOCAL_TIME localTime, float fDelta);	
	void SetInfo(DnSkillHandle &hSkill);
#if defined(PRE_ADD_PREFIX_SYSTE_RENEW)
	void SetInfo(CDnPrefixSkill* pPrefixSkill);
#endif // PRE_ADD_PREFIX_SYSTE_RENEW
};


class CoolTimeManager
{
protected:
	typedef std::map<int, CoolTimeInfo*> COOLTIME_LIST;
	COOLTIME_LIST m_CoolTimeList;

public:
	CoolTimeManager() {};
	~CoolTimeManager() {};
	
	bool IsCoolTime(int nSkillID);

	void AddCoolTime(DnSkillHandle hSkill);
#if defined(PRE_ADD_PREFIX_SYSTE_RENEW)
	void AddCoolTime(CDnPrefixSkill* pPrefixSkill);
#endif // PRE_ADD_PREFIX_SYSTE_RENEW

	void RemoveCoolTime(int nSkillID);
	void Process(LOCAL_TIME localTime, float fDelta);

	void InitList();
};
#endif // _GAMESERVER

//////////////////////////////////////////////////////////////////////////


#if defined(PRE_ADD_PREFIX_SYSTE_RENEW)
class CDnPrefixSkill
{
public:
	CDnPrefixSkill(int nPrefixType);
	~CDnPrefixSkill();

	enum PrefixType
	{
		Prefix_Non = -1,//접두사 아님.
		Prefix_000 = 0,	//파괴의
		Prefix_001,	//마법의
		Prefix_002,	//곰의 무기
		Prefix_003,	//바람의 무기
		Prefix_004,	//지혜의 무기
		Prefix_005,	//가혹한
		Prefix_006,	//어둠의 무기
		Prefix_007,	//생명의 무기
		Prefix_008,	//마나의 무기
		Prefix_009,	//활기의
		Prefix_010,	//치명적인
		Prefix_011,	//구속의
		Prefix_012,	//충격의
		Prefix_013,	//용자의
		Prefix_014,	//기사의
		Prefix_015,	//철벽의 무기
		Prefix_016,	//장막의 무기
		Prefix_017,	//불의 무기
		Prefix_018,	//물의 무기
		Prefix_019,	//빛의 무기
		Prefix_020,	//철벽의 방어구
		Prefix_021,	//장막의 방어구
		Prefix_022,	//곰의 방어구
		Prefix_023,	//바람의 방어구
		Prefix_024,	//지혜의 방어구
		Prefix_025,	//건강한
		Prefix_026,	//행운의
		Prefix_027,	//생명의 방어구
		Prefix_028,	//마나의 방어구
		Prefix_029,	//견고한
		Prefix_030, //유연한
		Prefix_031, //의지의
		Prefix_032, //강인한
		Prefix_033, //불굴의
		Prefix_034, //불의 방어구
		Prefix_035, //물의 방어구
		Prefix_036, //빛의 방어구
		Prefix_037, //어둠의 방어구
	};
protected:
	int m_nPrefixType;				//접두사 종류??
	float m_fProbability;			//접두사 스킬 발동 확률(스킬이 더해 질때 확률 더함)
	DnSkillHandle m_hSkill;	//대표 스킬..

	std::list<DnSkillHandle> m_SkillList;

	DNVector(CDnSkill::StateEffectStruct)		m_vlStateEffectList;
	
public:
	int GetSkillType() { return m_nPrefixType; }		//접두사 스킬 종류(행운의/파괴의....)
	float GetProbability() { return m_fProbability; }	//접두사 스킬 발동 확률

	DnSkillHandle GetSkillHandle() { return m_hSkill; }
	void SetSkillHandle(DnSkillHandle hSkill);		//대표 스킬 설정.

	int GetSkillCount() { return (int)m_SkillList.size(); }	//등록된 스킬 갯수

	void AddSkill(DnSkillHandle hSkill);			//접두사 스킬 추가..
	void RemoveSkill(DnSkillHandle hSkill);			//접두사 스킬 제거

	//등록된 스킬중에서 쿨타임이 짧은 녀석을 대표 스킬로 설정
	void UpdateCandidateSkill();

	//상태효과 값 빼기 연산 할때 문제가 발생(기존 값 - 스킬 하나 상태효과)
	//그래서 스킬 하나를 제거 하고 나서 스킬 리스트 전체를 돌면서 상태효과값 적용.
	void UpdateStateEffects();

	void AddStateEffectInfo(DnSkillHandle hSkill);		//스킬 상태효과 합치기..

	int GetStateEffectCount() { return (int)m_vlStateEffectList.size(); }
	CDnSkill::StateEffectStruct* GetStateEffectFromIndex(int nIndex) 
	{ 
		if( nIndex < 0 || nIndex >= (int)m_vlStateEffectList.size() ) return NULL;

		return &m_vlStateEffectList[nIndex]; 
	}

protected:
	void AddStateEffectInfo( STATE_BLOW::emBLOW_INDEX blowList[], int nCount, DnSkillHandle hSkill );

	void AddStateEffectInfo_Prefix_000(DnSkillHandle hSkill);//파괴의
	void AddStateEffectInfo_Prefix_001(DnSkillHandle hSkill);//마법의
	void AddStateEffectInfo_Prefix_002(DnSkillHandle hSkill);//곰의 무기
	void AddStateEffectInfo_Prefix_003(DnSkillHandle hSkill);//바람의 무기
	void AddStateEffectInfo_Prefix_004(DnSkillHandle hSkill);//지혜의 무기
	void AddStateEffectInfo_Prefix_005(DnSkillHandle hSkill);//가혹한
	void AddStateEffectInfo_Prefix_006(DnSkillHandle hSkill);//어둠의 무기
	void AddStateEffectInfo_Prefix_007(DnSkillHandle hSkill);//생명의 무기
	void AddStateEffectInfo_Prefix_008(DnSkillHandle hSkill);//마나의 무기
	void AddStateEffectInfo_Prefix_009(DnSkillHandle hSkill);//활기의
	void AddStateEffectInfo_Prefix_010(DnSkillHandle hSkill);//치명적인
	void AddStateEffectInfo_Prefix_011(DnSkillHandle hSkill);//구속의
	void AddStateEffectInfo_Prefix_012(DnSkillHandle hSkill);//충격의
	void AddStateEffectInfo_Prefix_013(DnSkillHandle hSkill);//용자의
	void AddStateEffectInfo_Prefix_014(DnSkillHandle hSkill);//기사의
	void AddStateEffectInfo_Prefix_015(DnSkillHandle hSkill);//철벽의 무기
	void AddStateEffectInfo_Prefix_016(DnSkillHandle hSkill);//장막의 무기
	void AddStateEffectInfo_Prefix_017(DnSkillHandle hSkill);//불의 무기
	void AddStateEffectInfo_Prefix_018(DnSkillHandle hSkill);//물의 무기
	void AddStateEffectInfo_Prefix_019(DnSkillHandle hSkill);//빛의 무기
	void AddStateEffectInfo_Prefix_020(DnSkillHandle hSkill);//철벽의 방어구
	void AddStateEffectInfo_Prefix_021(DnSkillHandle hSkill);//장막의 방어구
	void AddStateEffectInfo_Prefix_022(DnSkillHandle hSkill);//곰의 방어구
	void AddStateEffectInfo_Prefix_023(DnSkillHandle hSkill);//바람의 방어구
	void AddStateEffectInfo_Prefix_024(DnSkillHandle hSkill);//지혜의 방어구
	void AddStateEffectInfo_Prefix_025(DnSkillHandle hSkill);//건강한
	void AddStateEffectInfo_Prefix_026(DnSkillHandle hSkill);//행운의
	void AddStateEffectInfo_Prefix_027(DnSkillHandle hSkill);//생명의 방어구
	void AddStateEffectInfo_Prefix_028(DnSkillHandle hSkill);//마나의 방어구
	void AddStateEffectInfo_Prefix_029(DnSkillHandle hSkill);//견고한
	void AddStateEffectInfo_Prefix_030(DnSkillHandle hSkill);//유연한
	void AddStateEffectInfo_Prefix_031(DnSkillHandle hSkill);//의지의
	void AddStateEffectInfo_Prefix_032(DnSkillHandle hSkill);//강인한
	void AddStateEffectInfo_Prefix_033(DnSkillHandle hSkill);//불굴의
	void AddStateEffectInfo_Prefix_034(DnSkillHandle hSkill);//불의 방어구
	void AddStateEffectInfo_Prefix_035(DnSkillHandle hSkill);//물의 방어구
	void AddStateEffectInfo_Prefix_036(DnSkillHandle hSkill);//빛의 방어구
	void AddStateEffectInfo_Prefix_037(DnSkillHandle hSkill);//어둠의 방어구
};
#endif // PRE_ADD_PREFIX_SYSTE_RENEW
