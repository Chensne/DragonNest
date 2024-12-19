
#include "StdAfx.h"
#include "DNParty.h"
#include "DNUserSession.h"
#include "DNUserSessionManager.h"
#include "DNUserSendManager.h"
#include "DNMasterConnection.h"
#include "DNGameDataManager.h"
#include "Log.h"
#include "DNAuthManager.h"
#include "DNEvent.h"
#include "DNFriend.h"
#include "DNPartyManager.h"
#include "DNDBConnection.h"
#ifdef _USE_VOICECHAT
#include "DNVoiceChat.h"
#endif
#ifdef PRE_ADD_BEGINNERGUILD
#include "DNGuildSystem.h"
#endif		//#ifdef PRE_ADD_BEGINNERGUILD

extern TVillageConfig g_Config;

CDNParty::CDNParty(ULONG nCurTick)
{
	memset(m_nPartyMemberArray, 0, sizeof(m_nPartyMemberArray));
	memset(m_bPartyCheckMemberArray, 0, sizeof(m_bPartyCheckMemberArray));

	m_nLeaderSessionID = m_nLeaderAccountDBID = 0;
#if defined( PRE_PARTY_DB )
	m_Data.Clear();
#else
	m_PartyID = 0;
	m_nPartyMapIndex = m_nChannelID = m_nChannelMeritID = m_nRandomSeed = 0;
	memset(m_wszPartyPassword, 0, sizeof(m_wszPartyPassword));
	m_bRaidParty = false;
	m_cIsJobDice = 0;
	m_nMemberMax = PARTYMAX;
	m_nUserLvLimitMin = m_nUserLvLimitMax = 0;
	memset(m_wszPartyName, 0, sizeof(m_wszPartyName));
	m_Difficulty = 0;
	m_ItemLootRank = 0;
	m_ItemLootRule = 0;
	m_nTargetMapIdx = 0;
	m_nUpkeepCount = 0;
#endif // #if defined( PRE_PARTY_DB )
	m_boCompleteMember = false;
	m_bIsMove = m_bIsStartGame = false;

	m_nPreTick = 0;
	m_nCreateTick = nCurTick;

#ifdef _USE_VOICECHAT
	m_nVoiceChannelID = 0;
	m_nTalkingTick = 0;
	memset(m_MemberVoiceInfo, 0, sizeof(m_MemberVoiceInfo));
#endif

	m_nCurTick = 0;
}

CDNParty::~CDNParty(void)
{
	ScopeLock<CSyncLock> Lock(m_Sync);

	std::vector < std::pair <ULONG, WCHAR*> >::iterator ii;
	for (ii = m_VecInviteList.begin(); ii != m_VecInviteList.end(); ii++)
		SAFE_DELETEA((*ii).second);
	m_VecInviteList.clear();
}

#if defined( PRE_PARTY_DB )

void CDNParty::Create( TPARTYID PartyID, const DBCreateParty* pData )
{
	m_Data = pData->PartyData;
#if defined( PRE_WORLDCOMBINE_PARTY )
	if( Party::bIsWorldCombineParty(m_Data.Type) )
	{
		m_Data.nPrimaryIndex = m_Data.iTargetMapIndex;
	}
#endif
}

#else

void CDNParty::Create(CDNUserSession * pUser, TPARTYID PartyID, const WCHAR * pPartyName, const WCHAR * pPassWord, int nMemberMax, TPARTYITEMLOOTRULE ItemLootRule, TITEMRANK ItemRank, int nLvLimitMin, int nLvLimitMax, int nTargetMapIdx, TDUNGEONDIFFICULTY Difficulty, BYTE cIsPartyJobDice)
{
	SetPartyID( PartyID );
	m_nChannelID = pUser->GetChannelID();
	m_nChannelMeritID = g_pMasterConnection->GetMeritBonusID(m_nChannelID);
	m_nPartyMapIndex = g_pMasterConnection->GetChannelMapIndex(m_nChannelID);

	m_ItemLootRule = ItemLootRule;
	m_ItemLootRank = ItemRank;
	m_nUserLvLimitMin = nLvLimitMin;
	m_nUserLvLimitMax = nLvLimitMax;

	m_nTargetMapIdx = nTargetMapIdx;
	m_Difficulty = Difficulty;
	m_cIsJobDice = cIsPartyJobDice;

	_wcscpy(m_wszPartyName, _countof(m_wszPartyName), pPartyName, (int)wcslen(pPartyName));
	_wcscpy(m_wszPartyPassword, _countof(m_wszPartyPassword), pPassWord, (int)wcslen(pPassWord));

	m_nMemberMax = nMemberMax;
	m_bRaidParty = (m_nMemberMax >= RAIDPARTYCOUNTMIN) ? true : false;
}

#endif // #if defined( PRE_PARTY_DB )

#if defined( PRE_PARTY_DB )
void CDNParty::PushParty( TAAddPartyAndMemberVillage* pPacket )
{	
	m_Data = pPacket->Data.PartyData;
	for( int i=0; i < pPacket->Data.PartyData.iCurMemberCount; i++)
	{
		if( SetPartyMemberID(pPacket->nAccountDBID, pPacket->MemberInfo[i].nAccountDBID, pPacket->MemberInfo[i].cMemberIndex) == false)
			_DANGER_POINT();
	}

	for( int i = 0; i < PARTYKICKMAX; i++)
	{
		if (pPacket->nKickedMemberList[i] <= 0) 
			continue;
		m_VecKickList.push_back(pPacket->nKickedMemberList[i]);
	}	
}
#else
void CDNParty::PushParty( MAVIPushParty* pPacket )
{
	SetPartyID( pPacket->PartyID );

	m_ItemLootRule = pPacket->ItemLootRule;
	m_ItemLootRank = pPacket->ItemLootRank;
	m_nUserLvLimitMin = pPacket->nUserLvLimitMin;
	m_nUserLvLimitMax = pPacket->nUserLvLimitMax;

	m_nTargetMapIdx = pPacket->nTargetMapIdx;
	m_Difficulty = pPacket->Difficulty;
	m_cIsJobDice = pPacket->cIsPartyJobDice;
	_wcscpy(m_wszPartyPassword, _countof(m_wszPartyPassword), pPacket->wszPartyPass, (int)wcslen(pPacket->wszPartyPass));
	m_bRaidParty = (pPacket->cMemberMax >= RAIDPARTYCOUNTMIN) ? true : false;
	m_nChannelID = pPacket->nChannelID;
	m_nChannelMeritID = g_pMasterConnection->GetMeritBonusID(pPacket->nChannelID);
	m_nPartyMapIndex = g_pMasterConnection->GetChannelMapIndex(pPacket->nChannelID);

	_wcscpy(m_wszPartyName, _countof(m_wszPartyName), pPacket->wszPartyName, (int)wcslen(pPacket->wszPartyName));

	m_nMemberMax = pPacket->cMemberMax;
	m_nRandomSeed = pPacket->nRandomSeed;
	m_nUpkeepCount = pPacket->cUpkeepCount;

	for( int i=0; i < pPacket->cMemberCount; i++)
	{
		if( SetPartyMemberID(pPacket->nLeaderAccountDBID, pPacket->MemberInfo[i].nAccountDBID, pPacket->MemberInfo[i].cMemberIndex) == false)
			_DANGER_POINT();
	}

	for( int i = 0; i < PARTYKICKMAX; i++)
	{
		if (pPacket->nKickedMemberList[i] <= 0) 
			continue;
		m_VecKickList.push_back(pPacket->nKickedMemberList[i]);
	}
}
#endif

bool CDNParty::DoUpdate(DWORD CurTick)
{
#if defined( PRE_PARTY_DB )
#pragma message( "같은 마을에 있는 파티만 DoUpdate 해준다." )
#endif // #if defined( PRE_PARTY_DB )
#ifdef _USE_VOICECHAT
	TalkingUpdate(CurTick);
#endif

	if (m_nPreTick + (1000 * 60) < CurTick)
	{
		//너무 자주 돌아 봤자~ 1분에 한번씩 일단 체킹하며 초대한지 5분이 지난 녀석은 지워 줍니다.
		ScopeLock<CSyncLock> Lock(m_Sync);
		std::vector < std::pair <ULONG, WCHAR*> >::iterator ii;
		for (ii = m_VecInviteList.begin(); ii != m_VecInviteList.end();)
		{
			if ((*ii).first + (1000 * 60) < CurTick)
			{
				SAFE_DELETEA((*ii).second);
				ii = m_VecInviteList.erase(ii);
			}
			else
				ii++;
		}
		m_nPreTick = CurTick;

		//파티가 추가되고 게임(또는 다른 빌리지)에서 넘어오는 인원이 다 넘어오지 못할 경우 꼬이거나 또는 유령으로 남아 있을 수 있다.
		if (m_nCreateTick > 0 && m_nCreateTick + (1000*60*2) < CurTick)
		{
			//2분동안 처리가 안되고 있으면 정리해준다.
			if (AdjustmentPartyInfo())
			{
#if defined( PRE_PARTY_DB )
#else
				if (HasEmptyPlace())
					g_pPartyManager->PushWaitPartyList(this);
#endif // #if defined( PRE_PARTY_DB )
				return true;
			}
#if defined( PRE_PARTY_DB )
			else
			{
				return false;
			}
#else
			return false;
#endif
		}
	}
	return true;
}

