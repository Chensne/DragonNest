#include "StdAfx.h"
#include <wx/wx.h>
#include "cwxEventTimeLinePanel.h"
#include "cwxTimeLineCtrl.h"
#include "ToolData.h"
#include "RTCutSceneMakerFrame.h"
#include "CommandSet.h"
#include "cwxPropertyPanel.h"
#include "RTCutSceneRenderer.h"



BEGIN_EVENT_TABLE( cwxEventTimeLinePanel, wxPanel )
	EVT_MOUSE_EVENTS( cwxEventTimeLinePanel::OnMouseEvent )
	
	EVT_MENU( POPUP_INSERT_CAM_EVENT, cwxEventTimeLinePanel::OnMenuInsertCamEvent )
	EVT_MENU( POPUP_INSERT_PARTICLE_EVENT, cwxEventTimeLinePanel::OnMenuInsertParticleEvent )
	EVT_MENU( POPUP_INSERT_DOF, cwxEventTimeLinePanel::OnMenuInsertDOFEvent )
	//EVT_MENU( POPUP_INSERT_SOUND_EVENT, cwxEventTimeLinePanel::OnMenuInsertSoundEvent )
	EVT_MENU( POPUP_INSERT_FADE_EVENT, cwxEventTimeLinePanel::OnMenuInsertFadeEvent )
	EVT_MENU( POPUP_INSERT_PROP_EVENT, cwxEventTimeLinePanel::OnMenuInsertPropEvent )
	EVT_MENU( POPUP_INSERT_SOUND_1_EVENT, cwxEventTimeLinePanel::OnMenuInsertSoundEvent )
	EVT_MENU( POPUP_INSERT_SOUND_2_EVENT, cwxEventTimeLinePanel::OnMenuInsertSoundEvent )
	EVT_MENU( POPUP_INSERT_SOUND_3_EVENT, cwxEventTimeLinePanel::OnMenuInsertSoundEvent )
	EVT_MENU( POPUP_INSERT_SOUND_4_EVENT, cwxEventTimeLinePanel::OnMenuInsertSoundEvent )
	EVT_MENU( POPUP_INSERT_SOUND_5_EVENT, cwxEventTimeLinePanel::OnMenuInsertSoundEvent )
	EVT_MENU( POPUP_INSERT_SOUND_6_EVENT, cwxEventTimeLinePanel::OnMenuInsertSoundEvent )
	EVT_MENU( POPUP_INSERT_SOUND_7_EVENT, cwxEventTimeLinePanel::OnMenuInsertSoundEvent )
	EVT_MENU( POPUP_INSERT_SOUND_8_EVENT, cwxEventTimeLinePanel::OnMenuInsertSoundEvent )
	EVT_MENU( POPUP_INSERT_SUBTITLE_EVENT, cwxEventTimeLinePanel::OnMenuInsertSubtitleEvent )
	EVT_MENU( POPUP_REMOVE_EVENT, cwxEventTimeLinePanel::OnMenuRemoveEvent )
	EVT_MENU( POPUP_SNAP_EVENT, cwxEventTimeLinePanel::OnMenuSnapEvent )
	EVT_MENU( POPUP_SELECT_PARTICLE, cwxEventTimeLinePanel::OnMenuSelectParticleEvent )
	EVT_MENU( POPUP_UNSELECT_PARTICLE, cwxEventTimeLinePanel::OnMenuUnSelectParticleEvent )

#ifdef PRE_ADD_FILTEREVENT
	EVT_MENU( POPUP_INSERT_COLORFILTER_EVENT, cwxEventTimeLinePanel::OnMenuInsertColorFilterEvent )
#endif // PRE_ADD_FILTEREVENT

	EVT_TIMELINE_ACTION_SELECT( EVENT_TIMELINE_ID, cwxEventTimeLinePanel::OnTLObjectSelect )
	EVT_TIMELINE_ACTION_MOVE( EVENT_TIMELINE_ID, cwxEventTimeLinePanel::OnTLObjectMove )
	EVT_TIMELINE_ACTION_RESIZE( EVENT_TIMELINE_ID, cwxEventTimeLinePanel::OnTLObjectResize ) 

	EVT_TIMELINE_AXIS_EVENT( EVENT_TIMELINE_ID, cwxEventTimeLinePanel::OnTLAxisEvent )
END_EVENT_TABLE()




