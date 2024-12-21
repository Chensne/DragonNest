#include "StdAfx.h"
#include "EtRandomDungeon.h"
#include "EtMaze.h"
#include "EtMazeMask.h"
#include "EtRandomDungeonRoom.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CEtRandomDungeon::CEtRandomDungeon()
{
	m_pMaze = NULL;
	m_pMazeMask = NULL;
	m_pDungeonArray = NULL;
	m_nRandomRoomCount = 0;
	m_nTotalRandomRoomWeight = 0;
	m_fBlockSize = 4000.f;
	m_nSeed = 0;
	m_vOffset = EtVector3( 0.f, 0.f, 0.f );
}

CEtRandomDungeon::~CEtRandomDungeon()
{
	Destroy();
}

bool CEtRandomDungeon::Initialize( int nSeed )
{
	m_nRandomRoomCount = 0;
	m_nTotalRandomRoomWeight = 0;
	InitializeSeed( nSeed );

	return true;
}

void CEtRandomDungeon::Destroy()
{
	SAFE_DELETE_PVEC( m_pVecRoomList );
	SAFE_DELETE( m_pMaze );
	SAFE_DELETE( m_pMazeMask );
	SAFE_DELETE_VEC( m_VecRandomRoomList );
	FreeDungeonArray();
}


bool CEtRandomDungeon::GenerateMaze( int nWidth, int nHeight, int nLevel, int nSparseness, int nRandomness, int nDeadendRemove )
{
	SAFE_DELETE( m_pMaze );
	m_pMaze = new CEtMaze( nWidth, nHeight, nLevel, m_nSeed, m_pMazeMask );

	/*
	m_pMaze->Generation( nRandomness );
	m_pMaze->Sparsify( nSparseness );
	m_pMaze->ClearDeadends( nDeadendRemove );
	*/

	for( int i=0; i<nLevel; i++ ) m_pMaze->GenerationBySingleLevel( nRandomness, i );
	m_pMaze->Sparsify( nSparseness );
	for( int i=0; i<nLevel; i++ ) m_pMaze->ClearDeadendsBySingleLevel( nDeadendRemove, i );

	m_DungeonSize = IntVec3( nWidth, nHeight, nLevel );

	return true;
}

void CEtRandomDungeon::CreateMazeMask( int nWidth, int nHeight, int nLevel )
{
	SAFE_DELETE( m_pMazeMask );
	m_pMazeMask = new CEtMazeMask( nWidth, nHeight, nLevel );
}

void CEtRandomDungeon::InitializeSeed( int nSeed )
{
	if( nSeed > 0 ) m_nSeed = nSeed;
	else m_nSeed = ::rand()%LONG_MAX;

	srand( m_nSeed );
}

void CEtRandomDungeon::AllocDungeonArray()
{
	m_pDungeonArray = new int **[m_DungeonSize.nX];
	for( int i=0; i<m_DungeonSize.nX; i++ ) {
		m_pDungeonArray[i] = new int *[m_DungeonSize.nY];
		for( int j=0; j<m_DungeonSize.nY; j++ ) {
			m_pDungeonArray[i][j] = new int[m_DungeonSize.nZ];
			memset( m_pDungeonArray[i][j], 0, m_DungeonSize.nZ * sizeof(int) );
		}
	}
}

void CEtRandomDungeon::FreeDungeonArray()
{
	if( !m_pDungeonArray ) return;
	for( int i=0; i<m_DungeonSize.nX; i++ ) {
		for( int j=0; j<m_DungeonSize.nY; j++ ) {
			SAFE_DELETEA( m_pDungeonArray[i][j] );
		}
		SAFE_DELETEA( m_pDungeonArray[i] );
	}
	SAFE_DELETEA( m_pDungeonArray );
}

