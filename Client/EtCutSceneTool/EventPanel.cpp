#include "StdAfx.h"
#include "EventPanel.h"
#include "RTCutSceneMakerFrame.h"
#include "ToolData.h"
#include "ToolData.h"
#include "CommandSet.h"


CEventPanel::CEventPanel( wxWindow* pParent, wxWindowID id/* = -1*/ ) : wxPanel( pParent, id ),	
																		m_pListView( NULL ),
																		m_iEventType( EventInfo::TYPE_COUNT )
{

}

CEventPanel::~CEventPanel(void)
{

}


void CEventPanel::_Initialize( int iListViewID, int iEventType )
{
	m_iEventType = iEventType;

	m_pTopSizer =  new wxBoxSizer( wxVERTICAL );

	m_pListView = new wxListView( this, iListViewID, wxDefaultPosition, wxDefaultSize, 
								  wxLC_REPORT|wxLC_SINGLE_SEL );
	_InitListViewColumn();
	
	m_pTopSizer->Add( m_pListView, 1, wxALL|wxALIGN_CENTER|wxEXPAND, 2 );

	SetSizer( m_pTopSizer );
	m_pTopSizer->Fit( this );
	m_pTopSizer->SetSizeHints( this );

	// 휠 이벤트가 트리에게 가지 않도록 한다.
	CRTCutSceneMakerFrame* pMainFrame = static_cast<CRTCutSceneMakerFrame*>(TOOL_DATA.GetMainFrame());
	pMainFrame->ConnectMouseWheelEventToRenderPanel( m_pListView );
}


void CEventPanel::_InitListViewColumn( void )
{
	m_pListView->InsertColumn( 0, wxT("Name"), wxLIST_FORMAT_CENTRE, 180 );
	m_pListView->InsertColumn( 1, wxT("Start Time"), wxLIST_FORMAT_CENTRE, 80 );
	m_pListView->InsertColumn( 2, wxT("ID"), wxLIST_FORMAT_CENTRE, 40 );
}


void CEventPanel::_UpdateEventList( void )
{
	m_pListView->ClearAll();
	_InitListViewColumn();

	// 페이드 정보 긁어옴.
	int iNumEventInfo = TOOL_DATA.GetThisTypesEventNum( m_iEventType );
	for( int iEvent = 0; iEvent < iNumEventInfo; ++iEvent )
	{
		const EventInfo* pEventInfo = TOOL_DATA.GetEventInfoByIndex( m_iEventType, iEvent );

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


void CEventPanel::CommandPerformed( ICommand* pCommand )
{
	switch( pCommand->GetTypeID() )
	{
		case CMD_INSERT_EVENT:
		case CMD_REMOVE_EVENT:
		case CMD_EVENT_PROP_CHANGE:
		case CMD_UPDATE_VIEW:
		case CMD_BATCH_EDIT:
			_UpdateEventList();
			break;
	}
}