#pragma once

#include "DNProtocol.h"
#include "DNUserData.h"
#include <math.h>

#if !defined( PRE_TEST_PACKETMODULE )
#if defined(_GAMESERVER) || defined(_VILLAGESERVER) || (defined(_WINDOWS) && !defined(_LAUNCHER))
#include "../EngineUtil/EtMathWrapperD3D.h"
#endif
#endif // #if !defined( PRE_TEST_PACKETMODULE )

#include "../../GameCommon/DNNotifierCommon.h"
#include "DNCommonDef.h"

#if defined(_SERVER)
#include "DNServerDef.h"
#endif
#pragma pack(push, 1)

//--------------------------------------------------------------
//                       ��Ŷ����ü (��� + ������)
//--------------------------------------------------------------

struct DNTPacketHeader
{
	unsigned short iLen;
	unsigned char cMainCmd;
	unsigned char cSubCmd;
};

struct DNTPacketCompHeader
{
	unsigned short nLen;
	unsigned char cSeq;
	unsigned char cMainCmd;
	unsigned char cSubCmd;
};

struct DNTPacket
{
	unsigned short iLen;
	unsigned char cMainCmd;
	unsigned char cSubCmd;
	char buf[1024*40];
};

struct DNTCompPacket
{
	DNTPacketCompHeader header;
	char buf[1024*40];
};

struct DNGAME_PACKET
{
	unsigned short datasize;
	unsigned char header;			//tcp packet�� inteface ������ ���Ͽ� ����� �ø��ϴ�. main_cmd�� ����
	unsigned char sub_header;	//sub_cmd�� ����
	unsigned short checksum;
	unsigned char seq;					//tcp udp seq
	char data[1024*4];
};

/* Security Association per Client */
struct ClientSA
{
    DWORD   _attr1;
    DWORD   _attr2;
};

struct DNEncryptPacketSeq
{
	USHORT nLen;
	BYTE cSeq;
	
	union
	{
		struct
		{
			char Buf[1024*50];
		};
		DNTPacket Packet;
	};
};

#if defined(_GAMESERVER)

//External PacketMessage Process (udp = Internal, tcp = external)(��� �ǹ̸�~)

struct _STORE_BUFFER_HEADER
{
	unsigned short	iBufSize;
	unsigned short	iSize;
	unsigned int nAccountDBID;
	int				iMainCmd;
	int				iSubCmd;
	BYTE cMsgType;				//eStoreExternalType
	USHORT nMasterID;
#if defined( PRE_WORLDCOMBINE_PARTY )
	int iWorldSetID;
#endif // #if defined( PRE_WORLDCOMBINE_PARTY )
	//bool			bIsDataMsg;
};

#endif

struct DNDoorsPacketHeader
{
	unsigned short datasize;
	unsigned short protocol;
};

struct DNDoorsPacket
{
	DNDoorsPacketHeader header;
	char data[8192 - sizeof(DNDoorsPacketHeader)];
};

//--------------------------------------------------------------
//				���������� ����(?) ��Ÿ ����ü
//--------------------------------------------------------------


struct TDatabaseVersion
{
	enum
	{
		MaxDatabaseNameLen = 128,
	};

	TDatabaseVersion()
	{
		iVersion = -1;
		memset( wszName, 0, sizeof(wszName) );
	}

	int		iVersion;
	WCHAR	wszName[MaxDatabaseNameLen];
};

struct SPartyMemberInfo
{
	UINT	nSessionID;
	int		nExp;
	BYTE	cLevel;
	BYTE	cPvPLevel;
	BYTE	cJobArray[JOBMAX];
	char	cClassID;
	int		nMapIndex;
	WCHAR	wszCharacterName[NAMELENMAX];
	int		nHairID;
	int		nFaceID;
	DWORD	dwHairColor;
	DWORD	dwEyeColor;
	DWORD	dwSkinColor;
	USHORT	wFatigue;
	USHORT	wStdRebirthCoin;
	USHORT	wCashRebirthCoin;
	int		nHelmetID;
	int		nEarringID;
	BYTE cMemberIndex;		//0������ ����
	UINT uiPvPUserState;
#if defined( PRE_WORLDCOMBINE_PARTY )
	int		nWorldSetID;				//������̵�
#endif
#if defined( PRE_ADD_NEWCOMEBACK )
	bool bCheckComebackAppellation;	
#endif
};

struct TKeySetting
{
	enum
	{
			Max1 = 29,
#if defined(PRE_ADD_SKILLSLOT_EXTEND)
			Max2 = 49,
#else	// #if defined(PRE_ADD_SKILLSLOT_EXTEND)
			Max2 = 39,
#endif	// #if defined(PRE_ADD_SKILLSLOT_EXTEND)
	};

	BYTE cVersion;
	BYTE arrKey1[Max1];
	BYTE arrKey2[Max2];

	TKeySetting()
	{
		cVersion = 0;
		memset( arrKey1, 0, Max1 );
		memset( arrKey2, 0, Max2 );
	}
};

struct TPadSetting
{
	enum
	{
#if defined(PRE_ADD_SKILLSLOT_EXTEND)
		Max = 50,
#else	// #if defined(PRE_ADD_SKILLSLOT_EXTEND)
		Max = 40,
#endif	// #if defined(PRE_ADD_SKILLSLOT_EXTEND)
	};

	BYTE cVersion;
	BYTE arrKey1[Max];
	BYTE arrKey2[Max];
	BYTE arrKey3[Max];
	BYTE arrKey4[Max];

	TPadSetting()
	{
		cVersion = 0;
		memset( arrKey1, 255, Max );
		memset( arrKey2, 255, Max );
		memset( arrKey3, 255, Max );
		memset( arrKey4, 255, Max );
	}
};

struct TSwapMemberIndex
{
	UINT nSessionID;
	BYTE cIndex;
};

//GameOptions 20����Ʈ
struct TGameOptions
{
	//��Ʈ����Ʈ ���� �ٷ� ����ϱ� ���Ͽ�....db�� ���ӵȸ޸� ���·� ������ �ǹǷ� ����� �������ּ���. ���̸� ��ġ ���Ŀ�.
	union
	{
		struct
		{
			//���� �ٲ��� �����ּ���~ �߰��� �ϴ�����
			bool bUserName : 1;		//�ٸ������̸�ǥ��
			bool bPartyUserName : 1;	//��Ƽ���̸�ǥ��
			bool bMonsterName : 1;		//�����̸�ǥ��
			bool bStateBarInsideCount : 1;	//���¹ټ���ǥ��
			bool bMyHPBar : 1;			//����hp��ǥ��
			bool bPartyHPBar : 1;		//��Ƽ hpbarǥ��
			bool bMonsterHPBar : 1;		//���͹�ǥ��
			bool bMyCombatInfo : 1;		//��������ǥ��
			bool bPartyCombatInfo : 1;	//��Ƽ����������ǥ��
			bool bDamageDirMelee : 1;	 // ���� �ǰݹ��� ǥ�� 
			bool bDamageDirRange : 1;	 // ���Ÿ� �ǰݹ��� ǥ�� 
			bool bGuildName : 1;		// ���� ǥ��
			bool bGameTip : 1;			// �ϴ� �����̵� ������ ǥ��
			bool bSimpleGuide : 1;		// ���� ���̵�
			bool bPVPLevelMark : 1;		// �ݷμ��� ��� ǥ��
			bool bSkillCoolTimeCounter : 1; // ��ų ���� �ð� ǥ��

			//�߰��� DISPLAYOPTIONCOUNT ���ڸ� �߰� ������ŭ �÷��ּ���.
		};
		char cDisplayOption[5];		//1bit*40  = 5Byte
	};

	union
	{
		struct
		{
			//���� �ٲ��� �����ּ���~ �߰��� �ϴ�����
			bool bDisplayGuildName : 1;		//�� ������ ���� �� �� �ִ���
			bool bDisplayGender : 1;		//�� ������ ���� �� �� �ִ���
			bool bDisplayLevel : 1;			//�� ������ ���� �� �� �ִ���

			//�߰��� COMMDISPLAYOPTIONCOUNT ���ڸ� �߰� ������ŭ �÷��ּ���.
		};
		char cCommunityDisplayOption[2];		//1bit*16  = 2byte
	};

	union
	{
		struct
		{
			//���� �ٲ��� �����ּ���~ �߰��� �ϴ�����
			//use eCommunityAcceptableOptionType
			BYTE cPartyInviteAcceptable : 2;
			BYTE cGuildInviteAcceptable : 2;
			BYTE cTradeRequestAcceptable : 2;
			BYTE cDuelRequestAcceptable : 2;
			BYTE cObserveStuffAcceptable : 2;
			BYTE cLadderInviteAcceptable : 2;
			BYTE cQuickPvPInviteAcceptable : 2;
			BYTE cPartyChatAcceptable : 2; // PRE_ADD_CHAT_RENEWAL

			//�߰��� COMMOPTIONCOUNT ���ڸ� �߰� ������ŭ �÷��ּ���.
		};
		char cCommunityOption[11];			//11byte
	};

	union
	{
		struct
		{
			BYTE cDisplayOptionCount : 4;
			BYTE cCommunityDisplayOptionCount : 4;
			BYTE cCommunityOptionCount : 4;
		};
		char cGameOptionCount[2];			//2byte
	};

	void SetDefault()
	{
		bUserName = true;
		bPartyUserName = true;
		bMonsterName = true;
		bStateBarInsideCount = true;
		bMyHPBar = false;
		bPartyHPBar = false;
		bMonsterHPBar = true;
		bMyCombatInfo = true;
		bPartyCombatInfo = false;
		bDamageDirMelee = true;
		bDamageDirRange = true;
		bGuildName = true;
		bGameTip = true;
		bSimpleGuide = true;
		bPVPLevelMark = true;
		bSkillCoolTimeCounter = false;

		bDisplayGuildName = true;
		bDisplayGender = false;
		bDisplayLevel = true;

		cPartyInviteAcceptable = _CA_OPTIONTYPE_ACCEPT;
		cGuildInviteAcceptable = _CA_OPTIONTYPE_ACCEPT;
		cTradeRequestAcceptable = _CA_OPTIONTYPE_ACCEPT;
		cDuelRequestAcceptable = _CA_OPTIONTYPE_ACCEPT;
		cObserveStuffAcceptable = _CA_OPTIONTYPE_ACCEPT;
		cLadderInviteAcceptable	= _CA_OPTIONTYPE_ACCEPT;
		cQuickPvPInviteAcceptable = _CA_OPTIONTYPE_ACCEPT;
		cPartyChatAcceptable = _CA_OPTIONTYPE_ACCEPT;

		cDisplayOptionCount = DISPLAYOPTIONCOUNT;
		cCommunityDisplayOptionCount = COMMDISPLAYOPTIONCOUNT;
		cCommunityOptionCount = COMMOPTIONCOUNT;
	}
};

struct TProfile
{
	BYTE	cGender;
	WCHAR	wszGreeting[PROFILEGREETINGLEN+1];
	bool	bOpenPublic;
};

struct TItemInfo
{
	BYTE cSlotIndex;
	TItem Item;
};

struct TRepurchaseItemInfo
{
	int iRepurchaseID;
	__time64_t tSellDate;
	int iSellPrice;
	TItem Item;
};

struct TGuildWareSlot
{
	BYTE cSlotIndex;
	INT64 nSerial;		// �ø���
};

struct TGuildWareHistory
{
	int			nIndex;				// �ε���
	WCHAR		wszCharacterName[NAMELENMAX];		// ĳ���͸�
	BYTE		cInOut;								// ��/��
	short		wCount;								// ���� 0:Coin / +1:ItemCode
	int			nContent;							// ���� cCount(����)�� ���� �뵵�� �޶�����
	__time64_t	EventDate;							// �̺�Ʈ��¥
};


struct TEffectItemInfo
{
	int			iItemID;
	bool		bEternityFlag;
	__time64_t	ExpireDate;
	int			iProperty[5];
	INT64		biItemSerial;
};

struct TQuestItemCnt			// �κ��丮�� ������ ��� �߰� ���ɿ��θ� Ȯ���ϱ� ���� �ӽ÷� ������ �������� ����
{
public:
	int nItemID;
	short wCount;		// ����

public:
	void Set(int _nItemID, short _wCount)
	{
		nItemID = _nItemID;
		wCount = _wCount;
	}

	void Reset()
	{
		nItemID = 0;
		wCount = 0;
	}
};

struct TInvenItemCnt			// �κ��丮�� ������ ��� �߰� ���ɿ��θ� Ȯ���ϱ� ���� �ӽ÷� ������ �������� ����
{
public:
	int nItemID;
	short wCount;		// ����
	bool bSoulbound;
	char cSealCount;

public:
	void Set(int _nItemID, short _wCount, bool _bSoulbound, char _cSealCount )
	{
		nItemID = _nItemID;
		wCount = _wCount;
		bSoulbound = _bSoulbound;
		cSealCount = _cSealCount;
	}

	void Reset()
	{
		nItemID = 0;
		wCount = 0;
		bSoulbound = false;
		cSealCount = 0;
	}
};

struct TCommunityLocation
{
public:
	BYTE cServerLocation;	// eLocationState
	int nChannelID;			// �����ϰ�� 0 �Ǵ� -1
	int nMapIdx;			// �� ��� -1
public:
	TCommunityLocation()
	{
		Reset();
	}

	void Reset()
	{
		cServerLocation = _LOCATION_NONE;
		nChannelID = 0;
		nMapIdx = 0;
	}
	bool IsSet() const 
	{
		return(_LOCATION_NONE != cServerLocation);
	}
};

struct TCommunityLocationEx:public TCommunityLocation
{
public:
	INT64 biCharacterDBID;

	void Reset( INT64 CharDBID )
	{
		biCharacterDBID = CharDBID;
		TCommunityLocation::Reset();
	}
};

struct TGuildUID	// ��� UID
{
public:
	TGuildUID()
	{
		Reset();
	}
	TGuildUID(const TGuildUID& pRhv)
	{
		ui64UID = pRhv.ui64UID;
	}
	TGuildUID(int nWorldID, UINT nDBID)
	{
		Set(nWorldID, nDBID);
	}
	void Set(int nSetWorldID, UINT nSetDBID)
	{
		nWorldID = nSetWorldID;
		nDBID = nSetDBID;
	}
	void Reset()
	{
		ui64UID = GUILDUID_DEF;
	}
	bool IsSet() const
	{
		return(GUILDUID_DEF != ui64UID);
	}
	operator UINT64() const
	{
		return ui64UID;
	}
	bool operator==(const TGuildUID& pRhv) const	{ return(ui64UID == pRhv.ui64UID); }
	bool operator!=(const TGuildUID& pRhv) const	{ return(ui64UID != pRhv.ui64UID); }
	bool operator==(UINT64 pUid) const			{ return(ui64UID == pUid); }
	bool operator!=(UINT64 pUid) const			{ return(ui64UID != pUid); }
	bool operator<(const TGuildUID& pRhv) const		{ return(ui64UID < pRhv.ui64UID); }		// for stdext::hash_map��.

private:
	union {
		UINT64 ui64UID;		// ��� UID
		struct {
			int nWorldID;			// ���� ID
			UINT nDBID;	// ��� DBID
		};
	};
};

struct TGuildView	// ��� �ð�����
{
public:
	TGuildView()
	{
		Reset();
	}
	void Set(const TGuildView& pGuildView)
	{
		Set(pGuildView.GuildUID, pGuildView.wszGuildName);

		wWarSchduleID = pGuildView.wWarSchduleID;
		cTeamColorCode = pGuildView.cTeamColorCode;	

		wGuildMark = pGuildView.wGuildMark;
		wGuildMarkBG = pGuildView.wGuildMarkBG;
		wGuildMarkBorder = pGuildView.wGuildMarkBorder;
	}
	void Set(const TGuildUID pGuildUID, LPCWSTR lpwszGuildName)
	{
		GuildUID = pGuildUID;
		if (lpwszGuildName) 
			::wcsncpy_s(wszGuildName, _countof(wszGuildName), lpwszGuildName, _countof(wszGuildName));
		else 
			wszGuildName[0] = L'\0';		
	}
	void Reset()
	{
		GuildUID.Reset();
		wszGuildName[0] = L'\0';
		wWarSchduleID = 0;
		cTeamColorCode = 0;
		wGuildMark = 0;
		wGuildMarkBG = 0;
		wGuildMarkBorder = 0;
	}
	bool IsSet() const
	{
		return(GuildUID.IsSet());
	}
	bool operator==(const TGuildView& pRhv) const
	{
		return((pRhv.GuildUID == GuildUID));
	}
	bool operator!=(const TGuildView& pRhv) const
	{
		return(!(this->operator==(pRhv)));
	}
public:
	TGuildUID GuildUID;						// ��� UID (���� ID + ��� DBID)
	WCHAR wszGuildName[GUILDNAME_MAX];		// ��� �̸� (�ϴ� ��� â�� ���Ŀ��� �̸��� ������� �ʴ°��� ������ ��)
	short wWarSchduleID;
	BYTE cTeamColorCode;					// 0:���� / 1:û�� / 2:ȫ��
	short wGuildMark;						// ��帶ũ ����
	short wGuildMarkBG;						// ��帶ũ ���
	short wGuildMarkBorder;					// ��帶ũ �׵θ�
};

struct TGuildSelfView : public TGuildView	// ��� �ð����� (����)		// P.S.> ���� ���߻�� �ϸ� �ʵ�
{
public:
	TGuildSelfView()
	{
		Reset();
	}
	TGuildSelfView(const TGuildView& pGuildView, BYTE btGuildRole)
	{
		Set(pGuildView, btGuildRole);
	}
	TGuildSelfView(const TGuildSelfView& pGuildSelfView)
	{
		Set(pGuildSelfView);
	}
	void Reset()
	{
		btGuildRole = GUILDROLE_TYPE_CNT;
		TGuildView::Reset();
	}
	void Set(const TGuildView& pGuildView, BYTE cGuildRole)
	{
		btGuildRole = cGuildRole;
		TGuildView::Set(pGuildView);
	}	
	void Set(const TGuildSelfView& pGuildSelfView)
	{
		btGuildRole = pGuildSelfView.btGuildRole;
		TGuildView::Set(pGuildSelfView);
	}	
	bool operator==(const TGuildSelfView& pRhv) const
	{
		// ��� �� �� �θ� Ŭ���� (��� ��ü) ����� ���� ?
		return(TGuildView::operator==(pRhv) && pRhv.btGuildRole == btGuildRole);
	}
	bool operator!=(const TGuildSelfView& pRhv) const
	{
		return(!(this->operator==(pRhv)));
	}
public:
	BYTE btGuildRole;	// ��� ��å
};

struct TGuildRewardItem
{
	int			nItemID;		// �����۾��̵�	
	int			nItemType;		// Ÿ��
	short		    nEffectValue;	// ȿ����
#if defined( PRE_ADD_GUILDREWARDITEM )
	int		    nEffectValue2;	// ȿ����2
#endif
	bool		bEternity;		// ���� ����
	__time64_t	m_tExpireDate;	// �Ⱓ
};

struct TGuild		// ��� ����
{
public:
	TGuild()
	{
		Reset();
	}
	void Reset()
	{
		GuildView.Reset();
		ui64GuildMoney = 0;
		wGuildLevel = 1;
		wszGuildNotice[0] = L'\0';
		::memset(iGuildMedal, 0, sizeof(iGuildMedal));
		wGuildSize = 0;
		wGuildWareSize = 0;
		::memset(&OpenDate, 0, sizeof(OpenDate));
		::memset(wszGuildRoleName, 0, sizeof(wszGuildRoleName));
		::memset(wGuildRoleAuth, 0, sizeof(wGuildRoleAuth));
		::memset(nRoleMaxItem, 0, sizeof(nRoleMaxItem));
		::memset(nRoleMaxCoin, 0, sizeof(nRoleMaxCoin));
		nGuildWarFinalCount = 0;
		nGuildWarFinalWinCount = 0;
		iTotalGuildExp = 0;
		iDailyStagePoint = 0;
		iDailyMissionPoint = 0;
		iWarPoint = 0;
		::memset(GuildRewardItem, 0, sizeof(GuildRewardItem));
#if defined( PRE_ADD_GUILD_EASYSYSTEM )
		::memset(wszGuildHomePage, 0, sizeof(wszGuildHomePage));
#endif
	}
	bool IsSet() const
	{
		return(GuildView.IsSet());
	}
	bool CheckAuth(eGuildRoleType pGuildRoleType, eGuildAuthType pGuildAuthType) const
	{
		if (wGuildRoleAuth[pGuildRoleType] & (static_cast<int>(1) << pGuildAuthType)) {
			return true;
		}		
		return false;
	}

public:
	TGuildView GuildView;							// ��� �ð�����
	INT64 ui64GuildMoney;							// ��� �ڱ�
	short wGuildLevel;								// ��� ����
	WCHAR wszGuildNotice[GUILDNOTICE_MAX+1];		// ��� ����
	int iGuildMedal[GUILDMEDAL_TYPE_CNT];			// ��� �޴�
	short wGuildSize;								// ��� �߰�/��ü �����ο�
	short wGuildWareSize;							// ��� â�� �ִ��
	__time64_t OpenDate;							// ��� â����
	WCHAR wszGuildRoleName[GUILDROLE_TYPE_CNT][GUILDROLENAME_MAX+1];// ��� ���Һ� �̸�
	int wGuildRoleAuth[GUILDROLE_TYPE_CNT];			// ��� ���Һ� ����
	INT nRoleMaxItem[GUILDROLE_TYPE_CNT];			// ��� ���Һ� â�� ������ Ƚ������ (�ε���)
	INT nRoleMaxCoin[GUILDROLE_TYPE_CNT];			// ��� ���Һ� �ִ� ��ݾ� (�ε���)
	short nGuildWarFinalCount;						// ����� ���� ���� Ƚ��
	short nGuildWarFinalWinCount;					// ����� ���� ��� Ƚ��
	int iTotalGuildExp;								// ��� ����ġ
	int iDailyStagePoint;							// �������� ����Ʈ
	int iDailyMissionPoint;							// �̼� ����Ʈ
	int iWarPoint;									// ����� ����Ʈ
	TGuildRewardItem GuildRewardItem[GUILDREWARDEFFECT_TYPE_CNT];	// ��庸�� ���� ȿ��
	WCHAR wszGuildHomePage[GUILDHISTORYTEXT_MAX+1];		// ��� Ȩ������
	char cGuildType;							// ���� BeginnerGuild
};

struct TGuildMember		// ���� ����
{
public:
	TGuildMember()
	{
		Reset();
	}
	void Set(UINT nMemberADBID, INT64 nMemberCDBID, LPCWSTR lpwszMemberCharacterName, char cMemberJob, char cMemberLevel, BYTE cMemberGuildRole, __time64_t tMemberJoinDate = 0, long nMemberUseGuildCommonPoint = 0, 
		long nMemberTotGuildCommonPoint = 0, LPCWSTR lpwszGuildMemberIntroduce = NULL, __time64_t tLastLogoutDate = 0, BYTE cMemberServerLocation = 0, int nMemberChannelID = 0, int nMemberMapIdx = 0)
	{
		nAccountDBID = nMemberADBID;
		nCharacterDBID = nMemberCDBID;
		::wcsncpy_s(wszCharacterName, _countof(wszCharacterName), lpwszMemberCharacterName, _countof(wszCharacterName));
		nJob = cMemberJob;
		cLevel = cMemberLevel;
		btGuildRole = cMemberGuildRole;
		JoinDate = tMemberJoinDate;
		iUseGuildCommonPoint = nMemberUseGuildCommonPoint;
		iTotGuildCommonPoint = nMemberTotGuildCommonPoint;
		if (lpwszGuildMemberIntroduce) {
			::wcsncpy_s(wszGuildMemberIntroduce, _countof(wszGuildMemberIntroduce), lpwszGuildMemberIntroduce, _countof(wszGuildMemberIntroduce));
		}
		LastLogoutDate = tLastLogoutDate;
		Location.cServerLocation = cMemberServerLocation;
		Location.nChannelID = nMemberChannelID;
		Location.nMapIdx = nMemberMapIdx;
	}
	void Reset()
	{
		nCharacterDBID = 0;
		btGuildRole = GUILDROLE_TYPE_CNT;
		iUseGuildCommonPoint = 0;
		iTotGuildCommonPoint = 0;
		wszGuildMemberIntroduce[0] = L'\0';
		JoinDate = 0;

		nAccountDBID = 0;
		wszCharacterName[0] = L'\0';
		nJob = 0;
		cLevel = 0;
		LastLogoutDate = 0;

		Location.cServerLocation = _LOCATION_NONE;
		Location.nChannelID = 0;
		Location.nMapIdx = 0;

		nDelMembNo = 0;

		TakeWareItemCount = 0;
		WithdrawCoin = 0;

		WareVeriosnDate = 0;
		bOpenWareStatus = false;

		LastWareUseDate = 0;
	}
	bool IsSet() const
	{
		return(0 != nCharacterDBID);
	}
public:
	// ���� DB ����
	INT64 nCharacterDBID;					// ĳ���� DBID
	BYTE btGuildRole;				// ��� ����
	long iUseGuildCommonPoint;	// ��� �Ϲ� ����Ʈ (���)
	long iTotGuildCommonPoint;	// ��� �Ϲ� ����Ʈ (�⿩)
	WCHAR wszGuildMemberIntroduce[GUILDMEMBINTRODUCE_MAX+1];	// ���� �ڱ�Ұ�
	__time64_t JoinDate;				// ��� ���� ��¥

	// ĳ���� DB ����
	UINT nAccountDBID;					// ���� DBID
	WCHAR wszCharacterName[NAMELENMAX];	// ĳ���� �̸�
	TP_JOB nJob;							// ��������
	CHAR cLevel;							// ����
	__time64_t LastLogoutDate;			// ���� �α׾ƿ� ��¥

	// ĳ���� �ǽð� ����
	TCommunityLocation Location;			// ���� ��ġ (�α��� ���ε� �� ����� ���·� üũ)

	int TakeWareItemCount;				// ���� �������� ������ Ƚ��
	INT64 WithdrawCoin;					// ���� ������ �ݾ�

	__time64_t WareVeriosnDate;			// â�� ���� ����
	bool bOpenWareStatus;					// â�� ���� ����

	__time64_t LastWareUseDate;			// â�� ���������� ����� ��¥

	// ���� ����
	BYTE nDelMembNo;			// ĳ���� ���� ������ȣ
};

struct TGuildHistory	// ��� �����丮 ���� (�ν��Ͻ�)
{
	INT64 biIndex;		// ��� �����丮 �ε���
	__time64_t RegDate;				// ��� �����丮 �����
	BYTE btHistoryType;	// ��� �����丮 Ÿ�� (eGuildHistoryType)
	int nInt1;
	int nInt2;
	INT64 biInt64;
	WCHAR Text[GUILDHISTORYTEXT_MAX+1];
};

// �������Խ��� ��� �������
struct TGuildRecruitInfo
{
	TGuildUID GuildUID;										// ��� UID (���� ID + ��� DBID)
	WCHAR wszGuildName[GUILDNAME_MAX];					// ��� �̸�
	short wGuildMark;										// ��帶ũ ����
	short wGuildMarkBG;										// ��帶ũ ���
	short wGuildMarkBorder;									// ��帶ũ �׵θ�
	
	WCHAR wszGuildRecruitNotice[GUILDRECRUITINTRODUCE+1];	// �������Խ��� �Ұ���	
	short wGuildLevel;										// ��� ����
	short wGuildMemberPresentSize;							// ���� ���� ��	
	short wGuildMemberMaxSize;								// �ִ� ���� ��
#if defined( PRE_ADD_GUILD_EASYSYSTEM )
	BYTE		cPurposeCode;								// ��� ����(1:ģ�� 2:���� 3:�׽�Ʈ 4:�ݷμ���
	WCHAR		wszGuildHomePage[GUILDHISTORYTEXT_MAX+1];		//��� Ȩ������
	WCHAR		wszGuildMasterName[NAMELENMAX+1];			// ��� �����͸�
#endif
};
// �������Խ����� ���ؼ� ��尡�Խ�û�� �� �������
struct TGuildRecruitCharacter 
{
	INT64 nCharacterDBID;					// ĳ���� DBID	
	WCHAR wszCharacterName[NAMELENMAX+1];		// ĳ���� �̸�
	TP_JOB nJob;							// ��������
	BYTE cLevel;							// ����
};

#if defined( PRE_ADD_NPC_REPUTATION_SYSTEM )

struct TNpcReputation
{
	int					iNpcID;
	int					iFavorPoint;
	int					iMalicePoint;

	TNpcReputation()
		:iNpcID(0),iFavorPoint(0),iMalicePoint(0)
	{
	}
	TNpcReputation( int NpcID, int FavorPoint, int MalicePoint )
		:iNpcID(NpcID),iFavorPoint(FavorPoint),iMalicePoint(MalicePoint)
	{
	}

	void Set( int NpcID, int FavorPoint, int MalicePoint )
	{
		iNpcID			= NpcID;
		iFavorPoint		= FavorPoint;
		iMalicePoint	= MalicePoint;
	}
};
#endif // #if defined( PRE_ADD_NPC_REPUTATION_SYSTEM )

struct TUnionReputePointInfo
{
	INT64	iUnionPoint[NpcReputation::UnionType::Etc];

	void SetUnionReputePoint( BYTE cType, INT64 iPoint )
	{
		if (NpcReputation::UnionType::Commercial > cType || cType >= NpcReputation::UnionType::Etc)
			return;

		iUnionPoint[cType] = iPoint;
	}

	INT64	GetUnionReputePoint( BYTE cType ) const
	{
		if (NpcReputation::UnionType::Commercial > cType || cType >= NpcReputation::UnionType::Etc)
			return 0;

		return iUnionPoint[cType];
	}

	bool DelUnionReputePoint (BYTE cType, INT64 iPoint)
	{
		if (NpcReputation::UnionType::Commercial > cType || cType >= NpcReputation::UnionType::Etc)
			return false;

		if (iUnionPoint[cType] < iPoint)
			return false;

		iUnionPoint[cType] -= iPoint;

		return true;
	}
};

struct SCUnionPoint
{
	INT64 biUnionPoint[NpcReputation::UnionType::Etc];
};

struct SCUnionMembershipList		// SC_REPUTATION / SC_UNIONMEMBERSHIPLIST
{	
	int nItemID[NpcReputation::UnionType::Etc];
	int nLeftUseTime[NpcReputation::UnionType::Etc];
};

struct SCBuyUnionMembershipResult		// SC_REPUTATION / SC_REFRESHUNIONMEMBERSHIP
{
	BYTE cType;
	int nItemID;
	int nLeftUseTime; // ���� ���ð� (�д���)
};

struct TMasterInfo
{
	INT64	biCharacterDBID;
	WCHAR	wszCharName[NAMELENMAX];
	BYTE	cLevel;
	BYTE	cJobCode;
	BYTE	cGenderCode;
	int		iRespectPoint;
	WCHAR	wszSelfIntrodution[MasterSystem::Max::SelfIntrotuctionLen];
	int		iGraduateCount;
};

struct TPupilInfo
{
	INT64		biCharacterDBID;
	WCHAR		wszCharName[NAMELENMAX];
	BYTE		cLevel;
	BYTE		cJob;
	int			iFavorPoint;
	__time64_t	tLastConnectDate;

	TPupilInfo()
	{
		biCharacterDBID		= 0;
		cLevel				= 0;
		cJob				= 0;
		iFavorPoint			= 0;
		tLastConnectDate	= 0;
		memset( wszCharName, 0, sizeof(wszCharName) );
	}
};

struct TPupilInfoWithLocation:public TPupilInfo
{
	TCommunityLocation	Location;

	TPupilInfoWithLocation()
	{
		Location.Reset();
	}

	TPupilInfoWithLocation( TPupilInfo& data )
	{
		Location.Reset();

		biCharacterDBID		= data.biCharacterDBID;
		cLevel				= data.cLevel;
		cJob				= data.cJob;
		iFavorPoint			= data.iFavorPoint;
		tLastConnectDate	= data.tLastConnectDate;
		memcpy( wszCharName, data.wszCharName, sizeof(wszCharName) );
	}
};

struct TMasterAndClassmateInfo
{
	MasterSystem::MasterClassmateKind::eCode	Code;
	INT64										biCharacterDBID;
	WCHAR										wszCharName[NAMELENMAX];
	__time64_t									tLastConnectDate;

	TMasterAndClassmateInfo()
	{
		Code				= MasterSystem::MasterClassmateKind::Max;
		biCharacterDBID		= 0;
		tLastConnectDate	= 0;
		memset( wszCharName, 0, sizeof(wszCharName) );
	}
};

struct TMasterAndClassmateInfoWithLocation:public TMasterAndClassmateInfo
{
	TCommunityLocation	Location;

	TMasterAndClassmateInfoWithLocation()
	{
		Location.Reset();
	}

	TMasterAndClassmateInfoWithLocation( TMasterAndClassmateInfo& data )
	{
		Location.Reset();

		Code				= data.Code;
		biCharacterDBID		= data.biCharacterDBID;
		tLastConnectDate	= data.tLastConnectDate;
		memcpy( wszCharName, data.wszCharName, sizeof(wszCharName) );
	}
};

struct TClassmateInfo
{
	INT64	biCharacterDBID;
	WCHAR	wszCharName[NAMELENMAX];
	BYTE	cJob;
	BYTE	cLevel;
	BYTE	cMasterCount;
	WCHAR	wszMasterCharName[MasterSystem::Max::MasterCount][NAMELENMAX];

	TClassmateInfo()
	{
		biCharacterDBID = 0;
		cJob			= 0;
		cLevel			= 0;
		cMasterCount	= 0;
		memset( wszCharName, 0, sizeof(wszCharName) );
		memset( wszMasterCharName, 0, sizeof(wszMasterCharName) );
	}
};

struct TClassmateInfoWithLocation:public TClassmateInfo
{
	TCommunityLocation Location;

	TClassmateInfoWithLocation()
	{
		Location.Reset();
	}

	TClassmateInfoWithLocation( TClassmateInfo& data )
	{
		Location.Reset();

		biCharacterDBID = data.biCharacterDBID;
		cJob			= data.cJob;
		cLevel			= data.cLevel;
		cMasterCount	= data.cMasterCount;
		memcpy( wszCharName, data.wszCharName, sizeof(wszCharName) );
		for( UINT i=0 ; i<cMasterCount ; ++i )
			memcpy( wszMasterCharName[i], data.wszMasterCharName[i], sizeof(wszMasterCharName[i]) );
	}
};

struct TMasterCharacterInfo
{
	bool	bMasterListShowFlag;
	WCHAR	wszSelfIntroduction[MasterSystem::Max::SelfIntrotuctionLen];
	int		iGraduateCount;
	int		iRespectPoint;
};

struct TMyMasterInfo
{
	INT64	biCharacterDBID;
	WCHAR	wszCharName[NAMELENMAX];
	BYTE	cJob;
	BYTE	cLevel;
	int		iRespectPoint;
	int		iFavorPoint;

	TMyMasterInfo()
	{
		biCharacterDBID = 0;
		cJob			= 0;
		cLevel			= 0;
		iRespectPoint	= 0;
		iFavorPoint		= 0;
		memset( wszCharName, 0, sizeof(wszCharName) );
	}
};

struct TMyMasterInfoWithLocation:public TMyMasterInfo
{
	TCommunityLocation Location;

	TMyMasterInfoWithLocation()
	{
		Location.Reset();
	}

	TMyMasterInfoWithLocation( TMyMasterInfo& data )
	{
		Location.Reset();

		biCharacterDBID	= data.biCharacterDBID;
		cJob			= data.cJob;
		cLevel			= data.cLevel;
		iRespectPoint	= data.iRespectPoint;
		iFavorPoint		= data.iFavorPoint;
		memcpy( wszCharName, data.wszCharName, sizeof(wszCharName) );
	}
};

struct TMasterAndFavorPoint
{
	INT64	biCharacterDBID;
	int		iFavorPoint;
	WCHAR	wszCharName[NAMELENMAX];
};

struct TVehicleBase
{
	DWORD dwPartsColor1;	// Vehicle:Hair, Pet:Body
	DWORD dwPartsColor2;	// Vehicle:����, Pet:Nose
	int nExp;
	int nRange;				// �� �ݱ����
	WCHAR	wszNickName[NAMELENMAX];
	bool bSkillSlot;	// �� ��ų ���� Ȯ�忩��
	int nSkillID1;	// ��ų ID 1��
	int nSkillID2;	// ��ų ID 2��
	int nCurrentSatiety;
	int nType;
	DWORD dwLastHungerTick;
};

struct TVehicle: public TVehicleBase
{
	TItem Vehicle[Vehicle::Slot::Max];
};

struct TPartyVehicle
{
	UINT nSessionID;
	TVehicle tVehicle;
};

struct TVehicleItem
{
	int nItemID;
	INT64 nSerial;
	short wCount;
	bool bEternity;					// ����������
	__time64_t tExpireDate;

	void SetItem(const TItem &Item)
	{
		nItemID = Item.nItemID;
		nSerial = Item.nSerial;
		wCount = Item.wCount;
		bEternity = Item.bEternity;
		tExpireDate = Item.tExpireDate;
	}
};

struct TVehicleCompact: TVehicleBase
{
#ifdef PRE_MOD_SYNCPACKET
	UINT nSessionID;
#endif		//#ifdef PRE_MOD_SYNCPACKET
	TVehicleItem Vehicle[Vehicle::Slot::Max];
	__time64_t tLastHungerModifyDate;

	void SetCompact( const TVehicle &VehicleData )
	{
		dwPartsColor1 = VehicleData.dwPartsColor1;
		dwPartsColor2 = VehicleData.dwPartsColor2;
		nExp = VehicleData.nExp;
		nRange = VehicleData.nRange;
		memcpy( wszNickName, VehicleData.wszNickName, sizeof(wszNickName) );
		bSkillSlot = VehicleData.bSkillSlot;
		nSkillID1 = VehicleData.nSkillID1;
		nSkillID2 = VehicleData.nSkillID2;
		nType = VehicleData.nType;
		nCurrentSatiety = VehicleData.nCurrentSatiety;
		for (int i = 0; i < Vehicle::Slot::Max; i++){
			Vehicle[i].SetItem(VehicleData.Vehicle[i]);
		}
	}
};

#if defined( PRE_ADD_SECONDARY_SKILL )

struct TSecondarySkill
{
	int							iSkillID;
	int							iExp;
	//SecondarySkill::Type::eType	Type;
};

struct TSecondarySkillInfo
{
	int								iSkillID;
	int								iExp;
	SecondarySkill::Grade::eType	Grade;
	int								iLevel;
};

struct TSecondarySkillRecipe
{
	int			iSkillID;
	INT64		biItemSerial;
	int			iItemID;
	short		nExp;
};

#endif // #if defined( PRE_ADD_SECONDARY_SKILL )

struct TPvPLadderMatchTypeScore
{
	BYTE			cPvPLadderCode;						// LadderSystem::MatchType::eCode
	int				iPvPLadderGradePoint;
	int				iHiddenPvPLadderGradePoint;
	int				iWin,iLose,iDraw;
	short			nTodayWin,nTodayLose,nTodayDraw;
	short			nConsecutiveWin,nConsecutiveLose;
	__time64_t		tLasePvPLadderScoreDate;
	short			nWeeklyCount;

	void InitWeeklyGradePoint()
	{
		nWeeklyCount = 0;
	}

	int GetExchangeLadderPoint( bool bCheckMinGamePlay=true ) const
	{
		if( bCheckMinGamePlay == true )
		{
			if( nWeeklyCount < LadderSystem::Exchange::MinGamePlay )
				return 0;
		}

		int iLP = 0;
		if( iPvPLadderGradePoint <= 1500 )
		{
			iLP = static_cast<int>((0.34f*iPvPLadderGradePoint))+450;
		}
		else
		{
			iLP = static_cast<int>(1511.26f/static_cast<float>((1+40.f*pow(2.f,(-0.00412f*iPvPLadderGradePoint)))));
		}

		switch( cPvPLadderCode )
		{
			case LadderSystem::MatchType::_1vs1:
			{
				iLP = static_cast<int>(iLP*0.72f);
				break;
			}
			case LadderSystem::MatchType::_2vs2:
			{
#ifdef PRE_ADD_2vs2_LADDER
				iLP = static_cast<int>(iLP*0.89f);
#else
				iLP = static_cast<int>(iLP*0.84f);
#endif
				
				break;
			}
#if defined(PRE_ADD_DWC)			
			case LadderSystem::MatchType::_3vs3_DWC:
			case LadderSystem::MatchType::_3vs3_DWC_PRACTICE:
#endif
			case LadderSystem::MatchType::_3vs3:
			{
#ifdef PRE_ADD_2vs2_LADDER
				iLP = static_cast<int>(iLP*0.95f);
#else
				iLP = static_cast<int>(iLP*0.90f);
#endif
				
				break;
			}
		}
		return iLP;
	}
};

struct TPvPLadderScoreInfo
{
	int							iPvPLadderPoint;
	BYTE						cMaxKillJobCode;
	BYTE						cMaxDeathJobCode;
	BYTE						cLadderTypeCount;
	TPvPLadderMatchTypeScore	LadderScore[LadderSystem::MatchType::MaxCount];

	void SetLadderScoreForCheat( LadderSystem::MatchType::eCode Type, int iLadderPoint, int iGradePoint, int iHiddenGradePoint )
	{
		iPvPLadderPoint = iLadderPoint;

		bool bEmpty = true;
		for( int i=0 ; i<cLadderTypeCount ; ++i )
		{
			if( LadderScore[i].cPvPLadderCode == Type )
			{
				LadderScore[i].iPvPLadderGradePoint			= iGradePoint;
				LadderScore[i].iHiddenPvPLadderGradePoint	= iHiddenGradePoint;
				LadderScore[i].nWeeklyCount = LadderSystem::Exchange::MinGamePlay;
				bEmpty = false;
				break;
			}
		}

		if( bEmpty == true )
		{
			if( cLadderTypeCount < LadderSystem::MatchType::MaxCount )
				return;
			TPvPLadderMatchTypeScore Data;
			memset( &Data, 0, sizeof(Data) );
			Data.iPvPLadderGradePoint		= iGradePoint;
			Data.iHiddenPvPLadderGradePoint	= iHiddenGradePoint;
			LadderScore[cLadderTypeCount++] = Data;
		}
	}

	int GetGradePoint( LadderSystem::MatchType::eCode Type ) const
	{
		for( int i=0 ; i<cLadderTypeCount ; ++i )
		{
			if( LadderScore[i].cPvPLadderCode == Type )
				return LadderScore[i].iPvPLadderGradePoint;
		}

		return LadderSystem::Stats::InitGradePoint;
	}

	int GetTopGradePoint() const
	{
		int iTop = 0;
		for( int i=0 ; i<cLadderTypeCount ; ++i )
		{
			if( LadderScore[i].iPvPLadderGradePoint > iTop )
				iTop = LadderScore[i].iPvPLadderGradePoint;
		}

		return iTop;
	}

	int GetHiddenGradePoint( LadderSystem::MatchType::eCode Type ) const
	{
		for( int i=0 ; i<cLadderTypeCount ; ++i )
		{
			if( LadderScore[i].cPvPLadderCode == Type )
				return LadderScore[i].iHiddenPvPLadderGradePoint;
		}

		return LadderSystem::Stats::InitGradePoint;
	}

	int GetExchageLadderPoint( LadderSystem::MatchType::eCode Type ) const
	{
		for( int i=0 ; i<cLadderTypeCount ; ++i )
		{
			if( LadderScore[i].cPvPLadderCode == Type )
				return LadderScore[i].GetExchangeLadderPoint();
		}

		return 0;
	}

	int GetTopExchangeLadderPoint() const
	{
		int iTop = 0;
		for( int i=0 ; i<cLadderTypeCount ; ++i )
		{
			int iVal = LadderScore[i].GetExchangeLadderPoint();
			if( iVal > iTop )
				iTop = iVal;
		}

		return iTop;
	}

	void UseLadderPoint( int iPoint )
	{
		if( iPvPLadderPoint < iPoint )
		{
			_ASSERT(0);
			return;
		}

		iPvPLadderPoint -= iPoint;
	}
};

struct TPvPLadderJobScore
{
	BYTE	cJobCode;
	int		iWin;
	int		iLose;
	int		iDraw;
};

struct TPvPLadderScoreInfoByJob
{
	BYTE				cJobCount;
	TPvPLadderJobScore	LadderScoreByJob[JOB_KIND_MAX];
};

struct TPvPLadderRanking
{
	INT64	biCharacterDBID;
	int		iRank;
	int		iPvPLadderGradePoint;
	WCHAR	wszCharName[NAMELENMAX];
	BYTE	cJobCode;
	int		iWin,iLose,iDraw;
};
struct TPvPGhoulScores
{
	int		nPoint[GhoulMode::PointType::Max];	
};

struct TPeriodResetItem
{
	BYTE cType;
	int iValue;
	__time64_t tDate;
};

#if defined( PRE_ADD_DWC )
struct TDWCChannelInfo
{
	int nChannelID;				// DWC ��ȸ ȸ�� ����
	__time64_t tStartDate;		// ���۽ð�
	__time64_t tEndDate;		// ���� �ð�
	BYTE cStatus;				// DWC::DWCStatus
	int nLadderTime[2];			// DWC ���� ��Ī �ð�, �� 2ȸ�� ��Ī �ð��� ������ �� ����
	BYTE cLimitCharacterLevel;	// DWC ĳ���� �����ϱ� ���� ���� ĳ���� ���� ����
};
#endif // #if defined( PRE_ADD_DWC )

//--------------------------------------------------------------
//                         ��Ŷ����
//--------------------------------------------------------------
//-------------------------------------------------
//	Client <-> Login
//-------------------------------------------------
/* 
	CS_CHECKLOGIN -> SC_CHECKLOGIN
	CS_SERVERLIST -> SC_SERVERLIST
	CS_SELECTSERVER -> SC_CHARLIST
	CS_SELECTCHAR -> SC_CHANNELLIST
	CS_SELECTCHANNEL -> ������!!
*/


struct CSCheckVersion	 		// CS_LOGIN / CS_CHECKVERSION
{
	BYTE cNation;
	BYTE cVersion;
	bool bCheck;
	USHORT nMajorVersion;
	USHORT nMinorVersion;

#if defined (_KR)
	BYTE szMID[MACHINEIDMAX];
	DWORD dwGRC;
#endif	// _KR
};

struct SCCheckVersion			// SC_LOGIN / SC_CHECKVERISON
{
	int nRet;
	char szServerVersion[SERVERVERSIONMAX];
	TDatabaseVersion MembershipDBVersion;
	TDatabaseVersion WorldDBVersion;
};

struct CSCheckLoginKR			// CS_LOGIN / CS_CHECKLOGIN_KR
{
	WCHAR wszPassPort[1024];
	WCHAR wszVirtualIp[IPLENMAX];
};

struct CSCheckLoginKRAZ			// CS_LOGIN / CS_CHECKLOGIN_KRAZ
{
	WCHAR wszKeyID[128];
	WCHAR wszRequestTime[30];	// 19(YYYY-MM-DD HH:NN:SS) + 6(����->\_3, :->\_7 �� ��ü)
	WCHAR wszVirtualIp[IPLENMAX];
};

struct CSCheckLoginCH			// CS_LOGIN / CS_CHECKLOGIN_CH
{
	WCHAR wszVirtualIp[IPLENMAX];
	char szPassport[SNDAOAPASSPORTMAX + 1];
};

struct CSCheckLoginJP			// CS_LOGIN / CS_CHECKLOGIN_JP
{
	INT64 nMemberID;
	WCHAR wszVirtualIp[IPLENMAX];
	char szAuthString[4096];
	Partner::Type::eType	PartnerType;
};

struct CSCheckLoginTW			// CS_LOGIN / CS_CHECKLOGIN_TW
{
	char szAccountName[IDLENMAX];
	char szPassword[PASSWORDLENMAX];
	char szVirtualIp[IPLENMAX];
};

struct CSCheckLoginUS			// CS_LOGIN / CS_CHECKLOGIN_US
{
#ifdef PRE_ADD_STEAM_USERCOUNT
	bool bSteamUser;
#endif		//#ifdef PRE_ADD_STEAM_USERCOUNT
	WCHAR wszPassPort[1024];
	WCHAR wszVirtualIp[IPLENMAX];
};

struct CSCheckLoginSG			// CS_LOGIN / CS_CHECKLOGIN_SG
{
	char szLoginID[CHERRYLOGINIDMAX];
	char szPassWord[PASSWORDLENMAX];
	char szVirtualIp[IPLENMAX];
#if defined (PRE_MOD_SG_WITH_MACADDR)
	char szMacAddress[MACADDRLENMAX];
#endif		//#if defined (PRE_MOD_SG_WITH_MACADDR)
};

struct CSCheckLoginTH			// CS_LOGIN / CS_CHECKLOGIN_TH
{
	char szAccountName[IDLENMAX];
	char szPassword[PASSWORDLENMAX];
	char szDomain[DOMAINLENMAX];
	char szVirtualIp[IPLENMAX];
};

struct CSCheckLoginTHOTP		//CS_LOGIN / CS_CHECKLOGIN_TH_OTP
{
	char szOTPResponse[AsiaAuth::Common::OTPLenMax + 1];
};

struct SCOTRPResult				//CS_LOGIN / SC_ASIASOFT_RET_OTP
{
	int nRetCode;
};

struct CSCheckLoginID			//CS_LGIN / CS_CHECKLOGIN_ID
{
	char szAccountName[IDLENMAX];
	char szPassword[PASSWORDLENMAX];	
	char szVirtualIp[IPLENMAX];
	char szMacAddress[MACADDRLENMAX];
	char szKey[KREONKEYMAX];
	int nReturnError;
};

struct CSCheckLoginRU			// CS_LOGIN / CS_CHECKLOING_RU
{
	char szAccountName[IDLENMAX];
	char szOTPHash[MAILRUOTPMAX];			// 65..
	char szVirtualIp[IPLENMAX];
};

struct CSCheckLoginEU			// CS_LOGIN / CS_CHECKLOGIN_EU
{
	char szAccountName[IDLENMAX];
	char szPassword[PASSWORDLENMAX];
	char szVirtualIp[IPLENMAX];
	char cSelectedLanguage;
};

struct CSCheckLogin				// CS_LOGIN / CS_CHECKLOGIN
{
	WCHAR wszUserId[IDLENMAX];
	WCHAR wszPassword[PASSWORDLENMAX];
	WCHAR wszVirtualIp[IPLENMAX];
#ifdef PRE_ADD_MULTILANGUAGE
	char cSelectedLanguage;
#endif		//#ifdef PRE_ADD_MULTILANGUAGE
};

struct SCCheckLogin				// SC_LOGIN / SC_CHECKLOGIN
{
	UINT nSessionID;
	int nRet;
#if !defined(PRE_MOD_SELECT_CHAR)
	BYTE cCharCountMax;
	BYTE cGender;
#endif	// #if defined(PRE_MOD_SELECT_CHAR)
	__time64_t _tServerTime;
	TGameOptions GameOption;
#ifdef PRE_ADD_COMEBACK
	bool bComebackUser;
#endif		//#ifdef PRE_ADD_COMEBACK
#if defined( PRE_ADD_GAMEQUIT_REWARD )
	bool bReConnectNewbieReward;	// ��) �ű� ���� ���� ���� ���� �����(NewbieReward: true/RegTime: �����ð�)
	__time64_t _tAccountRegistTime; //     �ű� ���� ���� �ð��� ���� ���(NewbieReward: false/RegTime: �����ð�), ���� ��� �ƴѰ��(false/0)
#endif	// #if defined( PRE_ADD_GAMEQUIT_REWARD )
#ifdef PRE_ADD_DOORS
	bool bMobileAuthentication;
#endif		//PRE_ADD_DOORS
#if defined(_CH) || defined(_KRAZ)
	WCHAR wszAccountName[IDLENMAX];
#endif	// #if defined(_CH) || defined(_KRAZ)
#if defined(_CH)
	char szInfoURL[SNDAOAPASSPORTMAX];
#endif
};

struct TServerListData
{
	BYTE cWorldID;
	WCHAR wszServerName[WORLDNAMELENMAX];
	UINT nWorldMaxUser;
	UINT nWorldCurUser;
	bool bOnline;
	bool bOnTop;
	BYTE	cMyCharCount;
};

struct SCServerList				// SC_LOGIN / SC_SERVERLIST
{
	BYTE cServerCount;
	TServerListData ServerListData[WORLDCOUNTMAX];
};

struct CSSelectServer			// CS_LOGIN / CS_SELECTSERVER
{
	char cWorldID;
};

struct TCharListData
{
#if defined( PRE_ADD_TOTAL_LEVEL_SKILL )
	BYTE cWorldSetID;
#endif
#if defined(PRE_MOD_SELECT_CHAR)
	BYTE cWorldID;
#endif	// #if defined(PRE_MOD_SELECT_CHAR)
	WCHAR wszCharacterName[NAMELENMAX];
#if defined(PRE_MOD_SELECT_CHAR)
	INT64 biCharacterDBID;
#else	// #if defined(PRE_MOD_SELECT_CHAR)
	BYTE cCharIndex;
#endif	// #if defined(PRE_MOD_SELECT_CHAR)
	BYTE cLevel;
	BYTE cJob;
	int nMapID;
	int nEquipArray[EQUIPMAX];
	int nDefaultBody;	// ó�� �����ÿ� ������ equip
	int nDefaultLeg;
	int nDefaultHand;
	int nDefaultFoot;
	DWORD dwHairColor;
	DWORD dwEyeColor;
	DWORD dwSkinColor;
	INT64 biUniqueID;	// nexon �޽������� ���� �뵵(?)
	int nCashEquipArray[CASHEQUIPMAX];
	bool bDeleteFlag;
	__time64_t DeleteDate;
#if defined(PRE_CHARLIST_SORTING)
	__time64_t LastLoginDate;
	__time64_t CreateDate;
#endif	// #if defined(PRE_CHARLIST_SORTING)
#if defined( PRE_ADD_NEWCOMEBACK )
	bool bCombackEffectItem;
#endif
#if defined( PRE_ADD_DWC )
	BYTE cAccountLevel; // DWC ĳ���� = 40
#endif // #if defined( PRE_ADD_DWC )
};

struct SCCharList				// SC_LOGIN / SC_CHARLIST
{
	int nRet;
#if defined(PRE_CHARLIST_SORTING)
	BYTE cCharacterSortCode;	// Login::CharacterSortCode
#endif	// #if defined(PRE_CHARLIST_SORTING)
	BYTE cAccountLevel;
	BYTE cCharCountMax;
	BYTE cCharCount;	// �����
	TCharListData CharListData[CHARCOUNTMAX];
};

struct CSSelectChar				// CS_LOGIN / CS_SELECTCHAR
{
#if defined(PRE_MOD_SELECT_CHAR)
	INT64 biCharacterDBID;
#else	// #if defined(PRE_MOD_SELECT_CHAR)
	BYTE cCharIndex;
#endif	// #if defined(PRE_MOD_SELECT_CHAR)
	int nSeed;
	int nValue[SecondAuth::Common::PWMaxLength];	// 2�� ���� ��й�ȣ ��
#ifdef PRE_ADD_23829
	bool bSetIgnoreAuthNotify;
#endif
};

struct CSCreateChar				// CS_LOGIN / CS_CREATECHAR
{
#if defined(PRE_MOD_SELECT_CHAR)
	BYTE cWorldID;
#endif	// #if defined(PRE_MOD_SELECT_CHAR)
	WCHAR wszCharacterName[NAMELENMAX];
	int nEquipArray[EQUIPMAX];
	DWORD dwHairColor;
	DWORD dwEyeColor;
	DWORD dwSkinColor;
	BYTE cClass;
};

struct SCCreateChar				// SC_LOGIN / SC_CREATECHAR
{
	WCHAR wszCharacterName[NAMELENMAX];
	int nRet;
	char cDailyCreateMaxCount;	// �ִ� ����
};

struct CSDeleteChar				// CS_LOGIN / CS_DELETECHAR
{
#if defined(PRE_MOD_SELECT_CHAR)
	INT64 biCharacterDBID;
#else	// #if defined(PRE_MOD_SELECT_CHAR)
	BYTE cCharIndex;
#endif	// #if defined(PRE_MOD_SELECT_CHAR)
	int nSeed;
	int nValue[SecondAuth::Common::PWMaxLength];
};

struct SCDeleteChar				// SC_LOGIN / SC_DELETECHAR
{
#if defined(PRE_MOD_SELECT_CHAR)
	INT64 biCharacterDBID;
#else	// #if defined(PRE_MOD_SELECT_CHAR)
	BYTE cCharIndex;
#endif	// #if defined(PRE_MOD_SELECT_CHAR)
	int nRet;
	BYTE cFailCount;
};

struct sChannelInfo
{
	UINT nChannelID;
	USHORT nChannelIdx;			//for UI
	UINT nMapIdx;
	USHORT nCurrentUserCount;
	USHORT nMaxUserCount;
	int nChannelAttribute;
	int nMeritBonusID;
	BYTE cMinLevel;
	BYTE cMaxLevel;
	BYTE cVillageID;
	bool bVisibility;

	char szIP[IPLENMAX];
	USHORT nPort;
	int nLimitLevel;
	bool bShow;
	USHORT nDependentMapID;
	int nServerID;
	CHAR cThreadID;
#if defined(PRE_ADD_CHANNELNAME)
	WCHAR wszLanguageName[CHANNELLANGUAGEMAX];
#endif // #if defined(PRE_ADD_CHANNELNAME)
};

struct TMeritInfo
{
	int nID;
	int nMinLevel;
	int nMaxLevel;
	int nMeritType;
	int nExtendValue;
};

struct SCChannelList			// SC_LOGIN / SC_CHANNELLIST
{
	int  nRet;
	BYTE cFailCount;
	BYTE cCount;
	sChannelInfo Info[VILLAGECHANNELMAX];	
};

struct CSSelectChannel			// SC_LOGIN / SC_SELECTCHANNEL
{
	int nChannelID;
};

struct SCTutorialMapInfo		// SC_LOGIN / SC_TUTORIALMAPINFO
{
	int nMapIndex;
	char cGateNo;
};

struct SCWaitUser				// SC_LOGIN / SC_WAITUSER
{
	BYTE cWorldID;
	UINT nWaitUserCount;
	ULONG nEstimateTime;
};

struct SCCHeckBlock				// SC_LOGIN / SC_CHECKBLOCK
{
	__time64_t ServerDate;
	__time64_t StartDate;
	__time64_t EndDate;
	WCHAR wszBlockReason[RESTRAINTREASONMAX];
	BYTE cType;
	int iReasonID;
};

struct CSReviveChar
{
#if defined(PRE_MOD_SELECT_CHAR)
	INT64 biCharacterDBID;
#else	// #if defined(PRE_MOD_SELECT_CHAR)
	BYTE cCharIndex;
#endif	// #if defined(PRE_MOD_SELECT_CHAR)
};

struct SCReviveChar
{
#if defined(PRE_MOD_SELECT_CHAR)
	INT64 biCharacterDBID;
#else	// #if defined(PRE_MOD_SELECT_CHAR)
	BYTE cCharIndex;
#endif	// #if defined(PRE_MOD_SELECT_CHAR)
	int nRet;
};

struct CSCharacterSortCode		// CS_LOGIN / CS_CHARACTERSORTCODE
{
	BYTE cCharacterSortCode;
};

#if defined( PRE_ADD_DWC )
struct CSCreateDWCChar				// CS_LOGIN / CS_CREATEDWCCHAR
{
	BYTE cJobCode;
	// ������ ���� �Ǿ��ִ� ĳ���� �߿��� �ϳ��� �����ؼ� �̸��� ���� ��
#if defined( PRE_MOD_SELECT_CHAR )
	INT64 biSelectCharacterDBID;	
#else // #if defined( PRE_MOD_SELECT_CHAR )
	BYTE cSelectCharIndex;
#endif // #if defined( PRE_MOD_SELECT_CHAR )
};

struct SCCreateDWCChar				// SC_LOGIN / SC_CREATEDWCCHAR
{
	int nRet;
};
#endif // #if defined( PRE_ADD_DWC )

//-------------------------------------------------
//	SYSTEM (CS_SYSTEM, SC_SYSTEM)
//-------------------------------------------------

struct SCVillageInfo			// SC_SYSTEM / SC_VILLAGEINFO
{
	UINT	nSessionID;
	char	szVillageIp[IPLENMAX];
	USHORT	wVillagePort;
	short	nRet;
	UINT nAccountDBID;				// ���� DBID
	INT64 biCertifyingKey;			// ����Ű
};

struct CSConnectVillage			// CS_SYSTEM / CS_CONNECTVILLAGE
{
	UINT nSessionID;
	UINT nAccountDBID;				// ���� DBID
	INT64 biCertifyingKey;			// ����Ű
	WCHAR wszVirtualIp[IPLENMAX];	// nexon pcbang
#ifdef _KR
	BYTE szMID[MACHINEIDMAX];
	DWORD dwGRC;
#endif //#ifdef _KR
};

struct SCDetachMessage			//SC_SYSTEM / SC_DETACHCONNECTION_MSG
{
	int nRetCode;
#if defined(PRE_MOD_DUPLICATION_LOGIN_MES)
	bool bDisConnectByClient;
#endif	// #if defined(PRE_MOD_DUPLICATION_LOGIN_MES)
};

struct SCZeroPopulation			//SC_SYSTEM / SC_ZEROPOPULATION
{
	bool bZeroPopulation;
};

struct SCConnectVillage			// SC_SYSTEM / SC_CONNECTVILLAGE
{
	short	nRet;
	char	szServerVersion[SERVERVERSIONMAX];
	int		nChannelAttr;
	BYTE	cWithoutLoading;
};

struct CSVillageReady			// CS_SYSTEM / CS_VILLAGEREADY
{
	bool boFirst;
	bool boChannelMove;
};

struct SCGameInfo				// SC_SYSTEM / SC_GAMEINFO
{
	GameTaskType::eType GameTaskType;
	BYTE	cReqGameIDType;
	ULONG	nGameServerIP;
	USHORT	nGameServerPort;
	USHORT	nGameServerTcpPort;
	int nRet;
	UINT nAccountDBID;				// ���� DBID
	INT64 biCertifyingKey;			// ����Ű
};

struct CSConnectRequest			// CS_SYSTEM / CS_CONNECT_REQUEST
{
	UINT nSessionID;
	ULONG nAddrIP;
	USHORT nPort;
};

struct SCConnectedResult		// SC_SYSTEM / SC_CONNECTED_RESULT
{
	UINT nSessionID;				//�׳� ������ ���� ������~
	char szServerVersion[SERVERVERSIONMAX];
};

struct CSReady2Recieve			// CS_SYSTEM / CS_READY_2_RECIEVE
{
	UINT nSessionID;						//Ȥ�ó� �ؼ�
	WCHAR wszVirtualIp[IPLENMAX];	// nexon pcbang
};

struct CSIntendedDisconnect		// CS_SYSTEM / CS_INTENDED_DISCONNECT
{
};

struct CSConnectGame				// CS_SYSTEM / CS_CONNECTGAME
{
	UINT nSessionID;
	UINT nAccountDBID;				// ���� DBID
	INT64 biCertifyingKey;			// ����Ű
#ifdef _KR
	BYTE szMID[MACHINEIDMAX];
	DWORD dwGRC;
#endif //#ifdef _KR
};

struct CCPeerConnectRequest		// CS_SYSTEM / CC_PEER_CONNECT_REQUEST
{
	UINT nSessionID[2];
	char cState;			//0 = Connect Request, 1 = Connected, 2 = Disconnected
};

struct SCPeerConnectRequest		// SC_SYSTEM / SC_PEER_CONNECT_REQUEST
{
	//�Ե��� ���鳢�� ������ �ϻ�~
	ULONG nDestAddrIP;
	USHORT nDestAddrPort;
	UINT nSessionID[2];
};

struct CSReconnectLogin			// CS_SYSTEM / CS_RECONNECTLOGIN
{
	UINT nSessionID;
	UINT nAccountDBID;				// ���� DBID
	INT64 biCertifyingKey;			// ����Ű
#ifdef _KR
	BYTE szMID[MACHINEIDMAX];
	DWORD dwGRC;
#endif //#ifdef _KR
#if defined(_ID)
	char szVirtualIp[IPLENMAX];
	char szMacAddress[MACADDRLENMAX];
	char szKey[KREONKEYMAX];
#endif // #if defined(_ID)
};

struct CSAbandonStage			// CS_SYSTEM / CS_ABANDONSTAGE
{
	bool bIsPartyOut;			//��Ƽ���� ������ ���̸� Ʈ��, �׳� ����� �Ƚ�
	bool bIntenedDisconnect;
};

struct SCReconnectLogin			// SC_SYSTEM / SC_RECONNECTLOGIN
{
	int nRet;
	UINT nAccountDBID;				// ���� DBID
	INT64 biCertifyingKey;			// ����Ű
};

struct SCCountDownMsg			// SC_SYSTEM / SC_COUNTDOWNMSG
{
	BYTE cIsStart;				// 1:start 0:stop
	int nCountTime;
	UINT nCode;					//	����� uistringnum
	BYTE msgboxType;			//	0:prefix 1:number_only 2:progress
};

struct SCFCMState				// SC_SYSTEM / SC_CHN_FCM
{
	int nOnlineMin;
};

#if defined(_GPK)
const int GPKCODELENMAX = 1024 * 6;
struct SCGPKCode				// SC_SYSTEM / SC_CHN_GPKCODE
{
	int nCodeLen;
	BYTE Code[GPKCODELENMAX];
};

const char GPKDATALENMAX = 64;
struct CSGPKData				// CS_SYSTEM / CS_CHN_GPKDATA
{
	char Data[GPKDATALENMAX];
};

struct SCGPKData				// SC_SYSTEM / SC_CHN_GPKDATA
{
	char Data[GPKDATALENMAX];
};

const int GPKAUTHLENMAX = 128;
struct CSGPKAuthData			// SC_SYSTEM / SC_CHN_GPKAUTHDATA
{
	int nLen;
	unsigned char Data[GPKAUTHLENMAX];
};

struct SCGPKAuthData			// CS_SYSTEM / CS_CHN_GPKAUTHDATA
{
	int nLen;
	unsigned char Data[GPKAUTHLENMAX];
};
#endif	// _CH

#if defined(PRE_ADD_VIP)
struct CSVIPAutomaticPay		// CS_SYSTEM / CS_VIPAUTOMATICPAY,				// VIP�ڵ����� 
{
	bool bAutomaticPay;
};

struct SCVIPAutomaticPay		// SC_SYSTEM / SC_VIPAUTOMATICPAY,				// VIP�ڵ����� 
{
	bool bAutomaticPay;
};

#endif	// #if defined(PRE_ADD_VIP)


struct CSTCPPing
{
	DWORD dwTick;
};

typedef CSTCPPing SCTCPPing;


struct SCServiceClose			//SC_SYSTEM / SC_SERVICECLOSE				//����Ŭ�ο��� �˸���~
{
	__time64_t _tNow;				//��������Ƚð�
	__time64_t _tOderedTime;		//���񽺸Ŵ����� ��ɵǾ��� �ð�(Ȥ�ó� �ʿ��ұ� �ؼ�)
	__time64_t _tCloseTime;			//����Ŭ���� �����ð�
};

//-------------------------------------------------
//	CHAR (CS_CHAR, SC_CHAR)
//-------------------------------------------------

struct CSLoockUser			// CS_CHAR / CS_LOOKUSER
{
	WCHAR wszCharacterName[NAMELENMAX];
};

struct SCLoockUser			// SC_CHAR / SC_LOOKUSER
{
};

struct SCMapInfo				// SC_CHAR / SC_MAPINFO
{
	int nMapIndex;
	BYTE cMapArrayIndex;
	BYTE cEnviIndex;
	BYTE cEnviArrayIndex;
	int nChannelID;
	short wChannelIdx;
	TMeritInfo MeritInfo;
};

struct SCPvPInfo
{
	TPvPGroup	PvP;
};

struct SCPvPMemberIndex		//SC_PVP_MEMBERINDEX
{
	USHORT nTeam;
	int nRetCode;
	BYTE cCount;
	TSwapMemberIndex Index[PARTYMAX];
};

struct SCPvPMemberGrade		//SC_PVP_MEMBERGRADE
{
	int nRetCode;
	UINT uiUserState;
	UINT nSessionID;
};

struct SCPvPTryAcquirePoint		//SC_PVP_TRYACQUIREPOINT
{
	int nAreaID;
	int nRetCode;
};

struct SCPvPTryAcquireSkill		//SC_PVP_TRYACQUIRESKILL
{
	int nSkillID;
	int nLevel;
	int nRetCode;
};

struct SCPvPInitSkill			//SC_PVP_INITSKILL
{
	int nRetCode;
};

struct SCPvPUseSkill			//SC_PVP_USESKILL
{
	UINT nSessionID;		//����ѻ�� ���� �ʿ��Ҳ� ���Ƽ�
	int nSkillID;
	int nRetCode;
};

struct SCPvPSwapSkillIndex		//SC_PVP_SWAPSKILLINDEX
{
	char cFrom;
	char cTo;
	int nRetCode;
};

//#if defined(_GAMESERVER) || defined(_VILLAGESERVER) || defined(_WINDOWS)
#if defined(_GAMESERVER) || defined(_VILLAGESERVER) || (defined(_WINDOWS) && !defined(_LAUNCHER))
struct SCEnter					// SC_CHAR / SC_ENTER
{
	UINT nSessionID;
	UINT nVehicleObjectID;
	WCHAR wszCharacterName[NAMELENMAX];
	char cAccountLevel;	// �������� (eAccountLevel)
	BYTE cPartyState;
	BYTE cClassID;
	BYTE cJobArray[JOBMAX];
	BYTE cLevel;
	int	nExp;
	INT64 nCoin;	// ��ü ������ �ִ� �����̱� ������ int64��
	BYTE cRebirthCoin;
	BYTE cPCBangRebirthCoin;
	USHORT wRebirthCashCoin;
	EtVector3 Position;
	float fRotate;
	USHORT wSkillPoint[DualSkill::Type::MAX];
	int	nDefaultBody;	// ó�� �����ÿ� ������ equip
	int	nDefaultLeg;
	int	nDefaultHand;
	int	nDefaultFoot;
	DWORD dwHairColor;
	DWORD dwEyeColor;
	DWORD dwSkinColor;
	short nAppellation;
	short nCoverAppellation;
	USHORT wFatigue;
	USHORT wWeekFatigue;
	USHORT wPCBangFatigue;
	USHORT wEventFatigue;
	USHORT wMaxEventFatigue;
#if defined(PRE_ADD_VIP)
	bool bVIP;
	BYTE cVIPRebirthCoin;
	USHORT wVIPFatigue;	// ���� �Ƿε�
	int nVIPPoint;		// ����Ʈ
	__time64_t tVIPExpirationDate;	// ������
	bool bVIPAutomaticPay;	// �ڵ����� ����
#endif	// #if defined(PRE_ADD_VIP)
	TPvPGroup sPvPInfo;
	char cViewCashEquipBitmap[VIEWCASHEQUIPMAX_BITSIZE];
	TGuildSelfView GuildSelfView;	// ��� �ð����� (����)
	char cCurrentGuildWarEventStep;	// ���� ����� ����
	short wCurrentGuldWarScheduleID;	// ���� ����Ǵ� ����� ����
	char cSkillPage;					// ���� Ȱ��ȭ �Ǿ��ִ� ��ų������
	char cAge;
#if defined( PRE_ADD_NAMEDITEM_SYSTEM )
	int nNamedItemID;
#endif
#ifdef PRE_ADD_NEW_MONEY_SEED
	INT64 nSeed;
#endif // PRE_ADD_NEW_MONEY_SEED
};

struct SCEnterUser				// SC_CHAR / SC_ENTERUSER
{
	UINT nSessionID;
	UINT nVehicleObjectID;
	WCHAR wszCharacterName[NAMELENMAX];
	char cAccountLevel;	// ��������
	bool bHide;
	BYTE cPartyState;
	BYTE cClassID;
	BYTE cLevel;
	BYTE cPvPLevel;
	BYTE cJob;
	EtVector3 Position;
	float fRotate;
	bool bBattleMode;
	int	nDefaultBody;	// ó�� �����ÿ� ������ equip
	int	nDefaultLeg;
	int	nDefaultHand;
	int	nDefaultFoot;
	DWORD dwHairColor;
	DWORD dwEyeColor;
	DWORD dwSkinColor;
	short nAppellation;
	short nCoverAppellation;
	TGuildSelfView GuildSelfView;	// ��� �ð����� (����)
	char cViewCashEquipBitmap[VIEWCASHEQUIPMAX_BITSIZE];
#if defined(PRE_ADD_VIP)
	bool bVIP;
#endif	// #if defined(PRE_ADD_VIP)
	bool bIsPartyLeader;
	TVehicle VehicleEquip;
	TVehicle PetEquip;

	char cCount;
	char cCashCount;
	char cGlyphCount;
#if defined(PRE_ADD_TALISMAN_SYSTEM)
	char cTalismanCount;
#endif

	int	nSourceItemID;
#if defined( PRE_ADD_NAMEDITEM_SYSTEM )
	int nNamedItemID;
#endif
#if defined ( PRE_ADD_BESTFRIEND)
	INT64 biBestFriendItemSerial;
	WCHAR wszBestFriendName[NAMELENMAX];
#endif
#if defined(PRE_ADD_TRANSFORM_POTION)
	int	nTransformID;
#endif //#if defined(PRE_ADD_TRANSFORM_POTION)
#if defined(PRE_ADD_DWC)
	WCHAR wszDWCTeamName[GUILDNAME_MAX];
#endif

#if defined(PRE_ADD_TALISMAN_SYSTEM)
	TItemInfo EquipArray[EQUIPMAX+CASHEQUIPMAX+GLYPHMAX+TALISMAN_MAX];		// �̰� �׻� �� ���̾���մϴ�.
#else
	TItemInfo EquipArray[EQUIPMAX+CASHEQUIPMAX+GLYPHMAX];		// �̰� �׻� �� ���̾���մϴ�.
#endif
};

struct SCEnterNpc				// SC_CHAR / SC_ENTERNPC
{
	UINT nNpcObjectID;
	UINT nNpcID;
	EtVector3 Position;
	float fRotate;
};
#endif

struct SCLeaveUser				// SC_CHAR / SC_LEAVEUSER
{
	UINT nSessionID;
	char cLeaveType;		// eLeaveType
	char cAccountLevel;	// ��������
};

struct SCLeaveNpc				// SC_CHAR / SC_LEAVENPC
{
	UINT nObjectID;
};

struct SCEnterPropNpc
{
	UINT nNpcObjectID;
	int nPropUniqueID;
};

struct SCLeavePropNpc
{
	int nPropUniqueID;
};

struct TQuickSlotEx: TQuickSlot
{
	BYTE cIndex;
};

struct SCQuickSlotList		// SC_CHAR / SC_QUICKSLOTLIST
{
	BYTE cSlotCount;
	TQuickSlotEx QuickSlot[QUICKSLOTMAX];
};

struct CSAddQuickSlot		// SC_CHAR / SC_ADDQUICKSLOT
{
	BYTE cSlotIndex;		// ���ཽ��â index
	BYTE cSlotType;			// eQuickSlotType ����
	INT64 nID;				// �ش� ID (�����۾��̵� Ȥ�� ��ų���̵� Ȥ�� Ż�� ������ �ø���)
};

struct SCAddQuickSlot		// SC_CHAR / SC_ADDQUICKSLOT
{
	BYTE cSlotIndex;		// ���ཽ��â index
	BYTE cSlotType;			// eQuickType ����
	int nID;				// �ش� ID (�����۾��̵� Ȥ�� ��ų���̵�)
	int nRet;
};

struct CSDelQuickSlot		// SC_CHAR / CS_DELQUICKSLOT
{
	BYTE cSlotIndex;
};

struct SCDelQuickSlot		// SC_CHAR / SC_DELQUICKSLOT
{
	BYTE cSlotIndex;
	int nRet;
};


struct CSCloseUIWindow
{
	eWindowState WindowState; // eWindowState
};


struct SCFatigue			// SC_CHAR / SC_FATIGUE
{
	UINT iResult;
	UINT nSessionID;
	short wFatigue;		// ���簪
	short wWeekFatigue;		// �ְ� ���簪
	short wPCBangFatigue;	// �Ǿ��� ���簪
	short wEventFatigue;
#if defined(PRE_ADD_VIP)
	short wVIPFatigue;		// VIP ���簪
#endif	// #if defined(PRE_ADD_VIP)
};

struct SCHide				// SC_CHAR / SC_HIDE
{
	UINT nSessionID;
	bool bHide;
};

struct TShutdownData
{
	char cType;	// eShutdownType
	short nYear;
	char nMonth;
	char nDay;
	char nHour;
};

struct SCPCBang				// SC_CHAR / SC_PCBANG
{
	char cPCBangGrade;
#if defined(_KR)
	char cAuthorizeType;
	int nArgument;
	int nShutdownTime;
#endif	// #if defined(_KR)
#if defined(_KRAZ)
	TShutdownData Shutdown;
#endif	// #if defined(_KRAZ)
};

struct SCNestClear			// SC_CHAR / SC_NESTCLEAR
{
	char cCount;
	TNestClearData NestClear[NESTCLEARMAX];
};

struct SCUpdateNestClear	// SC_CHAR / SC_UPDATENESTCLEAR
{
	TNestClearData Update;
};

struct SCMaxLevelCharacterCount
{
	int iCount;
};

struct SCCommonVariableList
{
	BYTE cCount;
	CommonVariable::Data List[CommonVariable::Type::Max];
};

struct SCModCommonVariable
{
	CommonVariable::Data Data;
};

#if defined( PRE_ADD_NOTIFY_ITEM_COMPOUND )
struct CSCompoundNotify
{
	int nItemID;
	int nItemOption;
};
#endif
#if defined(PRE_ADD_STAGE_CLEAR_ADD_REWARD)
struct CSStageClearBonusRewardSelect			// CS_CHAR / CS_STAGECLEAR_BONUSREWARD_SELECT
{
	UINT nPropID;
	int nItemID;
};

struct SCStageClearBonusRewardSelect			// SC_CHAR / SC_STAGECLEAR_BONUSREWARD_SELECT
{
	int nResult;
};
#endif	// #if defined(PRE_ADD_STAGE_CLEAR_ADD_REWARD)
//-------------------------------------------------
//	ACTOR (CS_ACTOR, SC_ACTOR)
//-------------------------------------------------

struct CSActorMessage			// CS_ACTOR / CS_ACTOR_MSG
{
	UINT nSessionID;
	char cBuf[128];
};

struct SCActorMessage			// SC_ACTOR / SC_ACTOR_MSG
{
	UINT nSessionID;
	char cBuf[128];
};

//-------------------------------------------------
//	ACTOR BUNDLE (CS_ACTORBUNDLE, SC_ACTORBUNDLE)
//-------------------------------------------------	
struct SCActorBundleMessage
{
	UINT nSessionID;
	char cBuf[256];
};

struct CSActorBundleMessage
{
	UINT nSessionID;
	char cBuf[256];
};

struct SCActorShadowMessage
{
	UINT nSessionID;
	char cBuf[256];
};


//-------------------------------------------------
//	PROP (CS_PROP, SC_PROP)
//-------------------------------------------------

struct CSPropMessage			// CS_PROP / CS_PROP_MSG
{
	UINT nUniqueID;
	char cBuf[128];
};

struct SCPropMessage			// SC_PROP / SC_PROP_MSG
{
	UINT nUniqueID;
	char cBuf[128];
};


//-------------------------------------------------
//	PARTY (CS_PARTY, SC_PARTY)
//-------------------------------------------------

#ifdef PRE_MOD_SYNCPACKET
const int MAX_SEND_SYNCPLAYER = 4;
#else		//#ifdef PRE_MOD_SYNCPACKET
const int MAX_SEND_SYNCPLAYER = 8;
#endif		//#ifdef PRE_MOD_SYNCPACKET

struct CSCreateParty			// CS_PARTY / CS_CREATEPARTY
{
#if defined( PRE_PARTY_DB )
	ePartyType PartyType;
	int iBitFlag;
	int iPassword;
#else
	BYTE cRaidParty;			//�ϴ��� 5���̻� 8�������� ��Ƽ�� ��Ī�� 1:true, otherwise:false
	BYTE cIsJobDice;
	BYTE cUserLvLimitMax;
#endif // #if defined( PRE_PARTY_DB )
	BYTE cPartyMemberMax;

	TPARTYITEMLOOTRULE ItemLootRule;			//ePartyItemLootRule ����
	TITEMRANK ItemRank;				//�����۷�ũ(��Ʈ���ȹ����)
	BYTE cUserLvLimitMin;		//�θƽ����� �ִܴ�.

	int nTargetMapIdx;
	TDUNGEONDIFFICULTY Difficulty;

#if defined( PRE_PARTY_DB )
	WCHAR wszPartyName[PARTYNAMELENMAX];
#else
	BYTE cUseVoice;				//zero is nouse otherwise use
	BYTE cNameLen;
	BYTE cPassWordLen;
	WCHAR wszBuf[PARTYNAMELENMAX + PARTYPASSWORDMAX];
#endif // #if defined( PRE_PARTY_DB )
};

struct SCCreateParty			// SC_PARTY / SC_CREATEPARTY
{
	int nRetCode;
	TPARTYID PartyID;
	ePartyType PartyType;
	BYTE cIsAutoCreate;			//�ڵ����� ���� 0: �Ϲݻ��� 1�ڵ�����
};

struct CSDestroyParty			// CS_PARTY / CS_DESTROYPARTY
{
	TPARTYID PartyIndex;
};

struct CSInvitePartyMember			//CS_PARTY / CS_PARTYINVITE
{
	BYTE cInviteMemberSize;
	WCHAR wszBuf[NAMELENMAX];
};

struct CSInviteDenied
{
	TPARTYID PartyIdx;
	BYTE cIsOpenBlind;		//��ȭ���̰ų� â�� �����ִ� ���
	BYTE cInviterNameLen;
	WCHAR wszBuf[NAMELENMAX];
};

struct CSPartyMemberInfo			//CS_PARTY / CS_PARTYMEMBER_INFO
{
	TPARTYID PartyID;
};

struct CSPartyInviteOverServerResult			//CS_PARTY / CS_PARTYINVITE_OVERSERVER_RESULT
{
	int nGameServerID;				//from
	WCHAR wszInviterName[NAMELENMAX];
	int nRetCode;					//0 invite agreed otherwise denied
};

struct CSPartyMemberKick			//CS_PARTY / CS_PARTYMEMBER_KICK
{
	UINT nSessionID;
	char cKickKind;						//0�Ϲ� 1����
};

struct CSPartyLeaderSwap			//CS_PARTY / CS_PARTYLEADER_SWAP
{
	UINT nSessionID;
};

struct CSJoinGetReversionItem			//CS_PARTY / CS_JOINGETREVERSEITEM
{
	bool bJoin;
};

struct CSMemberAbsent			//CS_PARTY / CS_ABSENT
{
	BYTE cAbsentFlag;			// 0 absent, 1 present
};

struct CSVerifyDungeon			//CS_PARTY / CS_VERIFYTARGETDUNGEON
{
	int nMapIdx;
	BYTE cDifficulty;
};

struct CSPartyAskJoin			//CS_PARTY / CS_PARTYASKJOIN
{
	WCHAR wszPartyLeaderName[NAMELENMAX];
};

struct CSPartyAskJoinDecision			//CS_PARTY / CS_PARTYASKJOINDECISION
{
	BYTE cRet;								//0 denied, 1 agree
	WCHAR wszPartyAsker[NAMELENMAX];		//�����
};

struct CSPartySwapMemberIndex		//CS_PARTY / CS_SWAPMEMBERINDEX
{
	BYTE cCount;
	TSwapMemberIndex Index[PARTYMAX];
};

struct SCPartySwapMemberIndex		//SC_SWAPMEMBERINDEX
{
	int nRetCode;
	BYTE cCount;
	TSwapMemberIndex Index[PARTYMAX];
};

struct CSPartyWorldZoneList
{
	bool bFlag;
};

struct SCPartyWorldZoneList
{
	int iRet;
};

struct CSVillageSelectChannel			//CS_PARTY / CS_SELECTCHANNEL
{
	int nChannelID;	
};

struct CSPartyInfoModify					//CS_PARTY / CS_PARTYMODIFY
{
	BYTE cPartyMemberMax;
	TPARTYITEMLOOTRULE LootRule;
	TITEMRANK LootRank;
	BYTE cUserLvLimitMin;
	int nTargetMapIdx;
	TDUNGEONDIFFICULTY Difficulty;
	WCHAR wszName[PARTYNAMELENMAX];
#if defined( PRE_PARTY_DB )
	int iBitFlag;
	int iPassword;
#else
	BYTE cUserLvLimitMax;
	BYTE cIsJobDice;
	WCHAR wszPass[PARTYPASSWORDMAX];
#endif // #if defined( PRE_PARTY_DB )
};

struct CSSelectDunGeon				//CS_PARTY / CS_SELECTDUNGEON
{
	BYTE cDiffyculty;
	UINT nMapIndex;
	//short nCanEnterCode;				//ERROR_NONE�̸� �� �� ���� �ƴ� ���̸� ��Ȳ�� ������ eError ������
};

struct CSJoinParty				// CS_PARTY / CS_JOINPARTY
{
	TPARTYID PartyID;
#if defined( PRE_PARTY_DB )
	int iPassword;
#else
	BYTE cPartyPasswordSize;
	WCHAR wszBuf[PARTYPASSWORDMAX];
#endif // #if defined( PRE_PARTY_DB )
};

struct CSPartyListInfo				// CS_PARTY / CS_PARTYLISTINFO
{
#if defined( PRE_PARTY_DB )
	USHORT unReqPage;
	WCHAR wszSearchWord[Party::Max::SearchWordLen];
	Party::OrderType::eCode OrderType;
	Party::StageOrderType::eCode StageOrderType;
	bool bOnlySameVillage;
#if defined( PRE_FIX_61545 )
	WCHAR cSeperator;
#endif // #if defined( PRE_FIX_61545 )

	bool operator==(const CSPartyListInfo& rhs) const
	{
		if( this->unReqPage == rhs.unReqPage && this->OrderType == rhs.OrderType && this->StageOrderType == rhs.StageOrderType && this->bOnlySameVillage == rhs.bOnlySameVillage && wcscmp(this->wszSearchWord,rhs.wszSearchWord) == 0 )
			return true;
		return false;
	}

	void Clear()
	{
		this->OrderType = Party::OrderType::Max;
	}
#else
	BYTE cOffSetCnt;		//�ѹ��� �ް� ���� ī��Ʈ
	BYTE cGetPage;		//���� �ִ� ī��Ʈ�� ���������� ���° �������� �ް� ������
	TDUNGEONDIFFICULTY Difficulty;
	BYTE cIsRefresh;
	int nSortMapIdx[PARTYSORTCONDITIONMAX];
#endif // #if defined( PRE_PARTY_DB )
};

#if defined(PRE_MOD_REQ_JOIN_PARTY_ANSWER_MSG_APP)
struct CSPartyInfo						// CS_PARTY / CS_PARTYINFO
{
	TPARTYID PartyID;
	WCHAR wszCharName[NAMELENMAX];
};
#endif

struct SCJoinParty				// SC_PARTY / SC_JOINPARTY
{
	int nRetCode;
	TPARTYID PartyID;
	WCHAR wszPartyName[PARTYNAMELENMAX];
};

struct SCRefreshParty			// SC_PARTY / SC_REFRESHPARTY
{
	UINT nLeaderSessionID;
	BYTE cCurrentUserCount;
	BYTE cPartyMemberMax;
	BYTE cUserLvLimitMin;
	TPARTYITEMLOOTRULE ItemLootRule;
	TITEMRANK ItemLootRank;
	BYTE cVoiceChatAvailable;
	int nTargetMapIdx;
	TDUNGEONDIFFICULTY Difficulty;
#if defined( PRE_PARTY_DB )
	int iBitFlag;
	ePartyType PartyType;
	int iPassword;
#else
	BYTE cUserLvLimitMax;
	BYTE cIsJobDice;
	BYTE cPartyType;							//ePartyType
	WCHAR wszPass[PARTYPASSWORDMAX];
#endif // #if defined( PRE_PARTY_DB )
	BYTE cPartyRefreshSubJect;					//ePartyRefreshSubject ����
#if defined( PRE_WORLDCOMBINE_PARTY )
	int nWorldCombinePartyTableIndex;
#endif
	WCHAR wszName[PARTYNAMELENMAX];
	SPartyMemberInfo Member[PARTYMAX];
};

struct SCPartyOut				// SC_PARTY / SC_PARTYOUT
{
	UINT nSessionID;		//���� ��������
	int nRetCode;	// success, fail ����
	char cKickKind;	//-1ű�ƴϰ� ������ 0�Ϲ�ű 1����ű
};

//struct SCDelPartyMember			// SC_PARTY / SC_DELPARTYMEMBER
//{
//	UINT nMemberUID;
//	BYTE cIsUnintenedDelMember;
//};

struct SPartyListInfo
{
	TPARTYID PartyID;
	BYTE cUserLvLimitMin;
#if defined( PRE_PARTY_DB )
	int iCurMapIndex;
	bool bIsBonus;
#if defined( _FINAL_BUILD )
#else
	INT64 biSortPoint;
#endif // #if defined( _FINAL_BUILD )
#else
	BYTE cUserLvLimitMax;
	USHORT nChannelMaxUser;
	USHORT nChannelCurUser;
#endif // #if defined( PRE_PARTY_DB )
	BYTE cCurrentCount;
	BYTE cMaxCount;
	BYTE cIsPrivateRoom;
	int nTargetMapIdx;
	TDUNGEONDIFFICULTY TargetDifficulty;
#if defined( PRE_PARTY_DB )
#else
	USHORT wMasterFatigue;
#endif // #if defined( PRE_PARTY_DB )
	BYTE cUseVoice;
	ePartyType PartyType;				//ePartyType
	BYTE cPartyNameLen;
	WCHAR wszBuf[PARTYNAMELENMAX];
#if defined( PRE_WORLDCOMBINE_PARTY )
	int nWorldCombinePartyTableIndex;		// ������Ƽ �ε���
#endif
#if defined( PRE_ADD_NEWCOMEBACK )
	bool bCheckComeBackParty;
#endif
};

struct SCPartyListInfo				// SC_PARTY / SC_PARTYLISTINFO
{
	int nRetCode;
#if defined( PRE_PARTY_DB )
	USHORT unReqPage;
	int iTotalCount;
	int iListCount;
	SPartyListInfo PartyListInfo[PARTYLISTOFFSET*2];	// ����¡ ����� ���̱� ���� �ѹ��� 2�������� ������.
#else
	USHORT nWholeCount;
	BYTE cCount;
	SPartyListInfo PartyListInfo[PARTYLISTOFFSET*2];
#endif // #if defined( PRE_PARTY_DB )
};

#if defined(PRE_MOD_REQ_JOIN_PARTY_ANSWER_MSG_APP)
struct SCPartyInfo				// SC_PARTY / SC_PARTYINFO
{
	int nRetCode;
	SPartyListInfo PartyInfo;	
};
#endif


//#if defined(_GAMESERVER) || defined(_VILLAGESERVER) || defined(_WINDOWS)
#if defined(_GAMESERVER) || defined(_VILLAGESERVER) || (defined(_WINDOWS) && !defined(_LAUNCHER))
struct CSRefreshGateInfo		// CS_PARTY / CS_REFRESHGATEINFO
{
	EtVector3 Position;
	bool boEnter;
};
#endif

struct SCRefreshGateInfo		// SC_PARTY / SC_REFRESHGATEINFO
{
	int nRet;
	BYTE cCount;
	char GateNo[PARTYMAX];
};

struct SCGateInfo				// SC_PARTY / SC_GATEINFO
{
	UINT nLeaderUID;
	char cGateNo;
	char cMapCount;
	char cPermitFlag[5];
	char cCanEnter[5][5];
};

struct TPartyMemberDefaultParts
{
#ifdef PRE_MOD_SYNCPACKET
	UINT nSessionID;
#endif		//#ifdef PRE_MOD_SYNCPACKET
	int iDefaultPartsIndex[4];
};

struct TPartyMemberWeaponOrder
{
#ifdef PRE_MOD_SYNCPACKET
	UINT nSessionID;
#endif		//#ifdef PRE_MOD_SYNCPACKET
	char cViewCashEquipBitmap[VIEWCASHEQUIPMAX_BITSIZE];
};

struct TPartyMemberEquip
{
	char cCount;
#ifdef PRE_MOD_SYNCPACKET
	UINT nSessionID;
#endif		//#ifdef PRE_MOD_SYNCPACKET
	TItemInfo EquipArray[EQUIPMAX];
};

struct TPartyMemberCashEquip
{
	char cCount;
#ifdef PRE_MOD_SYNCPACKET
	UINT nSessionID;
#endif		//#ifdef PRE_MOD_SYNCPACKET
	TItemInfo EquipArray[CASHEQUIPMAX];
};

struct TPartyMemberSkill
{
#ifdef PRE_MOD_SYNCPACKET
	UINT nSessionID;
#endif		//#ifdef PRE_MOD_SYNCPACKET
	char cCount;
	TSkill SkillArray[SKILLMAX];
};

struct TPartyMemberGlyphEquip
{
#ifdef PRE_MOD_SYNCPACKET
	UINT nSessionID;
#endif		//#ifdef PRE_MOD_SYNCPACKET
	char cCount;
	TItemInfo EquipArray[GLYPHMAX];
};

#if defined(PRE_ADD_TALISMAN_SYSTEM)
struct TPartyMemberTalismanEquip
{
#ifdef PRE_MOD_SYNCPACKET
	UINT nSessionID;
#endif		//#ifdef PRE_MOD_SYNCPACKET	
	char cCount;
	TItemInfo TalismanArray[TALISMAN_MAX];
};
#endif

struct CSStartStage
{

	bool bReturnVillage;
	TDUNGEONDIFFICULTY Difficulty;
	char cSelectDungeonIndex;
	UINT nFarmDBID;
	char cSelectMapIndex;
};


#define SCSelectDunGeon CSSelectDunGeon 		// SC_PARTY //SC_SELECTDUNGEON

struct SCPartyInfoModify			// SC_PARTY / SC_PARTYMODIFY
{
	int nRet;
};

struct SCPartyQuickJoinFail		// SC_PARTY / SC_PARTYQUICKJOINFAIL
{
	int nRet;
};

struct SCVerifyJoinParty			// SC_PARTY / SC_VERIFYJOINPARTY
{
	int nPartyIdx;
	int nChannelID;
	int nRet;
};

struct SCDefaultPartsData	// SC_PARTY //SC_DEFAULTPARTSDATA
{
	BYTE	cCount;
#ifdef PRE_MOD_SYNCPACKET
#else		//#ifdef PRE_MOD_SYNCPACKET
	USHORT	unStartIndex;
#endif		//#ifdef PRE_MOD_SYNCPACKET
	TPartyMemberDefaultParts DefaultParts[MAX_SEND_SYNCPLAYER];
};

struct SCWeaponOrderData
{
	BYTE	cCount;
#ifdef PRE_MOD_SYNCPACKET
#else		//#ifdef PRE_MOD_SYNCPACKET
	USHORT	unStartIndex;
#endif		//#ifdef PRE_MOD_SYNCPACKET
	TPartyMemberWeaponOrder WeaponOrder[MAX_SEND_SYNCPLAYER];
};

struct SCEquipData				// SC_PARTY //SC_EQUIPDATA
{
	BYTE	cCount;
#ifdef PRE_MOD_SYNCPACKET
#else		//#ifdef PRE_MOD_SYNCPACKET
	USHORT	unStartIndex;
#endif		//#ifdef PRE_MOD_SYNCPACKET
	TPartyMemberEquip MemberEquip[MAX_SEND_SYNCPLAYER];
};

struct SCCashEquipData
{
	BYTE	cCount;
#ifdef PRE_MOD_SYNCPACKET
#else		//#ifdef PRE_MOD_SYNCPACKET
	USHORT	unStartIndex;
#endif		//#ifdef PRE_MOD_SYNCPACKET
	TPartyMemberCashEquip MemberEquip[MAX_SEND_SYNCPLAYER];
};

struct SCGlyphEquipData
{
	BYTE	cCount;
#ifdef PRE_MOD_SYNCPACKET
#else		//#ifdef PRE_MOD_SYNCPACKET
	USHORT	unStartIndex;
#endif		//#ifdef PRE_MOD_SYNCPACKET
	TPartyMemberGlyphEquip MemberEquip[MAX_SEND_SYNCPLAYER];
};

#if defined(PRE_ADD_TALISMAN_SYSTEM)
struct SCTalismanEquipData
{
	BYTE	cCount;	
#ifdef PRE_MOD_SYNCPACKET
#else		//#ifdef PRE_MOD_SYNCPACKET
	USHORT	unStartIndex;
#endif		//#ifdef PRE_MOD_SYNCPACKET
	TPartyMemberTalismanEquip MemberEquip[MAX_SEND_SYNCPLAYER];
};
#endif

struct SCVehicleEquipData
{
	BYTE	cCount;
#ifdef PRE_MOD_SYNCPACKET
#else		//#ifdef PRE_MOD_SYNCPACKET
	USHORT	unStartIndex;
#endif		//#ifdef PRE_MOD_SYNCPACKET
	TVehicleCompact MemberVehicle[MAX_SEND_SYNCPLAYER];
};

struct SCSkillData				// SC_PARTY //SC_SKILLDATA
{
	BYTE	cCount;
#ifdef PRE_MOD_SYNCPACKET
#else		//#ifdef PRE_MOD_SYNCPACKET
	USHORT	unStartIndex;
#endif		//#ifdef PRE_MOD_SYNCPACKET
	TPartyMemberSkill MemberSkill[MAX_SEND_SYNCPLAYER];
};

struct SCMoveChannelFail		// SC_PARTY / SC_MOVECHANNELFAIL
{
	int nRetCode;
};

struct SCPartyInfoErr			// SC_PARTY / SC_PARTYINFO_ERROR
{
	int nRetCode;
};

struct SCStartStage				// SC_PARTY / SC_STARTSTAGE
{
#if defined(PRE_FIX_RANDOM_MAPINDEX)
	int nMapIndex;
#endif	// #if defined(PRE_FIX_RANDOM_MAPINDEX)
	int nRandomSeed;
	TDUNGEONDIFFICULTY Difficulty;
	short nExtendDropRate;			//�̺�Ʈ�� �߰� ���Ȯ��
};
	
struct SCStartStageDirect
{
	int nMapIndex;
	char cGateNo;
	int nRandomSeed;
	TDUNGEONDIFFICULTY Difficulty;
	short nExtendDropRate;			//�̺�Ʈ�� �߰� ���Ȯ��
};

struct SCCancelStage
{
	int nRetCode;
};

struct CSSelectStage
{
	char cSelectMapIndex;
};		

struct SCSelectStage
{
	char cSelectMapIndex;
};

struct SCPartyChangeLeader		// SC_PARTY  / SC_CHANGEPARTYLEADER
{
	UINT nNewLeaderSessionID;
};

struct SCPartyInvite				// SC_PARTY  / SC_PARTYINVITE
{
	TPARTYID PartyID;
	USHORT nPartyNameLen;
	BYTE cPartyInviterNameLen;
	BYTE cMaxUserCount;
	BYTE cCurUserCount;
	USHORT nAvrLevel;
	int nGameServerID;					//
	int nGameMapIdx;					//�̰� 0 ���� ũ�ٸ� ���Ӽ������� �ʴ�޴°Ŵ�
#if defined( PRE_PARTY_DB )
	int iPassword;
	WCHAR wszBuf[PARTYNAMELENMAX+NAMELENMAX];
#else
	BYTE cPartyPassLen;
	WCHAR wszBuf[PARTYNAMELENMAX + PARTYPASSWORDMAX + NAMELENMAX];
#endif // #if defined( PRE_PARTY_DB )
};

struct SCPartyInviteNotice		//SC_PARTY / SC_PARTYINVITENOTICE
{
	int nRetCode;
	WCHAR wszInvitedCharacterName[NAMELENMAX];		//�ʴ��ϴ� ����� ĳ�����̸�
};

struct SCPartyInviteFail			//SC_PARTY / SC_PARTYINVITEFAIL
{
	int nRetCode;
};

struct SCPartyAskJoinToLeader				//SC_PARTY / SC_ASKJOINPARTYTOLEADER
{
	char cJob;
	BYTE cLevel;
	WCHAR wszAskChracterName[NAMELENMAX];
};

struct SCPartyAskJoinResult			//SC_PARTY / SC_ASKJOINPARTYRESULT
{
	int nRet;
};

struct SCPartyAskJoinDecisionInfo		//SC_PARTY / SC_ASKJOINPARTYDECISIONINFO
{
	TPARTYID PartyID;
#if defined( PRE_PARTY_DB )
	int iPassword;
#else
	WCHAR wszPartyPassword[PARTYPASSWORDMAX];
#endif // #if defined( PRE_PARTY_DB )
};

struct SCPartyUpdateFieldUI				//SC_PARTY / SC_PARTYUPDATEFIELDUI
{
	UINT nSessionID;
	BYTE cLeader;						//0 none, 1 leader
};

//struct SCPartySwapMemberIndex			//SC_PARTY / SC_SWAPMEMBERINDEX
//{
//	BYTE cOriginIndex;
//	BYTE cSwapIndex;
//	int nRetCode;
//};

struct TPartyMemberInfo
{
	BYTE cUserLevel;
	USHORT wJob;
	BYTE cClassID;
	WCHAR wszName[NAMELENMAX];
};

struct SCPartyMemberInfo		//SC_PARTY / SC_PARTYMEMBER_INFO
{
#if defined( PRE_PARTY_DB )
	TPARTYID PartyID;
	ePartyType PartyType;
	short nCount;
	Party::MemberData MemberData[PARTYCOUNTMAX];
#else
	TPARTYID PartyID;
	BYTE cLeaderIdx;
	BYTE cIsRaidParty;
	BYTE cCount;
	TPartyMemberInfo Info[PARTYMAX];
#endif // #if defined( PRE_PARTY_DB )
};

struct SCPartyInviteDenied		//SC_PARTY / SC_PARTYINVITEDENIED
{
	int nRetCode;
	WCHAR wszCharName[NAMELENMAX];
};

struct SCPartyKickedMember	//SC_PARTY / SC_PARTYMEMBERKICKED
{
	UINT nSessionID;
	int nRetCode;
};

struct SCSwapPartyLeader		//SC_PARTY / SC_PARTYLEADERSWAP
{
	UINT nSessionID;
	int nRetCode;
};

#if defined(_GAMESERVER) || defined(_VILLAGESERVER) || (defined(_WINDOWS) && !defined(_LAUNCHER))
struct SCPartyMemberMove		// SC_PARTY / SC_PARTYMEMBER_MOVE
{
	UINT nSessionID;
	EtVector3 Position;
};
#endif

struct SCPartyMemberPart		// SC_PARTY / SC_PARTYMEMBER_PART
{
	UINT nSessionID;
	int nExp;
	BYTE cLevel;
	BYTE cJobArray[JOBMAX];
	int	nHairID;
	int	nFaceID;
	DWORD dwHairColor;
	DWORD dwEyeColor;
	DWORD dwSkinColor;
	USHORT wFatigue;
	int nHelmetID;
	int	nEarringID;
};

struct TPartyEtcData
{
#ifdef PRE_MOD_SYNCPACKET
	UINT nSessionID;
#endif		//#ifdef PRE_MOD_SYNCPACKET
	int		nSelectAppellation;
	short	nCoverAppellation;
	BYTE	cGMTrace;
#if defined(PRE_ADD_VIP)
	bool	bVIP;
#endif	// #if defined(PRE_ADD_VIP)
	char	cAccountLevel; // ��������(eAccountLevel)
};

struct SCEtcData
{
	BYTE	cCount;
#ifdef PRE_MOD_SYNCPACKET
#else		//#ifdef PRE_MOD_SYNCPACKET
	USHORT	unStartIndex;
#endif		//#ifdef PRE_MOD_SYNCPACKET
	TPartyEtcData EtcData[MAX_SEND_SYNCPLAYER];
};

// SC_PARTY / SC_GUILDDATA
struct TPartyMemberGuild
{
public:
	struct TPartyMemberGuildView : public TGuildSelfView
	{
	public:
		TPartyMemberGuildView()
		{
			Reset();
		}
		void Set(UINT nMemberSessionID, const TGuildSelfView& pGuildSelfView)
		{
			nSessionID = nMemberSessionID;
			TGuildSelfView::Set(pGuildSelfView);
		}
		void Reset()
		{
			nSessionID = 0;
			TGuildSelfView::Reset();
		}
		bool IsSet() {
			return((0 != nSessionID) && TGuildSelfView::IsSet());
		}
	public:
		UINT nSessionID;
	};
public:
	TPartyMemberGuild()
	{
		Reset();
	}
	void Set()
	{
		nCount = 0;
	}
	void Reset()
	{
		nCount = 0;
		unStartIndex = 0;
	}	
	bool Add(UINT nSessionID, const TGuildSelfView& pGuildSelfView)
	{
		if (PARTYMAX <= nCount) {
			return false;
		}

		List[nCount].Set(nSessionID, pGuildSelfView);
		++nCount;

		return true;
	}

	const TPartyMemberGuildView* At(int iIndex) const
	{
		if (0 > iIndex || _countof(List) <= iIndex)
			return NULL;
		
		return(&List[iIndex]);
	}
public:
	int nCount;	// ��Ƽ�� ��� �ð�ȭ ���� ��� ����
	USHORT unStartIndex;
	TPartyMemberGuildView List[MAX_SEND_SYNCPLAYER];	// ��Ƽ�� ��� �ð�ȭ ���� ���
};

struct SCRollDiceForGetReverseItem
{
	UINT nSessionID;
	bool bRoll;
	int	nDiceValue;
};

struct SCResultGetReverseItem
{
	UINT nSessionID;
	TItem itemInfo;
	DWORD dropItemUniqueId;
};

struct SCPartyJoinGetReversionItemInfo
{
	TItem itemInfo;
	DWORD dropItemUniqueId;
};

struct SCMemberAbsent		//SC_PARTY / SC_ABSENT
{
	UINT nSessionID;
	BYTE cAbsentFlag;		//0 absent, 1 present
};

struct SCVerifyDungeon		//SC_PARTY / SC_VERIFYTARGETDUNGEON
{	
	BYTE cVerifyFlag[5];		//0���� ���������� �������//0 not allowed, 1 allowed
};

struct SCPartyBonusValue	//SC_PARTY / SC_PARTYBONUSVALUE
{
	BYTE cUpkeepBonus;		//%����
	BYTE cFriendBonus;		//%����
	BYTE cUpKeepBonusLimit;	//�������ʽ� �ƽ� ��ȣ�� �̰� 0�̻��̸� ����á�ٰ� ����ֽø� ����~ 
	//�ϴ��� �������־ ���ÿ��� 0�� ���� Ŭ����������� �ǴܵǾ 0�̻��� �����ϴ�. �̰� �޾����� 0�� �ƴϸ� ���� ����ֽø鵩
#ifdef PRE_ADD_BEGINNERGUILD
	BYTE cBeginnerGuildBonus;	//�ʺ��������� ��Ƽ�ϰ��
#endif		//#ifdef PRE_ADD_BEGINNERGUILD
};

struct SCPartyCheckLastDungeonInfo
{
	WCHAR wszPartyName[PARTYNAMELENMAX];
};

struct CSPartyConfirmLastDungeonInfo
{
	bool bIsOK;
};

struct SCPartyConfirmLastDungeonInfo
{
	int iRet;
};

#if defined (PRE_ADD_BESTFRIEND)
struct TPartyBestFriend
{
	int nSessionID;
	INT64 biBFItemSerial;
	WCHAR wszBFName[NAMELENMAX];
};

struct SCPartyBestFriend
{
	int nCount;
	TPartyBestFriend List[PARTYMAX];
};
#endif

//-------------------------------------------------
//	ITEM (CS_ITEM, SC_ITEM)
//-------------------------------------------------

struct SCEquipList
{
	char cCount;
	char cCashCount;
	char cGlyphCount;
#if defined(PRE_ADD_TALISMAN_SYSTEM)
	char cTalismanCount;
	int	nTalismanSlotOpenFlag;
#endif

#if defined(PRE_ADD_TALISMAN_SYSTEM)
	TItemInfo EquipArray[EQUIPMAX+CASHEQUIPMAX+GLYPHMAX+TALISMAN_MAX];
#else
	TItemInfo EquipArray[EQUIPMAX+CASHEQUIPMAX+GLYPHMAX];
#endif
};

struct SCInvenList					// SC_ITEM / SC_INVENLIST
{
	BYTE cInvenMaxCount;	// ����� �����ִ��� (�Ϲ��κ���. �Ⱓ�� ���� �ȵǾ�����)
	BYTE cInvenCount;
#if defined(PRE_PERIOD_INVENTORY)
	TItemInfo Inven[INVENTORYTOTALMAX];
#else	// #if defined(PRE_PERIOD_INVENTORY)
	TItemInfo Inven[INVENTORYMAX];
#endif	// #if defined(PRE_PERIOD_INVENTORY)
};

struct SCCashInvenList
{
	int nTotalCashInventoryCount;
	BYTE cInvenCount;
	TItem Inven[CASHINVENTORYMAX];
};

struct SCBuyedItemCount
{
	short count;
	ItemCount items[INVENTORYMAX + WAREHOUSEMAX];
};

// api_trigger_InvenToEquip Ʈ���ſ��� ����ϹǷ� �ش籸��ü ����� ���� ����Ǿ�� �մϴ�!!!
struct CSMoveItem				// CS_ITEM / CS_MOVEITEM,
{
	char cMoveType;	// eItemMoveType ���� (MoveType_EquipToInven, MoveType_InvenToEquip)
	BYTE cSrcIndex;
	INT64 biSrcItemSerial;
	BYTE cDestIndex;
	short wCount;
};

struct SCMoveItem				// SC_ITEM / SC_MOVEITEM,
{
	char cMoveType;	// eItemMoveType ���� (MoveType_EquipToInven, MoveType_InvenToEquip...)
	int nRetCode;
	TItemInfo SrcItem;
	TItemInfo DestItem;
};

// api_trigger_UnRideVehicle Ʈ���ſ��� ����ϹǷ� �ش籸��ü ����� ���� ����Ǿ�� �մϴ�!!!
struct CSMoveCashItem			// CS_ITEM / CS_MOVECASHITEM,
{
	char cMoveType;	// eItemMoveType ���� (MoveType_CashEquipToCashInven, MoveType_CashInvenToCashEquip)
	BYTE cCashEquipIndex;
	INT64 biEquipItemSerial;
	INT64 biCashInvenSerial;
	short wCount;
};

struct SCMoveCashItem			// SC_ITEM / SC_MOVECASHITEM,
{
	char cMoveType;	// eItemMoveType ���� (MoveType_CashEquipToCashInven, MoveType_CashInvenToCashEquip)
	int nRetCode;
	TItemInfo CashEquipItem;
	TItem CashInvenItem;
};

#if defined(PRE_ADD_SERVER_WAREHOUSE)
struct SCServerWareList			// SC_ITEM / SC_SERVERWARE_LIST
{
	BYTE cWareCount;
	TItem WareItems[WAREHOUSEMAX];
};

struct SCCashServerWareList			// SC_ITEM / SC_CASHSERVERWARE_LIST
{
	BYTE cWareCount;
	TItem WareCashItems[WAREHOUSEMAX];
};

struct CSMoveServerWare			// CS_ITEM / CS_MOVE_SERVERWARE
{
	char cMoveType;	// eItemMoveType ���� (MoveType_InvenToServerWare, MoveType_ServerWareToInven)
	BYTE cInvenIndex;
	INT64 biInvenItemSerial;
	INT64 biWareItemSerial;	
	short wCount;
};

struct SCMoveServerWare			// SC_ITEM / SC_MOVE_SERVERWARE
{
	char cMoveType;	// eItemMoveType ���� (MoveType_CashEquipToCashInven, MoveType_CashInvenToCashEquip)
	int nRetCode;
	TItemInfo InvenItem;
	TItem ServerWareItem;
};

struct CSMoveCashServerWare		// CS_ITEM / CS_MOVE_CASHSERVERWARE
{
	char cMoveType;	// eItemMoveType ���� (MoveType_CashToServerWare, MoveType_ServerWareToCash)
	INT64 biItemSerial;		
};

struct SCMoveCashServerWare			// SC_ITEM / SC_MOVE_CASHSERVERWARE
{
	char cMoveType;	// eItemMoveType ���� (MoveType_CashToServerWare, MoveType_ServerWareToCash)
	int nRetCode;
	INT64 biSrcItemSerial;
	TItem SrcItem;
	TItem DestItem;
};
#endif //#if defined(PRE_ADD_SERVER_WAREHOUSE)

struct CSMoveGuildItem			// CS_ITEM / CS_MOVEGUILDITEM,
{
	char	cMoveType;				// eItemMoveType ���� (MoveType_InvenToGuildWare, MoveType_GuildWareToInven)
	BYTE	cDestIndex;				// ���� �ε��� (���â�� <--> �κ�)
	INT64	biDestItemSerial;		// �̺�Ʈ�� ���� ������ �ø���
	BYTE	cSrcIndex;				// ��� �ε��� (���â�� <--> �κ�)
	INT64	biSrcItemSerial;		// �̺�Ʈ �߻��� ������
	short	wCount;					// �ش� ����
};

struct SCMoveGuildItem			// SC_ITEM / SC_MOVEGUILDITEM,
{
	char cMoveType;	// eItemMoveType ���� (MoveType_InvenToGuildWare, MoveType_GuildWareToInven)

	int nRetCode;
	int nTakeItemCount;

	INT64	biSrcSerial;
	INT64   biDestSerial;

	TItemInfo DestItem;
	TItemInfo SrcItem;
};

struct SCRefreshGuildItem		// SC_ITEM / SC_REFRESH_GUILDITEM
{
	char cMoveType;	// eItemMoveType ���� (MoveType_InvenToGuildWare, MoveType_GuildWareToInven)

	INT64	biSrcSerial;
	INT64   biDestSerial;

	TItemInfo SrcItem;
	TItemInfo DestItem;	
};

struct SCRefreshGuildCoin	 // SC_ITEM / SC_REFRESH_GUILDCOIN
{
	INT64 biTotalCoin;		// ��ü�ݾ�
};

struct CSMoveGuildCoin			// CS_ITEM / CS_MOVEGUILDCOIN
{
	char	cMoveType;			// eItemMoveType ���� (MoveType_InvenToGuildWare, MoveType_GuildWareToInven)
	INT64	nMoveCoin;
};

struct SCMoveGuildCoin			// SC_ITEM / SC_MOVEGUILDCOIN
{
	int nRet;
	char cMoveType;				// eItemMoveType ���� (MoveType_InvenToGuildWare, MoveType_GuildWareToInven)
	INT64 nMoveCoin;
	INT64 nInventoryCoin;
	INT64 nWarehouseCoin;
	INT64 nWithdrawCoin;
};

struct CSPickUp					// CS_ITEM / CS_PICKUP,
{
	int nItemID;
	short wCount;
};

struct SCPickUp					// SC_ITEM / SC_PICKUP,
{
	int nRetCode;
	short wCount;
	TItemInfo PickUpItem;
};

struct CSRemoveItem				// CS_ITEM / CS_REMOVEITEM,
{
	char cType;				// eItemPositionType 1: Equip 2: Inven 3: Ware
	BYTE cSlotIndex;
	short wCount;
	INT64 biItemSerial;
};

struct SCRemoveItem				// SC_ITEM / SC_REMOVEITEM,
{
	int nRetCode;
	char cType;				// eItemPositionType 1: Equip 2: Inven 3: Ware
	TItemInfo ItemInfo;
};

struct SCChangeEquip			// SC_ITEM / SC_CHANGEEQUIP
{
	UINT nSessionID;
	TItemInfo ItemInfo;
};

struct SCChangeCashEquip			// SC_ITEM / SC_CHANGECASHEQUIP
{
	UINT nSessionID;
	TItemInfo ItemInfo;
};

struct SCRefreshInven			// SC_ITEM / SC_REFRESHINVEN
{
	bool bNewSign;	// newǥ�� �Ҳ��� ���Ҳ���
	TItemInfo ItemInfo;
};

struct SCRefreshCashInven		// SC_ITEM / SC_REFRESHINVEN
{
	bool bNewSign;	// newǥ�� �Ҳ��� ���Ҳ���
	int nCount;
	TItem ItemList[CASHINVENTORYMAX];
};

struct SCRefreshEquip			// SC_ITEM / SC_REFRESHEQUIP
{
	TItemInfo ItemInfo;
#if defined(PRE_ADD_EQUIPED_ITEM_ENCHANT)
	bool bRefreshState;
#endif
};

struct SCRefreshWarehouse			// SC_ITEM / SC_REFRESHWAREHOUSE
{
	TItemInfo ItemInfo;
};
struct SCCreateDropItem			// SC_ITEM / SC_CREATE_DROPITEM
{
	UINT nSessionID;
	float fpos[3];
	UINT nItemID;
	UINT nRandomSeed;
	char cOption;
	USHORT nCount;
	USHORT nRotate;
	UINT nOwnerSessionID;
};

struct SCCreateDropItemList
{
	int nCount;
	SCCreateDropItem sDropItem[20];	//	�ѹ��� �ִ� ���� �� �ִ� DropItem �ִ� ����
};

struct SCDropItem				// SC_ITEM / SC_DROPITEM_MSG
{
	UINT nSessionID;
	USHORT nItemProtocol;
	char cBuf[16];
};

struct SCWarehouseList			// SC_ITEM / SC_WAREHOUSELIST
{
	INT64 nWarehouseCoin;
	BYTE cWareMaxCount;	// â�� max���� ����� ���� �ִ��� (�Ϲ�â��. �Ⱓ�� ���� �ȵǾ�����)
	BYTE cWareCount;
#if defined(PRE_PERIOD_INVENTORY)
	TItemInfo Warehouse[WAREHOUSETOTALMAX];
#else	// #if defined(PRE_PERIOD_INVENTORY)
	TItemInfo Warehouse[WAREHOUSEMAX];
#endif	// #if defined(PRE_PERIOD_INVENTORY)
};

struct CSUseItem				// CS_ITEM / CS_USEITEM
{
	char cInvenType;			// eItemPositionType
	BYTE cInvenIndex;			// �Ϲ��κ��϶��� index�־��ָ� �� (ĳ���� ������������)
	INT64 biInvenSerial;
};

struct SCUseItem				// SC_ITEM / SC_USEITEM
{
	char cInvenType;			// eItemPositionType
	BYTE cInvenIndex;			// �Ϲ��κ��϶��� index�־��ָ� �� (ĳ���� ������������)
	INT64 biInvenSerial;
};

struct CSCompleteRandomItem
{
	char cInvenType;			// eItemPositionType
	BYTE cInvenIndex;			// �Ϲ��κ��϶��� index�־��ָ� �� (ĳ���� ������������)
	INT64 biInvenSerial;
};

// ���Ŀ� NPC ���� ����Ʈ �����޶�� �־�� �� ��..
// ������ �ӽ÷� CompoundTableID ����
struct CSEmblemCompoundOpenReq	// CS_ITEM / CS_EMBLEM_COMPOUND_OPEN_REQ
{
	BYTE cPlateItemSlotIndex;
	INT64 biPlateItemSerialID;

	int nCompoundTableID;
};

struct SCEmblemCompundOpenRes	// CS_ITEM / SC_EMBLEM_COMPOUND_OPEN_RES
{
	int nResult;
};

struct CSItemCompoundOpenReq	// CS_ITEM / CS_ITEM_COMPOUND_OPEN_REQ
{
	int nCompoundTableID;
	INT64 biNeedItemSerialID;
};

struct SCItemCompoundOpenRes	// CS_ITEM / CS_ITEM_COMPOUND_OPEN_RES
{
	int nResult;
};

struct CSCompoundEmblemReq
{
	BYTE cPlateItemSlotIndex;
	INT64 biPlateItemSerialID;

	int nCompoundTableID;
};

struct CSCompoundItemReq		// CS_ITEM / CS_COMPOUND_EMBLEM_REQ
{
	int nCompoundTableID;
	INT64 biNeedItemSerialID;
};

struct SCCompoundEmblemRes		// CS_ITEM / CS_COMPOUND_EMBLEM_RES
{
	short sResult;
	int nResultItemID;
};

struct SCCompoundItemRes		// SC_ITEM / SC_ITEM_COMPOUND_RES
{
	short sResult;
	int nResultItemID;
	char cLevel;
	char cOption;
};

struct CSEmblemCompoundCancelReq	// CS_ITEM / CS_EMBLEM_COMPOUND_CANCEL_REQ
{
	int nCompoundTableID;
};

struct SCEmblemCompoundCancelRes	// CS_ITEM / CS_EMBLEM_COMPOUND_CANCEL_RES
{
	int nResult;
};

struct CSItemCompoundCancelReq		// CS_ITEM / CS_ITEM_COMPOUND_CANCEL_REQ
{
	int nCompoundTableID;
};

struct SCItemCompoundCancelRes		// CS_ITEM / CS_ITEM_COMPOUND_CANCEL_RES
{
	int nResult;
};

struct CSItemDisjointReq		// CS_ITEM / CS_DISJOINT_REQ 
{
	UINT nNpcUniqueID;			// ���Ӽ������� ó���ؾ��ϰ�, Npc��ġ���� �˾ƾ��ؼ� UniqueID �߰��� ������.
	BYTE cSlotIndex;
	INT64 biItemSerial;
	short nCount;
};

struct SCItemDisjointRes		// SC_ITEM / SC_DISJOINT_RES
{
	int nItemID;
	bool bSuccess;
};

struct SCInventoryMaxCount		// SC_ITEM / SC_INVENTORYMAXCOUNT
{
	BYTE cInvenMaxCount;	// ����� �����ִ���
};

struct SCGlyphExtendCount
{
	BYTE cCount;
};
struct SCGlyphExpireData
{
	BYTE cCount;
	TCashGlyphData TGlyphData[CASHGLYPHSLOTMAX];
};

struct SCWarehouseMaxCount		// SC_ITEM / SC_WAREHOUSEMAXCOUNT
{
	BYTE cWareMaxCount;	// â�� max���� ����� ���� �ִ���
};

struct SCQuestInvenList			// SC_ITEM / SC_QUESTINVENLIST
{
	char cQuestInvenCount;
	TItemInfo QuestInventory[QUESTINVENTORYMAX];
};

struct SCRefreshQuestInven		// SC_ITEM / SC_REFRESHQUESTINVEN
{
	TItemInfo ItemInfo;
};

struct SCRebirthCoin			// SC_ITEM / SC_REBIRTH_COIN
{
	int nUsableRebirthCoin;		// ���� ���� ���� ���ΰ���
	BYTE cRebirthCoin;			// ��Ȱ���� ��������
	USHORT wRebirthCashCoin;	// ĳ���� ��Ȱ���� ��������
	int nRet;						// ���� ��� ���� ����
	int nRebirthType;
	UINT nSessionID;
	BYTE cPCBangRebirthCoin;			// ��Ȱ���� ��������
#if defined(PRE_ADD_VIP)
	BYTE cVIPRebirthCoin;
#endif	// #if defined(PRE_ADD_VIP)
};

struct SCRefreshRebirthCoin		// SC_PARTY / SC_REFRESHREBIRTHCOIN
{
	UINT nSessionID;
	BYTE cRebirthCoin;
	BYTE cPCBangRebirthCoin;
	short nCashRebirthCoin;
#if defined(PRE_ADD_VIP)
	BYTE cVIPRebirthCoin;
#endif	// #if defined(PRE_ADD_VIP)
};

#if defined(PRE_ADD_REBIRTH_EVENT)
struct SCRebirthMaxCoin		// SC_CHAR / SC_REBIRTH_MAXCOIN
{
	int nRebirthMaxCoin;
};
#endif

struct SCRepairResult			// SC_ITEM / SC_REPAIR_EQUIP, SC_REPAIR_ALL
{
	int nRet;
};

struct CSMoveCoin				// CS_ITEM / CS_MOVEITEM
{
	char cMoveType;	// eItemMoveType ���� (MoveType_EquipToInven, MoveType_InvenToEquip)
	INT64 nMoveCoin;
};

struct SCMoveCoin				// SC_ITEM / SC_MOVECOIN
{
	char cMoveType;	// eItemMoveType ���� (MoveType_EquipToInven, MoveType_InvenToEquip)
	INT64 nMoveCoin;
	INT64 nInventoryCoin;
	INT64 nWarehouseCoin;
	int nRet;
};

struct SCRequestRandomItem
{
	char cInvenType;			// eItemPositionType
	BYTE cInvenIndex;			// �Ϲ��κ��϶��� index�־��ָ� �� (ĳ���� ������������)
	INT64 biInvenSerial;
	int nRet;
};

struct SCCompleteRandomItem
{
	char cInvenType;			// eItemPositionType
	char cLevel;
	int nResultItemID;
	int nItemCount;
	int nRet;
};

struct SCChangeEternityItem		// SC_ITEM / SC_ETERNITYITEM
{
	UINT nSessionID;
	char cType;
	USHORT wEternityItem;
};

struct SCGlyphTimeInfo			// SC_ITEM / SC_GLYPHTIMEINFO
{
	int nDelayTime;			// glyph ���õ� cooltime
	int nRemainTime;
};

struct TSortSlot
{
	BYTE cCurrent;
	BYTE cNew;
};

struct CSSortInventory			// CS_ITEM / CS_SORTINVENTORY, CS_SORTPERIODINVENTORY 
{
	BYTE cTotalCount;
	TSortSlot SlotInfo[INVENTORYMAX];
};

struct SCSortInventory			// SC_ITEM / SC_SORTINVENTORY, SC_SORTPERIODINVENTORY
{
	int nRet;
};

struct SCDecreaseDurabilityInventory //SC_ITEM / SC_DECREASE_DURABILITY_INVENTORY
{
	char cType;
	union {
		int nAbsolute;
		float fRatio;
	};
};

struct CSSortWarehouse			// CS_ITEM / CS_SORTWAREHOUSE, CS_SORTPERIODWAREHOUSE 
{
	BYTE cTotalCount;
	TSortSlot SlotInfo[WAREHOUSEMAX];
};

struct SCSortWarehouse			// SC_ITEM / SC_SORTWAREHOUSE, SC_SORTPERIODWAREHOUSE
{
	int nRet;
};

#if defined( PRE_ADD_STAGE_USECOUNT_ITEM )
struct SCStageUseLimitItem	// SC_ITEM / SC_STAGE_USELIMITITEM
{
	int nItemID;
	int nUseCount;
};
#endif

// ��ȭ
/*
CS_ENCHANT			��ȭ�� ������ �÷����� ��ȭ��ư ������
SC_ENCHANT			�������� ����ް� �����󳪿���
CS_ENCHANTCOMPLETE	������ ������ ��ȭ�Ϸ��� ������
SC_ENCHANTCOMPLETE	��ȭ��� �˷��ְ� (��)
CS_ENCHANTCANCEL	�߰��� ����Ҳ��� �����ְ�
SC_ENCHANTCANCEL	
*/

struct CSEnchantItem
{
#if defined(PRE_ADD_EQUIPED_ITEM_ENCHANT)
	BYTE cItemIndex;
	bool bEnchantEquipedItem;
#else
	BYTE cInvenIndex;
#endif
	INT64 biItemSerial;
#if defined(PRE_ADD_ENCHANTSHIELD_CASHITEM) // ���Ŀ� ��ȭ ���� �������� �� �߰��ɼ� �־ �����մϴ�.
	char cEnchantGuardType;				// ��ȭ ��ȣ Ÿ�� 0-NONE, 1-��ȭ��ȣ����(�ı�x), 2-������ȭ��ȣ����(�ı�x,�ٿ�x) eEnchantGuardType
#else
	bool bUseEnchantGuard;
#endif	// #if defined(PRE_ADD_ENCHANTSHIELD_CASHITEM)
};

struct SCEnchantItem
{
#if defined(PRE_ADD_EQUIPED_ITEM_ENCHANT)
	BYTE cItemIndex;
	bool bEnchantEquipedItem;
#else
	BYTE cInvenIndex;
#endif
	int nRet;
};

struct SCEnchantComplete
{
	int nItemID;
	char cLevel;
	char cOption;
	int nRet;
};

struct SCEnchantCancel
{
	int nRet;
};

struct SCBroadcastingEffect			// SC_ITEM / SC_BROADCASTINGEFFECT
{
	UINT nSessionID;
	char cType;		// eBroadcastingEffect
	char cState;	// eBroadcastingEffect
};

// SC_CHANGEGLYPH,					// �ֺ��ֵ����� glyph���� �����ֱ�
struct SCChangeGlyph
{
	UINT nSessionID;
	TItemInfo ItemInfo;
};

struct SCVehicleEquipList			// SC_ITEM, SC_VEHICLEEQUIPLIST
{
	TVehicleCompact VehicleEquip;
};

struct SCVehicleInvenList			// SC_ITEM, SC_VEHICLEINVENLIST
{
	int nTotalInventoryCount;
	BYTE cInvenCount;
	TVehicleCompact VehicleInven[VEHICLEINVENTORYPAGEMAX];
};

struct SCChangeVehicleParts		// SC_ITEM, SC_CHANGEVEHICLEPARTS, SC_CHANGEPETPARTS
{
	UINT nSessionID;
	char cSlotIndex;
	TVehicleItem Equip;
};

struct SCChangeVehicleColor		// SC_ITEM, SC_CHANGEVEHICLECOLOR
{
	UINT nSessionID;
	DWORD dwColor;
	char cSlotIndex;
	INT64 biSerial;
};

struct SCRefreshVehicleInven		// SC_ITEM / SC_REFRESHVEHICLEINVEN
{
	bool bNewSign;	// newǥ�� �Ҳ��� ���Ҳ���
	int nCount;
	TVehicleCompact ItemList[VEHICLEINVENTORYPAGEMAX];
};

struct SCAddPetExp	// SC_ITEM / SC_ADDPETEXP
{
	UINT nSessionID;
	INT64 biPetSerial;
	int nExp;
};

struct SCChangePetBody			// SC_ITEM, SC_CHANGEPETBODY
{
	UINT nSessionID;
	TVehicleCompact PetInfo;
};

struct SCPetCurrentSatiety
{
	INT64 biPetSerial;
	int nCurrentSatiety;
};
struct SCPetFoodEat
{
	int nResult;
	int nIncSatiety;
};

#if defined(PRE_ADD_EXPUP_ITEM)
struct SCDailyLimitItemError
{
	int nWeightTableIndex;
};
#endif

struct CSRemoveCash		// CS_ITEM, CS_REMOVECASH
{
	INT64 biItemSerial;
	bool bRecovery;
};

struct SCRemoveCash		// SC_ITEM, SC_REMOVECASH
{
	INT64 biItemSerial;
	bool bEternity;		// ����������
	__time64_t tExpireDate;
	bool bExpireComplete;
	int nRet;
};

struct SCModItemExpireDate
{
	int nRet;
};

#if defined( PRE_ADD_LIMITED_SHOP )
struct SCLimitedShopItemData	// SC_ITEM, SC_LIMITEDSHOPITEMDATA
{
	short count;
	bool bAllClear;
	LimitedShop::LimitedItemData items[INVENTORYMAX + WAREHOUSEMAX];
};
#endif

#if defined(PRE_ADD_TALISMAN_SYSTEM)

struct CSOpenTalismanSlot	// CS_ITEM / CS_OPEN_TALISMANSLOT
{
	int nSlotIndex;
};

struct SCOpenTalismanSlot	// SC_ITEM / SC_OPEN_TALISMANSLOT
{
	int nRet;
	int nTalismanOpenFlag;
};

struct SCChangeTalisman	// SC_ITEM / SC_CHANGETALISMAN
{
	UINT nSessionID;
	TItemInfo ItemInfo;
};

struct SCTalismanExpireData	// SC_ITEM / SC_TALISMANEXPIREDATA
{
	bool bActivate;				// Ż������ ĳ�� Ȯ�� ���� Ȱ��ȭ ����
	__time64_t	tTalismanExpireDate;	// Ż������ ���� �Ⱓ
};
#endif

#if defined(PRE_ADD_EQUIPLOCK)
struct CSItemLockReq	// CS_ITEM / CS_ITEM_LOCK_REQ, CS_ITEM_UNLOCK_REQ
{
	bool IsCashEquip;
	BYTE nItemSlotIndex; // Lock(UnLock)��ų Item�� �ε��� ��ȣ	
	INT64 biItemSerial;	// �ش� ������ Serial
};

struct SCItemLockRes	// SC_ITEM / SC_ITEM_LOCK_RES, SC_ITEM_UNLOCK_RES
{
	int nRet; //�����
	bool IsCashEquip;
	EquipItemLock::TLockItemInfo LockItems;
};

struct SCEquipItemLockList	// SC_ITEM / SC_EQUIPITEM_LOCKLIST
{
	int nEquipCount;	//�Ϲ� ��� ����
	int nCashEquipCount; //ĳ�� ��� ����
	EquipItemLock::TLockItemInfo LockItems[EQUIPMAX+CASHEQUIPMAX];
};
#endif	// #if defined(PRE_ADD_EQUIPLOCK)

#if defined(PRE_PERIOD_INVENTORY)
struct SCPeriodInventory		// SC_ITEM / SC_PERIODINVENTORY
{
	bool bEnable;
	__time64_t tExpireDate;
};

struct SCPeriodWarehouse		// SC_ITEM / SC_PERIODWAREHOUSE
{
	bool bEnable;
	__time64_t tExpireDate;
};
#endif	// #if defined(PRE_PERIOD_INVENTORY)


//--------------------------------------------------------------------------------------------------
//	Item Goods (CS_ITEMGOODS, SC_ITEMGOODS)
//--------------------------------------------------------------------------------------------------

struct CSUnSealItem
{
	BYTE cInvenIndex;
	INT64 biInvenSerial;
};

struct SCUnSealItem
{
	int nRet;
	BYTE cInvenIndex;
};

struct CSSealItem
{
	BYTE cInvenIndex;
	INT64 biInvenSerial;
	INT64 biItemSerial;
};

struct SCSealItem
{
	int nRet;
	BYTE cInvenIndex;
};

struct SCRequestCashWorldMsg
{
	INT64 biItemSerial;
	char cType;
};

// Ŭ���̾�Ʈ���� ��û�� ��ų ���� ������ ��뿡 ���� ��� �������� �ƴ��� ����.
struct SCCanUseSkillResetCashItem
{
	INT64 biItemSerial;
	int nRet;
	int nRewardSP;
};

// Ŭ���̾�Ʈ���� ���������� ��ų ���� ������ ��� ��Ŷ�� ����
struct CSUseSkillResetCashItemReq
{
#if defined(PRE_MOD_SKILLRESETITEM)
	char cInvenType;			// eItemPositionType
	BYTE cInvenIndex;			// �Ϲ��κ��϶��� index�־��ָ� �� (ĳ���� ������������)
#endif
	INT64 biItemSerial;
	char cSkillPage;
};

// ��ų ���� ĳ���� ����� ����� Ŭ��� �뺸
struct SCUseSkillResetCashItemRes
{
	int nRet;
	int nResultSP;		// ���� ����� ���� ��ų ����Ʈ.
	char cSkillPage;
};

#ifdef PRE_ADD_CHANGEJOB_CASHITEM
struct CSUseChangeJobCashItemReq
{
	INT64 biItemSerial;
	int iJobToChange;
};

struct SCUseChangeJobCashItemRes
{
	int nRet;
	int nFirstJobID;
	int nSecondJobID;
	int nResultSkillPoint;
};
#endif // #ifdef PRE_ADD_CHANGEJOB_CASHITEM

// SC_CHANGECOLOR
struct SCChangeColor
{
	UINT nSessionID;
	char cItemType;
	DWORD dwColor;
};

#ifdef PRE_ADD_GACHA_JAPAN
struct SCGachaShopOpen_JP	// SC_ITEMGOODS / SC_JP_GACHAPON_OPEN_SHOP_RES
{
	int nGachaShopIndex;
};

struct CSGachaRequest_JP	// SC_ITEMGOODS / CS_JP_GACHAPON_REQ
{
	int nJobClassID;
	int nSelectedPart;
};

struct SCGachaRes_JP		// SC_ITEMGOODS / SC_JP_GACHAPON_RES
{
	int nRet;
	int nResultItemID;
};
#endif

//	�ڽ�Ƭ �ռ�
struct SCCosMixOpen
{
	int nRet;
};

struct SCCosMixClose
{
	int nRet;
	bool bComplete;
};

struct CSCosMix
{
	INT64	nStuffSerialID[MAXCOSMIXSTUFF];
	UINT	nResultItemID;
	char	cOption;
};

struct SCCosMix
{
	int	 nRet;
	UINT nResultItemID;
	char cOption;
};

struct SCCosDesignMixOpen
{
	int nRet;
};

struct SCCosDesignMixClose
{
	int nRet;
	bool bComplete;
};

struct CSCosDesignMix
{
	INT64	nStuffSerialID[eCDMST_MAX];
};

struct SCCosDesignMix
{
	int nRet;
	INT64 biItemSertial;
	int nLookItemID;

};

#ifdef PRE_ADD_COSRANDMIX
struct SCCosRandomMixOpen
{
	int nRet;
#ifdef PRE_ADD_COSRANDMIX_ACCESSORY
	int nOpenType;
#endif
};

struct SCCosRandomMixClose
{
	int nRet;
	bool bComplete;
};

struct CSCosRandomMix
{
	INT64 nStuffSerialID[MAX_COSTUME_RANDOMMIX_STUFF];
	UINT nResultItemID;
};

struct SCCosRandomMix
{
	int nRet;
	UINT nResultItemID;
};
#endif // PRE_ADD_COSRANDMIX

struct CSPotentialItem
{
	BYTE cInvenIndex;
	INT64 biInvenSerial;
	INT64 biItemSerial;
};

struct SCPotentialItem
{
	int nRet;
	BYTE cInvenIndex;
};
#if defined(PRE_MOD_POTENTIAL_JEWEL_RENEWAL)
struct SCPotentialItemRollback
{
	int nRet;
};
#endif	// #if defined(PRE_MOD_POTENTIAL_JEWEL_RENEWAL)
struct CSCharmItemRequest		// CS_ITEMGOODS / CS_CHARMITEMREQUEST
{
	char cInvenType;			// eItemPositionType
	short sInvenIndex;			// �Ϲ��κ��϶��� index�־��ָ� �� (ĳ���� ������������)
	INT64 biInvenSerial;
	BYTE cKeyInvenIndex;			// �Ϲ��κ��϶��� index�־��ָ� �� (ĳ���� ������������)
	int nKeyItemID;
	INT64 biKeyItemSerial;
};

#ifdef PRE_ADD_GETITEM_WITH_RANDOMVALUE
struct TRangomGiveItemData
{
	int nItemID;	// ������
	int nCount;		// ������ ȹ���� ����
	int nPeriod;	// �Ⱓ
	int nGold;
};
#endif		//#ifdef PRE_ADD_GETITEM_WITH_RANDOMVALUE

struct SCCharmItemRequest		// CS_ITEMGOODS / SC_CHARMITEMREQUEST
{
	char cInvenType;			// eItemPositionType
	short sInvenIndex;			// �Ϲ��κ��϶��� index�־��ָ� �� (ĳ���� ������������)
	INT64 biInvenSerial;
	int nRet;
#ifdef PRE_ADD_GETITEM_WITH_RANDOMVALUE
	BYTE cRandomGiveCount;
	TRangomGiveItemData RandomGiveItems[RANDOMGIVE_CHARMITEM_MAX];
#endif		//#ifdef PRE_ADD_GETITEM_WITH_RANDOMVALUE
};

struct CSCharmItemComplete		// CS_ITEMGOODS / CS_CHARMITEMCOMPLETE
{
	char cInvenType;			// eItemPositionType
	short sInvenIndex;			// �Ϲ��κ��϶��� index�־��ָ� �� (ĳ���� ������������)
	INT64 biInvenSerial;
	BYTE cKeyInvenIndex;			// �Ϲ��κ��϶��� index�־��ָ� �� (ĳ���� ������������)
	int nKeyItemID;
	INT64 biKeyItemSerial;
};

struct SCCharmItemComplete		// SC_ITEMGOODS / SC_CHARMITEMCOMPLETE
{
	char cInvenType;			// eItemPositionType
	int nResultItemID;
	int nItemCount;
	int nPeriod;
	int nRet;
};

#if defined (PRE_ADD_CHAOSCUBE)
struct TChaosItem
{
	short wCount;
	short sInvenIndex;
	INT64 biInvenSerial;
};

struct CSChaosCubeRequest		// CS_ITEMGOODS / CS_CHAOSCUBEREQUEST
{
	char cCubeInvenType;
	short sCubeInvenIndex;
	INT64 biCubeInvenSerial;
#if defined( PRE_ADD_CHOICECUBE )
	int nChoiceItemID;	// ������ ���������(���̽��ڽ������� ���)
#endif
	char cInvenType;
	int nCount;
	TChaosItem ChaosItem[MAX_CHAOSCUBE_STUFF];

#if defined( PRE_ADD_CHOICECUBE )
	CSChaosCubeRequest() : nChoiceItemID(0) {}
#endif
};

struct SCChaosCubeRequest		// CS_ITEMGOODS / SC_CHAOSCUBEREQUEST
{
	int nRet;
	char cInvenType;
	int nCount;
	TChaosItem ChaosItem[MAX_CHAOSCUBE_STUFF];
};

struct CSChaosCubeComplete		// CS_ITEMGOODS / CS_CHAOSCUBECOMPLETE
{
	char cCubeInvenType;
	short sCubeInvenIndex;
	INT64 biCubeInvenSerial;
#if defined( PRE_ADD_CHOICECUBE )
	int nChoiceItemID;	// ������ ���������(���̽��ڽ������� ���)
#endif
	char cInvenType;
	int nCount;
	TChaosItem ChaosItem[MAX_CHAOSCUBE_STUFF];

#if defined( PRE_ADD_CHOICECUBE )
	CSChaosCubeComplete() : nChoiceItemID(0) {}
#endif
};

struct SCChaosCubeComplete		// SC_ITEMGOODS / SC_CHAOSCUBECOMPLETE
{
	char cInvenType;
	int nResultItemID;
	int nItemCount;
	int nPeriod;
	int nRet;
};
#endif // #if defined (PRE_ADD_CHAOSCUBE)

struct SCPetalTokenResult		// SC_ITEMGOODS / SC_PETALTOKENRESULT,			// ��Ż ��ǰ�� ��� ä��â��..
{
	int nUseItemID;
	int nTotalPetal;
};

struct SCAppellationGainResult
{
	int nArrayIndex;
	int nRet;
};

struct CSGuildRename			// CS_ITEMGOODS	/ CS_GUILDRENAME
{
	char cInvenType;
	BYTE cInvenIndex;
	INT64 biItemSerial;
	WCHAR wszGuildName[GUILDNAME_MAX];
};

struct SCGuildRename			// SC_ITEMGOODS / SC_GUILDRENAME
{
	int nRet;
};

struct CSCharacterRename		// CS_ITEMGOODS / CS_CHARACTERRENAME
{
	char cInvenType;
	BYTE cInvenIndex;
	INT64 biItemSerial;
	WCHAR wszCharacterName[NAMELENMAX];
};

struct SCCharacterRename		// SC_ITEMGOODS / SC_CHARACTERRENAME
{
	int nRet;					//ERROR_DB_CHANGENAME_NEXTDAY�� ���tNextChangeDate�� ��¥���� ǥ��
	UINT nUserSessionID;
	__time64_t tNextChangeDate;
	WCHAR wszCharacterName[NAMELENMAX];
};

#if defined (PRE_ADD_BESTFRIEND)
struct CSBestFriendItemRequest
{
	INT64 biGiftSerial;
	int nMemuItemID;
};

struct SCBestFriendItemRequest
{
	INT64 biGiftSerial;
	int nRet;
};

struct CSBestFriendItemComplete
{
	INT64 biGiftSerial;
	int nMemuItemID;
};

struct SCBestFriendItemComplete
{
	int nResultItemID;
	int nItemCount;
	int nPeriod;
	int nRet;
};
#endif

struct CSChangePetName
{
	INT64 itemSerial;
	INT64 petSerial;
	WCHAR name[NAMELENMAX];
};

struct SCChangePetName
{
	int nRet;
	INT64 biItemSerial;
	UINT nUserSessionID;
	WCHAR wszPetName[NAMELENMAX];
};

struct CSModItemExpireDate
{
	INT64 biExpireDateItemSerial;	// �Ⱓ���忡 ����� ������ �ø���
	INT64 biItemSerial;				// �Ⱓ ������ ������ �ø���
};

struct CSPetSkillItem
{	
	INT64 biItemSerial;				// ������� Serial ��ȣ
};

struct SCPetSkillItem
{
	int nRet;
	char cSlotNum;					// ���� Num
	int nSkillID;					// ��ų ID
};

struct CSPetSkillDelete
{
	char cSlotNum;					// ������ ���� Num
};

struct SCPetSkillDelete
{
	int nRet;
	char cSlotNum;
};

struct CSPetSkillExpand
{
	INT64 biItemSerial;				// ��ų Ȯ�� ������ Serial
};

struct SCPetSkillExpand
{
	int nRet;
};

struct CSEnchantJewelItem
{
	BYTE cInvenIndex;
	INT64 biInvenSerial;
	INT64 biItemSerial;
};

struct SCEnchantJewelItem
{
	int nRet;
	BYTE cInvenIndex;
};

struct SCExpandSkillPageItem
{
	int nRet;	
};


struct CSGuildMark		// CS_ITEMGOODS / CS_GUILDMARK		// ��帶ũ ����
{
	int nItemID;
	INT64 biItemSerial;
	int nInvenIndex;		// �Ϲݾ������� ��� �κ��丮�ε���
	short wGuildMark;
	short wGuildMarkBG;
	short wGuildMarkBorder;

};

struct SCGuildMark		// SC_ITEMGOODS / SC_GUILDMARK
{
	int nRet;
	short wGuildMark;
	short wGuildMarkBG;
	short wGuildMarkBorder;
};

struct SCSourceItem
{
	int nSessionID;
	int nItemID;
	int nRemainTime;
	bool bUsedInGameServer;
};

#if defined( PRE_ADD_NAMEDITEM_SYSTEM )
struct SCEffectSkillItem
{
	int nSessionID;
	int nCount;
	bool bUsedInGameServer;
	TEffectSkillData EffectSkill[EFFECTSKILLMAX];
};
struct SCDelEffectSkillItem
{
	int nSessionID;
	int nItemID;
};
struct SCShowEffect
{
	int nItemID;
	int nSessionID;
	bool bUseSession;	
};
struct SCNamedItemID
{
	int nSessionID;
	int nItemID;	
};
#endif

#if defined(PRE_ADD_TRANSFORM_POTION)
struct SCChangeTransform
{
	int nSessionID;
	int nTransformID;
	int nExpireTime;	// minute;
};
#endif

//---------------------------------------------------------------1-----------------------------------
//	NPC (CS_NPC, SC_NPC)
//--------------------------------------------------------------------------------------------------

struct CSNpcTalk				// CS_NPC / CS_NPCTALK
{
	UINT  nNpcUniqueID;
	DWORD dwIndexHashCode;
	DWORD dwTargetHashCode;
};

struct CSNpcTalkEnd
{
	UINT nNpcUniqueID;
};

struct TalkParam 
{
	enum
	{
		STRING = 1,
		INT,
	};
	char  cType;
	WCHAR wszKey[NAMELENMAX];
	int   nValue;
};

const int TALK_PARAM_MAX = 10;
struct SCNpcTalk				// SC_NPC / SC_NPCTALK
{
	UINT  nNpcUniqueID;
	DWORD dwIndexHashCode;
	DWORD dwTargetHashCode;
	bool bIsEnd;

	short nCount;
	TalkParam TalkParamArray[TALK_PARAM_MAX];
};

struct SCShowWareHouse
{
	int iItemID;
};

struct SCOpenUpgradeItem
{
	int iItemID;
};

struct SCOpenCompoundItem				// SC_NPC / eNpc::SC_OPENCOMPOUND_ITEM
{
	UINT  nCompoundShopID;
};

struct SCOpenCompound2Item				// SC_NPC / eNpc::SC_OPENCOMPOUND2_ITEM
{
	UINT  nCompoundGroupID;
	int	  iItemID;
};

struct SCOpenGuildMgrBox				// SC_NPC / eNpc::SC_OPENGUILDMGRBOX
{
	int  nGuildMgrNo;	// ?
};

struct SCOpenBrowser
{
	char url[MAX_BROWSER_URL];
	int nX;
	int nY;
	int nWidth;
	int nHeight;
};

#if defined(PRE_ADD_OPEN_QUEST_TEXTURE_DIALOG)
struct SCOpenTextureDialog
{
	int		nFileIndex;
	float	fX;
	float	fY;
	int		nPosition;
	int		nTime;
	int		nDialogIndex;
	bool	bAutoCloseDialog;
};

struct SCCloseTextureDialog
{
	int nDialogID;
};
#endif

//--------------------------------------------------------------------------------------------------
//	QUEST (CS_QUEST, SC_QUEST)
//--------------------------------------------------------------------------------------------------
struct CSCancelQuest				// CS_QUEST / CS_CANCEL_QUEST
{
	int nQuestIndex;
};

struct SCQuestInfo					// SC_QUEST / SC_QUESTINFO
{
	DNNotifier::Data	NotifierData[DNNotifier::RegisterCount::Total];
	char cCount;
	TQuest Quest[MAX_PLAY_QUEST];
};

struct SCQuestCompleteInfo			// SC_QUEST / SC_QUESTCOMPLETEINFO
{
	char CompleteQuest[COMPLETEQUEST_BITSIZE];
};

struct RefreshType 
{
	enum
	{
		ADD_QUEST = 1,
		CANCEL_QUEST,
		REFRESH_STEP,
		REFRESH_JOURNAL,
		REFRESH_COUNTING,
		REFRESH_COMPLETECOUNTING,
		REFRESH_ALLCOMPLETECOUNTING,
		REFRESH_STEPANDJOURNAL,
	};
};

struct SCRefreshQuest				// SC_QUEST / SC_REFRESH_QUEST
{
	short		  nRefreshType;
	char		  cQuestSlot;
	UINT		  nQuestID;
	TQuest		  Quest;
};

struct SCCompleteQuest				// SC_QUEST / SC_COMPLETE_QUEST
{
	bool		  bDeletePlayList;
	char		  cQuestSlot;
	UINT		  nQuestID;
	bool		  bNeedMarking;
};

struct SCMarkingCompleteQuest			// SC_QUEST / SC_MARKING_COMPLETE_QUEST
{
	UINT		  nQuestID;
};


struct SCRemoveQuest				// SC_QUEST / SC_REMOVE_QUEST
{
	char		  cQuestSlot;
	UINT		  nQuestID;
	int nRetCode;
};

struct SCPlayCutScene			// SC_ETC / SC_PLAYCUTSCENE
{
	UINT nCutSceneTableID;
	int  nQuestIndex;
	int  nQuestStep;
	UINT nNpcObjectID;
	bool bQuestAutoFadeIn;
	DWORD dwQuestPlayerUniqueID;
};

struct SCQuestReward		// SC_QUEST / SC_QUEST_REWARD 
{
	int nRewardTableIndex;
	bool bActivate;
};

struct SCQuestResult		// SC_QUEST / SC_QUESTRESULT
{
	int nRetCode;
};

struct SCCompleteCutScene
{
	bool bFadeIn;
};

struct SCSkipCutScene {
	UINT nSessionID;
};

struct CSSelectQuestReward
{
	UINT		nIndex;				// �������̺� �ε���
	bool		SelectArray[QUESTREWARD_INVENTORYITEMMAX];
	bool		SelectCashArray[MAILATTACHITEMMAX];
};

struct CSCompleteCutScene
{
	UINT nSessionID;
};

struct SCAssignPeriodQuest			// SC_QUEST / SC_ASSIGN_PERIODQUEST
{
	UINT		  nQuestID;
	bool		  bFlag;
};

struct SCPeriodQuestNotice			// SC_QUEST / SC_NOTICE_PERIODQUEST
{
	int			nItemID;
	int			nNoticeCount;
};

struct SCScorePeriodQuest			// SC_QUEST / SC_SCORE_PERIODQUEST
{
	int			nCurrentCount;
	int			nMaxCount;
};

struct CSShortCutQuest
{
	UINT nQuestID;
};

#if defined(PRE_ADD_REMOTE_QUEST)
struct SCGainRemoteQuest			// CS_QUEST / CS_ACCEPT_REMOTE_QEUST
{
	UINT nQuestID;
};

struct CSAcceptRemoteQuest			// CS_QUEST / CS_ACCEPT_REMOTE_QEUST
{
	UINT nQuestID;
};

struct SCAcceptRemoteQuest			// SC_QUEST / SC_ACCEPT_REMOTE_QEUST
{
	int nRet;
	UINT nQuestID;
};

struct CSCompleteRemoteQuest : public CSSelectQuestReward	// CS_QUEST / CS_COMPLETE_REMOTE_QUEST
{
	UINT nQuestID;
};

struct CSCancelRemoteQuest			// CS_QUEST / CS_CANCEL_REMOTE_QUEST
{
	UINT nQuestID;
};

struct SCRemoveRemoteQuest			// SC_QUEST / SC_REMOVE_REMOTE_QEUST
{
	UINT nQuestID;
};

struct SCCompleteRemoteQuest			// SC_QUEST / SC_COMPLETE_REMOTE_QEUST
{
	UINT nQuestID;
};

#endif


//--------------------------------------------------------------------------------------------------
//	CHAT (CS_CHAT, SC_CHAT)
//--------------------------------------------------------------------------------------------------

struct CSChat					// CS_ETC / CS_CHAT
{
	eChatType eType;
	INT64 biItemSerial;
	short nLen;
	WCHAR wszChatMsg[CHATLENMAX];
};

struct CSChatPrivate			// CS_ETC / CS_CHAT_PRIVATE
{
	WCHAR wszToCharacterName[NAMELENMAX];
	short nLen;
	WCHAR wszChatMsg[CHATLENMAX];
};

struct SCChat					// SC_ETC / SC_CHATMSG
{
	eChatType eType;
	WCHAR wszFromCharacterName[NAMELENMAX];
	int nRet;					// 1: success -1: ������ -2: �������� ������
	short nLen;
	WCHAR wszChatMsg[CHATLENMAX];
};

struct SCWorldSystemMsg			// SC_WORLDSYSTEMMSG
{
	WCHAR wszFromCharacterName[NAMELENMAX];
	char cType;	// eChatType (WORLDCHATTYPE_...)
	int nID;
	int nValue;
	WCHAR wszToCharacterName[NAMELENMAX];
};

struct SCNotice			//SC_NOTICE
{
	int nShowSec;
	short nLen;
	WCHAR wszNoticeMsg[CHATLENMAX];
};

struct SCGuildChat
{
	TGuildUID GuildUID;
	UINT nAccountDBID;			// ���� DBID
	INT64 nCharacterDBID;			// ĳ���� DBID
	short nLen;					// ä�� ����
	WCHAR wszChatMsg[CHATLENMAX];	// ä�� �޽���
};

#ifdef PRE_ADD_DOORS_GUILDCHAT_DISCONNECT
struct SCDoorsGuildChat
{
	TGuildUID GuildUID;
	INT64 biCharacterDBID;			// ĳ���� DBID
	short nLen;					// ä�� ����
	WCHAR wszChatMsg[CHATLENMAX];	// ä�� �޽���
};
#endif		//#ifdef PRE_ADD_DOORS_GUILDCHAT_DISCONNECT

struct CSDice
{
	eChatType eType;
	short range;
};

struct CSChatChatRoom			// CS_CHAT_CHATROOM
{
	short nLen;
	WCHAR wszChatMsg[CHATLENMAX];
};

struct SCChatServerMsg			// SC_SERVERMSG
{
	char cType;	
	char cStringType;
	int nMsgIndex;
	char cCaption;				// Ŭ���� CDnInterface::emCAPTION_TYPE �����.
	BYTE cFadeTime;				// ���̵� �ð�(second)
};

//--------------------------------------------------------------------------------------------------
//	ROOM (CS_ROOM, SC_ROOM)
//--------------------------------------------------------------------------------------------------

struct CSSyncWait				// CS_ROOM / CS_SYNC_WAIT
{
	UINT nSessionID;
};

struct CSOKDungeonClear
{
	UINT nSessionID;
};

//struct CSWarpDungeonClear
//{
//	bool bWarpVillage;
//};

struct CSWarpDungeon
{
	bool bWarpVillage;
};

struct CSSelectRewardItem
{
	char cItemIndex;
};

struct CSIdentifyRewardItem
{
	char cItemIndex;
};

struct CSDLChallengeResponse
{
	bool bAccept;
};

struct SCDLChallengeResponse
{
	bool bAccept;
};

struct SCSyncWait				// SC_ROOM / SC_SYNC_WAIT
{
	UINT nSessionID;
};

struct SCSyncStart				// SC_ROOM / SC_SYNC_START
{
	UINT nSessionID;
};

struct SCRefreshMember		// SC_ROOM / SC_REFRESH_MEMBER
{
	BYTE cCount;
	UINT nMemberSessionID[PARTYMAX];
};

struct SCSelectRewardItem
{
	UINT nSessionID;
	char cItemIndex;
};

struct SCIdentifyRewardItem
{
	char cItemIndex;
	char cBoxType;
};

struct SCRoomSyncSeqLevel		//SC_ROOM / SC_ROOMSYNC_SEQLEVEL
{
	BYTE cSeqLevel;
};

//#if defined(_GAMESERVER) || defined(_WINDOWS)
#if defined(_GAMESERVER) || (defined(_WINDOWS) && !defined(_LAUNCHER))
struct SCGenerationMonster		//SC_GENERATION_MONSTER_MSG
{
	UINT nSessionID;
	int nSeed;
	UINT nMonsterID;
	int nEventAreaUniqueID;
	EtVector3 vPos;
	EtVector3 vVel;
	EtVector3 vResistance;
	int nTeam;
	bool bRandomFrameSummon;
	bool bUseRotationQuat;		// SummonMonster �ñ׳��� ����� ��� ��ȯ�� ������ �������� ��Ŷ���� ������.
	EtQuat qRotation;
};

// ���� ��Ŷ���� ��ü��. /////////////////////////////////////////////////
struct SCGenerationProp			//SC_GENERATION_PROP_MSG
{
	int nSummonPropTableID;
	int nPropUniqueID;
	int nSeed;
	EtVector3 vPos;
	EtVector3 vRotate;
	EtVector3 vScale;
};
//////////////////////////////////////////////////////////////////////////
#endif

struct SCGateInfoMessage		// SC_ROOM / SC_GATEINFO_MSG
{
	BYTE cCount;
	BYTE cGateIndex[WORLDMAP_GATECOUNT];
	BYTE cPermitFlag[WORLDMAP_GATECOUNT];
};

struct SCGateEjectInfo			// SC_ROOM / SC_GATEEJECTINFO_MSG
{
	BYTE cGateIndex;
	BYTE cLessLevelCount;
	BYTE cNotEnoughItemCount;
	BYTE cExceedCount;
	BYTE cNotRideVehicleCount;
	BYTE cActorIndex[PARTYMAX*2];
};

struct sEventStageClearBonus
{
	BYTE nType;							//eEventType3 ����
	UINT nClearEventBonusExperience;
};

struct TDungeonClearInfo
{
	UINT nSessionID;
	BYTE cRank;
	UINT nDefaultCP;
	UINT nBonusCP;
	USHORT nMaxComboCount;
	USHORT nKillBossCount;
	UINT nRewardGold;
	char cClearRewardType;
	UINT nCompleteExperience;
	UINT nRewardExperience;
	UINT nMeritBonusExperience;
	char cMaxLevelCharCount;
	UINT nPromotionExperience;
	char cShowBoxCount;
	char cSelectBoxCount;
	BYTE cBonusCount;
	sEventStageClearBonus EventClearBonus[EVENTCLEARBONUSMAX];
};

struct SCDungeonClear			// SC_ROOM / SC_DUNGEONCLEAR_MSG
{
	BYTE cCount;
	UINT nDungeonClearTableID;
	UINT nClearTime;
	char cCurLevel;
	int nCurExp;
	bool bIgnoreRewardItem;
	TDungeonClearInfo Info[PARTYMAX];
};

#if defined(PRE_ADD_CP_RANK)
struct TStageClearBest
{
	INT64 biCharacterDBID;
	WCHAR wszCharName[NAMELENMAX];
	char cRank;
	int nClearPoint;
};

struct SCAbyssStageClearBest		// SC_ROOM / SC_ABYSS_STAGE_CLEAR_BEST
{
	TStageClearBest sLegendClearBest;
	TStageClearBest sMonthlyClearBest;
	char cMyBestRank;
	int nMyBestCP;
};
#endif

struct TRewardItemResultStruct {
	UINT nSessionID;
	BYTE cIndex;
};

struct SCDungeonClearRewardItemResult
{
	BYTE cCount;
	TRewardItemResultStruct Info[PARTYMAX];
};

struct TRewardItemStruct {
	UINT nSessionID;
	UINT nRewardItemID[4];
#if !defined( PRE_ADD_NAMEDITEM_SYSTEM )
	BYTE cRewardItemType[4];
#endif
	int nRewardItemRandomSeed[4];
	char cRewardItemOption[4];
	short wRewardItemCount[4];
	char cRewardItemSoulBound[4];
#if defined(PRE_ADD_STAGE_CLEAR_ENCHANT_REWARD)
	char cRewardItemLevel[4]; //��ȭ����
	char cRewardItemPotential[4]; //����� ����
#endif	// #if defined(PRE_ADD_STAGE_CLEAR_ENCHANT_REWARD)
};

#if defined( PRE_ADD_NAMEDITEM_SYSTEM )
struct TRewardBoxTypeStruct
{
	UINT nSessionID;
	BYTE cRewardItemType[4];
};
struct SCDungeonClearBoxType
{
	BYTE cCount;
	TRewardBoxTypeStruct Info[PARTYMAX];
};
#endif
struct SCDungeonClearRewardItem
{
	BYTE cCount;
	TRewardItemStruct Info[PARTYMAX];
};

struct SCWarpDungeonClear
{
#if defined(PRE_ADD_RETURN_VILLAGE_SHOWNAME)
	int nLastVillageMapIndex;
#endif	// #if defined(PRE_ADD_RETURN_VILLAGE_SHOWNAME)
};

struct SCDungeonClearLeaderWarp
{
	bool bEnable;
};

struct SCDungeonFailed
{
	bool bCanWarpWorldMap;
	bool bTimeOut;
#if defined(PRE_ADD_RETURN_VILLAGE_SHOWNAME)
	int nLastVillageMapIndex;
#endif	// #if defined(PRE_ADD_RETURN_VILLAGE_SHOWNAME)
};

struct SCDungeonClearWarpOutFail
{
	int nRet;
};

struct SCOpenDungeonOpenLevel
{
	UINT nMapIndex;
	char cDifficulty;
};

struct SCSyncDungeonTimeAttack
{
	int iRemainSec;
	int iOrgSec;
};

#if defined(PRE_MOD_DARKLAIR_RECONNECT)
struct SCDLRoundInfo	// SC_ROOM / SC_DLROUND_INFO
{
	int nCurrentRound;
	int nTotalRound;
	bool bBossRound;
};
#endif

struct TDLDungeonClearInfo
{
	UINT nSessionID;
	UINT nExperience;
	UINT nMeritBonusExperience;
	char cShowBoxCount;
	char cSelectBoxCount;
	UINT nRewardGold;
	char cClearRewardType;
	char cMaxLevelCharCount;
	UINT nPromotionExperience;
	BYTE cBonusCount;
	sEventStageClearBonus EventClearBonus[EVENTCLEARBONUSMAX];
};

struct SCDLDungeonClear			// SC_ROOM / SC_DUNGEONCLEAR_MSG
{
	BYTE cCount;
	UINT nClearTime;
	int nClearRound;
	bool bClear;
	TDLDungeonClearInfo Info[PARTYMAX];
};

struct TDLRankHistoryMemberInfo {
	USHORT nLevel;
	int	nJobIndex;
	WCHAR wszCharacterName[NAMELENMAX];
};

struct TDLRankHistoryPartyInfo {
	int nRank;
	USHORT nPlayRound;
	UINT nPlaySec;
	WCHAR wszPartyName[PARTYNAMELENMAX];
	BYTE cPartyUserCount;
	TDLRankHistoryMemberInfo Info[DarkLair::Common::MaxPlayer];
};

struct SCDLRankInfo
{
	TDLRankHistoryPartyInfo CurScore;
	TDLRankHistoryPartyInfo TopScore;
	TDLRankHistoryPartyInfo HistoryScore[DarkLair::Rank::SelectTop];
};

struct SCChangeGateState		// SC_ROOM / SC_CHANGE_GATESTATE_MSG
{
	BYTE cGateInde;
	UINT nPermitFlag;
};

struct SCChangeGameSpeed		// SC_ROOM /SC_CHANGE_GAMESPEED
{
	float fSpeed;
	UINT dwDelay;
};

struct SCDungeonHistory
{
	BYTE cBuf[1024];
};

struct SCROOM_SYNC_MEMBERINFO
{
	bool				bIsBreakInto;
	bool				bIsStart;
	short				nCount;
	SPartyMemberInfo	Member[MAX_SEND_SYNCPLAYER];
};

#ifdef PRE_MOD_SYNCPACKET
struct TSyncTeam
{
	UINT nSessionID;
	USHORT usTeam;
};

struct SCROOM_SYNC_MEMBERTEAM
{
	BYTE				cCount;
	TSyncTeam tTeam[MAX_SEND_SYNCPLAYER];
};
#else		//#ifdef PRE_MOD_SYNCPACKET
struct SCROOM_SYNC_MEMBERTEAM
{
	BYTE				cCount;
	USHORT				unStartIndex;
	USHORT				usTeam[MAX_SEND_SYNCPLAYER];
};
#endif		//#ifdef PRE_MOD_SYNCPACKET

struct SCROOM_SYNC_MEMBERDEFAULTPARTS
{
	short				nCount;
	int					iDefaultPartsIndex[PvPCommon::Common::MaxPlayer*4];
};

struct SCROOM_SYNC_MEMBEREQUIP
{
	short				nCount;
	TPartyMemberEquip	MemberEquip[PvPCommon::Common::MaxPlayer];
};

struct SCROOM_SYNC_MEMBERSKILL
{
	short				nCount;
	TPartyMemberSkill	MemberSkill[PvPCommon::Common::MaxPlayer];
};

struct SCROOM_SYNC_MEMBER_BREAKINTO
{
	SPartyMemberInfo	Member;
	USHORT				usTeam;
	BYTE				cGMTrace;
	int					iDefaultPartsIndex[4];
	char				cViewCashEquipBitmap[VIEWCASHEQUIPMAX_BITSIZE];
	TGuildSelfView		GuildSelfView;	// ��� �ð����� (����)
	short				nEquipCount;
	short				nCashEquipCount;
	short				nSkillCount;
	short				nGlyphCount;
#if defined(PRE_ADD_TALISMAN_SYSTEM)
	short				nTalismanCount;
#endif
	int					iSelectAppellation;
	int					nCoverAppellation;
#if defined(PRE_ADD_VIP)
	bool				bVIP;
#endif	// #if defined(PRE_ADD_VIP)
	char				cAccountLevel; // �������� (eAccountLevel)
	char				cEternityItemCount;
	TVehicleCompact		VehicleInfo;
	TVehicleCompact		PetInfo;
	TItemInfo			EquipArray[EQUIPMAX];
	TItemInfo			CashEquipArray[CASHEQUIPMAX];
	TItemInfo			GlyphArray[GLYPHMAX];
#if defined(PRE_ADD_TALISMAN_SYSTEM)
	TItemInfo			TalismanArray[TALISMAN_MAX];
#endif
	TSkill				SkillArray[SKILLMAX];
};

#ifdef PRE_MOD_SYNCPACKET
struct TSyncHPSP
{
	UINT nSessionID;
	INT64 iHP;
	int iSP;
};

struct SCROOM_SYNC_MEMBERHPSP
{
	BYTE	cCount;
	TSyncHPSP tHPSP[MAX_SEND_SYNCPLAYER];
};
#else		//#ifdef PRE_MOD_SYNCPACKET
struct SCROOM_SYNC_MEMBERHPSP
{
	BYTE	cCount;
	USHORT	unStartIndex;
	int		iHPSPArr[MAX_SEND_SYNCPLAYER*2];
};
#endif		//#ifdef PRE_MOD_SYNCPACKET

#ifdef PRE_MOD_SYNCPACKET
struct TBattleMode
{
	UINT nSessionID;
	BYTE cBattleMode;
};

struct SCROOM_SYNC_MEMBERBATTLEMODE
{
	BYTE	cCount;
	TBattleMode tBattleMode[MAX_SEND_SYNCPLAYER];
};
#else		//#ifdef PRE_MOD_SYNCPACKET
struct SCROOM_SYNC_MEMBERBATTLEMODE
{
	BYTE	cCount;
	USHORT	unStartIndex;
	BYTE	cBattleModeArr[MAX_SEND_SYNCPLAYER];
};
#endif		//#ifdef PRE_MOD_SYNCPACKET

struct TDropItemSync:public SCCreateDropItem
{
	UINT	uiLifeTime;	// m_fLifeTime*1000
	float	fDistance;
};

struct SCROOM_SYNC_DROPITEMLIST
{
	BYTE			cCount;
	TDropItemSync	sDropItemList[SENDDROPITEMLISTMAX];
};



//--------------------------------------------------------------------------------------------------
//	SKILL (CS_SKILL, SC_SKILL)
//--------------------------------------------------------------------------------------------------

struct SCSkillList				// SC_SKILL / SC_SKILLLIST
{
	BYTE cSkillCount;
	char cSkillPage;
	TSkill Skill[SKILLMAX];
};

struct CSAddSkill				// CS_SKILL / CS_ADDSKILL
{
	int nSkillID;
};

struct SCAddSkill				// SC_SKILL / SC_ADDSKILL
{
	int nSkillID;
	int nRet;
};

struct CSDelSkill				// CS_SKILL / CS_ADDSKILL
{
	int nSkillID;
};

struct SCDelSkill				// SC_SKILL / SC_DELSKILL
{
	int nSkillID;
	int nRet;
};

struct SCSkillLevelUp			// SC_SKILL / SC_SKILLLEVELUP 
{
	int nSkillID;
	BYTE cLevel;
	int nRetCode;
};

struct CSSkillLevelUpReq		// CS_SKILL / CS_SKILLLEVELUP_REQ
{
	int nSkillID;
	BYTE cUseSkillPoint;
};

struct SCOtherPlayerSkillLevelUp	// CS_SKILL / CS_OTHERPLAYERSKILLLEVELUP
{
	int nPartyIndex;
	int nSkillID;
	BYTE cLevel;
};

struct SCPushSkillPoint
{
	unsigned short usSkillPoint;
};

struct SCUnlockSkill	// CS_SKILL / SC_UNLOCKSKILL
{
	int nSkillID;
	int nRetCode;
};

struct CSAcquireSkillReq		// CS_SKILL / CS_ACQUIRESKILL_REQ
{
	int nSkillID;
};

struct SCAcquireSkill		// CS_SKILL / SC_ACQUIRESKILL
{
	int nSkillID;
	int nRetCode;
};


struct CSUnlockSkillByMoneyReq	// CS_SKILL / SC_UNLOCKSKILL_BY_MONEY
{
	int nSkillID;
};

#if defined(PRE_ADD_SKILL_LEVELUP_RESERVATION)
struct TReservationSkillReq
{
	char cType;			// eSkillReservation
	int nSkillID;		
	BYTE nLevel;		// LevelUp�� ��츸 ���� ����.
};

struct TReservationSkillAck
{
	TReservationSkillReq tReservationSkill;	
	int nResult;
};

struct CSReservationSkillListReq // CS_SKILL / CS_RESERVATION_SKILL_LIST_REQ
{
	int nCount;
	TReservationSkillReq tReservationSkill[ReservationSKillList::ReservationMax];
};

struct SCReservationSkillListAck // SC_SKILL / SC_RESERVATION_SKILL_LIST_ACK
{
	int nCount;
	TReservationSkillAck tReservationSkillAck[ReservationSKillList::ReservationMax];
};
#endif // #if defined(PRE_ADD_SKILL_LEVELUP_RESERVATION)

#ifdef PRE_ADD_PRESET_SKILLTREE
struct TSkillSetPartialData
{
	int nSkillID;
	BYTE cLevel;
};

struct CSSaveSkillSet			// CS_SKILL / CS_SAVE_SKILLSET
{
	BYTE cIndex;
	WCHAR wszSetName[SKILLPRESETNAMEMAX];
	BYTE cCount;
	TSkillSetPartialData SKills[SKILLMAX];
};

struct CSDeleteSkillSet			// CS_SKILL / CS_DELETE_SKILLSET
{
	BYTE cIndex;
};

struct TSkillSetIndexData
{
	BYTE cIndex;
	BYTE cCount;				//�ش� �������� ������ �ִ� ��ų�� ��
	WCHAR wszSetName[SKILLPRESETNAMEMAX];
};

struct SCSKillSetList			// CS_SKILL / SC_LIST_SKILLSET
{
	int nRetCode;
	TSkillSetIndexData SKillIndex[SKILLPRESETMAX];
	TSkillSetPartialData SKills[SKILLMAX * SKILLPRESETMAX];
};

struct SCSKillSaveResult		// CS_SKILL / SC_SAVE_SKILLSET
{
	int nRetCode;
};

struct SCSKillDeleteResult		// CS_SKILL / SC_DELETE_SKILLSET
{
	int nRetCode;
};
#endif		//#ifdef PRE_ADD_PRESET_SKILLTREE

struct SCSkillReset // CS_SKILL / SC_RESET_SKILL
{
	char cSkillPage;
	int nResultSkillPoint;	
};

struct SCChangeSkillPage		//SC_SKILL	/ SC_CHANGE_SKILLPAGE_RES	
{
	char cSkillPage;	
};

struct SCSkillPageCount		//SC_SKILLPAGE_COUNT
{
	char cSkillPageCount;	
};

struct CSChangeSkillPage		// CS_SKILL	/ CS_CHANGE_SKILLPAGE
{
	char cSkillPageIndex;
};
//--------------------------------------------------------------------------------------------------
//	TRADE (CS_TRADE, SC_TRADE)
//--------------------------------------------------------------------------------------------------

// Shop
// npc����
struct CSShopRemoteOpen			// CS_TRADE / CS_SHOP_REMOTEOPEN
{
	Shop::Type::eCode Type;
};

struct SCShopOpen				// SC_TRADE / SC_SHOP_OPEN
{
	int nShopID;
	Shop::Type::eCode Type;
};

struct CSShopBuy				// CS_TRADE / CS_SHOP_BUY
{
	char cTapIndex;
	BYTE cShopIndex;			// shoplist���� ���� �ش� index��
	short wCount;
};

struct SCShopBuyResult			// SC_TRADE / SC_SHOP_BUY
{
	int nRet;
};

struct CSShopSell				// CS_TRADE / CS_SHOP_SELL
{
	BYTE cInvenIndex;			// inven index
	short wCount;
	INT64 biItemSerial;
};

struct SCShopSellResult			// SC_TRADE / SC_SHOP_SELL
{
	int nRet;
};

struct SCShopRepurchaseList
{
	int nRet;
	BYTE cCount;
	TRepurchaseItemInfo List[Shop::Repurchase::MaxList];
};

struct CSShopRepurchase
{
	int iRepurchaseID;
};

struct SCShopRepurchase
{
	int iRet;
	int iRepurchaseID;
};

// SkillShop
struct TSkillShopInfo
{
	BYTE cSlotIndex;
	int nSkillID;
	int nPrice;
};

struct SCSkillShopOpen			// SC_TRADE / SC_SKILLSHOP_OPEN
{
	int nRet;	// ���� �ǹ� ����.
};

struct SCSkillShopBuyResult		// SC_TRADE / SC_SKILLSHOP_BUY
{
	int nSkillID;
	short wSkillPoint;
	int nRet;
};

// ����
struct CSMailBox				// CS_TRADE / CS_MAILBOX
{
	char cPageNum;			// ������ ��ȣ: 1���� ����
};

struct TMailBoxInfo
{
	int nMailDBID;							// mail db table id
	short wIndex;
	WCHAR wszFromCharacterName[MAILNAMELENMAX];	// ������
	WCHAR wszTitle[MAILTITLELENMAX];		// ���� ����
	BYTE cRemainDay;						// ������¥
	char cReadType;							// ���� ����
	char cAttachType;						// ������ ÷�λ���
	char cDeliveryType;
	char cMailType;							// �������� (0:�Ϲݸ���, 1:�̼Ǹ���, 2:���ϸ���)
	bool bNewFlag;							// ����/���� �ڵ�ȭ ����
};

struct SCMailBox				// SC_TRADE / SC_MAILBOX
{
	int nRetCode;
	char cDailyMailCount;		// ���Ϲ߼۰���
	short wTotalCount;
	short wUnreadCount;
	char cMailBoxCount;					// ������°���
	TMailBoxInfo MailBox[MAILBOXMAX];	// ���� ����
};

struct TAttachMailItem
{
	int nInvenIndex;	// ÷�εǴ� �������� �κ� �ε���
	int nItemID;
	int nCount;			// ÷�εǴ� ������ ����
	INT64 biSerial;
};

struct CSSendMail				// CS_TRADE / CS_SENDMAIL
{
	WCHAR wszToCharacterName[NAMELENMAX];	// �޴���
	WCHAR wszTitle[MAILTITLELENMAX];		// ����
	WCHAR wszText[MAILTEXTLENMAX];			// ����
	char cDeliveryType;						// eMailType (MAILTYPE_NORMAL, MAILTYPE_PREMIUM)
	int nAttachCoin;						// ����÷�� (21���� ����������)
	char cAttachItemTotalCount;
	TAttachMailItem AttachMailItem[MAILATTACHITEMMAX];
};

struct CSMailDBIDArray			// CS_TRADE / CS_MAIL_DELETE, CS_MAIL_ATTACHALL
{
	int nMailDBID[MAILPAGEMAX];
};

struct SCSendMail				// SC_TRADE / SC_MAIL_SEND
{
	char cDailyMailCount;		// ���Ϲ߼۰���
	int nRet;			// ERROR_MAIL_  �� ���۵Ǵ� �͵�� ����������
};

struct CSAttachMail
{
	int nMailDBID;
	char cAttachSlotIndex;		// -1: 5ĭ ��� �ޱ�, 0 - 4: �����ޱ�
};

struct SCMailResult				// SC_TRADE / SC_MAIL_ATTACH
{
	int nRet;			// ERROR_MAIL_  �� ���۵Ǵ� �͵�� ����������
};

struct SCMailDeleteResult		// SC_TRADE / SC_MAIL_DELETE
{
	int nTotalMailCount;
	int nRet;			// ERROR_MAIL_  �� ���۵Ǵ� �͵�� ����������
};

struct SCAttachAllMailResult	// SC_TRADE / SC_MAIL_ATTACHALL
{
	int nMailDBID[MAILPAGEMAX];	
	int nRet;			// ERROR_MAIL_  �� ���۵Ǵ� �͵�� ����������
};

struct CSReadMail				// CS_TRADE / CS_MAIL_READ, CS_MAIL_READCADGE
{
	int nMailDBID;
};

struct SCReadMail				// SC_TRADE / SC_MAIL_READ
{
	int nRet;			// ERROR_MAIL_  �� ���۵Ǵ� �͵�� ����������
	bool bNewFlag;							// ����, ���� �ڵ�ȭ ����
	short wNotReadMailCount;				// ���� ���� ����
	int nMailDBID;
	WCHAR wszText[MAILTEXTLENMAX];			// ����
	__time64_t tSendDate;
	int nAttachCoin;
	char cDeliveryType;
	char cAttachCount;
	TItemInfo AttachItem[MAILATTACHITEMMAX];	// ÷�ε� ������
};

struct TWishItemInfo
{
	int nItemSN;
	int nItemID;
	char nOptionIndex;
};

struct SCReadCadgeMail			// SC_TRADE / SC_MAIL_READCADGE
{
	int nRet;								// ERROR_MAIL_  �� ���۵Ǵ� �͵�� ����������
	bool bNewFlag;							// ����, ���� �ڵ�ȭ ����
	short wNotReadMailCount;				// ���� ���� ����
	int nMailDBID;
	WCHAR wszText[MAILTEXTLENMAX];			// ����
	__time64_t tSendDate;
	bool bCadgeComplete;					// ������ �Ϸ� ����
	int nPackageSN;
	char cWishListCount;
	TWishItemInfo WishList[PACKAGEITEMMAX];
};

struct SCNotifyMail
{
	bool bNewMail;						// �� �������� �׳� �������� ���ϸ�����
	short wTotalMailCount;				// �� �����
	short wNotReadMailCount;				// ���� ���� ����
	bool bExpiration;					// ������
};

// ���λ���
#ifdef PRE_ADD_ITEMNAME_AUTOCOMPLETE
struct TSearchTrade
{
	int nSearchItemID;
	int nSearchNameID;
};
#endif		//#ifdef PRE_ADD_ITEMNAME_AUTOCOMPLETE

struct CSMarketList				// CS_TRADE / CS_MARKETLIST
{
	short wPageNum;						// ������ ��ȣ: 1���� ����
	char cMinLevel;
	char cMaxLevel;
	char cItemGrade[ITEMRANK_MAX];		// ��޺з�
	char cJob;							// �������� (���: -1)
	short cMainType;					// �⺻�з� (���: -1)
	char cDetailType;					// �󼼺з� (���: -1)
	WCHAR wszSearchWord1[SEARCHLENMAX];	// �˻��ܾ�
	WCHAR wszSearchWord2[SEARCHLENMAX];	// �˻��ܾ�
	WCHAR wszSearchWord3[SEARCHLENMAX];	// �˻��ܾ�
	char cSortType;						// eSortType
	WCHAR wszSearchItemName[SEARCHLENMAX];	// ������ �˻��ܾ�
	int nExchangeItemID;				// ExchangeTable�� ItemID
	bool bLowJobGroup;					// ���� ������ ����
	char cPayMethodCode;					// 0-ȥ��, 1-����, 3-��Ż
#ifdef PRE_ADD_ITEMNAME_AUTOCOMPLETE
	BYTE cCount;
	TSearchTrade SearchList[MultiLanguage::Common::SerachCountMax];
#endif		//#ifdef PRE_ADD_ITEMNAME_AUTOCOMPLETE
};

struct TMarketInfo
{
	int nMarketDBID;				// market table db ID

	WCHAR wszSellerName[NAMELENMAX];	// �Ǹ��� �̸�

	int nItemID;
	short wCount;		// ����
	USHORT wDur;			// ������
	int nRandomSeed;
	char cLevel;		// ����
	char cPotential;	// �����
	char cOption;
	char cSealCount;	// �к�ī��Ʈ

	int nPrice;		// ����
	int nUnitPrice;	// �ܰ�
	bool bDisplayFlag1;		// 0�� �Է��� �ּ���. ���� ������� �ʴ� �ʵ�
	bool bDisplayFlag2;		// 0�� �Է��� �ּ���. ���� ������� �ʴ� �ʵ�
	bool bPremiumTrade;	// ����������� ���� �����̾� �ŷ�������
	char cMethodCode;	// 1=���ӸӴ�, 3=��Ż
	char cPotentialMoveCount;		//��������Ƚ��
};

struct SCMarketList				// SC_TRADE / SC_MARKETLIST
{
	int nRetCode;
	int nMarketTotalCount;
	char cMarketCount;
	TMarketInfo MarketInfo[MARKETMAX];
};

struct TMyMarketInfo
{
	int nMarketDBID;					// market table db ID

	int nItemID;
	short wCount;		// ����
	USHORT wDur;			// ������
	int nRandomSeed;
	DWORD dwColor;		// ����
	char cLevel;		// ����
	char cPotential;	// �����
	char cOption;
	char cSealCount;	// �к�ī��Ʈ

	int nItemPrice;						// ����
	char cSellType;						// �ǸŻ��� 0:���ȷ���, 1:�ȷ���
	int nRemainTime;
	bool bPremiumTrade;	// ����������� ���� �����̾� �ŷ�������
	char cPayMethodCode; // PayMethodCode (DB������) 1-���, 3-��Ż
	char cItemPotentialMoveCount;		//��������ī��Ʈ
};

struct SCMyMarketList			// SC_TRADE / SC_MYMARKETLIST
{
	int nRetCode;
	short wSellingCount;	// �Ǹ���
	short wClosingCount;	// �Ⱓ����
	short wWeeklyRegisterCount;		// ����� AM 04:00 �� �������� �����ϰ� ����� �ŷ� Ƚ��
	short wRegisterItemCount;		// ����� ������ ����
	bool bPremiumTrade;
	char cMarketCount;
	TMyMarketInfo MarketInfo[MYMARKETMAX];
};

struct CSMarketRegister			// CS_TRADE / CS_MARKET_REGISTER
{
	char cInvenType;			// eItemPositionType
	BYTE cInvenIndex;			// �Ϲ��κ��϶��� index�־��ָ� �� (ĳ���� ������������)
	INT64 biInvenSerial;
	short wCount;
#if defined(PRE_ADD_PETALTRADE)
	char cPayMethodCode;		// PayMethodCode (DB������) 1-���, 3-��Ż
#endif
	int nPrice;
	char cPeriodIndex;
	bool bPremiumTrade;
};

struct CSMarketBuy				// CS_TRADE / CS_MARKET_BUY
{
	int nMarketDBID;
#if defined(PRE_ADD_DIRECT_BUY_UPGRADEITEM)
	bool bMini;
#endif	// #if defined(PRE_ADD_DIRECT_BUY_UPGRADEITEM)
};

struct SCMarketBuyResult		// SC_TRADE / SC_MARKET_BUY
{
	int nMarketDBID;
#if defined(PRE_ADD_DIRECT_BUY_UPGRADEITEM)
	bool bMini;
#endif	// #if defined(PRE_ADD_DIRECT_BUY_UPGRADEITEM)
	int nRet;
};

struct CSMarketDBID				// CS_TRADE / CS_MARKET_INTERRUPT, CS_MARKET_CALCULATION
{
	int nMarketDBID;
};

struct SCMarketResult			// SC_TRADE / SC_MARKET_REGISTER, SC_MARKET_CALCULATION
{
	int nMarketDBID;
	int nRet;
};

struct SCMarketInterrupt			// SC_TRADE / SC_MARKET_INTERRUPT
{
	int nMarketDBID;
	short wRegisterItemCount;		// ����� ������ ����
	int nRet;
};

struct TMarketCalculationInfo
{
	int nMarketDBID;					// market table db ID

	int nItemID;
	short wCount;		// ����
	USHORT wDur;			// ������
	int nRandomSeed;
	char cLevel;		// ����
	char cPotential;	// �����
	char cOption;
	char cSealCount;	// �к�ī��Ʈ

	int nPrice;			// ����
	int nUnitPrice;		// �ܰ�
	__time64_t tBuyDate;	// �Ǹ�����
	char cPayMethodCode;	// PayMethodCode (DB������) 1-���, 3-��Ż
	char cPotentialMoveCount;
};

struct SCMarketCalculationList			// SC_TRADE / SC_MARKET_CALCULATIONLIST
{
	int nRetCode;
	char cMarketCount;
	TMarketCalculationInfo MarketInfo[MYMARKETMAX];
};

struct SCMarketCalculationAll			// SC_TRADE / SC_MARKET_CALCULATIONALL
{
	int nRet;
};

struct SCNotifyMarket					// SC_TRADE / SC_MARKET_NOTIFY
{
	int nItemID;				// �Ǹŵ������ ItemID�� �־��ش�
	short wCalculationCount;	// ���갳��
};

struct SCMarketPetalBalance				// SC_TRADE / SC_MARKET_PETALBALANCE
{
	int nRetCode;
	int nPetalBalance;				// ��Ż
};

struct TMarketPrice
{
	bool bFlag;					// ��Ͽ���

	int nAvgPrice;
	int nMinPrice;
	int nMaxPrice;

	BYTE cPayMethodCode;		// 1 = ���ӸӴ�, 3 = ��Ż
};

struct CSMarketPrice			// CS_MARKET_PRICE
{
	int nMarketDBID;
	int nItemID;
	BYTE cLevel;
	BYTE cOption;
};

struct SCMarketPrice			// SC_MARKET_PRICE
{
	int nRetCode;

	int nMarketDBID;
	TMarketPrice ItemPrices[2];		// Index 0 : ���ӸӴ� 1: ��Ż
};

struct CSMarketMiniList						// CS_TRADE / CS_MARKETMINILIST
{
	int nItemID;
	char cPayMethodCode;					// 0-ȥ��, 1-����, 3-��Ż
};

typedef SCMarketList SCMarketMiniList;		// SC_TRADE / SC_MARKETMINILIST


// ���ΰŷ�
struct CSExchangeRequest		// CS_TRADE / CS_EXCHANGE_REQUEST		// �ŷ� ��û
{
	UINT nReceiverSessionID;	// �ŷ� �� ����
	bool bCancel;				// �ٽ� ����Ҳ���
};

struct SCExchangeRequest		// SC_TRADE / SC_EXCHANGE_REQUEST		// �ŷ� ��û
{
	UINT nSenderSessionID;		// �ŷ� ��û�� ����
	int nRet;
};

struct SCExchangeReject			// SC_TRADE / SC_EXCHANGE_REJECT		// �ŷ����
{
	UINT nSessionID;
	int nRetCode;
};

struct CSExchangeAccept			// CS_TRADE / CS_EXCHANGE_ACCEPT		// �ŷ� ����
{
	UINT nSenderSessionID;		// �ŷ� ��û�� ����
	bool bAccept;				// ���� ����
};

struct SCExchangeStart			// SC_TRADE / SC_EXCHANGE_START			// �ŷ� ����
{
	UINT nTargetSessionID;		// ����
	int nRet;
};

struct CSExchangeAddItem		// CS_TRADE / CS_EXCHANGE_ADDITEM,		// ������ ���
{
	BYTE cExchangeIndex;		// �ŷ�â �ε���
	BYTE cInvenIndex;
	short wCount;
	INT64 biItemSerial;
};

struct SCExchangeAddItem		// SC_TRADE / SC_EXCHANGE_ADDITEM		// ������ ���
{
	UINT nSessionID;
	BYTE cExchangeIndex;		// �ŷ�â �ε���
	TItemInfo ItemInfo;
	int nRet;
};

struct CSExchangeDeleteItem		// CS_TRADE / CS_EXCHANGE_DELETEITEM,	// ������ �������
{
	BYTE cExchangeIndex;		// �ŷ�â index
};

struct SCExchangeDeleteItem		// SC_TRADE / SC_EXCHANGE_DELETEITEM
{
	UINT nSessionID;			// �ŷ�â�� �ش��ϴ� ����
	BYTE cExchangeIndex;		// �ŷ�â index
	int nRet;
};

struct CSExchangeAddCoin		// CS_TRADE / CS_EXCHANGE_ADDCOIN,		// ���� ���
{
	INT64 nCoin;	// 21���� ���� ������
};

struct SCExchangeAddCoin		// SC_TRADE / SC_EXCHANGE_ADDCOIN
{
	UINT nSessionID;			// �ŷ�â�� �ش��ϴ� ����
	INT64 nCoin;	// 21���� ���� ������
	int nRet;
};

struct CSExchangeConfirm		// CS_TRADE / CS_EXCHANGE_CONFIRM
{
	char cType;		// eExchangeButton
};

struct SCExchangeConfirm		// SC_TRADE / SC_EXCHANGE_CONFIRM
{
	char cType;		// eExchangeButton
	UINT nSessionID;
};

struct SCExchangeComplete		// SC_TRADE / SC_EXCHANGE_COMPLETE
{
	int nRet;
};

#if defined(PRE_SPECIALBOX)
// specialbox
struct TSpecialBoxInfo			// CS_TRADE / CS_SPECIALBOX_LIST
{
	int nEventRewardID;
	BYTE cReceiveTypeCode;		// SpecialBox::ReceiveTypeCode
	BYTE cTargetTypeCode;		// SpecialBox::TargetTypeCode
	BYTE cWorldID;
	BYTE cTargetClassCode;
	BYTE cTargetMinLevel;
	BYTE cTargetMaxLevel;
	__time64_t tReserveSendDate;
	__time64_t tExpirationDate;
	//WCHAR wszEventName[MAILTITLELENMAX];
	WCHAR wszSenderName[NAMELENMAX];
	WCHAR wszContent[MAILTEXTLENMAX];
	INT64 biRewardCoin;
	bool bSystemSendFlag;
	char cEventRewardTypeCode;
};

struct SCSpecialBoxList			// SC_TRADE / SC_SPECIALBOX_LIST
{
	int nRet;
	char cCount;
	TSpecialBoxInfo BoxInfo[SpecialBox::Common::ListMax];
};

struct CSSpecialBoxItemList		// CS_TRADE / CS_SPECIALBOX_ITEMLIST
{
	int nEventRewardID;
};

struct TSpecialBoxItemInfo
{
	bool bCashItem;
	TItem RewardItem;
};

struct SCSpecialBoxItemList		// SC_TRADE / SC_SPECIALBOX_ITEMLIST
{
	int nRet;
	INT64 biRewardCoin;
	BYTE cCount;
	TSpecialBoxItemInfo BoxItem[SpecialBox::Common::RewardSelectMax];
};

struct CSReceiveSpecialBoxItem	// CS_TRADE / CS_SPECIALBOX_RECEIVEITEM
{
	int nEventRewardID;
	int nItemID;	// ���ú����϶��� ����
};

struct SCReceiveSpecialBoxItem	// SC_TRADE / SC_SPECIALBOX_RECEIVEITEM
{
	int nRet;
};

struct SCNotifySpecialBox		// SC_TRADE / SC_SPECIALBOX_NOTIFY
{
	int nNotifyCount;
	bool bNew;
};
#endif	// #if defined(PRE_SPECIALBOX)

//Community
//--------------------------------------------------------------------------------------------------
//	FRIEND (CS_FRIEND, SC_FRIEND)	
//--------------------------------------------------------------------------------------------------

//Client -> Server
struct CSFriendAddGroup			//CS_FRIEND / CS_FRIEND_GROUP_CREATE
{
	BYTE cNameLen;
	WCHAR wszBuf[FRIEND_GROUP_NAMELENMAX];
};

struct CSFriendDelGourp			//CS_FRIEND / CS_FRIEND_GROUP_DELETE
{
	UINT nGroupDBID;
};

struct CSFriendGroupUpdate	//CS_FRIEND / CS_FRIEND_GROUP_STATEUPDATE
{
	UINT nGroupDBID;
	BYTE cNameLen;
	WCHAR wszBuf[FRIEND_GROUP_NAMELENMAX];
};

struct CSFriendAdd					//CS_FRIEND / CS_FRIEND_ADD
{
	UINT nGroupDBID;
	BYTE cNameLen;
	WCHAR wszBuf[NAMELENMAX];
};

struct CSFriendDelete				//CS_FRIEND / CS_FRIEND_DELETE
{
	INT64 biFriendCharacterDBID;
};

struct CSFriendUpdate				//CS_FRIEND / CS_FRIEND_STATEUPDATE
{
	INT64 biFriendCharacterDBID;
	UINT nGroupDBID;
	BYTE cMemoLen;
	WCHAR wszBuf[FRIEND_MEMO_LEN_MAX];
};

struct CSFriendDetailInfo					//CS_FRIEND / CS_FRIEND_INFO
{
	INT64 biFriendCharacterDBID;
};

//Server -> Client
struct SCFriendGroupList			//SC_FRIEND / SC_FRIEND_GROUP_LIST
{
	BYTE cGroupCount;
	UINT nGroupDBID[FRIEND_GROUP_MAX];
	BYTE cGroupNameLen[FRIEND_GROUP_MAX];
	WCHAR wszBuf[FRIEND_GROUP_NAMELENMAX * FRIEND_GROUP_MAX];
};

struct TFriendInfo
{
	UINT nGroupDBID;
	INT64 biFriendCharacterDBID;
	TCommunityLocation Location;
	WCHAR wszFriendName[NAMELENMAX];
};

struct SCFriendList				//SC_FRIEND / SC_FRIEND_LIST
{
	int nRetCode;
	BYTE cCount;
	TFriendInfo Info[FRIEND_MAXCOUNT];
};

struct TFriendLocation
{
	UINT nGroupDBID;
	INT64 biFriendCharacterDBID;
	TCommunityLocation Location;
};

struct SCFriendLocationList		//SC_FRIEND / SC_FRIEND_LOCATIONLIST
{
	BYTE cCount;
	TFriendLocation FriendLocation[FRIEND_MAXCOUNT];
};

struct SCFriendGroupAdded		//SC_FRIEND / SC_FRIEND_GROUP_ADDED
{
	int nRetCode;
	UINT nGroupDBID;
	BYTE cNameLen;
	WCHAR wszBuf[FRIEND_GROUP_NAMELENMAX];
};

struct SCFriendAdded				//SC_FRIEND / SC_FRIEND_ADDED
{
	int nRetCode;
	TFriendInfo Info;
};

struct SCFriendGroupDeleted	//SC_FRIEND / SC_FRIEND_GROUP_DELETED
{
	UINT nGroupDBID;
	int nRetCode;
};


struct SCFriendGroupUpdated			//SC_FRIEND / SC_FRIEND_GROUP_UPDATED
{
	int nRetCode;
	UINT nGroupDBID;
	BYTE cNameLen;
	WCHAR wszBuf[FRIEND_GROUP_NAMELENMAX];
};

struct SCFriendDeleted				//SC_FRIEND / SC_FRIEND_ADDED_DELETED
{
	int nRetCode;
	BYTE cCount;
	INT64 biFriendCharacterDBID[FRIEND_MAXCOUNT];
};

struct SCFriendUpdated				//SC_FRIEND / SC_FRIEND_UPDATED
{
	int nRetCode;
	UINT nGroupDBID;					//�̵��� �׷��� �����̵�
	BYTE cCount;
	INT64 biFriendCharacterDBID[FRIEND_MAXCOUNT];
};

struct SCFriendDetailInfo			//SC_FRIEND / SC_FRIEND_INFO
{
	INT64 biFriendCharacterDBID;
	UINT nGroupDBID;
	USHORT nClass;
	USHORT nJob;
	BYTE cCharacterLevel;
	TCommunityLocation Location;
};

struct SCFriendResult						//SC_FRIEND / SC_FRIEND_RESULT
{
	int nRet;
};

struct SCFriendAddNotice		//SC_FRIEND / SC_FRIEND_ADDNOTICE
{
	WCHAR wszName[NAMELENMAX];
};

//--------------------------------------------------------------------------------------------------
//	SC_ISOLATE (CS_BLACKLIST_GET, SC_GETGUILD)
//--------------------------------------------------------------------------------------------------
// Isolation
struct CSIsolateAdd			//CS_ISOLATE / CS_ISOLATE_ADD
{
	WCHAR wszIsolateName[NAMELENMAX];
};

struct CSIsoLateDelete		//CS_ISOLATE / CS_ISOLATE_DELETE
{
	WCHAR wszIsolateName[NAMELENMAX];
};

struct SCIsolateList		//SC_ISOLATE / SC_ISOLATE_LIST
{
	BYTE cCount;
	BYTE cNameLen[ISOLATELISTMAX];
	WCHAR wszIsolateName[NAMELENMAX * ISOLATELISTMAX];
};

struct SCIsolateAdd			//SC_ISOLATE / SC_ISOLATE_ADD
{
	WCHAR wszIsolateName[NAMELENMAX];
};

struct SCIsolateDelete		//SC_ISOLATE / SC_ISOLATE_DELETE
{
	BYTE cCount;
	BYTE cNameLen[ISOLATELISTMAX];
	WCHAR wszIsolateName[NAMELENMAX * ISOLATELISTMAX];
};

struct SCIsolateResult		//SC_ISOLATE / SC_ISOLATE_RESULT
{
	int nRetCode;
};


//--------------------------------------------------------------------------------------------------
//	GUILD (CS_GUILD, SC_GUILD)
//--------------------------------------------------------------------------------------------------

// [CL -> GA/VI]

// CS_GUILD / CS_CREATEGUILD					// ��� â�� ��û
struct CSCreateGuild
{
	WCHAR wszGuildName[GUILDNAME_MAX];		// ��� �̸�
};

// CS_GUILD / CS_INVITEGUILDMEMBREQ				// ���� �ʴ� ��û
struct CSInviteGuildMemberReq
{
	WCHAR wszToCharacterName[NAMELENMAX];		// �ʴ��� ĳ���� �̸�
};

// CS_GUILD / CS_INVITEGUILDMEMBACK				// ���� �ʴ� ����
struct CSInviteGuildMemberAck
{
	TGuildUID GuildUID;		// ��� UID
	UINT nFromSessionID;		// �ʴ��� ���� ID
	bool bAccept;				// ���� ����

	UINT nFromAccountID;		// �ʴ��� UINT
};

// CS_GUILD / CS_EXILEGUILDMEMB					// ���� �߹� ��û
struct CSExileGuildMember
{
	UINT nAccountDBID;		// ���� DBID
	INT64 nCharacterDBID;		// ĳ���� DBID
};

// CS_GUILD / CS_CHANGEGUILDINFO				// ��� ���� ���� ��û
struct CSChangeGuildInfo
{
	BYTE btGuildUpdate;		// ��� ���� ���� Ÿ�� (eGuildUpdateType)
	int Int1;
	int Int2;
	int Int3;
	int Int4;
	INT64 Int64;
	WCHAR Text[GUILDPARAMTEXT_MAX+1];
};

// CS_GUILD / CS_CHANGEGUILDMEMBINFO			// ���� ���� ���� ��û
struct CSChangeGuildMemberInfo
{
	BYTE btGuildMemberUpdate;		// ���� ���� ���� Ÿ�� (eGuildMemberUpdateType)
	UINT nChgAccountDBID;					// ������ ���� DBID (�� ������ �����ϴ� ���̸� 0 �Է�)
	INT64 nChgCharacterDBID;					// ������ ĳ���� DBID (�� ������ �����ϴ� ���̸� 0 �Է�)
	int Int1;
	int Int2;
	INT64 Int64;
	WCHAR Text[GUILDPARAMTEXT_MAX+1];
};

// CS_GUILD / CS_GETGUILDHISTORYLIST			// ��� �����丮 ��� ��û
struct CSGetGuildHistoryList
{
	INT64 biIndex;	// ��� �����丮 ��� �ε��� (0 �̸� ���� �ֱ� ��Ϻ��� Ȯ�� ?)
	bool bDirection;				// ��� ���� ���� (0:�ε��� ���� / 1:�ε��� ����)
};

// CS_GUILD / CS_GETGUILDINFO					// ��� ���� ��û
struct CSGetGuildInfo
{
	bool bNeedMembList;	// ���� ��ϵ� �ʿ����� ����
};

// SC_GUILD / SC_CREATEGUILD					// ��� â�� ���
struct SCCreateGuild
{
	UINT nSessionID;				// ���� ID
	TGuildUID GuildUID;			// ��� UID (������ ��� ���õ�)
	TGuild Info;					// ��� ����
	UINT nAccountDBID;			// â���� ����� ���� DBID
	INT64 nCharacterDBID;			// â���� ĳ���� DBID
	TP_JOB nJob;					// �ʴ��� ĳ���� ��������
	CHAR cLevel;					// �ʴ��� ĳ���� ����
	TCommunityLocation Location;	// ���� ��ġ (�α��� ���ε� �� ����� ���·� üũ)
	int iErrNo;					// ��� (NOERROR : ���� / �׿� : ����)
	WCHAR wszGuildName[GUILDNAME_MAX];	// ��� �̸�
};

// SC_GUILD / SC_DISMISSGUILD					// ��� ��ü ���
struct SCDismissGuild
{
	UINT nSessionID;		// ���� ID
	int iErrNo;			// ��� (NOERROR : ���� / �׿� : ����)
	TGuildUID GuildUID;	// ��� UID (������ ��� ���õ�)
};

// SC_GUILD / SC_INVITEGUILDMEMBREQ				// ���� �ʴ� ��û
struct SCInviteGuildMemberReq
{
	TGuildUID GuildUID;		// ��� UID
	UINT nFromAccountDBID;	// ��û�� ����� ���� DBID
	UINT nFromSessionID;		// ��û�� ���� ID
	UINT nToAccountDBID;		// �ʴ��� ����� ���� DBID
	UINT nToSessionID;		// �ʴ��� ���� ID
	int iErrNo;				// ��� (NOERROR : ���� / �׿� : ����)
	WCHAR wszFromCharacterName[NAMELENMAX];	// ��û�� ĳ���� �̸� (P.S.> ��û�� ����ڿ��� ������ �۽ŵ� ��� �ʴ��� ������ �̸��� ��)
	WCHAR wszGuildName[GUILDNAME_MAX];		// ����
};

// SC_GUILD / SC_INVITEGUILDMEMBACK				// ���� �ʴ� ���
struct SCInviteGuildMemberAck
{
	UINT nToAccountDBID;			// �ʴ��� ���� DBID
	INT64 nToCharacterDBID;		// �ʴ��� ĳ���� DBID
	UINT nFromAccountDBID;		// ��û�� ���� DBID
	INT64 nFromCharacterDBID;		// ��û�� ĳ���� DBID
	TGuildUID GuildUID;			// ��� UID (������ ��� ���õ�)
	TP_JOB nJob;					// �ʴ��� ĳ���� ��������
	CHAR cLevel;					// �ʴ��� ĳ���� ����
	TCommunityLocation Location;	// ���� ��ġ (�α��� ���ε� �� ����� ���·� üũ)
	int iErrNo;					// ��� (NOERROR : ���� / �׿� : ����)
	WCHAR wszToCharacterName[NAMELENMAX];	// �ʴ��� ĳ���� �̸�
};

// SC_GUILD / SC_LEAVEGUILDMEMB					// ���� Ż�� ���
struct SCLeaveGuildMember
{
	UINT nAccountDBID;		// Ż���� ���� DBID
	INT64 nCharacterDBID;		// Ż���� ĳ���� DBID
	TGuildUID GuildUID;		// ��� UID (������ ��� ���õ�)
	int iErrNo;				// ��� (NOERROR : ���� / �׿� : ����)
#ifdef PRE_ADD_BEGINNERGUILD
	bool bGraduateBeginnerGuild;		//�ʺ��ڱ������
#endif		//#ifdef PRE_ADD_BEGINNERGUILD
};

// SC_GUILD / SC_EXILEGUILDMEMB					// ���� �߹� ���
struct SCExileGuildMember
{
	UINT nAccountDBID;		// �߹��� ���� DBID
	INT64 nCharacterDBID;		// �߹��� ĳ���� DBID
	TGuildUID GuildUID;		// ��� UID (������ ��� ���õ�)
	int iErrNo;				// ��� (NOERROR : ���� / �׿� : ����)
};

// SC_GUILD / SC_CHANGEGUILDINFO				// ��� ���� ���� ���
struct SCChangeGuildInfo
{
	UINT nAccountDBID;				// ��û�� ���� DBID (�Ϲ����� ��� �߿����� �ʰ� ���� �뵵�θ� ���)
	INT64 nCharacterDBID;				// ��û�� ĳ���� DBID (�Ϲ����� ��� �߿����� �ʰ� ���� �뵵�θ� ���)
	BYTE btGuildUpdate;		// ��� ���� ���� Ÿ�� (eGuildUpdateType)
	int Int1;
	int Int2;
	int Int3;
	int Int4;
	INT64 Int64;
	WCHAR Text[GUILDHISTORYTEXT_MAX+1];
	TGuildUID GuildUID;		// ��� UID (������ ��� ���õ�)
	int iErrNo;				// ��� (NOERROR : ���� / �׿� : ����)
};

// SC_GUILD / SC_CHANGEGUILDMEMBINFO			// ���� ���� ���� ���
struct SCChangeGuildMemberInfo
{
	UINT nReqAccountDBID;	// ��û�� ���� DBID
	INT64 nReqCharacterDBID;	// ��û�� ĳ���� DBID
	UINT nChgAccountDBID;	// ����� ���� DBID (������� ���� ������ �����ϴ� ��쿡�� ��û�ڿ� �޶���)
	INT64 nChgCharacterDBID;	// ����� ĳ���� DBID (������� ���� ������ �����ϴ� ��쿡�� ��û�ڿ� �޶���)
	BYTE btGuildMemberUpdate;		// ���� ���� ���� Ÿ�� (eGuildMemberUpdateType)
	int Int1;
	int Int2;
	INT64 Int64;
	WCHAR Text[GUILDPARAMTEXT_MAX+1];
	TGuildUID GuildUID;		// ��� UID (������ ��� ���õ�)
	int iErrNo;				// ��� (NOERROR : ���� / �׿� : ����)
};

// SC_GUILD / SC_GETGUILDHISTORYLIST			// ��� �����丮 ��� ����
struct SCGetGuildHistoryList
{
	TGuildUID GuildUID;			// ��� UID
	INT64 biIndex;	// ��� �����丮 ��� �ε��� (0 �̸� ���� �ֱ� ��Ϻ��� Ȯ�� ?)
	bool bDirection;				// ��� ���� ���� (0:�ε��� ���� / 1:�ε��� ����)
	int nCurCount;				// ��� �����丮 ���� �ε��� �� �Է� �ε��� ������ ����
	int nTotCount;				// ��� �����丮 ��ü ����
	int iErrNo;					// ��� (NOERROR : ���� / �׿� : ����)
	int nCount;					// ��� �����丮 ��� ���� (������ ��� ���õ�)
	TGuildHistory HistoryList[GUILDHISTORYLIST_MAX];	// ��� �����丮 ��� (������ ��� ���õ�)
};

// SC_GUILD / SC_GETGUILDINFO					// ��� ���� ����
struct SCGetGuildInfo
{
	int iErrNo;	// ��� (NOERROR : ���� / �׿� : ����)
 	TGuild Info;	// ��� ���� (������ ��� ���õ�)	
	bool bNotRecruitMember;		// ������ �Խ��� ���Խ��� ���ɻ��� üũ( false:���԰��� true:���ԺҰ�)
};
struct SCGetGuildMember
{	
	int iErrNo;	// ��� (NOERROR : ���� / �׿� : ����)
	int nPage;	// ���� ������ ����
	bool bEndMember;	// ���������� üũ
	int nCount;	// ���� ���� ���� (������ ��� ���õ�)
	TGuildMember MemberList[SENDGUILDMEMBER_MAX];	// ���� ����

};

// SC_GUILD / SC_CHANGEGUILDSELFVIEW
struct SCChangeGuildSelfView
{
	UINT nSessionID;				// ���� ID
	TGuildSelfView GuildSelfView;	// ��� �ð����� (����)
};

// SC_GUILD / SC_GUILDMEMBLOGINLIST
struct SCGuildMemberLoginList
{
	TGuildUID GuildUID;			// ��� UID
	int nCount;					// �α����� ���� ��
	INT64 List[GUILDSIZE_MAX];	// �α����� ���� ���
};


struct SCOpenGuildWare				// SC_GUILD	/ SC_OPEN_GUILDWARE
{
	int					nError;							// ������ó��
	int					nResult;						// 0:����Ʈ ���� 1:����Ʈ������
	short				wWareSize;						// â�������
	
	USHORT				nTakeWareItemCount;				// â��������� ���� Ƚ�� (�Ϸ����)
	INT64				nWithdrawCoin;					// ���� ��带 ���� �ݾ�
	INT64				nGuildCoin;						// �������

	__time64_t			VersionDate;					// â�� ����

	BYTE				cWareCount;						// ���������ī��Ʈ
	TItemInfo			WareList[GUILD_WAREHOUSE_MAX];	// �����Ʈ
};

struct CSGetGuildWareHistory		// CS_GUILD / CS_GET_GUILDWARE_HISTORY		// ���â�� ��� �����丮
{
	int					nIndex;							// �����丮 ��û �ε���
};

struct SCGetGuildWareHistory		// SC_GUILD / SC_GET_GUILDWARE_HISTORY
{
	int					nCurrCount;
	int					nTotalCount;
	int					nErrNo;
	int					nCount;
	TGuildWareHistory	HistoryList[GUILD_WAREHOUSE_HISTORYLIST_MAX];// �����丮 ����Ʈ
};

// SC_GUILD / SC_GUILDMEMBLOGINLIST
struct SCChangeGuildName
{	
	WCHAR wszGuildName[GUILDNAME_MAX];		// ��� �̸� (�ϴ� ��� â�� ���Ŀ��� �̸��� ������� �ʴ°��� ������ ��)
};

struct SCChangeGuildMark			// SC_GUILD / SC_CHANGEGUILDMARK
{
	short wMark;
	short wMarkBG;
	short wMarkBorder;
};

struct SCGuildLevelUp				// SC_GUILD / SC_GUILDLEVELUP
{
	int nLevel;
};

struct SCUpdateGuildExp				// SC_GUILD / SC_UPDATEGUILDEXP
{
	int nError;				// ��� (ERROR_NONE : ���� / ERROR_GUILD_DAILYLIMIT : ����)
	BYTE cPointType;				
	int nPointValue;
	int nGuildExp;
	INT64 biCharacterDBID;
	int nMissionID;
};

// SC_GUILD_GET_REWARDITEM
struct SCGetGuildRewardItem
{
	TGuildRewardItem GuildRewardItem[GUILDREWARDEFFECT_TYPE_CNT];
};
struct CSBuyGuildRewardItem
{
	int nItemID;
};
struct SCBuyGuildRewardItem
{
	int	iRet;
	int nItemID;
};
struct SCAddGUildRewardItem
{
	TGuildRewardItem GuildRewardItem;
};
struct SCExtendGuildSize
{
	short	nGuildSize;
};
struct CSPlayerGuildInfo
{
	int nSessionID;
};
struct SCPlayerGuildInfo
{
	UINT nSessionID;							// ���� ���Ǿ��̵�
	WCHAR wszMasterName[NAMELENMAX];			// ��帶���� ĳ���� �̸�
	WCHAR wszGuildName[GUILDNAME_MAX];		// ����
	int nGuildLevel;							// ��� ����
	int nMemberSize;
	int nGuildSize;
	TGuildRewardItem GuildRewardItem[GUILDREWARDEFFECT_TYPE_CNT];
};

//SC_GUILD / SC_EXTEND_GUILDWARESIZE
struct SCExtendGuildWare
{
	short wTotalSize;
};

// SC_GUILD / SC_ENROLL_GUILDWAR
struct SCEnrollGuildWar
{
	int iErrorNo;
	short wScheduleID;
	BYTE cTeamColorCode;
};

// SC_GUILD / SC_CHANGE_WAREVENT
struct SCGuildWarEvent
{
	short wScheduleID;		// ����
	char cEventStep;		// �̺�Ʈ ����: GUILDWAR_STEP_NONE ~ GUILDWAR_STEP_END
	char cEventType;		// GUILDWAR_EVENT_START(����) / GUILDWAR_EVENT_END(��)
};

// SC_GUILD / SC_GUILDWAR_FESTIVAL_POINT
struct SCGuildWarFestivalPoint
{
	INT64 biGuildWarFestivalPoint;
};

// SC_GUILD / SC_GUILDWAR_TRIAL_RESULT_OPEN
struct SGuildWarRankingInfo	
{
	short wRanking;							// ��� ����
	WCHAR wszGuildName[GUILDNAME_MAX];	// ��� �̸�
	WCHAR wszGuildMasterName[NAMELENMAX];	// ����� �̸�
	short wCurGuildUserCount;				// ���� ��� �ο�
	short wMaxGuildUserCount;				// �ִ� ��� �ο�
	int nTotalPoint;						// ȹ���� ����
	UINT nGuildDBID;						// ��� DBID
};

struct SGuildWarMissionGuildRankingInfo	//�� �ι��� ��� ����
{
	WCHAR wszGuildName[GUILDNAME_MAX];	// ��� �̸�
	WCHAR wszGuildMasterName[NAMELENMAX];	// ����� �̸�
	int nTotalPoint;						// ȹ���� ����
};

struct SMyGuildWarMissionGuildRankingInfo // �츮 ����� �ι��� ����
{
	short wRanking;							// �ι��� ����
	int nTotalPoint;						// ȹ���� ����
};

struct SGuildWarMissionRankingInfo
{
	WCHAR wszCharName[NAMELENMAX];			// 1�� �̸�
	WCHAR wszGuildName[GUILDNAME_MAX];	// ��� �̸�
	int nTotalPoint;						// ȹ���� ����
};

struct SMyGuildWarMissionRankingInfo
{
	short wRanking;							// ����
	int nTotalPoint;						// ȹ���� ����
};

struct SGuildWarDailyAward
{
	WCHAR wszCharName[NAMELENMAX];			// ������ ĳ����
	TGuildUID nGuildUID;					// ������ ����� UID;
};

struct SCGuildWarTrialResult
{
	// ��� ���� ���
	int nBluePoint;			// ���� û�� ����
	int nRedPoint;			// ���� ȫ�� ����
	bool bGuildWar;			// ����� ���� ����(��� false�� �ؿ� ����,�츮��� ������ ��Ŷ�� ���Ե��� �ʽ��ϴ�.)	
	bool bReward;			// ����ޱ� ����
	// ���� ���� ���
	SGuildWarRankingInfo	sGuildWarRankingInfo[GUILDWAR_FINALS_TEAM_MAX];	

	// �ι��� ��� ����(��ü)
	SGuildWarMissionGuildRankingInfo sGuildWarMissionGuildRankingInfo[GUILDWAR_RANKINGTYPE_MAX];	// ��ü ��� ����
	// �ι��� ���� ����(��ü)
	SGuildWarMissionRankingInfo sGuildWarMissionRankingInfo[GUILDWAR_RANKINGTYPE_MAX];			// ���κ� ����
	// Ư����
	// ������ ������ ���μ����� ���� �״�� �����ֽø� �˴ϴ�.
	SGuildWarDailyAward sGuildWarDailyAward[GUILDWAR_DAILY_AWARD_MAX];							// ���ں� �û�	

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// ������, �츮��� ������(���� ��尡 ���ų� ������� �������� �ʴ³��̸� �� ������ ������ �ʽ��ϴ�.)
	int nFestivalPoint;		// �⺻ ���� 
	int nFestivalPointAdd;	// ������ ���� +�� ����
	// �츮 ��� ���� ���� (��ü����)
	SMyGuildWarMissionGuildRankingInfo sMyGuildWarRankingInfo;
	// �ι��� ��� ����(�츮���)
	SMyGuildWarMissionGuildRankingInfo sMyGuildWarMissionGuildRankingInfo[GUILDWAR_RANKINGTYPE_MAX]; // �츮 ��� ����	
	// �ι��� �� ����
	SMyGuildWarMissionRankingInfo sMyGuildWarMissionRaningInfo[GUILDWAR_RANKINGTYPE_MAX];			 // �� ����	
};

// SC_GUILD / SC_GUILDWAR_STATUS_OPEN
struct SCGuildWarStatus
{	
	int nBluePoint;			// û�� ����	
	int nRedPoint;			// ȫ�� ����	
	int nMyPoint;			// �� ����
	int nMyGuildPoint;		// ��� ���� ����
	int nRankingCount;		// ��ŷ ī��Ʈ..��� ���ڸ�ŭ ���� ���ϴ�.
	SGuildWarRankingInfo	sGuildWarPointTrialRanking[GUILDWAR_TRIAL_POINT_TEAM_MAX];
};
// CS_GUILD / CS_GUILDWAR_VOTE
struct CSGuildWarVote
{
	TGuildUID GuildUID;		// ��ǥ�� ��� UID (���� ID + ��� DBID)
};
// SC_GUILD / SC_GUILDWAR_VOTE
struct SCGuildWarVote
{
	int nRetCode;			// ��ǥ ���
	int nAddFestivalPoint;	// ���� ���� ����Ʈ
};
// SC_GUILD / SC_GUILDWAR_TOURNAMENT_INFO_OPEN
struct SGuildTournamentInfo
{
	TGuildUID GuildUID;						// ��� UID (���� ID + ��� DBID)
	WCHAR wszGuildName[GUILDNAME_MAX];	// ��� �̸�
	char cMatchTypeCode;					// ������ ���� 1=���, 2=4��, 3=8��, 4=16��
	bool bWin;								// �̰���� ������ ����	
	short wGuildMark;						// ��帶ũ ����
	short wGuildMarkBG;						// ��帶ũ ���
	short wGuildMarkBorder;					// ��帶ũ �׵θ�
};
struct SCGuildTournamentInfo
{
	SGuildTournamentInfo TournamentInfo[GUILDWAR_FINALS_TEAM_MAX];
	char cMatchTypeCode;					// ���� �������� ���� 1=���, 2=4��, 3=8��, 4=16��
	__time64_t tStartTime;					// �̰� 0�̸� ���� �������̰� ���� ������ �̶� �����ϴ� ���Դϴ�.
	bool bPopularityVote;					// �α���ǥ Ȱ��ȭ, ��Ȱ��ȭ
};
// SC_GUILD / SC_GUILDWAR_VOTE_TOP
struct SCGuildWarVoteTop
{
	WCHAR wszGuildName[GUILDNAME_MAX];	// ��� �̸�
	int nVoteCount;							// �� ��ǥ�� ��
};
// SC_GUILD / SC_GUILDWAR_COMPENSATION
struct SCGuildWarCompensation
{
	int nRetCode;				// ���� �ޱ� ���
};
// SC_GUILD / SC_GUILDWAR_PRE_WIN
struct SCGuildWarPreWin
{
	bool bPreWin;				// true�� �츮��尡 ��±����, false�� ��±�� ����..
};
// CS_GUILD / CS_GUILDWAR_WIN_SKILL
struct CSGuildWarWinSkill
{
	int nSkillID;				// ���� ��ųID
};
// SC_GUILD / SC_GUILDWAR_WIN_SKILL
struct SCGuildWarWinSkill
{
	int nRetCode;
	DWORD dwCoolTime;			// ���� ��Ÿ��
};
// SC_GUILD / SC_GUILDWAR_USER_WINSKILL
#if defined(_GAMESERVER) || defined(_VILLAGESERVER) || defined(_CLIENT)
struct SCGuildWarUserWinSkill
{
	TGuildUID GuildUID;			// ��ų�� ������ GuildUID
	EtVector3 vPos;				// ��ų�� ������ ��ǥ.
	int nSkillID;				// ��ųID
};
#endif //#if defined(_GAMESERVER) || defined(_VILLAGESERVER) || defined(_CLIENT)

// SC_GUILD / SC_GUILDWAR_EVENT_TIME
struct SGuildWarTime
{
	__time64_t tBeginTime;
	__time64_t tEndTime;
};
struct SCGuildWarEventTime
{	
	bool bFinalProgress;
	// ��ü ����� ������
	SGuildWarTime tGuildWarTime[GUILDWAR_STEP_END-1]; // eGuildWarStepType���� -1�� ���ּ���..
	// ���� ���� ����
	SGuildWarTime tFinalPartTime[GUILDWAR_FINALPART_MAX-1]; //eGuildFinalPart���� -1�� ���ּ���..
};

// SC_GUILD / SC_GUILDWAR_TOURNAMENT_WIN
struct SCGuildWarTournamentWin
{
	// �¸� ����
	char cMatchTypeCode;					// ���� �������� ���� 1=���, 2=4��, 3=8��, 4=16��
	// �¸��� ����
	WCHAR wszGuildName[GUILDNAME_MAX];	// ��� �̸�
};

//--------------------------------------------------------------------------------------------------
//	PVP (CS_PVP, SC_PVP)
//--------------------------------------------------------------------------------------------------

//blondy 
//Ŭ���̾�Ʈ���� �� �����̳�
struct sRoomInfo
{
	BYTE	cGameMode;		// PvPCommon::GameMode
	BYTE	cVSMode;		// PvPCommon::VSMode
	WCHAR	RoomName[PvPCommon::TxtMax::RoomName];
	BYTE	cRoomState;		// �����
	BYTE    cPlayerNum;		//�����ο�
	BYTE    cMaxPlayerNum;	//�ִ��ο�
	UINT    nMapIndex;      //���ε���
	UINT    nPVPIndex;      //���ε���
	bool	bInGameJoin;	//���԰���
    BYTE    cMinLevel; 
	BYTE    cMaxLevel;    
	UINT    uiObjective;      //������Ƽ��
	bool	bDropItem;		//��������� ���
	int		nEventID;
	BYTE	cShowHP;
#ifdef PRE_ADD_COLOSSEUM_BEGINNER
	BYTE	cRoomType;		//PvPCommon::RoomType
#endif // #ifdef PRE_ADD_COLOSSEUM_BEGINNER

	UINT nGuildDBID[PvPCommon::TeamIndex::Max];
#ifdef PRE_MOD_PVPOBSERVER
	bool bExtendObserver;
#endif // PRE_MOD_PVPOBSERVER
#ifdef PRE_WORLDCOMBINE_PVP
	int nWorldPvPRoomType;
#endif // PRE_WORLDCOMBINE_PVP
#ifdef PRE_PVP_GAMBLEROOM
	BYTE cGambleType;
	int nGamblePrice;
#endif // PRE_PVP_GAMBLEROOM
};
//blondyend

struct CSPVP_CREATEROOM
{
	UINT	uiMapIndex;
	UINT	uiGameModeTableID;
	UINT	uiSelectWinCondition;
	UINT	uiSelectPlayTimeSec;
	BYTE	cMaxUser;
	BYTE	cMinUser;
	BYTE	cRoomNameLen;
	BYTE	cRoomPWLen;
	BYTE	cMinLevel;
	BYTE	cMaxLevel;
	USHORT	unRoomOptionBit;	// PvPCommon::RoomOption Bit
	UINT    uiEventItemID;
#ifdef PRE_ADD_COLOSSEUM_BEGINNER
	BYTE cRoomType;		//PvPCommon::RoomType
#endif		//#ifdef PRE_ADD_COLOSSEUM_BEGINNER
#ifdef PRE_MOD_PVPOBSERVER
	bool bExtendObserver;
#endif		//#ifdef PRE_MOD_PVPOBSERVER
#if defined( PRE_WORLDCOMBINE_PVP )
	int nWorldPvPRoomType;	
#endif
	WCHAR	wszBuf[PvPCommon::TxtMax::RoomName + PvPCommon::TxtMax::RoomPW];
};

struct SCPVP_CREATEROOM
{
	int					nRetCode;
	UINT				uiPvPIndex;
	CSPVP_CREATEROOM	sCSPVP_CREATEROOM;
};

struct CSPVP_MODIFYROOM
{
	CSPVP_CREATEROOM	sCSPVP_CREATEROOM;
};

struct SCPVP_MODIFYROOM
{
	int					nRetCode;
	CSPVP_MODIFYROOM	sCSPVP_MODIFYROOM;
};

struct SCPVP_LEAVEROOM
{
	short								nRetCode;
	UINT								uiLeaveUserSessionID;
	PvPCommon::LeaveType::eLeaveType	eType;
};

struct CSPVP_CHANGECAPTAIN
{
	PvPCommon::CaptainType::eCode Type;
	UINT	uiNewCaptainSessionID;
};

struct CSPVP_ROOMLIST
{
	UINT	uiPage;
	BYTE	cRoomSortType;		// PvPCopmmon::RoomSortType ����
	BYTE	cIsAscend;			// ��������:1 ��������:0
	UINT	uiSortData;			// cRoomSortType �� ���� �ʿ��� �ΰ� ����
#ifdef PRE_ADD_COLOSSEUM_BEGINNER
	BYTE cChannelType;
#endif		//#ifdef PRE_ADD_COLOSSEUM_BEGINNER
};

struct SCPVP_ROOMLIST
{
	short					nRetCode;
	BYTE					cRoomCount;
	UINT					uiMaxPage;
	PvPCommon::RoomInfoList	RoomInfoList;
};

struct CSPVP_WAITUSERLIST
{
	PvPCommon::WaitUserList::SortType::eSortType	SortType;
	bool											bIsAscend;	// ��������:true ��������:false
	USHORT											unPage;	// ZeroBase
};

struct SCPVP_WAITUSERLIST
{
	short								nRetCode;
	BYTE								cUserCount;
	USHORT								unMaxPage;
	PvPCommon::WaitUserList::Repository	Repository;
};

struct CSPVP_JOINROOM
{
	UINT	uiPvPIndex;
	bool	bIsObserver;		// �����ϱ�
	UINT nGuildDBID;			//����� ������ ���
	BYTE	cRoomPWLen;
	WCHAR	wszRoomPW[PvPCommon::TxtMax::RoomPW];
};

struct SCPVP_JOINROOM
{
	short					nRetCode;
	PvPCommon::RoomInfo		RoomInfo;				// ������
	BYTE					cUserCount;				// ������
	PvPCommon::UserInfoList	UserInfoList;
};

struct SCPVP_JOINUSER
{
	PvPCommon::UserInfo		UserInfo;
};

struct CSPVP_READY
{
	BYTE	cReady;
};

struct CSPVP_START
{
	USHORT	unCheck;		// PvPCommon::StartCheat ����
};

struct CSPVP_CHANGETEAM
{
	USHORT	usTeam;			// PvPCommon::Team
};

struct SCPVP_CHANGETEAM
{
	short	nRetCode;
	UINT	uiSessionID;
	USHORT	usTeam;			// PvPCommon::Team
	char cTeamSlotIndex;
};

struct SCPVP_USERSTATE
{
	UINT	uiSessionID;
	UINT	uiUserState;		// PvPCommon::UserState
};

struct SCPVP_START
{
	short	nRetCode;
};

struct SCPVP_STARTMSG
{
	BYTE	cSec;
};

struct SCPVP_ROOMSTATE
{
	UINT	uiRoomState;	
};

struct SCPVP_MODE_STARTTICK
{
	UINT	uiStartTick;
	UINT	uiCurTick;
};

struct SCPVP_FINISH_PVPMODE
{
	UINT	uiWinTeam;		// PvPCommon::Team ����
	PvPCommon::FinishReason::eCode	Reason;		// PvPCommon::FinishReason ����
	UINT	uiATeamScore;
	UINT	uiBTeamScore;
#if defined(PRE_ADD_PVP_TOURNAMENT)
	UINT	uiWinSessionID;
#endif
};

struct SCPVP_FINISH_PVPROUND
{
	UINT	uiWinTeam;		// PvPCommon::Team ����
	PvPCommon::FinishReason::eCode	Reason;		// PvPCommon::FinishReason ����
	UINT	uiATeamScore;	// RoundMode�� GameModeScore�� �������� �˷��ش�.
	UINT	uiBTeamScore;
#if defined(PRE_ADD_PVP_TOURNAMENT)
	UINT	uiWinSessionID;
	char	cTournamentStep; // PvPCommon::Tournament::eStep
#endif
};

struct SCPVP_SUCCESSBREAKINTO
{
	UINT	uiSessionID;
};

struct SCPVP_ADDPOINT
{
	UINT	uiSessionID;
	UINT	uiScoreType;
	UINT	uiAddPoint;
};

struct SCPVP_SELECTCAPTAIN
{
	UINT	uiSessionID;
};

struct TZombieInfo
{
	UINT	uiSessionID;
	int		nMonsterMutationTableID;
	bool	bZombie;
	bool	bRemoveStateBlow;
	int		iScale;
};

struct SCPVP_SELECTZOMBIE
{
	BYTE	cCount;
	TZombieInfo Zombies[PARTYMAX];
};

struct SCPVP_LEVEL
{
	UINT	uiSessionID;
	BYTE	cLevel;
};

struct SCPVP_ROOMINFO
{
	UINT	uiIndex;
	UINT	uiWinCondition;
	UINT	uiGameModeTableID;
	UINT	uiPlayTimeSec;
	BYTE	cMaxUser;
	bool bIsGuildWar;		//�̰� Ʈ��� �����⸸ Ȱ��ȭ �������� ���� ����
#if defined( PRE_PVP_GAMBLEROOM )
	BYTE cGambleType;
	int nGamblePrice;
#endif
};

struct TPvPUserScore
{
	UINT	uiSessionID;
	UINT	uiKillScore;
	UINT	uiAssistScore;
	UINT	uiSupportScore;
	UINT	uiInstantSupportScore;
	UINT	uiKillClassScore[PvPCommon::Common::MaxClass];
	UINT	uiKilledClassScore[PvPCommon::Common::MaxClass];
};

struct TPvPUserDieCount
{
	UINT	uiSessionID;
	UINT	uiDieCount;
};

struct TPvPUserCount
{
	UINT	uiSessionID;
	UINT	uiKillCount;
	UINT	uiDieCount;
	UINT	uiContinuousKillCount;
};

struct SCPVP_MODE_SCORE
{
	USHORT				unATeamScore;
	USHORT				unBTeamScore;
	BYTE				cScoreCount;
	BYTE				cCount;
	TPvPUserScore		sPvPUserScore[PvPCommon::Common::MaxPlayer];
	TPvPUserCount		sPvPUserCount[PvPCommon::Common::MaxPlayer];
};

struct TPvPCaptainKillCount
{
	UINT		uiSessionID;
	UINT		uiCaptainKillCount;
};

struct SCPVP_CAPTAINKILL_COUNT
{
	BYTE					cCount;
	TPvPCaptainKillCount	sCaptainKillCount[PvPCommon::Common::MaxPlayer];
};

struct TPvPZombieKillCount
{
	UINT		uiSessionID;
	UINT		uiZombieKillCount;
};

struct SCPVP_ZOMBIEKILL_COUNT
{
	BYTE				cCount;
	TPvPZombieKillCount	sZombieKillCount[PvPCommon::Common::MaxPlayer];
};

struct TPvPOccupationScoreInfo
{
	UINT nSessionID;
	USHORT nTryAcquireCount;
	USHORT nAcquireCount;
	BYTE cBossKillCount;
	USHORT nStealAquireCount;
};

struct TPvPOccupationTeamScoreInfo
{
	USHORT nTeamID;
	int nTeamKillScore;
	int nTeamAcquireScore;
	int nTeamStealAcquireScore;

	TPvPOccupationTeamScoreInfo()
		: nTeamID( )
		, nTeamKillScore( 0 )
		, nTeamAcquireScore( 0 )
		, nTeamStealAcquireScore( 0 )
	{}
};

struct TPvPOccupationScore
{
	TPvPOccupationTeamScoreInfo TeamScoreInfo[PvPCommon::TeamIndex::Max];
	BYTE cCount;
	TPvPOccupationScoreInfo Info[PvPCommon::Common::MaxPlayer];
};

#if defined(_GAMESERVER) || (defined(_WINDOWS) && !defined(_LAUNCHER))
struct SCPVP_RESPAWN_POINT
{
	char				szName[32];
	int					iBasePreferPoint;
	int					iPoint;
	USHORT				unTeam;
	EtVector3			Position;
};
#endif

struct TPvPUserXPScore
{
	UINT	uiSessionID;
	union
	{
		UINT uiAddXPScore;
		int  iAddLadderGradePoint;
	};
	union
	{
		UINT uiResultXPScore;
		UINT uiResultLadderGradePoint;
	};
	UINT	uiGainMedalCount;
#if defined(PRE_MOD_PVP_LADDER_XP)
	UINT	uiAddLadderXPScore;	//�������ӿ��� �����Ǵ� ����ġ
#endif
};

struct SCPVP_XPSCORE
{
	char			cCount;
	TPvPUserXPScore	sXPScoreArr[PvPCommon::Common::MaxPlayer];
};

#if defined(PRE_ADD_RACING_MODE)
struct TPvPUserRapTime
{
	UINT	uiSessionID;
	DWORD	dwLapTime; //Tick
};

//SC_PVP_RACING_RAPTIME
struct SCPVP_RACING_RAPTIME
{
	char			cCount;
	TPvPUserRapTime	sRapTimeArr[PvPCommon::Common::MaxPlayer];
};

//SC_PVP_RACING_FIRST_END
struct SCPVP_RACING_FIRST_END
{
	UINT	uiSessionID;		// 1���� ���� SessionID
};
#endif // #if defined(PRE_ADD_RACING_MODE)

struct CSPVP_BAN
{
	UINT							uiSessionID;
	PvPCommon::BanType::eBanType	eType;
};

struct CSPVP_CHANGECHANNEL
{
	BYTE cType;		//PvPCommon::RoomType;
};

namespace LadderSystem
{
	struct CS_ENTER_CHANNEL
	{
		MatchType::eCode	MatchType;
	};

	struct SC_ENTER_CHANNEL
	{
		int iRet;
		MatchType::eCode	MatchType;
	};
	
	struct SC_LEAVE_CHANNEL
	{
		int iRet;
	};

	struct SC_NOTIFY_LEAVEUSER
	{
		INT64			biCharDBID;
		Reason::eCode	Reason;
	};

	struct LadderUserInfo
	{
		INT64	biCharDBID;
		WCHAR	wszCharName[NAMELENMAX];
		int		iGradePoint;
		BYTE	cJob;
	};

	struct SC_NOTIFY_JOINUSER
	{
		LadderUserInfo sUserInfo;
	};

	struct SC_NOTIFY_LEADER
	{
		WCHAR	wszLeaderName[NAMELENMAX];
	};

	struct SC_NOTIFY_ROOMSTATE
	{
		RoomState::eCode		State;
		RoomStateReason::eCode	Reason;
	};

	struct CS_LADDER_MATCHING
	{
		bool	bIsCancel;
	};

	struct SC_LADDER_MATCHING
	{
		int		iRet;
		bool	bIsCancel;
	};

	struct SC_NOTIFY_GAMEMODE_TABLEID
	{
		int		iTableID;
	};

	struct CS_PLAYING_ROOMLIST
	{
		UINT	uiReqPage;
	};

	struct SC_PLAYING_ROOMLIST
	{
		int		iRet;
		USHORT	unMaxPage;
		USHORT	unNameCount;
		BYTE	cJob[Common::RoomListPerPage*MatchType::MaxMatchType*2];
		WCHAR	wszCharName[Common::RoomListPerPage*MatchType::MaxMatchType*2][NAMELENMAX];
	};

	struct CS_OBSERVER
	{
		WCHAR wszCharName[NAMELENMAX];
	};

	struct SC_OBSERVER
	{
		int		iRet;
	};

	struct SC_SCOREINFO
	{
		TPvPLadderScoreInfo Data;
	};

	struct SC_SCOREINFO_BYJOB
	{
		int	iRet;
		TPvPLadderScoreInfoByJob Data;
	};

	struct SC_MATCHING_AVGSEC
	{
		int iSec;	// -1 �ΰ�� �˼�����
	};

	struct SC_LADDERPOINT_REFRESH
	{
		int iPoint;
	};

	struct CS_INVITE
	{
		WCHAR wszCharName[NAMELENMAX];
	};

	struct SC_INVITE
	{
		int iRet;
		WCHAR wszCharName[NAMELENMAX];
	};

	struct CS_INVITE_CONFIRM
	{
		bool bAccept;
		WCHAR wszCharName[NAMELENMAX];
	};

	struct SC_INVITE_CONFIRM_REQ
	{
		WCHAR wszCharName[NAMELENMAX];
		LadderSystem::MatchType::eCode MatchType;
		int iCurUserCount;
		int iAvgGradePoint;
	};

	struct SC_INVITE_CONFIRM
	{
		int iRet;
		WCHAR wszCharName[NAMELENMAX];
	};

	struct SC_REFRESH_USERINFO
	{
		BYTE			cCount;
		LadderUserInfo	sUserInfoArr[MatchType::MaxMatchType];
	};

	struct CS_KICKOUT
	{
		INT64 biCharacterDBID;
	};

	struct SC_KICKOUT
	{
		int iRet;
	};
};

struct SCPVP_FATIGUE_REWARD	// SC_FATIGUE_REWARD
{
	int nGainExp;
	int nGainMedal;
};

struct CSPVP_FATIGUE_OPTION // CS_FATIGUE_OPTION
{
	bool bOption; // On,Off
};

struct SCPVP_GHOUL_SCORES // SC_PVP_GHOULSCORES
{
	TPvPGhoulScores GhoulScores;
};
struct SCPVP_HOLYWATER_KILLCOUNT //SC_PVP_HOLYWATER_KILLCOUNT
{
	UINT nSessionID;      // ���� ���
	UINT nKillerSessionID;// ���� ���
};

struct SCPVP_CHANGE_CHANNEL
{
	BYTE cType;
	int nRetCode;
};

#if defined(PRE_ADD_QUICK_PVP)
// CS_PVP / CS_QUICKPVP_INVITE					// ������û ��û
struct CSQuickPvPInvite
{
	UINT nReceiverSessionID;				// �ʴ��� ����
};

// CS_PVP / CS_QUICKPVP_RESULT				// ������û ����
struct CSQuickPvPResult
{		
	bool bAccept;							// ���� ����
	UINT nSenderSessionID;					// �ʴ��� ����
};

// SC_PVP / SC_QUICKPVP_INVITE			// ������û ��û	
struct SCQuickPvPInvite
{
	UINT nSenderSessionID;				// �ʴ��� ����
};

// SC_PVP / SC_QUICKPVP_RESULT			// ������û ����
struct SCQuickPvPResult
{
	int	nResult;						// ��û ���
};
#endif //#if defined(PRE_ADD_QUICK_PVP)
#if defined(PRE_ADD_PVP_VILLAGE_ACCESS)
// SC_PVP / SC_PVP_LIST_OPEN_UI
struct SCPvPListOpenUI
{
	int nResult;						// UI���� ���
	int nLimitLevel;					// ���� ����
};
#endif

struct SCPVP_ALLKILL_SHOW_SELECTPLAYER
{
	bool bSelectPlayer;
};

struct CSPVP_ALLKILL_SELECTPLAYER
{
	UINT uiSelectPlayerSessionID;
};

struct SCPVP_ALLKILL_SELECTPLAYER
{
	UINT uiSelectPlayerSessionID;
};

struct SCPVP_ALLKILL_GROUPCAPTAIN
{
	UINT uiGroupCaptainSessionID;
};

struct SCPVP_ALLKILL_ACTIVEPLAYER
{
	UINT uiActivePlayerSessionID;
};

struct SCPVP_ALLKILL_BATTLEPLAYER
{
	UINT uiSessionIDArr[2];
};

struct SCPVP_ALLKILL_CONTINUOUSWIN
{
	UINT uiSessionID;
	UINT uiCount;
};

struct SCPVP_ALLKILL_FINISHDETAILREASON
{
	PvPCommon::FinishDetailReason::eCode Reason;
};

struct CSPvPTeamSwapMemberIndex		//CS_SWAPMEMBERINDEX
{
	BYTE cCount;
	TSwapMemberIndex Index[PARTYMAX];
};

struct CSPvPGuildWarChangeMemberGrade		//CS_GUILDWAR_CHANGEMEMBER_GRADE
{
	bool bAsign;		//true�� �Ӹ� false�� ����
	USHORT nType;		//PvPCommon::UserState
	UINT nSessionID;	//Target SessionID
};

#if defined(_GAMESERVER) || (defined(_WINDOWS) && !defined(_LAUNCHER))
struct CSOrderConcentrate					//CS_CONCENTRATE_ORDER
{
	EtVector3 vPosition;
};

struct SCConcentrateOrder				 //SC_CONCENTRATE_ORDER
{
	UINT nSessionID;
	EtVector3 vPosition;
};
#endif		//#if defined(_GAMESERVER) || (defined(_WINDOWS) && !defined(_LAUNCHER))

struct CSPvPTryAcquirePoint		//CS_PVP / CS_TRYCAPTURE
{
	int nAreaID;
};

struct CSPvPTryAcquireSKill		//CS_PVP / CS_TRYACQUIRE_SKILL
{
	int nSkillID;
	int nLevel;
	char cSlotIndex;
};

struct CSPvPUseSkill			//CS_PVP / CS_USESKILL
{
	int nSkillID;
};

struct CSPvPSwapSkillIndex		//CS_PVP / CS_SWAPSKILLINDEX
{
	char cFromIndex;
	char cToIndex;
};

struct SCPvPOccupationModeState		//SC_PVP / SC_PVP_OCCUPATION_MODESTATE
{
	int nState;		//PvPCommon::OccupationSystemState ����
};

struct SCPvPOccupationState		//SC_PVP / SC_PVP_OCCUPATIONSTATE
{
	BYTE cCount;
	PvPCommon::OccupationStateInfo Info[PARTYMAX];
};

struct SCPvPOccupationTeamState		//SC_PVP / SC_PVP_OCCUPATION_TEAMSTATE
{
	int nTeam;
	int nResource;			//�����ڿ�
	int nResourceSum;		//ȹ���ѷ�
};

struct SCPvPOccupationSkillState
{
	PvPCommon::OccupationSkillSlotInfo Info;
};

#if defined(PRE_ADD_REVENGE)
// CS_PVP / SC_PVP_SET_REVENGE_TARGET
struct SCPvPSetRevengeTarget
{
	UINT uiSessionID;
	UINT uiRevengeTargetSessionID;
	Revenge::TargetReason::eCode eReason;
};

// CS_PVP / CS_PVP_SUCCESS_REVENGE
struct SCPvPSuccessRevenge
{
	UINT uiSessionID;
	UINT uiRevengeTargetSessionID;
};
#endif

#if defined(PRE_ADD_PVP_TOURNAMENT)
// CS_PVP / CS_PVP_SWAP_TOURNAMENT_INDEX
struct CSPvPSwapTournamentIndex
{
	char cSourceIndex;
	char cDestIndex;
};

// SC_PVP / SC_PVP_SWAP_TOURNAMENT_INDEX
struct SCPvPSwapTournamentIndex
{
	int nRetCode;
	char cSourceIndex;
	char cDestIndex;
};

// SC_PVP / SC_PVP_TOURNAMENT_MATCHLIST
struct TPvPTournamentUserInfo
{	
	char	cTournamentStep;		// ��ʸ�Ʈ ����(�?) // PvPCommon::Tournament::eStep
	bool	bWin;					// �̰���� ������.
	BYTE	cJob;					// ����
	UINT	uiSessionID;
	WCHAR	wszCharName[NAMELENMAX];
};
struct SCPvPTournamentMatchList
{
	int nCount;
#if defined( PRE_PVP_GAMBLEROOM )
	BYTE cGambleType;
	int nGamblePrice;
#endif
	TPvPTournamentUserInfo sTournamentUserInfo[PvPCommon::Common::PvPTournamentUserMax];	
};

// SC_PVP / SC_PVP_TOURNAMENT_DEFAULTWIN
struct SCPvPTournamentDefaultWin
{
	char	cTournamentStep;		// ��ʸ�Ʈ ����(�?) // PvPCommon::Tournament::eStep
	UINT	uiWinSessionID;			// ���������� �̱���� SessionID
};

// SC_PVP / SC_PVP_TOURNAMENT_IDLE_TICK
struct SCPvPTournamentIdleTick
{
	UINT	uiIdleTick;
	UINT	uiCurTick;
};

// SC_PVP / SC_PVP_TOURNAMENT_TOP4
struct SCPvPTournamentTop4
{
	UINT	uiSessionID[4];			// 1~3(3�� 2��)������ ��ʸ�Ʈ ���� uiSeesionID�� ���� ������ ���� ������..
#if defined( PRE_PVP_GAMBLEROOM )
	BYTE    cGambleType;
	int     nGamblePrice[4];
#endif
};
#endif

// SC

//--------------------------------------------------------------------------------------------------
//	Trigger (CS_TRIGGER, SC_TRIGGER)
//--------------------------------------------------------------------------------------------------
struct SCTriggerCallAction
{
	short wSectorIndex;
	short wObjectIndex;
	UINT nRandomSeed;
};

struct SCChangeMyBGM
{
	int nBGM;
	int nFadeDelta;
	UINT nSessionID;
};

struct SCRadioImage
{
	int nFileID;
	UINT nSessionID;
	DWORD nTime;
};

struct SCFileTableBGMOff
{
	UINT nSessionID;
};

struct SCForceEnableRide
{
	UINT nSessionID;
	bool bForceEnableRide;
};

//MISSION
//--------------------------------------------------------------------------------------------------
//	MISSION (CS_MISSION, SC_MISSION)
//--------------------------------------------------------------------------------------------------
struct SCMissionList
{
	int nMissionScore;
	char cGain[MISSIONMAX_BITSIZE];
	char cAchieve[MISSIONMAX_BITSIZE];
	short wLastUpdate[LASTMISSIONACHIEVEMAX];
};

struct SCMissionGain
{
	int nArrayIndex;
};

struct SCMissionAchieve
{
	int nArrayIndex;
};

struct SCMissionEventPopUp
{
	int nItemID;
};

struct SCMissionHelpAlarm
{
	int nItemID;
};

struct SCDailyMissionList
{
	char cType;
	int nCount;
	TDailyMission Mission[DAILYMISSIONMAX > WEEKLYMISSIONMAX ? DAILYMISSIONMAX : WEEKLYMISSIONMAX];
};

struct SCWeeklyMissionList
{
	int nCount;
	TDailyMission Mission[WEEKLYMISSIONMAX];
};


struct SCDailyMissionCount
{
	char cType;
	int nArrayIndex;
	int nCount;
};

struct SCDailyMissionAchieve
{
	char cType;
	int nArrayIndex;
	bool bSecret;
};

struct SCDailyMissionAlarm
{
	char cType;
};

struct SCConnectingTime
{
	int nItemID;	// TimeEventTable�� ItemID
	int nDuration;	// �̺�Ʈ ���۽ð����κ��� ����� �ð�(��)
};

#if defined(PRE_ADD_ACTIVEMISSION)
struct SCActiveMissionGain	//SC_MISSION / SC_GAIN_ACTIVE_MISSION
{
	int nIndex;
};

struct SCActiveMissionAchieve	//SC_MISSION / SC_ACHIEVE_ACTIVE_MISSION
{
	int nIndex;
};
#endif

#if defined(PRE_ADD_CHAT_MISSION)
struct CSChatMission	//CS_MISSION / CS_CHAT_MISSION
{
	int nUIStringMid;
};
#endif

//APPELLATION
//--------------------------------------------------------------------------------------------------
//	Appellation (CS_APPELLATION, SC_APPELLATION)
//--------------------------------------------------------------------------------------------------
struct SCAppellationList
{
	char cGain[APPELLATIONMAX_BITSIZE];
};

struct SCAppellationGain
{
	int nArrayIndex;
};

struct SCSelectAppellation
{
	int nArrayIndex;
	int nCoverArrayIndex;
};

struct SCChangeAppellation
{
	UINT nSessionID;
	int nArrayIndex;
	int nCoverArrayIndex;
};

struct CSSelectAppellation
{
	int nArrayIndex;
	int nCoverArrayIndex;
};

struct SCPeriodAppellationTIme
{	
	BYTE cPeriodCount;
	TAppellationTime AppellationTime[PERIODAPPELLATIONMAX];
};

struct CSCollectionBook		// CS_COLLECTIONBOOK
{
	int nCollectionID;
};

//--------------------------------------------------------------------------------------------------
//	GameOption (CS_GAMEOPTION, SC_GAMEOPTION)
//--------------------------------------------------------------------------------------------------
struct CSGameOptionUpdate		//CS_GAMEOPTION_UPDATEOPTION
{
	TGameOptions Option;
};

struct CSGameOptionReqComm		// CS_GAMEOPTION / CS_GAMEOPTION_REQCOMMOPTION
{
	UINT nSessionID;
};

struct SCGameOptionCommOption		// SC_GAMEOPTION / SC_GAMEOPTION_USERCOMMOPTION
{
	UINT nSessionID;
	char cCommunityOption[13];
	TPARTYID PartyID;
};

struct SCGameOptionRefreshNotifier
{
	DNNotifier::Data data[DNNotifier::RegisterCount::Total];
};

struct CSGameOptionQuestNotifier			// CS_GAMEOPTION / CS_GAMEOPTION_QUEST_NOTIFIER
{
	DNNotifier::Data	data[DNNotifier::RegisterCount::TotalQuest];
};

struct CSGameOptionMissionNotifier			// CS_GAMEOPTION / CS_GAMEOPTION_MISSION_NOTIFIER
{
	DNNotifier::Data	data[DNNotifier::RegisterCount::TotalMission];
};

struct SCGameOptionGetProfile
{
	TProfile	sProfile;
};

struct CSGameOptionSetProfile
{
	TProfile	sProfile;
};

struct SCGameOptionDisplayProfile	// �ٸ� �������� ������ ������ �����ٶ� ����Ѵ�. (ä�ù�����, ��Ƽ���� ��...)
{
	int			nSessionID;		// ������ ����
	TProfile	sProfile;		// ������ ����
};

struct SCGameOptionSelectKeySetting
{
	bool		bIsDefault;
	TKeySetting sKeySetting;
};

struct CSGameOptionUpdateKeySetting
{
	bool		bIsDefault;
	TKeySetting	sKeySetting;
};

struct SCGameOptionSelectPadSetting
{
	bool		bIsDefault;
	TPadSetting sPadSetting;
};

struct CSGameOptionUpdatePadSetting
{
	bool		bIsDefault;
	TPadSetting	sPadSetting;
};

struct SCGameOptionNotifySecondAuthInfo
{
	bool bIsSetSecondAuthPW;	// 2�� ���� ��й�ȣ ���� ����
	bool bIsSetSecondAuthLock;	// 2�� ���� ���� Lock ����
	__time64_t	tSecondAuthResetDate;
#ifdef PRE_ADD_23829
	bool bSecondAuthPassFlag;
#endif
#ifdef PRE_ADD_IGNORESECONDAUTH_EKEYECARD
	bool bCompletlySecondAuthPassFlag;
#endif
};

struct CSGameOptionUpdateSecondAuthPassword
{
	int nOldSeed;
	int nOldValue[SecondAuth::Common::PWMaxLength];
	int nNewSeed;
	int nNewValue[SecondAuth::Common::PWMaxLength];
};

struct SCGameOptionUpdateSecondAuthPassword
{
	int		iRet;
	BYTE	cFailCount;
};

struct CSGameOptionUpdateSecondAuthLock
{
	bool	bIsLock;
	int nSeed;
	int nValue[SecondAuth::Common::PWMaxLength];
};

struct SCGameOptionUpdateSecondAuthLock
{
	int		iRet;
	BYTE	cFailCount;
};

struct CSGameOptionValidateSecondAuth
{
	int nSeed;
	int nValue[SecondAuth::Common::PWMaxLength];
	int		nAuthCheckType;
};

struct SCGameOptionValidateSecondAuth
{
	int		nRet;
	int		nAuthCheckType;
	BYTE	cFailCount;
};

struct CSGameOptionInitSecondAuth
{
	int nSeed;
	int nValue[SecondAuth::Common::PWMaxLength];
};

struct SCGameOptionInitSecondAuth
{
	int		iRet;
	BYTE	cFailCount;
};

//--------------------------------------------------------------------------------------------------
//	Radio (CS_RADIO, SC_RADIO)
//--------------------------------------------------------------------------------------------------
struct CSUseRadio					//CS_RADIO / CS_USERADIO
{
	USHORT nID;
};

struct SCUseRadio					//SC_RADIO / SC_USERADIO
{
	UINT nSessionID;
	USHORT nID;
};

//--------------------------------------------------------------------------------------------------
//	Gesture (CS_GESTURE, SC_GESTURE)
//--------------------------------------------------------------------------------------------------
struct SCGestureList
{
	BYTE cGestureCount;
	USHORT nGestureID[GESTUREMAX];
};

struct SCGestureAdd
{
	USHORT nGestureID;
};

struct CSUseGesture
{
	USHORT nGestureID;
};

struct SCUseGesture
{
	USHORT nGestureID;
};

//--------------------------------------------------------------------------------------------------
//	VoiceChat (CS_VOICECHAT, SC_VOICECHAT)
//--------------------------------------------------------------------------------------------------
struct TTalkingInfo
{
	UINT nSessionID;
	BYTE cTalking;		//none zero is talking. 0-255 volume
};

struct SCTalkingInfo		//CS_VOICECHAT / SC_TALKINGINFO
{
	BYTE cCount;
	TTalkingInfo Talking[PARTYMAX];
};

struct CSVoiceChatAvailable		//CS_VOICECHAT / CS_VOICEAVAILABLE
{
	BYTE cAvailable;
};

struct CSVoiceMute		//CS_VOICECHAT / CS_VOICEMUTE
{
	UINT nSessionID;
	BYTE cMute;			//0 mute, 1 unmute
};

struct CSVoiceComplaintReq	//CS_VOICECHAT / CS_VOICECOMPLAINTREQ
{
	UINT nSessionID;
	char szCategory[32];
	char szSubject[32];
	char szMsg[32];
};

struct SCVoiceChatInfo				// SC_VOICECHAT / SC_VOICECHATINFO
{
	char szVoiceChatIp[IPLENMAX];
	USHORT wControlPort;
	USHORT wAudioPort;
	UINT nAccountDBID;
	BOOL	bInit;
	USHORT	wManagedID;

};

struct TVoiceMemberInfo
{
	UINT nSessionID;
	BYTE cVoiceAvailable;
};

struct SCVoiceMemberInfo		// SC_VOICECHAT / SC_VOICEMEMBERINFO
{
	UINT nVoiceMutedList[PARTYCOUNTMAX];
	BYTE cCount;
	TVoiceMemberInfo Info[PARTYMAX];
};

//--------------------------------------------------------------------------------------------------
//	Restraint (SC_RESTRAINT)
//--------------------------------------------------------------------------------------------------
struct SCRestraintAdd			// SC_RESTRAINT / SC_RESTRAINTADD
{
	TRestraint Restraint;
};

struct SCRestraintDel			// SC_RESTRAINT / SC_RESTRAINTDEL
{
	UINT nRestraintKey;
};

struct SCRestraintRet			// SC_RESTRAINT / SC_RESTRAINTRETCODE
{
	short nRestraintRet;
};

struct SCRestraintList		// SC_RESTRAINT / SC_RESTRAINTLIST
{
	BYTE cCount;
	TRestraint restraint[RESTRAINTMAX];
};

//--------------------------------------------------------------------------------------------------
//	CashShop (CS_CASHSHOP, SC_CASHSHOP)
//--------------------------------------------------------------------------------------------------

struct SCCashShopOpen			// SC_CASHSHOP / SC_SHOPOPEN,			// �� ����
{
#if defined(_US)
	int nNxAPrepaid;
	int nNxACredit;
#endif	// #if defined(_US)
	UINT nCashAmount;	// ĳ�ñݾ�
	int nReserveAmount;	// ������
#ifdef PRE_ADD_NEW_MONEY_SEED
	INT64 nSeedAmount;	// �ͼ�ȭ�� �õ�
#endif // PRE_ADD_NEW_MONEY_SEED
	int nRet;
	WCHAR wszAccountName[IDLENMAX];
};

struct SCCashShopClose			// SC_CASHSHOP / SC_SHOPCLOSE,			// �� �ݱ�
{
	int nRet;
};

struct SCCashShopBalanceInquiry			// SC_CASHSHOP / SC_BALANCEINQUIRY
{
	int nRet;
	int nPetal;
	int nCash;
#ifdef PRE_ADD_NEW_MONEY_SEED
	INT64 nSeed;
#endif // PRE_ADD_NEW_MONEY_SEED
#if defined(_US)
	int nNxAPrepaid;
	int nNxACredit;
#endif	// #if defined(_US)
};

struct TCashShopInfo
{
	char cSlotIndex;	// īƮ�ε���
	int nItemSN;
	int nItemID;	// Commodity�� ItemID01-05���� �����۾��̵�
	char nOptionIndex;
};

typedef TWishItemInfo TCashShopPackageInfo;

struct TPaymentItem
{
	int nItemSN;
	int nItemID;
	char cItemOption;
};

struct TPaymentItemInfo
{	
	INT64 biDBID;				// (biPurchaseOrderDetailID)
	__time64_t tPaymentDate;	// ����(����)��
	TPaymentItem ItemInfo;	// ������ ����
};

struct TPaymentPackageItemInfo
{	
	INT64 biDBID;				// (biPurchaseOrderDetailID)
	int	 nPackageSN;			// ��Ű�� ������
	__time64_t tPaymentDate;	// ����(����)��
	TPaymentItem ItemInfoList[PACKAGEITEMMAX];		// ���������� ó���ؾ� �ϴµ�..������ ������..
};

struct TPaymentItemInfoEx		// ������ ���� ����ü
{
	UINT uiOrderNo;				// Nexon 
	int nPrice;					// ��ǰ ����(ȯ�ҿ�)
	TPaymentItemInfo PaymentItemInfo;
};

struct TPaymentPackageItemInfoEx  // ������ ���� ����ü
{
	UINT uiOrderNo;
	int nPrice;					// ��ǰ ����(ȯ�ҿ�)
	TPaymentPackageItemInfo PaymentPackageItemInfo;
};

struct CSCashShopBuy			// CS_CASHSHOP / CS_BUY,			// �����ϱ�
{
	char cType;		// ���� īƮ Ÿ�� (3,7: PREVIEWCARTLISTMAX, ������: CARTLISTMAX, 50 + eInstantCashShopBuyType: �������)	
#if defined(PRE_ADD_CASH_REFUND)
	bool bMoveCashInven;	// ĳ���κ����� �ٷ� �ű�� �÷���
#endif
	char cPaymentRules;	// Cash::PaymentRules
#if defined(PRE_ADD_SALE_COUPON)
	int nSaleCouponSN;				// Sale ���� CashCommodity SN;
	INT64 biSaleCouponSerial;		// Sale ������ ����� �����ϰ��(������ �Ѱ��� ���Ű� �Ǿ���)
#endif // #if defined(PRE_ADD_SALE_COUPON)
	char cCount;
	TCashShopInfo BuyList[PREVIEWCARTLISTMAX];
};

struct SCCashShopBuy			// SC_CASHSHOP / SC_BUY,			// �����ϱ�
{
#if defined(_US)
	int nNxAPrepaid;
	int nNxACredit;
#endif	// #if defined(_US)
	UINT nCashAmount;	// ĳ�ñݾ�
	int nReserveAmount;	// ������
#ifdef PRE_ADD_NEW_MONEY_SEED
	INT64 nSeedAmount;	// �ͼ�ȭ�� �õ�
#endif // PRE_ADD_NEW_MONEY_SEED
	int nRet;
	char cType;	// (3,7: PREVIEWCARTLISTMAX, ������: CARTLISTMAX)
#if defined(PRE_ADD_CASH_REFUND)
	TPaymentItemInfo ItemList[PREVIEWCARTLISTMAX];	//�׳� ä���� ���ϴ�.biDBID�� 0�̸� �����Ͻø� �ɵ�.
#endif
	char cCount;
	TCashShopInfo BuyList[PREVIEWCARTLISTMAX];
};

struct CSCashShopPackageBuy		// CS_CASHSHOP / CS_PACKAGEBUY
{
	int	 nPackageSN;
#if defined(PRE_ADD_CASH_REFUND)
	bool bMoveCashInven;	// ĳ���κ����� �ٷ� �ű�� �÷���
#endif
	char cPaymentRules;	// Cash::PaymentRules
#if defined(PRE_ADD_SALE_COUPON)
	int nSaleCouponSN;
	INT64 biSaleCouponSerial;
#endif
	char cCount;
	TCashShopPackageInfo BuyList[PACKAGEITEMMAX];
};

struct SCCashShopPackageBuy		// SC_CASHSHOP / SC_PACKAGEBUY
{
#if defined(_US)
	int nNxAPrepaid;
	int nNxACredit;
#endif	// #if defined(_US)
	int	nPackageSN;
	int nCashAmount;	// ĳ�ñݾ�
	int nReserveAmount;	// ������
#ifdef PRE_ADD_NEW_MONEY_SEED
	INT64 nSeedAmount;	// �ͼ�ȭ�� �õ�
#endif // PRE_ADD_NEW_MONEY_SEED
	int nRet;
#if defined(PRE_ADD_CASH_REFUND)
	TPaymentPackageItemInfo PackageItem;	// ��Ű���� ������ �ϳ�.
#endif
};

struct CSCashShopCheckReceiver	// CS_CASHSHOP / CS_CHECKRECEIVER,	// �޴��� üũ (�����Ҷ� �ʿ�)
{
	WCHAR wszToCharacterName[NAMELENMAX];	// �޴���
};

struct SCCashShopCheckReceiver	// SC_CASHSHOP / SC_CHECKRECEIVER,	// �޴��� üũ (�����Ҷ� �ʿ�)
{
	BYTE cLevel;
	BYTE cJob;
	int nRet;
};

struct CSCashShopGift			// CS_CASHSHOP / CS_GIFT,			// �����ϱ�, ������
{
	WCHAR wszToCharacterName[NAMELENMAX];	// �޴���
	WCHAR wszMessage[GIFTMESSAGEMAX];	// �޸�
#if defined( _US ) || defined( PRE_ADD_NEW_MONEY_SEED )
	char cPaymentRules;	// Cash::PaymentRules
#endif	// _US or PRE_ADD_NEW_MONEY_SEED
#if defined(PRE_ADD_CADGE_CASH)
	int nMailDBID;
#endif	// #if defined(PRE_ADD_CADGE_CASH)
	char cType;		// (3,7: PREVIEWCARTLISTMAX, ������: CARTLISTMAX)
	char cCount;
	TCashShopInfo GiftList[PREVIEWCARTLISTMAX];
};

struct SCCashShopGift			// SC_CASHSHOP / SC_GIFT,			// �����ϱ�
{
#if defined(_US)
	int nNxAPrepaid;
	int nNxACredit;
#endif	// #if defined(_US)
	int nCashAmount;	// ĳ�ñݾ�
	int nReserveAmount;	// ������
#ifdef PRE_ADD_NEW_MONEY_SEED
	INT64 nSeedAmount;	// �ͼ�ȭ�� �õ�
#endif // PRE_ADD_NEW_MONEY_SEED
	int nRet;
	char cType;		// (3,7: PREVIEWCARTLISTMAX, ������: CARTLISTMAX)
	char cCount;
	TCashShopInfo GiftList[PREVIEWCARTLISTMAX];
};

struct CSCashShopPackageGift	// CS_CASHSHOP / CS_PACKAGEGIFT
{
	WCHAR wszToCharacterName[NAMELENMAX];	// �޴���
	WCHAR wszMessage[GIFTMESSAGEMAX];	// �޸�
#if defined( _US ) || defined( PRE_ADD_NEW_MONEY_SEED )
	char cPaymentRules;	// Cash::PaymentRules
#endif	// _US or PRE_ADD_NEW_MONEY_SEED
#if defined(PRE_ADD_CADGE_CASH)
	int nMailDBID;		// ������ ������ �������� �� ���� (ĳ�������� ������ 0)
#endif	// #if defined(PRE_ADD_CADGE_CASH)
	int	 nPackageSN;
	char cCount;
	TCashShopPackageInfo PackageGiftList[PACKAGEITEMMAX];
};

struct SCCashShopPackageGift	// SC_CASHSHOP / SC_PACKAGEGIFT
{
#if defined(_US)
	int nNxAPrepaid;
	int nNxACredit;
#endif	// #if defined(_US)
	int nPackageSN;
	int nCashAmount;	// ĳ�ñݾ�
	int nReserveAmount;	// ������
#ifdef PRE_ADD_NEW_MONEY_SEED
	INT64 nSeedAmount;	// �ͼ�ȭ�� �õ�
#endif // PRE_ADD_NEW_MONEY_SEED
	int nRet;
};

typedef CSCashShopPackageGift CSCashShopCadge;	// CS_CASHSHOP / CS_CADGE

struct SCCashShopCadge			// SC_CASHSHOP / SC_CADGE,		// ������
{
	int nRet;
};

// CS_CASHSHOP / CS_GIFTLIST,		// ������ (��������) ��û

struct TGiftInfo
{
	char cPayMethodCode;				// ���� ���� (db:PayMethodCode)
	INT64 nGiftDBID;					// (db:PurchaseOrderID)
	WCHAR wszSenderName[MAILNAMELENMAX];	// �������
	int nItemSN;
	int nItemID;						// ��Ű���� ��쿣 �̰� ����(?)
	char cItemOption;
	__time64_t tOrderDate;
	WCHAR wszMessage[GIFTMESSAGEMAX];	// �޸�
	__time64_t tGiftExpireDate;			// ������ ����Ⱓ
#if defined(PRE_ADD_GIFT_RETURN)
	bool bGiftReturn;					// ���� �ݼۿ���
#endif
	bool bNewFlag;
	int nPaidCashAmount;
};

struct SCCashShopGiftList		// SC_CASHSHOP / SC_GIFTLIST,		// ������ (��������)
{
	int nRet;
	char cCount;
	TGiftInfo GiftInfo[GIFTBOXLISTMAX];
};

struct TReceiveGiftData
{
	INT64 nGiftDBID;
	char cPayMethodCode;						// ���� ���� (db:PayMethodCode)
	WCHAR wszEmoticonTitle[MAILTITLELENMAX];
	WCHAR wszReplyMessage[GIFTMESSAGEMAX];	// �޸�
};

struct CSCashShopReceiveGift	// CS_CASHSHOP / CS_RECEIVEGIFT
{
	TReceiveGiftData GiftData;
};

struct CSCashShopReceiveGiftAll	// CS_CASHSHOP / CS_RECEIVEGIFTALL
{
	char cCount;
	TReceiveGiftData GiftData[GIFTPAGEMAX];
};

struct SCCashShopReceiveGift	// SC_CASHSHOP / SC_RECEIVEGIFT
{
	int nRet;
	INT64 nGiftDBID;
};

struct SCCashShopReceiveGiftAll	// SC_CASHSHOP / SC_RECEIVEGIFTALL
{
	int nRet;
	char cCount;
	INT64 nGiftDBID[GIFTPAGEMAX];
};

#if defined(PRE_ADD_GIFT_RETURN)
struct CSCashShopGiftReturn		// CS_CASHSHOP / CS_GIFTRETURN
{
	INT64 nGiftDBID;	
};

struct SCCashShopGiftReturn	// SC_CASHSHOP / SC_GIFTRETURN
{
	INT64 nGiftDBID;
	int nRet;
};
#endif

// SC_CASHSHOP / SC_COUPON
struct CSCashShopCoupon			// CS_CASHSHOP / CS_COUPON
{
	WCHAR wszCoupon[COUPONMAX];
};

struct SCCashShopCoupon			// CS_CASHSHOP / SC_COUPON
{
	int nRet;
};

struct SCNotifyGift				// SC_CASHSHOP / SC_NOTIFYGIFT
{
	bool bNew;	// �� �������� �ƴ��� (���ڱ���)
	int nGiftCount;
};

struct CSVIPBuy				// CS_CASHSHOP / CS_VIPBUY
{
	int nItemSN;
};

struct SCVIPBuy				// CS_CASHSHOP / SC_VIPBUY,						// vip ���������ֱ�
{
	UINT nCashAmount;	// ĳ�ñݾ�
	int nReserveAmount;	// ������
#ifdef PRE_ADD_NEW_MONEY_SEED
	INT64 nSeedAmount;	// �ͼ�ȭ�� �õ�
#endif // PRE_ADD_NEW_MONEY_SEED
	int nRet;
	int nVIPPoint;		// ����Ʈ
	__time64_t tVIPExpirationDate;	// ������
	bool bAutoPay;		// �ڵ����� ����
};

struct CSVIPGift			// CS_CASHSHOP / CS_VIPGIFT
{
	WCHAR wszToCharacterName[NAMELENMAX];	// �޴���
	WCHAR wszMessage[GIFTMESSAGEMAX];	// �޸�
	int nItemSN;
};

struct SCVIPGift			// CS_CASHSHOP / SC_VIPGIFT,						// vip ���������ֱ�
{
	UINT nCashAmount;	// ĳ�ñݾ�
	int nReserveAmount;	// ������
#ifdef PRE_ADD_NEW_MONEY_SEED
	INT64 nSeedAmount;	// �ͼ�ȭ�� �õ�
#endif // PRE_ADD_NEW_MONEY_SEED
	int nRet;
};

struct SCVIPInfo			// SC_CASHSHOP / SC_VIPINFO
{
	bool bVIP;
	int nVIPPoint;		// ����Ʈ
	__time64_t tVIPExpirationDate;	// ������
	bool bAutoPay;		// �ڵ����� ����
};

struct SCSaleAbortList		// SC_CASHSHOP / SC_SALEABORTLIST
{
	BYTE cCount;
	int nAbortList[SALEABORTLISTMAX];
};

#if defined(PRE_ADD_CASH_REFUND)
struct SCPaymentList		// SC_CASHSHOP / SC_PAYMENT_LIST
{
	int nTotalPaymentCount;		// ��ü �����κ� ����(�Ϲ�)
	BYTE cInvenCount;			// ���� ���󰡴� �κ� ����
	TPaymentItemInfo ItemList[CASHINVENTORYMAX];
};

struct SCPaymentPackageList	// SC_CASHSHOP / SC_PAYMENT_PACKAGELIST
{
	int nTotalPaymentCount;		// ��ü �����κ� ����(��Ű��)
	BYTE cInvenCount;			// ���� ���󰡴� �κ� ����
	TPaymentPackageItemInfo ItemList[CASHINVENTORYMAX];		// ���⼭ ������ ����Ʈ �� �����..
};

struct CSMoveCashInven		// CS_CASHSHOP / CS_MOVE_CASHINVEN
{
	BYTE cItemType;			// 1..�Ϲ�, 2..��Ű��
	INT64 biDBID;			// (biPurchaseOrderDetailID)
};

struct SCMoveCashInven		// SC_CASHSHOP / SC_MOVE_CASHINVEN
{
	INT64 biDBID;			// (biPurchaseOrderDetailID)	
	int nRet;				// ���
	int nReserveAmount;		// ��Ż ������
#if defined( PRE_ADD_NEW_MONEY_SEED )
	INT64 nSeedAmount;		// �õ�
#endif
};

struct CSCashRefund			// CS_CASHSHOP / CS_CASH_REFUND
{
	BYTE cItemType;			// 1..�Ϲ�, 2..��Ű��
	INT64 biDBID;			// (biPurchaseOrderDetailID)
};

struct SCCashRefund			// SC_CASHSHOP / SC_CASH_REFUND
{
	INT64 biDBID;			// (biPurchaseOrderDetailID)
	int nRet;				// ���
	int nCashAmount;		// �����Ͻÿ� ���� ĳ��
};
#endif //#if defined(PRE_ADD_CASH_REFUND)

struct SCCashShopChargeTime
{
	__time64_t tTime;
};

#ifdef PRE_ADD_LIMITED_CASHITEM
struct CSQuantityLimitedItem
{
};

struct SCQuantityLimitedItem
{
	BYTE cIsLast;
	BYTE cCount;
	LimitedCashItem::TLimitedQuantityCashItem Limited[LimitedCashItem::Common::DefaultPacketCount];
};

struct SCChangedQuantityLimitedItem
{
	BYTE cCount;
	LimitedCashItem::TChangedLimitedQuantity Changed[LimitedCashItem::Common::DefaultPacketCount];
};
#endif		//#ifdef PRE_ADD_LIMITED_CASHITEM

struct CSDarkLairRankBoard
{
	int		iMapIndex;
	BYTE	cPartyUserCount;
};

struct SCDarkLairRankBoard
{
	int						iRet;
#if defined(PRE_ADD_CHALLENGE_DARKLAIR)
	int						iMapIndex;
#endif
	TDLRankHistoryPartyInfo	sBestHistory;
	TDLRankHistoryPartyInfo	sHistoryTop[DarkLair::Rank::SelectRankBoardTop];
};

struct CSPvPLadderRankBoard
{
	LadderSystem::MatchType::eCode MatchType;
};

struct SCPvPLadderRankBoard
{
	int iRet;
	TPvPLadderRanking MyRanking;
	TPvPLadderRanking Top[LadderSystem::Common::RankingTop];
};

struct CSWindowState
{
	short sWinState;
};

#if defined(PRE_ADD_PVP_RANKING)

struct TPvPRanking
{
	int		iLevel;	
	BYTE	cJobCode;	
	BYTE	cPvPLevel;
	UINT	uiExp;
	int		iPvPRank;
	float	fPvPRaito;
	INT64	biRank;
	int		iChangedRank;	
	int		iKill;
	int		iDeath;

	WCHAR	wszGuildName[GUILDNAME_MAX];
	WCHAR	wszCharName[NAMELENMAX];
};

struct TPvPRankingDetail : public TPvPRanking
{
	INT64	biClassRank;
	int		iChangedClassRank;
	INT64	biSubClassRank;	
	int		iChangedSubClassRank;
};

struct TPvPLadderRanking2
{
	int		iLevel;
	BYTE	cJobCode;
	BYTE	cPvPLevel;
	UINT	uiExp;
	int		iPvPRank;
	float	fPvPRaito;
	INT64	biRank;
	int		iChangedRank;	
	int		iWin;
	int		iLose;
	int		iDraw;	
	int		iPvPLadderGradePoint;

	WCHAR	wszGuildName[GUILDNAME_MAX];
	WCHAR	wszCharName[NAMELENMAX];
};

struct TPvPLadderRankingDetail : public TPvPLadderRanking2
{
	INT64	biClassRank;
	int		iChangedClassRank;
	INT64	biSubClassRank;	
	int		iChangedSubClassRank;
};

// struct CSPvPRankBoard	//CS_PVP_RANK_BOARD
// {
// };

struct SCPvPRankBoard	//SC_PVP_RANK_BOARD
{
	int iRet;	
	TPvPRankingDetail MyRanking;
};

struct CSPvPRankInfo	//CS_PVP_RANK_INFO
{
	WCHAR	wszCharName[NAMELENMAX];
};

struct SCPvPRankInfo	//SC_PVP_RANK_INFO
{
	int iRet;	
	TPvPRankingDetail RankingInfo;
};

struct CSPvPRankList	//CS_PVP_RANK_LIST
{
	int	iPage;
	BYTE cClassCode;
	BYTE cSubClassCode;
	WCHAR wszGuildName[GUILDNAME_MAX];
};

struct SCPvPRankList	//SC_PVP_RANK_LIST
{
	int iRet;	
	BYTE cRankingCount;
	INT64 nRankingTotalCount;
	TPvPRanking RankingInfo[RankingSystem::RANKINGMAX];
};

//Ladder Ranking
struct CSPvPLadderRankBoard2	//CS_PVP_LADDER_RANK_BOARD
{
	LadderSystem::MatchType::eCode MatchType;
};

struct SCPvPLadderRankBoard2	//SC_PVP_LADDER_RANK_BOARD
{
	int iRet;
	LadderSystem::MatchType::eCode MatchType;
	TPvPLadderRankingDetail MyRanking;
};

struct CSPvPLadderRankInfo	//CS_PVP_LADDER_RANK_INFO
{
	LadderSystem::MatchType::eCode MatchType;
	WCHAR	wszCharName[NAMELENMAX];
};

struct SCPvPLadderRankInfo	//SC_PVP_LADDER_RANK_INFO
{
	int iRet;
	LadderSystem::MatchType::eCode MatchType;
	TPvPLadderRankingDetail RankingInfo;
};

struct CSPvPLadderRankList	//CS_PVP_LADDER_RANK_LIST
{
	int	iPage;
	BYTE cClassCode;
	BYTE cSubClassCode;
	LadderSystem::MatchType::eCode MatchType;
	WCHAR wszGuildName[GUILDNAME_MAX];
};

struct SCPvPLadderRankList	//SC_PVP_LADDER_RANK_LIST
{
	int iRet;	
	BYTE cRankingCount;
	INT64 nRankingTotalCount;
	LadderSystem::MatchType::eCode MatchType;
	TPvPLadderRanking2 RankingInfo[RankingSystem::RANKINGMAX];
};

#endif

#if defined( PRE_ADD_GAMEQUIT_REWARD )
struct SCGameQuitRewardCheckRes	//SC_ETC / SC_GAMEQUIT_REWARDCHECK_RES / ����(Ŭ���̾�Ʈ)�� ���� ���� ���� ����
{
	GameQuitReward::RewardType::eType eRewardType;	// ���� ����� ���� ����
};

struct CSGameQuitRewardReq		//CS_ETC / CS_GAMEQUIT_REWARD_REQ / ���� ��û ��Ŷ
{
	GameQuitReward::RewardType::eType eRewardType;	// SCGameQuitRewardCheckRes���� ������ ���� ����(Ÿ��)
};

struct SCGameQuitRewardRes		//SC_ETC / SC_GAMEQUIT_REWARD_REQ / ���� �ޱ� ���
{
	int nRet;	// ERROR_NONE = ���� �ޱ� ���� / ������ ��� ERROR_ITEM_NOTFOUD(dnt ���̺��� ������ ã�µ� �����Ѱ��,  ���� ���� �� ���� ������ ���)
};
#endif	// #if defined( PRE_ADD_GAMEQUIT_REWARD )

//--------------------------------------------------------------------------------------------------
//	Farm
//--------------------------------------------------------------------------------------------------

struct TFarmItemPartial
{
	int		iFarmDBID;
	int		iFarmMapID;
#if defined( PRE_ADD_FARM_DOWNSCALE )
	int		iAttr;
#elif defined( PRE_ADD_VIP_FARM )
	Farm::Attr::eType	Attr;
#endif // #if defined( PRE_ADD_FARM_DOWNSCALE )
	WCHAR	wszFarmName[Farm::Max::FARMNAMELEN];		//Ȥ�ó��ؼ� �ϴ� �־����
};

struct TFarmItemFromDB:public TFarmItemPartial			// DB���� ���� ������ü����
{
	int		iFarmMaxUser;
	bool	bEnableFlag;
	bool bStartActivate;		//���½ý��ۿ���
};

struct TFarmItem:public TFarmItemFromDB					// ������ü����
{
	int		iFarmCurUser;
	bool	bActivate;				//��뿩��
};

struct TFarmAttachItem
{
	int		iItemID;
	short	iCount;
};

struct TFarmField
{
	short			nFieldIndex;
	INT64			biCharacterDBID;
	WCHAR			wszCharName[NAMELENMAX];
	BYTE			cLevel;
	BYTE			cJob;
	int				iItemID;
	int				iElapsedTimeSec;
	TFarmAttachItem	AttachItems[Farm::Max::ATTACHITEM_KIND];
};

struct TFarmFieldForCharacter
{
	short			nFieldIndex;
	int				iItemID;
	int				iElapsedTimeSec;
	__time64_t		tUpdateDate;
	TFarmAttachItem	AttachItems[Farm::Max::ATTACHITEM_KIND];
};

struct TFarmFieldPartial
{
	int				iFarmDBID;
	short			nFieldIndex;
	int				iItemID;
	int				iElapsedTimeSec;
	TFarmAttachItem AttachItems[Farm::Max::ATTACHITEM_KIND];
};

//S-C

#if defined( PRE_ADD_VIP_FARM )
struct SCFarmStart
{
	int iRet;
};

struct SCEffectItemInfo
{
	int iItemID;
	__time64_t tExpireDate;
};

#endif // #if defined( PRE_ADD_VIP_FARM )

struct SCChangeOwnerName
{
	WCHAR		wszOriginName[NAMELENMAX];
	WCHAR		wszCharacterName[NAMELENMAX];	
};

struct SCFarmInfo				//SC_FARMINFO
{
	bool		bRefreshGate;
	BYTE		cCount;
	TFarmItem	Farms[Farm::Max::FARMCOUNT];
};

struct SCFarmPlantedInfo		//SC_FARMPLANTEDINFO
{
	BYTE				cCount;
	TFarmFieldPartial	FarmFields[Farm::Max::FIELDCOUNT_PERCHARACTER];
};

struct CSFarmEnter
{
	int iFarmDBID;
	int iMapID;
};

struct SCFarmBeginAreaState
{
	int						iAreaIndex;
	Farm::AreaState::eState State;
};

struct TFarmAttachItemSerial
{
	INT64	biSerial;
	BYTE	cCount;
#if defined( PRE_ADD_TOTAL_LEVEL_SKILL )
	bool bRemoveItem;			// �����ؾ��� ���������� üũ(��ų�� ���� �������� ���� ���� �������� ���� ������ üũ
#endif
};

struct CSFarmPlant
{
	int						iAreaIndex;
	Farm::ActionType::eType	ActionType;
	BYTE					cSeedInvenIndex;
	BYTE					cCount;
	TFarmAttachItemSerial	AttachItems[Farm::Max::PLANTING_ATTACHITEM];
};

struct SCFarmPlant
{
	int						iRet;
	Farm::ActionType::eType	ActionType;
	int						iAreaIndex;
	int						iItemID;
	int						iAttachItemID;
};

struct CSFarmHarvest
{
	int						iAreaIndex;
	Farm::ActionType::eType	ActionType;
	BYTE					cNeedItemInvenIndex;	// ��Ȯ�� �ʿ��� ������ �ִ� �κ��丮 �ε���
};

struct SCFarmHarvest
{
	int						iRet;
	int						iAreaIndex;
	Farm::ActionType::eType	ActionType;
	int						iResultItemIDs[Farm::Max::HARVESTDEPOT_COUNT];
};

struct CSFarmAddWater
{
	int						iAreaIndex;
	Farm::ActionType::eType	ActionType;
	BYTE					cInvenType;
	BYTE					cWaterItemInvenIndex;	// ���� �ִ� �κ��丮 �ε���
	INT64					biWaterItemItemSerial;
};

struct SCFarmAddWater
{
	int						iRet;
	int						iAreaIndex;
	Farm::ActionType::eType	ActionType;
};

struct TFarmAreaInfo
{
	BYTE					iAreaIndex;
	int						iRandSeed;
	Farm::AreaState::eState	State;
	int						iElapsedTick;
	int						iItemID;
	WCHAR					wszCharName[NAMELENMAX];
	BYTE					cAttachCount;
	TFarmAttachItem			AttachItems[Farm::Max::ATTACHITEM_KIND];
};

struct SCAreaInfo
{
	TFarmAreaInfo	AreaInfo;
};

struct SCAreaInfoList
{
	USHORT			unCount;
	TFarmAreaInfo	AreaInfos[Farm::Max::FIELDCOUNT];
};

struct SCFarmWareHouseList
{
	int					iRet;
	BYTE				cCount;
	TFarmWareHouseItem	Items[Farm::Max::HARVESTDEPOT_COUNT];
};

struct CSFarmTakeWareHouseItem
{
	INT64	biUniqueID;
};

struct SCFarmTakeWareHouseItem
{
	int		iRet;
	INT64	biUniqueID;
};

struct SCFarmFieldCountInfo
{
	int		iCount;
};

struct SCFarmWareHouseItemCount
{
	int iCount;
};

struct SCFarmAddWaterAnotherUser
{
	WCHAR wszCharName[NAMELENMAX];
};

#ifdef PRE_ADD_CASHFISHINGITEM
struct TFishingToolInfo
{
	int nFishingRodInvenIdx;		//����� ���ô��� �κ��ε���	(���ô밡 �������� ����� ����Ʈ�� ���ô� ������� ������ Ŀ���ϱ�����)
	int nFishingBaitInvenIdx;		//����� �̳��� �κ��ε���
	INT64 biCashRodSerial;
	INT64 biCashBaitSerial;
};
#endif		//#ifdef PRE_ADD_CASHFISHINGITEM

struct CSFishingReady		//CS_READYFISHING
{
	int nFishingAreaIdx;
#ifdef PRE_ADD_CASHFISHINGITEM
	TFishingToolInfo ToolInfo;
#else		//#ifdef PRE_ADD_CASHFISHINGITEM
	int nFishingRodInvenIdx;		//����� ���ô��� �κ��ε���	(���ô밡 �������� ����� ����Ʈ�� ���ô� ������� ������ Ŀ���ϱ�����)
	int nFishingBaitInvenIdx;		//����� �̳��� �κ��ε���
#endif		//#ifdef PRE_ADD_CASHFISHINGITEM
};

struct SCFishingReady		//SC_READYFISHING
{
	int nRetCode;
};

struct CSCastBait			//CS_CASTBAIT
{
	Fishing::Cast::eCast eCastType;					//Fishing::Cast����
#ifdef PRE_ADD_CASHFISHINGITEM
	TFishingToolInfo ToolInfo;
#else		//#ifdef PRE_ADD_CASHFISHINGITEM
	int nFishingRodInvenIdx;		//����� ���ô��� �κ��ε���	(���ô밡 �������� ����� ����Ʈ�� ���ô� ������� ������ Ŀ���ϱ�����)
	int nFishingBaitInvenIdx;		//����� �̳��� �κ��ε���
#endif		//#ifdef PRE_ADD_CASHFISHINGITEM
};

struct CSPullingRod			//CS_PULLINGROD
{
	Fishing::Control::eControl ePullingControl;
};

struct SCCastBait			//SC_CASTBAITRESULT
{
	int nRetCode;
};

struct SCStopFishing		//SC_STOPFISHINGRESULT
{
	int nRetCode;
};

struct SCFishingPattern		//SC_FISHINGPATTERN
{
	int nPatternID;
#ifdef PRE_ADD_CASHFISHINGITEM
	int nFishingAutoMaxTime;
#endif		//#ifdef PRE_ADD_CASHFISHINGITEM
};

struct SCFishingReward		//SC_FISHINGREWARD
{
	UINT nSessionID;
	int nRewardItemID;
	int nRetCode;			//-1�̸� ȹ�����, 0�̸� ���� nRewardItemID�� ȹ���� ���̵� 0���� ū���̸� ����
};

struct SCFishingSync		//SC_FISHINGSYNC
{
	int nReduceTick;
	int nFishingGauge;
};

struct SCFishingPlayer		//SC_FISHINGPLAYER
{
	UINT nSessionID;
#ifdef PRE_ADD_CASHFISHINGITEM
	int nRodItemID;
#endif		//#ifdef PRE_ADD_CASHFISHINGITEM
};



struct SCStartDragonNest
{
	eDragonNestType Type;
	int nRebirthCount;
};

//--------------------------------------------------------------------------------------------------
//	ChatRoom
//--------------------------------------------------------------------------------------------------

struct TChatRoomView	// ä�ù� �ð�����
{
public:
	TChatRoomView()
	{
		Reset();
	}

	void Set(const TChatRoomView& pChatRoomView)
	{
		Set(pChatRoomView.m_nChatRoomID,
			pChatRoomView.m_wszChatRoomName,
			pChatRoomView.m_nChatRoomType,
			pChatRoomView.m_bHasPassword,
			pChatRoomView.m_wszChatRoomPRLine1,
			pChatRoomView.m_wszChatRoomPRLine2,
			pChatRoomView.m_wszChatRoomPRLine3
			);
	}

	void Set( UINT nChatRoomID, const LPCWSTR lpwszChatRoomName, BYTE nChatRoomType, bool bHasPassword,
			  const LPCWSTR lpwszChatRoomPRLine1, LPCWSTR lpwszChatRoomPRLine2, LPCWSTR lpwszChatRoomPRLine3 )
	{
		m_nChatRoomID = nChatRoomID;
		::wcsncpy_s(m_wszChatRoomName, _countof(m_wszChatRoomName), lpwszChatRoomName, _countof(m_wszChatRoomName));
		m_nChatRoomType = nChatRoomType;
		m_bHasPassword = bHasPassword;
		::wcsncpy_s(m_wszChatRoomPRLine1, _countof(m_wszChatRoomPRLine1), lpwszChatRoomPRLine1, _countof(m_wszChatRoomPRLine1));
		::wcsncpy_s(m_wszChatRoomPRLine2, _countof(m_wszChatRoomPRLine2), lpwszChatRoomPRLine2, _countof(m_wszChatRoomPRLine2));
		::wcsncpy_s(m_wszChatRoomPRLine3, _countof(m_wszChatRoomPRLine3), lpwszChatRoomPRLine3, _countof(m_wszChatRoomPRLine3));
	}

	void Reset()
	{
		m_nChatRoomID = 0;
		m_wszChatRoomName[0] = L'\0';
		m_nChatRoomType = 0;
		m_bHasPassword = false;
		m_wszChatRoomPRLine1[0] = L'\0';
		m_wszChatRoomPRLine2[0] = L'\0';
		m_wszChatRoomPRLine3[0] = L'\0';
	}

public:
	UINT m_nChatRoomID;
	WCHAR m_wszChatRoomName[CHATROOMNAME_MAX + 1];
	WCHAR m_wszChatRoomPRLine1[CHATROOMNAME_MAX + 1];
	WCHAR m_wszChatRoomPRLine2[CHATROOMNAME_MAX + 1];
	WCHAR m_wszChatRoomPRLine3[CHATROOMNAME_MAX + 1];
	BYTE m_nChatRoomType;
	bool m_bHasPassword;
};

struct CSCreateChatRoom
{
	WCHAR		wszName[CHATROOMNAME_MAX + 1];
	WCHAR		wszPassword[CHATROOMPASSWORDMAX + 1];

	BYTE		nRoomType;
	USHORT		nRoomAllow;

	WCHAR		PRLine1[CHATROOMNAME_MAX + 1];
	WCHAR		PRLine2[CHATROOMNAME_MAX + 1];
	WCHAR		PRLine3[CHATROOMNAME_MAX + 1];
};

struct CSChatRoomEnterRoom
{
	UINT		nChatRoomID;
	WCHAR		wszPassword[CHATROOMPASSWORDMAX + 1];	// �н�����
};

struct CSChatRoomChangeRoomOption
{
	TChatRoomView	ChatRoomView;

	WCHAR			wszPassword[CHATROOMPASSWORDMAX + 1];
	USHORT			nRoomAllow;
};

struct CSChatRoomKickUser
{
	int			nKickUserSessionID;		// �i�Ƴ� ĳ������ SessionID
	int			nKickReason;			// CHATROOMLEAVE_KICKED or CHATROOMLEAVE_FOREVERKICKED
};

//----------------------------------------------------------------------------------
struct SCCreateChatRoom
{
	int				nRet;

	TChatRoomView	ChatRoomView;
};

struct SCChatRoomEnterRoom
{
	int				nRet;

	int				LeaderSID;
	int				UserSessionIDs[CHATROOMMAX];	// ������ ����Ʈ
};

struct SCChatRoomChangeRoomOption
{
	int				nRet;
	TChatRoomView	ChatRoomView;
};

struct SCChatRoomEnterUser
{
	UINT		nChatRoomID;
	BYTE		cChatRoomType;
	int			nUserSessionID;
};

struct SCChatRoomKickUser
{
	int			nRet;
	int			nKickedSessionID;
};

struct SCChatRoomLeaveUser
{
	int			nUserSessionID;
	BYTE		nLeaveReason;
};

struct SCChatRoomView
{
	int				nSessionID;		// ä�ù��� ������ �ִ� ĳ����
	TChatRoomView	ChatRoomView;	// ä�ù� ���� ���� - ChatRoomID�� 0�̸� ���� �����Ǵ� ����̴�.
};

struct SCROOM_SYNC_CHATROOMINFO
{
	UINT			nLeaderID;		// ä�ù��� ������ �ִ� ĳ����
	TChatRoomView	ChatRoomView;	// ä�ù� ���� ���� - ChatRoomID�� 0�̸� ���� �����Ǵ� ����̴�.
	int				nCount;
	UINT			nMembers[CHATROOMMAX];
};

#if defined( PRE_ADD_NPC_REPUTATION_SYSTEM )

struct SCReputationList
{
	BYTE				cCount;
	TNpcReputation		ReputationArr[30];
};

struct SCModReputation
{
	BYTE				cCount;
	TNpcReputation		UpdateArr[NpcReputation::Common::MaxEffectNpcCount];
};

struct SCOpenGiveNpcPresent
{
	UINT nNpcID;
};

struct SCRequestSendSelectedPresent
{
	UINT nNpcID;
};

struct SCShowNpcEffect
{
	UINT nNpcID;
	int nEffectIndex;
};

struct CSGiveNpcPresent
{
	UINT nNpcID;
	int nPresentID;		// npc ���� ���̺� �ִ� itemid
	int nPresentCount;  // ���� ����
};

#endif // #if defined( PRE_ADD_NPC_REPUTATION_SYSTEM )

struct SCOpenDarkLairRankBoard
{
	UINT	uiNpcID;
	int		iMapIndex;
	BYTE	cPlayerCount;
};

struct SCOpenPvPLadderRankBoard
{
	UINT uiNpcID;
	LadderSystem::MatchType::eCode MatchType;
};

namespace MasterSystem
{
	struct SCSimpleInfo
	{
		TMasterSystemSimpleInfo SimpleInfo;
		bool bRefresh;	// ����Ʈ ���ſ���
	};

	struct SCCountInfo
	{
		int		iMasterCount;
		int		iPupilCount;
		int		iClassmateCount;
	};

	struct CSMasterList
	{
		UINT	uiPage;		// Zerobase
		BYTE	cJob;		// 0:���
		BYTE	cGender;	// 0:���
	};

	struct SCMasterList
	{
		int			iRet;
		BYTE		cCount;
		TMasterInfo	MasterInfoList[MasterSystem::Max::MasterPageRowCount];
	};

	struct SCMasterCharacterInfo
	{
		int						iRet;
		TMasterCharacterInfo	MasterCharacterInfo;
	};

	struct SCPupilList
	{
		int						iRet;
		INT64					biMasterCharacterDBID;
		BYTE					cCount;
		TPupilInfoWithLocation	PupilInfoList[MasterSystem::Max::PupilCount];
	};

	struct CSIntroduction
	{
		bool	bRegister;	// ��� ����
		WCHAR	wszSelfIntroduction[MasterSystem::Max::SelfIntrotuctionLen];
	};

	struct SCIntroduction
	{
		int		iRet;
		bool	bRegister;
		WCHAR	wszSelfIntroduction[MasterSystem::Max::SelfIntrotuctionLen];
	};

	struct CSMasterApplication
	{
		INT64	biCharacterDBID;
	};

	struct SCMasterApplication
	{
		int	iRet;
	};

	struct CSJoin
	{
		INT64	biMasterCharacterDBID;
		WCHAR	wszMasterCharName[NAMELENMAX];
	};

	struct SCJoin
	{
		int		iRet;
		bool	bIsDirectMenu;
		bool	bIsAddPupil;
		bool	bIsTransactor;	// ���� ��������� ���� Flag
	};

	struct SCMasterClassmate
	{
		int									iRet;
		BYTE								cCount;
		TMasterAndClassmateInfoWithLocation	DataList[MasterSystem::Max::MasterAndClassmateCount];
	};

	struct CSClassmateInfo
	{
		INT64	biClassmateCharacterDBID;
	};

	struct SCClassmateInfo
	{
		int							iRet;
		TClassmateInfoWithLocation	ClassmateInfo;
	};

	struct CSMyMasterInfo
	{
		INT64	biMasterCharacterDBID;
	};

	struct SCMyMasterInfo
	{
		int							iRet;
		TMyMasterInfoWithLocation	MasterInfo;
	};

	struct CSLeave
	{
		INT64	biDestCharacterDBID;
		bool	bIsMaster;		// �����߹�:true ���»���:false
	};

	struct SCLeave
	{
		int		iRet;
		bool	bIsDelPupil;			// true:���ڻ��� false:���»���
		int		iPenaltyRespectPoint;	// bIsDelPupil==true �ΰ�� ������ �����ġ��(���)
	};

	struct SCGraduate
	{
		WCHAR	wszCharName[NAMELENMAX];
	};

	struct SCConnect
	{
		bool	bIsConnect;
		WCHAR	wszCharName[NAMELENMAX];
	};

	struct SCRespectPoint
	{
		int iRespectPoint;
	};

	struct SCFavorPoint
	{
		INT64	biMasterCharacterDBID;
		INT64	biPupilCharacterDBID;
		int		iFavorPoint;
	};

	struct CSInvitePupil
	{
		WCHAR	wszCharName[NAMELENMAX];
	};

	struct SCInvitePupil
	{
		int		iRet;
		WCHAR	wszCharName[NAMELENMAX];
	};

	struct SCInvitePupilConfirm
	{
		int		iRet;
		WCHAR	wszMasterCharName[NAMELENMAX];
	};

	struct CSInvitePupilConfirm
	{
		bool	bIsAccept;
		WCHAR	wszMasterCharName[NAMELENMAX];
	};

	struct CSJoinDirect
	{
		WCHAR	wszMasterCharName[NAMELENMAX];
	};

	struct SCJoinDirect
	{
		int		iRet;
		WCHAR	wszMasterCharName[NAMELENMAX];
	};

	struct SCJoinDirectConfirm
	{
		int		iRet;
		WCHAR	wszPupilCharName[NAMELENMAX];
	};

	struct CSJoinDirectConfirm
	{
		bool	bIsAccept;
		WCHAR	wszPupilCharName[NAMELENMAX];
	};

	struct CSRecallMaster
	{
		WCHAR	wszMasterCharName[NAMELENMAX];
	};

	struct SCRecallMaster
	{
		int		iRet;
		bool	bIsConfirm;					// true:��ȯȮ����Ŷ
		WCHAR	wszCharName[NAMELENMAX];	// bIsConfirm==true : �����̸� bIsConfirm==false : �����̸�
	};

	struct CSBreakInto
	{
		int		iRet;
		WCHAR	wszPupilCharName[NAMELENMAX];
	};
	struct SCJoinComfirm
	{
		BYTE cLevel;				// ���� ����
		BYTE cJob;					// ���� ����
		WCHAR wszPupilCharName[NAMELENMAX];		// ���� �̸�
	};
	struct CSJoinComfirm
	{
		bool	bIsAccept;
		WCHAR	wszPupilCharName[NAMELENMAX];	// ���� �̸�
	};
}

#if defined( PRE_ADD_SECONDARY_SKILL )

namespace SecondarySkill
{
	struct SCList
	{
		BYTE				cCount;
		TSecondarySkillInfo	SkillList[SecondarySkill::Max::LearnSecondarySkill];
	};

	struct CSDelete
	{
		int iSkillID;
	};

	struct SCCreate
	{
		int	iRet;
		int iSkillID;
	};

	struct SCDelete
	{
		int iRet;
		int iSkillID;
	};

	struct SCUpdateExp
	{
		int	iRet;
		int iSkillID;
		SecondarySkill::Grade::eType Grade;
		int iExp;
		int iLevel;
	};

	struct SCRecipeList
	{
		BYTE					cCount;
		TSecondarySkillRecipe	RecipeList[SecondarySkill::Max::LearnManufactureRecipe];
	};

	struct CSAddRecipe
	{
		int		iSkillID;
		char	cInvenType;				// eItemPositionType
		BYTE	cInvenIndex;			// �Ϲ��κ��϶��� index�־��ָ� �� (ĳ���� ������������)
		INT64	biInvenSerial;		
	};

	struct SCAddRecipe
	{
		int		iRet;
		int		iSkillID;
		int		iItemID;
		INT64	biItemSerial;
		short	nExp;
	};

	struct CSDeleteRecipe
	{
		int		iSkillID;
		int		iItemID;
	};

	struct SCDeleteRecipe
	{
		int		iRet;
		int		iSkillID;
		int		iItemID;
	};

	struct CSExtractRecipe
	{
		int		iSkillID;
		int		iItemID;
	};

	struct SCExtractRecipe
	{
		int		iRet;
		int		iSkillID;
		int		iItemID;
	};

	struct SCUpdateRecipeExp
	{
		int		iRet;
		int		iSkillID;
		int		iItemID;
		int		iExp;		// ������(assign�ؾ���)
	};

	struct CSManufacture
	{
		bool	bIsStart;
		int		iSkillID;
		int		iItemID;
	};

	struct SCManufacture
	{
		int		iRet;
		bool	bIsStart;
		int		iSkillID;
		int		iItemID;
	};

	struct SCCancelManufacture
	{
		int		iRet;
		int		iSkillID;
	};
}

#endif // #if defined( PRE_ADD_SECONDARY_SKILL )

#if defined (PRE_ADD_BESTFRIEND)

/* ��ģ ���� Ȯ��
bStatus : 0 / bCancel 0
-> ������ ��ģ �ıⰡ�� ��
bStatus : 1 / bCancel 1
-> ��ģ �ı⸦ �� �����̱� ������ ����� �� ����
bStatus : 1 / tEndDate�� ����ð����� 4���� �ʰ��� ���
-> ��ģ ����
*/
struct TBestFriendInfo
{
	// Common
	__time64_t tStartDate;
	__time64_t tEndDate;
	int nDay; // ��
	bool bStatus; // 0:��ģ 1:�ı�
	INT64 biWaitingTimeForDelete; // ���� ���ð�

	// Me
	INT64 biItemSerial;
	WCHAR wszMyMemo[BESTFRIENDMEMOMAX];
	bool bCancel; // 0:��ģ�ı���� ĳ���� 1:��ģ�ı��� ĳ����

	// Friend
	UINT nSessionID;
	INT64 biCharacterDBID;
	WCHAR wszName[NAMELENMAX];
	WCHAR wszFriendMemo[BESTFRIENDMEMOMAX];
	char cLevel;
	char cJob;
	TCommunityLocation Location;
};

namespace BestFriend
{
	struct CSSearch
	{
		WCHAR wszName[NAMELENMAX];
	};

	struct CSRegist
	{
		INT64 biInvenSerial;
		WCHAR wszBestFriendName[NAMELENMAX];
	};

	struct CSAccept
	{
		bool bAccept;
		UINT nFromAccountDBID;
		INT64 biFromCharacterDBID;
		WCHAR wszFromName[NAMELENMAX];
	};

	struct CSCancel
	{
		bool bCancel;
	};

	struct CSEditMemo
	{
		WCHAR wszMemo[BESTFRIENDMEMOMAX];
	};

	struct SCGetInfo
	{
		int iRet;

		TBestFriendInfo Info;
	};

	struct SCSearch
	{
		int iRet;
		char cLevel;
		char cJob;
		WCHAR wszName[NAMELENMAX];
	};

	struct SCRegistReq
	{
		int iRet;

		// ��û��
		UINT nFromAccountDBID;	
		INT64 biFromCharacterDBID;
		WCHAR wszFromName[NAMELENMAX];

		// ������
		UINT nToAccountDBID;	
		INT64 biToCharacterDBID;
		WCHAR wszToName[NAMELENMAX];
	};

	struct SCRegistAck
	{
		int iRet;
		UINT nFromAccountDBID;	// ������
		UINT nToAccountDBID;	// ��û��
		WCHAR wszFromName[NAMELENMAX];
		WCHAR wszToName[NAMELENMAX];
	};

	struct SCComplete
	{
		int iRet;
		WCHAR wszName[NAMELENMAX];
	};

	struct SCEditMemo
	{
		int iRet;
		bool bFromMe;
		WCHAR wszMemo[BESTFRIENDMEMOMAX];
	};

	struct SCCancel
	{
		int iRet;
		bool bCancel;
		WCHAR wszName[NAMELENMAX];
	};

	struct SCClose
	{
		int iRet;
		WCHAR wszName[NAMELENMAX];
	};
}
#endif

//--------------------------------------------------------------------------------------------------
//	Etc
//--------------------------------------------------------------------------------------------------

#if defined( PRE_USA_FATIGUE )

struct SCChannelInfoFatigueInfo
{
	int iNoFatigueExpValue;
	int iFatigueExpValue;
};

#endif // #if defined( PRE_USA_FATIGUE )


struct TEvent
{
	int		nEventID;
	time_t _tBeginTime;
	time_t _tEndTime;

	union 
	{
		struct
		{
			int nEventType1;
			int nEventType2;
			int nEventType3;
		};
		int nEventType[3];
	};

	union
	{
		struct
		{
			int nAtt1;
			int nAtt2;
			int nAtt3;
			int nAtt4;
			int nAtt5;
		};
		int nAtt[5];
	};
};

struct SCIncreaseLife
{
	char cType;
	int nIncreaseLife;
};

struct SCSpecialRebirthItem
{
	int nItemID;
	int nRebirthItemCount;
};
// TIMEEVENT
//--------------------------------------------------------------------------------------------------
//	TIMEEVENT (CS_TIMEEVENT, SC_TIMEEVENT)
//--------------------------------------------------------------------------------------------------
struct SCTimeEventList
{
	int nCount;
	TTimeEvent Event[TIMEEVENTMAX];
};

struct SCTimeEventAchieve
{
	int nItemID;
	INT64 nRemainTime;
};

struct SCTimeEventExpire
{
	int nItemID;
};

struct CSAttendanceEvent
{
	bool bIsOK;
};

struct SCAttendanceEvent
{
	bool bCheckAttendanceFirst;
};

//PlayerCustomEventUI
//--------------------------------------------------------------------------------------------------
//	PlayerCustomEventUI (CS_CUSTOMEVENTUI, SC_CUSTOMEVENTUI)
//--------------------------------------------------------------------------------------------------
struct CSGetPlayerCustomEventUI
{
	UINT nSessionID;
};

struct TCustomEventInfo
{
	int nMissionID;
	BYTE cFlag;
};

struct SCGetPlayerCustomEventUI
{
	UINT nSessionID;
	BYTE cCount;
	TCustomEventInfo UIs[PlayerCustomEventUI::PlayerCustomEventUIMax];
};

//--------------------------------------------------------------------------------------------------
//	�������Խ��� (CS_GUILDRECRUIT, SC_GUILDRECRUIT)
//--------------------------------------------------------------------------------------------------
namespace GuildRecruitSystem
{
	struct CSGuildRecruitList
	{
		UINT	uiPage;	
#if defined( PRE_ADD_GUILD_EASYSYSTEM )
		BYTE	cPurposeCode;				// ��� ����(1:ģ�� 2:���� 3:�׽�Ʈ 4:�ݷμ���
		WCHAR	wszGuildName[GUILDNAME_MAX];		//��� Ȩ������
		BYTE	cSortType;  
#endif
	};
	struct SCGuildRecruitList
	{
		int			iRet;
		UINT		uiPage;
		BYTE		cCount;
		TGuildRecruitInfo	GuildRecruitList[GuildRecruitSystem::Max::GuildRecruitRowCount];
	};
	struct SCGuildRecruitCharacterList
	{
		int			iRet;
		BYTE		cCount;
		TGuildRecruitCharacter	GuildRecruitCharacterList[GuildRecruitSystem::Max::GuildRecruitCharacterRowCount];
	};	
	struct SCMyGuildRecruitList
	{
		int			iRet;
		BYTE		cCount;
		TGuildRecruitInfo	MyGuildRecruitList[GuildRecruitSystem::Max::MaxRequestCount];
	};
	struct SCGuildRecruitRequestCount
	{
		int		iRet;
		BYTE	cRequestCount;
		BYTE	cMaxRequestCount;
	};
	struct SCGuildRecruitRegisterInfo
	{		
		BYTE		cClassGrade[CLASSKINDMAX];	// ����Ŭ����
		int			nMinLevel;					// �ּ� ����
		int			nMaxLevel;					// �ִ� ����
		WCHAR		wszGuildIntroduction[GUILDRECRUITINTRODUCE+1];	//��� �Ұ���
#if defined( PRE_ADD_GUILD_EASYSYSTEM )
		BYTE		cPurposeCode;				// ��� ����(1:ģ�� 2:���� 3:�׽�Ʈ 4:�ݷμ���
		bool		bCheckHomePage;		//��� Ȩ������
#endif
	};
	struct CSGuildRecruitRegister
	{
		BYTE		cRegisterType;				// ��� ���� ����
		BYTE		cClassGrade[CLASSKINDMAX];	// ����Ŭ����
		int			nMinLevel;					// �ּ� ����
		int			nMaxLevel;					// �ִ� ����
		WCHAR		wszGuildIntroduction[GUILDRECRUITINTRODUCE+1];	//��� �Ұ���
#if defined( PRE_ADD_GUILD_EASYSYSTEM )
		BYTE		cPurposeCode;				// ��� ����(1:ģ�� 2:���� 3:�׽�Ʈ 4:�ݷμ���
		bool		bCheckHomePage;		//��� Ȩ������
#endif
	};
	struct SCGuildRecruitRegister
	{
		int			iRet;
		BYTE		cRegisterType;				// ��� ���� ����
	};
	struct CSGuildRecruitRequest
	{		
		BYTE		cRequestType;				// ���� ��û/����
		TGuildUID	GuildUID;					// ��� UID (���� ID + ��� DBID)
	};
	struct SCGuildRecruitRequest
	{		
		int			iRet;
		BYTE		cRequestType;				// ���� ��û/����
	};
	struct CSGuildRecruitAccept
	{		
		BYTE		cAcceptType;				// ���� ����/����
		TGuildUID	GuildUID;					// ��� UID (���� ID + ��� DBID)
		INT64		biAcceptCharacterDBID;		// ���Խ����� ĳ���� DBID
		WCHAR		wszToCharacterName[NAMELENMAX];		// ���Խ����� ĳ���� �̸�
	};
	struct SCGuildRecruitAccept
	{
		int			iRet;
		BYTE		cAcceptType;
		INT64		biAcceptCharacterDBID;		// ���Խ����� ĳ���� DBID		
		bool		bDelGuildRecruit;			// ��� ���� ��� ���� ����
	};
	struct SCGuildRecruitMemberResult
	{
		int			iRet;
		BYTE		cAcceptType;
		WCHAR		wszGuildName[GUILDNAME_MAX];	// ��� �̸�
	};
};

struct CSWarpVillageList
{
	INT64 nItemSerial;
};

struct SCWarpVillageList
{
	BYTE btCount;
	WarpVillage::WarpVillageInfo VillageList[128];
};

struct CSWarpVillage
{
	int nMapIndex;
	INT64 nItemSerial;
};

struct SCWarpVillage
{
	int nResult;
};

#if defined (PRE_ADD_DONATION)
struct CSDonate
{
	INT64 nCoin;
};

struct SCDonate
{
	int nResult;
};

struct CSDonationRanking
{

};

struct SCDonationRanking
{
	Donation::Ranking Ranking[Donation::MAX_RANKING];
	INT64 nMyCoin;
	int nMyRanking;
};
#endif // #if defined (PRE_ADD_DONATION)

#ifdef PRE_ADD_AUTOUNPACK

struct CSAutoUnpack
{
	bool bShow;
	INT64 serial;
};

struct SCAutoUnpack
{	
	int nRetCode;
	INT64 serial;
};

#endif

#ifdef PRE_ADD_EXCHANGE_POTENTIAL
struct CSExchangePotential		//CS_ITEMGOODS : CS_EXCHANGE_POTENTIAL
{
	int nExtractItemIndex;
	INT64 biExtractItemSerial;

	int nInjectItemIndex;
	INT64 biInjectItemSerial;
};

struct SCExchangePotential		//SC_ITEMGOODS : SC_EXCHANGE_POTENTIAL
{
	int nRetCode;
};
#endif		//#ifdef PRE_ADD_EXCHANGE_POTENTIAL

#if defined(PRE_ADD_EXCHANGE_ENCHANT)
struct CSExchangeEnchant		//CS_ITEMGOODS : CS_EXCHANGE_ENCHANT
{
	int nExtractItemIndex;
	INT64 biExtractItemSerial;

	int nInjectItemIndex;
	INT64 biInjectItemSerial;
};

struct SCExchangeEnchant			//SC_ITEMGOODS : SC_EXCHANGE_ENCHANT
{
	int nRetCode;
};
#endif

#if defined(PRE_ADD_PCBANG_RENTAL_ITEM)
struct SCPcCafeRentItem				// SC_PCCAFE_RENT_ITEM
{
	int nRetCode;				// ���
	int nNeedCount;				// �ʿ��� �κ���
};
#endif // #if defined(PRE_ADD_PCBANG_RENTAL_ITEM)

#ifdef PRE_ADD_DOORS
struct SCDoorsGetAuthKey
{
	int nRetCode;
	bool bMobileAuthentication;
	WCHAR wszAuthKey[Doors::Common::AuthenticationKey_LenMax];
};

struct SCDoorsCancelAuth
{
	int nRetCode;
};

struct SCDoorsGetAuthFlag
{
	bool bFlag;
};
#endif		//#ifdef PRE_ADD_DOORS

#if defined( PRE_PRIVATECHAT_CHANNEL )
namespace PrivateChatChannel
{
	struct SCPrivateChannleInfo
	{
		int nCount;
		WCHAR wszChannelName[PrivateChatChannel::Common::MaxNameLen];
		PrivateChatChannel::TMemberInfo Members[PrivateChatChannel::Common::MaxMemberCount];
	};

	struct SCPrivateChatChannleResult	// CS ��Ŷ�� ���� ��� ������� ���⼭ ó��
	{
		int nRet;		
	};

	struct CSPrivateChatChannleAdd	// CS_PRIVATECHAT_CHANNEL_ADD
	{
		WCHAR wszChannelName[PrivateChatChannel::Common::MaxNameLen];		
#if defined( PRE_ADD_PRIVATECHAT_CHANNEL )
		int nPassWord;
#endif
	};	

	struct CSPrivateChatChannleJoin	// CS_PRIVATECHAT_CHANNEL_JOIN
	{
		WCHAR wszChannelName[PrivateChatChannel::Common::MaxNameLen];
		int nPassWord;
	};

	struct CSPrivateChatChannleInvite	// CS_PRIVATECHAT_CHANNEL_INVITE
	{
		WCHAR wszCharacterName[NAMELENMAX];
	};
	
	struct SCPrivateChatChannleInviteResult	// SC_PRIVATECHAT_CHANNEL_INVITERESULT
	{
		int nRet;
	};

	struct SCPrivateChatChannleJoinResult	// SC_PRIVATECHAT_CHANNEL_JOINRESULT
	{
		int nRet;
		WCHAR wszChannelName[PrivateChatChannel::Common::MaxNameLen];		
	};

	struct CSPrivateChatChannleOut	// CS_PRIVATECHAT_CHANNEL_OUT 
	{	
	};	

	struct SCPrivateChatChannleOutResult	// SC_PRIVATECHAT_CHANNEL_OUTRESULT 
	{	
		int nRet;
	};

	struct CSPrivateChatChannleKick	// CS_PRIVATECHAT_CHANNEL_KICK
	{			
		WCHAR wszCharacterName[NAMELENMAX];
	};
	
	struct CSPrivateChatChannleKickResult	// SC_PRIVATECHAT_CHANNEL_KICKRESULT
	{			
		int nRet;
		bool bMe;
	};

	struct SCPrivateChatChannleAdd // SC_PRIVATECHAT_CHANNEL_ADD
	{
		int nRet;
		WCHAR wszChannelName[PrivateChatChannel::Common::MaxNameLen];
		PrivateChatChannel::TMemberInfo Member;
	};	

	struct SCPrivateChatChannleDel // SC_PRIVATECHAT_CHANNEL_DEL
	{
		int nRet;			
		INT64 biCharacterDBID;
		WCHAR wszCharacterName[NAMELENMAX];
	};	

	struct SCPrivateChatChannleKick	// SC_PRIVATECHAT_CHANNEL_KICK
	{
		int nRet;		
		INT64 biCharacterDBID;
	};

	struct CSPrivateChatChannleMod	// CS_PRIVATECHAT_CHANNEL_MOD
	{
		int nModType;
		int nPassWord;
		INT64 biCharacterDBID;
	};

	struct SCPrivateChatChannleMod	// SC_PRIVATECHAT_CHANNEL_MOD
	{
		int nRet;
		int nModType;
		INT64 biCharacterDBID;
		WCHAR wszCharacterName[NAMELENMAX];
	};
}
#endif

#if defined( PRE_WORLDCOMBINE_PVP )

namespace WorldPvPMissionRoom
{
	struct SCWorldPvPRoomStartMsg	// SC_PVP_WORLDPVPROOM_STARTMSG
	{
		bool bShow;
		UINT uiPvPIndex;
	};

	struct CSWorldPvPRoomStartMsg	// CS_PVP_WORLDPVPROOM_STARTMSG
	{		
		UINT uiPvPIndex;
	};

	struct SCWorldPvPRoomStartReturn	// SC_PVP_WORLDPVPROOM_STARTMSG
	{		
		int nRet;	
	};

	struct SCWorldPvPRoomJoinResult	// SC_PVP_WORLDPVPROOM_STARTMSG
	{		
		int nRet;	
	};

	struct SCWorldPvPRoomAllKillTeamInfo	// SC_PVP_WORLDPVPROOM_STARTMSG
	{		
		UINT nSessionID[PARTYMAX];
		int nTeam[PARTYMAX];
	};

	struct SCWorldPvPRoomTournamentUserInfo
	{		
		BYTE					cUserCount;				// ������
		PvPCommon::UserInfoList	UserInfoList;
	};
}

#endif

#if defined( PRE_ADD_PVP_COMBOEXERCISE )
struct SCPvPComboExerciseRoomMasterInfo // SC_PVP_COMBOEXERCISE_ROOMMASTERINFO
{
	UINT uiRoomMasterSessionID;
};

struct SCPvPComboExerciseChangeRoomMaster // SC_PVP_COMBOEXERCISE_CHANGEROOMMASTER
{
	UINT uiRoomMasterSessionID;
};

struct SCPvPComboExerciseRecallMonsterResult // SC_PVP_COMBOEXERCISE_RECALLMONSTER
{
	int nRet;
};
#endif // #if defined( PRE_ADD_PVP_COMBOEXERCISE )

#if defined( PRE_ALTEIAWORLD_EXPLORE )

namespace AlteiaWorld
{
	struct CSAlteiaWorldInfo // CS_ALTEIAWORLD_INFO
	{
		AlteiaWorld::Info::eInfoType eType;
	};

	struct SCAlteiaWorldJoinInfo // SC_ALTEIAWORLD_JOIN_INFO
	{	
		BYTE cWeeklyClearCount;
		BYTE cDailyClearCount;
		BYTE cTicketCount;		
	};

	struct SCAlteiaWorldPrivateGoldKeyRankingInfo // SC_ALTEIAWORLD_PRIVATEGOLDKEYRANKING_INFO
	{
		int nMyBestGoldKeyCount;
		int nCount;
		AlteiaWorld::GoldKeyRankMemberInfo MemberInfo[AlteiaWorld::Common::MaxRankCount];
	};

	struct SCAlteiaWorldPrivatePlayTimeRankingInfo // SC_ALTEIAWORLD_PRIVATEPLAYTIMERANKING_INFO
	{
		UINT nPlaySec;		
		int nCount;
		AlteiaWorld::PlayTimeRankMemberInfo MemberInfo[AlteiaWorld::Common::MaxRankCount];
	};

	struct SCAlteiaWorldGuildGoldKeyRankingInfo // SC_ALTEIAWORLD_GUILDGOLDKEYRANKING_INFO
	{
		int nMyBestGoldKeyCount;	
		int nCount;
		AlteiaWorld::GuildGoldKeyRankInfo GuildRankInfo[AlteiaWorld::Common::MaxRankCount];
	};

	struct SCAlteiaWorldSendTicketInfo // SC_ALTEIAWORLD_GUILDGOLDKEYRANKING_INFO
	{
		BYTE cSendTicketCount;
		INT64 biCharacterDBID[AlteiaWorld::Common::MaxSendCount];
	};

	struct CSAlteiaWorldJoin // CS_ALTEIAWORLD_JOIN
	{	
	};

	struct SCAlteiaWorldJoinResult
	{
		int nRet;
	};	

	struct CSAlteiaWorldSendTicket // CS_ALTEIAWORLD_SENDTICKET
	{	
		WCHAR wszCharacterName[NAMELENMAX];
	};

	struct SCAlteiaWorldSendTicket // SC_ALTEIAWORLD_SENDTICKET
	{	
		WCHAR wszCharacterName[NAMELENMAX];
		int nTicketCount;
	};

	struct SCAlteiaWorldSendTicketResult // SC_ALTEIAWORLD_SENDTICKET_RESULT
	{	
		int nRet;
		int nSendTicketCount;
	};

	struct CSAlteiaWorldDice // CS_ALTEIAWORLD_DICE
	{
	};

	struct SCAlteiaWorldDiceResult // SC_ALTEIAWORLD_DICE_RESULT
	{
		int nRet;
		int nNumber;
	};
}

#endif // #if defined( PRE_ALTEIAWORLD_EXPLORE )

#if defined( PRE_ADD_STAMPSYSTEM )
namespace StampSystem
{
	struct SCStampSystemInit
	{
		int nWeekDay;
		INT64 biTableStartTime;
		bool bCompleteFlagData[Common::MaxStampSlotCount];
	};

	struct SCStampSystemAddComplete
	{
		int nChallengeIndex;
		int nWeekDay;
	};

	struct SCStampSystemChangeWeekDay
	{
		int nWeekDay;
	};

	struct SCStampSystemChangeTable
	{
		INT64 biTableStartTime;
	};
}
#endif // #if defined( PRE_ADD_STAMPSYSTEM )

#if defined( PRE_ADD_PVP_EXPUP_ITEM )
struct SCUsePvPExpup
{
	int nRet;
	BYTE cLevel;
	UINT uiXP;
	int nIncrement; 
};
#endif // #if defined( PRE_ADD_PVP_EXPUP_ITEM )

#if defined( PRE_DRAGONBUFF )
struct SCWorldBuffMsg
{
	WCHAR wszCharacterName[NAMELENMAX];
	int nItemID;
};
#endif

#if defined(PRE_ADD_GUILD_CONTRIBUTION)
namespace GuildContribution
{
	// SC_GUILD / SC_GUILD_CONTRIBUTION_POINT
	struct SCGuildContributionPoint
	{
		int nWeeklyContributionPoint;	//�ְ� ��� ���嵵
		int nTotalContributionPoint;	//��� ���嵵 ����
	};

	// SC_GUILD / SC_GUILD_CONTRIBUTION_RANK
	struct SCGuildContributionRank
	{
		int nCount;	// ���嵵 ��Ŀ ��
		GuildContribution::TGuildContributionRankingData Data[GUILDSIZE_MAX]; // ���嵵 ��Ŀ ����
	};
}
#endif	// #if defined(PRE_ADD_GUILD_CONTRIBUTION)

//--------------------------------------------------------------------------------------------------
//	DWC (CS_DWC, SC_DWC)
//--------------------------------------------------------------------------------------------------
#if defined(PRE_ADD_DWC)
struct TDWCScore
{
	int nTotalWin;
	int nTotalLose;
	int nTotalDraw;
	int nDWCPoint;
	int nHiddenDWCPoint;
	short wTodayWin;
	short wTodayLose;
	short wTodayDraw;
	short wConsecutiveWin;
	short wConsecutiveLose;
	int nWeeklyPlayCount;
};

struct TDWCTeam
{	
	int nDWCRank;
	WCHAR wszTeamName[GUILDNAME_MAX];
	int nTotalWin;
	int nTotalLose;
	int nTotalDraw;
	int nDWCPoint;
	int nHiddenDWCPoint;
	short wTodayWin;
	short wTodayLose;
	short wTodayDraw;
	short wConsecutiveWin;
	short wConsecutiveLose;
	int nWeeklyPlayCount;
	__time64_t tLastPlayDate;	//�ʿ������ ����
};

struct TDWCTeamMember
{
	TDWCTeamMember::TDWCTeamMember(){;}
	TDWCTeamMember::TDWCTeamMember( UINT nAccountDBID, INT64 biCharacterDBID, LPCWSTR lpwszMemberCharacterName, BYTE cJobCode, bool bTeamLeader )
		: nAccountDBID(nAccountDBID), biCharacterDBID(biCharacterDBID), cJobCode(cJobCode), bTeamLeader(bTeamLeader)
	{
		Location.Reset();
		memset(wszCharacterName, 0, sizeof(wszCharacterName));
		::wcsncpy_s(wszCharacterName, _countof(wszCharacterName), lpwszMemberCharacterName, _countof(wszCharacterName));
	}
	UINT nAccountDBID;
	INT64 biCharacterDBID;
	WCHAR wszCharacterName[NAMELENMAX];
	BYTE cJobCode;
	bool bTeamLeader;
	TCommunityLocation Location;
};

struct CSCreateDWCTeam	// CS_DWC / CS_CREATE_DWCTEAM
{
	WCHAR wszTeamName[GUILDNAME_MAX];
};

struct SCCreateDWCTeam	//SC_DWC / SC_CREATE_DWCTEAM
{
	int nRet;
	UINT nSessionID;
	UINT nTeamID;
	WCHAR wszTeamName[GUILDNAME_MAX];
};

struct SCDismissDWCTeam	// SC_DWC / SC_DISMISS_DWCTEAM
{	
	int nRet;
	UINT nTeamID; 
};

struct SCLeaveDWCTeamMember // SC_DWC / SC_LEAVE_DWCTEAM_MEMB
{
	int nRet;
	UINT nTeamID;
	INT64 biLeaveUserCharacterDBID;
	WCHAR wszCharacterName[NAMELENMAX];
};

struct CSInviteDWCTeamMemberReq	// CS_DWC / CS_INVITE_DWCTEAM_MEMBREQ			// ���� �ʴ� ��û
{
	WCHAR wszToCharacterName[NAMELENMAX];		// �ʴ��� ĳ���� �̸�
};

struct SCInviteDWCTeamMemberReq	// SC_DWC / SC_INVITE_DWCTEAM_MEMBREQ			// ���� �ʴ� ��û
{
	UINT nTeamID;
	UINT nFromAccountDBID;	// �߽��� ���� DBID
	UINT nToAccountDBID;	// ������ ���� DBID
	WCHAR wszFromCharacterName[NAMELENMAX];	// �߽��� ĳ���� �̸�
	WCHAR wszTeamName[GUILDNAME_MAX];		// ����
};

struct CSInviteDWCTeamMemberAck	// CS_DWC / CS_INVITE_DWCTEAM_MEMBACK
{
	UINT nTeamID;				// ��� UID
	UINT nFromAccountDBID;		// �߽��� ���� DBID
	bool bAccept;				// ���� ����
};

struct SCInviteDWCTeamMemberAck	// SC_DWC / SC_INVITE_DWCTEAM_MEMBACK
{
	UINT nToAccountDBID;			// ������ ���� DBID
	INT64 biToCharacterDBID;			// ������ ĳ���� DBID
	UINT nFromAccountDBID;			// �߽��� ���� DBID
	INT64 biFromCharacterDBID;		// �߽��� ĳ���� DBID
	UINT nTeamID;					// ��ID
	TP_JOB nJob;					// ������ ����
	TCommunityLocation Location;	// ���� ��ġ (�α��� ���ε� �� ����� ���·� üũ)
	int nRet;						// ��� (NOERROR : ���� / �׿� : ����)
	WCHAR wszToCharacterName[NAMELENMAX];	// ������ ĳ���� �̸�
};

struct SCInviteDWCTeamMemberAckResult	// SC_DWC / SC_INVITE_DWCTEAM_MEMBACK_RESULT
{	
	int nRet;						// ��� (NOERROR : ���� / �׿� : ����)
};

struct CSGetDWCTeamInfo	// CS_DWC / SC_GET_DWCTEAM_INFO
{
	bool bNeedMembList;	// ���� ��ϵ� �ʿ����� ����
};

struct SCGetDWCTeamInfo	// SC_DWC / SC_GET_DWCTEAM_INFO
{
	int nRet;
	UINT nTeamID;
	TDWCTeam Info;
};

struct SCGetDWCTeamMember // SC_DWC / SC_GET_DWCTEAM_MEMBER
{
	int nRet;
	UINT nTeamID;
	int nCount;
	TDWCTeamMember MemberList[DWC::DWC_MAX_MEMBERISZE];
};

struct TRankMemberData
{
	BYTE cJobCode;
	WCHAR wszMemberName[NAMELENMAX];
};

struct TDWCRankData
{
	int nRank;
	int nTeamID;
	WCHAR wszTeamName[GUILDNAME_MAX];
	int nWinCount;
	int nLoseCount;
	int nDrawCount;
	int nDWCPoint;
	int nChangedRanking;
	TRankMemberData LeaderData;
	TRankMemberData MemberData[DWC::DWC_MAX_MEMBERISZE];
};

struct CSGetDWCRankPage		//CS_DWC / CS_GET_DWC_RANKPAGE
{
	int nPageNum;
	int nPageSize;
};

struct SCGetDWCRankPage
{
	int nRetCode;
	UINT nTotalRankSize;
	int nPageNum;
	int nPageSize;
	TDWCRankData RankDataList[RankingSystem::RANKINGMAX];
};

struct CSGetDWCFindRank		//CS_DWC / CS_GET_DWC_FINDRANK
{
	BYTE cType;				//0 ĳ���͸� 1 ����
	WCHAR wszFindKey[NAMELENMAX + GUILDNAME_MAX];
};

struct SCGetDWCFindRank		//SC_DWC / SC_GET_DWC_FINDRANK
{
	int nRetCode;
	TDWCRankData Rank;
};

struct SCChangeDWCTeamMemberState	//SC_DWC / SC_CHANGE_DWCTEAM_MEMBERSTATE
{
	UINT nTeamID;
	INT64 biCharacterDBID;
	TCommunityLocation Location;
};

struct SCDWCTeamNameInfo	//SC_DWC / SC_DWC_TEAMNAME_INFO
{
	WCHAR wszATeamName[GUILDNAME_MAX];
	WCHAR wszBTeamName[GUILDNAME_MAX];
};

struct SCDWCChannelInfo				// SC_DWC / SC_DWCCHANNELINFO
{
	TDWCChannelInfo ChannelInfo;
};

struct SCChangeDWCTeam // SC_DWC / SC_CHANGE_DWCTEAM
{
	UINT nSessionID;
	WCHAR wszTeamName[GUILDNAME_MAX];
};

#endif	//#if defined(PRE_ADD_DWC)

#if defined( PRE_ADD_NEW_MONEY_SEED )
struct SCSendSeedPoint
{
	INT64 nSeedPoint;
	bool bInc;
};
#endif

#pragma pack(pop)