bool CDNParty::AddPartyMember(UINT nAccountDBID, UINT nSessionID, const WCHAR * pMemberName, int &nMemberIdx, bool boLeader)
{
	if (m_bIsMove || m_bIsStartGame) return false;
	if (m_boCompleteMember || nAccountDBID == 0) return false;

	if (boLeader){
		m_nLeaderSessionID = nSessionID;
		m_nLeaderAccountDBID = nAccountDBID;
	}

	//Verify
	if (GetMemberCount() >= GetMemberMax() )
		return false;

	for (int i = 0; i < PARTYCOUNTMAX; i++)
	{
		if (m_nPartyMemberArray[i] <= 0) continue;
		if (m_nPartyMemberArray[i] == nAccountDBID)
		{
			_DANGER_POINT();
			return false;
		}
	}

#if defined( PRE_ADD_NEWCOMEBACK )
	CDNUserSession * pJoinSession = g_pUserSessionManager->FindUserSessionByAccountDBID(nAccountDBID);
	if( pJoinSession && bIsComebackParty() )
	{
		ApplyJoinMemberAppellation(pJoinSession);
	}	
	if( pJoinSession && pJoinSession->GetComebackAppellation() > 0 )
	{
		if( !CheckPartyMemberAppellation( pJoinSession->GetComebackAppellation() ) )
		{
			TAppellationData *pAData = g_pDataManager->GetAppellationData( pJoinSession->GetComebackAppellation() );
			if( pAData )
			{
				CSUseItem pPacket;
				memset(&pPacket, 0, sizeof(pPacket));
				pPacket.biInvenSerial = pJoinSession->GetItem()->MakeItemSerial();
				TItemData* pBuffItemData = g_pDataManager->GetItemData( pAData->nSkillItemID );
				if( pBuffItemData )
				{
					ApplyPartyEffectSkillItemData(&pPacket, pBuffItemData, pJoinSession->GetSessionID(), EffectSkillNameSpace::ShowEffectType::NONEEFFECT, false, true);					
					SetComebackParty(true);
				}		
			}			
		}
	}
#endif

	bool bCheck = false;
	for (int i = 0; i < PARTYCOUNTMAX; i++)
	{
		if (m_nPartyMemberArray[i] > 0) continue;
		m_nPartyMemberArray[i] = nAccountDBID;
		m_bPartyCheckMemberArray[i] = true;
		nMemberIdx = i;
		bCheck = true;
		break;
	}

	if (bCheck == false) return false;
	
	if (pMemberName)	
		InviteDenied(pMemberName);

#ifdef _USE_VOICECHAT
	if (m_nVoiceChannelID > 0)
	{
		CDNUserSession * pSession = g_pUserSessionManager->FindUserSessionByAccountDBID(nAccountDBID);
		if (!pSession) return false;
		if (pSession->JoinVoiceChannel(m_nVoiceChannelID, pSession->GetIp(), &pSession->GetTargetPos(), (int)pSession->m_nVoiceRotate))
			SendRefreshVoiceInfo();
	}
#endif

	if (boLeader)
	{
		CDNUserSession * pUserObj = g_pUserSessionManager->FindUserSessionByAccountDBID(nAccountDBID);
		if (pUserObj)
			pUserObj->SendUserLocalMessage(0, FM_ENTERPARTY);
	}
	return true;
}

bool CDNParty::DelPartyMember(CDNUserSession * pUserObj, char cKickKind)
{
	if (pUserObj == NULL)
	{
		_DANGER_POINT();
		return false;
	}

	if ((pUserObj->m_nPartyMemberIndex < 0) || (pUserObj->m_nPartyMemberIndex >= PARTYMAX))
	{
		_DANGER_POINT();
		return false;
	}
	
	for (int i = 0; i < PARTYMAX; i++)
	{
		if (m_nPartyMemberArray[i] == 0) continue;
		CDNUserSession * pOtherUser = g_pUserSessionManager->FindUserSessionByAccountDBID(m_nPartyMemberArray[i]);
		if (!pOtherUser) continue;

		if (m_bIsMove == false)
			pOtherUser->SendPartyOut(ERROR_NONE, cKickKind, pUserObj->GetSessionID());
	}
	
	if (pUserObj->GetAccountDBID() == m_nLeaderAccountDBID)
		ChangePartyLeader();

	int nDelIndex = pUserObj->m_nPartyMemberIndex;
	m_nPartyMemberArray[pUserObj->m_nPartyMemberIndex] = 0;
	m_bPartyCheckMemberArray[pUserObj->m_nPartyMemberIndex] = false;
	pUserObj->ClearPartyInfo();

	//처리가 다데었으면 인덱스를 스왑처리해줘야한다아~
	if (m_bIsMove == false)
		SortMemberIndex(nDelIndex);

	if (cKickKind == 1)	
		m_VecKickList.push_back(pUserObj->GetAccountDBID());

#ifdef _USE_VOICECHAT
	if (m_nVoiceChannelID > 0)
	{
		pUserObj->LeaveVoiceChannel();
		SendRefreshVoiceInfo(pUserObj->GetAccountDBID());
	}
#endif
	pUserObj->SendPartyBonusValue(0, 0);

	if (GetUpkeepCount() > 0 && GetMemberCount() <= 1)
	{
		SetUpkeepCount( 0 );
	}

	pUserObj->SendUserLocalMessage(0, FM_LEAVEPARTY);

#if defined( PRE_ADD_NEWCOMEBACK )
	if( bIsComebackParty() )
		DelOutMemberAppellation(pUserObj);
	if( pUserObj->GetComebackAppellation() > 0 )
	{
		DelPartyMemberAppellation(pUserObj->GetComebackAppellation());
	}
#endif

	return true;
}

#if defined( PRE_PARTY_DB )
bool CDNParty::DelPartyMember(const TAOutParty* pA, char cKickKind)
{	
	for (int i = 0; i < PARTYMAX; i++)
	{
		if (m_nPartyMemberArray[i] == 0) continue;
		CDNUserSession * pOtherUser = g_pUserSessionManager->FindUserSessionByAccountDBID(m_nPartyMemberArray[i]);
		if (!pOtherUser) continue;

		if (m_bIsMove == false)
			pOtherUser->SendPartyOut(ERROR_NONE, cKickKind, pA->nSessionID);
	}

	if (pA->nAccountDBID == m_nLeaderAccountDBID)
		ChangePartyLeader();

	int nDelIndex = 0;
	for(int i=0;i<PARTYMAX;i++)
	{
		if( m_nPartyMemberArray[i] == pA->nAccountDBID )
		{
			nDelIndex = i;
			break;
		}
	}
	m_nPartyMemberArray[nDelIndex] = 0;
	m_bPartyCheckMemberArray[nDelIndex] = false;	

	//처리가 다데었으면 인덱스를 스왑처리해줘야한다아~
	if (m_bIsMove == false)
		SortMemberIndex(nDelIndex);
	
#ifdef _USE_VOICECHAT
	if (m_nVoiceChannelID > 0)
	{		
		SendRefreshVoiceInfo( pA->nAccountDBID );
	}
#endif	

	if (GetUpkeepCount() > 0 && GetMemberCount() <= 1)
	{
		SetUpkeepCount(0);
	}
	return true;
}
#endif

void CDNParty::UpdateMasterSystemCountInfo()
{
	for( int i=0 ; i<PARTYCOUNTMAX; ++i )
	{
		if( m_nPartyMemberArray[i] <= 0 ) 
			continue;

		CDNUserSession* pSession = g_pUserSessionManager->FindUserSessionByAccountDBID( m_nPartyMemberArray[i] );
		if( pSession )
			pSession->QueryGetMasterSystemCountInfo( true );
	}
}

#if defined( PRE_ADD_NAMEDITEM_SYSTEM )
void CDNParty::SendPartyEffectSkillItemData()
{
	CDNUserSession *pUserObj = NULL;
	for (int i = 0; i < PARTYMAX; i++)
	{
		if (m_nPartyMemberArray[i] == 0) continue;

		pUserObj = g_pUserSessionManager->FindUserSessionByAccountDBID(m_nPartyMemberArray[i]);
		if (!pUserObj) continue;

		if (pUserObj->GetPartyID() != GetPartyID())
		{
			m_nPartyMemberArray[i] = 0;
			m_bPartyCheckMemberArray[i] = false;
			_ASSERT_EXPR(0, L"명재를 찾아주세요!");
			continue;
		}

		std::vector<TEffectSkillData> vEffectSkill;
		vEffectSkill.clear();
		pUserObj->GetItem()->GetEffectSkillItem( vEffectSkill );
		for (int i = 0; i < PARTYMAX; i++)
		{
			CDNUserSession *pUser = NULL;
			if (m_nPartyMemberArray[i] == 0) continue;
			pUser = g_pUserSessionManager->FindUserSessionByAccountDBID(m_nPartyMemberArray[i]);
			if (!pUser) continue;
			if(pUserObj == pUser) continue;			
			pUser->SendEffectSkillItemData(pUserObj->GetSessionID(), vEffectSkill, false);

		}
	}	
}
void CDNParty::ApplyPartyEffectSkillItemData( CSUseItem *pPacket, TItemData *pItemData, UINT nUseSessionID, EffectSkillNameSpace::ShowEffectType::eType eType, bool bSendDB, bool bUseItem )
{	
	CDNUserSession *pUserObj = NULL;

	for (int i = 0; i < PARTYMAX; i++)
	{
		if (m_nPartyMemberArray[i] == 0) continue;

		pUserObj = g_pUserSessionManager->FindUserSessionByAccountDBID(m_nPartyMemberArray[i]);
		if (!pUserObj) continue;		

		if (pUserObj->GetPartyID() != GetPartyID())
		{
			m_nPartyMemberArray[i] = 0;
			m_bPartyCheckMemberArray[i] = false;
			_ASSERT_EXPR(0, L"명재를 찾아주세요!");
			continue;
		}

		if( pUserObj->GetItem() )
		{
			pUserObj->GetItem()->ApplyPartyEffectSkillItemData( pPacket, pItemData, nUseSessionID, eType, bSendDB, bUseItem );					
		}
	}
	SendAllRefreshParty(PARTYREFRESH_NONE);
}
void CDNParty::RemovePartyEffectSkillItemData( int nSkillID, int nItemID )
{	
	CDNUserSession *pUserObj = NULL;

	for (int i = 0; i < PARTYMAX; i++)
	{
		if (m_nPartyMemberArray[i] == 0) continue;

		pUserObj = g_pUserSessionManager->FindUserSessionByAccountDBID(m_nPartyMemberArray[i]);
		if (!pUserObj) continue;		

		if (pUserObj->GetPartyID() != GetPartyID())
		{
			m_nPartyMemberArray[i] = 0;
			m_bPartyCheckMemberArray[i] = false;
			_ASSERT_EXPR(0, L"명재를 찾아주세요!");
			continue;
		}

		if( pUserObj->GetItem() )
		{
			TEffectSkillData* EffectSkill = pUserObj->GetItem()->GetEffectSkillItem( EffectSkillNameSpace::SearchType::SkillID, nSkillID );
			if( EffectSkill )
			{
				pUserObj->GetItem()->DelEffectSkillItem( nSkillID );				
			}			
		}			
	}	
}
#endif

