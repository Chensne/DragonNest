#include "StdAfx.h"
#include "DnCutSceneData.h"
#include <assert.h>
#include "LuaDelegate.h"
#include "EtSoundEngine.h"

#if !defined( USE_BOOST_MEMPOOL )
#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif
#endif // #if !defined( USE_BOOST_MEMPOOL ) 



EtVector3 vIdentity(0.0f, 0.0f, 0.0f);
const char* CUTSCENEDATA_TABLE_NAME = "DN_CutSceneData";
const char* CUTSCENEDATA_VERSION_KEY = "Version";
const char* CS_RES_SET_TABLE_KEY = "Used_Resource_Set";
//const char* CS_RES_TABLE_KEY = "Resource";
const char* CS_ACT_SEQUENCE_SET_KEY = "Action_Sequence_Set";
const char* CS_KEY_SEQUENCE_SET_KEY = "Key_Sequence_Set";
const char* CS_EVENT_SEQUENCE_SET_KEY = "Event_Sequence_Set";
const char* CS_NAME_KEY = "Name";
const char* CS_TYPE_KEY = "Type";
const char* CS_MONSTER_TABLE_ID = "MonsterTableIDAsBoss";		// 현재 보스 뿐만 아니라 일반몹도 셋팅할 수 있다.
const char* CS_SCENE_START_FADEOUT_TIME = "SceneStart_FadeOut_Time";
const char* CS_SCENE_END_FADEIN_TIME = "SceneEnd_FadeIn_Time";
const char* CS_USE_SCENE_END_CLIPPING = "Use_Scene_End_Clipping";
const char* CS_SCENE_END_CLIPPING_TIME = "Scene_End_Clipping_Time";
const char* CS_SCENE_USE_SIGNAL_SOUND = "Scene_Use_Signal_Sound";
const char* CS_SCENE_ACADEMIC_NOT_SUPPORTED = "Scene_Academic_Not_Supported";
const char* CS_SCENE_MAIN_CHAR_NAME = "Scene_Main_Char_Name";
const char* CS_SCENE_FOGFAR_DELTA = "Scene_FogFar_Delta";
const char* CS_ACTOR_NAME_KEY = "Actor_Name";
const char* CS_START_TIME_KEY = "Start_Time";
const char* CS_TIME_LENGTH_KEY = "Time_Length";
const char* CS_UNIT_KEY = "Unit";
const char* CS_SPEED_KEY = "Speed";
const char* CS_USE_ANI_DIST_KEY = "Use_Ani_Distance";
const char* CS_ANI_INDEX_KEY = "Ani_Index";
const char* CS_ANI_NAME_KEY = "Ani_Name";
const char* CS_ANI_SPEED_KEY = "Ani_Speed";
const char* CS_ANI_NEXT_FRAME_KEY = "Ani_Next_Frame";
const char* CS_ID_KEY = "ID";
const char* CS_ROT_KEY = "Rotation";
const char* CS_START_X = "Start_X";
const char* CS_START_Y = "Start_Y";
const char* CS_START_Z = "Start_Z";
const char* CS_DEST_X = "Dest_X";
const char* CS_DEST_Y = "Dest_Y";
const char* CS_DEST_Z = "Dest_Z";
const char* CS_ROTATEFRONT_KEY = "RotateFront";
const char* CS_YPOS_TO_MAP_KEY = "YPos_To_Map";
const char* CS_INFLUENCE_LIGHTMAP = "Influence_Lightmap";
const char* CS_LOCK_SCALE = "LockScale";
const char* CS_SUBKEYSET_TABLE_KEY = "SubKeySet";
const char* CS_FILEPATH_KEY = "FilePath";
const char* CS_FILENAME_KEY = "FileName";
const char* CS_NEXT_ACTION_NAME_KEY = "Next_Action";
const char* CS_LINK_ANI_NAME_KEY = "Link_Ani";
const char* CS_NEXT_ACTION_FRAME_KEY = "Next_Action_Frame";
const char* CS_BLEND_FRAME_KEY = "Blend_Frame";
const char* CS_ACTION_ELEMENT_SET_KEY = "Action_Element_Set";
const char* CS_USE_START_ROTATION = "Use_Start_Rotation";
const char* CS_USE_START_POSITION = "Use_Start_Position";

// 카메라 데이터 관련
const char* CS_CAMERA_DATA_KEY = "CameraDataSet";
const char* CS_CAMERA_HEADERSTRING_KEY = "Camera_Header_String";
const char* CS_FOV_COUNT_KEY = "FOV_Count";
const char* CS_POS_COUNT_KEY = "Position_Count";
const char* CS_ROT_COUNT_KEY = "Rotation_Count";
const char* CS_LOOKAT_COUNT_KEY = "LookAt_Count";
const char* CS_DEFAULT_FOV_KEY = "Default_FOV";
const char* CS_DEFAULT_POS_X = "Default_Pos_X";
const char* CS_DEFAULT_POS_Y = "Default_Pos_Y";
const char* CS_DEFAULT_POS_Z = "Default_Pos_Z";
const char* CS_DEFAULT_ROT_X = "Default_Rot_X";
const char* CS_DEFAULT_ROT_Y = "Default_Rot_Y";
const char* CS_DEFAULT_ROT_Z = "Default_Rot_Z";
const char* CS_DEFAULT_ROT_W = "Default_Rot_W";
const char* CS_DEFAULT_LOOKAT_X = "Default_LookAt_X";
const char* CS_DEFAULT_LOOKAT_Y = "Default_LookAt_Y";
const char* CS_DEFAULT_LOOKAT_Z = "Default_LookAt_Z";

const char* CS_FOV_SET_KEY = "FOVKeySet";
const char* CS_POS_SET_KEY = "POSKeySet";
const char* CS_ROT_SET_KEY = "RotKeySet";
const char* CS_LOOKAT_SET_KEY = "LookAtKeySet";
const char* CS_FRAME_KEY = "Frame";
const char* CS_FOV_KEY = "FOV";
const char* CS_TRACE_TYPE_KEY = "Trace_Type";
const char* CS_ACTOR_TO_TRACE_KEY = "Actor_Name_to_Trace";
const char* CS_ACTORS_IN_CAMERA_KEY = "Actors_In_Camera";

const char* CS_FADE_KEY = "Fade_In_Out";
const char* CS_PROP_NAME = "PropName";
const char* CS_PROP_ID = "PropID";
const char* CS_PROP_SHOW = "PropShow";
const char* CS_PROP_ACTION_NAME = "Prop_Action_Name";
const char* CS_VOLUME = "Volume";
const char* CS_RED = "Red";
const char* CS_GREEN = "Green";
const char* CS_BLUE = "Blue";

const char* CS_DOF_ENABLE = "DOF_Enable";
const char* CS_DOF_USE_NOW_VALUE_AS_START = "DOF_Use_Now_Value_As_Start";
const char* CS_DOF_NEAR_START_FROM = "DOF_Near_Start_From";
const char* CS_DOF_NEAR_END_FROM = "DOF_Near_End_From";
const char* CS_DOF_FAR_START_FROM = "DOF_Far_Start_From";
const char* CS_DOF_FAR_END_FROM = "DOF_Far_End_From"; 
const char* CS_DOF_FOCUS_DIST_FROM = "DOF_Focus_Dist_From";
const char* CS_DOF_NEAR_BLUR_SIZE_FROM = "Near_Blur_Size_From"; 
const char* CS_DOF_FAR_BLUR_SIZE_FROM = "DOF_Far_Blur_Size_From"; 
const char* CS_DOF_NEAR_START_DEST = "DOF_Near_Start_Dest"; 
const char* CS_DOF_NEAR_END_DEST = "DOF_Near_End_Dest"; 
const char* CS_DOF_FAR_START_DEST = "DOF_Far_Start_Dest"; 
const char* CS_DOF_FAR_END_DEST = "DOF_Far_End_Dest"; 
const char* CS_DOF_FOCUS_DIST_DEST = "DOF_Focus_Dist_Dest"; 
const char* CS_DOF_NEAR_BLUR_SIZE_DEST = "DOF_Near_Blur_Size_Dest"; 
const char* CS_DOF_FAR_BLUR_SIZE_DEST = "DOF_Far_Blur_Size_Dest";

const char* CS_SUBTITLE_UISTRING_ID = "Subtitle_UIString_ID";
//const char* CS_SUBTITLE_COLOR = "Subtitle_Color";


//#ifdef PRE_ADD_FILTEREVENT
const char* CS_COLORFILTER_MONO = "COLORFILTER_MONO";
const char* CS_COLORFILTER_R = "COLORFILTER_RED";
const char* CS_COLORFILTER_G = "COLORFILTER_GREEN";
const char* CS_COLORFILTER_B = "COLORFILTER_BLUE";
const char* CS_COLORFILTER_VOL = "COLORFILTER_VOLUME";
//#endif // PRE_ADD_FILTEREVENT

const float CUTSCENEDATA_VERSION = 1.0f;



CDnCutSceneData::CDnCutSceneData( void ) : m_dwNowActionIDOffset( 0 )
{
	
}


CDnCutSceneData::~CDnCutSceneData(void)
{
	Clear();
}



DWORD CDnCutSceneData::GenerateID( void )
{
	return m_dwNowActionIDOffset++;
}



bool CDnCutSceneData::RegisterResource( /*int iTableID,*/ const char* pResName, int iResourceKind )
{
	bool bResult = false;

	map<string, S_USE_RESOURCE*>::iterator iter = m_mapUseRes.find( string(pResName) );
	bool bNotRegistered = (m_mapUseRes.end() == iter);
	bool bValidResType = (-1 != iResourceKind);
	assert( bNotRegistered && "CDnCutSceneData::RegisterResource() -> 이미 등록된 리소스입니다." );
	assert( bValidResType  && "CDnCutSceneData::RegisterResource() -> 잘못된 리소스 종류입니다." );
	if( bNotRegistered && bValidResType )
	{
		S_USE_RESOURCE* pNewResource = NULL;

		switch( iResourceKind )
		{
			case RT_RES_MAP:
				pNewResource = new S_USE_RESOURCE;
				break;

			case RT_RES_ACTOR:
				{
					pNewResource = new S_ACTOR_RESOURCE;
					//static_cast<S_ACTOR_RESOURCE*>(pNewResource)->iActorTableID = iTableID;
					m_vlActors.push_back( pNewResource );
					m_mapActionSequenceDB.insert( make_pair( string(pResName), vector<ActionInfo*>() ) );
					m_mapKeySequenceDB.insert( make_pair( string(pResName), vector<KeyInfo*>() ) );

					// 카메라 이벤트가 있는 경우도 뒤져서 업데이트 해준다..
					int iNumCamEvent = (int)m_avlEventSequenceDB[ EventInfo::CAMERA ].size();
					for( int iEvent = 0; iEvent < iNumCamEvent; ++iEvent )
					{
						CamEventInfo* pEventInfo = static_cast<CamEventInfo*>(m_avlEventSequenceDB[ EventInfo::CAMERA ].at( iEvent ));
						pEventInfo->mapActorsShowHide.insert( make_pair(pResName, true) );		// 디폴트 값은 true 임.
					}
				}
				break;

			//case RT_RES_MAX_CAMERA:
			//	pNewResource = new S_USE_RESOURCE;
			//	break;

			case RT_RES_SOUND:
				pNewResource = new S_USE_RESOURCE;
				break;
		}

		pNewResource->strResName.assign( pResName );
		pNewResource->iResourceKind = iResourceKind;

		m_mapUseRes.insert( make_pair(pNewResource->strResName, pNewResource) );
		m_vlUseRes.push_back( pNewResource );

		bResult = true;
	}

	return bResult;
}




bool CDnCutSceneData::UnRegisterResource( const char* pResName, int iResourceKind )
{
	bool bResult = false;

	map<string, S_USE_RESOURCE*>::iterator iter = m_mapUseRes.find( string(pResName) );
	assert( m_mapUseRes.end() != iter && "CDnCutSceneData::UnRegisterResource() -> 등록되지 않은 리소스는 삭제할 수 없습니다." );
	if( m_mapUseRes.end() != iter )
	{
		S_USE_RESOURCE* pToDelete = iter->second;

		vector<S_USE_RESOURCE*>::iterator iterVec = find( m_vlUseRes.begin(), m_vlUseRes.end(), pToDelete );
		
		m_vlUseRes.erase( iterVec );
		m_mapUseRes.erase( iter );

		// 액터인 경우엔 딸려있는 액션들까지 모두 삭제해야 한다.
		if( RT_RES_ACTOR == pToDelete->iResourceKind )
		{
			// 액션 관련 데이터 삭제
			map< string, vector<ActionInfo*> >::iterator iterMASDB = m_mapActionSequenceDB.find( pToDelete->strResName );
			assert( iterMASDB != m_mapActionSequenceDB.end() && "CDnCutSceneData::UnRegisterResource() -> 삭제할 액터가 액션 시퀀스 DB 에 없습니다." );
			vector<ActionInfo*>& vlActionSequence = iterMASDB->second;

			for( int iAction = 0; iAction < (int)vlActionSequence.size(); ++iAction )
			{
				map<int, ActionInfo*>::iterator iterAIDT = m_mapActionIDTable.find( vlActionSequence.at(iAction)->iID );
				assert( m_mapActionIDTable.end() != iterAIDT );
				m_mapActionIDTable.erase( iterAIDT );
			}

			for_each( vlActionSequence.begin(), vlActionSequence.end(), DeleteData<ActionInfo*>() );
			vlActionSequence.clear();

			m_mapActionSequenceDB.erase( iterMASDB );

			// 키 관련 데이터 삭제
			map< string, vector<KeyInfo*> >::iterator iterKSDB = m_mapKeySequenceDB.find( pToDelete->strResName );
			assert( iterKSDB != m_mapKeySequenceDB.end() && "CDnCutSceneData::UnRegisterResource() -> 삭제할 키가 키 시퀀스 DB 에 없습니다." );
			vector<KeyInfo*>& vlKeySequence = iterKSDB->second;

			for( int iKey = 0; iKey < (int)vlKeySequence.size(); ++iKey )
			{
				map<int, KeyInfo*>::iterator iterKIDT = m_mapKeyIDTable.find( vlKeySequence.at(iKey)->iID );
				assert( m_mapKeyIDTable.end() != iterKIDT );
				m_mapKeyIDTable.erase( iterKIDT );
			}
			
			for_each( vlKeySequence.begin(), vlKeySequence.end(), DeleteData<KeyInfo*>() );
			vlKeySequence.clear();

			m_mapKeySequenceDB.erase( iterKSDB );

			bool bDelActor = false;
			int iNumActors = (int)m_vlActors.size();
			for( int iActor = 0; iActor < iNumActors; ++iActor )
			{
				if( m_vlActors.at( iActor )->strResName == pResName )
				{
					m_vlActors.erase( m_vlActors.begin()+iActor );
					bDelActor = true;
					break;
				}
			}
			assert( bDelActor && "CDnCutSceneData::UnRegisterResource() -> 삭제할 액터가 액터 리스트에 없습니다." );

			// 카메라 이벤트에 걸려있는 이 녀석 이름도 삭제
			int iNumCamEvent = (int)m_avlEventSequenceDB[ EventInfo::CAMERA ].size();
			for( int iEvent = 0; iEvent < iNumCamEvent; ++iEvent )
			{
				CamEventInfo* pEventInfo = static_cast<CamEventInfo*>(m_avlEventSequenceDB[ EventInfo::CAMERA ].at( iEvent ));
				map<string, bool>::iterator iterInner = pEventInfo->mapActorsShowHide.find( pToDelete->strResName );
				_ASSERT( pEventInfo->mapActorsShowHide.end() != iterInner );
				pEventInfo->mapActorsShowHide.erase( iterInner );
			}
		}

		delete pToDelete;

		bResult = true;
	}

	return bResult;
}


