#include "StdAfx.h"
#include "EtMaze.h"
#include "EtMazeMask.h"
#include "EtRandomDungeon.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif


CEtMaze::CEtMaze( int nWidth, int nHeight, int nLevel, int nSeed, CEtMazeMask *pMask )
{
	m_Size.nX = nWidth;
	m_Size.nY = nHeight;
	m_Size.nZ = nLevel;
	m_pMask = pMask;

	/*
	if( nSeed > 0 ) m_nSeed = nSeed;
	else m_nSeed = rand()%LONG_MAX;

	srand( m_nSeed );
	*/

	m_pMaze = new int **[nWidth];
	for( int i=0; i<nWidth; i++ ) {
		m_pMaze[i] = new int *[nHeight];
		for( int j=0; j<nHeight; j++ ) {
			m_pMaze[i][j] = new int[nLevel];
			memset( m_pMaze[i][j], 0, nLevel * sizeof(int) );
		}
	}

}

CEtMaze::~CEtMaze()
{
	for( int i=0; i<m_Size.nX; i++ ) {
		for( int j=0; j<m_Size.nY; j++ ) {
			SAFE_DELETEA( m_pMaze[i][j] );
		}
		SAFE_DELETEA( m_pMaze[i] );
	}
	SAFE_DELETEA( m_pMaze );

	SAFE_DELETE_VEC( m_VecSolveList );
}

