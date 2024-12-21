#include "StdAfx.h"
#include "ColorFilterEventProperty.h"
#include "CommandSet.h"
#include "ToolData.h"

// PRE_ADD_FILTEREVENT

CColorFilterEventProperty::CColorFilterEventProperty( wxWindow* pParent, int id ) : IPropertyDelegate( pParent, id )
{
	wxPGId CategoryID = m_pPropGrid->AppendCategory( wxT("ColorFilter Event Property") );
	m_pPropGrid->SetPropertyHelpString( CategoryID, wxT("Set ColroFilter Property") );

	m_aPGID[ START_TIME ] = m_pPropGrid->Append( wxFloatProperty( wxT("Start Time"), wxPG_LABEL, 0.0f ) );
	m_aPGID[ TIME_LENGTH ] = m_pPropGrid->Append( wxFloatProperty( wxT( "Length" ), wxPG_LABEL, 0 ) );
	m_aPGID[ MONOCHROME ] = m_pPropGrid->Append( wxBoolProperty( wxT("Monochrome"), wxPG_LABEL, 0 ) );
	m_aPGID[ COLOR_R ] = m_pPropGrid->Append( wxFloatProperty( wxT("Color_Red"), wxPG_LABEL, 0 ) );
	m_aPGID[ COLOR_G ] = m_pPropGrid->Append( wxFloatProperty( wxT("Color_Green"), wxPG_LABEL, 0 ) );
	m_aPGID[ COLOR_B ] = m_pPropGrid->Append( wxFloatProperty( wxT("Color_Blue"), wxPG_LABEL, 0 ) );
	m_aPGID[ COLOR_VOL ] = m_pPropGrid->Append( wxFloatProperty( wxT("Volume"), wxPG_LABEL, 0 ) );

	//m_pPropGrid->SetPropertyAttribute( m_aPGID[ IMAGE_FILE_NAME ], wxPG_FILE_WILDCARD, wxT("dds Image File(*.dds)|*.dds") );
	//m_pPropGrid->SetPropertyAttribute( m_aPGID[ IMAGE_FILE_NAME ], wxPG_FILE_DIALOG_TITLE, wxT("Select Image File") );
	//m_pPropGrid->SetPropertyAttribute( m_aPGID[ IMAGE_FILE_NAME ], wxPG_FILE_SHOW_FULL_PATH, 0 );



	//m_pPropGrid->EnableProperty( m_aPGID[ ID ], false );
	//m_pPropGrid->EnableProperty( m_aPGID[ TIME_LENGTH ], false );

//	m_pPropGrid->SetPropertyValue( m_aPGID[ COLOR_R ], 1 );
//	m_pPropGrid->SetPropertyValue( m_aPGID[ COLOR_G ], 1 );
//	m_pPropGrid->SetPropertyValue( m_aPGID[ COLOR_B ], 1 );
//	m_pPropGrid->SetPropertyValue( m_aPGID[ COLOR_VOL ], 1 );
}


CColorFilterEventProperty::~CColorFilterEventProperty(void)
{
}


void CColorFilterEventProperty::_UpdateProp( int iEventInfoID )
{
	/*if( wxNOT_FOUND != iEventInfoID )
	{
		const FadeEventInfo* pFadeEventInfo = static_cast<const FadeEventInfo*>(TOOL_DATA.GetEventInfoByID(iEventInfoID));

		m_pPropGrid->SetPropertyValue( m_aPGID[ FADE_KIND ], pFadeEventInfo->iFadeKind );
		m_pPropGrid->SetPropertyValueDouble( m_aPGID[ START_TIME ], pFadeEventInfo->fStartTime);
		m_pPropGrid->SetPropertyValueLong( m_aPGID[ COLOR_RED ], pFadeEventInfo->iColorRed );
		m_pPropGrid->SetPropertyValueLong( m_aPGID[ COLOR_GREEN ], pFadeEventInfo->iColorGreen );
		m_pPropGrid->SetPropertyValueLong( m_aPGID[ COLOR_BLUE ], pFadeEventInfo->iColorBlue );
		m_pPropGrid->SetPropertyValueDouble( m_aPGID[ LENGTH ], pFadeEventInfo->fTimeLength );

		m_iMyEventInfoID = pFadeEventInfo->iID;
	}*/

	if( wxNOT_FOUND != iEventInfoID )
	{
		const ColorFilterEventInfo* pCFEventInfo = static_cast<const ColorFilterEventInfo*>(TOOL_DATA.GetEventInfoByID(iEventInfoID));

		m_pPropGrid->SetPropertyValueDouble( m_aPGID[ START_TIME ], pCFEventInfo->fStartTime );
		m_pPropGrid->SetPropertyValueDouble( m_aPGID[ TIME_LENGTH ], pCFEventInfo->fTimeLength );
		m_pPropGrid->SetPropertyValueBool( m_aPGID[ MONOCHROME ], pCFEventInfo->bMonochrome );
		m_pPropGrid->SetPropertyValueDouble( m_aPGID[ COLOR_R ], pCFEventInfo->vColor.x );
		m_pPropGrid->SetPropertyValueDouble( m_aPGID[ COLOR_G ], pCFEventInfo->vColor.y );
		m_pPropGrid->SetPropertyValueDouble( m_aPGID[ COLOR_B ], pCFEventInfo->vColor.z );
		m_pPropGrid->SetPropertyValueDouble( m_aPGID[ COLOR_VOL ], pCFEventInfo->fVolume );
		
		m_iMyEventInfoID = pCFEventInfo->iID;
	}
}

