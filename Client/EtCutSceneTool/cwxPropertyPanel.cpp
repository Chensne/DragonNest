#include "StdAfx.h"
#include <wx/wx.h>
#include "cwxPropertyPanel.h"
#include "ActorProperty.h"
#include "ActionProperty.h"
#include "KeyProperty.h"
#include "CameraEventProperty.h"
#include "ParticleEventProperty.h"
#include "SoundEventProperty.h"
#include "FadeEventProperty.h"
#include "SceneProperty.h"
#include "ActionPropProperty.h"
#include "DofEventProperty.h"
#include "SubtitleEventProperty.h"
#include "ImageEventProperty.h"
//#include "MoveProperty.h"
//#include "RotateProperty.h"

//#ifdef PRE_ADD_FILTEREVENT
#include "ColorFilterEventProperty.h"
//#endif PRE_ADD_FILTEREVENT

#include "ToolData.h"
#include "CommandSet.h"
#include "cwxRegisteredResPanel.h"
#include "cwxTimeLinePanel.h"
#include "VectorProperty.h"



BEGIN_EVENT_TABLE( cwxPropertyPanel, wxPanel )
	//EVT_UPDATE_UI_RANGE( ACTOR_PROP_ID, PROP_COUNT-1, cwxPropertyPanel::OnUpdateProperty )

	EVT_PG_CHANGED( ACTOR_PROP_ID, cwxPropertyPanel::OnPropertyChanged )
	EVT_PG_CHANGED( ACTION_PROP_ID, cwxPropertyPanel::OnPropertyChanged )
	EVT_PG_CHANGED( KEY_PROP_ID, cwxPropertyPanel::OnPropertyChanged )
	//EVT_PG_CHANGED( MOVE_PROP_ID, cwxPropertyPanel::OnPropertyChanged )
	//EVT_PG_CHANGED( ROTATE_PROP_ID, cwxPropertyPanel::OnPropertyChanged )
	EVT_PG_CHANGED( CAMERA_EVENT_PROP_ID, cwxPropertyPanel::OnPropertyChanged )
	EVT_PG_CHANGED( PARTICLE_EVENT_PROP_ID, cwxPropertyPanel::OnPropertyChanged )
	EVT_PG_CHANGED( SOUND_EVENT_PROP_ID, cwxPropertyPanel::OnPropertyChanged )
	EVT_PG_CHANGED( FADE_EVENT_PROP_ID, cwxPropertyPanel::OnPropertyChanged )
	EVT_PG_CHANGED( SCENE_PROP_ID, cwxPropertyPanel::OnPropertyChanged )
	EVT_PG_CHANGED( PROP_EVENT_ID, cwxPropertyPanel::OnPropertyChanged )
	EVT_PG_CHANGED( DOF_EVENT_PROP_ID, cwxPropertyPanel::OnPropertyChanged )
	EVT_PG_CHANGED( SUBTITLE_EVENT_PROP_ID, cwxPropertyPanel::OnPropertyChanged )
	EVT_PG_CHANGED( IMAGE_EVENT_PROP_ID, cwxPropertyPanel::OnPropertyChanged )
//#ifdef PRE_ADD_FILTEREVENT
	EVT_PG_CHANGED( COLORFILTER_EVENT_PROP_ID, cwxPropertyPanel::OnPropertyChanged )
//#endif // PRE_ADD_FILTEREVENT
END_EVENT_TABLE()


// 우선 테스트로 몇 가지만 붙여보자
cwxPropertyPanel::cwxPropertyPanel( wxWindow* pParent, wxWindowID id ) : wxPanel( pParent, id ),
																		 m_pTopSizer( NULL ),
																		 m_pNowPropGrid( NULL ),
																		 m_iFocusingWndID( wxNOT_FOUND )

