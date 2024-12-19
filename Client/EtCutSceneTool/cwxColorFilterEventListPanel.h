#pragma once
#include "eventpanel.h"

// PRE_ADD_FILTEREVENT
class cwxColorFilterEventListPanel : public CEventPanel
{
private:
	enum
	{
		LIST_PROP_ID = 2600,
	};

public:
	cwxColorFilterEventListPanel( wxWindow* pParent, wxWindowID id = -1 );
	virtual ~cwxColorFilterEventListPanel(void);

	void OnSelChangePropList( wxListEvent& ListEvent );

	DECLARE_EVENT_TABLE()

	//void _UpdateColorFilterList();
};
