#include "StdAfx.h"
#include "DnLatencyDlg.h"
#include "DnTableDB.h"
#include "DnFarmGameTask.h"
#include "TaskManager.h"
#include "DnInterface.h"
#include "DnQuestTask.h"
#include "DnLocalPlayerActor.h"
#include "DnGuildWarTask.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)	
#endif

//rlkt_ok 19.06
CDnLatencyDlg::CDnLatencyDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback )
: CEtUIDialog( dialogType, pParentDialog, nID, pCallback )
{
	for(int i=0;i<3;i++)
	{
		m_pStaticLag[i] = NULL;
	}
}

CDnLatencyDlg::~CDnLatencyDlg(void)
{
}

void CDnLatencyDlg::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "delaydlg.ui" ).c_str(), bShow );
}

void CDnLatencyDlg::InitialUpdate()
{	
	m_pStaticLag[0] = GetControl<CEtUIStatic>("ID_STATIC_DELAY0");
	m_pStaticLag[1] = GetControl<CEtUIStatic>("ID_STATIC_DELAY1");
	m_pStaticLag[2] = GetControl<CEtUIStatic>("ID_STATIC_DELAY2");
}

void CDnLatencyDlg::SetDelay(int ping)
{	
	for(int i=0;i<3;i++)
	{
		m_pStaticLag[i]->Show(false);
	}
	//rlkt_ok
	
	if(ping > 0 && ping <= 100)
	{
		m_pStaticLag[2]->Show(true);
		m_pStaticLag[2]->SetTooltipText(FormatW(L"�����ӳ�: %dms",ping).c_str());
	}
	else if(ping > 100 && ping <= 300)
	{
		m_pStaticLag[1]->Show(true);
		m_pStaticLag[1]->SetTooltipText(FormatW(L"�����ӳ�: %dms",ping).c_str());
	}
	else if(ping > 300)
	{
		m_pStaticLag[0]->Show(true);
		m_pStaticLag[0]->SetTooltipText(FormatW(L"�����ӳ�: %dms",ping).c_str());
	}
}


void CDnLatencyDlg::Render(float  fElapsedTime)
{
	CEtUIDialog::Render( fElapsedTime );
}
