
#include "stdafx.h"
#include "PvPRoundModeScoreSystem.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif 
void CPvPRoundModeScoreSystem::OnDie( DnActorHandle hActor, DnActorHandle hHitter )
{
	// PvPRoundMode 의 스코어는 서버에서 계산되어 보내진다.
}
