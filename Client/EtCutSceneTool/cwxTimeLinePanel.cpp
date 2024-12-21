#include "StdAfx.h"
#include <wx/wx.h>

#include "cwxTimeLinePanel.h"
#include "cwxTimeLineCtrl.h"
#include "cwxEventTimeLinePanel.h"
#include "cwxActionTimeLinePanel.h"
#include <wx/aui/auibook.h>
#include "CommandSet.h"

BEGIN_EVENT_TABLE( cwxTimeLinePanel, wxPanel )
	//EVT_CONTEXT_MENU( cwxTimeLinePanel::OnContextMenu )
END_EVENT_TABLE()



cwxTimeLinePanel::cwxTimeLinePanel( wxWindow* pParent, wxWindowID id ) : wxPanel( pParent, id ),
																		 m_pTopSizer( NULL ),
																		 //m_pEventTimeLine( NULL ),
																		 m_pTimeLineNotebook( NULL ),
																		 m_pEventTimeLinePanel( NULL ),
																		 m_pActionTimeLinePanel( NULL )
{
	//m_pEventTimeLine = new cwxTimeLineCtrl( this, -1 );
	m_pTimeLineNotebook = new wxAuiNotebook( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 
											 wxAUI_NB_TAB_SPLIT | wxAUI_NB_TAB_MOVE | wxAUI_NB_SCROLL_BUTTONS | wxAUI_NB_BOTTOM );
	
	m_pEventTimeLinePanel = new cwxEventTimeLinePanel( this, EVENT_TIMELINE_ID );
	
	m_pActionTimeLinePanel = new cwxActionTimeLinePanel( this, ACTION_TIMELINE_ID );

	m_pTimeLineNotebook->AddPage( m_pEventTimeLinePanel, wxT("Event"), true );
	m_pTimeLineNotebook->AddPage( m_pActionTimeLinePanel, wxT("Action"), false );

	// 사이저 생성
	m_pTopSizer = new wxBoxSizer( wxVERTICAL );
	m_pTopSizer->Add( m_pTimeLineNotebook, 1, wxALL|wxALIGN_CENTER|wxEXPAND, 2 );


	//m_pTopSizer->Add( m_pEventTimeLine, 1, wxALL|wxALIGN_CENTER|wxEXPAND, 2 );

	//wxTextCtrl* pTestWnd1 = new wxTextCtrl( this, wxID_ANY, wxT("테스트 입니다."),
	//										wxPoint(0,0), wxSize(150,90),
	//										wxNO_BORDER | wxTE_MULTILINE);

	//m_pTopSizer->Add( pTestWnd1, 1, wxALL|wxALIGN_CENTER|wxEXPAND, 2 );
	
	this->SetSizer( m_pTopSizer );
	m_pTopSizer->Fit( this );
	m_pTopSizer->SetSizeHints( this );
}

cwxTimeLinePanel::~cwxTimeLinePanel(void)
{

}



bool cwxTimeLinePanel::Enable( bool bEnable )
{
	m_pEventTimeLinePanel->Enable( bEnable );
	m_pActionTimeLinePanel->Enable( bEnable );

	return true;/*wxPanel::Enable( bEnable );*/
}


//
//void cwxTimeLinePanel::OnContextMenu( wxContextMenuEvent& ContextEvent )
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
//	//_PopupContextMenu( Point.x, Point.y );
//}



DWORD cwxTimeLinePanel::GetNowTime( void )
{
	DWORD dwResult = 0;

	int iNowFocusingTimeLine = m_pTimeLineNotebook->GetSelection();
	switch( iNowFocusingTimeLine )
	{
		case 0:
			dwResult = static_cast<cwxEventTimeLinePanel*>(m_pEventTimeLinePanel)->GetNowTime();
			break;

		case 1:
			dwResult = static_cast<cwxActionTimeLinePanel*>(m_pActionTimeLinePanel)->GetNowTime();
			break;
	}

	return dwResult;
}



