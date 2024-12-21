#include "StdAfx.h"
#include "VectorProperty.h"
#include "SoundEventProperty.h"
#include "CommandSet.h"
#include "ToolData.h"
#include "EtSoundEngine.h"
#include <wx/msgdlg.h>



CSoundEventProperty::CSoundEventProperty( wxWindow* pParent, int id ) : IPropertyDelegate( pParent, id ),
																		m_iMyEventInfoID( -1 )
{
	wxPGId CategoryID = m_pPropGrid->AppendCategory( wxT("Sound Event Property") );
	m_pPropGrid->SetPropertyHelpString( CategoryID, wxT("Set Sound Event Property") );

	m_aPGID[ SOUND_NAME ] = m_pPropGrid->Append( wxStringProperty( wxT("Name"), wxPG_LABEL, wxT("") ) );

	m_aPGID[ IMPORT_FILE_PATH ] = m_pPropGrid->Append( wxFileProperty( wxT("Import File Name"), wxT("") ) );
	m_pPropGrid->SetPropertyAttribute( m_aPGID[ IMPORT_FILE_PATH ], wxPG_FILE_WILDCARD, wxT("Sound File(*.wav *.ogg)|*.wav;*.ogg") );
	m_pPropGrid->SetPropertyAttribute( m_aPGID[ IMPORT_FILE_PATH ], wxPG_FILE_DIALOG_TITLE, wxT("Select Sound Data File") );
	m_pPropGrid->SetPropertyAttribute( m_aPGID[ IMPORT_FILE_PATH ], wxPG_FILE_SHOW_FULL_PATH, 0 );

	m_aPGID[ ID ] = m_pPropGrid->Append( wxUIntProperty( wxT("ID"), wxPG_LABEL, 0 ) );
	m_aPGID[ VOLUME ] = m_pPropGrid->Append( wxFloatProperty( wxT("VOLUME"), wxPG_LABEL, 0 ) );
	//m_aPGID[ POSITION ] = m_pPropGrid->Append( VectorProperty( wxT("Position"), wxPG_LABEL, EtVector3( 0.0f, 0.0f, 0.0f ) ) );
	m_aPGID[ START_TIME ] = m_pPropGrid->Append( wxFloatProperty( wxT("Start Time"), wxPG_LABEL, 0 ) );
	m_aPGID[ TIME_LENGTH ] = m_pPropGrid->Append( wxFloatProperty( wxT("Time Length"), wxPG_LABEL, 0 ) );

	m_pPropGrid->EnableProperty( m_aPGID[ ID ], false );
	m_pPropGrid->EnableProperty( m_aPGID[ TIME_LENGTH ], false );
}

CSoundEventProperty::~CSoundEventProperty(void)
{
}


void CSoundEventProperty::Initialize( void )
{

}


void CSoundEventProperty::_UpdateProp( int iEventInfoID )
{
	if( wxNOT_FOUND != iEventInfoID )
	{
		const SoundEventInfo* pSoundEventInfo = static_cast<const SoundEventInfo*>( TOOL_DATA.GetEventInfoByID(iEventInfoID) );

		wxCSConv MBConv( wxFONTENCODING_CP949 );
		wxChar caWBuf[ 256 ];

		ZeroMemory( caWBuf, sizeof(caWBuf) );
		MBConv.MB2WC( caWBuf, pSoundEventInfo->strEventName.c_str(), 256 );
		m_pPropGrid->SetPropertyValueString( m_aPGID[ SOUND_NAME ], caWBuf );

		ZeroMemory( caWBuf, sizeof(caWBuf) );
		MBConv.MB2WC( caWBuf, pSoundEventInfo->strImportFilePath.c_str(), 256 );
		m_pPropGrid->SetPropertyValueString( m_aPGID[ IMPORT_FILE_PATH ], caWBuf );

		m_pPropGrid->SetPropertyValueLong( m_aPGID[ ID ], pSoundEventInfo->iID );
		m_pPropGrid->SetPropertyValueDouble( m_aPGID[ VOLUME ], pSoundEventInfo->fVolume );

		//wxVariantData_EtVector3* pData = new wxVariantData_EtVector3(pSoundEventInfo->vPos);
		//wxVariant Value( pData, wxT("EtVector3") );
		//m_pPropGrid->SetPropertyValue( m_aPGID[ POSITION ], Value );

		m_pPropGrid->SetPropertyValueDouble( m_aPGID[ START_TIME ], pSoundEventInfo->fStartTime );
		m_pPropGrid->SetPropertyValueDouble( m_aPGID[ TIME_LENGTH ], pSoundEventInfo->fTimeLength );

		m_iMyEventInfoID = pSoundEventInfo->iID;
	}
}


void CSoundEventProperty::OnShow( void )
{
	int iSelectedObjectID = TOOL_DATA.GetSelectedObjectID();
	if( wxNOT_FOUND != iSelectedObjectID )
	{
		_UpdateProp( iSelectedObjectID );
	}
}



