#include "stdafx.h"
#include "FadeEventProperty.h"
#include "CommandSet.h"
#include "ToolData.h"




CFadeEventProperty::CFadeEventProperty( wxWindow* pParent, int id ) : IPropertyDelegate( pParent, id ), 
																	  m_iMyEventInfoID( -1 )
{
	wxPGId CategoryID = m_pPropGrid->AppendCategory( wxT("Fade Event Property") );
	m_pPropGrid->SetPropertyHelpString( CategoryID, wxT("Set Fade In/Out Property") );

	wxArrayString FadeArray;
	FadeArray.Add( wxT("Fade In") );
	FadeArray.Add( wxT("Fade Out") );
	m_aPGID[ FADE_KIND ] = m_pPropGrid->Append( wxEnumProperty( wxT("Kind"), wxPG_LABEL, FadeArray ) );
	m_aPGID[ START_TIME ] = m_pPropGrid->Append( wxFloatProperty( wxT("Start Time"), wxPG_LABEL, 0 ) );
	m_aPGID[ COLOR_RED ] = m_pPropGrid->Append( wxUIntProperty( wxT("Color_Red"), wxPG_LABEL, 0 ) );
	m_aPGID[ COLOR_GREEN ] = m_pPropGrid->Append( wxUIntProperty( wxT("Color_Green"), wxPG_LABEL, 0 ) );
	m_aPGID[ COLOR_BLUE ] = m_pPropGrid->Append( wxUIntProperty( wxT("Color_Blue"), wxPG_LABEL, 0 ) );
	m_aPGID[ LENGTH ] = m_pPropGrid->Append( wxFloatProperty( wxT("Length"), wxPG_LABEL, 0 ) );

	m_pPropGrid->SetPropertyValue( m_aPGID[ FADE_KIND ], 0 );
}

CFadeEventProperty::~CFadeEventProperty(void)
{
}


void CFadeEventProperty::Initialize( void )
{

}


void CFadeEventProperty::_UpdateProp( int iEventInfoID )
{
	if( wxNOT_FOUND != iEventInfoID )
	{
		const FadeEventInfo* pFadeEventInfo = static_cast<const FadeEventInfo*>(TOOL_DATA.GetEventInfoByID(iEventInfoID));

		m_pPropGrid->SetPropertyValue( m_aPGID[ FADE_KIND ], pFadeEventInfo->iFadeKind );
		m_pPropGrid->SetPropertyValueDouble( m_aPGID[ START_TIME ], pFadeEventInfo->fStartTime);
		m_pPropGrid->SetPropertyValueLong( m_aPGID[ COLOR_RED ], pFadeEventInfo->iColorRed );
		m_pPropGrid->SetPropertyValueLong( m_aPGID[ COLOR_GREEN ], pFadeEventInfo->iColorGreen );
		m_pPropGrid->SetPropertyValueLong( m_aPGID[ COLOR_BLUE ], pFadeEventInfo->iColorBlue );
		m_pPropGrid->SetPropertyValueDouble( m_aPGID[ LENGTH ], pFadeEventInfo->fTimeLength );

		m_iMyEventInfoID = pFadeEventInfo->iID;
	}
}


void CFadeEventProperty::OnShow( void )
{
	int iSelectedObjectID = TOOL_DATA.GetSelectedObjectID();
	if( wxNOT_FOUND != iSelectedObjectID )
	{
		_UpdateProp( iSelectedObjectID );
	}
}



void CFadeEventProperty::_GetPropertyValue( EventInfo* pEventInfo )
{
	FadeEventInfo* pFadeEventInfo = static_cast<FadeEventInfo*>(pEventInfo);

	pFadeEventInfo->fUnit = 10.0f;

	pFadeEventInfo->iFadeKind = m_pPropGrid->GetPropertyValueAsInt( m_aPGID[ FADE_KIND ] );
	pFadeEventInfo->fStartTime = m_pPropGrid->GetPropertyValueAsDouble( m_aPGID[ START_TIME ] );
	pFadeEventInfo->iColorRed = m_pPropGrid->GetPropertyValueAsInt( m_aPGID[ COLOR_RED ] );
	pFadeEventInfo->iColorGreen = m_pPropGrid->GetPropertyValueAsInt( m_aPGID[ COLOR_GREEN ] );
	pFadeEventInfo->iColorBlue = m_pPropGrid->GetPropertyValueAsInt( m_aPGID[ COLOR_BLUE ] );
	pFadeEventInfo->fTimeLength = m_pPropGrid->GetPropertyValueAsDouble( m_aPGID[ LENGTH ] );

	// 컬러 값 클리핑
	if( 255 < pFadeEventInfo->iColorRed )
		pFadeEventInfo->iColorRed = 255;
	else
	if( pFadeEventInfo->iColorRed < 0 )
		pFadeEventInfo->iColorRed = 0;

	if( 255 < pFadeEventInfo->iColorGreen )
		pFadeEventInfo->iColorGreen = 255;
	else
	if( pFadeEventInfo->iColorGreen < 0 )
		pFadeEventInfo->iColorGreen = 0;

	if( 255 < pFadeEventInfo->iColorBlue )
		pFadeEventInfo->iColorBlue = 255;
	else
	if( pFadeEventInfo->iColorBlue < 0 )
		pFadeEventInfo->iColorBlue = 0;
}



void CFadeEventProperty::OnPropertyChanged( wxPropertyGridEvent& PGEvent )
{
	const EventInfo* pFadeEventInfo = TOOL_DATA.GetEventInfoByID( m_iMyEventInfoID );
	EventInfo* pNewFadeEventInfo = pFadeEventInfo->clone();

	_GetPropertyValue( pNewFadeEventInfo );

	CEventPropChange EventChange( &TOOL_DATA, pNewFadeEventInfo );
	TOOL_DATA.RunCommand( &EventChange );

	delete pNewFadeEventInfo;
}


void CFadeEventProperty::CommandPerformed( ICommand* pCommand )
{
	// 자신의 ID 가 사라지면 숨긴다
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


void CFadeEventProperty::ClearTempData( void )
{
	m_iMyEventInfoID = -1;
}
