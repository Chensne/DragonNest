#include "stdafx.h"
#include "SceneProperty.h"
#include "CommandSet.h"
#include "ToolData.h"



CSceneProperty::CSceneProperty( wxWindow* pParent, int id ) : IPropertyDelegate( pParent, id )
{
	wxPGId CategoryID = m_pPropGrid->AppendCategory( wxT("Scene Property") );
	m_pPropGrid->SetPropertyHelpString( CategoryID, wxT("Set Scene Property") );

	// 신 시작 시에 게임 클라이언트에서 페이드 아웃을 할 시간
	m_aPGID[ SCENE_START_FADE_OUT_TIME ] = m_pPropGrid->Append( wxFloatProperty( wxT("Scene Start Fade Out Time"), wxPG_LABEL, 0.0f ) );
	m_aPGID[ SCENE_END_FADE_IN_TIME ] = m_pPropGrid->Append( wxFloatProperty( wxT("Scene End Fade In Time"), wxPG_LABEL, 0.0f ) );
	
	m_aPGID[ USE_SCENE_END_CLIPPING ] = m_pPropGrid->Append( wxBoolProperty( wxT("Use Scene End Clipping"), wxPG_LABEL, false ) );
	m_aPGID[ SCENE_END_CLIP_TIME ] = m_pPropGrid->Append( wxFloatProperty( wxT("End Clipping Time (sec)"), wxPG_LABEL, 0.0f ) );
	m_aPGID[ USE_SIGNAL_SOUND ] = m_pPropGrid->Append( wxBoolProperty( wxT("Use Signal Sound"), wxPG_LABEL, false ) );

	// 액터가 추가되거나 사라지면 변경해줘야 함. 데이터 쪽도 주인공 골라놨던게 없어지면 초기화 해줘야 함
	m_aPGID[ MAIN_CHARACTER ] = m_pPropGrid->Append( wxEnumProperty( wxT("Main Character"), wxPG_LABEL, wxArrayString() ) );

	// 조정할 fog far 가 있다면 입력 받는다.
	m_aPGID[ FOGFAR_DELTA ] = m_pPropGrid->Append( wxFloatProperty( wxT("Fog Far Delta"), wxPG_LABEL, 0.0f ) );

	// 이 컷신에서는 아카데믹을 지원하지 않음.
	m_aPGID[ ACADEMIC_NOT_SUPPORTED ] = m_pPropGrid->Append( wxBoolProperty( wxT("Academic Not Support"), wxPG_LABEL, false ) );
	m_pPropGrid->SetPropertyAttribute( m_aPGID[ ACADEMIC_NOT_SUPPORTED ], wxPG_BOOL_USE_CHECKBOX, true );

	m_pPropGrid->EnableProperty( m_aPGID[ SCENE_END_CLIP_TIME ], false );
}



CSceneProperty::~CSceneProperty(void)
{

}


void CSceneProperty::Initialize( void )
{

}


void CSceneProperty::OnShow( void )
{
	// 신 속성인 경우는 보이게 되는 시점이.. 다른 속성들과 좀 다르다.. 노트북으로 처리해야 하나.
	_UpdateProp();
}



