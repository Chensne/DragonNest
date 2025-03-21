#include "StdAfx.h"
#include "DnGameProfileOptDlg.h"
#include "DnInterface.h"
#include "DnInterfaceString.h"
#include "DnGameOptionTask.h"
#include "GameOptionSendPacket.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnGameProfileOptDlg::CDnGameProfileOptDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback )
: CEtUIDialog( dialogType, pParentDialog, nID, pCallback, true )
, m_pCheckMale(NULL)
, m_pCheckFemale(NULL)
, m_pEditBoxPR(NULL)
, m_pCheckOpen(NULL)
{
}

CDnGameProfileOptDlg::~CDnGameProfileOptDlg(void)
{
}

void CDnGameProfileOptDlg::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "ProfileOptDlg.ui" ).c_str(), bShow );

	if( CDnGameOptionTask::IsActive() )
	{
		GetGameOptionTask().SetProfileDialog( this );
	}
	else
	{
		CDebugSet::ToLogFile( "CDnGameProfileOptDlg::Initialize, 프로필 다이얼로그가 만들어 지기 전에 게임옵션 테스크가 생성되어야 합니다." );
	}
}

void CDnGameProfileOptDlg::InitialUpdate()
{
#ifdef PRE_FIX_USA_PROFILE
#else
	m_pCheckMale = GetControl<CEtUICheckBox>("ID_CHECKBOX_MALE");
	m_pCheckFemale = GetControl<CEtUICheckBox>("ID_CHECKBOX_FEMALE");
#endif
	m_pEditBoxPR = GetControl<CEtUILineIMEEditBox>("ID_LINEIMEEDITBOX_PR");
	m_pCheckOpen = GetControl<CEtUICheckBox>("ID_CHECKBOX_OPEN");

	m_pEditBoxPR->SetMaxEditLength( PROFILEGREETINGLEN+1 );
}