cwxEventTimeLinePanel::cwxEventTimeLinePanel( wxWindow* pParent, wxWindowID id /* = -1  */ ) : wxPanel( pParent, id ), 
																							   m_pTimeLine( NULL ),
																							   m_iOriEditMode( CToolData::EM_EDIT ),
																							   m_dwNowTime( 0 )
{
	m_pTimeLine = new cwxTimeLineCtrl( this, EVENT_TIMELINE_ID );
	m_pTopSizer = new wxBoxSizer( wxVERTICAL );
	m_pTopSizer->Add( m_pTimeLine, 1, wxALL|wxALIGN_CENTER|wxEXPAND, 2 );

	this->SetSizer( m_pTopSizer );
	m_pTopSizer->Fit( this );
	m_pTopSizer->SetSizeHints( this );

	// 이벤트 타입별로 문자열 정리해 둔다.
	m_astrEventObjectNameByType.Add( wxT("Camera") );
	m_astrEventObjectNameByType.Add( wxT("Particle") );
	m_astrEventObjectNameByType.Add( wxT("DOF") );
	m_astrEventObjectNameByType.Add( wxT("Fade") );
	m_astrEventObjectNameByType.Add( wxT("Prop") );
	m_astrEventObjectNameByType.Add( wxT("Sound_1") );
	m_astrEventObjectNameByType.Add( wxT("Sound_2") );
	m_astrEventObjectNameByType.Add( wxT("Sound_3") );
	m_astrEventObjectNameByType.Add( wxT("Sound_4") );
	m_astrEventObjectNameByType.Add( wxT("Sound_5") );
	m_astrEventObjectNameByType.Add( wxT("Sound_6") );
	m_astrEventObjectNameByType.Add( wxT("Sound_7") );
	m_astrEventObjectNameByType.Add( wxT("Sound_8") );
	m_astrEventObjectNameByType.Add( wxT("Subtitle") );
	m_astrEventObjectNameByType.Add( wxT("Image") );
#ifdef PRE_ADD_FILTEREVENT
	m_astrEventObjectNameByType.Add( wxT("ColorFilter") );
#endif // PRE_ADD_FILTEREVENT

	assert( m_astrEventObjectNameByType.size() == EventInfo::TYPE_COUNT  && "타입 카운트에 맞게 문자열 배열을 채워주세요~" );

	TOOL_DATA.SetEventTLPanel( this );

	m_pTimeLine->SetViewScale( 6.0f );
}

cwxEventTimeLinePanel::~cwxEventTimeLinePanel(void)
{

}


void cwxEventTimeLinePanel::_PopupContextMenu( int iXPos, int iYPos )
{
	wxMenu Menu;
	int iSelected = m_pTimeLine->GetSelectedObjectID();
	if( wxNOT_FOUND != iSelected )
	{
		int iUseType = m_pTimeLine->GetSelectedObjectUseType();
		switch( iUseType )
		{
			case CToolData::MAX_CAMERA:
				break;

			case CToolData::PARTICLE:
				{
					CRTCutSceneRenderer* pRenderer = TOOL_DATA.GetRenderer();
					
					if( IRenderBase::SO_PARTICLE == pRenderer->GetSelectedObjectType() &&
						pRenderer->IsObjectSelected() ) 
						Menu.Append( POPUP_UNSELECT_PARTICLE, wxT("&UnSelect") );
					else
						Menu.Append( POPUP_SELECT_PARTICLE, wxT("&Select") );
				}
				break;

			//case CToolData::SOUND:

			//	break;

			case CToolData::FADE:
				break;

			case CToolData::DOF:
				break;

			case CToolData::PROP:
				break;

			case CToolData::SOUND_1:
			case CToolData::SOUND_2:
			case CToolData::SOUND_3:
			case CToolData::SOUND_4:
			case CToolData::SOUND_5:
			case CToolData::SOUND_6:
			case CToolData::SOUND_7:
			case CToolData::SOUND_8:
				break;

			case CToolData::SUBTITLE:
				break;

//PRE_ADD_FILTEREVENT
			case CToolData::COLORFILTER:
				break;
//
		}

		Menu.Append( POPUP_REMOVE_EVENT, wxT("&Remove") );
	}
	else
	{
		Menu.Append( POPUP_INSERT_CAM_EVENT, wxT("Insert &Camera Event") );
		Menu.Append( POPUP_INSERT_PROP_EVENT, wxT("Insert P&rop Event") );
		Menu.Append( POPUP_INSERT_PARTICLE_EVENT, wxT("Insert &Particle Event") );
		//Menu.Append( POPUP_INSERT_SOUND_EVENT, wxT("Insert &Sound Event") );
		Menu.Append( POPUP_INSERT_DOF, wxT("Insert &DOF Event") );
		Menu.Append( POPUP_INSERT_FADE_EVENT, wxT("Insert &Fade Event") );
		Menu.Append( POPUP_INSERT_SOUND_1_EVENT, wxT("Insert &Sound_1 Event") );
		Menu.Append( POPUP_INSERT_SOUND_2_EVENT, wxT("Insert &Sound_2 Event") );
		Menu.Append( POPUP_INSERT_SOUND_3_EVENT, wxT("Insert &Sound_3 Event") );
		Menu.Append( POPUP_INSERT_SOUND_4_EVENT, wxT("Insert &Sound_4 Event") );
		Menu.Append( POPUP_INSERT_SOUND_5_EVENT, wxT("Insert &Sound_5 Event") );
		Menu.Append( POPUP_INSERT_SOUND_6_EVENT, wxT("Insert &Sound_6 Event") );
		Menu.Append( POPUP_INSERT_SOUND_7_EVENT, wxT("Insert &Sound_7 Event") );
		Menu.Append( POPUP_INSERT_SOUND_8_EVENT, wxT("Insert &Sound_8 Event") );
		Menu.Append( POPUP_INSERT_SUBTITLE_EVENT, wxT("Insert S&ubtitle Event") );
//#ifdef PRE_ADD_FILTEREVENT
		Menu.Append( POPUP_INSERT_COLORFILTER_EVENT, wxT("Insert S&ColorFilter Event") );
//#endif // PRE_ADD_FILTEREVENT
	}

	Menu.AppendSeparator();
	Menu.Append( POPUP_SNAP_EVENT, wxT("&Snap") );

	PopupMenu( &Menu, iXPos+m_pTimeLine->GetPosition().x, iYPos+m_pTimeLine->GetPosition().y );
}


bool cwxEventTimeLinePanel::Enable( bool bEnable/* = true*/ )
{
	m_pTimeLine->Enable( bEnable );

	return true;
}


