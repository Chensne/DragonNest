#pragma once

#include "EventPanel.h"


// 카메라 이벤트를 리스트로 선택할 수 있는 패널
class cwxCamEventListPanel : public CEventPanel
{
private:
	enum
	{
		LIST_CAMERA_ID = 2000,
	};


private:


public:
	cwxCamEventListPanel( wxWindow* pParent, wxWindowID id = -1 );
	virtual ~cwxCamEventListPanel(void);

	void OnSelChangeCameraList( wxListEvent& ListEvent );

	DECLARE_EVENT_TABLE()
};
