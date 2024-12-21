#include "stdafx.h"
#include <wx/wx.h>
#include "cwxRegisteredResPanel.h"
#include "ToolData.h"
#include "CommandSet.h"
#include "RTCutSceneMakerFrame.h"
#include "cwxPropertyPanel.h"
#include "cwxTimeLineCtrl.h"
#include "cwxEventTimeLinePanel.h"
#include "RTCutSceneRenderer.h"





BEGIN_EVENT_TABLE( cwxRegisteredResPanel, wxPanel )
	//EVT_BUTTON( BTN_UNREGISTER_RES_ID, cwxRegisteredResPanel::OnBtnUnRegister )
	EVT_MENU( POPUP_REMOVE_RES, cwxRegisteredResPanel::OnMenuRemoveRes )
	EVT_MENU( POPUP_INSERT_ACTION, cwxRegisteredResPanel::OnMenuInsertAction )
	EVT_MENU( POPUP_INSERT_KEY, cwxRegisteredResPanel::OnMenuInsertKey )
	EVT_MENU( POPUP_SEE_THIS, cwxRegisteredResPanel::OnMenuSeeThis )
	EVT_MENU( POPUP_SELECT, cwxRegisteredResPanel::OnMenuSelect )
	//EVT_MENU( POPUP_INSERT_EVENT, cwxRegisteredResPanel::OnMenuInsertEvent )
	EVT_CONTEXT_MENU( cwxRegisteredResPanel::OnContextMenu )
	EVT_LIST_ITEM_SELECTED( LIST_REGISTERED_RES_ID, cwxRegisteredResPanel::OnSelChangeRegResList )
END_EVENT_TABLE()


cwxRegisteredResPanel::cwxRegisteredResPanel( wxWindow* pParent, wxWindowID Id ) : wxPanel( pParent, Id ),
																				   m_pTopSizer( NULL ),
																				   m_pListRegisteredRes( NULL )
{
	m_pTopSizer = new wxBoxSizer( wxVERTICAL );

	m_pListRegisteredRes = new wxListView( this, LIST_REGISTERED_RES_ID, 
										   wxDefaultPosition, wxDefaultSize, wxLC_REPORT|wxLC_SINGLE_SEL );
	_InsertResListColumn();
	m_pTopSizer->Add( m_pListRegisteredRes, 1, wxALL|wxCENTER|wxEXPAND, 2 );

	SetSizer( m_pTopSizer );
	m_pTopSizer->Fit( this );
	m_pTopSizer->SetSizeHints( this );

	static_cast<CRTCutSceneMakerFrame*>( TOOL_DATA.GetMainFrame() )->ConnectMouseWheelEventToRenderPanel( m_pListRegisteredRes );
}


cwxRegisteredResPanel::~cwxRegisteredResPanel(void)
{

}


void cwxRegisteredResPanel::_InsertResListColumn( void )
{
	m_pListRegisteredRes->InsertColumn( 0, wxT("Name"), wxLIST_FORMAT_CENTRE, 200 );
	m_pListRegisteredRes->InsertColumn( 1, wxT("Kind"), wxLIST_FORMAT_CENTRE, 50 );
}



//int cwxRegisteredResPanel::_GetResInfo( int iIndex, /*IN OUT*/ wxString* pstrResName, /*IN OUT*/ wxString* pstrResFileName )
//{
//	int iResult = -1;
//
//	if( wxNOT_FOUND != iIndex )
//	{
//		wxString strSelectedName = m_pListRegisteredRes->GetItemText(iIndex);
//		int iOffset = (int)strSelectedName.find_first_of( wxT("_") );
//		wxString strResFileName = strSelectedName.substr( 0, iOffset );
//		int iResourceKind = TOOL_DATA.GetResourceKind( strResFileName.c_str() );
//
//		if( NULL != pstrResName )
//			*pstrResName = strSelectedName;
//
//		if( NULL != pstrResFileName )
//			*pstrResFileName = strResFileName;
//
//		iResult = iResourceKind;
//	}
//	
//	return iResult;
//}



