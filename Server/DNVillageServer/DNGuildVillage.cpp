#include "stdafx.h"
#include "DNGuildVillage.h"
#include "DNMasterConnection.h"
#include "DNGameDataManager.h"
#include "DNSchedule.h"
#include "DNGuildWarManager.h"

CDNGuildVillage::CDNGuildVillage() 
{
	m_GuildWare = NULL;
	m_nResetWareHour = 4;
	GuildWarReset();	
};

CDNGuildVillage::~CDNGuildVillage() 
{

};

void CDNGuildVillage::Reset()
{
	if (m_GuildWare)
		m_GuildWare->ReportWareSlot();

	CDNGuildBase::Reset ();
	GuildWarReset();

	SAFE_DELETE(m_GuildWare);	
}

void CDNGuildVillage::OpenWare(const TAGetGuildWareInfo* pPacket)
{
	if (m_GuildWare == NULL)
		m_GuildWare = new CDNGuildWare();

	if (m_GuildWare)
		m_GuildWare->Open(this, pPacket);
	else
		_DANGER_POINT ();
}

// 길드원인지 확인
bool CDNGuildVillage::IsValiadMember(CDNUserSession *pUserObj, INT64 nCharacterDBID)
{
	if (!pUserObj || !pUserObj->GetGuildUID().IsSet() || GetUID() != pUserObj->GetGuildUID() || (nCharacterDBID != pUserObj->GetCharacterDBID()))
		return false;

	return true;
}

// 로그인한 길드원에 대해 전체발송
void CDNGuildVillage::SendGuildMemberLoginList(SCGuildMemberLoginList* pPacket)
{
	std::vector <std::pair<UINT, INT64>, boost::pool_allocator<UINT>> List;
	GetMemberList (List);

	if (List.empty ())
		return;

	std::vector <std::pair<UINT, INT64>, boost::pool_allocator<UINT>>::iterator iter = List.begin ();

	for (; iter != List.end (); iter++)
	{
		CDNUserSession *pUserObj = g_pUserSessionManager->FindUserSessionByAccountDBID (iter->first);
		if (false == IsValiadMember (pUserObj, iter->second))
			continue;

		pUserObj->SendGuildMemberLoginList(pPacket);
	}
}

// 길드자원을 확인한다.
bool CDNGuildVillage::CheckGuildResource(TGuildMember* pGuildMemeber)
{
	if (false == CDNGuildBase::CheckGuildResource(pGuildMemeber))
		return false;

	CDNUserSession *pUserObj = g_pUserSessionManager->FindUserSessionByAccountDBID (pGuildMemeber->nAccountDBID);

	// 유저상태 확인
	if (pUserObj && pUserObj->GetGuildUID().IsSet())
	{
		// 길드정보 확인
		// 사용자 세션포인터를 얻어도 동기화 보장되지 않으므로 확인용도로만 사용할것!
		if (GetUID() == pUserObj->GetGuildUID() && (pGuildMemeber->nCharacterDBID == pUserObj->GetCharacterDBID()))
			return false;
	}

	// 멤버리스트를 계속 조회한다
	return true;
}

void CDNGuildVillage::SendAddGuildMember(UINT nAccountDBID, INT64 nCharacterDBID, LPCWSTR lpwszCharacterName, TP_JOB nJob, char cLevel, __time64_t pJoinDate, TCommunityLocation* pLocation)
{
	DN_ASSERT(IsOpen(),		"Not Opened!");
	DN_ASSERT(IsAttach(),	"Not Attached!");

	std::vector <std::pair<UINT, INT64>, boost::pool_allocator<UINT>> List;
	GetMemberList (List);

	if (List.empty ())
		return;

	std::vector <std::pair<UINT, INT64>, boost::pool_allocator<UINT>>::iterator iter = List.begin ();

	for (; iter != List.end (); iter++)
	{
		CDNUserSession *pUserObj = g_pUserSessionManager->FindUserSessionByAccountDBID(iter->first);
		if (false == IsValiadMember (pUserObj, iter->second))
			continue;

		pUserObj->SendInviteGuildMemberAck (nAccountDBID, nCharacterDBID, lpwszCharacterName, ERROR_NONE, 0, 0, nJob, cLevel, pLocation, &GetUID());
	}
}

void CDNGuildVillage::SendDelGuildMember(UINT nAccountDBID, INT64 nCharacterDBID, bool bIsExiled)
{
	DN_ASSERT(IsOpen(),		"Not Opened!");
	DN_ASSERT(IsAttach(),	"Not Attached!");
	DN_ASSERT(!IsLock(),	"Already Locked!");

	std::vector <std::pair<UINT, INT64>, boost::pool_allocator<UINT>> List;
	GetMemberList (List);

	if (List.empty ())
		return;

	std::vector <std::pair<UINT, INT64>, boost::pool_allocator<UINT>>::iterator iter = List.begin ();

	for (; iter != List.end (); iter++)
	{
		CDNUserSession *pUserObj = g_pUserSessionManager->FindUserSessionByAccountDBID(iter->first);
		// 우승 길드인지 확인
		if( pUserObj && g_pGuildWarManager && g_pGuildWarManager->GetPreWinGuildUID().IsSet() 
			&& GetUID() == g_pGuildWarManager->GetPreWinGuildUID() && nAccountDBID == pUserObj->GetAccountDBID() )
			pUserObj->SendGuildWarPreWinGuild(false);
		
		if (false == IsValiadMember(pUserObj, iter->second))
			continue;

		if( nCharacterDBID == pUserObj->GetCharacterDBID() )
		{

			pUserObj->GetItem()->RemoveGuildReversionItem( true );
			pUserObj->GetItem()->RemoveGuildReversionVehicleItemData( true );
			pUserObj->SendGuildRewardItem(NULL);
		}
		if (bIsExiled)
			pUserObj->SendExileGuildMember(nAccountDBID, nCharacterDBID, ERROR_NONE, &GetUID());
		else
			pUserObj->SendLeaveGuildMember(nAccountDBID, nCharacterDBID, ERROR_NONE, &GetUID(), false);
	}
}

