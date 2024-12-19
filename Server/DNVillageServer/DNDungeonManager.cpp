
#include "Stdafx.h"
#include "DNDungeonManager.h"
#include "DNUserSessionManager.h"
#include "DNUserSession.h"
#include "DnWorldGateStruct.h"

CDNDungeonManager* g_pDungeonManager;

CDNDungeonManager::CDNDungeonManager()
{
}

CDNDungeonManager::~CDNDungeonManager()
{
	ScopeLock <CSyncLock> sync(m_Sync);

	std::map <int, DungeonGateStruct*>::iterator ii;
	for (ii = m_DungeonList.begin(); ii != m_DungeonList.end(); ii++)
		SAFE_DELETE((*ii).second);
}

EWorldEnum::PermitGateEnum DungeonGateStruct::CanEnterDungeon( CDNParty * pParty )
{
	UINT nMemberArr[PARTYMAX];
	memset(nMemberArr, 0, sizeof(nMemberArr));

	pParty->GetMembersByAccountDBID(nMemberArr);
	EWorldEnum::PermitGateEnum PermitFlag = EWorldEnum::PermitEnter;
	int nCount = pParty->GetMemberCount();

	if( nMinPartyCount != -1 ) {
		if( nCount < nMinPartyCount ) PermitFlag = (EWorldEnum::PermitGateEnum)( PermitFlag | EWorldEnum::PermitLessPartyCount );
	}
	if( nMaxPartyCount != -1 ) {
		if( nCount > nMaxPartyCount ) PermitFlag = (EWorldEnum::PermitGateEnum)( PermitFlag | EWorldEnum::PermitLessPartyCount );
	}

	for( int i=0; i<PARTYMAX; i++ ) {
		if (nMemberArr[i] <= 0) continue;
		CDNUserSession *pSession = g_pUserSessionManager->FindUserSessionByAccountDBID(nMemberArr[i]);
		if (pSession == NULL) continue;
		// 원래 Attribute 가 항상 갱신됬었는데..지금 이상하게 안된다.
		int nLevel = pSession->GetLevel();

		if( nMinLevel != -1 ) {
			if( nLevel < nMinLevel ) PermitFlag = (EWorldEnum::PermitGateEnum)( PermitFlag | EWorldEnum::PermitLessPlayerLevel );
		}
		if( nMaxLevel != -1 ) {
			if( nLevel > nMaxLevel ) PermitFlag = (EWorldEnum::PermitGateEnum)( PermitFlag | EWorldEnum::PermitLessPlayerLevel );
		}
		if( nNeedItemID > 0 && nNeedItemCount > 0 ) {
			if( pSession->GetItem()->GetInventoryItemCount( nNeedItemID ) < nNeedItemCount ) 
				PermitFlag = (EWorldEnum::PermitGateEnum)( PermitFlag | EWorldEnum::PermitNotEnoughItem );
		}
	}
	return PermitFlag;
}

EWorldEnum::PermitGateEnum DungeonGateStruct::CanEnterDungeon( CDNUserSession * pSession )
{
	EWorldEnum::PermitGateEnum PermitFlag = EWorldEnum::PermitEnter;
	int nCount = 1;

	if( nMinPartyCount != -1 ) {
		if( nCount < nMinPartyCount ) PermitFlag = (EWorldEnum::PermitGateEnum)( PermitFlag | EWorldEnum::PermitLessPartyCount );
	}
	if( nMaxPartyCount != -1 ) {
		if( nCount > nMaxPartyCount ) PermitFlag = (EWorldEnum::PermitGateEnum)( PermitFlag | EWorldEnum::PermitLessPartyCount );
	}

	// 원래 Attribute 가 항상 갱신됬었는데..지금 이상하게 안된다.
	int nLevel = pSession->GetLevel();

	if( nMinLevel != -1 ) {
		if( nLevel < nMinLevel ) PermitFlag = (EWorldEnum::PermitGateEnum)( PermitFlag | EWorldEnum::PermitLessPlayerLevel );
	}
	if( nMaxLevel != -1 ) {
		if( nLevel > nMaxLevel ) PermitFlag = (EWorldEnum::PermitGateEnum)( PermitFlag | EWorldEnum::PermitLessPlayerLevel );
	}
	if( nNeedItemID > 0 && nNeedItemCount > 0 ) {
		if( pSession->GetItem()->GetInventoryItemCount( nNeedItemID ) < nNeedItemCount ) 
			PermitFlag = (EWorldEnum::PermitGateEnum)( PermitFlag | EWorldEnum::PermitNotEnoughItem );
	}
	return PermitFlag;
}

