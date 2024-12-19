#include "StdAfx.h"
#include <wx/wx.h>
#include "cwxActionTimeLinePanel.h"
#include "ToolData.h"
#include "cwxPropertyPanel.h"
#include "CommandSet.h"
#include "RTCutSceneMakerFrame.h"
#include "cwxRenderPanel.h"
#include "RTCutSceneRenderer.h"



BEGIN_EVENT_TABLE( cwxActionTimeLinePanel, wxPanel )
	EVT_MOUSE_EVENTS( cwxActionTimeLinePanel::OnMouseEvent )
	EVT_MENU( POPUP_COPY, cwxActionTimeLinePanel::OnMenuCopy )
	EVT_MENU( POPUP_PASTE, cwxActionTimeLinePanel::OnMenuPaste )
	EVT_MENU( POPUP_SNAP_ACTION, cwxActionTimeLinePanel::OnMenuSnapAction )
	EVT_MENU( POPUP_REMOVE_ACTION, cwxActionTimeLinePanel::OnMenuRemoveAction )

	EVT_TIMELINE_ACTION_SELECT( ACTION_TIMELINE_ID, cwxActionTimeLinePanel::OnTLObjectSelect )
	EVT_TIMELINE_ACTION_MOVE( ACTION_TIMELINE_ID, cwxActionTimeLinePanel::OnTLObjectMove )
	EVT_TIMELINE_ACTION_RESIZE( ACTION_TIMELINE_ID, cwxActionTimeLinePanel::OnTLObjectResize ) 

	EVT_TIMELINE_AXIS_EVENT( ACTION_TIMELINE_ID, cwxActionTimeLinePanel::OnTLAxisEvent )

	// 팝업 메뉴
	//EVT_CONTEXT_MENU( cwxActionTimeLinePanel::OnContextMenu )
END_EVENT_TABLE()


cwxActionTimeLinePanel::cwxActionTimeLinePanel( wxWindow* pParent, wxWindowID id ) : wxPanel( pParent, id ),
																					 m_pTopSizer( NULL ),
																					 m_pTimeLine( NULL ),
																					 m_iOriEditMode( CToolData::EM_EDIT )
{
	m_pTimeLine = new cwxTimeLineCtrl( this, ACTION_TIMELINE_ID );
	m_pTopSizer = new wxBoxSizer( wxVERTICAL );
	m_pTopSizer->Add( m_pTimeLine, 1, wxALL|wxALIGN_CENTER|wxEXPAND, 2 );

	this->SetSizer( m_pTopSizer );
	m_pTopSizer->Fit( this );
	m_pTopSizer->SetSizeHints( this );

	TOOL_DATA.SetActionTimeLine( m_pTimeLine );

	m_pTimeLine->SetViewScale( 6.0f );
}

cwxActionTimeLinePanel::~cwxActionTimeLinePanel(void)
{

}



bool cwxActionTimeLinePanel::Enable( bool bEnable /* = true  */ )
{
	m_pTimeLine->Enable( bEnable );

	return true;
}



void cwxActionTimeLinePanel::OnTLObjectSelect( cwxTLObjectSelectEvent& TLSelectEvent )
{
	TOOL_DATA.SetSelectedObjectID( TLSelectEvent.GetObjectID() );
	TOOL_DATA.SetSelectedObjectUseType( TLSelectEvent.GetObjectUseType() );

	cwxPropertyPanel* pPropertyPanel = TOOL_DATA.GetPropertyPanel();
	pPropertyPanel->OnTLActionSelect();
}


