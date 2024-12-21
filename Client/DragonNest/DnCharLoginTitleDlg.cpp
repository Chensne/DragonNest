#include "StdAfx.h"
#include "DnCharLoginTitleDlg.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif 

#ifdef PRE_MOD_SELECT_CHAR

CDnCharLoginTitleDlg::CDnCharLoginTitleDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback  )
: CEtUIDialog( dialogType, pParentDialog, nID, pCallback  )
{
}

CDnCharLoginTitleDlg::~CDnCharLoginTitleDlg(void)
{
}

void CDnCharLoginTitleDlg::Initialize( bool bShow )
{
#ifdef _ADD_NEWLOGINUI
	CEtUIDialog::Initialize(CEtResourceMng::GetInstance().GetFullName("charselecttitledlg.ui").c_str(), bShow);
#else
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "CharCreate_TitleDlg.ui" ).c_str(), bShow );
#endif
}

void CDnCharLoginTitleDlg::SetTitle( int nTitleString )
{
	CEtUIStatic* pStaticTitle = GetControl<CEtUIStatic>( "ID_STATIC_TITLE" );
	if( pStaticTitle )
		pStaticTitle->SetText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, nTitleString ) );
}

#endif // PRE_MOD_SELECT_CHAR