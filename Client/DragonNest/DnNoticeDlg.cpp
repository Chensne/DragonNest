#include "StdAfx.h"
#include "DnNoticeDlg.h"
#include "EtUIHtmlTextBox.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif 

CDnNoticeDlg::CDnNoticeDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback  )
	: CEtUIDialog( dialogType, pParentDialog, nID, pCallback  )
	, m_pTextBox(NULL)
{
}

CDnNoticeDlg::~CDnNoticeDlg(void)
{
}

void CDnNoticeDlg::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "NoticeDlg.ui" ).c_str(), bShow );
}

void CDnNoticeDlg::InitialUpdate()
{
	m_pTextBox = GetControl<CEtUIHtmlTextBox>("ID_HTMLTEXTBOX");
}