void cwxActionTimeLinePanel::OnTLObjectMove( cwxTLObjectMoveEvent& TLMoveEvent )
{
	int iSelectedObjectID = TLMoveEvent.GetObjectID();// TOOL_DATA.GetSelectedActionID();
	int iObjectUseType = TOOL_DATA.GetSelectedObjectUseType();

	switch( iObjectUseType )
	{
		case CToolData::ACTION:
			{
				const ActionInfo* pActionInfo = TOOL_DATA.GetActionInfoByID( iSelectedObjectID );

				ActionInfo NewActionInfo = *pActionInfo;
				NewActionInfo.fStartTime = TLMoveEvent.GetActionStartTime();
				
				CActionPropChange ActionPropChange( &TOOL_DATA, &NewActionInfo );
				TOOL_DATA.RunCommand( &ActionPropChange );
			}
			break;

		case CToolData::KEY:
			{
				const KeyInfo* pKeyInfo = TOOL_DATA.GetKeyInfoByID( iSelectedObjectID );
				
				KeyInfo NewKeyInfo = *pKeyInfo;
				NewKeyInfo.fStartTime = TLMoveEvent.GetActionStartTime();

				CKeyPropChange KeyPropChange( &TOOL_DATA, &NewKeyInfo );
				TOOL_DATA.RunCommand( &KeyPropChange );
			}
			break;

		case CToolData::SUBKEY:
			{
				const KeyInfo* pKeyInfo = TOOL_DATA.GetKeyInfoByID( iSelectedObjectID );

				KeyInfo NewKeyInfo = *pKeyInfo;

				vector<SubKey>::iterator iterBegin = NewKeyInfo.vlMoveKeys.begin();
				vector<SubKey>::iterator iter = iterBegin;

				vector<SubKey>::iterator iterFind = find( NewKeyInfo.vlMoveKeys.begin(), NewKeyInfo.vlMoveKeys.end(), iSelectedObjectID );
				assert( NewKeyInfo.vlMoveKeys.end() != iter );
				
				float fSubKeyStart = TLMoveEvent.GetActionStartTime();
				for( iter; iter != iterFind; ++iter )
					fSubKeyStart -= iterBegin->fTimeLength;	

				if( fSubKeyStart < 0.0f )
				{
					fSubKeyStart = 0.0f;
					
					//iter = iterBegin;
					//for( iter; iter != NewKeyInfo.vlMoveKeys.end(); ++iter )
					//	iter->fT
				}
				
				NewKeyInfo.fStartTime = fSubKeyStart;
				//NewKeyInfo.fStartTime = NewKeyInfo.fTimeLength - (fSubKeyStart + iter->fTimeLength);
				//NewKeyInfo.fStartTime = TLMoveEvent.GetActionStartInUnit();

				CKeyPropChange KeyPropChange( &TOOL_DATA, &NewKeyInfo );
				TOOL_DATA.RunCommand( &KeyPropChange );
			}
			break;
	}
}


void cwxActionTimeLinePanel::OnTLObjectResize( cwxTLObjectResizeEvent& TLResizeEvent )
{
	int iSelectedObjectID = TLResizeEvent.GetObjectID();//TOOL_DATA.GetSelectedActionID();
	int iObjectUseType = TOOL_DATA.GetSelectedObjectUseType();

	if( CToolData::ACTION == iObjectUseType )
	{
		const ActionInfo* pActionInfo = TOOL_DATA.GetActionInfoByID( iSelectedObjectID );

		ActionInfo NewActionInfo = *pActionInfo;
		NewActionInfo.fStartTime = TLResizeEvent.GetActionStartInUnit();
		NewActionInfo.fTimeLength = TLResizeEvent.GetNewWidthInUnit();

		CActionPropChange ActionPropChange( &TOOL_DATA, &NewActionInfo );
		TOOL_DATA.RunCommand( &ActionPropChange );
	}
	else
	{
		const KeyInfo* pKeyInfo = TOOL_DATA.GetKeyInfoByID( iSelectedObjectID );

		KeyInfo NewKeyInfo = *pKeyInfo;
		NewKeyInfo.fStartTime = TLResizeEvent.GetActionStartInUnit();
		NewKeyInfo.fTimeLength = TLResizeEvent.GetNewWidthInUnit();

		CKeyPropChange KeyPropChange( &TOOL_DATA, &NewKeyInfo );
		TOOL_DATA.RunCommand( &KeyPropChange );
	}
}



