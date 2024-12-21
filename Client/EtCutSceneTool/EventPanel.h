#pragma once
#include <wx/wx.h>
#include <wx/ListCtrl.h>
#include "ICommandListener.h"


// 이벤트/액션 리스트들의 공통된 점을 모아놓은 클래스.
class CEventPanel : public wxPanel,
					public ICommandListener
{
protected:
	wxBoxSizer*				m_pTopSizer;
	wxListView*				m_pListView;

	int						m_iEventType;

protected:
	void _Initialize( int iListViewID, int iEventType );
	void _InitListViewColumn( void );

private:
	virtual void _UpdateEventList( void );

public:
	CEventPanel( wxWindow* pParent, wxWindowID id = -1 );
	virtual ~CEventPanel(void);

	virtual void CommandPerformed( ICommand* pCommand );
};
