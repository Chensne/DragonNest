#include "StdAfx.h"
#include "DnSecurityNUMPADCheckDlg.h"
#include "DnInterface.h"
#ifdef PRE_ADD_VIP
#else
#include "DnAuthTask.h"
#endif // PRE_ADD_VIP
#include "TaskManager.h"
#include "DnLoginTask.h"
#include <MMSystem.h>

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnSecurityNUMPADCheckDlg::CDnSecurityNUMPADCheckDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback )
: CEtUIDialog( dialogType, pParentDialog, nID, pCallback  , true )
{
	m_SecondPass1.Clear();
	m_vecNumberStr.clear();
	m_pEditCurPw = NULL;
	m_pEditEnterPw = NULL;
	m_pText_Unlock = NULL;
}

CDnSecurityNUMPADCheckDlg::~CDnSecurityNUMPADCheckDlg(void)
{
	m_SecondPass1.Clear();
	m_vecNumberStr.clear();
}

void CDnSecurityNUMPADCheckDlg::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "SecurityNUMPAD_Check.ui" ).c_str(), bShow );
}

void CDnSecurityNUMPADCheckDlg::InitialUpdate()
{
	char Strtmp[1024]={0,};
	WCHAR wszBuffer[32]={0,};

	for(int i = 0 ; i< 10 ; ++i)
	{
		sprintf_s(Strtmp , 1024 , "ID_BUTTON_NUM%d" , i );
		m_pBtnNumPad.push_back(GetControl<CEtUIButton>( Strtmp));
		m_vecNumberStr.push_back( m_pBtnNumPad[i]->GetText() );
	}


	m_pEditCurPw = GetControl<CEtUIEditBox>("ID_EDITBOX0") ; 
	m_pEditCurPw->KeyLock(false);
	m_pEditCurPw->ClearTextMemory();

	m_pBtnDelete = GetControl<CEtUIButton>("ID_BTN_DELETE");

#ifdef PRE_MOD_SECURITYNUMPAD_RENEWAL
	m_pBtnShuffle = GetControl<CEtUIButton>("ID_BTN_SHUFFLE");
#endif
	//m_pText_Unlock = 
	WCHAR wzStrTmp[256]={0,};
	swprintf_s(wzStrTmp , _countof(wzStrTmp) , GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 6426 )  ,(int)SecondAuth::Common::LimitCount  , SecondAuth::Common::RestraintTimeMin );
	GetControl<CEtUIStatic>("ID_TEXT4")->SetText(wzStrTmp);
	
	

}

void CDnSecurityNUMPADCheckDlg::Show( bool bShow , int AuthType /* = 2 */ )
{
	if( bShow == m_bShow )
		return;
	
#ifdef PRE_ADD_VIP
	m_emAuthCheckType = (CDnAuthTask::emAuthCheckType)AuthType;
#else
	m_emAuthCheckType = (emAuthCheckType)AuthType;
#endif // PRE_ADD_VIP
	
	//ȣ����� ���� .. ���� .. �����ְ� .. 
	CEtUIDialog::Show( bShow );

	

	if( !bShow )
	{
	}
	else 
	{
		InitializeNumber();
		SetCurrentFocus( emAuthPwFocus::NewPw) ;
		ShuffleNumber();
		m_SecondPass2.nSeed = m_SecondPass1.nSeed;
#ifdef PRE_MDD_USA_SECURITYNUMPAD
		GetControl<CEtUIButton>("ID_BUTTON_DEL")->Show(false);
#endif 
	}
}

bool CDnSecurityNUMPADCheckDlg::MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	if( !IsShow() )
		return false;

	if( uMsg == WM_KEYDOWN )
	{
		return true;
	}
	
	else if ( uMsg == WM_LBUTTONDOWN )
	{
		SetCurrentFocus(m_eAuthPwFocus);
		POINT MousePoint;
		float fMouseX, fMouseY;
		MousePoint.x = short( LOWORD( lParam ) );
		MousePoint.y = short( HIWORD( lParam ) );
		PointToFloat( MousePoint, fMouseX, fMouseY );
	
		if( m_pBtnDelete->IsInside(fMouseX ,fMouseY ) ) 
		{
		
			MouseLClickPress = true; 
			m_fTime = 0.0f; 
		}
	}