void cwxActionTimeLinePanel::OnTLAxisEvent( cwxTLAxisEvent& TLAxisEvent )
{
	CRTCutSceneRenderer* pRenderer = TOOL_DATA.GetRenderer();
	CRTCutSceneMakerFrame* pMainFrame = static_cast<CRTCutSceneMakerFrame*>(TOOL_DATA.GetMainFrame());
	cwxRenderPanel* pRenderPanel = pMainFrame->GetRenderPanel();

	if( TLAxisEvent.OnLBDown() )
	{
		m_iOriEditMode = TOOL_DATA.GetEditMode();
		TOOL_DATA.SetEditMode( CToolData::EM_FREE_TIME );
		pRenderer->SetMode( IRenderBase::FREE_TIME_MODE );
		TOOL_DATA.SetLocalTime( GetNowTime() );
	}
	else
	if( TLAxisEvent.OnLBUp() )
	{
		TOOL_DATA.SetEditMode( m_iOriEditMode );

		if( CToolData::EM_EDIT == m_iOriEditMode )
			pRenderer->SetMode( IRenderBase::EDITOR_MODE );//pRenderPanel->StopPlay();
		else
		if( CToolData::EM_PLAY == m_iOriEditMode )
		{
			pRenderer->SetMode( IRenderBase::PLAY_MODE );
		}
	}
	else
	if( TLAxisEvent.IsAxisMove() && TLAxisEvent.IsLBDown() )
	{
		TOOL_DATA.SetLocalTime( GetNowTime() );
	}
}




void cwxActionTimeLinePanel::_PopupContextMenu( int iXPos, int iYPos )
{
	//long iSelected = m_pListRegisteredRes->GetNextItem( -1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED );
	//int iSelected = TOOL_DATA.GetSelectedActionID();
	int iSelected = m_pTimeLine->GetSelectedObjectID();
	
	wxMenu Menu;
	wxMenuItem* pCopyItem = Menu.Append( POPUP_COPY, wxT("&Copy") );		// 선택된 이벤트가 있어야 활성화 됨.
	wxMenuItem* pPasteItem = Menu.Append( POPUP_PASTE, wxT("&Paste") );		// 복사된 내용이 있어야 활성화 됨.

	Menu.AppendSeparator();
	wxMenuItem* pSnapItem = Menu.Append( POPUP_SNAP_ACTION, wxT("&Snap") );
	Menu.AppendSeparator(); 
	wxMenuItem* pRemoveItem = Menu.Append( POPUP_REMOVE_ACTION, wxT("&Remove") );

	if( wxNOT_FOUND == iSelected )
	{
		pCopyItem->Enable( false );
		pSnapItem->Enable( false );
		pRemoveItem->Enable( false );
	}

	// 현재 커서가 어디있는지에 따라 다름
	int iNowOnOverYIndex = m_pTimeLine->GetYLineIndex();

	bool bAction = iNowOnOverYIndex % 2 == 0;
	if( bAction )
		pPasteItem->Enable( TOOL_DATA.IsActionCopied() );
	else
		pPasteItem->Enable( TOOL_DATA.IsKeyCopied() );		// 짝수 라인은 키임

	PopupMenu( &Menu, iXPos, iYPos );
}



//void cwxActionTimeLinePanel::OnContextMenu( wxContextMenuEvent& ContextEvent )
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



void cwxActionTimeLinePanel::OnMouseEvent( wxMouseEvent& MouseEvent )
{
	if( MouseEvent.RightUp() )
		_PopupContextMenu( MouseEvent.GetX(), MouseEvent.GetY() );

	//MouseEvent.
}


void cwxActionTimeLinePanel::OnMenuCopy( wxCommandEvent& MenuEvent )
{
	// 데이터 클립보드에 넣어두는 커맨드
	int iSelectedObjectID = m_pTimeLine->GetSelectedObjectID();
	int iSelectedUseType = m_pTimeLine->GetSelectedObjectUseType();

	CCopyToClipboard* pCopyToClipboard = NULL;

	switch( iSelectedUseType )
	{
		case CToolData::ACTION:
			{
				const ActionInfo* pActionInfo = TOOL_DATA.GetActionInfoByID( iSelectedObjectID );
				if( pActionInfo )
				{
					pCopyToClipboard = new CCopyToClipboard( &TOOL_DATA, pActionInfo );
				}
			}
			break;

		case CToolData::KEY:
			{
				const KeyInfo* pKeyInfo = TOOL_DATA.GetKeyInfoByID( iSelectedObjectID );
				if( pKeyInfo )
				{
					pCopyToClipboard = new CCopyToClipboard( &TOOL_DATA, pKeyInfo );
				}
			}
			break;

		case CToolData::SUBKEY:
			{
				const KeyInfo* pKeyInfo = TOOL_DATA.GetKeyInfoByID( iSelectedObjectID );
				if( pKeyInfo )
				{
					pCopyToClipboard = new CCopyToClipboard( &TOOL_DATA, pKeyInfo );
				}
			}
	}

	TOOL_DATA.RunCommand( pCopyToClipboard );
	delete pCopyToClipboard;
}