DungeonGateStruct * CDNDungeonManager::GetDungeonPtr(int nMapIdx)
{
	ScopeLock <CSyncLock> sync(m_Sync);

	std::map <int, DungeonGateStruct*>::iterator ii = m_DungeonList.find(nMapIdx);
	if (ii != m_DungeonList.end())
		return (*ii).second;		

	DungeonGateStruct * pDungeon = (DungeonGateStruct*)CalcMapInfo(nMapIdx);
	if (pDungeon != NULL)
	{
		if (pDungeon->MapType == EWorldEnum::MapTypeDungeon)
			m_DungeonList[nMapIdx] = pDungeon;
		else
		{
			SAFE_DELETE(pDungeon);
			return NULL;
		}
	}
	return pDungeon;
};

void CDNDungeonManager::CalcDungeonConstructionLevel(int nMapIndex, DungeonGateStruct * pDungeonStruct, CDNParty * pParty)
{
	DNTableFileFormat *pSox = GetDNTable( CDnTableDB::TMAP );
	DNTableFileFormat *pDungeonSox = GetDNTable( CDnTableDB::TDUNGEONENTER );
	int nTemp = pSox->GetFieldFromLablePtr( nMapIndex, "_EnterConditionTableID" )->GetInteger();
	
	UINT nPartyMemberArr[PARTYMAX];
	memset(nPartyMemberArr, 0, sizeof(nPartyMemberArr));
	pParty->GetMembersByAccountDBID(nPartyMemberArr);

	pDungeonStruct->cCanDifficult[4] = TRUE;
	for (int j = 0; j < PARTYMAX; j++)
	{
		if (nPartyMemberArr[j] <= 0) continue;
		CDNUserSession * pSession = g_pUserSessionManager->FindUserSessionByAccountDBID(nPartyMemberArr[j]);
		if (!pSession) continue;

#if !defined(PRE_DELETE_DUNGEONCLEAR)
		// 리더일 경우에만 요것 체크하구.
		if( pParty->GetLeaderAccountDBID() == pSession->GetAccountDBID() )
			pSession->CalcDungeonEnterLevel( nMapIndex, pDungeonStruct->cCanDifficult[2], pDungeonStruct->cCanDifficult[3] );
#endif	// #if !defined(PRE_DELETE_DUNGEONCLEAR)

		// 어비스 모드 체크한다.
		if( pDungeonStruct->cCanDifficult[4] == TRUE ) {
			if( pDungeonStruct->nAbyssMinLevel != -1 && pSession->GetLevel() < pDungeonStruct->nAbyssMinLevel )
				pDungeonStruct->cCanDifficult[4] |= 0x10;
			if( pDungeonStruct->nAbyssMaxLevel != -1 && pSession->GetLevel() > pDungeonStruct->nAbyssMaxLevel )
				pDungeonStruct->cCanDifficult[4] |= 0x10;
			if( pDungeonStruct->nAbyssNeedQuestID != -1 && !pSession->GetQuest()->IsClearQuest( pDungeonStruct->nAbyssNeedQuestID ) )
				pDungeonStruct->cCanDifficult[4] |= 0x20;
		}
	}

	int nValue = pDungeonSox->GetFieldFromLablePtr( nTemp, "_CanEasyLevel" )->GetInteger();
	if( nValue > 0 ) pDungeonStruct->cCanDifficult[0] = ( pDungeonSox->GetFieldFromLablePtr( nTemp, "_CanEasyLevel" )->GetInteger() == 1 ) ? false : true;
	nValue = pDungeonSox->GetFieldFromLablePtr( nTemp, "_CanNormalLevel" )->GetInteger();
	if( nValue > 0 ) pDungeonStruct->cCanDifficult[1] = ( pDungeonSox->GetFieldFromLablePtr( nTemp, "_CanNormalLevel" )->GetInteger() == 1 ) ? false : true;
	nValue = pDungeonSox->GetFieldFromLablePtr( nTemp, "_CanHardLevel" )->GetInteger();
	if( nValue > 0 ) pDungeonStruct->cCanDifficult[2] = ( pDungeonSox->GetFieldFromLablePtr( nTemp, "_CanHardLevel" )->GetInteger() == 1 ) ? false : true;
	nValue = pDungeonSox->GetFieldFromLablePtr( nTemp, "_CanVeryHardLevel" )->GetInteger();
	if( nValue > 0 ) pDungeonStruct->cCanDifficult[3] = ( pDungeonSox->GetFieldFromLablePtr( nTemp, "_CanVeryHardLevel" )->GetInteger() == 1 ) ? false : true;
	nValue = pDungeonSox->GetFieldFromLablePtr( nTemp, "_CanNightmareLevel" )->GetInteger();
	if( nValue > 0 ) pDungeonStruct->cCanDifficult[4] = ( pDungeonSox->GetFieldFromLablePtr( nTemp, "_CanNightmareLevel" )->GetInteger() == 1 ) ? false : true;
}