void CEtMaze::Generation( int nRandomness )
{
	unsigned long nRemainCount = 0;
	int nAllDir = MD_WEST | MD_EAST | MD_NORTH | MD_SOUTH | MD_UP | MD_DOWN;
	int nAccumDir = MD_NONE;
	int nCurDir = MD_NONE;
	int nLastDir = MD_NONE;
	bool bRandomSelect;
	int nStraightStretch = 0;

	for( int i=0; i<m_Size.nX; i++ ) {
		for( int j=0; j<m_Size.nY; j++ ) {
			for( int k=0; k<m_Size.nZ; k++ ) {
				if( !m_pMask->IsCheckMask( i, j, k ) ) nRemainCount++;
			}
		}
	}
//	nRemainCount *= m_Size.nZ;
	nRemainCount--;

	IntVec3 Pos;
	do {
		Pos.nX = rand() % m_Size.nX;
		Pos.nY = rand() % m_Size.nY;
		Pos.nZ = rand() % m_Size.nZ;
	} while( m_pMask->IsCheckMask( Pos.nX, Pos.nY, Pos.nZ ) );

	int nUpDownCount = 10;
	while( nRemainCount > 0 ) {
		if( nAccumDir == nAllDir ) {
			do {
				Pos.nX = rand() % m_Size.nX;
				Pos.nY = rand() % m_Size.nY;
				Pos.nZ = rand() % m_Size.nZ;
			} while( m_pMaze[Pos.nX][Pos.nY][Pos.nZ] == 0 );
			nAccumDir = m_pMaze[Pos.nX][Pos.nY][Pos.nZ];
		}

		if( Pos.nX < 1 ) nAccumDir |= MD_WEST;
		if( Pos.nX + 1 >= m_Size.nX ) nAccumDir |= MD_EAST;
		if( Pos.nY < 1 ) nAccumDir |= MD_NORTH;
		if( Pos.nY + 1 >= m_Size.nY ) nAccumDir |= MD_SOUTH;
		if( Pos.nZ < 1 ) nAccumDir |= MD_DOWN;
		if( Pos.nZ + 1 >= m_Size.nZ ) nAccumDir |= MD_UP;

		bRandomSelect = false;

		if( rand() % 100 < nRandomness ) bRandomSelect = true;
		else {
			switch( nLastDir ) {
				case MD_NORTH:
					if( ( nStraightStretch < ( m_Size.nY >> 1 ) ) && ( Pos.nY > 0 ) && ( m_pMaze[Pos.nX][Pos.nY-1][Pos.nZ] == 0 ) && ( !m_pMask->IsCheckMask( Pos.nX, Pos.nY-1, Pos.nZ ) ) )
						nCurDir = nLastDir;
					else bRandomSelect = true;
					break;
				case MD_SOUTH:
					if( ( nStraightStretch < ( m_Size.nY >> 1 ) ) && ( Pos.nY+1 < m_Size.nY ) && ( m_pMaze[Pos.nX][Pos.nY+1][Pos.nZ] == 0 ) && ( !m_pMask->IsCheckMask( Pos.nX, Pos.nY+1, Pos.nZ ) ) )
						nCurDir = nLastDir;
					else bRandomSelect = true;
					break;
				case MD_WEST:
					if( ( nStraightStretch < ( m_Size.nX >> 1 ) ) && ( Pos.nX > 0 ) && ( m_pMaze[Pos.nX-1][Pos.nY][Pos.nZ] == 0 ) && ( !m_pMask->IsCheckMask( Pos.nX-1, Pos.nY, Pos.nZ ) ) )
						nCurDir = nLastDir;
					else bRandomSelect = true;
					break;
				case MD_EAST:
					if( ( nStraightStretch < ( m_Size.nX >> 1 ) ) && ( Pos.nX+1 < m_Size.nX ) && ( m_pMaze[Pos.nX+1][Pos.nY][Pos.nZ] == 0 ) && ( !m_pMask->IsCheckMask( Pos.nX+1, Pos.nY, Pos.nZ ) ) )
						nCurDir = nLastDir;
					else bRandomSelect = true;
					break;
				case MD_UP:
					/*
					if( ( nStraightStretch < ( m_Size.nZ >> 1 ) ) && ( Pos.nZ > 0 ) && ( m_pMaze[Pos.nX][Pos.nY][Pos.nZ-1] == 0 ) )
						nCurDir = nLastDir;
					else bRandomSelect = true;
					*/
					bRandomSelect = true;
					break;
				case MD_DOWN:
					/*
					if( ( nStraightStretch < ( m_Size.nZ >> 1 ) ) && ( Pos.nZ+1 < m_Size.nZ ) && ( m_pMaze[Pos.nX][Pos.nY][Pos.nZ+1] == 0 ) )
						nCurDir = nLastDir;
					else bRandomSelect = true;
					*/
					bRandomSelect = true;
					break;
				default:
					bRandomSelect = true;
			}
		}

		if( bRandomSelect ) {
			nStraightStretch = 0;
			nCurDir = 0;

			while( ( nCurDir == MD_NONE ) || ( nAccumDir & nCurDir ) ) {
				IntVec3 Temp;
				Temp = Pos;

				int nRandomSeed;
				if( nLastDir == MD_UP || nLastDir == MD_DOWN || 
					m_pMaze[Pos.nX][Pos.nY][Pos.nZ] & MD_UP || m_pMaze[Pos.nX][Pos.nY][Pos.nZ] & MD_DOWN ) {
					nRandomSeed = rand()%4;
					nAccumDir |= ( MD_UP | MD_DOWN );
				}
				else {
					nRandomSeed = rand()%6;
				}

				switch( nRandomSeed ) {
					case 0: if( Pos.nY > 0 ) { nCurDir = MD_NORTH; Temp.nY--; } else { nAccumDir |= MD_NORTH; } break;
					case 1: if( Pos.nY+1 < m_Size.nY ) { nCurDir = MD_SOUTH; Temp.nY++; } else { nAccumDir |= MD_SOUTH; }  break;
					case 2: if( Pos.nX > 0 ) { nCurDir = MD_WEST; Temp.nX--; } else { nAccumDir |= MD_WEST; }  break;
					case 3: if( Pos.nX+1 < m_Size.nX ) { nCurDir = MD_EAST; Temp.nX++; } else { nAccumDir |= MD_EAST; }  break;
					case 4: if( Pos.nZ > 0 ) { nCurDir = MD_DOWN; Temp.nZ--; } else { nAccumDir |= MD_DOWN; }  break;
					case 5: if( Pos.nZ+1 < m_Size.nZ ) { nCurDir = MD_UP; Temp.nZ++; } else { nAccumDir |= MD_UP; }  break;
				}
				if( ( m_pMask->IsCheckMask( Temp.nX, Temp.nY, Temp.nZ ) ) || ( m_pMaze[Temp.nX][Temp.nY][Temp.nZ] != 0 ) ) {
					nAccumDir |= nCurDir;
					if( nAccumDir == nAllDir ) break;
					nCurDir = MD_NONE;
				}
			}
		} 
		else nStraightStretch++;

		if( nAccumDir == nAllDir ) continue;

		nLastDir = nCurDir;

		m_pMaze[Pos.nX][Pos.nY][Pos.nZ] |= nCurDir;
		switch( nCurDir ) {
			case MD_NORTH: Pos.nY--; nCurDir = MD_SOUTH; break;
			case MD_SOUTH: Pos.nY++; nCurDir = MD_NORTH; break;
			case MD_WEST: Pos.nX--; nCurDir = MD_EAST; break;
			case MD_EAST: Pos.nX++; nCurDir = MD_WEST; break;
			case MD_DOWN: Pos.nZ--; nCurDir = MD_UP; break;
			case MD_UP: Pos.nZ++; nCurDir = MD_DOWN; break;
		}
		m_pMaze[Pos.nX][Pos.nY][Pos.nZ] |= nCurDir;
		nAccumDir = m_pMaze[Pos.nX][Pos.nY][Pos.nZ];

		nRemainCount--;
	}
}

