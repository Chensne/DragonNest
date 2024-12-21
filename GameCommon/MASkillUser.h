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

	DnSkillHandle m_hToggleSkill;		//�̳༮�� ���̻� ������ ����..�������� �ڵ� ������ ���� �־ ������ ��� ����..
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
	

	// �����ۿ� ���� ���ӽð� �ִ� ��ų, Į �����ⰰ�� ��ų ��ƾ �״�� ����� �͵��� ���� ����ش�.
	deque<DnSkillHandle> m_dqhItemSkillList;

	DnActorHandle m_hSkillTarget;
	EtVector3 m_vSkillTargetPos;

	// ���, ���� ��ų�� ��Ƶд�.
	DNVector(DnSkillHandle) m_vlhToggleSkills;
	DNVector(DnSkillHandle) m_vlhAuraSkills;

	// �Ŀ� �����͸� ����, �ɷ�ġ�� ������ �÷��ִ� ��ų���� ���� ��Ƽ� ���� �ִٰ� ȿ�� ����, ���� Ÿ�̹��� �����Ѵ�.
	DNVector(DnSkillHandle) m_vlhSelfPassiveBlowSkill;
	CDnActor *m_pActor;
	bool m_bIsValidActor;

#ifdef _GAMESERVER
	// ��� ��ų�� ���� ��ÿ� ����Ǿ�� �� ���¸� �����ص�. (������Ÿ�Ͽ� �Ǿ �����ֱ� ����)
	CDnState m_ActorStateSnapshotForToggleProjectile;
#endif

	// ���� ��ų ��ų ����Ʈ. MASkillUser::Process ���� ó���Ѵ�.
	DNVector(DnSkillHandle) m_vlhReservedFinishSkill;

	// ��Ÿ�� ����ȿ�� ���� ���� ����޴� ���� ����.
	//float m_fAdjustCoolTime;

#ifdef _GAMESERVER
	// cheat
	bool m_bIgnoreCoolTime;

	list<DnSkillHandle> m_listAutoPassiveSkills;
	list<DnSkillHandle> m_listProcessAutoPassiveSkills;

	// �����ۿ� ����ִ�.. ��Ÿ�� �ǰݵ� ��󿡰� ���� Ȯ���� ��ų�� Target ���� ������ ����ȿ������ 
	// �ο��ϴ� ����Ÿ���� ����.
	DNVector(IDnSkillProcessor*) m_vlpApplySEWhenNormalHitProcessor;
#else
	bool m_bSkillExecutedThisFrame;		// ��ų ��� ��� �ñ׳��� ���� ���� ������ ������ ������ ���ÿ� ��ų ���Ǵ� ���� ���´�. (#9886)

	// �����ϰ� �ִ� ��ų���� ������� �׼��̸����� ����
	set<string> m_setUseActionNames;
#endif

	// ��Ÿ�� ���� ����ȿ�� ������ �ٲ����. ���� ��Ÿ�� ���� ȿ���ð����� ��Ÿ���� ��Ÿ���� ������ �ش�. (#21146)
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
	// ��ų ���
	// ���Ӽ������� ���۵Ǵ� ���� �нú� ��ų �÷��״� Ŭ���̾�Ʈ������ �Լ� ���ڷ� �޽��ϴ�.
#ifdef _GAMESERVER
	virtual bool ExecuteSkill( DnSkillHandle hSkill, LOCAL_TIME LocalTime, float fDelta );
#else
	virtual bool ExecuteSkill( DnSkillHandle hSkill, LOCAL_TIME LocalTime, float fDelta, bool bCheckValid = true, bool bAutoUsedFromServer = false, bool bSendPacketToServer = true );
#endif
	// ����� �ɷ������� ������Ÿ���� �߻�Ǹ� ��۵� ��ų�� ������Ÿ�� �����Ϳ� �Բ� �ҷ�����.
	virtual void OnSkillToggleProjectile( DnSkillHandle hSkill, CDnProjectile *pProjectile );
public:
	// ��� ��ų�� ���� �� �ҷ�����.
	virtual void OnSkillToggle( DnSkillHandle hSkill, bool bEnable );
	// ���� ��ų�� ���� �� �ҷ�����.
	virtual void OnSkillAura( DnSkillHandle hSkill, bool bEnable );
	// ������Ÿ��
	virtual void OnSkillProjectile( CDnProjectile *pProjectile );
	virtual void OnAddSkill( DnSkillHandle hSkill, bool isInitialize = false );
	virtual void OnRemoveSkill( DnSkillHandle hSkill );
	virtual void OnReplacementSkill( DnSkillHandle hLegacySkill, DnSkillHandle hNewSkill ) {};

