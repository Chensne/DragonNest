#include "StdAfx.h"
#include "DnGameHelpOptDlg.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnGameHelpOptDlg::CDnGameHelpOptDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback  )
	: CEtUIDialog( dialogType, pParentDialog, nID, pCallback, false )
	, m_pButtonPrev(NULL)
	, m_pButtonNext(NULL)
	, m_pStaticPage(NULL)
	, m_pCheckBoxCaption(NULL)
{
}

CDnGameHelpOptDlg::~CDnGameHelpOptDlg(void)
{
}

void CDnGameHelpOptDlg::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "GameHelpOptDlg.ui" ).c_str(), bShow );
}

void CDnGameHelpOptDlg::InitialUpdate()
{
	m_pButtonPrev = GetControl<CEtUIButton>("ID_BUTTON_PREV");
	m_pButtonPrev->Enable(false);
	m_pButtonNext = GetControl<CEtUIButton>("ID_BUTTON_NEXT");
	m_pButtonNext->Enable(false);
	m_pStaticPage = GetControl<CEtUIStatic>("ID_STATIC_PAGE");
	m_pCheckBoxCaption = GetControl<CEtUICheckBox>("ID_CHECKBOX_CAPTION");
}

void CDnGameHelpOptDlg::ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg )
{
	SetCmdControlName( pControl->GetControlName() );

	if( nCommand == EVENT_BUTTON_CLICKED )
	{
		if( IsCmdControl("ID_BUTTON_APPLY" ) )
		{
		}

		if( IsCmdControl("ID_BUTTON_CANCEL" ) )
		{
		}
	}

	CEtUIDialog::ProcessCommand( nCommand, bTriggeredByUser, pControl, uMsg );
}

void CDnGameHelpOptDlg::Show( bool bShow ) 
{
	if( m_bShow == bShow )
		return;

	if( bShow )
	{
	}
	else
	{
	}

	CEtUIDialog::Show( bShow );
//	CDnMouseCursor::GetInstance().ShowCursor( bShow );
}