void cwxEventTimeLinePanel::OnTLObjectSelect( cwxTLObjectSelectEvent& TLSelectEvent )
{
	TOOL_DATA.SetSelectedObjectID( TLSelectEvent.GetObjectID() );
	TOOL_DATA.SetSelectedObjectUseType( TLSelectEvent.GetObjectUseType() );

	// 속성 패널에게 이벤트 속성을 알려줌
	cwxPropertyPanel* pPropertyPanel = TOOL_DATA.GetPropertyPanel();
	pPropertyPanel->OnTLActionSelect();

	CRTCutSceneRenderer* pRenderer = TOOL_DATA.GetRenderer();
	pRenderer->OnSelectEventUnit( TLSelectEvent.GetObjectUseType(), TLSelectEvent.GetObjectID() );
}


void cwxEventTimeLinePanel::OnTLObjectMove( cwxTLObjectMoveEvent& TLMoveEvent )
{
	int iSelectedObjectID = TLMoveEvent.GetObjectID();
	int iObjectUseType = TOOL_DATA.GetSelectedObjectUseType();

	switch( iObjectUseType )
	{
		case CToolData::MAX_CAMERA:
		case CToolData::PARTICLE:
		case CToolData::DOF:
		case CToolData::FADE:
		case CToolData::PROP:
		case CToolData::SOUND_1:
		case CToolData::SOUND_2:
		case CToolData::SOUND_3:
		case CToolData::SOUND_4:
		case CToolData::SOUND_5:
		case CToolData::SOUND_6:
		case CToolData::SOUND_7:
		case CToolData::SOUND_8:
		case CToolData::SUBTITLE:

//PRE_ADD_FILTEREVENT
		case CToolData::COLORFILTER:
//
			{
				const EventInfo* pEventInfo = TOOL_DATA.GetEventInfoByID( iSelectedObjectID );

				EventInfo* pNewEventInfo = pEventInfo->clone();
				pNewEventInfo->fStartTime = TLMoveEvent.GetActionStartTime();

				CEventPropChange EventPropChange( &TOOL_DATA, pNewEventInfo );
				TOOL_DATA.RunCommand( &EventPropChange );

				delete pNewEventInfo;
			}
			break;
	}
}


void cwxEventTimeLinePanel::OnTLObjectResize( cwxTLObjectResizeEvent& TLResizeEvent )
{
	int iSelectedObjectID = TLResizeEvent.GetObjectID();// TOOL_DATA.GetSelectedActionID();
	int iObjectUseType = TOOL_DATA.GetSelectedObjectUseType();

	switch( iObjectUseType )
	{
		case CToolData::MAX_CAMERA:
		case CToolData::PARTICLE:
		//case CToolData::SOUND:
		case CToolData::DOF:
		case CToolData::FADE:
		case CToolData::PROP:
		case CToolData::SOUND_1:
		case CToolData::SOUND_2:
		case CToolData::SOUND_3:
		case CToolData::SOUND_4:
		case CToolData::SOUND_5:
		case CToolData::SOUND_6:
		case CToolData::SOUND_7:
		case CToolData::SOUND_8:

//PRE_ADD_FILTEREVENT
		case CToolData::COLORFILTER:
//
			{
				const EventInfo* pEventInfo = TOOL_DATA.GetEventInfoByID( iSelectedObjectID );

				EventInfo* pNewEventInfo = pEventInfo->clone();
				pNewEventInfo->fStartTime = TLResizeEvent.GetActionStartInUnit();
				pNewEventInfo->fTimeLength = TLResizeEvent.GetNewWidthInUnit();

				CEventPropChange EventPropChange( &TOOL_DATA, pNewEventInfo );
				TOOL_DATA.RunCommand( &EventPropChange );

				delete pNewEventInfo;
			}
			break;

		//case CToolData::UT_PARTICLE:
		//	break;

		//case CToolData::UT_SOUND:
		//	break;
	}
}


// 액시스 이벤트는 나중에.. 아무래도 액션 패널의 있는걸로 그냥 할 거 같은데..
void cwxEventTimeLinePanel::OnTLAxisEvent( cwxTLAxisEvent& TLAxisEvent )
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
			pRenderer->SetMode( IRenderBase::EDITOR_MODE );
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


void cwxEventTimeLinePanel::OnMouseEvent( wxMouseEvent& MouseEvent )
{
	if( MouseEvent.RightUp() )
		_PopupContextMenu( MouseEvent.GetX(), MouseEvent.GetY() );
}


