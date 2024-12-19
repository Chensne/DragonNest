template< class T >
void TKdTree< T >::BuildTree()
{
	if( ( m_vecPrimitive.empty() ) || ( m_bIsBuild ) )
	{
		return;
	}

	int i, nTriangleCount;
	std::vector< SKdTreeEvent > vecXAxis, vecYAxis, vecZAxis;
	std::vector< SAABox > vecBoundingBox;

	nTriangleCount = ( int )m_vecPrimitive.size();
	vecXAxis.reserve( nTriangleCount * 2 );
	vecYAxis.reserve( nTriangleCount * 2 );
	vecZAxis.reserve( nTriangleCount * 2 );
	vecBoundingBox.reserve(  nTriangleCount );

	for( i = 0; i < nTriangleCount; i++ )
	{
		SAABox BoundingBox;

		m_vecPrimitive[ i ].GetBoundingBox( BoundingBox );
		vecBoundingBox.push_back( BoundingBox );

		if( BoundingBox.Min[ 0 ] != BoundingBox.Max[ 0 ] )
		{
			vecXAxis.push_back( SKdTreeEvent( BoundingBox.Min[ 0 ], SKdTreeEvent::STATE_IN, i ) );
			vecXAxis.push_back( SKdTreeEvent( BoundingBox.Max[ 0 ], SKdTreeEvent::STATE_OUT, i ) );
		}
		else
		{
			vecXAxis.push_back( SKdTreeEvent( BoundingBox.Max[ 0 ], SKdTreeEvent::STATE_ON, i ) );
		}

		if( BoundingBox.Min[ 1 ] != BoundingBox.Max[ 1 ] )
		{
			vecYAxis.push_back( SKdTreeEvent( BoundingBox.Min[ 1 ], SKdTreeEvent::STATE_IN, i ) );
			vecYAxis.push_back( SKdTreeEvent( BoundingBox.Max[ 1 ], SKdTreeEvent::STATE_OUT, i ) );
		}
		else
		{
			vecYAxis.push_back( SKdTreeEvent( BoundingBox.Max[ 1 ], SKdTreeEvent::STATE_ON, i ) );
		}

		if( BoundingBox.Min[ 2 ] != BoundingBox.Max[ 2 ] )
		{
			vecZAxis.push_back( SKdTreeEvent( BoundingBox.Min[ 2 ], SKdTreeEvent::STATE_IN, i ) );
			vecZAxis.push_back( SKdTreeEvent( BoundingBox.Max[ 2 ], SKdTreeEvent::STATE_OUT, i ) );
		}
		else
		{
			vecZAxis.push_back( SKdTreeEvent( BoundingBox.Max[ 2 ], SKdTreeEvent::STATE_ON, i ) );
		}

		m_BoundingBox.AddPoint( BoundingBox.Max );
		m_BoundingBox.AddPoint( BoundingBox.Min );
	}

	sort( vecXAxis.begin(), vecXAxis.end() );
	sort( vecYAxis.begin(), vecYAxis.end() );
	sort( vecZAxis.begin(), vecZAxis.end() );

	m_vecSplit.resize( 2 );
	m_vecLeaf.clear();

	BuildTree( 0, vecXAxis, vecYAxis, vecZAxis, m_BoundingBox, vecBoundingBox );
	m_bIsBuild = true;

//	GenerateMesh();
}

