#include "stdafx.h"
#include "DofEventProperty.h"
#include "CommandSet.h"
#include "ToolData.h"
#include "RTCutSceneRenderer.h"
#include "DnCutSceneWorld.h"



CDofEventProperty::CDofEventProperty( wxWindow* pParent, int id ) : IPropertyDelegate( pParent, id ), 
																	m_iMyEventInfoID( 0 )
{
	wxPGId CategoryID = m_pPropGrid->AppendCategory( wxT("DofEventProperty") );
	m_pPropGrid->SetPropertyHelpString( CategoryID, wxT("Set Depth of Field Event Property") );

	m_aPGID[ ENABLE ] = m_pPropGrid->Append( wxBoolProperty( wxT("Enable DOF"), wxPG_LABEL, true ) );
	m_aPGID[ USE_NOW_DATA_AS_START ] = m_pPropGrid->Append( wxBoolProperty( wxT("Use Now Data As Start"), wxPG_LABEL, false ) );
	m_pPropGrid->SetPropertyAttribute( m_aPGID[ ENABLE ], wxPG_BOOL_USE_CHECKBOX, true );
	m_pPropGrid->SetPropertyAttribute( m_aPGID[ USE_NOW_DATA_AS_START ], wxPG_BOOL_USE_CHECKBOX, true );

	m_pPropGrid->AppendCategory( wxT("From Value") );
	m_aPGID[ NEAR_START_FROM ] = m_pPropGrid->Append( wxFloatProperty( wxT("Near Start From"), wxPG_LABEL, 0.0f ) );
	m_aPGID[ NEAR_END_FROM ] = m_pPropGrid->Append( wxFloatProperty( wxT("Near End From"), wxPG_LABEL, 0.0f ) );
	m_aPGID[ FAR_START_FROM ] = m_pPropGrid->Append( wxFloatProperty( wxT("Far Start From"), wxPG_LABEL, 0.0f ) );
	m_aPGID[ FAR_END_FROM ] = m_pPropGrid->Append( wxFloatProperty( wxT("Far End From"), wxPG_LABEL, 0.0f ) );
	m_aPGID[ FOCUS_DIST_FROM ] = m_pPropGrid->Append( wxFloatProperty( wxT("Focus Dist From"), wxPG_LABEL, 0.0f ) );
	m_aPGID[ NEAR_BLUR_SIZE_FROM ] = m_pPropGrid->Append( wxFloatProperty( wxT("Near Blur Size From"), wxPG_LABEL, 0.0f ) );
	m_aPGID[ FAR_BLUR_SIZE_FROM ] = m_pPropGrid->Append( wxFloatProperty( wxT("Far Blur Size From"), wxPG_LABEL, 0.0f ) );

	m_pPropGrid->AppendCategory( wxT("Dest Value") );
	m_aPGID[ NEAR_START_DEST ] = m_pPropGrid->Append( wxFloatProperty( wxT("Near Start Dest"), wxPG_LABEL, 0.0f ) );
	m_aPGID[ NEAR_END_DEST ] = m_pPropGrid->Append( wxFloatProperty( wxT("Near End Dest"), wxPG_LABEL, 0.0f ) );
	m_aPGID[ FAR_START_DEST ] = m_pPropGrid->Append( wxFloatProperty( wxT("Far Start Dest"), wxPG_LABEL, 0.0f ) );
	m_aPGID[ FAR_END_DEST ] = m_pPropGrid->Append( wxFloatProperty( wxT("Far End Dest"), wxPG_LABEL, 0.0f ) );
	m_aPGID[ FOCUS_DIST_DEST ] = m_pPropGrid->Append( wxFloatProperty( wxT("Focus Dist Dest"), wxPG_LABEL, 0.0f ) );
	m_aPGID[ NEAR_BLUR_SIZE_DEST ] = m_pPropGrid->Append( wxFloatProperty( wxT("Near Blur Size Dest"), wxPG_LABEL, 0.0f ) );
	m_aPGID[ FAR_BLUR_SIZE_DEST ] = m_pPropGrid->Append( wxFloatProperty( wxT("Far Blur Size Dest"), wxPG_LABEL, 0.0f ) );

	m_pPropGrid->AppendCategory( wxT("EventData") );
	m_aPGID[ ID ] = m_pPropGrid->Append( wxUIntProperty( wxT("ID"), wxPG_LABEL, 0 ) );
	m_aPGID[ START_TIME ] = m_pPropGrid->Append( wxFloatProperty( wxT("Start Time"), wxPG_LABEL, 0.0f ) );
	m_aPGID[ LENGTH ] = m_pPropGrid->Append( wxFloatProperty( wxT("Length"), wxPG_LABEL, 0 ) );

	m_pPropGrid->EnableProperty( m_aPGID[ ID ], false );
}