void CEtMaze::GenerationBySingleLevel( int nRandomness, int nLevel )
{
	unsigned long nRemainCount = 0;
	int nAllDir = MD_WEST | MD_EAST | MD_NORTH | MD_SOUTH;
	int nAccumDir = MD_NONE;
	int nCurDir = MD_NONE;
	int nLastDir = MD_NONE;
	bool bRandomSelect;
	int nStraightStretch = 0;

	for( int i=0; i<m_Size.nX; i++ ) {
		for( int j=0; j<m_Size.nY; j++ ) {
			if( !m_pMask->IsCheckMask( i, j, nLevel ) ) nRemainCount++;
		}
	}
	nRemainCount--;

	IntVec3 Pos;
	do {
		Pos.nX = rand() % m_Size.nX;
		Pos.nY = rand() % m_Size.nY;
		Pos.nZ = nLevel;
	} while( m_pMask->IsCheckMask( Pos.nX, Pos.nY, nLevel ) );

	int nUpDownCount = 10;
	while( nRemainCount > 0 ) {
		if( nAccumDir == nAllDir ) {
			do {
				Pos.nX = rand() % m_Size.nX;
				Pos.nY = rand() % m_Size.nY;
				Pos.nZ = nLevel;
			} while( m_pMaze[Pos.nX][Pos.nY][Pos.nZ] == 0 );
			nAccumDir = m_pMaze[Pos.nX][Pos.nY][Pos.nZ];
		}

		if( Pos.nX < 1 ) nAccumDir |= MD_WEST;
		if( Pos.nX + 1 >= m_Size.nX ) nAccumDir |= MD_EAST;
		if( Pos.nY < 1 ) nAccumDir |= MD_NORTH;
		if( Pos.nY + 1 >= m_Size.nY ) nAccumDir |= MD_SOUTH;

		bRandomSelect = false;

		if( rand() % 100 < nRandomness ) bRandomSelect = true;
		else {
			switch( nLastDir ) {
				case MD_NORTH:
					if( ( nStraightStretch < ( m_Size.nY >> 1 ) ) && ( Pos.nY > 0 ) && ( m_pMaze[Pos.nX][Pos.nY-1][Pos.nZ] == 0 ) && ( !m_pMask->IsCheckMask( Pos.nX, Pos.nY-1, Pos.nZ ) ) )
						nCurDir = nLastDir;
					else bRandomSelect = true;
					break;
				case MD_SOUTH:
					if( ( nStraightStretch < ( m_Size.nY >> 1 ) ) && ( Pos.nY+1 < m_Size.nY ) && ( m_pMaze[Pos.nX][Pos.nY+1][Pos.nZ] == 0 ) && ( !m_pMask->IsCheckMask( Pos.nX, Pos.nY+1, Pos.nZ ) ) )
						nCurDir = nLastDir;
					else bRandomSelect = true;
					break;
				case MD_WEST:
					if( ( nStraightStretch < ( m_Size.nX >> 1 ) ) && ( Pos.nX > 0 ) && ( m_pMaze[Pos.nX-1][Pos.nY][Pos.nZ] == 0 ) && ( !m_pMask->IsCheckMask( Pos.nX-1, Pos.nY, Pos.nZ ) ) )
						nCurDir = nLastDir;
					else bRandomSelect = true;
					break;
				case MD_EAST:
					if( ( nStraightStretch < ( m_Size.nX >> 1 ) ) && ( Pos.nX+1 < m_Size.nX ) && ( m_pMaze[Pos.nX+1][Pos.nY][Pos.nZ] == 0 ) && ( !m_pMask->IsCheckMask( Pos.nX+1, Pos.nY, Pos.nZ ) ) )
						nCurDir = nLastDir;
					else bRandomSelect = true;
					break;
				default:
					bRandomSelect = true;
			}
		}

		if( bRandomSelect ) {
			nStraightStretch = 0;
			nCurDir = 0;

			while( ( nCurDir == MD_NONE ) || ( nAccumDir & nCurDir ) ) {
				IntVec3 Temp;
				Temp = Pos;

				switch( rand()%4 ) {
					case 0: if( Pos.nY > 0 ) { nCurDir = MD_NORTH; Temp.nY--; } else { nAccumDir |= MD_NORTH; } break;
					case 1: if( Pos.nY+1 < m_Size.nY ) { nCurDir = MD_SOUTH; Temp.nY++; } else { nAccumDir |= MD_SOUTH; }  break;
					case 2: if( Pos.nX > 0 ) { nCurDir = MD_WEST; Temp.nX--; } else { nAccumDir |= MD_WEST; }  break;
					case 3: if( Pos.nX+1 < m_Size.nX ) { nCurDir = MD_EAST; Temp.nX++; } else { nAccumDir |= MD_EAST; }  break;
				}
				if( ( m_pMask->IsCheckMask( Temp.nX, Temp.nY, Temp.nZ ) ) || ( m_pMaze[Temp.nX][Temp.nY][Temp.nZ] != 0 ) ) {
					nAccumDir |= nCurDir;
					if( nAccumDir == nAllDir ) break;
					nCurDir = MD_NONE;
				}
			}
		} 
		else nStraightStretch++;

		if( nAccumDir == nAllDir ) continue;

		nLastDir = nCurDir;

		m_pMaze[Pos.nX][Pos.nY][Pos.nZ] |= nCurDir;
		switch( nCurDir ) {
			case MD_NORTH: Pos.nY--; nCurDir = MD_SOUTH; break;
			case MD_SOUTH: Pos.nY++; nCurDir = MD_NORTH; break;
			case MD_WEST: Pos.nX--; nCurDir = MD_EAST; break;
			case MD_EAST: Pos.nX++; nCurDir = MD_WEST; break;
		}

		m_pMaze[Pos.nX][Pos.nY][Pos.nZ] |= nCurDir;
		nAccumDir = m_pMaze[Pos.nX][Pos.nY][Pos.nZ];

		nRemainCount--;
	}
}