{
	// 테스트로 조금만 생성해 보자.
	m_pTopSizer = new wxBoxSizer( wxVERTICAL );

	//m_pNowPropGrid = new wxPropertyGrid( this, id, wxDefaultPosition, wxDefaultSize, 
	//								  wxPG_BOLD_MODIFIED | wxPG_TOOLTIPS | wxTAB_TRAVERSAL | wxEXPAND );

	m_apPropGrids[ ACTOR_PROP_ID ] = new CActorProperty( this, ACTOR_PROP_ID );
	m_apPropGrids[ ACTION_PROP_ID ] = new CActionProperty( this, ACTION_PROP_ID ); 
	m_apPropGrids[ KEY_PROP_ID ] = new CKeyProperty( this, KEY_PROP_ID );
	m_apPropGrids[ CAMERA_EVENT_PROP_ID ] = new CCameraEventProperty( this, CAMERA_EVENT_PROP_ID );
	m_apPropGrids[ PARTICLE_EVENT_PROP_ID ] = new CParticleEventProperty( this, PARTICLE_EVENT_PROP_ID );
	m_apPropGrids[ SOUND_EVENT_PROP_ID ] = new CSoundEventProperty( this, SOUND_EVENT_PROP_ID );
	m_apPropGrids[ FADE_EVENT_PROP_ID ] = new CFadeEventProperty( this, FADE_EVENT_PROP_ID );
	m_apPropGrids[ SCENE_PROP_ID ] = new CSceneProperty( this, SCENE_PROP_ID );
	m_apPropGrids[ PROP_EVENT_ID ] = new CActionPropProperty( this, PROP_EVENT_ID );
	m_apPropGrids[ DOF_EVENT_PROP_ID ] = new CDofEventProperty( this, DOF_EVENT_PROP_ID );
	m_apPropGrids[ SUBTITLE_EVENT_PROP_ID ] = new CSubtitleEventProperty( this, SUBTITLE_EVENT_PROP_ID );
	m_apPropGrids[ IMAGE_EVENT_PROP_ID ] = new CImageEventProperty( this, IMAGE_EVENT_PROP_ID );
	//m_apPropGrids[ MOVE_PROP_ID ] = new CMoveProperty( this, MOVE_PROP_ID );
	//m_apPropGrids[ ROTATE_PROP_ID ] = new CRotateProperty( this, ROTATE_PROP_ID );
	
	//m_pNowPropGrid->AppendCategory( wxT("Property") );
	//m_pNowPropGrid->Append( wxStringProperty( wxT("Name"), wxPG_LABEL, wxT("") ) );

//#ifdef PRE_ADD_FILTEREVENT
	m_apPropGrids[ COLORFILTER_EVENT_PROP_ID ] = new CColorFilterEventProperty( this, COLORFILTER_EVENT_PROP_ID );
//#endif PRE_ADD_FILTEREVENT

	for( int iProp = 0; iProp < PROP_COUNT; ++iProp )
	{
		m_pTopSizer->Add( m_apPropGrids[ iProp ]->GetPropertyGrid(), 1, wxALL|wxALIGN_CENTER|wxEXPAND, 2 );
		m_apPropGrids[ iProp ]->GetPropertyGrid()->Show( false );
	}
	
	this->SetSizer( m_pTopSizer );
	m_pTopSizer->Fit( this );
	m_pTopSizer->SetSizeHints( this );

	ShowThisProp( ACTOR_PROP_ID );
	m_pNowPropGrid->GetPropertyGrid()->Show( false );

	TOOL_DATA.SetPropertyPanel( this );
}



cwxPropertyPanel::~cwxPropertyPanel(void)
{
	for( int iProp = 0; iProp < PROP_COUNT; ++iProp )
	{
		if( NULL != m_apPropGrids[ iProp ] )
		{
			delete m_apPropGrids[ iProp ];
			m_apPropGrids[ iProp ] = NULL;
		}
	}
}


bool cwxPropertyPanel::Enable( bool bEnable /* = true  */ )
{
	for( int iProperty = 0; iProperty < PROP_COUNT; ++iProperty )
		m_apPropGrids[iProperty]->GetPropertyGrid()->Enable( bEnable );

	return wxPanel::Enable( bEnable );
}


void cwxPropertyPanel::ShowThisProp( int iPropToShow )
{
	for( int iProp = 0; iProp < PROP_COUNT; ++iProp )
	{
		m_apPropGrids[ iProp ]->Show( false );
	}

	m_apPropGrids[ iPropToShow ]->Show( true );
	m_pNowPropGrid = m_apPropGrids[ iPropToShow ];

	m_pTopSizer->Layout();
}


void cwxPropertyPanel::OnPropertyChanged( wxPropertyGridEvent& PGEvent )
{
	m_pNowPropGrid->OnPropertyChanged( PGEvent );
}



void cwxPropertyPanel::OnSelChangeRegResList( int iSelection )
{
	if( wxNOT_FOUND != iSelection )
	{
		int iResourceKind = TOOL_DATA.GetRegResKindByIndex( iSelection );
		switch( iResourceKind )
		{
			case CDnCutSceneData::RT_RES_MAP:
				//m_pNowPropGrid->GetPropertyGrid()->Show( false );
				ShowThisProp( SCENE_PROP_ID );
				break;

			case CDnCutSceneData::RT_RES_ACTOR:
				ShowThisProp( ACTOR_PROP_ID );
				break;

			//case CDnCutSceneData::RT_RES_MAX_CAMERA:
			//	//_ShowThisProp( CAMERA_EVENT_PROP_ID );
			//	m_pNowPropGrid->GetPropertyGrid()->Show( false );
			//	break;

			case CDnCutSceneData::RT_RES_SOUND:
				m_pNowPropGrid->GetPropertyGrid()->Show( false );
				break;
		}
	}
}