CDofEventProperty::~CDofEventProperty(void)
{
}



void CDofEventProperty::Initialize( void )
{

}


void CDofEventProperty::_UpdateProp( int iEventInfoID )
{
	if( wxNOT_FOUND != iEventInfoID )
	{
		const DofEventInfo* pDofEventInfo = static_cast<const DofEventInfo*>( TOOL_DATA.GetEventInfoByID(iEventInfoID) );

		if( pDofEventInfo->bUseNowValueAsStart )
		{
			// ù��° �̺�Ʈ���� üũ�ؼ� ù��° �̺�Ʈ�� ��� ���� ������ ��� DOF ���� ������ ���� �´�.
			int iNumEvent = TOOL_DATA.GetThisTypesEventNum( EventInfo::DOF );
			for( int iEvent = 0; iEvent < iNumEvent; ++iEvent )
			{
				const EventInfo* pEventInfo = TOOL_DATA.GetEventInfoByIndex( EventInfo::DOF, iEvent );
				if( pEventInfo->iID == pDofEventInfo->iID )
				{
					if( 0 == iEvent )
					{
						CRTCutSceneRenderer* pRenderer = TOOL_DATA.GetRenderer();
						CDnCutSceneWorld* pWorld = pRenderer->GetWorld();
						if( pWorld )
						{
							DofEventInfo* pDofEventInfoNotConst = const_cast<DofEventInfo*>(pDofEventInfo);
							pDofEventInfoNotConst->fNearStartFrom = pDofEventInfoNotConst->fNearStartDest = pWorld->GetDOFNearStart();
							pDofEventInfoNotConst->fNearEndFrom = pDofEventInfoNotConst->fNearEndDest = pWorld->GetDOFNearEnd();
							pDofEventInfoNotConst->fFarStartFrom = pDofEventInfoNotConst->fFarStartDest = pWorld->GetDOFFarStart();
							pDofEventInfoNotConst->fFarEndFrom = pDofEventInfoNotConst->fFarEndDest = pWorld->GetDOFFarEnd();
							pDofEventInfoNotConst->fFocusDistFrom = pDofEventInfoNotConst->fFocusDistDest = pWorld->GetDOFFocusDistance();
							pDofEventInfoNotConst->fNearBlurSizeFrom = pDofEventInfoNotConst->fNearBlurSizeDest = pWorld->GetDOFNearBlur();
							pDofEventInfoNotConst->fFarBlurSizeFrom = pDofEventInfoNotConst->fFarBlurSizeDest = pWorld->GetDOFFarBlur();
						}
					}
					else
					// ù ��°�� �ƴ� DOF �� ���� DOF �̺�Ʈ dest ������ ������ �޾� �����Ѵ�.
					{
						DofEventInfo* pDofEventInfoNotConst = const_cast<DofEventInfo*>(pDofEventInfo);
						const DofEventInfo* pPrevEventInfo = static_cast<const DofEventInfo*>(TOOL_DATA.GetEventInfoByIndex( EventInfo::DOF, iEvent-1 ));
						pDofEventInfoNotConst->fNearStartFrom = pDofEventInfoNotConst->fNearStartDest = pPrevEventInfo->fNearStartDest;
						pDofEventInfoNotConst->fNearEndFrom = pDofEventInfoNotConst->fNearEndDest = pPrevEventInfo->fNearEndDest;
						pDofEventInfoNotConst->fFarStartFrom = pDofEventInfoNotConst->fFarStartDest = pPrevEventInfo->fFarStartDest;
						pDofEventInfoNotConst->fFarEndFrom = pDofEventInfoNotConst->fFarEndDest = pPrevEventInfo->fFarEndDest;
						pDofEventInfoNotConst->fFocusDistFrom = pDofEventInfoNotConst->fFocusDistDest = pPrevEventInfo->fFocusDistDest;
						pDofEventInfoNotConst->fNearBlurSizeFrom = pDofEventInfoNotConst->fNearBlurSizeDest = pPrevEventInfo->fNearBlurSizeDest;
						pDofEventInfoNotConst->fFarBlurSizeFrom = pDofEventInfoNotConst->fFarBlurSizeDest = pPrevEventInfo->fFarBlurSizeDest;
					}
				}
			}
		}
		
		m_pPropGrid->SetPropertyValue( m_aPGID[ ENABLE ], pDofEventInfo->bEnable );

		m_pPropGrid->SetPropertyValue( m_aPGID[ USE_NOW_DATA_AS_START ], pDofEventInfo->bUseNowValueAsStart );

		m_pPropGrid->SetPropertyValue( m_aPGID[ NEAR_START_FROM ], pDofEventInfo->fNearStartFrom );
		m_pPropGrid->SetPropertyValue( m_aPGID[ NEAR_END_FROM ], pDofEventInfo->fNearEndFrom );
		m_pPropGrid->SetPropertyValue( m_aPGID[ FAR_START_FROM ], pDofEventInfo->fFarStartFrom );
		m_pPropGrid->SetPropertyValue( m_aPGID[ FAR_END_FROM ], pDofEventInfo->fFarEndFrom );
		m_pPropGrid->SetPropertyValue( m_aPGID[ FOCUS_DIST_FROM ], pDofEventInfo->fFocusDistFrom );
		m_pPropGrid->SetPropertyValue( m_aPGID[ NEAR_BLUR_SIZE_FROM ], pDofEventInfo->fNearBlurSizeFrom );
		m_pPropGrid->SetPropertyValue( m_aPGID[ FAR_BLUR_SIZE_FROM ], pDofEventInfo->fFarBlurSizeFrom );

		m_pPropGrid->SetPropertyValue( m_aPGID[ NEAR_START_DEST ], pDofEventInfo->fNearStartDest );
		m_pPropGrid->SetPropertyValue( m_aPGID[ NEAR_END_DEST ], pDofEventInfo->fNearEndDest );
		m_pPropGrid->SetPropertyValue( m_aPGID[ FAR_START_DEST ], pDofEventInfo->fFarStartDest );
		m_pPropGrid->SetPropertyValue( m_aPGID[ FAR_END_DEST ], pDofEventInfo->fFarEndDest );
		m_pPropGrid->SetPropertyValue( m_aPGID[ FOCUS_DIST_DEST ], pDofEventInfo->fFocusDistDest );
		m_pPropGrid->SetPropertyValue( m_aPGID[ NEAR_BLUR_SIZE_DEST ], pDofEventInfo->fNearBlurSizeDest );
		m_pPropGrid->SetPropertyValue( m_aPGID[ FAR_BLUR_SIZE_DEST ], pDofEventInfo->fFarBlurSizeDest );

		if( pDofEventInfo->bEnable )
		{
			if( pDofEventInfo->bUseNowValueAsStart )
			{
				m_pPropGrid->EnableProperty( m_aPGID[ NEAR_START_FROM ], false );
				m_pPropGrid->EnableProperty( m_aPGID[ NEAR_END_FROM ], false );
				m_pPropGrid->EnableProperty( m_aPGID[ FAR_START_FROM ], false );
				m_pPropGrid->EnableProperty( m_aPGID[ FAR_END_FROM ], false );
				m_pPropGrid->EnableProperty( m_aPGID[ FOCUS_DIST_FROM ], false );
				m_pPropGrid->EnableProperty( m_aPGID[ NEAR_BLUR_SIZE_FROM ], false );
				m_pPropGrid->EnableProperty( m_aPGID[ FAR_BLUR_SIZE_FROM ], false );
			}
			else
			{
				m_pPropGrid->EnableProperty( m_aPGID[ NEAR_START_FROM ], true );
				m_pPropGrid->EnableProperty( m_aPGID[ NEAR_END_FROM ], true );
				m_pPropGrid->EnableProperty( m_aPGID[ FAR_START_FROM ], true );
				m_pPropGrid->EnableProperty( m_aPGID[ FAR_END_FROM ], true );
				m_pPropGrid->EnableProperty( m_aPGID[ FOCUS_DIST_FROM ], true );
				m_pPropGrid->EnableProperty( m_aPGID[ NEAR_BLUR_SIZE_FROM ], true );
				m_pPropGrid->EnableProperty( m_aPGID[ FAR_BLUR_SIZE_FROM ], true );
			}

			m_pPropGrid->EnableProperty( m_aPGID[ NEAR_START_DEST ], true );
			m_pPropGrid->EnableProperty( m_aPGID[ NEAR_END_DEST ], true );
			m_pPropGrid->EnableProperty( m_aPGID[ FAR_START_DEST ], true );
			m_pPropGrid->EnableProperty( m_aPGID[ FAR_END_DEST ], true );
			m_pPropGrid->EnableProperty( m_aPGID[ FOCUS_DIST_DEST ], true );
			m_pPropGrid->EnableProperty( m_aPGID[ NEAR_BLUR_SIZE_DEST ], true );
			m_pPropGrid->EnableProperty( m_aPGID[ FAR_BLUR_SIZE_DEST ], true );
		}
		else
		{
			m_pPropGrid->EnableProperty( m_aPGID[ NEAR_START_FROM ], false );
			m_pPropGrid->EnableProperty( m_aPGID[ NEAR_END_FROM ], false );
			m_pPropGrid->EnableProperty( m_aPGID[ FAR_START_FROM ], false );
			m_pPropGrid->EnableProperty( m_aPGID[ FAR_END_FROM ], false );
			m_pPropGrid->EnableProperty( m_aPGID[ FOCUS_DIST_FROM ], false );
			m_pPropGrid->EnableProperty( m_aPGID[ NEAR_BLUR_SIZE_FROM ], false );
			m_pPropGrid->EnableProperty( m_aPGID[ FAR_BLUR_SIZE_FROM ], false );

			m_pPropGrid->EnableProperty( m_aPGID[ NEAR_START_DEST ], false );
			m_pPropGrid->EnableProperty( m_aPGID[ NEAR_END_DEST ], false );
			m_pPropGrid->EnableProperty( m_aPGID[ FAR_START_DEST ], false );
			m_pPropGrid->EnableProperty( m_aPGID[ FAR_END_DEST ], false );
			m_pPropGrid->EnableProperty( m_aPGID[ FOCUS_DIST_DEST ], false );
			m_pPropGrid->EnableProperty( m_aPGID[ NEAR_BLUR_SIZE_DEST ], false );
			m_pPropGrid->EnableProperty( m_aPGID[ FAR_BLUR_SIZE_DEST ], false );
		}
		
		m_pPropGrid->SetPropertyValue( m_aPGID[ ID ], pDofEventInfo->iID );
		m_pPropGrid->SetPropertyValueDouble( m_aPGID[ START_TIME ], pDofEventInfo->fStartTime );
		m_pPropGrid->SetPropertyValueDouble( m_aPGID[ LENGTH ], pDofEventInfo->fTimeLength );

		m_iMyEventInfoID = pDofEventInfo->iID;
	}
}


