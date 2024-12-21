
#include "StdAfx.h"
#include "DnIntegrateEventDlg.h"
#include "DnAttendanceEventDlg.h"
#include "DnTimeEventDlg.h"
#include "DnInterface.h"
#include "DnMainDlg.h"
#include "DnMainMenuDlg.h"
#include "DnTimeEventTask.h"

#ifdef PRE_ADD_STAMPSYSTEM
#include "DnStampEventDlg.h"
#endif // PRE_ADD_STAMPSYSTEM

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

#if defined(PRE_ADD_INTEGERATE_EVENTUI)
CDnIntegrateEventDlg::CDnIntegrateEventDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback )
: CEtUITabDialog( dialogType, pParentDialog, nID, pCallback, true )
,m_pButtonTimeEventDlg(NULL)
,m_pButtonAttendanceEventDlg(NULL)
,m_pAttendanceEventDlg(NULL)
,m_pTimeEventDlg(NULL)
#ifdef PRE_ADD_STAMPSYSTEM
,m_pButtonStampEventDlg(NULL)
,m_pStampEventDlg(NULL)
#endif // PRE_ADD_STAMPSYSTEM
,m_SelectedTabIndex(0)
{
}

CDnIntegrateEventDlg::~CDnIntegrateEventDlg()
{
}

void CDnIntegrateEventDlg::Initialize( bool bShow )
{
	CEtUITabDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "EventTabDlg.ui" ).c_str(), bShow );
}

void CDnIntegrateEventDlg::InitialUpdate()
{
	CEtUITabDialog::InitialUpdate();

	m_pButtonTimeEventDlg = GetControl<CEtUIRadioButton>( "ID_RBT0" );
	m_pButtonTimeEventDlg->SetText(GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 7742));//"타임 이벤트" 7742
	m_pTimeEventDlg = new CDnTimeEventDlg( UI_TYPE_CHILD, this );
	m_pTimeEventDlg->Initialize( false );
	AddTabDialog( m_pButtonTimeEventDlg, m_pTimeEventDlg );

	m_pButtonAttendanceEventDlg = GetControl<CEtUIRadioButton>( "ID_RBT1" );
	m_pButtonAttendanceEventDlg->SetText(GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 7743)); //"출석 이벤트" 7743
	m_pAttendanceEventDlg = new CDnAttendanceEventDlg( UI_TYPE_CHILD, this );
	m_pAttendanceEventDlg->Initialize( false );
	AddTabDialog( m_pButtonAttendanceEventDlg, m_pAttendanceEventDlg );

#ifdef PRE_ADD_STAMPSYSTEM
	m_pButtonStampEventDlg = GetControl<CEtUIRadioButton>( "ID_RBT2" );
	m_pButtonStampEventDlg->SetText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 8316) ); // "일일 스탬프"
	m_pStampEventDlg = new CDnStampEventDlg( UI_TYPE_CHILD, this );
	m_pStampEventDlg->Initialize( false );
	AddTabDialog( m_pButtonStampEventDlg, m_pStampEventDlg );
#endif // PRE_ADD_STAMPSYSTEM


	//나머지 버튼들 안 보이게....
#ifdef PRE_ADD_STAMPSYSTEM
	for (int i = TAB_COUNT; i < 6; ++i)
#else
	for (int i = 2; i < 6; ++i)
#endif // PRE_ADD_STAMPSYSTEM
	{
		char szButtonName[128];
		_snprintf_s(szButtonName, _countof(szButtonName), _TRUNCATE, "ID_RBT%d", i );

		CEtUIRadioButton* pButton = GetControl<CEtUIRadioButton>( szButtonName );
		if (pButton)
			pButton->Show(false);
	}


	//타임 이벤트 설정...
	SetCheckedTab( m_pButtonTimeEventDlg->GetTabID() );
}