void CDNGuildVillage::SendChangeGuildInfo(UINT nAccountDBID, INT64 nCharacterDBID, BYTE btGuildUpdate, int iInt1, int iInt2, int iInt3, int iInt4, INT64 biInt64, LPCWSTR pText)
{
	DN_ASSERT(IsOpen(),		"Not Opened!");
	DN_ASSERT(IsAttach(),	"Not Attached!");
	DN_ASSERT(!IsLock(),	"Already Locked!");

	std::vector <std::pair<UINT, INT64>, boost::pool_allocator<UINT>> List;
	GetMemberList (List);

	if (List.empty ())
		return;

	std::vector <std::pair<UINT, INT64>, boost::pool_allocator<UINT>>::iterator iter = List.begin ();

	for (; iter != List.end (); iter++)
	{
		CDNUserSession *pUserObj = g_pUserSessionManager->FindUserSessionByAccountDBID(iter->first);
		if (false == IsValiadMember(pUserObj, iter->second))
			continue;

		pUserObj->SendChangeGuildInfoEx(nAccountDBID, nCharacterDBID, btGuildUpdate, ERROR_NONE, iInt1, iInt2, iInt3, iInt4, biInt64, pText, &GetUID());
	}
}

void CDNGuildVillage::SendChangeGuildMemberInfo(UINT nReqAccountDBID, INT64 nReqCharacterDBID, UINT nChgAccountDBID, INT64 nChgCharacterDBID, BYTE btGuildMemberUpdate, int iInt1, int iInt2, INT64 biInt64, LPCWSTR pText)
{
	DN_ASSERT(IsOpen(),		"Not Opened!");
	DN_ASSERT(IsAttach(),	"Not Attached!");
	DN_ASSERT(!IsLock(),	"Already Locked!");

	std::vector <std::pair<UINT, INT64>, boost::pool_allocator<UINT>> List;
	GetMemberList (List);

	if (List.empty ())
		return;

	std::vector <std::pair<UINT, INT64>, boost::pool_allocator<UINT>>::iterator iter = List.begin ();

	for (; iter != List.end (); iter++)
	{
		CDNUserSession *pUserObj = g_pUserSessionManager->FindUserSessionByAccountDBID (iter->first);
		if (false == IsValiadMember(pUserObj, iter->second))
			continue;

		pUserObj->SendChangeGuildMemberInfo (nReqAccountDBID, nReqCharacterDBID, nChgAccountDBID, nChgCharacterDBID, btGuildMemberUpdate, ERROR_NONE, iInt1, iInt2, biInt64, pText, &GetUID());

		// [주의] 캐릭터ID기준으러 처리해야 함.
		if (nChgCharacterDBID == pUserObj->GetCharacterDBID())
		{
			switch(btGuildMemberUpdate) 
			{
			case GUILDMEMBUPDATE_TYPE_ROLE: // 직급 변경
				{
					pUserObj->GetGuildSelfView().btGuildRole = static_cast<BYTE>(iInt1);

					// 길드 시각정보 갱신
					pUserObj->RefreshGuildSelfView();
				}
				break;
			
			case GUILDMEMBUPDATE_TYPE_GUILDMASTER: // 길드장 위임
				{
					pUserObj->GetGuildSelfView().btGuildRole = static_cast<BYTE>(GUILDROLE_TYPE_MASTER);

					// 길드 시각정보 갱신
					pUserObj->RefreshGuildSelfView();
				}
				break;
			default:
				break;
			}
		}
	}
}

void CDNGuildVillage::SendGuildChat(UINT nAccountDBID, INT64 nCharacterDBID, LPCWSTR lpwszChatMsg, short nLen)
{
	DN_ASSERT(IsOpen(),		"Not Opened!");
	DN_ASSERT(IsAttach(),	"Not Attached!");
	DN_ASSERT(!IsLock(),	"Already Locked!");

	std::vector <std::pair<UINT, INT64>, boost::pool_allocator<UINT>> List;
	GetMemberList(List);

	if (List.empty ())
		return;

	std::vector <std::pair<UINT, INT64>, boost::pool_allocator<UINT>>::iterator iter = List.begin ();

	for (; iter != List.end (); iter++)
	{
		CDNUserSession *pUserObj = g_pUserSessionManager->FindUserSessionByAccountDBID(iter->first);
		if (false == IsValiadMember (pUserObj, iter->second))
			continue;

		pUserObj->SendGuildChat(GetUID(), nAccountDBID, nCharacterDBID, lpwszChatMsg, nLen);
	}
}