void CDnGameProfileOptDlg::ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg )
{
	SetCmdControlName( pControl->GetControlName() );

	if( nCommand == EVENT_BUTTON_CLICKED )
	{
		if( IsCmdControl("ID_OK") )
		{
			// 라인에딧박스에서 개행문자는 SetMaxEditLength검사할때 안속하는지, 중간에 개행이 들어가도 24개의 글자까지는 써진다.
			// 문제는 이렇게 얻은 스트링에서 개행을 스페이스로 바꾸고 24개 문자 얻어오면 뒤에 글자 몇개가 짤려버린다.
			// 그래서 적혀있는 글자를 다 얻어올 수 있게 하기 위해 넉넉하게 스트링 버퍼를 잡아 읽어온 후 가공처리 하겠다.
			wchar_t wszTemp[PROFILEGREETINGLEN+5] = {0, };
			m_pEditBoxPR->GetTextCopy( wszTemp, PROFILEGREETINGLEN+5 );

			// 이상하게 마지막에 개행문자가 온다. 널문자로 강제 지정.
			int nLen = (int)wcslen( wszTemp );			
			wszTemp[nLen-1] = '\0';

			// 다시 길이를 재서 사이에 들어있는 개행문자를 공백문자로 바꾼 후
			nLen = (int)wcslen( wszTemp );
			for( int i = 0; i < nLen; ++i )
			{
				if( wszTemp[i] == '\n' )
					wszTemp[i] = ' ';
			}

			// 길이가 맥스치보다 작으면 그대로 전송하면 끝. 그러나 맥스치보다 크다면 최대한 공백을 줄이고 앞으로 당긴다.
			std::wstring wszPR;
			if( nLen > PROFILEGREETINGLEN )
			{
				wchar_t wszTemp2[PROFILEGREETINGLEN+1] = {0, };
				int nIndex = 0;
				for( int j = 0; j < nLen; ++j )
				{
					if( wszTemp[j] != ' ' )
					{
						wszTemp2[nIndex] = wszTemp[j];
						++nIndex;
						if( nIndex == PROFILEGREETINGLEN )
							break;
					}
				}
				wszPR = wszTemp2;
			}
			else
			{
				wszPR = wszTemp;
			}

			if( m_pCheckOpen->IsChecked() )
			{
				bool bEnable = true;
#ifdef PRE_FIX_USA_PROFILE
#else
				if( m_pCheckMale->IsChecked() == false && m_pCheckFemale->IsChecked() == false )
					bEnable = false;
#endif
				
				if( wszPR.empty() )
					bEnable = false;
				// 저장 안된 정보 있는지 확인.
				if( !bEnable )
				{
					GetInterface().MessageBox( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 8141 ) );
					return;
				}
			}

			if( DN_INTERFACE::UTIL::CheckChat( wszPR ) )
			{
				GetInterface().MessageBox( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3772 ), MB_OK, 0, this );
				return;
			}

			// PR정보 셋팅 후 Send
			BYTE cGender = 0;
#ifdef PRE_FIX_USA_PROFILE
#else
			if( m_pCheckMale->IsChecked() ) cGender = 1;
			else if( m_pCheckFemale->IsChecked() ) cGender = 2;
#endif
			SendSetProfile( cGender, wszPR.c_str(), m_pCheckOpen->IsChecked() );

			// 응답 받을 필요 없을거다. 저장하는데 문제 될일 없다고 가정.
			GetInterface().AddChatMessage( CHATTYPE_SYSTEM, NULL, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 100060 ) );
			return;
		}
		else if( IsCmdControl("ID_CANCEL") )
		{
			Show(false);
			return;
		}
	}
	else if( nCommand == EVENT_CHECKBOX_CHANGED )
	{
#ifdef PRE_FIX_USA_PROFILE
#else
		if( IsCmdControl("ID_CHECKBOX_MALE") )
		{
			m_pCheckFemale->SetChecked( !m_pCheckMale->IsChecked(), false );
		}
		else if( IsCmdControl("ID_CHECKBOX_FEMALE") )
		{
			m_pCheckMale->SetChecked( !m_pCheckFemale->IsChecked(), false );
		}
#endif
	}

	CEtUIDialog::ProcessCommand( nCommand, bTriggeredByUser, pControl, uMsg );
}

void CDnGameProfileOptDlg::OnUICallbackProc( int nID, UINT nCommand, CEtUIControl *pControl, UINT uiMsg )
{
	SetCmdControlName( pControl->GetControlName() );

	if( nCommand == EVENT_BUTTON_CLICKED )
	{
		if( IsCmdControl( "ID_OK" ) )
		{
			RequestFocus( m_pEditBoxPR );
		}
	}
}

void CDnGameProfileOptDlg::Show( bool bShow ) 
{
	if( m_bShow == bShow )
		return;

	if( bShow )
	{
		// 컨트롤 초기화
#ifdef PRE_FIX_USA_PROFILE
#else
		m_pCheckMale->SetChecked( false, false );
		m_pCheckFemale->SetChecked( false, false );
#endif
		m_pEditBoxPR->ClearText();
		m_pCheckOpen->SetChecked( false, false );

		// 마을진입과 상관없이 프로필 설정창 열때마다 요청하는 구조로 가기로 함.
		// 한번 설정해두면 거의 다시 열지 않을 거 같아서.
		SendGetProfile();
	}

	CEtUIDialog::Show( bShow );
}

void CDnGameProfileOptDlg::SetProfile( TProfile *pProfile )
{
#ifdef PRE_FIX_USA_PROFILE
#else
	m_pCheckMale->SetChecked( pProfile->cGender == 1, false );
	m_pCheckFemale->SetChecked( pProfile->cGender == 2, false );
#endif
	m_pEditBoxPR->SetTextBuffer( pProfile->wszGreeting );
	m_pCheckOpen->SetChecked( pProfile->bOpenPublic );
}