bool CEtRandomDungeon::CalcDungeonArray()
{
	if( !m_pMaze ) return false;
	if( !m_pMazeMask ) return false;

	AllocDungeonArray();

	for( int z=0; z<m_DungeonSize.nZ; z++ ) {
		for( int x=0;x<m_DungeonSize.nX; x++ ) {
			for( int y=0; y<m_DungeonSize.nY; y++ ) {
				int nDir = m_pMaze->GetMazeAt( x, y, z );
				int nMask = m_pMazeMask->GetMaskAt( x, y, z );

				// 일단 길 체크
				if( nDir != CEtMaze::MD_NONE ) {
					m_pDungeonArray[x][y][z] |= BA_PASSAGE;
					if( nDir & CEtMaze::MD_WEST ) m_pDungeonArray[x][y][z] |= BA_WEST;
					if( nDir & CEtMaze::MD_EAST ) m_pDungeonArray[x][y][z] |= BA_EAST;
					if( nDir & CEtMaze::MD_SOUTH ) m_pDungeonArray[x][y][z] |= BA_SOUTH;
					if( nDir & CEtMaze::MD_NORTH ) m_pDungeonArray[x][y][z] |= BA_NORTH;
					if( nDir & CEtMaze::MD_UP ) m_pDungeonArray[x][y][z] |= BA_UP;
					if( nDir & CEtMaze::MD_DOWN ) m_pDungeonArray[x][y][z] |= BA_DOWN;
				}

				// 고정 방 체크
				if( nMask & BA_ROOM ) m_pDungeonArray[x][y][z] |= BA_ROOM;
				if( nMask & BA_DOOR ) m_pDungeonArray[x][y][z] |= BA_DOOR;
				if( nMask & BA_EAST ) m_pDungeonArray[x][y][z] |= BA_EAST;
				if( nMask & BA_WEST ) m_pDungeonArray[x][y][z] |= BA_WEST;
				if( nMask & BA_NORTH ) m_pDungeonArray[x][y][z] |= BA_NORTH;
				if( nMask & BA_SOUTH ) m_pDungeonArray[x][y][z] |= BA_SOUTH;
				if( nMask & BA_UP ) m_pDungeonArray[x][y][z] |= BA_UP;
				if( nMask & BA_DOWN ) m_pDungeonArray[x][y][z] |= BA_DOWN;

			}
		}
	}

	return true;
}

