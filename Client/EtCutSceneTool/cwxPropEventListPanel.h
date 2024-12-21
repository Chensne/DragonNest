#pragma once
#include "eventpanel.h"

class cwxPropEventListPanel : public CEventPanel
{
private:
	enum
	{
		LIST_PROP_ID = 2300,
	};

public:
	cwxPropEventListPanel( wxWindow* pParent, wxWindowID id = -1 );
	virtual ~cwxPropEventListPanel(void);

	void OnSelChangePropList( wxListEvent& ListEvent );

	DECLARE_EVENT_TABLE()
};
