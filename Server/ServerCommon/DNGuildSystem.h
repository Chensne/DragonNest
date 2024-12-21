#pragma once

#include "DNGuildBase.h"
#include "BitSetEx1.hpp"

typedef std::vector<class CDNGuildBase*, boost::pool_allocator<class CDNGuildBase*> > TVecGuilds;

class CDNGuildSystem
{
public:
	CDNGuildSystem ();
	~CDNGuildSystem ();

	DWORD				Open (DWORD dwSize = GUILDMNGRSIZE_DEF);
	void				Close ();
	bool				IsOpen () const {return m_bOpen;}
	DWORD				Release (CDNGuildBase* pGuild);
	CDNGuildBase*		At (const TGuildUID pGuildUID);
	const CDNGuildBase*	At (const TGuildUID pGuildUID) const;
	CDNGuildBase*		Get (const TGuild* pInfo);

	char		GetDefaultSize() const { return  m_DefaultSize; }

	// Sync
	CSyncLock*			GetLock () {return (&m_Lock);}

	// Copy List
	void				GetMainList (TVecGuilds& pList);
	void				GetCurrList (std::vector<TGuildUID, boost::pool_allocator<TGuildUID>>& pList);

	// Dismiss
	bool				AddDismiss (const TGuildUID pGuildUID);
	bool				IsDismissExist (const TGuildUID pGuildUID);

	// UPDATE
	void				DoUpdate (DWORD CurTick);

	// RECV FROM MA
	void				OnRecvMaDismissGuild (const MADismissGuild *pPacket);
	void				OnRecvMaAddGuildMember (const MAAddGuildMember *pPacket);
	void				OnRecvMaDelGuildMember (const MADelGuildMember *pPacket);
	void				OnRecvMaChangeGuildInfo (const MAChangeGuildInfo *pPacket);
	void				OnRecvMaChangeGuildMemberInfo (const MAChangeGuildMemberInfo *pPacket);
	void				OnRecvMaGuildChat (const MAGuildChat *pPacket);
#ifdef PRE_ADD_DOORS_GUILDCHAT_DISCONNECT
	void OnRecvMaDoorsGuildChat(const MADoorsGuildChat *pPacket);
#endif		//#ifdef PRE_ADD_DOORS_GUILDCHAT_DISCONNECT
	void				OnRecvMaMemberLevelUp (const MAGuildMemberLevelUp *pPacket);
	void				OnRecvMaGuildChangeName(const MAGuildChangeName *pPacket);
	void				OnRecvMaGuildChangeMark(const MAGuildChangeMark *pPacket);
	void				OnRecvMaGuildUpdateExp( const MAUpdateGuildExp* pPacket );
	void				OnRecvMaEnrollGuildWar(const MAEnrollGuildWar *pPacket);

	// RECV FROM CS
	void				OnRecvCsGetGuildInfo (CDNUserSession * pSession, const CSGetGuildInfo *pPacket);
	void				OnRecvCsGetGuildWareHistory (CDNUserSession * pSession, CSGetGuildWareHistory *pPacket);
	void				OnRecvCsCloseGuildWare (CDNUserSession * pSession);
	
	// FROM LUA API
	bool				OnRecvApiOpenGuildWare (CDNUserSession * pSession);

	// AUTH
	DWORD				GetAuthBase (eGuildRoleType pGuildRoleType) const;
	DWORD				GetAuthPrmt (eGuildRoleType pGuildRoleType) const;
	DWORD				GetAuthRstt (eGuildRoleType pGuildRoleType) const;
	const CBitSetEx1<GUILDAUTH_TYPE_CNT, WORD>& RefAuthBase (eGuildRoleType pGuildRoleType) const;
	const CBitSetEx1<GUILDAUTH_TYPE_CNT, WORD>& RefAuthPrmt (eGuildRoleType pGuildRoleType) const;
	const CBitSetEx1<GUILDAUTH_TYPE_CNT, WORD>& RefAuthRstt (eGuildRoleType pGuildRoleType) const;
	void				SetAuthBase (int pGuildRoleAuth[GUILDROLE_TYPE_CNT]) const;	// 길드 직급 세팅 (최초)
	void				SetAuthMust (int pGuildRoleAuth[GUILDROLE_TYPE_CNT]) const;	// 길드 직급 세팅 (허용/불가)
	void				SetRoleLimitation (TGuild* pInfo);

