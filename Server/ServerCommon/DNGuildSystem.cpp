#include "Stdafx.h"
#include "Util.h"
#include "DNGuildSystem.h"
#include "DNGameDataManager.h"

#if defined(_VILLAGESERVER)
#include "DNGuildVillage.h"
extern TVillageConfig g_Config;
#elif defined(_GAMESERVER)
#include "DNGuildGame.h"
extern TGameConfig g_Config;
#endif	// _GAMESERVER

#if defined(_VILLAGESERVER)
#include "DNGuildWarManager.h"
#elif defined(_GAMESERVER)
#include "DNMasterConnectionManager.h"
#endif // #if defined(_VILLAGESERVER)

CDNGuildSystem * g_pGuildManager = NULL;


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////						PUBLIC FUNCTION
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

CDNGuildSystem::CDNGuildSystem ()
{
	m_bOpen			= FALSE;

	m_DefaultSize	= GUILDSIZE_DEF;

	m_TickDoUpdate	= 0;

	m_nResetWareHour = 4; // �⺻��
	DNTableFileFormat* pSox = GetDNTable(CDnTableDB::TSCHEDULE );
	if (pSox)
		m_nResetWareHour = pSox->GetFieldFromLablePtr(CDNSchedule::ResetGuildWareLimit + 1, "_Hour")->GetInteger();
}

CDNGuildSystem::~CDNGuildSystem ()
{

}

DWORD CDNGuildSystem::Open (DWORD dwSize)
{
	for (DWORD i=0; i<dwSize; i++)
	{
		// ������ ���� ����ν��Ͻ� ����
		CDNGuildBase* pGuild = CreateGuildInstance ();
		DN_ASSERT(NULL != pGuild,	"Invalid!");		

		// ��� �ʱ�ȭ
		DWORD dwRetVal = pGuild->Open (this);
		if (NOERROR != dwRetVal)
			DN_RETURN (dwRetVal);

		// ����ڿ� Ǯ�� �߰�
		m_Pool.push (pGuild);

		// ��ü ����Ͽ� �߰�
		m_MainList.push_back (pGuild);
	}

	// ���� �ʱ�ȭ
	InitAuth();

	// ���⺻ �ο�
	m_DefaultSize = GUILDSIZE_DEF;

	m_bOpen = true;

	return NOERROR;
}

void CDNGuildSystem::Close()
{
	// ��ü ����ڿ� ����
	std::vector<class CDNGuildBase*, boost::pool_allocator<class CDNGuildBase*>>::iterator iter = m_MainList.begin ();

	for (; iter != m_MainList.end(); iter++)
	{
		CDNGuildBase* pGuild = (*iter);
		if (pGuild)
			pGuild->Close ();
		SAFE_DELETE (pGuild);
	}

	m_MainList.clear();
	m_CurrList.clear();
	m_DismList.clear();

	while(!m_Pool.empty()) 
		m_Pool.pop();
		
	for (int iIndex = 0 ; GUILDROLE_TYPE_CNT > iIndex ; ++iIndex) 
	{
		m_AuthBase[iIndex].Reset();
		m_AuthPrmt[iIndex].Reset();
		m_AuthRstt[iIndex].Reset();
	}
	
	m_bOpen = false;

	m_TickDoUpdate	= 0;
}

DWORD CDNGuildSystem::Release (CDNGuildBase* pGuild)
{
	// ��� �ڿ��� ������ ������ �ӽ� ����
	const TGuildUID GuildUID = pGuild->GetUID();

	// ����ڿ� �ʱ�ȭ
	pGuild->Reset ();

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// ����ȭ ����
	ScopeLock <CSyncLock>	AutoLock (GetLock());

#if !defined(_FINAL_BUILD)
	std::map<TGuildUID, class CDNGuildBase*>::iterator iter = m_CurrList.find(GuildUID);
	if (m_CurrList.end() == iter) 
	{
		DN_ASSERT(0,	"Invalid!");
	}
#endif	// _FINAL_BUILD

	// ���縮��Ʈ���� �����Ѵ�
	m_CurrList.erase (GuildUID);

	// �޸�Ǯ�� ��ȯ
	m_Pool.push (pGuild);

	return NOERROR;
}

CDNGuildBase* CDNGuildSystem::At (const TGuildUID pGuildUID)
{
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// ����ȭ ����
	ScopeLock <CSyncLock>	AutoLock (GetLock());

	std::map<TGuildUID, class CDNGuildBase*>::iterator iter = m_CurrList.find(pGuildUID);

	if (m_CurrList.end() == iter)
		return NULL;

	CDNGuildBase* pGuild = iter->second;
	// CDNGuildManager::Release(...) ���� CDNGuildBase �ʱ�ȭ ���Ŀ� ��ȯ�ϹǷ� Ÿ�̹� ������ �̷� ��찡 �߻��� �� ����
	if (!pGuild->IsSet()) 
		return NULL;

	if (pGuild->GetUID() != pGuildUID)
		return NULL;

	return pGuild;
}

const CDNGuildBase* CDNGuildSystem::At (const TGuildUID pGuildUID) const
{
	return(static_cast<CDNGuildBase*>(const_cast<CDNGuildSystem*>(this)->At(pGuildUID)));
}

CDNGuildBase* CDNGuildSystem::Get (const TGuild* pInfo)
{
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// ����ȭ ����
	ScopeLock <CSyncLock>	AutoLock (GetLock());

	std::map<TGuildUID, class CDNGuildBase*>::iterator iter = m_CurrList.find(pInfo->GuildView.GuildUID);
	if (iter != m_CurrList.end())
		return (iter->second);

	CDNGuildBase* pGuild = NULL;
	// �޸�Ǯ�� �ڿ��� ���� ���
	if (m_Pool.empty())
	{
		pGuild = CreateGuildInstance();
		if (!pGuild)
			DN_RETURN (NULL);

		// ��� �ʱ�ȭ
		DWORD dwRetVal = pGuild->Open (this);
		if (NOERROR != dwRetVal)
			DN_RETURN(NULL);

		// ��ü ����Ͽ� �߰�
		m_MainList.push_back (pGuild);
	}
	else
	{
		pGuild = m_Pool.front ();
		if (!pGuild)
			DN_RETURN (NULL);

		m_Pool.pop ();
	}

	DN_ASSERT(NULL != pGuild,		"Invalid!");
	DN_ASSERT(pGuild->IsOpen(),		"Invalid!");
	DN_ASSERT(!pGuild->IsSet(),		"Invalid!");	// ������ ���õ� ������ ����� ��
	DN_ASSERT(!pGuild->IsEnable(),	"Invalid!");	// �׼����� ��Ȱ��ȭ �����̾�� ��

	pGuild->ResetAccess();	// �׼��� ��Ȱ��ȭ
	pGuild->Set (pInfo);


	// ������� ��帮��Ʈ�� �߰�
	std::pair<std::map<TGuildUID, class CDNGuildBase*>::iterator, bool> RetVal 
		= m_CurrList.insert(std::map<TGuildUID, class CDNGuildBase*>::value_type(pInfo->GuildView.GuildUID, pGuild));

	if (!RetVal.second)
		DN_RETURN(NULL);

	return pGuild;
}