EventInfo* cwxEventTimeLinePanel::_OnInsertEvent( const char* pEventName, int iToolDataType )
{
	int iEventType = -1;
	EventInfo* pEventInfo = NULL;
	switch( iToolDataType )
	{
		case CToolData::MAX_CAMERA:
			{
				pEventInfo = new CamEventInfo;
				CamEventInfo* pCamEventInfo = static_cast<CamEventInfo*>(pEventInfo);
				iEventType = EventInfo::CAMERA;

				// 현재 편집중인 액터들을 모두 넣어둠
				wxCSConv MBConv( wxFONTENCODING_CP949 );
				int iNumActors = TOOL_DATA.GetNumActors();
				char acBuf[ 256 ];
				for( int iActor = 0; iActor < iNumActors; ++iActor )
				{
					wxString strActorName;
					TOOL_DATA.GetActorNameByIndex( iActor, strActorName );
					ZeroMemory( acBuf, sizeof(acBuf) );
					MBConv.WC2MB( acBuf, strActorName.c_str(), 256 );
					const char* pActorName = acBuf;
					pCamEventInfo->mapActorsShowHide.insert( make_pair(string(pActorName), true) );		// 디폴트값은 true 임
				}
			}
			break;

		case CToolData::PARTICLE:
			pEventInfo = new ParticleEventInfo;
			iEventType = EventInfo::PARTICLE;
			break;

		//case CToolData::SOUND:
		//	pEventInfo = new SoundEventInfo;
		//	iEventType = EventInfo::SOUND;
		//	break;

		case CToolData::DOF:
			pEventInfo = new DofEventInfo;
			iEventType = EventInfo::DOF;
			break;

		case CToolData::FADE:
			pEventInfo = new FadeEventInfo;
			iEventType = EventInfo::FADE;
			break;

		case CToolData::PROP:
			pEventInfo = new PropEventInfo;
			iEventType = EventInfo::PROP;
			break;

		case CToolData::SOUND_1:
		case CToolData::SOUND_2:
		case CToolData::SOUND_3:
		case CToolData::SOUND_4:
		case CToolData::SOUND_5:
		case CToolData::SOUND_6:
		case CToolData::SOUND_7:
		case CToolData::SOUND_8:
			pEventInfo = new SoundEventInfo;
			iEventType = EventInfo::SOUND_1 + (iToolDataType - CToolData::SOUND_1);
			break;

		case CToolData::SUBTITLE:
			pEventInfo = new SubtitleEventInfo;
			iEventType = EventInfo::SUBTITLE;
			break;

//#ifdef PRE_ADD_FILTEREVENT
		case CToolData::COLORFILTER:
			pEventInfo = new ColorFilterEventInfo;
			iEventType = EventInfo::COLORFILTER;
			break;
//#endif PRE_ADD_FILTEREVENT

	}

	pEventInfo->strEventName.assign( pEventName );
	pEventInfo->iID = TOOL_DATA.GenerateID();
	pEventInfo->fUnit = 10.0f;

	float fNextStartTime = 0.0f;
	int iLastEventIndex = TOOL_DATA.GetThisTypesEventNum( iEventType ) - 1;
	if( iLastEventIndex >= 0 )
	{
		const EventInfo* pLastEvent = TOOL_DATA.GetEventInfoByIndex( iEventType, iLastEventIndex );
		fNextStartTime = pLastEvent->fStartTime + pLastEvent->fTimeLength;
	}

	pEventInfo->fStartTime = fNextStartTime;

	// 현재 스크롤 위치를 저장해 둔다.
	m_iXScrollPos = m_pTimeLine->GetScrollPos( wxHORIZONTAL );
	m_iYScrollPos = m_pTimeLine->GetScrollPos( wxVERTICAL );

	return pEventInfo;
}



void cwxEventTimeLinePanel::OnMenuInsertCamEvent( wxCommandEvent& MenuEvent )
{
	EventInfo* pEventInfo = _OnInsertEvent( "Camera", CToolData::MAX_CAMERA );	

	CEventInsertCmd InsertEventCmd( &TOOL_DATA, pEventInfo ); 
	TOOL_DATA.RunCommand( &InsertEventCmd );

	SAFE_DELETE( pEventInfo );

	//EventInfo* pEventInfo = NULL;

	//// TODO: 일단.. 임시로 카메라 파일 확장자..
	//pEventInfo = new CamEventInfo;
	//CamEventInfo* pCamEventInfo = static_cast<CamEventInfo*>(pEventInfo);

	//pEventInfo->strEventName.assign( "Camera" );
	//pEventInfo->iID = TOOL_DATA.GenerateID();
	//pEventInfo->fUnit = 10.0f;					// 이벤트는 10ms 단위

	//float fNextStartTime = 0.0f;
	//int iLastCamEventIndex = TOOL_DATA.GetThisTypesEventNum( EventInfo::CAMERA ) - 1;
	//if( iLastCamEventIndex >= 0 )
	//{
	//	const EventInfo* pLastCamEvent = TOOL_DATA.GetEventInfoByIndex( EventInfo::CAMERA, iLastCamEventIndex );
	//	fNextStartTime = pLastCamEvent->fStartTime + pLastCamEvent->fTimeLength;
	//}

	//pEventInfo->fStartTime = fNextStartTime;

	//CEventInsertCmd InsertEventCmd( &TOOL_DATA, pEventInfo ); 
	//TOOL_DATA.RunCommand( &InsertEventCmd );

	//SAFE_DELETE( pEventInfo );
}





void cwxEventTimeLinePanel::OnMenuInsertParticleEvent( wxCommandEvent& MenuEvent )
{
	EventInfo* pEventInfo = _OnInsertEvent( "Particle", CToolData::PARTICLE );

	CRTCutSceneRenderer* pRenderer = TOOL_DATA.GetRenderer();
	pRenderer->GetHeightPos( static_cast<ParticleEventInfo*>(pEventInfo)->vPos );

	CEventInsertCmd InsertEventCmd( &TOOL_DATA, pEventInfo );
	TOOL_DATA.RunCommand( &InsertEventCmd );

	SAFE_DELETE( pEventInfo );

	//EventInfo* pEventInfo = new ParticleEventInfo;
	//
	//pEventInfo->strEventName.assign( "Particle" );
	//pEventInfo->iID = TOOL_DATA.GenerateID();
	//pEventInfo->fUnit = 10.0f;

	//float fNextStartTime = 0.0f;
	//int iLastParticleEventIndex = TOOL_DATA.GetThisTypesEventNum( EventInfo::PARTICLE ) - 1;
	//if( iLastParticleEventIndex >= 0 )
	//{
	//	const EventInfo* pLastParticleEvent = TOOL_DATA.GetEventInfoByIndex( EventInfo::PARTICLE, iLastParticleEventIndex );
	//	fNextStartTime = pLastParticleEvent->fStartTime + pLastParticleEvent->fTimeLength;
	//}

	//pEventInfo->fStartTime = fNextStartTime;

	//CRTCutSceneRenderer* pRenderer = TOOL_DATA.GetRenderer();
	//pRenderer->GetHeightPos( static_cast<ParticleEventInfo*>(pEventInfo)->vPos );

	//CEventInsertCmd InsertEventCmd( &TOOL_DATA, pEventInfo );
	//TOOL_DATA.RunCommand( &InsertEventCmd );

	//SAFE_DELETE( pEventInfo );
}



