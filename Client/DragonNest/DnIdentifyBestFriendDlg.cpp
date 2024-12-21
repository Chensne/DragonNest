#include "Stdafx.h"

#ifdef PRE_ADD_BESTFRIEND

#include "DNPacket.h"
#include "DnItemTask.h"
#include "DnInterfaceString.h"

#include "DnIdentifyBestFriendDlg.h"


#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif


CDnIdentifyBestFriendDlg::CDnIdentifyBestFriendDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback, bool bAutoCursor )
 : CEtUIDialog( dialogType, pParentDialog, nID, pCallback, bAutoCursor )
{
	m_pStaticBFID = NULL;
	m_pStaticLevel = NULL;
	m_pStaticJob = NULL;
}

void CDnIdentifyBestFriendDlg::ReleaseDlg()
{

}


void CDnIdentifyBestFriendDlg::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName("BFAskDlg.ui").c_str(), bShow );
}


void CDnIdentifyBestFriendDlg::InitialUpdate()
{
	m_pStaticBFID = GetControl<CEtUIStatic>("ID_TEXT_NAME");
	m_pStaticLevel = GetControl<CEtUIStatic>("ID_TEXT_LEVEL");
	m_pStaticJob = GetControl<CEtUIStatic>("ID_TEXT_CLASS");
}


void CDnIdentifyBestFriendDlg::ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg )
{
	SetCmdControlName( pControl->GetControlName() );

	if( nCommand == EVENT_BUTTON_CLICKED )
	{
		if( IsCmdControl("ID_CANCEL") || IsCmdControl("ID_BT_CANCEL") )
		{
			Show( false );
		}

		else if( IsCmdControl("ID_BT_OK") )
		{
			RequestBF(); // 절친등록요청.
			Show( false );
		}
	}

	CEtUIDialog::ProcessCommand( nCommand, bTriggeredByUser, pControl, uMsg );
}


void CDnIdentifyBestFriendDlg::Show( bool bShow )
{
	if( m_bShow == bShow )
		return;

	CEtUIDialog::Show( bShow );

	if( bShow )
	{

	}
	else
	{

	}
}


// 절친확인정보.
void CDnIdentifyBestFriendDlg::SetSearchBF( BestFriend::SCSearch * pData )
{
	wchar_t buf[32]={0,};
	m_pStaticBFID->SetText( pData->wszName );
	m_pStaticLevel->SetText( _itow( pData->cLevel, buf, 10 ) );
	m_pStaticJob->SetText( DN_INTERFACE::STRING::GetJobString( pData->cJob ) );
}


// 절친등록요청.
void CDnIdentifyBestFriendDlg::RequestBF()
{
	GetItemTask().RequestRegistBF( std::wstring( m_pStaticBFID->GetText() ) );
}


#endif