void CEtMaze::Sparsify( int nAmount )
{
	for( int i=0; i<nAmount; i++ ) {
		for( int x=0; x<m_Size.nX; x++ ) {
			for( int y=0; y<m_Size.nY; y++ ) {
				for( int z=0; z<m_Size.nZ; z++ ) {
					if( m_pMask->GetMaskAt( x, y, z ) != 0 ) continue;

					int nDir = m_pMaze[x][y][z];
					switch( nDir ) {
						case MD_NORTH:
						case MD_SOUTH:
						case MD_WEST:
						case MD_EAST:
						case MD_UP:
						case MD_DOWN:
							break;
						default:
							continue;
					}


					m_pMaze[x][y][z] = 0;
					if( ( nDir & MD_NORTH ) != 0 ) {
						m_pMaze[ x ][ y - 1 ][ z ] &= ~MD_SOUTH;
						m_pMaze[ x ][ y - 1 ][ z ] |= 0x8000;
					} 
					else if( ( nDir & MD_SOUTH ) != 0 ) {
						m_pMaze[ x ][ y + 1 ][ z ] &= ~MD_NORTH;
						m_pMaze[ x ][ y + 1 ][ z ] |= 0x8000;
					} 
					else if( ( nDir & MD_WEST ) != 0 ) {
						m_pMaze[ x - 1 ][ y ][ z ] &= ~MD_EAST;
						m_pMaze[ x - 1 ][ y ][ z ] |= 0x8000;
					} 
					else if( ( nDir & MD_EAST ) != 0 ) {
						m_pMaze[ x + 1 ][ y ][ z ] &= ~MD_WEST;
						m_pMaze[ x + 1 ][ y ][ z ] |= 0x8000;
					} 
					else if( ( nDir & MD_DOWN ) != 0 ) {
						m_pMaze[ x ][ y ][ z - 1 ] &= ~MD_UP;
						m_pMaze[ x ][ y ][ z - 1 ] |= 0x8000;
					} 
					else if( ( nDir & MD_UP ) != 0 ) {
						m_pMaze[ x ][ y ][ z + 1 ] &= ~MD_DOWN;
						m_pMaze[ x ][ y ][ z + 1 ] |= 0x8000;
					}
				}
			}
		}
		for( int l=0; l<m_Size.nX; l++ ) {
			for( int m=0; m<m_Size.nY; m++ ) {
				for( int n=0; n<m_Size.nZ; n++ ) {
					m_pMaze[l][m][n] &= ~0x8000;
				}
			}
		}
	}
}

