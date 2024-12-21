#include "stdafx.h"
#include "ActionPropProperty.h"
#include "CommandSet.h"
#include "ToolData.h"
#include "RTCutSCeneMakerFrame.h"
#include "RTCutSceneRenderer.h"

const float DEFAULT_ANI_FPS = 60.0f;


CActionPropProperty::CActionPropProperty( wxWindow* pParent, int id ) : IPropertyDelegate( pParent, id ), m_iMyEventInfoID( -1 )
{
	// 속성들을 채움.. 이미 프랍 데이터 등은 로드 되어 있다고 가정한다.
	wxPGId CategoryID = m_pPropGrid->AppendCategory( wxT("PropActionProperty") );
	m_pPropGrid->SetPropertyHelpString( CategoryID, wxT("Set Prop Action Property") );

	m_aPGID[ PROP_LIST ] = m_pPropGrid->Append( wxEnumProperty( wxT("Prop List"), wxPG_LABEL, wxArrayString() ) );
	m_aPGID[ PROP_ID ] = m_pPropGrid->Append( wxUIntProperty( wxT("Prop ID"), wxPG_LABEL, 0 ) );
	m_aPGID[ ACTION_LIST ] = m_pPropGrid->Append( wxEnumProperty( wxT("Action List"), wxPG_LABEL, wxArrayString() ) );
	m_aPGID[ SHOW ] = m_pPropGrid->Append( wxBoolProperty( wxT("Show"), wxPG_LABEL, true) );
	m_pPropGrid->SetPropertyAttribute( m_aPGID[ SHOW ], wxPG_BOOL_USE_CHECKBOX, true );
	m_aPGID[ EVENT_ID ] = m_pPropGrid->Append( wxUIntProperty( wxT("Event ID"), wxPG_LABEL, 0 ) );
	m_aPGID[ START_TIME ] = m_pPropGrid->Append( wxFloatProperty( wxT( "StartTime"), wxPG_LABEL, 0 ) );
	m_aPGID[ TIME_LENGTH ] = m_pPropGrid->Append( wxFloatProperty( wxT("Length"), wxPG_LABEL, 0 ) );

	m_pPropGrid->EnableProperty( m_aPGID[ PROP_ID ], false );
	//m_pPropGrid->EnableProperty( m_aPGID[ ACTION_NAME ], false );
	m_pPropGrid->EnableProperty( m_aPGID[ EVENT_ID ], false );
	m_pPropGrid->EnableProperty( m_aPGID[ TIME_LENGTH ], false );

	CRTCutSceneMakerFrame* pMainFrame = static_cast<CRTCutSceneMakerFrame*>(TOOL_DATA.GetMainFrame());
	pMainFrame->ConnectMouseWheelEventToRenderPanel( m_pPropGrid );
}

CActionPropProperty::~CActionPropProperty(void)
{
	
}


void CActionPropProperty::Initialize( void )
{

}


