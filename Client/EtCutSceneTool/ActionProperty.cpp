#include "stdafx.h"
#include "VectorProperty.h"
#include "ActionProperty.h"
#include "CommandSet.h"
#include "ToolData.h"
#include "RTCutSceneRenderer.h"
#include "RTCutSceneMakerFrame.h"


const wxChar* AT_ONCE = wxT( "Once" );				// 일회성인 경우는 액션 길이 편집이 불가능
const wxChar* AT_REPEAT = wxT( "Repeat" );
const wxChar* AT_CONTINUE = wxT( "Continue" );		// 지속 애니메이션. 이게 박혀있으면 지속적으로 애니메이션이 재생된다.


CActionProperty::CActionProperty( wxWindow* pParent, int id ) : IPropertyDelegate( pParent, id ),
																m_pReflectedActionInfo( new ActionInfo )
{
	_RebuildProperty();

	CRTCutSceneMakerFrame* pMainFrame = static_cast<CRTCutSceneMakerFrame*>(TOOL_DATA.GetMainFrame());
	pMainFrame->ConnectMouseWheelEventToRenderPanel( m_pPropGrid );
}


CActionProperty::~CActionProperty(void)
{
	delete m_pReflectedActionInfo;
}



void CActionProperty::Initialize( void )
{

}



void CActionProperty::_RebuildProperty( void )
{
	m_pPropGrid->Clear();

	wxPGId CategoryID = m_pPropGrid->AppendCategory( wxT("Action Property") );
	m_pPropGrid->SetPropertyHelpString( CategoryID, wxT("Set Action Property") );

	m_aPGID[ ACTOR_NAME ] = m_pPropGrid->Append( wxStringProperty( wxT("Actor"), wxPG_LABEL, wxT("") ) );
	m_aPGID[ ACTION_NAME ] = m_pPropGrid->Append( wxStringProperty( wxT("Action Name"), wxPG_LABEL, wxT("") ) );
	m_aPGID[ ID ] = m_pPropGrid->Append( wxUIntProperty( wxT("unique ID"), wxPG_LABEL, 0 ) );
	m_aPGID[ ANIMATION_NAME_ENUM ] = m_pPropGrid->Append( wxEnumProperty( wxT("Animation"), wxPG_LABEL, wxArrayString() ) );
	m_pPropGrid->SetPropertyValue( m_aPGID[ ANIMATION_NAME_ENUM ], 0 );
	m_aPGID[ USE_ANI_DISTANCE ] = m_pPropGrid->Append( wxBoolProperty( wxT("Use Ani Distance"), wxPG_LABEL, false ) );
	m_pPropGrid->SetPropertyAttribute( m_aPGID[ USE_ANI_DISTANCE ], wxPG_BOOL_USE_CHECKBOX, true );
	m_aPGID[ FIT_Y_ANI_DISTANCE_TO_MAP ] = m_pPropGrid->Append( wxBoolProperty( wxT("Fit AD Y To Map" ), wxPG_LABEL, false ) );
	m_pPropGrid->SetPropertyAttribute( m_aPGID[ FIT_Y_ANI_DISTANCE_TO_MAP ], wxPG_BOOL_USE_CHECKBOX, true );
	m_aPGID[ SPEED ] = m_pPropGrid->Append( wxFloatProperty( wxT("Speed"), wxPG_LABEL, 0.0f ) );

	m_aPGID[ USE_START_ROTATION ] = m_pPropGrid->Append( wxBoolProperty( wxT("Use Start Rotation"), wxPG_LABEL, false ) );
	m_pPropGrid->SetPropertyAttribute( m_aPGID[ USE_START_ROTATION ], wxPG_BOOL_USE_CHECKBOX, true );
	m_aPGID[ ROTATION ] = m_pPropGrid->Append( wxFloatProperty( wxT("Start Rotation"), wxPG_LABEL, 0.0f ) );

	m_aPGID[ USE_START_POSITION ] = m_pPropGrid->Append( wxBoolProperty( wxT("Use Start Position"), wxPG_LABEL, false ) );
	m_pPropGrid->SetPropertyAttribute( m_aPGID[ USE_START_POSITION ], wxPG_BOOL_USE_CHECKBOX, true );
	m_aPGID[ POSITION ] = m_pPropGrid->Append( VectorProperty( wxT("Start Position"), wxPG_LABEL, EtVector3( 0.0f, 0.0f, 0.0f ) ) );

	m_pPropGrid->EnableProperty( m_aPGID[ ROTATION ], false );

	const wxChar* astrActionType [] = { AT_ONCE, AT_REPEAT, AT_CONTINUE, NULL };
	m_aPGID[ ACTION_TYPE ] = m_pPropGrid->Append( wxEnumProperty( wxT("Type") , wxPG_LABEL, astrActionType ) );
	m_aPGID[ START_TIME ] = m_pPropGrid->Append( wxFloatProperty( wxT("Start Time"), wxPG_LABEL, 0.0f ) );
	m_aPGID[ TIME_LENGTH ] = m_pPropGrid->Append( wxFloatProperty( wxT("Time Length(sec)"), wxPG_LABEL, 0.0f ) );

	//m_aPGID[ ROTATION ] = m_pPropGrid->Append( wxFloatProperty( wxT("Rotation"), wxPG_LABEL, 0.0f ) );

	//m_pPropGrid->AppendCategory( wxT("Start Pos") );
	//m_aPGID[ START_X_POS ] = m_pPropGrid->Append( wxFloatProperty( wxT("Start X"), wxPG_LABEL, 0.0f ) );
	//m_aPGID[ START_Y_POS ] = m_pPropGrid->Append( wxFloatProperty( wxT("Start Y"), wxPG_LABEL, 0.0f ) );
	//m_aPGID[ START_Z_POS ] = m_pPropGrid->Append( wxFloatProperty( wxT("Start Z"), wxPG_LABEL, 0.0f ) );

	//m_pPropGrid->AppendCategory( wxT("End Pos") );
	//m_aPGID[ END_X_POS ] = m_pPropGrid->Append( wxFloatProperty( wxT("End X"), wxPG_LABEL, 0.0f ) );
	//m_aPGID[ END_Y_POS ] = m_pPropGrid->Append( wxFloatProperty( wxT("End Y"), wxPG_LABEL, 0.0f ) );
	//m_aPGID[ END_Z_POS ] = m_pPropGrid->Append( wxFloatProperty( wxT("End Z"), wxPG_LABEL, 0.0f ) );
}