template< class T >
void TKdTree< T >::BuildTree( int nTreeIndex, std::vector< SKdTreeEvent > &vecXAxis, std::vector< SKdTreeEvent > &vecYAxis, std::vector< SKdTreeEvent > &vecZAxis, SAABox &BoundingBox, std::vector< SAABox > &vecBoundingBox )
{
	int i;
	float fMinSAH, fMinSplitPoint, fAxisRatio[ 3 ], fAxisSplitPoint[ 3 ];
	SKdTreeSplit::SPLIT_AXIS MinAxis = SKdTreeSplit::SPLIT_END;

	fAxisRatio[ 0 ] = CalcMinSAH( SKdTreeSplit::SPLIT_X, vecXAxis, BoundingBox, fAxisSplitPoint[ 0 ] );
	fAxisRatio[ 1 ] = CalcMinSAH( SKdTreeSplit::SPLIT_Y, vecYAxis, BoundingBox, fAxisSplitPoint[ 1 ] );
	fAxisRatio[ 2 ] = CalcMinSAH( SKdTreeSplit::SPLIT_Z, vecZAxis, BoundingBox, fAxisSplitPoint[ 2 ] );

	fMinSAH = 1.0f;

	fMinSplitPoint = 0.0f;
	for( i = 0; i < 3; i++ )
	{
		if( fAxisRatio[ i ] < fMinSAH )
		{
			fMinSAH = fAxisRatio[ i ];
			MinAxis = ( SKdTreeSplit::SPLIT_AXIS )i;
			fMinSplitPoint = fAxisSplitPoint[ i ];
		}
	}

	if( MinAxis != SKdTreeSplit::SPLIT_END )
	{
		int nChildIndex;
		std::vector< SKdTreeEvent > vecTempXAxis, vecTempYAxis, vecTempZAxis;
		std::vector< SKdTreeEvent >::iterator Iter;
		SAABox SplitBox;

		nChildIndex = ( int )m_vecSplit.size();

		m_vecSplit[ nTreeIndex ].SetAxis( MinAxis );
		m_vecSplit[ nTreeIndex ].SetChildren( nChildIndex );
		m_vecSplit[ nTreeIndex ].m_fSplitPoint = fMinSplitPoint;

		vecTempXAxis.reserve( vecXAxis.size() );
		vecTempYAxis.reserve( vecYAxis.size() );
		vecTempZAxis.reserve( vecZAxis.size() );

		m_vecSplit.resize( m_vecSplit.size() + 2 );

		for( Iter = vecXAxis.begin(); Iter != vecXAxis.end(); Iter++ )
		{
			const SAABox& TriBoundingBox = vecBoundingBox[ Iter->GetOffset() ];

			if( TriBoundingBox.Min[ MinAxis ] < fMinSplitPoint )
			{ 
				vecTempXAxis.push_back( *Iter );
			}
			else if( TriBoundingBox.Max[ MinAxis ] == fMinSplitPoint )	
			{ 
				vecTempXAxis.push_back( *Iter );	
			}
		}
		for( Iter = vecYAxis.begin(); Iter != vecYAxis.end(); Iter++ )
		{
			const SAABox& TriBoundingBox = vecBoundingBox[ Iter->GetOffset() ];

			if( TriBoundingBox.Min[ MinAxis ] < fMinSplitPoint )
			{ 
				vecTempYAxis.push_back( *Iter );
			}
			else if( TriBoundingBox.Max[ MinAxis ] == fMinSplitPoint )	
			{ 
				vecTempYAxis.push_back( *Iter );	
			}
		}
		for( Iter = vecZAxis.begin(); Iter != vecZAxis.end(); Iter++ )
		{
			const SAABox& TriBoundingBox = vecBoundingBox[ Iter->GetOffset() ];

			if( TriBoundingBox.Min[ MinAxis ] < fMinSplitPoint )
			{ 
				vecTempZAxis.push_back( *Iter );
			}
			else if( TriBoundingBox.Max[ MinAxis ] == fMinSplitPoint )	
			{ 
				vecTempZAxis.push_back( *Iter );	
			}
		}
		SplitBox = BoundingBox;
		SplitBox.Max[ MinAxis ] = fMinSplitPoint;
		BuildTree( nChildIndex, vecTempXAxis, vecTempYAxis, vecTempZAxis, SplitBox, vecBoundingBox );

		vecTempXAxis.clear(); 
		vecTempYAxis.clear(); 
		vecTempZAxis.clear();
		for( Iter = vecXAxis.begin(); Iter != vecXAxis.end(); Iter++ )
		{
			const SAABox& TriBoundingBox = vecBoundingBox[ Iter->GetOffset() ];

			if( fMinSplitPoint < TriBoundingBox.Max[ MinAxis ] )
			{ 
				vecTempXAxis.push_back(*Iter);	
			}
			else if( fMinSplitPoint == TriBoundingBox.Min[ MinAxis ] )
			{ 
				vecTempXAxis.push_back(*Iter);	
			}
		}
		for( Iter = vecYAxis.begin(); Iter != vecYAxis.end(); Iter++ )
		{
			const SAABox& TriBoundingBox = vecBoundingBox[ Iter->GetOffset() ];

			if( fMinSplitPoint < TriBoundingBox.Max[ MinAxis ] )
			{ 
				vecTempYAxis.push_back(*Iter);	
			}
			else if( fMinSplitPoint == TriBoundingBox.Min[ MinAxis ] )
			{ 
				vecTempYAxis.push_back(*Iter);	
			}
		}
		for( Iter = vecZAxis.begin(); Iter != vecZAxis.end(); Iter++ )
		{
			const SAABox& TriBoundingBox = vecBoundingBox[ Iter->GetOffset() ];

			if( fMinSplitPoint < TriBoundingBox.Max[ MinAxis ] )
			{ 
				vecTempZAxis.push_back(*Iter);	
			}
			else if( fMinSplitPoint == TriBoundingBox.Min[ MinAxis ] )
			{ 
				vecTempZAxis.push_back(*Iter);	
			}
		}
		SplitBox = BoundingBox;
		SplitBox.Min[ MinAxis ] = fMinSplitPoint;
		BuildTree( nChildIndex + 1, vecTempXAxis, vecTempYAxis, vecTempZAxis, SplitBox, vecBoundingBox );
	}
	else
	{
		std::vector< int > vecOffset;
		std::vector< SKdTreeEvent >::iterator Iter;

		for( Iter = vecXAxis.begin(); Iter != vecXAxis.end(); Iter++ )
		{
			if( ( BoundingBox.Min[ 0 ] <= Iter->m_fSplitPoint ) && ( Iter->m_fSplitPoint <= BoundingBox.Max[ 0 ] ) )
			{ 
				vecOffset.push_back( Iter->GetOffset() );
			}
			else
			{
				SAABox PrimBoundingBox;
				m_vecPrimitive[ Iter->GetOffset() ].GetBoundingBox( PrimBoundingBox );
				if( ( PrimBoundingBox.Min[ 0 ] <= BoundingBox.Min[ 0 ] ) && ( BoundingBox.Min[ 0 ] <= PrimBoundingBox.Max[ 0 ] ) )
				{
					vecOffset.push_back( Iter->GetOffset() );
				}
			}
		}
		for( Iter = vecYAxis.begin(); Iter != vecYAxis.end(); Iter++ )
		{
			if( ( BoundingBox.Min[ 1 ] <= Iter->m_fSplitPoint ) && ( Iter->m_fSplitPoint <= BoundingBox.Max[ 1 ] ) )
			{ 
				vecOffset.push_back( Iter->GetOffset() );
			}
			else
			{
				SAABox PrimBoundingBox;
				m_vecPrimitive[ Iter->GetOffset() ].GetBoundingBox( PrimBoundingBox );
				if( ( PrimBoundingBox.Min[ 1 ] <= BoundingBox.Min[ 1 ] ) && ( BoundingBox.Min[ 1 ] <= PrimBoundingBox.Max[ 1 ] ) )
				{
					vecOffset.push_back( Iter->GetOffset() );
				}
			}
		}
		for( Iter = vecZAxis.begin(); Iter != vecZAxis.end(); Iter++ )
		{
			if( ( BoundingBox.Min[ 2 ] <= Iter->m_fSplitPoint ) && ( Iter->m_fSplitPoint <= BoundingBox.Max[ 2 ] ) )
			{ 
				vecOffset.push_back( Iter->GetOffset() );
			}
			else
			{
				SAABox PrimBoundingBox;
				m_vecPrimitive[ Iter->GetOffset() ].GetBoundingBox( PrimBoundingBox );
				if( ( PrimBoundingBox.Min[ 2 ] <= BoundingBox.Min[ 2 ] ) && ( BoundingBox.Min[ 2 ] <= PrimBoundingBox.Max[ 2 ] ) )
				{
					vecOffset.push_back( Iter->GetOffset() );
				}
			}
		}

		int nCount;

		std::sort( vecOffset.begin(), vecOffset.end() );
		std::vector< int >::iterator End = std::unique( vecOffset.begin(), vecOffset.end() );
		nCount = ( int )( End - vecOffset.begin() );

		m_vecSplit[ nTreeIndex ].SetAxis( SKdTreeSplit::SPLIT_END );
		m_vecSplit[ nTreeIndex ].SetChildren( ( int )m_vecLeaf.size() );
		m_vecSplit[ nTreeIndex ].SetChildCount( nCount );

		m_vecLeaf.insert( m_vecLeaf.end(), vecOffset.begin(), End );
	}
}

