#pragma once

//#include "DnActor.h"
#include "MIInventoryItem.h"
#include "DnWeapon.h"
#include "DnSkillTreeSystem.h"
#include "SmartPtr.h"


class IDnSkillUsableChecker;
class IDnSkillProcessor;

const int MAX_SKILL_NEED_EQUIP_COUNT = 2;

const int TIME_ACCELERATION_SKILL_ID = 2415;
const int SPRIT_BOOST_SKILL_ID = 1412;


// 스킬 객체
class CDnSkill : public CSmartPtrBase< CDnSkill >, public MIInventoryItem
{
public:
	CDnSkill( DnActorHandle hActor );
	virtual ~CDnSkill(void);


	enum SkillTypeEnum 	{
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
		ActiveToggleForSummon,
		/*
		몬스터 소환 스킬은 UI표시는 Toggle로 표시 되도록 하고, 실제 스킬 사용은 Instanly와 같은 처리되도록함.
		OnBegin에서 EnableToggle 토글 활성화 시키고, 몬스터 소환 해제 시점에서 EnableToggle 비활성화 시킴.
		스킬 사용 시점에서 스킬 사용이 가능 하더라도 소환 몬스터가 있으면 소환 해제만 하고 스킬 사용은 안됨.
		스킬 사용 실패시에도 소환 몬스터가 있으면 소환 몬스터 소환 해제 시킴.
		*/
		SummonOnOff,					//소환 몬스터 소환 On/Off용 스킬(알프레도 소환, 소환해제)
		StanceChange,
		TotalLevelSkill,				// 통합 레벨 스킬
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
		int nFromEnchantPassiveSkillID;			// 강화 패시브 스킬 ID 로 부터 덧붙여진 상태효과라면 해당 스킬의 아이디를 담고 있는다.

		// #40643 핑퐁밤EX 처럼 특수하게 구현된 상태효과의 인자를 건드리는 경우..
		// 어쩔 수 없이 합쳐줘야 한다.
		// 이 플래그가 true 이면 이미 기존에 있는 상태효과에 합쳐진 상태이기 때문에 실제 상태효과 적용할 땐 적용하지 않는다.
		string strEnchantSkillSEParam;

		// #52905 중첩 추가 효과 여부 
		bool bAddtionalStateInfo;

		StateEffectStruct( void ) : nID( 0 ), ApplyType( ApplySelf ), nDurationTime( 0 ), nGlyphID( 0 ), nFromEnchantPassiveSkillID( 0 ), bAddtionalStateInfo(false) {}
	};

	enum GLYPH_EFFECT {
		NONE,
		SKILLMP_PRO,
		SKILLDELAY_PRO,
		SKILLDELAY_RESET_PRO,
		SKILLDURATION_PRO,
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
		int iSkillLevelIDOffset;
#ifdef PRE_FIX_SYNC_ENCHANT_SKILL
		int iAppliedEnchantSkillID;
#endif

		// CS_CMDADDSTATEEFFECT 패킷으로 날라오는 스킬 정보는 위의 두 변수만.
		DurationTypeEnum	eDurationType;
		SkillTypeEnum		eSkillType;
		int					iDissolvable;
		int					iSkillBufIcon;
		string				effectOutputIDs;
		DnActorHandle		hSkillUser;
		bool				bIsItemSkill;
		int					iBuffIconImageIndex;
#ifdef PRE_ADD_SKILL_ADDTIONAL_BUFF
		int                 iDebuffIconImageIndex;
		string				debuffEffectOutputIDs;
#endif
		TargetTypeEnum		eTargetType;
		int					iSkillUserTeam;
		bool				bFromInputHasPassiveSignal;
		bool				bFromBuffProp;
		
		// 아이템 접두어 시스템에서 사용하는 스킬. 서버에서는 값이 직접 셋팅되며 클라이언트로는 SC_CMDADDSTATEEFFECT 패킷으로 셋팅된다.
		// 이 플래그가 켜져 있는 접두어 스킬의 상태효과들은 Duplicate 처리를 하면 안됨. 패킷받자마자 곧바로 begin 됨.
		bool				bItemPrefixSkill;

