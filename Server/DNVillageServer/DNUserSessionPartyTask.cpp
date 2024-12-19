
#include "Stdafx.h"
#include "DNUserSession.h"
#include "DNMasterConnection.h"
#include "DNDBConnection.h"
#include "DNPartyManager.h"
#include "DNUserSessionManager.h"
#include "DNFieldDataManager.h"
#include "DNField.h"
#include "DNGameDataManager.h"
#include "DNFarm.h"
#include "DNIsolate.h"
#include "EtUIXML.h"
#include "DNGuildWarManager.h"
#if defined( PRE_PARTY_DB )
#include "DNPartySortObject.hpp"
#endif // #if defined( PRE_PARTY_DB )

extern TVillageConfig g_Config;

int CDNUserSession::OnRecvPartyMessage(int nSubCmd, char *pData, int nLen)
{
	switch(nSubCmd)
	{
		case CS_CREATEPARTY:
		{
			int iRet = _RecvPartyCreate( reinterpret_cast<const CSCreateParty*>(pData), nLen );
			switch( iRet )
			{
			case ERROR_NONE:
			case ERROR_INVALIDPACKET:
				{
					return iRet;
				}
			case ERROR_PARTY_CREATE_FAIL:
			case ERROR_PARTY_CREATE_FAIL_NEED_PARTYNAME:
			case ERROR_PARTY_CREATE_FAIL_PASSWORD_ERR:
			case ERROR_PARTY_CREATE_FAIL_RESTRICT_LEVEL_ERROR:
				{
					SendCreateParty( iRet, 0 );
					return ERROR_GENERIC_INVALIDREQUEST;
				}
			}
			break;
		}
#if defined( PRE_PARTY_DB ) && defined( _WORK )
		case CS_CREATEPARTY_CHEAT:
			{
				m_pDBCon->QueryAddParty( this, g_Config.nManagedID, Party::LocationType::Village, GetChannelID(), reinterpret_cast<const CSCreateParty*>(pData), 0, true );
				break;
			}
#endif // #if defined( PRE_PARTY_DB ) && defined( _WORK )
	case CS_JOINPARTY:
		{
			int iRet = _RecvPartyJoin( reinterpret_cast<const CSJoinParty*>(pData), nLen );
			switch( iRet )
			{
			case ERROR_NONE:
			case ERROR_INVALIDPACKET:
				{
					return iRet;
				}
			case ERROR_PARTY_JOINFAIL:
			case ERROR_PARTY_JOINFAIL_INREADYSTATE:
			case ERROR_PARTY_JOINFAIL_CHARACTERLEVEL_SHORTAGE:
			case ERROR_PARTY_JOINFAIL_CHARACTERLEVEL_OVER:
			case ERROR_PARTY_JOINFAIL_PASSWORDINCORRECT:
			case ERROR_PARTY_JOINFAIL_YOU_ARE_KICKEDUSER:
				{
					SendJoinParty( iRet, 0, NULL );
					return ERROR_NONE;
				}
			}
			break;
		}
	case CS_PARTYOUT:
		{
			int iRet = _RecvPartyOut( nLen );
			switch( iRet )
			{
			case ERROR_NONE:
			case ERROR_INVALIDPACKET:
				{
					return iRet;
				}
			case ERROR_PARTY_LEAVEFAIL:
				{
					SendPartyOut( iRet );
					return ERROR_NONE;
				}
			}
			break;
		}
	case CS_PARTYLISTINFO:
		{
			int iRet = _RecvPartyListInfo( reinterpret_cast<const CSPartyListInfo*>(pData), nLen );
			switch( iRet )
			{
				case ERROR_NONE:
				case ERROR_INVALIDPACKET:
				{
					return iRet;
				}
				case ERROR_PARTY_LIST_ERROR:
#if defined( PRE_PARTY_DB )
				case ERROR_PARTY_REFRESHLIST_TIMERESTRAINT:
#endif // #if defined( PRE_PARTY_DB )
				{
					SendPartyInfoErr( iRet );
					return ERROR_NONE;
				}
			}
			break;
		}
	case CS_REFRESHGATEINFO:
		{
			int iRet = _RecvPartyRefreshGateInfo( reinterpret_cast<const CSRefreshGateInfo*>(pData), nLen );
			switch( iRet )
			{
			case ERROR_NONE:
			case ERROR_INVALIDPACKET:
			case ERROR_GENERIC_UNKNOWNERROR:
				return iRet;
			}
			break;
		}
	case CS_STARTSTAGE:
		{
			int iRet = _RecvPartyStartStage( reinterpret_cast<const CSStartStage*>(pData), nLen );
			switch( iRet )
			{
			case ERROR_NONE:
			case ERROR_INVALIDPACKET:
				return iRet;
			case ERROR_PARTY_STARTSTAGE_FAIL:
			case ERROR_CASHSHOP_OPEN:
			case ERROR_GENERIC_MASTERCON_NOT_FOUND:
			case ERROR_FARM_INVALID_VIP:
			case ERROR_FARM_ENTERANCE_CONDITION:
				{
					SendCancelStage( iRet );
					return ERROR_NONE;
				}
			}
			break;
		}

	case CS_CANCELSTAGE:
		{
			int iRet = _RecvPartyCancelStage( nLen );
			switch( iRet )			
			{
			case ERROR_NONE:
			case ERROR_INVALIDPACKET:
				return iRet;
			}
			break;
		}

	case CS_SELECTSTAGE:
		{
			int iRet = _RecvPartySelectStage( reinterpret_cast<const CSSelectStage*>(pData), nLen );
			switch( iRet )			
			{
			case ERROR_NONE:
			case ERROR_INVALIDPACKET:
				return iRet;
			}
			break;
		}

	case CS_PARTYINVITE:
		{
			int iRet = _RecvPartyInvite( reinterpret_cast<const CSInvitePartyMember*>(pData), nLen );
			switch( iRet )
			{
			case ERROR_NONE:
			case ERROR_INVALIDPACKET:
				return iRet;
			case ERROR_PARTY_INVITEFAIL:
			case ERROR_PARTY_INVITEFAIL_DESTUSER_NOTFOUND:
			case ERROR_PARTY_INVITEFAIL_DEST_USER_NOWREADY:
			case ERROR_PARTY_INVITEFAIL_DESTUSER_SITUATION_NOTALLOWED:
			case ERROR_PARTY_INVITEFAIL_DESTLOCATION_NOT_SAME:
			case ERROR_PARTY_INVITEFAIL_ALREADY_HAS_PARTY:
			case ERROR_PARTY_INVITEFAIL_CHARACTERLEVEL_SHORTAGE:
			case ERROR_PARTY_JOINFAIL_CHARACTERLEVEL_OVER:
			case ERROR_PARTY_INVITEFAIL_ALREADY_INVITED:
				{
					SendPartyInviteFail( iRet );
					return ERROR_NONE;
				}
			case ERROR_ISOLATE_REQUESTFAIL:
				{
					SendIsolateResult( iRet );
					return ERROR_NONE;
				}
			}
			break;
		}
	case CS_PARTYINVITE_DENIED:
		{
			int iRet = _RecvPartyInviteDenied( reinterpret_cast<const CSInviteDenied*>(pData), nLen );
			switch( iRet )
			{
			case ERROR_NONE:
			case ERROR_INVALIDPACKET:
				return iRet;
			}
			break;
		}
	case CS_PARTYINVITE_OVERSERVER_RESULT:
		{
			int iRet = _RecvPartyInviteOverServerResult( reinterpret_cast<const CSPartyInviteOverServerResult*>(pData), nLen );
			switch( iRet )
			{
			case ERROR_NONE:
			case ERROR_INVALIDPACKET:
				return iRet;
			}
			break;
		}
	case CS_PARTYMEMBER_INFO:
		{
			int iRet = _RecvPartyMemberInfo( reinterpret_cast<const CSPartyMemberInfo*>(pData), nLen );
			switch( iRet )
			{
			case ERROR_NONE:
			case ERROR_INVALIDPACKET:
				return iRet;
			case ERROR_PARTY_NOTFOUND_MEMBERINFO:
				{
					SendPartyInfoErr( iRet );
					return ERROR_NONE;
				}
			}
			break;
		}
	case CS_PARTYMEMBER_KICK:
		{
			int iRet = _RecvPartyMemberKick( reinterpret_cast<const CSPartyMemberKick*>(pData), nLen );
			switch( iRet )
			{
			case ERROR_NONE:
			case ERROR_INVALIDPACKET:
				return iRet;
			case ERROR_PARTY_KICKFAIL:
			case ERROR_PARTY_KICKFAIL_DEST_IS_YOU:
				{
					SendKickPartymember( reinterpret_cast<const CSPartyMemberKick*>(pData)->nSessionID, iRet );
					return ERROR_NONE;
				}
			}
			break;
		}
	case CS_PARTYLEADER_SWAP:
		{
			int iRet = _RecvPartyLeaderSwap( reinterpret_cast<const CSPartyLeaderSwap*>(pData), nLen );
			switch( iRet )
			{
			case ERROR_NONE:
			case ERROR_INVALIDPACKET:
				return iRet;
			case ERROR_PARTY_LEADER_APPOINTMENTFAIL:
				{
					SendSwapPartyLeader(0, iRet );
					return ERROR_NONE;
				}
			}
			break;
		}
	case CS_CHANNELLIST:
		{
			int iRet = _RecvPartyChannelList( nLen );
			switch( iRet )
			{
			case ERROR_NONE:
			case ERROR_INVALIDPACKET:
			case ERROR_GENERIC_UNKNOWNERROR:
			case ERROR_GENERIC_INVALIDREQUEST:
				return iRet;
			}
			break;
		}
	case CS_SELECTCHANNEL:
		{
			return _RecvPartySelectChannel( reinterpret_cast<const CSVillageSelectChannel*>(pData), nLen );
		}
	case CS_PARTYMODIFY:
		{
			int iRet = _RecvPartyInfoModify( reinterpret_cast<const CSPartyInfoModify*>(pData), nLen );
			switch( iRet )
			{
			case ERROR_NONE:
			case ERROR_INVALIDPACKET:
			case ERROR_GENERIC_INVALIDREQUEST:
				return iRet;
			case ERROR_PARTY_MODIFYINFO_FAIL:
			case ERROR_PARTY_MODIFYINFO_FAIL_NEED_NAME:
			case ERROR_PARTY_MODIFYINFO_FAIL_PASSWORD_ERR:
			case ERROR_PARTY_MODIFYINFO_FAIL_RESTRICT_LEVEL_ERROR:
				{
					SendPartyModify( iRet );
					return ERROR_NONE;
				}
			}
			break;
		}	
	case CS_READYREQUEST:
		{
			int iRet = _RecvPartyReadyQuest( nLen );
			switch( iRet )
			{
			case ERROR_NONE:
			case ERROR_INVALIDPACKET:
			case ERROR_GENERIC_INVALIDREQUEST:
				return iRet;
			}
			break;
		}
	case CS_PARTYASKJOIN:
		{
			int iRet = _RecvPartyAskJoin( reinterpret_cast<const CSPartyAskJoin*>(pData), nLen );
			switch( iRet )
			{
			case ERROR_NONE:
			case ERROR_INVALIDPACKET:
				return iRet;
			case ERROR_PARTY_ASKJOIN_FAIL:
			case ERROR_PARTY_ASKJOIN_DEST_DIFFMAP:
			case ERROR_PARTY_ASKJOIN_PARTY_NOTFOUND:
			case ERROR_PARTY_ASKJOIN_DESTUSER_SITUATION_NOTALLOWED:
				{
					SendAskJoinResult( iRet );
					return ERROR_NONE;
				}
			}
			break;
		}
	case CS_PARTYASKJOINDECISION:
		{
			int iRet = _RecvPartyAskJoinDecision( reinterpret_cast<const CSPartyAskJoinDecision*>(pData), nLen );
			switch( iRet )
			{
			case ERROR_NONE:
			case ERROR_INVALIDPACKET:
				return iRet;
			case ERROR_PARTY_ASKJOIN_RESULT_AGREE_FAIL:
				{
					SendAskJoinResult( iRet );
					return ERROR_NONE;
				}
			}
			break;
		}
	case CS_SWAPMEMBERINDEX:
		{
			int iRet = _RecvPartySwapMemberIndex( reinterpret_cast<const CSPartySwapMemberIndex*>(pData), nLen );
			switch( iRet )
			{
			case ERROR_NONE:
			case ERROR_INVALIDPACKET:
				return iRet;
			case ERROR_PARTY_SWAPINDEX_FAIL:
				{
					SendPartySwapMemberIndex( NULL, iRet );
					return ERROR_NONE;
				}
			}
			break;
		}
	case CS_CONFIRM_LASTDUNGEONINFO:
		{
			int iRet = _RecvPartyConfirmLastDungeonInfo( reinterpret_cast<const CSPartyConfirmLastDungeonInfo*>(pData), nLen );
			switch( iRet )
			{
			case ERROR_NONE:
			case ERROR_INVALIDPACKET:
				return iRet;
			}
			break;
		}
#if defined(PRE_MOD_REQ_JOIN_PARTY_ANSWER_MSG_APP)	
	case CS_PARTYINFO:
		{
			int iRet = _RecvPartyInfo( reinterpret_cast<const CSPartyInfo*>(pData), nLen );
			switch( iRet )
			{
			case ERROR_NONE:
			case ERROR_INVALIDPACKET:
				return iRet;
			case ERROR_PARTY_JOINFAIL:
			case ERROR_PARTY_ASKJOIN_PARTY_NOTFOUND:
				SendPartyInfo(iRet, NULL);
				return ERROR_NONE;
			}
		}
		break;
#endif
	}	

	_DANGER_POINT();
	return ERROR_UNKNOWN_HEADER;			//상위에서 반드시! 리턴되어야 합니다.
}

