#include "StdAfx.h"
#include "DnSecurityEkeyDlg.h"
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
// EKey 사용시 
//--- A8 사용시 풀넘버 사용방식
//	A8 가운데 버튼 왼쪽 문서표시의 아이콘을 먼저 누른다.
//	서버로부터 받은 첼린지 번호 를 누른다. 
//	가운데 버튼 OK 버튼을 누르면 A8의 풀넘버를 보여주고 
//	이 풀넘버를 넣어주면된다.
//
//--- D6 X6 
//	서버로부터 첼린지 번호를 번호를 받는다.
//	이번호는 자릿수를 표시하고 해당 자릿수에 번호를 넣어주면 된다. 
//	ex) 929101  첼린지 : 4623  => 1129 
//


#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnSecurityEkeyDlg::CDnSecurityEkeyDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback )
: CEtUIDialog( dialogType, pParentDialog, nID, pCallback )
{
}

CDnSecurityEkeyDlg::~CDnSecurityEkeyDlg(void)
{
}

void CDnSecurityEkeyDlg::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "Security_Ekey_Number.ui" ).c_str(), bShow );
}

void CDnSecurityEkeyDlg::InitialUpdate()
{
	m_ChallengeKey.clear();
}

void CDnSecurityEkeyDlg::Show( bool bShow )
{
	if( !bShow )
	{
		GetControl<CEtUIEditBox>("ID_EDITBOX1")->ClearTextMemory();
	}

	CEtUIDialog::Show(bShow );
}

void CDnSecurityEkeyDlg::SetType(emSecurityType Stype )
{
	m_SecurityType = Stype; 
	WCHAR szStr[256]={0,};
	switch(Stype)
	{
		case emEKey:
			GetControl<CEtUIStatic>("ID_TEXT0")->SetText(GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 6469)); // 6469 Ekey입력 	break; 
			swprintf_s(szStr, _countof(szStr) ,  _T("%s : %s") ,GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 6470) , m_ChallengeKey.c_str() );	//6470 입력할코드값 
			GetControl<CEtUIStatic>("ID_TEXT1")->SetText(szStr);
			GetControl<CEtUIStatic>("ID_TEXT1")->Show(true);
			break; 
		case emECradNo: 			
			GetControl<CEtUIStatic>("ID_TEXT0")->SetText(GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 6473));break; //6473 EcardNo 
			GetControl<CEtUIStatic>("ID_TEXT1")->Show(false);
			break; 
		case emResvPass: 
			GetControl<CEtUIStatic>("ID_TEXT0")->SetText(GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 6472));break; //6472 예비키 
			GetControl<CEtUIStatic>("ID_TEXT1")->Show(false);
			break; 
	}
}

void CDnSecurityEkeyDlg::ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg )
{
	SetCmdControlName( pControl->GetControlName() );

	if( nCommand == EVENT_BUTTON_CLICKED )
	{
		if( IsCmdControl( "ID_BUTTON_CLOSE" ) || IsCmdControl("ID_BUTTON_CANCEL")) 
		{
			GetInterface().GetLoginDlg()->EnableLoginButton( true );
			Show( false );
		}
		else if(IsCmdControl( "ID_BUTTON_OK" ))
		{
			std::wstring str; 
			str = GetControl<CEtUIEditBox>("ID_EDITBOX1")->GetText();
			CDnLoginTask *pTask = (CDnLoginTask *)CTaskManager::GetInstance().GetTask("LoginTask");

			switch(m_SecurityType)
			{
			case emEKey: 
				{
					if( pTask )
					{
						pTask->RequestCheckEKey((WCHAR*)str.c_str());
					}
					break; 
				}
			
			case emECradNo: 
				{
					if( pTask )
					{
						pTask->RequestCheckECardNo((WCHAR*)str.c_str());
					}
				
					break; 
				}
			case emResvPass: 
				{
					if( pTask )
					{
						pTask->RequestCheckEKeyResvPass((WCHAR*)str.c_str());
					}
					break; 
				}
			}
			Show( false );
		}
	}

	CEtUIDialog::ProcessCommand( nCommand, bTriggeredByUser, pControl, uMsg );
}

void CDnSecurityEkeyDlg::Process( float fElapsedTime )
{
	CEtUIDialog::Process( fElapsedTime ); 
	
	if( !IsShow() )
		return;

}

void CDnSecurityEkeyDlg::SetChallengeKey(WCHAR *pStr)
{
	m_ChallengeKey = pStr; 
}

#endif 
#endif	// #if defined(PRE_ADD_CH_SNDAAUTH_02)

*/