		// 길드전 스킬(성문 관련) 면역 처리 무시를 위해 추가 [2011/04/14 semozz]
		bool				bIgnoreImmune;

#if defined(PRE_FIX_NEXTSKILLINFO)
		int					nSkillLevel;	//스킬레벨 정보
#endif // PRE_FIX_NEXTSKILLINFO

		SkillInfo( void ) : iSkillID( 0 ), iSkillLevelID( 0 ), iSkillLevelIDOffset( 0 ),
#ifdef PRE_FIX_SYNC_ENCHANT_SKILL
							iAppliedEnchantSkillID(0),
#endif
							eDurationType( Instantly ), iDissolvable( 0 ),
							iSkillBufIcon( 0 ), 
							eSkillType( Active ), bIsItemSkill(false),
							iBuffIconImageIndex( -1 ), eTargetType(Self), iSkillUserTeam(-1), bFromInputHasPassiveSignal(false), bFromBuffProp( false ), bIgnoreImmune(false)
#ifdef PRE_ADD_SKILL_ADDTIONAL_BUFF
							, iDebuffIconImageIndex( -1 )
#endif
#if defined(PRE_FIX_NEXTSKILLINFO)
							, nSkillLevel(0)
#endif // PRE_FIX_NEXTSKILLINFO
							, bItemPrefixSkill( false ) {}

		bool operator == ( const SkillInfo& Rhs ) const
		{
			return (iSkillID == Rhs.iSkillID) &&
				   (iSkillLevelID == Rhs.iSkillLevelID);
		}

		bool operator < ( const SkillInfo& Rhs ) const
		{
			return (iSkillID < Rhs.iSkillID);
		}
	};


protected:

	DnActorHandle m_hActor;

	bool m_bLock;			// 현재 스킬북을 사용하지 않은 락 상태인지.
	bool m_bAcquired;		// 스킬 획득 상태인지. 

	bool m_bAcquiredByBlow;	//rlkt_ blow_345

	// 현재 pvp/pve 데이터인지
	int m_iSelectedSkillLevelDataApplyType;

	SkillTypeEnum				m_eSkillType;

	int							m_iSkillID;
	int							m_iSkillLevelID;
	int							m_iNextLevelSkillPoint;		// 다음 스킬 레벨업을 하기 위해 필요한 스킬 포인트. 
	int							m_iFirstLevelSkillPoint;	// 첫번째 레벨의 스킬 포인트
	tstring						m_strName;
	string						m_strStaticName;
	tstring						m_strSkillExplainString[ NUM_SKILLLEVEL_APPLY_TYPE ];
	int							m_iSkillExplainStringID;		// 스킬 설명 문자열 아이디. CEtUIXML::GetUIString() 함수로 얻을 때 사용하는 ID
	int							m_iNextSkillExplainStringID;	// 다음 스킬 설명 문자열 아이디. 
	tstring						m_strNextSkillExplainString[ NUM_SKILLLEVEL_APPLY_TYPE ];

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
	int							m_iNeedHP[ NUM_SKILLLEVEL_APPLY_TYPE ];
	int							m_iNeedMP[ NUM_SKILLLEVEL_APPLY_TYPE ];
	int							m_iOriginalNeedMP[ NUM_SKILLLEVEL_APPLY_TYPE ];	//문장 스킬효과로 변경 되기전에 MP를 기록함
	int							m_iCPScore;

	int							m_iLevel;
	int							m_iMaxLevel;
	int							m_iUsePoint;
	int							m_iLevelLimit;
	int							m_iFirstLevelLimit;		// 스킬 습득하기 위한 최소 레벨(1레벨 스킬의 필요레벨)
	int							m_iIncreaseRange[ NUM_SKILLLEVEL_APPLY_TYPE ];		// 추가 사거리
	int							m_iDecreaseHP[ NUM_SKILLLEVEL_APPLY_TYPE ];
	int							m_iDecreaseMP[ NUM_SKILLLEVEL_APPLY_TYPE ]; 

