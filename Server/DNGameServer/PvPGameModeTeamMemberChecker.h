
#pragma once

#include "PvPGameModeChecker.h"

class CPvPGameModeTeamMemberChecker:public IPvPGameModeChecker,public TBoostMemoryPool<CPvPGameModeTeamMemberChecker>
{
public:

	CPvPGameModeTeamMemberChecker( CPvPGameMode* pGameMode ):IPvPGameModeChecker(pGameMode){}
	virtual ~CPvPGameModeTeamMemberChecker(){}

	virtual void Check()
	{
		if( !m_pPvPGameMode )
			return;

		if( !(m_pPvPGameMode->GetGameModeCheck()&PvPCommon::Check::CheckTeamMemberZero) )
			return;

		if( !m_pPvPGameMode->bIsFinishFlag()  )
		{
			// 한팀이 전부 나갔는 지 체크
			if( m_pPvPGameMode->OnCheckZeroUserWinTeam() != PvPCommon::Team::Max )
				m_pPvPGameMode->OnCheckFinishRound( PvPCommon::FinishReason::OpponentTeamAllGone );
		}
	}
};
