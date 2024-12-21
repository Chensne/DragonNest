#include "Stdafx.h"

#ifdef PRE_ADD_BESTFRIEND

#include "DnItemTask.h"
#include "DnInterface.h"
#include "DnInterfaceString.h"

#include "DnBestFriendMemoDlg.h"


#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnBestFriendMemoDlg::CDnBestFriendMemoDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback, bool bAutoCursor )
: CEtUIDialog( dialogType, pParentDialog, nID, pCallback, bAutoCursor )
, m_IMEmemo( NULL )
{	
}

void CDnBestFriendMemoDlg::ReleaseDlg()
{

}


void CDnBestFriendMemoDlg::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName("BFMessageDlg.ui").c_str(), bShow );
}


void CDnBestFriendMemoDlg::InitialUpdate()
{
	m_IMEmemo = GetControl<CEtUIIMEEditBox>( "ID_IMEEDITBOX_MESSAGE" );

}


void CDnBestFriendMemoDlg::ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg )
{
	SetCmdControlName( pControl->GetControlName() );

	if( nCommand == EVENT_BUTTON_CLICKED )
	{
		if( IsCmdControl("ID_BT_CLOSE") || IsCmdControl("ID_BT_CANCEL") )
		{
			Show( false );
		}

		else if( IsCmdControl("ID_BT_OK") )
		{
			InputMemo();
			Show( false );
		}
	}



	CEtUIDialog::ProcessCommand( nCommand, bTriggeredByUser, pControl, uMsg );
}


void CDnBestFriendMemoDlg::Show( bool bShow )
{
	if( m_bShow == bShow )
		return;

	CEtUIDialog::Show( bShow );

	if( bShow )
	{
		RequestFocus( m_IMEmemo );
	}
	else
	{
		m_IMEmemo->ClearText();
	}
}


// 메모입력.
void CDnBestFriendMemoDlg::InputMemo()
{
	LPCWSTR strMemo = m_IMEmemo->GetText();
	if( wcslen(strMemo) > 0 )
	{
		// 비속어필터링 추가.
		std::wstring str( strMemo );
		if( DN_INTERFACE::UTIL::CheckChat( str ) )
		{
			GetInterface().MessageBox( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3772 ), MB_OK );
			return;
		}	

		GetItemTask().RequestMemoBF( m_IMEmemo->GetText() );
	}
}



#endif