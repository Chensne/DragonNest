#include "StdAfx.h"
#include "DnChatRoomMemberListDlg.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnChatRoomMemberListDlg::CDnChatRoomMemberListDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback )
: CEtUIDialog( dialogType, pParentDialog, nID, pCallback )
, m_pStaticLevel(NULL)
, m_pStaticJop(NULL)
, m_pStaticName(NULL)
{
}

CDnChatRoomMemberListDlg::~CDnChatRoomMemberListDlg(void)
{
}

void CDnChatRoomMemberListDlg::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "ChatListDlg.ui" ).c_str(), bShow );
}

void CDnChatRoomMemberListDlg::InitialUpdate()
{
	m_pStaticLevel = GetControl<CEtUIStatic>( "ID_TEXT_USERLV" );
	m_pStaticJop = GetControl<CEtUIStatic>( "ID_TEXT_USERJOB" );
	m_pStaticName = GetControl<CEtUIStatic>( "ID_TEXT_USERNAME" );
}

void CDnChatRoomMemberListDlg::SetInfo( const WCHAR *wszName, const WCHAR *wszJop, int nLevel )
{
	m_pStaticLevel->SetIntToText( nLevel );
	m_pStaticJop->SetText( wszJop );
	m_pStaticName->SetText( wszName );
}