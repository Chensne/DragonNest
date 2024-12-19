/* Copyright (C) Greg Snook, 2000. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Greg Snook, 2000"
 */
#define NAVIGATIONCELL_CPP
/****************************************************************************************\
	NavigationCell.cpp

	NavigationCell component implementation for the Navimesh sample program.
	Included as part of the Game Programming Gems sample code.

	Created 3/18/00 by Greg Snook
	greg@mightystudios.com
    ------------------------------------------------------------------------------------- 
	Notes/Revisions:

\****************************************************************************************/

#include "stdafx.h"
#include "navigationcell.h"
#include "navigationheap.h"
#include <stdlib.h>

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif 

NavigationCell::NavigationCell() : m_SessionID(0),
								   m_Open( false ),
								   m_ArrivalCost( 0 ),
								   m_Heuristic( 0 ),
								   m_ArrivalWall( 0 ),
								   m_bUse( true ),
								   m_bEAttribute( false ),
								   m_iIndex( -1 ),
								   m_Type( CT_TERRAIN ),
								   m_nNavMeshType( 0 )
{
	memset( m_nWallAttribute, 0, sizeof( int ) * 3 );
	memset( m_Link, 0, sizeof( m_Link ) );
	memset( m_WallDistance, 0, sizeof( m_WallDistance ) );
};


//	ClassifyPathToCell
//------------------------------------------------------------------------------------------
//	
//	Classifies a Path in relationship to this cell. A path is represented by a 2D line
//	where Point A is the start of the path and Point B is the desired position.
//
//	If the path exits this cell on a side which is linked to another cell, that cell index
//	is returned in the NextCell parameter and SideHit contains the side number of the wall 
//	exited through.
//	
//	If the path collides with a side of the cell which has no link (a solid edge), 
//	SideHit contains the side number (0-2) of the colliding wall.
//
//	In either case PointOfIntersection will contain the point where the path intersected
//	with the wall of the cell if it is provided by the caller.
//	
//------------------------------------------------------------------------------------------
NavigationCell::PATH_RESULT NavigationCell::ClassifyPathToCell(const Line2D& MotionPath, NavigationCell** pNextCell, CELL_SIDE& Side, EtVector2* pPointOfIntersection) const
{
	int InteriorCount = 0;
	for (int i=0; i<3; ++i)
	{
		if (m_Side[i].ClassifyPoint(MotionPath.EndPointB()) != Line2D::RIGHT_SIDE)
		{
			if (m_Side[i].ClassifyPoint(MotionPath.EndPointA()) != Line2D::LEFT_SIDE)
			{
				Line2D::LINE_CLASSIFICATION IntersectResult = MotionPath.Intersection(m_Side[i], pPointOfIntersection);
				if (IntersectResult == Line2D::SEGMENTS_INTERSECT || IntersectResult == Line2D::A_BISECTS_B)
				{
					*pNextCell = m_Link[i];
					Side = (CELL_SIDE)i;
					return (EXITING_CELL);
				}
			}
		}
		else
		{
			InteriorCount++;
		}
	}
	if (InteriorCount == 3)
	{
		return (ENDING_CELL);
	}
	return (NO_RELATIONSHIP);
}

/*	ProjectPathOnCellWall
------------------------------------------------------------------------------------------
	
	ProjectPathOnCellWall projects a path intersecting the wall with the wall itself. This
	can be used to convert a path colliding with a cell wall to a resulting path moving
	along the wall. The input parameter MotionPath MUST contain a starting point (EndPointA)
	which is the point of intersection with the path and cell wall number [SideNumber]
	and an ending point (EndPointB) which resides outside of the cell.
	
------------------------------------------------------------------------------------------
*/
void NavigationCell::ProjectPathOnCellWall(CELL_SIDE SideNumber, Line2D& MotionPath)const
{
	// compute the normalized vector of the cell wall in question
	EtVector2 WallNormal = m_Side[SideNumber].EndPointB() - m_Side[SideNumber].EndPointA();
	EtVec2Normalize( &WallNormal, &WallNormal );

	// determine the vector of our current movement
	EtVector2 MotionVector = MotionPath.EndPointB() - MotionPath.EndPointA();

	// compute dot product of our MotionVector and the normalized cell wall
	// this gives us the magnatude of our motion along the wall
	float DotResult = EtVec2Dot( &MotionVector, &WallNormal );
    
	// our projected vector is then the normalized wall vector times our new found magnatude
	MotionVector = (DotResult * WallNormal);

	// redirect our motion path along the new reflected direction
	MotionPath.SetEndPointB(MotionPath.EndPointA() + MotionVector);

	//
	// Make sure starting point of motion path is within the cell
	//
	EtVector2 NewPoint = MotionPath.EndPointA();
	ForcePointToCellCollumn(NewPoint);
	MotionPath.SetEndPointA(NewPoint);

	//
	// Make sure destination point does not intersect this wall again
	//
	NewPoint = MotionPath.EndPointB();
	ForcePointToWallInterior(SideNumber, NewPoint);
	MotionPath.SetEndPointB(NewPoint);
}

