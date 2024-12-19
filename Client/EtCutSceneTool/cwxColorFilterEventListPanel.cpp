#include "StdAfx.h"
#include "cwxColorFilterEventListPanel.h"
#include "ToolData.h"
#include "RTCutSceneRenderer.h"
#include "cwxPropertyPanel.h"
#include "cwxEventTimeLinePanel.h"

// PRE_ADD_FILTEREVENT
BEGIN_EVENT_TABLE( cwxColorFilterEventListPanel, wxPanel )
	EVT_LIST_ITEM_SELECTED( LIST_PROP_ID, cwxColorFilterEventListPanel::OnSelChangePropList )
END_EVENT_TABLE()


cwxColorFilterEventListPanel::cwxColorFilterEventListPanel( wxWindow* pParent, wxWindowID id/* = -1 */) : CEventPanel( pParent, id )
{
	CEventPanel::_Initialize( LIST_PROP_ID, EventInfo::COLORFILTER );
}

cwxColorFilterEventListPanel::~cwxColorFilterEventListPanel(void)
{
}


void cwxColorFilterEventListPanel::OnSelChangePropList( wxListEvent& ListEvent )
{
	// ���õ� ������ �̺�Ʈ Ÿ�� ���ο��� ������ ��.
	int iSelected = m_pListView->GetNextItem( -1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED );
	if( wxNOT_FOUND != iSelected )
	{
		// ����Ʈ ���� �ε����� ������ ���� �ε����� ��ġ�Ѵ�.
		const EventInfo* pSelectedEventInfo = TOOL_DATA.GetEventInfoByIndex( EventInfo::COLORFILTER, iSelected );

		if( pSelectedEventInfo )
		{
			// �̺�Ʈ Ÿ�Ӷ��ο��� ������ ��Ŵ
			cwxTLObjectSelectEvent SelectEvent( wxEVT_TIMELINE_OBJECT_SELECT, -1, wxString(wxT("List Select")),
				pSelectedEventInfo->iID, CToolData::COLORFILTER, 0 );
			cwxEventTimeLinePanel* pEventTimeLinePanel = TOOL_DATA.GetEventTLPanel();
			pEventTimeLinePanel->OnTLObjectSelect( SelectEvent );		// �Ӽ�â ��� �ڵ����� �ٲ�
			pEventTimeLinePanel->SelectObject( pSelectedEventInfo->iID, DWORD(pSelectedEventInfo->fStartTime*10.0f) );
		}
	}
}

//
//
//
//
//
//void cwxColorFilterEventListPanel::_UpdateColorFilterList( void )
//{
//	m_pParticleEventList->Hide();
//	m_pParticleEventList->Clear();
//
//	wxCSConv MBConv( wxFONTENCODING_CP949 );
//	wxChar caWBuf[ 256 ];
//
//	int iNumParticleEvent = TOOL_DATA.GetThisTypesEventNum( EventInfo::PARTICLE );
//	for( int i = 0; i < iNumParticleEvent; ++i )
//	{
//		const EventInfo* pEventInfo = TOOL_DATA.GetEventInfoByIndex( EventInfo::PARTICLE, i );
//
//		MBConv.MB2WC( caWBuf, pEventInfo->strEventName.c_str(), 256 );
//		m_pParticleEventList->Insert( caWBuf, i );
//	}
//
//	m_pParticleEventList->Show();
//}
//
//void cwxColorFilterEventListPanel::OnSelChangeParticleList( wxCommandEvent& ListEvent )
//{
//	int iSelection = ListEvent.GetSelection();
//
//	const EventInfo* pEventInfo = TOOL_DATA.GetEventInfoByIndex( EventInfo::PARTICLE, iSelection );
//	TOOL_DATA.SetSelectedObjectID( pEventInfo->iID );
//
//	cwxPropertyPanel* pPropertyPanel = TOOL_DATA.GetPropertyPanel();
//	pPropertyPanel->ShowThisProp( cwxPropertyPanel::PARTICLE_EVENT_PROP_ID );
//
//	CRTCutSceneRenderer* pRenderer = TOOL_DATA.GetRenderer();
//	pRenderer->OnSelectEventUnit( CToolData::PARTICLE, pEventInfo->iID );
//}
//
//void cwxColorFilterEventListPanel::CommandPerformed( ICommand* pCommand )
//{
//	switch( pCommand->GetTypeID() )
//	{
//	case CMD_INSERT_EVENT:
//	case CMD_REMOVE_EVENT:
//	case CMD_UPDATE_VIEW:
//		_UpdateColorFilterList();
//		break;
//	}
//}