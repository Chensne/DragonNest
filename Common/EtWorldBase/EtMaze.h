#pragma once

#include "./Common.h"

class CEtMazeMask;
class CEtMaze {
public:
	CEtMaze( int nWidth, int nHeight, int nLevel, int nSeed = 0, CEtMazeMask *pMask = NULL );
	~CEtMaze();
	
	enum Maze_Direction {
		MD_NONE = 0,
		MD_WEST = 0x0001,
		MD_EAST = 0x0002,
		MD_NORTH = 0x0004,
		MD_SOUTH = 0x0008,
		MD_UP = 0x0010,
		MD_DOWN = 0x0020,
	};

protected:
	IntVec3 m_Size;
	CEtMazeMask *m_pMask;
	std::vector<IntVec3> m_VecSolveList;

	int ***m_pMaze;

//	bool IsApplyBlock( int nX, int nY );

public:
	void Generation( int nRandomness );
	void GenerationBySingleLevel( int nRandomness, int nLevel );
	void Sparsify( int nAmount );
	void ClearDeadends( int nPer );
	void ClearDeadendsBySingleLevel( int nPer, int nLevel );
	bool CalcSolve( IntVec3 &StartPos, IntVec3 &EndPos );

	int GetMazeAt( int nX, int nY, int nZ );
	int GetWidth();
	int GetHeight();
	int GetLevel();
};