void CDnIntegrateEventDlg::Show( bool bShow )
{
	//bShow가 true일때만 호출 하도록 함. 마우스 커서 참조 카운트 엉킬 수 있음.
	if (bShow == true)
	{
#ifdef PRE_ADD_STAMPSYSTEM
		// 만렙이 아닌경우 보이지 않는다.
		bool bMaxLevel = false;
		if( CDnActor::s_hLocalActor )
		{			
			char szButtonName[128];
			_snprintf_s(szButtonName, _countof(szButtonName), _TRUNCATE, "ID_RBT%d", STAMP_EVENT_TAB );
			CEtUIRadioButton* pButton = GetControl<CEtUIRadioButton>( szButtonName );
			if (pButton)
			{
				int nLimitLevel = (int)CGlobalWeightTable::GetInstance().GetValue( CGlobalWeightTable::PlayerLevelLimit );
				if( CDnActor::s_hLocalActor->GetLevel() < nLimitLevel )
					pButton->Show(false);
				else
					pButton->Show(true);
			}
		}
#endif // PRE_ADD_STAMPSYSTEM

		SetCheckedTab(m_SelectedTabIndex);
	}

	CDnMainDlg* pMainDlg = GetInterface().GetMainBarDialog();
	if (pMainDlg)
		pMainDlg->ToggleMenuButton(CDnMainMenuDlg::INTEGRATE_EVENT_DIALOG, bShow);

	if( m_bShow == bShow )
		return;

	CEtUITabDialog::Show( bShow );

	DWORD eventCount = m_pAttendanceEventDlg->GetEventCount();
	m_pButtonAttendanceEventDlg->Enable(eventCount > 0);
}

bool CDnIntegrateEventDlg::IsEventTabShow(DWORD tabIndex)
{
	if (m_bShow == false)
		return false;

	return (tabIndex == GetCurrentTabID());
}

void CDnIntegrateEventDlg::SetEnableEventTab(bool bEnable, DWORD tabIndex)
{
	switch(tabIndex)
	{
	case TIME_EVENT_TAB:
		{
			m_pButtonTimeEventDlg->Enable(bEnable);
		}
		break;
	case ATTENDANCE_EVENT_TAB:
		{
			m_pButtonAttendanceEventDlg->Enable(bEnable);
		}
		break;
	}
}

void CDnIntegrateEventDlg::ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg/* = 0*/ )
{
	SetCmdControlName( pControl->GetControlName() );

	switch(nCommand)
	{
	case EVENT_BUTTON_CLICKED:
		{
			if( IsCmdControl( "ID_BT_CANCEL" ) || IsCmdControl("ID_BT_CLOSE") ) 
			{
				Show( false );
			}
		}
		break;
	}

	CEtUITabDialog::ProcessCommand( nCommand, bTriggeredByUser, pControl, uMsg );
}

void CDnIntegrateEventDlg::RefreshTimeEvent()
{
	if (m_pTimeEventDlg)
		m_pTimeEventDlg->RefreshInfo();
}

bool CDnIntegrateEventDlg::GetActiveEventCount(DWORD& timeEventCount, DWORD& attendanceEventCount )
{
	timeEventCount = attendanceEventCount = 0;

	CDnTimeEventTask *pTask = NULL;
	if (CDnTimeEventTask::IsActive())
		pTask = &CDnTimeEventTask::GetInstance();

	timeEventCount = pTask ? pTask->GetTimeEventCount() : 0;


	if (m_pAttendanceEventDlg)
		m_pAttendanceEventDlg->SetAttendanceEventReward();

	attendanceEventCount = m_pAttendanceEventDlg->GetEventCount();

	//이벤트가 하나라도 있으면 true/ 둘다 없으면 false;
	return (timeEventCount > 0 || attendanceEventCount > 0);
}

#ifdef PRE_ADD_STAMPSYSTEM
bool CDnIntegrateEventDlg::IsShowStampDlg()
{
	if( m_pStampEventDlg )
		return m_pStampEventDlg->IsShow();
	return false;
}
#endif // PRE_ADD_STAMPSYSTEM

#endif // PRE_ADD_INTEGERATE_EVENTUI