public:
	DnSkillHandle FindSkill( int nSkillTableID );

	virtual bool CanAddSkill( int nSkillTableID, int nLevel = 1 ){ return true; }
	virtual bool AddSkill( int nSkillTableID, int nLevel = 1, int iSkillLevelApplyType = CDnSkill::PVE );

#ifndef PRE_FIX_SKILLLIST		// ���� �˻��� ���Ǽ��� ���ؼ�.
#else
	virtual void AddSkillObject( S_SKILL_OBJECT& SkillObject ) { m_vlSkillObjects.push_back( SkillObject ); };
#endif // #ifdef PRE_FIX_SKILLIST

	// �׼��������� ���Ϳ����� �����.
	// �÷��̾� ���ʹ� ��ųƮ�� ������ �߰��� ���� ������ �������� �� �Լ� ����ϸ� �ȵ˴ϴ�.
	bool AddSkill( DnSkillHandle hSkill );
	bool RemoveSkill( int nSkillTableID );
	bool RemoveSkillAll();
	bool EndStateEffectSkill(int nSkillTableID);
	// ���� Ŭ�󿡼��� ���̰� ����.
#ifndef _GAMESERVER
	bool ReplacementSkill( int nSkillTableID, int nLevel = 1 );		// �׼������� ��� ��ų ��ü self allocation
	bool ReplacementSkill( DnSkillHandle hNewSkill );