void cwxEventTimeLinePanel::OnMenuInsertDOFEvent( wxCommandEvent& MenuEvent )
{
	EventInfo* pEventInfo = _OnInsertEvent( "DOF", CToolData::DOF );

	CEventInsertCmd InsertEventCmd( &TOOL_DATA, pEventInfo );
	TOOL_DATA.RunCommand( &InsertEventCmd );

	SAFE_DELETE( pEventInfo );
}




void cwxEventTimeLinePanel::OnMenuInsertSoundEvent( wxCommandEvent& MenuEvent )
{
	int iSoundIndex = MenuEvent.GetId() - POPUP_INSERT_SOUND_1_EVENT;
	char strBuf[ 256 ];
	ZeroMemory( strBuf, sizeof(strBuf) );
	sprintf_s( strBuf, "Sound_%d", iSoundIndex+1 );
	EventInfo* pEventInfo = _OnInsertEvent( strBuf, CToolData::SOUND_1+iSoundIndex );
	pEventInfo->iType = EventInfo::SOUND_1 + iSoundIndex;

	CEventInsertCmd InsertEventCmd( &TOOL_DATA, pEventInfo );
	TOOL_DATA.RunCommand( &InsertEventCmd );

	SAFE_DELETE( pEventInfo );

	//EventInfo* pEventInfo = new SoundEventInfo;

	//pEventInfo->strEventName.assign( "Sound" );
	//pEventInfo->iID = TOOL_DATA.GenerateID();
	//pEventInfo->fUnit = 10.0f;

	//float fNextStartTime = 0.0f;
	//int iLastSoundEventIndex = TOOL_DATA.GetThisTypesEventNum( EventInfo::SOUND ) - 1;
	//if( iLastSoundEventIndex >= 0 )
	//{
	//	const EventInfo* pLastSoundEvent = TOOL_DATA.GetEventInfoByIndex( EventInfo::SOUND, iLastSoundEventIndex );
	//	fNextStartTime = pLastSoundEvent->fStartTime + pLastSoundEvent->fTimeLength;
	//}

	//pEventInfo->fStartTime = fNextStartTime;

	//CEventInsertCmd InsertEventCmd( &TOOL_DATA, pEventInfo );
	//TOOL_DATA.RunCommand( &InsertEventCmd );

	//SAFE_DELETE( pEventInfo );
}



void cwxEventTimeLinePanel::OnMenuInsertFadeEvent( wxCommandEvent& MenuEvent )
{
	EventInfo* pEventInfo = _OnInsertEvent( "Fade", CToolData::FADE );

	CEventInsertCmd InsertEventCmd( &TOOL_DATA, pEventInfo );
	TOOL_DATA.RunCommand( &InsertEventCmd );

	SAFE_DELETE( pEventInfo );

	//EventInfo* pEventInfo = new FadeEventInfo;

	//pEventInfo->strEventName.assign( "Fade" );
	//pEventInfo->iID = TOOL_DATA.GenerateID();
	//pEventInfo->fUnit = 10.0f;
	//pEventInfo->fTimeLength = 100.0f;

	//float fNextStartTime = 0.0f;
	//int iLastFadeEventIndex = TOOL_DATA.GetThisTypesEventNum( EventInfo::FADE ) - 1;
	//if( iLastFadeEventIndex >= 0 )
	//{
	//	const EventInfo* pLastFadeEvent = TOOL_DATA.GetEventInfoByIndex( EventInfo::FADE, iLastFadeEventIndex );
	//	fNextStartTime = pLastFadeEvent->fStartTime + pLastFadeEvent->fTimeLength;
	//}

	//pEventInfo->fStartTime = fNextStartTime;

	//CEventInsertCmd InsertEventCmd( &TOOL_DATA, pEventInfo );
	//TOOL_DATA.RunCommand( &InsertEventCmd );

	//SAFE_DELETE( pEventInfo );
}




void cwxEventTimeLinePanel::OnMenuInsertPropEvent( wxCommandEvent& MenuEvent )
{
	EventInfo* pEventInfo = _OnInsertEvent( "Prop", CToolData::PROP );

	CEventInsertCmd InsertEventCmd( &TOOL_DATA, pEventInfo );
	TOOL_DATA.RunCommand( &InsertEventCmd );

	SAFE_DELETE( pEventInfo );

	//EventInfo* pEventInfo = new PropEventInfo;

	//pEventInfo->strEventName.assign( "Prop" );
	//pEventInfo->iID = TOOL_DATA.GenerateID();
	//pEventInfo->fUnit = 10.0f;
	//pEventInfo->fTimeLength = 100.0f;

	//float fNextStartTime = 0.0f;
	//int iLastPropEventIndex = TOOL_DATA.GetThisTypesEventNum( EventInfo::PROP ) - 1;
	//if( iLastPropEventIndex >= 0 )
	//{
	//	const EventInfo* pLastPropEvent = TOOL_DATA.GetEventInfoByIndex( EventInfo::PROP, iLastPropEventIndex );
	//	fNextStartTime = pLastPropEvent->fStartTime + pLastPropEvent->fTimeLength;
	//}

	//pEventInfo->fStartTime = fNextStartTime;

	//CEventInsertCmd InsertEventCmd( &TOOL_DATA, pEventInfo );
	//TOOL_DATA.RunCommand( &InsertEventCmd );

	//SAFE_DELETE( pEventInfo );
}