void CEtRandomDungeon::SetMaskRoom( int nX, int nY, int nZ, int nWidth, int nHeight )
{
	int nRX, nRY;
	/*
	for( int j=0; j<nWidth; j++ ) {
		for( int k=0; k<nHeight; k++ ) {
			nRX = nX + j;
			nRY = nY + k;
			if( nRX < 0 || nRX >= m_DungeonSize.nX || nRY < 0 || nRY >= m_DungeonSize.nY ) continue;

			if( m_pMazeMask->GetMaskAt( nRX, nRY ) ) return;
			if( m_pDungeonArray[nRX][nRY][nZ] & BA_ROOM ) return;
		}
	}
	*/

	CEtRandomDungeonRoom *pRoom = new CEtRandomDungeonRoom( CEtRandomDungeonRoom::RT_RANDOM );
	pRoom->Initialize( nX, nY, nZ, nWidth, nHeight );

	bool bDoorExist = false;
	for( int j=0; j<nWidth; j++ ) {
		for( int k=0; k<nHeight; k++ ) {
			nRX = nX + j;
			nRY = nY + k;
			if( nRX < 0 || nRX >= m_DungeonSize.nX || nRY < 0 || nRY >= m_DungeonSize.nY ) continue;

			if( !m_pMazeMask->GetMaskAt( nRX, nRY, nZ ) ) {
				int nAttr = m_pDungeonArray[nRX][nRY][nZ];
				int *pAttr = &m_pDungeonArray[nRX][nRY][nZ];
				*pAttr = BA_ROOM;
				// 길이 내부에 나있는 경우 문 체크해서 만들어준다.
				if( j == 0 || j == nWidth-1 || k == 0 || k == nHeight-1 ) {
					if( nAttr & BA_PASSAGE ) {
						bool bDoor = false;
						if( j == 0 && k == 0 ) {
							if( nAttr & BA_WEST ) *pAttr |= BA_WEST, bDoor = true;
							if( nAttr & BA_NORTH ) *pAttr |= BA_NORTH, bDoor = true;
						}
						else if( j == nWidth-1 && k == 0 ) {
							if( nAttr & BA_EAST ) *pAttr |= BA_EAST, bDoor = true;
							if( nAttr & BA_NORTH ) *pAttr |= BA_NORTH, bDoor = true;
						}
						else if( j == nWidth-1 && k == nHeight-1 ) {
							if( nAttr & BA_EAST ) *pAttr |= BA_EAST, bDoor = true;
							if( nAttr & BA_SOUTH ) *pAttr |= BA_SOUTH, bDoor = true;
						}
						else if( j == 0 && k == nHeight-1 ) {
							if( nAttr & BA_WEST ) *pAttr |= BA_WEST, bDoor = true;
							if( nAttr & BA_SOUTH ) *pAttr |= BA_SOUTH, bDoor = true;
						}
						else {
							if( j == 0 ) {
								if( nAttr & BA_WEST ) *pAttr |= BA_WEST, bDoor = true;
							}
							if( j == nWidth-1 ) {
								if( nAttr & BA_EAST ) *pAttr |= BA_EAST, bDoor = true;
							}
							if( k == 0 ) {
								if( nAttr & BA_NORTH ) *pAttr |= BA_NORTH, bDoor = true;
							}
							if( k == nHeight-1 ) {
								if( nAttr & BA_SOUTH ) *pAttr |= BA_SOUTH, bDoor = true;
							}
						}
						if( bDoor == true ) {
							bDoorExist = true;
							*pAttr |= BA_DOOR;

							int nDirection = 0;
							if( *pAttr & BA_WEST ) nDirection |= CEtRandomDungeonRoom::DD_WEST;
							if( *pAttr & BA_EAST ) nDirection |= CEtRandomDungeonRoom::DD_EAST;
							if( *pAttr & BA_NORTH ) nDirection |= CEtRandomDungeonRoom::DD_NORTH;
							if( *pAttr & BA_SOUTH ) nDirection |= CEtRandomDungeonRoom::DD_SOUTH;
							pRoom->AddDoor( j, k, (CEtRandomDungeonRoom::DoorDirection)nDirection );
						}
					}
				}
			}
		}
	}

	// 길이 내부에 없을 경우 외부 체크해서 이여준다.
	if( bDoorExist == false ) {
		FindSideDoor( nX - 1, nY - 1, nZ, nWidth + 2, nHeight + 2, pRoom );
	}


	AddRandomRoom( pRoom );
}

