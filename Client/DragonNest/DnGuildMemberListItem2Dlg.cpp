#include "StdAfx.h"
#include "DnGuildMemberListItem2Dlg.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnGuildMemberListItem2Dlg::CDnGuildMemberListItem2Dlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback )
: CEtUIDialog( dialogType, pParentDialog, nID, pCallback )
, m_pStaticName(NULL)
, m_pStaticList1(NULL)
{
}

CDnGuildMemberListItem2Dlg::~CDnGuildMemberListItem2Dlg(void)
{
}

void CDnGuildMemberListItem2Dlg::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "GuildLastListDlg.ui" ).c_str(), bShow );
}

void CDnGuildMemberListItem2Dlg::InitialUpdate()
{
	m_pStaticName = GetControl<CEtUIStatic>( "ID_LIST0" );
	m_pStaticList1 = GetControl<CEtUIStatic>( "ID_LIST1" );	
}

void CDnGuildMemberListItem2Dlg::SetInfo( const WCHAR *wszName , const WCHAR *wszItemText, bool bEnable )
{
	m_pStaticName->SetText( wszName );
	m_pStaticList1->SetText( wszItemText );

	m_pStaticName->Enable( bEnable );
	m_pStaticList1->Enable( bEnable );
}