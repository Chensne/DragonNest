#include "StdAfx.h"
#include "DnSpecialBoxTabItemDlg.h"
#include "DnSpecialBoxListDlg.h"
#include "DnEventReceiverTabDlg.h"
#include "DnMainMenuDlg.h"
#include "DnInterface.h"
#include "DnInterfaceString.h"
#include "DnPlayerActor.h"
#include "TradeSendPacket.h"

#ifdef PRE_SPECIALBOX

CDnSpecialBoxTabItemDlg::CDnSpecialBoxTabItemDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback )
: CEtUIDialog( dialogType, pParentDialog, nID, pCallback )
{
	for( int i=0; i< eStaticMember::Max; i++ )
		m_pStatic[i] = NULL;

	m_pButtonAll = NULL;
	m_pButtonChoice = NULL;

	m_bConditionCheck = false;
	memset( &m_SpecialBoxInfo , 0 , sizeof(m_SpecialBoxInfo) );
}

CDnSpecialBoxTabItemDlg::~CDnSpecialBoxTabItemDlg(void)
{
}

void CDnSpecialBoxTabItemDlg::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "KeepBoxCabinetListDlg.ui" ).c_str(), bShow );
}

void CDnSpecialBoxTabItemDlg::InitialUpdate()
{
	m_pStatic[eStaticMember::Title] = GetControl<CEtUIStatic>("ID_TEXT_TITLE");
	m_pStatic[eStaticMember::Condition] = GetControl<CEtUIStatic>("ID_TEXT_GETITEM");
	m_pStatic[eStaticMember::ReceiveCondition] = GetControl<CEtUIStatic>("ID_TEXT_GETITEM2");
	m_pStatic[eStaticMember::Date] = GetControl<CEtUIStatic>("ID_TEXT_TIME");
	m_pStatic[eStaticMember::Level] = GetControl<CEtUIStatic>("ID_TEXT_LEVEL");
	m_pStatic[eStaticMember::Job] = GetControl<CEtUIStatic>("ID_TEXT_CLASS");
	m_pStatic[eStaticMember::From] = GetControl<CEtUIStatic>("ID_TEXT_FROM");
	m_pStatic[eStaticMember::Arrow] = GetControl<CEtUIStatic>("ID_STATIC0");
	m_pStatic[eStaticMember::Cover] = GetControl<CEtUIStatic>("ID_STATIC1");
	m_pStatic[eStaticMember::CoverBar] = GetControl<CEtUIStatic>("ID_STATIC2");


	m_pButtonAll = GetControl<CEtUIButton>("ID_BT_OK0");
	m_pButtonAll->Show( false );

	m_pButtonChoice = GetControl<CEtUIButton>("ID_BT_OK1");
	m_pButtonChoice->Show( false );
}

void CDnSpecialBoxTabItemDlg::Process( float fElapsedTime )
{
	CEtUIDialog::Process( fElapsedTime );
}

void CDnSpecialBoxTabItemDlg::Show( bool bShow )
{ 
	CEtUIDialog::Show( bShow );
}

void CDnSpecialBoxTabItemDlg::ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg )
{
	SetCmdControlName( pControl->GetControlName() );

	if (nCommand == EVENT_BUTTON_CLICKED)
	{
		if( IsCmdControl("ID_BT_OK0") || IsCmdControl("ID_BT_OK1") )
		{
			RequestSpecialBoxItemList();
		}
	}

	CEtUIDialog::ProcessCommand( nCommand, bTriggeredByUser, pControl, uMsg );
}

void CDnSpecialBoxTabItemDlg::SetSpecialBoxInfo( TSpecialBoxInfo *pData )
{
	if( pData )
	{
		m_SpecialBoxInfo = *pData;

		m_bConditionCheck = true;

		SetMessageText( pData );
		SetConditionText( pData->cTargetTypeCode , pData->cReceiveTypeCode );
		SetExpireTimeText( pData->tExpirationDate );
		SetLevelText( pData->cTargetMinLevel ,  pData->cTargetMaxLevel  );
		SetJobText( pData->cTargetClassCode );
		SetButtonState( pData );
	}
}


void CDnSpecialBoxTabItemDlg::SetMessageText( TSpecialBoxInfo *pData )
{
	if (pData->bSystemSendFlag)
	{
		int nContentUIString = _wtoi( pData->wszContent );
		int nSenderNameUIString = _wtoi( pData->wszSenderName );

		m_pStatic[eStaticMember::Title]->SetText( GetEtUIXML().GetUIString(CEtUIXML::idCategory1, nContentUIString ) );
		m_pStatic[eStaticMember::From]->SetText( GetEtUIXML().GetUIString(CEtUIXML::idCategory1, nSenderNameUIString) );
	}
	else
	{
		m_pStatic[eStaticMember::Title]->SetText( pData->wszContent );
		m_pStatic[eStaticMember::From]->SetText( pData->wszSenderName );
	}
}

