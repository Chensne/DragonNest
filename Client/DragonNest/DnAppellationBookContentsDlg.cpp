#include "StdAfx.h"
#include "DnAppellationBookContentsDlg.h"


#ifdef PRE_MOD_APPELLATIONBOOK_RENEWAL

CDnAppellationBookContentsDlg::CDnAppellationBookContentsDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID , CEtUICallback *pCallback )
: CEtUIDialog( dialogType, pParentDialog, nID, pCallback) 
, m_pStaticContentsName_Select(NULL)
, m_pStaticContentsName_DeSelect(NULL)
, m_pStaticLine(NULL)
, m_pStaticEnable(NULL)
, m_pStaticDisable(NULL)
, m_pSelectImage(NULL)
, m_nIndexID(-1)
{
}

CDnAppellationBookContentsDlg::~CDnAppellationBookContentsDlg(void)
{
}

void CDnAppellationBookContentsDlg::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "AppellationBookListDlg.ui" ).c_str(), bShow );
}

void CDnAppellationBookContentsDlg::InitialUpdate()
{
	m_pStaticContentsName_Select = GetControl<CEtUIStatic>("ID_TEXT_SELECT");
	m_pStaticContentsName_Select->Show(false);
	m_pStaticContentsName_Select->Enable(false);

	m_pStaticContentsName_DeSelect = GetControl<CEtUIStatic>("ID_STATIC1");
	m_pStaticContentsName_DeSelect->Show(false);
	m_pStaticContentsName_DeSelect->Enable(false);
	
	
	m_pStaticLine = GetControl<CEtUIStatic>("ID_STATIC0");
	m_pStaticLine->Show(false);
	m_pStaticLine->Enable(false);
	
	m_pStaticEnable = GetControl<CEtUIStatic>("ID_PAGE_IN");
	m_pStaticEnable->Show(false);
	m_pStaticEnable->Enable(false);
	
	m_pStaticDisable = GetControl<CEtUIStatic>("ID_PAGE_OUT");
	m_pStaticDisable->Show(false);
	m_pStaticDisable->Enable(false);

	m_pSelectImage = GetControl<CEtUIStatic>("ID_STATIC_SELECTBAR");
	m_pSelectImage->Show(false);
	m_pSelectImage->Enable(false);
}

void CDnAppellationBookContentsDlg::Show( bool bShow )
{
	if(bShow == IsShow())
	{
		return;
	}

	CEtUIDialog::Show( bShow );
}

void CDnAppellationBookContentsDlg::ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg )
{
	SetCmdControlName( pControl->GetControlName() );

	if(nCommand == EVENT_BUTTON_CLICKED)
	{
	}

	CEtUIDialog::ProcessCommand( nCommand, bTriggeredByUser, pControl, uMsg );
}

bool CDnAppellationBookContentsDlg::MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	if( !IsShow() )
		return false;

	bool bRet = CEtUIDialog::MsgProc( hWnd, uMsg, wParam, lParam );

	switch( uMsg )
	{
	case WM_LBUTTONDOWN:
		{
		}
		break;
	}

	return bRet;
}

void CDnAppellationBookContentsDlg::SetData(std::wstring wszTitle, bool bIsEnable, int nIndexID)
{
	if(wszTitle.empty())
		return;

	if(bIsEnable)
	{
		m_pStaticEnable->Show(true);
		m_pStaticEnable->Enable(true);

		m_pStaticContentsName_Select->Show(true);
		m_pStaticContentsName_Select->Enable(true);
		m_pStaticContentsName_Select->SetText(wszTitle);
	}
	else
	{
		m_pStaticDisable->Show(true);
		m_pStaticDisable->Enable(true);

		m_pStaticContentsName_DeSelect->Show(true);
		m_pStaticContentsName_DeSelect->Enable(true);
		m_pStaticContentsName_DeSelect->SetText(wszTitle);
	}

	if(nIndexID == 0)
	{
		m_pSelectImage->Show(true);
		m_pSelectImage->Enable(true);
	}

	m_nIndexID = nIndexID;
}

void CDnAppellationBookContentsDlg::ResetData()
{
	m_pStaticContentsName_Select->Show(false);
	m_pStaticContentsName_Select->Enable(false);

	m_pStaticContentsName_DeSelect->Show(false);
	m_pStaticContentsName_DeSelect->Enable(false);

	m_pStaticLine->Show(false);
	m_pStaticLine->Enable(false);

	m_pStaticEnable->Show(false);
	m_pStaticEnable->Enable(false);
	
	m_pStaticDisable->Show(false);
	m_pStaticDisable->Enable(false);
	
	m_pSelectImage->Show(false);
	m_pSelectImage->Show(false);
	
	m_nIndexID = -1;
}

void CDnAppellationBookContentsDlg::SelectListBox(bool bShow)
{
	m_pSelectImage->Show(bShow);
	m_pSelectImage->Enable(bShow);
}

#endif