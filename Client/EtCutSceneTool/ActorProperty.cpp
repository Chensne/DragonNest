#include "stdafx.h"
#include "VectorProperty.h"
#include "ActorProperty.h"
#include "ToolData.h"
#include "CommandSet.h"
#include "DnCutSceneData.h"
#include "RTCutSceneMakerFrame.h"
#include "RtCutSceneRenderer.h"



CActorProperty::CActorProperty( wxWindow* pParent, int id ) : IPropertyDelegate( pParent, id ),
															  m_vPrevPos( 0.0f, 0.0f, 0.0f ),
															  m_fPrevRotation( 0.0f ),
															  m_bPrevInfluenceLightmap( false ),
															  m_bPrevScaleLock( false )
{
	_RebuildProperty();

	CRTCutSceneMakerFrame* pMainFrame = static_cast<CRTCutSceneMakerFrame*>(TOOL_DATA.GetMainFrame());
	pMainFrame->ConnectMouseWheelEventToRenderPanel( m_pPropGrid );

	//m_pPropGrid->EnableProperty( m_aPGID[ POS_ ], true );
	//m_pPropGrid->EnableProperty( m_aPGID[ ROTATION ], true );
}

CActorProperty::~CActorProperty(void)
{

}


void CActorProperty::Initialize( void )
{
	
}



void CActorProperty::_RebuildProperty( void )
{
	m_pPropGrid->Clear();

	wxPGId CategoryID = m_pPropGrid->AppendCategory( wxT("Actor Property") );
	m_pPropGrid->SetPropertyHelpString( CategoryID, wxT("Set Actor's Property") );

	m_aPGID[ ACTOR_NAME ] = m_pPropGrid->Append( wxStringProperty( wxT("Name"), wxPG_LABEL, wxT("") ) );

	// 모든 액터를 다 변경할 수 있게 해줌..
	m_aPGID[ ACTOR_RES_NAME ] = m_pPropGrid->Append( wxEnumProperty( wxT("Actor Table Name"), wxPG_LABEL, wxArrayString() ) );

	int iNumActorRes = TOOL_DATA.GetNumActorResInfo();
	for( int i = 0 ; i < iNumActorRes; ++i )
	{
		const S_ACTOR_RES_INFO* pActorResInfo = TOOL_DATA.GetActorResInfoByIndex( i );
		m_pPropGrid->AddPropertyChoice( m_aPGID[ ACTOR_RES_NAME ], pActorResInfo->strActorName.c_str() );
	}

	m_aPGID[ POSITION ] = m_pPropGrid->Append( VectorProperty( wxT("Position"), wxPG_LABEL, EtVector3(0.0f, 0.0f, 0.0f) ) );
	m_aPGID[ FIT_YPOS_TO_MAP ] = m_pPropGrid->Append( wxBoolProperty(wxT("Y pos to map"), wxPG_LABEL, true ) );
	m_pPropGrid->SetPropertyAttribute( m_aPGID[ FIT_YPOS_TO_MAP ], wxPG_BOOL_USE_CHECKBOX, true );
	m_aPGID[ ROTATION ] = m_pPropGrid->Append( wxFloatProperty( wxT("Rot"), wxPG_LABEL, 0.0f ) );
	m_aPGID[ INFLUENCE_LIGHTMAP ] = m_pPropGrid->Append( wxBoolProperty( wxT("Influence Lightmap"), wxPG_LABEL, true ) );

	m_aPGID[ SCALE_LOCK ] = m_pPropGrid->Append( wxBoolProperty( wxT("Scale Lock"), wxPG_LABEL, true ) );		// 기본 값이 true 임.
	m_aPGID[ SELECT_BOSS ] = m_pPropGrid->Append( wxEnumProperty( wxT("Select Monster"), wxPG_LABEL, wxArrayString() ) );
	m_pPropGrid->AddPropertyChoice( m_aPGID[ SELECT_BOSS ], wxT("None") );		// 맨 첫번째는 None 으로 셋팅
	m_pPropGrid->SetPropertyValueLong( m_aPGID[ SELECT_BOSS ], 0 );
	m_pPropGrid->HideProperty( m_aPGID[ SELECT_BOSS ], true );
}