	float						m_fOriginalDelayTime[ NUM_SKILLLEVEL_APPLY_TYPE ];	//문장 스킬효과로 변경 되기전에 쿨타임을 기록함
	float						m_fDelayTime[ NUM_SKILLLEVEL_APPLY_TYPE ];
	float						m_fLeftDelayTime;
	float						m_fCoolTime;

	//--------------------------------------------------------------
	//[debug_skill]
	int                         m_nMaxUseNum;
	int                         m_nNowNum;
	//--------------------------------------------------------------


	int							m_iBuffIconImageIndex;
	string						m_EffectOutputIDs;
#ifdef PRE_ADD_SKILL_ADDTIONAL_BUFF
	int                         m_iDebuffIconImageIndx;
	string						m_DebuffEffectOutputIDs;
#endif
	
	int							m_iUnlockSkillBookItemID;
	int							m_iUnlockPrice;

	int							m_iBaseSkillID;	// 패시브 강화 스킬인 경우에만 유효함. 강화 시킬 대상의 스킬 ID.
	int							m_iAppliedEnchantPassiveSkillID;
	vector<IDnSkillProcessor*> m_vlpProcessorBackup[ NUM_SKILLLEVEL_APPLY_TYPE ];

	LOCAL_TIME					m_LastTimeToggleMPDecreaseTime;
	bool						m_bFinished;

	// 토글, 오라 스킬인 경우 토글 된 상태인지.
	bool						m_bNowToggled;
	bool						m_bNowAura;

	bool						m_bCantUseInVillage;

	bool                        m_bEnableActiveToggleCoolTime;
	
	vector<StateEffectStruct>		m_vlStateEffectList[ NUM_SKILLLEVEL_APPLY_TYPE ];
	vector< pair<int, int> >		m_vlStateDurationList[ NUM_SKILLLEVEL_APPLY_TYPE ];

	// 스킬 발동 조건 리스트
	vector<IDnSkillUsableChecker*>	m_vlpUsableCheckers[ NUM_SKILLLEVEL_APPLY_TYPE ];

	// 스킬 프로세스 리스트.. 상태 이상이라던지 기타 등등의 처리해야 하는 상황들의 추상화.
	vector<IDnSkillProcessor*>		m_vlpProcessors[ NUM_SKILLLEVEL_APPLY_TYPE ];

	bool							m_bItemSkill;							// 이 스킬, 아이템 스킬인가.

	// 클라이언트에서는 아직 사용하지 않음. 외부에 전달해줄 스킬의 핵심 정보들.
	// 매번 TableDB에서 꺼내오는 번거로움을 줄임.
	SkillInfo						m_SkillInfo[ NUM_SKILLLEVEL_APPLY_TYPE ];

	// 패시브 액션 스킬을 위해 셋팅된 지속시간
	float							m_fPassiveActionSkillLength;
	vector<DnBlowHandle>			m_vlAppliedNoPacketInstantBlow;			// 서버에서 패킷을 보내주지 않고 직접 판단하는 상태효과 중에 지속시간이 없는 Instant 인 경우
	CDnState::ElementEnum			m_eElement;

	// 이 스킬이 Play Ani 를 사용한다면 채워지게되는 해당 액션
	string							m_strAction;

	set<string>						m_setUseActionNames;

	// 쿨타임 한번만 돌고나서 원래 쿨타임 값을 사용한다. 장비아이템 스킬 때문에 같은 슬롯의 직전 장비 아이템의 쿨타임을 적용받는다.
	float m_fOnceDelayTime;
	float m_fOnceElapsedDelayTime;

	// 장비 아이템 스킬인가
	bool m_bEquipItemSkill;

	// 스킬트리 노드 렌더링 정보
	CDnSkillTreeSystem::S_NODE_RENDER_INFO m_NodeRenderInfo;

	// 쿨타임 조절 상태효과의 값을 스킬 쓸 당시에 가져와서 저장해둔다. 
	float m_fCoolTimeAdjustBlowValue;

