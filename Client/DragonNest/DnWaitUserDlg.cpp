#include "StdAfx.h"
#include "DnWaitUserDlg.h"
#include "LoginSendPacket.h"
#include "DnInterface.h"
#ifdef PRE_MOD_SELECT_CHAR
#include "TaskManager.h"
#include "DnLoginTask.h"
#endif // PRE_MOD_SELECT_CHAR


#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnWaitUserDlg::CDnWaitUserDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback )
: CEtUIDialog( dialogType, pParentDialog, nID, pCallback )
, m_pServerName(NULL)
, m_pPeopleNum(NULL)
, m_pWaitTime(NULL)
, m_pQuitButton(NULL)
{
}

CDnWaitUserDlg::~CDnWaitUserDlg(void)
{
}

void CDnWaitUserDlg::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "WaitListDlg.ui" ).c_str(), bShow );
}

void CDnWaitUserDlg::InitialUpdate()
{
	m_pServerName = GetControl<CEtUIStatic>("ID_SERVER");
	m_pPeopleNum = GetControl<CEtUIStatic>("ID_PEOPLE_NUM");
	m_pWaitTime = GetControl<CEtUIStatic>("ID_TIME_NUM");
	m_pQuitButton = GetControl<CEtUIButton>("ID_BUTTON0");

	GetControl<CEtUIStatic>("ID_TIME")->Show( false );
	m_pWaitTime->Show( false );

	m_wszServerNameStatic = m_pServerName->GetText();
}

void CDnWaitUserDlg::SetProperty( LPCWSTR  wszServerName, UINT nWaitUserCount, ULONG nEstimateTime )
{
	WCHAR wszTemp[256] = { 0, };
	wsprintf( wszTemp, L"%s %s", m_wszServerNameStatic.c_str(), wszServerName );
	m_pServerName->SetText( wszTemp );

	wchar_t wszWaitUserCount[255]	={0,};
	wsprintf(wszWaitUserCount, L"%d %s", nWaitUserCount,  GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 1208 ) );
	m_pPeopleNum->SetText( wszWaitUserCount );

	wchar_t wszEstimateTime[255]	={0,};
	wsprintf(wszEstimateTime, L"%d %s", nEstimateTime, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 1209 ));
	m_pWaitTime->SetText( wszEstimateTime );
}

void CDnWaitUserDlg::ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg )
{
	SetCmdControlName( pControl->GetControlName() );

	if( nCommand == EVENT_BUTTON_CLICKED )
	{
		if( IsCmdControl("ID_BUTTON0" ) ) 
		{
#ifdef PRE_MOD_SELECT_CHAR
			CDnLoginTask* pTask = (CDnLoginTask *)CTaskManager::GetInstance().GetTask( "LoginTask" );
			if( pTask )
			{
				pTask->ChangeState( CDnLoginTask::CharSelect, false );
				SendBackButton();
			}
#else // PRE_MOD_SELECT_CHAR
			SendServerList();
			GetInterface().ShowServerSelectDlg( true );
#endif // PRE_MOD_SELECT_CHAR
			Show( false );
		}
	}
}
