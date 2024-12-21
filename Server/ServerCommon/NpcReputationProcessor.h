
#pragma once

#if defined( PRE_ADD_NPC_REPUTATION_SYSTEM )

#include "../../GameCommon/ReputationSystem.h"

class CDNUserSession;

class CNpcReputationProcessor
{
public:
	
	static void Process( CDNUserSession* pSession, const int iNpcID, const IReputationSystem::eType Type, REPUTATION_TYPE value );	
	static void PresentProcess( CDNUserSession* pSession, const int iNpcID, const int iPresentID, const int iPresentCount = 1);
	
	// 상점 혜택 계산용 정적 함수.
	static void CheckAndCalcStoreBenefit( CDNUserSession* pUserSession, int iNpcID, TStoreBenefitData::eType Type, /*IN OUT*/ int& iNeedCoin );

	// 연합 혜택 계산용 정적함수
	static void CheckAndCalcUnionBenefit( CDNUserSession* pUserSession, TStoreBenefitData::eType Type, /*IN OUT*/ int& iNeedPoint );
	static void UseUnionReputePoint( CDNUserSession* pSession, int nType, int nUsePoint );

private:

#if defined ( PRE_ADD_REPUTATION_EXPOSURE )
	static bool _bIsCheckMission( CDNUserSession* pSession, const int iNpcID );
#else
	static bool _bIsCheckQuest( CDNUserSession* pSession, const int iNpcID );
#endif // #if defined ( PRE_ADD_REPUTATION_EXPOSURE )

	static BYTE _ConvertUnionToPointType( int nType );
	static void _AddUnionReputePoint( CDNUserSession* pSession, int iUnionID, int nUnionPoint );
};

#endif // #if defined( PRE_ADD_NPC_REPUTATION_SYSTEM )
