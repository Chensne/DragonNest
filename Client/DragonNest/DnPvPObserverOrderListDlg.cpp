#include "StdAfx.h"
#include "DnPvPObserverOrderListDlg.h"
#include "DnActor.h"
#include "DnLocalPlayerActor.h"
#include "DnPartyTask.h"
#include "PvPSendPacket.h"
#include "DnInterface.h"


#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif


#ifdef PRE_MOD_PVPOBSERVER

CDnPvPObserverOrderListDlg::CDnPvPObserverOrderListDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback )
{
	m_uiGroupCaptainSessionID[ETeamInfo_BLUE] = m_uiGroupCaptainSessionID[ETeamInfo_RED] = 0;

	m_bAutoCursor = true;
	m_fTimeLimit = eOrderListCommon::TIME_LIMIT;

	m_bForceView = false;
	m_nCrrCaptainOrder = 0;
}

CDnPvPObserverOrderListDlg::~CDnPvPObserverOrderListDlg()
{
}

void CDnPvPObserverOrderListDlg::InitialUpdate()
{
	char * arrStaticSelect[128] = { "ID_STATIC_BLUESELECT%d", "ID_STATIC_REDSELECT%d" };
	char * arrStaticName[128] = { "ID_TEXT_BLUENAME%d", "ID_TEXT_REDNAME%d" };
	char * arrStaticReady[128] = { "ID_STATIC_BLUEREADY%d", "ID_STATIC_REDREADY%d" };
	char * arrStaticCaptain[128] = { "ID_STATIC_BLUECAPTAIN", "ID_STATIC_REDCAPTAIN" };
	char * arrStaticTime[128] = { "ID_TEXT_BLUETIME", "ID_TEXT_REDTIME" };

	for( int k=0; k<ETeamInfo::ETeamInfo_MAX; ++k )
	{
		for(int i=0; i<eOrderListCommon::MAX_PLAYER; i++)
		{
			m_SelectPlayer[ k ].m_sSelectSlot[i].pStatic_Selection = GetControl<CEtUIStatic>( FormatA( arrStaticSelect[ k ] , i ).c_str() );
			m_SelectPlayer[ k ].m_sSelectSlot[i].pStatic_Selection->Show(false);
			m_SelectPlayer[ k ].m_sSelectSlot[i].pStatic_Name = GetControl<CEtUIStatic>( FormatA( arrStaticName[ k ] , i ).c_str() );
			m_SelectPlayer[ k ].m_sSelectSlot[i].pStatic_Ready = GetControl<CEtUIStatic>( FormatA( arrStaticReady[ k ] , i ).c_str() );
			m_SelectPlayer[ k ].m_sSelectSlot[i].pStatic_Ready->Show(false);
		}

		m_SelectPlayer[ k ].m_pStaticGroupCaptain = GetControl<CEtUIStatic>( arrStaticCaptain[ k ] );
		m_SelectPlayer[ k ].m_pStaticGroupCaptain->Show(true);

		m_SelectPlayer[ k ].m_pStaticTimeLimit = GetControl<CEtUIStatic>( FormatA( arrStaticTime[ k ] ).c_str() );
	}
}

void CDnPvPObserverOrderListDlg::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "PvpOrder_TeamListDlg.ui" ).c_str(), bShow );
}

void CDnPvPObserverOrderListDlg::ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg )
{
	CEtUIDialog::ProcessCommand( nCommand, bTriggeredByUser, pControl, uMsg );
}

void CDnPvPObserverOrderListDlg::ForceView(  bool bShow )
{
	m_bForceView = bShow;

	if( bShow == true )
	{
		MakePlayerList();
	}
	
	Show( bShow );
}

void CDnPvPObserverOrderListDlg::StartCountDown()
{
	m_bForceView = false;
	m_fTimeLimit = eOrderListCommon::TIME_LIMIT;

	for( int k=0; k<ETeamInfo::ETeamInfo_MAX; ++k )
	{
		m_SelectPlayer[ k ].m_pStaticTimeLimit->Show( true );
	}
	Show( true );
}

