#pragma once
#include "Task.h"
#include "MessageListener.h"
#include "DNProtocol.h"
#include "DNPacket.h"

#include "DnTableDB.h"

#include "DnBlindDlg.h"

// Ŭ���̾�Ʈ���� ������ ����������� Ǯ��������� �� �������� Ȱ��ȭ�ϼ���.(��帮���� �Ŀ��� ���밡���մϴ�.)
//#define PRE_DEBUG_GUILDAUTH

class CDnGuildDlg;

class CDnGuildTask : public CTask, public CTaskListener, public CSingleton<CDnGuildTask>, public CEtUICallback, public CLostDeviceProcess, public CDnBlindCallBack
{
public:
	enum
	{
		AUTO_REQUEST_GUILDINFO_FIRST = 30,				// SCEnter 30�� ��(����½�ũ���� ���� 30�� ��) ù��° ����û�� �Ѵ�.
		AUTO_REQUEST_GUILDINFO_DIALOG_SHOW_TRUE = 120,	// ù ��û �� ���̾�α� ���������� �����ֱ�
		AUTO_REQUEST_GUILDINFO_DIALOG_SHOW_FALSE = 300,	// ù ��û �� ���̾�α� �� ���������� �����ֱ�
		REQUEST_GUILDINFO_DELAY = 60,					// ���̾�α� �ѹ� ���� 1�� �Ŀ� �ٽ� ��û ���� �� �ִ�.
	};

	enum eGuildMemberSortType
	{
		GuildMemberSort_Name,
		GuildMemberSort_Level,
		GuildMemberSort_Job,
		GuildMemberSort_Role,
		GuildMemberSort_LastLogoutDate,

		GuildMemberSort_Amount = 5,
	};


	struct SGuildLevelInfo
	{
		int nLevel;
		int nReqGuildPoint;
		int nDailyGuildPointLimit;
		int nDailyMissionPointLimit;
	};

	CDnGuildTask(void);
	virtual ~CDnGuildTask(void);

	bool Initialize();
	void Finalize() {}

public:
	virtual void OnDisconnectTcp( bool bValidDisconnect ) {}
	virtual void OnDisconnectUdp( bool bValidDisconnect ) {}
	virtual void OnDispatchMessage( int nMainCmd, int nSubCmd, char *pData, int nSize );

	virtual void Process( LOCAL_TIME LocalTime, float fDelta );

	virtual void OnUICallbackProc( int nID, UINT nCommand, CEtUIControl *pControl, UINT uiMsg = 0 );

#ifdef PRE_ADD_BEGINNERGUILD
	virtual void OnBlindClosed();
#endif

public:
	void RequestCreateGuild( const WCHAR *wszGuildName );			// Note : ��� ����
	void RequestGetGuildInfo( bool bNeedMemeberList );				// Note : ��� ���� ��û
	void RequestDismissGuild();										// Note : ��� ����
	void RequestInviteGuildMember( const WCHAR *wszPlayerName );	// Note : ��� �ʴ�
	void RequestInviteGuildMemberAck( bool bAccept );				// Note : ��� �ʴ� ����
	void RequestLeaveGuild();										// Note : ��� Ż��
	void RequestExileGuild( UINT nAccountDBID, INT64 nCharacterDBID );
	void RequestChangeGuildInfo(BYTE btGuildUpdate, int iInt1 = 0, int iInt2 = 0, INT64 biInt64 = 0, LPCWSTR pText = NULL);
	void RequestChangeGuildMemberInfo(BYTE btGuildUpdate, int iInt1 = 0, int iInt2 = 0, INT64 biInt64 = 0, LPCWSTR pText = NULL,  UINT nChgAccountDBID = 0, INT64 nChgCharacterDBID = 0);
	void RequestGetGuildHistoryList(INT64 biIndex, bool bDirection);
	void RequestGetGuildStorageHistoryList(int nPage);
	void RequestChangeGuildInfoEx(BYTE btGuildUpdate, int iInt1 = 0, int iInt2 = 0, int iInt3 = 0, int iInt4 = 0, INT64 biInt64 = 0, LPCWSTR pText = NULL);