	// 새로 얻은 것은 true
	bool m_bNewAcquired;

	bool m_bAppliedPassiveSelfBlows;

	bool m_bResetSkill;
	std::vector<GlyphEffectStruct> m_vGlyphEffectList;

	int m_iExclusiveID;				// 스킬 테이블에 있는 같이 배울 수 없는 스킬들의 구분을 위한 id

	bool m_bTempSkill;				// 임시로 생성된 스킬 #26902

	bool m_bEnableTooltip;

	bool m_bGlyphActiveSkillDisbale;

	// 아이템에 의해 성성된 스킬인지 확인용(m_bItemSkill과는 다른용도) [2011/03/16 semozz]
	bool m_isCreatedByItem;

	int m_iGlobalSkillGroupID;
	float m_afGlobalCoolTime[ NUM_SKILLLEVEL_APPLY_TYPE ];	// 테이블에선 ms 단위. 갖고 있을 땐 초 단위로 변환.
	float m_fAnotherGlobalSkillCoolTime;					// 다른 글로벌 스킬이 스킬을 사용했을 경우 해당 스킬의 쿨타임으로 한번 돌려준다.
	int m_iAnotherGlobalSkillLevelDataType;
	int m_nAnotherGlobakSkillID;

protected:
	// 서버에서 따로 패킷을 보내주지 않는 StateEffect 를 체크. 화면에 뿌려줄 이펙트를 결정한다.
	//void _CheckNoPacketSelfStateEffect( void );	
	void _OnBeginProcessException( void );

	bool _LoadSkillLevelData( int iSkillTableID, int iLevel, int iSkillLevelDataApplyType );
	void _RefreshDecreaseMP( int iSkillLevelDataApplyType );

	// input 시그널로 관련된 액션들 모두 추가함.
	void _AddUsedActions( const char* pActionName );

public:

	bool bIsExtremitySkill(){ return m_iExclusiveID>0; }	// 궁극스킬이냐???

	// 스킬 생성용 데이터 검증
	static bool ValidateSkillData( int iSkillTableID, int iLevel );

	// 스킬 생성 팩토리 함수. 얻지 못한 스킬은 레벨 0으로 생성됩니다.
	static DnSkillHandle CreateSkill( DnActorHandle hActor, int iSkillTableID, int iLevel );

	// 언락 조작은 이 스킬을 실제 유저가 소유하게 된 이후에는 불가능.
	void Lock( void ) { m_bLock = true; m_bAcquired = false; };
	void UnLock( void ) { _ASSERT( 1 == m_iLevel); m_bLock = false; };		// 클라에서 락상태는 초기 레벨 1상태임.
	bool IsLocked( void ) { return m_bLock; };

	// 언락된 스킬을 처음으로 획득함. 획득할 땐 반드시 언락 상태여야 함.
	void Acquire( void );
	bool IsAcquired( void ) { return m_bAcquired; };

	bool IsAcquiredByBlow(void) { return m_bAcquiredByBlow; };
	void AcquireByBlow(bool nValue) { m_bAcquiredByBlow = nValue; };

	DnActorHandle GetActor( void ) { return m_hActor; };
	void SetHasActor( DnActorHandle hActor );
	void UpdateRangeChecker( int iSkillLevelDataApplyType );

	int GetClassID( void ) const { return m_iSkillID; };
	int GetSkillLevelID( void ) const { return m_iSkillLevelID; }
	
	void SetClassID(int nNewSkill)  { m_iSkillID = nNewSkill; };
	int GetApplyType() { return m_iSelectedSkillLevelDataApplyType; };

	//--------------------------------------------------------------
	//[debug_skill]
	int GetSkillMaxUseNum( void ) { return m_nMaxUseNum; }
	int GetSkillNowNum( void ) { return m_nNowNum; }

	int SetSkillMaxUseNum( int nMaxUseNum ) { return m_nMaxUseNum = nMaxUseNum; }
	int SetSkillNowNum( int nNowNum ) { return m_nNowNum = nNowNum; }

	//--------------------------------------------------------------