#ifdef PRE_ADD_DOORS_GUILDCHAT_DISCONNECT
void CDNGuildVillage::SendDoorsGuildChat(INT64 nCharacterDBID, LPCWSTR lpwszChatMsg, short nLen)
{
	DN_ASSERT(IsOpen(),		"Not Opened!");
	DN_ASSERT(IsAttach(),	"Not Attached!");
	DN_ASSERT(!IsLock(),	"Already Locked!");

	std::vector <std::pair<UINT, INT64>, boost::pool_allocator<UINT>> List;
	GetMemberList(List);

	if (List.empty())
		return;

	std::vector <std::pair<UINT, INT64>, boost::pool_allocator<UINT>>::iterator iter = List.begin ();
	for (; iter != List.end (); iter++)
	{
		CDNUserSession *pUserObj = g_pUserSessionManager->FindUserSessionByAccountDBID(iter->first);
		if (false == IsValiadMember(pUserObj, iter->second))
			continue;

		pUserObj->SendDoorsGuildChat(GetUID(), nCharacterDBID, lpwszChatMsg, nLen);
	}
}
#endif		//#ifdef PRE_ADD_DOORS_GUILDCHAT_DISCONNECT

void CDNGuildVillage::SendChangeGuildName(const wchar_t* wszGuildName)
{
	DN_ASSERT(IsOpen(),		"Not Opened!");
	DN_ASSERT(IsAttach(),	"Not Attached!");
	DN_ASSERT(!IsLock(),	"Already Locked!");

	std::vector <std::pair<UINT, INT64>, boost::pool_allocator<UINT>> List;
	GetMemberList (List);

	if (List.empty ())
		return;

	std::vector <std::pair<UINT, INT64>, boost::pool_allocator<UINT>>::iterator iter = List.begin ();

	for (; iter != List.end (); iter++)
	{
		CDNUserSession *pUserObj = g_pUserSessionManager->FindUserSessionByAccountDBID(iter->first);
		if (false == IsValiadMember (pUserObj, iter->second))
			continue;

		pUserObj->SendChangeGuildName(wszGuildName);
		_wcscpy( pUserObj->GetGuildSelfView().wszGuildName, _countof(pUserObj->GetGuildSelfView().wszGuildName), wszGuildName, (int)wcslen(wszGuildName) );
		pUserObj->RefreshGuildSelfView();
	}
}

void CDNGuildVillage::SendChangeGuildMark(const MAGuildChangeMark *pPacket)
{
	DN_ASSERT(IsOpen(),		"Not Opened!");
	DN_ASSERT(IsAttach(),	"Not Attached!");
	DN_ASSERT(!IsLock(),	"Already Locked!");

	std::vector <std::pair<UINT, INT64>, boost::pool_allocator<UINT>> List;
	GetMemberList (List);

	if (List.empty ())
		return;

	std::vector <std::pair<UINT, INT64>, boost::pool_allocator<UINT>>::iterator iter = List.begin ();

	for (; iter != List.end (); iter++)
	{
		CDNUserSession *pUserObj = g_pUserSessionManager->FindUserSessionByAccountDBID(iter->first);
		if (false == IsValiadMember (pUserObj, iter->second))
			continue;

		pUserObj->SendChangeGuildMark(pPacket);
		pUserObj->GetGuildSelfView().wGuildMark = pPacket->wMark;
		pUserObj->GetGuildSelfView().wGuildMarkBG = pPacket->wMarkBG;
		pUserObj->GetGuildSelfView().wGuildMarkBorder = pPacket->wMarkBorder;
		pUserObj->RefreshGuildSelfView();
	}
}

void CDNGuildVillage::SendExtendGuildWare(short wTotalSize)
{
	std::vector <std::pair<UINT, INT64>, boost::pool_allocator<UINT>> List;
	GetMemberList (List);

	if (List.empty ())
		return;

	std::vector <std::pair<UINT, INT64>, boost::pool_allocator<UINT>>::iterator iter = List.begin ();

	for (; iter != List.end (); iter++)
	{
		CDNUserSession *pUserObj = g_pUserSessionManager->FindUserSessionByAccountDBID(iter->first);
		if (false == IsValiadMember(pUserObj, iter->second))
			continue;

		pUserObj->SendExtendGuildWare(wTotalSize);
	}
}

void CDNGuildVillage::SendGuildLevelUp (int nLevel)
{
	std::vector <std::pair<UINT, INT64>, boost::pool_allocator<UINT>> List;
	GetMemberList (List);

	if (List.empty ())
		return;

	std::vector <std::pair<UINT, INT64>, boost::pool_allocator<UINT>>::iterator iter = List.begin ();

	for (; iter != List.end (); iter++)
	{
		CDNUserSession *pUserObj = g_pUserSessionManager->FindUserSessionByAccountDBID(iter->first);
		if (false == IsValiadMember (pUserObj, iter->second))
			continue;

		pUserObj->SendGuildLevelUp(nLevel);
	}
}

