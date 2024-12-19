#pragma once

#include "EventPanel.h"


class cwxFadeEventListPanel : public CEventPanel
{
private:
	enum
	{
		LIST_FADE_ID = 2100,
	};


private:
	

public:
	cwxFadeEventListPanel( wxWindow* pParent, wxWindowID id = -1 );
	virtual ~cwxFadeEventListPanel(void);

	void OnSelChangeFadeList( wxListEvent& ListEvent );

	DECLARE_EVENT_TABLE()
};