#ifdef PRE_MOD_SECURITYNUMPAD_RENEWAL
	else if ( uMsg == WM_MOUSEMOVE )
	{
		POINT MousePoint;
		float fMouseX, fMouseY;
		MousePoint.x = short( LOWORD( lParam ) );
		MousePoint.y = short( HIWORD( lParam ) );
		PointToFloat( MousePoint, fMouseX, fMouseY );

		if( CheckAreaSecurityNumber( fMouseX ,fMouseY ) )
		{
			CDnMouseCursor::GetInstance().ShowCursor( false );

			if( m_vecNumberStrCopy.size() == ( m_pBtnNumPad.size() + 2 ) )
				return false;

			for( int i = 0 ; i< (int)m_pBtnNumPad.size() ; ++i )
			{
				m_vecNumberStrCopy.push_back ( m_pBtnNumPad[i]->GetText() );
				m_pBtnNumPad[i]->ClearText();
			}
			m_vecNumberStrCopy.push_back ( m_pBtnDelete->GetText() );
			m_vecNumberStrCopy.push_back ( m_pBtnShuffle->GetText() );

			m_pBtnDelete->ClearText();
			m_pBtnShuffle->ClearText();

		}
		else 
		{
			for( int i = 0 ; i < (int)m_vecNumberStrCopy.size(); ++i )
			{
				if( i < 10)	m_pBtnNumPad[i]->SetText( m_vecNumberStrCopy[i].c_str() );
				else if( i == 10 ) m_pBtnDelete->SetText( m_vecNumberStrCopy[10].c_str() );
				else if( i == 11) m_pBtnShuffle->SetText( m_vecNumberStrCopy[11].c_str() );
			}
			m_vecNumberStrCopy.clear();

			CDnMouseCursor::GetInstance().ShowCursor( true );
		}
	}
#endif 


	return CEtUIDialog::MsgProc(hWnd, uMsg, wParam, lParam);
}

#ifdef PRE_MOD_SECURITYNUMPAD_RENEWAL
bool CDnSecurityNUMPADCheckDlg::CheckAreaSecurityNumber( float fMouseX, float fMouseY )
{	
	// ���콺 Ű�е� ��ü �˻� 
	int nSize = (int)m_pBtnNumPad.size();
	for( int i = 0; i < nSize; ++i )
		if( m_pBtnNumPad[i]->IsInside( fMouseX, fMouseY ) )	return true;

	if( m_pBtnDelete->IsInside( fMouseX, fMouseY ) )	return true;
	if( m_pBtnShuffle->IsInside( fMouseX, fMouseY ) )	return true;

	return false;
}
#endif 

