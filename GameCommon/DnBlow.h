#pragma once
#include "DnBlowDef.h"
#include "DnDamageBase.h"
#include "DnActor.h"
#ifdef PRE_FIX_MEMOPT_SIGNALH
#include "DnCommonUtil.h"
#endif

#ifndef _GAMESERVER
// 이펙트 출력 방식 데이터
struct EffectOutputInfo
{
	int iStateEffectIndex;
	int iShowTimingType;
	int iOutputType;
	int iPlayType;
	string strSkinFileName;
	string strAniFileName;
	string strActFileName;
	string strActorActionName;
	int iUseDiffuseVariationMethod;
	float afDiffuseRed[ 2 ];
	float afDiffuseGreen[ 2 ];
	float afDiffuseBlue[ 2 ];
	float fDiffuseChangeSpeed;
	vector<int> vlDummyBoneIndices;
	int iEffectFileDataIndex;			// 데이터를 한 번 읽어놓고 계속 사용하는 이펙트 인덱스

	// 발동 타이밍
	enum
	{
		ALL_TIME,
		OWNER_HANDLING,
	};

	// 재생 타입
	enum
	{
		REPEAT,
		ONCE,
		PART_REPEAT,
	};

	// 이펙트 출력 타입
	enum
	{
		ATTACH,
		DO_ACTION,
		DUMMY_BONE_ATTACH,
	};

	// StateEffectTable 상의 디폴트 내용과 일치해야 함
	EffectOutputInfo( void ) : iStateEffectIndex( 0 ), iShowTimingType( ALL_TIME ), iOutputType( ATTACH ), iPlayType( REPEAT ),
		iUseDiffuseVariationMethod( 0 ), fDiffuseChangeSpeed( 1.0f ), iEffectFileDataIndex( -1 )
	{
		memset( afDiffuseRed, 1, sizeof(afDiffuseRed) );
		memset( afDiffuseBlue, 1, sizeof(afDiffuseBlue) );
		memset( afDiffuseGreen, 1, sizeof(afDiffuseGreen) );
	};

	const char* GetSkinFileName( void ) const { return strSkinFileName.empty() ? NULL : strSkinFileName.c_str(); };
	const char* GetAniFileName( void ) const { return strAniFileName.empty() ? NULL : strAniFileName.c_str(); };
	const char* GetActFileName( void ) const { return strActFileName.empty() ? NULL : strActFileName.c_str(); };
	const char* GetActorActFileName( void ) const { return strActorActionName.empty() ? NULL : strSkinFileName.c_str(); };
};
#endif

