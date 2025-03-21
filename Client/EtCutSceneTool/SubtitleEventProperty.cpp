#include "stdafx.h"
#include "SubtitleEventProperty.h"
#include "CommandSet.h"
#include "ToolData.h"
#include <wx/propgrid/propgrid.h>
#include <wx/propgrid/advprops.h>



CSubtitleEventProperty::CSubtitleEventProperty( wxWindow* pParent, int id ) : IPropertyDelegate( pParent, id ),
																			  m_iMyEventInfoID( -1 )
{
	wxPGId CategoryID = m_pPropGrid->AppendCategory( wxT("Subtitle Property") );
	m_pPropGrid->SetPropertyHelpString( CategoryID, wxT("Set Subtitle Property") );
	
	m_aPGID[ ID ] = m_pPropGrid->Append( wxUIntProperty( wxT("ID"), wxPG_LABEL, 0 ) );
	m_aPGID[ UISTRING_ID ] = m_pPropGrid->Append( wxIntProperty( wxT("UIString ID") ) );
	//m_aPGID[ COLOR ] = m_pPropGrid->Append( wxColourProperty( wxT("Color"), wxPG_LABEL, *wxBLACK ) );
	m_aPGID[ START_TIME ] = m_pPropGrid->Append( wxFloatProperty( wxT("Start Time"), wxPG_LABEL, 0 ) );
	m_aPGID[ TIME_LENGTH ] = m_pPropGrid->Append( wxFloatProperty( wxT("Length"), wxPG_LABEL, 0 ) );

	m_pPropGrid->EnableProperty( m_aPGID[ ID ], false );
}

CSubtitleEventProperty::~CSubtitleEventProperty(void)
{

}


void CSubtitleEventProperty::Initialize( void )
{

}


void CSubtitleEventProperty::OnShow( void )
{
	int iSelectedObjectID = TOOL_DATA.GetSelectedObjectID();
	if( wxNOT_FOUND != iSelectedObjectID )
	{
		_UpdateProp( iSelectedObjectID );
	}
}


void CSubtitleEventProperty::_UpdateProp( int iEventInfoID )
{
	if( wxNOT_FOUND != iEventInfoID )
	{
		const SubtitleEventInfo* pSubtitleEventInfo = static_cast<const SubtitleEventInfo*>(TOOL_DATA.GetEventInfoByID(iEventInfoID));

		m_pPropGrid->SetPropertyValue( m_aPGID[ ID ], pSubtitleEventInfo->iID );
		m_pPropGrid->SetPropertyValue( m_aPGID[ UISTRING_ID ], pSubtitleEventInfo->iUIStringID );
		//wxColour Color( (pSubtitleEventInfo->dwColor >> 16), (pSubtitleEventInfo->dwColor >> 8), pSubtitleEventInfo->dwColor, (pSubtitleEventInfo->dwColor >> 24) );
		//m_pPropGrid->SetPropertyValue( m_aPGID[ COLOR ], Color );
		m_pPropGrid->SetPropertyValueDouble( m_aPGID[ START_TIME ], pSubtitleEventInfo->fStartTime );
		m_pPropGrid->SetPropertyValueDouble( m_aPGID[ TIME_LENGTH ], pSubtitleEventInfo->fTimeLength );

		m_iMyEventInfoID = pSubtitleEventInfo->iID;
	}
}


void CSubtitleEventProperty::_GetPropertyValue( EventInfo* pEventInfo, wxPropertyGridEvent& PGEvent )
{
	SubtitleEventInfo* pSubtitleEventInfo = static_cast<SubtitleEventInfo*>(pEventInfo);
	pSubtitleEventInfo->iID = m_pPropGrid->GetPropertyValueAsInt( m_aPGID[ ID ] );
	pSubtitleEventInfo->iUIStringID = m_pPropGrid->GetPropertyValueAsInt( m_aPGID[ UISTRING_ID ] );
	//wxVariantData_wxColour* pData = static_cast<wxVariantData_wxColour*>(value.GetData());
	//const wxColour Color = pData->GetValue();
	//wxVariant Value = m_pPropGrid->GetPropertyValue( m_aPGID[ COLOR ] );
 //	wxObject* pData = Value.GetWxObjectPtr();

	//// 아 짱나 버그있네 이거 일단 냅두자. -_- 따로 RGB 입력하게 하던가 해야지 원...
	//if( PGEvent.GetProperty() == m_aPGID[ COLOR ] )
	//{
	//	wxColour Color = *((wxColour*)m_pPropGrid->GetPropertyValueAsWxObjectPtr( m_aPGID[ COLOR ] ));
	//	pSubtitleEventInfo->dwColor = D3DCOLOR_ARGB( Color.Alpha(), Color.Red(), Color.Green(), Color.Blue() );
	//}
	//wxVariantData_wxColour* pColor = static_cast<wxVariantData_wxColour*>( Value.GetData() );
	//wxColourPropertyValue& Color = *wxGetVariantCast(Value, wxColourPropertyValue);
	pSubtitleEventInfo->fStartTime = m_pPropGrid->GetPropertyValueAsDouble( m_aPGID[ START_TIME ] );
	pSubtitleEventInfo->fTimeLength = m_pPropGrid->GetPropertyValueAsDouble( m_aPGID[ TIME_LENGTH ] );
}


void CSubtitleEventProperty::OnPropertyChanged( wxPropertyGridEvent& PGEvent )
{
	const EventInfo* pSubtitleEventInfo = TOOL_DATA.GetEventInfoByID( m_iMyEventInfoID );
	EventInfo* pNewSubtitleEventInfo = pSubtitleEventInfo->clone();
	_GetPropertyValue( pNewSubtitleEventInfo, PGEvent );

	CEventPropChange EventChange( &TOOL_DATA, pNewSubtitleEventInfo );
	TOOL_DATA.RunCommand( &EventChange );

	delete pNewSubtitleEventInfo;
}


void CSubtitleEventProperty::ClearTempData( void )
{
	m_iMyEventInfoID = -1;
}


void CSubtitleEventProperty::CommandPerformed( ICommand* pCommand )
{
	// 자신의 id 가 사라지면 숨긴다.
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

				// 액터가 추가된 경우 액터 리스트를 업데이트
			case CMD_REGISTER_RES:
				_UpdateProp( m_iMyEventInfoID );
				break;
		}
	}
}