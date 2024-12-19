
#pragma once

#include "PvPScoreSystem.h"

#if defined( PRE_ADD_PVP_COMBOEXERCISE )

class CPvPComboExerciseScoreSystem:public CPvPScoreSystem,public TBoostMemoryPool<CPvPComboExerciseScoreSystem>
{
public:
	CPvPComboExerciseScoreSystem() {}
	virtual ~CPvPComboExerciseScoreSystem() {}

	virtual UINT GetKillCount( DnActorHandle hActor ) { return 0; }
	virtual UINT GetDeathCount( DnActorHandle hActor ) { return 0; }

	virtual UINT GetTotalScore( DnActorHandle hActor ) { return 0; }
	virtual UINT GetKillScore( DnActorHandle hActor ) { return 0; }
	virtual UINT GetAssistScore( DnActorHandle hActor ) { return 0; }
	virtual UINT GetXPScore( DnActorHandle hActor, const UINT uiWinTeam ) { return 0; }
};

#endif // #if defined( PRE_ADD_PVP_COMBOEXERCISE )