//============================================================================================================================================================

int CDNUserSession::_RecvPartyCreate( const CSCreateParty* pPacket, int iLen )
{
#if defined( PRE_PARTY_DB )
	if (sizeof(CSCreateParty) != iLen)
		return ERROR_INVALIDPACKET;
#else
	if (sizeof(CSCreateParty) - sizeof(pPacket->wszBuf) +((pPacket->cNameLen + pPacket->cPassWordLen) * sizeof(WCHAR)) != iLen)
		return ERROR_INVALIDPACKET;
#endif // #if defined( PRE_PARTY_DB )

	WCHAR wszName[PARTYNAMELENMAX];
	memset(wszName, 0, sizeof(wszName));
#if defined( PRE_PARTY_DB )
#else
	WCHAR wszPass[PARTYPASSWORDMAX];
	memset(wszPass, 0, sizeof(wszPass));
#endif // #if defined( PRE_PARTY_DB )

#if defined( PRE_PARTY_DB )
	if( wcslen(pPacket->wszPartyName) <= 0 )
#else
	if (pPacket->cNameLen <= 0)
#endif // #if defined( PRE_PARTY_DB )
		return ERROR_PARTY_CREATE_FAIL_NEED_PARTYNAME;

#if defined( PRE_PARTY_DB )
	if( pPacket->iBitFlag&Party::BitFlag::Password && CDNParty::ValidatePassword(pPacket->iPassword) == false )
#else
	if (pPacket->cPassWordLen > PARTYPASSWORDMAX-1)
#endif // #if defined( PRE_PARTY_DB )
		return ERROR_PARTY_CREATE_FAIL_PASSWORD_ERR;

#if defined( PRE_PARTY_DB )
	if( wcslen(pPacket->wszPartyName) >= PARTYNAMELENMAX)
#else
	if (pPacket->cNameLen >= PARTYNAMELENMAX)
#endif // #if defined( PRE_PARTY_DB )
		return ERROR_PARTY_CREATE_FAIL;

#if defined( PRE_PARTY_DB )
	if( pPacket->PartyType < _NORMAL_PARTY || pPacket->PartyType >= _PARTY_TYPE_MAX )
		return ERROR_PARTY_CREATE_FAIL;

	if( pPacket->PartyType == _RAID_PARTY_8 )
#else
	if (pPacket->cRaidParty == 1)
#endif // #if defined( PRE_PARTY_DB )
	{
		if (pPacket->cPartyMemberMax > RAIDPARTYCOUNTMAX || pPacket->cPartyMemberMax < RAIDPARTYCOUNTMIN)
			return ERROR_PARTY_CREATE_FAIL;
	}
	else
	{
		if (pPacket->cPartyMemberMax > NORMPARTYCOUNTMAX || pPacket->cPartyMemberMax < NORMPARTYCOUNTMIN)
			return ERROR_PARTY_CREATE_FAIL;
	}

	const sChannelInfo * pChInfo = g_pMasterConnection->GetChannelInfo(GetChannelID());
	if (pChInfo)
	{
#if defined(PRE_ADD_DWC)
		if (pChInfo->nChannelAttribute&(GlobalEnum::CHANNEL_ATT_PVP|GlobalEnum::CHANNEL_ATT_PVPLOBBY|GlobalEnum::CHANNEL_ATT_FARMTOWN|GlobalEnum::CHANNEL_ATT_DWC))
#else
		if (pChInfo->nChannelAttribute&GlobalEnum::CHANNEL_ATT_PVP || pChInfo->nChannelAttribute&GlobalEnum::CHANNEL_ATT_PVPLOBBY || pChInfo->nChannelAttribute&GlobalEnum::CHANNEL_ATT_FARMTOWN)
#endif
			return ERROR_PARTY_CREATE_FAIL;

#if defined( PRE_PARTY_DB )
		if (pPacket->PartyType != _NORMAL_PARTY && pChInfo->nChannelAttribute&GlobalEnum::CHANNEL_ATT_DARKLAIR)
#else
		if (pPacket->cRaidParty >= 1 && pChInfo->nChannelAttribute&GlobalEnum::CHANNEL_ATT_DARKLAIR)
#endif // #if defined( PRE_PARTY_DB )
			return ERROR_PARTY_CREATE_FAIL;
	}
	else
	{
		return ERROR_PARTY_CREATE_FAIL;
	}			

#if defined( PRE_PARTY_DB )
	int iPermitLevel = g_pDataManager->GetMapPermitLevel( GetMapIndex() );
	if( pPacket->cUserLvLimitMin > GetLevel() || pPacket->cUserLvLimitMin <= 0 || pPacket->cUserLvLimitMin < iPermitLevel )
#else
	if (pPacket->cUserLvLimitMin > pPacket->cUserLvLimitMax || pPacket->cUserLvLimitMin > GetLevel() || pPacket->cUserLvLimitMin <= 0 || pPacket->cUserLvLimitMax > 100)
#endif // #if defined( PRE_PARTY_DB )
		return ERROR_PARTY_CREATE_FAIL_RESTRICT_LEVEL_ERROR;

	if ( pPacket->ItemLootRule >= ITEMLOOTRULE_MAX)
		return ERROR_PARTY_CREATE_FAIL;

	if ( pPacket->ItemRank > ITEMRANK_NONE)
		return ERROR_PARTY_CREATE_FAIL;

#if defined( PRE_PARTY_DB )
	_wcscpy(wszName, PARTYNAMELENMAX, pPacket->wszPartyName, static_cast<int>(wcslen(pPacket->wszPartyName)));
	int nPass = pPacket->iPassword;
#else
	_wcscpy(wszName, PARTYNAMELENMAX, pPacket->wszBuf, pPacket->cNameLen);
	if (pPacket->cPassWordLen > 0)
		_wcscpy(wszPass, PARTYPASSWORDMAX, pPacket->wszBuf + pPacket->cNameLen, pPacket->cPassWordLen);

	int nPass = _wtoi(wszPass);
#endif // #if defined( PRE_PARTY_DB )

	if( nPass < 0 || nPass > MAXPARTYPASSWORD )
		return ERROR_PARTY_CREATE_FAIL_PASSWORD_ERR;

#if defined( PRE_PARTY_DB )
	if ( m_pDBCon && m_pDBCon->GetActive() )
		m_pDBCon->QueryAddParty( this, g_Config.nManagedID, Party::LocationType::Village, GetChannelID(), pPacket );
#else
	//생성타이밍 뒤로 파티아이디 관리가 단독으로는 불가능해서 마스터가 생성해서 알려줌
	if (g_pMasterConnection && g_pMasterConnection->GetActive())
		g_pMasterConnection->SendReqPartyID(GetAccountDBID(), GetChannelID(), pPacket->cPartyMemberMax, pPacket->ItemLootRule, pPacket->ItemRank, pPacket->cUserLvLimitMin, pPacket->cUserLvLimitMax, pPacket->nTargetMapIdx, pPacket->Difficulty, pPacket->cIsJobDice, wszName, wszPass, 0, pPacket->cUseVoice > 0 ? true : false);
#endif // #if defined( PRE_PARTY_DB )
	else
		return ERROR_PARTY_CREATE_FAIL;

	return ERROR_NONE;
}

