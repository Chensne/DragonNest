#pragma once

#include "MAAiBase.h"
#include "DnActor.h"
#include "MAAiSkill.h"
#include "MAAiChecker.h"
#include "MAAiMultipleTarget.h"

class MAAiCommand;
class CMAAiProcessorManager;

struct ActionTableCommon
{
	ActionTableCommon():Type(Max)
	{
	}

	enum eType
	{
		Action = 0,
		Skill,
		Max,
	};

	enum eCommon
	{
		MaxGlobalCoolTime = 5,
	};

	eType  Type;
};

struct ActionTable : public ActionTableCommon
{
	ActionTable() 
		:nDistanceState(0),nIdx(0),nRate(0),nLoop(0),nCoolTime(0),nLastTime(0),nCancelLook(0),fApproach(100.0f),nMinAttackMissRadian(0),nMaxAttackMissRadian(0)
		,fRandomTarget(0.f)
#if defined (PRE_MOD_AIMULTITARGET)
		,nMultipleTarget(0)
		,nSummonerTarget(0)
#else
		,bMultipleTarget(false)
#endif
		,pMAAiCheckerManager(NULL)
		,nLuaSkillIndex(-1)
		,nBeHitSkillIndex(-1)
		,iMaxMultipleTargetCount(0)
		,bIsAnyMultipleTarget(false)
		,bIsExceptCannonPlayer(false)
#if !defined( PRE_FIX_68096 )
		,iGlobalCoolTimeIndex(-1)
#endif		
#ifdef PRE_ADD_MONSTER_NORMAL_ACTION_PRIORITY
		,nActionPriority(0)
#endif
		{
#if defined( PRE_FIX_68096 )
			memset(iGlobalCoolTimeIndex, -1, sizeof(iGlobalCoolTimeIndex));
#endif
	}
	std::string				szActionName;
	int						nDistanceState;			// DistanceState
	int						nIdx;
	int						nRate;
	int						nLoop;
	int						nCoolTime;
	int						nCancelLook;			// 이액션을 취할때 Look 을 풀것인지 아닌지 1이면 푼다.
	LOCAL_TIME				nLastTime;
	float					fApproach;
	std::string				szTargetCondition;		// 타겟의 컨디션 테이블
	int						nMinAttackMissRadian;	// 공격시 미스Target 각도
	int						nMaxAttackMissRadian;
	float					fRandomTarget;
#if defined (PRE_MOD_AIMULTITARGET)
	int						nMultipleTarget;
	int						nSummonerTarget;
#else
	bool					bMultipleTarget;
#endif
	int						iMaxMultipleTargetCount;
	bool					bIsAnyMultipleTarget;
	bool					bIsExceptCannonPlayer;
	CMAAiCheckerManager*	pMAAiCheckerManager;
	int						nLuaSkillIndex;
	int						nBeHitSkillIndex;
#if defined( PRE_FIX_68096 )
	int						iGlobalCoolTimeIndex[ActionTableCommon::eCommon::MaxGlobalCoolTime];
#else
	int						iGlobalCoolTimeIndex;
#endif
#ifdef PRE_ADD_MONSTER_NORMAL_ACTION_PRIORITY
	int                     nActionPriority;
#endif
};

struct AISkillProcessor
{
	int						nSkillIndex;
	bool					bActive;
	CMAAiProcessorManager*	pMAAiProcessorManager;
};

#if defined( PRE_ADD_LOTUSGOLEM )
struct AIPartsProcessor
{
	int iMinHP,iMaxHP;
	int iCheckSkillIndex;
	int iCheckBlowIndex;
	std::string strCheckActionName;
	std::vector<int> vIgnorePartsIndex;
	std::vector<int> vNoDamagePartsIndex;
	std::vector<int> vActivePartsIndex;
};
#endif // #if defined( PRE_ADD_LOTUSGOLEM )

struct AIAggroRange
{
	int		iSelfHPPercent;
	int		iThreatRange;
	int		iCognizanceGentleRange;
	int		iCognizanceThreatRange;
};

struct AIGlobalCoolTime
{
	DWORD	dwCoolTime;
};

