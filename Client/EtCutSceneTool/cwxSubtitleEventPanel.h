#pragma once
#include "eventpanel.h"

class cwxSubtitleEventPanel : public CEventPanel
{
private:
	enum
	{
		LIST_SUBTITLE_ID = 2500,
	};

public:
	cwxSubtitleEventPanel( wxWindow* pParent, wxWindowID id = -1 );
	virtual ~cwxSubtitleEventPanel(void);

	void OnSelChangeSubtitleList( wxListEvent& ListEvent );

	DECLARE_EVENT_TABLE()
};
