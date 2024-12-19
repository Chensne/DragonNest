#include "stdafx.h"
#include <wx/wx.h>
#include "cwxParticleListPanel.h"
#include "ToolData.h"
#include "CommandSet.h"
#include "cwxPropertyPanel.h"
#include "RTCutSceneMakerFrame.h"
#include "RTCutSceneRenderer.h"


BEGIN_EVENT_TABLE( cwxParticleListPanel, wxPanel )
	//EVT_CONTEXT_MENU( cwxParticleListPanel::OnContextMenu )
	EVT_LISTBOX( LIST_PARTICLE, cwxParticleListPanel::OnSelChangeParticleList )
END_EVENT_TABLE()




cwxParticleListPanel::cwxParticleListPanel( wxWindow* pParent, int id ) : wxPanel( pParent, id /*= -1*/ )
{
	m_pTopSizer = new wxBoxSizer( wxVERTICAL );

	m_pParticleEventList = new wxListBox( this, LIST_PARTICLE );
	m_pTopSizer->Add( m_pParticleEventList, 1, wxALL|wxALIGN_CENTER|wxEXPAND, 2 );

	SetSizer( m_pTopSizer );
	m_pTopSizer->Fit( this );
	m_pTopSizer->SetSizeHints( this );

	// 휠 이벤트가 트리거에게 가지 않도록 한다.
	CRTCutSceneMakerFrame* pMainFrame = static_cast<CRTCutSceneMakerFrame*>(TOOL_DATA.GetMainFrame());
	pMainFrame->ConnectMouseWheelEventToRenderPanel( m_pParticleEventList );
}

cwxParticleListPanel::~cwxParticleListPanel(void)
{
}



//void cwxParticleListPanel::_PopupContextMenu( int iXPos, int iYPos )
//{
//	long iSelected = m_pParticleEventList->GetNextItem( -1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED );
//	wxMenu Menu;
//	//wxTreeItemId SelectedID = m_pTreeKeySet->GetSelection();
//	//if( SelectedID.IsOk() )
//	//{
//	//	Menu.AppendSeparator();
//	//}
//
//	Menu.Append( POPUP_SELECT, wxT("&Select") );
//	//Menu.Append( POPUP_SEE_THIS, wxT("S&ee This") );
//
//	PopupMenu( &Menu, iXPos, iYPos );
//}


void cwxParticleListPanel::_UpdateParticleList( void )
{
	m_pParticleEventList->Hide();
	m_pParticleEventList->Clear();

	wxCSConv MBConv( wxFONTENCODING_CP949 );
	wxChar caWBuf[ 256 ];

	int iNumParticleEvent = TOOL_DATA.GetThisTypesEventNum( EventInfo::PARTICLE );
	for( int i = 0; i < iNumParticleEvent; ++i )
	{
		const EventInfo* pEventInfo = TOOL_DATA.GetEventInfoByIndex( EventInfo::PARTICLE, i );

		MBConv.MB2WC( caWBuf, pEventInfo->strEventName.c_str(), 256 );
		m_pParticleEventList->Insert( caWBuf, i );
	}

	m_pParticleEventList->Show();
}

void cwxParticleListPanel::OnSelChangeParticleList( wxCommandEvent& ListEvent )
{
	int iSelection = ListEvent.GetSelection();

	const EventInfo* pEventInfo = TOOL_DATA.GetEventInfoByIndex( EventInfo::PARTICLE, iSelection );
	TOOL_DATA.SetSelectedObjectID( pEventInfo->iID );

	cwxPropertyPanel* pPropertyPanel = TOOL_DATA.GetPropertyPanel();
	pPropertyPanel->ShowThisProp( cwxPropertyPanel::PARTICLE_EVENT_PROP_ID );

	CRTCutSceneRenderer* pRenderer = TOOL_DATA.GetRenderer();
	pRenderer->OnSelectEventUnit( CToolData::PARTICLE, pEventInfo->iID );
}


//void cwxParticleListPanel::OnContextMenu( wxContextMenuEvent& ContextMenu )
//{
//	wxPoint Point = ContextEvent.GetPosition();
//
//	if( -1 == Point.x && -1 == Point.y )
//	{
//		wxSize Size = GetSize();
//		Point.x = Size.x / 2;
//		Point.y = Size.y / 2;
//	}
//	else
//	{
//		Point = ScreenToClient( Point );
//	}
//
//	_PopupContextMenu( Point.x, Point.y );
//}
//
//
//void cwxParticleListPanel::OnMenuSeeThis( wxCommandEvent& MenuEvent )
//{
//	CRTCutSceneRenderer* pRenderer = TOOL_DATA.GetRenderer();
//
//	long iSelected = m_pParticleEventList->GetNextItem( -1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED );
//	if( wxNOT_FOUND != iSelected )
//	{
//		wxString strSelectedName;
//		TOOL_DATA.GetRegResNameByIndex( iSelected, strSelectedName );
//		pRenderer->SeeThisActor();
//	}
//}
//
//
//void cwxParticleListPanel::OnMenuSelect( wxCommandEvent& MenuEvent )
//{
//
//}


void cwxParticleListPanel::CommandPerformed( ICommand* pCommand )
{
	switch( pCommand->GetTypeID() )
	{
		case CMD_INSERT_EVENT:
		case CMD_REMOVE_EVENT:
		case CMD_UPDATE_VIEW:
			_UpdateParticleList();
			break;
	}
}