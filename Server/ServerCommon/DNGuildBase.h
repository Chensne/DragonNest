#pragma once

#include "Access.h"
#include "DNGuildSystem.h"
#include "DNDBConnection.h"
#include "DNMasterConnection.h"
#include "DNUserBase.h"

#include "BitSetEx1.hpp"

class CDNGuildSystem;

class CDNGuildBase
{
public:
	CDNGuildBase();
	virtual ~CDNGuildBase();

	DWORD				Open(CDNGuildSystem* pManager);
	bool				IsOpen() const		{return m_bOpen;}
	void				Close();
	void				Release();
	virtual void		Reset();

	// TGuild
	void				Set(const TGuild* pInfo);
	bool				IsSet()			{return m_Info.IsSet();}
	const TGuildUID&	GetUID()			{return m_Info.GuildView.GuildUID;}

	// Access
	CSyncLock*			GetLock() const	{return m_Access.GetLock();}
#if !defined( PRE_ADD_NODELETEGUILD )
	void				Attach();
	void				Detach();
	void				DisableAttach();
	void				DisableAttachNoLock();
	void				DetachWork();
	bool				IsAttach() const;
	bool				IsEnable() const;
	DWORD				GetAccessCount() const;
#endif
	void				ResetAccess();				// 액세스 재활성화

	// INFO
	const TGuild*		GetInfo() const;
	TGuild*				SetInfo();

	// UPDATE
	void				DoUpdate(DWORD CurTick);

#if defined(_DEBUG)
	bool				IsLock() const;
	bool				m_bSkipDisableOnDebugging;			// 디버깅 중 길드 자원 해제 방지 여부
#endif	// _DEBUG

	void SetGuildInfoAuth(int pGuildRoleAuth[GUILDROLE_TYPE_CNT]) const;
	void SetGuildInfoAuth(eGuildRoleType pGuildRoleType, int pGuildAuth);
	void SetGuildInfoAuth(eGuildRoleType pGuildRoleType, eGuildAuthType pGuildAuthType);
	void ResetGuildInfoAuth(eGuildRoleType pGuildRoleType, eGuildAuthType pGuildAuthType);
	bool CheckGuildInfoAuth(eGuildRoleType pGuildRoleType, eGuildAuthType pGuildAuthType) const;
	short GetGuildMemberSize() const;

	// SEND
	virtual void		SendAddGuildMember(UINT nAccountDBID, INT64 nCharacterDBID, LPCWSTR lpwszCharacterName, TP_JOB nJob, char cLevel, __time64_t pJoinDate, TCommunityLocation* pLocation) = 0;
	virtual	void		SendDelGuildMember(UINT nAccountDBID, INT64 nCharacterDBID, bool bIsExiled) = 0;
	virtual void		SendChangeGuildInfo(UINT nAccountDBID, INT64 nCharacterDBID, BYTE btGuildUpdate, int iInt1, int iInt2, int iInt3, int iInt4, INT64 biInt64, LPCWSTR pText) {};
	virtual void		SendChangeGuildMemberInfo(UINT nReqAccountDBID, INT64 nReqCharacterDBID, UINT nChgAccountDBID, INT64 nChgCharacterDBID, BYTE btGuildMemberUpdate, int iInt1, int iInt2, INT64 biInt64, LPCWSTR pText) = 0;
	virtual void		SendGuildChat(UINT nAccountDBID, INT64 nCharacterDBID, LPCWSTR lpwszChatMsg, short nLen) = 0;
#ifdef PRE_ADD_DOORS_GUILDCHAT_DISCONNECT
	virtual void SendDoorsGuildChat(INT64 nCharacterDBID, LPCWSTR lpwszChatMsg, short nLen) = 0;
#endif		//#ifdef PRE_ADD_DOORS_GUILDCHAT_DISCONNECT
	virtual void		SendChangeGuildName(const wchar_t* wszGuildName) = 0;
	virtual void		SendChangeGuildMark(const MAGuildChangeMark *pPacket) = 0;
	virtual void		SendExtendGuildWare(short wTotalSize) {};
	virtual void		SendGuildLevelUp(int nLevel) = 0;
	virtual void		SendUpdateGuildExp(BYTE cPointType, int nPointValue, int nGuildExp, INT64 biCharacterDBID = 0, int nMissionID = 0) = 0;
	virtual void		SendGuildRewardItem( TGuildRewardItem* GuildRewardItem ) = 0;
	virtual void		SendAddGuildRewardItem( TGuildRewardItem GuildRewardItem ) = 0;
	virtual void		SendExtendGuildSize(short nGuildSize) = 0;

	// DB
	virtual void		ReqGetGuildWareInfo(bool bForceReq = false) {}

	// RECV
	virtual void		RecvCsCloseGuildWare(CDNUserSession * pSession) {};
	virtual void		RecvApiOpenGuildWare(CDNUserSession * pSession) {};
	virtual void		RecvCsGetGuildWareHistory(CDNUserSession * pSession, CSGetGuildWareHistory *pPacket) {};

	// RECV FROM DB
	void				DBRecv();

	// UPDATE
	void				UpdateMember(TAGetGuildMember *pPacket);
	void				UpdateGuildExp(const MAUpdateGuildExp* pPacket);