//:	ForcePointToWallInterior
//----------------------------------------------------------------------------------------
//
//	Force a 2D point to the interior side of the specified wall. 
//
//-------------------------------------------------------------------------------------://
bool NavigationCell::ForcePointToWallInterior(CELL_SIDE SideNumber, EtVector2& TestPoint)const
{
	float Distance = m_Side[SideNumber].SignedDistance(TestPoint);
	float Epsilon = 0.001f;

	if (Distance <= Epsilon)
	{
		if (Distance <= 0.0f)
		{
			Distance -= Epsilon;
		}

		Distance = (float)fabs(Distance);
		Distance = (Epsilon>Distance ? Epsilon : Distance);

		// this point needs adjustment
		EtVector2 Normal = m_Side[SideNumber].Normal();
		TestPoint += (Normal * Distance);
		return (true);
	}
	return (false);
}

//:	ForcePointToWallInterior
//----------------------------------------------------------------------------------------
//
//	Force a 3D point to the interior side of the specified wall. 
//
//-------------------------------------------------------------------------------------://
bool NavigationCell::ForcePointToWallInterior(CELL_SIDE SideNumber, EtVector3& TestPoint)const
{
	EtVector2 TestPoint2D(TestPoint.x,TestPoint.z);
	bool PointAltered = ForcePointToWallInterior(SideNumber, TestPoint2D);

	if (PointAltered)
	{
		TestPoint.x = TestPoint2D.x;
		TestPoint.z = TestPoint2D.y;
	}

	return (PointAltered);
}

//:	ForcePointToCellCollumn
//----------------------------------------------------------------------------------------
//
//	Force a 2D point to the interior cell by forcing it to the interior of each wall 
//
//-------------------------------------------------------------------------------------://

bool NavigationCell::ForcePointToCellCollumn(EtVector2& TestPoint)const
{
	//bool PointAltered = false;

	// create a motion path from the center of the cell to our point
	Line2D TestPath(EtVector2(m_CenterPoint.x, m_CenterPoint.z), TestPoint);
	EtVector2 PointOfIntersection;
	CELL_SIDE Side;
	NavigationCell* NextCell;

	PATH_RESULT result = ClassifyPathToCell(TestPath, &NextCell, Side, &PointOfIntersection);
	// compare this path to the cell.

	if (result == EXITING_CELL)
	{
		EtVector2 PathDirection(PointOfIntersection.x - m_CenterPoint.x, PointOfIntersection.y - m_CenterPoint.z);

		PathDirection *= 0.9f;

		TestPoint.x = m_CenterPoint.x + PathDirection.x;
		TestPoint.y = m_CenterPoint.z + PathDirection.y;
		return (true);
	}
	else if (result == NO_RELATIONSHIP)
	{
		TestPoint.x = m_CenterPoint.x;
		TestPoint.y = m_CenterPoint.z;
		return (true);
	}

	return (false);
}

//:	ForcePointToCellCollumn
//----------------------------------------------------------------------------------------
//
//	Force a 3D point to the interior cell by forcing it to the interior of each wall 
//
//-------------------------------------------------------------------------------------://
bool NavigationCell::ForcePointToCellCollumn(EtVector3& TestPoint)const
{
	EtVector2 TestPoint2D(TestPoint.x,TestPoint.z);
	bool PointAltered = ForcePointToCellCollumn(TestPoint2D);

	if (PointAltered)
	{
		TestPoint.x=TestPoint2D.x;
		TestPoint.z=TestPoint2D.y;
	}
	return (PointAltered);
}

