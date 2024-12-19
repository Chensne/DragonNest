#include "Stdafx.h"
#include "DnMasterJoinListDlg.h"
#include "DnInterfaceString.h"

CDnMasterJoinListDlg::CDnMasterJoinListDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback )
: CEtUIDialog( dialogType, pParentDialog, nID, pCallback )
, m_pStaticName(NULL)
, m_pStaticLevel(NULL)
, m_pStaticClass(NULL)
{
}

CDnMasterJoinListDlg::~CDnMasterJoinListDlg(void)
{
}

void CDnMasterJoinListDlg::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "MasterJoinListDlg.ui" ).c_str(), bShow );
}

void CDnMasterJoinListDlg::InitialUpdate()
{
	m_pStaticName = GetControl<CEtUIStatic>( "ID_TEXT_NAME" );
	m_pStaticLevel = GetControl<CEtUIStatic>( "ID_TEXT_LEVEL" );
	m_pStaticClass = GetControl<CEtUIStatic>( "ID_TEXT_CLASS" );
}

void CDnMasterJoinListDlg::Process( float fElapsedTime )
{
	CEtUIDialog::Process( fElapsedTime );
}

void CDnMasterJoinListDlg::SetInfo( const WCHAR * wszName, BYTE cLevel, BYTE cClass )
{
	WCHAR szString[256];

	m_pStaticName->SetText( wszName );

	wsprintf( szString, L"%s %d", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 64 ), (int)cLevel );
	m_pStaticLevel->SetText( szString );

	m_pStaticClass->SetText( DN_INTERFACE::STRING::GetJobString( cClass ) );
}
