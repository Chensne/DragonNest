#pragma once

#include <vector>
#include "mtxlib.h"

using namespace std;

namespace EtNavigationMesh
{

struct S_RECT
{
	int iIndex;
	int iLeft;
	int iRight;
	int iBottom;
	int iHeight;
	int iAttribute;

	S_RECT() : iIndex( 0 ), iLeft( 0 ), iRight( 0 ), iBottom( 0 ), iHeight( 0 ), iAttribute( 0 ) {};
	S_RECT( int _index, int _left, int _right, int _bottom, int _height, int _attribute ) :
			iIndex( _index ), iLeft( _left ), iRight( _right ), iBottom( _bottom ), 
			iHeight( _height ), iAttribute( _attribute ) {};
};


struct S_FACE
{
	int aiVertexIndex[ 3 ];
	int iNaviRectID;
	bool bRemoved;

	S_FACE() : iNaviRectID( 0 ), bRemoved( false ) { SecureZeroMemory(aiVertexIndex, sizeof(aiVertexIndex)); }
	void Removed( void ) { bRemoved = true; };
};


struct S_VERTEX
{
	int iIndex;
	int iX;
	int iY;
	vector<int> vlContainedFaceIndices;
	set<int> setNeighborVertexIndices;
	S_VERTEX* pCollapse;
	float fCollapseCost;
	bool bRemoved;
	bool bBound;

	S_VERTEX() : iX( 0 ), iY( 0 ), iIndex( 0 ), pCollapse( NULL ), fCollapseCost( FLT_MAX ), bRemoved( false ), bBound( false ) {};
	S_VERTEX( int x, int y ) : iX(x), iY(y), iIndex( 0 ), pCollapse( NULL ), fCollapseCost( FLT_MAX ), bRemoved( false ), bBound( false ) {};

	int GetX( void ) const { return iX; };
	int GetY( void ) const { return iY; };
	void Set( int x, int y )
	{
		iX = x;
		iY = y;
	}

	void Removed( void ) 
	{ 
		bRemoved = true; 
		vlContainedFaceIndices.clear();
		setNeighborVertexIndices.clear();
		pCollapse = NULL;
		fCollapseCost = FLT_MAX;
	};

	bool operator == ( const S_VERTEX& rhs )
	{
		return ((iX == rhs.iX) && (iY == rhs.iY));
	}

	bool operator < ( const S_VERTEX& rhs ) const
	{
		if( iY == rhs.iY )
			return iX < rhs.iX;

		return iY < rhs.iY;
	}
};


class CEtNavigationMeshGenerator
{
private:
	enum
	{
		LB_TO_RB,
		RB_TO_RT,
		RT_TO_LT,
		LT_TO_LB,
	};

private:
	int m_iXBlockCount;
	int m_iYBlockCount;
	int m_iXAttributeCount;
	int m_iYAttributeCount;
	vector<unsigned char> m_vlMapAttributes;
	vector<int> m_vlNaviAttributes;
	
	vector<S_VERTEX> m_vlNaviMeshVertices;
	vector<S_FACE> m_vlNaviMeshFaces;
	vector<S_RECT> m_vlNaviRects;

	vector<vector3> m_vlResultVertices;
	vector<int> m_vlResultFaceIndices;

	int m_iNumUsingVertices;

private:
	char NormalizeAttribute( char cAttribute );
	void SetNaviAttributes( int iAttribute, int iXStart, int iYStart, int iXEnd, int iYEnd );
	void CollapseAttributeBlocks( void );
	void CollapseNaviRectsAndTriangulatePolygon( void );
	void ScanSplitVertex( int iScanDirection, int iXStart, int iYStart, 
						  int iXEnd, int iYEnd, int iRectWidth, int iRectHeight, 
						  int& iSplitWidth, int& iSplitHeight,
						  int& iXAttributeBefore, int& iYAttributeBefore,
						  int iCurRectNaviAttribute, /*OUT*/ vector<S_VERTEX>& vlNaviVertices );
	void TriangulatePolygonAndRegister( /*IN*/ const vector<S_VERTEX>& vlVertices, int iAttribute );
	void RegisterFace( S_VERTEX* apVertices, int iAttribute );
	void RemoveFace( int iFaceIndex );
	float Area( const vector<S_VERTEX>& contour );
	bool InsideTriangle( int Ax, int Ay, int Bx, int By, int Cx, int Cy, int Px, int Py );
	bool Snip( const vector<S_VERTEX>& contour,int u,int v,int w,int n,int *V );
	bool TriangulatePolygon( const vector<S_VERTEX>& contour, vector<S_VERTEX>& result );
	void ReorderFaceIndex( void );
	
	void PolygonReduction( void );
	S_VERTEX* CalcMinimumCostEdge( void );
	void CollapseEdge( S_VERTEX* pVertexToRemove );
	bool HasVertex( int iFaceIndex, S_VERTEX* pVertex );
	void RemoveVertex( S_VERTEX* pVertexToRemove );
	void ReplaceVertex( int iFaceIndex, S_VERTEX* pVertexToReplace, S_VERTEX* pDestVertex );
	void ComputeEdgeCostAtVertex( int iVertexIndex );
	float ComputeEdgeCollapseCost( S_VERTEX* pVertexStart, S_VERTEX* pVertexEnd );
	void MakeupNavigationData( int& iNumVertices, int& iNumFaces );

public:
	CEtNavigationMeshGenerator( void );
	~CEtNavigationMeshGenerator( void );

	void Initialize( int iX, int iY, char* pAttribute );
	void Generate( int &iNumVertices, int &iNumFaces );

	int getNumNavEvent( void ) { return 0; };
	int* getEventFaceBegin( int at, int &numFace ) { numFace = 0; return NULL; };

	int getNumNavFaceNormal( void ) { return int(m_vlResultFaceIndices.size()) / 3; };
	vector<vector3>& getVertexBegin( void ) { return m_vlResultVertices; };
	vector<int>& getFaceBegin( void ) { return m_vlResultFaceIndices; };

	inline int NaviAttrAt( int iX, int iY );

	
};

inline
int CEtNavigationMeshGenerator::NaviAttrAt( int iX, int iY )
{
	int iIndex = (iY*m_iXAttributeCount) + iX;
	if( 0 <= iIndex && iIndex < (int)m_vlNaviAttributes.size() )
		return m_vlNaviAttributes.at( iIndex );
	else
		return 1;
}

}	// namespace EtNavigation