void cwxRegisteredResPanel::_UpdateRegResList( void )
{
	m_pListRegisteredRes->Hide();

	m_pListRegisteredRes->ClearAll();
	_InsertResListColumn();
	
	wxString strResName;
	int iNumRegRes = TOOL_DATA.GetNumRegRes();
	for( int iRes = 0; iRes < iNumRegRes; ++iRes )
	{
		TOOL_DATA.GetRegResNameByIndex( iRes, strResName );
		long temp = m_pListRegisteredRes->InsertItem( iRes, strResName.c_str() );
		m_pListRegisteredRes->SetItemData( temp, iRes );
	
		int iResourceKind = TOOL_DATA.GetRegResKindByIndex( iRes );

		switch( iResourceKind )
		{
			case CDnCutSceneData::RT_RES_MAP:
				m_pListRegisteredRes->SetItem( iRes, 1, wxT("Map") );
				break;

			case CDnCutSceneData::RT_RES_ACTOR:
				m_pListRegisteredRes->SetItem( iRes, 1, wxT("Actor") );
				break;

			//case CDnCutSceneData::RT_RES_MAX_CAMERA:
			//	m_pListRegisteredRes->SetItem( iRes, 1, wxT("Camera") );
			//	break;

			case CDnCutSceneData::RT_RES_SOUND:
				m_pListRegisteredRes->SetItem( iRes, 1, wxT("Sound") );
				break;
		}
	}

	m_pListRegisteredRes->Show();
}




void cwxRegisteredResPanel::OnSelChangeRegResList( wxListEvent& ListEvent )
{
	int iSelection = m_pListRegisteredRes->GetNextItem( -1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED );
	TOOL_DATA.SelectRegResource( iSelection );

	//if( CDnCutSceneData::RT_RES_ACTOR == TOOL_DATA.GetRegResKindByIndex( iSelection ) )
	//{
	//	wxString strActorName;
	//	TOOL_DATA.GetRegResNameByIndex( iSelection, strActorName );
	//	TOOL_DATA.GetActorIDByName( strActorName.c_str() );
	//}

	cwxPropertyPanel* pPropertyPanel = TOOL_DATA.GetPropertyPanel();
	pPropertyPanel->OnSelChangeRegResList( iSelection );
}




void cwxRegisteredResPanel::OnMenuRemoveRes( wxCommandEvent& MenuEvent )
{
	long iSelected = m_pListRegisteredRes->GetNextItem( -1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED );
	if( wxNOT_FOUND != iSelected )
	{
		// TODO: ���߿� ��¥�� ������ ������ ���� ���â�� ���;� ��.
		wxString strSelectedName;
		TOOL_DATA.GetRegResNameByIndex( iSelected, strSelectedName );
		int iResourceKind = TOOL_DATA.GetRegResKindByIndex( iSelected );

		CRemoveRegResCmd Command( CToolData::GetInstancePtr(), strSelectedName.c_str(), iResourceKind );
		TOOL_DATA.RunCommand( &Command );
	}
}





