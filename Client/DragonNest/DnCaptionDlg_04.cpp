#include "StdAfx.h"
#include "DnCaptionDlg_04.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnCaptionDlg_04::CDnCaptionDlg_04( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback )
: CDnCustomDlg( dialogType, pParentDialog, nID, pCallback )
{
}

CDnCaptionDlg_04::~CDnCaptionDlg_04(void)
{
}

void CDnCaptionDlg_04::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "CaptionDlg_04.ui" ).c_str(), bShow );
}

void CDnCaptionDlg_04::InitialUpdate()
{
	CDnMessageStatic *pMsgStatic(NULL);
	
	pMsgStatic = GetControl<CDnMessageStatic>("ID_CAPTION0");
	m_vecStaticMessage.push_back( pMsgStatic );
	
	pMsgStatic = GetControl<CDnMessageStatic>("ID_CAPTION1");
	m_vecStaticMessage.push_back( pMsgStatic );
	
}

void CDnCaptionDlg_04::SetCaption( LPCWSTR wszMsg, DWORD dwColor, float fFadeTime , int bCaptionNum)
{
	m_vecStaticMessage[bCaptionNum]->SetMsgTime( fFadeTime );
	m_vecStaticMessage[bCaptionNum]->SetText( wszMsg );
	m_vecStaticMessage[bCaptionNum]->Show( true );
	m_vecStaticMessage[bCaptionNum]->SetTextColor( dwColor );

	m_bShow = true;
}

void CDnCaptionDlg_04::CloseCaption()
{
	for( int i=0; i<2; i++ )
	{
		if( m_vecStaticMessage[i]->IsShow() )
		{
			m_vecStaticMessage[i]->SetMsgTime( 0.0f );
			m_vecStaticMessage[i]->SetText( L"" );
			m_vecStaticMessage[i]->Show( false );
		}
	}
}