template< class T >
float TKdTree< T >::CalcMinSAH( SKdTreeSplit::SPLIT_AXIS Axis, std::vector< SKdTreeEvent > &vecSplit, SAABox &BoundingBox, float &fMinSplit )
{
	float fMinRatio, fCurSAH;
	int nLeft, nRight;
	std::vector< SKdTreeEvent >::iterator Begin, End, Iter;

	fMinRatio = 1.0f;
	nLeft = 0;
	nRight = 0;
	for( Begin = vecSplit.begin(); Begin != vecSplit.end() && Begin->m_fSplitPoint < BoundingBox.Min[ Axis ]; Begin++ )
	{
		if( Begin->GetState() == SKdTreeEvent::STATE_OUT )	
		{ 
			nRight--;	
		}
		if( Begin->GetState() == SKdTreeEvent::STATE_IN )
		{ 
			nRight++;	
		}
	}
	nLeft = nRight;
	for( ; Begin != vecSplit.end() && Begin->m_fSplitPoint == BoundingBox.Min[ Axis ]; Begin++ )
	{
		if( Begin->GetState() == SKdTreeEvent::STATE_OUT )	
		{ 
			nLeft--; 
			nRight--;	
		}
		if( Begin->GetState() == SKdTreeEvent::STATE_ON )		
		{ 
			nLeft++;		
		}
		if( Begin->GetState() == SKdTreeEvent::STATE_IN )		
		{ 
			nLeft++; 
			nRight++;	
		}
	}
	for( End = Begin; End != vecSplit.end() && End->m_fSplitPoint < BoundingBox.Max[ Axis ]; End++ )
	{
		if( ( End->GetState() == SKdTreeEvent::STATE_ON ) || ( End->GetState() == SKdTreeEvent::STATE_IN ) )
		{ 
			nRight++;		
		}
	}
	for( Iter = End; Iter != vecSplit.end() && Iter->m_fSplitPoint == BoundingBox.Max[ Axis ]; Iter++ )
	{
		if( Iter->GetState() == SKdTreeEvent::STATE_ON )		
		{ 
			nRight++;		
		}
	}

	fCurSAH = CalcSAH( nLeft, 0, nRight, 0, BoundingBox.Max[ Axis ] - BoundingBox.Min[ Axis ] );
	for( Iter = Begin; Iter != End; )
	{
		int nCenter, nTemp;
		float fSplitPoint, fRatio;

		nCenter = 0;
		nTemp = 0;
		fSplitPoint = Iter->m_fSplitPoint;

		ASSERT(BoundingBox.Min[ Axis ] < fSplitPoint && fSplitPoint < BoundingBox.Max[ Axis ] );

		for( ; Iter != End && Iter->m_fSplitPoint == fSplitPoint; Iter++ )
		{
			if( Iter->GetState() == SKdTreeEvent::STATE_OUT ) 
			{
				nRight--;
			}
			if( Iter->GetState() == SKdTreeEvent::STATE_ON )
			{
				nCenter++;
			}
			if( Iter->GetState() == SKdTreeEvent::STATE_IN ) 
			{
				nTemp++;
			}
		}
		fRatio = CalcSAH( nLeft, nCenter, nRight, fSplitPoint - BoundingBox.Min[ Axis ], BoundingBox.Max[ Axis ] - fSplitPoint ) / fCurSAH;
		if( fRatio < fMinRatio)
		{
			fMinRatio = fRatio;
			fMinSplit = fSplitPoint;
		}
		nLeft += nCenter + nTemp;
	}

	return fMinRatio;
}