bool CDnCutSceneData::SetMonsterTableID( const char* pActorName, int iMonsterTableID )
{
	bool bSuccess = false;

	map<string, S_USE_RESOURCE*>::iterator iter = m_mapUseRes.find( string(pActorName) );
	assert( m_mapUseRes.end() != iter && "CDnCutSceneData::UnRegisterResource() -> 등록되지 않은 액터입니다." );
	if( m_mapUseRes.end() != iter )
	{
		static_cast<S_ACTOR_RESOURCE*>(iter->second)->iMonsterTableID = iMonsterTableID;
		bSuccess = true;
	}

	return bSuccess;
}


int CDnCutSceneData::GetMonsterTableID( const char* pActorName )
{
	int iResult = 0;

	map<string, S_USE_RESOURCE*>::iterator iter = m_mapUseRes.find( string(pActorName) );
	assert( m_mapUseRes.end() != iter && "CDnCutSceneData::UnRegisterResource() -> 등록되지 않은 액터입니다." );
	if( m_mapUseRes.end() != iter )
	{
		iResult = static_cast<S_ACTOR_RESOURCE*>(iter->second)->iMonsterTableID;
	}

	return iResult;
}


void CDnCutSceneData::_UpdateOrderActionSequence( const char* pActorName )
{
	vector<ActionInfo*>& vlActionSequence = m_mapActionSequenceDB[ pActorName ];

	struct SortByStartTime : public binary_function<const ActionInfo*, const ActionInfo*, bool>
	{
		bool operator () ( const ActionInfo* pA, const ActionInfo* pB )
		{
			return pA->fStartTime < pB->fStartTime;/*((pB->fStartTime+pB->fTimeLength);*/
		}
	};

	sort( vlActionSequence.begin(), vlActionSequence.end(), SortByStartTime() );
}





bool CDnCutSceneData::InsertAction( ActionInfo* pActionInfo )
{
	bool bResult = false;

	// 해당 키 값이 없는 새로운 액터의 액션인 경우 자동으로 insert 됨.
	vector<ActionInfo*>& vlActionSequence = m_mapActionSequenceDB[ pActionInfo->strActorName ];

	vector<ActionInfo*>::iterator iter = find( vlActionSequence.begin(), vlActionSequence.end(), pActionInfo );
	if( vlActionSequence.end() == iter )
	{
		ActionInfo* pNewActionInfo = new ActionInfo(*pActionInfo);
		vlActionSequence.push_back( pNewActionInfo );
		m_mapActionIDTable.insert( make_pair(pNewActionInfo->iID, pNewActionInfo) );

		_UpdateOrderActionSequence( pActionInfo->strActorName.c_str() );
		
		bResult = true;
	}

	return bResult;
}




bool CDnCutSceneData::RemoveAction( int iID )
{
	bool bResult = false;

	map<int, ActionInfo*>::iterator iterIDTableMap = m_mapActionIDTable.find( iID );

	if( iterIDTableMap != m_mapActionIDTable.end() )
	{
		ActionInfo* pToDelete = iterIDTableMap->second;
		m_mapActionIDTable.erase( iterIDTableMap );

		map<string, vector<ActionInfo*> >::iterator iterASDB = m_mapActionSequenceDB.find( pToDelete->strActorName );
		assert( iterASDB != m_mapActionSequenceDB.end() && "CDnCutSceneData::RemoveAction() -> 액션 시퀀스 DB에서 해당 액터를 찾을 수 없습니다." ); 
		vector<ActionInfo*>& vlActionSequence = iterASDB->second;
		vector<ActionInfo*>::iterator iterVAS = find( vlActionSequence.begin(), vlActionSequence.end(), pToDelete );
		assert( iterVAS != vlActionSequence.end() && "CDnCutSceneData::RemoveAction() -> 해당 액터의 액션 시퀀스에서 삭제할 액션 객체를 찾을 수 없습니다." ); 
		vlActionSequence.erase( iterVAS );

		delete pToDelete;

		bResult = true;
	}
	
	return bResult;
}



bool CDnCutSceneData::ModifyActionInfo( ActionInfo& Action )
{
	bool bResult = false;

	map<int, ActionInfo*>::iterator iter = m_mapActionIDTable.find( Action.iID );
	if( m_mapActionIDTable.end() != iter )
	{
		bool bChangeOrderPossiblity = false;
		ActionInfo* pOriActionInfo = iter->second;

		if( pOriActionInfo->fStartTime != Action.fStartTime ||
			pOriActionInfo->fTimeLength != Action.fTimeLength )
			bChangeOrderPossiblity = true;

		*(iter->second) = Action;

		if( bChangeOrderPossiblity )
			_UpdateOrderActionSequence( Action.strActorName.c_str() );

		bResult = true;
	}

	return bResult;
}



void CDnCutSceneData::_UpdateOrderKeySequence( const char* pActorName )
{
	vector<KeyInfo*>& vlKeySequence = m_mapKeySequenceDB[ string(pActorName) ];

	struct SortByStartTime : public binary_function<const KeyInfo*, const KeyInfo*, bool>
	{
		bool operator () ( const KeyInfo* pA, const KeyInfo* pB )
		{
			return pA->fStartTime < pB->fStartTime;/*((pB->fStartTime+pB->fTimeLength);*/
		}
	};

	sort( vlKeySequence.begin(), vlKeySequence.end(), SortByStartTime() );
}


void CDnCutSceneData::_UpdateOrderEventSequence( int iEventType )
{
	vector<EventInfo*>& vlEventSequence = m_avlEventSequenceDB[ iEventType ];

	struct SortByStartTime : public binary_function<const EventInfo*, const EventInfo*, bool>
	{
		bool operator () ( const EventInfo* pA, const EventInfo* pB )
		{
			return pA->fStartTime < pB->fStartTime;
		}
	};

	sort( vlEventSequence.begin(), vlEventSequence.end(), SortByStartTime() );
}



bool CDnCutSceneData::InsertKey( KeyInfo* pKeyInfo )
{
	bool bResult = false;

	// 해당 키 값이 없는 새로운 액터의 액션인 경우 자동으로 insert 됨.
	vector<KeyInfo*>& vlKeySequence = m_mapKeySequenceDB[ pKeyInfo->strActorName ];

	vector<KeyInfo*>::iterator iter = find( vlKeySequence.begin(), vlKeySequence.end(), pKeyInfo );
	if( vlKeySequence.end() == iter )
	{
		KeyInfo* pNewKeyInfo = new KeyInfo(*pKeyInfo);
		vlKeySequence.push_back( pNewKeyInfo );
		m_mapKeyIDTable.insert( make_pair(pNewKeyInfo->iID, pNewKeyInfo) );

		// TODO: 시작 시간 순서대로 정렬. 함수객체 사용.
		_UpdateOrderKeySequence( pKeyInfo->strActorName.c_str() );

		bResult = true;
	}

	return bResult;
}


bool CDnCutSceneData::ModifyKeyInfo( KeyInfo& Key )
{
	bool bResult = false;

	map<int, KeyInfo*>::iterator iter = m_mapKeyIDTable.find( Key.iID );
	if( m_mapKeyIDTable.end() != iter )
	{
		bool bChangeOrderPossiblity = false;
		KeyInfo* pOriKeyInfo = iter->second;

		if( pOriKeyInfo->fStartTime != Key.fStartTime ||
			pOriKeyInfo->fTimeLength != Key.fTimeLength )
			bChangeOrderPossiblity = true;

		*(iter->second) = Key;

		if( bChangeOrderPossiblity )
			_UpdateOrderKeySequence( Key.strActorName.c_str() );

		bResult = true;
	}

	return bResult;
}


bool CDnCutSceneData::RemoveKey( int iID )
{
	bool bResult = false;

	map<int, KeyInfo*>::iterator iterIDTableMap = m_mapKeyIDTable.find( iID );

	if( iterIDTableMap != m_mapKeyIDTable.end() )
	{
		KeyInfo* pToDelete = iterIDTableMap->second;
		m_mapKeyIDTable.erase( iterIDTableMap );

		map<string, vector<KeyInfo*> >::iterator iterASDB = m_mapKeySequenceDB.find( pToDelete->strActorName );
		assert( iterASDB != m_mapKeySequenceDB.end() && "CDnCutSceneData::RemoveKey() -> 액션 시퀀스 DB에서 해당 액터를 찾을 수 없습니다." ); 
		vector<KeyInfo*>& vlKeySequence = iterASDB->second;
		vector<KeyInfo*>::iterator iterVAS = find( vlKeySequence.begin(), vlKeySequence.end(), pToDelete );
		assert( iterVAS != vlKeySequence.end() && "CDnCutSceneData::RemoveKey() -> 해당 액터의 액션 시퀀스에서 삭제할 액션 객체를 찾을 수 없습니다." ); 
		vlKeySequence.erase( iterVAS );

		delete pToDelete;

		bResult = true;
	}
	else
	{
		// 못 찾은 경우 서브키 아이디일 가능성이 있으므로 뒤져서 삭제토록 한다.
		map<int, KeyInfo*>::iterator iter = m_mapKeyIDTable.begin();
		for( iter; iter != m_mapKeyIDTable.end(); ++iter )
		{
			const KeyInfo* pKeyInfo = iter->second;
			if( pKeyInfo->iKeyType )
			{
				vector<SubKey>::const_iterator iterSubKey = find( pKeyInfo->vlMoveKeys.begin(), pKeyInfo->vlMoveKeys.end(), iID );
				if( pKeyInfo->vlMoveKeys.end() != iterSubKey )
					RemoveKey( pKeyInfo->iID );
			}
		}
	}

	return bResult;
}



bool CDnCutSceneData::IsRegResource( const char* pResName )
{
	bool bResult = false;

	map<string, S_USE_RESOURCE*>::iterator iter = m_mapUseRes.find( string(pResName) );
	if( m_mapUseRes.end() != iter )
		bResult = true;

	return bResult;
}


int CDnCutSceneData::GetNumActors( void ) const
{
	return (int)m_vlActors.size();
}



const char* CDnCutSceneData::GetActorByIndex( int iActorIndex )
{
	const char* pResult = NULL;

	if( iActorIndex < (int)m_vlActors.size() )
		pResult = m_vlActors.at( iActorIndex )->strResName.c_str();

	return pResult;
}


const ActionInfo* CDnCutSceneData::GetActionInfoByID( int iID )
{
	ActionInfo* pResult = NULL;

	map<int, ActionInfo*>::iterator iter = m_mapActionIDTable.find( iID );

	if( m_mapActionIDTable.end() != iter )
	{
		pResult = iter->second;
	}

	return pResult;
}



int CDnCutSceneData::GetThisActorsActionNum( const char* pActorName )
{
	int iResult = -1;

	map< string, vector<ActionInfo*> >::iterator iterMap = m_mapActionSequenceDB.find( string(pActorName) );
	if( m_mapActionSequenceDB.end() != iterMap )
	{
		iResult = (int)iterMap->second.size();
	}

	return iResult;
}



const ActionInfo* CDnCutSceneData::GetThisActorsActionInfoByIndex( const char* pActorName, int iActionIndex )
{
	const ActionInfo* pResult = NULL;

	map< string, vector<ActionInfo*> >::iterator iterMap = m_mapActionSequenceDB.find( string(pActorName) );
	if( m_mapActionSequenceDB.end() != iterMap )
	{
		vector<ActionInfo*>& vlActionSequence = iterMap->second;
		if( iActionIndex < (int)vlActionSequence.size() )
		{
			pResult = vlActionSequence.at(iActionIndex );
			assert( !pResult->strActorName.empty() );
		}
	}

	return pResult;
}



const KeyInfo* CDnCutSceneData::GetKeyInfoByID( int iID )
{
	KeyInfo* pResult = NULL;

	map<int, KeyInfo*>::iterator iter = m_mapKeyIDTable.find( iID );

	if( m_mapKeyIDTable.end() != iter )
	{
		pResult = iter->second;
	}
	else
	{
		// 못 찾은 경우 서브키 아이디일 가능성이 있으므로 뒤져본다.
		iter = m_mapKeyIDTable.begin();
		for( iter; iter != m_mapKeyIDTable.end(); ++iter )
		{
			const KeyInfo* pKeyInfo = iter->second;
			if( pKeyInfo->iKeyType )
			{
				vector<SubKey>::const_iterator iterSubKey = find( pKeyInfo->vlMoveKeys.begin(), pKeyInfo->vlMoveKeys.end(), iID );
				if( pKeyInfo->vlMoveKeys.end() != iterSubKey )
				{
					pResult = iter->second;
					break;
				}
			}
		}
	}

	return pResult;
}


int CDnCutSceneData::GetThisActorsKeyNum( const char* pActorName )
{
	int iResult = -1;

	map< string, vector<KeyInfo*> >::iterator iterMap = m_mapKeySequenceDB.find( string(pActorName) );
	if( m_mapKeySequenceDB.end() != iterMap )
	{
		iResult = (int)iterMap->second.size();
	}

	return iResult;
}


const KeyInfo* CDnCutSceneData::GetThisActorsKeyInfoByIndex( const char* pActorName, int iKeyIndex )
{
	const KeyInfo* pResult = NULL;

	map< string, vector<KeyInfo*> >::iterator iterMap = m_mapKeySequenceDB.find( string(pActorName) );
	if( m_mapKeySequenceDB.end() != iterMap )
	{
		vector<KeyInfo*>& vlKeySequence = iterMap->second;
		if( iKeyIndex < (int)vlKeySequence.size() )
		{
			pResult = vlKeySequence.at(iKeyIndex );
			assert( !pResult->strActorName.empty() );
		}
	}

	return pResult;
}




int CDnCutSceneData::GetThisActorsAnimationNum( const char* pActorName )
{
	int iResult = 0;

	map<string, S_USE_RESOURCE*>::iterator iter =  m_mapUseRes.find( string(pActorName) );
	if( m_mapUseRes.end() != iter )
	{
		assert( RT_RES_ACTOR == iter->second->iResourceKind );
		S_ACTOR_RESOURCE* pActorRes = static_cast<S_ACTOR_RESOURCE*>( iter->second );

		iResult = (int)pActorRes->vlpActionElements.size();
	}

	return iResult;
}



bool CDnCutSceneData::GetThisActorsFitYPosToMap( const char* pActorName )
{
	bool bResult = false;

	map<string, S_USE_RESOURCE*>::iterator iter =  m_mapUseRes.find( string(pActorName) );
	if( m_mapUseRes.end() != iter )
	{
		assert( RT_RES_ACTOR == iter->second->iResourceKind );
		S_ACTOR_RESOURCE* pActorRes = static_cast<S_ACTOR_RESOURCE*>( iter->second );

		bResult = pActorRes->bFitYPosToMap;
	}

	return bResult;
}


bool CDnCutSceneData::GetThisActorsInfluenceLightmap( const char* pActorName )
{
	bool bResult = false;

	map<string, S_USE_RESOURCE*>::iterator iter =  m_mapUseRes.find( string(pActorName) );
	if( m_mapUseRes.end() != iter )
	{
		assert( RT_RES_ACTOR == iter->second->iResourceKind );
		S_ACTOR_RESOURCE* pActorRes = static_cast<S_ACTOR_RESOURCE*>( iter->second );

		bResult = pActorRes->bInfluenceLightmap;
	}

	return bResult;
}