int CDNUserSession::_RecvPartyJoin( const CSJoinParty* pPacket, int iLen )
{
#if defined(PRE_ADD_DWC)
	if(IsDWCCharacter())
		return ERROR_DWC_FAIL;
#endif
	if( m_eUserState == STATE_READY )
		return ERROR_PARTY_JOINFAIL_INREADYSTATE;

	if( GetPartyID() > 0 )
		return ERROR_PARTY_JOINFAIL;

	if( m_bIsMove || m_bIsStartGame )
		return ERROR_PARTY_JOINFAIL;

#if defined( PRE_PARTY_DB )
	if( sizeof(CSJoinParty) != iLen )
#else
	if( sizeof(CSJoinParty) - sizeof(pPacket->wszBuf) +(pPacket->cPartyPasswordSize * sizeof(WCHAR)) != iLen )
#endif // #if defined( PRE_PARTY_DB )
		return ERROR_INVALIDPACKET;

#if defined( PRE_PARTY_DB )
#else
	if( pPacket->cPartyPasswordSize >= PARTYPASSWORDMAX )
		return ERROR_PARTY_JOINFAIL;

	WCHAR wszPass[PARTYPASSWORDMAX] = { 0, };
	if( pPacket->cPartyPasswordSize > 0 )
		_wcscpy( wszPass, PARTYPASSWORDMAX, pPacket->wszBuf, pPacket->cPartyPasswordSize );
#endif // #if defined( PRE_PARTY_DB )

	// 상태 해제
	IsWindowStateNoneSet(WINDOW_ISACCEPT);

	CDNParty* pParty = g_pPartyManager->GetParty( pPacket->PartyID );
	if( pParty == NULL )
	{
#if defined( PRE_PARTY_DB )
		m_PrevPartyListInfo.Clear();
#endif // #if defined( PRE_PARTY_DB )
		return ERROR_PARTY_JOINFAIL;
	}

	pParty->InviteDenied(GetCharacterName());

	if( pParty->CheckSameGateNo() != -1 || pParty->GetCompleteMember() || pParty->bIsMove() || pParty->bIsStartGame() )
		return ERROR_PARTY_JOINFAIL_INREADYSTATE;

	if (pParty->HasEmptyPlace() == false)
		return ERROR_PARTY_JOINFAIL;

#if defined( PRE_PARTY_DB )
#else
	CDNUserSession * pUser = g_pUserSessionManager->FindUserSessionByAccountDBID(pParty->GetLeaderAccountDBID());
	if (pUser == NULL)
		return ERROR_PARTY_JOINFAIL;
#endif // #if defined( PRE_PARTY_DB )

	if (pParty->GetMinLevel() > GetLevel())
		return ERROR_PARTY_JOINFAIL_CHARACTERLEVEL_SHORTAGE;

#if defined( PRE_PARTY_DB )
#else
	if (pParty->GetMaxLevel() < GetLevel())
		return ERROR_PARTY_JOINFAIL_CHARACTERLEVEL_OVER;
#endif // #if defined( PRE_PARTY_DB )

	if (pParty->bIsPasswordParty() == true )
	{
#if defined( PRE_PARTY_DB )
		if( pParty->CheckPassword(pPacket->iPassword) == false )
#else
		if( pParty->CheckPassword(wszPass) == false )
#endif // #if defined( PRE_PARTY_DB )
			return ERROR_PARTY_JOINFAIL_PASSWORDINCORRECT;
	}

	if (pParty->IsKickedMember(GetAccountDBID()))
		return ERROR_PARTY_JOINFAIL_YOU_ARE_KICKEDUSER;

#if defined( PRE_WORLDCOMBINE_PARTY )
	if(Party::bIsWorldCombineParty(pParty->GetPartyType()))
	{
		WorldCombineParty::WrldCombinePartyData* WorldCombinePartyData = g_pDataManager->GetCombinePartyData(pParty->GetWorldPartyPrimaryIndex());
		if( WorldCombinePartyData )
		{
			// 입장권체크
			if(WorldCombinePartyData->nItemID > 0)
			{
				if( GetItem()->GetInventoryItemCount( WorldCombinePartyData->nItemID ) <= 0 )
					return ERROR_PARTY_JOINFAIL;
			}
			// 입장횟수체크
			const TMapInfo* pMapInfo = g_pDataManager->GetMapInfo( pParty->GetTargetMapIndex() );
			if( pMapInfo && pMapInfo->nMaxClearCount > 0 )
			{
				int nExpandNestClearCount = m_nExpandNestClearCount;
				int nPCBangExpandNestClearCount = g_pDataManager->GetPCBangNestClearCount(GetPCBangGrade(), pParty->GetTargetMapIndex());
				if (!pMapInfo->bExpandable)
				{
					nExpandNestClearCount = 0;
					nPCBangExpandNestClearCount = 0;
				}
				if( GetNestClearTotalCount( pParty->GetTargetMapIndex() ) >= (pMapInfo->nMaxClearCount + nExpandNestClearCount + nPCBangExpandNestClearCount) ) {
					return ERROR_PARTY_JOINFAIL;
				}
			}
		}		
		else
		{
			return ERROR_PARTY_JOINFAIL;
		}
	}
#endif
#if defined( PRE_PARTY_DB )

	if( m_pDBCon == NULL || m_pDBCon->GetActive() == false )
		return ERROR_PARTY_JOINFAIL;

	if( pParty->bIsInVillage() )
	{
		if( pParty->GetPartyStructData().iServerID == g_Config.nManagedID && pParty->GetChannelID() == GetChannelID() )
		{
			m_pDBCon->QueryJoinParty( this, this->GetWorldSetID(), pParty->GetPartyID(), pParty->GetMemberMax() );		
		}
		else
		{
			ModCommonVariableData( CommonVariable::Type::MOVESERVER_PARTYID, pParty->GetPartyID() );
			CmdSelectChannel( pParty->GetChannelID(), true );
		}
	}
	else if( pParty->bIsInGame() )
	{
		const TMapInfo* pMapInfo = g_pDataManager->GetMapInfo( pParty->GetPartyMapIndex() );
		if( pMapInfo == NULL )
			return ERROR_PARTY_JOINFAIL;

		if( pMapInfo->MapType != GlobalEnum::MAP_WORLDMAP )
			return ERROR_PARTY_JOINFAIL;

		INT64 biValue = MAKELONG64( pParty->GetServerID(),pParty->GetPartyStructData().iRoomID );
		g_pMasterConnection->SendConfirmLastDungeonInfo( this, biValue, BreakInto::Type::PartyJoin );
	}
	else
	{
		return ERROR_PARTY_JOINFAIL;
	}

#else
	if (pParty->GetChannelID() != GetChannelID())
	{
		const sChannelInfo * pChInfo = g_pMasterConnection->GetChannelInfo(GetChannelID());
		const sChannelInfo * pPartyChInfo = g_pMasterConnection->GetChannelInfo(pParty->GetChannelID());
		if (pChInfo && pPartyChInfo)
		{
			if (pChInfo->nMapIdx != pPartyChInfo->nMapIdx)
				return ERROR_PARTY_JOINFAIL;
		}
		else
			return ERROR_PARTY_JOINFAIL;

		if (!g_pMasterConnection || g_pMasterConnection->GetActive() == false)
			return ERROR_PARTY_JOINFAIL;
	}

	int nMemberIdx = -1;
	if( pParty->AddPartyMember(GetAccountDBID(), GetSessionID(), GetCharacterName(), nMemberIdx ) == false )
		return ERROR_PARTY_JOINFAIL;
	if( pParty->HasEmptyPlace() == false )
		g_pPartyManager->EraseWaitPartyList(pParty);

	m_PartyID = pParty->GetPartyID();
	m_nPartyMemberIndex = nMemberIdx;

	SendJoinParty(ERROR_NONE, pParty->GetPartyID(), pParty->GetPartyName() );
	pParty->SendAllRefreshParty(PARTYREFRESH_NONE);
	pParty->SendPartyMemberMoveEachOther(GetSessionID(), m_BaseData.TargetPos.nX, m_BaseData.TargetPos.nY, m_BaseData.TargetPos.nZ);
	if( GetProfile()->bOpenPublic )
		pParty->SendUserProfile( GetSessionID(), *GetProfile() );

	if( pParty->GetChannelID() != GetChannelID() )
		g_pMasterConnection->SendVillageToVillage(REQINFO_TYPE_SINGLE_SAMEDEST, GetAccountDBID(), GetMapIndex(), -1, pParty->GetChannelID());
#endif // #if defined( PRE_PARTY_DB )

	return ERROR_NONE;
}

int	CDNUserSession::_RecvPartyOut( int iLen )
{
	if( iLen != 0 )
		return ERROR_INVALIDPACKET;

	if (m_eUserState == STATE_READY)
		return ERROR_PARTY_LEAVEFAIL;

	if (GetPartyID() <= 0)
		return ERROR_PARTY_LEAVEFAIL;

	CDNParty* pParty = g_pPartyManager->GetParty(GetPartyID());
	if( pParty == NULL )
	{
		SendPartyOut(ERROR_NONE, -1, GetSessionID());
		ClearPartyInfo();
		return ERROR_NONE;
	}

#if defined( PRE_PARTY_DB )
	g_pPartyManager->QueryOutParty( GetPartyID(), GetAccountDBID(), GetCharacterDBID(), GetSessionID(), Party::QueryOutPartyType::Normal );
	return ERROR_NONE;
#else
	return g_pPartyManager->DelPartyMember( pParty, this );
#endif // #if defined( PRE_PARTY_DB )
}

int	CDNUserSession::_RecvPartyListInfo( const CSPartyListInfo* pPacket, int iLen )
{
	if (sizeof(CSPartyListInfo) != iLen)
		return ERROR_INVALIDPACKET;

	if (GetPartyID() > 0)
		return ERROR_PARTY_LIST_ERROR;

	int nRetCode = ERROR_NONE;
#if defined( PRE_PARTY_DB )

	DWORD dwCurTime = timeGetTime();
	if( m_PrevPartyListInfo == *pPacket )
	{
		if( dwCurTime-m_dwPrevPartyListInfoTick < Party::Constants::PARTY_LIST_REFRESH_SEC_CLIENT*1000 )
			return ERROR_PARTY_REFRESHLIST_TIMERESTRAINT;
	}
	
	m_PrevPartyListInfo = *pPacket;
	m_dwPrevPartyListInfoTick = dwCurTime;

	std::list<CDNParty*> lList;
	g_pPartyManager->GetPartyListInfo(GetMapIndex(), GetChannelID(), g_pMasterConnection->GetMeritBonusID(GetChannelID()), PARTYLISTOFFSET, pPacket, lList );
#else
	std::list <SPartyListInfo> lList;
	int nWholeCount = g_pPartyManager->GetPartyListInfo(GetMapIndex(), GetChannelID(), g_pMasterConnection->GetMeritBonusID(GetChannelID()), PARTYLISTOFFSET, pPacket->cGetPage, const_cast<CSPartyListInfo*>(pPacket)->nSortMapIdx, pPacket->Difficulty, nRetCode, &lList);
	if( pPacket->cIsRefresh > 0 )
		nRetCode = ERROR_NONE;
#endif // #if defined( PRE_PARTY_DB )

#if defined( PRE_PARTY_DB )

	if( lList.empty() == false )
	{
		if( pPacket->unReqPage*PARTYLISTOFFSET >= (int)(lList.size()) )
			nRetCode = ERROR_PARTY_PARTYLIST_OVERFLOW;
	}

	SendPartyList( nRetCode, pPacket, lList );
#else
	SendPartyList( &lList, nWholeCount, nRetCode );
#endif // #if defined( PRE_PARTY_DB )
	return ERROR_NONE;
}

#if defined(PRE_MOD_REQ_JOIN_PARTY_ANSWER_MSG_APP)
int	CDNUserSession::_RecvPartyInfo( const CSPartyInfo* pPacket, int iLen )
{
	if (sizeof(CSPartyInfo) != iLen)
		return ERROR_INVALIDPACKET;
	
	CDNParty* pParty = NULL;
	if( pPacket->PartyID > 0 )
	{
		pParty = g_pPartyManager->GetParty(pPacket->PartyID);
	}
	else
	{
		if( CheckLastSpace(pPacket->wszCharName) )
			return ERROR_PARTY_JOINFAIL;

		if( !__wcsicmp_l(pPacket->wszCharName, GetCharacterName()) )
			return ERROR_PARTY_JOINFAIL;

		CDNUserSession* pUser = g_pUserSessionManager->FindUserSessionByName(pPacket->wszCharName);
		if( !pUser )
		{	//마스터 서버에 파티ID 요청
			if (!g_pMasterConnection || g_pMasterConnection->GetActive() == false)
				return ERROR_PARTY_ASKJOIN_PARTY_NOTFOUND;

			g_pMasterConnection->SendGetPartyID( GetAccountDBID(), pPacket->wszCharName );
			return ERROR_NONE;
		}

		pParty = g_pPartyManager->GetParty(pUser->GetPartyID());
	}

	SendPartyInfo(ERROR_NONE, pParty);
	return ERROR_NONE;
}
#endif

