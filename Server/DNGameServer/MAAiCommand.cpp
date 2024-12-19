
#include "stdafx.h"
#include "MAAiCommand.h"
#include "MAAiScript.h"
#include "DnBlow.h"

using namespace AICommand;

void MAAiCommand::MsgProc( MAAiScript* pAIScript, const UINT uiMsg, void* pParam )
{
	switch( uiMsg )
	{
		case CMD_USESKILL:
		{
			SUseSkill* pUseSkill = static_cast<SUseSkill*>(pParam);

			DnActorHandle hActor = pAIScript->GetActor();
			if( hActor )
			{
				// 시킬 사용중이라면 강제로 종료시킨다.
				hActor->ResetProcessSkill();
				
				if( CDnSkill::UsingResult::Success == hActor->UseSkill( pUseSkill->iSkillIndex ) )
				{
					pAIScript->SetAIState( MAAiScript::AT_UseSkill );
				}
			}
			break;
		}
		case CMD_REMOVEBLOW_BYSKILLINDEX:
		{
			SRemoveBlowBySkillIndex* pData = static_cast<SRemoveBlowBySkillIndex*>(pParam);

			DnActorHandle hActor = pAIScript->GetActor();
			if( hActor )
			{
				DNVector(DnBlowHandle) BlowList;
				hActor->GetAllAppliedStateBlowBySkillID( pData->iSkillIndex, BlowList );
				for( size_t i=0 ; i<BlowList.size() ; ++i )
				{
					DnBlowHandle hBlow = BlowList[i];
					hActor->CmdRemoveStateEffect( hBlow->GetBlowIndex() );
				}
			}
			break;
		}
	}
}
