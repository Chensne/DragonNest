#include "StdAfx.h"
#include "DnMutatorGame.h"
#include "DnPlayerActor.h"
#include "DnInterface.h"
#include "ScoreSystem.h"
#include "DnPvPBaseHud.h"
#include "DnMonsterActor.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif 
CDnMutatorGame::CDnMutatorGame( CDnPvPGameTask * pGameTask ):m_pGameTask(pGameTask),m_pScoreSystem(NULL)
{
}

CDnMutatorGame::~CDnMutatorGame()
{
	SAFE_DELETE( m_pScoreSystem );
}

void CDnMutatorGame::ProcessExitUser( DnActorHandle hActor )
{	
	if( !hActor)
		return;

	WCHAR wszMessage[256];

	SecureZeroMemory(wszMessage,sizeof(wszMessage));

	CDnPlayerActor* pPlayerActor = dynamic_cast<CDnPlayerActor*>(hActor.GetPointer());

	if( pPlayerActor )
	{
		if( hActor->GetTeam() != PvPCommon::Team::Observer 

			)
		{
			if( !CDnBridgeTask::GetInstance().IsPvPGameMode( PvPCommon::GameMode::PvP_GuildWar ) )
			{
				wsprintf(wszMessage,GetEtUIXML().GetUIString( CEtUIXML::idCategory1, GameString::ExitGame ),pPlayerActor->GetName());
				GetInterface().AddMessageText( wszMessage , textcolor::WHITE );
				GetInterface().AddChatMessage(CHATTYPE_SYSTEM, L"", wszMessage);
			}
			GetInterface().RemovePVPGameUer( hActor );
		}

	}

}

//===============================================================================================================================
// ScoreSystem
//===============================================================================================================================

bool CDnMutatorGame::InitializeBase( const UINT uiItemID, DNTableFileFormat*  pSox )
{
	m_pScoreSystem = CreateScoreSystem();
	if( !m_pScoreSystem )
		return false;

	return m_pScoreSystem->InitializeBase( uiItemID, pSox );
}

void CDnMutatorGame::OnDie( DnActorHandle hActor, DnActorHandle hHitter )
{
	if( m_pScoreSystem )
		m_pScoreSystem->OnDie( hActor, hHitter );	

	ProcessActorDie( hHitter, hActor );

	if( hActor == CDnActor::s_hLocalActor && m_pScoreSystem && hHitter ) 
	{
		WCHAR *wstrKillerName = hHitter->GetName();
		int nKillCount = m_pScoreSystem->GetKillCount( hHitter );

		if(hHitter->IsMonsterActor())
		{
			CDnMonsterActor *pMonster = static_cast<CDnMonsterActor*>(hHitter.GetPointer());
			if( pMonster->GetSummonerPlayerActor() )
			{
				wstrKillerName = pMonster->GetSummonerPlayerActor()->GetName();
				nKillCount = m_pScoreSystem->GetKillCount( pMonster->GetSummonerPlayerActor() );
			}
		}

		GetInterface().ShowPvPKilledMeDlg( true, wstrKillerName , nKillCount );
	}
	if( hActor && hHitter ) {
		if( GetInterface().GetHUD() )
		{
			DnWeaponHandle hWeapon = hHitter->GetWeapon( 0, false );

			if(hHitter->GetActorType() == CDnActorState::Soceress)
				hWeapon = hHitter->GetWeapon(1, false);

			if( hWeapon ) {
				GetInterface().GetHUD()->ShowKillInfo( hHitter, hActor, hWeapon->GetEquipType() );
			}
		}
	}
}

void CDnMutatorGame::OnDamage( DnActorHandle hActor, DnActorHandle hHitter, const INT64 iDamage )
{
	if( m_pScoreSystem )
		m_pScoreSystem->OnDamage( hActor, hHitter, iDamage );
}

void CDnMutatorGame::OnCmdAddStateEffect( const CDnSkill::SkillInfo* pSkillInfo )
{
	if( m_pScoreSystem )
		m_pScoreSystem->OnCmdAddStateEffect( pSkillInfo );
}

void CDnMutatorGame::OnLeaveUser( DnActorHandle hActor )
{
	if( m_pScoreSystem )
		m_pScoreSystem->OnLeaveUser( hActor );

	ProcessExitUser( hActor );
}