void cwxActionTimeLinePanel::OnMenuPaste( wxCommandEvent& MenuEvent )
{
	int iNowType = -1;

	// 현재 커서가 어디있는지에 따라 다름
	int iNowOnOverYIndex = m_pTimeLine->GetYLineIndex();

	bool bAction = iNowOnOverYIndex % 2 == 0;
	if( bAction )
		iNowType = CCopyToClipboard::COPY_ACTION;
	else
		iNowType = CCopyToClipboard::COPY_KEY;		// 짝수 라인은 키임

	wxASSERT( -1 != iNowType );
	if( -1 != iNowType )
	{
		int iActorIndex = iNowOnOverYIndex / 2;
		if( TOOL_DATA.GetNumActors() <= iActorIndex )
			iActorIndex = TOOL_DATA.GetNumActors() - 1;

		wxString strActorNameToPaste;
		TOOL_DATA.GetActorNameByIndex( iActorIndex, strActorNameToPaste );

		wxCSConv MBConv( wxFONTENCODING_CP949 );
		char acBuf[ 256 ];
		ZeroMemory( acBuf, sizeof(acBuf) );
		MBConv.WC2MB( acBuf, strActorNameToPaste.c_str(), 256 );

		float fNowTimeByUnit = m_pTimeLine->GetNowMousePosTime();
		CPasteFromClipboard PasteFromClipboard( &TOOL_DATA, iNowType, acBuf, fNowTimeByUnit );
		TOOL_DATA.RunCommand( &PasteFromClipboard );
	}
}



void cwxActionTimeLinePanel::OnMenuSnapAction( wxCommandEvent& MenuEvent )
{
	int iSelectedID = m_pTimeLine->GetSelectedObjectID();
	int iSelectedUseType = m_pTimeLine->GetSelectedObjectUseType();

	switch( iSelectedUseType )
	{
		case CToolData::ACTION:
			{
				const ActionInfo* pActionInfo = TOOL_DATA.GetActionInfoByID( iSelectedID );
				if( pActionInfo )
				{
					// 바로 이전의 액션 정보를 찾는다.
					wxCSConv MBConv( wxFONTENCODING_CP949 );
					wxChar wcaBuf[ 256 ];
					ZeroMemory( wcaBuf, sizeof(wcaBuf) );
					MBConv.MB2WC( wcaBuf, string(pActionInfo->strActorName + "_ACTION").c_str(), 256 );

					ActionInfo NewActionInfo = *pActionInfo;
					NewActionInfo.fStartTime =  m_pTimeLine->GetPrevObjectEndTime( wcaBuf, NewActionInfo.iID );
					
					CActionPropChange ActionPropChange( &TOOL_DATA, &NewActionInfo );
					TOOL_DATA.RunCommand( &ActionPropChange );
				}
			}
			break;
		
		// 서브키는 아직...
		case CToolData::KEY:
			{
				const KeyInfo* pKeyInfo = TOOL_DATA.GetKeyInfoByID( iSelectedID );
				if( pKeyInfo )
				{
					// 바로 이전의 키 정보를 찾는다.
					wxCSConv MBConv( wxFONTENCODING_CP949 );
					wxChar wcaBuf[ 256 ];
					ZeroMemory( wcaBuf, sizeof(wcaBuf) );
					MBConv.MB2WC( wcaBuf, (pKeyInfo->strActorName+"_KEY").c_str(), 256 );

					KeyInfo NewKeyInfo = *pKeyInfo;
					NewKeyInfo.fStartTime =  m_pTimeLine->GetPrevObjectEndTime( wcaBuf, NewKeyInfo.iID );

					CKeyPropChange KeyPropChange( &TOOL_DATA, &NewKeyInfo );
					TOOL_DATA.RunCommand( &KeyPropChange );
				}
			}
			break;
	}
}




