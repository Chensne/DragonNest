#include "StdAfx.h"
#include "DnCaptionDlg_06.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif


CDnCaptionDlg_06::CDnCaptionDlg_06( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback )
: CDnCustomDlg( dialogType, pParentDialog, nID, pCallback ),
m_pStaticMessage(NULL)
{
}

CDnCaptionDlg_06::~CDnCaptionDlg_06(void)
{
}

void CDnCaptionDlg_06::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "CaptionDlg_06.ui" ).c_str(), bShow );
}

void CDnCaptionDlg_06::InitialUpdate()
{
	m_pStaticMessage = GetControl<CDnMessageStatic>("ID_CAPTION0");
}

void CDnCaptionDlg_06::SetCaption( LPCWSTR wszMsg, DWORD dwColor, float fFadeTime )
{
	m_pStaticMessage->SetMsgTime( fFadeTime );
	m_pStaticMessage->SetText( wszMsg );
	m_pStaticMessage->Show( true );
	//m_pStaticMessage->SetTextColor( dwColor ); // 컨트롤의 색상을 따른다.

	m_bShow = true;
}

void CDnCaptionDlg_06::CloseCaption()
{	
	if( m_pStaticMessage->IsShow() )
	{
		m_pStaticMessage->SetMsgTime( 0.0f );
		m_pStaticMessage->SetText( L"" );
		m_pStaticMessage->Show( false );
	}
	
}