void cwxEventTimeLinePanel::OnMenuInsertSubtitleEvent( wxCommandEvent& MenuEvent )
{
	EventInfo* pEventInfo = _OnInsertEvent( "Subtitle", CToolData::SUBTITLE );

	CEventInsertCmd InsertEventCmd( &TOOL_DATA, pEventInfo );
	TOOL_DATA.RunCommand( &InsertEventCmd );
	
	SAFE_DELETE( pEventInfo );
}



void cwxEventTimeLinePanel::OnMenuRemoveEvent( wxCommandEvent& MenuEvent )
{
	int iSelectedID = m_pTimeLine->GetSelectedObjectID();
	int iUseType = m_pTimeLine->GetSelectedObjectUseType();

	switch( iUseType )
	{
		case CToolData::PARTICLE:
		//case CToolData::SOUND:
		case CToolData::DOF:
		case CToolData::MAX_CAMERA:
		case CToolData::FADE:
		case CToolData::PROP:
		case CToolData::SOUND_1:
		case CToolData::SOUND_2:
		case CToolData::SOUND_3:
		case CToolData::SOUND_4:
		case CToolData::SOUND_5:
		case CToolData::SOUND_6:
		case CToolData::SOUND_7:
		case CToolData::SOUND_8:
		case CToolData::SUBTITLE:

// PRE_ADD_FILTEREVENT
		case CToolData::COLORFILTER:
//

			{
				CEventRemoveCmd RemoveEventCmd( &TOOL_DATA, iSelectedID );
				TOOL_DATA.RunCommand( &RemoveEventCmd );
			}
			break;
	}
}



void cwxEventTimeLinePanel::_SnapEvent( int iEventType )
{
	int iSelectedID = m_pTimeLine->GetSelectedObjectID();

	const EventInfo* pPrevEventInfo = NULL;
	int iNumEvent = TOOL_DATA.GetThisTypesEventNum( iEventType );
	for( int iEvent = 0; iEvent <iNumEvent; ++iEvent )
	{
		const EventInfo* pEventInfo = TOOL_DATA.GetEventInfoByIndex( iEventType, iEvent );
		if( pEventInfo->iID == iSelectedID )
		{
			if( iEvent > 0 )
			{
				pPrevEventInfo = TOOL_DATA.GetEventInfoByIndex( iEventType, iEvent-1 );
				break;
			}
		}
	}

	if( pPrevEventInfo )
	{
		const EventInfo* pNowEventInfo = TOOL_DATA.GetEventInfoByID( iSelectedID );
		EventInfo* pNewEventInfo = pNowEventInfo->clone();
		pNewEventInfo->fStartTime = pPrevEventInfo->fStartTime + pPrevEventInfo->fTimeLength;

		CEventPropChange EventPropChange( &TOOL_DATA, pNewEventInfo );
		TOOL_DATA.RunCommand( &EventPropChange );
	}
}



void cwxEventTimeLinePanel::OnMenuSnapEvent( wxCommandEvent& MenuEvent )
{
	int iSelectedUseType = m_pTimeLine->GetSelectedObjectUseType();

	// 바로 이전의 이벤트 정보를 찾는다.
	//const EventInfo* pEventInfo = TOOL_DATA.GetEventInfoByID( iSelectedID );

	//wxCSConv MBConv( wxFONTENCODING_CP949 );
	//wxChar wcaBuf[ 256 ];
	//ZeroMemory( wcaBuf, sizeof(wcaBuf) );
	//MBConv.MB2WC( wcaBuf, string(pEventInfo->strActorName + "_ACTION").c_str(), 256 );
	
	switch( iSelectedUseType )
	{
		case CToolData::MAX_CAMERA:
			_SnapEvent( EventInfo::CAMERA );
			break;

		case CToolData::PARTICLE:
			_SnapEvent( EventInfo::PARTICLE );
			break;

		//case CToolData::SOUND:
		//	_SnapEvent( EventInfo::SOUND );
		//	break;

		case CToolData::DOF:
			_SnapEvent( EventInfo::DOF );
			break;

		case CToolData::FADE:
			_SnapEvent( EventInfo::FADE );
			break;

		case CToolData::PROP:
			_SnapEvent( EventInfo::PROP );
			break;

		case CToolData::SOUND_1:
		case CToolData::SOUND_2:
		case CToolData::SOUND_3:
		case CToolData::SOUND_4:
		case CToolData::SOUND_5:
		case CToolData::SOUND_6:
		case CToolData::SOUND_7:
		case CToolData::SOUND_8:
			_SnapEvent( EventInfo::SOUND_1 + (iSelectedUseType-CToolData::SOUND_1) );
			break;

		case CToolData::SUBTITLE:
			_SnapEvent( EventInfo::SUBTITLE );
			break;

// PRE_ADD_FILTEREVENT
		case CToolData::COLORFILTER:
			_SnapEvent( EventInfo::COLORFILTER );
			break;
//		

	}
}



