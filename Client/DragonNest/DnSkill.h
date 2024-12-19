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


// ��ų ��ü
class CDnSkill : public CSmartPtrBase< CDnSkill >, public MIInventoryItem
{
public:
	CDnSkill( DnActorHandle hActor );
	virtual ~CDnSkill(void);


	enum SkillTypeEnum 	{
		Active,
		Passive,
		AutoPassive,
		EnchantPassive,					// 2�� ���� ��ų���� ���� ���� ���� ��ų ��ȭ ��ų Ÿ��.
		AutoActive,						// ��Ÿ�Ӹ��� �ڵ����� ���Ǵ� ��ų
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
		���� ��ȯ ��ų�� UIǥ�ô� Toggle�� ǥ�� �ǵ��� �ϰ�, ���� ��ų ����� Instanly�� ���� ó���ǵ�����.
		OnBegin���� EnableToggle ��� Ȱ��ȭ ��Ű��, ���� ��ȯ ���� �������� EnableToggle ��Ȱ��ȭ ��Ŵ.
		��ų ��� �������� ��ų ����� ���� �ϴ��� ��ȯ ���Ͱ� ������ ��ȯ ������ �ϰ� ��ų ����� �ȵ�.
		��ų ��� ���нÿ��� ��ȯ ���Ͱ� ������ ��ȯ ���� ��ȯ ���� ��Ŵ.
		*/
		SummonOnOff,					//��ȯ ���� ��ȯ On/Off�� ��ų(�������� ��ȯ, ��ȯ����)
		StanceChange,
		TotalLevelSkill,				// ���� ���� ��ų
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

		// ���� ���� ������ TargetType �� All �� ��쿡�� ��ȿ��.
		ApplyEnemy,
		ApplyFriend,
	};

	// ��ų ��� ���� ������ �ʿ��Ѵ�� �߰�.
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

	// ���� pvp/pve ���� ����. ���� ���̺�� ������ ��. PVE:0, PVP:1
	enum Mode
	{
		PVE = 0,
		PVP,
		NUM_SKILLLEVEL_APPLY_TYPE,
	};

	// #31013 ���̺� ���ڵ� �ϳ� �� ����ȿ�� ���� ���� �ִ� ����.
	enum MAX_DATA_COUNT
	{	
		MAX_PROCESSOR_COUNT = 5,
		MAX_STATE_EFFECT_COUNT = 7,
		MAX_GLYPH_STATE_EFFECT_COUNT = 5,
	};

	// ���� �̻� �ߵ� ���� ������
	struct StateEffectStruct
	{
		int nID;
		StateEffectApplyType ApplyType;
		int nDurationTime;
		string szValue;
		int nGlyphID;
		int nFromEnchantPassiveSkillID;			// ��ȭ �нú� ��ų ID �� ���� ���ٿ��� ����ȿ����� �ش� ��ų�� ���̵� ��� �ִ´�.

		// #40643 ������EX ó�� Ư���ϰ� ������ ����ȿ���� ���ڸ� �ǵ帮�� ���..
		// ��¿ �� ���� ������� �Ѵ�.
		// �� �÷��װ� true �̸� �̹� ������ �ִ� ����ȿ���� ������ �����̱� ������ ���� ����ȿ�� ������ �� �������� �ʴ´�.
		string strEnchantSkillSEParam;

		// #52905 ��ø �߰� ȿ�� ���� 
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

		// CS_CMDADDSTATEEFFECT ��Ŷ���� ������� ��ų ������ ���� �� ������.
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
		
		// ������ ���ξ� �ý��ۿ��� ����ϴ� ��ų. ���������� ���� ���� ���õǸ� Ŭ���̾�Ʈ�δ� SC_CMDADDSTATEEFFECT ��Ŷ���� ���õȴ�.
		// �� �÷��װ� ���� �ִ� ���ξ� ��ų�� ����ȿ������ Duplicate ó���� �ϸ� �ȵ�. ��Ŷ���ڸ��� ��ٷ� begin ��.
		bool				bItemPrefixSkill;

		// ����� ��ų(���� ����) �鿪 ó�� ���ø� ���� �߰� [2011/04/14 semozz]
		bool				bIgnoreImmune;

#if defined(PRE_FIX_NEXTSKILLINFO)
		int					nSkillLevel;	//��ų���� ����
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

	bool m_bLock;			// ���� ��ų���� ������� ���� �� ��������.
	bool m_bAcquired;		// ��ų ȹ�� ��������. 

	bool m_bAcquiredByBlow;	//rlkt_ blow_345

	// ���� pvp/pve ����������
	int m_iSelectedSkillLevelDataApplyType;

	SkillTypeEnum				m_eSkillType;

