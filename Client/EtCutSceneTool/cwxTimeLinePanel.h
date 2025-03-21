#pragma once

#include "ICommandListener.h"

//class cwxTimeLineCtrl;
class wxAuiNotebook;


// 타임 라인 컨트롤을 품고 있는 타임 라인 패널
class cwxTimeLinePanel : public wxPanel,
						 public ICommandListener
{
public:
	enum
	{
		EVENT_TIMELINE_ID = 20600,
		ACTION_TIMELINE_ID,
	};

private:
	wxBoxSizer*				m_pTopSizer;
	wxAuiNotebook*			m_pTimeLineNotebook;

	wxPanel*				m_pEventTimeLinePanel;
	wxPanel*				m_pActionTimeLinePanel;
	//cwxTimeLineCtrl*		m_pEventTimeLine;


public:
	cwxTimeLinePanel( wxWindow* pParent, wxWindowID id = -1 );
	virtual ~cwxTimeLinePanel(void);

	//void OnContextMenu( wxContextMenuEvent& ContextEvent );

	// from ICommandListener
	void CommandPerformed( ICommand* pCommand );
	bool Enable( bool bEnable = true );

	// 현재 포커싱된 타임 라인 컨트롤의 시간을 받아옴.
	DWORD GetNowTime( void );

	void SyncTimePanels( DWORD dwLocalTime );

	void OnMouseEvent( wxMouseEvent& MouseEvent );

	//void Refresh( bool eraseBackground = true, const wxRect *rect = NULL );

	DECLARE_EVENT_TABLE()
};