	bool AddUsableCheckers( IDnSkillUsableChecker* pUsableChecker, int iSkillLevelDataType );
	bool AddProcessor( IDnSkillProcessor* pProcessor, int iSkillLevelDataType );
	
	// 각종 쿼리 함수들
	SkillTypeEnum GetSkillType( void ) { return m_eSkillType; };
	DurationTypeEnum GetDurationType( void ) { return m_eDurationType; };
	TargetTypeEnum GetTargetType( void ) { return m_eTargetType; };


	void SetDurationType(DurationTypeEnum eType) { m_eDurationType = eType; };
	void SetSkillType(SkillTypeEnum eType) { m_eSkillType = eType; };


	DWORD GetStateEffectCount( void );
	StateEffectStruct *GetStateEffectFromIndex( DWORD dwIndex );
	
	int GetIncreaseRange( void ) { return m_iIncreaseRange[ m_iSelectedSkillLevelDataApplyType ]; };
	int GetDecreaseHP( void ) { return m_iNeedHP[ m_iSelectedSkillLevelDataApplyType ]; };
	int GetDecreaseMP( void ) { return m_iNeedMP[ m_iSelectedSkillLevelDataApplyType ]; };
	
	// 이 함수들은 UI 에서만 쓰입니다...
	TCHAR* GetName( void ) const { return const_cast<TCHAR*>(m_strName.c_str()); };
	int GetLevel( void ) const { return m_iLevel; };
	int GetMaxLevel( void ) { return m_iMaxLevel; };
	int GetNextLevelSkillPoint( void ) { return m_iNextLevelSkillPoint; };
	//int GetBuySkillPoint( void ) { return m_iFirstLevelSkillPoint; };
	void SetUsePoint( int nPoint ) { m_iUsePoint = nPoint; }
	int GetUsePoint() { return m_iUsePoint; }
	void CalcLevelUpUsePoint( int iLevelCount );
	void ResetLevelUpUsePoint( int iLevelCount );
	int GetLevelLimit( void ) { return m_iLevelLimit; }
	int GetFirstLevelLimit( void ) { return m_iFirstLevelLimit; }

	int GetExplainStringID( void ) { return m_iSkillExplainStringID; };
	int GetNextExplainStringID( void ) { return m_iNextSkillExplainStringID; };
	const TCHAR *GetNextExplainString( void ) { return m_strNextSkillExplainString[ m_iSelectedSkillLevelDataApplyType ].c_str(); }
	// from MIInventoryItem
	const TCHAR* GetDescription( void ) { return m_strSkillExplainString[ m_iSelectedSkillLevelDataApplyType ].c_str(); };
	
	int GetNeedItemID( void ) { return m_iNeedItemID[ m_iSelectedSkillLevelDataApplyType ]; };
	int GetNeedItemDecreaseCount( void ) { return m_iNeedItemDecreaseCount[ m_iSelectedSkillLevelDataApplyType ]; };
	int GetNeedJobClassID( void ) { return m_iNeedJobClassID; };
	int GetNeedEquipType( int iIndex ) 
	{ 
		_ASSERT( 0 <= iIndex && iIndex < 2 );
		if( 0 <= iIndex && iIndex < 2 )
			return m_aeNeedEquipType[ iIndex ]; 
		else
			return CDnWeapon::EquipTypeEnum_Amount;
	};

	int GetCPScore( void ) { return m_iCPScore; }
	
	int GetBuffIconImageIndex( void ) { return m_iBuffIconImageIndex; };
#ifdef PRE_ADD_SKILL_ADDTIONAL_BUFF
	int GetAdditionalIconImageIndex( void ) { return m_iDebuffIconImageIndx; };
#endif
	
	bool CanDissolve( void ) { return (m_iDissolvable == 0) ? true : false; }; 
	
	int GetMaxDuplicateCount( void ) { return m_iDuplicateCount; };
	int GetDuplicateMethod( void ) { return m_iSkillDuplicateMethod; };
	int	GetEffectDuplicateMethod( void ) { return m_iEffectDuplicateMethod; };