	int							m_iSkillID;
	int							m_iSkillLevelID;
	int							m_iNextLevelSkillPoint;		// ���� ��ų �������� �ϱ� ���� �ʿ��� ��ų ����Ʈ. 
	int							m_iFirstLevelSkillPoint;	// ù��° ������ ��ų ����Ʈ
	tstring						m_strName;
	string						m_strStaticName;
	tstring						m_strSkillExplainString[ NUM_SKILLLEVEL_APPLY_TYPE ];
	int							m_iSkillExplainStringID;		// ��ų ���� ���ڿ� ���̵�. CEtUIXML::GetUIString() �Լ��� ���� �� ����ϴ� ID
	int							m_iNextSkillExplainStringID;	// ���� ��ų ���� ���ڿ� ���̵�. 
	tstring						m_strNextSkillExplainString[ NUM_SKILLLEVEL_APPLY_TYPE ];

	DurationTypeEnum			m_eDurationType;
	TargetTypeEnum				m_eTargetType;

	int							m_iNeedJobClassID;
	CDnWeapon::EquipTypeEnum	m_aeNeedEquipType[ MAX_SKILL_NEED_EQUIP_COUNT ];
	int							m_iNeedItemID[ NUM_SKILLLEVEL_APPLY_TYPE ];
	int							m_iNeedItemDecreaseCount[ NUM_SKILLLEVEL_APPLY_TYPE ];

	int							m_iDissolvable;					// ���� ���� ����. 0: ��������, 1: ���� �Ұ�
	int							m_iDuplicateCount;				// ��ų �ߺ� Ƚ�� 
	int							m_iSkillDuplicateMethod;		// ����ȿ�� ����
	int							m_iEffectDuplicateMethod;		// ����Ʈ ��� ����

	float						m_fHPConsumeType[ NUM_SKILLLEVEL_APPLY_TYPE ];
	float						m_fMPConsumeType[ NUM_SKILLLEVEL_APPLY_TYPE ];
	int							m_iNeedHP[ NUM_SKILLLEVEL_APPLY_TYPE ];
	int							m_iNeedMP[ NUM_SKILLLEVEL_APPLY_TYPE ];
	int							m_iOriginalNeedMP[ NUM_SKILLLEVEL_APPLY_TYPE ];	//���� ��ųȿ���� ���� �Ǳ����� MP�� �����
	int							m_iCPScore;

	int							m_iLevel;
	int							m_iMaxLevel;
	int							m_iUsePoint;
	int							m_iLevelLimit;
	int							m_iFirstLevelLimit;		// ��ų �����ϱ� ���� �ּ� ����(1���� ��ų�� �ʿ䷹��)
	int							m_iIncreaseRange[ NUM_SKILLLEVEL_APPLY_TYPE ];		// �߰� ��Ÿ�
	int							m_iDecreaseHP[ NUM_SKILLLEVEL_APPLY_TYPE ];
	int							m_iDecreaseMP[ NUM_SKILLLEVEL_APPLY_TYPE ]; 

	float						m_fOriginalDelayTime[ NUM_SKILLLEVEL_APPLY_TYPE ];	//���� ��ųȿ���� ���� �Ǳ����� ��Ÿ���� �����
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

	int							m_iBaseSkillID;	// �нú� ��ȭ ��ų�� ��쿡�� ��ȿ��. ��ȭ ��ų ����� ��ų ID.
	int							m_iAppliedEnchantPassiveSkillID;
	vector<IDnSkillProcessor*> m_vlpProcessorBackup[ NUM_SKILLLEVEL_APPLY_TYPE ];

	LOCAL_TIME					m_LastTimeToggleMPDecreaseTime;
	bool						m_bFinished;

	// ���, ���� ��ų�� ��� ��� �� ��������.
	bool						m_bNowToggled;
	bool						m_bNowAura;

	bool						m_bCantUseInVillage;

	bool                        m_bEnableActiveToggleCoolTime;
	
	vector<StateEffectStruct>		m_vlStateEffectList[ NUM_SKILLLEVEL_APPLY_TYPE ];
	vector< pair<int, int> >		m_vlStateDurationList[ NUM_SKILLLEVEL_APPLY_TYPE ];

	// ��ų �ߵ� ���� ����Ʈ
	vector<IDnSkillUsableChecker*>	m_vlpUsableCheckers[ NUM_SKILLLEVEL_APPLY_TYPE ];

	// ��ų ���μ��� ����Ʈ.. ���� �̻��̶���� ��Ÿ ����� ó���ؾ� �ϴ� ��Ȳ���� �߻�ȭ.
	vector<IDnSkillProcessor*>		m_vlpProcessors[ NUM_SKILLLEVEL_APPLY_TYPE ];

	bool							m_bItemSkill;							// �� ��ų, ������ ��ų�ΰ�.