bool CDnCutSceneData::GetThisActorsScaleLock( const char* pActorName )
{
	bool bResult = false;

	map<string, S_USE_RESOURCE*>::iterator iter =  m_mapUseRes.find( string(pActorName) );
	if( m_mapUseRes.end() != iter )
	{
		assert( RT_RES_ACTOR == iter->second->iResourceKind );
		S_ACTOR_RESOURCE* pActorRes = static_cast<S_ACTOR_RESOURCE*>( iter->second );

		bResult = pActorRes->bScaleLock;
	}

	return bResult;
}


const ActionEleInfo* CDnCutSceneData::GetThisActorsAnimation( const char* pActorName, int iAnimationIndex )
{
	const ActionEleInfo* pResult = NULL;

	map<string, S_USE_RESOURCE*>::iterator iter =  m_mapUseRes.find( string(pActorName) );
	if( m_mapUseRes.end() != iter )
	{
		assert( RT_RES_ACTOR == iter->second->iResourceKind );
		S_ACTOR_RESOURCE* pActorRes = static_cast<S_ACTOR_RESOURCE*>( iter->second );

		if( iAnimationIndex < (int)pActorRes->vlpActionElements.size() )
			pResult = pActorRes->vlpActionElements.at( iAnimationIndex );	
	}

	return pResult;
}





const EtVector3& CDnCutSceneData::GetRegResPos( const char* pResName )
{
	map<string, S_USE_RESOURCE*>::iterator iter = m_mapUseRes.find( string(pResName) );
	assert( m_mapUseRes.end() != iter && "CDnCutSceneData::GetRegResPos() -> 해당되는 리소스를 찾을 수 없습니다."  );
	if( m_mapUseRes.end() != iter )
	{
		S_USE_RESOURCE* pRes = iter->second;
		if( RT_RES_ACTOR == pRes->iResourceKind )
		{
			return static_cast<S_ACTOR_RESOURCE*>(pRes)->vPos;
		}
	}

	return vIdentity;
}



float CDnCutSceneData::GetRegResRot( const char* pResName )
{
	map<string, S_USE_RESOURCE*>::iterator iter = m_mapUseRes.find( string(pResName) );
	assert( m_mapUseRes.end() != iter && "CDnCutSceneData::GetRegResPos() -> 해당되는 리소스를 찾을 수 없습니다."  );
	if( m_mapUseRes.end() != iter )
	{
		S_USE_RESOURCE* pRes = iter->second;
		if( RT_RES_ACTOR == pRes->iResourceKind )
		{
			return static_cast<S_ACTOR_RESOURCE*>(pRes)->fRotation;
		}
	}

	return 0.0f;
}



bool CDnCutSceneData::ChangeRegResName( const char* pOriResName, const char* pNewResName )
{
	bool bResult = false;

	// 리소스 이름이 키 값으로 쓰이므로 거의 모든 것이 다 바뀐다.
	map<string, S_USE_RESOURCE*>::iterator iterUR = m_mapUseRes.find( string(pOriResName) );
	if( m_mapUseRes.end() != iterUR )
	{
		S_USE_RESOURCE* pRes = iterUR->second;
		pRes->strResName.assign( pNewResName );

		m_mapUseRes.erase( iterUR );
		m_mapUseRes.insert( make_pair(string(pNewResName), pRes) );

		switch( pRes->iResourceKind )
		{
			case RT_RES_MAP:
				bResult = true;
				break;

			case RT_RES_ACTOR:
				{
					map<string, vector<ActionInfo*> >::iterator iterASDB = m_mapActionSequenceDB.find( string(pOriResName) );
					if( m_mapActionSequenceDB.end() != iterASDB )
					{
						vector<ActionInfo*> vlActions = iterASDB->second;
						int iNumActions = (int)vlActions.size();
						for( int iAction = 0; iAction < iNumActions; ++iAction )
							vlActions.at( iAction )->strActorName.assign( pNewResName );

						m_mapActionSequenceDB.insert( make_pair(string(pNewResName), vlActions) );
						m_mapActionSequenceDB.erase( iterASDB );
					}

					map<string, vector<KeyInfo*> >::iterator iterKSDB = m_mapKeySequenceDB.find( string(pOriResName) );
					if( m_mapKeySequenceDB.end() != iterKSDB )
					{
						vector<KeyInfo*> vlKeys = iterKSDB->second;
						int iNumKeys = (int)vlKeys.size();
						for( int iKey = 0; iKey < iNumKeys; ++iKey )
							vlKeys.at( iKey )->strActorName.assign( pNewResName );

						m_mapKeySequenceDB.insert( make_pair(string(pNewResName), vlKeys) );
						m_mapKeySequenceDB.erase( iterKSDB );
					}


					// 카메라 이벤트가 있는 경우도 뒤져서 업데이트 해준다..
					int iNumCamEvent = (int)m_avlEventSequenceDB[ EventInfo::CAMERA ].size();
					for( int iEvent = 0; iEvent < iNumCamEvent; ++iEvent )
					{
						CamEventInfo* pEventInfo = static_cast<CamEventInfo*>(m_avlEventSequenceDB[ EventInfo::CAMERA ].at( iEvent ));
						map<string, bool>::iterator iter = pEventInfo->mapActorsShowHide.begin();
						_ASSERT( pEventInfo->mapActorsShowHide.end() != iter );
						for( iter; iter != pEventInfo->mapActorsShowHide.end(); ++iter )
						{
							if( iter->first == pOriResName )
							{
								bool bShow = iter->second;
								pEventInfo->mapActorsShowHide.erase( iter );
								pEventInfo->mapActorsShowHide.insert( make_pair(pNewResName, bShow) );
								break;
							}
						}
					}

					bResult = true;
				}
				break;

			case RT_RES_SOUND:
				bResult = true;
				break;
		}
	}

	return bResult;
}




bool CDnCutSceneData::AddActorsAnimation( const char* pActorName, ActionEleInfo* pAnimationElement )
{
	bool bResult = false;

	map<string, S_USE_RESOURCE*>::iterator iter =  m_mapUseRes.find( string(pActorName) );
	if( m_mapUseRes.end() != iter )
	{
		assert( RT_RES_ACTOR == iter->second->iResourceKind );
		S_ACTOR_RESOURCE* pActorRes = static_cast<S_ACTOR_RESOURCE*>( iter->second );
		pActorRes->vlpActionElements.push_back( new ActionEleInfo( *pAnimationElement ) );

		bResult = true;
	}

	return bResult;
}



bool CDnCutSceneData::SetActorsFitYPosToMap( const char* pActorName, bool bFitYPosToMap )
{
	bool bResult = false;

	map<string, S_USE_RESOURCE*>::iterator iter =  m_mapUseRes.find( string(pActorName) );
	if( m_mapUseRes.end() != iter )
	{
		assert( RT_RES_ACTOR == iter->second->iResourceKind );
		S_ACTOR_RESOURCE* pActorRes = static_cast<S_ACTOR_RESOURCE*>( iter->second );
		pActorRes->bFitYPosToMap = bFitYPosToMap;

		bResult = true;
	}

	return bResult;
}



bool CDnCutSceneData::SetActorsInfluenceLightmap( const char* pActorName, bool bInfluenceLightmap )
{
	bool bResult = false;

	map<string, S_USE_RESOURCE*>::iterator iter =  m_mapUseRes.find( string(pActorName) );
	if( m_mapUseRes.end() != iter )
	{
		assert( RT_RES_ACTOR == iter->second->iResourceKind );
		S_ACTOR_RESOURCE* pActorRes = static_cast<S_ACTOR_RESOURCE*>( iter->second );
		pActorRes->bInfluenceLightmap = bInfluenceLightmap;

		bResult = true;
	}

	return bResult;
}


bool CDnCutSceneData::SetActorsScaleLock( const char* pActorName, bool bScaleLock )
{
	bool bResult = false;

	map<string, S_USE_RESOURCE*>::iterator iter =  m_mapUseRes.find( string(pActorName) );
	if( m_mapUseRes.end() != iter )
	{
		assert( RT_RES_ACTOR == iter->second->iResourceKind );
		S_ACTOR_RESOURCE* pActorRes = static_cast<S_ACTOR_RESOURCE*>( iter->second );
		pActorRes->bScaleLock = bScaleLock;

		bResult = true;
	}

	return bResult;
}


bool CDnCutSceneData::ClearActorsAnimations( const char* pActorName )
{
	bool bResult = false;

	map<string, S_USE_RESOURCE*>::iterator iter =  m_mapUseRes.find( string(pActorName) );
	if( m_mapUseRes.end() != iter )
	{
		assert( RT_RES_ACTOR == iter->second->iResourceKind );
		S_ACTOR_RESOURCE* pActorRes = static_cast<S_ACTOR_RESOURCE*>( iter->second );
		for_each( pActorRes->vlpActionElements.begin(), pActorRes->vlpActionElements.end(), 
				  DeleteData<ActionEleInfo*>() );
		pActorRes->vlpActionElements.clear();

		bResult = true;
	}

	return bResult;
}



bool CDnCutSceneData::SetRegResPos( const char* pResName, EtVector3& vPos )
{
	bool bResult = false;

	map<string, S_USE_RESOURCE*>::iterator iter = m_mapUseRes.find( string(pResName) );
	if( m_mapUseRes.end() != iter )
	{
		S_USE_RESOURCE* pRes = iter->second;
		if( RT_RES_ACTOR == pRes->iResourceKind )
		{
			static_cast<S_ACTOR_RESOURCE*>(pRes)->vPos = vPos;
			bResult = true;
		}
	}


	return bResult;
}


bool CDnCutSceneData::SetRegResRot( const char* pResName, float fRot )
{
	bool bResult = false;

	map<string, S_USE_RESOURCE*>::iterator iter = m_mapUseRes.find( string(pResName) );
	if( m_mapUseRes.end() != iter )
	{
		S_USE_RESOURCE* pRes = iter->second;
		if( RT_RES_ACTOR == pRes->iResourceKind )
		{
			static_cast<S_ACTOR_RESOURCE*>(pRes)->fRotation = fRot;
			bResult = true;
		}
	}


	return bResult;
}





int CDnCutSceneData::GetNumRegResource( void ) const
{
	return (int)m_vlUseRes.size();
}


const char* CDnCutSceneData::GetRegResNameByIndex( int iIndex ) const
{
	const char* pResult = NULL;
	
	if( iIndex < (int)m_vlUseRes.size() )
		pResult = m_vlUseRes.at( iIndex )->strResName.c_str();
	
	return pResult;
}


int CDnCutSceneData::GetRegResKindByIndex( int iIndex ) const
{
	int iResult = -1;

	if( iIndex < (int)m_vlUseRes.size() )
		iResult = m_vlUseRes.at( iIndex )->iResourceKind;

	return iResult;
}



EventInfo* CDnCutSceneData::_CreateEventInfo( int iEventType )
{
	EventInfo* pNewEvent = NULL;

	switch( iEventType )
	{
		case EventInfo::CAMERA:
			pNewEvent = new CamEventInfo;
			break;

		case EventInfo::PARTICLE:
			pNewEvent = new ParticleEventInfo;
			break;

		case EventInfo::DOF:
			pNewEvent = new DofEventInfo;
			break;

		//case EventInfo::SOUND:
		//	pNewEvent = new SoundEventInfo;
		//	break;

		case EventInfo::FADE:
			pNewEvent = new FadeEventInfo;
			break;

		case EventInfo::PROP:
			pNewEvent = new PropEventInfo;
			break;

		case EventInfo::SOUND_1:
		case EventInfo::SOUND_2:
		case EventInfo::SOUND_3:
		case EventInfo::SOUND_4:
		case EventInfo::SOUND_5:
		case EventInfo::SOUND_6:
		case EventInfo::SOUND_7:
		case EventInfo::SOUND_8:
			pNewEvent = new SoundEventInfo;
			break;

		case EventInfo::SUBTITLE:
			pNewEvent = new SubtitleEventInfo;
			break;


//#ifdef PRE_ADD_FILTEREVENT
		case EventInfo::COLORFILTER:
			pNewEvent = new ColorFilterEventInfo;
			break;
//#endif // PRE_ADD_FILTEREVENT
	}

	return pNewEvent;
}



bool CDnCutSceneData::InsertEvent( const EventInfo* pEventInfo )
{
	bool bResult = false;
	
	map<int, EventInfo*>::iterator iterIDTable = m_mapEventIDTable.find( pEventInfo->iID );
	if( iterIDTable == m_mapEventIDTable.end() )
	{
		if( pEventInfo->iType < EventInfo::TYPE_COUNT )
		{
			vector<EventInfo*>& vlEventSequence = m_avlEventSequenceDB[ pEventInfo->iType ];
			EventInfo* pNewEvent = pEventInfo->clone();
			//EventInfo* pNewEvent = _CreateEventInfo( pEventInfo->iType );

			//pNewEvent->copy_from( pEventInfo );

			vlEventSequence.push_back( pNewEvent );
			m_mapEventIDTable.insert( make_pair(pNewEvent->iID, pNewEvent) );

			_UpdateOrderEventSequence( pNewEvent->iType );

			bResult = true;
		}
	}

	return bResult;
}




bool CDnCutSceneData::ModifyEventInfo( const EventInfo* pEventInfo )
{
	bool bResult = false;

	map<int, EventInfo*>::iterator iterIDTable = m_mapEventIDTable.find( pEventInfo->iID );
	if( iterIDTable != m_mapEventIDTable.end() )
	{
		bool bChangeOrderPossiblity = false;
		EventInfo* pOriEventInfo = iterIDTable->second;
		
		if( pOriEventInfo->fStartTime !=  pEventInfo->fStartTime ||
			pOriEventInfo->fTimeLength != pEventInfo->fTimeLength )
			bChangeOrderPossiblity = true;

		pOriEventInfo->copy_from( pEventInfo );

		if( bChangeOrderPossiblity )
			_UpdateOrderEventSequence( iterIDTable->second->iType );

		bResult = true;
	}

	return bResult;
}




bool CDnCutSceneData::RemoveEvent( int iID )
{
	bool bResult = false;

	map<int, EventInfo*>::iterator iterIDTable = m_mapEventIDTable.find( iID );
	if( iterIDTable != m_mapEventIDTable.end() )
	{
		EventInfo* pNewEvent = iterIDTable->second;

		vector<EventInfo*>::iterator iterEventSequence = find( m_avlEventSequenceDB[ pNewEvent->iType ].begin(), 
															   m_avlEventSequenceDB[ pNewEvent->iType ].end(), iterIDTable->second );
		m_avlEventSequenceDB[ pNewEvent->iType ].erase( iterEventSequence );
		m_mapEventIDTable.erase( iterIDTable );

		delete pNewEvent;

		bResult = true;
	}

	return bResult;
}



const EventInfo* CDnCutSceneData::GetEventInfoByID( int iID )
{
	EventInfo* pResult = NULL;

	map<int, EventInfo*>::iterator iterIDTable = m_mapEventIDTable.find( iID );
	if( m_mapEventIDTable.end() != iterIDTable )
	{
		pResult = iterIDTable->second;
	}

	return pResult;
}


int CDnCutSceneData::GetThisTypesEventNum( int iEventType )
{
	int iResult = 0;

	if( iEventType < EventInfo::TYPE_COUNT )
		iResult = (int)m_avlEventSequenceDB[ iEventType ].size();

	return iResult;
}


const EventInfo* CDnCutSceneData::GetEventInfoByIndex( int iEventType, int iIndex )
{
	EventInfo* pResult = NULL;

	if( iEventType < EventInfo::TYPE_COUNT && 
		iIndex < (int)m_avlEventSequenceDB[ iEventType ].size() )
		pResult = m_avlEventSequenceDB[ iEventType ].at( iIndex );

	return pResult;
}