void cwxActionTimeLinePanel::OnMenuRemoveAction( wxCommandEvent& MenuEvent )
{
	int iSelectedID = m_pTimeLine->GetSelectedObjectID();
	int iSelectedUseType = m_pTimeLine->GetSelectedObjectUseType();

	switch( iSelectedUseType )
	{
		case CToolData::ACTION:
			{
				CActionRemoveCmd ActionRemove( &TOOL_DATA, iSelectedID );
				TOOL_DATA.RunCommand( &ActionRemove );
			}
			break;
		
		case CToolData::KEY:
		case CToolData::SUBKEY:
			{
				CKeyRemoveCmd KeyRemove( &TOOL_DATA, iSelectedID );
				TOOL_DATA.RunCommand( &KeyRemove );
			}
			break;
	}
}



// 데이터의 내용을 그대로 반영
void cwxActionTimeLinePanel::ReflectData( void )
{
	int iSelectedActionID = m_pTimeLine->GetSelectedObjectID();

	m_pTimeLine->Clear();

	wxCSConv MBConv( wxFONTENCODING_CP949 );
	wxChar wcaBuf[ 256 ];
	ZeroMemory( wcaBuf, sizeof(wcaBuf) ); 

	wxString strActorName;
	int iNumActor = TOOL_DATA.GetNumActors();
	for( int iActor = 0; iActor < iNumActor; ++iActor )
	{
		TOOL_DATA.GetActorNameByIndex( iActor, strActorName );
		
		wxString strActionObject = strActorName+TL_ACTION_POSTFIX;
		m_pTimeLine->AddItem( strActionObject, wxColour(wxT("BLUE")) );
		
		int iNumAction = TOOL_DATA.GetThisActorsActionNum( strActorName.c_str() );
		//int iNumAction = TOOL_DATA.GetThisActorsActionNum( strActionObject.c_str() );
		for( int iAction = 0; iAction < iNumAction; ++iAction )
		{
			const ActionInfo* pActionInfo = TOOL_DATA.GetThisActorsActionInfoByIndex( strActorName.c_str(), iAction );
	
			MBConv.MB2WC( wcaBuf, pActionInfo->strActionName.c_str(), 256 );
			bool bFixObject = (pActionInfo->iActionType == ActionInfo::AT_ONCE) ? true : false;

			// 컬러는 액션 타입에 따라..
			wxColour Color;
			switch( pActionInfo->iActionType )
			{
				case ActionInfo::AT_ONCE:
					Color.Set( 255, 151, 255 );
					break;

				case ActionInfo::AT_REPEAT:
					Color.Set( 122, 192, 67 );
					break;

				case ActionInfo::AT_CONTINUE:
					Color.Set( 255, 167, 25 );
					break;
			}

			m_pTimeLine->AddObject( strActionObject, wcaBuf, pActionInfo->iID, 
									 CToolData::ACTION, bFixObject, pActionInfo->fStartTime, pActionInfo->fTimeLength, Color );

			//if( ActionInfo::AT_ONCE == pActionInfo->iActionType )
			//	m_pTimeLine->FixObjectSize( pActionInfo->iID, true );
		}

		wxString strKeyObject = strActorName+TL_KEY_POSTFIX;
		m_pTimeLine->AddItem( strKeyObject, wxColour(wxT("BLUE")) );

		int iNumKey = TOOL_DATA.GetThisActorsKeyNum( strActorName.c_str() );
		//int iNumKey = TOOL_DATA.GetThisActorsKeyNum( strKeyObject.c_str() );
		for( int iKey = 0; iKey < iNumKey; ++iKey )
		{
			const KeyInfo* pKeyInfo = TOOL_DATA.GetThisActorsKeyInfoByIndex( strActorName.c_str(), iKey );

			wxColour Color;
			Color.Set( 200, 200, 200 );
			
			MBConv.MB2WC( wcaBuf, pKeyInfo->strKeyName.c_str(), 256 );
			
			switch( pKeyInfo->iKeyType )
			{
				case KeyInfo::MOVE:
				case KeyInfo::ROTATION:
				case KeyInfo::SHOW:
				case KeyInfo::HIDE:
					m_pTimeLine->AddObject( strKeyObject, wcaBuf, pKeyInfo->iID, 
											 CToolData::KEY, false, pKeyInfo->fStartTime, pKeyInfo->fTimeLength, Color );
					break;

				case KeyInfo::MULTI_MOVE:
					{
						float fNowStartTime = pKeyInfo->fStartTime;
						int iNumKey = (int)pKeyInfo->vlMoveKeys.size();
						for( int iKey = 0; iKey < iNumKey-1; ++iKey )			// 마지막 키는 목적지이므로 타임 라인 컨트롤에 넣지 않는다.
						{
							const SubKey& MoveKey = pKeyInfo->vlMoveKeys.at(iKey);

							// 갯수만큼 추가. 가운데 녀석에게만 이름을 넣어주자.
							if( iNumKey / 2 == iKey )
								m_pTimeLine->AddObject( strKeyObject, wcaBuf, MoveKey.iID, CToolData::SUBKEY, true, 
														 fNowStartTime, MoveKey.fTimeLength, Color );
							else
								m_pTimeLine->AddObject( strKeyObject, wxEmptyString, MoveKey.iID, CToolData::SUBKEY, true, 
														 fNowStartTime, MoveKey.fTimeLength, Color );

							fNowStartTime += MoveKey.fTimeLength;
						}
					}
					break;
			}
		}
	}

	if( wxNOT_FOUND != iSelectedActionID )
		m_pTimeLine->SelectObject( iSelectedActionID );

	int iHorizontal = m_pTimeLine->GetScrollPos( wxHORIZONTAL );
	int iVertical = m_pTimeLine->GetScrollPos( wxVERTICAL );
	m_pTimeLine->SetScrollPos( wxHORIZONTAL, iHorizontal, false );
	m_pTimeLine->SetScrollPos( wxVERTICAL, iVertical, false );
	m_pTimeLine->Refresh( true, &m_pTimeLine->GetClientRect() );
}


