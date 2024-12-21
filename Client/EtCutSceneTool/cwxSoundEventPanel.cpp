#include "StdAfx.h"
#include "cwxSoundEventPanel.h"
#include "ToolData.h"
#include "RTCutSceneMakerFrame.h"
#include "cwxPropertyPanel.h"
#include "cwxEventTimeLinePanel.h"



BEGIN_EVENT_TABLE( cwxSoundEventPanel, wxPanel )
	EVT_LIST_ITEM_SELECTED( LIST_CAMERA_ID, cwxSoundEventPanel::OnSelChangeCameraList )
END_EVENT_TABLE()


cwxSoundEventPanel::cwxSoundEventPanel( wxWindow* pParent, wxWindowID id /* = -1  */ ) : CEventPanel( pParent, id )
{
	CEventPanel::_Initialize( LIST_CAMERA_ID, EventInfo::SOUND_1 );	
}

cwxSoundEventPanel::~cwxSoundEventPanel(void)
{

}


void cwxSoundEventPanel::_UpdateEventList( void )
{
	m_pListView->ClearAll();
	_InitListViewColumn();

	// �߰������� SOUND_8 ������ ����Ʈ�� �ִ´�.

	// ���� �̺�Ʈ ���� �ܾ��.
	for( int iEventType = EventInfo::SOUND_1; iEventType <= EventInfo::SOUND_8; ++iEventType )
	{
		int iNumEventInfo = TOOL_DATA.GetThisTypesEventNum( iEventType );
		for( int iEvent = 0; iEvent < iNumEventInfo; ++iEvent )
		{
			const EventInfo* pEventInfo = TOOL_DATA.GetEventInfoByIndex( iEventType, iEvent );

			wxCSConv MBConv( wxFONTENCODING_CP949 );

			wxChar caWBuf[ 256 ];
			ZeroMemory( caWBuf, sizeof(caWBuf) );
			MBConv.MB2WC( caWBuf, pEventInfo->strEventName.c_str(), 256 );

			long iTemp = m_pListView->InsertItem( iEvent, caWBuf );
			m_pListView->SetItemData( iTemp, iEvent );

			wxSnprintf( caWBuf, 256, wxT("%dms"), DWORD(pEventInfo->fStartTime*10.0f) );
			m_pListView->SetItem( iEvent, 1, caWBuf );

			wxSnprintf( caWBuf, 256, wxT("%d"), pEventInfo->iID );
			m_pListView->SetItem( iEvent, 2, caWBuf );
		}
	}
}


// TODO: ����Ʈ���� ���� �̺�Ʈ�� ���õǸ� Property�� ����� �ٲ��� �Ѵ�. �׸��� ���콺�� ���������� ���°� �ȴ�.
void cwxSoundEventPanel::OnSelChangeCameraList( wxListEvent& ListEvent )
{
	// ���õ� ������ �̺�Ʈ Ÿ�� ���ο��� ������ ��.
	int iSelected = m_pListView->GetNextItem( -1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED );
	if( wxNOT_FOUND != iSelected )
	{
		// ����Ʈ ���� �ε����� ������ ���� �ε����� ��ġ�Ѵ�.
		wxListItem Item;
		Item.SetId( iSelected );
		Item.SetColumn( 2 );
		Item.SetMask( wxLIST_MASK_TEXT );
		m_pListView->GetItem( Item );
		//const EventInfo* pSelectedEventInfo = TOOL_DATA.GetEventInfoByIndex( EventInfo::SOUND_1, iSelected );
		long ID = 0;
		Item.GetText().ToLong( &ID );
		const EventInfo* pSelectedEventInfo = TOOL_DATA.GetEventInfoByID( ID );

		if( pSelectedEventInfo )
		{
			// �̺�Ʈ Ÿ�Ӷ��ο��� ������ ��Ŵ
			cwxTLObjectSelectEvent SelectEvent( wxEVT_TIMELINE_OBJECT_SELECT, -1, wxString(wxT("List Select")),
												 pSelectedEventInfo->iID, pSelectedEventInfo->iType+3, 0 );	// ������ ���� �̺�Ʈ Ÿ���� ���庸�� ���� 3���� �� ����.
			cwxEventTimeLinePanel* pEventTimeLinePanel = TOOL_DATA.GetEventTLPanel();
			pEventTimeLinePanel->OnTLObjectSelect( SelectEvent );		// �Ӽ�â ��� �ڵ����� �ٲ�
			pEventTimeLinePanel->SelectObject( pSelectedEventInfo->iID, DWORD(pSelectedEventInfo->fStartTime*10.0f) );
		}
	}
}