void CDNGuildVillage::SendUpdateGuildExp(BYTE cPointType, int nPointValue, int nGuildExp, INT64 biCharacterDBID, int nMissionID)
{
	std::vector <std::pair<UINT, INT64>, boost::pool_allocator<UINT>> List;
	GetMemberList (List);

	if (List.empty ())
		return;

	std::vector <std::pair<UINT, INT64>, boost::pool_allocator<UINT>>::iterator iter = List.begin ();

	for (; iter != List.end (); iter++)
	{
		CDNUserSession *pUserObj = g_pUserSessionManager->FindUserSessionByAccountDBID(iter->first);
		if (false == IsValiadMember (pUserObj, iter->second))
			continue;

		if (cPointType == GUILDPOINTTYPE_STAGE)
		{
			if (pUserObj->GetCharacterDBID() == biCharacterDBID)
			{
				pUserObj->SendUpdateGuildExp(ERROR_NONE, cPointType, nPointValue, nGuildExp, biCharacterDBID, nMissionID);
				break;
			}
		}
		else
			pUserObj->SendUpdateGuildExp(ERROR_NONE, cPointType, nPointValue, nGuildExp, biCharacterDBID, nMissionID);
	}
}

#ifdef PRE_ADD_JOINGUILD_SUPPORT
void CDNGuildVillage::SendRefreshGuildItem(char cType, INT64 biSrcSerial, INT64 biDestSerial, const TItemInfo *pSrcInfo, const TItemInfo *pDestInfo, CDNUserSession* pSession, bool bNoUpdate)
#else		//#ifdef PRE_ADD_JOINGUILD_SUPPORT
void CDNGuildVillage::SendRefreshGuildItem(char cType, INT64 biSrcSerial, INT64 biDestSerial, const TItemInfo *pSrcInfo, const TItemInfo *pDestInfo, CDNUserSession* pSession)
#endif		//#ifdef PRE_ADD_JOINGUILD_SUPPORT
{
	// 멤버가없으면리턴
	if (m_MapTotalMember.empty())
		return;

	// 길드가 없으면 리턴
	if (!m_GuildWare)
		return;

	ScopeLock <CSyncLock> AutoLock (GetLock());

	std::map<INT64, TGuildMember>::iterator iter = m_MapTotalMember.begin ();

	for (; iter != m_MapTotalMember.end(); iter++)
	{
		TGuildMember* pGuildMember = &(iter->second);
	
		CDNUserSession *pUserObj = g_pUserSessionManager->FindUserSessionByAccountDBID(pGuildMember->nAccountDBID);
		if (false == IsValiadMember (pUserObj, pGuildMember->nCharacterDBID))
			continue;

		if (pUserObj == pSession)
		{
			// 요청자인 자기자신에게는 버젼업데이트 한다.
#ifdef PRE_ADD_JOINGUILD_SUPPORT
			if (bNoUpdate == true)
				continue;

			pGuildMember->WareVeriosnDate = m_GuildWare->GetVersionDate();
#else		//#ifdef PRE_ADD_JOINGUILD_SUPPORT
			pGuildMember->WareVeriosnDate = m_GuildWare->GetVersionDate();
#endif		//#ifdef PRE_ADD_JOINGUILD_SUPPORT
			continue;
		}

		// 길드창을 열지 않은 유저는 버젼업데이트를 하지 않는다.
		if (pGuildMember->bOpenWareStatus == false)
			continue;

		pGuildMember->WareVeriosnDate = m_GuildWare->GetVersionDate();

		pUserObj->SendRefreshGuildItem(cType, biSrcSerial, biDestSerial, pSrcInfo, pDestInfo);
	}
}

void CDNGuildVillage::SendRefreshGuildCoin(INT64 biTotalCoin, CDNUserSession* pSession)
{
	DN_ASSERT(IsOpen(),		"Not Opened!");
	DN_ASSERT(IsAttach(),	"Not Attached!");

	std::vector <std::pair<UINT, INT64>, boost::pool_allocator<UINT>> List;
	GetMemberList (List);

	if (List.empty ())
		return;

	std::vector <std::pair<UINT, INT64>, boost::pool_allocator<UINT>>::iterator iter = List.begin ();

	for (; iter != List.end (); iter++)
	{
		CDNUserSession *pUserObj = g_pUserSessionManager->FindUserSessionByAccountDBID(iter->first);
		if (false == IsValiadMember(pUserObj, iter->second))
			continue;

		if (pUserObj == pSession)
			continue;

		pUserObj->SendRefreshGuildCoin(biTotalCoin);
	}
}