	// Ŭ���̾�Ʈ������ ���� ������� ����. �ܺο� �������� ��ų�� �ٽ� ������.
	// �Ź� TableDB���� �������� ���ŷο��� ����.
	SkillInfo						m_SkillInfo[ NUM_SKILLLEVEL_APPLY_TYPE ];

	// �нú� �׼� ��ų�� ���� ���õ� ���ӽð�
	float							m_fPassiveActionSkillLength;
	vector<DnBlowHandle>			m_vlAppliedNoPacketInstantBlow;			// �������� ��Ŷ�� �������� �ʰ� ���� �Ǵ��ϴ� ����ȿ�� �߿� ���ӽð��� ���� Instant �� ���
	CDnState::ElementEnum			m_eElement;

	// �� ��ų�� Play Ani �� ����Ѵٸ� ä�����ԵǴ� �ش� �׼�
	string							m_strAction;

	set<string>						m_setUseActionNames;

	// ��Ÿ�� �ѹ��� ������ ���� ��Ÿ�� ���� ����Ѵ�. �������� ��ų ������ ���� ������ ���� ��� �������� ��Ÿ���� ����޴´�.
	float m_fOnceDelayTime;
	float m_fOnceElapsedDelayTime;

	// ��� ������ ��ų�ΰ�
	bool m_bEquipItemSkill;

	// ��ųƮ�� ��� ������ ����
	CDnSkillTreeSystem::S_NODE_RENDER_INFO m_NodeRenderInfo;

	// ��Ÿ�� ���� ����ȿ���� ���� ��ų �� ��ÿ� �����ͼ� �����صд�. 
	float m_fCoolTimeAdjustBlowValue;

	// ���� ���� ���� true
	bool m_bNewAcquired;

	bool m_bAppliedPassiveSelfBlows;

	bool m_bResetSkill;
	std::vector<GlyphEffectStruct> m_vGlyphEffectList;

	int m_iExclusiveID;				// ��ų ���̺� �ִ� ���� ��� �� ���� ��ų���� ������ ���� id

	bool m_bTempSkill;				// �ӽ÷� ������ ��ų #26902

	bool m_bEnableTooltip;

	bool m_bGlyphActiveSkillDisbale;

	// �����ۿ� ���� ������ ��ų���� Ȯ�ο�(m_bItemSkill���� �ٸ��뵵) [2011/03/16 semozz]
	bool m_isCreatedByItem;

	int m_iGlobalSkillGroupID;
	float m_afGlobalCoolTime[ NUM_SKILLLEVEL_APPLY_TYPE ];	// ���̺��� ms ����. ���� ���� �� �� ������ ��ȯ.
	float m_fAnotherGlobalSkillCoolTime;					// �ٸ� �۷ι� ��ų�� ��ų�� ������� ��� �ش� ��ų�� ��Ÿ������ �ѹ� �����ش�.
	int m_iAnotherGlobalSkillLevelDataType;
	int m_nAnotherGlobakSkillID;

protected:
	// �������� ���� ��Ŷ�� �������� �ʴ� StateEffect �� üũ. ȭ�鿡 �ѷ��� ����Ʈ�� �����Ѵ�.
	//void _CheckNoPacketSelfStateEffect( void );	
	void _OnBeginProcessException( void );

	bool _LoadSkillLevelData( int iSkillTableID, int iLevel, int iSkillLevelDataApplyType );
	void _RefreshDecreaseMP( int iSkillLevelDataApplyType );

	// input �ñ׳η� ���õ� �׼ǵ� ��� �߰���.
	void _AddUsedActions( const char* pActionName );

public:

	bool bIsExtremitySkill(){ return m_iExclusiveID>0; }	// �ñؽ�ų�̳�???

	// ��ų ������ ������ ����
	static bool ValidateSkillData( int iSkillTableID, int iLevel );

	// ��ų ���� ���丮 �Լ�. ���� ���� ��ų�� ���� 0���� �����˴ϴ�.
	static DnSkillHandle CreateSkill( DnActorHandle hActor, int iSkillTableID, int iLevel );

	// ��� ������ �� ��ų�� ���� ������ �����ϰ� �� ���Ŀ��� �Ұ���.
	void Lock( void ) { m_bLock = true; m_bAcquired = false; };
	void UnLock( void ) { _ASSERT( 1 == m_iLevel); m_bLock = false; };		// Ŭ�󿡼� �����´� �ʱ� ���� 1������.
	bool IsLocked( void ) { return m_bLock; };

	// ����� ��ų�� ó������ ȹ����. ȹ���� �� �ݵ�� ��� ���¿��� ��.
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
	
	// ���� ���� �Լ���
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
	
	// �� �Լ����� UI ������ ���Դϴ�...
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

	// �� Ÿ�� ����� ó��
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

