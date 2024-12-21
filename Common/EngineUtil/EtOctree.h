#pragma once

#include "EtPrimitive.h"
#include "EtCollisionFunc.h"
#include "CriticalSection.h"
#include "PerfCheck.h"

template < class T >
class CEtOctreeNode: public TBoostMemoryPool< CEtOctreeNode<T> >
{
public:
	CEtOctreeNode()
	{
		Init();
	}
	CEtOctreeNode( SSphere &Sphere )
	{
		Init();
		m_NodeSphere = Sphere;
	}
	~CEtOctreeNode()
	{
		int i;

		for( i = 0; i < 8; i++ )
		{
			SAFE_DELETE( m_pChildNode[ i ] );
		}
	}

protected:

	SSphere m_NodeSphere;
	CEtOctreeNode *m_pParent;
	CEtOctreeNode *m_pChildNode[ 8 ];
	UINT			m_uiChildObjectCount;

	DNVector(T) m_vecObjectList;
	static float s_fMinRadius;

public:
	void Init()
	{
		m_pParent = NULL;
		m_uiChildObjectCount = 0;
		memset( m_pChildNode, 0, sizeof( CEtOctreeNode * ) * 8 );
	}
	CEtOctreeNode *GetParent()
	{
		return m_pParent;
	}
	void SetParent( CEtOctreeNode* pParent )
	{
		m_pParent = pParent;
	}
	UINT GetChildObjectCount(){ return m_uiChildObjectCount; }
	SSphere& GetNodeSphere(){ return m_NodeSphere; }
	int GetObjectCount()
	{
		return ( int )m_vecObjectList.size();
	}
	T GetObject( int nIndex )
	{
		return m_vecObjectList[ nIndex ];
	}
	void SetRange( EtVector3 &CenterPos, float fRadius )
	{
		m_NodeSphere.Center = CenterPos;
		m_NodeSphere.fRadius = fRadius;
	}

