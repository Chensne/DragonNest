
#pragma once
#include "DNParty.h"
#include "DNUserSession.h"
#include "DnWorldGateStruct.h"

class CDNDungeonManager
{
public:
	CDNDungeonManager();
	~CDNDungeonManager();	

	DungeonGateStruct * GetDungeonPtr(int nMapIdx);
	void CalcDungeonConstructionLevel( int nMapIndex, DungeonGateStruct * pDungeonStruct, CDNParty * pParty );
	void CalcDungeonConstructionLevel( int nMapIndex, DungeonGateStruct * pDungeonStruct, CDNUserSession * pSession );

private:
	std::map <int, DungeonGateStruct*> m_DungeonList;

	CSyncLock m_Sync;
};

extern CDNDungeonManager* g_pDungeonManager;