bool CDNParty::SetPartyMemberID(UINT nLeaderDBID, UINT nAccountDBID, int nMemberIndex)
{
	if ((nMemberIndex < 0) || (nMemberIndex >= PARTYCOUNTMAX)) return false;

	//verify
	for (int i = 0; i < PARTYMAX; i++)
	{
		if (m_nPartyMemberArray[i] <= 0) continue;
		if (m_nPartyMemberArray[i] == nAccountDBID)
		{
			_DANGER_POINT();
			return false;
		}
	}

	m_nLeaderAccountDBID = nLeaderDBID;
	if (m_nPartyMemberArray[nMemberIndex] == 0)
	{
		//이동시 최초 아이디세팅이므로 이쪽에서는 항상 비어있어야 한다.
		m_nPartyMemberArray[nMemberIndex] = nAccountDBID;
		m_bPartyCheckMemberArray[nMemberIndex] = false;
	}
	else 
	{
		_DANGER_POINT();		//인덱스를 할당받지 않고 게임에서 넘어오는 값으로 사용하므로 검증 조건을 하나 추가
		return false;
	}
	return true;
}

#if defined( PRE_PARTY_DB )
bool CDNParty::ModifyPartyInfo(int nChannelID, const WCHAR * pName, int iPassword, BYTE cPartyMemberMax, TPARTYITEMLOOTRULE LootRule, TITEMRANK LootRank, BYTE cUserLvLimitMin, int nTargetMapIdx, TDUNGEONDIFFICULTY Difficulty, int iBitFlag, int &nWaitListRet)
#else
bool CDNParty::ModifyPartyInfo(int nChannelID, const WCHAR * pName, const WCHAR * pPass, BYTE cPartyMemberMax, TPARTYITEMLOOTRULE LootRule, TITEMRANK LootRank, BYTE cUserLvLimitMin, BYTE cUserLvLimitMax, int nTargetMapIdx, TDUNGEONDIFFICULTY Difficulty, BYTE cJobDice, int &nWaitListRet)
#endif // #if defined( PRE_PARTY_DB )
{
	SetChannelID( nChannelID );
#if defined( PRE_PARTY_DB )
	_wcscpy( m_Data.wszPartyName, _countof(m_Data.wszPartyName), pName, static_cast<int>(wcslen(pName)) );
#else
	_wcscpy(m_wszPartyName, _countof(m_wszPartyName), pName, (int)wcslen(pName));
#endif // #if defined( PRE_PARTY_DB )

	nWaitListRet = _PARTY_MOD_RET_NONE;
	if( GetMemberMax() != cPartyMemberMax)
	{
		nWaitListRet = GetMemberMax() > cPartyMemberMax ? _PARTY_MOD_RET_DEL : _PARTY_MOD_RET_ADD;

		SetMemberMax( cPartyMemberMax );

		if (nWaitListRet == _PARTY_MOD_RET_ADD)
		{
			if (!HasEmptyPlace())
				nWaitListRet = _PARTY_MOD_RET_DEL;
		}
		else if (nWaitListRet == _PARTY_MOD_RET_DEL)
		{
			if (HasEmptyPlace())
				nWaitListRet = _PARTY_MOD_RET_ADD;
		}
	}
	else
	{
		SetMemberMax( cPartyMemberMax );
	}


#if defined( PRE_PARTY_DB )
	m_Data.iPassword = iPassword;
	m_Data.iBitFlag = iBitFlag;
	m_Data.cMinLevel = cUserLvLimitMin;
	m_Data.LootRule = LootRule;
	m_Data.LootItemRank = LootRank;
	m_Data.iTargetMapIndex = nTargetMapIdx;
	m_Data.TargetMapDifficulty = Difficulty;
#else
	_wcscpy(m_wszPartyPassword, _countof(m_wszPartyPassword), pPass, (int)wcslen(pPass));
	m_cIsJobDice = cJobDice;
	m_nUserLvLimitMin = cUserLvLimitMin;
	m_nUserLvLimitMax = cUserLvLimitMax;
	m_ItemLootRule = LootRule;
	m_ItemLootRank = LootRank;
	m_nTargetMapIdx = nTargetMapIdx;
	m_Difficulty = Difficulty;
#endif // #if defined( PRE_PARTY_DB )

	return true;
}

int CDNParty::GetPartyMemberIdx(UINT nAccountDBID)
{
	if (nAccountDBID == 0) return -1;

	for (int i = 0; i < PARTYMAX; i++)
	{
		if (m_nPartyMemberArray[i] == nAccountDBID)
		{
			m_bPartyCheckMemberArray[i] = true;
			return i;
		}
	}
	return -1;
}

bool CDNParty::bIsPasswordParty() const
{
#if defined( PRE_PARTY_DB )
	return (m_Data.iBitFlag&Party::BitFlag::Password) ? true : false;
#else
	return wcslen(m_wszPartyPassword)>0;
#endif // #if defined( PRE_PARTY_DB )
}

bool CDNParty::IsKickedMember(UINT nAccountDBID)
{
	for (UINT i = 0; i < m_VecKickList.size(); i++)
	{
		if (m_VecKickList[i] == nAccountDBID)
			return true;
	}
	return false;
}

#if defined( PRE_PARTY_DB )

bool CDNParty::CheckPassword( int iPassword )
{
	return GetPassword() == iPassword;
}

void CDNParty::GetCharNameList( std::vector<std::wstring>& vCharName )
{
	CDNUserSession *pUserObj = NULL;
	for (int i = 0; i < PARTYMAX; i++)
	{
		if (m_nPartyMemberArray[i] == 0) 
			continue;

		pUserObj = g_pUserSessionManager->FindUserSessionByAccountDBID(m_nPartyMemberArray[i]);
		if(!pUserObj) 
			continue;
		vCharName.push_back( pUserObj->GetCharacterName() );
	}
}

#else

bool CDNParty::CheckPassword( const WCHAR* pwszPassword )
{
	return wcscmp(m_wszPartyPassword, pwszPassword)?false:true;
}

#endif // #if defined( PRE_PARTY_DB )

bool CDNParty::HasEmptyPlace()
{
	int nCount = 0;
	for (int i = 0; i < PARTYCOUNTMAX; i++)
	{
		if (m_nPartyMemberArray[i] > 0)
			nCount++;
	}

	if (nCount >= GetMemberMax() )
		return false;
	return true;
}

void CDNParty::BanAllPartyMember(UINT nLeaderAccountDBID)
{
	CDNUserSession *pUserObj = NULL;
	for (int i = 0; i < PARTYMAX; i++){
		if (m_nPartyMemberArray[i] == 0) continue;

		pUserObj = g_pUserSessionManager->FindUserSessionByAccountDBID(m_nPartyMemberArray[i]);
		if (!pUserObj) continue;

		pUserObj->ClearPartyInfo();
		//if (pUserObj->GetAccountDBID() != nLeaderAccountDBID)
			pUserObj->SendPartyOut(ERROR_NONE, -1, pUserObj->GetSessionID());	// 강퇴시켜버린다. (리더는 방을 뽀갠 사람이라 보낼 필요 없다)
	}

	memset(&m_nPartyMemberArray, 0, sizeof(m_nPartyMemberArray));
	memset(&m_bPartyCheckMemberArray, 0, sizeof(m_bPartyCheckMemberArray));
}

int CDNParty::GetMemberInfo(SPartyMemberInfo *pInfo)
{
	int nCount = 0;

	CDNUserSession *pUserObj = NULL;
	for (int i = 0; i < PARTYMAX; i++){
		if (m_nPartyMemberArray[i] == 0) continue;

		pUserObj = g_pUserSessionManager->FindUserSessionByAccountDBID(m_nPartyMemberArray[i]);
		if (!pUserObj)	continue;
		if (pUserObj->GetPartyID() != GetPartyID()) 
			continue;
		
		pUserObj->GetPartyMemberInfo(pInfo[nCount]);
		nCount++;
	}

	return nCount;
}

void CDNParty::GetMemberCharacterDBIDInfo( std::vector<INT64>& Data, INT64 biExceptCharDBID/*=0*/ )
{
	for (int i = 0; i < PARTYMAX; i++)
	{
		if (m_nPartyMemberArray[i] == 0) 
			continue;

		CDNUserSession* pUserObj = g_pUserSessionManager->FindUserSessionByAccountDBID(m_nPartyMemberArray[i]);
		if( !pUserObj )	
			continue;
		if( biExceptCharDBID > 0 )
		{
			if( pUserObj->GetCharacterDBID() == biExceptCharDBID )
				continue;
		}

		Data.push_back( pUserObj->GetCharacterDBID() );
	}
}

int CDNParty::GetMemberPartsInfo(int *EquipArray)
{
	int nCount = 0;

	CDNUserSession *pUserObj = NULL;
	for (int i = 0; i < PARTYMAX; i++){
		if (m_nPartyMemberArray[i] == 0) continue;

		pUserObj = g_pUserSessionManager->FindUserSessionByAccountDBID(m_nPartyMemberArray[i]);
		if (!pUserObj) continue;

		pUserObj->GetItem()->GetEquipIDs(EquipArray);
		nCount++;
	}

	return nCount;
}

void CDNParty::SetMemberTargetMapIndexByStartStage()
{
	CDNUserSession *pUserObj = NULL;
	int nTargetMapIndex = 0;
	for (int i = 0; i < PARTYMAX; i++){
		if (m_nPartyMemberArray[i] == 0) continue;

		pUserObj = g_pUserSessionManager->FindUserSessionByAccountDBID(m_nPartyMemberArray[i]);
		if (!pUserObj) continue;

		pUserObj->SetTargetMapIndexByStartStage();
	}
}

