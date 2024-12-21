#pragma once

#include "ICommandListener.h"

// �����丮�� �����ִ� �����丮 �г��Դϴ�
class cwxHistoryPanel : public wxPanel,
						public ICommandListener
		
{
private:
	enum
	{
		LIST_HISTORY_ID = 20700,
	};

	wxBoxSizer*				m_pTopSizer;
	wxListBox*				m_pHistoryListBox;

private:
	void _UpdateHistorySelection( void );

public:
	cwxHistoryPanel( wxWindow* pParent, wxWindowID id = -1 );
	virtual ~cwxHistoryPanel(void);

	void OnSelChangeHistory( wxCommandEvent& ListEvent );

	// from ICommandListener
	void CommandPerformed( ICommand* pCommand );

	DECLARE_EVENT_TABLE()
};