template< class T >
float TKdTree< T >::CalcSAH( int nLeft, int nCenter, int nRight, float fLeftPoint, float fRightPoint)
{
	return 220 + 30 * min( fLeftPoint * ( nLeft + nCenter ) + fRightPoint * nRight, fLeftPoint * nLeft + fRightPoint * ( nRight + nCenter ) );
}

template< class T >
void TKdTree< T >::LoadTree( CStream *pStream )
{
	int i, nCount;
	SKdTreeFileHeader Header;

	SAFE_DELETE_VEC( m_vecPrimitive );
	SAFE_DELETE_VEC( m_vecSplit );
	SAFE_DELETE_VEC( m_vecLeaf );

	pStream->Read( &Header, sizeof( SKdTreeFileHeader ) );
	pStream->Seek( KDTREE_HEADER_RESERVED, SEEK_CUR );

	if( strstr( Header.szHeaderString, KDTREE_FILE_STRING ) == NULL ) return;

	pStream->Read( &nCount, sizeof( int ) );
	if( nCount )
	{
		m_vecPrimitive.resize( nCount );
	}
	for( i = 0; i < nCount; i++ )
	{
		m_vecPrimitive[ i ].Load( pStream );
	}

	pStream->Read( &nCount, sizeof( int ) );
	if( nCount )
	{
		m_vecSplit.resize( nCount );
		pStream->Read( &m_vecSplit[ 0 ], sizeof( SKdTreeSplit ) * nCount );
	}

	pStream->Read( &nCount, sizeof( int ) );
	if( nCount )
	{
		m_vecLeaf.resize( nCount );
		pStream->Read( &m_vecLeaf[ 0 ], sizeof( int ) * nCount );
	}

	pStream->Read( &m_BoundingBox, sizeof( SAABox ) );

	m_bIsBuild = true;
}

