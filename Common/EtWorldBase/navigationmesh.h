/* Copyright (C) Greg Snook, 2000. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Greg Snook, 2000"
 */
#ifndef NAVIGATIONMESH_H
#define NAVIGATIONMESH_H
/****************************************************************************************\
	NavigationMesh.h

	NavigationMesh component interface for the Navimesh sample program.
	Included as part of the Game Programming Gems sample code.

	Created 3/18/00 by Greg Snook
	greg@mightystudios.com
    ------------------------------------------------------------------------------------- 
	Notes/Revisions:

\****************************************************************************************/
#include "navigationcell.h"
#include "navigationheap.h"
#include "EtQuadTree.h"
#include <vector>

// forward declaration required
class NavigationPath;

/*	NavigationMesh
------------------------------------------------------------------------------------------
	
	A NavigationMesh is a collecion of NavigationCells used to control object movement while
	also providing path finding line-of-sight testing. It serves as a parent to all the
	Actor objects which exist upon it.
	
------------------------------------------------------------------------------------------
*/

struct SNavFileHeader
{
	int nSignature;
	int nVersion;
	int nTriangleCount;
};

class NavigationMesh
{
public:
	// ----- CREATORS ---------------------

	NavigationMesh();
	~NavigationMesh();

	// ----- OPERATORS --------------------

	// ----- MUTATORS ---------------------
	void Clear();

	void CreateNaviCells( int iCount );

	int AddCell(const EtVector3& PointA, const EtVector3& PointB, const EtVector3& PointC, int nIndex );
	int AddCell( NavigationCell* pCell, int nIndex );
	void DeleteCell( int index );
	void AddCellCount( int nCount );

	void LinkCells();	

	EtVector3 FittingPoint(NavigationCell* Cell, const EtVector3& Point);
	EtVector3 SnapPointToMesh(NavigationCell** CellOut, const EtVector3& Point);
	NavigationCell* FindClosestCell(const EtVector3& Point)const;
	NavigationCell* FindCell(const EtVector3& Point);

	bool LineOfSightTest(NavigationCell* StartCell, const EtVector3& StartPos, NavigationCell* EndCell, const EtVector3& EndPos);
	bool BuildNavigationPath(NavigationPath& NavPath, NavigationCell* StartCell, const EtVector3& StartPos, NavigationCell* EndCell, const EtVector3& EndPos);

	bool ResolveMotionOnMesh(const EtVector3& StartPos, NavigationCell* StartCell, EtVector3& EndPos, NavigationCell** EndCell);

	void Update(float elapsedTime = 1.0f);
	void Render();

	// ----- ACCESSORS --------------------
	int TotalCells()const;
	NavigationCell* Cell(int index);

	bool SaveToStream( CStream* pStream );
	bool LoadFromStream( CStream* pStream, EtMatrix *pWorldMat = NULL );

	int GetEntranceCount() { return ( int )m_vlLinkableNodes.size(); }
	std::vector< int > &GetEntrance() { return m_vlLinkableNodes; }

	NavigationCell * SplitCell( NavigationCell *pCell, int nEdgeIndex, EtVector3 &vSplitPos );
	void CopyFrom( NavigationMesh *pMesh );

	void RemoveAcuteCell();

	int GetIndex( NavigationCell* pCell );

private:

	// ----- DATA -------------------------
	NavigationCell** m_apNavigationCells;
	int m_iWholeCountOfCell;

	// path finding data...
	int m_PathSession;
	NavigationHeap m_NavHeap;

	SAABox m_BoundingBox;
	CEtQuadtree< NavigationCell * > m_NaviCellQuadTree;
	std::vector< int > m_vlLinkableNodes;
	

	// ----- HELPER FUNCTIONS -------------

	// ----- UNIMPLEMENTED FUNCTIONS ------

	NavigationMesh( const NavigationMesh& Src);
	NavigationMesh& operator=( const NavigationMesh& Src);

	enum
	{
		NUM_MAX_WAYPOINT = 200,
	};
};

//- Inline Functions ---------------------------------------------------------------------

//= CREATORS =============================================================================

/*	NavigationMesh
------------------------------------------------------------------------------------------
	
	Default Object Constructor
	
------------------------------------------------------------------------------------------
*/
inline NavigationMesh::NavigationMesh() : m_PathSession( 0 ),
										  m_apNavigationCells( NULL ),
										  m_iWholeCountOfCell( 0 )
{

}





/*	~NavigationMesh
------------------------------------------------------------------------------------------
	
	Default Object Destructor
	
------------------------------------------------------------------------------------------
*/
inline NavigationMesh::~NavigationMesh()
{
	Clear();
}

//= OPERATORS ============================================================================

//= MUTATORS =============================================================================

//:	Clear
//----------------------------------------------------------------------------------------
//
//	Delete all cells associated with this mesh 
//
//-------------------------------------------------------------------------------------://
inline void NavigationMesh::Clear()
{
	for( int i = 0; i < m_iWholeCountOfCell; i++ )
	{
		if( m_apNavigationCells[i] )
			delete m_apNavigationCells[i];
	}

	delete[] m_apNavigationCells;
	m_apNavigationCells = NULL;
	m_iWholeCountOfCell = 0;
}


//:	AddCell
//----------------------------------------------------------------------------------------
//
//	Add a new cell, defined by the three vertices in clockwise order, to this mesh 
//
//-------------------------------------------------------------------------------------://
inline int NavigationMesh::AddCell( const EtVector3& PointA, const EtVector3& PointB, const EtVector3& PointC, int nIndex )
{
	NavigationCell* pNewCell = new NavigationCell;
	pNewCell->Initialize(PointA, PointB, PointC);
	pNewCell->SetIndex( nIndex );
	m_apNavigationCells[ nIndex ] = pNewCell;

	return nIndex;
}

inline int NavigationMesh::AddCell( NavigationCell* pCell, int nIndex )
{
	m_apNavigationCells[ nIndex ] = pCell;

	return nIndex;
}


inline void NavigationMesh::DeleteCell( int nIndex )
{
	if( nIndex >= m_iWholeCountOfCell )
		return;

	delete m_apNavigationCells[ nIndex ];
	m_iWholeCountOfCell--;
	int nCount = m_iWholeCountOfCell - nIndex;
	if( nCount )
	{
		memcpy( m_apNavigationCells + nIndex, m_apNavigationCells + nIndex + 1, sizeof(NavigationCell*) * nCount );
	}
}


//= ACCESSORS ============================================================================
inline int NavigationMesh::TotalCells()const
{
	return m_iWholeCountOfCell;
}

inline NavigationCell* NavigationMesh::Cell(int index)
{
	return m_apNavigationCells[index];
}


//- End of NavigationMesh ----------------------------------------------------------------

//****************************************************************************************

#endif  // end of file      ( NavigationMesh.h )