int CDNParty::CheckSameGateNo()
{
	int GateNo = -1;

	int nCount = 0;
	CDNUserSession *pUserObj = NULL;
	for (int i = 0; i < PARTYMAX; i++){
		if (m_nPartyMemberArray[i] == 0) continue;

		pUserObj = g_pUserSessionManager->FindUserSessionByAccountDBID(m_nPartyMemberArray[i]);
		if (!pUserObj) continue;

		if (nCount == 0) GateNo = pUserObj->m_cGateNo;
		else if (GateNo != pUserObj->m_cGateNo) return -1;
		nCount++;
	}

	m_boCompleteMember = GateNo > 0 ? true : false;
	return GateNo;
}

int CDNParty::GetMemberCount() const
{
	int nCount = 0;
	for (int i = 0; i < PARTYMAX; i++){
		if (m_nPartyMemberArray[i] == 0) continue;
		if (m_bPartyCheckMemberArray[i] == false) continue;	//인덱스에는 존재 하지만 실재로 서버에는 없는 인원
		nCount++;
	}

	return nCount;
}

void CDNParty::GetMembersByAccountDBID(UINT *nMembers)
{
	memcpy(nMembers, m_nPartyMemberArray, sizeof(m_nPartyMemberArray));
}

void CDNParty::GetMemberCheckFlag(bool * bMembers)
{
	memcpy(bMembers, m_bPartyCheckMemberArray, sizeof(m_bPartyCheckMemberArray));
}

bool CDNParty::DiffMemberGateNo(char cGateNo)
{
	if (cGateNo == -1)
		return false;

	bool bValid = true;
	CDNUserSession *pUserObj = NULL;
	for (int i = 0; i < PARTYMAX; i++){
		if (m_nPartyMemberArray[i] == 0) continue;

		pUserObj = g_pUserSessionManager->FindUserSessionByAccountDBID(m_nPartyMemberArray[i]);
		if (!pUserObj){
			bValid = false;
			break;
		}
		if (pUserObj->m_cGateNo != cGateNo) {
			bValid = false;
			break;
		}
	}
	return bValid;
}

bool CDNParty::InvitePartyMember(const WCHAR * pMemberName)
{
	ScopeLock<CSyncLock> Lock(m_Sync);

	std::vector < std::pair <ULONG, WCHAR*> >::iterator ii;
	for (ii = m_VecInviteList.begin(); ii != m_VecInviteList.end(); ii++)
		if (!__wcsicmp_l((*ii).second, pMemberName))
			return false;

	WCHAR * pBuf = new WCHAR[NAMELENMAX];
	_wcscpy(pBuf, NAMELENMAX, pMemberName, (int)wcslen(pMemberName));
	m_VecInviteList.push_back(std::make_pair(timeGetTime(), pBuf));
	
	return true;
}

bool CDNParty::InviteDenied(const WCHAR * pMemberName)
{
	if (m_VecInviteList.empty()) return false;

	ScopeLock<CSyncLock> Lock(m_Sync);

	std::vector < std::pair <ULONG, WCHAR*> >::iterator ii;
	for (ii = m_VecInviteList.begin(); ii != m_VecInviteList.end(); ii++)
	{
		if (!__wcsicmp_l((*ii).second, pMemberName))
		{
			SAFE_DELETEA((*ii).second);
			m_VecInviteList.erase(ii);
			return true;
		}
	}
	
	return false;
}

#if defined( PRE_PARTY_DB )
int CDNParty::GetPartyInfo(SPartyListInfo * pInfo)
#else
int CDNParty::GetPartyInfo(SPartyListInfo * pInfo, const sChannelInfo * pChInfo)
#endif // #if defined( PRE_PARTY_DB )
{
#if defined( PRE_PARTY_DB )
	if (m_bIsMove || m_bIsStartGame ) return -1;
#else
	if (m_bIsMove || m_bIsStartGame || pChInfo == NULL) return -1;
#endif // #if defined( PRE_PARTY_DB )

	int LowMapIndex = 0;
	int nCount = 0;
	int nLeaderLevel = 0;
	USHORT nFatigue = 0;

#if defined( PRE_PARTY_DB )
#else
	CDNUserSession *pUserObj = NULL;
	for (int i = 0; i < PARTYMAX; i++){
		if (m_nPartyMemberArray[i] == 0) continue;

		CDNUserSession * pUser = g_pUserSessionManager->FindUserSessionByAccountDBID(m_nPartyMemberArray[i]);
		if (pUser)
		{
			if (m_nLeaderAccountDBID == pUser->GetAccountDBID())
				nFatigue = pUser->GetFatigue();
			nCount++;
		}
		else
			return -1;
	}

	if (nCount <= 0)
	{
		if (m_nCreateTick == 0)
			return -2;
		return -1;
	}
#endif // #if defined( PRE_PARTY_DB )

	pInfo->PartyID = GetPartyID();
	pInfo->cUserLvLimitMin = GetMinLevel();
#if defined( PRE_PARTY_DB )
	if( m_Data.LocationType == Party::LocationType::Worldmap )
		pInfo->iCurMapIndex = m_Data.iLocationID;
	else
		pInfo->iCurMapIndex = g_pMasterConnection->GetChannelMapIndex( m_Data.iLocationID );

	if( m_Data.iServerID == g_Config.nManagedID )
		pInfo->cCurrentCount = GetMemberCount();
	else
		pInfo->cCurrentCount = m_Data.iCurMemberCount;
	pInfo->bIsBonus = GetUpkeepCount()>0 ? true : false;
#if defined( _FINAL_BUILD )
#else
	pInfo->biSortPoint = m_biSortPoint;
#endif // #if defined( _FINAL_BUILD )
#else
	pInfo->cUserLvLimitMax = GetMaxLevel();
	pInfo->nChannelMaxUser = pChInfo->nMaxUserCount;
	pInfo->nChannelCurUser = pChInfo->nCurrentUserCount;
	pInfo->wMasterFatigue = nFatigue;
	pInfo->cCurrentCount = nCount;
#endif // #if defined( PRE_PARTY_DB )
	pInfo->cMaxCount = GetMemberMax();
	pInfo->cIsPrivateRoom = bIsPasswordParty() ? 1 : 0;
	pInfo->nTargetMapIdx = GetTargetMapIndex();
	pInfo->TargetDifficulty = GetDifficulty();
#ifdef _USE_VOICECHAT
	pInfo->cUseVoice = m_nVoiceChannelID > 0 ? 1 : 0;
#endif

#if defined( PRE_PARTY_DB )
	pInfo->PartyType = m_Data.Type;
#else
	pInfo->PartyType = m_bRaidParty ? _RAID_PARTY_8 : _NORMAL_PARTY;
#endif // #if defined( PRE_PARTY_DB )

	pInfo->cPartyNameLen = (int)wcslen(GetPartyName());
	_wcscpy( pInfo->wszBuf, _countof(pInfo->wszBuf), GetPartyName(), pInfo->cPartyNameLen );

#if defined( PRE_WORLDCOMBINE_PARTY )
	if( Party::bIsWorldCombineParty(m_Data.Type) )
	{
		WorldCombineParty::WrldCombinePartyData* WorldCombinePartyData = g_pDataManager->GetCombinePartyData(m_Data.nPrimaryIndex);
		if( WorldCombinePartyData )
			pInfo->nWorldCombinePartyTableIndex = WorldCombinePartyData->cIndex;
	}
#endif
#if defined( PRE_ADD_NEWCOMEBACK )
	pInfo->bCheckComeBackParty = bIsComebackParty();
#endif
	return ERROR_NONE;
}

int CDNParty::GetAvrUserLv()
{
	int nUserCnt, nMountLv;
	nUserCnt = nMountLv = 0;
	CDNUserSession *pUserObj = NULL;

	for (int i = 0; i < PARTYMAX; i++)
	{
		if (m_nPartyMemberArray[i] == 0) continue;
		pUserObj = g_pUserSessionManager->FindUserSessionByAccountDBID(m_nPartyMemberArray[i]);
		if (!pUserObj) continue;

		nMountLv += pUserObj->GetLevel();
		nUserCnt++;
	}

	return nUserCnt == 0 ? 0 : nMountLv / nUserCnt;
}

bool CDNParty::CheckCompleteParty()
{
	bool bCheck = true;
	for (int i = 0; i < PARTYMAX; i++)
	{
		if (m_nPartyMemberArray[i] <= 0) continue;
		if (m_bPartyCheckMemberArray[i] == false) return false;
	}
	m_nCreateTick = 0;
	return true;
}

bool CDNParty::AdjustmentPartyInfo()
{
	int nCount = 0;
	CDNUserSession * pUser = NULL;
	for (int i = 0; i < PARTYMAX; i++)
	{
		if (m_nPartyMemberArray[i] <= 0) continue;
		pUser = g_pUserSessionManager->FindUserSessionByAccountDBID(m_nPartyMemberArray[i]);
		if (pUser == NULL)
		{
#if defined( PRE_PARTY_DB )
			g_pPartyManager->GetPartyID( m_nPartyMemberArray[i] );
#endif
			m_nPartyMemberArray[i] = 0;
			m_bPartyCheckMemberArray[i] = false;
			continue;
		}
#if defined( PRE_PARTY_DB )
		else if( pUser->GetPartyID() != GetPartyID() )
		{
			m_nPartyMemberArray[i] = 0;
			m_bPartyCheckMemberArray[i] = false;
			continue;
		}
#endif
		nCount++;
	}
	m_nCreateTick = 0;
	
	return nCount > 0 ? true : false;
}