void CActionProperty::_UpdateProp( int iActionInfoID )
{
	if( wxNOT_FOUND != iActionInfoID )
	{
		const ActionInfo* pActionInfo = TOOL_DATA.GetActionInfoByID( iActionInfoID );
		
		_RebuildProperty();

		wxCSConv MBConv( wxFONTENCODING_CP949 );
		wxChar wcaBuf[ 256 ];
		wxChar wcaActorName[ 256 ];
		ZeroMemory( wcaBuf, sizeof(wcaBuf) );

		MBConv.MB2WC( wcaActorName, pActionInfo->strActorName.c_str(), 256 );

		int iNumAni = TOOL_DATA.GetThisActorsAnimationNum( wcaActorName );
		for( int iAni = 0; iAni < iNumAni; ++iAni )
		{
			const ActionEleInfo* pAnimation = TOOL_DATA.GetThisActorsAnimation( wcaActorName, iAni );

			MBConv.MB2WC( wcaBuf, pAnimation->szName.c_str(), 256 );
			m_pPropGrid->AddPropertyChoice( m_aPGID[ ANIMATION_NAME_ENUM ], wcaBuf );
		}

		m_pPropGrid->SetPropertyValue( m_aPGID[ ANIMATION_NAME_ENUM ], pActionInfo->iAnimationIndex );
		
		MBConv.MB2WC( wcaBuf, pActionInfo->strActorName.c_str(), 256 );
		wxString strActorName( wcaBuf );
		m_pPropGrid->SetPropertyValueString( m_aPGID[ ACTOR_NAME ], strActorName );
		m_pPropGrid->EnableProperty( m_aPGID[ ACTOR_NAME ], false );
		
		MBConv.MB2WC( wcaBuf, pActionInfo->strActionName.c_str(), 256 );
		wxString strActionName( wcaBuf );
		m_pPropGrid->SetPropertyValueString( m_aPGID[ ACTION_NAME ], strActionName );

		// Action Type 지정에 따라 액션 길이 수정 가능여부가 달라짐
		m_pPropGrid->SetPropertyValueLong( m_aPGID[ ACTION_TYPE ], pActionInfo->iActionType );
		switch( pActionInfo->iActionType )
		{
			case ActionInfo::AT_ONCE:
				m_pPropGrid->EnableProperty( m_aPGID[ TIME_LENGTH ], false );
				break;

			case ActionInfo::AT_REPEAT:
			case ActionInfo::AT_CONTINUE:
				m_pPropGrid->EnableProperty( m_aPGID[ TIME_LENGTH ], true );
				break;
		}
		
		m_pPropGrid->SetPropertyValueLong( m_aPGID[ ID ], pActionInfo->iID );
		m_pPropGrid->EnableProperty( m_aPGID[ ID ], false );
		m_pPropGrid->SetPropertyValueBool( m_aPGID[ USE_ANI_DISTANCE ], pActionInfo->bUseAniDistance );

		if( false == pActionInfo->bUseAniDistance )
		{
			m_pPropGrid->EnableProperty( m_aPGID[ FIT_Y_ANI_DISTANCE_TO_MAP ], false );
			m_pPropGrid->SetPropertyValueBool( m_aPGID[ USE_ANI_DISTANCE ], false );
		}
		else
			m_pPropGrid->EnableProperty( m_aPGID[ FIT_Y_ANI_DISTANCE_TO_MAP ], true );

		m_pPropGrid->SetPropertyValueBool( m_aPGID[ FIT_Y_ANI_DISTANCE_TO_MAP ], pActionInfo->bFitAniDistanceYToMap );

		m_pPropGrid->SetPropertyValueDouble( m_aPGID[ SPEED ], pActionInfo->fAnimationSpeed );

		m_pPropGrid->SetPropertyValueBool( m_aPGID[ USE_START_ROTATION ], pActionInfo->bUseStartRotation );
		m_pPropGrid->EnableProperty( m_aPGID[ ROTATION ], pActionInfo->bUseStartRotation );
		m_pPropGrid->SetPropertyValueDouble( m_aPGID[ ROTATION ], pActionInfo->fStartRotation );
		
		m_pPropGrid->SetPropertyValueBool( m_aPGID[ USE_START_POSITION ], pActionInfo->bUseStartPosition );
		m_pPropGrid->EnableProperty( m_aPGID[ POSITION ], pActionInfo->bUseStartPosition );
		wxVariantData_EtVector3* pData = new wxVariantData_EtVector3(pActionInfo->vStartPos);
		wxVariant Value( pData, wxT("EtVector3") );
		m_pPropGrid->SetPropertyValue( m_aPGID[ POSITION ], Value );
		
		m_pPropGrid->SetPropertyValueDouble( m_aPGID[ START_TIME ], pActionInfo->fStartTime );
		m_pPropGrid->SetPropertyValueDouble( m_aPGID[ TIME_LENGTH ], pActionInfo->fTimeLength );

		//m_pPropGrid->SetPropertyValueDouble( m_aPGID[ ROTATION ], pActionInfo->fRotation );
		//m_pPropGrid->SetPropertyValueDouble( m_aPGID[ START_X_POS ], pActionInfo->vStartPos.x );
		//m_pPropGrid->SetPropertyValueDouble( m_aPGID[ START_Y_POS ], pActionInfo->vStartPos.y );
		//m_pPropGrid->SetPropertyValueDouble( m_aPGID[ START_Z_POS ], pActionInfo->vStartPos.z );
		//m_pPropGrid->SetPropertyValueDouble( m_aPGID[ END_X_POS ], pActionInfo->vEndPos.x );
		//m_pPropGrid->SetPropertyValueDouble( m_aPGID[ END_Y_POS ], pActionInfo->vEndPos.y );
		//m_pPropGrid->SetPropertyValueDouble( m_aPGID[ END_Z_POS ], pActionInfo->vEndPos.z );

		_GetPropertyValues( m_pReflectedActionInfo );
	}
}