void cwxEventTimeLinePanel::OnMenuSelectParticleEvent( wxCommandEvent& MenuEvent )
{
	// 렌더러에게 파티클 셀렉팅 해준다.
	CRTCutSceneRenderer* pRenderer = TOOL_DATA.GetRenderer();
	//pRenderer->SelectParticle( m_pTimeLine->GetSelectedObjectID() );
	pRenderer->OnSelectEventUnit( CToolData::PARTICLE, m_pTimeLine->GetSelectedObjectID() );
}



void cwxEventTimeLinePanel::OnMenuUnSelectParticleEvent( wxCommandEvent& MenuEvent )
{
	CRTCutSceneRenderer* pRenderer = TOOL_DATA.GetRenderer();
	pRenderer->UnSelectObject();
}


#ifdef PRE_ADD_FILTEREVENT
void cwxEventTimeLinePanel::OnMenuInsertColorFilterEvent( wxCommandEvent& MenuEvent )
{
	EventInfo* pEventInfo = _OnInsertEvent( "ColorFilter", CToolData::COLORFILTER );

	CEventInsertCmd InsertEventCmd( &TOOL_DATA, pEventInfo );
	TOOL_DATA.RunCommand( &InsertEventCmd );

	SAFE_DELETE( pEventInfo );

}
#endif // PRE_ADD_FILTEREVENT