void CDNGuildVillage::UpdateGuildInfo(CDNUserSession* pSession, const TAChangeGuildInfo *pPacket)
{
	switch(pPacket->btGuildUpdate) 
	{
	case GUILDUPDATE_TYPE_NOTICE:			// 공지변경
		{
			{
				ScopeLock <CSyncLock> AutoLock (GetLock());

				_wcscpy(SetInfo()->wszGuildNotice, _countof(GetInfo()->wszGuildNotice), pPacket->Text, (int)wcslen(pPacket->Text));
			}
		}
		break;

	case GUILDUPDATE_TYPE_ROLEAUTH:			// 직급권한변경
		{
			SetInfo()->wGuildRoleAuth[pPacket->nInt1] = static_cast<int>(pPacket->nInt2);
			SetInfo()->nRoleMaxItem[pPacket->nInt1] = static_cast<int>(pPacket->nInt3);
			SetInfo()->nRoleMaxCoin[pPacket->nInt1] = static_cast<int>(pPacket->nInt4);
		}
		break;
#if defined( PRE_ADD_GUILD_EASYSYSTEM )
	case GUILDUPDATE_TYPE_HOMEPAGE:
		{
			{
				ScopeLock <CSyncLock> AutoLock (GetLock());
				_wcscpy(SetInfo()->wszGuildHomePage, _countof(GetInfo()->wszGuildHomePage), pPacket->Text, (int)wcslen(pPacket->Text));
			}			
		}
		break;
#endif
		

	
	default:
		{
			pSession->SendChangeGuildInfo(pPacket->nAccountDBID, pPacket->nCharacterDBID, pPacket->btGuildUpdate, ERROR_GENERIC_INVALIDREQUEST, pPacket->nInt1, pPacket->nInt2, pPacket->biInt64, pPacket->Text, NULL);
			return;
		}
		break;
	}

	// 현재서버에 접속중인 길드원들에게 통보
	SendChangeGuildInfo(pPacket->nAccountDBID, pPacket->nCharacterDBID, pPacket->btGuildUpdate, pPacket->nInt1, pPacket->nInt2, pPacket->nInt3, pPacket->nInt4, pPacket->biInt64, pPacket->Text);

	// MA 를통하여 다른서버의 길드일반정보도 갱신하도록 지정
	if (g_pMasterConnection)
		g_pMasterConnection->SendChangeGuildInfo(GetUID(), pPacket->nAccountDBID, pPacket->nCharacterDBID, pPacket->btGuildUpdate, 
													pPacket->nInt1, pPacket->nInt2, pPacket->nInt3, pPacket->nInt4, pPacket->biInt64, pPacket->Text);
}

CDNGuildWare* CDNGuildVillage::GetGuildWare()
{
	return m_GuildWare;
}

void CDNGuildVillage::RecvCsCloseGuildWare(CDNUserSession * pSession)
{
	// 창고를 처음 연 사람인지 확인한다.
	TGuildMember* pGuildMember = GetMemberInfo(pSession->GetCharacterDBID());
	if (pGuildMember)
	{
		pGuildMember->bOpenWareStatus = false;
	}
}

void CDNGuildVillage::RecvApiOpenGuildWare(CDNUserSession * pSession)
{
	if (!m_GuildWare)
	{
		// 길드창고 정보가 구성되어 있지 않으면 DB에 재요청한다.
		ReqGetGuildWareInfo();
		g_Log.Log(LogType::_ERROR, pSession, L"ReqGetGuildWareInfo[CDBID:%I64d][GuildDBID:%d][WorldSetID:%d]\r\n", pSession->GetCharacterDBID(), GetUID().nDBID, pSession->GetWorldSetID());
		return;
	}

	TGuildMember* pGuildMember = GetMemberInfo(pSession->GetCharacterDBID());
	if (!pGuildMember)
		return;

	// 창고이용 횟수 리셋을 검사한다.
	CheckResetWareLimit (pGuildMember);

	// 창고를 처음 연 사람인지 확인한다.
	if (pGuildMember->WareVeriosnDate != m_GuildWare->GetVersionDate())
		SendGuildWareList (pSession, 0);
	else 
		SendGuildWareList (pSession, 1);


	pGuildMember->WareVeriosnDate = m_GuildWare->GetVersionDate();
	pGuildMember->bOpenWareStatus = true;
}

void CDNGuildVillage::CheckResetWareLimit(TGuildMember* pGuildMember)
{
	if (!pGuildMember)
		return;

	if (pGuildMember->LastWareUseDate < 0)
		return;

	tm tLastTime = *localtime(&pGuildMember->LastWareUseDate);

	time_t tCurTime;
	time(&tCurTime);

	tm tCurWareTime = *localtime(&tCurTime);


	tm tResetTime;
	tResetTime = tCurWareTime;
	tResetTime.tm_hour = m_nResetWareHour;
	tResetTime.tm_min = 0;
	tResetTime.tm_sec = 0;

	if (tCurWareTime.tm_hour < m_nResetWareHour)
		tResetTime.tm_yday--;

	if (tLastTime.tm_yday < tResetTime.tm_yday)
	{
		pGuildMember->TakeWareItemCount = 0;
		pGuildMember->WithdrawCoin = 0;	
	}
	else if (tLastTime.tm_yday == tResetTime.tm_yday && tLastTime.tm_hour < tResetTime.tm_hour)
	{
		pGuildMember->TakeWareItemCount = 0;
		pGuildMember->WithdrawCoin = 0;	
	}
}

void CDNGuildVillage::RecvCsGetGuildWareHistory(CDNUserSession * pSession, CSGetGuildWareHistory *pPacket)
{
	if (m_GuildWare)
		m_GuildWare->OnRecvGetWareHistory(pSession, pPacket);
}

