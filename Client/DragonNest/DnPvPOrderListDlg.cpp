#include "StdAfx.h"
#include "DnPvPOrderListDlg.h"
#include "DnActor.h"
#include "DnPartyTask.h"
#include "PvPSendPacket.h"
#include "DnInterface.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnPvPOrderListDlg::CDnPvPOrderListDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback )
{
	m_pStaticGroupCaptain = NULL;
	m_pStaticTimeLimit = NULL;
	m_pStaticTitleMsg = NULL;
	m_uiGroupCaptainSessionID = 0;
	m_nCurrentUserSize = 0;

	m_bAutoCursor = true;
	m_fTimeLimit = eOrderListCommon::TIME_LIMIT;
}

CDnPvPOrderListDlg::~CDnPvPOrderListDlg()
{
}

void CDnPvPOrderListDlg::InitialUpdate()
{
	for(int i=0; i<eOrderListCommon::MAX_PLAYER; i++)
	{
		m_sSelectSlot[i].pStatic_Selection = GetControl<CEtUIStatic>( FormatA( "ID_STATIC_SELECT%d" , i ).c_str() );
		m_sSelectSlot[i].pStatic_Selection->Show(false);
		m_sSelectSlot[i].pStatic_Name = GetControl<CEtUIStatic>( FormatA( "ID_TEXT_NAME%d" , i ).c_str() );
		m_sSelectSlot[i].pStatic_Ready = GetControl<CEtUIStatic>( FormatA( "ID_STATIC_READY%d" , i ).c_str() );
		m_sSelectSlot[i].pStatic_Ready->Show(false);
	}

	m_pStaticGroupCaptain = GetControl<CEtUIStatic>("ID_STATIC_CAPTAIN");
	m_pStaticGroupCaptain->Show(true);
	m_pStaticTimeLimit = GetControl<CEtUIStatic>("ID_TEXT_TIME");
	m_pStaticTitleMsg = GetControl<CEtUIStatic>("ID_TEXT0");
}

void CDnPvPOrderListDlg::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "PvpOrderListDlg.ui" ).c_str(), bShow );
}

void CDnPvPOrderListDlg::ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg )
{
	CEtUIDialog::ProcessCommand( nCommand, bTriggeredByUser, pControl, uMsg );
}

void CDnPvPOrderListDlg::Show( bool bShow )
{
	CEtUIDialog::Show( bShow );
}

void CDnPvPOrderListDlg::StartCountDown()
{
	m_fTimeLimit = eOrderListCommon::TIME_LIMIT;
	Show( true );
}

void CDnPvPOrderListDlg::Process( float fElapsedTime )
{
	if(IsShow())
	{
		if(m_fTimeLimit >= 0)
		{
			m_fTimeLimit -= fElapsedTime;
		}
		else
		{
			Show(false);
		}

		m_pStaticTimeLimit->SetText( FormatW( GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 121127) , (int)m_fTimeLimit ) );
	}

	CEtUIDialog::Process( fElapsedTime );
}

bool CDnPvPOrderListDlg::MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	if( !IsShow() || !CDnActor::s_hLocalActor ) 
		return false;

	if( CDnActor::s_hLocalActor->GetUniqueID() != m_uiGroupCaptainSessionID  )
		return false;
	
	POINT MousePoint;
	float fMouseX, fMouseY;

	MousePoint.x = short( LOWORD( lParam ) );
	MousePoint.y = short( HIWORD( lParam ) );
	PointToFloat( MousePoint, fMouseX, fMouseY );

	switch( uMsg )
	{

	case WM_LBUTTONDOWN:
		{
			for(int n = 0; n < m_nCurrentUserSize; n++ )
			{
				if(m_sSelectSlot[n].pStatic_Selection->IsInside(fMouseX,fMouseY))
				{
					if( m_sSelectSlot[n].bIsAlive )
					{
						m_sSelectSlot[n].pStatic_Selection->Show(true);
						SendPvPAllKillSelectPlayer(m_sSelectSlot[n].nUserSessionID);
					}
				}
				else
					m_sSelectSlot[n].pStatic_Selection->Show(false);
			}
		}
		break;
	}

	return CEtUIDialog::MsgProc( hWnd , uMsg , wParam , lParam );
}