void CEtRandomDungeon::FindSideDoor( int nX, int nY, int nZ, int nWidth, int nHeight, CEtRandomDungeonRoom *pRoom )
{
	int nRX, nRY;
	bool bDoorExist = false;
	std::vector<IntVec3> VecPos;
	std::vector<char> VecFlag;

	for( int j=0; j<nWidth; j++ ) {
		for( int k=0; k<nHeight; k++ ) {
			nRX = nX + j;
			nRY = nY + k;
			if( nRX < 0 || nRX >= m_DungeonSize.nX || nRY < 0 || nRY >= m_DungeonSize.nY ) continue;

			if( !m_pMazeMask->IsCheckMask( nRX, nRY, nZ ) ) {
				int nAttr = m_pDungeonArray[nRX][nRY][nZ];
				int *pAttr = &m_pDungeonArray[nRX][nRY][nZ];
				if( j == 0 || j == nWidth-1 || k == 0 || k == nHeight-1 ) {
					if( nAttr & BA_PASSAGE ) {
						char cDoorFlag = 0;
						if( !( j == 0 && k == 0 ) && !( j == nWidth-1 && k == 0 ) && !( j == nWidth-1 && k == nHeight-1 ) && !( j == 0 && k == nHeight-1 ) ) {
							if( j == 0 ) {
								if( nAttr & BA_WEST ) cDoorFlag = 1;
								else if( nAttr & BA_EAST || nAttr & BA_NORTH || nAttr & BA_SOUTH || nAttr & BA_UP || nAttr & BA_DOWN ) cDoorFlag = -1;
							}
							if( j == nWidth-1 ) {
								if( nAttr & BA_EAST ) cDoorFlag = 2;
								else if( nAttr & BA_WEST || nAttr & BA_NORTH || nAttr & BA_SOUTH || nAttr & BA_UP || nAttr & BA_DOWN ) cDoorFlag = -2;
							}
							if( k == 0 ) {
								if( nAttr & BA_NORTH ) cDoorFlag = 3;
								else if( nAttr & BA_EAST || nAttr & BA_WEST || nAttr & BA_SOUTH || nAttr & BA_UP || nAttr & BA_DOWN ) cDoorFlag = -3;
							}
							if( k == nHeight-1 ) {
								if( nAttr & BA_SOUTH ) cDoorFlag = 4;
								else if( nAttr & BA_EAST || nAttr & BA_WEST || nAttr & BA_NORTH || nAttr & BA_UP || nAttr & BA_DOWN ) cDoorFlag = -4;
							}
						}

						if( cDoorFlag > 0 ) {
							bDoorExist = true;

							int nDirection = 0;
							if( *pAttr & BA_WEST ) nDirection |= CEtRandomDungeonRoom::DD_WEST;
							if( *pAttr & BA_EAST ) nDirection |= CEtRandomDungeonRoom::DD_EAST;
							if( *pAttr & BA_NORTH ) nDirection |= CEtRandomDungeonRoom::DD_NORTH;
							if( *pAttr & BA_SOUTH ) nDirection |= CEtRandomDungeonRoom::DD_SOUTH;

							switch( cDoorFlag ) {
								case 1:
									*pAttr |= BA_EAST;

									m_pDungeonArray[nRX+1][nRY][nZ] |= BA_DOOR | BA_WEST;
									pRoom->AddDoor( j+1, k, (CEtRandomDungeonRoom::DoorDirection)nDirection );
									break;
								case 2:
									*pAttr |= BA_WEST;

									m_pDungeonArray[nRX-1][nRY][nZ] |= BA_DOOR | BA_EAST;
									pRoom->AddDoor( j-1, k, (CEtRandomDungeonRoom::DoorDirection)nDirection );
									break;
								case 3:
									*pAttr |= BA_SOUTH;

									m_pDungeonArray[nRX][nRY+1][nZ] |= BA_DOOR | BA_NORTH;
									pRoom->AddDoor( j, k+1, (CEtRandomDungeonRoom::DoorDirection)nDirection );
									break;
								case 4:
									*pAttr |= BA_NORTH;

									m_pDungeonArray[nRX][nRY-1][nZ] |= BA_DOOR | BA_SOUTH;
									pRoom->AddDoor( j, k-1, (CEtRandomDungeonRoom::DoorDirection)nDirection );
									break;
							}
							return;
						}
						else if( cDoorFlag < 0 ) {
							VecPos.push_back( IntVec3( nRX, nRY, nZ ) );
							VecFlag.push_back( cDoorFlag );
						}
					}
				}
			}
		}
	}

	// 정면으로 바라보는 방향이 없었을 경우엔 아무길 하나 랜덤하게 찾아서 길을 내준다. 아구찮아!!
	if( bDoorExist == false ) {
		if( VecPos.empty() ) {
			ASSERT(0&&"말두안돼!!");
			return;
		}
		int nRandom = rand() % (int)VecPos.size();

		nRX = VecPos[nRandom].nX;
		nRY = VecPos[nRandom].nY;
		int j = nRX - nX;
		int k = nRY - nY;
		int *pAttr = &m_pDungeonArray[nRX][nRY][nZ];

		switch( VecFlag[nRandom] ) {
			case -1:
				*pAttr |= BA_EAST;

				m_pDungeonArray[nRX+1][nRY][nZ] |= BA_DOOR | BA_WEST;
				pRoom->AddDoor( j+1, k, CEtRandomDungeonRoom::DD_WEST );
				break;
			case -2:
				*pAttr |= BA_WEST;

				m_pDungeonArray[nRX-1][nRY][nZ] |= BA_DOOR | BA_EAST;
				pRoom->AddDoor( j-1, k, CEtRandomDungeonRoom::DD_EAST );
				break;
			case -3:
				*pAttr |= BA_SOUTH;

				m_pDungeonArray[nRX][nRY+1][nZ] |= BA_DOOR | BA_NORTH;
				pRoom->AddDoor( j, k+1, CEtRandomDungeonRoom::DD_NORTH );
				break;
			case -4:
				*pAttr |= BA_NORTH;

				m_pDungeonArray[nRX][nRY-1][nZ] |= BA_DOOR | BA_SOUTH;
				pRoom->AddDoor( j, k-1, CEtRandomDungeonRoom::DD_SOUTH );
				break;
		}
	}
}