void cwxActionTimeLinePanel::_KeyObjectUpdate( ICommand* pCommand, wxCSConv &MBConv )
{
	//const KeyInfo& NewKeyInfo = static_cast<CKeyPropChange*>(pCommand)->m_NewKeyInfo;
	CKeyPropChange* pKeyPropChange = static_cast<CKeyPropChange*>(pCommand);

	const KeyInfo* pNewKeyInfo = NULL;
	if( pKeyPropChange->m_bUndo )
		pNewKeyInfo = &pKeyPropChange->m_PrevKeyInfo;
	else
		pNewKeyInfo = &pKeyPropChange->m_NewKeyInfo;

	wxColour Color;
	Color.Set( 200, 200, 200 );

	wxChar wcaActorName[ 256 ];
	ZeroMemory( wcaActorName, sizeof(wcaActorName) );
	MBConv.MB2WC( wcaActorName, pNewKeyInfo->strActorName.c_str(), 256 );
	wxStrcat( wcaActorName, TL_KEY_POSTFIX );

	wxChar wcaKeyName[ 256 ];
	ZeroMemory( wcaKeyName, sizeof(wcaKeyName) );
	MBConv.MB2WC( wcaKeyName, pNewKeyInfo->strKeyName.c_str(), 256 );

	switch( pNewKeyInfo->iKeyType )
	{
		case KeyInfo::MOVE:
		case KeyInfo::ROTATION:
		case KeyInfo::SHOW:
		case KeyInfo::HIDE:
			m_pTimeLine->ModifyObject( wcaActorName, wcaKeyName, pNewKeyInfo->iID, 
				CToolData::KEY, false, pNewKeyInfo->fStartTime, pNewKeyInfo->fTimeLength, Color );
			break;

		case KeyInfo::MULTI_MOVE:
			{
				float fNowStartTime = pNewKeyInfo->fStartTime;
				int iNumKey = (int)pNewKeyInfo->vlMoveKeys.size();
				for( int iKey = 0; iKey < iNumKey-1; ++iKey )			// 마지막 키는 목적지이므로 타임 라인 컨트롤에 넣지 않는다.
				{
					const SubKey& MoveKey = pNewKeyInfo->vlMoveKeys.at(iKey);

					// 갯수만큼 추가. 가운데 녀석에게만 이름을 넣어주자.
					if( iNumKey / 2 == iKey )
						m_pTimeLine->ModifyObject( wcaActorName, wcaKeyName, MoveKey.iID, CToolData::SUBKEY, true, 
						fNowStartTime, MoveKey.fTimeLength, Color );
					else
						m_pTimeLine->ModifyObject( wcaActorName, wxEmptyString, MoveKey.iID, CToolData::SUBKEY, true, 
						fNowStartTime, MoveKey.fTimeLength, Color );

					fNowStartTime += MoveKey.fTimeLength;
				}
			}
			break;
	}
}
void cwxActionTimeLinePanel::_ActionObjectUpdate( ICommand* pCommand, wxCSConv &MBConv )
{
	CActionPropChange* pActionPropChange = static_cast<CActionPropChange*>(pCommand);

	const ActionInfo* pNewActionInfo = NULL;

	if( pActionPropChange->m_bUndo )
		pNewActionInfo = &pActionPropChange->m_PrevActionInfo;
	else
		pNewActionInfo = &pActionPropChange->m_NewActionInfo;

	wxChar wcaActorName[ 256 ];
	ZeroMemory( wcaActorName, sizeof(wcaActorName) );
	MBConv.MB2WC( wcaActorName, pNewActionInfo->strActorName.c_str(), 256 );
	wxStrcat( wcaActorName, TL_ACTION_POSTFIX );

	wxChar wcaActionName[ 256 ];
	ZeroMemory( wcaActionName, sizeof(wcaActionName) );
	MBConv.MB2WC( wcaActionName, pNewActionInfo->strActionName.c_str(), 256 );

	bool bFixObject = (pNewActionInfo->iActionType == ActionInfo::AT_ONCE) ? true : false;

	// 컬러는 액션 타입에 따라..
	wxColour Color;
	switch( pNewActionInfo->iActionType )
	{
		case ActionInfo::AT_ONCE:
			Color.Set( 255, 151, 255 );
			break;

		case ActionInfo::AT_REPEAT:
			Color.Set( 122, 192, 67 );
			break;

		case ActionInfo::AT_CONTINUE:
			Color.Set( 255, 167, 25 );
			break;
	}

	m_pTimeLine->ModifyObject( wcaActorName, wcaActionName, pNewActionInfo->iID, 
								CToolData::ACTION, bFixObject, pNewActionInfo->fStartTime, pNewActionInfo->fTimeLength, Color );
}