template< class T >
bool TKdTree< T >::SaveTree( const char *pFileName )
{
	CFileStream Stream( pFileName, CFileStream::OPEN_WRITE );

	if( !Stream.IsValid() )
	{
		return false;
	}

	SaveTree( &Stream );

	return true;
}

template< class T >
void TKdTree< T >::SaveTree( CStream *pStream )
{
	int i, nCount;
	char szReserved[ KDTREE_HEADER_RESERVED ];
	SKdTreeFileHeader Header;

	memset( &Header, 0, sizeof( SKdTreeFileHeader ) );
	Header.nVersion = KDTREE_FILE_VERSION;
	strcpy_s( Header.szHeaderString, 256, KDTREE_FILE_STRING );
	memset( szReserved, 0, KDTREE_HEADER_RESERVED );

	pStream->Write( &Header, sizeof( SKdTreeFileHeader ) );
	pStream->Write( szReserved, KDTREE_HEADER_RESERVED );

	nCount = ( int )m_vecPrimitive.size();
	pStream->Write( &nCount, sizeof( int ) );
	for( i = 0; i < nCount; i++ )
	{
		m_vecPrimitive[ i ].Save( pStream );
	}

	nCount = ( int )m_vecSplit.size();
	pStream->Write( &nCount, sizeof( int ) );
	if( nCount )
	{
		pStream->Write( &m_vecSplit[ 0 ], sizeof( SKdTreeSplit ) * nCount );
	}

	nCount = ( int )m_vecLeaf.size();
	pStream->Write( &nCount, sizeof( int ) );
	if( nCount )
	{
		pStream->Write( &m_vecLeaf[ 0 ], sizeof( int ) * nCount );
	}

	pStream->Write( &m_BoundingBox, sizeof( SAABox ) );
}

