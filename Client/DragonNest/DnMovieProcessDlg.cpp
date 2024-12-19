#include "StdAfx.h"
#include "DnMovieProcessDlg.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif 

CDnMovieProcessDlg::CDnMovieProcessDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback )
: CEtUIDialog( dialogType, pParentDialog, nID, pCallback, true )
, m_pStatic(NULL)
, m_pProcessComplete(NULL)
, m_pCancelButton(NULL)
, m_pProgressBarTime(NULL)
, m_fMaxTimer(0.0f)
, m_fTimer(0.f)
, m_cFlag(0)
{
}

CDnMovieProcessDlg::~CDnMovieProcessDlg(void)
{
}

void CDnMovieProcessDlg::Initialize( bool bShow )
{
	// 기본적으로 버튼과 프로세스바만 가지고 있고, 무비 컨트롤은 DnInterface의 공용다이얼로그를 사용한다. 어차피 보여주기만 할 부분이므로.
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "MovieProcessDlg.ui" ).c_str(), bShow );
}

void CDnMovieProcessDlg::InitialUpdate()
{
	m_pStatic = GetControl<CEtUIStatic>("ID_STATIC0");
	m_pProcessComplete = GetControl<CEtUIButton>("ID_PROCESS_COMPLETE");
	m_pCancelButton = GetControl<CEtUIButton>("ID_BUTTON_CANCEL");
	m_pProgressBarTime = GetControl<CEtUIProgressBar>("ID_PROGRESSBAR_TIME");

	m_pProcessComplete->Show( false );
}

void CDnMovieProcessDlg::Show( bool bShow )
{
	if( bShow == m_bShow )
		return;

	if( !bShow )
	{
		// 일반적인 경우에선 항상 캔슬이 눌러져야, Show(false)가 호출되기때문에 m_cFlag가 0일 것이다.
		// 그러나 파티에 가입된 상태에서 프로그래스바 올라갈때 파티장이 채널을 이동한다거나해서
		// (즉, 일반적이지 않게 창이 강제로 닫힐 경우)
		// 플래그를 확인해 강제캔슬을 호출하도록 처리한다.
		if( m_cFlag == 1 )
			m_pCallback->OnUICallbackProc( m_nDialogID, EVENT_BUTTON_CLICKED, m_pCancelButton, 0 );

		SetDialogID(-1);
		SetCallback(NULL);
		m_cFlag = 0;
		m_fTimer = 0.0f;
		m_pProgressBarTime->SetProgress( 0.0f );
	}

	CEtUIDialog::Show( bShow );
}

void CDnMovieProcessDlg::Process( float fElapsedTime )
{
	switch( m_cFlag )
	{
	case 1:
		{
			m_fTimer -= fElapsedTime;
			if( m_fTimer <= 0.f )
			{
				m_pCancelButton->Enable( false );
				m_pProgressBarTime->Enable( false );
				// 이런 방법으로 상위단에 프로세스 종료를 알린다.
				m_pCallback->OnUICallbackProc( m_nDialogID, EVENT_BUTTON_CLICKED, m_pProcessComplete, 0 );
				m_cFlag = 0;
				Show( false );
			}
			m_pProgressBarTime->SetProgress( (1.0f - (m_fTimer / m_fMaxTimer)) * 100.0f );
		}
		break;
	}

	CEtUIDialog::Process( fElapsedTime );
}

void CDnMovieProcessDlg::ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg )
{
	SetCmdControlName( pControl->GetControlName() );

	// 취소버튼을 누를때 클라이언트단에선 취소패킷을 보내게 된다.
	// 이 패킷에 대한 응답이 오기전 프로그래스바 시간이 다 되서 완료를 보내면 엉뚱한 패킷을 보내게 되므로,
	// 처리 플래그를 끈다.
	if( nCommand == EVENT_BUTTON_CLICKED )
	{
		if( IsCmdControl("ID_BUTTON_CANCEL" ) )
		{
			m_cFlag = 0;
		}
	}

	CEtUIDialog::ProcessCommand( nCommand, bTriggeredByUser, pControl, uMsg );
}

void CDnMovieProcessDlg::SetInfo( LPCWSTR wszMessage, float fTimer, int nID, CEtUICallback *pCall, bool bShowButton )
{
	// SetInfo호출하면 초기화 후 프로세스를 시작하는 것으로 여긴다.
	m_cFlag = 1;

	m_pStatic->SetText( wszMessage );
	m_fMaxTimer = m_fTimer = fTimer;

	m_nDialogID = nID;
	m_pCallback = pCall;

	m_pCancelButton->Show( bShowButton );
	m_pCancelButton->Enable( true );
	m_pProgressBarTime->Show( true );
}