	// External Interface
	void				UpdateGuildResource (TAGetGuildInfo *pPacket);
	void				UpdateGuildMember(TAGetGuildMember *pPacket);
	void				OpenGuildWare (const TAGetGuildWareInfo *pPacket);

	int					GetResetWareHour () {return m_nResetWareHour;}
	void				MaChangePoint (CDNGuildBase* pGuild, const MAChangeGuildInfo *pPacket);

#ifdef PRE_ADD_CHANGEJOB_CASHITEM
	void				MaModifyMembJob (CDNGuildBase* pGuild, INT64 nCharacterDBID, int nChangeJob);
#endif

	void				UpdateGuildRewardItem( TAGetGuildRewardItem *pGuildRewardItem );	
	int					BuyGuildRewardItem( CDNUserSession * pSession, int nItemID );

private:
	CDNGuildBase*		CreateGuildInstance ();

	// RECV
	bool				PreRecvProcess (int nManagedID, TGuildUID pGuildUID, CDNGuildBase*& pGuild, bool bCheckManagdID = true);

	// RECV FROM MASTER SERVER
	// 길드정보 변경
	void				MaChangeNotification (CDNGuildBase* pGuild, const MAChangeGuildInfo *pPacket);
	void				MaChangeRoleAuth (CDNGuildBase* pGuild, const MAChangeGuildInfo *pPacket);
	void				MaChangeGuildWarFinalInfo(CDNGuildBase* pGuild, const MAChangeGuildInfo *pPacket);
#if defined( PRE_ADD_GUILD_EASYSYSTEM )
	void				MaChangeGuildHomePage(CDNGuildBase* pGuild, const MAChangeGuildInfo *pPacket);
#endif
	
	// 길드원 정보 변경
	void				MaModifyMembIntro (CDNGuildBase* pGuild, const MAChangeGuildMemberInfo* pPacket);
	void				MaModifyTypeRole (CDNGuildBase* pGuild, const MAChangeGuildMemberInfo* pPacket);
	void				MaDelegateGuildMaster (CDNGuildBase* pGuild, const MAChangeGuildMemberInfo* pPacket);
	void				MaChangeConnectState (CDNGuildBase* pGuild, const MAChangeGuildMemberInfo* pPacket);
	
	// 권한 초기화
	void				InitAuth ();

#if defined(_DEBUG)
	bool				IsLock() const;
#endif

private:
	bool				m_bOpen;				// 초기화 여부
	BYTE				m_DefaultSize;			// 기본 길드자원 사이즈
	
	// 길드자원 풀
	std::queue <class CDNGuildBase*> m_Pool;					
	
	// 전체 길드자원 목록
	TVecGuilds			m_MainList;							
	std::map <TGuildUID, class CDNGuildBase*> m_CurrList;	// 현재 운용중인 길드목록

	std::set <TGuildUID> m_DismList;	// 해체 길드목록 - 서버운용시 항상 유지

	CSyncLock			m_Lock;					

	CBitSetEx1<GUILDAUTH_TYPE_CNT, WORD>		m_AuthBase[GUILDROLE_TYPE_CNT];		// 길드 직급 별 기본 권한 (최초)
	CBitSetEx1<GUILDAUTH_TYPE_CNT, WORD>		m_AuthPrmt[GUILDROLE_TYPE_CNT];		// 길드 직급 별 기본 권한 (허용)
	CBitSetEx1<GUILDAUTH_TYPE_CNT, WORD>		m_AuthRstt[GUILDROLE_TYPE_CNT];		// 길드 직급 별 기본 권한 (불가)

	DWORD				m_TickDoUpdate;
	int					m_nResetWareHour;
};

extern CDNGuildSystem * g_pGuildManager;