template< class T >
void TKdTree< T >::Pick( SAABox &Box, std::vector< T * > &vecPrimitive )
{
	if( m_vecPrimitive.empty() )
	{
		return;
	}

//	vecCollisionResult.clear();

	int nTreeIndex;
	SKdTreeSplit *pSplitList;
	T *pTriangleList;
	int nSearchStack[ 100 ], nCurStack;

	pSplitList = &m_vecSplit[ 0 ];
	pTriangleList = &m_vecPrimitive[ 0 ];
	nTreeIndex = 0;
	nCurStack = 0;
	while( 1 )
	{
		SKdTreeSplit::SPLIT_AXIS Axis;
		int nChildIndex;

		Axis = pSplitList[ nTreeIndex ].GetAxis();
		nChildIndex = pSplitList[ nTreeIndex ].GetChildren();
		if( Axis == SKdTreeSplit::SPLIT_END )
		{
			int i;
			int *pLeafList;

			pLeafList = &m_vecLeaf[ 0 ];
			pLeafList += nChildIndex;

			for( i = 0; i < pSplitList[ nTreeIndex ].m_nChildCount; i++ )
			{
				// Valid üũ
				if( ( int )m_vecPrimitive.size() > pLeafList[i] )
				{
					if( std::find( vecPrimitive.begin(), vecPrimitive.end(), &m_vecPrimitive[ pLeafList[ i ] ] ) == vecPrimitive.end() )
					{
						vecPrimitive.push_back( &m_vecPrimitive[ pLeafList[ i ] ] );
					}
				}
//				vecCollisionResult.push_back( pLeafList[ i ] );
			}
			if( nCurStack )

			{
				nCurStack--;
				nTreeIndex = nSearchStack[ nCurStack ];
			}
			else
			{
//				GenerateCollsionMesh();
				return;
			}
		}
		else
		{
			float fSplitPoint;

			fSplitPoint = pSplitList[ nTreeIndex ].m_fSplitPoint;
			if( Box.Max[ Axis ] < fSplitPoint )
			{
				nTreeIndex = nChildIndex;
			}
			else if( Box.Min[ Axis ] > fSplitPoint )
			{
				nTreeIndex = nChildIndex + 1;
			}
			else
			{
				nSearchStack[ nCurStack ] = nChildIndex + 1;
				nCurStack++;
				nTreeIndex = nChildIndex;
			}
		}
	}
}

