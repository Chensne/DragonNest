
#pragma once

#include "DnPlayerChecker.h"

class CDnPlayerSpeedHackChecker:public IDnPlayerChecker, public TBoostMemoryPool<CDnPlayerSpeedHackChecker>
{
public:

	CDnPlayerSpeedHackChecker( CDnPlayerActor* pActor ):IDnPlayerChecker(pActor),m_uiHackCount(0),m_dwCheckTick(0),m_dwGapSum(0),m_uiGapCount(0),m_uiDoubtCount(0)
	{
		m_uiPosHackCount	= 0;
		m_dwLastPosHackTick	= 0;
		m_nInvalidCount = 0;
		m_nValidCount = 0;
		m_dwLastInvalidTick = 0;
		m_bLastCheckPress = false;
		m_fPressDist = 0.f;
		m_LastCheckTime = 0;
		m_dwFirstGap = 0;
		m_dwLastGap = 0;

		m_dwLastMoveSpeedCheckTick = 0;
		m_dwLastMoveSpeedHackTick = 0;
		m_uiMoveSpeedHackCount = 0;
	}

	virtual bool OnSyncDatumGap( const DWORD dwGap );
	virtual bool OnSyncPosition( const EtVector3& vPos );
	virtual bool OnSyncMoveSpeed( const int nMoveSpeed );
	virtual bool IsInvalidPlayer();
	void ResetInvalid();

	virtual void Process( LOCAL_TIME LocalTime, float fDelta );

	void SetCheckPress( bool bCheck, float fDist = 0.f ) { m_bLastCheckPress = bCheck; m_fPressDist += fDist; }
#if defined(PRE_ADD_POSITIONHACK_POS_LOG)
	void CheckInvalidActorAndPositionLog();
#endif	// #if defined(PRE_ADD_POSITIONHACK_POS_LOG)
private:

	UINT	m_uiHackCount;
	DWORD	m_dwCheckTick;
	DWORD	m_dwGapSum;
	UINT	m_uiGapCount;
	UINT	m_uiDoubtCount;
	DWORD	m_dwFirstGap;
	DWORD	m_dwLastGap;
	UINT	m_uiPosHackCount;
	DWORD	m_dwLastPosHackTick;
	DWORD	m_dwLastInvalidTick;

	DWORD   m_dwLastMoveSpeedCheckTick;
	DWORD   m_dwLastMoveSpeedHackTick;
	UINT    m_uiMoveSpeedHackCount;

	LOCAL_TIME m_LastCheckTime;
	int m_nInvalidCount;
	int m_nValidCount;
	float m_fPressDist;
	struct CheckType
	{
		enum
		{
			Average = 0,
			DoubtPacketCount,
			Max,
		};
	};

	bool m_bLastCheckPress;

	struct PosHistory {
		EtVector3 vPos;
		DWORD dwTime;
		int nMoveSpeed;
	};
	std::vector<PosHistory> m_VecPosList;

	void InsertPos( const EtVector3 &vPos, DWORD dwTime, int nMoveSpeed );
	bool IsValidDist();
};
