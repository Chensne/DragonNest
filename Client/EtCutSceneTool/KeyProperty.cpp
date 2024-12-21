#include "stdafx.h"
#include "VectorProperty.h"
#include "KeyProperty.h"
#include "CommandSet.h"
#include "ToolData.h"
#include "RTCutSceneRenderer.h"
#include "RTCutSceneMakerFrame.h"



const wxChar* KEY_TYPE_MOVE = wxT("MOVE");
const wxChar* KEY_TYPE_MULTI_MOVE = wxT("MULTI_MOVE");
const wxChar* KEY_TYPE_ROTATION = wxT("ROTATION");
const wxChar* KEY_TYPE_SHOW = wxT("SHOW");
const wxChar* KEY_TYPE_HIDE = wxT("HIDE");
//const wxChar* KEY_TYPE_SCALE = wxT("SCALE");


CKeyProperty::CKeyProperty( wxWindow* pParent, int id ) : IPropertyDelegate( pParent, id ),
														  m_pReflectedKeyInfo( new KeyInfo )/*,
														  m_bChangeToMultiMove( false )*/
{
	_RebuildProperty();

	CRTCutSceneMakerFrame* pMainFrame = static_cast<CRTCutSceneMakerFrame*>(TOOL_DATA.GetMainFrame());
	pMainFrame->ConnectMouseWheelEventToRenderPanel( m_pPropGrid );
}

CKeyProperty::~CKeyProperty(void)
{
	delete m_pReflectedKeyInfo;
}




void CKeyProperty::Initialize( void )
{	

}


void CKeyProperty::_RebuildProperty( void )
{
	//m_pPropGrid->Clear();

	wxPGId CategoryID = m_pPropGrid->AppendCategory( wxT("Key Property") );
	m_pPropGrid->SetPropertyHelpString( CategoryID, wxT("Set Key Property") );

	m_aPGID[ ACTOR_NAME ] = m_pPropGrid->Append( wxStringProperty( wxT("Actor Name"), wxPG_LABEL, wxT("") ) );
	m_aPGID[ KEY_NAME ] = m_pPropGrid->Append( wxStringProperty( wxT("Key Name"), wxPG_LABEL, wxT("") ) );
	m_aPGID[ ID ] = m_pPropGrid->Append( wxUIntProperty( wxT("unique ID"), wxPG_LABEL, 0 ) );

	m_aPGID[ START_TIME ] = m_pPropGrid->Append( wxFloatProperty( wxT("Start Time"), wxPG_LABEL, 0.0f ) );
	m_aPGID[ TIME_LENGTH ] = m_pPropGrid->Append( wxFloatProperty( wxT("Time Length(sec)"), wxPG_LABEL, 0.0f ) );

	const wxChar* astrKeyType [] = { KEY_TYPE_MOVE, KEY_TYPE_MULTI_MOVE, KEY_TYPE_ROTATION, KEY_TYPE_SHOW, KEY_TYPE_HIDE, NULL };
	m_aPGID[ KEY_TYPE ] = m_pPropGrid->Append( wxEnumProperty( wxT("Type") , wxPG_LABEL, astrKeyType ) );

	// 회전 키인 경우
	m_aPGID[ ROTATION ] = m_pPropGrid->Append( wxFloatProperty( wxT("Rotate Degree"), wxPG_LABEL, 0.0f ) );

	// 이동 키인 경우
	m_aPGID[ ROTATION_FRONT ] = m_pPropGrid->Append( wxBoolProperty(wxT("Rotation Front"), wxPG_LABEL, false) );
	m_pPropGrid->SetPropertyAttribute( m_aPGID[ ROTATION_FRONT ], wxPG_BOOL_USE_CHECKBOX, true );

	m_aPGID[ FIT_YPOS_TO_MAP ] = m_pPropGrid->Append( wxBoolProperty(wxT("Y pos to map"), wxPG_LABEL, true) );
	m_pPropGrid->SetPropertyAttribute( m_aPGID[ FIT_YPOS_TO_MAP ], wxPG_BOOL_USE_CHECKBOX, true );

	m_aPGID[ DEST_POS ] = m_pPropGrid->Append( VectorProperty( wxT("Destinaion"), wxPG_LABEL, EtVector3(0.0f, 0.0f, 0.0f) ) );	

	wxPGChoices NumKeys;
	wxString strBuf;
	for( int i = 3; i <= 30; ++i )
	{
		strBuf.Printf( wxT("%d"), i );
		NumKeys.Add( strBuf );
	}
	m_aPGID[ NUM_MOVEKEY ] = m_pPropGrid->Append( wxEnumProperty( wxT("Num Move Keys"), wxPG_LABEL, NumKeys ) );
	m_aPGID[ MOVEKEY_LIST ] = m_pPropGrid->Append( wxParentProperty( wxT("Move Key List"), wxEmptyString ) );

	int iCount = 1;
	for( int i = MOVEKEY_1; i <= MOVEKEY_30; ++i )
	{
		strBuf.Printf( wxT("MoveKey %d"), iCount );
		m_aPGID[ i ] = m_pPropGrid->AppendIn( m_aPGID[ MOVEKEY_LIST ], VectorProperty( strBuf, wxPG_LABEL, EtVector3( 0.0f, 0.0f, 0.0f ) ) );

		strBuf.Printf( wxT("Time Length(sec)") );
		m_aPGID[ MOVEKEY_30 + iCount] = m_pPropGrid->AppendIn( m_aPGID[ MOVEKEY_LIST ], wxFloatProperty( strBuf, wxPG_LABEL, 0.0f ) );
		++iCount;
	}


	m_aPGID[ START_POS ] =  m_pPropGrid->Append( VectorProperty( wxT("Start Position"), wxPG_LABEL, EtVector3( 0.0f, 0.0f, 0.0f) ) );
}