int CDNUserSession::_RecvPartyRefreshGateInfo( const CSRefreshGateInfo* pPacket, int iLen )
{
	if( sizeof(CSRefreshGateInfo) != iLen )
		return ERROR_INVALIDPACKET;

	if( m_pField == NULL )
		return ERROR_NONE;

	TPosition Pos = { 0, };
	Pos.nX = (int)(pPacket->Position.x * 1000.f);
	Pos.nY = (int)(pPacket->Position.y * 1000.f);
	Pos.nZ = (int)(pPacket->Position.z * 1000.f);

	if (m_pField == NULL)
	{
		char Gate[PARTYMAX] = { 0, };
		Gate[0] = g_pFieldDataManager->CheckGateArea(GetMapIndex(), Pos);
		SendRefreshGateInfo(Gate, 1, ERROR_PARTY_CANT_ENTERGATE);
		return ERROR_NONE;
	}

	int GateType = -1;
	int GateNo = -1;
	if( pPacket->boEnter )
	{
		if (m_cGateNo > 0)
			return ERROR_NONE;

		if (m_pField)
		{
			GateNo = m_pField->GetFieldData()->CheckGateArea(Pos);
			GateType = m_pField->GetFieldData()->GetGateType(Pos);
		}
		else
			GateNo = g_pFieldDataManager->CheckGateArea(GetMapIndex(), Pos);

		if (GateNo < 0) 
			GateNo = -1;
	}
	else 
	{
		GateNo = -1;
	}

	m_cGateNo = GateNo;		// GateNo 세팅해주고
	m_nGateType = GateType;
	m_cGateSelect = 0;

	const sChannelInfo * pChannelInfo = g_pMasterConnection->GetChannelInfo(GetChannelID());
	if (pChannelInfo == NULL)
		return ERROR_GENERIC_UNKNOWNERROR;

	int TargetMapIndex = GetTargetMapIndex();

	if (GetPartyID() > 0)
	{
		CDNParty *pParty = g_pPartyManager->GetParty(GetPartyID());
		if( pParty == NULL )
			return ERROR_GENERIC_UNKNOWNERROR;

		pParty->SendAllRefreshGateInfo();

		if ((GateNo = pParty->CheckSameGateNo()) > 0)
		{
			if (g_pDataManager->GetMapType(TargetMapIndex) != GlobalEnum::MAP_VILLAGE )
			{
				if (pChannelInfo->nChannelAttribute&GlobalEnum::CHANNEL_ATT_FARMTOWN || GateNo == 11 && TargetMapIndex == 15106)
					pParty->SendAllFarmInfo();
				else
					pParty->SendAllGateInfo(GateNo);
			}
			else
			{
				if (g_pMasterConnection && g_pMasterConnection->GetActive())
					g_pMasterConnection->SendVillageToVillage(REQINFO_TYPE_PARTY, pParty->GetPartyID(), TargetMapIndex, GateNo, -1, pParty);
			}
		}
	}
	else 
	{
		char Gate[PARTYMAX] = { 0, };
		Gate[0] = GateNo;
		SendRefreshGateInfo(Gate, 1, ERROR_NONE);
		if ( GateNo > 0 )
		{
			m_nSingleRandomSeed = timeGetTime();//_rand();
			m_eUserState = STATE_READY;
			if (g_pDataManager->GetMapType(TargetMapIndex) != GlobalEnum::MAP_VILLAGE)
			{
				if (pChannelInfo->nChannelAttribute&GlobalEnum::CHANNEL_ATT_FARMTOWN || GateNo == 11 && TargetMapIndex == 15106)
					ReqFarmInfo();
				else
					SendGateInfo(GetSessionID(), GateNo, 0, NULL, NULL);
			}
			else
			{
				if (g_pMasterConnection && g_pMasterConnection->GetActive())
					g_pMasterConnection->SendVillageToVillage(REQINFO_TYPE_SINGLE, GetAccountDBID(), GetMapIndex(), m_cGateNo);
			}
		}
	}

	return ERROR_NONE;
}

int	CDNUserSession::_RecvPartySelectStage( const CSSelectStage* pPacket, int iLen )
{
	if (sizeof(CSSelectStage) != iLen)
		return ERROR_INVALIDPACKET;

	if( GetPartyID() > 0 )
	{
		CDNParty *pParty = g_pPartyManager->GetParty(GetPartyID());
		if( pParty)
			pParty->SendSelectStage(pPacket->cSelectMapIndex);
	}
	else
		SendSelectStage(pPacket->cSelectMapIndex);

	return ERROR_NONE;
}

int	CDNUserSession::_RecvPartyStartStage( const CSStartStage* pPacket, int iLen )
{
	if (sizeof(CSStartStage) != iLen)
		return ERROR_INVALIDPACKET;

	if( GetPartyID() > 0 )
	{
		CDNParty *pParty = g_pPartyManager->GetParty(GetPartyID());
		if( pParty == NULL )
		{
			if (!IsNoneWindowState())
				return ERROR_CASHSHOP_OPEN;
		}
		else
		{
			if( !pParty->IsPartyMemberWindowStateNone() )
				pParty->SendAllCancelStage( ERROR_CASHSHOP_OPEN );
		}
	}
	else 
	{
		if (!IsNoneWindowState())
		{
			m_cGateNo = -1;
			m_cGateSelect = -1;

			return ERROR_CASHSHOP_OPEN;
		}
	}

	if (m_bIsStartGame || m_bIsMove)
	{
		g_Log.Log(LogType::_GAMECONNECTLOG, this, L"CS_STARTSTAGE Repetitive Move\n");
		return ERROR_PARTY_STARTSTAGE_FAIL;
	}

	int TargetMapIndex = -1;
	int TargetGateNo = -1;

	m_cGateSelect = pPacket->cSelectMapIndex;
	SendSelectStage(m_cGateSelect);

	TargetMapIndex = GetTargetMapIndex();
	TargetGateNo = g_pDataManager->GetGateNoByGateNo(GetMapIndex(), m_cGateNo, m_cGateSelect);

	if( TargetMapIndex <= 0 || m_pField == NULL )
	{
		if (GetPartyID() > 0)
		{
			CDNParty *pParty = g_pPartyManager->GetParty(GetPartyID());
			if( pParty == NULL )
				return ERROR_PARTY_STARTSTAGE_FAIL;

			pParty->SendAllCancelStage( ERROR_PARTY_STARTSTAGE_FAIL );
		}
		else 
		{
			m_cGateNo = -1;
			m_cGateSelect = -1;

			return ERROR_PARTY_STARTSTAGE_FAIL;
		}

		return ERROR_NONE;
	}

	// 파티가 Ready 된 후에 클라이언트에서 임의로 움직이거나 했을 경우 움직인 놈이 RefreshGateInfo 를 요청하면서
	// 그 클라이언트의 GateIndex 가 -1 이 되버린다.
	// 그럴 경우 파티원 전원이 시작이 안되기땜에 일단 여기서 파티원중 Ready 가 풀린놈이 있으면 시작을 못하게 하자.
	// 혹은 StartStage 할때 다시 GateIndex 와 RandomSeed 를 보내줘도 되는데.. 이럴 경우 풀린놈이 거래따위를 하고
	// 있다가 강제로 이동될 시 무슨일이 일어날지 모르니 일단은 Cancel 을 시키는 쪽으로 해두는 것이 좋을듯 함.
	// 자기자신(리더 혹은 개인) 은 위에서 이미 게이트가 -1 일 경우에 TargetMapIndex 가 -1 나오면서 켄슬되므로 무시~
	if( g_pMasterConnection->GetActive() == false )
		return ERROR_GENERIC_MASTERCON_NOT_FOUND;

	const sChannelInfo * pChannelInfo = g_pMasterConnection->GetChannelInfo(GetChannelID());
	if (pChannelInfo == NULL)
		return ERROR_PARTY_STARTSTAGE_FAIL;

	int nFarmMapID = -1;
	bool bEnterFarm = false;
	bEnterFarm = g_pFarm->GetFarmMapID(pPacket->nFarmDBID, nFarmMapID);

	GameTaskType::eType TaskType = m_pField->GetGameTaskType();
	if (m_nGateType > 0)
		TaskType = (GameTaskType::eType)m_nGateType;

	CDNParty* pParty = NULL;

#if defined(_FINAL_BUILD)
	if ((GetAccountLevel() <= AccountLevel_QA) && (GetAccountLevel() >= AccountLevel_New))
	{
		// GM 은 패스한다.
	}
	else
#endif // #if defined(_FINAL_BUILD)
	{
		if (!g_pDataManager->CheckChangeMap(GetMapIndex(), TargetMapIndex))
		{
			WCHAR wszBuf[100];
			wsprintf( wszBuf, L"VILLAGE Prev[%d]->Next[%d]", GetMapIndex(), TargetMapIndex);
			GetDBConnection()->QueryAddAbuseLog(this, ABUSE_MOVE_SERVERS, wszBuf);
		}
	}

	if (GetPartyID() > 0 ) 
	{
		if (bEnterFarm)
			return ERROR_PARTY_STARTSTAGE_FAIL;

		if( m_boPartyLeader == false )
			return ERROR_PARTY_STARTSTAGE_FAIL;

		pParty = g_pPartyManager->GetParty(GetPartyID());
		if( pParty == NULL )
			return ERROR_PARTY_STARTSTAGE_FAIL;

		//파티상태라면 중복이동체크를 전유저에 걸처서 해보자
		if( pParty->CheckPartyUserStartFlag() == false )
		{
			g_Log.Log(LogType::_GAMECONNECTLOG, this, L"CS_STARTSTAGE Repetitive Move\n");
			return ERROR_PARTY_STARTSTAGE_FAIL;
		}

		if( pParty->DiffMemberGateNo(m_cGateNo) )
		{
			if (g_pMasterConnection && g_pMasterConnection->GetActive())
			{
				pParty->SetStartGame( true );
				pParty->SendSelectStage(m_cGateSelect);

				g_pMasterConnection->SendReqGameID( TaskType, REQINFO_TYPE_PARTY, pParty->GetPartyID(), GetChannelID(), pParty->GetRandomSeed(), GetMapIndex(), m_cGateNo, pPacket->Difficulty, false, pParty, 0, m_cGateSelect );		// GameID를 요청
			}
			else
			{
				pParty->SendAllCancelStage(ERROR_PARTY_STARTSTAGE_FAIL);
				return ERROR_NONE;
			}
		}
		else
		{
			pParty->SendAllCancelStage(ERROR_PARTY_STARTSTAGE_FAIL);
			return ERROR_NONE;
		}
	}
	else
	{
		if (g_pMasterConnection && g_pMasterConnection->GetActive())
		{
			if (bEnterFarm)
			{
#if defined( PRE_ADD_FARM_DOWNSCALE )
				int iAttr = Farm::Attr::None;
				if( g_pFarm->GetFarmAttr( pPacket->nFarmDBID, iAttr ) == false )
					return ERROR_NONE;

#if defined( PRE_ADD_VIP_FARM )
				if( iAttr&Farm::Attr::Vip )
				{
					if( bIsFarmVip() == false )
						return ERROR_FARM_INVALID_VIP;
				}
#endif // #if defined( PRE_ADD_VIP_FARM )
				if( iAttr&Farm::Attr::GuildChampion )
				{
					// 길드농장 입장 검사
					if( GetGuildUID().IsSet() == false || g_pGuildWarManager->GetPreWinGuildUID().IsSet() == false || g_pGuildWarManager->GetPreWinGuildUID() != GetGuildUID() )
						return ERROR_FARM_ENTERANCE_CONDITION;
				}
#elif defined( PRE_ADD_VIP_FARM )
				Farm::Attr::eType Attr = Farm::Attr::None;
				if( g_pFarm->GetFarmAttr( pPacket->nFarmDBID, Attr ) == false )
				{
					_DANGER_POINT();
					return ERROR_NONE;
				}

				switch( Attr )
				{
				case Farm::Attr::Vip:
					{
						if( bIsFarmVip() == false )
							return ERROR_FARM_INVALID_VIP;
						break;
					}
				}
#endif // #if defined( PRE_ADD_FARM_DOWNSCALE )
				SetLastSubVillageMapIndex( GetMapIndex() );
				g_pMasterConnection->SendReqGameID( GameTaskType::Farm, REQINFO_TYPE_FARM, m_nAccountDBID, GetChannelID(), timeGetTime(), nFarmMapID, 0, CONVERT_TO_DUNGEONDIFFICULTY(0), true, NULL, pPacket->nFarmDBID );
			}
			else
			{
				g_pMasterConnection->SendReqGameID( TaskType, REQINFO_TYPE_SINGLE, m_nAccountDBID, GetChannelID(), m_nSingleRandomSeed, GetMapIndex(), m_cGateNo, pPacket->Difficulty, false, NULL, 0, m_cGateSelect );	// GameID를 요청
			}
		}
		else
			return ERROR_PARTY_STARTSTAGE_FAIL;
	}

	//여기까지 오면 마스터에 보낸것이다. 리턴메세지를 받기전까지는 제한처리를 위한 플래그세팅해주자
	if (pParty)
	{
		//pParty가 유효하다면 파티단위 이동이 이루어진 것이다. 파티유저들의 플래그를 바꾸어 준다.
		pParty->SetPartyUserStartFlag(true);
	}
	else
	{
		//아니라면 개인이동
		m_bIsStartGame = true;
	}

	g_Log.Log(LogType::_NORMAL, this, L"[ADBID:%u, CDBID:%I64d, SID:%u, ObjID:%u] [CS_STARTSTAGE] TargetMap:%d, MapIndex:%d GateNo:%d\r\n", m_nAccountDBID, m_biCharacterDBID, GetSessionID(), m_BaseData.nObjectID, TargetMapIndex, GetMapIndex(), m_cGateNo);
	return ERROR_NONE;
}

