#include "stdafx.h"
#include "cwxCamEventListPanel.h"
#include "ToolData.h"
#include "RTCutSceneMakerFrame.h"
#include "cwxPropertyPanel.h"
#include "cwxEventTimeLinePanel.h"


BEGIN_EVENT_TABLE( cwxCamEventListPanel, wxPanel )
	EVT_LIST_ITEM_SELECTED( LIST_CAMERA_ID, cwxCamEventListPanel::OnSelChangeCameraList )
END_EVENT_TABLE()


cwxCamEventListPanel::cwxCamEventListPanel( wxWindow* pParent, wxWindowID id /* = -1  */ ) : CEventPanel( pParent, id )
{
	CEventPanel::_Initialize( LIST_CAMERA_ID, EventInfo::CAMERA );
}

cwxCamEventListPanel::~cwxCamEventListPanel(void)
{

}





// TODO: 리스트에서 카메라가 선택되면 Property도 카메라로 바뀌어야 한다. 그리고 마우스로 편집가능한 상태가 된다.
void cwxCamEventListPanel::OnSelChangeCameraList( wxListEvent& ListEvent )
{
	// 셋팅된 곳으로 이벤트 타임 라인에서 선택해 줌.
	int iSelected = m_pListView->GetNextItem( -1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED );
	if( wxNOT_FOUND != iSelected )
	{
		// 리스트 뷰의 인덱스는 데이터 상의 인덱스와 일치한다.
		const EventInfo* pSelectedEventInfo = TOOL_DATA.GetEventInfoByIndex( EventInfo::CAMERA, iSelected );

		if( pSelectedEventInfo )
		{
			// 이벤트 타임라인에게 셀렉션 시킴
			cwxTLObjectSelectEvent SelectEvent( wxEVT_TIMELINE_OBJECT_SELECT, -1, wxString(wxT("List Select")),
												 pSelectedEventInfo->iID, CToolData::MAX_CAMERA, 0 );
			cwxEventTimeLinePanel* pEventTimeLinePanel = TOOL_DATA.GetEventTLPanel();
			pEventTimeLinePanel->OnTLObjectSelect( SelectEvent );		// 속성창 등등 자동으로 바뀜
			pEventTimeLinePanel->SelectObject( pSelectedEventInfo->iID, DWORD(pSelectedEventInfo->fStartTime*10.0f) );
		}
	}
}