void CDNGuildSystem::GetMainList (TVecGuilds& pList)
{
	DN_ASSERT(IsOpen(),		"Not Opened!");
	DN_ASSERT(!IsLock(),	"Already Locked!");


	ScopeLock <CSyncLock>	AutoLock (GetLock());

	std::copy(m_MainList.begin(), m_MainList.end(), std::back_inserter(pList));
}

void CDNGuildSystem::GetCurrList (std::vector<TGuildUID, boost::pool_allocator<TGuildUID>>& pList)
{
	DN_ASSERT(IsOpen(),		"Not Opened!");
	DN_ASSERT(!IsLock(),	"Already Locked!");

	
	ScopeLock <CSyncLock>	AutoLock (GetLock());

	std::map<TGuildUID, class CDNGuildBase*>::iterator iter = m_CurrList.begin ();
	for (; iter != m_CurrList.end(); iter++)
		pList.push_back (iter->first);
}

bool CDNGuildSystem::AddDismiss (const TGuildUID pGuildUID)
{
	DN_ASSERT(IsOpen(),				"Not Opened!");
	DN_ASSERT(!IsLock(),			"Already Locked!");
	DN_ASSERT(pGuildUID.IsSet(),	"Invalid!");

	
	ScopeLock <CSyncLock>	AutoLock (GetLock());

	std::pair <std::set<TGuildUID>::const_iterator, bool> aRetVal = m_DismList.insert (pGuildUID);
	if (!aRetVal.second)
		DN_RETURN (false);

	return true;
}

bool CDNGuildSystem::IsDismissExist (const TGuildUID pGuildUID)
{
	DN_ASSERT(IsOpen(),				"Not Opened!");
	DN_ASSERT(!IsLock(),			"Already Locked!");
	DN_ASSERT(pGuildUID.IsSet(),	"Invalid!");

	ScopeLock <CSyncLock>	AutoLock (GetLock());

	std::set<TGuildUID>::const_iterator iter = m_DismList.find (pGuildUID);
	if (iter != m_DismList.end ())
		return true;

	return false;
}

void CDNGuildSystem::DoUpdate (DWORD CurTick)
{
	DN_ASSERT(IsOpen(),		"Not Opened!");
	DN_ASSERT(!IsLock(),	"Already Locked!");

	// ��ü ��� �������� (10��)
	if (GUILDMGRUPDTERM > GetTickTerm (m_TickDoUpdate, CurTick))
		return;

	m_TickDoUpdate = CurTick;

	// ���纻�� �����
	std::vector <std::pair<TGuildUID, class CDNGuildBase*>, boost::pool_allocator<TGuildUID>> aList;

	{
		ScopeLock <CSyncLock>	AutoLock (GetLock());
		std::copy(m_CurrList.begin(), m_CurrList.end(), std::back_inserter(aList));
	}

	// ��ϵ� ��尡 ������ ����
	if (aList.empty ())
		return;


	// ����̺�Ʈ�� ���� ������Ʈ
	std::vector <std::pair<TGuildUID, class CDNGuildBase*>, boost::pool_allocator<TGuildUID>>::iterator iter = aList.begin ();

	for (; iter != aList.end(); iter++)
	{
		const TGuildUID GuildUID = iter->first;
		DN_ASSERT(GuildUID.IsSet(),	"Invalid!");

		CDNGuildBase* pGuild = static_cast<CDNGuildBase*>(iter->second);
		DN_ASSERT(NULL != pGuild,	"Invalid!");
		if (!pGuild) continue;

#if !defined( PRE_ADD_NODELETEGUILD )
		CDetachAutoEx<CDNGuildBase>	AutoDetach (pGuild);
		if (FALSE == pGuild->IsEnable()) continue;
#endif

		if (!pGuild->IsSet ()) continue;

		pGuild->DoUpdate (CurTick);
	}
}

void CDNGuildSystem::OnRecvMaDismissGuild (const MADismissGuild *pPacket)
{
	DN_ASSERT(IsOpen(),						"Not Opened!");
	DN_ASSERT(!IsLock(),					"Already Locked!");
	DN_ASSERT(NULL != pPacket,				"Invalid!");
#if defined(_WORK)
	DN_ASSERT(0 != pPacket->nManagedID,	"Invalid!");
#endif	// _WORK

	// �ڽ��� MA �� ���´� ��Ŷ�� �۽� ������ �� �������� ���� ó���Ǿ����� ������ �ϹǷ� �߰��� ó������ ����
	if (pPacket->nManagedID == g_Config.nManagedID)
		return;
	
	// �̹� ��ü�� ������� Ȯ���Ѵ�.
	if (IsDismissExist (pPacket->GuildUID))
		return;
	
	// ��� �ػ� ��Ͽ� ���
	AddDismiss (pPacket->GuildUID);

	CDNGuildBase* pGuild = At (pPacket->GuildUID);
	if (!pGuild) return;
	
#if !defined( PRE_ADD_NODELETEGUILD )
	CDetachAutoEx<CDNGuildBase>	AutoDetach (pGuild);
	if (FALSE == pGuild->IsEnable()) return;
#endif

#if !defined( PRE_ADD_NODELETEGUILD )
	// ��� �ڿ� �ݳ�
	pGuild->DisableAttach ();
#endif

}

bool CDNGuildSystem::PreRecvProcess (int nManagedID, TGuildUID pGuildUID, CDNGuildBase*& pGuild, bool bCheckManagdID)
{
	DN_ASSERT(IsOpen(),						"Not Opened!");
	DN_ASSERT(!IsLock(),					"Already Locked!");
#if defined(_WORK)
	DN_ASSERT(0 != nManagedID,				"Invalid!");
#endif	// _WORK

	if (nManagedID == g_Config.nManagedID && bCheckManagdID)
		return false;

	if (IsDismissExist(pGuildUID))
		return false;

	pGuild = At (pGuildUID);
	if (!pGuild) return false;

	return true;
}

void CDNGuildSystem::OnRecvMaAddGuildMember (const MAAddGuildMember *pPacket)
{
	CDNGuildBase* pGuild = NULL;
	if (false == PreRecvProcess (pPacket->nManagedID, pPacket->GuildUID, pGuild))
		return;
	
#if !defined( PRE_ADD_NODELETEGUILD )
	CDetachAutoEx<CDNGuildBase>	AutoDetach (pGuild);
	if (FALSE == pGuild->IsEnable()) return;
#endif

	//��ġ����
	TCommunityLocation Location;
	Location.Reset();

	// ���� ���� �߰�
	TGuildMember GuildMember;
	GuildMember.Set(pPacket->nAccountDBID, pPacket->nCharacterDBID, pPacket->wszCharacterName, pPacket->nJob, pPacket->cLevel, GUILDROLE_TYPE_JUNIOR, pPacket->JoinDate);
		
	if (false == pGuild->AddMember(&GuildMember, &Location))
		return;
	
	pGuild->SendAddGuildMember (pPacket->nAccountDBID, pPacket->nCharacterDBID, pPacket->wszCharacterName, pPacket->nJob, pPacket->cLevel, pPacket->JoinDate, &Location);
}

