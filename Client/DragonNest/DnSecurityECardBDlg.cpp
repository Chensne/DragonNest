#include "StdAfx.h"
#include "DnSecurityECardBDlg.h"
#include "DnInterface.h"
#include "DnChannelListDlg.h"
#include "DnLoginTask.h"
#include "TaskManager.h"
#include "DnInterface.h"
#include "DnLoginDlg.h"

/*
#if defined(PRE_ADD_CH_SNDAAUTH_02)
#else	// #if defined(PRE_ADD_CH_SNDAAUTH_02)
#ifdef PRE_ADD_CH_SNDAAUTH_01

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnSecurityECardBDlg::CDnSecurityECardBDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback )
: CEtUIDialog( dialogType, pParentDialog, nID, pCallback )
{
}

CDnSecurityECardBDlg::~CDnSecurityECardBDlg(void)
{
}

void CDnSecurityECardBDlg::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "Security_Ekey_Table02.ui" ).c_str(), bShow );
}

void CDnSecurityECardBDlg::InitialUpdate()
{
	ResetArray();
}

void CDnSecurityECardBDlg::ResetArray()
{
	char str[256]={0,};
	for(int i = 0 ; i < 120; ++i )
	{
		sprintf_s(str , _countof(str) , "ID_STATIC_COVER%d" , i );
		GetControl<CEtUIStatic>(str)->Show(false);
	}

}

void CDnSecurityECardBDlg::Show( bool bShow )
{
	if( m_bShow == bShow) return; 
	
	CEtUIDialog::Show(bShow );

	GetControl<CEtUIEditBox>("ID_EDITBOX_EKEY0")->ClearText();
	GetControl<CEtUIEditBox>("ID_EDITBOX_EKEY1")->ClearText();
	GetControl<CEtUIEditBox>("ID_EDITBOX_EKEY2")->ClearText();



}

int CDnSecurityECardBDlg::ExtractNumber(WCHAR *szStr) 
{
	int strlen = (int)wcslen( szStr );
	int nXPos  = ( szStr[0]- L'A' ) + 1;
	int nYPos  = _wtoi(szStr+1);
	int nindex = (((nYPos-1)*8) + nXPos)-1; 

	return nindex;
}

void CDnSecurityECardBDlg::SetECardCood(WCHAR *szCood)
{	
	m_szPosStr.clear();
	m_szECardCood = szCood;

	//좌표를 분리해낸다
	std::vector<std::wstring> tokens;
	TokenizeW( m_szECardCood , tokens, std::wstring(L"|"));

	for(int i = 0;i< (int)tokens.size();i++)
	{
		m_szPosStr.push_back(tokens[i].c_str());
		char szTemp[256]={0,};
		sprintf_s(szTemp , "ID_STATIC_COVER%d" , ExtractNumber((WCHAR*)m_szPosStr[i].c_str()) );
		GetControl<CEtUIStatic>(szTemp)->Show(true);
	}	

	GetControl<CEtUIStatic>("ID_TEXT_CODE0")->SetText(m_szPosStr[0]);
	GetControl<CEtUIStatic>("ID_TEXT_CODE1")->SetText(m_szPosStr[1]);
	GetControl<CEtUIStatic>("ID_TEXT_CODE2")->SetText(m_szPosStr[2]);

	//ABCDE


}


void CDnSecurityECardBDlg::ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg )
{
	SetCmdControlName( pControl->GetControlName() );

	if( nCommand == EVENT_BUTTON_CLICKED )
	{
		if( IsCmdControl( "ID_BUTTON_CLOSE" ) || IsCmdControl( "ID_BUTTON_CANCEL" ) ) 
		{
			GetInterface().GetLoginDlg()->EnableLoginButton( true );
			ResetArray();
			Show( false );
		}
		else if( IsCmdControl( "ID_BUTTON_OK" ) ) 
		{
			
			WCHAR szstr[64]={0,};
			std::wstring szEdit0,szEdit1, szEdit2;
			szEdit0 = GetControl<CEtUIEditBox>("ID_EDITBOX_EKEY0")->GetText();
			szEdit1 = GetControl<CEtUIEditBox>("ID_EDITBOX_EKEY1")->GetText();
			szEdit2 = GetControl<CEtUIEditBox>("ID_EDITBOX_EKEY2")->GetText();

			swprintf_s(szstr , L"%s:%s|%s:%s|%s:%s",m_szPosStr[0].c_str() , szEdit0.c_str() , m_szPosStr[1].c_str() , szEdit1.c_str() , m_szPosStr[2].c_str() , szEdit2.c_str() );
			
			CDnLoginTask *pTask = (CDnLoginTask *)CTaskManager::GetInstance().GetTask("LoginTask");
			if( pTask )
			{
				pTask->RequestCheckECard(szstr);
			}

			ResetArray();
			Show( false );
		}
		
	}

	CEtUIDialog::ProcessCommand( nCommand, bTriggeredByUser, pControl, uMsg );
}

void CDnSecurityECardBDlg::Process( float fElapsedTime )
{
	CEtUIDialog::Process( fElapsedTime ); 
	
	if( !IsShow() )
		return;

	
}

#endif 
#endif	// #if defined(PRE_ADD_CH_SNDAAUTH_02)
*/