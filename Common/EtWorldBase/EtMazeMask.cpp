#include "StdAfx.h"
#include "EtMazeMask.h"
#include "EtRandomDungeon.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CEtMazeMask::CEtMazeMask( int nWidth, int nHeight, int nLevel )
{
	m_pMask = new int **[nWidth];
	for( int i=0; i<nWidth; i++ ) {
		m_pMask[i] = new int *[nHeight];
		for( int j=0; j<nHeight; j++ ) {
			m_pMask[i][j] = new int[nLevel];
			memset( m_pMask[i][j], 0, nLevel * sizeof(int) );
		}
	}

	m_nWidth = nWidth;
	m_nHeight = nHeight;
	m_nLevel = nLevel;
}

CEtMazeMask::~CEtMazeMask()
{
	for( int i=0; i<m_nWidth; i++ ) {
		for( int j=0; j<m_nHeight; j++ ) {
			SAFE_DELETEA( m_pMask[i][j] );
		}
		SAFE_DELETEA( m_pMask[i] );
	}
	SAFE_DELETEA( m_pMask );
}

int CEtMazeMask::GetMaskAt( int nX, int nY, int nZ ) 
{ 
	return m_pMask[nX][nY][nZ]; 
}

bool CEtMazeMask::IsCheckMask( int nX, int nY, int nZ )
{
	if( m_pMask[nX][nY][nZ] == CEtRandomDungeon::BA_NONE ) return false;
	if( m_pMask[nX][nY][nZ] & CEtRandomDungeon::BA_PASSAGE ) return false;

	return true;
}

void CEtMazeMask::AddBlock( int nX, int nY, int nZ, int nWidth, int nHeight )
{
	if( nZ == -1 ) {
		for( int i=0; i<m_nLevel; i++ ) AddBlock( nX, nY, i, nWidth, nHeight );
	}
	else {
		bool bFlag = true;
		for( int i=0; i<nWidth; i++ ) {
			for( int j=0; j<nHeight; j++ ) {
				if( m_pMask[nX+i][nY+j][nZ] != CEtRandomDungeon::BA_NONE ) {
					bFlag = false;
					break;
				}
			}
		}
		if( bFlag == false ) return;

		for( int i=0; i<nWidth; i++ ) {
			for( int j=0; j<nHeight; j++ ) {
				m_pMask[nX+i][nY+j][nZ] = CEtRandomDungeon::BA_BLOCK;
			}
		}
	}
}

void CEtMazeMask::AddRoad( int nX, int nY, int nZ, CEtMaze::Maze_Direction Dir )
{
	if( nZ == -1 ) {
		for( int i=0; i<m_nLevel; i++ ) AddRoad( nX, nY, i, Dir );
	}
	else {
		if( m_pMask[nX][nY][nZ] != CEtRandomDungeon::BA_NONE ) return;

		m_pMask[nX][nY][nZ] = CEtRandomDungeon::BA_PASSAGE;
		if( Dir & CEtMaze::MD_WEST ) m_pMask[nX][nY][nZ] |= CEtRandomDungeon::BA_WEST;
		if( Dir & CEtMaze::MD_EAST ) m_pMask[nX][nY][nZ] |= CEtRandomDungeon::BA_EAST;
		if( Dir & CEtMaze::MD_NORTH ) m_pMask[nX][nY][nZ] |= CEtRandomDungeon::BA_NORTH;
		if( Dir & CEtMaze::MD_SOUTH ) m_pMask[nX][nY][nZ] |= CEtRandomDungeon::BA_SOUTH;
		if( Dir & CEtMaze::MD_UP ) m_pMask[nX][nY][nZ] |= CEtRandomDungeon::BA_UP;
		if( Dir & CEtMaze::MD_DOWN ) m_pMask[nX][nY][nZ] |= CEtRandomDungeon::BA_DOWN;
	}
}