void CActionProperty::_GetPropertyValues( /*IN OUT*/ ActionInfo* pActionInfo )
{
	wxCSConv MBConv( wxFONTENCODING_CP949 );
	char caBuf[ 256 ];

	wxString strActorName = m_pPropGrid->GetPropertyValueAsString( m_aPGID[ ACTOR_NAME ] );
	ZeroMemory( caBuf, sizeof(caBuf) );
	MBConv.WC2MB( caBuf, strActorName.c_str(), 256 );
	pActionInfo->strActorName.assign( caBuf );

	wxString strActionName = m_pPropGrid->GetPropertyValueAsString( m_aPGID[ ACTION_NAME ] );
	ZeroMemory( caBuf, sizeof(caBuf) );
	MBConv.WC2MB( caBuf, strActionName.c_str(), 256 );
	pActionInfo->strActionName.assign( caBuf );

	// 애니메이션 속도 먼저 받아둬야 일회성 액션의 길이가 정확하게 길이가 나온다. 
	pActionInfo->iAnimationIndex = m_pPropGrid->GetPropertyValueAsInt( m_aPGID[ ANIMATION_NAME_ENUM ] );
	if( pActionInfo->iAnimationIndex < 0 ) 
		pActionInfo->iAnimationIndex = 0;
	const ActionEleInfo* pAnimationInfo = TOOL_DATA.GetThisActorsAnimation( strActorName.c_str(), pActionInfo->iAnimationIndex );

	pActionInfo->bUseStartRotation = m_pPropGrid->GetPropertyValueAsBool( m_aPGID[ USE_START_ROTATION ] );
	pActionInfo->fAnimationSpeed = (float)m_pPropGrid->GetPropertyValueAsDouble( m_aPGID[ SPEED ] );
	pActionInfo->fStartRotation = (float)m_pPropGrid->GetPropertyValueAsDouble( m_aPGID[ ROTATION ] );
	
	pActionInfo->bUseStartPosition = m_pPropGrid->GetPropertyValueAsBool( m_aPGID[ USE_START_POSITION ] );

	wxVariant value = m_pPropGrid->GetPropertyValue( m_aPGID[ POSITION ] );
	wxVariantData_EtVector3* pData = static_cast<wxVariantData_EtVector3*>(value.GetData());
	const EtVector3 vPos = pData->GetValue();
	pActionInfo->vStartPos = vPos;
	
	if( pAnimationInfo )
	{
		pActionInfo->strAnimationName.assign( pAnimationInfo->szName );
		pActionInfo->iNextActionFrame = (int)pAnimationInfo->dwNextActionFrame;
	}

	pActionInfo->iActionType = (int)m_pPropGrid->GetPropertyValueAsInt( m_aPGID[ ACTION_TYPE ] );

	switch( pActionInfo->iActionType )
	{
		// 일회성인 경우는 애니메이션 속도에 따라 액션 길이가 종속적이다.
		case ActionInfo::AT_ONCE:
			pActionInfo->fTimeLength = (float)pAnimationInfo->dwLength / pActionInfo->fAnimationSpeed;
			m_pPropGrid->SetPropertyValueDouble( m_aPGID[ TIME_LENGTH ], pActionInfo->fTimeLength );
			break;

		case ActionInfo::AT_REPEAT:
		case ActionInfo::AT_CONTINUE:
			pActionInfo->fTimeLength = (float)m_pPropGrid->GetPropertyValueAsDouble( m_aPGID[ TIME_LENGTH ] );
			break;
	}
	
	pActionInfo->iID = m_pPropGrid->GetPropertyValueAsInt( m_aPGID[ ID ] );
	pActionInfo->fStartTime = (float)m_pPropGrid->GetPropertyValueAsDouble( m_aPGID[ START_TIME ] );
	pActionInfo->bUseAniDistance = m_pPropGrid->GetPropertyValueAsBool( m_aPGID[ USE_ANI_DISTANCE ] );
	pActionInfo->bFitAniDistanceYToMap = m_pPropGrid->GetPropertyValueAsBool( m_aPGID[ FIT_Y_ANI_DISTANCE_TO_MAP ] );
	pActionInfo->fUnit = 1000.0f;
	//pActionInfo->fRotation = (float)m_pPropGrid->GetPropertyValueAsDouble( m_aPGID[ ROTATION ] );
	//pActionInfo->vStartPos.x = (float)m_pPropGrid->GetPropertyValueAsDouble( m_aPGID[ START_X_POS ] );
	//pActionInfo->vStartPos.y = (float)m_pPropGrid->GetPropertyValueAsDouble( m_aPGID[ START_Y_POS ] );
	//pActionInfo->vStartPos.z = (float)m_pPropGrid->GetPropertyValueAsDouble( m_aPGID[ START_Z_POS ] );
	//pActionInfo->vEndPos.x = (float)m_pPropGrid->GetPropertyValueAsDouble( m_aPGID[ END_X_POS ] );
	//pActionInfo->vEndPos.y = (float)m_pPropGrid->GetPropertyValueAsDouble( m_aPGID[ END_Y_POS ] );
	//pActionInfo->vEndPos.z = (float)m_pPropGrid->GetPropertyValueAsDouble( m_aPGID[ END_Z_POS ] );
}