void CDnCutSceneData::Clear( void )
{
	map<string, vector<ActionInfo*> >::iterator iterASDB = m_mapActionSequenceDB.begin();
	for( iterASDB; iterASDB != m_mapActionSequenceDB.end(); ++iterASDB )
	{
		vector<ActionInfo*>& vlActionSequence = iterASDB->second;
		for_each( vlActionSequence.begin(), vlActionSequence.end(), DeleteData<ActionInfo*>() );
	}
	m_mapActionSequenceDB.clear();
	m_mapActionIDTable.clear();

	map<string, vector<KeyInfo*> >::iterator iterKSDB = m_mapKeySequenceDB.begin();
	for( iterKSDB; iterKSDB != m_mapKeySequenceDB.end(); ++iterKSDB )
	{
		vector<KeyInfo*>& vlKeySequence = iterKSDB->second;
		for_each( vlKeySequence.begin(), vlKeySequence.end(), DeleteData<KeyInfo*>() );
	}
	m_mapKeySequenceDB.clear();
	m_mapKeyIDTable.clear();

	for( int iEventType = 0; iEventType < EventInfo::TYPE_COUNT; ++iEventType )
	{
		vector<EventInfo*>& vlEventSequence = m_avlEventSequenceDB[ iEventType ];

		// 엔진이 Finalize 될 때 알아서 해제 됨
		switch( iEventType )
		{
			case EventInfo::PARTICLE:
				{
					int iNumEvent = (int)vlEventSequence.size();
					for( int iEvent = 0; iEvent < iNumEvent; ++iEvent )
					{
						EventInfo* pEventInfo = vlEventSequence.at( iEvent );
						if( -1 != static_cast<ParticleEventInfo*>(pEventInfo)->iParticleDataIndex )
							EternityEngine::DeleteParticleData( static_cast<ParticleEventInfo*>(pEventInfo)->iParticleDataIndex );
					}
				}
				break;

			case EventInfo::SOUND_1:
			case EventInfo::SOUND_2:
			case EventInfo::SOUND_3:
			case EventInfo::SOUND_4:
			case EventInfo::SOUND_5:
			case EventInfo::SOUND_6:
			case EventInfo::SOUND_7:
			case EventInfo::SOUND_8:
				{
					int iNumEvent = (int)vlEventSequence.size();
					for( int iEvent = 0; iEvent < iNumEvent; ++iEvent )
					{
						EventInfo* pEventInfo = vlEventSequence.at( iEvent );
						if( -1 != static_cast<SoundEventInfo*>(pEventInfo)->iSoundDataIndex )
							CEtSoundEngine::GetInstance().RemoveSound( static_cast<SoundEventInfo*>(pEventInfo)->iSoundDataIndex );
					}
				}
				break;
		}

		for_each( vlEventSequence.begin(), vlEventSequence.end(), DeleteData<EventInfo*>() );
		vlEventSequence.clear();
	}
	m_mapEventIDTable.clear();

	for_each( m_vlUseRes.begin(), m_vlUseRes.end(), DeleteData<S_USE_RESOURCE*>() );
	m_vlUseRes.clear();
	m_mapUseRes.clear();
	m_vlActors.clear();

	m_dwNowActionIDOffset = 0;
}




void CDnCutSceneData::_UpdateID( DWORD dwID )
{
	if( m_dwNowActionIDOffset <= dwID )
		m_dwNowActionIDOffset = dwID+1;
}