// ���� ���� ����
void CDNGuildSystem::OnRecvMaDelGuildMember (const MADelGuildMember *pPacket)
{
	CDNGuildBase* pGuild = NULL;
	if (false == PreRecvProcess (pPacket->nManagedID, pPacket->GuildUID, pGuild))
		return;

#if !defined( PRE_ADD_NODELETEGUILD )
	CDetachAutoEx<CDNGuildBase>	AutoDetach (pGuild);
	if (FALSE == pGuild->IsEnable()) return;
#endif

	pGuild->SendDelGuildMember (pPacket->nAccountDBID, pPacket->nCharacterDBID, pPacket->bIsExiled);

	// ������ ���� �����Ͽ� ������ �켱 ���� �޽��� ���� ����, �ٸ� ���Ӽ����� �ش� ������� �۾� �����忡 ���� ���� ���� ���������Ͽ��� �̹� ������ �����̹Ƿ� ��ü ����ڸ� ������� ã�ƾ� �� (20100210 b4nfter)
	pGuild->DelMember (pPacket->nCharacterDBID);
}

// ������� ����
void CDNGuildSystem::OnRecvMaChangeGuildInfo (const MAChangeGuildInfo *pPacket)
{
	CDNGuildBase* pGuild = NULL;
	if (false == PreRecvProcess (pPacket->nManagedID, pPacket->GuildUID, pGuild))
		return;

#if !defined( PRE_ADD_NODELETEGUILD )
	CDetachAutoEx<CDNGuildBase>	AutoDetach (pGuild);
	if (FALSE == pGuild->IsEnable()) return;
#endif

	switch (pPacket->btGuildUpdate)
	{
		// ��������
		case GUILDUPDATE_TYPE_NOTICE:
			MaChangeNotification (pGuild, pPacket);
			break;
		
		// ���� ���� ����
		case GUILDUPDATE_TYPE_ROLEAUTH:
			MaChangeRoleAuth (pGuild, pPacket);
			break;
		case GUILDUPDATE_TYPE_GUILDWAR :
			MaChangeGuildWarFinalInfo(pGuild, pPacket);
			break;
#if defined( PRE_ADD_GUILD_EASYSYSTEM )
		case GUILDUPDATE_TYPE_HOMEPAGE:
			MaChangeGuildHomePage(pGuild, pPacket);
			break;
#endif
		default:
			break;
	}

	// ���� ������ ���� ���� �����鿡�� �뺸
	pGuild->SendChangeGuildInfo (pPacket->nAccountDBID, pPacket->nCharacterDBID, pPacket->btGuildUpdate, pPacket->Int1, pPacket->Int2, pPacket->Int3, pPacket->Int4, pPacket->Int64, pPacket->Text);
}

void CDNGuildSystem::OnRecvMaChangeGuildMemberInfo (const MAChangeGuildMemberInfo *pPacket)
{
	DN_ASSERT(NULL != pPacket,				"Invalid!");

	CDNGuildBase* pGuild = NULL;
	if (false == PreRecvProcess (pPacket->nManagedID, pPacket->GuildUID, pGuild))
		return;

#if !defined( PRE_ADD_NODELETEGUILD )
	CDetachAutoEx<CDNGuildBase>	AutoDetach (pGuild);
	if (FALSE == pGuild->IsEnable()) return;
#endif

	switch (pPacket->btGuildMemberUpdate)
	{
		// �ڱ�Ұ� ���� (�����ڽ�)
		case GUILDMEMBUPDATE_TYPE_INTRODUCE:	
			MaModifyMembIntro (pGuild, pPacket);
			break;

		// ���� ����
		case GUILDMEMBUPDATE_TYPE_ROLE:
			MaModifyTypeRole (pGuild, pPacket);
			break;

		// ����� ����
		case GUILDMEMBUPDATE_TYPE_GUILDMASTER:
			MaDelegateGuildMaster (pGuild, pPacket);
			break;

#ifdef PRE_ADD_CHANGEJOB_CASHITEM
		case GUILDMEMBUPDATE_TYPE_JOBCODE:
			MaModifyMembJob (pGuild, pPacket->nReqCharacterDBID, pPacket->Int1);
			break;
#endif			
		case GUILDMEMBUPDATE_TYPE_LOGINOUT:
			MaChangeConnectState(pGuild, pPacket);
			break;
		default:
			return;
		
	}

	pGuild->SendChangeGuildMemberInfo (pPacket->nReqAccountDBID, pPacket->nReqCharacterDBID, pPacket->nChgAccountDBID, pPacket->nChgCharacterDBID, pPacket->btGuildMemberUpdate, pPacket->Int1, pPacket->Int2, pPacket->Int64, pPacket->Text);
}

void CDNGuildSystem::OnRecvMaGuildChat (const MAGuildChat *pPacket)
{
	CDNGuildBase* pGuild = NULL;
	if (false == PreRecvProcess (pPacket->nManagedID, pPacket->GuildUID, pGuild, false))
		return;
	
#if !defined( PRE_ADD_NODELETEGUILD )
	CDetachAutoEx<CDNGuildBase>	AutoDetach (pGuild);
	if (FALSE == pGuild->IsEnable()) return;
#endif

	pGuild->SendGuildChat (pPacket->nAccountDBID, pPacket->nCharacterDBID, pPacket->wszChatMsg, pPacket->nLen);
}

#ifdef PRE_ADD_DOORS_GUILDCHAT_DISCONNECT
void CDNGuildSystem::OnRecvMaDoorsGuildChat(const MADoorsGuildChat *pPacket)
{
	CDNGuildBase* pGuild = NULL;
	if (false == PreRecvProcess(pPacket->nManagedID, pPacket->GuildUID, pGuild, false))
		return;

#if !defined( PRE_ADD_NODELETEGUILD )
	CDetachAutoEx<CDNGuildBase>	AutoDetach (pGuild);
	if (FALSE == pGuild->IsEnable()) return;
#endif

	if (pGuild->IsMemberExist(pPacket->nCharacterDBID, NULL))
	{
		pGuild->SendDoorsGuildChat(pPacket->nCharacterDBID, pPacket->wszChatMsg, pPacket->nLen);
		return;
	}
	_DANGER_POINT_MSG(L"����ο��� �ƴѵ� ����Ͽ��� �޼����� ���Դ�");
}
#endif		//#ifdef PRE_ADD_DOORS_GUILDCHAT_DISCONNECT

