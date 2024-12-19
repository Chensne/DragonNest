#pragma once

#include "EtMatrixEx.h"
#include "DnItem.h"
#include "DnSkill.h"

class CDnActor;
class CDnProjectile;

class MASkillUser 
{
public:
	MASkillUser();
	virtual ~MASkillUser();

protected:
	LOCAL_TIME m_LocalTime;
	DnSkillHandle m_hProcessSkill;
	DnSkillHandle m_hCanceledSkill;

	DnSkillHandle m_hToggleSkill;		//이녀석은 더이상 사용되지 않음..여기저기 코드 흔적이 남아 있어서 변수만 살려 놓음..
	DnSkillHandle m_hAuraSkill;

	DnSkillHandle m_hItemSkill;

#ifndef PRE_FIX_SKILLLIST
	DNVector(DnSkillHandle) m_vlhSkillList;
	vector<bool> m_vbSelfAllocList;
#else
	struct S_SKILL_OBJECT
	{
		DnSkillHandle hSkill;
		bool bSelfAlloc;

		S_SKILL_OBJECT( void ) : bSelfAlloc( false ) {};
		S_SKILL_OBJECT( DnSkillHandle _hSkill, bool _bSelfAlloc ) 
		{  
			hSkill = _hSkill;
			bSelfAlloc = _bSelfAlloc;
		};
	};
#endif // #ifndef PRE_FIX_SKILLLIST
	

	// 아이템에 붙은 지속시간 있는 스킬, 칼 던지기같은 스킬 루틴 그대로 따루는 것들은 따로 모아준다.
	deque<DnSkillHandle> m_dqhItemSkillList;

	DnActorHandle m_hSkillTarget;
	EtVector3 m_vSkillTargetPos;

	// 토글, 오라 스킬을 모아둔다.
	DNVector(DnSkillHandle) m_vlhToggleSkills;
	DNVector(DnSkillHandle) m_vlhAuraSkills;

	// 파워 마스터리 같은, 능력치를 영구히 늘려주는 스킬들은 따로 모아서 갖고 있다가 효과 적용, 삭제 타이밍을 제어한다.
	DNVector(DnSkillHandle) m_vlhSelfPassiveBlowSkill;
	CDnActor *m_pActor;
	bool m_bIsValidActor;

#ifdef _GAMESERVER
	// 토글 스킬이 켜질 당시에 적용되어야 할 상태를 저장해둠. (프로젝타일에 실어서 날려주기 위함)
	CDnState m_ActorStateSnapshotForToggleProjectile;
#endif

	// 종료 시킬 스킬 리스트. MASkillUser::Process 에서 처리한다.
	DNVector(DnSkillHandle) m_vlhReservedFinishSkill;

	// 쿨타임 상태효과 조절 값에 영향받는 비율 변수.
	//float m_fAdjustCoolTime;

#ifdef _GAMESERVER
	// cheat
	bool m_bIgnoreCoolTime;

	list<DnSkillHandle> m_listAutoPassiveSkills;
	list<DnSkillHandle> m_listProcessAutoPassiveSkills;

	// 아이템에 들어있는.. 평타로 피격된 대상에게 일정 확률로 스킬의 Target 으로 지정된 상태효과들을 
	// 부여하는 발현타입의 모임.
	DNVector(IDnSkillProcessor*) m_vlpApplySEWhenNormalHitProcessor;
#else
	bool m_bSkillExecutedThisFrame;		// 스킬 사용 허용 시그널이 들어온 순간 퀵슬롯 여러개 눌러서 동시에 스킬 사용되는 것을 막는다. (#9886)

	// 보유하고 있는 스킬에서 사용중인 액션이름들의 모음
	set<string> m_setUseActionNames;
#endif

	// 쿨타임 조절 상태효과 구현이 바뀌었음. 현재 쿨타임 계산시 효과시간동안 쿨타임의 델타값에 영향을 준다. (#21146)
	float m_fCoolTimeDeltaAdjustValue;

private:
#ifdef PRE_FIX_SKILLLIST
	vector<S_SKILL_OBJECT> m_vlSkillObjects;
#endif // #ifdef PRE_FIX_SKILLLIST

protected:
	bool IsValidActor();