	void RequestGetGuildRecruitRegisterInfo();
#ifdef PRE_ADD_GUILD_EASYSYSTEM
	void RequestRegisterGuildRecruit( bool bModify, BYTE *pClassGrade, int nMinLevel, int nMaxLevel, const WCHAR *pGuildIntroduction, BYTE cPurposeCode, bool bCheckHomepage );
	void RequestCancelGuildRecruit();
	void RequestGetGuildRecruitList( int nPage, BYTE cPurposeCode, const WCHAR *pGuildName, BYTE cSortType );
#else
	void RequestRegisterGuildRecruit( bool bModify, BYTE *pClassGrade, int nMinLevel, int nMaxLevel, const WCHAR *pGuildIntroduction );
	void RequestCancelGuildRecruit();
	void RequestGetGuildRecruitList( int nPage );
#endif
	void RequestGetGuildRecruitRequestCount();
	void RequestGetGuildRecruitMyList();
	void RequestGetGuildRecruitCharacter();
	void RequestJoinGuild( TGuildUID GuildUID, bool bCancel = false );
	void RequestAcceptJoinGuild( bool bAccept, INT64 nCharacterDBID, const WCHAR *pToCharacterName );

	void RequestEnrollGuildWar();

	TGuild *GetGuildInfo() { return &m_Guild; }
	bool GetAuth( eGuildRoleType pGuildRoleType, eGuildAuthType pGuildAuthType );
	int GetGuildMemberCount() { return (int)m_vecGuildMember.size(); }
	int GetGuildMemberLogonCount();
	TGuildMember *GetGuildMemberFromArrayIndex( int nArrayIndex );
	TGuildMember *GetGuildMemberFromCharacterDBID( INT64 nDBID );
	TGuildMember *GetGuildMemberFromName( LPCWSTR wszName );
	TGuildMember *GetMyGuildMemberInfo();

	// ��� ä��
#ifdef PRE_ADD_DOORS_GUILDCHAT_DISCONNECT
	void OnRecvGuildChat( LPCWSTR wszMsg, INT64 nDBID, bool bIsDoorsMobile );
#else
	void OnRecvGuildChat( LPCWSTR wszMsg, INT64 nDBID );
#endif

	// �÷��̾ü�� ���� �ڽ��� ��������� ���´�.
	bool IsMaster();
	BYTE GetMyRole();

	// �� �� ��� ������
	int GetGuildCreateTex() { return m_nGuildCreateTax; }

	SGuildLevelInfo *GetGuildLevelInfo( int nGuildLevel );
	SGuildLevelInfo *GetGuildLevelInfoFromGuildExp( int nGuildExp );

	int GetMaxTakeItemByRole( eGuildRoleType eRole );	// �ش������� �����۲����� Ƚ�� �ε���
	int GetMaxTakeItemData( int nIndex );				// �ε����� ���� ������ ���̺� ������
	int GetMaxTakeItemIndexCount();						// ���̺� ������ �� ����
	int GetMaxWithDrawByRole( eGuildRoleType eRole );
	int GetMaxWithDrawData( int nIndex );
	int GetMaxWithDrawIndexCount();

	// ����� ���� 
	bool SortGuildMember( eGuildMemberSortType Type, bool bReverse );

	// ����� ���� ����. �������� ����ü�� ������ ������..
	void SetCurrentGuildWarEventStep( char cCurrentStep ) { m_cCurrentGuildWarEventStep = cCurrentStep; }
	void SetCurrentGuldWarScheduleID( short wCurrentScheduleID ) { m_wCurrentGuldWarScheduleID = wCurrentScheduleID; }	
	char GetCurrentGuildWarEventStep() { return m_cCurrentGuildWarEventStep; }
	short GetCurrentGuldWarScheduleID() { return m_wCurrentGuldWarScheduleID; }	

	// �Ŵ��� ���·� �ڵ� �����ִ°Ŵ� ������ �ʰ� �׳� ����Ͻø� �˴ϴ�.
	EtTextureHandle GetGuildMarkTexture( const TGuildView &View );
	bool IsShowGuildMark( const TGuildView &View );


	// �Ŵ��� ���·� �ڵ� �����ִ°Ŵ� ������ �ʰ� �׳� ����Ͻø� �˴ϴ�.
	EtTextureHandle GetGuildMarkTexture( const TGuildSelfView &View );
	bool IsShowGuildMark( const TGuildSelfView &View );

	bool IsNotRecruitMember() { return m_bNotRecruitMember; }