	float GetHPConsumeType( void ) { return m_fHPConsumeType[ m_iSelectedSkillLevelDataApplyType ]; };
	float GetMPConsumeType( void ) { return m_fMPConsumeType[ m_iSelectedSkillLevelDataApplyType ]; };

	CDnState::ElementEnum GetElement( void ) { return m_eElement; };

	virtual bool Initialize( int iSkillTableID, int iLevel );

	bool IsSatisfyWeapon( void );
	UsingResult CanExecute( void );
	bool IsFinished( void );

	void OnBeginCoolTime();
	void OnBegin( LOCAL_TIME LocalTime, float fDelta );
	void OnBeginForceSync( LOCAL_TIME LocalTime, float fDelta );
	void ProcessExecute( LOCAL_TIME LocalTime, float fDelta );
	void OnEnd( LOCAL_TIME LocalTime, float fDelta );

	// 쿨 타임 등등을 처리
	void Process( LOCAL_TIME LocalTime, float fDelta );

	void EnableToggle( bool bEnable ) { m_bNowToggled = bEnable; };
	bool IsToggleOn( void ) { return m_bNowToggled; };
	bool IsEanbleActiveToggleCoolTime(){ return m_bEnableActiveToggleCoolTime; }
	void EnableActiveToggleCoolTime(bool bEnable){ m_bEnableActiveToggleCoolTime = bEnable; }

	void EnableAura( bool bEnable ) { m_bNowAura = bEnable; };
	bool IsAuraOn( void ) { return m_bNowAura; };

	IDnSkillUsableChecker* GetChecker( int iType );
	IDnSkillProcessor* GetProcessor( int iType );

	bool IsUseCheckerType( int iCheckerType );
	bool IsUseProcessorType( int iProcessorType );

	int GetNumChecker( void ) { return (int)m_vlpUsableCheckers[ m_iSelectedSkillLevelDataApplyType ].size(); };
	int GetNumProcessor( void ) { return (int)m_vlpProcessors[ m_iSelectedSkillLevelDataApplyType ].size(); };

	// from MIInventoryItem
	virtual float GetCoolTime() { return m_fCoolTime; };
	virtual void SetForceCoolTime( float fCoomTime ) { m_fCoolTime = fCoomTime; }
	virtual ItemConditionEnum GetItemCondition();
	///////////////////////////////////////////////////////////////////
	void ResetCoolTime( void ) 
	{ 
		m_fCoolTime = 0.0f; m_fLeftDelayTime = 0.0f;  

		// 같은 글로벌 ID 를 사용하는 다른 글로벌 스킬을 사용하여 셋팅된 쿨타임 값을 실제로 이 스킬 사용될 때는 제거한다.
		m_fAnotherGlobalSkillCoolTime = 0.0f;

		m_nAnotherGlobakSkillID = 0;

#if defined(PRE_ADD_TOTAL_LEVEL_SKILL)
		m_fDeltaGlobalCoolTime = 0.0f;
#endif // PRE_ADD_TOTAL_LEVEL_SKILL
	}
	void AsItemSkill( void ) { m_bItemSkill = true; };
	bool IsItemSkill( void ) { return m_bItemSkill; };

	const SkillInfo* GetInfo() const { return &m_SkillInfo[ m_iSelectedSkillLevelDataApplyType ]; };

	float GetDelayTime( void );
	void SetElapsedDelayTime( float fValue ) { m_fLeftDelayTime = fValue; }
	float GetElapsedDelayTime() { return m_fLeftDelayTime; }

	bool HasCoolTime() {return (m_fDelayTime[ m_iSelectedSkillLevelDataApplyType ] != 0.f); }

	void SetSkillLevelIDOffset( int iSkillLevelIDOffset, int iSelectedSkillLevelDataType ) { m_SkillInfo[ iSelectedSkillLevelDataType ].iSkillLevelIDOffset = iSkillLevelIDOffset; };

	float GetPassiveSkillLength(){ return m_fPassiveActionSkillLength; }
	void SetPassiveSkillLength( float fTimeLength ) { m_fPassiveActionSkillLength = fTimeLength; };
	void SetPassiveSkillActionName( const char* pActionName );

