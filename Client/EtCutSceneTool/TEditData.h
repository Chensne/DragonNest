#pragma once
#include "stdafx.h"
#include "IDnCutSceneDataReader.h"

//template <class Entity>
//class TEditData
//{
//public:
//
//private:
//	Entity m_pData;
//	int m_iType;
//
//	TEditData( void ) {};
//public:
//	TEditData( Entity pData ) : m_pData( pData ), m_iType( -1 )
//	{
//		_ASSERT( "템플릿에 정의되지 않은 데이터 타입입니다." );
//	};
//
//	//TEditData( ActionInfo* pData ) : m_pData( pData ), m_iType( ACTION ) {};
//	//TEditData( KeyInfo* pData ) : m_pData( pData ), m_iType( KEY ) {};
//	//TEditData( SubKey* pData ) : m_pData( pData ), m_iType( SUBKEY ) {};
//	//TEditData( CamEventInfo* pData ) : m_pData( pData ), m_iType( MAX_CAMERA ) {};
//	//TEditData( PropEventInfo* pData ) : m_pData( pData ), m_iType( PROP ) {};
//	//TEditData( ParticleEventInfo* pData ) : m_pData( pData ), m_iType( PARTICLE ) {};
//	//TEditData( SoundEventInfo* pData ) : m_pData( pData ), m_iType( SOUND ) {};
//	//TEditData( FadeEventInfo* pData ) : m_pData( pData ), m_iType( FADE ) {};
//
//	int GetType( void ) { return m_iType; };
//};
//
//
//
//// 툴에서 데이터를 갖고 작업할 때 간편하게 하기 위한 헬퍼 클래스.
//// 툴의 코어에서 관리하는 데이터와는 별도로 편집용으로 포인터를 공유하여 따로 리스트를 관리한다.
//template <class Entity>
//class TDataHelper
//{
//public:
//	static vector< TEditData<Entity*>* > s_vlpEditData;		// 데이터가 insert 될 때 new 하고 CToolData 가 해제될 때에 for_each로 파괴 시키자.
//	static map< int, TEditData<Entity*>* >s_mapEditData;
//
//	TDataHelper( void ) {};
//	~TDataHelper( void )
//	{
//		for_each( s_vlpEditData.begin(), s_vlpEditData.end(), DeleteData< TEditData<Entity*>* >() );
//	};
//
//	static void InsertData( Entity* pData )
//	{
//		TEditData<Entity*>* pTData = new TEditData<Entity*>(pData);
//		s_vlpEditData.push_back( pTData );
//		s_mapEditData.insert( make_pair(pData->iID, pTData) );
//	};
//
//	static bool RemoveData( Entity* pData )
//	{
//		bool bResult = false;
//
//		vector< TEditData<Entity*>* >::iterator iter = find( s_vlpEditData.begin(), s_vlpEditData.end(), pData );
//		if( s_vlpEditData.end() != iter )
//		{
//			map< int, TEditData<Entity*>* >::iterator iterMap = s_mapEditData.find( iter->iID );
//			_ASSERT( s_mapEditData.end() != iterMap );
//			if( s_mapEditData.end() != iterMap )
//			{
//				delete iterMap->second;
//				s_mapEditData.erase( iterMap );
//				s_vlpEditData.erase( iter );
//			}
//		}
//
//		return bResult;
//	}
//
//	static bool RemoveData( int iID )
//	{
//		bool bResult = false;
//
//		map< int, TEditData<Entity*>* >::iterator iterMap = s_mapEditData.find( iID );
//		if( s_mapEditData.end() != iterMap )
//		{
//			vector< TEditData<Entity*>* >::iterator iter = find( s_vlpEditData.begin(), s_vlpEditData.end(), iterMap->second );
//			_ASSERT( s_vlpEditData.end() != iter );
//			if( s_vlpEditData.end() != iter )
//			{
//				delete iterMap->second;
//				s_mapEditData.erase( iterMap );
//				s_vlpEditData.erase( iter );
//			}
//		}
//
//		return bResult;
//	}
//
//	// 기타 등등 필요한 헬퍼 함수들을 선언한다.
//	static int GetType( int iID )
//	{
//		int iResult = TEditData::COUNT;
//
//		map< int, TEditData<Entity*>* >::iterator iter = s_mapEditData.find( iID );
//		if( s_mapEditData.end() != iter )
//		{
//			iResult = iter->second->GetType();
//		}
//
//		return iResult;
//	}
//};
//
//
//template <class Entity>
//vector< TEditData<Entity*>* > TDataHelper<Entity>::s_vlpEditData;
//
//template <class Entity>
//map< int, TEditData<Entity*>* > TDataHelper<Entity>::s_mapEditData;