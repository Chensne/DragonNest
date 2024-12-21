#include "StdAfx.h"
#include "DnCashShopCadgeDlg.h"
#include "DnFriendTask.h"
#include "DnCommonUtil.h"
#include "DnInterface.h"
#include "DnCashShopTask.h"
#include "DnInterfaceString.h"



#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif


#ifdef PRE_ADD_CADGE_CASH

CDnCashShopCadgeDlg::CDnCashShopCadgeDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback )
: CEtUIDialog( dialogType, pParentDialog, nID, pCallback, true )
, m_pLevelTitle( NULL )
, m_pLevel( NULL )
, m_pClassTitle( NULL )
, m_pClass( NULL )
, m_pEditBoxName( NULL )
, m_pFriendComboBox( NULL )
, m_pMemoEditBox( NULL )
, m_pCheckUserInfoBtn( NULL )
, m_CheckUserInfoBtnCooltime( 0 )
, m_bUserAuthorized( false )
, m_ReceiverJob( 0 )
, m_ReceiverLevel( 0 )
, m_Mode( eCashUnit_None )
, m_pCancelBtn( NULL )
, m_pSendBtn( NULL )
{
}

void CDnCashShopCadgeDlg::Initialize(bool bShow)
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "CSPresentAskDlg.ui" ).c_str(), bShow );
}

void CDnCashShopCadgeDlg::InitialUpdate()
{
	m_pLevelTitle		= GetControl<CEtUIStatic>( "ID_STATIC_LEVELTITLE" );
	m_pLevel			= GetControl<CEtUIStatic>( "ID_STATIC_CLEVEL" );
	m_pClassTitle		= GetControl<CEtUIStatic>( "ID_STATIC_CLASSTITLE" );
	m_pClass			= GetControl<CEtUIStatic>( "ID_STATIC_CJOB" );
	m_pEditBoxName		= GetControl<CEtUIIMEEditBox>( "ID_IMEEDITBOX_NAME" );
	m_pFriendComboBox	= GetControl<CEtUIComboBox>( "ID_COMBOBOX_FLIST" );
	m_pFriendComboBox->SetEditMode( true );
	m_pCheckUserInfoBtn = GetControl<CEtUIButton>( "ID_BUTTON_CHECK" );
	m_pMemoEditBox		= GetControl<CEtUIIMEEditBox>( "ID_IMEEDITBOX_MEMO" );
#ifdef PRE_REMOVE_CASHSHOP_GIFT_CADGE_MSGMAX
#else
	m_pMemoEditBox->SetMaxEditLength( GIFTMESSAGEMAX + 1 );
#endif
	m_pSendBtn			= GetControl<CEtUIButton>( "ID_BUTTON_SEND" );
	m_pCancelBtn		= GetControl<CEtUIButton>( "ID_BUTTON_CANCEL" );
}

void CDnCashShopCadgeDlg::SetFriendComboBox()
{
	int i = 0;
	const CDnFriendTask::FRIEND_MAP& friendList = GetFriendTask().GetFriendList();
	m_pFriendComboBox->RemoveAllItems();

	CDnFriendTask::FRIEND_MAP::const_iterator iter = friendList.begin();
	for( ; iter != friendList.end(); ++iter )
	{
		CDnFriendTask::FRIENDINFO_MAP *pFriendInfo = iter->second;
		if( pFriendInfo == NULL )
			continue;

		CDnFriendTask::FRIENDINFO_MAP::const_iterator fmIter = pFriendInfo->begin();
		for( ; fmIter != pFriendInfo->end(); ++fmIter )
		{
			const CDnFriendTask::SFriendInfo& info = fmIter->second;
			m_pFriendComboBox->AddItem( info.wszFriendName.c_str(), NULL, i );
			m_FriendNameList.insert( make_pair( info.wszFriendName[0], info.wszFriendName ) );
			++i;
		}
	}

	m_pFriendComboBox->ClearSelectedItem();
}

void CDnCashShopCadgeDlg::OnAddEditBoxString( const std::wstring& strName )
{
	int curEditBoxCaretPos = m_pEditBoxName->GetCaretPos();
	if( strName.size() < 1 ) return;

	std::pair<std::multimap<wchar_t, std::wstring>::iterator, std::multimap<wchar_t, std::wstring>::iterator> range;
	range = m_FriendNameList.equal_range( strName[0] );

	std::wstring almost = _T("");
	std::wstring::size_type longest = 0;
	std::multimap<wchar_t, std::wstring>::iterator it = range.first;
	for( ; it != range.second; ++it )
	{
		std::wstring& friendName = (*it).second;
		if( friendName == strName )
			return;
		std::wstring::size_type size = friendName.find( strName );
		if( size != std::wstring::npos && size >= longest )
			almost = friendName;
	}

	if( almost != _T("") )
	{
		m_pEditBoxName->ClearText();
		m_pEditBoxName->SetText( almost.c_str() );
		m_pEditBoxName->SetSelection( m_pEditBoxName->GetTextLength(), curEditBoxCaretPos );
		m_AutoCompleteTextCache = almost;
	}
	else
	{
		m_AutoCompleteTextCache = _T("");
	}
}

