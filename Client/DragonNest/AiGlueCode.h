#pragma once
/*
#include "Singleton.h"


//-----------------------------------------------------------------------
// 인공지능 API
void	cppGlue_Log(const char* szMsg);

int		cppGlue_IsCurrentAction(DWORD nUID, const char* szAction);
const char* cppGlue_GetCurrentAction(DWORD nUID);
void	cppGlue_FindWalk(DWORD nUID);
int		cppGlue_FindTarget(DWORD nUID, float fDistance = 500.0f);
void	cppGlue_ChaseTarget(DWORD nMyUID, DWORD nTargetUID);
float	cppGlue_TargetDistance(DWORD nMyUID);
float	cppGlue_UnitSize(DWORD nMyUID);
void	cppGlue_Tracing(DWORD nMyUID, float fDist, const char* szAction);
void	cppGlue_LookTarget(DWORD nMyUID);
void	cppGlue_CmdAction(DWORD nMyUID, const char* szAction);

void	cppGlue_FinishDying(DWORD nMyUID, float fDieDelta);
void	cppGlud_Rotate(DWORD nMyUID, float fAngle);
// State 가 변한 이후로 얼만큼의 시간이 흘렀는지를 리턴한다.

DWORD	cppGlue_GetCurTime();



class MAAiScript : public MAAiBase
{
public:

	MAAiScript( DnActorHandle hActor, MAAiReceiver *pReceiver );
	virtual ~MAAiScript();

protected:
	DWORD	m_nUID;
	int m_nState;

	// 루아
	lua_State*	  m_pLua;
	// 루아 로드 여부 false 로 바꾸면 process 에서 로드한다.
	bool		  m_bLoadScript;
	std::string	  m_szLuaFileName;

	DWORD		  m_nLastChangedTime;


private:
	// key 는 몬스터 UID,
	typedef std::map<DWORD, MAAiScript*>	MonsterScriptAIMap;
	typedef MonsterScriptAIMap::iterator	MonsterScriptAIMapIt;

	static  MonsterScriptAIMap	ms_MonsterScriptAIMap;

public:
	static  bool		AddMonsterScriptAI(DWORD nUID, MAAiScript* pAI);
	static  bool		RemoveMonsterScriptAI(DWORD nUID);
	static	MAAiScript* FindMonsterScriptAI(DWORD nUID);
	static  bool		ReloadAllScript();

private:
	bool	_LoadScript(const char* szFileName);
	bool	_PreScriptCall();

public:

	virtual void	Process( LOCAL_TIME LocalTime, float fDelta );
	inline const TCHAR*	 GetStateText(int nState);
	inline DnActorHandle GetActor() { return m_hActor; }
	inline DnActorHandle GetTarget() { return m_hTarget; }
	inline void			 SetTarget(DnActorHandle& hTarget) { m_hTarget = hTarget; }

public:
	virtual void OnFinishAction(const char* szPrevAction);
	virtual void OnDie();

};


//----------------------------------------------------------------------------------------------

*/