int CDNUserSession::_RecvPartyCancelStage( int iLen )
{
	if( iLen != 0)
		return ERROR_INVALIDPACKET;

	if( GetPartyID() > 0 )
	{
		CDNParty *pParty = g_pPartyManager->GetParty(GetPartyID());
		if( pParty == NULL )
			return ERROR_NONE;

		if( pParty->bIsStartGame() == false )
			pParty->SendAllCancelStage( ERROR_NONE );
	}
	else 
	{
		m_eUserState = STATE_NONE;
		SendCancelStage( ERROR_NONE );
	}

	return ERROR_NONE;
}

#if defined(PRE_FIX_62281)
int CDNUserSession::_RecvPartyInvite( const CSInvitePartyMember* pPacket, int iLen )
{
	if( sizeof(CSInvitePartyMember)-sizeof(pPacket->wszBuf)+(pPacket->cInviteMemberSize*sizeof(WCHAR)) != iLen )
		return ERROR_INVALIDPACKET;

	if( pPacket->cInviteMemberSize <= 0 || pPacket->cInviteMemberSize >= NAMELENMAX )
		return ERROR_PARTY_INVITEFAIL;

#if defined(PRE_ADD_DWC)
	if(IsDWCCharacter())
		return ERROR_DWC_FAIL;
#endif

	WCHAR wszName[NAMELENMAX];
	memset(wszName, 0, sizeof(wszName));
	_wcscpy(wszName, NAMELENMAX, pPacket->wszBuf, pPacket->cInviteMemberSize);

	if( CheckLastSpace(wszName) )
		return ERROR_PARTY_INVITEFAIL;

	if( !__wcsicmp_l(wszName, GetCharacterName()) )
		return ERROR_PARTY_INVITEFAIL;

	if (m_eUserState != STATE_NONE)		//대상이 이동중이거나 레디 등등의 상태이면 초대 불가하다
		return ERROR_PARTY_INVITEFAIL;	

	const sChannelInfo* pChInfo = g_pMasterConnection->GetChannelInfo(GetChannelID());
	if( pChInfo == NULL )
		return ERROR_PARTY_INVITEFAIL;

#if defined(PRE_ADD_DWC)
	if (pChInfo->nChannelAttribute&(GlobalEnum::CHANNEL_ATT_PVP|GlobalEnum::CHANNEL_ATT_PVPLOBBY|GlobalEnum::CHANNEL_ATT_FARMTOWN|GlobalEnum::CHANNEL_ATT_DWC))
#else
	if (pChInfo->nChannelAttribute&GlobalEnum::CHANNEL_ATT_PVP || pChInfo->nChannelAttribute&GlobalEnum::CHANNEL_ATT_PVPLOBBY || pChInfo->nChannelAttribute&GlobalEnum::CHANNEL_ATT_FARMTOWN )
#endif
		return ERROR_PARTY_INVITEFAIL;

	if( m_pIsolate->IsIsolateItem(wszName) )
		return ERROR_ISOLATE_REQUESTFAIL;
	
	//파티 확인
	CDNParty* pParty = g_pPartyManager->GetParty(GetPartyID());
	if(pParty)
	{
		if( pParty->GetLeaderAccountDBID() != m_nAccountDBID )
			return ERROR_PARTY_INVITEFAIL;

		if( pParty->IsKickedMember(GetAccountDBID()) )
			return ERROR_PARTY_INVITEFAIL_DESTUSER_SITUATION_NOTALLOWED;

		if( pParty->bIsMove() || pParty->bIsStartGame() )
			return ERROR_PARTY_INVITEFAIL;

		if( pParty->HasEmptyPlace() == false )
			return ERROR_PARTY_INVITEFAIL;

		//상대방 확인
		CDNUserSession* pUser = g_pUserSessionManager->FindUserSessionByName(wszName);
		if( pUser == NULL )
		{
#if defined( PRE_PARTY_DB )
			//마스터 서버에 Invite 요청
			if( pParty->InvitePartyMember(wszName) == false )	//m_VecInviteList 에 추가
				return ERROR_PARTY_INVITEFAIL_ALREADY_INVITED;
			g_pMasterConnection->SendReqPartyInvite( this, wszName, pParty );
			return ERROR_NONE;
#else
			return ERROR_PARTY_INVITEFAIL_DESTUSER_NOTFOUND;
#endif // #if defined( PRE_PARTY_DB )
		}

		//상대가 초대 받을 수 있는 상황인지 체크
#if defined(PRE_ADD_DWC)
		if(pUser->IsDWCCharacter())
			return ERROR_PARTY_INVITEFAIL_DESTUSER_NOTFOUND;
#endif
#if defined(PRE_PARTY_DB)
		int nRet = pUser->CheckPartyInvitableState(GetCharacterName(), pParty->GetMinLevel());
#else
		int nRet = pUser->CheckPartyInvitableState(GetCharacterName(), pParty->GetMinLevel(), pParty->GetMaxLevel());
#endif
		if(nRet != ERROR_NONE)
			return nRet;

		if( pParty->InvitePartyMember(wszName) == false )
			return ERROR_PARTY_INVITEFAIL_ALREADY_INVITED;

		pUser->SetWindowState(WINDOW_ISACCEPT);
#if defined( PRE_PARTY_DB )
		pUser->SendPartyInviteMsg(pParty->GetPartyID(), pParty->GetPartyName(), pParty->GetPassword(), GetCharacterName(), (BYTE)pParty->GetMemberMax(), (BYTE)pParty->GetMemberCount(), (USHORT)pParty->GetAvrUserLv(), -1, GetMapIndex() ); 
#else
		pUser->SendPartyInviteMsg(pParty->GetPartyID(), pParty->GetPartyName(), pParty->m_wszPartyPassword, GetCharacterName(), (BYTE)pParty->GetMemberMax(), (BYTE)pParty->GetMemberCount(), (USHORT)pParty->GetAvrUserLv()); 
#endif // #if defined( PRE_PARTY_DB )

		return ERROR_NONE;
	}
	else	//파티 새로 생성
	{
		WCHAR wszInvitePartyName[PARTYNAMELENMAX];
		memset(wszInvitePartyName, 0, sizeof(wszInvitePartyName));

#if defined(PRE_ADD_MULTILANGUAGE)
		if (wcslen(GetCharacterName()) + wcslen(CEtUIXML::GetInstance().GetUIString(CEtUIXML::idCategory1, 3535, m_eSelectedLanguage)) >= PARTYNAMELENMAX)
			_wcscpy(wszInvitePartyName, _countof(wszInvitePartyName), GetCharacterName(), (int)wcslen(GetCharacterName()));
		else
			swprintf(wszInvitePartyName, L"%s%s", GetCharacterName(), CEtUIXML::GetInstance().GetUIString(CEtUIXML::idCategory1, 3535, m_eSelectedLanguage));
#else		//#if defined(PRE_ADD_MULTILANGUAGE)
		if (wcslen(GetCharacterName()) + wcslen(CEtUIXML::GetInstance().GetUIString(CEtUIXML::idCategory1, 3535)) >= PARTYNAMELENMAX)
			_wcscpy(wszInvitePartyName, _countof(wszInvitePartyName), GetCharacterName(), (int)wcslen(GetCharacterName()));
		else
			swprintf(wszInvitePartyName, L"%s%s", GetCharacterName(), CEtUIXML::GetInstance().GetUIString(CEtUIXML::idCategory1, 3535));
#endif		//#if defined(PRE_ADD_MULTILANGUAGE)						

#if defined( PRE_PARTY_DB )
		if( m_pDBCon && m_pDBCon->GetActive() )
		{
			CSCreateParty CreatePartyPacket;
			memset( &CreatePartyPacket, 0, sizeof(CreatePartyPacket) );

			CreatePartyPacket.PartyType = _NORMAL_PARTY;
			CreatePartyPacket.cPartyMemberMax = NORMPARTYCOUNTMAX;
			CreatePartyPacket.ItemLootRule = ITEMLOOTRULE_INORDER;
			CreatePartyPacket.ItemRank = ITEMRANK_B;
			int iPermitLevel = g_pDataManager->GetMapPermitLevel(GetMapIndex());
			CreatePartyPacket.cUserLvLimitMin = (iPermitLevel == 0) ? 1 : iPermitLevel ;
			CreatePartyPacket.nTargetMapIdx = 0;
			CreatePartyPacket.Difficulty = Dungeon::Difficulty::Easy;
			if( m_bVoiceAvailable )
				CreatePartyPacket.iBitFlag |= Party::BitFlag::VoiceChat;
			_wcscpy( CreatePartyPacket.wszPartyName, _countof(CreatePartyPacket.wszPartyName), wszInvitePartyName, static_cast<int>(wcslen(wszInvitePartyName)) );

			m_pDBCon->QueryAddParty( this, g_Config.nManagedID, Party::LocationType::Village, GetChannelID(), &CreatePartyPacket, wszName );
		}
#else
		if (g_pMasterConnection && g_pMasterConnection->GetActive())
		{
			CDNUserSession* pUser = g_pUserSessionManager->FindUserSessionByName(wszName);
			if( pUser == NULL )
				return ERROR_PARTY_INVITEFAIL_DESTUSER_NOTFOUND;

			g_pMasterConnection->SendReqPartyID(GetAccountDBID(), GetChannelID(), NORMPARTYCOUNTMAX, ITEMLOOTRULE_INORDER, ITEMRANK_B, 1, 100, 0, 0, 1, wszInvitePartyName, L"", pUser->GetAccountDBID(), m_bVoiceAvailable); // ITEMRANK modified by kalliste 2010.07.09
		}
#endif // #if defined( PRE_PARTY_DB )
		else
			return ERROR_PARTY_INVITEFAIL;		
	}

	return ERROR_NONE;
}

