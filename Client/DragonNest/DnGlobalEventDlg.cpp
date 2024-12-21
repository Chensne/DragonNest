#include "StdAfx.h"
#include "DnGlobalEventDlg.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnGlobalEventDlg::CDnGlobalEventDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback )
: CEtUIDialog( dialogType, pParentDialog, nID, pCallback , true )
{
	m_pStaticCount = NULL;
	m_pStaticPercent = NULL;
	m_pProgressBar = NULL;

}

CDnGlobalEventDlg::~CDnGlobalEventDlg(void)
{
}

void CDnGlobalEventDlg::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "Event_Global.ui" ).c_str(), bShow );
}

void CDnGlobalEventDlg::InitialUpdate()
{
	m_pStaticCount = GetControl<CEtUIStatic>("ID_TEXT_COUNT");
	m_pStaticPercent = GetControl<CEtUIStatic>("ID_STATIC1");
	m_pProgressBar = GetControl<CEtUIProgressBar>("ID_PROGRESSBAR0");
	if(m_pProgressBar)
		m_pProgressBar->SetProgress(0.f);
}

void CDnGlobalEventDlg::Show( bool bShow )
{ 
	if( bShow == m_bShow )
		return;

	CEtUIDialog::Show( bShow );
}

void CDnGlobalEventDlg::SetEventInfo(SCScorePeriodQuest* pInfo)
{
	if(pInfo->nMaxCount <= 0 )
		return;

	WCHAR wszMsg[256] = {0,};
	wsprintf( wszMsg, L"%d%%", int(((float)pInfo->nCurrentCount / (float)pInfo->nMaxCount) * 100.f) );
	m_pStaticPercent->SetText(wszMsg);
	wsprintf( wszMsg, L"%d / %d",pInfo->nCurrentCount , pInfo->nMaxCount );
	m_pStaticCount->SetText(wszMsg);

	m_pProgressBar->SetProgress(((float)pInfo->nCurrentCount / (float)pInfo->nMaxCount) * 100.f );
	
}

void CDnGlobalEventDlg::ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg )
{
	SetCmdControlName( pControl->GetControlName() );

	if( nCommand == EVENT_BUTTON_CLICKED )
	{
		if( IsCmdControl("ID_BT_CLOSE"))	
			Show(false);
	}
}