		// ���� �۷ι� ID �� ����ϴ� �ٸ� �۷ι� ��ų�� ����Ͽ� ���õ� ��Ÿ�� ���� ������ �� ��ų ���� ���� �����Ѵ�.
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

	// �ѱ� 2009.7.27 
	// �ڽſ��� ����ϴ� ȿ�� ���� �ð� ���� (#1911)
	void AddSignalApplyNoPacketStateEffectHandle( DnBlowHandle hBlow ) { m_vlAppliedNoPacketInstantBlow.push_back(hBlow); };
	//

	void SetOnceCoolTime( float fDelayTime, float fElapsedDelayTime );
	void AsEquipItemSkill( void ) { m_bEquipItemSkill = true; };
	bool IsEquipItemSkill( void ) { return m_bEquipItemSkill; };

	int GetUnlockSkillBookItemID( void ) { return m_iUnlockSkillBookItemID; };

	// �׼� ��� ���μ����� �ݵ�� �ִ� ��Ƽ�� ��ų�� ����ȿ�� ��� ��û! (STE_Input �ñ׳� ó���� ȣ���)
	void OnChainInput( const char* pActionName );

	void SetNodeRenderInfo( CDnSkillTreeSystem::S_NODE_RENDER_INFO& NodeRenderInfo ) { m_NodeRenderInfo = NodeRenderInfo; };
	const CDnSkillTreeSystem::S_NODE_RENDER_INFO& GetNodeRenderInfo( void ) { return m_NodeRenderInfo; };

	int GetNeedAcquireSkillPoint( void ) { return m_iFirstLevelSkillPoint; };
	int GetUnlockPrice( void ) { return m_iUnlockPrice; };

	bool IsEnableTooltip() { return m_bEnableTooltip; }
	// ���� ���� ���ΰ�.
	bool IsNewAcquire( void ) { return m_bNewAcquired; };
	void SetNewAcquire( void ) { m_bNewAcquired = true; };
	void ResetNewAcquire( void ) { m_bNewAcquired = false; };

	// �нú� ���� ��ų�� ��� ���� ����ǰ� �ִ��� ����. 
	// ���� ������ ��󹫱� ���������� ������.
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

	// �ӽ÷� ���� ��ų�� ���� �Լ���..
	void AsTempSkill( void ) { m_bTempSkill = true; };
	bool IsTempSkill( void ) { return m_bTempSkill; };
	void SetLevelLimit( int iLevelLimit ) { m_iLevelLimit = iLevelLimit; };
	void SetNeedJobClassID( int iJobID ) { m_iNeedJobClassID = iJobID; };
	//////////////////////////////////////////////////////////////////////////

	// ��ų ���� ������ �� ����. ���ǿ� ���� ������ ���� �ȵȴ�.
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

	// ��ȭ �нú� ��ų Ÿ�� (EnchantPassive) �� ��� ����� �Ǵ� ��ų ���̵� ����.
	int GetBaseSkillID( void );

	// ��ȭ �нú� ��ų ���� �����Ŵ.
	// ��ȭ �нú� ��ų ���̵�, ������ ���� ���� ����ü�� ��Ƽ� ������ �ִ´�.
	// �ѹ� ����Ǹ� ���� ��ȭ �нú� ��ų ID �� ������� �ʵ��� �Ѵ�.
	bool ApplyEnchantSkill( DnSkillHandle hEnchantPassiveSkill );
	bool CheckAndUnifyStateEffectArgument( int iSkillLevelApplyType, StateEffectStruct* pEnchantSkillSE );
	void ReleaseEnchantSkill( void );
	bool CheckAndDivideStateEffectArgument( StateEffectStruct* pDestSkillSE );
	bool IsEnchantedSkill( void ) { return (0 != m_iAppliedEnchantPassiveSkillID); };
	int GetEnchantedSkillID( void ) { return m_iAppliedEnchantPassiveSkillID; };

protected:
	float m_fCoolTimeMultipier;	//��Ŭ ��Ÿ�ӿ� ���� ��Ÿ�� ������
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

	void OnInitializeSummonMonsterInfo();				//��ȯ ���� ID������ ��ų �׼� �ñ׳ο��� ã�´�.
	DnActorHandle FindSummonMonster(int nMonsterID);	//������ ���Ͱ� ��ȯ �Ǿ� �ִ��� Ȯ���Ѵ�.

public:
	int GetSummonMonsterID() { return m_SummonMonsterID; }
	bool SummonMonsterOff();				//��ȥ ���� ��ȯ ���� ��Ų��.

	bool IsUseInVillage() { return !m_bCantUseInVillage; }


#if defined(PRE_FIX_45899)
protected:
	bool m_isDisableSkillByItemMove;		//������ �̵���û�� �Ϸ� ��Ŷ�� ���� ������ ��ų ����� ����..
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