#include "StdAfx.h"
#include "DnGuildWantedListPopupDlg.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

#define LINE_HEIGHT 6

CDnGuildWantedListPopupDlg::CDnGuildWantedListPopupDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback )
: CEtUIDialog( dialogType, pParentDialog, nID, pCallback )
, m_pTextBox(NULL)
, m_pStaticLine(NULL)
#ifdef PRE_ADD_GUILD_EASYSYSTEM
, m_pStaticLine2(NULL)
#endif
{
}

CDnGuildWantedListPopupDlg::~CDnGuildWantedListPopupDlg(void)
{
}

void CDnGuildWantedListPopupDlg::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "GuildWantedADTooltipDlg.ui" ).c_str(), bShow );
}

void CDnGuildWantedListPopupDlg::InitialUpdate()
{
	m_pTextBox = GetControl<CEtUITextBox>( "ID_TEXTBOX0" );
	m_pStaticLine = GetControl<CEtUIStatic>( "ID_STATIC_LINE" );
#ifdef PRE_ADD_GUILD_EASYSYSTEM
	m_pStaticLine2 = GetControl<CEtUIStatic>( "ID_STATIC_LINE2" );
#endif
}

#ifdef PRE_ADD_GUILD_EASYSYSTEM
void CDnGuildWantedListPopupDlg::SetInfo( const WCHAR *wszName, int nLevel, int nCount, int nCountMax, const WCHAR *wszNotice, const WCHAR *wszHomepage, const WCHAR *wszGuildMasterName )
#else
void CDnGuildWantedListPopupDlg::SetInfo( const WCHAR *wszName, int nLevel, int nCount, int nCountMax, const WCHAR *wszNotice )
#endif
{
	WCHAR wszText[256] = {0,};
	m_pTextBox->ClearText();

	m_pTextBox->AddText( wszName, descritioncolor::ORANGE );
	swprintf_s( wszText, _countof(wszText), GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3347 ), nLevel );
	m_pTextBox->AddText( wszText );	
	swprintf_s( wszText, _countof(wszText), GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3348 ), nCount, nCountMax );
	m_pTextBox->AddText( wszText );
#ifdef PRE_ADD_GUILD_EASYSYSTEM
	swprintf_s( wszText, _countof(wszText), L"%s %s", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3855 ), wszGuildMasterName );
	m_pTextBox->AddText( wszText );
	m_pStaticLine->Show( false );
	m_pStaticLine2->Show( false );
	if( wszNotice != NULL )
	{
		int nLen = (int)wcslen( wszNotice );
		if( nLen > 0 )
		{
			m_pTextBox->AddText( L"" );

			SUICoord LastLineCoord;
			m_pTextBox->GetLastLineCoord( LastLineCoord );
			float fNewY = LastLineCoord.fY + (float)LINE_HEIGHT/DEFAULT_UI_SCREEN_HEIGHT;
			fNewY = ((int)(fNewY*DEFAULT_UI_SCREEN_HEIGHT)) / (float)DEFAULT_UI_SCREEN_HEIGHT;
			m_pStaticLine->SetPosition( m_pStaticLine->GetUICoord().fX, fNewY );
			m_pStaticLine->Show( true );

			m_pTextBox->SetText( wszNotice );
		}
	}
	if( wszHomepage != NULL )
	{
		int nLen = (int)wcslen( wszHomepage );
		if( nLen > 0 )
		{
			m_pTextBox->AddText( L"" );

			SUICoord LastLineCoord;
			m_pTextBox->GetLastLineCoord( LastLineCoord );
			float fNewY = LastLineCoord.fY + (float)LINE_HEIGHT/DEFAULT_UI_SCREEN_HEIGHT;
			fNewY = ((int)(fNewY*DEFAULT_UI_SCREEN_HEIGHT)) / (float)DEFAULT_UI_SCREEN_HEIGHT;
			m_pStaticLine2->SetPosition( m_pStaticLine2->GetUICoord().fX, fNewY );
			m_pStaticLine2->Show( true );

			m_pTextBox->SetText( wszHomepage );
		}
	}
#else
	m_pStaticLine->Show( false );
	if( !wszNotice ) return;
	int nLen = (int)wcslen( wszNotice );
	if( nLen > 0 )
	{
		m_pTextBox->AddText( L"" );

		SUICoord LastLineCoord;
		m_pTextBox->GetLastLineCoord( LastLineCoord );
		float fNewY = LastLineCoord.fY + (float)LINE_HEIGHT/DEFAULT_UI_SCREEN_HEIGHT;
		fNewY = ((int)(fNewY*DEFAULT_UI_SCREEN_HEIGHT)) / (float)DEFAULT_UI_SCREEN_HEIGHT;
		m_pStaticLine->SetPosition( m_pStaticLine->GetUICoord().fX, fNewY );
		m_pStaticLine->Show( true );

		m_pTextBox->SetText( wszNotice );
	}
#endif
}