struct CustomAction 
{
	std::string szActionName;
	union 
	{
		int	nLoop;
		int nLuaSkillIndex;
	};
};


struct CustomActionTable
{
	std::string szCustomActionName;
	std::vector<CustomAction>	m_ActionList;
};

struct StateOperation
{
	CDnActorState::ActorStateEnum nState;
	std::string szString;	// for debug
	bool bNOT;

	bool Check(DnActorHandle hTarget)
	{
		switch( nState )
		{
		case CDnActorState::Stay:
			{
				if ( bNOT )	return !(hTarget->IsStay());
				else		return (hTarget->IsStay());
			}
			break;
		case CDnActorState::Move:
			{
				if ( bNOT )	return !(hTarget->IsMove());
				else		return (hTarget->IsMove());
			}
			break;
		case CDnActorState::Attack:
			{
				if ( bNOT )	return !(hTarget->IsAttack());
				else		return (hTarget->IsAttack());
			}
			break;
		case CDnActorState::Hit:
			{
				if ( bNOT )	return !(hTarget->IsHit());
				else		return (hTarget->IsHit());
			}
			break;
		case CDnActorState::Air:
			{
				if ( bNOT )	return !(hTarget->IsAir());
				else		return (hTarget->IsAir());
			}
			break;
		case CDnActorState::Down:
			{
				if ( bNOT )	return !(hTarget->IsDown());
				else		return (hTarget->IsDown());
			}
			break;
		case CDnActorState::Stun:
			{
				if ( bNOT )	return !(hTarget->IsStun());
				else		return (hTarget->IsStun());
			}
			break;
		case CDnActorState::Stiff:
			{
				if ( bNOT )	return !(hTarget->IsStiff());
				else		return (hTarget->IsStiff());
			}
			break;
		}
		return false;
	}
};

struct OrOperator
{
	// 여기선 or 연산해서 true 면 true 
	std::vector<StateOperation> OrOperatorArray;
	bool Check(DnActorHandle hTarget)
	{
		for ( size_t i = 0 ; i < OrOperatorArray.size() ; i++ )
		{
			if ( OrOperatorArray[i].Check(hTarget) ) 
				return true;
		}
		return false;
	}
};

struct StateCondition 
{
	// 이것은 and 연산일경우 만 true
	std::vector<OrOperator>	 AndOperatorArray;

	bool Check(DnActorHandle hTarget)
	{
		for ( size_t i = 0 ; i < AndOperatorArray.size() ; i++ )
		{
			if ( AndOperatorArray[i].Check(hTarget) == false ) 
				return false;
		}
		return true;
	}
};

struct AI_TABLE 
{
	std::vector<ActionTable>	VecActionTable;
	float	fNearValue;
	float	fNearValueSq;
	int		nRateSum;
};

struct AI_SETPARTSREBIRTH_TABLE
{
	std::vector<UINT>	vPartsTableID;
	UINT				uiRebirthTick;
};

struct ScriptData 
{
public:

	ScriptData()
	{
		m_Type = 0;
		m_nNearTableCnt = 0;
		m_nPatrolBaseTime = 0;
		m_nPatrolRandTime = 0;
		m_fApproachValue = 0;
		m_nAssaultTime = 0;
		m_nCurrentCustomActionPlayIndex = 0;
		m_nMinAttackMissRadian = 0;
		m_nMaxAttackMissRadian = 0;
#if defined( PRE_ADD_LOTUSGOLEM )
		m_iOnlyPartsDamage = 0;
#endif // #if defined( PRE_ADD_LOTUSGOLEM )
	}

	enum eTypeBit
	{
		None				= 0,
		NoAggroOwnerFollow	= 1,	// 어그로 없을때 주인(?) 따라댕기는 타입
		NoAggroStand		= 2,	// 어그로 없을 때 자리에 서있음
	};

	int		m_Type;
	int		m_nNearTableCnt;

	int		m_nPatrolBaseTime;
	int		m_nPatrolRandTime;
	float	m_fApproachValue;
	int		m_nAssaultTime;

	std::string	m_szSettingFile;

