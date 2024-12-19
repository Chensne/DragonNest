#pragma once

#include "EtMaze.h"
#include "EtRandomDungeonRoom.h"
class CEtMazeMask {
public:
	CEtMazeMask( int nWidth, int nHeight, int nLevel );
	~CEtMazeMask();

protected:
	int m_nWidth;
	int m_nHeight;
	int m_nLevel;
	int ***m_pMask;

public:
	int GetMaskAt( int nX, int nY, int nZ );
	bool IsCheckMask( int nX, int nY, int nZ );
	void AddBlock( int nX, int nY, int nZ, int nWidth, int nHeight );
	void AddRoad( int nX, int nY, int nZ, CEtMaze::Maze_Direction Dir );
	void AddRoom( int nX, int nY, int nZ, int nWidth, int nHeight, std::vector<CEtRandomDungeonRoom::DoorStruct> *pVecDoorList );
	void AddStartBlock( int nX, int nY, int nZ );
};