void CDnSecurityNUMPADCheckDlg::Process( float fElapsedTime )
{
	CEtUIDialog::Process( fElapsedTime ); 

	if( IsShow() ) {
		bool bCanShuffle = false;

		if( IsCurPwFocus(emAuthPwFocus::NewPw) && m_SecondPass1.IsEmpty() ) bCanShuffle = true;
		else if( IsCurPwFocus(emAuthPwFocus::EnterPw) && m_SecondPass2.IsEmpty() ) bCanShuffle = true;

		if( bCanShuffle != GetControl<CEtUIButton>("ID_BTN_SHUFFLE")->IsEnable() )
			GetControl<CEtUIButton>("ID_BTN_SHUFFLE")->Enable( bCanShuffle );
	}

	if( MouseLClickPress )
	{
		m_fTime += fElapsedTime ; 
		
		if( m_fTime > DELETE_BTN_DELAY)
		{
			m_fTime = 0.0f;
			MouseLClickPress = false; 
			SetCurrentFocus(m_eAuthPwFocus);
			if( IsCurPwFocus(emAuthPwFocus::NewPw) )
			{
				if( !m_SecondPass1.IsEmpty() )
				{
					m_SecondPass1.Clear( false );
					m_pEditCurPw->SetText( m_SecondPass1.GetStr().c_str() );
				}
			}
			else 
			{
				if( !m_SecondPass2.IsEmpty() )
				{
					m_SecondPass2.Clear( false );
					m_pEditEnterPw->SetText( m_SecondPass2.GetStr().c_str() );
				}
			}
		}
	}
	else 
	{
		if( m_fTime > 0.00001f )
		{
			m_fTime = 0.0f; 
			SetCurrentFocus(emAuthPwFocus::NewPw); 
			if( !m_SecondPass1.IsEmpty() )
			{
				m_SecondPass1.RemoveValue();
				m_pEditCurPw->SetText( m_SecondPass1.GetStr().c_str() );
			}
		}
	}
}


void CDnSecurityNUMPADCheckDlg::ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg )
{
	SetCmdControlName( pControl->GetControlName() );

	if( nCommand == EVENT_BUTTON_CLICKED )
	{
		if ( IsCmdControl("ID_BTN_SHUFFLE"))
		{
			SetCurrentFocus(emAuthPwFocus::NewPw); 
			ShuffleNumber();
		}
		else if ( IsCmdControl("ID_BTN_DELETE"))
		{
			MouseLClickPress = false; 
		}
		else if( IsCmdControl( "ID_BUTTON_CLOSE" )  || IsCmdControl( "ID_BUTTON_CANCEL" ) )
		{
			Show( false );
		}
		else if( IsCmdControl( "ID_BUTTON_OK" ))
		{
			SendAuthLock();
		}
		else if( IsCmdControl( "ID_BUTTON_CHG")) 
		{
			GetInterface().OpenSecurityChangeDlg();
			Show( false );
		}
		else if( IsCmdControl( "ID_BUTTON_DEL"))
		{
			GetInterface().OpenSecurityDelete();
			Show( false );
		}
		else if( IsCmdControl("ID_BTN_PW") )
		{			
			SetCurrentFocus(emAuthPwFocus::NewPw);
		}
	
		if( strstr( pControl->GetControlName() , "ID_BUTTON_NUM"))
		{
			for( int i = 0 ; i < (int)m_pBtnNumPad.size() ; ++i )
			{
				if( m_pBtnNumPad[i] == pControl ) 
				{	
					if( IsCurPwFocus(emAuthPwFocus::NewPw)  ) 
					{
						SetCurrentFocus(emAuthPwFocus::NewPw); 

						if( m_SecondPass1.GetSize() < SecondAuth::Common::PWMaxLength )
						{
							m_SecondPass1.AddValue(i);
							m_pEditCurPw->SetText( m_SecondPass1.GetStr().c_str() );
						}
					}
				}
			}
		}
	}

	CEtUIDialog::ProcessCommand( nCommand, bTriggeredByUser, pControl, uMsg );
}

