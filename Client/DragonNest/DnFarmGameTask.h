#pragma once

#include "DnGameTask.h"


class CDnFarmGameTask:public CDnGameTask
{
public:

	CDnFarmGameTask();
	virtual ~CDnFarmGameTask();

	virtual bool Initialize();
	virtual bool InitializeStage( int nCurrentMapIndex, int nGateIndex, int nStageConstructionLevel, int nRandomSeed, bool bContinueStage, bool bDirectConnect );
	virtual void OnInitializeStartPosition( CDnPartyTask::PartyStruct *pStruct, const int iPartyIndex, EtVector3& vStartPos, float& fRotate, const UINT uiSessionID );

	bool OnPostInitializeStage();

	void RequestMoveVillage();

	// Process
	virtual void Process( LOCAL_TIME LocalTime, float fDelta );

	// Dispatch

	// Interface
	virtual int GetFarmZoneNumber();

private:

	std::vector<CDnWorldProp *>	m_vPreInitProp;
};