void CEtMaze::ClearDeadends( int nPer )
{
	int nDir, nRDir, nTestDir;
	IntVec3 Pos;
	IntVec3 TempPos;
	for( int x=0; x<m_Size.nX; x++ ) {
		for( int y=0; y<m_Size.nY; y++ ) {
			for( int z=0; z<m_Size.nZ; z++ ) {
				nDir = m_pMaze[ x ][ y ][ z ];
				switch( nDir ) {
					case MD_NORTH:
					case MD_SOUTH:
					case MD_WEST:
					case MD_EAST:
					case MD_UP:
					case MD_DOWN:
						break;
					default:
						continue;
				}

				if( rand() % 100 + 1 > nPer ) continue;

				Pos = IntVec3( x, y, z );
				do {
					nDir = 0;
					nTestDir = 0;

					do {
						TempPos = Pos;
						switch( rand() % 6 ) {
							case 0: 
								if( Pos.nY > 0 ) { nDir = MD_NORTH; nRDir = MD_SOUTH; TempPos.nY--; } 
								else { nTestDir |= MD_NORTH; } 
								break;
							case 1: 
								if( Pos.nY+1 < m_Size.nY ) { nDir = MD_SOUTH; nRDir = MD_NORTH; TempPos.nY++; } 
								else { nTestDir |= MD_SOUTH; } 
								break;
							case 2: 
								if( Pos.nX > 0 ) { nDir = MD_WEST; nRDir = MD_EAST; TempPos.nX--; } 
								else { nTestDir |= MD_WEST; } 
								break;
							case 3: 
								if( Pos.nX+1 < m_Size.nX ) { nDir = MD_EAST; nRDir = MD_WEST; TempPos.nX++; } 
								else { nTestDir |= MD_EAST; } 
								break;
							case 4: 
								if( Pos.nZ > 0 ) { nDir = MD_DOWN; nRDir = MD_UP; TempPos.nZ--; } 
								else { nTestDir |= MD_DOWN; } 
								break;
							case 5: 
								if( Pos.nZ+1 < m_Size.nZ ) { nDir = MD_UP; nRDir = MD_DOWN; TempPos.nZ++; } 
								else { nTestDir |= MD_UP; } 
								break;
						}
						if( m_pMaze[ Pos.nX ][ Pos.nY ][ Pos.nZ ] == nDir ) {
							nTestDir |= nDir;
							nDir = 0;
						}
						if( m_pMask->GetMaskAt( TempPos.nX, TempPos.nY, TempPos.nZ ) ) {
							nTestDir |= nDir;
							nDir = 0;
						}
						if( nTestDir == ( MD_WEST | MD_EAST | MD_NORTH | MD_SOUTH | MD_UP | MD_DOWN ) ) break;
					} while( nDir == 0 );

					if( nTestDir == ( MD_WEST | MD_EAST | MD_NORTH | MD_SOUTH | MD_UP | MD_DOWN ) ) break;

					m_pMaze[ Pos.nX ][ Pos.nY ][ Pos.nZ ] |= nDir;
					m_pMaze[ TempPos.nX ][ TempPos.nY ][ TempPos.nZ ] |= nRDir;

					Pos = TempPos;
				} while( m_pMaze[ TempPos.nX ][ TempPos.nY ][ TempPos.nZ ] == nRDir );
			}
		}
	}
}

