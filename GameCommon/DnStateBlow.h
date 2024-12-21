#pragma once
#include "DnBlowDef.h"
#include "DnDamageBase.h"
#include "DnBlow.h"

class RemoveStateBlowInfo
{
public:
	enum enmRSBI
	{
		RSBI_BLOW_INDEX = 0,
		RSBI_BLOW_ID,
		RSBI_SKILL_DURATION_TYPE,
		RSBI_BLOW_SERVER_ID
	};

	RemoveStateBlowInfo(enmRSBI type, int nValue)
	{
		m_Type = type;
		m_Value = nValue;
	}

	bool Compare(DnBlowHandle hBlow);
public:
	enmRSBI m_Type;
	int m_Value;
};

// 서버와 클라이언트에서 같이 사용하는 상태효과 관리 클래스
// 클라이언트엔 디파인으로 이펙트 관련 처리 코드가 포함됩니다.
class CDnStateBlow: public TBoostMemoryPool< CDnStateBlow >
{
public:
	enum
	{
		ADD_SUCCESS,
		ADD_FAIL_BY_IMMUNE,
		ADD_FAIL_BY_DIED,
		ADD_FAIL_BY_INVINCIBLE,
		ADD_FAIL_BY_REVIVAL,		// 부활 5초 진행 중엔 디버프 효과 무시
		ADD_FAIL_BY_PROBABILITY,	// 확률이 있는 상태효과 실패..
		ADD_FAIL_BY_GUILDBLOW_PRIORITY,		//길드스킬(길드전스킬)의 우선순위에 의한 효과무시
		ADD_FAIL_BY_PROB_SKILL_INVINCIBLE,	// 226번 특정 스킬 확률 무시 상태효과 발동에 의해 무시.
		ADD_FAIL_BY_COMBOLIMITBLOW,			// 242번 상태효과에 의해서 상태효고 발동 무시
#ifdef PRE_ADD_DECREASE_EFFECT
		ADD_DECREASE_EFFECT_BY_IMMUNE,
#endif // PRE_ADD_DECREASE_EFFECT
	};

	CDnStateBlow( DnActorHandle hActor );
	virtual ~CDnStateBlow(void);

	static bool InitializeClass();
	static void ReleaseClass();

protected:
#ifdef PRE_ADD_MONSTER_CHEAT
public:
#endif 
	typedef std::list<DnBlowHandle>		BLOW_HANDLE_LIST;
	typedef BLOW_HANDLE_LIST::iterator	BLOW_HANDLE_LIST_ITER;
#ifndef _GAMESERVER
	typedef std::map<int, EffectOutputInfo*> TMapEffectOutputInfo;

#if !defined(SW_MODIFY_SE_EFFECTOUTPUT_20091119_jhk8211)
	// 본에 겹치는 이펙트는 1초마다 교차출력 하도록 처리.
	struct S_BONE_EFFECT
	{
		DnEtcHandle hEffect;
		float fOutputElapsedTime;
		bool bShow;

		S_BONE_EFFECT( void ) : fOutputElapsedTime( 0.0f ), bShow( false ) {};
		bool operator == ( const DnEtcHandle& rhs )
		{
			return (hEffect == rhs);
		}

		void Show( bool _bShow )
		{  
			bShow = _bShow;
			//hEffect->Show( true );
			if( hEffect )
			{
				hEffect->Show( bShow );
				
				if( bShow )
					hEffect->SetActionQueue( "Idle", 0, 0.f );
			}
		}
	};
	list<S_BONE_EFFECT> m_aListBoneEffectStatus[ CDnActor::Max_FX_Dummy_Bone ];
#endif

#endif
	BLOW_HANDLE_LIST m_listBlowHandle;
	DnActorHandle m_hActor;
	int m_aiApplied[ STATE_BLOW::BLOW_MAX ];
	//map<string, int> m_mapEffectData;

	// 상태효과 객체들 루프돌면서 process 중에 리스트가 제거될 경우. 파이널 릴리즈가 아닌경우 깔금하게 죽어라.
	bool m_bLockStateBlowList;
	bool m_bLockRemoveReservedList;