void CDNGuildVillage::RecvMaRefreshGuildItem(MARefreshGuildItem* pPacket)
{
	if (!m_GuildWare)
		return; // 길드창고가 생성되기 전에 다른 서버에서 정보가 들어올 수 있으므로 리턴

	switch (pPacket->cMoveType)
	{
		case MoveType_GuildWare:
		{
			// 빈칸이동
			if (pPacket->biDestSerial == 0)
				m_GuildWare->UpdateWareInfo(pPacket->DestInfo);
			else // 합병하는 케이스
			{
				m_GuildWare->RemoveWareItem(pPacket->biDestSerial);
				m_GuildWare->AddWareItemInfo(pPacket->DestInfo);
			}
		}
		break;

		case MoveType_InvenToGuildWare:
		{
			// 빈칸이동
			if (pPacket->biDestSerial == 0)
				m_GuildWare->UpdateWareInfo(pPacket->DestInfo);
			else // 합병하는 케이스
			{
				m_GuildWare->RemoveWareItem(pPacket->biDestSerial);
				m_GuildWare->AddWareItemInfo(pPacket->DestInfo);
			}			
		}	
		break;

		case MoveType_GuildWareToInven:
		{
			m_GuildWare->UpdateWareInfo (pPacket->SrcInfo);
		}
		break;

#ifdef PRE_ADD_JOINGUILD_SUPPORT
		case MoveType_MailToGuildWare:
		{
			m_GuildWare->AddWareItemInfo(pPacket->DestInfo);
		}
		break;
#endif		//#ifdef PRE_ADD_JOINGUILD_SUPPORT
	}

	// 버젼을 설정한다.
	m_GuildWare->SetVersionDate(pPacket->VersionDate);

	// 길드원들에게정보를알려준다.
	SendRefreshGuildItem (pPacket->cMoveType, pPacket->biSrcSerial, pPacket->biDestSerial, &pPacket->SrcInfo, &pPacket->DestInfo);
}

void CDNGuildVillage::RecvMaRefreshGuildCoin(MARefreshGuildCoin* pPacket)
{
	SetInfo()->ui64GuildMoney = pPacket->biTotalCoin;

	SendRefreshGuildCoin (pPacket->biTotalCoin);
}
void CDNGuildVillage::UpdateMemberInfo(CDNUserSession* pSession, const TAChangeGuildMemberInfo *pPacket)
{
	DN_ASSERT(IsOpen(),								"Not Opened!");
	DN_ASSERT(IsAttach(),							"Not Attached!");
	DN_ASSERT(0 != pPacket->nReqCharacterDBID,	"Invalid!");

	switch (pPacket->btGuildMemberUpdate)
	{

		// 자기소개 변경 (길드원자신)
	case GUILDMEMBUPDATE_TYPE_INTRODUCE:	
		ModifyMembIntro(pSession, pPacket);
		break;

		// 직급변경
	case GUILDMEMBUPDATE_TYPE_ROLE:			
		ModifyMembRole(pSession, pPacket);
		break;

		// 길드장 위임
	case GUILDMEMBUPDATE_TYPE_GUILDMASTER:  
		ChangeGuildMaster(pSession, pPacket);
		break;

		// 일반포인트증가/감소(기여분)
	case GUILDMEMBUPDATE_TYPE_CMMPOINT:		
		ApplyGuildPoint(pSession, pPacket);
		break;

		// 나머지는 에러처리!
	default:	
		pSession->SendChangeGuildMemberInfo(pPacket->nReqAccountDBID, pPacket->nReqCharacterDBID, pPacket->nChgAccountDBID, pPacket->nChgCharacterDBID, pPacket->btGuildMemberUpdate, 
			ERROR_GENERIC_INVALIDREQUEST, pPacket->nInt1, pPacket->nInt2, pPacket->biInt64, pPacket->Text, NULL);
		break;
	}

	// 현재서버에접속중인길드원들에게통보
	SendChangeGuildMemberInfo(pPacket->nReqAccountDBID, pPacket->nReqCharacterDBID, pPacket->nChgAccountDBID, pPacket->nChgCharacterDBID, pPacket->btGuildMemberUpdate, pPacket->nInt1, pPacket->nInt2, pPacket->biInt64, pPacket->Text);

	// MA 를통하여다른서버에접속중인길드원들에게도통보
	if (g_pMasterConnection)
		g_pMasterConnection->SendChangeGuildMemberInfo(GetUID(), pPacket->nReqAccountDBID, pPacket->nReqCharacterDBID, pPacket->nChgAccountDBID, pPacket->nChgCharacterDBID, pPacket->btGuildMemberUpdate, pPacket->nInt1, pPacket->nInt2, pPacket->biInt64, pPacket->Text);
}

void CDNGuildVillage::GuildWarReset()
{
	CDNGuildBase::GuildWarReset();
	m_bGuildWarStats = false;
	m_nGuildWarPoint = 0;
	memset(&m_sMyGuildWarRankingInfo, 0, sizeof(m_sMyGuildWarRankingInfo));
	memset(&m_sMyGuildWarMissionGuildRankingInfo, 0, sizeof(m_sMyGuildWarMissionGuildRankingInfo));
}

void CDNGuildVillage::UpdateWarInfoOnAllMember(short wScheduleID, BYTE cTeamColorCode)
{
	if (g_pGuildWarManager && g_pGuildWarManager->GetScheduleID() != wScheduleID)
		return;

	m_Info.GuildView.wWarSchduleID = wScheduleID;
	m_Info.GuildView.cTeamColorCode = cTeamColorCode;

	ScopeLock <CSyncLock> AutoLock (GetLock());

	std::map<INT64, TGuildMember>::iterator iter = m_MapTotalMember.begin ();

	for (; iter != m_MapTotalMember.end(); iter++)
	{
		TGuildMember* pGuildMember = &(iter->second);

		CDNUserSession *pUserObj = g_pUserSessionManager->FindUserSessionByAccountDBID(pGuildMember->nAccountDBID);
		if (false == IsValiadMember (pUserObj, pGuildMember->nCharacterDBID))
			continue;

		pUserObj->GetGuildSelfView().wWarSchduleID = wScheduleID;
		pUserObj->GetGuildSelfView().cTeamColorCode = cTeamColorCode;
		pUserObj->RefreshGuildSelfView();

		// 각 유저에게 길드전에 신청을 알린다.
		pUserObj->SendEnrollGuildWar(ERROR_NONE, wScheduleID, cTeamColorCode);
	}
}

