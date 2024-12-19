#pragma once

#include "MAAiBase.h"
#include "DnActor.h"
//#include "BasiliskState.h"


struct BasiliskState
{
	enum
	{
		NoneState = 0,
		Ground_Stand,
		Ground_TailAttack,
		Ground_BodyAttack,
		Ground_RunAttack,
		Ground_StandUp,
		Ground_StandDead,

		Turn_Left,
		Turn_Right,
		FootAttack_Left,
		FootAttack_Right,

		OverTurned,
		OverTurned_Dead,
		OverTurned_Hit,

		Pillar_Climb,
		Pillar_Stand,
		Pillar_ThornAttack,
		Pillar_LoseBalance,
		Pillar_KeepBalance,
		Pillar_Fall,
		Pillar_ComeDown,

		Vaulting_Climb,
		Vaulting_Stand,
		Vaulting_ThornAttack,
		Vaulting_LoseBalance,
		Vaulting_KeepBalance,
		Vaulting_Fall,

		PlayAnimation,
		StateMax
	};
};

struct BasiliskPosition 
{
	enum
	{
		None = 0,
		Ground,
		Pillar,
		Pillar_Climb,
		Pillar_ComeDown,
		Pillar_LoseBalance,
		Pillar_KeepBalance,
		Vaulting, 
		Vaulting_Climb,
		OverTurned
	};
};

struct EnermyDir
{
	enum 
	{
		None = 0,
		Front,	
		Left,
		Right,
		Behind
	};
};

struct _POS_STATE_TABLE
{
	WORD nState;
	BYTE nPosition;
	std::string szActionName;
	LOCAL_TIME nKeepupTime;
};


struct CurrentInfo 
{
	DnActorHandle hActor;
	DnActorHandle hTargetActor;
	LOCAL_TIME	  nCurTime;
	int			  nMaxHP;
	int			  nCurHP;
	BYTE		  nPosState;
	DWORD		  nState;
};

struct BasiliskStateBase;

typedef std::vector<std::string>	PLAY_ANI_ARRAY;
typedef PLAY_ANI_ARRAY::iterator	PLAY_ANI_ARRAY_IT;

class MAAiBasilisk : public MAAiBase
{
public:
	MAAiBasilisk( DnActorHandle hActor, MAAiReceiver *pReceiver );
	virtual ~MAAiBasilisk();

	virtual void	Process( LOCAL_TIME LocalTime, float fDelta );
	virtual void	OnFinishAction(const char* szPrevAction, LOCAL_TIME time);
	virtual void	OnDie(LOCAL_TIME LocalTime);
	virtual void	OnDamage(DnActorHandle hActor, CDnActor::SHitParam &HitParam, LOCAL_TIME time);

	void ChangeState(DWORD nState);
	void CmdAction(const char *szActionName, int nLoopCount = 0, float fBlendFrame = 3.f );

	// 바실이가 -z 축으로 바라보고 있기때문에 -z축 과 타겟의 각도를 구한다.
	float GetDegree(DnActorHandle hActor, IN const EtVector3& AxisVector );
	bool  ScanEnermy(float fDist, OUT std::vector<DnActorHandle>& EnermyList );

	int GetEnermyDir();
	int WhereisActor(DnActorHandle hActor);

	// 현재 위치가 된지 얼마나 지났는가.
	LOCAL_TIME	GetPosStateElapsedTime() { return m_LocalTime-m_nCurPosStateTime; }
	bool		IsPassedKeepupTime();

	const CurrentInfo& GetCurrentInfo() { return m_CurrentInfo; }

	void LoadState(const char* szFile);
	lua_State* GetLua() { return m_pLua; }

private:

	void _InitState();
			
	//int _RegionDamage(const char* szRegion);
	
	void _OnAI(const char* szPrevAction, LOCAL_TIME time);

	void _OnPlayAnimation();

	int _FindCurPosition(const char* szActionName);
	
private:	

	BasiliskStateBase*	m_StateArray[BasiliskState::StateMax];
	_POS_STATE_TABLE	m_PositionStateTable[BasiliskState::StateMax];
	std::map<std::string, int>	m_AnimationState;
	
