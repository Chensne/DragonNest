#include "StdAfx.h"
#include "DnCharCreateServerNameDlg.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif 

CDnCharCreateServerNameDlg::CDnCharCreateServerNameDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback )
: CEtUIDialog( dialogType, pParentDialog, nID, pCallback )
, m_pStaticName( NULL )
{
}

CDnCharCreateServerNameDlg::~CDnCharCreateServerNameDlg(void)
{
}

void CDnCharCreateServerNameDlg::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "CharCreateServerNameDlg.ui" ).c_str(), bShow );
}

void CDnCharCreateServerNameDlg::InitialUpdate()
{
	m_pStaticName = GetControl<CEtUIStatic>("ID_STATIC_NAME");
}