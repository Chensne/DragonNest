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

	// 추가적으로 SOUND_8 번까지 리스트에 넣는다.

	// 사운드 이벤트 정보 긁어옴.
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


// TODO: 리스트에서 사운드 이벤트가 선택되면 Property도 사운드로 바뀌어야 한다. 그리고 마우스로 편집가능한 상태가 된다.
void cwxSoundEventPanel::OnSelChangeCameraList( wxListEvent& ListEvent )
{
	// 셋팅된 곳으로 이벤트 타임 라인에서 선택해 줌.
	int iSelected = m_pListView->GetNextItem( -1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED );
	if( wxNOT_FOUND != iSelected )
	{
		// 리스트 뷰의 인덱스는 데이터 상의 인덱스와 일치한다.
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
			// 이벤트 타임라인에게 셀렉션 시킴
			cwxTLObjectSelectEvent SelectEvent( wxEVT_TIMELINE_OBJECT_SELECT, -1, wxString(wxT("List Select")),
												 pSelectedEventInfo->iID, pSelectedEventInfo->iType+3, 0 );	// 툴에서 쓰는 이벤트 타입이 사운드보다 위에 3개가 더 있음.
			cwxEventTimeLinePanel* pEventTimeLinePanel = TOOL_DATA.GetEventTLPanel();
			pEventTimeLinePanel->OnTLObjectSelect( SelectEvent );		// 속성창 등등 자동으로 바뀜
			pEventTimeLinePanel->SelectObject( pSelectedEventInfo->iID, DWORD(pSelectedEventInfo->fStartTime*10.0f) );
		}
	}
}