void CSoundEventProperty::_GetPropertyValue( EventInfo* pEventInfo )
{
	SoundEventInfo* pSoundEventInfo = static_cast<SoundEventInfo*>(pEventInfo);

	wxCSConv MBConv( wxFONTENCODING_CP949 );
	char caBuf[ 256 ];

	wxString strTemp = m_pPropGrid->GetPropertyValueAsString( m_aPGID[ SOUND_NAME ] );
	ZeroMemory( caBuf, sizeof(caBuf) );
	MBConv.WC2MB( caBuf, strTemp.c_str(), 256 );
	pSoundEventInfo->strEventName.assign( caBuf );

	strTemp = m_pPropGrid->GetPropertyValueAsString( m_aPGID[ IMPORT_FILE_PATH ] );
	ZeroMemory( caBuf, sizeof(caBuf) );
	MBConv.WC2MB( caBuf, strTemp.c_str(), 256 );

	// TODO: 새롭게 파일이 로딩 되었다면 파일을 실제로 로딩
	if( false == strTemp.IsEmpty() &&
		pSoundEventInfo->strImportFilePath != caBuf )
	{	
		// 패스를 잘라서 파일 이름.확장자도 저장
		char caFileName[ MAX_PATH ];
		char caFileExt[ 32 ];
		_splitpath_s( caBuf, NULL, 0, NULL, 0, caFileName, MAX_PATH, caFileExt, 32 );

		pSoundEventInfo->strImportFilePath.assign( caBuf );
		pSoundEventInfo->strImportFileName.assign( caFileName ).append( caFileExt );

		if( -1 != pSoundEventInfo->iSoundDataIndex )
			CEtSoundEngine::GetInstance().RemoveSound( pSoundEventInfo->iSoundDataIndex );

		pSoundEventInfo->iSoundDataIndex = CEtSoundEngine::GetInstance().LoadSound( pSoundEventInfo->strImportFileName.c_str(), false, false );
		if( -1 == pSoundEventInfo->iSoundDataIndex )
		{
			wxString strMsg;
			strMsg.Printf( wxT("Can't find %s in resource path."), strTemp.c_str() );
			wxMessageBox( strMsg, wxT("Sound Data Load Failed!"), wxOK | wxCENTRE | wxICON_ERROR );
		}
	}
	//else
	//	pSoundEventInfo->fTimeLength = 0.0f;

	if( pSoundEventInfo->iSoundDataIndex != -1 )
	{
		FMOD::Sound* pSoundInfo =  CEtSoundEngine::GetInstance().FindSoundStruct( pSoundEventInfo->iSoundDataIndex )->pSound;
		if( pSoundInfo )
		{
			unsigned int uiSoundLengthMS = 0;
			pSoundInfo->getLength( &uiSoundLengthMS, FMOD_TIMEUNIT_MS );
			pSoundEventInfo->fTimeLength = (float)uiSoundLengthMS / 1000.0f;
		}
		else
			pSoundEventInfo->fTimeLength = 0.0f;
	}

	pSoundEventInfo->strImportFilePath.assign( caBuf );

	pSoundEventInfo->iID = m_pPropGrid->GetPropertyValueAsLong( m_aPGID[ ID ] );	
	pSoundEventInfo->fVolume = (float)m_pPropGrid->GetPropertyValueAsDouble( m_aPGID[ VOLUME ] );

	// 볼륨 값 클리핑
	if( pSoundEventInfo->fVolume < 0.0f )
		pSoundEventInfo->fVolume = 0.0f;
	else
	if( pSoundEventInfo->fVolume > 1.0f )
		pSoundEventInfo->fVolume = 1.0f;

	//wxVariant value = m_pPropGrid->GetPropertyValue( m_aPGID[ POSITION ] );
	//wxVariantData_EtVector3* pData = static_cast<wxVariantData_EtVector3*>(value.GetData());
	//const EtVector3 vPos = pData->GetValue();
	//pSoundEventInfo->vPos = vPos;

	pSoundEventInfo->fStartTime = m_pPropGrid->GetPropertyValueAsDouble( m_aPGID[ START_TIME ] );
	pSoundEventInfo->fUnit = 10.0f;
}



void CSoundEventProperty::OnPropertyChanged( wxPropertyGridEvent& PGEvent )
{
	const EventInfo* pSoundEvent = TOOL_DATA.GetEventInfoByID( m_iMyEventInfoID );
	EventInfo* pNewSoundEvent = pSoundEvent->clone();

	_GetPropertyValue( pNewSoundEvent );

	CEventPropChange EventChange( &TOOL_DATA, pNewSoundEvent );
	TOOL_DATA.RunCommand( &EventChange );

	delete pNewSoundEvent;
}


void CSoundEventProperty::CommandPerformed( ICommand* pCommand )
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
void CSoundEventProperty::ClearTempData( void )
{
	m_iMyEventInfoID = -1;
}