void CKeyProperty::OnShow( void )
{
	int iSelectedObjectID = TOOL_DATA.GetSelectedObjectID();
	if( wxNOT_FOUND != iSelectedObjectID )
	{
		// 새로운 키 속성이면 값들을 초기화 해준다.
		if( m_pReflectedKeyInfo->iID != iSelectedObjectID )
		{
			m_pPropGrid->Clear();
			_RebuildProperty();

			for( int iProp = ACTOR_NAME; iProp < COUNT; ++iProp )
				m_pPropGrid->ClearPropertyValue( m_aPGID[ iProp ] );

			for( int i = MOVEKEY_1; i <= MOVEKEY_30; ++i )
			{
				wxVariantData_EtVector3* pData = new wxVariantData_EtVector3(EtVector3( 0.0f, 0.0f, 0.0f ));
				wxVariant Value( pData, wxT("EtVector3") );
				m_pPropGrid->SetPropertyValue( m_aPGID[ i ], Value );
			}

			m_pReflectedKeyInfo->iKeyType = KeyInfo::MOVE;
			_UpdateMoveType( KeyInfo::MOVE );
		}

		_UpdateProp( iSelectedObjectID );
	}	
}



void CKeyProperty::_UpdateMoveType( int iKeyType )
{
	switch( iKeyType )
	{
		case KeyInfo::MOVE:
			m_pPropGrid->HideProperty( m_aPGID[ START_POS ], false );
			m_pPropGrid->HideProperty( m_aPGID[ DEST_POS ], false );
			m_pPropGrid->HideProperty( m_aPGID[ FIT_YPOS_TO_MAP ], false );
			m_pPropGrid->HideProperty( m_aPGID[ ROTATION_FRONT ], false );
			m_pPropGrid->HideProperty( m_aPGID[ NUM_MOVEKEY ], true );
			m_pPropGrid->HideProperty( m_aPGID[ MOVEKEY_LIST ], true );
			m_pPropGrid->HideProperty( m_aPGID[ ROTATION ], true );
			m_pPropGrid->EnableProperty( m_aPGID[ TIME_LENGTH ], true );
			break;

		case KeyInfo::MULTI_MOVE:
			m_pPropGrid->HideProperty( m_aPGID[ START_POS ], true );
			m_pPropGrid->HideProperty( m_aPGID[ DEST_POS ], true );
			m_pPropGrid->HideProperty( m_aPGID[ ROTATION ], true );
			m_pPropGrid->HideProperty( m_aPGID[ FIT_YPOS_TO_MAP ], false );
			m_pPropGrid->HideProperty( m_aPGID[ ROTATION_FRONT ], false );
			m_pPropGrid->HideProperty( m_aPGID[ NUM_MOVEKEY ], false );
			m_pPropGrid->HideProperty( m_aPGID[ MOVEKEY_LIST ], false );
			m_pPropGrid->EnableProperty( m_aPGID[ TIME_LENGTH ], false );
			break;

		case KeyInfo::ROTATION:
			m_pPropGrid->HideProperty( m_aPGID[ ROTATION ], false );
			m_pPropGrid->HideProperty( m_aPGID[ FIT_YPOS_TO_MAP ], true );
			m_pPropGrid->HideProperty( m_aPGID[ ROTATION_FRONT ], true );
			m_pPropGrid->HideProperty( m_aPGID[ NUM_MOVEKEY ], true );
			m_pPropGrid->HideProperty( m_aPGID[ MOVEKEY_LIST ], true );
			m_pPropGrid->HideProperty( m_aPGID[ START_POS ], true );
			m_pPropGrid->HideProperty( m_aPGID[ DEST_POS ], true );
			m_pPropGrid->EnableProperty( m_aPGID[ TIME_LENGTH ], true );
			break;

		case KeyInfo::SHOW:
		case KeyInfo::HIDE:
			m_pPropGrid->HideProperty( m_aPGID[ ROTATION ], true );
			m_pPropGrid->HideProperty( m_aPGID[ FIT_YPOS_TO_MAP ], true );
			m_pPropGrid->HideProperty( m_aPGID[ ROTATION_FRONT ], true );
			m_pPropGrid->HideProperty( m_aPGID[ NUM_MOVEKEY ], true );
			m_pPropGrid->HideProperty( m_aPGID[ MOVEKEY_LIST ], true );
			m_pPropGrid->HideProperty( m_aPGID[ START_POS ], true );
			m_pPropGrid->HideProperty( m_aPGID[ DEST_POS ], true );
			m_pPropGrid->EnableProperty( m_aPGID[ TIME_LENGTH ], true );
			break;
	}
}



