
#include "stdafx.h"
#include "PvPRespawnModeScoreSystem.h"
#include "DnActor.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif 
void CPvPRespawnModeScoreSystem::OnDie( DnActorHandle hActor, DnActorHandle hHitter )
{
	if( !hActor )
		return;

	int iTeam = hActor->GetTeam();

	switch( iTeam )
	{
		case PvPCommon::Team::A:
		{
			++m_uiScore[PvPCommon::TeamIndex::B];
			break;
		}
		case PvPCommon::Team::B:
		{
			++m_uiScore[PvPCommon::TeamIndex::A];
			break;
		}
	}
}