	// 다음 스테이트 예약
	DWORD	m_nNextState;
	DWORD	m_nState;
	DWORD	m_nPrevState;
	DWORD   m_nCurPosition;
	

	PLAY_ANI_ARRAY	m_CurrentAnimationArray;
	size_t	m_nCurrentAnimationIndex;

	

	// 현재 위치로 된지 얼마나 지났는지 
	LOCAL_TIME	m_nCurPosStateTime;
	LOCAL_TIME	m_LocalTime;
	CurrentInfo m_CurrentInfo;
	lua_State*	m_pLua;

};



//class MAAiBasilisk;

struct BasiliskStateBase
{

	BasiliskStateBase(DWORD nState): m_nState(nState),m_nLastTime(0),m_nCoolTime(0),m_nStartTime(0),m_nKeepupTime(0) { }
	virtual ~BasiliskStateBase() { }


	virtual bool	Verify(CurrentInfo& property);

	virtual void	OnStartState(LOCAL_TIME time);
	virtual void	OnEndState(LOCAL_TIME time);
	virtual DWORD	OnState(LOCAL_TIME time);
	virtual bool	IsPassedCoolTime(LOCAL_TIME time);
	virtual bool	IsPassedKeepupTime(LOCAL_TIME time);


	void	Init(LOCAL_TIME time);
	void	ChangeState(DWORD nState);
	void	CmdAction(const char *szActionName, int nLoopCount = 0, float fBlendFrame = 3.f );

	void	SetParent(MAAiBasilisk* pParent) { m_pBasilisk = pParent; }
	void	SetPositionState(BYTE nPositionState) { m_nPositionState = nPositionState; }
	void	SetKeepupTime(LOCAL_TIME time) { m_nKeepupTime = time; }
	void	SetCoolTime(LOCAL_TIME time) { m_nCoolTime = time; }
	void	LoadSetting()
	{
		lua_State* pLua = m_pBasilisk->GetLua();
		lua_tinker::table cooltable = lua_tinker::get<lua_tinker::table>(pLua, "g_lua_CoolTimeTable");
		m_nCoolTime = cooltable.get<int>( m_szStateName.c_str() );														
		m_nCoolTime = rand()%5000;
	}

	MAAiBasilisk*	m_pBasilisk;
	LOCAL_TIME	m_nStartTime;
	LOCAL_TIME	m_nLastTime;
	LOCAL_TIME	m_nCoolTime;
	LOCAL_TIME  m_nKeepupTime; 
	DWORD		m_nState;
	BYTE		m_nPositionState;
	std::string m_szStateName;
};





#define DECL_STATE(state_name)										\
struct state_name : public BasiliskStateBase						\
{																	\
	state_name():BasiliskStateBase(BasiliskState::Ground_Stand)		\
	{																\
		m_nLastTime = 0;											\
		m_szStateName = ( #state_name"" );							\
	}																\
	virtual bool	Verify(CurrentInfo& property);					\
	virtual DWORD	OnState(LOCAL_TIME time);						\
};																	\

DECL_STATE(Ground_Stand);
DECL_STATE(Ground_TailAttack);
DECL_STATE(Ground_BodyAttack);
DECL_STATE(Ground_RunAttack);

DECL_STATE(Ground_StandUp);
DECL_STATE(Ground_StandDead);

DECL_STATE(Turn_Left);
DECL_STATE(Turn_Right);
DECL_STATE(FootAttack_Left);
DECL_STATE(FootAttack_Right);

DECL_STATE(OverTurned);
DECL_STATE(OverTurned_Dead);
DECL_STATE(OverTurned_Hit);

DECL_STATE(Pillar_Climb);
DECL_STATE(Pillar_Stand);
DECL_STATE(Pillar_ThornAttack);
DECL_STATE(Pillar_LoseBalance);
DECL_STATE(Pillar_KeepBalance);
DECL_STATE(Pillar_Fall);
DECL_STATE(Pillar_ComeDown);

DECL_STATE(Vaulting_Climb);
DECL_STATE(Vaulting_Stand);
DECL_STATE(Vaulting_ThornAttack);
DECL_STATE(Vaulting_LoseBalance);
DECL_STATE(Vaulting_KeepBalance);
DECL_STATE(Vaulting_Fall);

