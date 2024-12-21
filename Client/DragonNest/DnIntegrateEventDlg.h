#if defined(PRE_ADD_INTEGERATE_EVENTUI)
#pragma once
#include "DnCustomDlg.h"
#include "EtUITabDialog.h"

//통합Event UI
class CDnAttendanceEventDlg;
class CDnTimeEventDlg;

#ifdef PRE_ADD_STAMPSYSTEM
class CDnStampEventDlg;
#endif // PRE_ADD_STAMPSYSTEM

class CDnIntegrateEventDlg : public CEtUITabDialog
{
public:
	enum WINDOW_TAB
	{
		TIME_EVENT_TAB = 0,
		ATTENDANCE_EVENT_TAB = 1,
#ifdef PRE_ADD_STAMPSYSTEM
		STAMP_EVENT_TAB = 2,
#endif // PRE_ADD_STAMPSYSTEM
		TAB_COUNT
	};

	CDnIntegrateEventDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL );
	virtual ~CDnIntegrateEventDlg();

	virtual void Initialize( bool bShow );
	virtual void InitialUpdate();
	virtual void Show( bool bShow );
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0 );

	void SelectTab(DWORD tabIndex) { m_SelectedTabIndex = tabIndex; }
	void SetEnableEventTab(bool bEnable, DWORD tabIndex);

	CDnTimeEventDlg* GetTimeEventDlg() { return m_pTimeEventDlg; }
	CDnAttendanceEventDlg* GetAttendanceEventDlg() { return m_pAttendanceEventDlg; }

	//tabIndex 탭이 선택되어 져 있는지 확인..
	//다이얼로그 자체가 일단 보여지고 있는 상태(m_bShow == true)여야 한다.
	bool IsEventTabShow(DWORD tabIndex);


	void RefreshTimeEvent();

	bool GetActiveEventCount(DWORD& timeEventCount, DWORD& attendanceEventCount );

#ifdef PRE_ADD_STAMPSYSTEM
	bool IsShowStampDlg();
#endif // PRE_ADD_STAMPSYSTEM

#ifdef PRE_ADD_STAMPSYSTEM	
	CDnStampEventDlg * GetStampEventDlg(){
		return m_pStampEventDlg;
	}
#endif // PRE_ADD_STAMPSYSTEM


protected:
	CEtUIRadioButton*	m_pButtonTimeEventDlg;			// 타임이벤트
	CEtUIRadioButton*	m_pButtonAttendanceEventDlg;	// 출석이벤트
	
	CDnTimeEventDlg* m_pTimeEventDlg;
	CDnAttendanceEventDlg* m_pAttendanceEventDlg;

#ifdef PRE_ADD_STAMPSYSTEM
	CEtUIRadioButton * m_pButtonStampEventDlg;
	CDnStampEventDlg * m_pStampEventDlg;
#endif // PRE_ADD_STAMPSYSTEM

	DWORD m_SelectedTabIndex;
};
#endif // PRE_ADD_INTEGERATE_EVENTUI

