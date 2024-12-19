#include "StdAfx.h"
#include "DnGuildInviteReqDlg.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnGuildInviteReqDlg::CDnGuildInviteReqDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback )
: CEtUIDialog( dialogType, pParentDialog, nID, pCallback )
, m_pProgressBarTime(NULL)
, m_pGuildName(NULL)
, m_pCharacterName(NULL)
, m_pButtonOK(NULL)
, m_pButtonCancel(NULL)
, m_fTotalTime(0.0f)
, m_fElapsedTime(0.0f)
{
}

CDnGuildInviteReqDlg::~CDnGuildInviteReqDlg(void)
{
}

void CDnGuildInviteReqDlg::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "GuildMemberAgreeDlg.ui" ).c_str(), bShow );
}

void CDnGuildInviteReqDlg::InitialUpdate()
{
	m_pProgressBarTime = GetControl<CEtUIProgressBar>("ID_PROGRESSBAR_TIME");
	m_pGuildName = GetControl<CEtUIStatic>("ID_TEXT_GUILDNAME");
	m_pCharacterName = GetControl<CEtUIStatic>("ID_TEXT_NAME");
	m_pButtonOK = GetControl<CEtUIButton>("ID_OK");
	m_pButtonCancel = GetControl<CEtUIButton>("ID_CANCEL");
}

void CDnGuildInviteReqDlg::Process( float fElapsedTime )
{
	CEtUIDialog::Process( fElapsedTime );

	if( !IsShow() )
		return;

	if( m_fElapsedTime <= 0.0f )
	{
		CEtUIDialog::ProcessCommand( EVENT_BUTTON_CLICKED, false, m_pButtonCancel, 0 );
		Show(false);
	}
	else
	{
		// AcceptRequest다이얼로그와 달리 중첩처리등이 완전히 없기때문에 스스로 일랩스드갱신을 한다.
		m_fElapsedTime -= fElapsedTime;

		if( m_fTotalTime != 0.0f)
		{
			m_pProgressBarTime->SetProgress( m_fElapsedTime / m_fTotalTime * 100.0f );
		}
		else
		{
			m_pProgressBarTime->SetProgress( 0.0f );
		}
	}
}

void CDnGuildInviteReqDlg::Show( bool bShow )
{ 
	if( bShow == m_bShow )
		return;

	if( !bShow )
	{
		SetDialogID(-1);
		SetCallback(NULL);
		m_fTotalTime = 0.0f;
		m_fElapsedTime = 0.0f;
	}
	else
	{
		//m_pProgressBarTime->SetProgress(100.0f);
	}

	CEtUIDialog::Show( bShow );
}

void CDnGuildInviteReqDlg::SetInfo( LPCWSTR pwszGuildName, LPCWSTR wszCharacterName, float fTotalTime, int nID, CEtUICallback *pCall )
{
	m_nDialogID = nID;
	m_pCallback = pCall;
	m_fTotalTime = fTotalTime;
	m_fElapsedTime = fTotalTime;

	m_pCharacterName->SetText( wszCharacterName );

	m_pGuildName->SetText( pwszGuildName );
	m_pButtonOK->Show( true );
	m_pButtonCancel->Show( true );
}

void CDnGuildInviteReqDlg::SetElapsedTime( float fElapsedTime )
{
	fElapsedTime = min(fElapsedTime, m_fTotalTime);
	fElapsedTime = max(fElapsedTime, 0.0f);
	m_fElapsedTime = fElapsedTime;
}

void CDnGuildInviteReqDlg::SendReject()
{
	if( m_pButtonCancel )
		CEtUIDialog::ProcessCommand( EVENT_BUTTON_CLICKED, false, m_pButtonCancel, 0 );
}