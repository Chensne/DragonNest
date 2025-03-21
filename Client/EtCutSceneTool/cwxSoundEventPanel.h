#pragma once

#include "EventPanel.h"



class cwxSoundEventPanel : public CEventPanel
{
private:
	enum
	{
		LIST_CAMERA_ID = 2400,
	};

public:
	cwxSoundEventPanel( wxWindow* pParent, wxWindowID id = -1 );
	virtual ~cwxSoundEventPanel(void);

	void OnSelChangeCameraList( wxListEvent& ListEvent );
	virtual void _UpdateEventList( void );

	DECLARE_EVENT_TABLE()
};