void CEtMazeMask::AddRoom( int nX, int nY, int nZ, int nWidth, int nHeight, std::vector<CEtRandomDungeonRoom::DoorStruct> *pVecDoorList )
{
	if( nZ == -1 ) {
		for( int i=0; i<m_nLevel; i++ ) AddRoom( nX, nY, i, nWidth, nHeight, pVecDoorList );
	}
	else {
		bool bFlag = true;
		for( int i=0; i<nWidth; i++ ) {
			for( int j=0; j<nHeight; j++ ) {
				if( m_pMask[nX+i][nY+j][nZ] != CEtRandomDungeon::BA_NONE ) {
					bFlag = false;
					break;
				}
			}
		}
		if( bFlag == false ) return;

		for( int i=0; i<nWidth; i++ ) {
			for( int j=0; j<nHeight; j++ ) {
				m_pMask[nX+i][nY+j][nZ] = CEtRandomDungeon::BA_ROOM;
			}
		}
		int *pMask;
		for( DWORD i=0; i<pVecDoorList->size(); i++ ) {
			pMask = &m_pMask[nX + (*pVecDoorList)[i].nX][nY + (*pVecDoorList)[i].nY][nZ];
			if( (*pVecDoorList)[i].Direction & CEtRandomDungeonRoom::DD_WEST ) {
				*pMask |= CEtRandomDungeon::BA_DOOR | CEtRandomDungeon::BA_WEST;
				m_pMask[nX + (*pVecDoorList)[i].nX - 1][nY + (*pVecDoorList)[i].nY][nZ] |= CEtRandomDungeon::BA_PASSAGE | CEtRandomDungeon::BA_EAST;
			}
			if( (*pVecDoorList)[i].Direction & CEtRandomDungeonRoom::DD_EAST ) {
				*pMask |= CEtRandomDungeon::BA_DOOR | CEtRandomDungeon::BA_EAST;
				m_pMask[nX + (*pVecDoorList)[i].nX + 1][nY + (*pVecDoorList)[i].nY][nZ] |= CEtRandomDungeon::BA_PASSAGE | CEtRandomDungeon::BA_WEST;
			}
			if( (*pVecDoorList)[i].Direction & CEtRandomDungeonRoom::DD_NORTH ) {
				*pMask |= CEtRandomDungeon::BA_DOOR | CEtRandomDungeon::BA_NORTH;
				m_pMask[nX + (*pVecDoorList)[i].nX][nY + (*pVecDoorList)[i].nY - 1][nZ] |= CEtRandomDungeon::BA_PASSAGE | CEtRandomDungeon::BA_SOUTH;
			}
			if( (*pVecDoorList)[i].Direction & CEtRandomDungeonRoom::DD_SOUTH ) {
				*pMask |= CEtRandomDungeon::BA_DOOR | CEtRandomDungeon::BA_SOUTH;
				m_pMask[nX + (*pVecDoorList)[i].nX][nY + (*pVecDoorList)[i].nY + 1][nZ] |= CEtRandomDungeon::BA_PASSAGE | CEtRandomDungeon::BA_NORTH;
			}
		}
	}
}

void CEtMazeMask::AddStartBlock( int nX, int nY, int nZ )
{
	int nDir = CEtMaze::MD_EAST | CEtMaze::MD_WEST | CEtMaze::MD_NORTH | CEtMaze::MD_SOUTH;
	if( nX == 0 ) nDir &= ~CEtMaze::MD_WEST;
	if( nX == m_nWidth - 1 ) nDir &= ~CEtMaze::MD_EAST;
	if( nY == 0 ) nDir &= ~CEtMaze::MD_NORTH;
	if( nY == m_nHeight - 1 ) nDir &= ~CEtMaze::MD_SOUTH;

	std::vector<int> nVecList;
	if( nDir & CEtMaze::MD_EAST ) nVecList.push_back( CEtMaze::MD_EAST );
	if( nDir & CEtMaze::MD_WEST ) nVecList.push_back( CEtMaze::MD_WEST );
	if( nDir & CEtMaze::MD_NORTH ) nVecList.push_back( CEtMaze::MD_NORTH );
	if( nDir & CEtMaze::MD_SOUTH ) nVecList.push_back( CEtMaze::MD_SOUTH );
	if( nVecList.size() > 1 ) {
		int nResult = rand()%(int)nVecList.size() + 1;
		while( nResult != (int)nVecList.size() ) {
			int nIndex = rand()%(int)nVecList.size();
			nDir &= ~nVecList[nIndex];
			nVecList.erase( nVecList.begin() + nIndex );
		}
	}
	for( DWORD i=0; i<nVecList.size(); i++ ) {
		switch( nVecList[i] ) {
			case CEtMaze::MD_EAST:
				AddRoad( nX + 1, nY, nZ, CEtMaze::MD_WEST );
				break;
			case CEtMaze::MD_WEST:
				AddRoad( nX - 1, nY, nZ, CEtMaze::MD_EAST );
				break;
			case CEtMaze::MD_NORTH:
				AddRoad( nX, nY - 1, nZ, CEtMaze::MD_SOUTH );
				break;
			case CEtMaze::MD_SOUTH:
				AddRoad( nX, nY + 1, nZ, CEtMaze::MD_NORTH );
				break;
		}
	}

	AddRoad( nX, nY, nZ, CEtMaze::Maze_Direction( nDir ) );
}