#include "stdafx.h"
#include "DnWorldKeepOperationProp.h"
#include "DnLocalPlayerActor.h"
#include "DnInterface.h"
#include "DnMainMenuDlg.h"
#include "DnCharVehicleDlg.h"


#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnWorldKeepOperationProp::CDnWorldKeepOperationProp()
: CDnWorldOperationProp()
, m_bProgress( false )
, m_bChangeAction( false )
{

}

CDnWorldKeepOperationProp::~CDnWorldKeepOperationProp()
{

}

void CDnWorldKeepOperationProp::OnSetKeepOperationProp( const int nActorHandle, const int nKeepClick, const int nStringID, const int nActionIndex )
{
	if( !CDnActor::s_hLocalActor )
		return;

	CDnLocalPlayerActor * pLocalActor = static_cast<CDnLocalPlayerActor *>( CDnActor::s_hLocalActor.GetPointer() );
	if( NULL == pLocalActor )
		return;

	if( pLocalActor->GetUniqueID() != nActorHandle )
		return;

	if( pLocalActor->IsVehicleMode() )
	{
		CDnCharStatusDlg * pCharStatusDlg = (CDnCharStatusDlg *)GetInterface().GetMainMenuDialog( CDnMainMenuDlg::CHARSTATUS_DIALOG );
		CDnCharVehicleDlg * pCharVehicleDlg = pCharStatusDlg->GetVehicleDlg();

		if( pCharVehicleDlg )
			pCharVehicleDlg->ForceUnRideVehicle();
	}

	pLocalActor->CmdAction( "Stand" );
	pLocalActor->ProcessFlushPacketQueue();
	pLocalActor->CmdToggleBattle( false );
	pLocalActor->CmdAction( pLocalActor->GetElement( nActionIndex )->szName.c_str() );

	m_bProgress = true;

	GetInterface().OpenMovieProcessDlg( NULL, (float)nKeepClick, nStringID != 0 ? GetEtUIXML().GetUIString( CEtUIXML::idCategory1, nStringID ) : L"", KEEP_REQUEST_DIALOG, this, false );
	CDnMouseCursor::GetInstance().ShowCursor( false );
}

void CDnWorldKeepOperationProp::OnCancelKeepOperationProp()
{
	m_bProgress = false;
	GetInterface().CloseMovieProcessDlg();
}

void CDnWorldKeepOperationProp::OnUICallbackProc( int nID, UINT nCommand, CEtUIControl * pControl, UINT uiMsg )
{
	switch( nID ) 
	{
		case KEEP_REQUEST_DIALOG:
		{
			if( nCommand == EVENT_BUTTON_CLICKED )
			{
				if( strcmp( pControl->GetControlName(), "ID_BUTTON_CANCEL") == 0 )
				{
					SendKeepCancle();
					if( m_bProgress )
					{
						m_bProgress = false;
						GetInterface().CloseMovieProcessDlg();
					}
				}

				if( !CDnActor::s_hLocalActor ) return;
				CDnLocalPlayerActor * localActor = ((CDnLocalPlayerActor *)(CDnActor::s_hLocalActor.GetPointer()));
				if( !localActor) return;

				if( localActor->IsCanBattleMode() )
					localActor->CmdToggleBattle( true );

				if( m_bChangeAction )
					localActor->CmdAction("Jump");
				else
					localActor->CmdAction( "Stand" );

				m_bChangeAction = false;
				m_bProgress = false;
			}
			break;
		}
	}
}

void CDnWorldKeepOperationProp::Process( LOCAL_TIME LocalTime, float fDelta )
{
	if( m_bProgress )
	{
		if( !CDnActor::s_hLocalActor ) return;
		CDnPlayerActor * pPlayerActor = (CDnPlayerActor *)(CDnActor::s_hLocalActor.GetPointer());
		if( !pPlayerActor ) return;

		m_bChangeAction = pPlayerActor->IsJump();

		if( pPlayerActor->IsMove() || pPlayerActor->IsHit() || pPlayerActor->IsBattleMode() || m_bChangeAction )
			OnCancelKeepOperationProp();
	}

	CDnWorldOperationProp::Process( LocalTime, fDelta );
}

void CDnWorldKeepOperationProp::OnDispatchMessage( DWORD dwProtocol, BYTE *pPacket )
{
	switch( dwProtocol ) {
		case eProp::SC_CMDKEEPCLICKSTART:
			{
				CPacketCompressStream Stream( pPacket, 128 );
				int nActionIndex, nOperationTime, nStringID, nActorHandle;

				Stream.Read( &nActionIndex, sizeof(int) );
				Stream.Read( &nActorHandle, sizeof(int) );
				Stream.Read( &nOperationTime, sizeof(int) );
				Stream.Read( &nStringID, sizeof(int) );

				OnSetKeepOperationProp( nActorHandle, nOperationTime / 1000, nStringID, nActionIndex );
			}
			break;
	}
	CDnWorldOperationProp::OnDispatchMessage( dwProtocol, pPacket );
}

void CDnWorldKeepOperationProp::SendKeepCancle()
{
	FUNC_LOG();
	Send( eProp::CS_CMDKEEPCLICKCANCLE, NULL );
}