	CDnSkill::UsingResult CanExecuteSkill( DnSkillHandle hSkill );

	// Process
	virtual void OnSkillProcess( LOCAL_TIME LocalTime, float fDelta );
	// 스킬 사용
	// 게임서버에서 시작되는 오토 패시브 스킬 플래그는 클라이언트에서만 함수 인자로 받습니다.
#ifdef _GAMESERVER
	virtual bool ExecuteSkill( DnSkillHandle hSkill, LOCAL_TIME LocalTime, float fDelta );
#else
	virtual bool ExecuteSkill( DnSkillHandle hSkill, LOCAL_TIME LocalTime, float fDelta, bool bCheckValid = true, bool bAutoUsedFromServer = false, bool bSendPacketToServer = true );
#endif
	// 토글이 걸려있을때 프로젝타일이 발사되면 토글된 스킬과 프로젝타일 포인터와 함께 불려진다.
	virtual void OnSkillToggleProjectile( DnSkillHandle hSkill, CDnProjectile *pProjectile );
public:
	// 토글 스킬이 사용될 때 불려진다.
	virtual void OnSkillToggle( DnSkillHandle hSkill, bool bEnable );
	// 오라 스킬이 사용될 때 불려진다.
	virtual void OnSkillAura( DnSkillHandle hSkill, bool bEnable );
	// 프로젝타일
	virtual void OnSkillProjectile( CDnProjectile *pProjectile );
	virtual void OnAddSkill( DnSkillHandle hSkill, bool isInitialize = false );
	virtual void OnRemoveSkill( DnSkillHandle hSkill );
	virtual void OnReplacementSkill( DnSkillHandle hLegacySkill, DnSkillHandle hNewSkill ) {};

public:
	DnSkillHandle FindSkill( int nSkillTableID );

	virtual bool CanAddSkill( int nSkillTableID, int nLevel = 1 ){ return true; }
	virtual bool AddSkill( int nSkillTableID, int nLevel = 1, int iSkillLevelApplyType = CDnSkill::PVE );

#ifndef PRE_FIX_SKILLLIST		// 추후 검색의 편의성을 위해서.
#else
	virtual void AddSkillObject( S_SKILL_OBJECT& SkillObject ) { m_vlSkillObjects.push_back( SkillObject ); };
#endif // #ifdef PRE_FIX_SKILLIST

	// 겜서버에서는 몬스터에서만 사용함.
	// 플레이어 액터는 스킬트리 관리가 중간에 들어가기 때문에 서버에서 이 함수 사용하면 안됩니다.
	bool AddSkill( DnSkillHandle hSkill );
	bool RemoveSkill( int nSkillTableID );
	bool RemoveSkillAll();
	bool EndStateEffectSkill(int nSkillTableID);
	// 현재 클라에서만 쓰이고 있음.
#ifndef _GAMESERVER
	bool ReplacementSkill( int nSkillTableID, int nLevel = 1 );		// 겜서버에서 사용 스킬 객체 self allocation
	bool ReplacementSkill( DnSkillHandle hNewSkill );
#endif
	
	virtual bool ApplyPassiveSkill( DnSkillHandle hSkill, bool isInitialize = false );
	virtual void OnApplyPassiveSkillBlow( int iBlowID ) {};
	void ApplyPassiveSkills( void );					// 부활 할 때 모두 사라진 상태효과를 다시 적용시킬 때 호출함
	bool bIsPassiveSkill( DnBlowHandle hBlow );
	bool IsExistSkill( int nSkillTableID, int nLevel = -1 );
	void SetSkillLevel( int nSkillTableID, int nValue );
	bool IsSelfAllocSkill( int nSkillTableID );

	virtual CDnSkill::UsingResult UseSkill( int nSkillTableID, bool bCheckValid = true, bool bAutoUseFromServer = false, int nLuaSkillIndex=-1 );	// LuaSkillIndex 몬스터AI 에서 필요해서 추가함.
	bool IsProcessSkill();
	void ResetToggleSkill();
	void ResetAuraSkill();

	bool IsEnabledToggleSkill() 
	{ 
		//m_hToggleSkill변수 참조는 더 이상 없도록..
		return false;
	};
	