#else
int CDNUserSession::_RecvPartyInvite( const CSInvitePartyMember* pPacket, int iLen )
{
	if( sizeof(CSInvitePartyMember)-sizeof(pPacket->wszBuf)+(pPacket->cInviteMemberSize*sizeof(WCHAR)) != iLen )
		return ERROR_INVALIDPACKET;

	if (m_eUserState != STATE_NONE)		//대상이 이동중이거나 레디 등등의 상태이면 초대 불가하다
		return ERROR_PARTY_INVITEFAIL;

	if( pPacket->cInviteMemberSize <= 0 || pPacket->cInviteMemberSize >= NAMELENMAX )
		return ERROR_PARTY_INVITEFAIL;

	WCHAR wszName[NAMELENMAX];
	memset(wszName, 0, sizeof(wszName));
	_wcscpy(wszName, NAMELENMAX, pPacket->wszBuf, pPacket->cInviteMemberSize);

	if( CheckLastSpace(wszName) )
		return ERROR_PARTY_INVITEFAIL;

	if( !__wcsicmp_l(wszName, GetCharacterName()) )
		return ERROR_PARTY_INVITEFAIL;

	const sChannelInfo* pChInfo = g_pMasterConnection->GetChannelInfo(GetChannelID());
	if( pChInfo == NULL )
		return ERROR_PARTY_INVITEFAIL;
#if defined(PRE_ADD_DWC)
	if (pChInfo->nChannelAttribute&(GlobalEnum::CHANNEL_ATT_PVP|GlobalEnum::CHANNEL_ATT_PVPLOBBY|GlobalEnum::CHANNEL_ATT_FARMTOWN|GlobalEnum::CHANNEL_ATT_DWC))
#else
	if (pChInfo->nChannelAttribute&GlobalEnum::CHANNEL_ATT_PVP || pChInfo->nChannelAttribute&GlobalEnum::CHANNEL_ATT_PVPLOBBY || pChInfo->nChannelAttribute&GlobalEnum::CHANNEL_ATT_FARMTOWN )
#endif
		return ERROR_PARTY_INVITEFAIL;

	if( m_pIsolate->IsIsolateItem(wszName) )
		return ERROR_ISOLATE_REQUESTFAIL;

	CDNParty* pParty = g_pPartyManager->GetParty(GetPartyID());
	if( pParty != NULL )
	{
		if( pParty->GetLeaderAccountDBID() != m_nAccountDBID )
			return ERROR_PARTY_INVITEFAIL;

		if( pParty->bIsMove() || pParty->bIsStartGame() )
			return ERROR_PARTY_INVITEFAIL;

		if( pParty->HasEmptyPlace() == false )
			return ERROR_PARTY_INVITEFAIL;

		CDNUserSession* pUser = g_pUserSessionManager->FindUserSessionByName(wszName);
		if( pUser == NULL )
		{
#if defined( PRE_PARTY_DB )
			if( pParty->InvitePartyMember(wszName) == false )
				return ERROR_PARTY_INVITEFAIL_ALREADY_INVITED;
			g_pMasterConnection->SendReqPartyInvite( this, wszName, pParty );
			return ERROR_NONE;
#else
			return ERROR_PARTY_INVITEFAIL_DESTUSER_NOTFOUND;
#endif // #if defined( PRE_PARTY_DB )
		}

		if( pUser->m_eUserState != STATE_NONE )
			return ERROR_PARTY_INVITEFAIL_DEST_USER_NOWREADY;

		if( pParty->IsKickedMember(pUser->GetAccountDBID()) )
			return ERROR_PARTY_INVITEFAIL_DESTUSER_SITUATION_NOTALLOWED;

		if( pUser->IsAcceptAbleOption(GetCharacterDBID(), GetAccountDBID(), _ACCEPTABLE_CHECKTYPE_PARTYINVITE) == false )
			return ERROR_PARTY_INVITEFAIL;

		if( pUser->GetIsolate()->IsIsolateItem(GetCharacterName()) )
			return ERROR_PARTY_INVITEFAIL;

		if( (pUser->GetAccountLevel() >= AccountLevel_New) && (pUser->GetAccountLevel() <= AccountLevel_Master))
			return ERROR_PARTY_INVITEFAIL_DESTUSER_NOTFOUND;

		const sChannelInfo* pUserChInfo = g_pMasterConnection->GetChannelInfo(pUser->GetChannelID());
		if( pUserChInfo == NULL )
			return ERROR_PARTY_INVITEFAIL;

#if defined( PRE_PARTY_DB )
#else
		if( (pUser->m_cVillageID != m_cVillageID) ||(pUserChInfo->nMapIdx != pChInfo->nMapIdx) )
			return ERROR_PARTY_INVITEFAIL_DESTLOCATION_NOT_SAME;
#endif // #if defined( PRE_PARTY_DB )
#if defined(PRE_ADD_DWC)
		if (pChInfo->nChannelAttribute&(GlobalEnum::CHANNEL_ATT_PVP|GlobalEnum::CHANNEL_ATT_PVPLOBBY|GlobalEnum::CHANNEL_ATT_DWC))
#else
		if( pUserChInfo->nChannelAttribute&GlobalEnum::CHANNEL_ATT_PVP || pUserChInfo->nChannelAttribute&GlobalEnum::CHANNEL_ATT_PVPLOBBY)
#endif
			return ERROR_PARTY_INVITEFAIL_DESTLOCATION_NOT_SAME;

		if( !pUser->IsNoneWindowState())
			return ERROR_PARTY_INVITEFAIL_DESTUSER_SITUATION_NOTALLOWED;

		if( pUser->GetPartyID() > 0)
			return ERROR_PARTY_INVITEFAIL_ALREADY_HAS_PARTY;

		if( pUser->GetLevel() < pParty->GetMinLevel())
			return ERROR_PARTY_INVITEFAIL_CHARACTERLEVEL_SHORTAGE;

#if defined( PRE_PARTY_DB )
#else
		if( pUser->GetLevel() > pParty->GetMaxLevel())
			return ERROR_PARTY_JOINFAIL_CHARACTERLEVEL_OVER;
#endif // #if defined( PRE_PARTY_DB )

		if( pParty->InvitePartyMember(wszName) == false )
			return ERROR_PARTY_INVITEFAIL_ALREADY_INVITED;

		pUser->SetWindowState(WINDOW_ISACCEPT);
#if defined( PRE_PARTY_DB )
		pUser->SendPartyInviteMsg(pParty->GetPartyID(), pParty->GetPartyName(), pParty->GetPassword(), GetCharacterName(), (BYTE)pParty->GetMemberMax(), (BYTE)pParty->GetMemberCount(), (USHORT)pParty->GetAvrUserLv(), -1, GetMapIndex() ); 
#else
		pUser->SendPartyInviteMsg(pParty->GetPartyID(), pParty->GetPartyName(), pParty->m_wszPartyPassword, GetCharacterName(), (BYTE)pParty->GetMemberMax(), (BYTE)pParty->GetMemberCount(), (USHORT)pParty->GetAvrUserLv()); 
#endif // #if defined( PRE_PARTY_DB )

		return ERROR_NONE;
	}

	CDNUserSession* pUser = g_pUserSessionManager->FindUserSessionByName(wszName);
	if( pUser == NULL )
		return ERROR_PARTY_INVITEFAIL_DESTUSER_NOTFOUND;

	if( pUser->m_eUserState != STATE_NONE )
		return ERROR_PARTY_INVITEFAIL_DEST_USER_NOWREADY;

	if( pUser->IsAcceptAbleOption(GetCharacterDBID(), GetAccountDBID(), _ACCEPTABLE_CHECKTYPE_PARTYINVITE) == false )
		return ERROR_PARTY_INVITEFAIL;

	if( pUser->GetIsolate()->IsIsolateItem(GetCharacterName()) )
		return ERROR_PARTY_INVITEFAIL;

	if( (pUser->GetAccountLevel() >= AccountLevel_New) && (pUser->GetAccountLevel() <= AccountLevel_Master))
		return ERROR_PARTY_INVITEFAIL_DESTUSER_NOTFOUND;

	const sChannelInfo* pUserChInfo = g_pMasterConnection->GetChannelInfo(pUser->GetChannelID());
	if( pUserChInfo == NULL )
		return ERROR_PARTY_INVITEFAIL;

#if defined( PRE_PARTY_DB )
#else
	if( (pUser->m_cVillageID != m_cVillageID) ||(pUserChInfo->nMapIdx != pChInfo->nMapIdx))
		return ERROR_PARTY_INVITEFAIL_DESTLOCATION_NOT_SAME;
#endif
#if defined(PRE_ADD_DWC)
	if (pChInfo->nChannelAttribute&(GlobalEnum::CHANNEL_ATT_PVP|GlobalEnum::CHANNEL_ATT_PVPLOBBY|GlobalEnum::CHANNEL_ATT_DWC))
#else
	if( pUserChInfo->nChannelAttribute&GlobalEnum::CHANNEL_ATT_PVP || pUserChInfo->nChannelAttribute&GlobalEnum::CHANNEL_ATT_PVPLOBBY )
#endif
		return ERROR_PARTY_INVITEFAIL_DESTLOCATION_NOT_SAME;

	if( !pUser->IsNoneWindowState())
		return ERROR_PARTY_INVITEFAIL_DESTUSER_SITUATION_NOTALLOWED;

	if( pUser->GetPartyID() > 0 )
		return ERROR_PARTY_INVITEFAIL_ALREADY_HAS_PARTY;

	WCHAR wszInvitePartyName[PARTYNAMELENMAX];
	memset(wszInvitePartyName, 0, sizeof(wszInvitePartyName));

#if defined(PRE_ADD_MULTILANGUAGE)
	if (wcslen(GetCharacterName()) + wcslen(CEtUIXML::GetInstance().GetUIString(CEtUIXML::idCategory1, 3535, m_eSelectedLanguage)) >= PARTYNAMELENMAX)
		_wcscpy(wszInvitePartyName, _countof(wszInvitePartyName), GetCharacterName(), (int)wcslen(GetCharacterName()));
	else
		swprintf(wszInvitePartyName, L"%s%s", GetCharacterName(), CEtUIXML::GetInstance().GetUIString(CEtUIXML::idCategory1, 3535, m_eSelectedLanguage));
#else		//#if defined(PRE_ADD_MULTILANGUAGE)
	if (wcslen(GetCharacterName()) + wcslen(CEtUIXML::GetInstance().GetUIString(CEtUIXML::idCategory1, 3535)) >= PARTYNAMELENMAX)
		_wcscpy(wszInvitePartyName, _countof(wszInvitePartyName), GetCharacterName(), (int)wcslen(GetCharacterName()));
	else
		swprintf(wszInvitePartyName, L"%s%s", GetCharacterName(), CEtUIXML::GetInstance().GetUIString(CEtUIXML::idCategory1, 3535));
#endif		//#if defined(PRE_ADD_MULTILANGUAGE)						

#if defined( PRE_PARTY_DB )
	if( m_pDBCon && m_pDBCon->GetActive() )
	{
		CSCreateParty CreatePartyPacket;
		memset( &CreatePartyPacket, 0, sizeof(CreatePartyPacket) );

		CreatePartyPacket.PartyType = _NORMAL_PARTY;
		CreatePartyPacket.cPartyMemberMax = NORMPARTYCOUNTMAX;
		CreatePartyPacket.ItemLootRule = ITEMLOOTRULE_INORDER;
		CreatePartyPacket.ItemRank = ITEMRANK_B;
		int iPermitLevel = g_pDataManager->GetMapPermitLevel(GetMapIndex());
		CreatePartyPacket.cUserLvLimitMin = (iPermitLevel == 0) ? 1 : iPermitLevel ;
		CreatePartyPacket.nTargetMapIdx = 0;
		CreatePartyPacket.Difficulty = Dungeon::Difficulty::Easy;
		if( m_bVoiceAvailable )
			CreatePartyPacket.iBitFlag |= Party::BitFlag::VoiceChat;
		_wcscpy( CreatePartyPacket.wszPartyName, _countof(CreatePartyPacket.wszPartyName), wszInvitePartyName, static_cast<int>(wcslen(wszInvitePartyName)) );

		m_pDBCon->QueryAddParty( this, g_Config.nManagedID, Party::LocationType::Village, GetChannelID(), &CreatePartyPacket, pUser->GetAccountDBID() );
	}
#else
	if (g_pMasterConnection && g_pMasterConnection->GetActive())
	{
		g_pMasterConnection->SendReqPartyID(GetAccountDBID(), GetChannelID(), NORMPARTYCOUNTMAX, ITEMLOOTRULE_INORDER, ITEMRANK_B, 1, 100, 0, 0, 1, wszInvitePartyName, L"", pUser->GetAccountDBID(), m_bVoiceAvailable); // ITEMRANK modified by kalliste 2010.07.09
	}
#endif // #if defined( PRE_PARTY_DB )
	else
		return ERROR_PARTY_INVITEFAIL;

	return ERROR_NONE;
}
#endif

