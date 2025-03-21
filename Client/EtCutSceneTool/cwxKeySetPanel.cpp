#include "stdafx.h"
#include <wx/wx.h>
#include "cwxKeySetPanel.h"
#include "ToolData.h"
#include "RTCutSceneMakerFrame.h"


BEGIN_EVENT_TABLE( cwxKeySetPanel, wxPanel )
	EVT_CONTEXT_MENU( cwxKeySetPanel::OnContextMenu )
	EVT_MENU( POPUP_ADD_KEYSET, cwxKeySetPanel::OnAddKeySet )
	EVT_MENU( POPUP_DEL_KEYSET, cwxKeySetPanel::OnDelKeySet )
END_EVENT_TABLE()


cwxKeySetPanel::cwxKeySetPanel( wxWindow* pParent, wxWindowID id /* = -1  */ ) : wxPanel( pParent, id )
{
	m_pTopSizer =  new wxBoxSizer( wxVERTICAL );

	m_pTreeKeySet = new wxTreeCtrl( this, TREE_KEY_SET_ID, wxDefaultPosition, wxDefaultSize );
	m_pTopSizer->Add( m_pTreeKeySet, 1, wxALL|wxALIGN_CENTER|wxEXPAND, 2 );

	m_TreeRootID = m_pTreeKeySet->AddRoot( wxT("Set") );
	
	SetSizer( m_pTopSizer );
	m_pTopSizer->Fit( this );
	m_pTopSizer->SetSizeHints( this );

	// 휠 이벤트가 트리에게 가지 않도록 한다.
	CRTCutSceneMakerFrame* pMainFrame = static_cast<CRTCutSceneMakerFrame*>(TOOL_DATA.GetMainFrame());
	pMainFrame->ConnectMouseWheelEventToRenderPanel( m_pTreeKeySet );

	m_pTreeKeySet->Expand( m_TreeRootID );
}

cwxKeySetPanel::~cwxKeySetPanel(void)
{

}



void cwxKeySetPanel::_PopupContextMenu( int iXPos, int iYPos )
{
	//long iSelected = m_pListRegisteredRes->GetNextItem( -1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED );
	wxMenu Menu;
	wxTreeItemId SelectedID = m_pTreeKeySet->GetSelection();
	if( SelectedID.IsOk() )
	{
		// 중요: 리스트 뷰에서의 순서와 실제 추가된 순서가 일치한다.
		//int iResourceKind = TOOL_DATA.GetRegResKindByIndex( iSelected );

		//switch( iResourceKind )
		//{
		//case CDnCutSceneData::RT_RES_MAP:
		//	break;

		//case CDnCutSceneData::RT_RES_ACTOR:
		//	Menu.Append( POPUP_INSERT_ACTION, wxT("Insert &Action") );
		//	Menu.Append( POPUP_INSERT_KEY, wxT("Insert &Key") );
		//	break;

		//case CDnCutSceneData::RT_RES_SOUND:
		//	Menu.Append( POPUP_INSERT_EVENT, wxT("Insert &Event") );
		//	break;
		//}
		Menu.AppendSeparator();
	}

	Menu.Append( POPUP_ADD_KEYSET, wxT("&Add Key Set") );
	Menu.Append( POPUP_DEL_KEYSET, wxT("&Del Key Set") );

	PopupMenu( &Menu, iXPos, iYPos );
}


void cwxKeySetPanel::OnAddKeySet( wxCommandEvent& CommandEvent )
{
	// 빈 키 세트 추가
	
}


void cwxKeySetPanel::OnDelKeySet( wxCommandEvent& CommandEvent )
{
	// 비어있지 않으면 진짜로 삭제할 거냐고 물어본다?

}


void cwxKeySetPanel::OnContextMenu( wxContextMenuEvent& ContextEvent )
{
	wxPoint Point = ContextEvent.GetPosition();

	if( -1 == Point.x && -1 == Point.y )
	{
		wxSize Size = GetSize();
		Point.x = Size.x / 2;
		Point.y = Size.y / 2;
	}
	else
	{
		Point = ScreenToClient( Point );
	}

	_PopupContextMenu( Point.x, Point.y );
}



void cwxKeySetPanel::CommandPerformed( ICommand* pCommand )
{
	
}