void CActionProperty::OnShow( void )
{
	int iSelectedObjectID = TOOL_DATA.GetSelectedObjectID();
	if( wxNOT_FOUND != iSelectedObjectID )
	{
		_UpdateProp( iSelectedObjectID );
	}
}


void CActionProperty::OnPropertyChanged( wxPropertyGridEvent& PGEvent )
{
	_GetPropertyValues( m_pReflectedActionInfo );

	//CRTCutSceneRenderer* pRenderer = TOOL_DATA.GetRenderer();
	//
	//wxCSConv MBConv( wxFONTENCODING_CP949 );
	//wxChar wcaBuf[ 256 ];
	//ZeroMemory( wcaBuf, sizeof(wcaBuf) );
	//MBConv.MB2WC( wcaBuf, NewActionInfo.strActorName.c_str(), 256 );
	//
	//pRenderer->SetAnimation( wcaBuf, NewActionInfo.iAnimationIndex );

	CActionPropChange ActionInfoChange( &TOOL_DATA, m_pReflectedActionInfo );
	TOOL_DATA.RunCommand( &ActionInfoChange );
}


void CActionProperty::ClearTempData( void )
{
	SAFE_DELETE( m_pReflectedActionInfo );
	m_pReflectedActionInfo = new ActionInfo;
}


void CActionProperty::CommandPerformed( ICommand* pCommand )
{
	// 자신의 id가 사라지면 숨기도록.
	const ActionInfo* pMyActionInfo = TOOL_DATA.GetActionInfoByID( m_pReflectedActionInfo->iID );
	if( NULL == pMyActionInfo )
	{
		m_pPropGrid->Show( false );
	}
	else
	{
		switch( pCommand->GetTypeID() )
		{
			case CMD_UPDATE_VIEW:
				_UpdateProp( m_pReflectedActionInfo->iID );
				break;
				
			case CMD_ACTION_PROP_CHANGE:
				_UpdateProp( m_pReflectedActionInfo->iID/*static_cast<CActionPropChange*>(pCommand)->m_NewActionInfo.iID*/ );
				break;
		}
	}
}