void CDnSecurityNUMPADCheckDlg::SendAuthLock()
{

	switch(m_emAuthCheckType)
	{
#ifdef PRE_ADD_VIP
		case CDnAuthTask::LoginCharDel:
#else
		case emAuthCheckType::LoginCharDel:
#endif
		{
			//�ƴϸ� ���������̳�.. 
			CDnLoginTask *pLoginTask = (CDnLoginTask *)CTaskManager::GetInstance().GetTask("LoginTask");
			if( pLoginTask) 
			{
				pLoginTask->SetAuthPassword( m_SecondPass1.nSeed, m_SecondPass1.nValue );
				pLoginTask->DeleteCharacter();
			}
			break; 
		}
#ifdef PRE_ADD_VIP
		case CDnAuthTask::Login:
#else
		case emAuthCheckType::Login:
#endif
		{
			//ĳ���� �����̳� 
			CDnLoginTask* pLoginTask = (CDnLoginTask*)CTaskManager::GetInstance().GetTask("LoginTask");
			pLoginTask->SetAuthPassword( m_SecondPass1.nSeed, m_SecondPass1.nValue );
			pLoginTask->ChangeState(CDnLoginTask::LoginStateEnum::ChannelList);
			break;
		}
#ifdef PRE_ADD_VIP
		case CDnAuthTask::Game:
#else
		case emAuthCheckType::Game:
#endif
		{
			CDnAuthTask *pAuthTask = (CDnAuthTask *)CTaskManager::GetInstance().GetTask("AuthTask");
			if( !pAuthTask ) return ; 
			break; 
		}
	#ifdef PRE_ADD_VIP
		case CDnAuthTask::Validate_VIP:
	#else
		case emAuthCheckType::Validate_VIP:
	#endif // PRE_ADD_VIP
		{
			CDnAuthTask *pAuthTask = (CDnAuthTask *)CTaskManager::GetInstance().GetTask("AuthTask");
			if( !pAuthTask ) return ;

			break;
		}
	}

	Show(false);
}


bool CDnSecurityNUMPADCheckDlg::IsCurPwFocus(emAuthPwFocus AuthPwFocus)
{  
	return 	(m_eAuthPwFocus == AuthPwFocus) ? TRUE : FALSE; 
}

void CDnSecurityNUMPADCheckDlg::SetCurrentFocus(emAuthPwFocus AuthPwFocus)
{
	m_eAuthPwFocus = AuthPwFocus; 

	switch( AuthPwFocus  ) 
	{
	case emAuthPwFocus::NewPw:
		{
			RequestFocus(m_pEditCurPw);
		}
		break; 
	}

};

void CDnSecurityNUMPADCheckDlg::ShuffleNumber()
{

	std::vector<std::string> tokens;
	TokenizeA(std::string(SecondPassword::GetVersion()), tokens, ".");
	int nCount = rand()%( atoi(tokens[1].c_str())/20);
	int nSeed = 0;
	if( nCount%6 == 0 ) SuffleNumber1(m_vecNumberStr, m_pBtnNumPad, nCount, &nSeed );
	else if( nCount%6 == 1 ) SuffleNumber2(m_vecNumberStr, m_pBtnNumPad, nCount, &nSeed );
	else if( nCount%6 == 2 ) SuffleNumber3(m_vecNumberStr, m_pBtnNumPad, nCount, &nSeed );
	else if( nCount%6 == 3 ) SuffleNumber4(m_vecNumberStr, m_pBtnNumPad, nCount, &nSeed );
	else if( nCount%6 == 4 ) SuffleNumber5(m_vecNumberStr, m_pBtnNumPad, nCount, &nSeed );
	else if( nCount%6 == 5 ) SuffleNumber6(m_vecNumberStr, m_pBtnNumPad, nCount, &nSeed );

	if( IsCurPwFocus(emAuthPwFocus::NewPw) )
	{
		m_SecondPass1.nSeed = nSeed;
		m_SecondPass1.Clear( false );
		m_pEditCurPw->SetText( m_SecondPass1.GetStr().c_str() );
	}
	else if( IsCurPwFocus(emAuthPwFocus::EnterPw) )
	{
		m_SecondPass2.nSeed = nSeed;
		m_SecondPass2.Clear( false );
		m_pEditEnterPw->SetText( m_SecondPass2.GetStr().c_str() );
	}
}

void CDnSecurityNUMPADCheckDlg::InitializeNumber()
{
	m_SecondPass1.Clear();
	m_SecondPass2.Clear();
	m_pEditCurPw->ClearTextMemory();

	WCHAR wszBuffer[32]={0,};

	for(int i = 0 ; i< 10 ; ++i)
	{
		m_pBtnNumPad[i]->SetText(m_vecNumberStr[i].c_str());
	}
}