	void GetAllObject( DNVector(T) &vecPickObject )
	{
		if( !m_vecObjectList.empty() )
		{
			vecPickObject.insert( vecPickObject.end(), m_vecObjectList.begin(), m_vecObjectList.end() );
		}

		for( volatile int i = 0; i < 8; i++ )
		{
			if( m_pChildNode[ i ] && m_pChildNode[i]->GetChildObjectCount() )
			{
				m_pChildNode[ i ]->GetAllObject( vecPickObject );
			}
		}
	}
	CEtOctreeNode *Insert( T Object, SSphere &Sphere )
	{
		if( m_NodeSphere.fRadius <= s_fMinRadius )
		{
			++m_uiChildObjectCount;
			CEtOctreeNode* pParent = GetParent();
			while( pParent )
			{
				++pParent->m_uiChildObjectCount;
				pParent = pParent->GetParent();
			}

			m_vecObjectList.push_back( Object );
			return this;
		}
		if( Sphere.fRadius > m_NodeSphere.fRadius )
		{
			if( m_pParent )
			{
				return m_pParent->Insert( Object, Sphere );
			}
			else
			{
				ASSERT( 0 && "Object Is Too Big" );
				return NULL;
			}
			return this;
		}

		volatile bool bInside[ 8 ];
		volatile int i, nInsideCount, nChildIndex;
		volatile float fChildPivot;
		SSphere ChildSphere[ 8 ];
		static EtVector3 CenterTable[ 8 ] = 
		{
			EtVector3( 1.0f, 1.0f, 1.0f ),
			EtVector3( 1.0f, 1.0f, -1.0f ),
			EtVector3( 1.0f, -1.0f, 1.0f ),
			EtVector3( 1.0f, -1.0f, -1.0f ),
			EtVector3( -1.0f, 1.0f, 1.0f ),
			EtVector3( -1.0f, 1.0f, -1.0f ),
			EtVector3( -1.0f, -1.0f, 1.0f ),
			EtVector3( -1.0f, -1.0f, -1.0f ),
		};

		fChildPivot = ( m_NodeSphere.fRadius / 1.732050f ) * 0.5f;
		nInsideCount = 0;
		for( i = 0; i < 8; i++ )
		{
			ChildSphere[ i ].Center = m_NodeSphere.Center + CenterTable[ i ] * fChildPivot;
			ChildSphere[ i ].fRadius = m_NodeSphere.fRadius * 0.5f;
			bInside[ i ] = false;
			if( ChildSphere[ i ].IsInside( Sphere ) )
			{
				nChildIndex = i;
				nInsideCount++;
				bInside[ i ] = true;
			}
		}

		if( nInsideCount == 0 )
		{
			++m_uiChildObjectCount;
			CEtOctreeNode* pParent = GetParent();
			while( pParent )
			{
				++pParent->m_uiChildObjectCount;
				pParent = pParent->GetParent();
			}

			m_vecObjectList.push_back( Object );
			return this;
		}
		else if( nInsideCount >= 2 )
		{
			float fMinDist, fDist;

			fMinDist = FLT_MAX;
			for( i = 0; i < 8; i++ )
			{
				if( bInside[ i ] )
				{
					fDist = EtVec3LengthSq( &( Sphere.Center - ChildSphere[ i ].Center ) );
					if( fDist < fMinDist )
					{
						fMinDist = fDist;
						nChildIndex = i;
					}
				}
			}
		}
		if( m_pChildNode[ nChildIndex ] == NULL )
		{
			m_pChildNode[ nChildIndex ] = new CEtOctreeNode( ChildSphere[ nChildIndex ] );
			m_pChildNode[ nChildIndex ]->SetParent( this );
		}
		return m_pChildNode[ nChildIndex ]->Insert( Object, Sphere );
	}
	struct ValidCheckFind {
		T _val;
		ValidCheckFind( T &Object ) { _val = Object; }
		bool operator() ( T &Object ) { return ( _val && Object ) ? ( Object == _val ) : false; }
	};
	bool Remove( T Object )
	{
		DNVector(T)::iterator Iter;
		Iter = std::find_if( m_vecObjectList.begin(), m_vecObjectList.end(), ValidCheckFind(Object) );
		if( Iter != m_vecObjectList.end() )
		{
			--m_uiChildObjectCount;
			CEtOctreeNode* pParent = GetParent();
			while( pParent )
			{
				--pParent->m_uiChildObjectCount;
				pParent = pParent->GetParent();
			}

			m_vecObjectList.erase( Iter );
			return true;
		}

		volatile int i;

		for( i = 0; i < 8; i++ )
		{
			if( m_pChildNode[ i ] == NULL )
			{
				continue;
			}
			if( m_pChildNode[ i ]->Remove( Object ) )
			{
				return true;
			}
		}

		return false;
	}
	CEtOctreeNode *Update( T Object, SSphere &Sphere )
	{
		if( m_NodeSphere.IsInside( Sphere ) )
		{
			return this;
		}
		else
		{
			return NULL;
		}
	}
	void Pick( EtVector3 &Position, EtVector3 &Direction, T &PickObject, float &fMinDist )
	{
		int i;

		for( i = 0; i < ( int )m_vecObjectList.size(); i++ )
		{
			if( !m_vecObjectList[i] ) continue;
			SAABox ObjectBox;
			float fDist;

			m_vecObjectList[ i ]->GetBoundingBox( ObjectBox );
			if( TestLineToBox( Position, Direction, ObjectBox, fDist ) )
			{
				if( fDist < fMinDist )
				{
					PickObject = m_vecObjectList[ i ];
					fMinDist = fDist;
				}
			}

		}
		for( i = 0; i < 8; i++ )
		{
			if( m_pChildNode[ i ] )
			{
				m_pChildNode[ i ]->Pick( Position, Direction, PickObject, fMinDist );
			}
		}
	}
	void PickBySize( EtVector3 &Position, EtVector3 &Direction, T &PickObject, float &fMinSize, float &fMinDist, SOBB &Obb )
	{
		int i;
		EtVector3 vTemp;

		for( i = 0; i < ( int )m_vecObjectList.size(); i++ )
		{
			if( !m_vecObjectList[i] ) continue;
			SAABox AABox;
			SOBB ObjectBox;
			float fDist, fSize;
			SSegment Segment;
			Segment.vDirection = Direction * fMinDist;
			Segment.vOrigin = Position;

			m_vecObjectList[ i ]->GetBoundingBox( ObjectBox );
			m_vecObjectList[ i ]->GetBoundingBox( AABox );
			if( TestSegmentToOBB( Segment, ObjectBox ) )
			{
//				if( TestLineToBox( Position, Direction, AABox, fDist ) == false ) {
				fDist = EtVec3Length( &( Position - ObjectBox.Center ) ) + max( max( ObjectBox.Extent[0], ObjectBox.Extent[1] ), ObjectBox.Extent[2] );
//				}
				fSize = ( ObjectBox.Extent[0] / 100.f ) * ( ObjectBox.Extent[1] / 100.f ) * ( ObjectBox.Extent[2] / 100.f );
				if( fDist < fMinDist ) {
					PickObject = m_vecObjectList[ i ];
					fMinSize = fSize;
					fMinDist = fDist;
				}
				/*
				if( fSize < fMinSize && fDist < fMinDist )
				{
					if( fMinSize == FLT_MAX ) {
						Obb = ObjectBox;
//						TestOBBToOBB( Obb, ObjectBox )
					}
					PickObject = m_vecObjectList[ i ];
					fMinSize = fSize;
					fMinDist = fDist;
				}
				else if( fSize < fMinSize && fMinSize != FLT_MAX ) {
					if( TestOBBToOBB( Obb, ObjectBox ) == true ) {
						PickObject = m_vecObjectList[ i ];
						fMinSize = fSize;
						fMinDist = fDist;
					}
				}
				*/
			}

		}
		for( i = 0; i < 8; i++ )
		{
			if( m_pChildNode[ i ] )
			{
				m_pChildNode[ i ]->PickBySize( Position, Direction, PickObject, fMinSize, fMinDist, Obb );
			}
		}
	}