void CEtMaze::ClearDeadendsBySingleLevel( int nPer, int nLevel )
{
	int nDir, nRDir, nTestDir;
	IntVec3 Pos;
	IntVec3 TempPos;
	for( int x=0; x<m_Size.nX; x++ ) {
		for( int y=0; y<m_Size.nY; y++ ) {
			nDir = m_pMaze[ x ][ y ][ nLevel ];
			switch( nDir ) {
				case MD_NORTH:
				case MD_SOUTH:
				case MD_WEST:
				case MD_EAST:
				case MD_UP:
				case MD_DOWN:
					break;
				default:
					continue;
			}

			if( rand() % 100 + 1 > nPer ) continue;

			Pos = IntVec3( x, y, nLevel );
			do {
				nDir = 0;
				nTestDir = 0;

				do {
					TempPos = Pos;
					switch( rand() % 6 ) {
						case 0: 
							if( Pos.nY > 0 ) { nDir = MD_NORTH; nRDir = MD_SOUTH; TempPos.nY--; } 
							else { nTestDir |= MD_NORTH; } 
							break;
						case 1: 
							if( Pos.nY+1 < m_Size.nY ) { nDir = MD_SOUTH; nRDir = MD_NORTH; TempPos.nY++; } 
							else { nTestDir |= MD_SOUTH; } 
							break;
						case 2: 
							if( Pos.nX > 0 ) { nDir = MD_WEST; nRDir = MD_EAST; TempPos.nX--; } 
							else { nTestDir |= MD_WEST; } 
							break;
						case 3: 
							if( Pos.nX+1 < m_Size.nX ) { nDir = MD_EAST; nRDir = MD_WEST; TempPos.nX++; } 
							else { nTestDir |= MD_EAST; } 
							break;
					}
					if( m_pMaze[ Pos.nX ][ Pos.nY ][ Pos.nZ ] == nDir ) {
						nTestDir |= nDir;
						nDir = 0;
					}
					if( m_pMask->GetMaskAt( TempPos.nX, TempPos.nY, TempPos.nZ ) ) {
						nTestDir |= nDir;
						nDir = 0;
					}
					if( nTestDir == ( MD_WEST | MD_EAST | MD_NORTH | MD_SOUTH ) ) break;
				} while( nDir == 0 );

				if( nTestDir == ( MD_WEST | MD_EAST | MD_NORTH | MD_SOUTH ) ) break;

				m_pMaze[ Pos.nX ][ Pos.nY ][ Pos.nZ ] |= nDir;
				m_pMaze[ TempPos.nX ][ TempPos.nY ][ TempPos.nZ ] |= nRDir;

				Pos = TempPos;
			} while( m_pMaze[ TempPos.nX ][ TempPos.nY ][ TempPos.nZ ] == nRDir );
		}
	}
}

bool CEtMaze::CalcSolve( IntVec3 &StartPos, IntVec3 &EndPos )
{
	return true;
}


int CEtMaze::GetMazeAt( int nX, int nY, int nZ )
{
	return m_pMaze[nX][nY][nZ];
}

int CEtMaze::GetWidth()
{
	return m_Size.nX;
}

int CEtMaze::GetHeight()
{
	return m_Size.nY;
}

int CEtMaze::GetLevel()
{
	return m_Size.nZ;
}