void CDnCashShopCadgeDlg::Clear()
{
	m_pEditBoxName->ClearText();
	m_pLevel->ClearText();
	m_pClass->ClearText();
	m_pMemoEditBox->ClearText();

	m_CheckUserInfoBtnCooltime = 0;
	m_pCheckUserInfoBtn->Enable( true );

	m_bUserAuthorized = false;
	m_AuthorizedUserName.clear();

	m_ReceiverJob = 0;
	m_ReceiverLevel = 0;
}

void CDnCashShopCadgeDlg::Show( bool bShow )
{
	if( m_bShow == bShow )
		return;

	if( bShow )
	{
		m_Mode = eCashUnit_None;
		Clear();
		SetFriendComboBox();
		RequestFocus( m_pEditBoxName );
	}

	CEtUIDialog::Show( bShow );
}

void CDnCashShopCadgeDlg::Process( float fElapsedTime )
{
	if( m_CheckUserInfoBtnCooltime != 0 )
	{
		if( m_CheckUserInfoBtnCooltime > 0 )
		{
			m_CheckUserInfoBtnCooltime -= fElapsedTime;
		}
		else
		{
			m_CheckUserInfoBtnCooltime = 0;
			m_pCheckUserInfoBtn->Enable( true );
		}
	}

	CEtUIDialog::Process( fElapsedTime );
}

bool CDnCashShopCadgeDlg::CheckReceiverInfo()
{
	if( m_pCheckUserInfoBtn->IsEnable() == false )
		return false;

	if( m_AuthorizedUserName.compare(m_pEditBoxName->GetText()) == 0 )
		return false;

	std::wstring rawName = boost::algorithm::trim_copy( std::wstring( m_pEditBoxName->GetText() ) );

	if( rawName.compare(m_pEditBoxName->GetText()) != 0 )
	{
		GetInterface().MessageBox( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 4821 ), MB_OK, -1, this ); // UISTRING : ĳ���͸� �������� �ʴ� Ư�� ���ڸ� ����� �� �����ϴ�.
		return false;
	}

	if( m_pEditBoxName->GetTextLength() > 0 )
	{
		GetCashShopTask().RequestCashShopGiftReceiverInfo( m_pEditBoxName->GetText() );
		m_CheckUserInfoBtnCooltime = SEND_GIFT_CHECK_USER_COOLTIME;
		m_pCheckUserInfoBtn->Enable( false );
	}

	return true;
}

bool CDnCashShopCadgeDlg::DoSendCadge()
{
	std::wstring str;

	std::wstring testStr( m_pMemoEditBox->GetText() );
	bool bProhibition = DN_INTERFACE::UTIL::CheckChat( testStr );
	if( bProhibition )
	{
		str = GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 4578 ); // UISTRING : �޸� ��Ģ� ���ԵǾ� ������ ���� �� �����ϴ�
		GetInterface().MessageBox( str.c_str(), MB_OK );
		return false;
	}

	if( m_pEditBoxName->GetTextLength() <= 0 )
	{
		str = GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 4791 ); // UISTRING : ���� ���� ���� �̸��� �Է����ּ���
		GetInterface().MessageBox( str.c_str(), MB_OK );
		return false;
	}

	std::wstring rawName = boost::algorithm::trim_copy( std::wstring( m_pEditBoxName->GetText() ) );

	if( rawName.compare(m_pEditBoxName->GetText()) != 0 )
	{
		str = GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 4821 ); // UISTRING : ĳ���͸� �������� �ʴ� Ư�� ���ڸ� ����� �� �����ϴ�.
		GetInterface().MessageBox( str.c_str(), MB_OK, -1, this );
		return false;
	}

	if( m_bUserAuthorized == false || m_AuthorizedUserName.empty() || m_AuthorizedUserName.compare( m_pEditBoxName->GetText() ) )
	{
		str = GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 4713 ); // UISTRING : ���� ������ �ޱ� ������ ������ ���� �� �����ϴ�
		GetInterface().MessageBox( str.c_str(), MB_OK, -1, this );
		return false;
	}

	GetCashShopTask().RequestCadge( m_Mode, m_AuthorizedUserName, m_pMemoEditBox->GetText() );
	Show( false );

	return true;
}