void cwxTimeLinePanel::SyncTimePanels( DWORD dwLocalTime )
{
	static_cast<cwxActionTimeLinePanel*>(m_pActionTimeLinePanel)->SetTime( dwLocalTime * 0.001f );
	static_cast<cwxEventTimeLinePanel*>(m_pEventTimeLinePanel)->SetTime( dwLocalTime * 0.001f );
	//static_cast<cwxEventTimeLinePanel*>(m_pEventTimeLinePanel)->ReflectData();
}


//
//void cwxTimeLinePanel::Refresh( bool eraseBackground , const wxRect *rect )
//{
//	wxPanel::Refresh( eraseBackground, rect );
//
//	int iNowFocusingTimeLine = m_pTimeLineNotebook->GetSelection();
//	switch( iNowFocusingTimeLine )
//	{
//		case 0:
//			static_cast<cwxEventTimeLinePanel*>(m_pEventTimeLinePanel)->Refresh( eraseBackground, rect );
//			break;
//
//		case 1:
//			static_cast<cwxActionTimeLinePanel*>(m_pActionTimeLinePanel)->Refresh( eraseBackground, rect );
//			break;
//	}
//}


void cwxTimeLinePanel::OnMouseEvent( wxMouseEvent& MouseEvent )
{
	// 첫번째 페이지가 이벤트임.
	if( 0 == m_pTimeLineNotebook->GetSelection() )
		((cwxEventTimeLinePanel*)m_pEventTimeLinePanel)->OnMouseEventFromFrame( MouseEvent );
	else
		((cwxActionTimeLinePanel*)m_pActionTimeLinePanel)->OnMouseEventFromFrame( MouseEvent );
}


void cwxTimeLinePanel::CommandPerformed( ICommand* pCommand )
{
	switch( pCommand->GetTypeID() )
	{
		case CMD_INSERT_ACTION:
		case CMD_INSERT_KEY:
		case CMD_UNREGISTER_RES:
		case CMD_PASTE_FROM_CLIPBOARD:
			static_cast<cwxActionTimeLinePanel*>(m_pActionTimeLinePanel)->ReflectData();
			m_pTimeLineNotebook->SetSelection( 1 );
			break;

		case CMD_REMOVE_ACTION:	
		case CMD_REMOVE_KEY:
		case CMD_ACTOR_PROP_CHANGE:
		case CMD_CHANGE_ACTOR_RES:
			static_cast<cwxActionTimeLinePanel*>(m_pActionTimeLinePanel)->ReflectData();
			break;

		case CMD_ACTION_PROP_CHANGE:
			static_cast<cwxActionTimeLinePanel*>(m_pActionTimeLinePanel)->ModifyObject( pCommand );
			//static_cast<cwxActionTimeLinePanel*>(m_pActionTimeLinePanel)->ReflectData();
			break;

		case CMD_KEY_PROP_CHANGE:
			{
				CKeyPropChange* pKeyPropChange = static_cast<CKeyPropChange*>(pCommand);
				if( pKeyPropChange->m_PrevKeyInfo.iKeyType != pKeyPropChange->m_NewKeyInfo.iKeyType )
					static_cast<cwxActionTimeLinePanel*>(m_pActionTimeLinePanel)->ReflectData();
				else
					static_cast<cwxActionTimeLinePanel*>(m_pActionTimeLinePanel)->ModifyObject( pCommand );
			}
			break;

		case CMD_INSERT_EVENT:
			static_cast<cwxEventTimeLinePanel*>(m_pEventTimeLinePanel)->ReflectData();
			m_pTimeLineNotebook->SetSelection( 0 );
			break;

		case CMD_REMOVE_EVENT:
			static_cast<cwxEventTimeLinePanel*>(m_pEventTimeLinePanel)->ReflectData();
			break;

		case CMD_EVENT_PROP_CHANGE:
			static_cast<cwxEventTimeLinePanel*>(m_pEventTimeLinePanel)->ModifyObject( pCommand );
			//static_cast<cwxEventTimeLinePanel*>(m_pEventTimeLinePanel)->ReflectData();
			break;

		case CMD_BATCH_EDIT:
		case CMD_UPDATE_VIEW:
			static_cast<cwxEventTimeLinePanel*>(m_pEventTimeLinePanel)->ReflectData();
			static_cast<cwxActionTimeLinePanel*>(m_pActionTimeLinePanel)->ReflectData();
			break;
	}
}