void CDNGuildSystem::OnRecvMaGuildChangeName(const MAGuildChangeName *pPacket)
{
	// �ش��带 ã�´�.	
	if (!pPacket->GuildUID.IsSet())
		return;

	CDNGuildBase* pGuild = this->At (pPacket->GuildUID);
	if (!pGuild) return;

#if !defined( PRE_ADD_NODELETEGUILD )
	// ��尡 �����Ѵ�.
	CDetachAutoEx<CDNGuildBase>	AutoDetach (pGuild);
	if (FALSE == pGuild->IsEnable()) return;
#endif

	_wcscpy( pGuild->SetInfo()->GuildView.wszGuildName, _countof(pGuild->SetInfo()->GuildView.wszGuildName), pPacket->wszGuildName, (int)wcslen(pPacket->wszGuildName) );	
	
	pGuild->SendChangeGuildName(pPacket->wszGuildName);
}

void CDNGuildSystem::OnRecvMaGuildChangeMark(const MAGuildChangeMark *pPacket)
{
	// �ش��带 ã�´�.	
	if (!pPacket->GuildUID.IsSet())
		return;

	CDNGuildBase* pGuild = this->At (pPacket->GuildUID);
	if (!pGuild) return;

#if !defined( PRE_ADD_NODELETEGUILD )
	// ��尡 �����Ѵ�.
	CDetachAutoEx<CDNGuildBase>	AutoDetach (pGuild);
	if (FALSE == pGuild->IsEnable()) return;
#endif

	pGuild->SetInfo()->GuildView.wGuildMark = pPacket->wMark;
	pGuild->SetInfo()->GuildView.wGuildMarkBG = pPacket->wMarkBG;
	pGuild->SetInfo()->GuildView.wGuildMarkBorder = pPacket->wMarkBorder;

	pGuild->SendChangeGuildMark(pPacket);
}

void CDNGuildSystem::OnRecvMaGuildUpdateExp( const MAUpdateGuildExp* pPacket )
{
	// �ش��带 ã�´�.	
	if (!pPacket->GuildUID.IsSet())
		return;

	CDNGuildBase* pGuild = this->At (pPacket->GuildUID);
	if (!pGuild) return;

#if !defined( PRE_ADD_NODELETEGUILD )
	// ��尡 �����Ѵ�.
	CDetachAutoEx<CDNGuildBase>	AutoDetach (pGuild);
	if (FALSE == pGuild->IsEnable()) return;
#endif

	pGuild->UpdateGuildExp(pPacket);

}

void CDNGuildSystem::OnRecvMaEnrollGuildWar(const MAEnrollGuildWar *pPacket)
{
	// �ش��带 ã�´�.	
	if (!pPacket->GuildUID.IsSet())
		return;

	CDNGuildBase* pGuild = this->At (pPacket->GuildUID);
	if (!pGuild) return;

#if !defined( PRE_ADD_NODELETEGUILD )
	// ��尡 �����Ѵ�.
	CDetachAutoEx<CDNGuildBase>	AutoDetach (pGuild);
	if (FALSE == pGuild->IsEnable()) return;
#endif

	pGuild->UpdateWarInfoOnAllMember(pPacket->wScheduleID, pPacket->cTeamColorCode);
}

void CDNGuildSystem::OnRecvMaMemberLevelUp (const MAGuildMemberLevelUp *pPacket)
{
	CDNGuildBase* pGuild = NULL;
	if (false == PreRecvProcess (pPacket->nManagedID, pPacket->GuildUID, pGuild))
		return;

#if !defined( PRE_ADD_NODELETEGUILD )
	CDetachAutoEx<CDNGuildBase>	AutoDetach (pGuild);
	if (FALSE == pGuild->IsEnable()) return;
#endif

	pGuild->UpdateMemberLevel (pPacket->nCharacterDBID, pPacket->cLevel);
}

void CDNGuildSystem::OnRecvCsGetGuildInfo (CDNUserSession * pSession, const CSGetGuildInfo *pPacket)
{
	const TGuildUID GuildUID = pSession->GetGuildSelfView().GuildUID;
	if (!GuildUID.IsSet())
		return;
	
	CDNGuildBase* pGuild = this->At (GuildUID);
	if (pGuild) 
	{
		// ��� ���� ����
#if !defined( PRE_ADD_NODELETEGUILD )
		CDetachAutoEx<CDNGuildBase>	AutoDetach (pGuild);
		if (FALSE == pGuild->IsEnable()) return;
#endif

		if( pGuild->GetRecvAllMember() == false ) return; 	// ������� �� �޾Ҵ��� üũ
		// ��Ŷ�� �����Ѵ�
		pSession->SendGetGuildInfo(pGuild, ERROR_NONE);
		pSession->SendGetGuildMember(pGuild, ERROR_NONE);
		return;
	}
 
	if (IsDismissExist(GuildUID)) 
	{
		// ��尡 �̹� ��ü �Ǿ��ٸ� ��� �ð������� �ʱ�ȭ�Ͽ� �ֺ� ����ȭ
		pSession->ResetGuildSelfView ();
		return;
	}

	// P.S.> ��� ������ �������� �ʴ��� ���⿡���� ���� DB �� ���� Ȯ�� ��û�� �������� �ʵ� (����� �α��� �� 1ȸ �߻�)
}

void CDNGuildSystem::OnRecvCsGetGuildWareHistory (CDNUserSession * pSession, CSGetGuildWareHistory *pPacket)
{
	// �ش��带 ã�´�.
	const TGuildUID GuildUID = pSession->GetGuildSelfView().GuildUID;
	if (!GuildUID.IsSet())
		return;

	CDNGuildBase* pGuild = this->At (GuildUID);
	if (!pGuild) return;

#if !defined( PRE_ADD_NODELETEGUILD )
	// ��尡 �����Ѵ�.
	CDetachAutoEx<CDNGuildBase>	AutoDetach (pGuild);
	if (FALSE == pGuild->IsEnable()) return;
#endif

	// ���â�� �����丮 �̺�Ʈ ó��
	pGuild->RecvCsGetGuildWareHistory (pSession, pPacket);
}

void CDNGuildSystem::OnRecvCsCloseGuildWare (CDNUserSession * pSession)
{
	DN_ASSERT(IsOpen(),			"Not Opened!");
	DN_ASSERT(!IsLock(),		"Already Locked!");
	DN_ASSERT(NULL != pSession,	"Invalid!");

	// �ش��带 ã�´�.
	const TGuildUID GuildUID = pSession->GetGuildSelfView().GuildUID;
	if (!GuildUID.IsSet())
		return;

	CDNGuildBase* pGuild = this->At (GuildUID);
	if (!pGuild) return;

#if !defined( PRE_ADD_NODELETEGUILD )
	// ��尡 �����Ѵ�.
	CDetachAutoEx<CDNGuildBase>	AutoDetach (pGuild);
	if (FALSE == pGuild->IsEnable()) return;
#endif

	// ���â�� ���� �̺�Ʈ ó��
	pGuild->RecvCsCloseGuildWare (pSession);
}

