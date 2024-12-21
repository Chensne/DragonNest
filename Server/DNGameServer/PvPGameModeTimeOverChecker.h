
#pragma once

#include "PvPGameModeChecker.h"

class CPvPGameModeTimeOverChecker:public IPvPGameModeChecker,public TBoostMemoryPool<CPvPGameModeTimeOverChecker>
{
public:

	CPvPGameModeTimeOverChecker( CPvPGameMode* pGameMode ):IPvPGameModeChecker(pGameMode){}
	virtual ~CPvPGameModeTimeOverChecker(){}

	virtual void Check()
	{
		if( !m_pPvPGameMode )
			return;

		if( !(m_pPvPGameMode->GetGameModeCheck()&PvPCommon::Check::CheckTimeOver) )
			return;

		if( !m_pPvPGameMode->bIsFinishFlag() )
		{
			// TimeOver 체크
			if( m_pPvPGameMode->GetTimeOver() <= 0.f )
				m_pPvPGameMode->OnCheckFinishRound( PvPCommon::FinishReason::TimeOver );
		}
	}
};
