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
//		_ASSERT( "���ø��� ���ǵ��� ���� ������ Ÿ���Դϴ�." );
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
//// ������ �����͸� ���� �۾��� �� �����ϰ� �ϱ� ���� ���� Ŭ����.
//// ���� �ھ�� �����ϴ� �����Ϳʹ� ������ ���������� �����͸� �����Ͽ� ���� ����Ʈ�� �����Ѵ�.
//template <class Entity>
//class TDataHelper
//{
//public:
//	static vector< TEditData<Entity*>* > s_vlpEditData;		// �����Ͱ� insert �� �� new �ϰ� CToolData �� ������ ���� for_each�� �ı� ��Ű��.
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
//	// ��Ÿ ��� �ʿ��� ���� �Լ����� �����Ѵ�.
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