int CDNUserSession::_RecvPartyInviteDenied( const CSInviteDenied* pPacket, int iLen )
{
	if( sizeof(CSInviteDenied) - sizeof(pPacket->wszBuf) +(pPacket->cInviterNameLen * sizeof(WCHAR)) != iLen || pPacket->cInviterNameLen >= NAMELENMAX )
		return ERROR_INVALIDPACKET;

	// 상태 해제
	IsWindowStateNoneSet(WINDOW_ISACCEPT);

#if defined( PRE_PARTY_DB )
	CDNParty* pParty = g_pPartyManager->GetParty(pPacket->PartyIdx);
	if( pParty )
	{
		if( pParty->InviteDenied(GetCharacterName()) == true )
		{
			CDNUserSession* pUser = g_pUserSessionManager->FindUserSessionByAccountDBID(pParty->GetLeaderAccountDBID());
			if (pUser)
				pUser->SendPartyInviteDenied(GetCharacterName(), pPacket->cIsOpenBlind > 0 ? ERROR_PARTY_INVITEFAIL_DESTUSER_SITUATION_NOTALLOWED : ERROR_NONE);
			return ERROR_NONE;
		}
	}

	WCHAR wszReqCharName[NAMELENMAX];
	memset( wszReqCharName, 0, sizeof(wszReqCharName) );
	memcpy( wszReqCharName, pPacket->wszBuf, sizeof(WCHAR)*pPacket->cInviterNameLen );

	g_pMasterConnection->SendPartyInviteDenied( wszReqCharName, pPacket->PartyIdx, GetCharacterName(), pPacket->cIsOpenBlind );

#else
	CDNParty* pParty = g_pPartyManager->GetParty(pPacket->PartyIdx);
	if( pParty == NULL )	
		return ERROR_NONE;

	if( pParty->InviteDenied(GetCharacterName()) == true )
	{
		CDNUserSession* pUser = g_pUserSessionManager->FindUserSessionByAccountDBID(pParty->GetLeaderAccountDBID());
		if (pUser)
			pUser->SendPartyInviteDenied(GetCharacterName(), pPacket->cIsOpenBlind > 0 ? ERROR_PARTY_INVITEFAIL_DESTUSER_SITUATION_NOTALLOWED : ERROR_NONE);
	}
#endif // #if defined( PRE_PARTY_DB )

	return ERROR_NONE;
}

int CDNUserSession::_RecvPartyInviteOverServerResult( const CSPartyInviteOverServerResult* pPacket, int iLen )
{
	if( sizeof(CSPartyInviteOverServerResult) != iLen )
		return ERROR_INVALIDPACKET;

	IsWindowStateNoneSet(WINDOW_ISACCEPT);
	g_pMasterConnection->SendInvitePartyResult(pPacket->nGameServerID, pPacket->wszInviterName, GetCharacterName(), pPacket->nRetCode);
	return ERROR_NONE;
}

int	CDNUserSession::_RecvPartyMemberInfo( const CSPartyMemberInfo* pPacket, int iLen )
{
	if( sizeof(CSPartyMemberInfo) != iLen)
		return ERROR_INVALIDPACKET;

#if defined( PRE_PARTY_DB )
#if defined( PRE_WORLDCOMBINE_PARTY )
	CDNParty* pParty = g_pPartyManager->GetParty(pPacket->PartyID);
	if( pParty == NULL )
	{
		g_Log.Log(LogType::_PARTYERROR, this, L"[_RecvPartyMemberInfo]Not Exist pParty:%I64d\n", pPacket->PartyID);
		return ERROR_PARTY_NOTFOUND_MEMBERINFO;
	}
	if( Party::bIsWorldCombineParty( pParty->GetPartyType() ) )
	{		
		// 월드통합파티룸은 게임서버에서 직접 얻어오기
		g_pMasterConnection->SendGetWorldPartyMember( GetAccountDBID(), GetCharacterDBID(), pParty->GetPartyStructData() );
		return ERROR_NONE;
	}
#endif
	GetDBConnection()->QueryGetPartyJoinMembers( this, pPacket->PartyID );
#else
	CDNParty* pParty = g_pPartyManager->GetParty(pPacket->PartyID);
	if( pParty == NULL )
		return ERROR_PARTY_NOTFOUND_MEMBERINFO;

	pParty->SendPartyMemberInfo( pParty->bIsRaidParty(), this);
#endif // #if defined( PRE_PARTY_DB )
	return ERROR_NONE;
}

int CDNUserSession::_RecvPartyMemberKick( const CSPartyMemberKick* pPacket, int iLen )
{
	if( sizeof(CSPartyMemberKick) != iLen)
		return ERROR_INVALIDPACKET;

	if( m_eUserState != STATE_NONE)		//내가 이동중 또는 게이트등등의 상황에서는 킥이 불가능~
		return ERROR_PARTY_KICKFAIL;

	if( pPacket->nSessionID == GetSessionID() )
		return ERROR_PARTY_KICKFAIL_DEST_IS_YOU;

	CDNParty* pParty = g_pPartyManager->GetParty(GetPartyID());
	if( pParty == NULL )
		return ERROR_PARTY_KICKFAIL;

	if( pParty->bIsMove() || pParty->bIsStartGame())
		return ERROR_PARTY_KICKFAIL;

	CDNUserSession* pUser = g_pUserSessionManager->FindUserSession(pPacket->nSessionID);
	if (pUser == NULL)
		return ERROR_PARTY_KICKFAIL;

	if (pParty->GetLeaderAccountDBID() == GetAccountDBID() )
	{
#if defined( PRE_PARTY_DB )
		g_pPartyManager->QueryOutParty( GetPartyID(), pUser->GetAccountDBID(), pUser->GetCharacterDBID(), pUser->GetSessionID(), pPacket->cKickKind==0 ? Party::QueryOutPartyType::Kick : Party::QueryOutPartyType::EternityKick );
#else
		if( pParty->DelPartyMember(pUser, pPacket->cKickKind) )
		{
			pParty->SendKickPartyMember(pUser->GetSessionID());
			pParty->SendAllRefreshParty(PARTYREFRESH_NONE);
			g_pPartyManager->PushWaitPartyList(pParty);
		}
#endif // #if defined( PRE_PARTY_DB )
	}

	return ERROR_NONE;
}

int	CDNUserSession::_RecvPartyLeaderSwap( const CSPartyLeaderSwap* pPacket, int iLen )
{
	if( sizeof(CSPartyLeaderSwap) != iLen)
		return ERROR_INVALIDPACKET;

	CDNParty* pParty = g_pPartyManager->GetParty(GetPartyID());
	if( pParty == NULL )
		return ERROR_PARTY_LEADER_APPOINTMENTFAIL;

	if( pParty->GetLeaderAccountDBID() != GetAccountDBID() )
		return ERROR_PARTY_LEADER_APPOINTMENTFAIL;

	CDNUserSession* pUser = g_pUserSessionManager->FindUserSession(pPacket->nSessionID);
	if (pUser == NULL)
		return ERROR_PARTY_LEADER_APPOINTMENTFAIL;

	if( pParty->ChangePartyLeader(pUser->GetAccountDBID()) == false )
		return ERROR_PARTY_LEADER_APPOINTMENTFAIL;
	pParty->SendAllRefreshParty(PARTYREFRESH_CHANGE_LEADER);

	return ERROR_NONE;
}

int	CDNUserSession::_RecvPartyChannelList( int iLen )
{
	if( iLen != 0)
		return ERROR_INVALIDPACKET;

	if( m_pField == NULL )	
		return ERROR_GENERIC_UNKNOWNERROR;

	if( GetPartyID() > 0 )
	{
		CDNParty* pParty = g_pPartyManager->GetParty(GetPartyID());
		if( pParty == NULL )
			return ERROR_GENERIC_UNKNOWNERROR;

		if (pParty->GetLeaderAccountDBID() != GetAccountDBID())
			return ERROR_GENERIC_INVALIDREQUEST;
	}

	SCChannelList packet;
	memset(&packet, 0, sizeof(packet));

	int nMapIndex	= m_pField->GetMapIndex();
	int nChannelID	= m_pField->GetChnnelID();

	if( m_pField->bIsUseLastMapIndex() )
	{
		nMapIndex	= GetLastVillageMapIndex();
		nChannelID	= -1;
	}
	else if( m_pField->bIsPvPLobby() )
	{
		SendChannelInfo(&packet);
		return ERROR_NONE;
	}

	std::vector < std::pair < int, sChannelInfo > >::iterator ii;
	for(ii = g_pMasterConnection->m_vChannelList.begin(); ii != g_pMasterConnection->m_vChannelList.end(); ii++)
	{
		if ((*ii).second.bVisibility == false) 
			continue;

		if ((*ii).second.bShow == false )
			continue;

		// 제한레벨 체크
		if ((*ii).second.nLimitLevel > GetLevel() )
			continue;

#if defined(PRE_ADD_DWC)
		if(IsDWCCharacter())
		{	//DWC 채널만 보여준다.
			if (!((*ii).second.nChannelAttribute&GlobalEnum::CHANNEL_ATT_DWC) )
				continue;

			// 현재 위치하고 있는 PvP 채널은 보여주지 않는다.
			if ((*ii).second.nMapIdx == m_pField->GetMapIndex() && m_pField->GetChnnelID() == (*ii).second.nChannelID )
				continue;

			packet.Info[packet.cCount] = (*ii).second;
			++packet.cCount;
			continue;
		}
		else
		{
#endif
			// PvP 로비는 항상 보여주지 않는다.
			if ((*ii).second.nChannelAttribute&GlobalEnum::CHANNEL_ATT_PVPLOBBY )
				continue;

			// PvP 채널,다크레어 채널은 항상 보여준다.
			if ((*ii).second.nChannelAttribute&(GlobalEnum::CHANNEL_ATT_PVP|GlobalEnum::CHANNEL_ATT_DARKLAIR|GlobalEnum::CHANNEL_ATT_FARMTOWN) )
			{
				// 현재 위치하고 있는 PvP 채널은 보여주지 않는다.
				if ((*ii).second.nMapIdx == m_pField->GetMapIndex() && m_pField->GetChnnelID() == (*ii).second.nChannelID )
					continue;

				//농장 디펜던시체크
				if ((*ii).second.nChannelAttribute&(GlobalEnum::CHANNEL_ATT_FARMTOWN) && (*ii).second.nDependentMapID != nMapIndex)
					continue;

				packet.Info[packet.cCount] = (*ii).second;
				++packet.cCount;
				continue;
			}

#if defined(PRE_ADD_DWC)
		}
#endif
		if ((*ii).second.nMapIdx == nMapIndex && (*ii).second.nChannelID != nChannelID )
		{
			packet.Info[packet.cCount] = (*ii).second;
			++packet.cCount;
		}
	}

	SendChannelInfo(&packet);
	return ERROR_NONE;
}

int	CDNUserSession::_RecvPartySelectChannel( const CSVillageSelectChannel* pPacket, int iLen )
{
	if( sizeof(CSVillageSelectChannel) != iLen)
		return ERROR_INVALIDPACKET;

	return CmdSelectChannel( pPacket->nChannelID );
}