void CDnCutSceneData::_LoadFromLuaTable( lua_State* pLua, int iDataTableIndex )
{
	// 데이터는 모두 클리어
	Clear();

	lua_pushnil( pLua );
	while( lua_next(pLua, iDataTableIndex) )
	{
		if( lua_istable( pLua, -1 ) )
		{
			// 키 값이 무엇인가.
			const char* pTableKey = lua_tostring( pLua, -2 );
			if( strcmp( pTableKey, CS_RES_SET_TABLE_KEY) == 0 )
			{
				int iResSetTableIndex = lua_gettop( pLua );
				lua_pushnil( pLua );

				while( lua_next( pLua, iResSetTableIndex ) )
				{
					//const char* pResName = lua_tostring( pLua, -2 );
					assert( lua_istable( pLua, -1 ) );

					int iResTableIndex = lua_gettop( pLua );

					int iResourceKind = (int)LUA_DELEGATE.GetField( CS_TYPE_KEY );
					string strResName;
					LUA_DELEGATE.GetField( CS_NAME_KEY, strResName );
					bool bRegResSuccess = RegisterResource( strResName.c_str(), iResourceKind );
					assert( bRegResSuccess );

					switch( iResourceKind )
					{
						case RT_RES_MAP:
							{
								m_SceneInfo.fSceneStartFadeOutTime = (float)LUA_DELEGATE.GetField( CS_SCENE_START_FADEOUT_TIME );
								m_SceneInfo.fSceneEndFadeInTime = (float)LUA_DELEGATE.GetField( CS_SCENE_END_FADEIN_TIME );
								m_SceneInfo.bUseSceneEndClippingTime = ((int)LUA_DELEGATE.GetField( CS_USE_SCENE_END_CLIPPING ) == 1);
								m_SceneInfo.fSceneEndClipTime = (float)LUA_DELEGATE.GetField( CS_SCENE_END_CLIPPING_TIME );
								m_SceneInfo.bUseSignalSound = ((int)LUA_DELEGATE.GetField( CS_SCENE_USE_SIGNAL_SOUND ) == 1 );
								LUA_DELEGATE.GetField( CS_SCENE_MAIN_CHAR_NAME, m_SceneInfo.strMainCharacterName );
								m_SceneInfo.fFogFarDelta = (float)LUA_DELEGATE.GetField( CS_SCENE_FOGFAR_DELTA );
								m_SceneInfo.bAcademicNotSupported = ((int)LUA_DELEGATE.GetField( CS_SCENE_ACADEMIC_NOT_SUPPORTED ) == 1);
							}
							break;

						case RT_RES_ACTOR:
							{
								EtVector3 vPos;
								vPos.x = (float)LUA_DELEGATE.GetField( "X" );
								vPos.y = (float)LUA_DELEGATE.GetField( "Y" );
								vPos.z = (float)LUA_DELEGATE.GetField( "Z" );
								SetRegResPos( strResName.c_str(), vPos );

								float fRotation = (float)LUA_DELEGATE.GetField( CS_ROT_KEY );
								SetRegResRot( strResName.c_str(), fRotation );

								bool bFitYPosToMap = ((int)LUA_DELEGATE.GetField( CS_YPOS_TO_MAP_KEY ) == 1);
								SetActorsFitYPosToMap( strResName.c_str(), bFitYPosToMap );

								// 필드가 없는 경우(이전 버전)엔 디폴트값으로 true 로 해서 로드한다.
								// 디폴트로 true 로 되어있어야 하는데 기존 파일들엔 이 필드가 없기 때문이다.
								// 자연스럽게 컨버팅이 되도록 여기서 유도한다.
								if( LUA_DELEGATE.IsExistField( CS_INFLUENCE_LIGHTMAP ) )
								{
									bool bInfluenceLightmap = ((int)LUA_DELEGATE.GetField( CS_INFLUENCE_LIGHTMAP ) == 1);
									SetActorsInfluenceLightmap( strResName.c_str(), bInfluenceLightmap );
								}
								else
									SetActorsInfluenceLightmap( strResName.c_str(), true );

								int iMonsterTableID = (int)LUA_DELEGATE.GetField( CS_MONSTER_TABLE_ID );
								SetMonsterTableID( strResName.c_str(), iMonsterTableID );

								// 마찬가지로 자연스럽게 컨버팅이 되도록 여기서 유도한다.
								if( LUA_DELEGATE.IsExistField( CS_LOCK_SCALE ) )
								{
									bool bLockScale = ((int)LUA_DELEGATE.GetField( CS_LOCK_SCALE ) == 1);
									SetActorsScaleLock( strResName.c_str(), bLockScale );
								}
								else
									SetActorsScaleLock( strResName.c_str(), true );

								//// 관련 애니메이션 element도 추가한다.
								//lua_pushstring( pLua, CS_ACTION_ELEMENT_SET_KEY );
								//lua_gettable( pLua, -2 );

								//bool bIsTable = lua_istable(pLua, -1);
								//assert( bIsTable );

								//if( bIsTable )
								//{
								//	int iAniElementSetTableIndex = lua_gettop( pLua );
								//	lua_pushnil( pLua );
								//	while( lua_next( pLua, iAniElementSetTableIndex ) )
								//	{
								//		ActionEleInfo Element;

								//		LUA_DELEGATE.GetField( CS_NAME_KEY, Element.szName );
								//		Element.dwLength = (DWORD)LUA_DELEGATE.GetField( CS_FRAME_KEY );
								//		Element.dwBlendFrame = (DWORD)LUA_DELEGATE.GetField( CS_BLEND_FRAME_KEY );
								//		Element.dwNextActionFrame = (DWORD)LUA_DELEGATE.GetField( CS_NEXT_ACTION_FRAME_KEY );
								//		LUA_DELEGATE.GetField( CS_NEXT_ACTION_NAME_KEY, Element.szNextActionName );
								//		LUA_DELEGATE.GetField( CS_LINK_ANI_NAME_KEY, Element.szLinkAniName );

								//		// 시그널은 따로 저장하지 않았음. 제외.

								//		AddActorsAnimation( strResName.c_str(), &Element );

								//		lua_pop( pLua, 1 );
								//	}

								//	// 중요! 테이블을 얻어왔다면 테이블까지 마무리 해야함.
								//	lua_pop( pLua, 1 );
								//}
							}
							break;

						//case RT_RES_MAX_CAMERA:
						//	break;

						case RT_RES_SOUND:
							break;
					}

					lua_pop( pLua, 1 );
				}
			}
			else
			if( strcmp( pTableKey, CS_ACT_SEQUENCE_SET_KEY ) == 0 )
			{
				int iActionSeqTableIndex = lua_gettop( pLua );
				lua_pushnil( pLua );

				while( lua_next( pLua, iActionSeqTableIndex ) )
				{
					ActionInfo Action;
					
					Action.iID = (int)LUA_DELEGATE.GetField( CS_ID_KEY );
					_UpdateID( Action.iID );
					Action.iActionType = (int)LUA_DELEGATE.GetField( CS_TYPE_KEY );
					LUA_DELEGATE.GetField( CS_ACTOR_NAME_KEY, Action.strActorName );
					LUA_DELEGATE.GetField( CS_NAME_KEY, Action.strActionName );
					LUA_DELEGATE.GetField( CS_ANI_NAME_KEY, Action.strAnimationName );
					Action.fStartTime = (float)LUA_DELEGATE.GetField( CS_START_TIME_KEY );
					Action.fTimeLength = (float)LUA_DELEGATE.GetField( CS_TIME_LENGTH_KEY );
					Action.bUseAniDistance = ((int)LUA_DELEGATE.GetField( CS_USE_ANI_DIST_KEY ) == 1);
					Action.fSpeed = (float)LUA_DELEGATE.GetField( CS_SPEED_KEY );
					Action.bUseStartRotation = ((int)LUA_DELEGATE.GetField( CS_USE_START_ROTATION ) == 1);
					Action.fStartRotation = (float)LUA_DELEGATE.GetField( CS_ROT_KEY );
					Action.bUseStartPosition = ((int)LUA_DELEGATE.GetField( CS_USE_START_POSITION ) == 1);
					Action.vStartPos.x = (float)LUA_DELEGATE.GetField( "X" );
					Action.vStartPos.y = (float)LUA_DELEGATE.GetField( "Y" );
					Action.vStartPos.z = (float)LUA_DELEGATE.GetField( "Z" );
					Action.fUnit = (float)LUA_DELEGATE.GetField( CS_UNIT_KEY );
					Action.fAnimationSpeed = (float)LUA_DELEGATE.GetField( CS_ANI_SPEED_KEY );
					Action.iAnimationIndex = (int)LUA_DELEGATE.GetField( CS_ANI_INDEX_KEY );
					Action.iNextActionFrame = (int)LUA_DELEGATE.GetField( CS_NEXT_ACTION_FRAME_KEY );
					Action.bFitAniDistanceYToMap = LUA_DELEGATE.GetField( CS_YPOS_TO_MAP_KEY ) == 1;

					InsertAction( &Action );

					lua_pop( pLua, 1 );
				}
			}
			else
			if( strcmp( pTableKey, CS_KEY_SEQUENCE_SET_KEY ) == 0 )
			{
				int iKeySeqTableIndex = lua_gettop( pLua );
				lua_pushnil( pLua );
				
				while( lua_next( pLua, iKeySeqTableIndex ) )
				{
					KeyInfo Key;

					Key.iID = (int)LUA_DELEGATE.GetField( CS_ID_KEY );
					_UpdateID( Key.iID );
					Key.iKeyType = (int)LUA_DELEGATE.GetField( CS_TYPE_KEY );
					LUA_DELEGATE.GetField( CS_ACTOR_NAME_KEY, Key.strActorName );
					LUA_DELEGATE.GetField( CS_NAME_KEY, Key.strKeyName );
					Key.fStartTime = (float)LUA_DELEGATE.GetField( CS_START_TIME_KEY );
					Key.fTimeLength = (float)LUA_DELEGATE.GetField( CS_TIME_LENGTH_KEY );
					Key.fSpeed = (float)LUA_DELEGATE.GetField( CS_SPEED_KEY );
					Key.fUnit = (float)LUA_DELEGATE.GetField( CS_UNIT_KEY );
					Key.vStartPos.x = (float)LUA_DELEGATE.GetField( CS_START_X );
					Key.vStartPos.y = (float)LUA_DELEGATE.GetField( CS_START_Y );
					Key.vStartPos.z = (float)LUA_DELEGATE.GetField( CS_START_Z );
					
					switch( Key.iKeyType )
					{
						case KeyInfo::MOVE:
							Key.bRotateFront = ((int)LUA_DELEGATE.GetField( CS_ROTATEFRONT_KEY ) == 1);
							Key.bFitYPosToMap = ((int)LUA_DELEGATE.GetField( CS_YPOS_TO_MAP_KEY ) == 1);
							Key.vDestPos.x = (float)LUA_DELEGATE.GetField( CS_DEST_X );
							Key.vDestPos.y = (float)LUA_DELEGATE.GetField( CS_DEST_Y );
							Key.vDestPos.z = (float)LUA_DELEGATE.GetField( CS_DEST_Z );
							break;

						case KeyInfo::MULTI_MOVE:
							{
								Key.bRotateFront = ((int)LUA_DELEGATE.GetField( CS_ROTATEFRONT_KEY ) == 1);
								Key.bFitYPosToMap = ((int)LUA_DELEGATE.GetField( CS_YPOS_TO_MAP_KEY ) == 1);

								lua_pushstring( pLua, CS_SUBKEYSET_TABLE_KEY );
								lua_gettable( pLua, -2 );

								bool bIsTable = lua_istable( pLua, -1 );
								assert( bIsTable );
								
								if( bIsTable )
								{
									int iMultiMoveTableIndex = lua_gettop( pLua );
									lua_pushnil( pLua );

									while( lua_next( pLua, iMultiMoveTableIndex ) )
									{
										SubKey SubKey;

										SubKey.iID = (int)LUA_DELEGATE.GetField( CS_ID_KEY );
										_UpdateID( SubKey.iID );
										SubKey.fTimeLength = (float)LUA_DELEGATE.GetField( CS_TIME_LENGTH_KEY );
										SubKey.vPos.x = (float)LUA_DELEGATE.GetField( CS_DEST_X );
										SubKey.vPos.y = (float)LUA_DELEGATE.GetField( CS_DEST_Y );
										SubKey.vPos.z = (float)LUA_DELEGATE.GetField( CS_DEST_Z );

										Key.vlMoveKeys.push_back( SubKey );

										lua_pop( pLua, 1 );
									}
								}

								// 중요! 테이블을 얻어왔다면 테이블까지 마무리 해야함.
								lua_pop( pLua, 1 );
							}
							break;

						case KeyInfo::ROTATION:
							Key.fRotDegree = (float)LUA_DELEGATE.GetField( CS_ROT_KEY );
							break;
					}
					
					InsertKey( &Key );

					lua_pop( pLua, 1 );
				}
			}
			else
			if( strcmp( pTableKey, CS_EVENT_SEQUENCE_SET_KEY ) == 0 )
			{
				int iEventSeqTableIndex = lua_gettop( pLua );
				lua_pushnil( pLua );

				while( lua_next( pLua, iEventSeqTableIndex ) != 0 )
				{
					EventInfo* pEventInfo = NULL;
			
					int iEventType = (int)LUA_DELEGATE.GetField( CS_TYPE_KEY );
					
					pEventInfo = _CreateEventInfo( iEventType );
					
					pEventInfo->iType = iEventType;
					LUA_DELEGATE.GetField( CS_NAME_KEY, pEventInfo->strEventName );
					pEventInfo->iID = (int)LUA_DELEGATE.GetField( CS_ID_KEY );
					_UpdateID( pEventInfo->iID );
					pEventInfo->fStartTime = (float)LUA_DELEGATE.GetField( CS_START_TIME_KEY );
					pEventInfo->fTimeLength = (float)LUA_DELEGATE.GetField( CS_TIME_LENGTH_KEY );
					pEventInfo->fSpeed = (float)LUA_DELEGATE.GetField( CS_SPEED_KEY );
					pEventInfo->fUnit = (float)LUA_DELEGATE.GetField( CS_UNIT_KEY );

					switch( pEventInfo->iType )
					{
						case EventInfo::CAMERA:
							{
								CamEventInfo* pCamEventInfo = static_cast<CamEventInfo*>(pEventInfo);
								CCameraData* pNewCamData = pCamEventInfo->pCameraData;

								LUA_DELEGATE.GetField( CS_FILEPATH_KEY, pCamEventInfo->strImportFilePath );
								//pCamEventInfo->iTraceType = (int)LUA_DELEGATE.GetField( CS_TRACE_TYPE_KEY );
								//if( CamEventInfo::TRACE_NONE != pCamEventInfo->iTraceType )
								//	LUA_DELEGATE.GetField( CS_ACTOR_TO_TRACE_KEY, pCamEventInfo->strActorToTrace );

								char caFileName[ MAX_PATH ];
								char caFileExt[ 32 ];
								_splitpath( pCamEventInfo->strImportFilePath.c_str(), NULL, NULL, caFileName, caFileExt );
								pCamEventInfo->strImportFileName.assign( caFileName );
								pCamEventInfo->strImportFileName.append( caFileExt );

								pCamEventInfo->vCamStartOffset.x = (float)LUA_DELEGATE.GetField( CS_START_X );
								pCamEventInfo->vCamStartOffset.y = (float)LUA_DELEGATE.GetField( CS_START_Y );
								pCamEventInfo->vCamStartOffset.z = (float)LUA_DELEGATE.GetField( CS_START_Z );
								
								// 카메라 데이터 읽어들임.
								lua_pushstring( pLua, CS_CAMERA_DATA_KEY );
								lua_gettable( pLua, -2 );

								bool bIsTable = lua_istable( pLua, -1 );
								assert( bIsTable );

								if( bIsTable )
								{
									//CCameraData* pNewCamData = new CCameraData;
									pNewCamData->m_Header.nVersion = (int)LUA_DELEGATE.GetField( CUTSCENEDATA_VERSION_KEY );
									
									string strBuf;
									LUA_DELEGATE.GetField( CS_NAME_KEY, strBuf );
									assert( (int)strBuf.size() < sizeof(pNewCamData->m_Header.szCameraName) );
									strcpy( pNewCamData->m_Header.szCameraName, strBuf.c_str() );
									
									LUA_DELEGATE.GetField( CS_CAMERA_HEADERSTRING_KEY, strBuf );
									assert( (int)strBuf.size() < sizeof(pNewCamData->m_Header.szHeaderString) );
									strcpy( pNewCamData->m_Header.szHeaderString, strBuf.c_str() );

									pNewCamData->m_Header.nFrame = (int)LUA_DELEGATE.GetField( CS_FRAME_KEY );
									pNewCamData->m_Header.nType = (int)LUA_DELEGATE.GetField( CS_TYPE_KEY );
									pNewCamData->m_Header.nFOVKeyCount = (int)LUA_DELEGATE.GetField( CS_FOV_COUNT_KEY );
									pNewCamData->m_Header.nPositionKeyCount = (int)LUA_DELEGATE.GetField( CS_POS_COUNT_KEY );
									pNewCamData->m_Header.nRotationKeyCount = (int)LUA_DELEGATE.GetField( CS_ROT_COUNT_KEY );
									//pNewCamData->m_Header.nTargetPositionCount = (int)LUA_DELEGATE.GetField( CS_LOOKAT_COUNT_KEY );
									pNewCamData->m_Header.fDefaultFOV = (float)LUA_DELEGATE.GetField( CS_DEFAULT_FOV_KEY );

									pNewCamData->m_Header.vDefaultPosition.x = (float)LUA_DELEGATE.GetField( CS_DEFAULT_POS_X );
									pNewCamData->m_Header.vDefaultPosition.y = (float)LUA_DELEGATE.GetField( CS_DEFAULT_POS_Y );
									pNewCamData->m_Header.vDefaultPosition.z = (float)LUA_DELEGATE.GetField( CS_DEFAULT_POS_Z );

									pNewCamData->m_Header.qDefaultRotation.x = (float)LUA_DELEGATE.GetField( CS_DEFAULT_ROT_X );
									pNewCamData->m_Header.qDefaultRotation.y = (float)LUA_DELEGATE.GetField( CS_DEFAULT_ROT_Y );
									pNewCamData->m_Header.qDefaultRotation.z = (float)LUA_DELEGATE.GetField( CS_DEFAULT_ROT_Z );
									pNewCamData->m_Header.qDefaultRotation.w = (float)LUA_DELEGATE.GetField( CS_DEFAULT_ROT_W );

									//pNewCamData->m_Header.vDefaultTargetPosition.x = (float)LUA_DELEGATE.GetField( CS_DEFAULT_LOOKAT_X );
									//pNewCamData->m_Header.vDefaultTargetPosition.y = (float)LUA_DELEGATE.GetField( CS_DEFAULT_LOOKAT_Y );
									//pNewCamData->m_Header.vDefaultTargetPosition.z = (float)LUA_DELEGATE.GetField( CS_DEFAULT_LOOKAT_Z );

									// FOV 키 로드
									if( pNewCamData->m_Header.nFOVKeyCount > 0 )
									{
										lua_pushstring( pLua, CS_FOV_SET_KEY );
										lua_gettable( pLua, -2 );

										bIsTable = lua_istable( pLua, -1 );
										assert( bIsTable );

										if( bIsTable )
										{
											int iFOVSetTableIndex = lua_gettop( pLua );
											lua_pushnil( pLua );

											while( lua_next(pLua, iFOVSetTableIndex) )
											{
												SCameraFOVKey NewFOVKey;
												pNewCamData->m_vecFOV.push_back( NewFOVKey );
												SCameraFOVKey& FOVKey = pNewCamData->m_vecFOV.back();

												FOVKey.nTime = (int)LUA_DELEGATE.GetField( CS_FRAME_KEY );
												FOVKey.fFOV = (float)LUA_DELEGATE.GetField( CS_FOV_KEY );

												lua_pop( pLua, 1 );
											}

											// 중요! 테이블을 얻어왔다면 테이블까지 마무리 해야함.
											lua_pop( pLua, 1 );
										}
									}

									// Position Key 로드
									if( pNewCamData->m_Header.nPositionKeyCount > 0 )
									{
										lua_pushstring( pLua, CS_POS_SET_KEY );
										lua_gettable( pLua, -2 );

										bIsTable = lua_istable( pLua, -1 );
										assert( bIsTable );

										if( bIsTable )
										{
											int iPosSetTableIndex = lua_gettop( pLua );
											lua_pushnil( pLua );

											while( lua_next(pLua, iPosSetTableIndex) )
											{
												SCameraPositionKey NewPosKey;
												pNewCamData->m_vecPosition.push_back( NewPosKey );
												SCameraPositionKey& PosKey = pNewCamData->m_vecPosition.back();

												PosKey.nTime = (int)LUA_DELEGATE.GetField( CS_FRAME_KEY );
												PosKey.vPosition.x = (float)LUA_DELEGATE.GetField( "x" );
												PosKey.vPosition.y = (float)LUA_DELEGATE.GetField( "y" );
												PosKey.vPosition.z = (float)LUA_DELEGATE.GetField( "z" );

												lua_pop( pLua, 1 );
											}

											// 중요! 테이블을 얻어왔다면 테이블까지 마무리 해야함.
											lua_pop( pLua, 1 );
										}
									}

									// Rotation Key 로드
									if( pNewCamData->m_Header.nRotationKeyCount > 0 )
									{
										lua_pushstring( pLua, CS_ROT_SET_KEY );
										lua_gettable( pLua, -2 );

										bIsTable = lua_istable( pLua, -1 );
										assert( bIsTable );

										if( bIsTable )
										{
											int iRotSetTableIndex = lua_gettop( pLua );
											lua_pushnil( pLua );

											while( lua_next(pLua, iRotSetTableIndex) )
											{
												SCameraRotationKey NewRotKey;
												pNewCamData->m_vecRotation.push_back( NewRotKey );
												SCameraRotationKey& RotKey = pNewCamData->m_vecRotation.back();

												RotKey.nTime = (int)LUA_DELEGATE.GetField( CS_FRAME_KEY );
												RotKey.qRotation.x = (float)LUA_DELEGATE.GetField( "x" );
												RotKey.qRotation.y = (float)LUA_DELEGATE.GetField( "y" );
												RotKey.qRotation.z = (float)LUA_DELEGATE.GetField( "z" );
												RotKey.qRotation.w = (float)LUA_DELEGATE.GetField( "w" );

												lua_pop( pLua, 1 );
											}

											// 중요! 테이블을 얻어왔다면 테이블까지 마무리 해야함.
											lua_pop( pLua, 1 );
										}
									}

									// 등장 액터들 로드
									lua_pushstring( pLua, CS_ACTORS_IN_CAMERA_KEY );
									lua_gettable( pLua, -2 );

									bIsTable = lua_istable( pLua, -1 );
									if( bIsTable )
									{
										int iActorsInCameraTable = lua_gettop( pLua );
										lua_pushnil( pLua );

										while( lua_next( pLua, iActorsInCameraTable ) )
										{
											const char* pActorName = lua_tostring( pLua, -2 );				// key
											bool bShow = (int)lua_tonumber( pLua, -1 ) ? true : false;		// value
											pCamEventInfo->mapActorsShowHide.insert( make_pair(pActorName, bShow) );

											lua_pop( pLua, 1 );
										}
									}
									lua_pop( pLua, 1 );

									//// Target Camera Type(1) 이면 타겟 키 로드
									//if( CT_TARGET_CAMERA == pNewCamData->m_Header.nType )
									//{
									//	if( pNewCamData->m_Header.nTargetPositionCount > 0 )
									//	{
									//		lua_pushstring( pLua, CS_LOOKAT_SET_KEY );
									//		lua_gettable( pLua, -2 );

									//		bIsTable = lua_istable( pLua, -1 );
									//		assert( bIsTable );

									//		if( bIsTable )
									//		{
									//			int iLookAtTableIndex = lua_gettop( pLua );
									//			lua_pushnil( pLua );

									//			while( lua_next(pLua, iLookAtTableIndex) )
									//			{
									//				SCameraPositionKey NewLookAtKey;
									//				pNewCamData->m_vecTargetPosition.push_back( NewLookAtKey );
									//				SCameraPositionKey& LookAtKey = pNewCamData->m_vecTargetPosition.back();

									//				LookAtKey.nTime = (int)LUA_DELEGATE.GetField( CS_FRAME_KEY );
									//				LookAtKey.vPosition.x = (float)LUA_DELEGATE.GetField( "x" );
									//				LookAtKey.vPosition.y = (float)LUA_DELEGATE.GetField( "y" );
									//				LookAtKey.vPosition.z = (float)LUA_DELEGATE.GetField( "z" );

									//				lua_pop( pLua, 1 );
									//			}

									//			// 중요! 테이블을 얻어왔다면 테이블까지 마무리 해야함.
									//			lua_pop( pLua, 1 );
									//		}
									//	}
									//}

									// 중요! 테이블을 얻어왔다면 테이블까지 마무리 해야함.
									lua_pop( pLua, 1 );
								}
							}
							break;

						case EventInfo::PARTICLE:
							{
								ParticleEventInfo* pParticleEventInfo = static_cast<ParticleEventInfo*>(pEventInfo);

								LUA_DELEGATE.GetField( CS_FILEPATH_KEY, pParticleEventInfo->strImportFilePath );
								pParticleEventInfo->vPos.x = (float)LUA_DELEGATE.GetField( CS_DEFAULT_POS_X );
								pParticleEventInfo->vPos.y = (float)LUA_DELEGATE.GetField( CS_DEFAULT_POS_Y );
								pParticleEventInfo->vPos.z = (float)LUA_DELEGATE.GetField( CS_DEFAULT_POS_Z );

								// 파티클 파일 이름을 셋팅.
								char caFileName[ MAX_PATH ];
								char caFileExt[ 32 ];
								_splitpath_s( pParticleEventInfo->strImportFilePath.c_str(),
											  NULL, 0, NULL, 0, caFileName, MAX_PATH, caFileExt, 32 );
								pParticleEventInfo->strImportFileName.assign( caFileName ).append( caFileExt );

								pParticleEventInfo->iParticleDataIndex = EternityEngine::LoadParticleData( pParticleEventInfo->strImportFileName.c_str() );
							}
							break;

						case EventInfo::DOF:
							{
								DofEventInfo* pDofEventInfo = static_cast<DofEventInfo*>(pEventInfo);

								pDofEventInfo->bEnable = LUA_DELEGATE.GetField( CS_DOF_ENABLE ) == 1 ? true : false;
								pDofEventInfo->bUseNowValueAsStart = LUA_DELEGATE.GetField( CS_DOF_USE_NOW_VALUE_AS_START ) == 1 ? true : false;
								
								pDofEventInfo->fNearStartFrom = (float)LUA_DELEGATE.GetField( CS_DOF_NEAR_START_FROM );
								pDofEventInfo->fNearEndFrom = (float)LUA_DELEGATE.GetField( CS_DOF_NEAR_END_FROM );
								pDofEventInfo->fFarStartFrom = (float)LUA_DELEGATE.GetField( CS_DOF_FAR_START_FROM );
								pDofEventInfo->fFarEndFrom = (float)LUA_DELEGATE.GetField( CS_DOF_FAR_END_FROM );
								pDofEventInfo->fFocusDistFrom = (float)LUA_DELEGATE.GetField( CS_DOF_FOCUS_DIST_FROM );
								pDofEventInfo->fNearBlurSizeFrom = (float)LUA_DELEGATE.GetField( CS_DOF_NEAR_BLUR_SIZE_FROM );
								pDofEventInfo->fFarBlurSizeFrom = (float)LUA_DELEGATE.GetField( CS_DOF_FAR_BLUR_SIZE_FROM );

								pDofEventInfo->fNearStartDest = (float)LUA_DELEGATE.GetField( CS_DOF_NEAR_START_DEST );
								pDofEventInfo->fNearEndDest = (float)LUA_DELEGATE.GetField( CS_DOF_NEAR_END_DEST );
								pDofEventInfo->fFarStartDest = (float)LUA_DELEGATE.GetField( CS_DOF_FAR_START_DEST );
								pDofEventInfo->fFarEndDest = (float)LUA_DELEGATE.GetField( CS_DOF_FAR_END_DEST );
								pDofEventInfo->fFocusDistDest = (float)LUA_DELEGATE.GetField( CS_DOF_FOCUS_DIST_DEST );
								pDofEventInfo->fNearBlurSizeDest = (float)LUA_DELEGATE.GetField( CS_DOF_NEAR_BLUR_SIZE_DEST );
								pDofEventInfo->fFarBlurSizeDest = (float)LUA_DELEGATE.GetField( CS_DOF_FAR_BLUR_SIZE_DEST );
							}
							break;

						case EventInfo::SUBTITLE:
							{
								SubtitleEventInfo* pSubtitleEventInfo = static_cast<SubtitleEventInfo*>(pEventInfo);

								pSubtitleEventInfo->iUIStringID = (int)LUA_DELEGATE.GetField( CS_SUBTITLE_UISTRING_ID );
								//pSubtitleEventInfo->dwColor = (DWORD)LUA_DELEGATE.GetField( CS_SUBTITLE_COLOR );
							}
							break;

						//case EventInfo::SOUND:
						//	{
						//		SoundEventInfo* pSoundEventInfo = static_cast<SoundEventInfo*>(pEventInfo);

						//		LUA_DELEGATE.GetField( CS_FILEPATH_KEY, pSoundEventInfo->strImportFilePath );
						//		pSoundEventInfo->vPos.x = (float)LUA_DELEGATE.GetField( CS_DEFAULT_POS_X );
						//		pSoundEventInfo->vPos.y = (float)LUA_DELEGATE.GetField( CS_DEFAULT_POS_Y );
						//		pSoundEventInfo->vPos.z = (float)LUA_DELEGATE.GetField( CS_DEFAULT_POS_Z );

						//		char caFileName[ MAX_PATH ];
						//		char caFileExt[ 32 ];
						//		_splitpath_s( pSoundEventInfo->strImportFilePath.c_str(),
						//					  NULL, 0, NULL, 0, caFileName, MAX_PATH, caFileExt, 32 );
						//		pSoundEventInfo->strImportFileName.assign( caFileName ).append( caFileExt );

						//		pSoundEventInfo->iSoundDataIndex = CEtSoundEngine::GetInstance().LoadSound( pSoundEventInfo->strImportFileName.c_str(), true, false );
						//	}
						//	break;

						case EventInfo::FADE:
							{
								FadeEventInfo* pFadeEventInfo = static_cast<FadeEventInfo*>(pEventInfo);

								pFadeEventInfo->iFadeKind = (int)LUA_DELEGATE.GetField( CS_FADE_KEY );
								pFadeEventInfo->iColorRed = (int)LUA_DELEGATE.GetField( CS_RED );
								pFadeEventInfo->iColorGreen = (int)LUA_DELEGATE.GetField( CS_GREEN );
								pFadeEventInfo->iColorBlue = (int)LUA_DELEGATE.GetField( CS_BLUE );
							}
							break;

						case EventInfo::PROP:
							{
								PropEventInfo* pPropEventInfo = static_cast<PropEventInfo*>(pEventInfo);
								LUA_DELEGATE.GetField( CS_PROP_NAME, pPropEventInfo->strActPropName );
								pPropEventInfo->iPropID = (int)LUA_DELEGATE.GetField( CS_PROP_ID );
								LUA_DELEGATE.GetField( CS_PROP_ACTION_NAME, pPropEventInfo->strActionName );
								pPropEventInfo->bShow = (LUA_DELEGATE.GetField( CS_PROP_SHOW ) == 1) ? true : false;
							}
							break;

						case EventInfo::SOUND_1:
						case EventInfo::SOUND_2:
						case EventInfo::SOUND_3:
						case EventInfo::SOUND_4:
						case EventInfo::SOUND_5:
						case EventInfo::SOUND_6:
						case EventInfo::SOUND_7:
						case EventInfo::SOUND_8:
							{
								SoundEventInfo* pSoundEventInfo = static_cast<SoundEventInfo*>(pEventInfo);

								LUA_DELEGATE.GetField( CS_FILEPATH_KEY, pSoundEventInfo->strImportFilePath );
								//pSoundEventInfo->vPos.x = (float)LUA_DELEGATE.GetField( CS_DEFAULT_POS_X );
								//pSoundEventInfo->vPos.y = (float)LUA_DELEGATE.GetField( CS_DEFAULT_POS_Y );
								//pSoundEventInfo->vPos.z = (float)LUA_DELEGATE.GetField( CS_DEFAULT_POS_Z );

								char caFileName[ MAX_PATH ];
								char caFileExt[ 32 ];
								_splitpath_s( pSoundEventInfo->strImportFilePath.c_str(),
											  NULL, 0, NULL, 0, caFileName, MAX_PATH, caFileExt, 32 );
								pSoundEventInfo->strImportFileName.assign( caFileName ).append( caFileExt );

								pSoundEventInfo->iSoundDataIndex = CEtSoundEngine::GetInstance().LoadSound( pSoundEventInfo->strImportFileName.c_str(), false, false );
								if( pSoundEventInfo->iSoundDataIndex != -1 )
								{
									pSoundEventInfo->fTimeLength = 0.0f;

									CEtSoundEngine::SoundStruct* pSoundStruct = CEtSoundEngine::GetInstance().FindSoundStruct( pSoundEventInfo->iSoundDataIndex );
									if( pSoundStruct )		// 인덱스가 -1 은 아니지만 구조체를 못찾는 경우가 있어서 여기서 한 번 더 체크.
									{
										FMOD::Sound* pSoundInfo = pSoundStruct->pSound;
										if( pSoundInfo )
										{
											unsigned int uiSoundLengthMS = 0;
											pSoundInfo->getLength( &uiSoundLengthMS, FMOD_TIMEUNIT_MS );
											pSoundEventInfo->fTimeLength = (float)uiSoundLengthMS / 1000.0f;
										}
									}
								}


								pSoundEventInfo->fVolume = (float)LUA_DELEGATE.GetField( CS_VOLUME );
							}
							break;

//#ifdef PRE_ADD_FILTEREVENT
						case EventInfo::COLORFILTER:
							{
								ColorFilterEventInfo* pColorFilterEventInfo = static_cast<ColorFilterEventInfo*>(pEventInfo);		
								pColorFilterEventInfo->bMonochrome = (bool)LUA_DELEGATE.GetField( CS_COLORFILTER_MONO );
								pColorFilterEventInfo->vColor.x = (float)LUA_DELEGATE.GetField( CS_COLORFILTER_R );
								pColorFilterEventInfo->vColor.y = (float)LUA_DELEGATE.GetField( CS_COLORFILTER_G );
								pColorFilterEventInfo->vColor.z = (float)LUA_DELEGATE.GetField( CS_COLORFILTER_B );
								pColorFilterEventInfo->fVolume = (float)LUA_DELEGATE.GetField( CS_COLORFILTER_VOL );
							}
							break;
//#endif PRE_ADD_FILTEREVENT

					}

					InsertEvent( pEventInfo );
					SAFE_DELETE( pEventInfo );

					lua_pop( pLua, 1 );
				}
			}
		}

		lua_pop( pLua, 1 );
	}
}





