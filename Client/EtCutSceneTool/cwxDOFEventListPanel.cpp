#include "StdAfx.h"
#include "cwxDOFEventListPanel.h"
#include "ToolData.h"
#include "RTCutSceneRenderer.h"
#include "cwxPropertyPanel.h"
#include "cwxEventTimeLinePanel.h"


BEGIN_EVENT_TABLE( cwxDOFEventListPanel, wxPanel )
	EVT_LIST_ITEM_SELECTED( LIST_DOF_ID, cwxDOFEventListPanel::OnSelChangeDOFList )
END_EVENT_TABLE()


cwxDOFEventListPanel::cwxDOFEventListPanel( wxWindow* pParent, wxWindowID id/* = -1 */ ) : CEventPanel( pParent, id )
{
	CEventPanel::_Initialize( LIST_DOF_ID, EventInfo::DOF );
}

cwxDOFEventListPanel::~cwxDOFEventListPanel(void)
{
}


void cwxDOFEventListPanel::OnSelChangeDOFList( wxListEvent& ListEvent )
{
	// ���õ� ������ �̺�Ʈ Ÿ�� ���ο��� ������ ��.
	int iSelected = m_pListView->GetNextItem( -1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED );
	if( wxNOT_FOUND != iSelected )
	{
		// ����Ʈ ���� �ε����� ������ ���� �ε����� ��ġ�Ѵ�.
		const EventInfo* pSelectedEventInfo = TOOL_DATA.GetEventInfoByIndex( EventInfo::DOF, iSelected );

		if( pSelectedEventInfo )
		{
			// �̺�Ʈ Ÿ�Ӷ��ο��� ������ ��Ŵ
			cwxTLObjectSelectEvent SelectEvent( wxEVT_TIMELINE_OBJECT_SELECT, -1, wxString(wxT("List Select")),
				pSelectedEventInfo->iID, CToolData::DOF, 0 );
			cwxEventTimeLinePanel* pEventTimeLinePanel = TOOL_DATA.GetEventTLPanel();
			pEventTimeLinePanel->OnTLObjectSelect( SelectEvent );		// �Ӽ�â ��� �ڵ����� �ٲ�
			pEventTimeLinePanel->SelectObject( pSelectedEventInfo->iID, DWORD(pSelectedEventInfo->fStartTime*10.0f) );
		}
	}
}