bool CEtRandomDungeon::GenerateRandomRoom()
{
	if( m_VecRandomRoomList.empty() ) 
		return false;

	int rx, ry, cx, cy;
	RandomRoomPossibleStruct *pStruct;
	for( int z=0; z<m_DungeonSize.nZ; z++ ) {
		for( int i=0; i<m_nRandomRoomCount; i++ ) {
			pStruct = &m_VecRandomRoomList[ RollRandomRoom() ];

			rx = pStruct->nWidth;
			ry = pStruct->nHeight;

			if( FindOptimalRoomPlacement( rx, ry, z, cx, cy ) == false ) break;

			SetMaskRoom( cx, cy, z, rx, ry );
		}
	}
	return true;
}

bool CEtRandomDungeon::GenerateRandomRoom( int nRoomCount, int nMinWidth, int nMaxWidth, int nMinHeight, int nMaxHeight )
{
	int rx, ry, cx, cy;
	for( int z=0; z<m_DungeonSize.nZ; z++ ) {
		for( int i=0; i<nRoomCount; i++ ) {
			if( nMinWidth == nMaxWidth ) rx = nMinWidth;
			else rx = rand() % ( nMaxWidth - nMinWidth + 1 ) + nMinWidth;

			if( nMinHeight == nMaxHeight ) ry = nMinHeight;
			else ry = rand() % ( nMaxHeight - nMinHeight + 1 ) + nMinHeight;

			if( FindOptimalRoomPlacement( rx, ry, z, cx, cy ) == false ) break;
			else SetMaskRoom( cx, cy, z, rx, ry );
		}
	}
	return true;
}

bool CEtRandomDungeon::FindOptimalRoomPlacement( int &nWidth, int &nHeight, int nLevel, int &nResultX, int &nResultY )
{
	if( nWidth > m_DungeonSize.nX - 2 ) nWidth = m_DungeonSize.nX - 2;
	if( nHeight > m_DungeonSize.nY - 2 ) nHeight = m_DungeonSize.nY - 2;

	int spaceX = ( m_DungeonSize.nX - nWidth );
	int spaceY = ( m_DungeonSize.nY - nHeight );

	int nMinimumTally = 100000;
	int xForMin = -1;
	int yForMin = -1;

	int nTotal = 0;
	int nOverlapsRoom = 0;
	int nLowestOverlapsRoom = 0;
	bool bDiagonal;
	WEIGHTEDLIST *pWeightList = NULL;

	for( int x=1; x<spaceX; x++ ) {
		for( int y=1; y<spaceY; y++ ) {
			if( m_pMazeMask->GetMaskAt( x, y, nLevel ) ) continue;

			int nTally = 0;
			for( int i=-1; i<nWidth+1; i++ ) {
				for( int j=-1; j<nHeight+1; j++ ) {
					if( m_pMazeMask->GetMaskAt( x+i, y+j, nLevel ) ) {
						nTally += 1000;
						continue;
					}
					int nAttr = m_pDungeonArray[x+i][y+j][nLevel];
					if( ( ( i == -1 ) && ( j == -1 ) ) ||
						( ( i == -1 ) && ( j == nHeight ) ) ||
						( ( i == nWidth ) && ( j == -1 ) ) ||
						( ( i == nWidth ) && ( j == nHeight ) ) ) bDiagonal = true;
					else {
						if( nAttr & BA_PASSAGE ) {
							if( ( j == -1 ) || ( i == -1 ) || ( j == nHeight ) || ( i == nWidth ) ) 
								nTally++;
							else nTally += 3;
						}
						if( nAttr & BA_ROOM ) {
							nTally += 100;
							nOverlapsRoom = 1;
						}
						if( ( i >= 0 ) && ( j >= 0 ) && ( i < nWidth ) && ( j < nHeight ) ) {
							if( m_pMazeMask->GetMaskAt( x + i, y + j, nLevel ) ) nTally += 10;
						}
					}
				}
			}

			if( ( nTally > 0 ) && ( nTally <= nMinimumTally ) ) {
				if( nTally != nMinimumTally ) {
					DestroyWeightedList( &pWeightList );
					pWeightList = NULL;
					nTotal = 0;
					nLowestOverlapsRoom = nOverlapsRoom;
				}

				nMinimumTally = nTally;
				xForMin = x;
				yForMin = y;
				nTotal += AddToWeightedList( &pWeightList, ( ( xForMin << 16 ) + yForMin ), 1 );
			}

			nOverlapsRoom = 0;
		}
	}

	if( nLowestOverlapsRoom ) {
		if( ( nWidth <= 2 ) && ( nHeight <= 2 ) ) {
			if( pWeightList ) DestroyWeightedList( &pWeightList );
			return false;
		}
		if( nWidth > nHeight ) nWidth--;
		else nHeight--;

		return FindOptimalRoomPlacement( nWidth, nHeight, nLevel, nResultX, nResultY );
	}

	if( pWeightList == NULL ) {
		nResultX = 1 + rand() % ( spaceX - 1 );
		nResultY = 1 + rand() % ( spaceY - 1 );
	} 
	else {
		nTotal = GetWeightedItem( &pWeightList, RollDice( 1, nTotal ), &nTotal );
		nResultX = (unsigned int)( nTotal >> 16 );
		nResultY = (unsigned int)( nTotal & 0xFFFF );
	}

	if( pWeightList ) DestroyWeightedList( &pWeightList );
	return true;
}

