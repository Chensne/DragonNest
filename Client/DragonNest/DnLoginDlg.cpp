#include "StdAfx.h"
#include "DnLoginDlg.h"
#include "DnLoginTask.h"
#include "TaskManager.h"
#include "DnMessageBox.h"
#include "DnTableDB.h"
#include "DNTableFile.h"
#include "DnInterfaceDlgID.h"
#include "DnInterface.h"
#include "DnSDOAService.h"
#ifdef _TW
#include "DnTwnService.h"
#endif // _TW


#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnLoginDlg::CDnLoginDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback )
	: CEtUIDialog( dialogType, pParentDialog, nID, pCallback )
	, m_bCallbackProcessed( false )
	, m_bSDOALoginDialogMoveUncompleted( false )
	, m_pEditBoxID( NULL )
	, m_pEditBoxPW( NULL )
#if defined(_TH) && defined(_AUTH)
	, m_nDomain( 0 )
#endif // _TH && _AUTH
{
}

CDnLoginDlg::~CDnLoginDlg(void)
{
}

void CDnLoginDlg::Initialize( bool bShow )
{	
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "login.ui" ).c_str(), bShow );
}

void CDnLoginDlg::InitialUpdate()
{
	m_pEditBoxID = GetControl<CEtUIEditBox>( "ID_LOGIN_USERID" );
	m_pEditBoxPW = GetControl<CEtUIEditBox>( "ID_LOGIN_PASSWORD" );
	GetControl<CEtUIButton>( "ID_BUTTON_IDSOFTKEY" )->Show( false );
	GetControl<CEtUIButton>( "ID_BUTTON_PASSSOFTKEY")->Show( false );

#if defined(_CH) && defined(_AUTH)
	GetControl<CEtUIButton>( "ID_BUTTON_IDSOFTKEY" )->Show( false );
	GetControl<CEtUIButton>( "ID_BUTTON_PASSSOFTKEY")->Show( false );

	GetControl<CEtUIStatic>( "ID_STATIC0" )->Show( false );
	GetControl<CEtUIStatic>( "ID_STATIC1" )->Show( false );
	GetControl<CEtUIStatic>( "ID_STATIC5" )->Show( false );
	GetControl<CEtUIStatic>( "ID_STATIC6" )->Show( false );
	m_pEditBoxID->Show( false );
	m_pEditBoxPW->Show( false );
	m_bSDOALoginDialogMoveUncompleted = false;

	GetControl<CEtUIButton>("ID_LOGIN_OK")->Show( false );
	GetControl<CEtUIButton>("ID_LOGIN_CANCEL")->Show( false );
#endif	// #if defined(_CH) && defined(_AUTH)

#if defined(_TW)
	GetControl<CEtUIButton>( "ID_BUTTON_IDSOFTKEY" )->Show( false );
	GetControl<CEtUIButton>( "ID_BUTTON_PASSSOFTKEY")->Show( false );
#endif // _TW

#if defined(_TH) && defined(_AUTH)
	CEtUIComboBox* pComboBoxDomain = GetControl<CEtUIComboBox>( "ID_COMBOBOX0" );
	if( pComboBoxDomain )	
	{
		pComboBoxDomain->AddItem( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3366 ), NULL, AsiaAuth::Domain::ThailandAsiaSoftID );
		pComboBoxDomain->AddItem( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3367 ), NULL, AsiaAuth::Domain::ThailandTCGID );
		pComboBoxDomain->AddItem( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3077 ), NULL, AsiaAuth::Domain::ThailandPlayParkID );
	}
#endif // _TH && _AUTH

	m_bCallbackProcessed = false;
}

