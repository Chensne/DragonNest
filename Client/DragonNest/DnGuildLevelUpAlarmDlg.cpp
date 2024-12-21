#include "StdAfx.h"
#include "DnGuildLevelUpAlarmDlg.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnGuildLevelUpAlarmDlg::CDnGuildLevelUpAlarmDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback )
: CEtUIDialog( dialogType, pParentDialog, nID, pCallback )
, m_pStaticLevel(NULL)
, m_fShowTime(0.0f)
{
}

CDnGuildLevelUpAlarmDlg::~CDnGuildLevelUpAlarmDlg(void)
{
}

void CDnGuildLevelUpAlarmDlg::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "GuildLevelNoticeDlg.ui" ).c_str(), bShow );
}

void CDnGuildLevelUpAlarmDlg::InitialUpdate()
{
	m_pStaticLevel = GetControl<CEtUIStatic>("ID_TEXT0");
}

void CDnGuildLevelUpAlarmDlg::Process( float fElapsedTime )
{
	CEtUIDialog::Process( fElapsedTime );

	if( IsShow() )
	{
		if( m_fShowTime < 0.0f )
		{
			Show( false );
		}
		else
		{
			m_fShowTime -= fElapsedTime;
		}
	}
}

void CDnGuildLevelUpAlarmDlg::SetInfo( int nLevel, float fFadeTime )
{
	WCHAR wszText[64] = {0,};
	swprintf_s( wszText, _countof(wszText), GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3347 ), nLevel );
	m_pStaticLevel->SetText( wszText );
	m_fShowTime = fFadeTime;
}