int CEtRandomDungeon::AddToWeightedList( WEIGHTEDLIST** list, long data, int weight ) 
{
	WEIGHTEDLIST* item;
	WEIGHTEDLIST* i;

	item = new WEIGHTEDLIST;
	memset( item, 0, sizeof( *item ) );

	item->nData = data;
	item->nWeight = weight;

	if( *list == 0 ) {
		*list = item;
	} else {
		i = *list;
		while( i->pNext != 0 ) {
			i = i->pNext;
		}
		i->pNext = item;
	}

	return weight;
}


long CEtRandomDungeon::GetWeightedItem( WEIGHTEDLIST** list, int index, int* count )
{
	int total;
	long data;
	WEIGHTEDLIST* i;
	WEIGHTEDLIST* p;

	i = *list;
	p = 0;
	total = 0;
	while( i != 0 ) {
		total += i->nWeight;
		if( index <= total ) {
			data = i->nData;
			if( p == 0 ) {
				*list = i->pNext;
			} else {
				p->pNext = i->pNext;
			}
			*count -= i->nWeight;
			delete i;
			return data;
		}
		p = i;
		i = i->pNext;
	}

	return -1;
}


void CEtRandomDungeon::DestroyWeightedList( WEIGHTEDLIST** list ) 
{
	WEIGHTEDLIST* i;
	WEIGHTEDLIST* n;

	i = *list;
	while( i != 0 ) {
		n = i->pNext;
		SAFE_DELETE(i);
		i = n;
	}
	*list = 0;
}

int CEtRandomDungeon::RollDice( int count, int sides ) 
{
	int i;
	int total;

	total = 0;
	for( i = 0; i < count; i++ ) {
		total += rand() % sides + 1;
	}

	return total;
}

int CEtRandomDungeon::RollRandomRoom()
{
	int nTemp = 0;
	int nResult = rand() % m_nTotalRandomRoomWeight;

	for( DWORD i=0; i<m_VecRandomRoomList.size(); i++ ) {
		nTemp += m_VecRandomRoomList[i].nWeight;
		if( nResult < nTemp ) return i;
	}
	return -1;
}

bool IsInRect( int x, int y, RECT &rect ) 
{
	if( x >= rect.left && x <= rect.right && y >= rect.top && y <= rect.bottom ) return true;
	return false;
}