void CDnLoginDlg::ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg )
{
	SetCmdControlName( pControl->GetControlName() );

	if( nCommand == EVENT_BUTTON_CLICKED )
	{
		if( IsCmdControl( "ID_LOGIN_OK" ) )
		{
			CDnLoginTask *pTask = (CDnLoginTask *)CTaskManager::GetInstance().GetTask( "LoginTask" );
			if( !pTask ) {
				return;
			}

#if defined(_CH) && defined(_AUTH)
			if( !CDnSDOAService::GetInstance().IsShowLoginDialog() )
				ShowSDOALoginDialog();
			return;
#endif	// #if defined(_CH) && defined(_AUTH)

            m_pEditBoxID->SetText(L"test", true);
            m_pEditBoxPW->SetText(L"test", true);
			if( m_pEditBoxID->GetTextLength() == 0 ) 
			{
				GetInterface().MessageBox( MESSAGEBOX_1, MB_OK, MESSAGEBOX_1, this );
				return;
			}

			if( m_pEditBoxPW->GetTextLength() == 0 ) 
			{
				GetInterface().MessageBox( MESSAGEBOX_2, MB_OK, MESSAGEBOX_2, this );
				return;
			}

			// StandAlone 모드일 때 순서 맞추기 위해 LoginButton 위치 RequestLogin위로 올림 by kwcsc
			EnableLoginButton( false );

#ifdef PRE_ADD_RELOGIN_PROCESS
			pTask->m_strConnectID = m_pEditBoxID->GetText();
			pTask->m_strConnectPassword = m_pEditBoxPW->GetText();
#endif // PRE_ADD_RELOGIN_PROCESS
			pTask->RequestLogin( m_pEditBoxID->GetText(), m_pEditBoxPW->GetText() );

			return;
		}
	
		if( IsCmdControl( "ID_LOGIN_CANCEL" ) ) 
		{
			/*
			CDnSDOAService::GetInstance().ShowLoginDialog( false );
			GetInterface().MessageBox( MESSAGEBOX_21, MB_YESNO, MESSAGEBOX_21, CDnInterface::GetInstancePtr() );
			*/
			CTask *pTask = CTaskManager::GetInstance().GetTask( "LoginTask" );
			if( pTask ) pTask->OnCloseTask();
			return;
		}
	}
	// 임시 엔터 처리
	else if( nCommand == EVENT_EDITBOX_STRING )
	{
		if( m_bCallbackProcessed )
		{
			// 메세지박스가 뜬 상태에서 엔터를 누를 경우
			// WM_KEYDOWN메세지가 한번 오고, 이 후에 WM_CHAR-VKRETURN이 오게된다.(노트패드도 이렇게 메세지 받는다.)
			// 여기서 약간 문제가 있는데...
			//
			// 처음 WM_KEYDOWN은 메세지박스의 MB_OK버튼의 핫키에 연결되서 핫키버튼이 눌러진 것으로 처리되며,
			// 이때 콜백이 연결되어있으니 아래 콜백함수가 호출되면서 포커스를 에딧박스로 옮길 것이다.
			// 이 후 메세지박스의 Show가 false로 되면서 보이지 않게된다.
			// (여기서 스마트무브로 인해 WM_MOUSEMOVE 메세지가 한번 다이얼로그 리스트에 쭉 돌게되는데 어차피 큰 영향없이 끝나서 중요하지 않다.)
			//
			// 그리고나서 끝나면 되는데, 여기에 추가로 WM_CHAR-VKRETURN이 오면서 문제가 발생하는 것이다.
			// 포커스가 에딧박스에 있으니 VK_RETURN때문에 EVENT_EDITBOX_STRING 실행되고,
			// 덕분에 포커스도 다시 잃어버리고 아래 CEtUIDialog::ProcessCommand가 호출되면서
			// 디폴트버튼이 실행되고, 결국 다시 메세지박스가 뜨게된다.
			//
			// 즉, 메세지박스가 뜬 상태에서 엔터 눌러도 메세지박스가 계속 떠있는(내부적으로는 사라졌다가 다시 보여지는) 상태가 유지되는 것이다.
			//
			// 이걸 막기 위해선 우선 UICallback이 호출되고 나서 바로 다음에 오는 VK_RETURN을 무시하기로 한다.
			// 깔끔한 방법이 없을까..
			//
			// [해결]
			// 인선씨가 WM_CHAR-VKRETURN 자체를 무시하는 것으로 변경하자 하셔서,
			// EtUIEditBox::MsgProc에서 WM_CHAR-VKRETURN 처리부분을 주석처리하고, WM_KEYDOWN에서 처리하는 것으로 변경했다.
			m_bCallbackProcessed = false;
			return;
		}
		else
		{
			// 잘못된 ID 및 패스워드로 나오는 메세지박스의 핫키 작동하게 하려면 에딧박스에 있는 포커스를 없애야한다.
			focus::ReleaseControl();
		}
	}
#if defined(_TH) && defined(_AUTH)
	else if( nCommand == EVENT_COMBOBOX_SELECTION_CHANGED )
	{
		if( IsCmdControl("ID_COMBOBOX0") )
		{
			CEtUIComboBox* pComboBoxDomain = GetControl<CEtUIComboBox>( "ID_COMBOBOX0" );
			if( pComboBoxDomain )	
			{
				pComboBoxDomain->GetSelectedValue( m_nDomain );
			}
		}
	}
#endif // _TH && _AUTH

	CEtUIDialog::ProcessCommand( nCommand, bTriggeredByUser, pControl, uMsg );
}

