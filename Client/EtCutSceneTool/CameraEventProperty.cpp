#include "stdafx.h"
#include "VectorProperty.h"
#include "CameraEventProperty.h"
#include "CommandSet.h"
#include "ToolData.h"
#include "RTCutSceneMakerFrame.h"

#define DTRACE_NONE wxT("None")
#define DTRACE_ACTOR wxT("Actor")
//#define DTRACE_ACTOR_ONLY_TARGET wxT("Actor(Only CamTarget)")




CCameraEventProperty::CCameraEventProperty( wxWindow* pParent, int id ) : IPropertyDelegate( pParent, id ),
																		  m_iMyEventInfoID( -1 )
{
	wxPGId CategoryID = m_pPropGrid->AppendCategory( wxT("Max Camera Property") );
	m_pPropGrid->SetPropertyHelpString( CategoryID, wxT("Set Max Camera Property") );

	m_aPGID[ CAMERA_NAME ] = m_pPropGrid->Append( wxStringProperty( wxT("Name"), wxPG_LABEL, wxT("") ) );
	m_aPGID[ ID ] = m_pPropGrid->Append( wxUIntProperty( wxT("ID"), wxPG_LABEL, 0 ) );
	m_aPGID[ IMPORT_FILE_PATH ] = m_pPropGrid->Append( wxFileProperty( wxT("Import File Name"), wxT("") ) );
	m_pPropGrid->SetPropertyAttribute( m_aPGID[ IMPORT_FILE_PATH ], wxPG_FILE_WILDCARD, wxT("Camera Data File(*.cam)|*.cam") );
	m_pPropGrid->SetPropertyAttribute( m_aPGID[ IMPORT_FILE_PATH ], wxPG_FILE_DIALOG_TITLE, wxT("Select Camera Data File") );
	m_pPropGrid->SetPropertyAttribute( m_aPGID[ IMPORT_FILE_PATH ], wxPG_FILE_SHOW_FULL_PATH, 0 );

	m_aPGID[ START_TIME ] = m_pPropGrid->Append( wxFloatProperty( wxT("Start Time" ), wxPG_LABEL, 0 ) );
	m_aPGID[ SPEED ] = m_pPropGrid->Append( wxFloatProperty( wxT("Play Speed" ), wxPG_LABEL, 0.0f ) );
	m_aPGID[ TIME_LENGTH ] = m_pPropGrid->Append( wxFloatProperty( wxT( "Length" ), wxPG_LABEL, 0 ) );
	m_aPGID[ START_OFFSET ] = m_pPropGrid->Append( VectorProperty( wxT("Start Offset"), wxPG_LABEL, EtVector3( 0.0f, 0.0f, 0.0f ) ) );

	m_pPropGrid->EnableProperty( m_aPGID[ ID ], false );
	m_pPropGrid->EnableProperty( m_aPGID[ TIME_LENGTH ], false );

	m_aPGID[ ACTOR_CATEGORY ] = m_pPropGrid->AppendCategory( wxT("Actor Show/Hide Property") );
}

CCameraEventProperty::~CCameraEventProperty(void)
{

}


void CCameraEventProperty::Initialize( void )
{

}


void CCameraEventProperty::OnShow( void )
{
	int iSelectedObjectID = TOOL_DATA.GetSelectedObjectID();
	if( wxNOT_FOUND != iSelectedObjectID )
	{
		_UpdateProp( iSelectedObjectID );
	}
}