void CDNGuildVillage::SendGuildWarPreWinGuild(bool bPreWin)
{
	ScopeLock <CSyncLock> AutoLock (GetLock());

	std::map<INT64, TGuildMember>::iterator iter = m_MapTotalMember.begin ();

	for (; iter != m_MapTotalMember.end(); iter++)
	{
		TGuildMember* pGuildMember = &(iter->second);

		CDNUserSession *pUserObj = g_pUserSessionManager->FindUserSessionByAccountDBID (pGuildMember->nAccountDBID);
		if (false == IsValiadMember (pUserObj, pGuildMember->nCharacterDBID))
			continue;		

		// 각 유저에게 길드전 우승을 알림.
		pUserObj->SendGuildWarPreWinGuild(bPreWin);
	}
}

void CDNGuildVillage::SetGuildWarMyRankingInfo(TAGetGuildWarPointGuildTotal* pGetGuildWarPointGuildTotal )
{	
	m_sMyGuildWarRankingInfo.wRanking = pGetGuildWarPointGuildTotal->sGuildWarPointGuildTotal[0].wRanking;
	m_sMyGuildWarRankingInfo.nTotalPoint = pGetGuildWarPointGuildTotal->sGuildWarPointGuildTotal[0].nOpeningPointSum;
}

void CDNGuildVillage::SetGuildWarTotalRankingInfo(TAGetGuildWarPointGuildPartTotal* pGetGuildWarPointGuildPartTotal )
{
	for( int i=0; i<GUILDWAR_RANKINGTYPE_MAX; ++i)
	{
		m_sMyGuildWarMissionGuildRankingInfo[i].wRanking = pGetGuildWarPointGuildPartTotal->GuildWarPointPartTotal[i].wRanking;
		m_sMyGuildWarMissionGuildRankingInfo[i].nTotalPoint = pGetGuildWarPointGuildPartTotal->GuildWarPointPartTotal[i].nPoint;
	}
	m_bGuildWarStats = true;
}
// 길드 예선전 길드 포인트 공식
int	CDNGuildVillage::CalcGuildWarTrialPoint()
{
	//길드가 획득한 미션 포인트 + ( 길드가 획득한 미션 포인트 * 길드에서 수상한 특별상 총 개수 * 0.1)
	return (int)(m_nGuildWarPoint+(m_nGuildWarPoint*g_pGuildWarManager->GetGuildAwardCount(GetUID().nDBID)*0.1f));
}

void CDNGuildVillage::ReqGetGuildWareInfo(bool bForceReq)
{
	if (bForceReq == false)
	{
		if (m_GuildWare)
			return;
	}

	// 길드창고요청
	if (m_pDBCon)
		m_pDBCon->QueryGetGuildWareInfo(m_cDBThreadID, GetUID().nDBID, GetUID().nWorldID, GetWareSize());
	
}

void CDNGuildVillage::UpdateWareSize(CDNUserBase* pUserObject, short wSize)
{
	SetInfo()->wGuildWareSize = wSize;

	if (m_GuildWare)
		m_GuildWare->ChangeWareSize(pUserObject, wSize);
	else
		_ASSERT(0);
}