int CEtRandomDungeon::AddConstraintRoom( CEtRandomDungeonRoom *pRoom, bool bCheckRect )
{
	if( bCheckRect == true ) {
		RECT rcRect[2];
		rcRect[0].left = pRoom->GetPosition()->nX;
		rcRect[0].top = pRoom->GetPosition()->nY;
		rcRect[0].right = rcRect[0].left + pRoom->GetWidth() - 1;
		rcRect[0].bottom = rcRect[0].top + pRoom->GetHeight() - 1;

		// 기존 룸과 겹칠 경우엔 Add 안한다.
		for( DWORD i=0; i<m_pVecRoomList.size(); i++ ) {
			if( m_pVecRoomList[i]->GetPosition()->nZ != pRoom->GetPosition()->nZ ) continue;
			rcRect[1].left = m_pVecRoomList[i]->GetPosition()->nX;
			rcRect[1].top = m_pVecRoomList[i]->GetPosition()->nY;
			rcRect[1].right = rcRect[1].left + m_pVecRoomList[i]->GetWidth() - 1;
			rcRect[1].bottom = rcRect[1].top + m_pVecRoomList[i]->GetHeight() - 1;

			if( IsInRect( rcRect[0].left, rcRect[0].top, rcRect[1] ) 
			 || IsInRect( rcRect[0].left, rcRect[0].bottom, rcRect[1] ) 
			 || IsInRect( rcRect[0].right, rcRect[0].top, rcRect[1] ) 
			 || IsInRect( rcRect[0].right, rcRect[0].bottom, rcRect[1] ) ) return -1;
		}
	}
	if( pRoom->GetDoorCount() == 0 ) return -1;

	m_pVecRoomList.push_back( pRoom );

	// 마스크 해줘야 길생성을 바르게 한다!!
	m_pMazeMask->AddRoom( pRoom->GetPosition()->nX, pRoom->GetPosition()->nY, pRoom->GetPosition()->nZ, pRoom->GetWidth(), pRoom->GetHeight(), pRoom->GetDoorList() );

	return (int)m_pVecRoomList.size() - 1;
}

bool CEtRandomDungeon::AddRandomRoom( CEtRandomDungeonRoom *pRoom )
{
	m_pVecRoomList.push_back( pRoom );
	return true;
}

void CEtRandomDungeon::AddRandomRoomPossibleSize( int nWidth, int nHeight, int nWeight )
{
	for( DWORD i=0; i<m_VecRandomRoomList.size(); i++ ) {
		if( m_VecRandomRoomList[i].nWidth == nWidth && m_VecRandomRoomList[i].nHeight == nHeight ) return;
	}

	RandomRoomPossibleStruct Struct;
	Struct.nWidth = nWidth;
	Struct.nHeight = nHeight;
	Struct.nWeight = nWeight;
	m_VecRandomRoomList.push_back( Struct );
	m_nTotalRandomRoomWeight += nWeight;
}

void CEtRandomDungeon::SetRandomRoomCount( int nValue )
{
	m_nRandomRoomCount = nValue;
}


int CEtRandomDungeon::GetDungeonAt( int nX, int nY, int nZ )
{
	return m_pDungeonArray[nX][nY][nZ];
}

bool CEtRandomDungeon::Load( const char *szFileName )
{
	return true;
}

int CEtRandomDungeon::GetRoomCount()
{
	return (int)m_pVecRoomList.size();
}

CEtRandomDungeonRoom *CEtRandomDungeon::GetRoom( int nIndex )
{
	if( nIndex < 0 || nIndex >= (int)m_pVecRoomList.size() ) return NULL;
	return m_pVecRoomList[nIndex];
}

void CEtRandomDungeon::Render( LOCAL_TIME LocalTime )
{
}

EtVector3 *CEtRandomDungeon::GetOffset()
{
	return &m_vOffset;
}

float CEtRandomDungeon::GetBlockSize()
{
	return m_fBlockSize;
}

IntVec3 *CEtRandomDungeon::GetDungeonSize()
{
	return &m_DungeonSize;
}