// 파일 저장 및 로드
bool CDnCutSceneData::LoadFromFile( const char* pFileName )
{
	bool bResult = false;

	// 루아 환경에서 기존에 있던 컷신 데이터 테이블을 초기화 시켜버린다.
	// 같은 이름의 테이블을 한 번 더 선언해버림
	lua_State* pLua = LUA_DELEGATE.GetLuaState();
	lua_newtable( pLua );
	lua_setglobal( pLua, CUTSCENEDATA_TABLE_NAME );

	if( LUA_DELEGATE.RunScript( pFileName ) )
	{
		lua_pushstring( pLua, CUTSCENEDATA_TABLE_NAME );
		lua_gettable( pLua, LUA_GLOBALSINDEX );

		if( lua_istable( pLua, -1 ) )
		{
			int iDataTableIndex = lua_gettop( pLua );

			float fFileVersion = (float)LUA_DELEGATE.GetField( CUTSCENEDATA_VERSION_KEY );
			if( CUTSCENEDATA_VERSION == fFileVersion )
			{
				_LoadFromLuaTable( pLua, iDataTableIndex );

				bResult = true;
			}
		}
	}

	return bResult;
}



bool CDnCutSceneData::LoadFromStringBuffer( const char* pStringBuffer )
{
	bool bResult = false;

	// 루아 환경에서 기존에 있던 컷신 데이터 테이블을 초기화 시켜버린다.
	// 같은 이름의 테이블을 한 번 더 선언해버림
	lua_State* pLua = LUA_DELEGATE.GetLuaState();
	lua_newtable( pLua );
	lua_setglobal( pLua, CUTSCENEDATA_TABLE_NAME );

	if( LUA_DELEGATE.RunString( pStringBuffer ) )
	{
		lua_pushstring( pLua, CUTSCENEDATA_TABLE_NAME );
		lua_gettable( pLua, LUA_GLOBALSINDEX );

		if( lua_istable( pLua, -1 ) )
		{
			int iDataTableIndex = lua_gettop( pLua );

			float fFileVersion = (float)LUA_DELEGATE.GetField( CUTSCENEDATA_VERSION_KEY );
			if( CUTSCENEDATA_VERSION == fFileVersion )
			{
				_LoadFromLuaTable( pLua, iDataTableIndex );

				bResult = true;
			}
		}
	}

	return bResult;
}