	bool IsEnabledAuraSkill() { return (m_hAuraSkill != NULL); };

	DnSkillHandle GetProcessSkill();
	DnSkillHandle GetEnabledToggleSkill() { return m_hToggleSkill; };
	DnSkillHandle GetEnabledAuraSkill() { return m_hAuraSkill; };

	DnActorHandle GetSkillTarget() { return m_hSkillTarget; }
	EtVector3 *GetSkillTargetPos() { return &m_vSkillTargetPos; }

	DWORD GetSkillCount();
	DnSkillHandle GetSkillFromIndex( DWORD dwIndex );

	bool UseItemSkill( int nSkillTableID, int nSkillLevelTableID, CDnItem::ItemSkillApplyType ItemSkillApplyType, int nItemID = -1 );
	bool IsUsingItemSkill( void ) { return m_hItemSkill ? true : false; }

	void ResetSkillCoolTime( void );
	void ResetPvPSkillCoolTime();
	void ResetLadderSkillCoolTime();
#ifdef _GAMESERVER
	void IgnoreSkillCoolTime( void );
	bool IsIgnoreSkillCoolTime( void ) { return m_bIgnoreCoolTime; };
#endif

#ifndef _GAMESERVER
	bool IsUsingSkillAction( const char *szAction );
#endif
	void ReserveFinishSkill( DnSkillHandle hSkill );
	
	// 현재 사용중인 스킬 사용 중단.
	void CancelUsingSkill( void );

#ifdef _GAMESERVER
	// for Fire/Ice Shield
	void UseAutoPassiveSkill( LOCAL_TIME LocalTime, float fDelta );
	bool IsProcessingAutoPassive( int iSkillID );
	void EndAutoPassiveSkill( LOCAL_TIME LocalTime, float fDelta );
	//const CDnState *GetActorStateSnapshotForProjectile( void ) { return &m_ActorStateSnapshotForToggleProjectile; };
#endif

	//rlkt_mastery
	bool HavePassiveSkill(int iSkillID);
	//void AddCoolTimeAdjustValue( float fCoolTimeAdjustValue ) 
	//{
	//	_ASSERT( 0.0f < fCoolTimeAdjustValue+fCoolTimeAdjustValue && fCoolTimeAdjustValue+fCoolTimeAdjustValue < 1.0f ); 
	//	m_fAdjustCoolTime += fCoolTimeAdjustValue; 
	//};
	//float GetCoolTimeAdjustValue( void ) { return m_fAdjustCoolTime; };

	// 무기가 변경됨. 패시브로 특정 무기 사거리 변경 상태효과 등이 있으므로 테이블에 정의된 스킬 사용가능 무기인지 체크 필요.
	void OnAttachWeapon( DnWeaponHandle hWeapon, int iAttachedWeaponIndex );
	void OnDetachWeapon( DnWeaponHandle hWeapon, int iDetachedWeaponIndex );

	float GetCoolTimeDeltaAdjustValue( void ) { return m_fCoolTimeDeltaAdjustValue; };

#ifdef _GAMESERVER
	void SelectSkillLevelDataType( int iLevelDataType, bool bPlayerSummonedMonster = false );
	int GetSelectedSkillLevelDataType( void );
#endif

protected:
	std::map<int, int> m_vlStateBlowIDToRemove;	// [2010/12/15 semozz]해당 상태효과 ID중복 방지를 위해 map으로 변경
public:
	void AddStateBlowIDToRemove(int blowID);	//제거해야할 상태효과 ID등록
	void InitStateBlowIDToRemove();				//제거해야할 상태효과 ID리스트 초기화
	virtual void RemoveResetStateBlow() {};		//제거해야할 상태효과 제거

#ifdef _GAMESERVER
	void AddApplySEWhenTargetNormalHitProcessor( IDnSkillProcessor* pProcessor ) { m_vlpApplySEWhenNormalHitProcessor.push_back( pProcessor ); };
	void RemoveApplySEWhenTargetNormalHitProcessor( IDnSkillProcessor* pProcessor );
#endif

#if defined(_GAMESERVER)
protected:

	//슬롯별 스킬 정보 저장용.
	struct PREFIX_SYSTEM_SKILLINFO
	{
		int slotIndex;				//해당 슬롯 Index
		float fRatio;				//스킬 확률
		DnSkillHandle hSkill;		//스킬 핸들
	};

	typedef std::vector<PREFIX_SYSTEM_SKILLINFO> PRESIX_SYSTEM_SKILLS;			//접두어 스킬 정보 리스트
	typedef std::map<int, PRESIX_SYSTEM_SKILLS> PREFIX_SYSTEM_SKILL_COLLECTION;	//스킬 ID별 그룹핑/우선순위별 그룹핑용 리스트
	typedef std::map<int, PREFIX_SYSTEM_SKILLINFO> PREFIX_SYSTEM_SKILL_LIST;	//접두어 시스템용 슬롯별 스킬 정보 리스트
	
	PREFIX_SYSTEM_SKILL_LIST m_prefixSystemDefenceSkills;			//접두어 시스템의 방어용 스킬들
	PREFIX_SYSTEM_SKILL_COLLECTION m_PrefixSystemCandiateDefenceSkills;
	
	PREFIX_SYSTEM_SKILL_LIST m_prefixSystemOffenceSkills;			//접두어 시스템의 공격용 스킬들
	PREFIX_SYSTEM_SKILL_COLLECTION m_PrefixSystemCandiateOffenceSkills;

	list<DnSkillHandle> m_listProcessPreFixDefenceSkills;
	list<DnSkillHandle> m_listProcessPreFixOffenceSkills;

	void RefreshPrefixDefenceSkills();	//장비 장/탈착시 접두어 시스템 방어용 스킬 정보 갱신.(스킬 사용시점에 매번 하는것 보다 장비 장/탈착시 한번 해 놓는게 효율적일듯..)
	void RefreshPrefixOffenceSkills(float fHitDamageProb = 1.0f);	//장비 장/탈착시 접두어 시스템 공격용 스킬 정보 갱신.

	//접두어용 스킬 쿨타임 관리
	CoolTimeManager m_PrefixSkillCoolTimeManager;
public:
	void EndPrefixSystemSkill( LOCAL_TIME LocalTime, float fDelta );

	bool AddPreFixSystemDefenceSkill(int slotIndex, DnSkillHandle hSkill);
	void RemovePreFixSystemDefenceSkill(int slotIndex);
	void RemoveProcessPrefixDefenceSkill(DnSkillHandle hSkill);
	
	bool AddPreFixSystemOffenceSkill(int slotIndex, DnSkillHandle hSkill);
	void RemovePreFixSystemOffenceSkill(int slotIndex);
	void RemoveProcessPrefixOffenceSkill(DnSkillHandle hSkill);

	// 같은 스킬 아이디를 가진 스킬들을 수집한다.
	void GroupingPrefixDefenceSkillsBySameSkillID(PREFIX_SYSTEM_SKILL_LIST &skillList, PREFIX_SYSTEM_SKILL_COLLECTION &sameSkillList);
	// 수집된 같은 스킬 아이디의 스킬들 발동 확률을 조정(+)해서 후보 스킬을 등록한다.
	void CalculateProbabilitySkill(PREFIX_SYSTEM_SKILL_LIST &candidateSkills, PREFIX_SYSTEM_SKILL_COLLECTION &sameSkillList);
	// 같은 스킬ID의 스킬들중에 최종 선택된 스킬들을 우선순위별로 그룹핑을 한다.
	void GroupingSkillByPriority(PREFIX_SYSTEM_SKILL_COLLECTION& prefixSystemSkills, PREFIX_SYSTEM_SKILL_LIST& tempCandiateSkills, float fHitDamageProb = 1.0f);
	
	void ProcessPrefixDefenceSkill(DnActorHandle hHitter);	//접두어 시스템중 방어용 스킬 발동
	
	// 자신에게만 적용 될 상태효과만 적용 시킨다.
	void ProcessPrefixOffenceSkill(float fHitDamageProb);	//접두어 시스템중 공격용 스킬 발동
	void ApplyPrefixOffenceSkillToTarget(DnActorHandle hTarget);//접두어 시스템중 공격용 스킬에서 상대방에게 적용 시킬 상태효과들 적용
	