bool CDNGuildSystem::OnRecvApiOpenGuildWare (CDNUserSession * pSession)
{
	DN_ASSERT(IsOpen(),			"Not Opened!");
	DN_ASSERT(!IsLock(),		"Already Locked!");
	DN_ASSERT(NULL != pSession,	"Invalid!");

	// �ش��带 ã�´�.
	const TGuildUID GuildUID = pSession->GetGuildSelfView().GuildUID;
	if (!GuildUID.IsSet())
		return false;

	CDNGuildBase* pGuild = this->At (GuildUID);
	if (!pGuild) 
		return false;
	

#if !defined( PRE_ADD_NODELETEGUILD )
	// ��尡 �����Ѵ�.
	CDetachAutoEx<CDNGuildBase>	AutoDetach (pGuild);
	if (FALSE == pGuild->IsEnable()) 
		return false;
#endif

	// ���â�� ���� �̺�Ʈ ó��
	pGuild->RecvApiOpenGuildWare (pSession);

	return true;

}

DWORD CDNGuildSystem::GetAuthBase (eGuildRoleType pGuildRoleType) const
{
	DN_ASSERT(CHECK_LIMIT(pGuildRoleType, GUILDROLE_TYPE_CNT),	"Invalid!");

	return m_AuthBase[pGuildRoleType].GetRaw();
}


DWORD CDNGuildSystem::GetAuthPrmt (eGuildRoleType pGuildRoleType) const
{
	DN_ASSERT(CHECK_LIMIT(pGuildRoleType, GUILDROLE_TYPE_CNT),	"Invalid!");

	return m_AuthPrmt[pGuildRoleType].GetRaw();
}

DWORD CDNGuildSystem::GetAuthRstt (eGuildRoleType pGuildRoleType) const
{
	DN_ASSERT(CHECK_LIMIT(pGuildRoleType, GUILDROLE_TYPE_CNT),	"Invalid!");

	return m_AuthRstt[pGuildRoleType].GetRaw();
}

const CBitSetEx1<GUILDAUTH_TYPE_CNT, WORD>& CDNGuildSystem::RefAuthBase (eGuildRoleType pGuildRoleType) const
{
	DN_ASSERT(CHECK_LIMIT(pGuildRoleType, GUILDROLE_TYPE_CNT),	"Invalid!");

	return m_AuthBase[pGuildRoleType];
}


const CBitSetEx1<GUILDAUTH_TYPE_CNT, WORD>& CDNGuildSystem::RefAuthPrmt (eGuildRoleType pGuildRoleType) const
{
	DN_ASSERT(CHECK_LIMIT(pGuildRoleType, GUILDROLE_TYPE_CNT),	"Invalid!");

	return m_AuthPrmt[pGuildRoleType];
}

const CBitSetEx1<GUILDAUTH_TYPE_CNT, WORD>& CDNGuildSystem::RefAuthRstt (eGuildRoleType pGuildRoleType) const
{
	DN_ASSERT(CHECK_LIMIT(pGuildRoleType, GUILDROLE_TYPE_CNT),	"Invalid!");

	return m_AuthRstt[pGuildRoleType];
}

void CDNGuildSystem::SetAuthBase (int pGuildRoleAuth[GUILDROLE_TYPE_CNT]) const
{
	DN_ASSERT(NULL != pGuildRoleAuth,	"Invalid!");

	for (int iIndex = 0 ; GUILDROLE_TYPE_CNT > iIndex ; ++iIndex) {
		pGuildRoleAuth[iIndex] |= GetAuthBase(static_cast<eGuildRoleType>(iIndex));
	}
}

void CDNGuildSystem::SetAuthMust (int pGuildRoleAuth[GUILDROLE_TYPE_CNT]) const
{
	DN_ASSERT(NULL != pGuildRoleAuth,	"Invalid!");

	for (int iIndex = 0 ; GUILDROLE_TYPE_CNT > iIndex ; ++iIndex) 
	{
		pGuildRoleAuth[iIndex] |= GetAuthPrmt(static_cast<eGuildRoleType>(iIndex));
		pGuildRoleAuth[iIndex] &= (~GetAuthRstt(static_cast<eGuildRoleType>(iIndex)));
	}
}

void CDNGuildSystem::SetRoleLimitation (TGuild* pDestInfo)
{
	int wGuildRoleAuth [GUILDROLE_TYPE_CNT];
	INT nRoleMaxItem[GUILDROLE_TYPE_CNT];				
	INT nRoleMaxCoin[GUILDROLE_TYPE_CNT];

	
	memset(nRoleMaxItem, 0, sizeof(nRoleMaxItem));
	memset(nRoleMaxCoin, 0, sizeof(nRoleMaxCoin));

	SetAuthBase (wGuildRoleAuth);
	SetAuthMust (wGuildRoleAuth);

#if defined( PRE_ADD_CHANGEGUILDROLE )
	for (int i=GUILDROLE_TYPE_SENIOR; i<GUILDROLE_TYPE_CNT; i++)
#else
	for (int i=GUILDROLE_TYPE_SUBMASTER; i<GUILDROLE_TYPE_CNT; i++)
#endif
	{
		if (pDestInfo->wGuildRoleAuth[i] == -1 || pDestInfo->nRoleMaxItem[i] == -1 || pDestInfo->nRoleMaxCoin[i] == -1)
		{
			// �н��Ѵ�.
		}
		else
		{
			// ����
			wGuildRoleAuth [i] = pDestInfo->wGuildRoleAuth[i];

			// ���������� 
			nRoleMaxItem [i] = pDestInfo->nRoleMaxItem[i];	

			// ���� ����
			nRoleMaxCoin [i] = pDestInfo->nRoleMaxCoin[i];	
		}
	}

	memcpy (pDestInfo->wGuildRoleAuth, wGuildRoleAuth, sizeof(pDestInfo->wGuildRoleAuth));
	memcpy (pDestInfo->nRoleMaxItem, nRoleMaxItem, sizeof(pDestInfo->nRoleMaxItem));
	memcpy (pDestInfo->nRoleMaxCoin, nRoleMaxCoin, sizeof(pDestInfo->nRoleMaxCoin));
}