void CDnCutSceneData::_SaveToLuaTable( lua_State* pLua )
{
	// 사용하는 리소스 리스트 저장
	lua_newtable( pLua );

	int iNumUseRes = (int)m_vlUseRes.size();
	for( int iRes = 0; iRes < iNumUseRes; ++iRes )
	{
		const S_USE_RESOURCE* pRes = m_vlUseRes.at( iRes );

		lua_newtable( pLua );

		int iResTable = lua_gettop( pLua );

		LUA_DELEGATE.AddToTable( iResTable, CS_NAME_KEY, pRes->strResName.c_str() );
		LUA_DELEGATE.AddToTable( iResTable, CS_TYPE_KEY, pRes->iResourceKind );
		
		switch( pRes->iResourceKind )
		{
			case RT_RES_MAP:
				{
					LUA_DELEGATE.AddToTable( iResTable, CS_SCENE_START_FADEOUT_TIME, m_SceneInfo.fSceneStartFadeOutTime );
					LUA_DELEGATE.AddToTable( iResTable, CS_SCENE_END_FADEIN_TIME, m_SceneInfo.fSceneEndFadeInTime );
					LUA_DELEGATE.AddToTable( iResTable, CS_USE_SCENE_END_CLIPPING, m_SceneInfo.bUseSceneEndClippingTime ? 1 : 0 );

					if( m_SceneInfo.bUseSceneEndClippingTime )
						LUA_DELEGATE.AddToTable( iResTable, CS_SCENE_END_CLIPPING_TIME, m_SceneInfo.fSceneEndClipTime );
					else
						LUA_DELEGATE.AddToTable( iResTable, CS_SCENE_END_CLIPPING_TIME, 0.0f );

					LUA_DELEGATE.AddToTable( iResTable, CS_SCENE_USE_SIGNAL_SOUND, m_SceneInfo.bUseSignalSound ? 1 : 0 );
					LUA_DELEGATE.AddToTable( iResTable, CS_SCENE_MAIN_CHAR_NAME, m_SceneInfo.strMainCharacterName.c_str() );
					LUA_DELEGATE.AddToTable( iResTable, CS_SCENE_FOGFAR_DELTA, m_SceneInfo.fFogFarDelta );
					LUA_DELEGATE.AddToTable( iResTable, CS_SCENE_ACADEMIC_NOT_SUPPORTED, m_SceneInfo.bAcademicNotSupported ? 1 : 0 );
				}
				break;

			case RT_RES_ACTOR:
				{
					const S_ACTOR_RESOURCE* pActorRes = static_cast<const S_ACTOR_RESOURCE*>(pRes);
					LUA_DELEGATE.AddToTable( iResTable, "X", pActorRes->vPos.x );
					LUA_DELEGATE.AddToTable( iResTable, "Y", pActorRes->vPos.y );
					LUA_DELEGATE.AddToTable( iResTable, "Z", pActorRes->vPos.z );
					LUA_DELEGATE.AddToTable( iResTable, CS_ROT_KEY, pActorRes->fRotation );
					LUA_DELEGATE.AddToTable( iResTable, CS_YPOS_TO_MAP_KEY, pActorRes->bFitYPosToMap ? 1 : 0 );
					LUA_DELEGATE.AddToTable( iResTable, CS_INFLUENCE_LIGHTMAP, pActorRes->bInfluenceLightmap ? 1 : 0 );
					LUA_DELEGATE.AddToTable( iResTable, CS_MONSTER_TABLE_ID, pActorRes->iMonsterTableID );
					LUA_DELEGATE.AddToTable( iResTable, CS_LOCK_SCALE, pActorRes->bScaleLock ? 1 : 0 );

					//lua_newtable( pLua );

					//// 액션 엘레멘트 데이터 저장. 일단 필요한 것들만.
					//int iNumElement = (int)pActorRes->vlpActionElements.size();
					//for( int iElement = 0; iElement < iNumElement; ++iElement )
					//{
					//	const ActionEleInfo* pElement = pActorRes->vlpActionElements.at(iElement);
					//	
					//	lua_newtable( pLua );
					//	int iElementTable = lua_gettop( pLua );

					//	LUA_DELEGATE.AddToTable( iElementTable, CS_NAME_KEY, pElement->szName.c_str() );
					//	
					//	if( !pElement->szNextActionName.empty() )
					//		LUA_DELEGATE.AddToTable( iElementTable, CS_NEXT_ACTION_NAME_KEY, pElement->szNextActionName.c_str() );
					//	
					//	if( !pElement->szLinkAniName.empty() )
					//		LUA_DELEGATE.AddToTable( iElementTable, CS_LINK_ANI_NAME_KEY, pElement->szLinkAniName.c_str() );

					//	LUA_DELEGATE.AddToTable( iElementTable, CS_FRAME_KEY, pElement->dwLength );
					//	LUA_DELEGATE.AddToTable( iElementTable, CS_NEXT_ACTION_FRAME_KEY, pElement->dwNextActionFrame );
					//	LUA_DELEGATE.AddToTable( iElementTable, CS_BLEND_FRAME_KEY, pElement->dwBlendFrame );

					//	// 루아의 인덱스는 1부터임.
					//	lua_pushnumber( pLua, iElement+1 );
					//	lua_insert( pLua, -2 );
					//	lua_settable( pLua, -3 );
					//}

					//lua_pushstring( pLua, CS_ACTION_ELEMENT_SET_KEY );
					//lua_insert( pLua, -2 );
					//lua_settable( pLua, -3 );
				}
				break;
		}

		//lua_pushstring( pLua, pRes->strResName.c_str() );
		lua_pushnumber( pLua, iRes+1 ); 
		lua_insert( pLua, -2 );
		lua_settable( pLua, -3 );
	}

	// 리소스 테이블을 전체 테이블에 올림
	lua_pushstring( pLua, CS_RES_SET_TABLE_KEY );
	lua_insert( pLua, -2 );
	lua_settable( pLua, -3 );


	// 액션 시퀀스 저장
	lua_newtable( pLua );

	int iActionCount = 1;
	map<int, ActionInfo*>::iterator iterAction = m_mapActionIDTable.begin();
	for( iterAction; iterAction != m_mapActionIDTable.end(); ++iterAction )
	{
		const ActionInfo* pAction = iterAction->second;

		lua_newtable( pLua );
		
		int iActionTable = lua_gettop( pLua );

		LUA_DELEGATE.AddToTable( iActionTable, CS_NAME_KEY, pAction->strActionName.c_str() );
		LUA_DELEGATE.AddToTable( iActionTable, CS_ACTOR_NAME_KEY, pAction->strActorName.c_str() );
		LUA_DELEGATE.AddToTable( iActionTable, CS_ID_KEY, pAction->iID ); 
		LUA_DELEGATE.AddToTable( iActionTable, CS_START_TIME_KEY, pAction->fStartTime );
		LUA_DELEGATE.AddToTable( iActionTable, CS_TIME_LENGTH_KEY, pAction->fTimeLength );
		LUA_DELEGATE.AddToTable( iActionTable, CS_UNIT_KEY, pAction->fUnit );
		LUA_DELEGATE.AddToTable( iActionTable, CS_SPEED_KEY, pAction->fSpeed );
		LUA_DELEGATE.AddToTable( iActionTable, CS_USE_START_ROTATION, pAction->bUseStartRotation ? 1 : 0 );
		LUA_DELEGATE.AddToTable( iActionTable, CS_ROT_KEY, pAction->fStartRotation );
		LUA_DELEGATE.AddToTable( iActionTable, CS_USE_START_POSITION, pAction->bUseStartPosition ? 1 : 0 );
		LUA_DELEGATE.AddToTable( iActionTable, "X", pAction->vStartPos.x );
		LUA_DELEGATE.AddToTable( iActionTable, "Y", pAction->vStartPos.y );
		LUA_DELEGATE.AddToTable( iActionTable, "Z", pAction->vStartPos.z );
		LUA_DELEGATE.AddToTable( iActionTable, CS_USE_ANI_DIST_KEY, pAction->bUseAniDistance ? 1 : 0 );
		LUA_DELEGATE.AddToTable( iActionTable, CS_TYPE_KEY, pAction->iActionType );
		LUA_DELEGATE.AddToTable( iActionTable, CS_ANI_INDEX_KEY, pAction->iAnimationIndex );
		LUA_DELEGATE.AddToTable( iActionTable, CS_ANI_NAME_KEY, pAction->strAnimationName.c_str() );
		LUA_DELEGATE.AddToTable( iActionTable, CS_ANI_SPEED_KEY, pAction->fAnimationSpeed );
		LUA_DELEGATE.AddToTable( iActionTable, CS_ANI_NEXT_FRAME_KEY, pAction->iNextActionFrame );
		LUA_DELEGATE.AddToTable( iActionTable, CS_YPOS_TO_MAP_KEY, pAction->bFitAniDistanceYToMap );
		
		//lua_pushstring( pLua, pAction->strActionName.c_str() );
		// 루아의 인덱스는 1부터..
		lua_pushnumber( pLua, iActionCount );
		lua_insert( pLua, -2 );
		lua_settable( pLua, -3 );

		++iActionCount;
	}
	
	lua_pushstring( pLua, CS_ACT_SEQUENCE_SET_KEY );
	lua_insert( pLua, -2 );
	lua_settable( pLua, -3 );


	// 키 시퀀스 저장
	lua_newtable( pLua );

	int iKeyCount = 1;
	map<int, KeyInfo*>::iterator iterKey = m_mapKeyIDTable.begin();
	for( iterKey; iterKey != m_mapKeyIDTable.end(); ++iterKey )
	{
		const KeyInfo* pKey = iterKey->second;

		lua_newtable( pLua );

		int iKeyTable = lua_gettop( pLua );

		LUA_DELEGATE.AddToTable( iKeyTable, CS_NAME_KEY, pKey->strKeyName.c_str() );
		LUA_DELEGATE.AddToTable( iKeyTable, CS_ACTOR_NAME_KEY, pKey->strActorName.c_str() );
		LUA_DELEGATE.AddToTable( iKeyTable, CS_ID_KEY, pKey->iID );
		LUA_DELEGATE.AddToTable( iKeyTable, CS_START_TIME_KEY, pKey->fStartTime );
		LUA_DELEGATE.AddToTable( iKeyTable, CS_TIME_LENGTH_KEY, pKey->fTimeLength );
		LUA_DELEGATE.AddToTable( iKeyTable, CS_UNIT_KEY, pKey->fUnit );
		LUA_DELEGATE.AddToTable( iKeyTable, CS_SPEED_KEY, pKey->fSpeed );
		LUA_DELEGATE.AddToTable( iKeyTable, CS_TYPE_KEY, pKey->iKeyType );
		LUA_DELEGATE.AddToTable( iKeyTable, CS_START_X, pKey->vStartPos.x );
		LUA_DELEGATE.AddToTable( iKeyTable, CS_START_Y, pKey->vStartPos.y );
		LUA_DELEGATE.AddToTable( iKeyTable, CS_START_Z, pKey->vStartPos.z );

		switch( pKey->iKeyType )
		{
			case KeyInfo::MOVE:
				{
					LUA_DELEGATE.AddToTable( iKeyTable, CS_ROTATEFRONT_KEY, pKey->bRotateFront ? 1 : 0 );
					LUA_DELEGATE.AddToTable( iKeyTable, CS_YPOS_TO_MAP_KEY, pKey->bFitYPosToMap ? 1 : 0 );
					LUA_DELEGATE.AddToTable( iKeyTable, CS_DEST_X, pKey->vDestPos.x );
					LUA_DELEGATE.AddToTable( iKeyTable, CS_DEST_Y, pKey->vDestPos.y );
					LUA_DELEGATE.AddToTable( iKeyTable, CS_DEST_Z, pKey->vDestPos.z );
				}
				break;

			case KeyInfo::MULTI_MOVE:
				{
					LUA_DELEGATE.AddToTable( iKeyTable, CS_ROTATEFRONT_KEY, pKey->bRotateFront ? 1 : 0 );
					LUA_DELEGATE.AddToTable( iKeyTable, CS_YPOS_TO_MAP_KEY, pKey->bFitYPosToMap ? 1 : 0 );

					// 서브 키셋트 저장
					lua_newtable( pLua );
				
					int iNumSubKey = (int)pKey->vlMoveKeys.size();
					for( int iSubKey = 0; iSubKey < iNumSubKey; ++iSubKey )
					{
						const S_KEY& SubKey = pKey->vlMoveKeys.at( iSubKey );

						lua_newtable( pLua );
						int iSubKeyTable = lua_gettop( pLua );
						
						LUA_DELEGATE.AddToTable( iSubKeyTable, CS_ID_KEY, SubKey.iID );
						LUA_DELEGATE.AddToTable( iSubKeyTable, CS_TIME_LENGTH_KEY, SubKey.fTimeLength );
						LUA_DELEGATE.AddToTable( iSubKeyTable, CS_DEST_X, SubKey.vPos.x );
						LUA_DELEGATE.AddToTable( iSubKeyTable, CS_DEST_Y, SubKey.vPos.y );
						LUA_DELEGATE.AddToTable( iSubKeyTable, CS_DEST_Z, SubKey.vPos.z );
						
						lua_pushnumber( pLua, iSubKey+1 );
						lua_insert( pLua, -2 );
						lua_settable( pLua, -3 );
					}

					lua_pushstring( pLua, CS_SUBKEYSET_TABLE_KEY );
					lua_insert( pLua, -2 );
					lua_settable( pLua, -3 );
				}
				break;

			case KeyInfo::ROTATION:
				LUA_DELEGATE.AddToTable( iKeyTable, CS_ROT_KEY, pKey->fRotDegree );
				break;
		}

		//lua_pushstring( pLua, pKey->strKeyName.c_str() );
		lua_pushnumber( pLua, iKeyCount );
		lua_insert( pLua, -2 );
		lua_settable( pLua, -3 );

		++iKeyCount;
	}

	lua_pushstring( pLua, CS_KEY_SEQUENCE_SET_KEY );
	lua_insert( pLua, -2 );
	lua_settable( pLua, -3 );


	// 이벤트 시퀀스 저장
	lua_newtable( pLua );

	int iEventCount = 1;
	map<int, EventInfo*>::iterator iterEvent = m_mapEventIDTable.begin();
	for( iterEvent; iterEvent != m_mapEventIDTable.end(); ++iterEvent )
	{
		const S_EVENT_INFO* pEventInfo = iterEvent->second;

		lua_newtable( pLua );
		int iEventTableIndex = lua_gettop( pLua );

		LUA_DELEGATE.AddToTable( iEventTableIndex, CS_NAME_KEY, pEventInfo->strEventName.c_str() );
		//LUA_DELEGATE.AddToTable( iActionTable, CS_ACTOR_NAME_KEY, pAction->strActorName.c_str() );
		LUA_DELEGATE.AddToTable( iEventTableIndex, CS_ID_KEY, pEventInfo->iID ); 
		LUA_DELEGATE.AddToTable( iEventTableIndex, CS_START_TIME_KEY, pEventInfo->fStartTime/* / 100.0f*/ );
		LUA_DELEGATE.AddToTable( iEventTableIndex, CS_TIME_LENGTH_KEY, pEventInfo->fTimeLength/* / 100.0f*/ );
		LUA_DELEGATE.AddToTable( iEventTableIndex, CS_UNIT_KEY, pEventInfo->fUnit );
		LUA_DELEGATE.AddToTable( iEventTableIndex, CS_SPEED_KEY, pEventInfo->fSpeed );
		LUA_DELEGATE.AddToTable( iEventTableIndex, CS_TYPE_KEY, pEventInfo->iType );

		switch( pEventInfo->iType )
		{
			case EventInfo::CAMERA:
				{
					const CamEventInfo* pCamEventInfo = static_cast<const CamEventInfo*>(pEventInfo);
					LUA_DELEGATE.AddToTable( iEventTableIndex, CS_FILEPATH_KEY, pCamEventInfo->strImportFilePath.c_str() );
					//LUA_DELEGATE.AddToTable( iEventTableIndex, CS_TRACE_TYPE_KEY, pCamEventInfo->iTraceType );
					LUA_DELEGATE.AddToTable( iEventTableIndex, CS_START_X, pCamEventInfo->vCamStartOffset.x );
					LUA_DELEGATE.AddToTable( iEventTableIndex, CS_START_Y, pCamEventInfo->vCamStartOffset.y );
					LUA_DELEGATE.AddToTable( iEventTableIndex, CS_START_Z, pCamEventInfo->vCamStartOffset.z );
					
					//if( CamEventInfo::TRACE_NONE != pCamEventInfo->iTraceType &&
					//	false == pCamEventInfo->strActorToTrace.empty() )
					//	LUA_DELEGATE.AddToTable( iEventTableIndex, CS_ACTOR_TO_TRACE_KEY, pCamEventInfo->strActorToTrace.c_str() );

					// 카메라 데이터 저장!
					const CCameraData* pCameraData = pCamEventInfo->pCameraData;

					lua_newtable( pLua );
					int iCameraDataTable = lua_gettop( pLua );
					LUA_DELEGATE.AddToTable( iCameraDataTable, CUTSCENEDATA_VERSION_KEY, pCameraData->m_Header.nVersion );
					LUA_DELEGATE.AddToTable( iCameraDataTable, CS_NAME_KEY, pCameraData->m_Header.szCameraName );
					LUA_DELEGATE.AddToTable( iCameraDataTable, CS_CAMERA_HEADERSTRING_KEY, pCameraData->m_Header.szHeaderString );
					LUA_DELEGATE.AddToTable( iCameraDataTable, CS_FRAME_KEY, pCameraData->m_Header.nFrame );
					LUA_DELEGATE.AddToTable( iCameraDataTable, CS_TYPE_KEY, pCameraData->m_Header.nType );
					LUA_DELEGATE.AddToTable( iCameraDataTable, CS_FOV_COUNT_KEY, pCameraData->m_Header.nFOVKeyCount );
					LUA_DELEGATE.AddToTable( iCameraDataTable, CS_POS_COUNT_KEY, pCameraData->m_Header.nPositionKeyCount );
					LUA_DELEGATE.AddToTable( iCameraDataTable, CS_ROT_COUNT_KEY, pCameraData->m_Header.nRotationKeyCount );
					//LUA_DELEGATE.AddToTable( iCameraDataTable, CS_LOOKAT_COUNT_KEY, pCameraData->m_Header.nTargetPositionCount );
					LUA_DELEGATE.AddToTable( iCameraDataTable, CS_DEFAULT_FOV_KEY, pCameraData->m_Header.fDefaultFOV );
					
					LUA_DELEGATE.AddToTable( iCameraDataTable, CS_DEFAULT_POS_X, pCameraData->m_Header.vDefaultPosition.x );
					LUA_DELEGATE.AddToTable( iCameraDataTable, CS_DEFAULT_POS_Y, pCameraData->m_Header.vDefaultPosition.y );
					LUA_DELEGATE.AddToTable( iCameraDataTable, CS_DEFAULT_POS_Z, pCameraData->m_Header.vDefaultPosition.z );
					
					LUA_DELEGATE.AddToTable( iCameraDataTable, CS_DEFAULT_ROT_X, pCameraData->m_Header.qDefaultRotation.x );
					LUA_DELEGATE.AddToTable( iCameraDataTable, CS_DEFAULT_ROT_Y, pCameraData->m_Header.qDefaultRotation.y );
					LUA_DELEGATE.AddToTable( iCameraDataTable, CS_DEFAULT_ROT_Z, pCameraData->m_Header.qDefaultRotation.z );
					LUA_DELEGATE.AddToTable( iCameraDataTable, CS_DEFAULT_ROT_W, pCameraData->m_Header.qDefaultRotation.w );

					//LUA_DELEGATE.AddToTable( iCameraDataTable, CS_DEFAULT_LOOKAT_X, pCameraData->m_Header.vDefaultTargetPosition.x );
					//LUA_DELEGATE.AddToTable( iCameraDataTable, CS_DEFAULT_LOOKAT_Y,pCameraData->m_Header.vDefaultTargetPosition.y );
					//LUA_DELEGATE.AddToTable( iCameraDataTable, CS_DEFAULT_LOOKAT_Z,pCameraData->m_Header.vDefaultTargetPosition.z );
					
					// FOV 키 저장
					if( pCameraData->m_Header.nFOVKeyCount > 0 )
					{
						lua_newtable( pLua );
						for( int iFOVKey = 0; iFOVKey < pCameraData->m_Header.nFOVKeyCount; ++iFOVKey )
						{
							const SCameraFOVKey& CamFOVKey = pCameraData->m_vecFOV.at( iFOVKey );
							lua_newtable( pLua );
							int iCamFOVTableIndex = lua_gettop( pLua );
							
							LUA_DELEGATE.AddToTable( iCamFOVTableIndex, CS_FRAME_KEY, CamFOVKey.nTime );
							LUA_DELEGATE.AddToTable( iCamFOVTableIndex, CS_FOV_KEY, CamFOVKey.fFOV );
							
							lua_pushnumber( pLua, iFOVKey+1 );
							lua_insert( pLua, -2 );
							lua_settable( pLua, -3 );
						}
						lua_pushstring( pLua, CS_FOV_SET_KEY );
						lua_insert( pLua, -2 );
						lua_settable( pLua, -3 );
					}

					// 포지션 키 저장
					if( pCameraData->m_Header.nPositionKeyCount > 0 )
					{
						lua_newtable( pLua );
						for( int iPosKey = 0; iPosKey < pCameraData->m_Header.nPositionKeyCount; ++iPosKey )
						{
							const SCameraPositionKey& CamPosKey = pCameraData->m_vecPosition.at( iPosKey );
							lua_newtable( pLua );
							int iCamPosTableIndex = lua_gettop( pLua );

							LUA_DELEGATE.AddToTable( iCamPosTableIndex, CS_FRAME_KEY, CamPosKey.nTime );
							LUA_DELEGATE.AddToTable( iCamPosTableIndex, "x", CamPosKey.vPosition.x );
							LUA_DELEGATE.AddToTable( iCamPosTableIndex, "y", CamPosKey.vPosition.y );
							LUA_DELEGATE.AddToTable( iCamPosTableIndex, "z", CamPosKey.vPosition.z );
							
							lua_pushnumber( pLua, iPosKey+1 );
							lua_insert( pLua, -2 );
							lua_settable( pLua, -3 );
						}
						lua_pushstring( pLua, CS_POS_SET_KEY );
						lua_insert( pLua, -2 );
						lua_settable( pLua, -3 );
					}

					// 회전 키 저장
					if( pCameraData->m_Header.nRotationKeyCount > 0 )
					{
						lua_newtable( pLua );
						for( int iRotKey = 0; iRotKey < pCameraData->m_Header.nRotationKeyCount; ++iRotKey )
						{
							const SCameraRotationKey& CamRotKey = pCameraData->m_vecRotation.at( iRotKey );
							lua_newtable( pLua );
							int iCamRotTableIndex = lua_gettop( pLua );
							
							LUA_DELEGATE.AddToTable( iCamRotTableIndex, CS_FRAME_KEY, CamRotKey.nTime );
							LUA_DELEGATE.AddToTable( iCamRotTableIndex, "x", CamRotKey.qRotation.x );
							LUA_DELEGATE.AddToTable( iCamRotTableIndex, "y", CamRotKey.qRotation.y );
							LUA_DELEGATE.AddToTable( iCamRotTableIndex, "z", CamRotKey.qRotation.z );
							LUA_DELEGATE.AddToTable( iCamRotTableIndex, "w", CamRotKey.qRotation.w );
							
							lua_pushnumber( pLua, iRotKey+1 );
							lua_insert( pLua, -2 );
							lua_settable( pLua, -3 );
						}
						lua_pushstring( pLua, CS_ROT_SET_KEY );
						lua_insert( pLua, -2 );
						lua_settable( pLua, -3 );
					}

					//// 타겟 포지션 키 저장
					//if( CT_TARGET_CAMERA == pCameraData->m_Header.nType && 
					//	pCameraData->m_Header.nTargetPositionCount > 0 )
					//{
					//	{
					//		lua_newtable( pLua );
					//		for( int iLookAtKey = 0; iLookAtKey < pCameraData->m_Header.nTargetPositionCount; ++iLookAtKey )
					//		{
					//			const SCameraPositionKey& LookAtKey = pCameraData->m_vecTargetPosition.at( iLookAtKey );
					//			lua_newtable( pLua );
					//			int iLookAtTableIndex = lua_gettop( pLua );

					//			LUA_DELEGATE.AddToTable( iLookAtTableIndex, CS_FRAME_KEY, LookAtKey.nTime );
					//			LUA_DELEGATE.AddToTable( iLookAtTableIndex, "x", LookAtKey.vPosition.x );
					//			LUA_DELEGATE.AddToTable( iLookAtTableIndex, "y", LookAtKey.vPosition.y );
					//			LUA_DELEGATE.AddToTable( iLookAtTableIndex, "z", LookAtKey.vPosition.z );

					//			lua_pushnumber( pLua, iLookAtKey+1 );
					//			lua_insert( pLua, -2 );
					//			lua_settable( pLua, -3 );
					//		}
					//		lua_pushstring( pLua, CS_LOOKAT_SET_KEY );
					//		lua_insert( pLua, -2 );
					//		lua_settable( pLua, -3 );
					//	}
					//}

					// 등장하는 액터 리스트 저장
					if( false == pCamEventInfo->mapActorsShowHide.empty() )
					{
						lua_newtable( pLua );
						int iCamActorShowHideTableIndex = lua_gettop( pLua );

						map<string, bool>::const_iterator iter = pCamEventInfo->mapActorsShowHide.begin();
						for( iter; pCamEventInfo->mapActorsShowHide.end() != iter; ++iter )
							LUA_DELEGATE.AddToTable( iCamActorShowHideTableIndex, iter->first.c_str(), iter->second ? 1 : 0 );

						lua_pushstring( pLua, CS_ACTORS_IN_CAMERA_KEY );
						lua_insert( pLua, -2 );
						lua_settable( pLua, -3 );
					}
						
					lua_pushstring( pLua, CS_CAMERA_DATA_KEY );
					lua_insert( pLua, -2 );
					lua_settable( pLua, -3 );
				}
				break;

			case EventInfo::PARTICLE:
				{
					const ParticleEventInfo* pParticleEventInfo = static_cast<const ParticleEventInfo*>(pEventInfo);
					LUA_DELEGATE.AddToTable( iEventTableIndex, CS_FILEPATH_KEY, pParticleEventInfo->strImportFilePath.c_str() );
					LUA_DELEGATE.AddToTable( iEventTableIndex, CS_DEFAULT_POS_X, pParticleEventInfo->vPos.x );
					LUA_DELEGATE.AddToTable( iEventTableIndex, CS_DEFAULT_POS_Y, pParticleEventInfo->vPos.y );
					LUA_DELEGATE.AddToTable( iEventTableIndex, CS_DEFAULT_POS_Z, pParticleEventInfo->vPos.z );
				}
				break;

			case EventInfo::DOF:
				{
					const DofEventInfo* pDofEventInfo = static_cast<const DofEventInfo*>(pEventInfo);

					LUA_DELEGATE.AddToTable( iEventTableIndex, CS_DOF_ENABLE, pDofEventInfo->bEnable ? 1 : 0 );
					LUA_DELEGATE.AddToTable( iEventTableIndex, CS_DOF_USE_NOW_VALUE_AS_START, pDofEventInfo->bUseNowValueAsStart ? 1 : 0 );

					LUA_DELEGATE.AddToTable( iEventTableIndex, CS_DOF_NEAR_START_FROM, pDofEventInfo->fNearStartFrom );
					LUA_DELEGATE.AddToTable( iEventTableIndex, CS_DOF_NEAR_END_FROM, pDofEventInfo->fNearEndFrom );
					LUA_DELEGATE.AddToTable( iEventTableIndex, CS_DOF_FAR_START_FROM, pDofEventInfo->fFarStartFrom );
					LUA_DELEGATE.AddToTable( iEventTableIndex, CS_DOF_FAR_END_FROM, pDofEventInfo->fFarEndFrom );
					LUA_DELEGATE.AddToTable( iEventTableIndex, CS_DOF_FOCUS_DIST_FROM, pDofEventInfo->fFocusDistFrom );
					LUA_DELEGATE.AddToTable( iEventTableIndex, CS_DOF_NEAR_BLUR_SIZE_FROM, pDofEventInfo->fNearBlurSizeFrom );
					LUA_DELEGATE.AddToTable( iEventTableIndex, CS_DOF_FAR_BLUR_SIZE_FROM, pDofEventInfo->fFarBlurSizeFrom );

					LUA_DELEGATE.AddToTable( iEventTableIndex, CS_DOF_NEAR_START_DEST, pDofEventInfo->fNearStartDest );
					LUA_DELEGATE.AddToTable( iEventTableIndex, CS_DOF_NEAR_END_DEST, pDofEventInfo->fNearEndDest );
					LUA_DELEGATE.AddToTable( iEventTableIndex, CS_DOF_FAR_START_DEST, pDofEventInfo->fFarStartDest );
					LUA_DELEGATE.AddToTable( iEventTableIndex, CS_DOF_FAR_END_DEST, pDofEventInfo->fFarEndDest );
					LUA_DELEGATE.AddToTable( iEventTableIndex, CS_DOF_FOCUS_DIST_DEST, pDofEventInfo->fFocusDistDest );
					LUA_DELEGATE.AddToTable( iEventTableIndex, CS_DOF_NEAR_BLUR_SIZE_DEST, pDofEventInfo->fNearBlurSizeDest );
					LUA_DELEGATE.AddToTable( iEventTableIndex, CS_DOF_FAR_BLUR_SIZE_DEST, pDofEventInfo->fFarBlurSizeDest );
				}
				break;

			//case EventInfo::SOUND:
			//	{
			//		const SoundEventInfo* pSoundEventInfo = static_cast<const SoundEventInfo*>(pEventInfo);
			//		LUA_DELEGATE.AddToTable( iEventTableIndex, CS_FILEPATH_KEY, pSoundEventInfo->strImportFilePath.c_str() );
			//		LUA_DELEGATE.AddToTable( iEventTableIndex, CS_DEFAULT_POS_X, pSoundEventInfo->vPos.x );
			//		LUA_DELEGATE.AddToTable( iEventTableIndex, CS_DEFAULT_POS_Y, pSoundEventInfo->vPos.z );
			//		LUA_DELEGATE.AddToTable( iEventTableIndex, CS_DEFAULT_POS_Z, pSoundEventInfo->vPos.y );
			//	}
			//	break;

			case EventInfo::FADE:
				{
					const FadeEventInfo* pFadeEventInfo = static_cast<const FadeEventInfo*>(pEventInfo);
					LUA_DELEGATE.AddToTable( iEventTableIndex, CS_FADE_KEY, pFadeEventInfo->iFadeKind );
					LUA_DELEGATE.AddToTable( iEventTableIndex, CS_RED, pFadeEventInfo->iColorRed );
					LUA_DELEGATE.AddToTable( iEventTableIndex, CS_GREEN, pFadeEventInfo->iColorGreen );
					LUA_DELEGATE.AddToTable( iEventTableIndex, CS_BLUE, pFadeEventInfo->iColorBlue );
				}
				break;

			case EventInfo::PROP:
				{
					const PropEventInfo* pPropEventInfo = static_cast<const PropEventInfo*>(pEventInfo);
					LUA_DELEGATE.AddToTable( iEventTableIndex, CS_PROP_NAME, pPropEventInfo->strActPropName.c_str() );
					LUA_DELEGATE.AddToTable( iEventTableIndex, CS_PROP_ID, pPropEventInfo->iPropID );
					LUA_DELEGATE.AddToTable( iEventTableIndex, CS_PROP_ACTION_NAME, pPropEventInfo->strActionName.c_str() );
					LUA_DELEGATE.AddToTable( iEventTableIndex, CS_PROP_SHOW, pPropEventInfo->bShow );
				}
				break;

			case EventInfo::SOUND_1:
			case EventInfo::SOUND_2:
			case EventInfo::SOUND_3:
			case EventInfo::SOUND_4:
			case EventInfo::SOUND_5:
			case EventInfo::SOUND_6:
			case EventInfo::SOUND_7:
			case EventInfo::SOUND_8:
				{
					const SoundEventInfo* pSoundEventInfo = static_cast<const SoundEventInfo*>(pEventInfo);
					LUA_DELEGATE.AddToTable( iEventTableIndex, CS_FILEPATH_KEY, pSoundEventInfo->strImportFilePath.c_str() );
					//LUA_DELEGATE.AddToTable( iEventTableIndex, CS_DEFAULT_POS_X, pSoundEventInfo->vPos.x );
					//LUA_DELEGATE.AddToTable( iEventTableIndex, CS_DEFAULT_POS_Y, pSoundEventInfo->vPos.z );
					//LUA_DELEGATE.AddToTable( iEventTableIndex, CS_DEFAULT_POS_Z, pSoundEventInfo->vPos.y );
					LUA_DELEGATE.AddToTable( iEventTableIndex, CS_VOLUME, pSoundEventInfo->fVolume );
				}
				break;

			case EventInfo::SUBTITLE:
				{
					const SubtitleEventInfo* pSubtitleEventInfo = static_cast<const SubtitleEventInfo*>(pEventInfo);
					LUA_DELEGATE.AddToTable( iEventTableIndex, CS_SUBTITLE_UISTRING_ID, pSubtitleEventInfo->iUIStringID );
					//LUA_DELEGATE.AddToTable( iEventTableIndex, CS_SUBTITLE_COLOR, pSubtitleEventInfo->dwColor );
				}
				break;

//#ifdef PRE_ADD_FILTEREVENT
			case EventInfo::COLORFILTER:
				{
					const ColorFilterEventInfo* pFilterEventInfo = static_cast<const ColorFilterEventInfo*>(pEventInfo);
					LUA_DELEGATE.AddToTable( iEventTableIndex, CS_COLORFILTER_MONO, pFilterEventInfo->bMonochrome );
					LUA_DELEGATE.AddToTable( iEventTableIndex, CS_COLORFILTER_R, pFilterEventInfo->vColor.x );
					LUA_DELEGATE.AddToTable( iEventTableIndex, CS_COLORFILTER_G, pFilterEventInfo->vColor.y );
					LUA_DELEGATE.AddToTable( iEventTableIndex, CS_COLORFILTER_B, pFilterEventInfo->vColor.z );
					LUA_DELEGATE.AddToTable( iEventTableIndex, CS_COLORFILTER_VOL, pFilterEventInfo->fVolume );
				}
				break;
//#endif // PRE_ADD_FILTEREVENT

		}
		
		//lua_pushstring( pLua, pEventInfo->strEventName.c_str() );
		lua_pushnumber( pLua, iEventCount );
		lua_insert( pLua, -2 );
		lua_settable( pLua, -3 );

		++iEventCount;
	}

	lua_pushstring( pLua, CS_EVENT_SEQUENCE_SET_KEY );
	lua_insert( pLua, -2 );
	lua_settable( pLua, -3 );
}




