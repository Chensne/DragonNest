#include "StdAfx.h"
#include "cwxSubtitleEventPanel.h"
#include "ToolData.h"
#include "RTCutSceneRenderer.h"
#include "cwxPropertyPanel.h"
#include "cwxEventTimeLinePanel.h"


BEGIN_EVENT_TABLE( cwxSubtitleEventPanel, wxPanel )
	EVT_LIST_ITEM_SELECTED( LIST_SUBTITLE_ID, cwxSubtitleEventPanel::OnSelChangeSubtitleList )
END_EVENT_TABLE()


cwxSubtitleEventPanel::cwxSubtitleEventPanel( wxWindow* pParent, wxWindowID id/* = -1 */) : CEventPanel( pParent, id )
{
	CEventPanel::_Initialize( LIST_SUBTITLE_ID, EventInfo::SUBTITLE );
}

cwxSubtitleEventPanel::~cwxSubtitleEventPanel(void)
{
}


void cwxSubtitleEventPanel::OnSelChangeSubtitleList( wxListEvent& ListEvent )
{
	// ���õ� ������ �̺�Ʈ Ÿ�� ���ο��� ������ ��.
	int iSelected = m_pListView->GetNextItem( -1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED );
	if( wxNOT_FOUND != iSelected )
	{
		// ����Ʈ ���� �ε����� ������ ���� �ε����� ��ġ�Ѵ�.
		const EventInfo* pSelectedEventInfo = TOOL_DATA.GetEventInfoByIndex( EventInfo::SUBTITLE, iSelected );

		if( pSelectedEventInfo )
		{
			// �̺�Ʈ Ÿ�Ӷ��ο��� ������ ��Ŵ
			cwxTLObjectSelectEvent SelectEvent( wxEVT_TIMELINE_OBJECT_SELECT, -1, wxString(wxT("List Select")),
				pSelectedEventInfo->iID, CToolData::SUBTITLE, 0 );
			cwxEventTimeLinePanel* pEventTimeLinePanel = TOOL_DATA.GetEventTLPanel();
			pEventTimeLinePanel->OnTLObjectSelect( SelectEvent );		// �Ӽ�â ��� �ڵ����� �ٲ�
			pEventTimeLinePanel->SelectObject( pSelectedEventInfo->iID, DWORD(pSelectedEventInfo->fStartTime*10.0f) );
		}
	}
}