void CColorFilterEventProperty::_GetPropertyValue( EventInfo* pEventInfo )
{
	ColorFilterEventInfo* pCFEventInfo = static_cast<ColorFilterEventInfo*>(pEventInfo);

	//pFadeEventInfo->fUnit = 10.0f;

	pCFEventInfo->fStartTime = m_pPropGrid->GetPropertyValueAsDouble( m_aPGID[ START_TIME ] );
	pCFEventInfo->fTimeLength = m_pPropGrid->GetPropertyValueAsDouble( m_aPGID[ TIME_LENGTH ] );
	pCFEventInfo->bMonochrome = m_pPropGrid->GetPropertyValueAsBool( m_aPGID[ MONOCHROME ] );
	pCFEventInfo->vColor.x = m_pPropGrid->GetPropertyValueAsDouble( m_aPGID[ COLOR_R ] );
	pCFEventInfo->vColor.y = m_pPropGrid->GetPropertyValueAsDouble( m_aPGID[ COLOR_G ] );
	pCFEventInfo->vColor.z = m_pPropGrid->GetPropertyValueAsDouble( m_aPGID[ COLOR_B ] );
	pCFEventInfo->fVolume = m_pPropGrid->GetPropertyValueAsDouble( m_aPGID[ COLOR_VOL ] );	
}

void CColorFilterEventProperty::Initialize( void )
{

}

void CColorFilterEventProperty::OnShow( void )
{
	int iSelectedObjectID = TOOL_DATA.GetSelectedObjectID();
	if( wxNOT_FOUND != iSelectedObjectID )
	{
		_UpdateProp( iSelectedObjectID );
	}
}

void CColorFilterEventProperty::OnPropertyChanged( wxPropertyGridEvent& PGEvent )
{
	const EventInfo * pEventInfo = TOOL_DATA.GetEventInfoByID( m_iMyEventInfoID );
	EventInfo * pNewEventInfo = pEventInfo->clone();
	_GetPropertyValue( pNewEventInfo );

	CEventPropChange EventChange( &TOOL_DATA, pNewEventInfo );
	TOOL_DATA.RunCommand( &EventChange );

	delete pNewEventInfo;
}

void CColorFilterEventProperty::CommandPerformed( ICommand* pCommand )
{
	// 자신의 ID 가 아니면 숨긴다
	const EventInfo* pMyEventInfo = TOOL_DATA.GetEventInfoByID( m_iMyEventInfoID );
	if( NULL == pMyEventInfo )
	{
		m_pPropGrid->Show( false );
	}
	else
	{
		switch( pCommand->GetTypeID() )
		{
		case CMD_UPDATE_VIEW:
			_UpdateProp( m_iMyEventInfoID );
			break;

		case CMD_EVENT_PROP_CHANGE:
			_UpdateProp( m_iMyEventInfoID );
			break;

		case CMD_REGISTER_RES:
			_UpdateProp( m_iMyEventInfoID );
			break;
		}
	}
}

void CColorFilterEventProperty::ClearTempData( void )
{
	m_iMyEventInfoID = -1;
}