void CActionPropProperty::_UpdateProp( int iEventInfoID )
{
	EventInfo* pEventInfo = const_cast<EventInfo*>(TOOL_DATA.GetEventInfoByID( iEventInfoID ));
	PropEventInfo* pPropEventInfo = static_cast<PropEventInfo*>(pEventInfo);

	wxCSConv MBConv( wxFONTENCODING_CP949 );
	wxChar wcaBuf[ 256 ];

	ZeroMemory( wcaBuf, sizeof(wcaBuf) );
	MBConv.MB2WC( wcaBuf, pPropEventInfo->strEventName.c_str(), 256 );

	m_pPropGrid->SetPropertyValue( m_aPGID[ PROP_ID ], pPropEventInfo->iPropID );

	wxPGChoiceInfo ChoiceInfo;
	m_aPGID[ PROP_LIST ].GetProperty().GetChoiceInfo( &ChoiceInfo );
	while( ChoiceInfo.m_itemCount > 0 )
	{
		m_aPGID[ PROP_LIST ].GetProperty().DeleteChoice( 0 );
		m_aPGID[ PROP_LIST ].GetProperty().GetChoiceInfo( &ChoiceInfo );
	}

	m_aPGID[ ACTION_LIST ].GetProperty().GetChoiceInfo( &ChoiceInfo );
	while( ChoiceInfo.m_itemCount > 0 )
	{
		m_aPGID[ ACTION_LIST ].GetProperty().DeleteChoice( 0 );
		m_aPGID[ ACTION_LIST ].GetProperty().GetChoiceInfo( &ChoiceInfo );
	}

	m_pPropGrid->SetPropertyValueBool( m_aPGID[ SHOW ], pPropEventInfo->bShow );

	m_pPropGrid->SetPropertyValueDouble( m_aPGID[ TIME_LENGTH ], 0.0f );

	m_vlPropID.clear();
	int iFindedProp = -1;
	int iFindedAction = -1;
	S_PROP_INFO PropInfo;
	int iNumActionProp = TOOL_DATA.GetNumActionProp();
	for( int iProp = 0; iProp < iNumActionProp; ++iProp )
	{
		const S_PROP_INFO* pPropInfo = TOOL_DATA.GetActionPropInfo( iProp );
		m_pPropGrid->AddPropertyChoice( m_aPGID[ PROP_LIST ], pPropInfo->strPropName.c_str() );
		m_vlPropID.push_back( pPropInfo->iPropID );

		if( pPropInfo->iPropID == pPropEventInfo->iPropID )
		{
			iFindedProp = iProp;

			wxCSConv MBConv( wxFONTENCODING_CP949 );
			wxChar wcaBuf2[ 256 ];
			ZeroMemory( wcaBuf2, sizeof(wcaBuf2) );
			MBConv.MB2WC( wcaBuf2, pPropEventInfo->strActionName.c_str(), pPropEventInfo->strActionName.length() );

			int iNumAction = (int)pPropInfo->strActionList.size();
			for( int iAction = 0; iAction < iNumAction; ++iAction )
			{
				m_pPropGrid->AddPropertyChoice( m_aPGID[ ACTION_LIST ], pPropInfo->strActionList[iAction] );
				if( pPropInfo->strActionList[iAction] == wcaBuf2 )
				{
					iFindedAction = iAction;
					m_pPropGrid->SetPropertyValueDouble( m_aPGID[ TIME_LENGTH ], (float)pPropInfo->vldwLength.at(iAction)*1000.0f / DEFAULT_ANI_FPS );		// 디폴트 스피드는 60 프레임
				}
			}
		}
	}

	if( -1 != iFindedProp )
		m_pPropGrid->SetPropertyValue( m_aPGID[ PROP_LIST ], iFindedProp );

	if( -1 != iFindedAction )
		m_pPropGrid->SetPropertyValue( m_aPGID[ ACTION_LIST ], iFindedAction );

	ZeroMemory( wcaBuf, sizeof(wcaBuf) );
	MBConv.MB2WC( wcaBuf, pPropEventInfo->strActionName.c_str(), 256 );

	m_pPropGrid->SetPropertyValue( m_aPGID[ EVENT_ID ], pPropEventInfo->iID );
	m_pPropGrid->SetPropertyValueDouble( m_aPGID[ START_TIME ], pPropEventInfo->fStartTime );

	m_iMyEventInfoID = iEventInfoID;

	CRTCutSceneRenderer* pRenderer = TOOL_DATA.GetRenderer();
	pRenderer->OnSelecteProp( pPropEventInfo->iPropID );
}


void CActionPropProperty::OnShow( void )
{
	int iSelectedUnitID = TOOL_DATA.GetSelectedObjectID();
	if( wxNOT_FOUND != iSelectedUnitID )
	{
		_UpdateProp( iSelectedUnitID );
	}
}