int CDNUserSession::_RecvPartyInfoModify( const CSPartyInfoModify* pPacket, int iLen )
{
	if( sizeof(CSPartyInfoModify) != iLen )
		return ERROR_INVALIDPACKET;

	if( GetPartyID() <= 0 )
		return ERROR_GENERIC_INVALIDREQUEST;

	CDNParty* pParty = g_pPartyManager->GetParty(GetPartyID());
	if( pParty == NULL || pParty->GetLeaderAccountDBID() != GetAccountDBID() )
		return ERROR_GENERIC_INVALIDREQUEST;

	if( wcslen(pPacket->wszName) <= 0 )
		return ERROR_PARTY_MODIFYINFO_FAIL_NEED_NAME;

#if defined( PRE_PARTY_DB )
#else
	if( wcslen(pPacket->wszPass) > PARTYPASSWORDMAX-1 )
		return ERROR_PARTY_MODIFYINFO_FAIL_PASSWORD_ERR;
#endif // #if defined( PRE_PARTY_DB )

	if( wcslen(pPacket->wszName) >= PARTYNAMELENMAX || pPacket->cPartyMemberMax > PARTYMAX || pPacket->cPartyMemberMax < pParty->GetMemberCount())
		return ERROR_PARTY_MODIFYINFO_FAIL;

	if( pPacket->LootRule < ITEMLOOTRULE_NONE || pPacket->LootRule >= ITEMLOOTRULE_MAX)
		return ERROR_PARTY_MODIFYINFO_FAIL;

	if( pPacket->LootRank < ITEMRANK_D || pPacket->LootRank > ITEMRANK_NONE)
		return ERROR_PARTY_MODIFYINFO_FAIL;

#if defined( PRE_PARTY_DB )
	int iPermitLevel = g_pDataManager->GetMapPermitLevel( GetMapIndex() );
	if( pPacket->cUserLvLimitMin > GetLevel() || pPacket->cUserLvLimitMin <= 0 || pPacket->cUserLvLimitMin < iPermitLevel )
		return ERROR_PARTY_MODIFYINFO_FAIL_RESTRICT_LEVEL_ERROR;
#else
	if (pPacket->cUserLvLimitMin > pPacket->cUserLvLimitMax || pPacket->cUserLvLimitMin > GetLevel() || pPacket->cUserLvLimitMin <= 0 || pPacket->cUserLvLimitMax > 100)
		return ERROR_PARTY_MODIFYINFO_FAIL_RESTRICT_LEVEL_ERROR;
#endif // #if defined( PRE_PARTY_DB )

#if defined( PRE_PARTY_DB )
	int nPass = pPacket->iPassword;
#else
	int nPass = _wtoi(pPacket->wszPass	);
#endif // #if defined( PRE_PARTY_DB )
	if (nPass > MAXPARTYPASSWORD)
		return ERROR_PARTY_MODIFYINFO_FAIL_PASSWORD_ERR;	

#if defined( PRE_PARTY_DB )
	if ( m_pDBCon && m_pDBCon->GetActive() )
		m_pDBCon->QueryModParty( this, pParty, g_Config.nManagedID, Party::LocationType::Village, GetChannelID(), pPacket );
#else  // #if defined( PRE_PARTY_DB )
	int nWaitRet;
#if defined( PRE_PARTY_DB )
	if( pParty->ModifyPartyInfo(GetChannelID(), pPacket->wszName, pPacket->iPassword, pPacket->cPartyMemberMax, pPacket->LootRule, pPacket->LootRank, pPacket->cUserLvLimitMin, pPacket->nTargetMapIdx, pPacket->Difficulty, pPacket->iBitFlag, nWaitRet))
#else
	if( pParty->ModifyPartyInfo(GetChannelID(), pPacket->wszName, pPacket->wszPass, pPacket->cPartyMemberMax, pPacket->LootRule, pPacket->LootRank, pPacket->cUserLvLimitMin, pPacket->cUserLvLimitMax, pPacket->nTargetMapIdx, pPacket->Difficulty, pPacket->cIsJobDice, nWaitRet))
#endif // #if defined( PRE_PARTY_DB )
	{
#if defined( PRE_PARTY_DB )
#else
		//리스트 넣고 빼고
		if( nWaitRet == _PARTY_MOD_RET_ADD)
			g_pPartyManager->PushWaitPartyList(pParty);
		else if (nWaitRet == _PARTY_MOD_RET_DEL)
			g_pPartyManager->EraseWaitPartyList(pParty);
#endif // #if defined( PRE_PARTY_DB )
listinfo:
		pParty->SendAllRefreshParty(PARTYREFRESH_MODIFY_INFO);
	}
#endif  // #if defined( PRE_PARTY_DB )

	return ERROR_NONE;
}

int	CDNUserSession::_RecvPartyReadyQuest( int iLen )
{
	if( iLen != 0)
		return ERROR_INVALIDPACKET;

	if( GetPartyID() <= 0 )
		return ERROR_GENERIC_INVALIDREQUEST;

	g_pPartyManager->SendPartyReadyRequest(GetPartyID(), GetAccountDBID());
	return ERROR_NONE;
}

int CDNUserSession::_RecvPartyAskJoin( const CSPartyAskJoin* pPacket, int iLen )
{
	if( iLen != sizeof(CSPartyAskJoin) )
		return ERROR_INVALIDPACKET;

	if( GetPartyID() > 0 )
		return ERROR_PARTY_ASKJOIN_FAIL;

	CDNUserSession* pSession = g_pUserSessionManager->FindUserSessionByName(pPacket->wszPartyLeaderName);	
#if defined( PRE_PARTY_DB ) && defined(PRE_MOD_REQ_JOIN_PARTY_ANSWER_MSG_APP)
	//마스터 서버에 AskJoin 요청
	if( pSession == NULL )
	{
		g_pMasterConnection->SendReqPartyAskJoin( this, pPacket->wszPartyLeaderName );
		return ERROR_NONE;
	}
#else
	if( pSession == NULL )
		return ERROR_PARTY_ASKJOIN_FAIL;

	if( pSession->GetMapIndex() != GetMapIndex() )
		return ERROR_PARTY_ASKJOIN_DEST_DIFFMAP;
#endif // #if defined( PRE_PARTY_DB )

	if( pSession->GetPartyID() <= 0 )
		return ERROR_PARTY_ASKJOIN_PARTY_NOTFOUND;

	CDNParty* pParty = g_pPartyManager->GetParty(pSession->GetPartyID());
	if( pParty == NULL )
		return ERROR_PARTY_ASKJOIN_PARTY_NOTFOUND;

	CDNUserSession* pLeader = g_pUserSessionManager->FindUserSessionByAccountDBID(pParty->GetLeaderAccountDBID());
	if( pLeader == NULL )
		return ERROR_PARTY_ASKJOIN_FAIL;

	if( pLeader->GetIsolate()->IsIsolateItem(GetCharacterName()) )
		return ERROR_PARTY_ASKJOIN_FAIL;

	if( pLeader->IsAcceptAbleOption(GetCharacterDBID(), GetAccountDBID(), _ACCEPTABLE_CHECKTYPE_PARTYINVITE) == false)
		return ERROR_PARTY_ASKJOIN_FAIL;

	if( !pLeader->IsNoneWindowState() )
		return ERROR_PARTY_ASKJOIN_DESTUSER_SITUATION_NOTALLOWED;

	pLeader->SetWindowState(WINDOW_ISACCEPT);
	pLeader->SendAskJoinToLeader(GetCharacterName(), GetUserJob(), GetLevel());
	return ERROR_NONE;
}

int	CDNUserSession::_RecvPartyAskJoinDecision( const CSPartyAskJoinDecision* pPacket, int iLen )
{
	if( sizeof(CSPartyAskJoinDecision) != iLen)
		return ERROR_INVALIDPACKET;

	// 상태부터 풀어준다.
	IsWindowStateNoneSet(WINDOW_ISACCEPT);

	CDNParty* pParty = g_pPartyManager->GetParty(GetPartyID());
	if (pParty == NULL)
		return ERROR_PARTY_ASKJOIN_RESULT_AGREE_FAIL;
	
	CDNUserSession* pDestSession = g_pUserSessionManager->FindUserSessionByName(pPacket->wszPartyAsker);
#if defined( PRE_PARTY_DB ) && defined(PRE_MOD_REQ_JOIN_PARTY_ANSWER_MSG_APP)
	//마스터 서버에 AskJoin 요청
	if( pDestSession == NULL )
	{
		if(pPacket->cRet == 0)
			g_pMasterConnection->SendResPartyAskJoin( ERROR_PARTY_ASKJOIN_RESULT_PARTYLEADER_DENIED, 0, pPacket->wszPartyAsker );
		else
			g_pMasterConnection->SendAskJoinAgreeInfo( pPacket->wszPartyAsker, pParty->GetPartyID(), pParty->GetPassword() );
		return ERROR_NONE;
	}
#else
	if (pDestSession == NULL)
		return ERROR_PARTY_ASKJOIN_RESULT_AGREE_FAIL;
#endif

	if( pPacket->cRet == 0 )
	{
		pDestSession->SendAskJoinResult(ERROR_PARTY_ASKJOIN_RESULT_PARTYLEADER_DENIED);
		return ERROR_NONE;
	}
	
#if defined( PRE_PARTY_DB )
	pDestSession->SendAskJoinAgreeInfo(pParty->GetPartyID(), pParty->GetPassword() );
#else
	pDestSession->SendAskJoinAgreeInfo(pParty->GetPartyID(), pParty->m_wszPartyPassword);
#endif // #if defined( PRE_PARTY_DB )

	return ERROR_NONE;
}

int	CDNUserSession::_RecvPartySwapMemberIndex( const CSPartySwapMemberIndex* pPacket, int iLen )
{
	if( sizeof(CSPartySwapMemberIndex) != iLen )
		return ERROR_INVALIDPACKET;

	if( GetPartyID() <= 0 )
		return ERROR_PARTY_SWAPINDEX_FAIL;

	CDNParty* pParty = g_pPartyManager->GetParty(GetPartyID());
	if( pParty == NULL )
		return ERROR_PARTY_SWAPINDEX_FAIL;

	if( pParty->GetLeaderAccountDBID() != GetAccountDBID() )
		return ERROR_PARTY_SWAPINDEX_FAIL;

	if( pParty->SwapMemberIndex(pPacket) == false )
		return ERROR_PARTY_SWAPINDEX_FAIL;

	return ERROR_NONE;
}

int	CDNUserSession::_RecvPartyConfirmLastDungeonInfo( const CSPartyConfirmLastDungeonInfo* pPacket, int iLen )
{
	if( sizeof(CSPartyConfirmLastDungeonInfo) != iLen )
		return ERROR_INVALIDPACKET;

	if( bIsCheckLastDungeonInfo() == false )
		return ERROR_NONE;

#if defined( _WORK )
	std::cout << "CS_CONFIRM_LASTDUNGEONINFO=" <<(pPacket->bIsOK ? "true" : "false") << std::endl;
#endif // #if defined( _WORK )

	if ( pPacket->bIsOK == true )
	{
		INT64 biValue = 0;
		if ( GetCommonVariableDataValue( CommonVariable::Type::LastPartyDungeonInfo, biValue ) == false || biValue == 0 )
			return ERROR_NONE;

		g_pMasterConnection->SendConfirmLastDungeonInfo( this, biValue );
	}
	else
	{
		INT64 biValue = 0;
		if ( GetCommonVariableDataValue( CommonVariable::Type::LastPartyDungeonInfo, biValue ) == true || biValue > 0 )
#if defined( PRE_PARTY_DB )
			g_pMasterConnection->SendConfirmLastDungeonInfo( this, biValue, BreakInto::Type::PartyRestore, false );
#else
			g_pMasterConnection->SendConfirmLastDungeonInfo( this, biValue, false );
#endif // #if defined( PRE_PARTY_DB )

		ModCommonVariableData( CommonVariable::Type::LastPartyDungeonInfo, 0 );
	}

	return ERROR_NONE;
}
