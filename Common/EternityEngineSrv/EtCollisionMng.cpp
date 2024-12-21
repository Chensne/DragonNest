#include "StdAfx.h"
#include "EtCollisionMng.h"
#include "EtFindCollision.h"
#include "StringUtil.h"

std::map< long, CEtCollisionMng::SKdTreeData > CEtCollisionMng::s_mapKdTree;
CSyncLock CEtCollisionMng::s_Lock;

CEtCollisionMng::CEtCollisionMng( CMultiRoom *pRoom )
: CMultiSingleton<CEtCollisionMng, MAX_SESSION_COUNT>( pRoom ) 
{
	m_pCurKdTree = NULL;
}

CEtCollisionMng::~CEtCollisionMng(void)
{
	Clear();
}

void CEtCollisionMng::Clear()
{
	ScopeLock<CSyncLock> Lock(s_Lock);

	std::map< long, SKdTreeData >::iterator it = s_mapKdTree.begin();

	while( it != s_mapKdTree.end() )
	{
		if( it->second.pKdTree == m_pCurKdTree )
		{
			it->second.nRefCount--;
			if( it->second.nRefCount <= 0 )
			{
				delete m_pCurKdTree;
				m_pCurKdTree = NULL;
				s_mapKdTree.erase( it );
			}
			break;
		}
		it++;
	}
}

bool CEtCollisionMng::FindCollision( std::vector< SCollisionPrimitive * > &vecPrimitive, SAABox &BoundingBox, EtVector3 &vMove, DNVector(SCollisionResponse) &vecResponse, bool bCalcContactTime )
{
	if( m_pCurKdTree == NULL )
	{
		return false;
	}

	int i;
	bool bCollision;
	static std::vector< SKdTreeCollisionPrimitive * > vecResult[MAX_SESSION_COUNT];
	SCollisionResponse Response;

	bCollision = false;
	vecResult[GetRoom()->GetRoomID()].clear();
	m_pCurKdTree->Pick( BoundingBox, vecResult[GetRoom()->GetRoomID()] );
	if( !vecResult[GetRoom()->GetRoomID()].empty() )
	{
		std::vector< SKdTreeCollisionPrimitive * >::iterator it;
		it = vecResult[GetRoom()->GetRoomID()].begin();
		Response.vMove = vMove;
		for( i = 0; i < ( int )vecPrimitive.size(); i++ )
		{
			for( ; it != vecResult[GetRoom()->GetRoomID()].end(); it++ )
			{
				if( CEtCollisionFinder::GetInstance().FindCollision( *vecPrimitive[ i ], *( ( *it )->pPrimitive), Response, bCalcContactTime ) )
				{
					Response.pCollisionPrimitive = ( *it )->pPrimitive;
					vecResponse.push_back( Response );
					if( Response.vExtraNormal.x != FLT_MAX )
					{
						Response.vNormal = Response.vExtraNormal;
						vecResponse.push_back( Response );
					}
					bCollision = true;
				}
			}
		}
	}

	return bCollision;
}

void CEtCollisionMng::Load( CStream *pStream ) 
{ 
	if( m_pCurKdTree )
	{
		Clear();
	}

	s_Lock.Lock();

	std::string szTemp = pStream->GetName();
	ToLowerA( szTemp );
	long lHashCode = GetHashCode( szTemp.c_str() );
	std::map< long, SKdTreeData >::iterator it = s_mapKdTree.find( lHashCode );
	if( it != s_mapKdTree.end() )
	{
		it->second.nRefCount++;
		m_pCurKdTree = it->second.pKdTree;
		s_Lock.UnLock();
		return;
	}

	SKdTreeData KdTreeData;
	KdTreeData.pKdTree = new TKdTree< SKdTreeCollisionPrimitive >;
	KdTreeData.nRefCount = 1;
	s_mapKdTree.insert(  std::make_pair( lHashCode, KdTreeData ) );

	s_Lock.UnLock();

	m_pCurKdTree = KdTreeData.pKdTree;
	m_pCurKdTree->LoadTree( pStream );
}

void CEtCollisionMng::PreLoad( CStream *pStream )
{
	s_Lock.Lock();

	std::string szTemp = pStream->GetName();
	ToLowerA( szTemp );
	long lHashCode = GetHashCode( szTemp.c_str() );
	std::map< long, SKdTreeData >::iterator it = s_mapKdTree.find( lHashCode );
	if( it != s_mapKdTree.end() )
	{
		it->second.nRefCount++;
		s_Lock.UnLock();
		return;
	}

	SKdTreeData KdTreeData;
	KdTreeData.pKdTree = new TKdTree< SKdTreeCollisionPrimitive >;
	KdTreeData.nRefCount = 1;
	s_mapKdTree.insert(  std::make_pair( lHashCode, KdTreeData ) );

	s_Lock.UnLock();

	KdTreeData.pKdTree->LoadTree( pStream );
}