	bool IsHaveGuildInfo(){return m_bIsHaveGuildInfo;}
#ifdef PRE_ADD_BEGINNERGUILD
	bool IsBeginnerGuild() const;
	void MessageBoxForGraduateBeginnerGuild();
	bool CheckMessageBoxForGraduateBeginnerGuild() { return m_bMessageBoxForGraduateBeginnerGuild; }
#endif

	tstring GetGuildMasterName();

	virtual void OnLostDevice();
	virtual void OnResetDevice();

protected:
	void OnRecvGuildMessage(int nSubCmd, char *pData, int nSize);
	void OnRecvGuildRecruitMessage(int nSubCmd, char *pData, int nSize);

	void OnRecvCreateGuild( SCCreateGuild *pPacket );
	void OnRecvChangeGuildSelfView( SCChangeGuildSelfView *pPacket );
	void OnRecvGetGuildInfo( SCGetGuildInfo *pPacket );
	void OnRecvDismissGuild( SCDismissGuild *pPacket );
	void OnRecvInviteGuildMemberReq( SCInviteGuildMemberReq *pPacket );
	void OnRecvInviteGuildMemberAck( SCInviteGuildMemberAck *pPacket );
	void OnRecvLeaveGuild( SCLeaveGuildMember *pPacket );
	void OnRecvExileGuild( SCExileGuildMember *pPacket );
	void OnRecvChangeGuildInfo( SCChangeGuildInfo *pPacket );
	void OnRecvChangeGuildMemberInfo( SCChangeGuildMemberInfo *pPacket );
	void OnRecvGetGuildHistoryList( SCGetGuildHistoryList *pPacket );
	void OnRecvGuildMemberLoginList( SCGuildMemberLoginList *pPacket );
	void OnRecvOpenGuildWare( SCOpenGuildWare *pPacket );
	void OnRecvGetGuildWareHistoryList( SCGetGuildWareHistory *pPacket );
	void OnRecvExtendGuildWare( SCExtendGuildWare *pPacket );
	void OnRecvChangeGuildName( SCChangeGuildName *pPacket );
	void OnRecvEnrollGuildWar( SCEnrollGuildWar *pPacket );
	void OnRecvChangeGuildWarEvent( SCGuildWarEvent *pPacket );
	void OnRecvGuildWarTournamentWin( SCGuildWarTournamentWin* pPacket );
	void OnRecvUpdateGuildExp( SCUpdateGuildExp *pPacket );
	void OnRecvChangeGuildMark( SCChangeGuildMark *pPacket );

	void OnRecvGuildRecruitRegisterInfo( GuildRecruitSystem::SCGuildRecruitRegisterInfo *pPacket );
	void OnRecvGuildRecruitRegister( GuildRecruitSystem::SCGuildRecruitRegister *pPacket );
	void OnRecvGuildRecruitList( GuildRecruitSystem::SCGuildRecruitList *pPacket );
	void OnRecvGuildRecruitRequestCount( GuildRecruitSystem::SCGuildRecruitRequestCount *pPacket );
	void OnRecvGuildRecruitMyList( GuildRecruitSystem::SCMyGuildRecruitList *pPacket );
	void OnRecvGuildRecruitCharacter( GuildRecruitSystem::SCGuildRecruitCharacterList *pPacket );
	void OnRecvGuildRecruitRequest( GuildRecruitSystem::SCGuildRecruitRequest *pPacket );
	void OnRecvGuildRecruitAcceptResult( GuildRecruitSystem::SCGuildRecruitAccept *pPacket );
	void OnRecvGuildRecruitMemberResult( GuildRecruitSystem::SCGuildRecruitMemberResult *pPacket );

	void OnRecvGuildLevelUp( SCGuildLevelUp *pPacket );

	void OnRecvGuildGetRewardItem(SCGetGuildRewardItem* pPacket);
	void OnRecvGuildBuyRewardItem(SCBuyGuildRewardItem* pPacket);
	void OnRecvGuildAddRewardItem(SCAddGUildRewardItem* pPacket);
	void OnRecvGuildExtendGuildSize(SCExtendGuildSize* pPacket);
	void OnRecvPlayerGuildInfo(SCPlayerGuildInfo* pPacket);

	void RefreshGuildRewardInfo();
	void OnRecvGetGuildMember(SCGetGuildMember* pPacket);

	void SetGuildInfo( const TGuild &Guild );
	void SetGuildMemberInfo( int nMemberCount, int nPage, const TGuildMember *pMemberList );

	void AddGuildMember( TGuildMember *pMember );
	void DelGuildMember( INT64 nDBID );

