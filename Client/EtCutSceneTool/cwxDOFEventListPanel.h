#pragma once
#include "EventPanel.h"



class cwxDOFEventListPanel : public CEventPanel
{
private:
	enum
	{
		LIST_DOF_ID = 2200,
	};

public:
	cwxDOFEventListPanel( wxWindow* pParent, wxWindowID id = -1 );
	virtual ~cwxDOFEventListPanel(void);

	void OnSelChangeDOFList( wxListEvent& ListEvent );

	DECLARE_EVENT_TABLE()
};