void CKeyProperty::_GetVectorProperty( int iPropID, /*IN OUT*/ EtVector3& vVector )
{
	wxVariant value = m_pPropGrid->GetPropertyValue( m_aPGID[ iPropID ] );
	wxVariantData_EtVector3* pData = static_cast<wxVariantData_EtVector3*>(value.GetData());
	const EtVector3 vPos = pData->GetValue();
	vVector = vPos;
}



void CKeyProperty::_GetPropertyValues( KeyInfo* pKeyInfo )
{
	wxCSConv MBConv( wxFONTENCODING_CP949 );
	char caBuf[ 256 ];

	wxString strBuf = m_pPropGrid->GetPropertyValueAsString( m_aPGID[ ACTOR_NAME ] );
	ZeroMemory( caBuf, sizeof(caBuf) );
	MBConv.WC2MB( caBuf, strBuf.c_str(), 256 );
	pKeyInfo->strActorName.assign( caBuf );

	wxString strActorName = m_pPropGrid->GetPropertyValueAsString( m_aPGID[ KEY_NAME ] );
	ZeroMemory( caBuf, sizeof(caBuf) );
	MBConv.WC2MB( caBuf, strActorName.c_str(), 256 );
	pKeyInfo->strKeyName.assign( caBuf );

	pKeyInfo->iID = m_pPropGrid->GetPropertyValueAsInt( m_aPGID[ ID ] );
	pKeyInfo->fStartTime = (float)m_pPropGrid->GetPropertyValueAsDouble( m_aPGID[ START_TIME ] );
	pKeyInfo->fTimeLength = (float)m_pPropGrid->GetPropertyValueAsDouble( m_aPGID[ TIME_LENGTH ] );
	
	pKeyInfo->iKeyType = (int)m_pPropGrid->GetPropertyValueAsInt( m_aPGID[ KEY_TYPE ] );
	pKeyInfo->bRotateFront = (bool)m_pPropGrid->GetPropertyValueAsBool( m_aPGID[ ROTATION_FRONT ] );
	pKeyInfo->bFitYPosToMap = (bool)m_pPropGrid->GetPropertyValueAsBool( m_aPGID[ FIT_YPOS_TO_MAP ] );
	pKeyInfo->fUnit = 1000.0f;

	//_UpdateMoveType( pKeyInfo->iKeyType );
	const KeyInfo* pOldKeyInfo = TOOL_DATA.GetKeyInfoByID( pKeyInfo->iID );

	switch( pKeyInfo->iKeyType )
	{
		case KeyInfo::MOVE:
			{
				_GetVectorProperty( DEST_POS, pKeyInfo->vDestPos );
				_GetVectorProperty( START_POS, pKeyInfo->vStartPos );

				if( pKeyInfo->bFitYPosToMap )
				{
					CRTCutSceneRenderer* pRenderer = TOOL_DATA.GetRenderer();
					pKeyInfo->vDestPos.y = pRenderer->GetMapHeight( pKeyInfo->vDestPos.x, pKeyInfo->vDestPos.z );
					pKeyInfo->vStartPos.y = pRenderer->GetMapHeight( pKeyInfo->vStartPos.x, pKeyInfo->vDestPos.z );
				}
			}
			break;

		case KeyInfo::MULTI_MOVE:
			{
				EtVector3 vPos;
		
				//pKeyInfo->vlMoveKeys.clear();
				vector<SubKey> vlMoveKeys;
				int iNumMoveKeys = (int)m_pPropGrid->GetPropertyValueAsInt( m_aPGID[ NUM_MOVEKEY ] ) + 3;

				// 이동 키는 30개 까지만.
				if( iNumMoveKeys > 30 )
					iNumMoveKeys = 30;

				pKeyInfo->fTimeLength = 0.0f;
				for( int iKey = 0; iKey < iNumMoveKeys; ++iKey )
				{
					SubKey MoveKey;
					
					// 기존에 추가되어있던 거라면 id 를 복사해온다.
					if( iKey < (int)pOldKeyInfo->vlMoveKeys.size() )
						MoveKey = pOldKeyInfo->vlMoveKeys.at(iKey);

					_GetVectorProperty( MOVEKEY_1+iKey, MoveKey.vPos );
					
					if( pKeyInfo->bFitYPosToMap )
					{
						MoveKey.vPos.y = 0.0f;
						
						CRTCutSceneRenderer* pRenderer = TOOL_DATA.GetRenderer();
						MoveKey.vPos.y = pRenderer->GetMapHeight( MoveKey.vPos.x, MoveKey.vPos.z );
					}

					MoveKey.fTimeLength = (float)m_pPropGrid->GetPropertyValueAsDouble( m_aPGID[ MOVEKEY_LENGTH_1+iKey ] );

					// 새롭게 추가되는 이동 키는 여기서 초기화 해준다.
					if( -1 == MoveKey.iID )
					{
						MoveKey.iID = TOOL_DATA.GenerateID();
						MoveKey.fTimeLength = 3.0f;/*pKeyInfo->fTimeLength / (float)iNumMoveKeys;*/

						// 맨 끝의 키는 시간 간격이 무조건 0임
						if( iKey == iNumMoveKeys-1 )
							MoveKey.fTimeLength = 0.0f;
					}

					pKeyInfo->fTimeLength += MoveKey.fTimeLength;

					vlMoveKeys.push_back( MoveKey );
				}

				pKeyInfo->vlMoveKeys = vlMoveKeys;
			}
			break;

		case KeyInfo::ROTATION:
			pKeyInfo->fRotDegree = (float)m_pPropGrid->GetPropertyValueAsDouble( m_aPGID[ ROTATION ] );
			break;
	}

	//pKeyInfo->fRotation = (float)m_pPropGrid->GetPropertyValueAsDouble( m_aPGID[ ROTATION ] );
}