	// 로딩중일 땐 프로세스 하지 않는다.
#ifdef _GAMESERVER
	STATIC_DECL( bool s_bStopProcess );
#else
	static bool s_bStopProcess;
#endif

#ifdef _GAMESERVER
	STATIC_DECL( int s_iStateBlowIDCount );
#else
	static int s_iStateBlowIDCount;
#endif


#ifndef _GAMESERVER
	// 상태효과 출력 데이터 정보 from StateEffectTagle
	// 기획 문서상의 상태효과 인덱스와 일치하며, 해당 벡터리스트에 레벨별로 효과 출력 정보를 갖고 있다.
	static TMapEffectOutputInfo s_nMapEffectOutputInfo;
	bool m_bAllowDiffuseVariation;
#endif

protected:
	int GenerateStateBlowID();

#ifndef _GAMESERVER
	static void _LoadStateEffectOutputInfo( void );
	static void _ReleaseStateEffectOutputInfo( void );

#if !defined(SW_MODIFY_SE_EFFECTOUTPUT_20091119_jhk8211)
	// 겹쳐진 이펙트 본에 있는 효과는 1초 간격으로 순회하여 보여줌.
	void _ProcessDuplicateEffectBoneRotateShow( LOCAL_TIME LocalTime, float fDelta );
#endif

#endif

	// 게임 프레임 업데이트에 따라 서버와 클라간에 민감할 수 있는 특정 상태효과들은 (ex)프레임 변경) 
	// AddStateBlow 호출되자마자 곧바로 duration 상태가 되도록 처리.
	void _CheckImmediatelyBegin( DnBlowHandle hBlow );

public:
	DnBlowHandle CreateStateBlow( DnActorHandle hActor, const CDnSkill::SkillInfo* pParentSkill, STATE_BLOW::emBLOW_INDEX emBlowIndex, int nDurationTime, const char *szParam );
	int AddStateBlow( DnBlowHandle hBlow );
	DnBlowHandle GetStateBlowFromID( int nStateBlowID );
	void RemoveStateBlowFromID( int nStateBlowID );
	void GetStateBlowFromBlowIndex( STATE_BLOW::emBLOW_INDEX emBlowIndex, /*IN OUT*/ DNVector(DnBlowHandle)& vlhResult );
	bool IsExistStateBlowFromBlowIndex( STATE_BLOW::emBLOW_INDEX emBlowIndex );
	bool IsExistStateBlowFromBlowID( int nBlowID );
	int RemoveStateBlowByBlowDefineIndex( STATE_BLOW::emBLOW_INDEX emBlowIndex );
	void DelAllStateBlow();
	int GetNumStateBlow( void ) { return (int)m_listBlowHandle.size(); }
	DnBlowHandle GetStateBlow( int iIndex );
	

#ifdef _CLIENT
#ifdef PRE_ADD_MONSTER_CHEAT
	BLOW_HANDLE_LIST GetStateBlowList() { return m_listBlowHandle; }
#endif 
#endif 
#ifdef _GAMESERVER
#ifdef PRE_ADD_PROJECTILE_SE_INFO
	BLOW_HANDLE_LIST GetStateBlowList() { return m_listBlowHandle; }
	void MakeCloneStateBlowList( BLOW_HANDLE_LIST BlowList );
#endif
#endif