	void RefreshGuildDlg( bool bRefreshShowing = false );	// â�� ������������ ��������ȣ���ҰŸ� true. �⺻�� ���� ��������.

	void OnChangeGuildWareSize();

	int GetCurrentGuildStorageSlotCount();

	EtTextureHandle GenerateGuildMarkTexture( int nIndex1, int nIndex2, int nIndex3 );

	TGuild m_Guild;
	std::vector<TGuildMember> m_vecGuildMember;

	float m_fAutoRequestDelayTime;	// �ڵ����� ��û�ϴ�
	float m_fDialogShowDelayTime;	// ���̾�α� ������ �ݾҴٸ� �ݺ��ص� ���� ������ �ΰ� ��û�ϵ��� �ð����� ���.

	TGuildUID m_InviteGuildUID;		// �ڽſ��� ����ʴ� ��û�� �� ���. �ӽ� ��ﺯ��.
	UINT m_InviteSESID;			// �ڽſ��� ����ʴ� ��û�� �� ���� ���Ǿ��̵�.
	UINT m_InviteACCID;
	int m_nGuildCreateTax;

	bool m_bIsHaveGuildInfo;

	std::vector<SGuildLevelInfo> m_vecGuildLevelInfo;

	std::map<int, int> m_mapMaxTakeItemInfo;
	std::map<int, int> m_mapMaxWithDrawInfo;

	char	m_cCurrentGuildWarEventStep;
	short	m_wCurrentGuldWarScheduleID;

	bool m_bNotRecruitMember;

#ifdef PRE_ADD_BEGINNERGUILD
	bool m_bMessageBoxForGraduateBeginnerGuild;
	bool m_bChangedGuildChat;
#endif

	int m_nMarkGenerateDelayFrameCount;
	struct SGuildMarkInfo
	{
		int nIconIndex[3];
		EtTextureHandle hTexture;
	};
	std::vector< SGuildMarkInfo > m_vecGuildMarkInfo;


protected:
	TGuildRewardItem m_GuildRewardItem[GUILDREWARDEFFECT_TYPE_CNT];	//������ ���� ���� ���� ������..

public:
	typedef std::list<GuildReward::GuildRewardInfo> GUILDREWARDINFO_LIST;
	typedef std::map<int, GUILDREWARDINFO_LIST> GUILDREWARD_MAP;
	GUILDREWARD_MAP m_GuildRewardList;

	bool IsAvailableRepurchase(const GuildReward::GuildRewardInfo &info);
	bool IsPrerequisite(const GuildReward::GuildRewardInfo &info);
	bool IsAppliedRewardByBetterThan(const GuildReward::GuildRewardInfo &info);	//�̹� �� ���� �༮���� ������ �Ǿ� �ִ���..
	bool IsPurchased(const GuildReward::GuildRewardInfo &info);
	bool IsAvailableReward(const GuildReward::GuildRewardInfo &info);
	__time64_t GetRemainPeriod(const GuildReward::GuildRewardInfo &info);
	int GetPurchasedItemID(const GuildReward::GuildRewardInfo &info);

// 	bool IsAppliedGuildRewardType(int rewardType);
// 	bool IsAppliedGuildRewardType(int rewardType, int rewardID);

	int CollectGuildRewardInfo(GUILDREWARDINFO_LIST & guildRewardList, TGuildRewardItem *pGuildReward = NULL);

	void RequestBuyGuildRewardItem(int nItemID);

	//��� ���� �� Ȯ��..
	int GetGuildRewardValue(GuildReward::GuildRewardType _type);
	//��� ���� ���� �Ⱓ
	__time64_t GetGuildRewardExpireDate(GuildReward::GuildRewardType _type);

	void RequestPlayerGuildInfo(int nSessionID);

	int GetGuildRewardEffectValue(int nType)
	{
		if (nType < 0 || nType >= GUILDREWARDEFFECT_TYPE_CNT )
			return 0;

		return m_GuildRewardItem[nType].nEffectValue;
	}

#ifdef PRE_ADD_GUILD_CONTRIBUTION
	void OnRecvGuildContributionPoint( GuildContribution::SCGuildContributionPoint* pPacket );
	void OnRecvGuildContributionRankList( GuildContribution::SCGuildContributionRank* pPacket );

	void RequsetGuildRankList();
#endif 
};

#define GetGuildTask()	CDnGuildTask::GetInstance()