void cwxRegisteredResPanel::OnMenuInsertAction( wxCommandEvent& MenuEvent )
{
	// ����Ʈ ���� �־� �׼� �߰�.	
	ActionInfo DefaultActionInfo;
	DefaultActionInfo.iID = TOOL_DATA.GenerateID();

	long iSelected = m_pListRegisteredRes->GetNextItem( -1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED );
	assert( wxNOT_FOUND != iSelected );
	
	wxString strSelectedName;
	char caActorName[ 256 ];
	ZeroMemory( caActorName, sizeof(caActorName) );
	TOOL_DATA.GetRegResNameByIndex( iSelected, strSelectedName );
	TOOL_DATA.WideCharToMultiByte( strSelectedName.c_str(), caActorName, 256 );

	DefaultActionInfo.strActorName.assign( caActorName );
	DefaultActionInfo.strActionName.assign( "Action" );
	DefaultActionInfo.iActionType = ActionInfo::AT_ONCE;
	DefaultActionInfo.fAnimationSpeed = 60.0f;
	wxString strTLActorObjectName( strSelectedName );
	strTLActorObjectName.append( TL_ACTION_POSTFIX );
	DefaultActionInfo.fUnit = 1000.0f;						// �׼��� �� �����̴�.
	DefaultActionInfo.fStartTime = TOOL_DATA.GetActionTimeLine()->GetNextObjectStartTime( strTLActorObjectName.c_str() );

	// ���̴� �ִϸ��̼� �ӵ��� ����� �����ش�.
	//DefaultActionInfo.fTimeLength = 5.0f;
	const ActionEleInfo* pAnimationInfo = TOOL_DATA.GetThisActorsAnimation( strSelectedName.c_str(), 0 );
	DefaultActionInfo.fTimeLength = (float)pAnimationInfo->dwLength / DefaultActionInfo.fAnimationSpeed;
	//DefaultActionInfo.fRotation = 0.0f;
	DefaultActionInfo.iAnimationIndex = 0;
	DefaultActionInfo.strAnimationName.assign(pAnimationInfo->szName);
	DefaultActionInfo.iNextActionFrame = (int)pAnimationInfo->dwNextActionFrame;
	DefaultActionInfo.bUseAniDistance = true;
	DefaultActionInfo.bFitAniDistanceYToMap = true;
	//DefaultActionInfo.vStartPos.x = DefaultActionInfo.vStartPos.y = DefaultActionInfo.vStartPos.z = 0.0f;
	//DefaultActionInfo.vEndPos.x = DefaultActionInfo.vEndPos.y = DefaultActionInfo.vEndPos.z = 0.0f;

	CActionInsertCmd InsertActionCmd( &TOOL_DATA, &DefaultActionInfo );
	TOOL_DATA.RunCommand( &InsertActionCmd );
}



void cwxRegisteredResPanel::OnMenuInsertKey( wxCommandEvent& MenuEvent )
{
	KeyInfo DefaultKeyInfo;
	DefaultKeyInfo.iID = TOOL_DATA.GenerateID();

	long iSelected = m_pListRegisteredRes->GetNextItem( -1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED );
	assert( wxNOT_FOUND != iSelected );

	wxString strSelectedName;
	char caActorName[ 256 ];
	ZeroMemory( caActorName, sizeof(caActorName) );
	TOOL_DATA.GetRegResNameByIndex( iSelected, strSelectedName );
	TOOL_DATA.WideCharToMultiByte( strSelectedName.c_str(), caActorName, 256 );

	DefaultKeyInfo.strActorName.assign( caActorName );
	DefaultKeyInfo.strKeyName.assign( "Key" );
	DefaultKeyInfo.fUnit = 1000.0f;						// Ű�� �� �����̴�.
	wxString strTLKeyObjectName( strSelectedName );
	strTLKeyObjectName.append( TL_KEY_POSTFIX );
	DefaultKeyInfo.fStartTime = TOOL_DATA.GetActionTimeLine()->GetNextObjectStartTime( strTLKeyObjectName.c_str() );
	DefaultKeyInfo.fTimeLength = 10.0f;
	DefaultKeyInfo.fRotDegree = 0.0f;
	DefaultKeyInfo.vDestPos.x = DefaultKeyInfo.vDestPos.y = DefaultKeyInfo.vDestPos.z = 0.0f;
	DefaultKeyInfo.bFitYPosToMap = true;

	// ���� Ű ���� �����Ѵٸ� �������� �״�� �����Ѵ�
	int iNumKeys = TOOL_DATA.GetThisActorsKeyNum( strSelectedName.c_str() );
	if( 0 < iNumKeys )
	{
		DefaultKeyInfo.vStartPos = TOOL_DATA.GetThisActorsKeyInfoByIndex( strSelectedName.c_str(), iNumKeys-1 )->vDestPos;
	}
	else
	{
		// ���� ĳ���� ��ġ�� �޾ƿ´�.
		const EtVector3 vPos = TOOL_DATA.GetRegResPos( strSelectedName.c_str() );
		DefaultKeyInfo.vStartPos = vPos;
	}

	DefaultKeyInfo.fScale = 1.0f;

	CKeyInsertCmd InsertKeyCmd( &TOOL_DATA, &DefaultKeyInfo );
	TOOL_DATA.RunCommand( &InsertKeyCmd ); 
}




