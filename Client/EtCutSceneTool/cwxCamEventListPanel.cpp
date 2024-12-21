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





// TODO: ����Ʈ���� ī�޶� ���õǸ� Property�� ī�޶�� �ٲ��� �Ѵ�. �׸��� ���콺�� ���������� ���°� �ȴ�.
void cwxCamEventListPanel::OnSelChangeCameraList( wxListEvent& ListEvent )
{
	// ���õ� ������ �̺�Ʈ Ÿ�� ���ο��� ������ ��.
	int iSelected = m_pListView->GetNextItem( -1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED );
	if( wxNOT_FOUND != iSelected )
	{
		// ����Ʈ ���� �ε����� ������ ���� �ε����� ��ġ�Ѵ�.
		const EventInfo* pSelectedEventInfo = TOOL_DATA.GetEventInfoByIndex( EventInfo::CAMERA, iSelected );

		if( pSelectedEventInfo )
		{
			// �̺�Ʈ Ÿ�Ӷ��ο��� ������ ��Ŵ
			cwxTLObjectSelectEvent SelectEvent( wxEVT_TIMELINE_OBJECT_SELECT, -1, wxString(wxT("List Select")),
												 pSelectedEventInfo->iID, CToolData::MAX_CAMERA, 0 );
			cwxEventTimeLinePanel* pEventTimeLinePanel = TOOL_DATA.GetEventTLPanel();
			pEventTimeLinePanel->OnTLObjectSelect( SelectEvent );		// �Ӽ�â ��� �ڵ����� �ٲ�
			pEventTimeLinePanel->SelectObject( pSelectedEventInfo->iID, DWORD(pSelectedEventInfo->fStartTime*10.0f) );
		}
	}
}