	void RefreshDecreaseMP( int iSkillLevelDataApplyType = NUM_SKILLLEVEL_APPLY_TYPE );
	bool IsUseActionNames( const set<string>& setUseActionNames );

	// 한기 2009.7.27 
	// 자신에게 사용하는 효과 적용 시간 관련 (#1911)
	void AddSignalApplyNoPacketStateEffectHandle( DnBlowHandle hBlow ) { m_vlAppliedNoPacketInstantBlow.push_back(hBlow); };
	//

	void SetOnceCoolTime( float fDelayTime, float fElapsedDelayTime );
	void AsEquipItemSkill( void ) { m_bEquipItemSkill = true; };
	bool IsEquipItemSkill( void ) { return m_bEquipItemSkill; };

	int GetUnlockSkillBookItemID( void ) { return m_iUnlockSkillBookItemID; };

	// 액션 재생 프로세서가 반드시 있는 액티브 스킬의 상태효과 계승 요청! (STE_Input 시그널 처리시 호출됨)
	void OnChainInput( const char* pActionName );

	void SetNodeRenderInfo( CDnSkillTreeSystem::S_NODE_RENDER_INFO& NodeRenderInfo ) { m_NodeRenderInfo = NodeRenderInfo; };
	const CDnSkillTreeSystem::S_NODE_RENDER_INFO& GetNodeRenderInfo( void ) { return m_NodeRenderInfo; };

	int GetNeedAcquireSkillPoint( void ) { return m_iFirstLevelSkillPoint; };
	int GetUnlockPrice( void ) { return m_iUnlockPrice; };

	bool IsEnableTooltip() { return m_bEnableTooltip; }
	// 새로 얻은 것인가.
	bool IsNewAcquire( void ) { return m_bNewAcquired; };
	void SetNewAcquire( void ) { m_bNewAcquired = true; };
	void ResetNewAcquire( void ) { m_bNewAcquired = false; };

	// 패시브 버프 스킬인 경우 현재 적용되고 있는지 여부. 
	// 현재 기준은 대상무기 장착중인지 여부임.
	void SetAppliedPassiveBlows( bool bAppliedPassiveBlows ) { m_bAppliedPassiveSelfBlows = bAppliedPassiveBlows; };
	bool IsAppliedPassiveBlows( void ) { return m_bAppliedPassiveSelfBlows; };

	void AddGlyphStateEffect( int nGlyphID );
	void DelGlyphStateEffect( int nGlyphID );

	void SetGlyphActiveSkillDisable( bool bActiveSkillDisable );
	bool GetGlyphActiveSkillDisable();

	void SetResetSkill( bool bReset = true )	{ m_bResetSkill = bReset; }

	int GetExclusiveID( void ) { return m_iExclusiveID; };

	void CheckProcessorOnBegin( void );
	void CheckChangeActionStrByBubbleProcessor();
	void CheckProcessorOnEnd( void );

	// 임시로 얻은 스킬을 위한 함수들..
	void AsTempSkill( void ) { m_bTempSkill = true; };
	bool IsTempSkill( void ) { return m_bTempSkill; };
	void SetLevelLimit( int iLevelLimit ) { m_iLevelLimit = iLevelLimit; };
	void SetNeedJobClassID( int iJobID ) { m_iNeedJobClassID = iJobID; };
	//////////////////////////////////////////////////////////////////////////

	// 스킬 레벨 데이터 값 셋팅. 조건에 맞지 않으면 셋팅 안된다.
	bool SelectLevelDataType( int iSkillLevelDataType, bool bForce = false );
	int GetSelectedLevelDataType( void ) { return m_iSelectedSkillLevelDataApplyType; };

	const set<string>& GetUseActionSet( void ) { return m_setUseActionNames; };

	bool operator < ( const DnSkillHandle hSkill )
	{
		if( m_iFirstLevelLimit < hSkill->GetFirstLevelLimit() )
			return true;
		else
		if( m_iFirstLevelLimit == hSkill->GetFirstLevelLimit() )
			return (m_strName < hSkill->GetName());
		else
			return false;
	};