void cwxRegisteredResPanel::OnMenuSeeThis( wxCommandEvent& MenuEvent )
{
	CRTCutSceneRenderer* pRenderer = TOOL_DATA.GetRenderer();

	long iSelected = m_pListRegisteredRes->GetNextItem( -1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED );
	if( wxNOT_FOUND != iSelected )
	{
		wxString strSelectedName;
		TOOL_DATA.GetRegResNameByIndex( iSelected, strSelectedName );
		pRenderer->SeeThisActor( strSelectedName.c_str() );
	}
}




void cwxRegisteredResPanel::OnMenuSelect( wxCommandEvent& MenuEvent )
{
	CRTCutSceneRenderer* pRenderer = TOOL_DATA.GetRenderer();

	int iSelected = m_pListRegisteredRes->GetNextItem( -1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED );
	if( wxNOT_FOUND != iSelected )
	{
		wxString strSelectedName;
		TOOL_DATA.GetRegResNameByIndex( iSelected, strSelectedName );
		pRenderer->SelectActor( strSelectedName.c_str() );
	}
}





void cwxRegisteredResPanel::_PopupContextMenu( int iXPos, int iYPos )
{
	long iSelected = m_pListRegisteredRes->GetNextItem( -1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED );
	if( wxNOT_FOUND != iSelected )
	{
		// �߿�: ����Ʈ �信���� ������ ���� �߰��� ������ ��ġ�Ѵ�.
		int iResourceKind = TOOL_DATA.GetRegResKindByIndex( iSelected );

		// ���� ���õ� ���ҽ��� Ÿ�Կ� ���� �б�.
		wxMenu Menu;

		switch( iResourceKind )
		{
			case CDnCutSceneData::RT_RES_MAP:
				break;

			case CDnCutSceneData::RT_RES_ACTOR:
				Menu.Append( POPUP_INSERT_ACTION, wxT("Insert &Action") );
				Menu.Append( POPUP_INSERT_KEY, wxT("Insert &Key") );
				Menu.AppendSeparator();
				Menu.Append( POPUP_SEE_THIS, wxT("See &This") );
				Menu.Append( POPUP_SELECT, wxT("&Select") );
				break;
			
			//case CDnCutSceneData::RT_RES_MAX_CAMERA:
			//	Menu.Append( POPUP_INSERT_EVENT, wxT("Insert &Event") );
			//	/*Menu.Append( POPUP_INSERT )*/
			//	break;

			case CDnCutSceneData::RT_RES_SOUND:
				Menu.Append( POPUP_INSERT_EVENT, wxT("Insert &Event") );
				break;
		}

		Menu.AppendSeparator();
		Menu.Append( POPUP_REMOVE_RES, wxT("&Remove") );

		PopupMenu( &Menu, iXPos, iYPos );
	}
}



void cwxRegisteredResPanel::OnContextMenu( wxContextMenuEvent& ContextEvent )
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



void cwxRegisteredResPanel::CommandPerformed( ICommand* pCommand )
{
	switch( pCommand->GetTypeID() )
	{
		case CMD_UPDATE_VIEW:
		case CMD_REGISTER_RES:
		case CMD_UNREGISTER_RES:
		case CMD_ACTOR_PROP_CHANGE:
		case CMD_CHANGE_ACTOR_RES:
			_UpdateRegResList();
			break;
	}
}