// 데이터를 그대로 반영
void cwxEventTimeLinePanel::ReflectData( void )
{
	// 클리어 전에 미리 선택되었던 액션 아이디를 받아둠
	int iSelectedActionID = m_pTimeLine->GetSelectedObjectID();

	m_pTimeLine->Clear();

	m_pTimeLine->SetStyle( TL_SHOW_OBJECT_NAME );

	wxCSConv MBConv( wxFONTENCODING_CP949 );
	wxChar wcaBuf[ 256 ];
	ZeroMemory( wcaBuf, sizeof(wcaBuf) );

	for( int iEventType = 0; iEventType < EventInfo::TYPE_COUNT; ++iEventType )
	{
		int iNumEvent = TOOL_DATA.GetThisTypesEventNum( iEventType );

		wxString& strEventObject = m_astrEventObjectNameByType[ iEventType ];
		m_pTimeLine->AddItem( strEventObject, wxColour(wxT("RED")) );

		for( int iEvent = 0; iEvent < iNumEvent; ++iEvent )
		{
			const EventInfo* pEventInfo = TOOL_DATA.GetEventInfoByIndex( iEventType, iEvent );

			MBConv.MB2WC( wcaBuf, pEventInfo->strEventName.c_str(), 256 );

			switch( pEventInfo->iType )
			{
				case EventInfo::CAMERA:
					m_pTimeLine->AddObject( strEventObject, wcaBuf, pEventInfo->iID, 
											 CToolData::MAX_CAMERA, true, pEventInfo->fStartTime, 
											 pEventInfo->fTimeLength );
					break;

				case EventInfo::PARTICLE:
					m_pTimeLine->AddObject( strEventObject, wcaBuf, pEventInfo->iID, 
											 CToolData::PARTICLE, true, pEventInfo->fStartTime, 
											 pEventInfo->fTimeLength );
					break;

				case EventInfo::DOF:
					m_pTimeLine->AddObject( strEventObject, wcaBuf, pEventInfo->iID,
											 CToolData::DOF, true, pEventInfo->fStartTime,
											 pEventInfo->fTimeLength );
					break;

				case EventInfo::FADE:
					m_pTimeLine->AddObject( strEventObject, wcaBuf, pEventInfo->iID,
											 CToolData::FADE, true, pEventInfo->fStartTime,
											 pEventInfo->fTimeLength );
					break;

				case EventInfo::PROP:
					m_pTimeLine->AddObject( strEventObject, wcaBuf, pEventInfo->iID,
											 CToolData::PROP, true, pEventInfo->fStartTime,
											 pEventInfo->fTimeLength );
					break;

				case EventInfo::SOUND_1:
				case EventInfo::SOUND_2:
				case EventInfo::SOUND_3:
				case EventInfo::SOUND_4:
				case EventInfo::SOUND_5:
				case EventInfo::SOUND_6:
				case EventInfo::SOUND_7:
				case EventInfo::SOUND_8:
					m_pTimeLine->AddObject( strEventObject, wcaBuf, pEventInfo->iID, 
											 CToolData::SOUND_1 + (pEventInfo->iType-EventInfo::SOUND_1), true, pEventInfo->fStartTime, 
											 pEventInfo->fTimeLength );
					break;

				case EventInfo::SUBTITLE:
					m_pTimeLine->AddObject( strEventObject, wcaBuf, pEventInfo->iID,
											 CToolData::SUBTITLE, true, pEventInfo->fStartTime, pEventInfo->fTimeLength );
					break;

#ifdef PRE_ADD_FILTEREVENT
				case EventInfo::COLORFILTER:
					m_pTimeLine->AddObject( strEventObject, wcaBuf, pEventInfo->iID,
						CToolData::COLORFILTER, true, pEventInfo->fStartTime, pEventInfo->fTimeLength );
					break;
#endif // PRE_ADD_FILTEREVENT

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



void cwxEventTimeLinePanel::ModifyObject( ICommand* pCommand )
{
	assert( pCommand->GetTypeID() == CMD_EVENT_PROP_CHANGE );

	wxCSConv MBConv( wxFONTENCODING_CP949 );
	switch( pCommand->GetTypeID() )
	{
		case CMD_EVENT_PROP_CHANGE:
			{
				CEventPropChange* pEventPropChange = static_cast<CEventPropChange*>(pCommand);
				const EventInfo* pEventInfo = NULL;
				if(  pEventPropChange->m_bUndo )
					pEventInfo = pEventPropChange->m_pPrevEventInfo;
				else
					pEventInfo = pEventPropChange->m_pNewEventInfo;

				wxChar wcaEventName[ 256 ];
				ZeroMemory( wcaEventName, sizeof(wcaEventName) );
				MBConv.MB2WC( wcaEventName, pEventInfo->strEventName.c_str(), 256 );

				switch( pEventInfo->iType )
				{
					case EventInfo::CAMERA:
						m_pTimeLine->ModifyObject( m_astrEventObjectNameByType[pEventInfo->iType].c_str(), wcaEventName, pEventInfo->iID, 
													CToolData::MAX_CAMERA, true, pEventInfo->fStartTime, pEventInfo->fTimeLength );
						break;

					case EventInfo::PARTICLE:
						m_pTimeLine->ModifyObject( m_astrEventObjectNameByType[pEventInfo->iType].c_str(), wcaEventName, pEventInfo->iID, 
													CToolData::PARTICLE, true, pEventInfo->fStartTime, pEventInfo->fTimeLength );
						break;

					case EventInfo::FADE:
						m_pTimeLine->ModifyObject( m_astrEventObjectNameByType[pEventInfo->iType].c_str(), wcaEventName, pEventInfo->iID,
													CToolData::FADE, true, pEventInfo->fStartTime, pEventInfo->fTimeLength );
						break;

					case EventInfo::DOF:
						m_pTimeLine->ModifyObject( m_astrEventObjectNameByType[pEventInfo->iType].c_str(), wcaEventName, pEventInfo->iID,
													CToolData::DOF, true, pEventInfo->fStartTime, pEventInfo->fTimeLength );
						break;

					case EventInfo::PROP:
						m_pTimeLine->ModifyObject( m_astrEventObjectNameByType[pEventInfo->iType].c_str(), wcaEventName, pEventInfo->iID,
													CToolData::PROP, true, pEventInfo->fStartTime, pEventInfo->fTimeLength );
						break;

					case EventInfo::SOUND_1:
					case EventInfo::SOUND_2:
					case EventInfo::SOUND_3:
					case EventInfo::SOUND_4:
					case EventInfo::SOUND_5:
					case EventInfo::SOUND_6:
					case EventInfo::SOUND_7:
					case EventInfo::SOUND_8:
						m_pTimeLine->ModifyObject( m_astrEventObjectNameByType[pEventInfo->iType].c_str(), wcaEventName, pEventInfo->iID,
													CToolData::SOUND_1 + (pEventInfo->iType-EventInfo::SOUND_1), true, 
													pEventInfo->fStartTime, pEventInfo->fTimeLength );
						break;

					case EventInfo::SUBTITLE:
						m_pTimeLine->ModifyObject( m_astrEventObjectNameByType[pEventInfo->iType].c_str(), wcaEventName, pEventInfo->iID,
													CToolData::SUBTITLE, true, pEventInfo->fStartTime, pEventInfo->fTimeLength );
						break;

// PRE_ADD_FILTEREVENT
					case EventInfo::COLORFILTER:
						m_pTimeLine->ModifyObject( m_astrEventObjectNameByType[pEventInfo->iType].c_str(), wcaEventName, pEventInfo->iID,
							CToolData::COLORFILTER, true, pEventInfo->fStartTime, pEventInfo->fTimeLength );
						break;
//

				}
			}
			break;
	}

	int iHorizontal = m_pTimeLine->GetScrollPos( wxHORIZONTAL );
	int iVertical = m_pTimeLine->GetScrollPos( wxVERTICAL );
	m_pTimeLine->SetScrollPos( wxHORIZONTAL, iHorizontal, false );
	m_pTimeLine->SetScrollPos( wxVERTICAL, iVertical, false );
	m_pTimeLine->Refresh( true, &m_pTimeLine->GetClientRect() );
}



// 시간을 정하면 알아서 패널 내부에서 업데이트
void cwxEventTimeLinePanel::SetTime( float fTime )
{	
	// 이건 milliseconds 만 적용됨
	m_pTimeLine->UpdateTimeBar( fTime );			// 단위가 10... 이러면 안되는데. -_-

	int iHorizontal = m_pTimeLine->GetScrollPos( wxHORIZONTAL );
	int iVertical = m_pTimeLine->GetScrollPos( wxVERTICAL );
	m_pTimeLine->SetScrollPos( wxHORIZONTAL, iHorizontal, false );
	m_pTimeLine->SetScrollPos( wxVERTICAL, iVertical, false );
	m_pTimeLine->Refresh( true, &m_pTimeLine->GetClientRect() );
}



DWORD cwxEventTimeLinePanel::GetNowTime( void )
{
	DWORD dwNowTime = DWORD(m_pTimeLine->GetNowTime() * 1000.0f);
	
	return dwNowTime;
}


// 자동으로 찾아서 셀렉트 시킴
void cwxEventTimeLinePanel::SelectObject( int iObjectID, DWORD dwStartTime )
{
	m_dwNowTime = (dwStartTime / 1000) * 1000;

	// 우선 해당 act id 가 있는 초 단위 시간을 찾아서 셋팅해 줌
	ReflectData();
	m_pTimeLine->SelectObject( iObjectID );
}

void cwxEventTimeLinePanel::OnMouseEventFromFrame( wxMouseEvent& MouseEvent )
{
	m_pTimeLine->OnMouseEvent( MouseEvent );
}