void CCameraEventProperty::_GetPropertyValue( EventInfo* pEventInfo )
{
	CamEventInfo* pCamEventInfo = static_cast<CamEventInfo*>(pEventInfo);

	wxCSConv MBConv( wxFONTENCODING_CP949 );
	char caBuf[ 256 ];

	wxString strTemp = m_pPropGrid->GetPropertyValueAsString( m_aPGID[ CAMERA_NAME ] );
	ZeroMemory( caBuf, sizeof(caBuf) );
	MBConv.WC2MB( caBuf, strTemp.c_str(), 256 );
	pCamEventInfo->strEventName.assign( caBuf );

	pCamEventInfo->iID = m_pPropGrid->GetPropertyValueAsInt( m_aPGID[ ID ] );

	strTemp = m_pPropGrid->GetPropertyValueAsString( m_aPGID[ IMPORT_FILE_PATH ] );
	ZeroMemory( caBuf, sizeof(caBuf) );
	MBConv.WC2MB( caBuf, strTemp.c_str(), 256 );

	if( strlen(caBuf) > 0 )
	{
		CFileStream FileStream( caBuf );
		if( FileStream.IsValid() )
		{
			pCamEventInfo->pCameraData->Clear();
			pCamEventInfo->pCameraData->LoadResource( &FileStream );
			pCamEventInfo->strImportFilePath.assign( caBuf );
			
			char caFileName[ MAX_PATH ];
			char caFileExt[ 32 ];
			_splitpath( pCamEventInfo->strImportFilePath.c_str(), NULL, NULL, caFileName, caFileExt );
			pCamEventInfo->strImportFileName.assign( caFileName );
			pCamEventInfo->strImportFileName.append( "." );
			pCamEventInfo->strImportFileName.append( caFileExt );
		}
		// 파일이 없어도 데이터 클리어하지 말자. 불편하다고 해서. ㅎㅎ
		//else
		//{
		//	// import 한 카메라 파일이 있던 위치에 파일이 없으므로 
		//	// 데이터는 클리어 시키고,, 파일을 다시 import 해달라고 다이얼로그를 띄워준다.
		//	wxString strMessage;

		//	wxCSConv MBConv( wxFONTENCODING_CP949 );
		//	wxChar caWBuf[ 256 ];
		//	ZeroMemory( caWBuf, sizeof(caWBuf) );
		//	MBConv.MB2WC( caWBuf, pCamEventInfo->strImportFileName.c_str(), 256 );
		//	
		//	strMessage.Printf( wxT("[%s] path can not found.\nRe-Import camera file."), caWBuf );
		//	wxMessageBox( strMessage, wxT("Warnning"), wxCENTRE | wxICON_ERROR );

		//	pCamEventInfo->strImportFilePath.clear();
		//	pCamEventInfo->strImportFileName.clear();
		//	pCamEventInfo->pCameraData->Clear();
		//}
	}
	else
	{
		pCamEventInfo->strImportFilePath.clear();
		pCamEventInfo->strImportFileName.clear();
		pCamEventInfo->pCameraData->Clear();
	}

	float fStartTime = m_pPropGrid->GetPropertyValueAsDouble( m_aPGID[ START_TIME ] );
	pCamEventInfo->fStartTime = fStartTime;

	pCamEventInfo->fSpeed = (float)m_pPropGrid->GetPropertyValueAsDouble( m_aPGID[ SPEED ] );
	if( pCamEventInfo->fSpeed <= 0.0f )
		pCamEventInfo->fSpeed = 0.1f;
	float fTimeLength = m_pPropGrid->GetPropertyValueAsDouble( m_aPGID[ TIME_LENGTH ] );
	pCamEventInfo->fTimeLength = fTimeLength;

	wxVariant value = m_pPropGrid->GetPropertyValue( m_aPGID[ START_OFFSET ] );
	wxVariantData_EtVector3* pData = static_cast<wxVariantData_EtVector3*>(value.GetData());
	const EtVector3 vPos = pData->GetValue();
	pCamEventInfo->vCamStartOffset = vPos;

	pCamEventInfo->fUnit = 10.0f;

	// 액터들의 show/hide 여부
	// 변경사항 체크하면서 프로퍼티 그리드에 반영해준다.
	// 우선 빠진 액터들 먼저 제거한다.
	wxChar caWBuf[ 256 ];
	map<string, bool>::iterator iter = pCamEventInfo->mapActorsShowHide.begin();
	for( iter; iter != pCamEventInfo->mapActorsShowHide.end(); ++iter )
	{
		ZeroMemory( caWBuf, sizeof(caWBuf) );
		MBConv.MB2WC( caWBuf, iter->first.c_str(), 256 );
		wxPGId PGId = m_pPropGrid->GetPropertyByName( caWBuf );
		if( false == PGId.IsOk() )
			iter = pCamEventInfo->mapActorsShowHide.erase( iter );
	}

	// 새로 추가된 액터들은 추가하고 기존의 액터들은 데이터를 셋팅해준다.
	int iNumActors = TOOL_DATA.GetNumActors();
	char acBuf[ 256 ];
	for( int iActor = 0; iActor < iNumActors; ++iActor )
	{
		wxString strActorName;
		TOOL_DATA.GetActorNameByIndex( iActor, strActorName );
		ZeroMemory( acBuf, sizeof(acBuf) );
		MBConv.WC2MB( acBuf, strActorName.c_str(), 256 );
		const char* pActorName = acBuf;

		bool bShow = m_pPropGrid->GetPropertyValueAsBool( strActorName );
		
		map<string, bool>::iterator iter = pCamEventInfo->mapActorsShowHide.find( pActorName );
		if( pCamEventInfo->mapActorsShowHide.end() == iter )
			pCamEventInfo->mapActorsShowHide.insert( make_pair(string(pActorName), bShow) );
		else
			iter->second= bShow;

		
	}

	//// 등록된 액터들의 이름으로 show/hide 여부를 얻어온다
	//int iNumActors = TOOL_DATA.GetNumActors();
	//char acBuf[ 256 ];
	//for( int iActor = 0; iActor < iNumActors; ++iActor )
	//{
	//	wxString strActorName;
	//	TOOL_DATA.GetActorNameByIndex( iActor, strActorName );
	//	ZeroMemory( acBuf, sizeof(acBuf) );
	//	MBConv.WC2MB( acBuf, strActorName.c_str(), 256 );
	//	const char* pActorName = acBuf;
	//	map<string, bool>::iterator iter = pCamEventInfo->mapActorsShowHide.find( pActorName );
	//	_ASSERT( pCamEventInfo->mapActorsShowHide.end() != iter );

	//	iter->second = m_pPropGrid->GetPropertyValueAsBool( strActorName );
	//}
}