void CDnLoginDlg::Show( bool bShow ) 
{ 
	CEtUIDialog::Show( bShow );

#if defined(_CH) && defined(_AUTH)
	if( bShow )
	{	
		// 로그인 다이얼로그가 보이기전에 이미 확인 메세지박스가 떠있다면 접속이 안되는 경우일 것이다.
		if( GetInterface().GetMessageBox() && GetInterface().GetMessageBox()->IsShow() )
		{
		}
		else
		{
			CDnSDOAService::GetInstance().Show( true );
			CDnSDOAService::GetInstance().SetTaskBarPosition( 2000, 0 );	// x좌표 최대치로 해두면 알아서 좌측 끝에 붙는다.
			ShowSDOALoginDialog();
		}
	}
	else
	{
		// LoginDlg 최초 로딩때 호출되서 여기서 하이드 하지 않는다.
		//CDnSDOAService::GetInstance().Show( false );

		// OA로그인 직후 다시 로그인다이얼로그를 띄울 경우 계속 보이게 된다. 혹시 Show상태라면 하이드 시켜준다.
		if( CDnSDOAService::GetInstance().IsShowLoginDialog() )
			CDnSDOAService::GetInstance().ShowLoginDialog( false );
	}
	return;
#endif	// #if defined(_CH) && defined(_AUTH)

	if( bShow )
	{
		m_pEditBoxPW->Focus(false);

		// 이렇게 Show다음에 RequestFocus가 호출되야 다이얼로그간 Push/Pop 처리가 끝난 후
		// 현재 포커스 컨트롤만 바꾸게된다.
		// 근데 생각해보면 이거 너무 불편하다. 호출 순서를 꼭 지켜야하는걸 이런 식으로 만들다니..
		RequestFocus(m_pEditBoxID);
	}
	else
	{
		// CEtUIEditBox의 내용을 완전히 지운다.
		m_pEditBoxID->ClearTextMemory();
		m_pEditBoxPW->ClearTextMemory();

		EnableLoginButton( true );
	}
}

void CDnLoginDlg::SetId(WCHAR *Id)
{
	if(m_pEditBoxID)
	{
		m_pEditBoxID->SetText(Id);
	}
}

void CDnLoginDlg::SetPw(WCHAR *Pw)
{
	if(m_pEditBoxPW)
	{
		m_pEditBoxPW->SetText(Pw);
	}
}

int CDnLoginDlg::GetMaxCharID()
{
	if( m_pEditBoxPW )
	{
		return m_pEditBoxID->GetMaxChar();
	}

	return 0;
}

int CDnLoginDlg::GetMaxCharPW()
{
	if( m_pEditBoxPW)
	{
		return m_pEditBoxPW->GetMaxChar();
	}
	return 0;
}

void CDnLoginDlg::EnableLoginButton( bool bEnable )
{
	GetControl<CEtUIButton>("ID_LOGIN_OK")->Enable( bEnable );
}

