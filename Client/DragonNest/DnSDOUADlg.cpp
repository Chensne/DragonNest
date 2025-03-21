#include "StdAfx.h"
#include "DnSDOUADlg.h"
#include "DnSDOAService.h"
#include "DnInterface.h"
#include "DnLoginDlg.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

#if defined(_CH) && defined(_AUTH)

CDnSDOUADlg::CDnSDOUADlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback )
: CEtUIDialog( dialogType, pParentDialog, nID, pCallback, true )
, m_pIMEEditBoxName(NULL)
, m_pEditBoxNumber(NULL)
, m_pEditBoxEmail(NULL)
{
}

CDnSDOUADlg::~CDnSDOUADlg(void)
{
}

void CDnSDOUADlg::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "SDOUADlg.ui" ).c_str(), bShow );
}

void CDnSDOUADlg::InitialUpdate()
{
	m_pIMEEditBoxName = GetControl<CEtUIIMEEditBox>( "ID_INFO0" );
	m_pEditBoxNumber = GetControl<CEtUIEditBox>( "ID_INFO1" );
	m_pEditBoxEmail = GetControl<CEtUIEditBox>( "ID_INFO2" );
}

void CDnSDOUADlg::ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg )
{
	SetCmdControlName( pControl->GetControlName() );

	if( nCommand == EVENT_BUTTON_CLICKED )
	{
		if( IsCmdControl( "ID_OK" ) )
		{
			if( m_pEditBoxNumber->GetTextLength() == 0 ) 
			{
				GetInterface().MessageBox( MESSAGEBOX_1, MB_OK );
				return;
			}

			if( m_pEditBoxEmail->GetTextLength() == 0 ) 
			{
				GetInterface().MessageBox( MESSAGEBOX_2, MB_OK );
				return;
			}

			// 여기서 http 통신
			if( !MakeAndSendHttpInfo() )
				return;

			// 후에 원래 SDOA창 띄워주고
			if( GetInterface().GetLoginDlg() ) GetInterface().GetLoginDlg()->ShowSDOALoginDialog( true );
			Show( false );
			return;
		}
		else if( IsCmdControl( "ID_BT_CLOSE" ) )
		{
			if( GetInterface().GetLoginDlg() ) GetInterface().GetLoginDlg()->ShowSDOALoginDialog( true );
			Show( false );
			return;
		}
	}
	else if( nCommand == EVENT_EDITBOX_STRING )
	{
		focus::ReleaseControl();
	}

	CEtUIDialog::ProcessCommand( nCommand, bTriggeredByUser, pControl, uMsg );
}

void CDnSDOUADlg::Show( bool bShow ) 
{ 
	CEtUIDialog::Show( bShow );

	if( bShow )
	{
		m_pEditBoxNumber->Focus(false);
		m_pEditBoxEmail->Focus(false);
		RequestFocus(m_pIMEEditBoxName);
	}
	else
	{
		// CEtUIEditBox의 내용을 완전히 지운다.
		m_pIMEEditBoxName->ClearTextMemory();
		m_pEditBoxNumber->ClearTextMemory();
		m_pEditBoxEmail->ClearTextMemory();
	}
}

void CDnSDOUADlg::SetUrlInfo(WCHAR* wszAccountName, char* szUrlInfo)
{
	m_strAccountName = wszAccountName;
	ToWideString(szUrlInfo, m_strUrlInfo);
}

bool CDnSDOUADlg::MakeAndSendHttpInfo()
{	
	TCHAR szTemp[64] = {0,};		
	std::wstring strUrl = m_strUrlInfo;
	CHttpEncoderW::UrlEncode(szTemp, m_pIMEEditBoxName->GetText(), TRUE);		

	strUrl += L"&appid=89&username=";
	strUrl += szTemp;
	strUrl += L"&idcard=";
	strUrl += m_pEditBoxNumber->GetText();
	strUrl += L"&ptid=";
	strUrl += m_strAccountName;			
	WCHAR szLocalIP[ 32 ];
	CClientSessionManager::GetInstance().GetLocalIP( szLocalIP );
	strUrl += L"&endpointIP=";
	strUrl += szLocalIP;
	strUrl += L"&email=";
	strUrl += m_pEditBoxEmail->GetText();			

	char szResponse[256] = {0,};
	try
	{
		CHttpResponse* pResponse = m_HttpClient.RequestGet(strUrl.c_str());		
		if( pResponse )
		{
			pResponse->ReadContent((BYTE*)szResponse, 256);
			std::string strResponse = szResponse;
			RemoveStringA(strResponse, "\"");

			std::vector<std::string> tokens, words;
			TokenizeA(strResponse, tokens, ",");
			for( int i=0; i<tokens.size(); ++i)
			{
				TokenizeA(tokens[i], words, ":");
			}
			// {"result":"","code":"","msg":'',"guid":""}
			int nResult = atoi(words[1].c_str());
			if( nResult == 0)
				return true;
			else
			{				
				WCHAR strErrmsg[128] = {0,};
				CHttpEncoderA::UrlDecodeW(strErrmsg, words[5].c_str(), TRUE);			
				GetInterface().MessageBox(strErrmsg, MB_OK);
				return false;
			}			
		}
		return true;
	}
	catch (httpclientexceptionA&)	
	{
		return false;		
	}	
	return false;
}

#endif	// #if defined(_CH) && defined(_AUTH)