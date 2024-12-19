#include "StdAfx.h"
#include "DnGuildMemberListItem3Dlg.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnGuildMemberListItem3Dlg::CDnGuildMemberListItem3Dlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback )
: CEtUIDialog( dialogType, pParentDialog, nID, pCallback )
, m_pStaticName(NULL)
, m_pStaticList1(NULL)
, m_pStaticList2(NULL)
{
}

CDnGuildMemberListItem3Dlg::~CDnGuildMemberListItem3Dlg(void)
{
}

void CDnGuildMemberListItem3Dlg::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "GuildListDlg.ui" ).c_str(), bShow );
}

void CDnGuildMemberListItem3Dlg::InitialUpdate()
{
	m_pStaticName = GetControl<CEtUIStatic>( "ID_LIST0" );
	m_pStaticList1 = GetControl<CEtUIStatic>( "ID_LIST1" );	
	m_pStaticList2 = GetControl<CEtUIStatic>( "ID_LIST2" );	
}

void CDnGuildMemberListItem3Dlg::SetInfo( const WCHAR *wszName , const WCHAR *wszItemText1, const WCHAR *wszItemText2, bool bEnable )
{
	m_pStaticName->SetText( wszName );
	m_pStaticList1->SetText( wszItemText1 );
	m_pStaticList2->SetText( wszItemText2 );

	m_pStaticName->Enable( bEnable );
	m_pStaticList1->Enable( bEnable );
	m_pStaticList2->Enable( bEnable );
}