	// 접두어 시스템 공격용 스킬 적용이 끝나고 초기화.
	void InitPrefixOffenceSkills();

	bool IsPrefixTriggerSkill();

	bool IsApplyPrefixOffenceSkill() { return !m_listProcessPreFixOffenceSkills.empty(); }

#endif // _GAMESERVER

protected:
	struct SkillLevelUpInfo
	{
		int nSkillID;
		int nLevelUp;
	};

	std::map<int, SkillLevelUpInfo> m_SkillLevelUpInfoList;
	std::map<int, int> m_SkillLevelUpInfo;
	
	void UpdateSkillLevelUpInfo();
	void SkillLevelUp(int nSkillID, int nLevelUp);
public:
	void AddSkillLevelUpInfo(int nSlotIndex, int nSkillID, int nLevelUp);
	void RemoveSkillLevelUpInfo(int nSlotIndex);

	int GetSkillLevelUpValue(int nSkillID);
	bool ExistSkillLevelUpValue();

	void ApplySkillLevelUp( int nSkillTableID, int nValue ); //SetSkillLevel함수의 첫번째 조건 문제로 함수 추가함

	//클라이언트의 LocalPlayer의 구분 처리를 위해 가상 함수 선언(클라이언트에서 LocalPlayerActor에는 SkillTask를 통해 스킬 변경)
	//클라이언트의 LocalPlayer가 아닌 다른 캐릭터와 서버단의 PlayerActor는 MaSkillUser에서 처리됨
	virtual void ChangeSkillLevelUp(int nSkillID, int nSkillLevel);

protected:
	std::map<int, SkillLevelUpInfo> m_CashSkillLevelUpInfoList;
	std::map<int, int> m_CashSkillLevelUpInfo;

	void UpdateSkillLevelUpInfoByCashItem();

public:
	void AddSkillLevelUpInfoByCashItem(int nSlotIndex, int nSkillID, int nLevelUp);
	void RemoveSkillLevelUpInfoByCashItem(int nSlotIndex);


#if defined(PRE_ADD_PREFIX_SYSTE_RENEW)
#if defined(_GAMESERVER)
protected:
	typedef std::map<int, CDnPrefixSkill*> PREFIX_SKILL_INFO;
	PREFIX_SKILL_INFO m_PrefixDefenceSkills;
	PREFIX_SKILL_INFO m_PrefixOffenceSkills;

	std::list<CDnPrefixSkill*> m_ProcessPrefixOffenceSkills;

public:
	void AddPrefixDefenceSkill(DnSkillHandle hSkill);
	void RemovePrefixDefenceSkill(DnSkillHandle hSkill);

	void ProcessPrefixDefenceSkill_New(DnActorHandle hHitter);


	void AddPrefixOffenceSkill(DnSkillHandle hSkill);
	void RemovePrefixOffenceSkill(DnSkillHandle hSkill);
	void RemoveProcessPrefixOffenceSkill(CDnPrefixSkill* pRemovePrefixSkill);

	void ProcessPrefixOffenceSkill_New();
	void ApplyPrefixOffenceSkillToTarget_New(DnActorHandle hTarget);
#endif // _GAMESERVER
	
#endif // PRE_ADD_PREFIX_SYSTE_RENEW


protected:
	typedef std::map<int, DnSkillHandle> TOGGLESKILLLIST;
	TOGGLESKILLLIST m_ToggleOnSkillList;					//토글On된 스킬 리스트 관리

	void SkillToggle(DnSkillHandle hSkill, bool isOn);		//스킬 토글 On/Off

public:
	TOGGLESKILLLIST& GetActiveToggleSkillList() { return m_ToggleOnSkillList; }

	void SummonOnOffSkillInit(DWORD dwSummonMonsterID);

#if defined(PRE_FIX_62052)
	void ApplyGlobalSkillCoolTime(DnSkillHandle hSkill);
#endif // PRE_FIX_62052


#if defined(PRE_ADD_TOTAL_LEVEL_SKILL)
public:
	void UpdateGlobalCoolTime(float fRate);
	void ResetGlobalCoolTime(float fRate);
#endif // PRE_ADD_TOTAL_LEVEL_SKILL
};

