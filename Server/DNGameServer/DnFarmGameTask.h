
#pragma once

#include "DnGameTask.h"


class CGrowingArea;
class CFishingArea;

class CDnFarmGameTask:public CDnGameTask,public TBoostMemoryPool<CDnFarmGameTask>
{
public:
	CDnFarmGameTask( CDNGameRoom * pRoom );
	virtual ~CDnFarmGameTask();

	virtual bool	InitializeStage( int nCurrentMapIndex, int nGateIndex, TDUNGEONDIFFICULTY StageDifficulty, int nRandomSeed, bool bContinueStage, bool bDirectConnect, int nGateSelect );
	virtual bool	OnInitializeBreakIntoActor( CDNUserSession* pSession, const int iVectorIndex );
	virtual void	OnInitializeStartPosition( CDNUserSession* pSession, const int iPartyIndex );
	virtual void	OnInitializeRespawnPosition( DnActorHandle hActor, const bool bIsStartPosition=false );
	virtual int		OnDispatchMessage(CDNUserSession * pSession, int nMainCmd, int nSubCmd, char *pData, int nLen);
	virtual void	OnLeaveUser( UINT uiSessionID );
	void OnStartGuildWarFinal();
	// Process
	virtual void Process( LOCAL_TIME LocalTime, float fDelta );
	void ProcessGrowingArea( float fDelta );	

	//
	void InitializeGrowingArea();	
	void InitializePrivateGrowingArea( TAGetListFieldForCharacter* pPacket );
	void InitializeSeed( TAGetListField* pPacket );
	void InitializeFishingArea();

	//
	void			SetPauseFlag( bool bFlag ){ m_bPause = bFlag; }
	CGrowingArea*	GetGrowingArea( const int iIndex, CDNUserSession* pSession=NULL );
	CFishingArea * GetFishingArea(int nIdx);
	CFishingArea * GetFishingArea(CDNUserSession * pSession);
	CFishingArea * GetFishingArea(EtVector3 * pPosition);

	// Sync
	void	SyncArea( CDNUserSession* pBreakIntoGameSession );
	void	SyncFishing( CDNUserSession* pBreakIntoGameSession );

	bool	UpdateCharacterName( MAChangeCharacterName* pPacket );

	bool	bIsPrivateArea( int iAreaIndex );
	bool	CheckActivePrivateFieldCount( CDNUserSession* pSession );

protected:

	virtual int OnRecvFarmMessage(CDNUserSession * pSession, int nSubCmd, char *pData, int nLen);
	virtual int OnRecvFishingMessage(CDNUserSession * pSession, int nSubCmd, char *pData, int nLen);

private:

	std::map<int,CGrowingArea*>	m_mGrowingArea;
	std::map<int,SOBB*>							m_mPrivateGrowingAreaInfo;
	std::map<INT64,std::vector<CGrowingArea*>>	m_mPrivateGrowingArea;
	std::vector<int>			m_vUpdateAreaIndex;
	std::vector<int>			m_vUpdateElapsedTimeSec;
	bool						m_bPause;

	//Fishing
	std::map <int, CFishingArea*> m_mFishingArea;
};