void CDnPvPOrderListDlg::SelectPlayer( int nUserSessionID )
{
	for(int n=0;n<eOrderListCommon::MAX_PLAYER;n++)
	{
		if(m_sSelectSlot[n].nUserSessionID == nUserSessionID )
			m_sSelectSlot[n].pStatic_Ready->Show(true);
		else
			m_sSelectSlot[n].pStatic_Ready->Show(false);

	}
}

void CDnPvPOrderListDlg::SelectGroupCaptain( int nUserSessionID )
{
	m_uiGroupCaptainSessionID = nUserSessionID;
	MakePlayerList(); 

	if( CDnActor::s_hLocalActor->GetUniqueID() == m_uiGroupCaptainSessionID  )
	{
		m_pStaticTitleMsg->SetTextColor( textcolor::ORANGE );
		m_pStaticTitleMsg->SetText( GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 121126) );
	}
	else
	{
		m_pStaticTitleMsg->SetTextColor( textcolor::LIGHTGREY );
		m_pStaticTitleMsg->SetText( GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 121134) );
	}

}


void CDnPvPOrderListDlg::MakePlayerList()
{
	if(!CDnActor::s_hLocalActor) return;

	int nAddUserCount = 0;

	
	if( CDnPartyTask::IsActive() ) 
	{
		// For GroupCaptain Priority
		CDnPartyTask::PartyStruct *pStruct = CDnPartyTask::GetInstance().GetPartyDataFromSessionID(m_uiGroupCaptainSessionID);
		if(pStruct && pStruct->hActor && pStruct->hActor->IsPlayerActor() )
		{
			m_sSelectSlot[GROUPCAPTAIN_POSITION].pStatic_Name->SetText(pStruct->hActor->GetName());
			m_sSelectSlot[GROUPCAPTAIN_POSITION].bIsAlive = !pStruct->hActor->IsDie();
			m_sSelectSlot[GROUPCAPTAIN_POSITION].nUserSessionID = pStruct->nSessionID;
			if(pStruct->hActor->IsDie())
				m_sSelectSlot[GROUPCAPTAIN_POSITION].pStatic_Name->SetTextColor( textcolor::RED );

			nAddUserCount++;
		}

		for( DWORD i=0; i<CDnPartyTask::GetInstance().GetPartyCount(); i++ ) 
		{
			CDnPartyTask::PartyStruct *pStruct = CDnPartyTask::GetInstance().GetPartyData(i);
			if(pStruct && pStruct->hActor && pStruct->hActor->IsPlayerActor() && nAddUserCount < eOrderListCommon::MAX_PLAYER )
			{
				if(	pStruct->hActor->GetTeam() == CDnActor::s_hLocalActor->GetTeam() && pStruct->nSessionID != m_uiGroupCaptainSessionID )
				{
					m_sSelectSlot[nAddUserCount].pStatic_Name->SetText(pStruct->hActor->GetName());
					m_sSelectSlot[nAddUserCount].bIsAlive = !pStruct->hActor->IsDie();
					m_sSelectSlot[nAddUserCount].nUserSessionID = pStruct->nSessionID;
					if(pStruct->hActor->IsDie())
						m_sSelectSlot[nAddUserCount].pStatic_Name->SetTextColor( textcolor::RED );
					else
						m_sSelectSlot[nAddUserCount].pStatic_Name->SetTextColor( textcolor::WHITE );


					nAddUserCount++;
				}
			}
		}
	}

	for(int n = nAddUserCount; n<eOrderListCommon::MAX_PLAYER; n++ )
	{
		m_sSelectSlot[n].pStatic_Name->Show(false);
		m_sSelectSlot[n].pStatic_Selection->Show(false);
		m_sSelectSlot[n].pStatic_Ready->Show(false);
		m_sSelectSlot[n].nUserSessionID = 0;
	}

	m_nCurrentUserSize = nAddUserCount;
}