void CDnCashShopCadgeDlg::ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg )
{
	SetCmdControlName( pControl->GetControlName() );

	if( IsCmdControl( "ID_IMEEDITBOX_NAME" ) )
	{
		bool bProcessed = true;
		if( nCommand == EVENT_EDITBOX_IME_CHANGE )
		{
			std::wstring curText = m_pEditBoxName->GetText();
			std::wstring composeStr;
			m_pEditBoxName->GetIMECompositionString( composeStr );
			curText += composeStr;
			OnAddEditBoxString( curText );
		}
		else if( nCommand == EVENT_EDITBOX_CHANGE )
		{
			WCHAR ch = (WCHAR)uMsg;
			if( CommonUtil::IsCtrlChar(ch) == false )
			{
				std::wstring curText = m_pEditBoxName->GetText();
				OnAddEditBoxString( curText );
			}
		}
		else if( nCommand != EVENT_EDITBOX_CHANGE )
		{
			if( nCommand == EVENT_EDITBOX_FOCUS || nCommand == EVENT_EDITBOX_RELEASEFOCUS )
			{
				CEtUIDialog::ProcessCommand( nCommand, bTriggeredByUser, pControl, uMsg );
				return;
			}

			if( m_AutoCompleteTextCache != _T("") )
			{
				m_pEditBoxName->ClearText();
				m_pEditBoxName->SetText( m_AutoCompleteTextCache.c_str() );
				m_AutoCompleteTextCache = _T("");
			}
			else
			{
				bProcessed = false;
			}
		}
		else
		{
			bProcessed = false;
		}

		const std::wstring& classString = m_pClass->GetText();
		if( bProcessed && classString.empty() == false )
		{
			m_AuthorizedUserName.clear();
			m_bUserAuthorized = false;

			m_pClass->ClearText();
			m_pLevel->ClearText();
		}

		return;
	}

	if( nCommand == EVENT_BUTTON_CLICKED )
	{
		if( IsCmdControl( "ID_BUTTON_SEND" ) )
		{
			DoSendCadge();
			return;
		}
		else if( IsCmdControl( "ID_BUTTON_CANCEL" ) )
		{
			Show( false );
			return;
		}
		else if( IsCmdControl( "ID_BUTTON_CHECK" ) )
		{
			CheckReceiverInfo();
		}
	}
	else if( nCommand == EVENT_COMBOBOX_SELECTION_CHANGED && bTriggeredByUser )
	{
		if( IsCmdControl( "ID_COMBOBOX_FLIST" ) && m_pFriendComboBox->IsOpenedDropDownBox() == false )
		{
			SComboBoxItem* pItem = m_pFriendComboBox->GetSelectedItem();
			if( pItem )
			{
				m_pEditBoxName->ClearText();
				m_pEditBoxName->SetText( pItem->strText );
			}
		}
	}

	CEtUIDialog::ProcessCommand( nCommand, bTriggeredByUser, pControl, uMsg );
}

void CDnCashShopCadgeDlg::UpdateCadgeReceiverInfo( const SCCashShopCheckReceiver& info )
{
	m_pLevel->ClearText();
	m_pClass->ClearText();

	if( info.nRet != ERROR_NONE )
	{
		if( info.nRet == ERROR_CASHSHOP_DONTSENDYOURSELF ) 
		{
			GetInterface().ServerMessageBox( info.nRet );
		}
		else
		{
			m_pClass->SetText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 4712 ) );	// UISTRING : ���� ���� ����
			m_bUserAuthorized = false;
			focus::ReleaseControl();

			GetInterface().MessageBox( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 4206 ), MB_OK, MESSAGEBOX_NO_USER_EXIST, this ); // UISTRING : �������� �ʴ� ĳ�����Դϴ�
		}
	}
	else
	{
		if( CommonUtil::IsValidCharacterJobId( info.cJob ) == false )
		{
			m_pClass->SetText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 4712 ) );	// UISTRING : ���� ���� ����
			m_bUserAuthorized = false;
			return;
		}

		std::wstring str;
		str = DN_INTERFACE::STRING::GetJobString( info.cJob );
		m_pClass->SetText( str.c_str() );

		str = FormatW( L"%d", info.cLevel );
		m_pLevel->SetText( str.c_str() );

		m_ReceiverJob = info.cJob;
		m_ReceiverLevel = info.cLevel;

		m_bUserAuthorized = true;
		m_AuthorizedUserName = m_pEditBoxName->GetText();
	}
}

void CDnCashShopCadgeDlg::OnUICallbackProc( int nID, UINT nCommand, CEtUIControl *pControl, UINT uiMsg )
{
	SetCmdControlName( pControl->GetControlName() );

	switch( nID ) 
	{
	case MESSAGEBOX_NO_USER_EXIST:
		{
			if( IsCmdControl( "ID_OK" ) )
			{
				RequestFocus( m_pEditBoxName );
			}
		}
		break;
	}
}

bool CDnCashShopCadgeDlg::MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	if( !IsShow() )
		return false;

	if( uMsg == WM_KEYDOWN )
	{
		if( wParam == VK_RETURN )
		{
			if( focus::IsSameControl( m_pEditBoxName ) )
			{
				if( m_AuthorizedUserName.empty() == false && m_AuthorizedUserName.compare( m_pEditBoxName->GetText() ) == 0 )
				{
					DoSendCadge();
					return true;
				}

				CheckReceiverInfo();
				return true;
			}
		}
		else if( wParam == VK_ESCAPE )
		{
			if( focus::IsSameControl( m_pEditBoxName ) )
			{
				Show( false );
				return true;
			}
		}
	}

	return CEtUIDialog::MsgProc( hWnd, uMsg, wParam, lParam );
}

#endif // PRE_ADD_CADGE_CASH