	void Pick( SAABox &Box, DNVector(T) &vecPickObject, bool bInside )
	{
		if( !TestBoxToSphere( Box, m_NodeSphere ) )
		{
			return;
		}
		if( Box.IsInside( m_NodeSphere ) )
		{
			GetAllObject( vecPickObject );
		}
		else
		{
			int i;
			for( i = 0; i < ( int )m_vecObjectList.size(); i++ )
			{
				if( !m_vecObjectList[i] ) continue;
				SAABox ObjectBox;
				m_vecObjectList[ i ]->GetBoundingBox( ObjectBox );
				if( bInside )
				{
					if( Box.IsInside( ObjectBox ) )
					{
						vecPickObject.push_back( m_vecObjectList[ i ] );
					}
				}
				else
				{
					if( TestBoxToBox( ObjectBox, Box ) )
					{
						vecPickObject.push_back( m_vecObjectList[ i ] );
					}
				}
			}
			for( i = 0; i < 8; i++ )
			{
				if( m_pChildNode[ i ] )
				{
					m_pChildNode[ i ]->Pick( Box, vecPickObject, bInside );
				}
			}
		}
	}

	void Pick( SCircle &Circle, DNVector(T) &vecPickObject, bool bInside)
	{
		SCircle NodeCircle;

		NodeCircle.Center.x = m_NodeSphere.Center.x;
		NodeCircle.Center.y = m_NodeSphere.Center.z;
		NodeCircle.fRadius = m_NodeSphere.fRadius;
		if( !TestCircleToCircle( Circle, NodeCircle ) )
		{
			return;
		}
		if( Circle.IsInside( NodeCircle ) )
		{
			GetAllObject( vecPickObject );
		}
		else
		{
			int i;
			for( i = 0; i < ( int )m_vecObjectList.size(); i++ )
			{
				if( !m_vecObjectList[i] ) continue;
				SAABox ObjectBox;
				SAABox2D ObjectBox2D;
				m_vecObjectList[ i ]->GetBoundingBox( ObjectBox );
				ObjectBox2D.Reset();
				ObjectBox2D.AddPoint( EtVector2( ObjectBox.Max.x, ObjectBox.Max.z ) );
				ObjectBox2D.AddPoint( EtVector2( ObjectBox.Min.x, ObjectBox.Min.z ) );
				if( bInside )
				{
					if( Circle.IsInside( ObjectBox2D ) )
					{
						vecPickObject.push_back( m_vecObjectList[ i ] );
					}
				}
				else
				{
					if( TestCircleToBox2D( Circle, ObjectBox2D ) )
					{
						vecPickObject.push_back( m_vecObjectList[ i ] );
					}
				}
			}
			for( i = 0; i < 8; i++ )
			{
				if( m_pChildNode[ i ] )
				{
					m_pChildNode[ i ]->Pick( Circle, vecPickObject, bInside );
				}
			}
		}
	}

	void Pick( SSphere &Sphere, DNVector(T) &vecPickObject, bool bInside, bool bActorSize )
	{
		if( !TestSphereToSphere( Sphere, m_NodeSphere ) )
		{
			return;
		}
		if( Sphere.IsInside( m_NodeSphere ) )
		{
			GetAllObject( vecPickObject );
		}
		else
		{
			volatile int i;
			for( i = 0; i < ( int )m_vecObjectList.size(); i++ )
			{
				if( !m_vecObjectList[i] ) continue;
				SSphere ObjectSphere;
				m_vecObjectList[ i ]->GetBoundingSphere( ObjectSphere, bActorSize );
				
				if( bInside )
				{
					if( Sphere.IsInside( ObjectSphere ) )
					{
						vecPickObject.push_back( m_vecObjectList[ i ] );
					}
				}
				else
				{
					if( TestSphereToSphere( ObjectSphere, Sphere ) )
					{
						vecPickObject.push_back( m_vecObjectList[ i ] );
					}
				}
			}
			for( i = 0; i < 8; i++ )
			{
				if( m_pChildNode[ i ] && m_pChildNode[i]->GetChildObjectCount() > 0 )
				{
					m_pChildNode[ i ]->Pick( Sphere, vecPickObject, bInside, bActorSize );
				}
			}
		}
	}
};