void cwxPropertyPanel::OnTLActionSelect( void )
{
	switch( TOOL_DATA.GetSelectedObjectUseType() )
	{
		case CToolData::ACTION:
			ShowThisProp( ACTION_PROP_ID );
			break;

		case CToolData::KEY:
		case CToolData::SUBKEY:
			ShowThisProp( KEY_PROP_ID );
			break;

		case CToolData::MAX_CAMERA:
			ShowThisProp( CAMERA_EVENT_PROP_ID );
			break;

		case CToolData::PARTICLE:
			ShowThisProp( PARTICLE_EVENT_PROP_ID );
			break;

		//case CToolData::SOUND:
		//	ShowThisProp( SOUND_EVENT_PROP_ID );
		//	break;

		case CToolData::DOF:
			ShowThisProp( DOF_EVENT_PROP_ID );
			break;

		case CToolData::FADE:
			ShowThisProp( FADE_EVENT_PROP_ID );
			break;
		
		case CToolData::PROP:
			ShowThisProp( PROP_EVENT_ID );
			break;

		case CToolData::SOUND_1:
		case CToolData::SOUND_2:
		case CToolData::SOUND_3:
		case CToolData::SOUND_4:
		case CToolData::SOUND_5:
		case CToolData::SOUND_6:
		case CToolData::SOUND_7:
		case CToolData::SOUND_8:
			ShowThisProp( SOUND_EVENT_PROP_ID );
			break;

		case CToolData::SUBTITLE:
			ShowThisProp( SUBTITLE_EVENT_PROP_ID );
			break;

		case CToolData::IMAGE:
			ShowThisProp( IMAGE_EVENT_PROP_ID );
			break;

//#ifdef PRE_ADD_FILTEREVENT
		case CToolData::COLORFILTER:
			ShowThisProp( COLORFILTER_EVENT_PROP_ID );
			break;
//#endif PRE_ADD_FILTEREVENT
	}
}





// 현재 포커스가 가 있는 윈도우를 기준으로 property를 보여주자... 이번엔 방법을 바꿔볼까.
//void cwxPropertyPanel::OnUpdateProperty( wxUpdateUIEvent& UpdateEvent )
//{
//	wxWindow* pFocused = wxWindow::FindFocus();
//
//	if( pFocused )
//	{
//		int iNowFocusingWndID = pFocused->GetId();
//		
//		if( iNowFocusingWndID != m_iFocusingWndID )
//		{
//			m_iFocusingWndID = iNowFocusingWndID;
//			switch( iNowFocusingWndID )
//			{
//				case cwxRegisteredResPanel::LIST_REGISTERED_RES_ID:
//					{
//						int iSelection = TOOL_DATA.GetSelectedRegResIndex();
//						if( wxNOT_FOUND != iSelection )
//						{
//							int iResourceKind = TOOL_DATA.GetRegResKindByIndex( iSelection );
//							switch( iResourceKind )
//							{
//								case CDnCutSceneData::RT_RES_MAP:
//									m_pNowPropGrid->GetPropertyGrid()->Enable( false );
//									break;
//									
//								case CDnCutSceneData::RT_RES_ACTOR:
//									_ShowThisProp( ACTOR_PROP_ID );
//									break;
//
//								case CDnCutSceneData::RT_RES_SOUND:
//									m_pNowPropGrid->GetPropertyGrid()->Enable( false );
//									break;
//							}
//						}
//						else
//							m_pNowPropGrid->GetPropertyGrid()->Enable( false );
//
//					}
//					break;
//
//				case cwxTimeLinePanel::EVENT_TIMELINE_ID:
//					m_pNowPropGrid->GetPropertyGrid()->Enable( false );
//					break;
//
//				case cwxTimeLinePanel::ACTION_TIMELINE_ID:
//					_ShowThisProp( ACTION_PROP_ID );
//					break;
//			}
//		}
//	}
//}


void cwxPropertyPanel::ClearTempData( void )
{
	for( int iProp = 0; iProp < PROP_COUNT; ++iProp )
		m_apPropGrids[ iProp ]->ClearTempData();
}


void cwxPropertyPanel::AssignPosition( const EtVector3& vPos )
{
	if( m_pNowPropGrid )
	{
		wxPropertyGrid* pPropGrid = m_pNowPropGrid->GetPropertyGrid();
		wxPGId SelectedPropertyID = pPropGrid->GetSelectedProperty();
		if( SelectedPropertyID.IsOk() )
		{
			wxPGProperty& Property = SelectedPropertyID.GetProperty();
			if( Property.IsEnabled() &&
				wxStrcmp( Property.GetValueType()->GetTypeName(), wxT("EtVector3") ) == 0 )
			{
				wxVariantData_EtVector3* pData = new wxVariantData_EtVector3(vPos);
				wxVariant Value( pData, wxT("EtVector3") );

				pPropGrid->SetPropertyValue( SelectedPropertyID, Value );
				m_pNowPropGrid->OnPropertyChanged( wxPropertyGridEvent() );
			}
		}
	}
}


void cwxPropertyPanel::CommandPerformed( ICommand* pCommand )
{
	switch( pCommand->GetTypeID() )
	{
		case CMD_REGISTER_RES:
		case CMD_UNREGISTER_RES:
			if( TOOL_DATA.GetSelectedRegResIndex() == wxNOT_FOUND )
				m_pNowPropGrid->Show( false );
			break;
	}

	m_pNowPropGrid->CommandPerformed( pCommand );
}