//:	ProcessCell
//----------------------------------------------------------------------------------------
//
//	Process this cells neighbors using A* 
//
//-------------------------------------------------------------------------------------://
bool NavigationCell::ProcessCell(NavigationHeap* pHeap)
{
	if (m_SessionID==pHeap->SessionID())
	{
		// once we have been processed, we are closed
		m_Open  = false;

		// querry all our neigbors to see if they need to be added to the Open heap
		for (int i=0;i<3;++i)
		{
			if (m_Link[i])
			{
				// abs(i-m_ArrivalWall) is a formula to determine which distance measurement to use.
				// The Distance measurements between the wall midpoints of this cell
				// are held in the order ABtoBC, BCtoCA and CAtoAB. 
				// We add this distance to our known m_ArrivalCost to compute
				// the total cost to reach the next adjacent cell.
				m_Link[i]->QueryForPath(pHeap, this, m_ArrivalCost+m_WallDistance[abs(i-m_ArrivalWall)]);
			}
		}
		return(true);
	}
	return(false);
}

//:	QueryForPath
//----------------------------------------------------------------------------------------
//
//	Process this cell using the A* heuristic 
//
//-------------------------------------------------------------------------------------://
bool NavigationCell::QueryForPath(NavigationHeap* pHeap, NavigationCell* Caller, float arrivalcost)
{
	if (m_SessionID!=pHeap->SessionID())
	{
		// this is a new session, reset our internal data
		m_SessionID = pHeap->SessionID();

		if (Caller)
		{
			m_Open  = true;
			ComputeHeuristic(pHeap->Goal());
			m_ArrivalCost = arrivalcost;

			// remember the side this caller is entering from
			if (Caller == m_Link[0])
			{
				m_ArrivalWall = 0;
			}
			else if (Caller == m_Link[1])
			{
				m_ArrivalWall = 1;
			}
			else if (Caller == m_Link[2])
			{
				m_ArrivalWall = 2;
			}
		}
		else
		{
			// we are the cell that contains the starting location
			// of the A* search.
			m_Open  = false;
			m_ArrivalCost = 0;
			m_Heuristic = 0;
			m_ArrivalWall = 0;
		}

		// add this cell to the Open heap
		pHeap->AddCell(this);
		return(true);
	}
	else if (m_Open)
	{
		// m_Open means we are already in the Open Heap.
		// If this new caller provides a better path, adjust our data
		// Then tell the Heap to resort our position in the list.
		if ((arrivalcost + m_Heuristic) < (m_ArrivalCost + m_Heuristic))
		{
				m_ArrivalCost = arrivalcost;

				// remember the side this caller is entering from
				if (Caller == m_Link[0])
				{
					m_ArrivalWall = 0;
				}
				else if (Caller == m_Link[1])
				{
					m_ArrivalWall = 1;
				}
				else if (Caller == m_Link[2])
				{
					m_ArrivalWall = 2;
				}

				// ask the heap to resort our position in the priority heap
				pHeap->AdjustCell(this);
				return(true);
		}
	}
	// this cell is closed
	return(false);
}

//:	ComputeHeuristic
//----------------------------------------------------------------------------------------
//
//	Compute the A* Heuristic for this cell given a Goal point 
//
//-------------------------------------------------------------------------------------://
void NavigationCell::ComputeHeuristic(const EtVector3& Goal)
{
	// our heuristic is the estimated distance (using the longest axis delta) between our
	// cell center and the goal location

	float XDelta = fabsf(Goal.x - m_CenterPoint.x);
	float YDelta = fabsf(Goal.y - m_CenterPoint.y);
	float ZDelta = fabsf(Goal.z - m_CenterPoint.z);

#ifdef _WIN32
	m_Heuristic = __max(__max(XDelta,YDelta), ZDelta);
#else
	m_Heuristic = max(max(XDelta,YDelta), ZDelta);
#endif
}