// 수정된 Object 만 반영
void cwxActionTimeLinePanel::ModifyObject( ICommand* pCommand )
{
	assert( CMD_ACTION_PROP_CHANGE == pCommand->GetTypeID() ||
		    CMD_KEY_PROP_CHANGE == pCommand->GetTypeID() );

	wxCSConv MBConv( wxFONTENCODING_CP949 );

	switch( pCommand->GetTypeID() )
	{
		case CMD_ACTION_PROP_CHANGE:
			{
				_ActionObjectUpdate(pCommand, MBConv);
			}
			break;

		case CMD_KEY_PROP_CHANGE:
			{
				_KeyObjectUpdate(pCommand, MBConv);
			}
			break;
	}

	int iHorizontal = m_pTimeLine->GetScrollPos( wxHORIZONTAL );
	int iVertical = m_pTimeLine->GetScrollPos( wxVERTICAL );
	m_pTimeLine->SetScrollPos( wxHORIZONTAL, iHorizontal, false );
	m_pTimeLine->SetScrollPos( wxVERTICAL, iVertical, false );
	m_pTimeLine->Refresh( true, &m_pTimeLine->GetClientRect() );
}




DWORD cwxActionTimeLinePanel::GetNowTime( void )
{
	DWORD dwNowTime = DWORD(m_pTimeLine->GetNowTime() * 1000.0f);

	return dwNowTime;
}


void cwxActionTimeLinePanel::SetTime( float fTime )
{
	// 이건 milliseconds 만 적용됨
	m_pTimeLine->UpdateTimeBar( fTime );			// 액션 타임 라인은 단위가 초 단위이다.

	int iHorizontal = m_pTimeLine->GetScrollPos( wxHORIZONTAL );
	int iVertical = m_pTimeLine->GetScrollPos( wxVERTICAL );
	m_pTimeLine->SetScrollPos( wxHORIZONTAL, iHorizontal, false );
	m_pTimeLine->SetScrollPos( wxVERTICAL, iVertical, false );
	m_pTimeLine->Refresh( true, &m_pTimeLine->GetClientRect() );
}


void cwxActionTimeLinePanel::OnMouseEventFromFrame( wxMouseEvent& MouseEvent )
{
	m_pTimeLine->OnMouseEvent( MouseEvent );
}