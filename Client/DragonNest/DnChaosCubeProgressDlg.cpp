#include "StdAfx.h"


#ifdef PRE_ADD_CHAOSCUBE

#include "DnItemTask.h"

#include "DnInterface.h"
#include "DnChaosCubeDlg.h"
#include "DnChoiceCubeDlg.h"
#include "DnChaosCubeProgressDlg.h"


#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif 

CDnChaosCubeProgressDlg::CDnChaosCubeProgressDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback )
: CEtUIDialog( dialogType, pParentDialog, nID, pCallback, true )
, m_pText( NULL )
, m_pCancelButton( NULL )
, m_pProgressBarTime( NULL )
, m_fTimer(0.0f)
, m_pRequestData(NULL)
, m_cubeType( ECUBETYPE::ECUBETYPE_NONE )
{	
}

CDnChaosCubeProgressDlg::~CDnChaosCubeProgressDlg(void)
{
	SAFE_DELETE( m_pRequestData );
}

void CDnChaosCubeProgressDlg::Initialize( bool bShow )
{
	//CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "ProgressMBox.ui" ).c_str(), bShow );
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "RandomItemDlg.ui" ).c_str(), bShow );
}

void CDnChaosCubeProgressDlg::InitialUpdate()
{
	m_pRequestData = new SCChaosCubeRequest;
	m_pText = GetControl<CEtUIStatic>("ID_TEXT");
	m_pText->SetText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 7029 ) ); // "재료가 카오스큐브에서 변형되는 중입니다."

	m_pCancelButton = GetControl<CEtUIButton>("ID_BUTTON_CANCEL");
	m_pProgressBarTime = GetControl<CEtUIProgressBar>("ID_PROGRESSBAR_TIME");
}

void CDnChaosCubeProgressDlg::Process( float fElapsedTime )
{
	CEtUIDialog::Process( fElapsedTime );

	if( !IsShow() ) 
		return;
	
	m_fTimer += fElapsedTime;
	if( m_fTimer >= 1.5f ) 
	{
		// ChaosCube.
		if( m_cubeType == ECUBETYPE::ECUBETYPE_CHAOS )
		{
			CDnItem * pChaosItem = GetInterface().GetChaosCubeDlg()->GetChaosItem();

			CSChaosCubeComplete packet;
			if( pChaosItem->IsCashItem() )
				packet.cCubeInvenType = ITEMPOSITION_CASHINVEN;
			else
				packet.cCubeInvenType = ITEMPOSITION_INVEN;
			packet.sCubeInvenIndex = pChaosItem->GetSlotIndex();
			packet.biCubeInvenSerial = pChaosItem->GetSerialID();

			packet.cInvenType = m_pRequestData->cInvenType;
			packet.nCount = m_pRequestData->nCount;
			memcpy_s( packet.ChaosItem, sizeof(TChaosItem) * MAX_CHAOSCUBE_STUFF, m_pRequestData->ChaosItem, sizeof(TChaosItem) * MAX_CHAOSCUBE_STUFF );

			// Send.
			GetItemTask().CompleteChaosCube( packet );
		}

#ifdef PRE_ADD_CHOICECUBE
		// ChoiceCube.
		else if( m_cubeType == ECUBETYPE::ECUBETYPE_CHOICE )
		{	
			CDnItem * pChoiceItem = GetInterface().GetChoiceCubeDlg()->GetChoiceItem();		

			CSChaosCubeComplete packet;
			if( pChoiceItem->IsCashItem() )
				packet.cCubeInvenType = ITEMPOSITION_CASHINVEN;
			else
				packet.cCubeInvenType = ITEMPOSITION_INVEN;
			packet.sCubeInvenIndex = pChoiceItem->GetSlotIndex();
			packet.biCubeInvenSerial = pChoiceItem->GetSerialID();

			packet.cInvenType = m_pRequestData->cInvenType;
			packet.nCount = m_pRequestData->nCount;

			packet.nChoiceItemID = GetInterface().GetChoiceCubeDlg()->GetResultItemID();

			memcpy_s( packet.ChaosItem, sizeof(TChaosItem) * MAX_CHAOSCUBE_STUFF, m_pRequestData->ChaosItem, sizeof(TChaosItem) * MAX_CHAOSCUBE_STUFF );

			// Send.
			GetItemTask().CompleteChaosCube( packet );
		}
#endif

		
		Show( false );
	}
	m_pProgressBarTime->SetProgress( ( m_fTimer / 1.50f ) * 100.0f );
	
}

void CDnChaosCubeProgressDlg::ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg )
{
	SetCmdControlName( pControl->GetControlName() );

	if( nCommand == EVENT_BUTTON_CLICKED )
	{
		// 취소.
		if( IsCmdControl("ID_BUTTON_CANCEL" ) ) {
			GetItemTask().RequestCancelChaosCube();
			Show(false);
		}		

		//CDnTradeMail& tradeMail = GetTradeTask().GetTradeMail();
		//if (tradeMail.IsOnMailMode())
		//	tradeMail.LockMailDlg(false);

		//GetInterface().CloseMovieDlg();
	}

	CEtUIDialog::ProcessCommand( nCommand, bTriggeredByUser, pControl, uMsg );
}

void CDnChaosCubeProgressDlg::Show( bool bShow ) 
{
	if( m_bShow == bShow )
		return;

	if( !bShow )
	{
		m_fTimer = 0.f;

		m_cubeType = ECUBETYPE::ECUBETYPE_NONE;
	}

	CEtUIDialog::Show( bShow );
}


void CDnChaosCubeProgressDlg::SetRequestData( struct SCChaosCubeRequest * pData, ECUBETYPE type ){
	m_pRequestData->cInvenType = pData->cInvenType;
	m_pRequestData->nCount = pData->nCount;
	memcpy_s( m_pRequestData->ChaosItem, sizeof(TChaosItem) * MAX_CHAOSCUBE_STUFF, pData->ChaosItem, sizeof(TChaosItem) * MAX_CHAOSCUBE_STUFF );

	m_cubeType = type;
}

#endif