template < class T >
class CEtOctree:public TBoostMemoryPool<CEtOctree<T>>
{
public:
	CEtOctree(bool bLock = true) 
	{
		m_pRootNode = NULL;
		m_bInit = false;
		m_bUseLock = bLock;
	}
	virtual ~CEtOctree(void) 
	{
		SAFE_DELETE( m_pRootNode );
	}

protected:
	bool m_bInit;
	CEtOctreeNode< T > *m_pRootNode;
	CSyncLock m_Lock;
	bool m_bUseLock;

public:
	void UseLock( bool bUse ) { m_bUseLock = bUse; }
	void Initialize( EtVector3 &CenterPos, float fWorldSize )
	{
		m_bInit = true;
		float fRootRadius = fWorldSize * 1.414214f * 0.5f;
		SAFE_DELETE( m_pRootNode );
		m_pRootNode = new CEtOctreeNode< T >();
		m_pRootNode->SetRange( CenterPos, fRootRadius );
	}
	CEtOctreeNode< T > *Insert( T Object, SSphere &Sphere )
	{
		ScopeLock<CSyncLock> Lock(m_Lock, m_bUseLock);
		if( !m_bInit )
		{
			return NULL;
		}
		return m_pRootNode->Insert( Object, Sphere );
	}
	bool Remove( T Object, CEtOctreeNode< T > *pCurNode )
	{
		ScopeLock<CSyncLock> Lock(m_Lock, m_bUseLock);
		if( !m_bInit )
		{
			return false;
		}
		if( pCurNode )
		{
			return pCurNode->Remove( Object );
		}
		else
		{
			return m_pRootNode->Remove( Object );
		}
	}
	CEtOctreeNode< T > *Update( T Object, SSphere &Sphere, CEtOctreeNode< T > *pCurNode )
	{
		ScopeLock<CSyncLock> Lock(m_Lock, m_bUseLock);
		if( !m_bInit )
		{
			return NULL;
		}
		if( !pCurNode )
		{
			return NULL;
		}

		CEtOctreeNode< T > *pUpdateNode;

		pUpdateNode = pCurNode->Update( Object, Sphere );
		if( pUpdateNode == NULL )
		{
			CEtOctreeNode< T > *pParentNode;

			bool bRet = Remove( Object, pCurNode );
			pParentNode = pCurNode->GetParent();
			if( pParentNode )
			{
				pUpdateNode = pParentNode->Insert( Object, Sphere );
			}
			if( pUpdateNode == NULL )
			{
				pUpdateNode = m_pRootNode->Insert( Object, Sphere );
			}
			ASSERT( pUpdateNode );
		}

		return pUpdateNode;
	}
	void Pick( EtVector3 &Position, EtVector3 &Direction, T &PickObject )
	{
		ScopeLock<CSyncLock> Lock(m_Lock, m_bUseLock);
		if( !m_bInit )
		{
			return;
		}

		float fMinDist;

		fMinDist = FLT_MAX;
		m_pRootNode->Pick( Position, Direction, PickObject, fMinDist );
	}
	void PickBySize( EtVector3 &Position, EtVector3 &Direction, T &PickObject )
	{
		ScopeLock<CSyncLock> Lock(m_Lock, m_bUseLock);
		if( !m_bInit )
		{
			return;
		}

		float fMinSize, fMinDist;
		SOBB ObjectBox;


		fMinSize = FLT_MAX;
		fMinDist = 1000000.f;
		m_pRootNode->PickBySize( Position, Direction, PickObject, fMinSize, fMinDist, ObjectBox );
	}

	void Pick( SAABox &Box, DNVector(T) &vecPickObject, bool bInside = false )
	{
		ScopeLock<CSyncLock> Lock(m_Lock, m_bUseLock);
		if( m_bInit )
			m_pRootNode->Pick( Box, vecPickObject, bInside );
	}

	void Pick( SCircle &Circle, DNVector(T) &vecPickObject, bool bInside = false )
	{
		ScopeLock<CSyncLock> Lock(m_Lock, m_bUseLock);
		if( m_bInit )
			m_pRootNode->Pick( Circle, vecPickObject, bInside );
	}

	void Pick( SSphere &Sphere, DNVector(T) &vecPickObject, bool bInside = false, bool bActorSize = false )
	{
		ScopeLock<CSyncLock> Lock(m_Lock, m_bUseLock);
		if( m_bInit )
			m_pRootNode->Pick( Sphere, vecPickObject, bInside, bActorSize );
	}
};
