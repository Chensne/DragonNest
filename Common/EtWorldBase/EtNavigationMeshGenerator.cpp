#include "StdAfx.h"
#include "EtNavigationMeshGenerator.h"


namespace EtNavigationMesh
{

const int RECT_DIVIDE = 20;
static const float EPSILON = 0.0000000001f;
const float UNITSIZE = 50.0f;
const int REORDER_TOLERANCE = 3;
const int REORDER_FACE_MAX = 32;

CEtNavigationMeshGenerator::CEtNavigationMeshGenerator( void ) : m_iXBlockCount( 0 ), m_iYBlockCount( 0 ),
																 m_iXAttributeCount( 0 ), m_iYAttributeCount( 0 ), m_iNumUsingVertices( 0 )
{
	m_vlNaviRects.assign( 1, S_RECT() );
}

CEtNavigationMeshGenerator::~CEtNavigationMeshGenerator( void )
{

}

char CEtNavigationMeshGenerator::NormalizeAttribute( char cAttribute )
{
	char cResult = 0;

	if( 0xf == (cAttribute & 0xf) )
		cResult = 2;

	if( (cAttribute & 0x0f) > 0 )
		cResult = 1;

	return cResult;
}

void CEtNavigationMeshGenerator::SetNaviAttributes( int iAttribute, int iXStart, int iYStart, 
													int iXEnd, int iYEnd )
{
	for( int iY = iYStart; iY < iYEnd; ++iY )
	{
		for( int iX = iXStart; iX < iXEnd; ++iX )
		{
			m_vlNaviAttributes[ m_iXAttributeCount*iY + iX ] = iAttribute;
		}
	}
}

void CEtNavigationMeshGenerator::Initialize( int iX, int iY, char* pAttribute )
{
	m_iXAttributeCount = iX;
	m_iYAttributeCount = iY;
	m_iXBlockCount = m_iXAttributeCount - 1;
	m_iYBlockCount = m_iYAttributeCount - 1;

	int iNumAttributeCount = m_iXAttributeCount * m_iYAttributeCount;

	m_vlMapAttributes.assign( iNumAttributeCount, 0 );
	m_vlNaviAttributes.assign( iNumAttributeCount, 0 );

	for( int i = 0; i < iNumAttributeCount; ++i )
	{
		m_vlMapAttributes.at( i ) = NormalizeAttribute( *(pAttribute+i) );
	}
}

void CEtNavigationMeshGenerator::Generate( int &iNumVertices, int &iNumFaces )
{
	CollapseAttributeBlocks();
	CollapseNaviRectsAndTriangulatePolygon();
	ReorderFaceIndex();
	PolygonReduction();
	MakeupNavigationData( iNumVertices, iNumFaces );
}

void CEtNavigationMeshGenerator::CollapseAttributeBlocks()
{
	for( int iY = 0; iY < m_iYBlockCount; ++iY )
	{
		for( int iX = 0; iX < m_iXBlockCount; ++iX )
		{
			int iIndex = (iY*m_iXAttributeCount) + iX;
			unsigned char cMapAttribute = m_vlMapAttributes.at( iIndex );

			if( 1 == cMapAttribute )
			{
				continue;
			}

			int iNaviAttribute = NaviAttrAt( iX, iY );
			if( iNaviAttribute != 0 )
			{
				int iWidth = m_vlNaviRects.at(iNaviAttribute).iRight - m_vlNaviRects.at(iNaviAttribute).iLeft;
				iX += (iWidth - 1);
				continue;
			}

			bool bRight = true;
			bool bUp = true;
			int iCollapsedWidth = 1;
			int iCollapsedHeight = 1;
			while( bRight || bUp )
			{
				if( bRight )
				{
					for( int iYCheck = 0; iYCheck < iCollapsedHeight; ++iYCheck )
					{
						int iCurNaviAttribute = NaviAttrAt( iX+iCollapsedWidth, iY+iYCheck );
						int iCurMapAttribute = m_vlMapAttributes.at( m_iXAttributeCount*(iY+iYCheck) + iX+iCollapsedWidth );
						if( false == ((iX+iCollapsedWidth) < m_iXBlockCount) ||
							iCurMapAttribute != (int)cMapAttribute ||
							0 != iCurNaviAttribute )
						{
							bRight = false;
							break;
						}
					}

					if( bRight )
						iCollapsedWidth++;
				}

				if( bUp )
				{
					for( int iXCheck = 0; iXCheck < iCollapsedWidth; ++iXCheck )
					{ 
						int iCurNaviAttribute = NaviAttrAt( iX+iXCheck, iY+iCollapsedHeight );
						int iCurMapAttribute = m_vlMapAttributes.at( m_iXAttributeCount*(iY+iCollapsedHeight) + iX+iXCheck );
						if( false == ((iY+iCollapsedHeight) < m_iYBlockCount) ||
							iCurMapAttribute != (int)cMapAttribute ||
							0 != iCurNaviAttribute )
						{
							bUp = false;
							break;
						}
					}

					if( bUp )
						iCollapsedHeight++;                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                         
				}

				if( (iX + iCollapsedWidth) % RECT_DIVIDE == 0 )
					bRight = false;

				if( (iY + iCollapsedHeight) % RECT_DIVIDE == 0 )
					bUp = false;
			}

			int iNaviRectID = int(m_vlNaviRects.size());
			S_RECT CollapsedRect( iNaviRectID, iX, iX+iCollapsedWidth, 
								   iY, iY+iCollapsedHeight, cMapAttribute );
			m_vlNaviRects.push_back( CollapsedRect );
			SetNaviAttributes( iNaviRectID, iX, iY, iX+iCollapsedWidth, iY+iCollapsedHeight );
			iX += (iCollapsedWidth-1);
		}
	}
}

void CEtNavigationMeshGenerator::CollapseNaviRectsAndTriangulatePolygon( void )
{
	int iNumRects = (int)m_vlNaviRects.size();
	for( int iRect = 1; iRect < iNumRects; ++iRect )
	{
		S_RECT& Rect = m_vlNaviRects.at(iRect);
		S_VERTEX Vertex;
		vector<S_VERTEX> vlNaviVertices;

		int iRectWidth = Rect.iRight - Rect.iLeft;
		int iRectHeight = Rect.iHeight - Rect.iBottom;

		int iSplitWidth = 0;
		int iSplitHeight = 0;
		int iXAttributeBefore = 0;
		int iYAttributeBefore = 0;
	
		ScanSplitVertex( LB_TO_RB, Rect.iLeft, Rect.iBottom,
								   Rect.iRight, Rect.iBottom, 
								   iRectWidth, iRectHeight,
								   iSplitWidth, iSplitHeight,
								   iXAttributeBefore, iYAttributeBefore,
								   Rect.iAttribute, vlNaviVertices );
		
		ScanSplitVertex( RB_TO_RT, Rect.iRight, Rect.iBottom,
								   Rect.iRight, Rect.iHeight,
								   iRectWidth, iRectHeight,
								   iSplitWidth, iSplitHeight,
								   iXAttributeBefore, iYAttributeBefore,
								   Rect.iAttribute, vlNaviVertices );

		ScanSplitVertex( RT_TO_LT, Rect.iRight, Rect.iHeight,
								   Rect.iLeft, Rect.iHeight, 
								   iRectWidth, iSplitWidth, 
								   iSplitHeight, iRectHeight,
								   iXAttributeBefore, iYAttributeBefore,
								   Rect.iAttribute, vlNaviVertices );

		ScanSplitVertex( LT_TO_LB, Rect.iLeft, Rect.iHeight, 
								   Rect.iLeft, Rect.iBottom, 
								   iRectWidth, iRectHeight,
								   iSplitWidth, iSplitHeight,
								   iXAttributeBefore, iYAttributeBefore,
								   Rect.iAttribute, vlNaviVertices );

		TriangulatePolygonAndRegister( vlNaviVertices, Rect.iAttribute );
	}
}

void CEtNavigationMeshGenerator::ScanSplitVertex( int iScanDirection, int iXStart, int iYStart, int iXEnd, int iYEnd, 
												  int iRectWidth, int iRectHeight, int& iSplitWidth, int& iSplitHeight,
												  int& iXAttributeBefore, int& iYAttributeBefore,
												  int iCurRectNaviAttribute, vector<S_VERTEX>& vlNaviVertices )
{
	bool bXBound = false;
	bool bYBound = false;

	int iCurXAttr = 0;
	int iCurYAttr = 0;
	int iCurXPos = 0;
	int iCurYPos = 0;

	if( iXEnd < iXStart )
		swap( iXStart, iXEnd );

	if( iYEnd < iYStart )
		swap( iYStart, iYEnd );

	switch( iScanDirection )
	{
		case LB_TO_RB:
			{
				bXBound = (0 == iXStart);
				bYBound = (0 == iYStart);

				iCurYPos = iYStart;
				if( bXBound )
				{
					iYAttributeBefore = 0;
					iSplitHeight = iRectHeight;
				}
				else
				{
					iYAttributeBefore = NaviAttrAt( iXStart-1, iCurYPos );
					++iCurYPos;
					iSplitHeight = 1;

					for( iCurYPos; iCurYPos < iYEnd; ++iCurYPos )
					{
						iCurYAttr = NaviAttrAt( iXStart-1, iCurYPos );
						if( iCurYAttr != iYAttributeBefore )
						{
							--iCurYPos;
							break;
						}
						++iSplitHeight;
					}
				}

				iCurXPos = iXStart;
				if( bYBound )
				{
					iXAttributeBefore = 0;
					iSplitWidth = iRectWidth;
				}
				else
				{
					iXAttributeBefore = NaviAttrAt( iCurXPos, iYStart-1 );
					++iCurXPos;
					iSplitWidth = 1;

					for( iCurXPos; iCurXPos < iXEnd; ++iCurXPos )
					{
						iCurXAttr = NaviAttrAt( iCurXPos, iYStart-1 );
						if( iCurXAttr != iXAttributeBefore )
						{
							--iCurXPos;
							break;
						}
						++iSplitWidth;
					}
				}

				if( (((iSplitHeight > 1) || (iSplitWidth > 2)) && 
					((iSplitWidth > 1) || (iSplitHeight > 2))) || 
					false == (iXAttributeBefore == 0 && iYAttributeBefore == 0) )
				{
					vlNaviVertices.push_back( S_VERTEX(iXStart, iYStart) );
				}

				if( bYBound )
				{
					iCurXAttr = 0;
					iSplitWidth = iRectWidth;
				}

				if( iSplitWidth != iRectWidth )
				{
					iXAttributeBefore = NaviAttrAt( iCurXPos, iYStart-1 );
					++iCurXPos;
					iSplitWidth = 1;

					for( iCurXPos; iCurXPos < iXEnd; ++iCurXPos )
					{
						iCurXAttr = NaviAttrAt( iCurXPos, iYStart-1 );
						if( iCurXAttr == iXAttributeBefore )
						{
							++iSplitWidth;
						}
						else
						{
							iXAttributeBefore = iCurXAttr;
							vlNaviVertices.push_back( S_VERTEX(iCurXPos, iYStart) );
							iSplitWidth = 1;
						}
					}
				}
			}
			break;

		case RB_TO_RT:
			{
				bXBound = (m_iXBlockCount == iXEnd);
				bYBound = false;

				iCurYPos = iYStart;
				if( bXBound )
				{
					iYAttributeBefore = 0;
					iSplitHeight = iRectHeight;
				}
				else
				{
					iYAttributeBefore = NaviAttrAt( iXEnd, iCurYPos );
					++iCurYPos;
					iSplitHeight = 1;

					for( iCurYPos; iCurYPos < iYEnd; ++iCurYPos )
					{
						iCurYAttr = NaviAttrAt( iXEnd, iCurYPos );
						if( iCurYAttr != iYAttributeBefore )
						{
							--iCurYPos;
							break;
						}
						++iSplitHeight;
					}
				}

				if( (((iSplitHeight > 1) || (iSplitWidth > 2)) && 
					((iSplitWidth > 1) || (iSplitHeight > 2))) || 
					false == (iXAttributeBefore == 0 && iYAttributeBefore == 0) )
				{
					vlNaviVertices.push_back( S_VERTEX(iXEnd, iYStart) );
				}

				if( bXBound )
				{
					iYAttributeBefore = 0;
					iSplitHeight = iRectHeight;
				}

				if( iSplitHeight != iRectHeight )
				{
					iYAttributeBefore = NaviAttrAt( iXEnd, iCurYPos );
					++iCurYPos;
					iSplitHeight = 1;

					for( iCurYPos; iCurYPos < iYEnd; ++iCurYPos )
					{
						iCurYAttr = NaviAttrAt( iXEnd, iCurYPos );
						if( iCurYAttr == iYAttributeBefore )
						{
							++iSplitHeight;
						}
						else
						{
							iYAttributeBefore = iCurYAttr;	
							vlNaviVertices.push_back( S_VERTEX(iXEnd, iCurYPos) );
							iSplitHeight = 1;
						}
					}
				}
			}
			break;

		case RT_TO_LT:
			{
				bXBound = false;
				bYBound = (iYEnd == m_iYBlockCount);

				iCurXPos = iXEnd-1;
				if( bYBound )
				{
					iXAttributeBefore = 0;
					iSplitWidth = iRectWidth;
				}
				else
				{
					iXAttributeBefore = NaviAttrAt( iCurXPos, iYEnd );
					--iCurXPos;
					iSplitWidth = 1;

					for( iCurXPos; iCurXPos >= iXStart; --iCurXPos )
					{
						iCurXAttr = NaviAttrAt( iCurXPos, iYEnd );
						if( iCurXAttr != iXAttributeBefore )
						{
							++iCurXPos;
							break;
						}
						++iSplitWidth;
					}
				}

				if( (((iSplitHeight > 1) || (iSplitWidth > 2)) && 
					((iSplitWidth > 1) || (iSplitHeight > 2))) || 
					false == (iXAttributeBefore == 0 && iYAttributeBefore == 0) )
				{
					vlNaviVertices.push_back( S_VERTEX(iXEnd, iYEnd) );
				}

				if( bYBound )
				{
					iXAttributeBefore = 0;
					iSplitWidth = iRectWidth;
				}

				if( iSplitWidth != iRectWidth )
				{
					iXAttributeBefore = NaviAttrAt( iCurXPos, iYEnd );
					--iCurXPos;
					iSplitWidth = 1;

					for( iCurXPos; iCurXPos >= iXStart; --iCurXPos )
					{
						iCurXAttr = NaviAttrAt( iCurXPos, iYEnd );
						if( iCurXAttr == iXAttributeBefore )
						{
							++iSplitWidth;
						}
						else
						{
							iXAttributeBefore = iCurXAttr;
							vlNaviVertices.push_back( S_VERTEX(iCurXPos+1, iYEnd) );
							iSplitWidth = 1;
						}
					}
				}
			}
			break;

		case LT_TO_LB:
			{
				bXBound = (iXStart == 0);
				bYBound = false;

				iCurYPos = iYEnd-1;
				if( bXBound )
				{
					iYAttributeBefore = 0;
					iSplitHeight = iRectHeight;
				}
				else
				{
					iYAttributeBefore = NaviAttrAt( iXStart-1, iCurYPos );
					--iCurYPos;
					iSplitHeight = 1;

					for( iCurYPos; iCurYPos >= iYStart; --iCurYPos )
					{
						iCurYAttr = NaviAttrAt( iXStart-1, iCurYPos );
						if( iCurYAttr != iYAttributeBefore )
						{
							++iCurYPos;
							break;
						}
						++iSplitHeight;
					}
				}

				if( (((iSplitHeight > 1) || (iSplitWidth > 2)) && 
					((iSplitWidth > 1) || (iSplitHeight > 2))) || 
					false == (iXAttributeBefore == 0 && iYAttributeBefore == 0) )
				{
					vlNaviVertices.push_back( S_VERTEX(iXStart, iYEnd) );
				}

				if( bXBound )
				{
					iYAttributeBefore = 0;
					iSplitHeight = iRectHeight;
				}

				if( iSplitHeight != iRectHeight )
				{
					iYAttributeBefore = NaviAttrAt( iXStart-1, iCurYPos );
					--iCurYPos;
					iSplitHeight = 1;

					for( iCurYPos; iCurYPos >= iYStart; --iCurYPos )
					{
						iCurYAttr = NaviAttrAt( iXStart-1, iCurYPos );
						if( iCurYAttr == iYAttributeBefore )
						{
							++iSplitHeight;
						}
						else
						{
							iYAttributeBefore = iCurYAttr;
							vlNaviVertices.push_back( S_VERTEX(iXStart, iCurYPos+1) );
							iSplitHeight = 1;
						}
					}
				}
			}
			break;
	}
}

void CEtNavigationMeshGenerator::TriangulatePolygonAndRegister( /*IN*/ const vector<S_VERTEX>& vlVertices, int iAttribute )
{
	vector<S_VERTEX> vlResults;
	TriangulatePolygon( vlVertices, vlResults );

	for( int i = 0; i < (int)vlResults.size(); i += 3 )
	{
		S_VERTEX v[ 3 ];
		v[ 0 ] = vlResults.at( i );
		v[ 1 ] = vlResults.at( i+1 );
		v[ 2 ]= vlResults.at( i+2 );

		RegisterFace( v, iAttribute );
	}
}

void CEtNavigationMeshGenerator::RegisterFace( S_VERTEX* apVertices, int iAttribute )
{
	S_FACE Face;
	int aiIndex[ 3 ] = { 0 };
	for( int i = 0; i < 3; ++i )
	{
		vector<S_VERTEX>::iterator iter = find( m_vlNaviMeshVertices.begin(), m_vlNaviMeshVertices.end(), apVertices[ i ] );
		if( m_vlNaviMeshVertices.end() == iter )
		{
			Face.aiVertexIndex[ i ] = (int)m_vlNaviMeshVertices.size();
			apVertices[ i ].vlContainedFaceIndices.push_back( (int)m_vlNaviMeshFaces.size() );
			m_vlNaviMeshVertices.push_back( apVertices[ i ] );
		}
		else
		{
			Face.aiVertexIndex[ i ] = (int)std::distance( m_vlNaviMeshVertices.begin(), iter );
			iter->vlContainedFaceIndices.push_back( (int)m_vlNaviMeshFaces.size() );
			continue;
		}	
	}

	Face.iNaviRectID = iAttribute;
	
	m_vlNaviMeshFaces.push_back( Face );

}

void CEtNavigationMeshGenerator::RemoveFace( int iFaceIndex )
{
	vector<S_FACE>::iterator iter = m_vlNaviMeshFaces.begin() + iFaceIndex;
	
	for( int i = 0; i < 3; ++i )
	{
		int iVertexIndex = iter->aiVertexIndex[ i ];
		S_VERTEX& Vertex = m_vlNaviMeshVertices.at( iVertexIndex );
		vector<int>& vlCF = Vertex.vlContainedFaceIndices;
		vector<int>::iterator iter = find( vlCF.begin(), vlCF.end(), iFaceIndex );
		if( vlCF.end() != iter )
			vlCF.erase( iter );
	}
	
	m_vlNaviMeshFaces.at( iFaceIndex ).Removed();
}


float CEtNavigationMeshGenerator::Area(const vector<S_VERTEX> &contour)
{
	int n = (int)contour.size();

	float A=0.0f;

	for(int p=n-1,q=0; q<n; p=q++)
	{
		A+= contour[p].GetX()*contour[q].GetY() - contour[q].GetX()*contour[p].GetY();
	}
	
	return A*0.5f;
}

/*
 InsideTriangle decides if a point P is Inside of the triangle
 defined by A, B, C.
*/
bool CEtNavigationMeshGenerator::InsideTriangle( int Ax, int Ay,
											     int Bx, int By,
												 int Cx, int Cy,
												 int Px, int Py )

{
	int ax, ay, bx, by, cx, cy, apx, apy, bpx, bpy, cpx, cpy;
	int cCROSSap, bCROSScp, aCROSSbp;

	ax = Cx - Bx;  ay = Cy - By;
	bx = Ax - Cx;  by = Ay - Cy;
	cx = Bx - Ax;  cy = By - Ay;
	apx= Px - Ax;  apy= Py - Ay;
	bpx= Px - Bx;  bpy= Py - By;
	cpx= Px - Cx;  cpy= Py - Cy;

	aCROSSbp = ax*bpy - ay*bpx;
	cCROSSap = cx*apy - cy*apx;
	bCROSScp = bx*cpy - by*cpx;

	return ((aCROSSbp >= 0.0f) && (bCROSScp >= 0.0f) && (cCROSSap >= 0.0f));
};

bool CEtNavigationMeshGenerator::Snip(const vector<S_VERTEX> &contour,int u,int v,int w,int n,int *V)
{
	int p;
	//float Ax, Ay, Bx, By, Cx, Cy, Px, Py;
	int Ax, Ay, Bx, By, Cx, Cy, Px, Py;

	Ax = contour[V[u]].GetX();
	Ay = contour[V[u]].GetY();

	Bx = contour[V[v]].GetX();
	By = contour[V[v]].GetY();

	Cx = contour[V[w]].GetX();
	Cy = contour[V[w]].GetY();

	if ( EPSILON > (((Bx-Ax)*(Cy-Ay)) - ((By-Ay)*(Cx-Ax))) ) return false;

	for (p=0;p<n;p++)
	{
		if( (p == u) || (p == v) || (p == w) ) continue;
		Px = contour[V[p]].GetX();
		Py = contour[V[p]].GetY();
		if (InsideTriangle(Ax,Ay,Bx,By,Cx,Cy,Px,Py)) return false;
	}

	return true;
}

bool CEtNavigationMeshGenerator::TriangulatePolygon(const vector<S_VERTEX> &contour,vector<S_VERTEX> &result)
{
	/* allocate and initialize list of Vertices in polygon */

	int n = (int)contour.size();
	if ( n < 3 ) return false;

	int *V = new int[n];

	/* we want a counter-clockwise polygon in V */

	if ( 0.0f < Area(contour) )
	for (int v=0; v<n; v++) V[v] = v;
	else
	for(int v=0; v<n; v++) V[v] = (n-1)-v;

	int nv = n;

	/*  remove nv-2 Vertices, creating 1 triangle every time */
	int count = 2*nv;   /* error detection */

	for(int m=0, v=nv-1; nv>2; )
	{
		/* if we loop, it is probably a non-simple polygon */
		if (0 >= (count--))
		{
			//** Triangulate: ERROR - probable bad polygon!
			delete [] V;
			return false;
		}

		/* three consecutive vertices in current polygon, <u,v,w> */
		int u = v  ; if (nv <= u) u = 0;     /* previous */
		v = u+1; if (nv <= v) v = 0;     /* new v    */
		int w = v+1; if (nv <= w) w = 0;     /* next     */

		if ( Snip(contour,u,v,w,nv,V) )
		{
			int a,b,c,s,t;

			/* true names of the vertices */
			a = V[u]; b = V[v]; c = V[w];

			/* output Triangle */
			result.push_back( contour[a] );
			result.push_back( contour[b] );
			result.push_back( contour[c] );

			m++;

			/* remove v from remaining polygon */
			for(s=v,t=v+1;t<nv;s++,t++) V[s] = V[t]; nv--;

			/* resest error detection counter */
			count = 2*nv;
		}
	}

	delete []V;

	return true;
}

void CEtNavigationMeshGenerator::ReorderFaceIndex( void )
{
	bool bNeedReorder = true;

	while( bNeedReorder )
	{
		bNeedReorder = false;

		int iNumVertices = (int)m_vlNaviMeshVertices.size();
		for( int iVertex = 0; iVertex < iNumVertices; ++iVertex )
		{
			S_VERTEX& Vertex = m_vlNaviMeshVertices.at( iVertex );
			
			if( (int)Vertex.vlContainedFaceIndices.size() < REORDER_TOLERANCE )
				continue;

			vector<int> vlReordereTargetFaces;
			int iCurNaviRectID = 0;

			bool bCantReorder = false;
			int iContainedIndices = (int)Vertex.vlContainedFaceIndices.size();
			for( int k = 0; k < iContainedIndices; ++k )
			{
				int iFaceIndex = Vertex.vlContainedFaceIndices.at( k );
				if( -1 < iFaceIndex )
				{
					const S_FACE& Face = m_vlNaviMeshFaces.at( iFaceIndex );
					if( iCurNaviRectID != Face.iNaviRectID )
					{
						bCantReorder = true;
						break;
					}

					vlReordereTargetFaces.push_back( Vertex.vlContainedFaceIndices.at(k) ); 
				}
			}

			if( bCantReorder )
				continue;

			int iSelectedStartVertexIndex = 0;
			S_FACE& FirstFace = m_vlNaviMeshFaces.at( Vertex.vlContainedFaceIndices.front() );
			for( int i = 0; i < 3; ++i )
			{
				if( (m_vlNaviMeshVertices.at( FirstFace.aiVertexIndex[ i ] ).iX == Vertex.iX) &&
					(m_vlNaviMeshVertices.at( FirstFace.aiVertexIndex[ i ] ).iY == Vertex.iY) )
				{
					iSelectedStartVertexIndex = FirstFace.aiVertexIndex[ i ];
					break;
				}
			}

			vector<int> vlReorderedIndices[ 3 ];
			for( int i = 0; i < 3; ++i )
				vlReorderedIndices[ i ].assign( vlReordereTargetFaces.size(), int() );
			int iNumReorderTargetFaces = (int)vlReordereTargetFaces.size();
			for( int iFace = 0; iFace < iNumReorderTargetFaces; ++iFace )
			{
				S_FACE& CurFace = m_vlNaviMeshFaces.at( vlReordereTargetFaces.at(iFace) );
				for( int i = 0; i < 3; ++i )
				{
					if( CurFace.aiVertexIndex[ i ] == iSelectedStartVertexIndex )
					{
						for( int k = 0; k < 3; ++k ) 
						{
							int iVertexIndex = (i+k) % 3;
							vlReorderedIndices[ k ].at(iFace) = ( CurFace.aiVertexIndex[ iVertexIndex ] );
						}
						break;
					}
				}
			}

			for( int iLeftFace = 0; iLeftFace < iNumReorderTargetFaces-1; ++iLeftFace )
			{
				int iRightFace = iLeftFace+1;
				for( iRightFace; iRightFace < iNumReorderTargetFaces; ++iRightFace )
				{
					if( vlReorderedIndices[ 2 ].at(iLeftFace) == 
						vlReorderedIndices[ 1 ].at(iRightFace) )
					{
						if( iLeftFace+1 != iRightFace )
						{
							// 바로 옆에 인접해 있지 않다면 붙여버린다.
							std::swap( vlReordereTargetFaces[ iLeftFace+1 ], vlReordereTargetFaces[ iRightFace ] );

							for( int i = 0; i < 3; ++i )
								std::swap( vlReorderedIndices[ i ].at(iLeftFace+1), vlReorderedIndices[ i ].at(iRightFace) );
						}
						break;
					}
				}

				if( iRightFace == iNumReorderTargetFaces )
				{
					bCantReorder = true;
					break;
				}
			}

			if( bCantReorder )
				continue;

			if( vlReorderedIndices[ 2 ].back() != vlReorderedIndices[ 1 ].front() )
				continue;

			vector<S_VERTEX> vlSourceVertices;
			vlSourceVertices.assign( iNumReorderTargetFaces, S_VERTEX() );
			for( int i = 0; i < iNumReorderTargetFaces; ++i )
			{
				vlSourceVertices[ i ].iX = m_vlNaviMeshVertices.at( vlReorderedIndices[2].at(i) ).iX;
				vlSourceVertices[ i ].iY = m_vlNaviMeshVertices.at( vlReorderedIndices[2].at(i) ).iY;
			}

			vector<S_VERTEX> vlReorderedFaceVertices;
			TriangulatePolygon( vlSourceVertices, vlReorderedFaceVertices );

			for( int i = 0; i < (int)vlReorderedFaceVertices.size(); i += 3 )
			{
				S_VERTEX v[ 3 ];
				v[ 0 ] = vlReorderedFaceVertices[ i ];
				v[ 1 ] = vlReorderedFaceVertices[ i+1 ];
				v[ 2 ] = vlReorderedFaceVertices[ i+2 ];

				RegisterFace( v, iCurNaviRectID );

				bNeedReorder = true;
			}

			for( int i = 0; i < iNumReorderTargetFaces; ++i )
			{
				RemoveFace( vlReordereTargetFaces.at(i) );
				bNeedReorder = true;
			}

		}
	}
}

void CEtNavigationMeshGenerator::MakeupNavigationData( int& iNumVertices, int& iNumFaces )
{
	int iVertexCount = 0;
	int iFaceCount = 0;
	for( int i = 0; i < (int)m_vlNaviMeshVertices.size(); ++i )
	{
		S_VERTEX& Vertex = m_vlNaviMeshVertices.at( i );

		vector3 ResultVertex;
		ResultVertex.x = float( Vertex.iX ) * UNITSIZE;
		ResultVertex.y = 0.0f;
		ResultVertex.z = float( Vertex.iY ) * UNITSIZE;
		m_vlResultVertices.push_back( ResultVertex );

		Vertex.iIndex = iVertexCount;
		++iVertexCount;
	}
	iNumVertices = iVertexCount;

	for( int i = 0; i < (int)m_vlNaviMeshFaces.size(); ++i )
	{
		S_FACE& Face = m_vlNaviMeshFaces.at( i );
		if( false == Face.bRemoved )
		{
			m_vlResultFaceIndices.push_back( Face.aiVertexIndex[ 0 ] );
			m_vlResultFaceIndices.push_back( Face.aiVertexIndex[ 2 ] );
			m_vlResultFaceIndices.push_back( Face.aiVertexIndex[ 1 ] );
			++iFaceCount;
		}
	}
	iNumFaces = iFaceCount;
}

void CEtNavigationMeshGenerator::PolygonReduction( void )
{
	int iNumVertices = (int)m_vlNaviMeshVertices.size();
	for( int i = 0; i < iNumVertices; ++i )
	{
		S_VERTEX& Vertex = m_vlNaviMeshVertices.at( i );
		if( false == Vertex.vlContainedFaceIndices.empty() )
			m_iNumUsingVertices += 1;

		Vertex.iIndex = i;

		if( NaviAttrAt(Vertex.iX-1, Vertex.iY) == 0 ||
			NaviAttrAt(Vertex.iX+1, Vertex.iY) == 0 ||
			NaviAttrAt(Vertex.iX, Vertex.iY-1) == 0 ||
			NaviAttrAt(Vertex.iX, Vertex.iY+1) == 0 ||
			NaviAttrAt(Vertex.iX-1, Vertex.iY-1) == 0 ||
			NaviAttrAt(Vertex.iX+1, Vertex.iY+1) == 0)
			Vertex.bBound = true;
		else
			Vertex.bBound = false;
	}

	int iNumFaces = (int)m_vlNaviMeshFaces.size();
	for( int i = 0; i < iNumFaces; ++i )
	{
		S_FACE& Face = m_vlNaviMeshFaces.at( i );
		if( false == Face.bRemoved )
		{
			for( int k = 0; k < 3; ++k )
			{
				S_VERTEX& Vertex = m_vlNaviMeshVertices.at( Face.aiVertexIndex[ k ] );
				Vertex.setNeighborVertexIndices.insert( Face.aiVertexIndex[ (k+1)%3 ] );
				Vertex.setNeighborVertexIndices.insert( Face.aiVertexIndex[ (k+2)%3 ] );
			}
		}
	}

	int iNumDesireVertices = m_iNumUsingVertices - (m_iNumUsingVertices / 3);
	while( m_iNumUsingVertices > iNumDesireVertices )
	{
		S_VERTEX* pVertexToRemove = CalcMinimumCostEdge();
		CollapseEdge( pVertexToRemove );
	}
}

S_VERTEX* CEtNavigationMeshGenerator::CalcMinimumCostEdge( void )
{
	S_VERTEX* pResult = NULL;
	float fMinimumCost = FLT_MAX;

	for( int i = 0; i < (int)m_vlNaviMeshVertices.size(); ++i )
	{
		S_VERTEX& Vertex = m_vlNaviMeshVertices.at( i );
		if( Vertex.bRemoved )
			continue;

		if( FLT_MAX == Vertex.fCollapseCost )
			continue;

		if( Vertex.fCollapseCost < fMinimumCost )
		{
			pResult = &Vertex;
		}
	}

	if( NULL == pResult )
	{	
		int iGreatestContainFaceCount = 0;
		for( int i = 0; i < (int)m_vlNaviMeshVertices.size(); ++i )
		{
			S_VERTEX& Vertex = m_vlNaviMeshVertices.at( i );

			if( Vertex.bRemoved )
				continue;

			if( iGreatestContainFaceCount < (int)Vertex.vlContainedFaceIndices.size() )
			{
				pResult = &Vertex;
				iGreatestContainFaceCount = (int)Vertex.vlContainedFaceIndices.size();
			}
		}

		if( pResult )
		{
			ComputeEdgeCostAtVertex( pResult->iIndex );
		}
	}

	return pResult;
}

void CEtNavigationMeshGenerator::CollapseEdge( S_VERTEX* pVertexToRemove )
{
	if( NULL == pVertexToRemove->pCollapse )
	{
		RemoveVertex( pVertexToRemove );
		return;
	}

	vector<int> vlNeighborsOfVertexToRemove;
	set<int>::iterator iter = pVertexToRemove->setNeighborVertexIndices.begin();
	for( iter; iter != pVertexToRemove->setNeighborVertexIndices.end(); ++iter )
	{
		vlNeighborsOfVertexToRemove.push_back( *iter );
	}

	vector<int> vlTemp = pVertexToRemove->vlContainedFaceIndices;
	for( int i = (int)vlTemp.size()-1; i >= 0; --i )
	{
		int iFaceIndex = vlTemp.at( i );
		bool bHasVertex = HasVertex( iFaceIndex, pVertexToRemove->pCollapse );
		if( bHasVertex )
		{
			RemoveFace( iFaceIndex );
		}
	}

	S_VERTEX* pCollapseDest = pVertexToRemove->pCollapse;
	for( int i = (int)pVertexToRemove->vlContainedFaceIndices.size()-1; i >= 0; --i )
	{
		int iFaceIndex = pVertexToRemove->vlContainedFaceIndices.at( i );
		ReplaceVertex( iFaceIndex, pVertexToRemove, pCollapseDest );
	}

	RemoveVertex( pVertexToRemove );

	for( int i = 0; i < (int)vlNeighborsOfVertexToRemove.size(); ++i )
	{
		ComputeEdgeCostAtVertex( vlNeighborsOfVertexToRemove.at(i) );
	}
}

bool CEtNavigationMeshGenerator::HasVertex( int iFaceIndex, S_VERTEX* pVertex )
{
	const S_FACE& Face = m_vlNaviMeshFaces.at( iFaceIndex );
	for( int i = 0; i < 3; ++i )
	{
		if( Face.aiVertexIndex[ i ] == pVertex->iIndex )
			return true;
	}

	return false;
}

void CEtNavigationMeshGenerator::RemoveVertex( S_VERTEX* pVertexToRemove )
{
	if( NULL == pVertexToRemove )
		return;

	pVertexToRemove->Removed();
	m_iNumUsingVertices--;
}

void CEtNavigationMeshGenerator::ReplaceVertex( int iFaceIndex, S_VERTEX* pVertexToReplace, S_VERTEX* pDestVertex )
{
	S_FACE& Face = m_vlNaviMeshFaces.at( iFaceIndex );
	for( int i = 0; i < 3; ++i )
	{
		if( Face.aiVertexIndex[ i ] == pVertexToReplace->iIndex )
		{
			m_vlNaviMeshVertices.at( Face.aiVertexIndex[(i+1)%3] ).setNeighborVertexIndices.erase( pVertexToReplace->iIndex );
			m_vlNaviMeshVertices.at( Face.aiVertexIndex[(i+2)%3] ).setNeighborVertexIndices.erase( pVertexToReplace->iIndex );

			Face.aiVertexIndex[ i ] = pDestVertex->iIndex;

			pDestVertex->setNeighborVertexIndices.insert( Face.aiVertexIndex[(i+1)%3] );
			pDestVertex->setNeighborVertexIndices.insert( Face.aiVertexIndex[(i+2)%3] );

			m_vlNaviMeshVertices.at( Face.aiVertexIndex[(i+1)%3] ).setNeighborVertexIndices.insert( pDestVertex->iIndex );
			m_vlNaviMeshVertices.at( Face.aiVertexIndex[(i+2)%3] ).setNeighborVertexIndices.insert( pDestVertex->iIndex );

			break; 
		}
	}

	pDestVertex->setNeighborVertexIndices.erase( pVertexToReplace->iIndex );
	_ASSERT( find( pDestVertex->vlContainedFaceIndices.begin(), pDestVertex->vlContainedFaceIndices.end(), iFaceIndex ) == pDestVertex->vlContainedFaceIndices.end() );
	pDestVertex->vlContainedFaceIndices.push_back( iFaceIndex );
}

void CEtNavigationMeshGenerator::ComputeEdgeCostAtVertex( int iVertexIndex )
{
	S_VERTEX* pVertex = &m_vlNaviMeshVertices.at( iVertexIndex );
	if( pVertex->bRemoved )
		return;

	if( pVertex->setNeighborVertexIndices.empty() )
	{
		pVertex->pCollapse = NULL;
		pVertex->fCollapseCost = 0.01f;
		return;
	}

	pVertex->fCollapseCost = FLT_MAX;
	pVertex->pCollapse = NULL;

	if( pVertex->bBound )
		return;

	//if( 12 < (int)pVertex->vlContainedFaceIndices.size() )
	//	return;

	set<int>::iterator iter = pVertex->setNeighborVertexIndices.begin();
	for( iter; iter != pVertex->setNeighborVertexIndices.end(); ++iter )
	{
		S_VERTEX* pEdgeDestVertex = &m_vlNaviMeshVertices.at( *iter );
		if( pEdgeDestVertex->bRemoved )
			continue;

		if( iVertexIndex == pEdgeDestVertex->iIndex )
			continue;

		float fCost = ComputeEdgeCollapseCost( pVertex, pEdgeDestVertex );
		if( fCost < pVertex->fCollapseCost )
		{
			pVertex->fCollapseCost = fCost;
			pVertex->pCollapse = pEdgeDestVertex;
		}
	}
}

float CEtNavigationMeshGenerator::ComputeEdgeCollapseCost( S_VERTEX* pVertexStart, S_VERTEX* pVertexEnd )
{
	float fLengthSQ = 0.0f;

	
	fLengthSQ = float( ((pVertexEnd->iX-pVertexStart->iX)*(pVertexEnd->iX-pVertexStart->iX)) +
	  				   ((pVertexEnd->iY-pVertexStart->iY)*(pVertexEnd->iY-pVertexStart->iY)) );

	return fLengthSQ;
}

} // namespace EtNavigation