#ifdef _GAMESERVER
class CDnBlow : public CMultiSmartPtrBase< CDnBlow, MAX_SESSION_COUNT >
#else
class CDnBlow : public CSmartPtrBase< CDnBlow >
#endif
{

public:
	CDnBlow( DnActorHandle hActor );
	virtual ~CDnBlow();

protected:
	
	LOCAL_TIME m_LastLocalTime;
	DnActorHandle m_hActor;

	STATE_BLOW m_StateBlow;
	STATE_BLOW::emBLOW_INDEX m_eOriginalBlowIndex;
	
	bool m_bPermanentBlow;
	bool m_bIgnoreEffectAction;			// 상태효과 이펙트 테이블에 있는 액션 실행을 무시한다. 보스급에 적용되는 결빙 상태효과에서 사용됨.

	DWORD m_dwCallBackType;

	int m_nBlowID;
	int m_nThreatAggro;
	
	// 각각 스킬 테이블, 스킬 레벨 테이들의 ID 를 의미함 (ClassID)
	bool m_bHasParentSkill;				// 스킬로부터 비롯된 상태효과인가.
	CDnSkill::SkillInfo	m_ParentSkillInfo;

	bool m_bDuplicated;		// 중첩된 상태효과인지
	int m_iDuplicateCount;
	
	float m_fOriginalDurationTime;
	float m_fValue;

	bool m_bEternity;
	
	bool m_bGuildBlow;			//생성된 상태효과가 길드스킬에 의하여 사용된건지

	bool m_bFromSourceItem;
#ifdef _GAMESERVER
	bool m_bHPMPFullWhenBegin;
#endif

#ifndef _GAMESERVER
	const EffectOutputInfo* m_pEffectOutputInfo;
	DnEtcHandle m_hEtcObjectEffect;
	DNVector(DnEtcHandle) m_vlhDummyBoneEtcObjects;		// 더미 본에 붙게 되는 FX 이펙트임
	bool m_bUseTableDefinedGraphicEffect;
	float m_fAccumTime;
	bool m_bAllowDiffuseVariation;		// 바깥에서 Variation 숫자가 제일 높은 거 하나만 출력토록 되어있다.

	// 서버에서 생성된 상태효과의 ID 추후에. SC_CMDREMOVESTATEEFFECTFROMID 패킷으로 서버에서 날아온 아이디로
	// 삭제하기 위해 상태효과 생성될 시에 받아둠.
	int m_nServerBlowID;

#else
	char m_PacketBuffer[ 128 ];
	CPacketCompressStream* m_pPacketStream;
	// #31067 몬스터인 경우엔 직접 액션 실행 시키는 것이 있다면 실행된 상태.
	bool m_bStartedMonsterActionInSETableInfo;
#endif


private:

	// CDnStateBlow::Process() 에서만 호출되어야 함.
	friend class CDnStateBlow;
	void SetActorHandle( DnActorHandle hActor ) { m_hActor = hActor; };

public:

#ifndef _GAMESERVER
	DnEtcHandle _AttachGraphicEffect( bool bAllowSameSkillDuplicate = false );
	bool _DetachGraphicEffect( void );
	void _SetDiffuse( float fRed, float fGreen, float fBlue, float fAlpha );

	bool AttachGraphicEffectDefaultType( void );
	bool DetachGraphicEffectDefaultType( void );
	void UseTableDefinedGraphicEffect( bool bUseTableDefined ) { m_bUseTableDefinedGraphicEffect = bUseTableDefined; };
	bool IsUseTableDefinedGraphicEffect( void ) { return m_bUseTableDefinedGraphicEffect; };
	void SetEffectOutputInfo( const EffectOutputInfo* pOutputInfo ) { m_pEffectOutputInfo = pOutputInfo; };
	const EffectOutputInfo* GetEffectOutputInfo( void ) { return m_pEffectOutputInfo; };
	void AllowDiffuseVariation( bool bAllowDiffuseVariation );
	void SetServerBlowID( int nServerBlowID ) { m_nServerBlowID = nServerBlowID; };
	int GetServerBlowID( void ) { return m_nServerBlowID; };
#else
	// #31067 상태효과 이펙트 테이블에서 액터의 액션을 정의한 경우 몬스터인 경우는 서버에서도 실행시켜줘야 한다.
	void CheckAndStartActorActionInEffectInfo( void );
	void CheckAndStopActorActionInEffectInfo( void );
#endif
	
	bool CanAddGraphicsEffect();

	void SetCallBackType( DWORD dwType ) { m_dwCallBackType = dwType; }
	void AddCallBackType( DWORD dwType ) { m_dwCallBackType |= dwType; }
	DWORD GetCallBackType() { return m_dwCallBackType; }

	void SetBlowID( int nID ) { m_nBlowID = nID; }
	int GetBlowID() { return m_nBlowID; }

	void SetParentSkillInfo( const CDnSkill::SkillInfo* pParentSkillInfo );
	const CDnSkill::SkillInfo* GetParentSkillInfo( void ) { return &m_ParentSkillInfo; };

	void SetPermanent( bool bInstant ) { m_bPermanentBlow = bInstant; }
	__inline bool IsPermanent() { return m_bPermanentBlow; }

	DnActorHandle GetActorHandle() { return m_hActor; }

public:
	void SetStateBlow( STATE_BLOW &stateblow )			{ m_StateBlow = stateblow; m_fOriginalDurationTime = stateblow.fDurationTime; }
	void SetBlow( STATE_BLOW::emBLOW_INDEX blow )		{ m_StateBlow.emBlowIndex = blow; }
	void SetState( STATE_BLOW::emBLOW_STATE state )		{ m_StateBlow.emBlowState = state; }

	void ResetDurationTime( void ) { m_StateBlow.fDurationTime = m_fOriginalDurationTime; };

	void SetDurationTime( float fTime );
	void SetDelayTime( float fTime );
	void SetValue( const char *szValue )			{ m_StateBlow.szValue = szValue ? szValue : ""; }

	inline STATE_BLOW& GetStateBlow()				{ return m_StateBlow; }
	inline STATE_BLOW::emBLOW_INDEX GetBlowIndex()	{ return m_StateBlow.emBlowIndex; }
	inline STATE_BLOW::emBLOW_STATE GetBlowState()	{ return m_StateBlow.emBlowState; }

	inline float GetDurationTime()							{ return m_StateBlow.fDurationTime; }
	inline float GetDelayTime()							{ return m_StateBlow.fDelayTime; }
	inline const char* GetValue()							{ return m_StateBlow.szValue.c_str(); }
	
	inline float GetFloatValue()							{ return m_fValue; };
	
	inline bool IsBegin()		{ return (m_StateBlow.emBlowState==STATE_BLOW::STATE_BEGIN); }
	inline bool IsDuration()	{ return (m_StateBlow.emBlowState==STATE_BLOW::STATE_DURATION); }
	inline bool IsEnd()		{ return (m_StateBlow.emBlowState==STATE_BLOW::STATE_END); }

	int GetThreatAggro();
	inline bool IsGuildBlow() { return m_bGuildBlow; }

public:
	virtual bool CanBegin( void ) { return true; };		// 확률체크 등등 실제 상태효과 발동되기 전에 체크할 사항 체크해서 상태효과 발동 허용여부 판단.
	virtual void Process( LOCAL_TIME LocalTime, float fDelta );
	virtual void OnBegin( LOCAL_TIME LocalTime, float fDelta ) {}
	virtual void OnEnd( LOCAL_TIME LocalTime, float fDelta ) {}
	virtual void OnChangedWeapon() {}

	// 때린 녀석인 hActor 에서 state 를 가져다 사용하면 스킬 액션이 이미 끝난 상태일 수 있기 때문에 평소 공격력/방어력일 수 있으므로 
	// 발사체에 실어서 보내는 것까지 고려된 pAttackerState 를 사용한다. 
	// pAttackerState 를 함수 내부에서 사용할 경우, 반드시 내부에서 NULL 포인터 체크를 해야 함.
	virtual bool OnDefenseAttack( DnActorHandle hActor, CDnState* pAttackerState, CDnDamageBase::SHitParam &HitParam, bool bHitSuccess ) { return bHitSuccess; }
	virtual int OnUseMP( int nMPDelta ) { return nMPDelta; };

	// 클라에서는 실제 데미지 계산에 관여되지는 않고 OnDamage 패킷을 받았을 때 호출된다.
	virtual float OnCalcDamage( float fOriginalDamage, CDnDamageBase::SHitParam& HitParam ) { return 0.0f; };

	// 타겟이 hit 되는 시점.
	virtual void OnTargetHit( DnActorHandle hTargetActor ) {};

	// CDnActorStatIntervalManipulator 에서만 사용하는 함수임!
	virtual bool OnCheckIntervalProb( void ) { return true; };
	virtual float GetStatDelta( void ) { return 0.0f; };
	virtual bool OnCustomIntervalProcess( void ) { return false; };

	//bool IsPostStateEffect( void );
	CDnActorState::AddBlowStateType GetAddBlowStateType();

	// 효과 중첩에 대한 처리를 담당. 특정 상태효과별로 필요한 것이 있으면 함수 오버라이딩으로 처리.
	void Duplicate( const STATE_BLOW& StateBlowInfo );
	virtual void OnDuplicate( const STATE_BLOW& StateBlowInfo );
	virtual bool CalcDuplicateValue( const char* szValue ) { return false; };
	bool IsDuplicated( void ) { return m_bDuplicated; };
	int GetDuplicateCount( void ) { return m_iDuplicateCount; };

	virtual void OnCmdActionFromPacket( const char* pActionName ) {};

#ifdef _GAMESERVER
	const CPacketCompressStream* GetPacketStream( const char* pszOrgParam=NULL, const bool bOnInit=false );
	virtual void WriteAdditionalPacket( void ) {};
#else
	virtual void OnReceiveAddPacket( CPacketCompressStream& PacketStream ) {};
#endif

	void FromSourceItem( void ) { m_bFromSourceItem = true; };
	bool IsFromSourceItem( void ) { return m_bFromSourceItem; };

#ifdef _GAMESERVER
	void SetHPMPFullWhenBegin( void ) { m_bHPMPFullWhenBegin = true; };
#endif

#if !defined(_GAMESERVER)
	// 상태효과의 그래픽 이펙트를 패킷(eActor::SC_SHOW_STATE_EFFECT)으로 활성화 하기 위해. [2011/02/18 semozz]
	virtual void ShowGraphicEffect(bool bShow) {};
#endif // _GAMESERVER

	// Process에서 OnSignal함수 호출 하던 기능을 이곳으로 옮김..
	virtual void SignalProcess( LOCAL_TIME LocalTime, float fDelta ) {};

	void SetIgnoreEffectAction( bool bIgnore ) { m_bIgnoreEffectAction = bIgnore; }

	void SetOriginalBlowIndex( STATE_BLOW::emBLOW_INDEX eOriginalBlowIndex ) { m_eOriginalBlowIndex = eOriginalBlowIndex; };
	void RestoreOriginalBlowIndex( void ) { m_StateBlow.emBlowIndex = m_eOriginalBlowIndex; };
	bool IsChangedBlowIndex( void ) { return (m_eOriginalBlowIndex != STATE_BLOW::BLOW_NONE); };

	bool IsEternity() { return m_bEternity; }
	void SetEternity( bool bEternity ){ m_bEternity = bEternity; }

#if defined(_GAMESERVER)
#if defined(PRE_FIX_BLOCK_CONDITION)
protected:
	bool IsCanBlock(DnActorHandle hHitter, DnActorHandle hHitted, const CDnDamageBase::SHitParam &HitParam);
#endif // PRE_FIX_BLOCK_CONDITION

#if defined(PRE_ADD_49166)
	bool IsInVaildBlockCondition(DnActorHandle hActor);
#endif // PRE_ADD_49166
#endif // _GAMESERVER

protected:
	virtual void OnSetParentSkillInfo() {};

#if defined(PRE_FIX_51048)
public:
	virtual void RemoveDebufAction(LOCAL_TIME LocalTime, float fDelta) {};
#endif // PRE_FIX_51048

public:
	bool IsMatchStateEffectIndex(int nStateEffectIndex);

#if defined(PRE_ADD_DARKLAIR_HEAL_REGULATION) && defined(_GAMESERVER) 
	float CalcModifiedHealValue( float fModifyValue );
#endif

#if defined( _GAMESERVER )
	void CalcHealValueLimit( const STATE_BLOW::emBLOW_INDEX emBlowIndex, float & fModifyValue );
#endif
#if defined(PRE_ADD_DRAGON_FELLOWSHIP)
#if defined( _GAMESERVER )
	static bool CheckEffectIgnoreMapType(GlobalEnum::eMapTypeEnum eMapType, GlobalEnum::eMapSubTypeEnum eSubMapType);
#else
	static bool CheckEffectIgnoreMapType();
#endif
#endif	// #if defined(PRE_ADD_DRAGON_FELLOWSHIP)
	virtual int GetReplaceSkill(int nSkillID) { return 0; }; //rlkt_345
};