void CCameraEventProperty::OnPropertyChanged( wxPropertyGridEvent& PGEvent )
{
	const EventInfo* pCamEventInfo = TOOL_DATA.GetEventInfoByID( m_iMyEventInfoID );
	EventInfo* pNewCamEventInfo = pCamEventInfo->clone();
	_GetPropertyValue( pNewCamEventInfo );

	CEventPropChange EventChange( &TOOL_DATA, pNewCamEventInfo );
	TOOL_DATA.RunCommand( &EventChange );

	delete pNewCamEventInfo;
}



void CCameraEventProperty::_UpdateProp( int iEventInfoID )
{
	if( wxNOT_FOUND != iEventInfoID )
	{
		EventInfo* pEventInfo = const_cast<EventInfo*>(TOOL_DATA.GetEventInfoByID( iEventInfoID ));
		CamEventInfo* pCamEventInfo = static_cast<CamEventInfo*>(pEventInfo);

		wxCSConv MBConv( wxFONTENCODING_CP949 );
		wxChar caWBuf[ 256 ];

		ZeroMemory( caWBuf, sizeof(caWBuf) );
		MBConv.MB2WC( caWBuf, pCamEventInfo->strEventName.c_str(), 256 );
		m_pPropGrid->SetPropertyValueString( m_aPGID[ CAMERA_NAME ], caWBuf );

		m_pPropGrid->SetPropertyValueLong( m_aPGID[ ID ], pEventInfo->iID );

		ZeroMemory( caWBuf, sizeof(caWBuf) );
		MBConv.MB2WC( caWBuf, pCamEventInfo->strImportFilePath.c_str(), 256 );
		m_pPropGrid->SetPropertyValueString( m_aPGID[ IMPORT_FILE_PATH ], caWBuf );

		m_pPropGrid->SetPropertyValueDouble( m_aPGID[ START_TIME ], pEventInfo->fStartTime );

		m_pPropGrid->SetPropertyValue( m_aPGID[ SPEED ], pEventInfo->fSpeed );
		
		if( false == pCamEventInfo->strImportFilePath.empty() )
			pEventInfo->fTimeLength = pCamEventInfo->pCameraData->m_Header.nFrame / pCamEventInfo->fSpeed;
		else
			pEventInfo->fTimeLength = 0.0f;

		m_pPropGrid->SetPropertyValueDouble( m_aPGID[ TIME_LENGTH ], pCamEventInfo->fTimeLength );

		m_pPropGrid->EnableProperty( m_aPGID[ TIME_LENGTH ], false );

		wxVariantData_EtVector3* pDestData = new wxVariantData_EtVector3(pCamEventInfo->vCamStartOffset);
		wxVariant StartOffsetValue( pDestData, wxT("EtVector3") );
		m_pPropGrid->SetPropertyValue( m_aPGID[ START_OFFSET ], StartOffsetValue );

		// 액터리스트 모두 제거.. 이거 안먹네
		//m_pPropGrid->Delete( m_aPGID[ ACTOR_CATEGORY ] );
		int iNumRenderActors = (int)m_vlRenderingActors.size();
		for( int iActor = 0; iActor < iNumRenderActors; ++iActor )
		{
			wxString& strActorName = m_vlRenderingActors.at( iActor );
			m_pPropGrid->Delete( strActorName );
		}
		m_vlRenderingActors.clear();

		// 현재 사용하는 액터들을 모두 추가
		int iNumActors = TOOL_DATA.GetNumActors();
		for( int iActor = 0; iActor < iNumActors; ++iActor )
		{
			wxString strActorName;
			TOOL_DATA.GetActorNameByIndex( iActor, strActorName );

			// 디폴트는 true 값임
			m_pPropGrid->Append( wxBoolProperty( strActorName, wxPG_LABEL, true ) );
			m_pPropGrid->SetPropertyAttribute( strActorName, wxPG_BOOL_USE_CHECKBOX, true );
			m_vlRenderingActors.push_back( strActorName );
		}

		// 액터들의 show/hide 여부 데이터로부터 반영
		map<string, bool>::iterator iter = pCamEventInfo->mapActorsShowHide.begin();
		for( iter; iter != pCamEventInfo->mapActorsShowHide.end(); ++iter )
		{
			ZeroMemory( caWBuf, sizeof(caWBuf) );
			MBConv.MB2WC( caWBuf, iter->first.c_str(), 256 );
			m_pPropGrid->SetPropertyValueBool( caWBuf, iter->second );
		}

		//// 새로 추가된 액터들을 추가한다. 디폴트 값은 show 임
		//int iNumActors = TOOL_DATA.GetNumActors();
		//char acBuf[ 256 ];
		//for( int iActor = 0; iActor < iNumActors; ++iActor )
		//{
		//	wxString strActorName;
		//	TOOL_DATA.GetActorNameByIndex( iActor, strActorName );
		//	ZeroMemory( acBuf, sizeof(acBuf) );
		//	MBConv.WC2MB( acBuf, strActorName.c_str(), 256 );
 	//		const char* pActorName = acBuf;
		//	map<string, bool>::iterator iter = pCamEventInfo->mapActorsShowHide.find( pActorName );
		//	if( pCamEventInfo->mapActorsShowHide.end() == iter )
		//	{
		//		m_pPropGrid->Append( wxBoolProperty( strActorName, wxPG_LABEL, true ) );
		//		m_pPropGrid->SetPropertyAttribute( strActorName, wxPG_BOOL_USE_CHECKBOX, true );
		//		pCamEventInfo->mapActorsShowHide.insert( make_pair(string(pActorName), true) );		// 디폴트는 show 임.
		//	}
		//}
		
		m_iMyEventInfoID = pEventInfo->iID;
	}
}




void CCameraEventProperty::ClearTempData( void )
{
	m_iMyEventInfoID = -1;
}




void CCameraEventProperty::CommandPerformed( ICommand* pCommand )
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