void CKeyProperty::_UpdateProp( int iObjectID )
{
	if( wxNOT_FOUND != iObjectID )
	{
		const KeyInfo* pKeyInfo = TOOL_DATA.GetKeyInfoByID( iObjectID );
		
		//_RebuildProperty();
		if( m_pReflectedKeyInfo->iKeyType != pKeyInfo->iKeyType )
			_UpdateMoveType( pKeyInfo->iKeyType );

		wxCSConv MBConv( wxFONTENCODING_CP949 );
		wxChar wcaBuf[ 256 ];
		wxChar wcaActorName[ 256 ];
		ZeroMemory( wcaBuf, sizeof(wcaBuf) );

		MBConv.MB2WC( wcaActorName, pKeyInfo->strActorName.c_str(), 256 );
		m_pPropGrid->SetPropertyValueString( m_aPGID[ ACTOR_NAME ], wcaActorName );
		m_pPropGrid->EnableProperty( m_aPGID[ ACTOR_NAME ], false );

		MBConv.MB2WC( wcaBuf, pKeyInfo->strKeyName.c_str(), 256 );
		m_pPropGrid->SetPropertyValueString( m_aPGID[ KEY_NAME ], wcaBuf );
		m_pPropGrid->SetPropertyValueLong( m_aPGID[ ID ], pKeyInfo->iID );
		m_pPropGrid->EnableProperty( m_aPGID[ ID ], false );
		m_pPropGrid->SetPropertyValueDouble( m_aPGID[ START_TIME ], pKeyInfo->fStartTime );
		m_pPropGrid->SetPropertyValueDouble( m_aPGID[ TIME_LENGTH ], pKeyInfo->fTimeLength );

		m_pPropGrid->SetPropertyValue( m_aPGID[ KEY_TYPE ], pKeyInfo->iKeyType );
		m_pPropGrid->SetPropertyValue( m_aPGID[ ROTATION_FRONT ], pKeyInfo->bRotateFront );
		m_pPropGrid->SetPropertyValue( m_aPGID[ FIT_YPOS_TO_MAP ], pKeyInfo->bFitYPosToMap );

		switch( pKeyInfo->iKeyType )
		{
			case KeyInfo::MOVE:
				{
					wxVariantData_EtVector3* pDestData = new wxVariantData_EtVector3(pKeyInfo->vDestPos);
					wxVariant DestValue( pDestData, wxT("EtVector3") );
					m_pPropGrid->SetPropertyValue( m_aPGID[ DEST_POS ], DestValue );

					wxVariantData_EtVector3* pStartData = new wxVariantData_EtVector3(pKeyInfo->vStartPos);
					wxVariant StartValue( pStartData, wxT("EtVector3") );
					m_pPropGrid->SetPropertyValue( m_aPGID[ START_POS ], StartValue );
				}
				break;
			
			case KeyInfo::MULTI_MOVE:
				{
					// 서브 무브 키의 갯수가 달라졌을 경우엔 
					int iNumKeys = (int)pKeyInfo->vlMoveKeys.size();
					if( m_pReflectedKeyInfo->vlMoveKeys.size() != pKeyInfo->vlMoveKeys.size() )
					{
						for( int i = 0; i < 30; ++i )
						{
							m_pPropGrid->HideProperty( m_aPGID[ MOVEKEY_1+i ], true );
							m_pPropGrid->HideProperty( m_aPGID[ MOVEKEY_LENGTH_1+i ], true );
						}

						for( int iKey = 0; iKey < iNumKeys; ++iKey )
						{
							m_pPropGrid->HideProperty( m_aPGID[ MOVEKEY_1+iKey ], false );
							m_pPropGrid->HideProperty( m_aPGID[ MOVEKEY_LENGTH_1+iKey ], false );
						}
					}

					m_pPropGrid->SetPropertyValue( m_aPGID[ NUM_MOVEKEY ], iNumKeys-3 );
					for( int iKey = 0; iKey < iNumKeys; ++iKey )
					{
						const SubKey& MoveKey = pKeyInfo->vlMoveKeys.at(iKey);
						wxVariantData_EtVector3* pData = new wxVariantData_EtVector3(MoveKey.vPos);
						wxVariant Value( pData, wxT("EtVector3") );
						m_pPropGrid->SetPropertyValue( m_aPGID[ MOVEKEY_1+iKey ], Value );

						m_pPropGrid->SetPropertyValue( m_aPGID[ MOVEKEY_LENGTH_1+iKey ], MoveKey.fTimeLength );

						// 마지막 키는 목적지이므로 타임 간격이 0. 편집도 불가능하다.
						if( iNumKeys-1 == iKey )
						{
							m_pPropGrid->EnableProperty( m_aPGID[ MOVEKEY_LENGTH_1+iKey ], false );
						}
						else
						{
							m_pPropGrid->EnableProperty( m_aPGID[ MOVEKEY_LENGTH_1+iKey ], true );
						}
					}
				}
				break;

			case KeyInfo::ROTATION:
				{
					m_pPropGrid->SetPropertyValueDouble( m_aPGID[ ROTATION ], pKeyInfo->fRotDegree );
				}
				break;

		}

		*m_pReflectedKeyInfo = *pKeyInfo;

		//m_pPropGrid->SetPropertyValueDouble( m_aPGID[ ROTATION ], pKeyInfo->fRotation );

		//_GetPropertyValues( m_pReflectedKeyInfo );
	}
}


void CKeyProperty::OnPropertyChanged( wxPropertyGridEvent& PGEvent )
{
	KeyInfo NewKeyInfo;
	_GetPropertyValues( /*m_pReflectedKeyInfo*/&NewKeyInfo );

	CKeyPropChange KeyInfoChange( &TOOL_DATA, &NewKeyInfo );
	TOOL_DATA.RunCommand( &KeyInfoChange );
}




void CKeyProperty::ClearTempData( void )
{
	SAFE_DELETE( m_pReflectedKeyInfo );
	m_pReflectedKeyInfo = new KeyInfo;
}




void CKeyProperty::CommandPerformed( ICommand* pCommand )
{
	const KeyInfo* pMyKeyInfo = TOOL_DATA.GetKeyInfoByID( m_pReflectedKeyInfo->iID );
	if( NULL == pMyKeyInfo )
	{
		m_pPropGrid->Show( false );
	}
	else
	{
		switch( pCommand->GetTypeID() )
		{
			case CMD_UPDATE_VIEW:
				_UpdateProp( m_pReflectedKeyInfo->iID );
				break;

			case CMD_KEY_PROP_CHANGE:
				_UpdateProp( m_pReflectedKeyInfo->iID );
				break;
		}
	}
}