void CDofEventProperty::OnPropertyChanged( wxPropertyGridEvent& PGEvent )
{
	const EventInfo* pDofEvent = TOOL_DATA.GetEventInfoByID( m_iMyEventInfoID );
	EventInfo* pNewDofEvent = pDofEvent->clone();

	_GetPropertyValue( pNewDofEvent );

	CEventPropChange EventChange( &TOOL_DATA, pNewDofEvent );
	TOOL_DATA.RunCommand( &EventChange );

	delete pNewDofEvent;
}


void CDofEventProperty::OnShow( void )
{
	int iSelectedObjectID = TOOL_DATA.GetSelectedObjectID();
	if( wxNOT_FOUND != iSelectedObjectID )
	{
		_UpdateProp( iSelectedObjectID );
	}
}



void CDofEventProperty::_GetPropertyValue( EventInfo* pEventInfo )
{
	DofEventInfo* pDofEventInfo = static_cast<DofEventInfo*>(pEventInfo);

	pDofEventInfo->bEnable = m_pPropGrid->GetPropertyValueAsBool( m_aPGID[ ENABLE ] );
	pDofEventInfo->bUseNowValueAsStart = m_pPropGrid->GetPropertyValueAsBool( m_aPGID[ USE_NOW_DATA_AS_START ] );

	pDofEventInfo->fNearStartFrom = (float)m_pPropGrid->GetPropertyValueAsDouble( m_aPGID[ NEAR_START_FROM ] );
	pDofEventInfo->fNearEndFrom = (float)m_pPropGrid->GetPropertyValueAsDouble( m_aPGID[ NEAR_END_FROM ] );
	pDofEventInfo->fFarStartFrom = (float)m_pPropGrid->GetPropertyValueAsDouble( m_aPGID[ FAR_START_FROM ] );
	pDofEventInfo->fFarEndFrom = (float)m_pPropGrid->GetPropertyValueAsDouble( m_aPGID[ FAR_END_FROM ] );
	pDofEventInfo->fFocusDistFrom = (float)m_pPropGrid->GetPropertyValueAsDouble( m_aPGID[ FOCUS_DIST_FROM ] );
	pDofEventInfo->fNearBlurSizeFrom = (float)m_pPropGrid->GetPropertyValueAsDouble( m_aPGID[ NEAR_BLUR_SIZE_FROM ] );
	pDofEventInfo->fFarBlurSizeFrom = (float)m_pPropGrid->GetPropertyValueAsDouble( m_aPGID[ FAR_BLUR_SIZE_FROM ] );

	pDofEventInfo->fNearStartDest = (float)m_pPropGrid->GetPropertyValueAsDouble( m_aPGID[ NEAR_START_DEST ] );
	pDofEventInfo->fNearEndDest = (float)m_pPropGrid->GetPropertyValueAsDouble( m_aPGID[ NEAR_END_DEST ] );
	pDofEventInfo->fFarStartDest = (float)m_pPropGrid->GetPropertyValueAsDouble( m_aPGID[ FAR_START_DEST ] );
	pDofEventInfo->fFarEndDest = (float)m_pPropGrid->GetPropertyValueAsDouble( m_aPGID[ FAR_END_DEST ] );
	pDofEventInfo->fFocusDistDest = (float)m_pPropGrid->GetPropertyValueAsDouble( m_aPGID[ FOCUS_DIST_DEST ] );
	pDofEventInfo->fNearBlurSizeDest = (float)m_pPropGrid->GetPropertyValueAsDouble( m_aPGID[ NEAR_BLUR_SIZE_DEST ] );
	pDofEventInfo->fFarBlurSizeDest = (float)m_pPropGrid->GetPropertyValueAsDouble( m_aPGID[ FAR_BLUR_SIZE_DEST ] );
	pDofEventInfo->iID = m_pPropGrid->GetPropertyValueAsInt( m_aPGID[ ID ] );
	
	//pDofEventInfo->fUnit = 10.0f;
	pDofEventInfo->fStartTime = m_pPropGrid->GetPropertyValueAsDouble( m_aPGID[ START_TIME ] );
	pDofEventInfo->fTimeLength = m_pPropGrid->GetPropertyValueAsDouble( m_aPGID[ LENGTH ] );
}

void CDofEventProperty::CommandPerformed( ICommand* pCommand )
{
	// �ڽ��� ID �� �ƴϸ� �����
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


void CDofEventProperty::ClearTempData( void )
{
	m_iMyEventInfoID = -1;
}