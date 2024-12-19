#include "StdAfx.h"
#include "VectorProperty.h"
#include "ParticleEventProperty.h"
#include "CommandSet.h"
#include "ToolData.h"
#include <wx/msgdlg.h>



CParticleEventProperty::CParticleEventProperty( wxWindow* pParent, int id ) : IPropertyDelegate( pParent, id ),
																			  m_iMyEventInfoID( -1 )
{
	wxPGId CategoryID = m_pPropGrid->AppendCategory( wxT("Particle Event Property") );
	m_pPropGrid->SetPropertyHelpString( CategoryID, wxT("Set Particle Event Property") );

	m_aPGID[ PARTICLE_NAME ] = m_pPropGrid->Append( wxStringProperty( wxT("Name"), wxPG_LABEL, wxT("") ) );
	
	m_aPGID[ IMPORT_FILE_PATH ] = m_pPropGrid->Append( wxFileProperty( wxT("Import File Name"), wxT("") ) );
	m_pPropGrid->SetPropertyAttribute( m_aPGID[ IMPORT_FILE_PATH ], wxPG_FILE_WILDCARD, wxT("Particle Data File(*.ptc)|*.ptc") );
	m_pPropGrid->SetPropertyAttribute( m_aPGID[ IMPORT_FILE_PATH ], wxPG_FILE_DIALOG_TITLE, wxT("Select Particle Data File") );
	m_pPropGrid->SetPropertyAttribute( m_aPGID[ IMPORT_FILE_PATH ], wxPG_FILE_SHOW_FULL_PATH, 0 );

	m_aPGID[ ID ] = m_pPropGrid->Append( wxUIntProperty( wxT("ID"), wxPG_LABEL, 0 ) );
	m_aPGID[ POSITION ] = m_pPropGrid->Append( VectorProperty( wxT("Position"), wxPG_LABEL, EtVector3( 0.0f, 0.0f, 0.0f ) ) );
	m_aPGID[ START_TIME ] = m_pPropGrid->Append( wxFloatProperty( wxT("Start Time"), wxPG_LABEL, 0 ) );
	//m_aPGID[ SPEED ] = m_pPropGrid->Append( wxFloatProperty( wxT("Speed"), wxPG_LABEL, 0.0f ) );
	m_aPGID[ TIME_LENGTH ] = m_pPropGrid->Append( wxFloatProperty( wxT("Time Length"), wxPG_LABEL, 0 ) );

	m_pPropGrid->EnableProperty( m_aPGID[ ID ], false );
}

CParticleEventProperty::~CParticleEventProperty(void)
{
}


void CParticleEventProperty::Initialize( void )
{

}


void CParticleEventProperty::_UpdateProp( int iEventInfoID )
{
	if( wxNOT_FOUND != iEventInfoID )
	{
		const ParticleEventInfo* pParticleEventInfo = static_cast<const ParticleEventInfo*>( TOOL_DATA.GetEventInfoByID(iEventInfoID) );
		
		wxCSConv MBConv( wxFONTENCODING_CP949 );
		wxChar caWBuf[ 256 ];

		ZeroMemory( caWBuf, sizeof(caWBuf) );
		MBConv.MB2WC( caWBuf, pParticleEventInfo->strEventName.c_str(), 256 );
		m_pPropGrid->SetPropertyValueString( m_aPGID[ PARTICLE_NAME ], caWBuf );

		ZeroMemory( caWBuf, sizeof(caWBuf) );
		MBConv.MB2WC( caWBuf, pParticleEventInfo->strImportFilePath.c_str(), 256 );
		m_pPropGrid->SetPropertyValueString( m_aPGID[ IMPORT_FILE_PATH ], caWBuf );

		m_pPropGrid->SetPropertyValueLong( m_aPGID[ ID ], pParticleEventInfo->iID );

		wxVariantData_EtVector3* pData = new wxVariantData_EtVector3(pParticleEventInfo->vPos);
		wxVariant Value( pData, wxT("EtVector3") );
		m_pPropGrid->SetPropertyValue( m_aPGID[ POSITION ], Value );

		m_pPropGrid->SetPropertyValueDouble( m_aPGID[ START_TIME ], pParticleEventInfo->fStartTime );
		//m_pPropGrid->SetPropertyValueDouble( m_aPGID[ SPEED ], pParticleEventInfo->fSpeed );
		m_pPropGrid->SetPropertyValueDouble( m_aPGID[ TIME_LENGTH ], pParticleEventInfo->fTimeLength );

		m_iMyEventInfoID = pParticleEventInfo->iID;
	}
}


void CParticleEventProperty::OnShow( void )
{
	int iSelectedObjectID = TOOL_DATA.GetSelectedObjectID();
	if( wxNOT_FOUND != iSelectedObjectID )
	{
		_UpdateProp( iSelectedObjectID );
	}
}