	// 강화 패시브 스킬 타입 (EnchantPassive) 인 경우 기반이 되는 스킬 아이디를 얻어옴.
	int GetBaseSkillID( void );

	// 강화 패시브 스킬 값을 적용시킴.
	// 강화 패시브 스킬 아이디, 데이터 등은 따로 구조체에 담아서 가지고 있는다.
	// 한번 적용되면 같은 강화 패시브 스킬 ID 는 적용되지 않도록 한다.
	bool ApplyEnchantSkill( DnSkillHandle hEnchantPassiveSkill );
	bool CheckAndUnifyStateEffectArgument( int iSkillLevelApplyType, StateEffectStruct* pEnchantSkillSE );
	void ReleaseEnchantSkill( void );
	bool CheckAndDivideStateEffectArgument( StateEffectStruct* pDestSkillSE );
	bool IsEnchantedSkill( void ) { return (0 != m_iAppliedEnchantPassiveSkillID); };
	int GetEnchantedSkillID( void ) { return m_iAppliedEnchantPassiveSkillID; };

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

public:
	bool CheckAnimation();

protected:
	bool IsFinishedAuraSkill();

public:
	void SetCreatedByItem(bool bFlag) { m_isCreatedByItem = bFlag; }
	bool IsCreatedByItem() { return m_isCreatedByItem; }

	void SetItemPrefixSkill() { m_SkillInfo[ PVP ].bItemPrefixSkill = true; m_SkillInfo[ PVE ].bItemPrefixSkill = true; };
	bool IsItemPrefixSkill() { return m_SkillInfo[ m_iSelectedSkillLevelDataApplyType ].bItemPrefixSkill; };


protected:
	int m_nLevelUpValue;
public:
	void SetLevelUpValue(int nLevelUp) { m_nLevelUpValue = nLevelUp; }
	int GetLevelUpValue() { return m_nLevelUpValue; }

	int GetGlobalSkillGroupID( void ) { return m_iGlobalSkillGroupID; };
	void OnAnotherGlobalSkillBeginCoolTime( DnSkillHandle hSkill );

protected:
	int m_SummonMonsterID;

	void OnInitializeSummonMonsterInfo();				//소환 몬스터 ID정보를 스킬 액션 시그널에서 찾는다.
	DnActorHandle FindSummonMonster(int nMonsterID);	//설정된 몬스터가 소환 되어 있는지 확인한다.

public:
	int GetSummonMonsterID() { return m_SummonMonsterID; }
	bool SummonMonsterOff();				//소혼 몬스터 소환 해제 시킨다.

	bool IsUseInVillage() { return !m_bCantUseInVillage; }


#if defined(PRE_FIX_45899)
protected:
	bool m_isDisableSkillByItemMove;		//아이템 이동요청후 완료 패킷이 오기 전까지 스킬 사용을 중지..
public:
	void DisableSkillByItemMove(bool bDisable) { m_isDisableSkillByItemMove = bDisable; }
#endif // PRE_FIX_45899

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
	string m_BackupDebuffEffectOutputIDs[NUM_SKILLLEVEL_APPLY_TYPE];
public:
	string& GetEffectOutputIDs(int iSkillLevelDataApplyType) { return m_SkillInfo[iSkillLevelDataApplyType].effectOutputIDs; }
	string& GetDebuffEffectOutputIDs(int iSkillLevelDataApplyType) { return m_SkillInfo[iSkillLevelDataApplyType].debuffEffectOutputIDs; }

	string& GetBackupEffectOutputIDs(int iSkillLevelDataApplyType) { return m_BackupEffectOutputIDs[iSkillLevelDataApplyType]; }
	string& GetBackupDebuffEffectOutputIDs(int iSkillLevelDataApplyType) { return m_BackupDebuffEffectOutputIDs[iSkillLevelDataApplyType]; }

	int GetAnotherGlobalSkillID() { return m_nAnotherGlobakSkillID; }
};