bool CDnCutSceneData::SaveToFile( const char* pFileName )
{
	bool bResult = true;

	lua_State* pLua = LUA_DELEGATE.GetLuaState();

	// \ 이거 루아에서 하나만 쓰면 인식 못함. / 로 바꿔버리자.
	string strFileName( pFileName );
	int iFoundPos = 0;
	while( true )
	{
		iFoundPos = (int)strFileName.find_first_of( '\\', iFoundPos );

		if( iFoundPos == (int)string::npos )
			break;

		strFileName.at( iFoundPos ) = '/';
	}

	lua_newtable( pLua );
	
	// 버전 정보
	LUA_DELEGATE.AddToTable( lua_gettop(pLua), CUTSCENEDATA_VERSION_KEY, CUTSCENEDATA_VERSION );

	_SaveToLuaTable( pLua );

	// 루아 글로벌 전역 변수로 테이블로 등록
	lua_setglobal( pLua, CUTSCENEDATA_TABLE_NAME );

	//LUA_DELEGATE.RunString( "dofile( \"script/LuaUtil.lua\" )" );
	LUA_DELEGATE.RunScript( "script/LuaUtil.lua" );

	char caTempStr[ 256 ];
	sprintf( caTempStr, "fileOpen( \"%s\" )", strFileName.c_str() );
	LUA_DELEGATE.RunString( caTempStr );
	sprintf( caTempStr, "table_to_luafile( \"%s\", nil )", CUTSCENEDATA_TABLE_NAME );
	LUA_DELEGATE.RunString( caTempStr );
	LUA_DELEGATE.RunString( "fileClose()" );

	return bResult;
}