bool CDNParty::ChangePartyLeader(UINT nNewLeaderAID)
{
	UINT nNewLeader = nNewLeaderAID > 0 ? nNewLeaderAID : PickNewLeader();

	CDNUserSession * pOriginLeader = g_pUserSessionManager->FindUserSessionByAccountDBID(m_nLeaderAccountDBID);
	CDNUserSession * pNewLeader = g_pUserSessionManager->FindUserSessionByAccountDBID(nNewLeader);

	if (pNewLeader == NULL)
		return false;

	if (pNewLeader->GetPartyID() != GetPartyID())
		return false;
	
	if( m_bIsMove == true )
		return false;

	if (pNewLeader)
	{
		m_nLeaderAccountDBID = pNewLeader->GetAccountDBID();
		m_nLeaderSessionID = pNewLeader->GetSessionID();
		m_Data.biLeaderCharacterDBID = pNewLeader->GetCharacterDBID();
		pNewLeader->m_boPartyLeader = true;
	}

	if (pOriginLeader)
		pOriginLeader->m_boPartyLeader = false;

	if (pOriginLeader)
		pOriginLeader->SendUserLocalMessage(0, FM_LEAVEPARTY);

#if defined( PRE_PARTY_DB )
	if( pNewLeader )
		pNewLeader->GetDBConnection()->QueryModPartyLeader( pNewLeader, GetPartyID() );
#endif // #if defined( PRE_PARTY_DB )
	return true;
}

bool CDNParty::IsPartyMemberWindowStateNone()
{
	CDNUserSession *pUserObj = NULL;
	for (int i = 0; i < PARTYMAX; i++){
		if (m_nPartyMemberArray[i] == 0) continue;

		pUserObj = g_pUserSessionManager->FindUserSessionByAccountDBID(m_nPartyMemberArray[i]);
		if (!pUserObj) continue;
		if (!pUserObj->IsNoneWindowState()) return false;
	}

	return true;
}

bool CDNParty::IsMember(UINT nAccountDBID)
{
	for (int i = 0; i < PARTYMAX; i++)
	{
		if (m_nPartyMemberArray[i] <= 0) continue;
		if (m_nPartyMemberArray[i] == nAccountDBID)
			return true;
	}
	return false;
}

bool CDNParty::SwapMemberIndex( const CSPartySwapMemberIndex * pPacket)
{
	//verify count
	if (pPacket->cCount > PARTYCOUNTMAX)
		return false;							//이러시면 아니데옵니다.

	//verify index range
	std::vector <BYTE> vDuplicate;
	std::vector <BYTE>::iterator iDuplicater;
	for (int i = 0; i < pPacket->cCount; i++)
	{
		if (pPacket->Index[i].cIndex > (PARTYCOUNTMAX-1))
			return false;

		iDuplicater = std::find(vDuplicate.begin(), vDuplicate.end(), pPacket->Index[i].cIndex);
		if (vDuplicate.end() != iDuplicater)
			return false;						//중복인덱스가 있으면 안데자나

		vDuplicate.push_back(pPacket->Index[i].cIndex);
	}

	//make backupdata
	UINT nBackupMemberArray[PARTYMAX];
	bool bPartyCheckMemberArray[PARTYMAX];
	memcpy(nBackupMemberArray, m_nPartyMemberArray, sizeof(m_nPartyMemberArray));
	memcpy(bPartyCheckMemberArray, m_bPartyCheckMemberArray, sizeof(m_bPartyCheckMemberArray));

	//reset checkarry
	memset(m_bPartyCheckMemberArray, 0, sizeof(m_bPartyCheckMemberArray));
	memset(m_nPartyMemberArray, 0, sizeof(m_nPartyMemberArray));

	//find member instance & reinstruct array
	CDNUserSession * pSessionArr;
	for (int i = 0; i < PARTYMAX; i++)
	{
		if (nBackupMemberArray[i] <= 0)
			continue;

		pSessionArr = g_pUserSessionManager->FindUserSessionByAccountDBID(nBackupMemberArray[i]);
		if (pSessionArr)
		{
			bool bCheck = false;
			for (int j = 0; j < PARTYMAX; j++)
			{
				if (pPacket->Index[j].nSessionID == pSessionArr->GetSessionID())
				{
					m_nPartyMemberArray[pPacket->Index[j].cIndex] = pSessionArr->GetAccountDBID();
					m_bPartyCheckMemberArray[pPacket->Index[j].cIndex] = true;
					pSessionArr->m_nPartyMemberIndex = pPacket->Index[j].cIndex;
					bCheck = true;
					break;
				}
			}

			if (bCheck == false)
			{
				//원래대로 돌린다.
				memcpy(m_nPartyMemberArray, nBackupMemberArray, sizeof(m_nPartyMemberArray));
				memcpy(m_bPartyCheckMemberArray, bPartyCheckMemberArray, sizeof(m_bPartyCheckMemberArray));

				for (int j = 0; j < PARTYMAX; j++)
				{
					if (m_nPartyMemberArray[j] <= 0) continue;
					pSessionArr = g_pUserSessionManager->FindUserSessionByAccountDBID(m_nPartyMemberArray[j]);
					if (pSessionArr)
						pSessionArr->m_nPartyMemberIndex = j;
				}
				return false;
			}
		}
	}

	//SendAllSwapMemberIndex(pPacket);
	SendAllRefreshParty(PARTYREFRESH_NONE);
	return true;
}

void CDNParty::SortMemberIndex(int nOutIndex)
{
	//if (m_bRaidParty == false || nOutIndex < 0) return;		//레이드파티가 아니면 해줄 필요 없다.
	if (nOutIndex < 0) return;		//레이드파티가 아니면 해줄 필요 없다.
	if (nOutIndex < 0 || PARTYCOUNTMAX-1 < nOutIndex) return;

	//진짜 나간인덱스인지 확인해본다
	if (m_nPartyMemberArray[nOutIndex] != 0)
		return;

	//노말파티의 최대인원이 옵센카운트이다.
	int nOffSetRemainIndex = (nOutIndex+1)%NORMPARTYCOUNTMAX;
	if (nOffSetRemainIndex <= 0) return;		//나머지카운트가 0이면 소팅할꺼읍다

	int nOffSetIndex = (nOutIndex+1) > NORMPARTYCOUNTMAX ? (nOutIndex)/NORMPARTYCOUNTMAX : 0;
	int nSortEnd = (nOffSetIndex*NORMPARTYCOUNTMAX) + NORMPARTYCOUNTMAX;

	CDNUserSession * pSession = NULL;
	for (int i = nOutIndex; i < (nSortEnd-1); i++)
	{
		if ((i+1) >= PARTYCOUNTMAX) break;
		if (m_nPartyMemberArray[i+1] <= 0) continue;
		m_nPartyMemberArray[i] = m_nPartyMemberArray[i+1];
		m_bPartyCheckMemberArray[i] = m_bPartyCheckMemberArray[i+1];
		pSession = g_pUserSessionManager->FindUserSessionByAccountDBID(m_nPartyMemberArray[i+1]);
		if (pSession)
			pSession->m_nPartyMemberIndex = i;
		m_nPartyMemberArray[i+1] = 0;
		m_bPartyCheckMemberArray[i+1] = false;
	}
}

bool CDNParty::CheckPartyUserStartFlag()
{
	CDNUserSession * pUser;
	for (int i = 0; i < PARTYMAX; i++)
	{
		if (m_nPartyMemberArray[i] <= 0) continue;
		pUser = g_pUserSessionManager->FindUserSessionByAccountDBID(m_nPartyMemberArray[i]);
		if (pUser == NULL) continue;
		if (pUser->m_bIsStartGame)
			return false;
	}
	return true;
}

void CDNParty::SetPartyUserStartFlag(bool bFlag)
{
	CDNUserSession * pUser;
	for (int i = 0; i < PARTYMAX; i++)
	{
		if (m_nPartyMemberArray[i] <= 0) continue;
		pUser = g_pUserSessionManager->FindUserSessionByAccountDBID(m_nPartyMemberArray[i]);
		if (pUser == NULL) continue;
		pUser->m_bIsStartGame = bFlag;
	}
}

void CDNParty::SetPartyUserMoveFlag(bool bFlag)
{
	CDNUserSession * pUser;
	for (int i = 0; i < PARTYMAX; i++)
	{
		if (m_nPartyMemberArray[i] <= 0) continue;
		pUser = g_pUserSessionManager->FindUserSessionByAccountDBID(m_nPartyMemberArray[i]);
		if (pUser == NULL) continue;
		pUser->m_bIsMove = bFlag;
	}
}

#ifdef _USE_VOICECHAT
bool CDNParty::CreateVoiceChannel(UINT nVoiceChannelID)
{
	if (nVoiceChannelID <= 0 || g_pVoiceChat == NULL) return false;
	if (g_pVoiceChat->CreateVoiceChannel(nVoiceChannelID))
	{
		CDNUserSession * pSession = NULL;
		m_nVoiceChannelID = nVoiceChannelID;
		m_nTalkingTick = timeGetTime();
		for (int i = 0; i < PARTYMAX; i++)
		{
			if (m_nPartyMemberArray[i] <= 0) continue;
			pSession = g_pUserSessionManager->FindUserSessionByAccountDBID(m_nPartyMemberArray[i]);
			if (!pSession) continue;

			pSession->JoinVoiceChannel(nVoiceChannelID, pSession->GetIp(), &pSession->GetTargetPos(), (int)pSession->m_nVoiceRotate);
		}
		return true;
	}
	return false;
}

void CDNParty::DestroyVoiceChannel()
{
	if (m_nVoiceChannelID <= 0 || g_pVoiceChat == NULL) return;
	g_pVoiceChat->DestroyVoiceChannel(m_nVoiceChannelID);
	m_nVoiceChannelID = 0;
}

void CDNParty::TalkingUpdate(ULONG nCurTick)
{
	if (m_nVoiceChannelID <= 0) return;
	if (m_nTalkingTick + VOICEUPDATETIME > nCurTick) return;
	g_pUserSessionManager->TalkingUpdate(m_nPartyMemberArray);
	m_nTalkingTick = nCurTick;
}

void CDNParty::SetInitVoiceInfo(TMemberInfo * pInfo, int nCount)
{
	if (pInfo == NULL || nCount == 0) return;
	for (int i = 0; i < nCount; i++)
		m_MemberVoiceInfo[i] = pInfo[i];
}

