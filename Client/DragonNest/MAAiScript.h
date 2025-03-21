#pragma once

#include "MAAiBase.h"
#include "DnActor.h"

class MAAiScript : public MAAiBase
{
private:
	static std::vector<MAAiScript*>	ms_VecAIScript;

	struct ActionTable 
	{
		ActionTable() : nIdx(0), nRate(0), nLoop(0), nCoolTime(0), nLastTime(0) { }
		std::string szActionName;
		int			nIdx;
		int			nRate;
		int			nLoop;
		int			nCoolTime;
		LOCAL_TIME  nLastTime;
	};

	struct AI_TABLE 
	{
		std::vector<ActionTable>	VecActionTable;
		float	fNearValue;
		int		nRateSum;
	};

	std::vector<AI_TABLE>	m_AITable;
	int			m_nNearTableCnt;
	int			m_nLookTargetNearState;
	float		m_fWanderingDistance;

	int		m_nPatrolBaseTime;
	int		m_nPatrolRandTime;
	float	m_fApproachValue;

	enum ActionType
	{
		Normal = 0,
		Assault, 
	};
	ActionType	m_nState;
	LOCAL_TIME  m_ChangeStateTime;

	std::string	m_szSettingFile;

protected:
	virtual int GetThreatRange() { return (int)m_fWanderingDistance; }

public:
	MAAiScript( DnActorHandle hActor, MAAiReceiver *pReceiver );
	virtual ~MAAiScript();

	virtual void	Process( LOCAL_TIME LocalTime, float fDelta );
	virtual void	OnFinishAction(const char* szPrevAction, LOCAL_TIME time);
	virtual void	OnDie();

	void	OnAI(const char* szPrevAction, LOCAL_TIME time);
	int		GetTargetDistanceState();
	void	OnDistanceState(int state, float fLength, LOCAL_TIME time);
	int		SelectAction(std::vector<ActionTable*>& AtArray, LOCAL_TIME time);
	int		SelectActionFromCooltime(std::vector<ActionTable>& AtArray, LOCAL_TIME time);
	bool	IsPassedCoolTime(ActionTable& at, LOCAL_TIME time);

	void	OnAssault(int state, float fLength, LOCAL_TIME time);
	void	OnNormal(int state, float fLength, LOCAL_TIME time);
	
	bool	LoadScript(const char* szFilename);
	bool	FindTarget();
	static  void ReloadAllScript();
	
};