	// 해당 상태효과가 적용중인지 여부 조회
	// MAAiBase 쪽에서 꽤 자주 호출되고 있어서 최적화 되도록 인라인함수로 뺍니다.. 인덱스 체크 release 에선 안하니깐 주의!
	inline bool IsApplied( STATE_BLOW::emBLOW_INDEX emBlowIndex ) 
	{ 
		_ASSERT( emBlowIndex < STATE_BLOW::BLOW_MAX );
		if( emBlowIndex < STATE_BLOW::BLOW_NONE || emBlowIndex >= STATE_BLOW::BLOW_MAX ) return false;

		return (0 == m_aiApplied[ emBlowIndex ] ? false : true);
	};
	
#ifndef _GAMESERVER
	void AllowDiffuseVariation( bool bAllowDiffuseVariation ) { m_bAllowDiffuseVariation = bAllowDiffuseVariation; };
	void RestoreAllBlowGraphicEffect();
	static EffectOutputInfo *GetEffectOutputInfo( int nItemID );
	void RemoveStateBlowFromServerID( int nServerBlowID );
	DnBlowHandle GetStateBlowFromServerID( int nServerID );

#if !defined(SW_MODIFY_SE_EFFECTOUTPUT_20091119_jhk8211)
	void AddEffectAttachedBone( int iBone, DnEtcHandle hEffectHandle );
	void DelEffectAttachedBone( int iBone, DnEtcHandle hEffectHandle );
#endif

#endif


public:
	void Process( LOCAL_TIME LocalTime, float fDelta, bool bForceInitialize = false );

public:
	void OnChangedWeapon();
	bool OnDefenseAttack( DnActorHandle hHitter,CDnState* pAttackerState, CDnDamageBase::SHitParam &HitParam, bool bHitSuccess );
	int OnUseMP( int iMPDelta );
	float OnCalcDamage( float fOriginalDamage, CDnDamageBase::SHitParam& HitParam );
	bool OnDie( DnActorHandle hHitter );
	void OnTargetHit( DnActorHandle hTargetActor );
	int CanAddThisBlow( const CDnSkill::SkillInfo* pParentSkill, STATE_BLOW::emBLOW_INDEX BlowIndex );
	void ResetStateBlowBySkillType( int nSkillDurationType );
	void OnCmdActionFromPacket( const char* pActionName );

#ifdef _GAMESERVER
	// 새로운 StateBlow를 OnSignal->STE_ApplyStateEffect 시점에서 추가 가능한지 판단 [2010/11/12 semozz]
	CDnSkill::CanApply CanApplySkillStateEffect(const CDnSkill::SkillInfo *pUsingSkillInfo, const CDnSkill::StateEffectStruct &newStateEffect);
#endif


#ifdef _GAMESERVER
	static void StopProcess( CMultiRoom* pRoom ) {  STATIC_INSTANCE_(CDnStateBlow::s_bStopProcess) = true; }
	static void ResumeProcess( CMultiRoom* pRoom ) {  STATIC_INSTANCE_(CDnStateBlow::s_bStopProcess) = false; };
#else
	static void StopProcess( void ) { s_bStopProcess = true; }
	static void ResumeProcess( void ) { s_bStopProcess = false; };
#endif

protected:
	// 벡터에서 리스트로 변경. 리스트에 추가 될때 iterator 유효성 확보를 위해 [2011/03/21 semozz]
	//typedef std::vector<RemoveStateBlowInfo> REMOVE_STATEBLOW_LIST;
	typedef std::list<RemoveStateBlowInfo> REMOVE_STATEBLOW_LIST;
	REMOVE_STATEBLOW_LIST m_RemoveStateBlowList;
	void RemoveStateBlowReservedList();

	void AddRemoveStateBlowInfo(RemoveStateBlowInfo info);
public:
	//상태효과 즉시 제거를 위한 함수
	void RemoveImediatlyStateEffectFromID(int nStateBlowID);
	void RemoveImediatlyStateEffectByBlowIndex(STATE_BLOW::emBLOW_INDEX emBlowIndex);
#if !defined(_GAMESERVER)
	void RemoveImediatlyStateEffectByServerID(int nServerBlowID);
#endif // _GAMESERVER

	bool IsImmuned(STATE_BLOW::emBLOW_INDEX blowIndex);

#ifndef _FINAL_BUILD
	void _CrashIfProcessListLocked( void );
	void _CrashIfRemoveListLocked( void );
#endif // #ifdef _FINAL_BUILD

protected:
	void Process_StateBlow_Old(LOCAL_TIME LocalTime, float fDelta, bool bForceInitialize);
	void Process_StateBlow_New(LOCAL_TIME LocalTime, float fDelta, bool bForceInitialize);
	void Process_BlowHandle(DnBlowHandle hBlow, LOCAL_TIME LocalTime, float fDelta);
#ifdef PRE_MOD_MAGICALBREEZE_CHANGE_BLOW
	STATE_BLOW::emBLOW_INDEX CheckMagicalBreezeChangeBlow( const CDnSkill::SkillInfo* pParentSkill, STATE_BLOW::emBLOW_INDEX emBlowIndex, DnActorHandle hActor );
#endif // PRE_MOD_MAGICALBREEZE_CHANGE_BLOW

protected:
	float GetImmuneReduceTimeValue(STATE_BLOW::emBLOW_INDEX blowIndex);

};