void CDnSpecialBoxTabItemDlg::SetConditionText( int nTargetType , int nReceiveType )
{
	m_pStatic[eStaticMember::ReceiveCondition]->ClearText();
	m_pStatic[eStaticMember::Condition]->ClearText();

	// Target Type
	if( nTargetType == SpecialBox::TargetTypeCode::AccountAll || nTargetType == SpecialBox::TargetTypeCode::AccountSelect )
	{
		m_pStatic[eStaticMember::Condition]->SetText( GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 8206) );
		m_pStatic[eStaticMember::Condition]->SetTextColor( D3DCOLOR_RGBA(255, 153, 0, 255) );
	}
	else
	{
		if( CDnActor::s_hLocalActor )
		{
			m_pStatic[eStaticMember::Condition]->SetText( FormatW( GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 8207) , CDnActor::s_hLocalActor->GetName() ) );
			m_pStatic[eStaticMember::Condition]->SetTextColor( D3DCOLOR_RGBA(255, 100, 105, 255) );
		}
	}

	// Recevie Type
	if( nReceiveType == SpecialBox::ReceiveTypeCode::All )
	{
		m_pStatic[eStaticMember::ReceiveCondition]->SetText( GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 8228) );
		m_pStatic[eStaticMember::ReceiveCondition]->SetTextColor( D3DCOLOR_RGBA( 255, 100, 205, 255 ) );
	}
	else
	{
		m_pStatic[eStaticMember::ReceiveCondition]->SetText( GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 8227) );
		m_pStatic[eStaticMember::ReceiveCondition]->SetTextColor( D3DCOLOR_RGBA( 100, 200, 255, 255 ) );
	}

}

void CDnSpecialBoxTabItemDlg::SetExpireTimeText( __time64_t time )
{
	std::wstring wstExpireDate;
	tm date;
	DnLocalTime_s( &date, &(time));

	CommonUtil::GetDateString( CommonUtil::DATESTR_ACCEPT_LIMITTIME, wstExpireDate , date );
	m_pStatic[eStaticMember::Date]->SetText( wstExpireDate );
}


void CDnSpecialBoxTabItemDlg::SetLevelText( int nMinLevel , int nMaxLevel )
{
	m_pStatic[eStaticMember::Level]->SetText( FormatW( GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 8202), nMinLevel, nMaxLevel ) );

	if( CDnActor::s_hLocalActor )
	{
		int nCurrentLevel = CDnActor::s_hLocalActor->GetLevel();

		if( ( nMinLevel == 0 && nMaxLevel == 0 ) )
		{
			m_pStatic[eStaticMember::Level]->Show( false );
		}
		else if( nCurrentLevel < nMinLevel || nMaxLevel < nCurrentLevel )
		{
			m_pStatic[eStaticMember::Level]->SetTextColor( textcolor::RED );
			m_bConditionCheck = false;
		}
		else
		{
			m_pStatic[eStaticMember::Level]->SetTextColor( textcolor::GREENYELLOW );
		}
	}
}

void CDnSpecialBoxTabItemDlg::SetJobText( int nJobID )
{
	if( nJobID == 0 )
	{
		m_pStatic[eStaticMember::Job]->Show( false );
		return;
	}

	m_pStatic[eStaticMember::Job]->SetText( FormatW( GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 8203), DN_INTERFACE::STRING::GetJobString( nJobID ) ) );
	m_pStatic[eStaticMember::Job]->Show( true );


	if( CDnActor::s_hLocalActor )
	{
		CDnPlayerActor *pPlayer = static_cast<CDnPlayerActor*>(CDnActor::s_hLocalActor.GetPointer());

		std::vector<int> nVecJobList;
		pPlayer->GetJobHistory( nVecJobList );
		
		for( DWORD i=0; i<nVecJobList.size(); i++ )
		{
			if( nVecJobList[i] == nJobID )
			{
				m_pStatic[eStaticMember::Job]->SetTextColor( textcolor::GREENYELLOW );
				return;
			}
		}

		m_pStatic[eStaticMember::Job]->SetTextColor( textcolor::RED );
		m_bConditionCheck = false;
	}
}

void CDnSpecialBoxTabItemDlg::RequestSpecialBoxItemList()
{
	if( m_bConditionCheck == true )
	{
		CDnMainMenuDlg *pMainMenuDlg = GetInterface().GetMainMenuDialog();
		if( pMainMenuDlg )
		{
			CDnEventReceiverTabDlg *pEventReceiverTabDlg = static_cast<CDnEventReceiverTabDlg*>( pMainMenuDlg->GetMainDialog( CDnMainMenuDlg::EVENT_RECEIVER_DIALOG ) );
			if( pEventReceiverTabDlg )
			{
				pEventReceiverTabDlg->GetSpecialListDlg()->SetParentsInfo( m_SpecialBoxInfo );
				SendSpecialBoxItemList( m_SpecialBoxInfo.nEventRewardID );
			}
		}
	}
}

void CDnSpecialBoxTabItemDlg::SetButtonState( TSpecialBoxInfo *pData )
{
	m_pButtonAll->Show( pData->cReceiveTypeCode == SpecialBox::ReceiveTypeCode::All );
	m_pButtonChoice->Show( pData->cReceiveTypeCode == SpecialBox::ReceiveTypeCode::Select );

	m_pButtonAll->Enable( m_bConditionCheck );
	m_pButtonChoice->Enable( m_bConditionCheck );

	if( m_bConditionCheck == true )
	{
		m_pButtonAll->SetTooltipText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 8209 ) );
		m_pButtonChoice->SetTooltipText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 8208 ) );
		
		for( int i=0; i< eStaticMember::Max; i++ )
		{
			m_pStatic[i]->ClearTooltipText();
		}
	}
	else
	{
		m_pButtonAll->SetTooltipText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 8210 ) );
		m_pButtonChoice->SetTooltipText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 8210 ) );

		for( int i=0; i< eStaticMember::Max; i++ )
		{
			m_pStatic[i]->SetTooltipText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 8210 ) );
		}
	}

	m_pStatic[eStaticMember::Title]->SetTextColor( m_bConditionCheck == true ? textcolor::WHITE : textcolor::LIGHTGREY );

	for( int i=0; i< eStaticMember::Max; i++ )
	{
		if( i == eStaticMember::Level || i == eStaticMember::Job )
			continue;
		m_pStatic[i]->Enable( m_bConditionCheck );
	}
}

#endif // PRE_SPECIALBOX