void CDnPvPObserverOrderListDlg::Show( bool bShow )
{
	if( !bShow )
	{
		m_nCrrCaptainOrder = 0;
	}

	CEtUIDialog::Show( bShow );
}

void CDnPvPObserverOrderListDlg::Process( float fElapsedTime )
{
	if(m_fTimeLimit >= 0)
	{
		m_fTimeLimit -= fElapsedTime;
	}

	if(IsShow())
	{
		if( m_fTimeLimit < 0 )
		{
			if( m_bForceView == true )
			{
				for( int k=0; k<ETeamInfo::ETeamInfo_MAX; ++k )
				{
					m_SelectPlayer[ k ].m_pStaticTimeLimit->Show( false );
				}
			}
			else
			{
				Show(false);
			}
		}

		for( int k=0; k<ETeamInfo::ETeamInfo_MAX; ++k )
		{
			m_SelectPlayer[ k ].m_pStaticTimeLimit->SetText( FormatW( GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 121127) , (int)m_fTimeLimit ) );
		}
	}

	CEtUIDialog::Process( fElapsedTime );
}

void CDnPvPObserverOrderListDlg::SelectPlayer( int nUserSessionID, int orderListCnt )
{
	if( !CDnActor::s_hLocalActor )
		return;

	CDnLocalPlayerActor * pLocalPlayer = reinterpret_cast<CDnLocalPlayerActor*>( CDnActor::s_hLocalActor.GetPointer() );
	
	// 운영자계정 일때만 선택표시.
	if( pLocalPlayer->IsDeveloperAccountLevel() || orderListCnt > 1 )
	{
		// 해당 유저의 팀을 구함.
		int team = -1;
		for( int k=0; k<ETeamInfo_MAX; ++k )
		{	
			if( team != -1 )
				break;

			for(int n=0; n<eOrderListCommon::MAX_PLAYER;n++)
			{
				if( m_SelectPlayer[k].m_sSelectSlot[n].nUserSessionID == nUserSessionID )
				{
					team = k;
					break;
				}
			}			
		}

		// 레디체크.
		if( team != -1 )
		{
			for(int n=0; n<eOrderListCommon::MAX_PLAYER;n++)
			{
				if( m_SelectPlayer[ team ].m_sSelectSlot[n].nUserSessionID == nUserSessionID )
					m_SelectPlayer[ team ].m_sSelectSlot[n].pStatic_Ready->Show(true);
				else
					m_SelectPlayer[ team ].m_sSelectSlot[n].pStatic_Ready->Show(false);
			}	
		}	
	}

}

void CDnPvPObserverOrderListDlg::SelectGroupCaptain( int nUserSessionID )
{	
	if( m_nCrrCaptainOrder < ETeamInfo_MAX )
	{
		m_uiGroupCaptainSessionID[ m_nCrrCaptainOrder ] = nUserSessionID;
		MakePlayerList(); 
		++m_nCrrCaptainOrder;
	}
}