TMemberVoiceInfo * CDNParty::GetInitVoiceInfo(UINT nAccountDBID)
{
	for (int i = 0; i < PARTYMAX; i++)
	{
		if (m_MemberVoiceInfo[i].nAccountDBID == nAccountDBID)
			return &m_MemberVoiceInfo[i].VoiceInfo;
	}
	return NULL;
}

void CDNParty::SendRefreshVoiceInfo(UINT nOutAccountDBID)
{
	if (m_nVoiceChannelID <= 0 || m_bIsMove) return;
	TVoiceMemberInfo Info[PARTYMAX] = { 0, };
	int nCount = 0;

	CDNUserSession * pUser;
	for (int i = 0; i < PARTYMAX; i++)
	{
		if (m_nPartyMemberArray[i] <= 0) continue;
		pUser = g_pUserSessionManager->FindUserSessionByAccountDBID(m_nPartyMemberArray[i]);
		if (pUser == NULL) continue;
		Info[nCount].nSessionID = pUser->GetSessionID();
		Info[nCount].cVoiceAvailable = pUser->GetVoiceAvailable() == true ? 1 : 0;
		nCount++;

		if (nOutAccountDBID > 0)
			pUser->SetVoiceMute(nOutAccountDBID, false);
	}

	UINT nMuteSessionList[PARTYCOUNTMAX];
	CDNUserSession * pTempUser;
	for (int i = 0; i < PARTYMAX; i++)
	{
		if (m_nPartyMemberArray[i] <= 0) continue;
		pUser = g_pUserSessionManager->FindUserSessionByAccountDBID(m_nPartyMemberArray[i]);
		if (pUser == NULL) continue;

		memset(nMuteSessionList, 0, sizeof(nMuteSessionList));
		for (int j = 0; j < PARTYCOUNTMAX; j++)
		{
			if (pUser->m_nVoiceMutedList[j] <= 0) continue;
			pTempUser = g_pUserSessionManager->FindUserSessionByAccountDBID(pUser->m_nVoiceMutedList[j]);
			if (pTempUser == NULL) continue;
			nMuteSessionList[j] = pTempUser->GetSessionID();
		}
		pUser->SendVoiceMemberInfoRefresh(nCount, nMuteSessionList, Info);
	}
}
#endif

void CDNParty::SendAllRefreshParty(int nRefreshSubject)
{
	if (m_bIsMove) return;

	SPartyMemberInfo Info[PARTYMAX] = { 0, };
	int nCount = GetMemberInfo(Info);

	CDNUserSession *pUserObj = NULL;
	bool bAvailable = false;
#ifdef _USE_VOICECHAT
	bAvailable = m_nVoiceChannelID > 0 ? true : false;
#endif

#ifdef PRE_ADD_BEGINNERGUILD
	int nUpkeepBonus = 0, nFriendBonus = 0, nBeginnerGuildBonus = 0;
#else		//#ifdef PRE_ADD_BEGINNERGUILD
	int nUpkeepBonus = 0, nFriendBonus = 0;
#endif		//#ifdef PRE_ADD_BEGINNERGUILD
	
	if (GetUpkeepCount() > 0 && GetMemberCount() > 1)
	{
		int nTempOffSetRate = 0, nTempMaximumRate = 0;
		if (g_pEvent->GetUpkeepBonus(g_Config.nWorldSetID, nTempOffSetRate, nTempMaximumRate))
		{
			nUpkeepBonus = GetUpkeepCount() * nTempOffSetRate;
			nUpkeepBonus = nUpkeepBonus > nTempMaximumRate ? nTempMaximumRate : nUpkeepBonus;
		}
	}

	if (g_pEvent->GetFriendPartyBonus(g_Config.nWorldSetID, nFriendBonus) == false)
		nFriendBonus = 0;
	CheckFriendBonus();

#ifdef PRE_ADD_BEGINNERGUILD
	CheckBeginnerGuildBonus();
	nBeginnerGuildBonus = m_Data.bPartyBeginnerGuild == true ? BeginnerGuild::Common::PartyBonusRate : 0;
#endif		//#ifdef PRE_ADD_BEGINNERGUILD

#if defined( PRE_WORLDCOMBINE_PARTY )
	int nWorldCombinePartyTableIndex = 0;
	if(Party::bIsWorldCombineParty(m_Data.Type))
	{
		WorldCombineParty::WrldCombinePartyData* WorldCombinePartyData = g_pDataManager->GetCombinePartyData( m_Data.nPrimaryIndex );
		if(WorldCombinePartyData)
			nWorldCombinePartyTableIndex = WorldCombinePartyData->cIndex;
	}
#endif

	for (int i = 0; i < PARTYMAX; i++){
		if (m_nPartyMemberArray[i] == 0) continue;

		pUserObj = g_pUserSessionManager->FindUserSessionByAccountDBID(m_nPartyMemberArray[i]);
		if (!pUserObj) continue;

		if (pUserObj->GetPartyID() != GetPartyID())
		{
			m_nPartyMemberArray[i] = 0;
			m_bPartyCheckMemberArray[i] = false;
			_ASSERT_EXPR(0, L"명재를 찾아주세요!");
			continue;
		}

#if defined( PRE_PARTY_DB )
#if defined( PRE_WORLDCOMBINE_PARTY )		
		pUserObj->SendRefreshParty(GetLeaderSessionID(), nCount, bAvailable, Info, (short)nRefreshSubject, &m_Data, nWorldCombinePartyTableIndex);
#else
		pUserObj->SendRefreshParty(GetLeaderSessionID(), GetPartyName(), GetMemberMax(), \
			GetMinLevel(), nCount, GetItemLootRule(), GetItemLootRank(), \
			bAvailable, GetTargetMapIndex(), GetDifficulty(), Info, (short)nRefreshSubject, m_Data.Type, m_Data.iBitFlag, GetPassword() );
#endif
#else
		pUserObj->SendRefreshParty(GetLeaderSessionID(), m_wszPartyName, m_wszPartyPassword, m_nMemberMax, \
			m_nUserLvLimitMin, m_nUserLvLimitMax, nCount, m_ItemLootRule, m_ItemLootRank, \
			bAvailable, m_nTargetMapIdx, m_Difficulty, Info, (short)nRefreshSubject, m_cIsJobDice, m_bRaidParty ? _RAID_PARTY_8 : _NORMAL_PARTY);
#endif // #if defined( PRE_PARTY_DB )
#ifdef PRE_ADD_BEGINNERGUILD
		pUserObj->SendPartyBonusValue(nUpkeepBonus, nFriendBonus, false, nBeginnerGuildBonus);
#else		//#ifdef PRE_ADD_BEGINNERGUILD
		pUserObj->SendPartyBonusValue(nUpkeepBonus, nFriendBonus);
#endif		//#ifdef PRE_ADD_BEGINNERGUILD
	}

	if( nCount > 1)
		UpdateMasterSystemCountInfo();

#if defined( PRE_ADD_NAMEDITEM_SYSTEM )
	SendPartyEffectSkillItemData();
#endif

#ifdef _USE_VOICECHAT
	if (bAvailable) SendRefreshVoiceInfo();
#endif

	CDNUserSession * pLeader = g_pUserSessionManager->FindUserSessionByAccountDBID(m_nLeaderAccountDBID);
	if (pLeader)
		pLeader->SendUserLocalMessage(0, FM_ENTERPARTY);
}

void CDNParty::SendAllRefreshGateInfo()
{
	char GateNo[PARTYMAX] = { 0, };
	CDNUserSession *pUserObj = NULL;

	for (int i = 0; i < PARTYCOUNTMAX; i++)
	{
		if (m_nPartyMemberArray[i] == 0)
		{
			GateNo[i] = -1;
			continue;
		}

		pUserObj = g_pUserSessionManager->FindUserSessionByAccountDBID(m_nPartyMemberArray[i]);
		if (!pUserObj) continue;

		if (pUserObj) 
			GateNo[i] = pUserObj->m_cGateNo;
	}

	for (int i = 0; i < PARTYMAX; i++){
		if (m_nPartyMemberArray[i] == 0) continue;
		pUserObj = g_pUserSessionManager->FindUserSessionByAccountDBID(m_nPartyMemberArray[i]);
		if (!pUserObj) continue;
		pUserObj->SendRefreshGateInfo(GateNo, PARTYCOUNTMAX, ERROR_NONE);
	}
}

void CDNParty::SendAllGateInfo(int nGateNo)
{
	m_nRandomSeed = timeGetTime();//_rand();

	CDNUserSession *pUserObj = NULL;
	for (int i = 0; i < PARTYMAX; i++){
		if (m_nPartyMemberArray[i] == 0) continue;
		pUserObj = g_pUserSessionManager->FindUserSessionByAccountDBID(m_nPartyMemberArray[i]);
		if (!pUserObj) continue;
		pUserObj->m_eUserState = STATE_READY;
		pUserObj->SendGateInfo(GetLeaderSessionID(), nGateNo, 0, NULL, NULL);
	}
}

void CDNParty::SendAllFarmInfo()
{
	m_nRandomSeed = timeGetTime();//_rand();

	CDNUserSession *pUserObj = NULL;
	for (int i = 0; i < PARTYMAX; i++){
		if (m_nPartyMemberArray[i] == 0) continue;
		pUserObj = g_pUserSessionManager->FindUserSessionByAccountDBID(m_nPartyMemberArray[i]);
		if (!pUserObj) continue;
		pUserObj->m_eUserState = STATE_READY;
		pUserObj->ReqFarmInfo();
	}
}

void CDNParty::SendAllCancelStage( short cRetCode )
{
	CDNUserSession *pUserObj = NULL;
	for (int i = 0; i < PARTYMAX; i++){
		if (m_nPartyMemberArray[i] == 0) continue;
		pUserObj = g_pUserSessionManager->FindUserSessionByAccountDBID(m_nPartyMemberArray[i]);
		if (!pUserObj) continue;

		pUserObj->m_cGateNo = -1;
		pUserObj->m_cGateSelect = -1;
		pUserObj->m_eUserState = STATE_NONE;
		pUserObj->SendCancelStage( cRetCode );
	}

	m_boCompleteMember = false;
}