void CActionPropProperty::_GetPropertyValue( EventInfo* pEventInfo )
{
	PropEventInfo* pPropEventInfo = static_cast<PropEventInfo*>(pEventInfo);
	
	wxCSConv MBConv( wxFONTENCODING_CP949 );
	char caBuf[ 256 ];

	wxString strPropName = m_pPropGrid->GetPropertyValueAsString( m_aPGID[ PROP_LIST ] );

	// 같은 이름이고 id만 다른 경우도 있으므로.
	int iIndex = m_pPropGrid->GetPropertyValueAsInt( m_aPGID[ PROP_LIST ] );

	if( 0 <= iIndex && iIndex < (int)m_vlPropID.size() )
	{
		int iNowSelectedPropID = m_vlPropID.at( iIndex );

		ZeroMemory( caBuf, sizeof(caBuf) );
		MBConv.WC2MB( caBuf, strPropName.c_str(), 256 );
		pPropEventInfo->strEventName.assign( caBuf );

		pPropEventInfo->bShow = m_pPropGrid->GetPropertyValueAsBool( m_aPGID[ SHOW ] );

		int iNumActionProp = TOOL_DATA.GetNumActionProp();
		for( int iProp = 0; iProp < iNumActionProp; ++iProp )
		{
			const S_PROP_INFO* pPropInfo = TOOL_DATA.GetActionPropInfo( iProp );
			m_pPropGrid->AddPropertyChoice( m_aPGID[ PROP_LIST ], pPropInfo->strPropName.c_str() );

			if( pPropInfo->iPropID == iNowSelectedPropID )
			{
				pPropEventInfo->iPropID = pPropInfo->iPropID;
				break;
			}
		}

		wxString strActionName;
		strActionName = m_pPropGrid->GetPropertyValueAsString( m_aPGID[ ACTION_LIST ] );
		ZeroMemory( caBuf, sizeof(caBuf) );
		MBConv.WC2MB( caBuf, strActionName.c_str(), 256 );
		pPropEventInfo->strActionName.assign( caBuf );

		//m_pPropGrid->SetPropertyValueString( m_aPGID[ ACTION_NAME ], strActionName );

		pPropEventInfo->iID = m_pPropGrid->GetPropertyValueAsInt( m_aPGID[ EVENT_ID ] );
		pPropEventInfo->fStartTime = m_pPropGrid->GetPropertyValueAsDouble( m_aPGID[ START_TIME ] );
		//pPropEventInfo->fUnit = 10.0f;
	}
}


void CActionPropProperty::OnPropertyChanged( wxPropertyGridEvent& PGEvent )
{
	const EventInfo* pPropEventInfo = TOOL_DATA.GetEventInfoByID( m_iMyEventInfoID );
	EventInfo* pNewPropEventInfo = pPropEventInfo->clone();
	_GetPropertyValue( pNewPropEventInfo );

	CEventPropChange EventChange( &TOOL_DATA, pNewPropEventInfo );
	TOOL_DATA.RunCommand( &EventChange );

	delete pNewPropEventInfo;
}


void CActionPropProperty::ClearTempData( void )
{
	m_iMyEventInfoID = -1;
}


void CActionPropProperty::CommandPerformed( ICommand* pCommand )
{
	const EventInfo* pMyEventInfo = TOOL_DATA.GetEventInfoByID( m_iMyEventInfoID );
	if( NULL == pMyEventInfo )
	{
		m_pPropGrid->Show( false );
		CRTCutSceneRenderer* pRenderer = TOOL_DATA.GetRenderer();
		pRenderer->OnSelecteProp( -1 );
	}
	else
	{
		switch( pCommand->GetTypeID() )
		{
			case CMD_UPDATE_VIEW:
			case CMD_EVENT_PROP_CHANGE:
			case CMD_REGISTER_RES:
				_UpdateProp( m_iMyEventInfoID );
				break;
		}
	}
}