void CDnPvPObserverOrderListDlg::MakePlayerList()
{
	if(!CDnActor::s_hLocalActor) return;

	int nAddUserCount[ ETeamInfo_MAX ];
	nAddUserCount[ ETeamInfo_BLUE ] = nAddUserCount[ ETeamInfo_RED ] = 0;
	
	if( CDnPartyTask::IsActive() ) 
	{
		// UINT tempCaption[ ETeamInfo_MAX ];		
			
		int captainTeam = 0;

		// For GroupCaptain Priority
		CDnPartyTask::PartyStruct *pStruct = CDnPartyTask::GetInstance().GetPartyDataFromSessionID( m_uiGroupCaptainSessionID[ m_nCrrCaptainOrder ] );
		if(pStruct && pStruct->hActor && pStruct->hActor->IsPlayerActor() )
		{			
			int teamIndex = 0;
			if( pStruct->usTeam == PvPCommon::Team::eTeam::A )
				teamIndex = (int)ETeamInfo_BLUE;
			else if( pStruct->usTeam == PvPCommon::Team::eTeam::B )
				teamIndex = (int)ETeamInfo_RED;

			captainTeam = pStruct->usTeam;

			//tempCaption[ teamIndex ] = m_uiGroupCaptainSessionID[ k ]; // m_uiGroupCaptainSessionID 는 팀구분없이 패킷온 순서대로 받은 것 이라 팀을 맞추기위함.
							
			m_SelectPlayer[ teamIndex ].m_sSelectSlot[GROUPCAPTAIN_POSITION].pStatic_Name->SetText(pStruct->hActor->GetName());
			m_SelectPlayer[ teamIndex ].m_sSelectSlot[GROUPCAPTAIN_POSITION].bIsAlive = !pStruct->hActor->IsDie();
			m_SelectPlayer[ teamIndex ].m_sSelectSlot[GROUPCAPTAIN_POSITION].nUserSessionID = pStruct->nSessionID;
			if(pStruct->hActor->IsDie())
				m_SelectPlayer[ teamIndex ].m_sSelectSlot[GROUPCAPTAIN_POSITION].pStatic_Name->SetTextColor( textcolor::RED );

			nAddUserCount[ teamIndex ]++;
		}
		

		//m_uiGroupCaptainSessionID[ ETeamInfo_BLUE ] = tempCaption[ ETeamInfo_BLUE ];
		//m_uiGroupCaptainSessionID[ ETeamInfo_RED ] = tempCaption[ ETeamInfo_RED ];

		int partySize = CDnPartyTask::GetInstance().GetPartyCount();
		for( int i=0; i<partySize; i++ ) 
		{
			CDnPartyTask::PartyStruct *pStruct = CDnPartyTask::GetInstance().GetPartyData(i);
			if(pStruct && pStruct->hActor && pStruct->hActor->IsPlayerActor() )
			{
				if( captainTeam && captainTeam != pStruct->usTeam )
					continue;

				int teamIndex = -1;
				if( pStruct->usTeam == PvPCommon::Team::eTeam::A )
					teamIndex = ETeamInfo_BLUE;
				else if( pStruct->usTeam == PvPCommon::Team::eTeam::B )
					teamIndex = ETeamInfo_RED;
				
				if( teamIndex == -1 )
					continue;

				if( nAddUserCount[ teamIndex ] > eOrderListCommon::MAX_PLAYER )
					continue;
				
				if(	pStruct->nSessionID != m_uiGroupCaptainSessionID[ m_nCrrCaptainOrder ] )
				{
					m_SelectPlayer[ teamIndex ].m_sSelectSlot[ nAddUserCount[ teamIndex ] ].pStatic_Name->SetText(pStruct->hActor->GetName());
					m_SelectPlayer[ teamIndex ].m_sSelectSlot[ nAddUserCount[ teamIndex ] ].bIsAlive = !pStruct->hActor->IsDie();
					m_SelectPlayer[ teamIndex ].m_sSelectSlot[ nAddUserCount[ teamIndex ] ].nUserSessionID = pStruct->nSessionID;
					if(pStruct->hActor->IsDie())
						m_SelectPlayer[ teamIndex ].m_sSelectSlot[ nAddUserCount[ teamIndex ] ].pStatic_Name->SetTextColor( textcolor::RED );
					else
						m_SelectPlayer[ teamIndex ].m_sSelectSlot[ nAddUserCount[ teamIndex ] ].pStatic_Name->SetTextColor( textcolor::WHITE );

					nAddUserCount[ teamIndex ]++;					
				}
			}
		}
	}

	for( int k=0; k<ETeamInfo_MAX; ++k )
	{
		for(int n=nAddUserCount[ m_nCrrCaptainOrder ]; n<eOrderListCommon::MAX_PLAYER; n++ )
		{
			m_SelectPlayer[ m_nCrrCaptainOrder ].m_sSelectSlot[n].pStatic_Name->Show(false);
			m_SelectPlayer[ m_nCrrCaptainOrder ].m_sSelectSlot[n].pStatic_Selection->Show(false);
			m_SelectPlayer[ m_nCrrCaptainOrder ].m_sSelectSlot[n].pStatic_Ready->Show(false);
			m_SelectPlayer[ m_nCrrCaptainOrder ].m_sSelectSlot[n].nUserSessionID = 0;
		}
	}
}

#endif // PRE_MOD_PVPOBSERVER