void CDNDungeonManager::CalcDungeonConstructionLevel( int nMapIndex, DungeonGateStruct * pDungeonStruct, CDNUserSession * pSession )
{
	DNTableFileFormat *pSox = GetDNTable( CDnTableDB::TMAP );
	DNTableFileFormat *pDungeonSox = GetDNTable( CDnTableDB::TDUNGEONENTER );
	int nTemp = pSox->GetFieldFromLablePtr( nMapIndex, "_EnterConditionTableID" )->GetInteger();

	pDungeonStruct->cCanDifficult[4] = TRUE;
#if !defined(PRE_DELETE_DUNGEONCLEAR)
	pSession->CalcDungeonEnterLevel( nMapIndex, pDungeonStruct->cCanDifficult[2], pDungeonStruct->cCanDifficult[3] );
#endif	// #if !defined(PRE_DELETE_DUNGEONCLEAR)

	// 어비스 모드 체크한다.
	if( pDungeonStruct->cCanDifficult[4] == TRUE ) {
		if( pDungeonStruct->nAbyssMinLevel != -1 && pSession->GetLevel() < pDungeonStruct->nAbyssMinLevel )
			pDungeonStruct->cCanDifficult[4] |= 0x10;
		if( pDungeonStruct->nAbyssMaxLevel != -1 && pSession->GetLevel() > pDungeonStruct->nAbyssMaxLevel )
			pDungeonStruct->cCanDifficult[4] |= 0x10;
		if( pDungeonStruct->nAbyssNeedQuestID != -1 && !pSession->GetQuest()->IsClearQuest( pDungeonStruct->nAbyssNeedQuestID ) )
			pDungeonStruct->cCanDifficult[4] |= 0x20;
	}

	int nValue = pDungeonSox->GetFieldFromLablePtr( nTemp, "_CanEasyLevel" )->GetInteger();
	if( nValue > 0 ) pDungeonStruct->cCanDifficult[0] = ( pDungeonSox->GetFieldFromLablePtr( nTemp, "_CanEasyLevel" )->GetInteger() == 1 ) ? false : true;
	nValue = pDungeonSox->GetFieldFromLablePtr( nTemp, "_CanNormalLevel" )->GetInteger();
	if( nValue > 0 ) pDungeonStruct->cCanDifficult[1] = ( pDungeonSox->GetFieldFromLablePtr( nTemp, "_CanNormalLevel" )->GetInteger() == 1 ) ? false : true;
	nValue = pDungeonSox->GetFieldFromLablePtr( nTemp, "_CanHardLevel" )->GetInteger();
	if( nValue > 0 ) pDungeonStruct->cCanDifficult[2] = ( pDungeonSox->GetFieldFromLablePtr( nTemp, "_CanHardLevel" )->GetInteger() == 1 ) ? false : true;
	nValue = pDungeonSox->GetFieldFromLablePtr( nTemp, "_CanVeryHardLevel" )->GetInteger();
	if( nValue > 0 ) pDungeonStruct->cCanDifficult[3] = ( pDungeonSox->GetFieldFromLablePtr( nTemp, "_CanVeryHardLevel" )->GetInteger() == 1 ) ? false : true;
	nValue = pDungeonSox->GetFieldFromLablePtr( nTemp, "_CanNightmareLevel" )->GetInteger();
	if( nValue > 0 ) pDungeonStruct->cCanDifficult[4] = ( pDungeonSox->GetFieldFromLablePtr( nTemp, "_CanNightmareLevel" )->GetInteger() == 1 ) ? false : true;
}