void CActorProperty::OnPropertyChanged( wxPropertyGridEvent& PGEvent )
{
	// 데이터에서 값을 긁어온다.
	int iSelectedRegRes = TOOL_DATA.GetSelectedRegResIndex();
	if( wxNOT_FOUND != iSelectedRegRes )
	{
		// 현재 선택한 리소스가 액터가 아니면 액터 속성이 안보이므로 반영할 필요가 없다.
		if( CDnCutSceneData::RT_RES_ACTOR == TOOL_DATA.GetRegResKindByIndex( iSelectedRegRes ) )
		{
			wxString strActorName = m_pPropGrid->GetPropertyValueAsString( m_aPGID[ ACTOR_NAME ] );
			
			// 보여지는 플레이어 캐릭터를 변경한 경우 렌더러에서 캐릭터만 바꿔줌. 언두 리두 될 필요 없음.
			// 따라서 커맨드 프로세서 거쳐갈 필요 없다.
			// 해당 애니메이션이 있는지 정도는 체크해줘야 한다.
			if( PGEvent.GetProperty() == m_aPGID[ ACTOR_RES_NAME ] )
			{
				const S_ACTOR_RES_INFO* pActorResInfoToChange = TOOL_DATA.GetActorResInfoByIndex( PGEvent.GetPropertyValueAsInt() );

				// 액터를 바꾸는 것이 가능한지 체크. 
				// 주로 애니메이션(액션)이 호환되는지 체크한다.1
				CRTCutSceneRenderer* pRenderer = static_cast<CRTCutSceneRenderer*>(TOOL_DATA.GetRenderer());
				bool bCanChangeActor = pRenderer->CanChangeActor( strActorName.c_str(), pActorResInfoToChange->strActorName.c_str() );
				if( bCanChangeActor )
				{
					CChangeActorRes ChangeActorRes( &TOOL_DATA, pActorResInfoToChange->strActorName.c_str(), strActorName.c_str() );
					TOOL_DATA.RunCommand( &ChangeActorRes );
				}
				//pRenderer->ChangeActor( strActorName.c_str(), pActorResInfoToChange->strActorName.c_str() );

			}
			else
			{
				wxVariant value = m_pPropGrid->GetPropertyValue( m_aPGID[ POSITION ] );
				wxVariantData_EtVector3* pData = static_cast<wxVariantData_EtVector3*>(value.GetData());
				const EtVector3 vPos = pData->GetValue();

				bool bFitYPosToMap = m_pPropGrid->GetPropertyValueAsBool( m_aPGID[ FIT_YPOS_TO_MAP ] );
				float fRotation = (double)m_pPropGrid->GetPropertyValueAsDouble( m_aPGID[ ROTATION ] );
				bool bInfluenceLightmap = m_pPropGrid->GetPropertyValueAsBool( m_aPGID[ INFLUENCE_LIGHTMAP ] );
				bool bScaleLock = m_pPropGrid->GetPropertyValueAsBool( m_aPGID[ SCALE_LOCK ] );
				int iSelectedMonsterInfo = m_pPropGrid->GetPropertyValueAsInt( m_aPGID[ SELECT_BOSS ] );

				int iMonsterTableID = 0;
				if( false == bScaleLock )
				{
					// 몬스터 테이블 인덱스가 바뀌면 유효하지 않을 수 있다. 파일 로드시 예외처리 해준다.
					wxString strActorResName = m_pPropGrid->GetPropertyValueAsString( m_aPGID[ ACTOR_RES_NAME ] );
					wxString strActorName = m_pPropGrid->GetPropertyValueAsString( m_aPGID[ ACTOR_NAME ] );
					vector<const S_MONSTER_INFO*> vlResult;
					TOOL_DATA.GatherBossInfoOfThisActor( strActorResName, vlResult );
					if( 0 < iSelectedMonsterInfo )
					{
						assert( iSelectedMonsterInfo-1 < (int)vlResult.size() );
						iMonsterTableID = vlResult.at(iSelectedMonsterInfo-1)->iMonsterTableID;
					}
				}

				CActorResPropChange Command( &TOOL_DATA, m_strPrevActorName.c_str(), strActorName.c_str(), 
											 vPos, fRotation, bFitYPosToMap, bInfluenceLightmap, iMonsterTableID, bScaleLock );
				TOOL_DATA.RunCommand( &Command );

				m_strPrevActorName = strActorName;
				m_vPrevPos = vPos;
				m_fPrevRotation = fRotation;
				m_bPrevInfluenceLightmap = bInfluenceLightmap;
				m_bPrevScaleLock = bScaleLock;
			}
		}
	}
}



