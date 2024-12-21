#pragma once

#define KDTREE_FILE_STRING	"Eternity Engine Kd-Tree File"
#define KDTREE_FILE_VERSION	10
#define KDTREE_HEADER_RESERVED	( 1024 - sizeof( SKdTreeFileHeader ) )
struct SKdTreeFileHeader
{
	SKdTreeFileHeader() 
	{
		memset( szHeaderString, 0, 256 );
		nVersion = 0;
	};

	char szHeaderString[ 256 ];
	int nVersion;
};

struct SKdTreeEvent
{
	enum EVENT_STATE 
	{ 
		STATE_OUT = 0, 
		STATE_ON = 1, 
		STATE_IN = 2 
	};

	SKdTreeEvent( float fSplitPoint, EVENT_STATE State, int nTriangleIndex )
	{
		m_fSplitPoint = fSplitPoint;
		m_dwTriangleIndex = ( State << 30 ) + ( ( DWORD )nTriangleIndex & 0x3fffffff );
	}
	EVENT_STATE GetState()
	{
		return ( SKdTreeEvent::EVENT_STATE )( m_dwTriangleIndex >> 30 );
	}
	int GetOffset()
	{
		return ( m_dwTriangleIndex & 0x3fffffff );
	}
	bool operator<( SKdTreeEvent &Input )
	{
		if( m_fSplitPoint == Input.m_fSplitPoint )
		{
			if( GetState() == Input.GetState() )
			{
				return GetOffset() < Input.GetOffset();
			}
			else
			{
				return GetState() < Input.GetState();
			}
		}
		else
		{
			return m_fSplitPoint < Input.m_fSplitPoint;
		}
	}

	float m_fSplitPoint;
	DWORD m_dwTriangleIndex;
};

struct SKdTreeSplit
{
public:
	enum SPLIT_AXIS 
	{ 
		SPLIT_X = 0, 
		SPLIT_Y = 1, 
		SPLIT_Z = 2, 
		SPLIT_END = 3 
	};

	void SetAxis( SPLIT_AXIS Axis )
	{
		m_dwChildIndex = ( m_dwChildIndex & 0x3fffffff ) + ( Axis << 30 );
	}
	SPLIT_AXIS GetAxis()
	{
		return ( SPLIT_AXIS )( m_dwChildIndex >> 30 );
	}
	void SetChildren( int nChildIndex )
	{
		m_dwChildIndex = ( m_dwChildIndex & 0xc0000000 ) + ( nChildIndex & 0x3fffffff );
	}
	int GetChildren()
	{
		return ( m_dwChildIndex & 0x3fffffff );
	}
	void SetChildCount( int nCount )
	{	
		m_nChildCount = nCount;
	}

	union 
	{
		float m_fSplitPoint;
		int m_nChildCount;
	};
	DWORD m_dwChildIndex;
};

template < class T >
class TKdTree: public TBoostMemoryPool< TKdTree<T> >
{
public:
	TKdTree(void) 
	{
		m_bIsBuild = false;
	}
	virtual ~TKdTree(void) {}

protected:

	std::vector< T > m_vecPrimitive;
	std::vector< SKdTreeSplit > m_vecSplit;
	std::vector< int > m_vecLeaf;
	SAABox m_BoundingBox;
	bool m_bIsBuild;

public:
	void Clear()
	{
		m_bIsBuild = false;
		SAFE_DELETE_VEC( m_vecPrimitive );
		SAFE_DELETE_VEC( m_vecSplit );
		SAFE_DELETE_VEC( m_vecLeaf );
	}
	bool IsBuild() { return m_bIsBuild ; }
	void BuildTree();
	void BuildTree( int nTreeIndex, std::vector< SKdTreeEvent > &vecXAxis, std::vector< SKdTreeEvent > &vecYAxis, std::vector< SKdTreeEvent > &vecZAxis, SAABox &BoundingBox, std::vector< SAABox > &vecBoundingBox );
	float CalcMinSAH( SKdTreeSplit::SPLIT_AXIS Axis, std::vector< SKdTreeEvent > &vecSplit, SAABox &BoundingBox, float &fMinSplit );
	float CalcSAH( int nLeft, int nCenter, int nRight, float fLeftPoint, float fRightPoint);

	void AddPrimitive( T &Triangle ) { m_vecPrimitive.push_back( Triangle ); }

	void LoadTree( CStream *pStream );
	bool SaveTree( const char *pFileName );
	void SaveTree( CStream *pStream );

	void Pick( SAABox &Box, std::vector< T * > &vecPrimitive );

/*	struct STreeDrawVertex
	{
		EtVector3 Vertex;
		DWORD dwColor;
	};
	std::vector< int > vecCollisionResult;
	std::vector< STreeDrawVertex > m_vecVertex;
	std::vector< STreeDrawVertex > m_vecTriVertex;
	void GenerateCollsionMesh();
	void GenerateMesh();
	void AddBoundingBox( int nTreeIndex, SAABox &Boundingbox );
	void DrawTree();*/
};

#include "KdTree.inl"