	// 기본 AI 테이블
	std::vector<AI_TABLE>	m_AITable;
	
	// 파츠몬스터 공용 부활 테이블
	std::vector<AI_SETPARTSREBIRTH_TABLE>	m_vSetPartsRebirthTable;

	std::map<std::string, CustomActionTable>	m_CustomActionMap;

	CustomActionTable			m_CurrentCustomActionTable;
	int							m_nCurrentCustomActionPlayIndex;


	// Miss Radian
	int							m_nMinAttackMissRadian;
	int							m_nMaxAttackMissRadian;

	// 다운안되는 데미지를 입었을때 사용될 테이블
	std::vector<ActionTable>	m_NonDownMeleeDamageActionTable;
	std::vector<ActionTable>	m_NonDownRangeDamageActionTable;

	// 방어시 사용될 테이블
	std::vector<ActionTable>	m_MeleeDefenseActionTable;
	std::vector<ActionTable>	m_RangeDefenseActionTable;

	// Hit시 사용될 테이블
	std::vector<ActionTable>	m_BeHitSkillActionTable;

	// 돌진후 사용될 테이블
	std::vector<ActionTable>	m_AssaultActionTable;

	// 스킬 테이블
#if defined(PRE_ADD_64990)
	std::vector<MonsterSkillTable>			m_SkillTable[2];
#else
	std::vector<MonsterSkillTable>			m_SkillTable;
#endif // PRE_ADD_64990

	// 상대타켓 상태 체크
	std::map<std::string, StateCondition>	m_StateContidionMap;

	// 스킬 Processor
	std::vector<AISkillProcessor>		m_AISkillProcessorTable;
	std::vector<AIAggroRange>			m_AIAggroRange;
	std::vector<AIGlobalCoolTime>		m_AIGlobalCoolTime;
#if defined( PRE_ADD_LOTUSGOLEM )
	int									m_iOnlyPartsDamage;
	std::vector<AIPartsProcessor>		m_AIPartsProcessor;
#endif // #if defined( PRE_ADD_LOTUSGOLEM )
};

class AiScriptLoader
{
public:
	AiScriptLoader();
	virtual ~AiScriptLoader();

	bool	AllLoadScript();
	bool	GetScript(const char* szFileName, OUT ScriptData& data);
	bool	LoadScript(const char* szFileName);
	bool	LoadActionTable(lua_tinker::table& t, int nIdx, OUT ActionTable& at);
	bool	LoadTable( lua_State* pLua, const char* pTableName, std::vector<ActionTable>& vActionTable );
	bool	LoadSkillProcessor( lua_State* pLua, std::vector<AISkillProcessor>& vSkillProcessor );
#if defined( PRE_ADD_LOTUSGOLEM )
	bool	LoadPartsProcessor( lua_State* pLua, ScriptData& data );
#endif // #if defined( PRE_ADD_LOTUSGOLEM )
	bool	LoadAggroRange( lua_State* pLua, std::vector<AIAggroRange>& vAggroRange );
	bool	LoadGlobalCoolTime( lua_State* pLua, std::vector<AIGlobalCoolTime>& vGlobalCoolTime );
	bool	LoadSetPartsRebirthTable( lua_State* pLua, ScriptData& data );
	
	std::map<std::string, ScriptData>	m_ScriptMap;

};

extern AiScriptLoader g_AiScriptLoader;

class MAAiScript : public MAAiBase, public TBoostMemoryPool< MAAiScript >
{
public:
	enum ActionType
	{
		AT_Normal = 0,
		AT_Assault,
		AT_Defense,
		AT_Damage,
		AT_CustomAction,
		AT_UseSkill,
		AT_Patrol,
	};

	enum TargetDirection
	{
		TD_LF = 0,
		TD_FL,
		TD_FR,
		TD_RF,
		TD_RB,
		TD_BR,
		TD_BL,
		TD_LB,
		TD_MAX,
	};

private:

	ActionType		m_nState;
	LOCAL_TIME		m_ChangeStateTime;
	ScriptData		m_Data;
	MonsterSkillAI	m_SkillAI;
	int				m_nSeletedAssaultActionIndex;
	bool			m_bValidAI;
	// Trigger 관련
	SOBB*			m_pDestinationOBB;
	UINT			m_uiMoveFrontRate;
	DWORD			m_dwDestinationOBB;
	// Command 관련
	MAAiCommand*	m_pAICommand;
	std::string		m_strAILookAction;

	bool _FindCustomAction(const char* szCustomActionName, OUT CustomActionTable& table);

	void	_CmdAction(const char* szAction, float fLength, int nLoop);
	void	_CmdActionTable( const ActionTable* pTable, float fLength, int iLoop, ActionType State=AT_Normal );
	void	_CmdFinishProcessSkill( LOCAL_TIME time );

	bool	_bIsActorChecker( const ActionTable& AT );
	bool	_bIsTargetActorChecker( const ActionTable& AT );
	bool	_bDestinationProcess();

	void	_SelectRandomTarget( const int state, const float fRandomTarget );
#if defined( PRE_ADD_LOTUSGOLEM )
	void	_ProcessParts();
#endif // #if defined( PRE_ADD_LOTUSGOLEM )

public:

	CMAAiMultipleTarget	m_cMultipleTarget;
	CDNCoolTime			m_GlobalCoolTime;

protected:

	virtual void	_OnSetProjectileTarget();

public:

	MAAiScript( DnActorHandle hActor, MAAiReceiver *pReceiver );
	virtual ~MAAiScript();

	MonsterSkillAI* GetMonsterSkillAI(){ return &m_SkillAI; }
	void			SetAIState( ActionType State ){ m_nState = State; }
	ActionType		GetAIState(){ return m_nState; }

	virtual bool	IsScript(){ return true; };
	virtual void	Process( LOCAL_TIME LocalTime, float fDelta );
	virtual void	OnChangeAction(const char* szPrevAction);
	virtual void	OnFinishAction(const char* szPrevAction, LOCAL_TIME time);
	virtual void	OnDie();

	// Trigger 관련
	virtual void	OnInitNaviDestination( SOBB* pOBB, UINT uiMoveFrontRate );

	void			OnAI(const char* szPrevAction, LOCAL_TIME time);
	int				GetTargetDistanceState( DnActorHandle hTarget );
	void			GetTargetDistance( const int state,  DNVector(DnActorHandle)& vTarget, bool bExceptAggroTarget=true );
	void			GetTargetDistance( const int iMin, const int iMax, DNVector(DnActorHandle)& vTarget, bool bExceptAggroTarget=true );
	static bool		bIsTargetDistance( DnActorHandle hActor, DnActorHandle hTarget, const int iMin, const int iMax );
	void			OnDistanceState(int state, float fLength, LOCAL_TIME time);
	int				SelectAction_Random(std::vector<ActionTable*>& AtArray, LOCAL_TIME time);
	int				SelectAction(std::vector<ActionTable>& AtArray, LOCAL_TIME time);
	bool			IsPassedCoolTime(ActionTable& at, LOCAL_TIME time);
	bool			IsCheckedTargetCondition(ActionTable& at, DnActorHandle hTarget);
	
	void			OnAssault(int state, float fLength, LOCAL_TIME time);
	void			OnNormal(int state, float fLength, LOCAL_TIME time);
	void			OnCustomAction(int state, float fLength, LOCAL_TIME time);
	void			OnDefense(int state, float fLength, LOCAL_TIME time);
	void			OnDamage(int state, float fLength, LOCAL_TIME time);
	void			OnHitFinish( LOCAL_TIME LocalTime, HitStruct *pStruct );

	
	void			OnUseSkill(int state, float fLength, LOCAL_TIME time);
	void			OnBeginNaviMode( const char* szPrevAction, const bool bPrevIsMove );
	void			AICmdMsgProc( const UINT uiMsg, void* pParam );

	bool			LoadScript(const char* szFilename);
	static  void	ReloadAllScript();

	const ScriptData& GetScriptData(){ return m_Data; }
	
#ifndef _FINAL_BUILD
private:
	void	OnTestNaviAI( LOCAL_TIME time );
	void	OnTestAttackAI( LOCAL_TIME time );
#endif
};