bool NavigationCell::FindLastCollision( EtVector3 &vStart, EtVector3 &vEnd, NavigationCell **ppLastCollisionCell, int &nLastCollisionWall )
{
	int i;
	EtVector2 vStart2D( vStart.x, vStart.z );
	EtVector2 vEnd2D( vEnd.x, vEnd.z );
	Line2D MovingSegment( vStart2D, vEnd2D );
	NavigationCell *pNowCell = this;
	NavigationCell *pPrevCell = this;

	DWORD dwCount = 0;
	while( 1 )
	{
		if( dwCount++ >= 100 )
		{
			nLastCollisionWall = -1;
			return false;
		}

		bool bCollision = false;
		for( i = 0; i < 3; i++ )
		{
			if( pNowCell->Link( i ) == pPrevCell )
			{
				continue;
			}

			Line2D::POINT_CLASSIFICATION StartSideResult1 = pNowCell->m_Side[ i ].ClassifyPoint( vStart2D );
			Line2D::POINT_CLASSIFICATION EndSideResult1 = pNowCell->m_Side[ i ].ClassifyPoint( vEnd2D );

			Line2D::POINT_CLASSIFICATION StartSideResult2 = MovingSegment.ClassifyPoint( pNowCell->m_Side[ i ].EndPointA() );
			Line2D::POINT_CLASSIFICATION EndSideResult2 = MovingSegment.ClassifyPoint( pNowCell->m_Side[ i ].EndPointB() );
			if( ( StartSideResult1 != EndSideResult1 ) && ( StartSideResult2 != EndSideResult2 ) )
			{
				pPrevCell = pNowCell;
				*ppLastCollisionCell = pNowCell;
				nLastCollisionWall = i;
				if( pNowCell->Link( i ) == NULL )
				{
					return true;
				}

				pNowCell = pNowCell->Link( i );
				bCollision = true;
				break;
			}
		}

		if( !bCollision )
		{
			break;
		}
	}

	return true;
}


float NavigationCell::Length( int nSide )
{
	EtVector3 vSide = m_Vertex[ ( nSide + 1 ) % 3 ] - m_Vertex[ nSide ];
	return EtVec3Length( &vSide );
}

bool NavigationCell::SaveToStream( CStream* pStream )
{
	for( int i = 0; i < 3; i++ )
	{
		pStream->Write( &(m_Vertex[i].x), sizeof(float) );
		pStream->Write( &(m_Vertex[i].y), sizeof(float) );
		pStream->Write( &(m_Vertex[i].z), sizeof(float) );
	}

	int iAttribute = (int)m_bEAttribute;
	pStream->Write( &iAttribute, sizeof(int) );
	pStream->Write( &m_Type, sizeof( CELL_TYPE ) );
	pStream->Write( m_nWallAttribute, sizeof( int ) * 3 );
	pStream->Write( &m_nNavMeshType, sizeof( int ) );

	return true;
}

bool NavigationCell::LoadFromStream( CStream* pStream, int nVersion, EtMatrix *pWorldMat )
{
	memset( m_Link, 0, sizeof(m_Link) );

	for( int i = 0; i < 3; i++ )
	{
		pStream->Read( &(m_Vertex[i].x), sizeof(float) );
		pStream->Read( &(m_Vertex[i].y), sizeof(float) );
		pStream->Read( &(m_Vertex[i].z), sizeof(float) );
		if( pWorldMat )
		{
			EtVec3TransformCoord( m_Vertex + i, m_Vertex + i, pWorldMat );
		}
	}

	int iAttribute;
	pStream->Read( &iAttribute, sizeof(int) );
	m_bEAttribute = ( iAttribute != 0 );

	if( nVersion >= 10 )
	{
		pStream->Read( &m_Type, sizeof( CELL_TYPE ) );
	}
	if( nVersion >= 11 )
	{
		pStream->Read( m_nWallAttribute, sizeof( int ) * 3 );
	}
	if( nVersion >= 12 )
	{
		pStream->Read( &m_nNavMeshType, sizeof( int ) );
	}

	ComputeCellData();
	return true;
}


//****************************************************************************************
// end of file      ( NavigationCell.cpp )