void CDNGuildSystem::UpdateGuildResource (TAGetGuildInfo *pPacket)
{
	// ��� ���� ���� 
	this->SetAuthMust (pPacket->Info.wGuildRoleAuth);

	const TGuildUID GuildUID(pPacket->cWorldSetID, pPacket->nGuildDBID);

	CDNGuildBase* pGuild = this->At (GuildUID);	

	// ���� ����
	SetRoleLimitation (&pPacket->Info);

	// ����ڿ��� ���� ��� (�ش缭���� �����ϴ� ù ������ ���)
	if (!pGuild)
	{
		// ����ڿ��� ���´�.
		pGuild = this->Get (&pPacket->Info);

		if (!pGuild) return;		// �Ҵ����
#if defined(_VILLAGESERVER)
		pGuild->SetGuildWarScheduleID(g_pGuildWarManager->GetScheduleID());	
		// ���� ��±�� �̸�
		if( g_pGuildWarManager->GetPreWinGuildUID() == GuildUID )
		{
			CDNGuildVillage* pGuildVillage = static_cast<CDNGuildVillage *>(pGuild);
			pGuildVillage->SendGuildWarPreWinGuild(true);
		}
#elif defined(_GAMESERVER)
		pGuild->SetGuildWarScheduleID(g_pMasterConnectionManager->GetGuildWarScheduleID(pPacket->cWorldSetID));		
#endif // #if defined(_VILLAGESERVER)		
	}
	else // ���� ����ڿ��� �ִ� ���
		pGuild->Set (&pPacket->Info);
	
#if !defined( PRE_ADD_NODELETEGUILD )
	CDetachAutoEx<CDNGuildBase>	AutoDetach (pGuild);
	if (FALSE == pGuild->IsEnable()) return;
#endif

	pGuild->ReqGetGuildWareInfo ();
}

void CDNGuildSystem::UpdateGuildMember (TAGetGuildMember *pPacket)
{
	const TGuildUID GuildUID(pPacket->cWorldSetID, pPacket->nGuildDBID);

	CDNGuildBase* pGuild = this->At (GuildUID);	
	
	if(pGuild)
	{
#if !defined( PRE_ADD_NODELETEGUILD )
		CDetachAutoEx<CDNGuildBase>	AutoDetach (pGuild);
		if (FALSE == pGuild->IsEnable()) return;
#endif
	
		pGuild->UpdateMember (pPacket);
	}

}