#endif
	
	virtual bool ApplyPassiveSkill( DnSkillHandle hSkill, bool isInitialize = false );
	virtual void OnApplyPassiveSkillBlow( int iBlowID ) {};
	void ApplyPassiveSkills( void );					// ��Ȱ �� �� ��� ����� ����ȿ���� �ٽ� �����ų �� ȣ����
	bool bIsPassiveSkill( DnBlowHandle hBlow );
	bool IsExistSkill( int nSkillTableID, int nLevel = -1 );
	void SetSkillLevel( int nSkillTableID, int nValue );
	bool IsSelfAllocSkill( int nSkillTableID );

	virtual CDnSkill::UsingResult UseSkill( int nSkillTableID, bool bCheckValid = true, bool bAutoUseFromServer = false, int nLuaSkillIndex=-1 );	// LuaSkillIndex ����AI ���� �ʿ��ؼ� �߰���.
	bool IsProcessSkill();
	void ResetToggleSkill();
	void ResetAuraSkill();

	bool IsEnabledToggleSkill() 
	{ 
		//m_hToggleSkill���� ������ �� �̻� ������..
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
	
	// ���� ������� ��ų ��� �ߴ�.
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

	// ���Ⱑ �����. �нú�� Ư�� ���� ��Ÿ� ���� ����ȿ�� ���� �����Ƿ� ���̺� ���ǵ� ��ų ��밡�� �������� üũ �ʿ�.
	void OnAttachWeapon( DnWeaponHandle hWeapon, int iAttachedWeaponIndex );
	void OnDetachWeapon( DnWeaponHandle hWeapon, int iDetachedWeaponIndex );

	float GetCoolTimeDeltaAdjustValue( void ) { return m_fCoolTimeDeltaAdjustValue; };

#ifdef _GAMESERVER
	void SelectSkillLevelDataType( int iLevelDataType, bool bPlayerSummonedMonster = false );
	int GetSelectedSkillLevelDataType( void );
#endif

protected:
	std::map<int, int> m_vlStateBlowIDToRemove;	// [2010/12/15 semozz]�ش� ����ȿ�� ID�ߺ� ������ ���� map���� ����
public:
	void AddStateBlowIDToRemove(int blowID);	//�����ؾ��� ����ȿ�� ID���
	void InitStateBlowIDToRemove();				//�����ؾ��� ����ȿ�� ID����Ʈ �ʱ�ȭ
	virtual void RemoveResetStateBlow() {};		//�����ؾ��� ����ȿ�� ����

#ifdef _GAMESERVER
	void AddApplySEWhenTargetNormalHitProcessor( IDnSkillProcessor* pProcessor ) { m_vlpApplySEWhenNormalHitProcessor.push_back( pProcessor ); };
	void RemoveApplySEWhenTargetNormalHitProcessor( IDnSkillProcessor* pProcessor );
#endif

#if defined(_GAMESERVER)
protected:

	//���Ժ� ��ų ���� �����.
	struct PREFIX_SYSTEM_SKILLINFO
	{
		int slotIndex;				//�ش� ���� Index
		float fRatio;				//��ų Ȯ��
		DnSkillHandle hSkill;		//��ų �ڵ�
	};

	typedef std::vector<PREFIX_SYSTEM_SKILLINFO> PRESIX_SYSTEM_SKILLS;			//���ξ� ��ų ���� ����Ʈ
	typedef std::map<int, PRESIX_SYSTEM_SKILLS> PREFIX_SYSTEM_SKILL_COLLECTION;	//��ų ID�� �׷���/�켱������ �׷��ο� ����Ʈ
	typedef std::map<int, PREFIX_SYSTEM_SKILLINFO> PREFIX_SYSTEM_SKILL_LIST;	//���ξ� �ý��ۿ� ���Ժ� ��ų ���� ����Ʈ
	
	PREFIX_SYSTEM_SKILL_LIST m_prefixSystemDefenceSkills;			//���ξ� �ý����� ���� ��ų��
	PREFIX_SYSTEM_SKILL_COLLECTION m_PrefixSystemCandiateDefenceSkills;
	
	PREFIX_SYSTEM_SKILL_LIST m_prefixSystemOffenceSkills;			//���ξ� �ý����� ���ݿ� ��ų��
	PREFIX_SYSTEM_SKILL_COLLECTION m_PrefixSystemCandiateOffenceSkills;

	list<DnSkillHandle> m_listProcessPreFixDefenceSkills;
	list<DnSkillHandle> m_listProcessPreFixOffenceSkills;

	void RefreshPrefixDefenceSkills();	//��� ��/Ż���� ���ξ� �ý��� ���� ��ų ���� ����.(��ų �������� �Ź� �ϴ°� ���� ��� ��/Ż���� �ѹ� �� ���°� ȿ�����ϵ�..)
	void RefreshPrefixOffenceSkills(float fHitDamageProb = 1.0f);	//��� ��/Ż���� ���ξ� �ý��� ���ݿ� ��ų ���� ����.

	//���ξ�� ��ų ��Ÿ�� ����
	CoolTimeManager m_PrefixSkillCoolTimeManager;
public:
	void EndPrefixSystemSkill( LOCAL_TIME LocalTime, float fDelta );

	bool AddPreFixSystemDefenceSkill(int slotIndex, DnSkillHandle hSkill);
	void RemovePreFixSystemDefenceSkill(int slotIndex);
	void RemoveProcessPrefixDefenceSkill(DnSkillHandle hSkill);
	
	bool AddPreFixSystemOffenceSkill(int slotIndex, DnSkillHandle hSkill);
	void RemovePreFixSystemOffenceSkill(int slotIndex);
	void RemoveProcessPrefixOffenceSkill(DnSkillHandle hSkill);

	// ���� ��ų ���̵� ���� ��ų���� �����Ѵ�.
	void GroupingPrefixDefenceSkillsBySameSkillID(PREFIX_SYSTEM_SKILL_LIST &skillList, PREFIX_SYSTEM_SKILL_COLLECTION &sameSkillList);
	// ������ ���� ��ų ���̵��� ��ų�� �ߵ� Ȯ���� ����(+)�ؼ� �ĺ� ��ų�� ����Ѵ�.
	void CalculateProbabilitySkill(PREFIX_SYSTEM_SKILL_LIST &candidateSkills, PREFIX_SYSTEM_SKILL_COLLECTION &sameSkillList);
	// ���� ��ųID�� ��ų���߿� ���� ���õ� ��ų���� �켱�������� �׷����� �Ѵ�.
	void GroupingSkillByPriority(PREFIX_SYSTEM_SKILL_COLLECTION& prefixSystemSkills, PREFIX_SYSTEM_SKILL_LIST& tempCandiateSkills, float fHitDamageProb = 1.0f);
	
	void ProcessPrefixDefenceSkill(DnActorHandle hHitter);	//���ξ� �ý����� ���� ��ų �ߵ�
	
	// �ڽſ��Ը� ���� �� ����ȿ���� ���� ��Ų��.
	void ProcessPrefixOffenceSkill(float fHitDamageProb);	//���ξ� �ý����� ���ݿ� ��ų �ߵ�
	void ApplyPrefixOffenceSkillToTarget(DnActorHandle hTarget);//���ξ� �ý����� ���ݿ� ��ų���� ���濡�� ���� ��ų ����ȿ���� ����
	
	// ���ξ� �ý��� ���ݿ� ��ų ������ ������ �ʱ�ȭ.
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

	void ApplySkillLevelUp( int nSkillTableID, int nValue ); //SetSkillLevel�Լ��� ù��° ���� ������ �Լ� �߰���

	//Ŭ���̾�Ʈ�� LocalPlayer�� ���� ó���� ���� ���� �Լ� ����(Ŭ���̾�Ʈ���� LocalPlayerActor���� SkillTask�� ���� ��ų ����)
	//Ŭ���̾�Ʈ�� LocalPlayer�� �ƴ� �ٸ� ĳ���Ϳ� �������� PlayerActor�� MaSkillUser���� ó����
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
	TOGGLESKILLLIST m_ToggleOnSkillList;					//���On�� ��ų ����Ʈ ����

	void SkillToggle(DnSkillHandle hSkill, bool isOn);		//��ų ��� On/Off

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