void CDNGuildVillage::SendGuildRewardItem( TGuildRewardItem* GuildRewardItem )
{
	std::vector <std::pair<UINT, INT64>, boost::pool_allocator<UINT>> List;
	GetMemberList (List);

	if (List.empty ())
		return;

	std::vector <std::pair<UINT, INT64>, boost::pool_allocator<UINT>>::iterator iter = List.begin ();

	for (; iter != List.end (); iter++)
	{
		CDNUserSession *pUserObj = g_pUserSessionManager->FindUserSessionByAccountDBID(iter->first);
		if (false == IsValiadMember(pUserObj, iter->second))
			continue;

		pUserObj->SendGuildRewardItem(GuildRewardItem);
	}
}
void CDNGuildVillage::SendAddGuildRewardItem( TGuildRewardItem GuildRewardItem )
{
	std::vector <std::pair<UINT, INT64>, boost::pool_allocator<UINT>> List;
	GetMemberList (List);

	if (List.empty ())
		return;

	std::vector <std::pair<UINT, INT64>, boost::pool_allocator<UINT>>::iterator iter = List.begin ();

	for (; iter != List.end (); iter++)
	{
		CDNUserSession *pUserObj = g_pUserSessionManager->FindUserSessionByAccountDBID(iter->first);
		if (false == IsValiadMember(pUserObj, iter->second))
			continue;

		pUserObj->SendAddGuildRewardItem(GuildRewardItem);
	}
}
void CDNGuildVillage::SendExtendGuildSize(short nGuildSize)
{
	std::vector <std::pair<UINT, INT64>, boost::pool_allocator<UINT>> List;
	GetMemberList (List);

	if (List.empty ())
		return;

	std::vector <std::pair<UINT, INT64>, boost::pool_allocator<UINT>>::iterator iter = List.begin ();

	for (; iter != List.end (); iter++)
	{
		CDNUserSession *pUserObj = g_pUserSessionManager->FindUserSessionByAccountDBID(iter->first);
		if (false == IsValiadMember(pUserObj, iter->second))
			continue;

		pUserObj->SendExtendGuildSize(nGuildSize);
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////						PRIVATE FUNCTION
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void CDNGuildVillage::SendGuildWareList(CDNUserSession * pSession, int nResult)
{
	SCOpenGuildWare Packet;
	memset (&Packet, 0x00, sizeof(Packet));

	Packet.nError = ERROR_NONE;
	Packet.nResult = nResult;
	Packet.wWareSize = GetWareSize();

	int nSize = sizeof(Packet.nError) + sizeof (Packet.nResult) + sizeof(Packet.wWareSize);
	
	if (!m_GuildWare)
		Packet.nError = ERROR_GUILDWARE_NOT_READY;
	else
	{

		TGuildMember* pGuildMember = GetMemberInfo(pSession->GetCharacterDBID());
		if (pGuildMember)
		{
			Packet.nTakeWareItemCount = pGuildMember->TakeWareItemCount;
			Packet.nWithdrawCoin = pGuildMember->WithdrawCoin;

			nSize += sizeof(Packet.nTakeWareItemCount) + sizeof(Packet.nWithdrawCoin);
		
			if (GetWareSize() <= 0)
				Packet.nError = ERROR_GUILDWARE_NOT_READY;
			else
			{

				Packet.nGuildCoin = m_GuildWare->GetWarehouseCoin();
				nSize += sizeof(Packet.nGuildCoin);

				if (nResult == 0)
				{				
					Packet.VersionDate = m_GuildWare->GetVersionDate();
					nSize += sizeof(Packet.VersionDate);

					Packet.cWareCount = m_GuildWare->GetWareList (Packet.WareList);
					nSize += (sizeof(TItemInfo)*Packet.cWareCount) + sizeof(Packet.cWareCount);
					
				}
			}
			
		}
		else
			Packet.nError = ERROR_GUILD_YOUR_NOT_BELONGANYGUILD;
			
	}

	pSession->AddSendData (SC_GUILD, eGuild::SC_OPEN_GUILDWARE, (char*)&Packet, nSize);
}

// 길드원 자기소개를 수정한다
void CDNGuildVillage::ModifyMembIntro(CDNUserSession* pSession, const TAChangeGuildMemberInfo *pPacket)
{
	{
		ScopeLock <CSyncLock> AutoLock (GetLock());

		TGuildMember* pGuildMember = GetMemberInfoAsync(pPacket->nReqCharacterDBID);
		if (pGuildMember)
			_wcscpy(pGuildMember->wszGuildMemberIntroduce, _countof(pGuildMember->wszGuildMemberIntroduce), pPacket->Text, (int)wcslen(pPacket->Text));
	}

	USES_CONVERSION;
}

// 길드원 직급을 변경한다.
void CDNGuildVillage::ModifyMembRole(CDNUserSession* pSession, const TAChangeGuildMemberInfo *pPacket)
{
	ScopeLock <CSyncLock> AutoLock (GetLock());

	TGuildMember* pGuildMember = GetMemberInfoAsync(pPacket->nChgCharacterDBID);

	if (!pGuildMember)
		return;

	// 직급변경
	pGuildMember->btGuildRole = static_cast<BYTE>(pPacket->nInt1);
}

// 길드장을 위임한다.
void CDNGuildVillage::ChangeGuildMaster(CDNUserSession* pSession, const TAChangeGuildMemberInfo *pPacket)
{
	WCHAR aCharacterName[NAMELENMAX] = { L'\0', };
	TGuildMember* pGuildMember = NULL;
	{
		ScopeLock <CSyncLock> AutoLock (GetLock());

		pGuildMember = GetMemberInfoAsync (pPacket->nChgCharacterDBID);

		// 마스터로 롤 변경
		if (pGuildMember)
		{
			pGuildMember->btGuildRole = static_cast<BYTE>(GUILDROLE_TYPE_MASTER);
			_wcscpy(aCharacterName, _countof(aCharacterName), pGuildMember->wszCharacterName, (int)wcslen(pGuildMember->wszCharacterName));
			SetGuildMaster( pGuildMember );
		}

		pGuildMember = GetMemberInfoAsync(pPacket->nReqCharacterDBID);

		// 마스터->부마스터 (변경)
		if (pGuildMember)
			pGuildMember->btGuildRole = static_cast<BYTE>(GUILDROLE_TYPE_SUBMASTER);	

	}

	// 개인세션 정보 변경
	pSession->GetGuildSelfView().btGuildRole = static_cast<BYTE>(GUILDROLE_TYPE_SUBMASTER);

	// 길드시각정보갱신
	pSession->RefreshGuildSelfView();

	USES_CONVERSION;
}

// 길드포인터를 적용한다
void CDNGuildVillage::ApplyGuildPoint(CDNUserSession* pSession, const TAChangeGuildMemberInfo *pPacket)
{
	ScopeLock <CSyncLock> AutoLock (GetLock());

	TGuildMember* pGuildMember = GetMemberInfoAsync(pPacket->nReqCharacterDBID);

	if (pGuildMember)
	{
		pGuildMember->iUseGuildCommonPoint = static_cast<long>(pPacket->nInt1);
		pGuildMember->iTotGuildCommonPoint = static_cast<long>(pPacket->nInt2);
	}
}
