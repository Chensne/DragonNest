#include "StdAfx.h"
#include "DnCaptionDlg_03.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnCaptionDlg_03::CDnCaptionDlg_03( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback )
	: CDnCustomDlg( dialogType, pParentDialog, nID, pCallback )
{
}

CDnCaptionDlg_03::~CDnCaptionDlg_03(void)
{
}

void CDnCaptionDlg_03::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "CaptionDlg_03.ui" ).c_str(), bShow );
}

void CDnCaptionDlg_03::InitialUpdate()
{
	CDnMessageStatic *pMsgStatic(NULL);
	char szName[32]={0};
	for( int i=0; i<3; i++ )
	{
		sprintf_s( szName, 32, "ID_CAPTION%d", i );
		pMsgStatic = GetControl<CDnMessageStatic>(szName);
		m_vecStaticMessage.push_back( pMsgStatic );
	}
}

void CDnCaptionDlg_03::AddCaption( LPCWSTR wszMsg, DWORD dwColor, float fFadeTime )
{
	for( int i=1; i>=0; i-- )
	{
		if( m_vecStaticMessage[i]->IsShow() )
		{
			m_vecStaticMessage[i+1]->SetMsgTime( m_vecStaticMessage[i]->GetMsgTime() );
			m_vecStaticMessage[i+1]->SetText( m_vecStaticMessage[i]->GetText() );

			EtColor TextColor = m_vecStaticMessage[i]->GetTextColor();
			EtColor ShadowColor = m_vecStaticMessage[i]->GetShadowColor();

			// 2번째 라인부터는 알파값을 낮춘다.
			if( i == 1 )
			{
				if( TextColor.a > 0.7f ) TextColor.a = 0.7f;
				if( ShadowColor.a > 0.5f ) ShadowColor.a = 0.5f;
			}
			else if( i == 0 )
			{
				if( TextColor.a > 0.85f ) TextColor.a = 0.85f;
				if( ShadowColor.a > 0.6f ) ShadowColor.a = 0.6f;
			}
			m_vecStaticMessage[i+1]->SetTextColor(TextColor);
			m_vecStaticMessage[i+1]->SetShadowColor(ShadowColor);
			m_vecStaticMessage[i+1]->Show( true );
		}
	}

	m_vecStaticMessage[0]->SetMsgTime( fFadeTime );
	m_vecStaticMessage[0]->SetText( wszMsg );
	m_vecStaticMessage[0]->Show( true );
	m_vecStaticMessage[0]->SetTextColor( dwColor );

	m_bShow = true;
}

void CDnCaptionDlg_03::CloseCaption()
{
	for( int i=0; i<3; i++ )
	{
		if( m_vecStaticMessage[i]->IsShow() )
		{
			m_vecStaticMessage[i]->SetMsgTime( 0.0f );
			m_vecStaticMessage[i]->SetText( L"" );
			m_vecStaticMessage[i]->Show( false );
		}
	}
}