void CActorProperty::_UpdateProp( void )
{
	// 데이터에서 값을 긁어온다.
	int iSelectedRegRes = TOOL_DATA.GetSelectedRegResIndex();
	if( wxNOT_FOUND != iSelectedRegRes )
	{
		// 현재 선택한 리소스가 액터가 아니면 액터 속성이 안보이므로 반영할 필요가 없다.
		if( CDnCutSceneData::RT_RES_ACTOR == TOOL_DATA.GetRegResKindByIndex( iSelectedRegRes ) )
		{
			//_RebuildProperty();

			wxString strActorName;
			TOOL_DATA.GetRegResNameByIndex( iSelectedRegRes, strActorName );
			m_pPropGrid->SetPropertyValueString( m_aPGID[ ACTOR_NAME ], strActorName );

			wxString strActorResName;
			strActorResName = strActorName.substr( 0, strActorName.find_last_of(wxT("_")) );
			m_pPropGrid->SetPropertyValueString( m_aPGID[ ACTOR_RES_NAME ], strActorResName );

			EtVector3 vPos = TOOL_DATA.GetRegResPos( strActorName.c_str() );

			wxVariantData_EtVector3* pData = new wxVariantData_EtVector3(vPos);
			wxVariant Value( pData, wxT("EtVector3") );
			m_pPropGrid->SetPropertyValue( m_aPGID[ POSITION ], Value );

			float fRotation = TOOL_DATA.GetRegResRot( strActorName.c_str() );
			m_pPropGrid->SetPropertyValueDouble( m_aPGID[ ROTATION ], fRotation );

			bool bFitYPosToMap = TOOL_DATA.GetThisActorsFitYPosToMap( strActorName.c_str() );
			m_pPropGrid->SetPropertyValueBool( m_aPGID[ FIT_YPOS_TO_MAP ], bFitYPosToMap );

			bool bInfluenceLightmap = TOOL_DATA.GetThisActorsInfluenceLightmap( strActorName.c_str() );
			m_pPropGrid->SetPropertyValueBool( m_aPGID[ INFLUENCE_LIGHTMAP ], bInfluenceLightmap );

			bool bScaleLock = TOOL_DATA.GetThisActorsScaleLock( strActorName.c_str() );
			m_pPropGrid->SetPropertyValueBool( m_aPGID[ SCALE_LOCK ], bScaleLock );
			

			// 이 액터로 설정 가능한 보스 셋팅을 찾아서 리스트 박스에 셋팅해준다.
			wxPGChoiceInfo ChoiceInfo;
			m_aPGID[ SELECT_BOSS ].GetProperty().GetChoiceInfo( &ChoiceInfo );
			while( ChoiceInfo.m_itemCount > 0 )
			{
				m_aPGID[ SELECT_BOSS ].GetProperty().DeleteChoice( 0 );
				m_aPGID[ SELECT_BOSS ].GetProperty().GetChoiceInfo( &ChoiceInfo );
			}

			vector<const S_MONSTER_INFO*> vlBossInfo;
			TOOL_DATA.GatherBossInfoOfThisActor( strActorResName, vlBossInfo );
			m_pPropGrid->AddPropertyChoice( m_aPGID[ SELECT_BOSS ], wxT("None") );		// 맨 첫번째는 None 으로 셋팅
			for( int i = 0; i < (int)vlBossInfo.size(); ++i )
			{
				const S_MONSTER_INFO* pBossMonsterInfo = vlBossInfo.at( i );
				wxString strBossInfo;
				strBossInfo.Printf( wxT("%s [MonsterTableID:%d] (Scale:%2.2f)"), pBossMonsterInfo->strName.c_str(),
																				 pBossMonsterInfo->iMonsterTableID, 
																				 pBossMonsterInfo->m_fScale );
				m_pPropGrid->AddPropertyChoice( m_aPGID[ SELECT_BOSS ], strBossInfo );
			}

			m_pPropGrid->SetPropertyValueLong( m_aPGID[ SELECT_BOSS ], 0 );
			bool bSetAsBoss = false;
			if( false == bScaleLock )
			{
				int iMonsterTableID = TOOL_DATA.GetMonsterTableID( strActorName.c_str() );
				for( int i = 0; i < (int)vlBossInfo.size(); ++i )
				{
					const S_MONSTER_INFO* pBossMonsterInfo = vlBossInfo.at( i );
					if( pBossMonsterInfo->iMonsterTableID == iMonsterTableID )
					{
						m_pPropGrid->SetPropertyValueLong( m_aPGID[ SELECT_BOSS ], i+1 );

						// 렌더러에 보이는 몬스터 액터의 사이즈를 조절..
						// 액터를 바꾸는 것이 가능한지 체크. 
						// 주로 애니메이션(액션)이 호환되는지 체크한다.1
						CRTCutSceneRenderer* pRenderer = static_cast<CRTCutSceneRenderer*>(TOOL_DATA.GetRenderer());
						pRenderer->SetActorScale( strActorName.c_str(), pBossMonsterInfo->m_fScale );
						bSetAsBoss = true;
						break;
					}
				}
			}

			if( false == bSetAsBoss )
			{
				CRTCutSceneRenderer* pRenderer = static_cast<CRTCutSceneRenderer*>(TOOL_DATA.GetRenderer());
				pRenderer->SetActorScale( strActorName.c_str(), 1.0f );
			}

			m_strPrevActorName = strActorName;
			m_vPrevPos = vPos;
			m_fPrevRotation = fRotation;
			m_bPrevInfluenceLightmap = bInfluenceLightmap;
			m_bPrevScaleLock = bScaleLock;

			m_pPropGrid->HideProperty( m_aPGID[ SELECT_BOSS ], bScaleLock );
		}
	}
}



void CActorProperty::OnShow( void )
{
	_UpdateProp();
}


void CActorProperty::ClearTempData( void )
{
	m_strPrevActorName.clear();
	m_vPrevPos.x = 0.0f; m_vPrevPos.y = 0.0f; m_vPrevPos.z = 0.0f;
	m_fPrevRotation = 0.0f;
}


void CActorProperty::CommandPerformed( ICommand* pCommand )
{
	if( false == TOOL_DATA.IsRegResource( m_strPrevActorName.c_str() ) )
	{
		m_pPropGrid->Show( false );
	}
	else
	{
		switch( pCommand->GetTypeID() )
		{
			case CMD_UPDATE_VIEW:
			case CMD_ACTOR_PROP_CHANGE:
			case CMD_CHANGE_ACTOR_RES:
				_UpdateProp();
				break;
		}
	}
}