/*template< class T >
void TKdTree< T >::GenerateCollsionMesh()
{
	int i;

	for( i = 0; i < ( int )m_vecTriVertex.size(); i++ )
	{
		m_vecTriVertex[ i ].dwColor = 0xffff0000;
	}
	for( i = 0; i < ( int )vecCollisionResult.size(); i++ )
	{
		m_vecTriVertex[ vecCollisionResult[ i ] * 3 ].dwColor = 0xff0000ff;
		m_vecTriVertex[ vecCollisionResult[ i ] * 3 + 1 ].dwColor = 0xff0000ff;
		m_vecTriVertex[ vecCollisionResult[ i ] * 3 + 2 ].dwColor = 0xff0000ff;
	}
}

template< class T >
void TKdTree< T >::GenerateMesh()
{
	m_vecVertex.clear();
	m_vecTriVertex.clear();

	AddBoundingBox( 0, m_BoundingBox );

	int i;

	STreeDrawVertex DrawVertex;
	DrawVertex.dwColor = 0xffff0000;
	for( i = 0; i < ( int )m_vecPrimitive.size(); i++ )
	{
		DrawVertex.Vertex = m_vecPrimitive[ i ].vPoint[ 0 ];
		m_vecTriVertex.push_back( DrawVertex );
		DrawVertex.Vertex = m_vecPrimitive[ i ].vPoint[ 1 ] + m_vecPrimitive[ i ].vPoint[ 0 ];
		m_vecTriVertex.push_back( DrawVertex );
		DrawVertex.Vertex = m_vecPrimitive[ i ].vPoint[ 2 ] + m_vecPrimitive[ i ].vPoint[ 0 ];
		m_vecTriVertex.push_back( DrawVertex );
	}
}

template< class T >
void TKdTree< T >::AddBoundingBox( int nTreeIndex, SAABox &BoundingBox )
{
	SAABox SplitBox;
	SKdTreeSplit::SPLIT_AXIS SplitAxis;

	STreeDrawVertex DrawVertex;
	std::vector< EtVector3 > vecVertex;
	BoundingBox.GetVertices( vecVertex );

	int i;
	bool bFind = false;
	for( i = 0; i < ( int )vecCollisionResult.size(); i++ )
	{
		if( nTreeIndex == vecCollisionResult[ i ] )
		{
			bFind = true;
			break;
		}
	}
	if( bFind )
	{
		DrawVertex.dwColor = 0xff0000ff;
	}
	else
	{
		DrawVertex.dwColor = 0xffffffff;
	}
	
	DrawVertex.Vertex = vecVertex[ 0 ];
	m_vecVertex.push_back( DrawVertex );
	DrawVertex.Vertex = vecVertex[ 1 ];
	m_vecVertex.push_back( DrawVertex );
	DrawVertex.Vertex = vecVertex[ 0 ];
	m_vecVertex.push_back( DrawVertex );
	DrawVertex.Vertex = vecVertex[ 2 ];
	m_vecVertex.push_back( DrawVertex );
	DrawVertex.Vertex = vecVertex[ 0 ];
	m_vecVertex.push_back( DrawVertex );
	DrawVertex.Vertex = vecVertex[ 4 ];
	m_vecVertex.push_back( DrawVertex );
	DrawVertex.Vertex = vecVertex[ 7 ];
	m_vecVertex.push_back( DrawVertex );
	DrawVertex.Vertex = vecVertex[ 3 ];
	m_vecVertex.push_back( DrawVertex );
	DrawVertex.Vertex = vecVertex[ 7 ];
	m_vecVertex.push_back( DrawVertex );
	DrawVertex.Vertex = vecVertex[ 5 ];
	m_vecVertex.push_back( DrawVertex );
	DrawVertex.Vertex = vecVertex[ 7 ];
	m_vecVertex.push_back( DrawVertex );
	DrawVertex.Vertex = vecVertex[ 6 ];
	m_vecVertex.push_back( DrawVertex );

	SplitAxis = m_vecSplit[ nTreeIndex ].GetAxis();
	if( SplitAxis == SKdTreeSplit::SPLIT_END )
	{
		return;
	}
	SplitBox = BoundingBox;
	SplitBox.Max[ SplitAxis ] = m_vecSplit[ nTreeIndex ].m_fSplitPoint;
	AddBoundingBox( m_vecSplit[ nTreeIndex ].GetChildren(), SplitBox );
	SplitBox = BoundingBox;
	SplitBox.Min[ SplitAxis ] = m_vecSplit[ nTreeIndex ].m_fSplitPoint;
	AddBoundingBox( m_vecSplit[ nTreeIndex ].GetChildren() + 1, SplitBox );
}
*/