	virtual void		UpdateWarInfoOnAllMember(short wScheduleID, BYTE cTeamColorCode) = 0;
	virtual void		UpdateGuildInfo(CDNUserSession* pSession, const TAChangeGuildInfo *pPacket) {}
	virtual void		UpdateMemberInfo(CDNUserSession* pSession, const TAChangeGuildMemberInfo *pPacket) {}
	virtual void		OpenWare(const TAGetGuildWareInfo* pPacket) {}
	virtual void		UpdateWareSize(CDNUserBase* pUserObject, short wSize) {}

	// MEMEBER
	bool				AddMember(const TGuildMember* pGuildMember, TCommunityLocation* pLocation=NULL);
	bool				AddMemberAsync(const TGuildMember* pGuildMember, TCommunityLocation* pLocation=NULL);
	void				DelMember(INT64 nCharacterDBID);

	void				UpdateMemberLevel(INT64 nCharacterDBID, char cLevel);
	void				UpdateMemberName(INT64 nCharacterDBID, const WCHAR* wszName);

	INT64				UpdateMemberCoin(INT64 nCharacterDBID, INT64 nMoveCoin, __time64_t tUseDate);
	void				ResetMemberWareVersion(INT64 nCharacterDBID);

	bool				IsMemberExist(INT64 nCharacterDBID, TGuildMember* pGuildMember = NULL) const;
	bool				IsMemberAddable() const;
	
	int					GetMemberCount() const;

	void				GetMemberList(std::vector <std::pair<UINT, INT64>, boost::pool_allocator<UINT>>& pList) const;
	void				SetMemberList(TGuildMember *GuildMember);

	TGuildMember*		GetMemberInfo(INT64 nCharacterDBID);
	TGuildMember*		GetMemberInfoAsync(INT64 nCharacterDBID);

	// POINT
	bool				AddPoint(char cPointType, int nPointValue, INT64 biCharacterDBID = 0, int nMissionID = 0);
	int					GetExp()	{return m_Info.iTotalGuildExp;}
	
	CDNDBConnection*	GetDBCon() {return m_pDBCon;};
	BYTE				GetDBThreadID() {return m_cDBThreadID;}

	void				ResetCheckEmpty() {m_bCheckEmpty = false;}

	short				GetWareSize() {return m_Info.wGuildWareSize;}
	short				GetLevel()   {return m_Info.wGuildLevel;}
	

	virtual void		GuildWarReset();
	inline short		GetGuildWarScheduleID() { return m_wGuildWarScheduleID; };
	inline void			SetGuildWarScheduleID(short wScheduleID) { m_wGuildWarScheduleID = wScheduleID;};
	bool				IsEnrollGuildWar();
	void				SetGuildWarRewardResults(bool* pRewardResults );
	void				SetGuildWarRewardResultIndex(char cRewardType, bool bResult);
	bool				GetGuildWarRewardEnable(char cRewardType);	

	void				SetGuildRewardItem(TGuildRewardItem* GuildRewardItem);			// 길드보상아이템 적용 목록
	void				AddGuildRewardItem( int nItemID, __time64_t	tExpireDate, TGuildRewardItem &GuildRewardItem, CDNUserSession* pSession = NULL );		// 길드보상아이템 구입 적용
	TGuildRewardItem*	GetGuildRewardItem();		// 길드보상아이템 적용 목록
	void				SetGuildMaster(TGuildMember *GuildMaster) { m_GuildMaster = GuildMaster;};
	TGuildMember*		GetGuildMaster() { return m_GuildMaster; };

	void				SetRecvAllMember( bool bAllRecv ) {m_bRecvAllMember = bAllRecv;};	// 길드원목록 다 받았는지 체크
	bool				GetRecvAllMember() {return m_bRecvAllMember;};	// 길드원목록 다 받았는지 체크
	void CheckGuildInfo();
#if defined(PRE_ADD_GUILD_CONTRIBUTION)
	bool IsAddGuildExp(char cPointType, int nPointValue, OUT int& nAddPoint, OUT int& nLimitPoint);
#endif	// #if defined(PRE_ADD_GUILD_CONTRIBUTION)
protected:
	virtual void		SendGuildMemberLoginList(SCGuildMemberLoginList* pPacket) = 0;
	
	virtual bool		CheckGuildResource(TGuildMember* aGuildMemeber);

protected:
	TGuild				m_Info;					// 길드정보
	CDNDBConnection*	m_pDBCon;				// DB 커넥션
	BYTE				m_cDBThreadID;			// DB 쓰레드아이디
	std::map<INT64, TGuildMember>	m_MapTotalMember;		// 전체 길드원 정보 목록
	CDNGuildSystem*		m_pManager;				// 관리자 객체

private:
	bool				UpdateMemberState(TGuildMember* aGuildMember);

private:
	bool				m_bOpen;				// 길드객체 생성여부
	CAccess				m_Access;				// 접근참조 관리
	
	DWORD				m_TickCheckMember;		// 멤버체크
	bool				m_bCheckEmpty;			// 자원해제 체크
	short				m_wGuildWarScheduleID;			// 길드전 스케쥴ID	
	bool				m_bRewardResults[GUILDWAR_REWARD_GUILD_MAX];			// 저장된 길드보상 결과
	TGuildMember		*m_GuildMaster;			// 길드 마스터
	bool				m_bRecvAllMember;
};