void CDNParty::SendSelectStage(char cSelectIndex)
{
	CDNUserSession *pUserObj = NULL;
	for (int i = 0; i < PARTYMAX; i++){
		if (m_nPartyMemberArray[i] == 0) continue;
		pUserObj = g_pUserSessionManager->FindUserSessionByAccountDBID(m_nPartyMemberArray[i]);
		if (!pUserObj) continue;
		pUserObj->m_cGateSelect = cSelectIndex;
		pUserObj->SendSelectStage(cSelectIndex);
	}
}

void CDNParty::SendAllReadyToGame(ULONG nIP, USHORT nPort, USHORT nTcpPort)
{
	CDNUserSession *pUserObj = NULL;
	for (int i = 0; i < PARTYMAX; i++){
		if (m_nPartyMemberArray[i] == 0) continue;
		pUserObj = g_pUserSessionManager->FindUserSessionByAccountDBID(m_nPartyMemberArray[i]);
		if (!pUserObj) continue;
		pUserObj->m_nGameServerIP = nIP;
		pUserObj->m_nGameServerPort = nPort;
		pUserObj->m_nGameServerTcpPort = nTcpPort;
		pUserObj->m_eUserState = STATE_READYTOGAME;

		pUserObj->m_biCertifyingKey = g_pAuthManager->GetCertifyingKey();
		DN_ASSERT(0 != pUserObj->m_biCertifyingKey,	"Invalid!");	// 인증키가 0 이 생성되면 않됨 !!! (없음 의미)

		g_pAuthManager->QueryStoreAuth(SERVERTYPE_VILLAGE, pUserObj);
	}
}

void CDNParty::SendAllRoomID( GameTaskType::eType GameTaskType, BYTE cReqGameIDType, USHORT wGameID, int nRoomID)
{
	CDNUserSession *pUserObj = NULL;
	for (int i = 0; i < PARTYMAX; i++)
	{
		if (m_nPartyMemberArray[i] == 0) 
			continue;
		pUserObj = g_pUserSessionManager->FindUserSessionByAccountDBID(m_nPartyMemberArray[i]);
		if (!pUserObj) 
			continue;
		pUserObj->m_GameTaskType = GameTaskType;
		pUserObj->m_cReqGameIDType	= cReqGameIDType;
		pUserObj->SetGameID(wGameID);
		pUserObj->SetRoomID(nRoomID);
	}
}

#if defined( PRE_PARTY_DB )
#else
void CDNParty::SendPartyMemberInfo(bool bIsRaidParty, CDNUserSendManager * pSender)
{
	SCPartyMemberInfo packet;
	memset(&packet, 0, sizeof(packet));

	CDNUserSession *pUserObj = NULL;
	packet.PartyID = GetPartyID();
	packet.cIsRaidParty = bIsRaidParty == true ? 1 : 0;

	for (int i = 0; i < sizeof(m_nPartyMemberArray)/sizeof(*m_nPartyMemberArray); i++)
	{
		if (m_nPartyMemberArray[i] == 0) continue;
		pUserObj = g_pUserSessionManager->FindUserSessionByAccountDBID(m_nPartyMemberArray[i]);
		if (pUserObj != NULL)
		{
			if (packet.cLeaderIdx == 0 && m_nLeaderAccountDBID == m_nPartyMemberArray[i])
				packet.cLeaderIdx = packet.cCount;
			packet.Info[packet.cCount].cClassID = pUserObj->GetClassID();
			packet.Info[packet.cCount].wJob = pUserObj->GetUserJob();
			packet.Info[packet.cCount].cUserLevel = pUserObj->GetLevel();
			_wcscpy(packet.Info[packet.cCount].wszName, _countof(packet.Info[packet.cCount].wszName), pUserObj->GetCharacterName(), (int)wcslen(pUserObj->GetCharacterName()));
			packet.cCount++;
		}
	}
	pSender->SendPartyMemberInfo(&packet);
}
#endif // #if defined( PRE_PARTY_DB )

void CDNParty::SendKickPartyMember(UINT nSessionID)
{
	for (int i = 0; i < sizeof(m_nPartyMemberArray)/sizeof(*m_nPartyMemberArray); i++)
	{
		if (m_nPartyMemberArray[i] == 0) continue;
		CDNUserSession * pUser = g_pUserSessionManager->FindUserSessionByAccountDBID(m_nPartyMemberArray[i]);
		if (pUser != NULL)
			pUser->SendKickPartymember(nSessionID, ERROR_NONE);
	}
}

void CDNParty::SendPartyMemberMove(UINT nSessionID, EtVector3 vPos)
{
	CDNUserSession *pUserObj = NULL;
	for (int i = 0; i < PARTYMAX; i++){
		if (m_nPartyMemberArray[i] == 0) continue;

		pUserObj = g_pUserSessionManager->FindUserSessionByAccountDBID(m_nPartyMemberArray[i]);
		if (!pUserObj) continue;
		if (pUserObj->GetSessionID() == nSessionID) continue;	// 내자신은 패스

		pUserObj->SendPartyMemberMove(nSessionID, vPos);
	}
}

void CDNParty::SendPartyMemberMoveEachOther(UINT nSessionID, int nX, int nY, int nZ)
{
	CDNUserSession *pUserObj = NULL, *pSelfObj = NULL;
	EtVector3 vPos;

	for (int i = 0; i < PARTYMAX; i++){	// 파티원한테 내위치를 날리고
		if (m_nPartyMemberArray[i] == 0) continue;

		pUserObj = g_pUserSessionManager->FindUserSessionByAccountDBID(m_nPartyMemberArray[i]);
		if (!pUserObj) continue;
		if (pUserObj->GetSessionID() == nSessionID){
			pSelfObj = pUserObj;
			continue;	// 내자신은 패스
		}

		vPos.x = (float)nX / 1000.f;
		vPos.y = (float)nY / 1000.f;
		vPos.z = (float)nZ / 1000.f;
		pUserObj->SendPartyMemberMove(nSessionID, vPos);
	}

	if (pSelfObj){	// 나한테 파티원들의 위치를 알린다
		for (int i = 0; i < PARTYMAX; i++){
			if (m_nPartyMemberArray[i] == 0) continue;

			pUserObj = g_pUserSessionManager->FindUserSessionByAccountDBID(m_nPartyMemberArray[i]);
			if (!pUserObj) continue;
			if (pUserObj->GetSessionID() == nSessionID) continue;	// 내자신은 패스

			vPos.x = (float)pUserObj->GetCurrentPos().nX / 1000.f;
			vPos.y = (float)pUserObj->GetCurrentPos().nY / 1000.f;
			vPos.z = (float)pUserObj->GetCurrentPos().nZ / 1000.f;
			pSelfObj->SendPartyMemberMove(pUserObj->GetSessionID(), vPos);
		}
	}
}

void CDNParty::SendPartyMemberPart(CDNUserSession *pSession)
{
	CDNUserSession *pUserObj = NULL;
	for (int i = 0; i < PARTYMAX; i++){
		if (m_nPartyMemberArray[i] == 0) continue;

		pUserObj = g_pUserSessionManager->FindUserSessionByAccountDBID(m_nPartyMemberArray[i]);
		if (!pUserObj) continue;

		pUserObj->SendPartyMemberPart(pSession);
	}
}

void CDNParty::SendPartyChat(char cType, int cLen, const WCHAR *pwszCharacterName, const WCHAR *pwszChatMsg, int nRet)
{
	CDNUserSession *pUserObj = NULL;
	for (int i = 0; i < PARTYMAX; i++){
		if (m_nPartyMemberArray[i] == 0) continue;

		pUserObj = g_pUserSessionManager->FindUserSessionByAccountDBID(m_nPartyMemberArray[i]);
		if (!pUserObj) continue;

		pUserObj->SendChat(cType, cLen, pwszCharacterName, pwszChatMsg, NULL, nRet);
	}
}

void CDNParty::SendPartyUseRadio(UINT nSessionID, USHORT nRadioID)
{
	CDNUserSession *pUserObj = NULL;
	for (int i = 0; i < PARTYMAX; i++){
		if (m_nPartyMemberArray[i] == 0) continue;

		pUserObj = g_pUserSessionManager->FindUserSessionByAccountDBID(m_nPartyMemberArray[i]);
		if (!pUserObj) continue;

		pUserObj->SendUseRadio(nSessionID, nRadioID);
	}
}

void CDNParty::SendPartyReadyRequest()
{
	CDNUserSession *pUserObj = NULL;
	for (int i = 0; i < PARTYMAX; i++){
		if (m_nPartyMemberArray[i] == 0) continue;

		pUserObj = g_pUserSessionManager->FindUserSessionByAccountDBID(m_nPartyMemberArray[i]);
		if (!pUserObj) continue;

		pUserObj->SendReadyRequest();
	}
}

void CDNParty::SendUserProfile(UINT nSessionID, TProfile & profile)
{
	CDNUserSession *pUserObj = NULL;
	for (int i = 0; i < PARTYMAX; i++){
		if (m_nPartyMemberArray[i] == 0) continue;

		pUserObj = g_pUserSessionManager->FindUserSessionByAccountDBID(m_nPartyMemberArray[i]);
		if (!pUserObj) continue;

		pUserObj->SendDisplayProfile( nSessionID, profile );
	}
}

void CDNParty::SendAllSwapMemberIndex(CSPartySwapMemberIndex * pPacket)
{
	CDNUserSession *pUserObj = NULL;
	for (int i = 0; i < PARTYMAX; i++){
		if (m_nPartyMemberArray[i] == 0) continue;

		pUserObj = g_pUserSessionManager->FindUserSessionByAccountDBID(m_nPartyMemberArray[i]);
		if (!pUserObj) continue;

		pUserObj->SendPartySwapMemberIndex(pPacket, ERROR_NONE);
	}
}