void CParticleEventProperty::_GetPropertyValue( EventInfo* pEventInfo )
{
	ParticleEventInfo* pParticleEventInfo = static_cast<ParticleEventInfo*>(pEventInfo);

	wxCSConv MBConv( wxFONTENCODING_CP949 );
	char caBuf[ 256 ];

	wxString strTemp = m_pPropGrid->GetPropertyValueAsString( m_aPGID[ PARTICLE_NAME ] );
	ZeroMemory( caBuf, sizeof(caBuf) );
	MBConv.WC2MB( caBuf, strTemp.c_str(), 256 );
	pParticleEventInfo->strEventName.assign( caBuf );

	strTemp = m_pPropGrid->GetPropertyValueAsString( m_aPGID[ IMPORT_FILE_PATH ] );
	ZeroMemory( caBuf, sizeof(caBuf) );
	MBConv.WC2MB( caBuf, strTemp.c_str(), 256 );

	//pParticleEventInfo->fSpeed = (float)m_pPropGrid->GetPropertyValueAsDouble( m_aPGID[ SPEED ] );

	// TODO: 새롭게 파일이 로딩 되었다면 파일을 실제로 로딩
	if( false == strTemp.IsEmpty() &&
		pParticleEventInfo->strImportFilePath != caBuf )
	{	
		// 패스를 잘라서 파일 이름.확장자도 저장
		char caFileName[ MAX_PATH ];
		char caFileExt[ 32 ];
		_splitpath_s( caBuf, NULL, 0, NULL, 0, caFileName, MAX_PATH, caFileExt, 32 );

		pParticleEventInfo->strImportFileName.assign( caFileName ).append( caFileExt );
		pParticleEventInfo->strImportFilePath.assign( caBuf );

		if( -1 != pParticleEventInfo->iParticleDataIndex )
			EternityEngine::DeleteParticleData( pParticleEventInfo->iParticleDataIndex );

		pParticleEventInfo->iParticleDataIndex = EternityEngine::LoadParticleData( pParticleEventInfo->strImportFileName.c_str() );
		if( -1 == pParticleEventInfo->iParticleDataIndex )
		{
			wxString strMsg;
			strMsg.Printf( wxT("Can't find %s in resource path."), strTemp.c_str() );
			wxMessageBox( strMsg, wxT("Particle Data Load Failed!"), wxOK | wxCENTRE | wxICON_ERROR );
		}

		// 시간 길이도 셋팅.
		EtBillboardEffectHandle hParticle = EternityEngine::CreateBillboardEffect( pParticleEventInfo->iParticleDataIndex, NULL );
		EtBillboardEffectDataHandle hData = hParticle->GetBillboardEffectData();
		
		// 파티클 라이프 타임은 각 파티클의 라이프 타임이므로 마지막에 생성된 애가 사라지기까지는 결국 파티클 라이프 타임의 두 배가 된다.
		pParticleEventInfo->fTimeLength = (hData->GetMaxBillboardEffectLife()*2 / pParticleEventInfo->fSpeed);
		m_pPropGrid->SetPropertyValueDouble( m_aPGID[ TIME_LENGTH ], pParticleEventInfo->fTimeLength );
		hData->Release();
		hParticle->Release();
	}
	else
		pParticleEventInfo->fTimeLength = 0.0f;

	pParticleEventInfo->strImportFilePath.assign( caBuf );

	pParticleEventInfo->iID = m_pPropGrid->GetPropertyValueAsLong( m_aPGID[ ID ] );

	wxVariant value = m_pPropGrid->GetPropertyValue( m_aPGID[ POSITION ] );
	wxVariantData_EtVector3* pData = static_cast<wxVariantData_EtVector3*>(value.GetData());
	const EtVector3 vPos = pData->GetValue();
	pParticleEventInfo->vPos = vPos;

	pParticleEventInfo->fStartTime = m_pPropGrid->GetPropertyValueAsDouble( m_aPGID[ START_TIME ] );
	//pParticleEventInfo->fUnit = 10.0f;
}



void CParticleEventProperty::OnPropertyChanged( wxPropertyGridEvent& PGEvent )
{
	const EventInfo* pParticleEvent = TOOL_DATA.GetEventInfoByID( m_iMyEventInfoID );
	EventInfo* pNewParticleEvent = pParticleEvent->clone();

	_GetPropertyValue( pNewParticleEvent );

	CEventPropChange EventChange( &TOOL_DATA, pNewParticleEvent );
	TOOL_DATA.RunCommand( &EventChange );

	delete pNewParticleEvent;
}


void CParticleEventProperty::CommandPerformed( ICommand* pCommand )
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



// TODO: 호출해주는 부분 반드시 코딩해야함~ 아직 없음~
void CParticleEventProperty::ClearTempData( void )
{
	m_iMyEventInfoID = -1;
}