void CSceneProperty::_UpdateProp( void )
{
	const SceneInfo* pSceneInfo = TOOL_DATA.GetSceneInfo();
	
	m_pPropGrid->SetPropertyValueDouble( m_aPGID[ SCENE_START_FADE_OUT_TIME ], (float)pSceneInfo->fSceneStartFadeOutTime * 0.001f );
	m_pPropGrid->SetPropertyValueDouble( m_aPGID[ SCENE_END_FADE_IN_TIME ], (float)pSceneInfo->fSceneEndFadeInTime * 0.001f );

	m_pPropGrid->SetPropertyValueBool( m_aPGID[ USE_SCENE_END_CLIPPING ], pSceneInfo->bUseSceneEndClippingTime );
	m_pPropGrid->EnableProperty( m_aPGID[ SCENE_END_CLIP_TIME ], pSceneInfo->bUseSceneEndClippingTime );
	m_pPropGrid->SetPropertyValueDouble( m_aPGID[ SCENE_END_CLIP_TIME ], (float)pSceneInfo->fSceneEndClipTime );
	m_pPropGrid->SetPropertyValueBool( m_aPGID[ USE_SIGNAL_SOUND ], pSceneInfo->bUseSignalSound );
	m_pPropGrid->SetPropertyValueDouble( m_aPGID[ FOGFAR_DELTA ], pSceneInfo->fFogFarDelta );
	m_pPropGrid->SetPropertyValueBool( m_aPGID[ ACADEMIC_NOT_SUPPORTED ], pSceneInfo->bAcademicNotSupported );

	// 액터 리스트를 최신으로 갱신
	wxPGChoiceInfo ChoiceInfo;
	m_aPGID[ MAIN_CHARACTER ].GetProperty().GetChoiceInfo( &ChoiceInfo );
	while( ChoiceInfo.m_itemCount > 0 )
	{
		m_aPGID[ MAIN_CHARACTER ].GetProperty().DeleteChoice( 0 );
		m_aPGID[ MAIN_CHARACTER ].GetProperty().GetChoiceInfo( &ChoiceInfo );
	}

	m_pPropGrid->AddPropertyChoice( m_aPGID[ MAIN_CHARACTER ], wxT("NONE") );
	wxString strActorName;
	int iNumActors = TOOL_DATA.GetNumActors();
	for( int iActor = 0; iActor < iNumActors; ++iActor )
	{
		TOOL_DATA.GetActorNameByIndex( iActor, strActorName );
		m_pPropGrid->AddPropertyChoice( m_aPGID[ MAIN_CHARACTER ], strActorName.c_str() );
	}

	wxCSConv MBConv( wxFONTENCODING_CP949 );
	wxChar wcaBuf[ 256 ];
	ZeroMemory( wcaBuf, sizeof(wcaBuf) );
	MBConv.MB2WC( wcaBuf, pSceneInfo->strMainCharacterName.c_str(), 256 );
	m_pPropGrid->SetPropertyValue( m_aPGID[ MAIN_CHARACTER ], wcaBuf );
}


void CSceneProperty::_GetPropertyValue( SceneInfo* pInfo )
{
	pInfo->fSceneStartFadeOutTime = float(m_pPropGrid->GetPropertyValueAsDouble( m_aPGID[ SCENE_START_FADE_OUT_TIME ] ));
	pInfo->fSceneEndFadeInTime = float(m_pPropGrid->GetPropertyValueAsDouble( m_aPGID[ SCENE_END_FADE_IN_TIME ] ));
	pInfo->bUseSceneEndClippingTime = m_pPropGrid->GetPropertyValueAsBool( m_aPGID[ USE_SCENE_END_CLIPPING ] );
	m_pPropGrid->EnableProperty( m_aPGID[ SCENE_END_CLIP_TIME ], pInfo->bUseSceneEndClippingTime );
	pInfo->fSceneEndClipTime = float(m_pPropGrid->GetPropertyValueAsDouble( m_aPGID[ SCENE_END_CLIP_TIME ] ));	
	pInfo->bUseSignalSound = m_pPropGrid->GetPropertyValueAsBool( m_aPGID[ USE_SIGNAL_SOUND ] );
	pInfo->fFogFarDelta = m_pPropGrid->GetPropertyValueAsDouble( m_aPGID[ FOGFAR_DELTA ] );
	pInfo->bAcademicNotSupported = m_pPropGrid->GetPropertyValueAsBool( m_aPGID[ ACADEMIC_NOT_SUPPORTED ] );

	wxString strMainCharacterName = m_pPropGrid->GetPropertyValueAsString( m_aPGID[ MAIN_CHARACTER ] );
	wxCSConv MBConv( wxFONTENCODING_CP949 );
	char caBuf[ 256 ];
	ZeroMemory( caBuf, sizeof(caBuf) );
	MBConv.WC2MB( caBuf, strMainCharacterName.c_str(), 256 );
	pInfo->strMainCharacterName = caBuf;
}



void CSceneProperty::OnPropertyChanged( wxPropertyGridEvent& PGEvent )
{
	SceneInfo Info;
	_GetPropertyValue( &Info );

	CSceneInfoModify SceneInfoModify( &TOOL_DATA, &Info );
	TOOL_DATA.RunCommand( &SceneInfoModify );
}



void CSceneProperty::ClearTempData( void )
{

}



void CSceneProperty::CommandPerformed( ICommand* pCommand )
{
	switch( pCommand->GetTypeID() )
	{
		case CMD_UPDATE_VIEW:
			_UpdateProp();
			break;

		case CMD_MODIFY_SCENE_INFO:
			_UpdateProp();
			break;
	}
}