// 모든 파티 멤버의 게이트 번호를 임의로 세팅한다;
void CDNParty::SetAllMemberGate(int nGateNo)
{
	CDNUserSession *pUserObj = NULL;

	for (int i = 0; i < PARTYMAX; i++)
	{
		if (m_nPartyMemberArray[i] == 0)
			continue;
		pUserObj = g_pUserSessionManager->FindUserSessionByAccountDBID(m_nPartyMemberArray[i]);
		if (!pUserObj) continue;

		pUserObj->m_cGateNo = (char)nGateNo;
	}
}

void CDNParty::CheckFriendBonus()
{
	CDNUserSession * pUser = NULL, * pTempUser = NULL;

	for (int i = 0; i < PARTYMAX; i++)
	{
		if (m_nPartyMemberArray[i] == 0) continue;
		pUser = g_pUserSessionManager->FindUserSessionByAccountDBID(m_nPartyMemberArray[i]);
		if (!pUser) continue;

		bool bCheck = false;
		CDNFriend * pFriend = pUser->GetFriend();
		if (pFriend)
		{
			for (int h = 0; h < PARTYMAX; h++)
			{
				if (m_nPartyMemberArray[h] == 0) continue;
				pTempUser = g_pUserSessionManager->FindUserSessionByAccountDBID(m_nPartyMemberArray[h]);

				if (pTempUser == NULL) continue;
				CDNFriend * pTempFriend = pTempUser->GetFriend();
				if (pTempFriend == NULL) continue;

				if (pFriend->HasFriend(pTempUser->GetCharacterDBID()) && pTempFriend->HasFriend(pUser->GetCharacterDBID()))
				{
					bCheck = true;
					break;
				}
			}

			pUser->SetFriendBonus(bCheck);
		}
	}
}

#ifdef PRE_ADD_BEGINNERGUILD
void CDNParty::CheckBeginnerGuildBonus()
{
	m_Data.bPartyBeginnerGuild = false;

	CDNUserSession * pUser = NULL;
	int nBegginerGuildCount = 0;
	CDNGuildBase * pGuild = NULL;
	for (int i = 0; i < PARTYMAX; i++)
	{
		if (m_nPartyMemberArray[i] == 0) continue;
		pUser = g_pUserSessionManager->FindUserSessionByAccountDBID(m_nPartyMemberArray[i]);
		if (!pUser) continue;

		pUser->m_bPartyBegginerGuild = false;

		if (pUser->GetGuildUID().IsSet() == false)
			continue;
		
		pGuild = g_pGuildManager->At(pUser->GetGuildUID());
		if (pGuild) 
		{
#if !defined( PRE_ADD_NODELETEGUILD )
			CDetachAutoEx<CDNGuildBase>	AutoDetach(pGuild);
			if (TRUE == pGuild->IsEnable())
			{
				if (pGuild->GetInfo()->cGuildType == BeginnerGuild::Type::Beginner && pGuild->IsMemberExist(pUser->GetCharacterDBID()))
				{
					nBegginerGuildCount++;
					pUser->m_bPartyBegginerGuild = true;
				}
			}
#else		//#if !defined( PRE_ADD_NODELETEGUILD )
			if (pGuild->GetInfo()->cGuildType == BeginnerGuild::Type::Beginner && pGuild->IsMemberExist(pUser->GetCharacterDBID()))
			{
				nBegginerGuildCount++;
				pUser->m_bPartyBegginerGuild = true;
			}
#endif		//#if !defined( PRE_ADD_NODELETEGUILD )
		}
	}

	if (nBegginerGuildCount >= 2)
		m_Data.bPartyBeginnerGuild = true;
}
#endif		//#ifdef PRE_ADD_BEGINNERGUILD

UINT CDNParty::PickNewLeader()
{
	for (int i = 0; i < PARTYMAX; i++)
	{
		if (m_nPartyMemberArray[i] <= 0) continue;
		if (m_nPartyMemberArray[i] != m_nLeaderAccountDBID && m_bPartyCheckMemberArray[i] == true)
			return m_nPartyMemberArray[i];
	}
	return 0;
}

void CDNParty::SetChannelID( int iChannelID )
{
#if defined( PRE_PARTY_DB )
	if( m_Data.LocationType == Party::LocationType::Village )
		m_Data.iLocationID = iChannelID;
#else
	m_nChannelID = iChannelID;
#endif // #if defined( PRE_PARTY_DB )
}

void CDNParty::SetMove( bool bFlag )
{
	m_bIsMove = bFlag;
}

void CDNParty::SetStartGame( bool bFlag )
{
	m_bIsStartGame = bFlag;
}

void CDNParty::SetRandomSeed( UINT uiSeed )
{
	m_nRandomSeed = uiSeed;
}

bool CDNParty::bIsRaidParty()
{
#if defined( PRE_PARTY_DB )
	return m_Data.Type == _RAID_PARTY_8;
#else
	return m_bRaidParty;
#endif // #if defined( PRE_PARTY_DB )
}

#if defined( PRE_PARTY_DB )

int CDNParty::GetChannelID() const
{
	if( m_Data.LocationType != Party::LocationType::Village )
		return 0;

	return m_Data.iLocationID;
}

int CDNParty::GetChannelMeritID() const
{
	if( m_Data.LocationType != Party::LocationType::Village )
		return 0;

	return g_pMasterConnection->GetMeritBonusID( m_Data.iLocationID );
}

int CDNParty::GetPartyMapIndex() const
{
	if( m_Data.LocationType == Party::LocationType::Village )
		return g_pMasterConnection->GetChannelMapIndex( m_Data.iLocationID );

	return m_Data.iLocationID;
}

bool CDNParty::ValidatePassword( int iPassword )
{
	return (iPassword >= 0 && iPassword <= MAXPARTYPASSWORD);
}

#endif // #if defined( PRE_PARTY_DB )

#if defined( PRE_ADD_NEWCOMEBACK )
bool CDNParty::CheckPartyMemberAppellation( int nAppellationID )
{
	CDNUserSession *pUserObj = NULL;
	for (int i = 0; i < PARTYMAX; i++)
	{
		if (m_nPartyMemberArray[i] == 0) continue;

		pUserObj = g_pUserSessionManager->FindUserSessionByAccountDBID(m_nPartyMemberArray[i]);
		if (!pUserObj) continue;
		if( pUserObj->GetComebackAppellation() == nAppellationID )
		{
			return true;
		}
	}
	return false;
}

bool CDNParty::CheckPartyMemberAppellation()
{
	CDNUserSession *pUserObj = NULL;
	for (int i = 0; i < PARTYMAX; i++)
	{
		if (m_nPartyMemberArray[i] == 0) continue;

		pUserObj = g_pUserSessionManager->FindUserSessionByAccountDBID(m_nPartyMemberArray[i]);
		if (!pUserObj) continue;
		if( pUserObj->GetComebackAppellation() > 0 )
		{
			return true;
		}
	}
	return false;
}

void CDNParty::ApplyJoinMemberAppellation( CDNUserSession * pSession )
{
	CDNUserSession *pUserObj = NULL;
	for (int i = 0; i < PARTYMAX; i++)
	{
		if (m_nPartyMemberArray[i] == 0) continue;

		pUserObj = g_pUserSessionManager->FindUserSessionByAccountDBID(m_nPartyMemberArray[i]);
		if (!pUserObj) continue;
		if( pUserObj->GetComebackAppellation() > 0 )
		{
			TAppellationData *pAData = g_pDataManager->GetAppellationData( pUserObj->GetComebackAppellation() );
			if( pAData )
			{
				if( pAData->nSkillItemID > 0 )
				{
					CSUseItem pPacket;
					memset(&pPacket, 0, sizeof(pPacket));
					pPacket.biInvenSerial = pSession->GetItem()->MakeItemSerial();
					TItemData* pBuffItemData = g_pDataManager->GetItemData( pAData->nSkillItemID );
					if(pBuffItemData)
						pSession->GetItem()->ApplyPartyEffectSkillItemData(&pPacket, pBuffItemData, pSession->GetSessionID(), EffectSkillNameSpace::ShowEffectType::NONEEFFECT, false, true);
				}
			}			
		}
	}
}

void CDNParty::DelOutMemberAppellation( CDNUserSession * pSession )
{
	CDNUserSession *pUserObj = NULL;
	for (int i = 0; i < PARTYMAX; i++)
	{
		if (m_nPartyMemberArray[i] == 0) continue;

		pUserObj = g_pUserSessionManager->FindUserSessionByAccountDBID(m_nPartyMemberArray[i]);
		if (!pUserObj) continue;
		if( pUserObj->GetComebackAppellation() > 0 && pUserObj->GetComebackAppellation() != pSession->GetComebackAppellation() )
		{
			TAppellationData *pAData = g_pDataManager->GetAppellationData( pUserObj->GetComebackAppellation() );
			if( pAData )
			{				
				if( pAData->nSkillItemID > 0 )
				{
					TItemData* pBuffItemData = g_pDataManager->GetItemData( pAData->nSkillItemID );
					if( pBuffItemData && pSession->GetItem() )
					{
						TEffectSkillData* EffectSkill = pSession->GetItem()->GetEffectSkillItem( EffectSkillNameSpace::SearchType::SkillID, pBuffItemData->nSkillID );
						if( EffectSkill )
						{
							pSession->GetItem()->DelEffectSkillItem( pBuffItemData->nSkillID );							
						}
					}
				}
			}			
		}
	}	
}

void CDNParty::DelPartyMemberAppellation( int nAppellationID )
{
	if( !CheckPartyMemberAppellation( nAppellationID ) )
	{
		TAppellationData *pAData = g_pDataManager->GetAppellationData( nAppellationID );
		if( pAData )
		{
			TItemData* pBuffItemData = g_pDataManager->GetItemData( pAData->nSkillItemID );
			if(pBuffItemData)
			{				
				RemovePartyEffectSkillItemData( pBuffItemData->nSkillID, pBuffItemData->nItemID );	
				SendAllRefreshParty(PARTYREFRESH_NONE);
				if( !CheckPartyMemberAppellation() )				
					SetComebackParty(false);				
			}							
		}
	}	
}
#endif