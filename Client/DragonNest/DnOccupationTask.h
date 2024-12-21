#pragma once

#include "Task.h"
#include "MessageListener.h"
#include "DnOccupationZone.h"

class CDnWorldOperationProp;
class IScoreSystem;

class CDnOccupationTask : public CTask, public CTaskListener, public CEtUICallback, public CSingleton<CDnOccupationTask>
{
public:
	CDnOccupationTask();
	virtual ~CDnOccupationTask();

	bool Initialize( int nGameMode );
	void Finalize();
	virtual void Process( LOCAL_TIME LocalTime, float fDelta );
	virtual void OnUICallbackProc(int nID, UINT nCommand, CEtUIControl *pControl, UINT uiMsg);

	enum{
		STRING_IDLE_CMD = 0,
		STRING_TRY_CMD,
		STRING_MOVEWAIT_CMD,
		STRING_OWNED_CMD,
		STRING_MAX_CMD
	};

public:
	void TryAcquirePoint( int nAreaID );
	void OccupationPoint( SCPvPOccupationState * pData );
	void SetResource( SCPvPOccupationTeamState * pData );
	int  GetResource( PvPCommon::Team::eTeam eTeam );
	int  GetTotalResource( PvPCommon::Team::eTeam eTeam );

	void FlagOperation( DnPropHandle hProp );
	bool IsFlagOperation( DnPropHandle hProp );

	void SetBreakInfoUser( bool bBreakInfoUser ) { m_bBreakInfoUser = bBreakInfoUser; }
	void SetScoreSystem( IScoreSystem * pScoreSystem );

	std::vector<CDnOccupationZone *> & GetFlagVector() { return m_vFlag; }
	CDnOccupationZone * GetFlag( int nAreaID );
	CDnOccupationZone * GetFlag( DnPropHandle hProp );

	const char * GetAnimationName( int eString, int eTeam );

protected:
	int  m_nTableModeID;

	bool m_bProgress;
	bool m_bChangeAction;		// 점령액션이 끝난 후에 변경 될 필요가 없을 경우 : true
	bool m_bBreakInfoUser;		// 난입 유저는 처음 패킷 받았을 때 처리 하지 말아야 할것들 막아둔다.
	int  m_nTrySound;
	int  m_nTryArea;

	int m_nRedTeamResource;
	int m_nRedTeamTotalResource;
	int m_nBlueTeamResource;
	int m_nBlueTeamTotalResource;

	std::vector<CDnOccupationZone *>	m_vFlag;

	std::string m_BlueAniCmd[STRING_MAX_CMD];
	std::string m_RedAniCmd[STRING_MAX_CMD];

	IScoreSystem*	m_pScoreSystem;
};

#define GetOccupationTask()		CDnOccupationTask::GetInstance()