void CDNGuildSystem::OpenGuildWare (const TAGetGuildWareInfo *pPacket)
{
	const TGuildUID GuildUID (pPacket->cWorldSetID, pPacket->nGuildDBID);

	CDNGuildBase* pGuild = this->At (GuildUID);
	if (!pGuild) return;

#if !defined( PRE_ADD_NODELETEGUILD )
	CDetachAutoEx<CDNGuildBase>	AutoDetach (pGuild);	
	if (FALSE == pGuild->IsEnable()) return;
#endif

	pGuild->OpenWare (pPacket);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////						PRIVATE FUNCTION
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CDNGuildSystem::MaChangeNotification (CDNGuildBase* pGuild, const MAChangeGuildInfo *pPacket)
{
	_wcscpy(pGuild->SetInfo()->wszGuildNotice, _countof(pGuild->GetInfo()->wszGuildNotice), pPacket->Text, (int)wcslen(pPacket->Text));
}

void CDNGuildSystem::MaChangeRoleAuth (CDNGuildBase* pGuild, const MAChangeGuildInfo *pPacket)
{
	DN_ASSERT(CHECK_LIMIT(pPacket->Int1, GUILDROLE_TYPE_CNT),		"Invalid!");

	pGuild->SetInfo()->wGuildRoleAuth[pPacket->Int1] = static_cast<int>(pPacket->Int2);
	pGuild->SetInfo()->nRoleMaxItem[pPacket->Int1] = static_cast<int>(pPacket->Int3);
	pGuild->SetInfo()->nRoleMaxCoin[pPacket->Int1] = static_cast<int>(pPacket->Int4);
}

void CDNGuildSystem::MaChangePoint (CDNGuildBase* pGuild, const MAChangeGuildInfo *pPacket)
{
	pGuild->SetInfo()->iTotalGuildExp = static_cast<long>(pPacket->Int2);
	pGuild->CheckGuildInfo();
}

void CDNGuildSystem::MaChangeGuildWarFinalInfo(CDNGuildBase* pGuild, const MAChangeGuildInfo *pPacket)
{
	pGuild->SetInfo()->nGuildWarFinalCount += pPacket->Int1;
	pGuild->SetInfo()->nGuildWarFinalWinCount += pPacket->Int2;
}

#if defined( PRE_ADD_GUILD_EASYSYSTEM )
void CDNGuildSystem::MaChangeGuildHomePage(CDNGuildBase* pGuild, const MAChangeGuildInfo *pPacket)
{
	_wcscpy(pGuild->SetInfo()->wszGuildHomePage, _countof(pGuild->GetInfo()->wszGuildHomePage), pPacket->Text, (int)wcslen(pPacket->Text));
}
#endif

#ifdef PRE_ADD_CHANGEJOB_CASHITEM
void CDNGuildSystem::MaModifyMembJob (CDNGuildBase* pGuild, INT64 nCharacterDBID, int nChangeJob)
{
	TGuildMember* pGuildMember = NULL;

	ScopeLock <CSyncLock> AutoLock (pGuild->GetLock());

	pGuildMember = pGuild->GetMemberInfoAsync (nCharacterDBID);
	if (pGuildMember)
		pGuildMember->nJob = nChangeJob;
}
#endif
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CDNGuildSystem::MaModifyMembIntro (CDNGuildBase* pGuild, const MAChangeGuildMemberInfo* pPacket)
{	
	{
		ScopeLock <CSyncLock> AutoLock (pGuild->GetLock());

		TGuildMember* pGuildMember = pGuild->GetMemberInfoAsync (pPacket->nReqCharacterDBID);
		if (pGuildMember)
			_wcscpy(pGuildMember->wszGuildMemberIntroduce, _countof(pGuildMember->wszGuildMemberIntroduce), pPacket->Text, (int)wcslen(pPacket->Text));
	}
}

void CDNGuildSystem::MaModifyTypeRole (CDNGuildBase* pGuild, const MAChangeGuildMemberInfo* pPacket)
{	
	ScopeLock <CSyncLock> AutoLock (pGuild->GetLock());

	TGuildMember* pGuildMember = pGuild->GetMemberInfoAsync (pPacket->nChgCharacterDBID);

	if (pGuildMember)
		pGuildMember->btGuildRole = static_cast<BYTE>(pPacket->Int1);
}

void CDNGuildSystem::MaDelegateGuildMaster (CDNGuildBase* pGuild, const MAChangeGuildMemberInfo* pPacket)
{
	TGuildMember* pGuildMember = NULL;
	
	ScopeLock <CSyncLock> AutoLock (pGuild->GetLock());

	// ���ӹ��� ��
	pGuildMember = pGuild->GetMemberInfoAsync (pPacket->nChgCharacterDBID);
	if (pGuildMember)
	{
		pGuildMember->btGuildRole = static_cast<BYTE>(GUILDROLE_TYPE_MASTER);
		pGuild->SetGuildMaster( pGuildMember );
	}

	// ��û�� �� (����������)
	pGuildMember = pGuild->GetMemberInfoAsync (pPacket->nReqCharacterDBID);
	if (pGuildMember)
		pGuildMember->btGuildRole = static_cast<BYTE>(GUILDROLE_TYPE_SUBMASTER);
}

void CDNGuildSystem::MaChangeConnectState (CDNGuildBase* pGuild, const MAChangeGuildMemberInfo* pPacket)
{
	ScopeLock <CSyncLock> AutoLock (pGuild->GetLock());

	TGuildMember* pGuildMember = pGuild->GetMemberInfoAsync (pPacket->nChgCharacterDBID);

	if (!pGuildMember)
		return;

	//�α׾ƿ� ó��
	switch(pPacket->Int1)
	{
		case _LOCATION_NONE:		//�α� �ƿ�
			{
				pGuildMember->Location.Reset ();
			}
			break;
		case _LOCATION_MOVE:		//�α���
			{
				pGuildMember->Location.cServerLocation = _LOCATION_MOVE;
			}
			break;
		default:
			{
			}
			break;
	}
}

void CDNGuildSystem::UpdateGuildRewardItem( TAGetGuildRewardItem *pGuildRewardItem )
{
	TGuildRewardItem RewardItemInfo[GUILDREWARDEFFECT_TYPE_CNT]; 
	memset( RewardItemInfo, 0, sizeof(RewardItemInfo));

	for(int i=0;i<pGuildRewardItem->nCount;i++)
	{
		TGuildRewardItemData* pItemData = g_pDataManager->GetGuildRewardItemData( pGuildRewardItem->RewardItemInfo[i].nItemID );
		if(pItemData)
		{
			int nIndex = pItemData->nItemType;
			if( nIndex >= GUILDREWARDEFFECT_TYPE_CNT )
			{
				continue;
			}										
			RewardItemInfo[nIndex].nItemID = pGuildRewardItem->RewardItemInfo[i].nItemID;
			RewardItemInfo[nIndex].nItemType = pItemData->nItemType;
			RewardItemInfo[nIndex].nEffectValue = pItemData->nTypeParam1;
#if defined( PRE_ADD_GUILDREWARDITEM )
			RewardItemInfo[nIndex].nEffectValue2 = pItemData->nTypeParam2;
#endif
			RewardItemInfo[nIndex].bEternity = pGuildRewardItem->RewardItemInfo[i].bEternity;
			RewardItemInfo[nIndex].m_tExpireDate = pGuildRewardItem->RewardItemInfo[i].m_tExpireDate;
		}
	}			

	const TGuildUID GuildUID(pGuildRewardItem->cWorldSetID, pGuildRewardItem->nGuildDBID);			

	CDNGuildBase* pGuild = this->At(GuildUID);
	if (!pGuild) 
	{				
		return;
	}

#if !defined( PRE_ADD_NODELETEGUILD )
	CDetachAutoEx<CDNGuildBase>	AutoDetach(pGuild);
	if (FALSE == pGuild->IsEnable()) return;			
#endif

	pGuild->SetGuildRewardItem(RewardItemInfo);
	pGuild->SendGuildRewardItem(RewardItemInfo);
}
int CDNGuildSystem::BuyGuildRewardItem( CDNUserSession * pSession, int nItemID )
{
	int nRet = ERROR_NONE;
#if defined( _VILLAGESERVER )
	const TGuildUID GuildUID = pSession->GetGuildSelfView().GuildUID;
	if (!GuildUID.IsSet())
		return ERROR_GUILD_NOTEXIST_GUILD;

	CDNGuildBase* pGuild = this->At (GuildUID);
	if (!pGuild) return ERROR_GUILD_NOTEXIST_GUILD;

#if !defined( PRE_ADD_NODELETEGUILD )
	// ��尡 �����Ѵ�.
	CDetachAutoEx<CDNGuildBase>	AutoDetach (pGuild);
	if (FALSE == pGuild->IsEnable()) return ERROR_GUILD_NOTEXIST_GUILD;
#endif

	//��庸������� ����
	TGuildRewardItemData* pGuildRewardItemData = g_pDataManager->GetGuildRewardItemData(nItemID) ;	
	
	if(pGuildRewardItemData)
	{
		// ���� üũ
		if( pGuildRewardItemData->nItemType >= GUILDREWARDEFFECT_TYPE_CNT )
		{
			pSession->SendBuyGuildRewardItem(ERROR_ITEM_FAIL, 0);
			return ERROR_ITEM_FAIL;
		}
		if( pGuildRewardItemData->bCheckMaster )
		{			
			if(pSession->GetGuildSelfView().btGuildRole != GUILDROLE_TYPE_MASTER )
			{
				pSession->SendBuyGuildRewardItem(ERROR_GUILD_ONLYAVAILABLE_GUILDMASTER, 0);
				return ERROR_GUILD_ONLYAVAILABLE_GUILDMASTER;
			}
		}
		if( pGuild->GetLevel() < pGuildRewardItemData->nNeedGuildLevel )
		{			
			pSession->SendBuyGuildRewardItem(ERROR_GUILD_LACKOFCOUNT_GUILDLEVEL, 0);
			return ERROR_GUILD_LACKOFCOUNT_GUILDLEVEL;
		}

		if( pGuildRewardItemData->nItemType == GUILDREWARDEFFECT_TYPE_MAXGUILDWARE )
		{
			// ��� â�� �ִ�ũ�� üũ
			TGuildRewardItemData* PrevGuildRewardItemData = g_pDataManager->GetGuildRewardItemData(pGuildRewardItemData->nCheckID);
			short nGuildWareSize = pGuild->GetWareSize();
			int nExtendWareSize = pGuildRewardItemData->nTypeParam1;
			if(PrevGuildRewardItemData)
			{
				nExtendWareSize = nExtendWareSize - PrevGuildRewardItemData->nTypeParam1;
			}
			if( nGuildWareSize + nExtendWareSize > GUILD_WAREHOUSE_MAX)
			{
				pSession->SendBuyGuildRewardItem(ERROR_ALREADY_MAXGUILDWARE, 0);
				return ERROR_ALREADY_MAXGUILDWARE;
			}
		}
		
		if( pGuildRewardItemData->nItemType ==  GUILDREWARDEFFECT_TYPE_GUILDITEM || 
			pGuildRewardItemData->nItemType ==  GUILDREWARDEFFECT_TYPE_MAKEGUILDMARK )
		{
			// ���� ġ���� ������
			nRet = pSession->GetItem()->ProcessBuyPrivateGuildRewardItem( pGuildRewardItemData, 1 );
		}
		else
		{
			// �����ü ȿ��
			nRet = pSession->GetItem()->ProcessBuyPublicGuildRewardItem( pGuild->GetGuildRewardItem(), pGuildRewardItemData, GuildUID.nDBID );
		}
	}
	if(nRet	!= ERROR_NONE )
	{
		pSession->SendBuyGuildRewardItem(nRet, 0);
	}
#endif
	return nRet;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


CDNGuildBase* CDNGuildSystem::CreateGuildInstance ()
{
#ifdef _VILLAGESERVER	
	return new CDNGuildVillage;
#elif _GAMESERVER
	return new CDNGuildGame;
#endif
}

#if defined(_DEBUG)
bool CDNGuildSystem::IsLock() const
{
	return(m_Lock.IsLock());
}
#endif	// _DEBUG

void CDNGuildSystem::InitAuth ()
{
	for (int iIndex = 0 ; GUILDROLE_TYPE_CNT > iIndex ; ++iIndex) 
	{
		m_AuthBase[iIndex].Reset();
		m_AuthPrmt[iIndex].Reset();
		m_AuthRstt[iIndex].Reset();
	}

	// ����
	// GUILDROLE_TYPE_MASTER �����
	m_AuthBase[GUILDROLE_TYPE_MASTER].Set(GUILDAUTH_TYPE_EXILE, 1);
	m_AuthBase[GUILDROLE_TYPE_MASTER].Set(GUILDAUTH_TYPE_NOTICE, 1);
	m_AuthBase[GUILDROLE_TYPE_MASTER].Set(GUILDAUTH_TYPE_INVITE, 1);
	m_AuthBase[GUILDROLE_TYPE_MASTER].Set(GUILDAUTH_TYPE_BATTLE, 1);
	m_AuthBase[GUILDROLE_TYPE_MASTER].Set(GUILDAUTH_TYPE_SKILL, 1);
	m_AuthBase[GUILDROLE_TYPE_MASTER].Set(GUILDAUTH_TYPE_STOREITEM, 1);
	m_AuthBase[GUILDROLE_TYPE_MASTER].Set(GUILDAUTH_TYPE_TAKEITEM, 1);
	m_AuthBase[GUILDROLE_TYPE_MASTER].Set(GUILDAUTH_TYPE_WITHDRAW, 1);
	m_AuthBase[GUILDROLE_TYPE_MASTER].Set(GUILDAUTH_TYPE_HOMEPAGE, 1);

	// GUILDROLE_TYPE_SUBMASTER ������
	m_AuthBase[GUILDROLE_TYPE_SUBMASTER].Set(GUILDAUTH_TYPE_EXILE, 1);
	m_AuthBase[GUILDROLE_TYPE_SUBMASTER].Set(GUILDAUTH_TYPE_NOTICE, 1);
	m_AuthBase[GUILDROLE_TYPE_SUBMASTER].Set(GUILDAUTH_TYPE_INVITE, 1);
	m_AuthBase[GUILDROLE_TYPE_SUBMASTER].Set(GUILDAUTH_TYPE_BATTLE, 1);
	m_AuthBase[GUILDROLE_TYPE_SUBMASTER].Set(GUILDAUTH_TYPE_SKILL, 1);
	m_AuthBase[GUILDROLE_TYPE_SUBMASTER].Set(GUILDAUTH_TYPE_STOREITEM, 1);
	m_AuthBase[GUILDROLE_TYPE_SUBMASTER].Set(GUILDAUTH_TYPE_TAKEITEM, 1);
	m_AuthBase[GUILDROLE_TYPE_SUBMASTER].Set(GUILDAUTH_TYPE_WITHDRAW, 1);

	// GUILDROLE_TYPE_SENIOR ���ӱ���
	m_AuthBase[GUILDROLE_TYPE_SENIOR].Set(GUILDAUTH_TYPE_INVITE, 1);
	m_AuthBase[GUILDROLE_TYPE_SENIOR].Set(GUILDAUTH_TYPE_SKILL, 1);
	m_AuthBase[GUILDROLE_TYPE_SENIOR].Set(GUILDAUTH_TYPE_STOREITEM, 1);
	m_AuthBase[GUILDROLE_TYPE_SENIOR].Set(GUILDAUTH_TYPE_TAKEITEM, 1);
	m_AuthBase[GUILDROLE_TYPE_SENIOR].Set(GUILDAUTH_TYPE_WITHDRAW, 1);

	// GUILDROLE_TYPE_REGULAR ���ı���
	m_AuthBase[GUILDROLE_TYPE_REGULAR].Set(GUILDAUTH_TYPE_SKILL, 1);
	m_AuthBase[GUILDROLE_TYPE_REGULAR].Set(GUILDAUTH_TYPE_STOREITEM, 1);
	m_AuthBase[GUILDROLE_TYPE_REGULAR].Set(GUILDAUTH_TYPE_TAKEITEM, 1);


	// �Ұ�

	// ������
#if !defined( PRE_ADD_CHANGEGUILDROLE )
	m_AuthRstt[GUILDROLE_TYPE_SUBMASTER].Set(GUILDAUTH_TYPE_EXILE, 1);
#endif

	// ���ӱ���
	m_AuthRstt[GUILDROLE_TYPE_SENIOR].Set(GUILDAUTH_TYPE_EXILE, 1);

	// ���ı���
	m_AuthRstt[GUILDROLE_TYPE_REGULAR].Set(GUILDAUTH_TYPE_EXILE, 1);
	m_AuthRstt[GUILDROLE_TYPE_REGULAR].Set(GUILDAUTH_TYPE_NOTICE, 1);
	m_AuthRstt[GUILDROLE_TYPE_REGULAR].Set(GUILDAUTH_TYPE_INVITE, 1);
	m_AuthRstt[GUILDROLE_TYPE_REGULAR].Set(GUILDAUTH_TYPE_BATTLE, 1);
	
	// ���Ա���
	m_AuthRstt[GUILDROLE_TYPE_JUNIOR].Set(GUILDAUTH_TYPE_EXILE, 1);
	m_AuthRstt[GUILDROLE_TYPE_JUNIOR].Set(GUILDAUTH_TYPE_NOTICE, 1);
	m_AuthRstt[GUILDROLE_TYPE_JUNIOR].Set(GUILDAUTH_TYPE_INVITE, 1);
	m_AuthRstt[GUILDROLE_TYPE_JUNIOR].Set(GUILDAUTH_TYPE_BATTLE, 1);
	m_AuthRstt[GUILDROLE_TYPE_JUNIOR].Set(GUILDAUTH_TYPE_SKILL, 1);
	m_AuthRstt[GUILDROLE_TYPE_JUNIOR].Set(GUILDAUTH_TYPE_STOREITEM, 1);
	m_AuthRstt[GUILDROLE_TYPE_JUNIOR].Set(GUILDAUTH_TYPE_TAKEITEM, 1);
	m_AuthRstt[GUILDROLE_TYPE_JUNIOR].Set(GUILDAUTH_TYPE_WITHDRAW, 1);
}