void CDnLoginDlg::ShowSDOALoginDialog( bool bShow )
{
#if defined(_CH) && defined(_AUTH)
	CDnSDOAService::GetInstance().ShowLoginDialog( bShow );

	if( !bShow )
		return;

	bool bMoved = false;
	int nX, nY, nWidth, nHeight;
	if( CDnSDOAService::GetInstance().GetLoginDialogRect( nX, nY, nWidth, nHeight ) )
	{
		// 로그인버튼, 종료버튼 바로 위에 뜨게 한다.
		CEtUIButton *pButtonLogin = GetControl<CEtUIButton>( "ID_LOGIN_OK" );
		CEtUIButton *pButtonCancel = GetControl<CEtUIButton>( "ID_LOGIN_CANCEL" );
		float fX1 = pButtonLogin->GetUICoord().fX + pButtonLogin->GetUICoord().fWidth / 2.0f;
		float fX2 = pButtonCancel->GetUICoord().fX + pButtonCancel->GetUICoord().fWidth / 2.0f;
		float fY1 = pButtonLogin->GetUICoord().fY;

		// 클라이언트 영역좌표
		int nBaseX = (int)((GetXCoord() + ((fX1 + fX2) / 2.0f)) * GetScreenWidth());
		int nBaseY = (int)((GetYCoord() + fY1) * GetScreenHeight());

		int nNewX = nBaseX - nWidth/2;
		int nNewY = nBaseY - nHeight;
		if( CDnSDOAService::GetInstance().SetLoginDialogPosition( nNewX, nNewY ) )
			bMoved = true;
	}

	m_bSDOALoginDialogMoveUncompleted = !bMoved;
#endif	// #if defined(_CH) && defined(_AUTH)
}

void CDnLoginDlg::OnUICallbackProc( int nID, UINT nCommand, CEtUIControl *pControl, UINT uiMsg )
{
	SetCmdControlName( pControl->GetControlName() );

	switch( nID ) 
	{
	case MESSAGEBOX_1:
		{
			if( nCommand == EVENT_BUTTON_CLICKED ) 
			{
				if( IsCmdControl("ID_OK") )
				{
					RequestFocus(m_pEditBoxID);
					//m_bCallbackProcessed = true;
					// 위에 해결책이 나오면서 더 이상 필요없으나, 우선 코드는 그냥 두기로 한다.
					// 또 다른 해결책이었으므로.
				}
			}
		}
		break;
	case MESSAGEBOX_2:
		{
			if( nCommand == EVENT_BUTTON_CLICKED ) 
			{
				if( IsCmdControl("ID_OK") )
				{
#if (defined(_KR) || defined(_KRAZ) || defined(_JP) || defined(_US)) && defined(_AUTH)
					// 웹 로그인 방식일 경우 클라이언트를 종료시켜야 합니다.
					CTaskManager::GetInstance().RemoveAllTask( false );
					break;
#elif defined(_TW) && defined(_AUTH)
					DnTwnService* twnservice = static_cast<DnTwnService*>( g_pServiceSetup );
					if( twnservice->IsWebLogin() )
					{
						// 웹 로그인 방식일 경우 클라이언트를 종료시켜야 합니다.
						CTaskManager::GetInstance().RemoveAllTask( false );
						break;
					}
#endif
#if defined(_CH) && defined(_AUTH)
					// OA인증은 되었는데, 다른 곳에서 문제가 생겨 서버 에러 메세지가 떴을 때 OK누르면 이쪽으로 올 것이다.(거의 이런일 없을거다.)
					ShowSDOALoginDialog();
					break;
#endif	// #if defined(_CH) && defined(_AUTH)
					RequestFocus(m_pEditBoxPW);
					//m_bCallbackProcessed = true;
				}
			}
		}
		break;
	}
}

void CDnLoginDlg::Process( float fElapsedTime )
{
	CEtUIDialog::Process( fElapsedTime );

	if( IsShow() )
	{
#if defined(_CH) && defined(_AUTH)
		if( CDnSDOAService::GetInstance().IsShow() )
		{
			if( m_bSDOALoginDialogMoveUncompleted )
			{
				// ShowLoginDialog을 처음 호출한다음 바로 SetLoginDialogPosition을 호출할 경우
				// 아직 LoginDialog가 생성되기 전이라 그런지 GetWinProperty함수로 속성을 얻어오지 못한다.
				// 미리 SDOA 길쭉한 태스크바를 띄워놓지 않는 이상 다른 방법이 없어서 이렇게 프로세스 단에서 보여지도록 하겠다.
				// 한번만 이런 절차를 하고 나면 그 후엔 더 이상 같은 문제가 발생하지 않으니
				// 다른 다이얼로그에서는 이 과정을 할 필요 없을 것이다.
				ShowSDOALoginDialog();
			}
		}
#endif	// #if defined(_CH) && defined(_AUTH)
	}
}


