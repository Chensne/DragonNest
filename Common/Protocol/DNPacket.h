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
//                       패킷구조체 (헤더 + 데이터)
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
	unsigned char header;			//tcp packet과 inteface 공유를 위하여 헤더를 늘립니다. main_cmd와 같음
	unsigned char sub_header;	//sub_cmd와 같음
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

//External PacketMessage Process (udp = Internal, tcp = external)(고냥 의미만~)

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
//				여러곳에서 쓰는(?) 기타 구조체
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
	BYTE cMemberIndex;		//0번부터 시작
	UINT uiPvPUserState;
#if defined( PRE_WORLDCOMBINE_PARTY )
	int		nWorldSetID;				//월드아이디
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

//GameOptions 20바이트
struct TGameOptions
{
	//비트쉬프트 없이 바로 사용하기 위하여....db에 연속된메모리 형태로 저장이 되므로 변경시 참고해주세요. 꼬이면 골치 아파요.
	union
	{
		struct
		{
			//순서 바꾸지 말아주세요~ 추가는 하단으로
			bool bUserName : 1;		//다른유저이름표시
			bool bPartyUserName : 1;	//파티원이름표시
			bool bMonsterName : 1;		//몬스터이름표시
			bool bStateBarInsideCount : 1;	//상태바숫자표시
			bool bMyHPBar : 1;			//나의hp바표시
			bool bPartyHPBar : 1;		//파티 hpbar표시
			bool bMonsterHPBar : 1;		//몬스터바표시
			bool bMyCombatInfo : 1;		//전투정보표시
			bool bPartyCombatInfo : 1;	//파티원전투정보표시
			bool bDamageDirMelee : 1;	 // 근접 피격방향 표시 
			bool bDamageDirRange : 1;	 // 원거리 피격방향 표시 
			bool bGuildName : 1;		// 길드명 표시
			bool bGameTip : 1;			// 하단 슬라이드 게임팁 표시
			bool bSimpleGuide : 1;		// 간략 가이드
			bool bPVPLevelMark : 1;		// 콜로세움 계급 표시
			bool bSkillCoolTimeCounter : 1; // 스킬 재사용 시간 표시

			//추가시 DISPLAYOPTIONCOUNT 숫자를 추가 갯수만큼 올려주세요.
		};
		char cDisplayOption[5];		//1bit*40  = 5Byte
	};

	union
	{
		struct
		{
			//순서 바꾸지 말아주세요~ 추가는 하단으로
			bool bDisplayGuildName : 1;		//내 길드명을 남이 볼 수 있는지
			bool bDisplayGender : 1;		//내 성별을 남이 볼 수 있는지
			bool bDisplayLevel : 1;			//내 레벨을 남이 볼 수 있는지

			//추가시 COMMDISPLAYOPTIONCOUNT 숫자를 추가 갯수만큼 올려주세요.
		};
		char cCommunityDisplayOption[2];		//1bit*16  = 2byte
	};

	union
	{
		struct
		{
			//순서 바꾸지 말아주세요~ 추가는 하단으로
			//use eCommunityAcceptableOptionType
			BYTE cPartyInviteAcceptable : 2;
			BYTE cGuildInviteAcceptable : 2;
			BYTE cTradeRequestAcceptable : 2;
			BYTE cDuelRequestAcceptable : 2;
			BYTE cObserveStuffAcceptable : 2;
			BYTE cLadderInviteAcceptable : 2;
			BYTE cQuickPvPInviteAcceptable : 2;
			BYTE cPartyChatAcceptable : 2; // PRE_ADD_CHAT_RENEWAL

			//추가시 COMMOPTIONCOUNT 숫자를 추가 갯수만큼 올려주세요.
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
	INT64 nSerial;		// 시리얼
};

struct TGuildWareHistory
{
	int			nIndex;				// 인덱스
	WCHAR		wszCharacterName[NAMELENMAX];		// 캐릭터명
	BYTE		cInOut;								// 입/출
	short		wCount;								// 수량 0:Coin / +1:ItemCode
	int			nContent;							// 내역 cCount(수량)에 따라 용도가 달라진다
	__time64_t	EventDate;							// 이벤트날짜
};


struct TEffectItemInfo
{
	int			iItemID;
	bool		bEternityFlag;
	__time64_t	ExpireDate;
	int			iProperty[5];
	INT64		biItemSerial;
};

struct TQuestItemCnt			// 인벤토리의 아이템 목록 추가 가능여부를 확인하기 위해 임시로 생성할 목적으로 선언
{
public:
	int nItemID;
	short wCount;		// 개수

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

struct TInvenItemCnt			// 인벤토리의 아이템 목록 추가 가능여부를 확인하기 위해 임시로 생성할 목적으로 선언
{
public:
	int nItemID;
	short wCount;		// 개수
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
	int nChannelID;			// 게임일경우 0 또는 -1
	int nMapIdx;			// 모를 경우 -1
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

struct TGuildUID	// 길드 UID
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
	bool operator<(const TGuildUID& pRhv) const		{ return(ui64UID < pRhv.ui64UID); }		// for stdext::hash_map등.

public:
	union {
		UINT64 ui64UID;		// 길드 UID
		struct {
			int nWorldID;			// 월드 ID
			UINT nDBID;	// 길드 DBID
		};
	};
};

struct TGuildView	// 길드 시각정보
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
	TGuildUID GuildUID;						// 길드 UID (월드 ID + 길드 DBID)
	WCHAR wszGuildName[GUILDNAME_MAX];		// 길드 이름 (일단 길드 창설 이후에는 이름은 변경되지 않는것을 전제로 함)
	short wWarSchduleID;
	BYTE cTeamColorCode;					// 0:없음 / 1:청팀 / 2:홍팀
	short wGuildMark;						// 길드마크 문양
	short wGuildMarkBG;						// 길드마크 배경
	short wGuildMarkBorder;					// 길드마크 테두리
};

struct TGuildSelfView : public TGuildView	// 길드 시각정보 (개인)		// P.S.> 절대 다중상속 하면 않됨
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
		// 상등 비교 시 부모 클래스 (길드 자체) 멤버만 인정 ?
		return(TGuildView::operator==(pRhv) && pRhv.btGuildRole == btGuildRole);
	}
	bool operator!=(const TGuildSelfView& pRhv) const
	{
		return(!(this->operator==(pRhv)));
	}
public:
	BYTE btGuildRole;	// 길드 직책
};

struct TGuildRewardItem
{
	int			nItemID;		// 아이템아이디	
	int			nItemType;		// 타입
	short		    nEffectValue;	// 효과값
#if defined( PRE_ADD_GUILDREWARDITEM )
	int		    nEffectValue2;	// 효과값2
#endif
	bool		bEternity;		// 영구 여부
	__time64_t	m_tExpireDate;	// 기간
};

struct TGuild		// 길드 정보
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
	TGuildView GuildView;							// 길드 시각정보
	INT64 ui64GuildMoney;							// 길드 자금
	short wGuildLevel;								// 길드 레벨
	WCHAR wszGuildNotice[GUILDNOTICE_MAX+1];		// 길드 공지
	int iGuildMedal[GUILDMEDAL_TYPE_CNT];			// 길드 메달
	short wGuildSize;								// 길드 추가/전체 수용인원
	short wGuildWareSize;							// 길드 창고 최대수
	__time64_t OpenDate;							// 길드 창설일
	WCHAR wszGuildRoleName[GUILDROLE_TYPE_CNT][GUILDROLENAME_MAX+1];// 길드 역할별 이름
	int wGuildRoleAuth[GUILDROLE_TYPE_CNT];			// 길드 역할별 권한
	INT nRoleMaxItem[GUILDROLE_TYPE_CNT];			// 길드 역할별 창고 아이템 횟수제한 (인덱스)
	INT nRoleMaxCoin[GUILDROLE_TYPE_CNT];			// 길드 역할별 최대 출금액 (인덱스)
	short nGuildWarFinalCount;						// 길드전 본선 진출 횟수
	short nGuildWarFinalWinCount;					// 길드전 본선 우승 횟수
	int iTotalGuildExp;								// 길드 경험치
	int iDailyStagePoint;							// 스테이지 포인트
	int iDailyMissionPoint;							// 미션 포인트
	int iWarPoint;									// 길드전 포인트
	TGuildRewardItem GuildRewardItem[GUILDREWARDEFFECT_TYPE_CNT];	// 길드보상 적용 효과
	WCHAR wszGuildHomePage[GUILDHISTORYTEXT_MAX+1];		// 길드 홈페이지
	char cGuildType;							// 참조 BeginnerGuild
};

struct TGuildMember		// 길드원 정보
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
	// 길드원 DB 정보
	INT64 nCharacterDBID;					// 캐릭터 DBID
	BYTE btGuildRole;				// 길드 역할
	long iUseGuildCommonPoint;	// 길드 일반 포인트 (사용)
	long iTotGuildCommonPoint;	// 길드 일반 포인트 (기여)
	WCHAR wszGuildMemberIntroduce[GUILDMEMBINTRODUCE_MAX+1];	// 길드원 자기소개
	__time64_t JoinDate;				// 길드 가입 날짜

	// 캐릭터 DB 정보
	UINT nAccountDBID;					// 계정 DBID
	WCHAR wszCharacterName[NAMELENMAX];	// 캐릭터 이름
	TP_JOB nJob;							// 전직직업
	CHAR cLevel;							// 레벨
	__time64_t LastLogoutDate;			// 최종 로그아웃 날짜

	// 캐릭터 실시간 정보
	TCommunityLocation Location;			// 현재 위치 (로그인 여부도 본 멤버의 상태로 체크)

	int TakeWareItemCount;				// 금일 아이템을 꺼내기 횟수
	INT64 WithdrawCoin;					// 금일 인출한 금액

	__time64_t WareVeriosnDate;			// 창고 접근 버젼
	bool bOpenWareStatus;					// 창고 오픈 상태

	__time64_t LastWareUseDate;			// 창고에 마지막으로 사용한 날짜

	// 서버 관리
	BYTE nDelMembNo;			// 캐릭터 삭제 관리번호
};

struct TGuildHistory	// 길드 히스토리 정보 (인스턴스)
{
	INT64 biIndex;		// 길드 히스토리 인덱스
	__time64_t RegDate;				// 길드 히스토리 등록일
	BYTE btHistoryType;	// 길드 히스토리 타입 (eGuildHistoryType)
	int nInt1;
	int nInt2;
	INT64 biInt64;
	WCHAR Text[GUILDHISTORYTEXT_MAX+1];
};

// 길드모집게시판 등록 길드정보
struct TGuildRecruitInfo
{
	TGuildUID GuildUID;										// 길드 UID (월드 ID + 길드 DBID)
	WCHAR wszGuildName[GUILDNAME_MAX];					// 길드 이름
	short wGuildMark;										// 길드마크 문양
	short wGuildMarkBG;										// 길드마크 배경
	short wGuildMarkBorder;									// 길드마크 테두리
	
	WCHAR wszGuildRecruitNotice[GUILDRECRUITINTRODUCE+1];	// 길드모집게시판 소개글	
	short wGuildLevel;										// 길드 레벨
	short wGuildMemberPresentSize;							// 현재 길드원 수	
	short wGuildMemberMaxSize;								// 최대 길드원 수
#if defined( PRE_ADD_GUILD_EASYSYSTEM )
	BYTE		cPurposeCode;								// 길드 목적(1:친목 2:전투 3:네스트 4:콜로세움
	WCHAR		wszGuildHomePage[GUILDHISTORYTEXT_MAX+1];		//길드 홈페이지
	WCHAR		wszGuildMasterName[NAMELENMAX+1];			// 길드 마스터명
#endif
};
// 길드모집게시판을 통해서 길드가입신청을 한 유저목록
struct TGuildRecruitCharacter 
{
	INT64 nCharacterDBID;					// 캐릭터 DBID	
	WCHAR wszCharacterName[NAMELENMAX+1];		// 캐릭터 이름
	TP_JOB nJob;							// 전직직업
	BYTE cLevel;							// 레벨
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
	int nLeftUseTime; // 남은 사용시간 (분단위)
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
	DWORD dwPartsColor2;	// Vehicle:없음, Pet:Nose
	int nExp;
	int nRange;				// 펫 줍기범위
	WCHAR	wszNickName[NAMELENMAX];
	bool bSkillSlot;	// 펫 스킬 슬롯 확장여부
	int nSkillID1;	// 스킬 ID 1번
	int nSkillID2;	// 스킬 ID 2번
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
	bool bEternity;					// 영구아이템
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
	int nChannelID;				// DWC 대회 회차 정보
	__time64_t tStartDate;		// 시작시간
	__time64_t tEndDate;		// 종료 시간
	BYTE cStatus;				// DWC::DWCStatus
	int nLadderTime[2];			// DWC 래더 매칭 시간, 총 2회의 매칭 시간을 설정할 수 있음
	BYTE cLimitCharacterLevel;	// DWC 캐릭터 생성하기 위한 보유 캐릭터 레벨 조건
};
#endif // #if defined( PRE_ADD_DWC )

//--------------------------------------------------------------
//                         패킷관련
//--------------------------------------------------------------
//-------------------------------------------------
//	Client <-> Login
//-------------------------------------------------
/* 
	CS_CHECKLOGIN -> SC_CHECKLOGIN
	CS_SERVERLIST -> SC_SERVERLIST
	CS_SELECTSERVER -> SC_CHARLIST
	CS_SELECTCHAR -> SC_CHANNELLIST
	CS_SELECTCHANNEL -> 마을로!!
*/


struct CSCheckVersion	 		// CS_LOGIN / CS_CHECKVERSION
{
	BYTE cNation;
	BYTE cVersion;
	bool bCheck;
	USHORT nMajorVersion;
	USHORT nMinorVersion;
	char Stampila[4];

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
	WCHAR wszRequestTime[30];	// 19(YYYY-MM-DD HH:NN:SS) + 6(공백->\_3, :->\_7 로 대체)
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
#ifdef RLKT_ADD_MD5_AUTH
	char szPassword[32];//md5.
#else
	char szPassword[PASSWORDLENMAX];
#endif
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
#ifdef RLKT_ADD_MD5_AUTH
	unsigned char wszPassword[32]; //md5
#else
	WCHAR wszPassword[PASSWORDLENMAX];
#endif
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
	bool bReConnectNewbieReward;	// 예) 신규 유저 재접 보상 수령 대상자(NewbieReward: true/RegTime: 생성시간)
	__time64_t _tAccountRegistTime; //     신규 유저 보상 시간이 남은 경우(NewbieReward: false/RegTime: 생성시간), 보상 대상 아닌경우(false/0)
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
	int nDefaultBody;	// 처음 생성시에 설정한 equip
	int nDefaultLeg;
	int nDefaultHand;
	int nDefaultFoot;
	DWORD dwHairColor;
	DWORD dwEyeColor;
	DWORD dwSkinColor;
	INT64 biUniqueID;	// nexon 메신저에서 쓰는 용도(?)
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
	//[debug]  董珂빔쫠侶몸俚쌘
//#if 0
	BYTE cAccountLevel; // DWC 캐릭터 = 40
//#endif

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
	BYTE cCharCount;	// 몇개인지
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
	int nValue[SecondAuth::Common::PWMaxLength];	// 2차 인증 비밀번호 값
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
	bool isDarkClass;
};

struct SCCreateChar				// SC_LOGIN / SC_CREATECHAR
{
	WCHAR wszCharacterName[NAMELENMAX];
	int nRet;
	char cDailyCreateMaxCount;	// 최대 개수
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
	// 기존에 생성 되어있는 캐릭터 중에서 하나를 지정해서 이름을 생성 함
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
	UINT nAccountDBID;				// 계정 DBID
	INT64 biCertifyingKey;			// 인증키
};

struct CSConnectVillage			// CS_SYSTEM / CS_CONNECTVILLAGE
{
	UINT nSessionID;
	UINT nAccountDBID;				// 계정 DBID
	INT64 biCertifyingKey;			// 인증키
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
	UINT nAccountDBID;				// 계정 DBID
	INT64 biCertifyingKey;			// 인증키
};

struct CSConnectRequest			// CS_SYSTEM / CS_CONNECT_REQUEST
{
	UINT nSessionID;
	ULONG nAddrIP;
	USHORT nPort;
};

struct SCConnectedResult		// SC_SYSTEM / SC_CONNECTED_RESULT
{
	UINT nSessionID;				//그냥 ㅎㅎㅎ 님하 오케위~
	char szServerVersion[SERVERVERSIONMAX];
};

struct CSReady2Recieve			// CS_SYSTEM / CS_READY_2_RECIEVE
{
	UINT nSessionID;						//혹시나 해서
	WCHAR wszVirtualIp[IPLENMAX];	// nexon pcbang
};

struct CSIntendedDisconnect		// CS_SYSTEM / CS_INTENDED_DISCONNECT
{
};

struct CSConnectGame				// CS_SYSTEM / CS_CONNECTGAME
{
	UINT nSessionID;
	UINT nAccountDBID;				// 계정 DBID
	INT64 biCertifyingKey;			// 인증키
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
	//님들하 뉘들끼리 연결좀 하삼~
	ULONG nDestAddrIP;
	USHORT nDestAddrPort;
	UINT nSessionID[2];
};

struct CSReconnectLogin			// CS_SYSTEM / CS_RECONNECTLOGIN
{
	UINT nSessionID;
	UINT nAccountDBID;				// 계정 DBID
	INT64 biCertifyingKey;			// 인증키
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
	bool bIsPartyOut;			//파티에서 나가는 것이면 트루, 그냥 포기면 팔스
	bool bIntenedDisconnect;
};

struct SCReconnectLogin			// SC_SYSTEM / SC_RECONNECTLOGIN
{
	int nRet;
	UINT nAccountDBID;				// 계정 DBID
	INT64 biCertifyingKey;			// 인증키
};

struct SCCountDownMsg			// SC_SYSTEM / SC_COUNTDOWNMSG
{
	BYTE cIsStart;				// 1:start 0:stop
	int nCountTime;
	UINT nCode;					//	사용할 uistringnum
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
struct CSVIPAutomaticPay		// CS_SYSTEM / CS_VIPAUTOMATICPAY,				// VIP자동결제 
{
	bool bAutomaticPay;
};

struct SCVIPAutomaticPay		// SC_SYSTEM / SC_VIPAUTOMATICPAY,				// VIP자동결제 
{
	bool bAutomaticPay;
};

#endif	// #if defined(PRE_ADD_VIP)


struct CSTCPPing
{
	DWORD dwTick;
};

typedef CSTCPPing SCTCPPing;


struct SCFieldPing
{
	DWORD dwTick;
};

struct SCServiceClose			//SC_SYSTEM / SC_SERVICECLOSE				//서비스클로우즈 알리미~
{
	__time64_t _tNow;				//서버센드된시각
	__time64_t _tOderedTime;		//서비스매니저에 명령되어진 시각(혹시나 필요할까 해서)
	__time64_t _tCloseTime;			//서비스클로즈 예정시각
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
	UINT nSessionID;		//사용한사람 왠지 필요할꺼 같아서
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
	char cAccountLevel;	// 계정권한 (eAccountLevel)
	BYTE cPartyState;
	BYTE cClassID;
	BYTE cJobArray[JOBMAX];
	BYTE cLevel;
	int	nExp;
	INT64 nCoin;	// 전체 가지고 있는 코인이기 때문에 int64로
	BYTE cRebirthCoin;
	BYTE cPCBangRebirthCoin;
	USHORT wRebirthCashCoin;
	EtVector3 Position;
	float fRotate;
	USHORT wSkillPoint[DualSkill::Type::MAX];
	int	nDefaultBody;	// 처음 생성시에 설정한 equip
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
	USHORT wVIPFatigue;	// 남은 피로도
	int nVIPPoint;		// 포인트
	__time64_t tVIPExpirationDate;	// 만료일
	bool bVIPAutomaticPay;	// 자동결제 여부
#endif	// #if defined(PRE_ADD_VIP)
	TPvPGroup sPvPInfo;
	char cViewCashEquipBitmap[VIEWCASHEQUIPMAX_BITSIZE];
	TGuildSelfView GuildSelfView;	// 길드 시각정보 (개인)
	char cCurrentGuildWarEventStep;	// 현재 길드전 스텝
	short wCurrentGuldWarScheduleID;	// 현재 진행되는 길드전 차수
	char cSkillPage;					// 현재 활성화 되어있는 스킬페이지
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
	char cAccountLevel;	// 계정권한
	bool bHide;
	BYTE cPartyState;
	BYTE cClassID;
	BYTE cLevel;
	BYTE cPvPLevel;
	BYTE cJob;
	EtVector3 Position;
	float fRotate;
	bool bBattleMode;
	int	nDefaultBody;	// 처음 생성시에 설정한 equip
	int	nDefaultLeg;
	int	nDefaultHand;
	int	nDefaultFoot;
	DWORD dwHairColor;
	DWORD dwEyeColor;
	DWORD dwSkinColor;
	short nAppellation;
	short nCoverAppellation;
	TGuildSelfView GuildSelfView;	// 길드 시각정보 (개인)
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
	TItemInfo EquipArray[EQUIPMAX+CASHEQUIPMAX+GLYPHMAX+TALISMAN_MAX];		// 이게 항상 맨 끝이어야합니다.
#else
	TItemInfo EquipArray[EQUIPMAX+CASHEQUIPMAX+GLYPHMAX];		// 이게 항상 맨 끝이어야합니다.
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
	char cAccountLevel;	// 계정권한
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
	BYTE cSlotIndex;		// 단축슬롯창 index
	BYTE cSlotType;			// eQuickSlotType 참조
	INT64 nID;				// 해당 ID (아이템아이디 혹은 스킬아이디 혹은 탈것 아이템 시리얼)
};

struct SCAddQuickSlot		// SC_CHAR / SC_ADDQUICKSLOT
{
	BYTE cSlotIndex;		// 단축슬롯창 index
	BYTE cSlotType;			// eQuickType 참조
	int nID;				// 해당 ID (아이템아이디 혹은 스킬아이디)
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
	short wFatigue;		// 현재값
	short wWeekFatigue;		// 주간 현재값
	short wPCBangFatigue;	// 피씨방 현재값
	short wEventFatigue;
#if defined(PRE_ADD_VIP)
	short wVIPFatigue;		// VIP 현재값
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
	BYTE cRaidParty;			//일단은 5인이상 8인이하의 파티를 지칭함 1:true, otherwise:false
	BYTE cIsJobDice;
	BYTE cUserLvLimitMax;
#endif // #if defined( PRE_PARTY_DB )
	BYTE cPartyMemberMax;

	TPARTYITEMLOOTRULE ItemLootRule;			//ePartyItemLootRule 참조
	TITEMRANK ItemRank;				//아이템랭크(루트룰기획참조)
	BYTE cUserLvLimitMin;		//민맥스값이 있단다.

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
	BYTE cIsAutoCreate;			//자동생성 여부 0: 일반생성 1자동생성
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
	BYTE cIsOpenBlind;		//대화중이거나 창이 열려있는 경우
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
	char cKickKind;						//0일반 1영구
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
	WCHAR wszPartyAsker[NAMELENMAX];		//대상자
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
	//short nCanEnterCode;				//ERROR_NONE이면 들어갈 수 있음 아닌 값이면 상황에 따른값 eError 값참조
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
	BYTE cOffSetCnt;		//한번에 받고 싶은 카운트
	BYTE cGetPage;		//위에 있는 카운트를 오프셋으로 몇번째 페이지를 받고 싶은지
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
	BYTE cPartyRefreshSubJect;					//ePartyRefreshSubject 참조
#if defined( PRE_WORLDCOMBINE_PARTY )
	int nWorldCombinePartyTableIndex;
#endif
	WCHAR wszName[PARTYNAMELENMAX];
	SPartyMemberInfo Member[PARTYMAX];
};

struct SCPartyOut				// SC_PARTY / SC_PARTYOUT
{
	UINT nSessionID;		//누가 나간건지
	int nRetCode;	// success, fail 구별
	char cKickKind;	//-1킥아니고 나간거 0일반킥 1영구킥
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
	int nWorldCombinePartyTableIndex;		// 월드파티 인덱스
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
	SPartyListInfo PartyListInfo[PARTYLISTOFFSET*2];	// 페이징 비용을 줄이기 위해 한번에 2페이지씩 보낸다.
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

struct CSGameToGameStageEnter
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
	short nExtendDropRate;			//이벤트용 추가 드랍확율
};
	
struct SCStartStageDirect
{
	int nMapIndex;
	char cGateNo;
	int nRandomSeed;
	TDUNGEONDIFFICULTY Difficulty;
	short nExtendDropRate;			//이벤트용 추가 드랍확율
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
	int nGameMapIdx;					//이게 0 보다 크다면 게임서버에서 초대받는거다
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
	WCHAR wszInvitedCharacterName[NAMELENMAX];		//초대하는 사람의 캐릭터이름
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
	char	cAccountLevel; // 계정권한(eAccountLevel)
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
	int nCount;	// 파티원 길드 시각화 정보 목록 개수
	USHORT unStartIndex;
	TPartyMemberGuildView List[MAX_SEND_SYNCPLAYER];	// 파티원 길드 시각화 정보 목록
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
	BYTE cVerifyFlag[5];		//0부터 순차적으로 어려워짐//0 not allowed, 1 allowed
};

struct SCPartyBonusValue	//SC_PARTY / SC_PARTYBONUSVALUE
{
	BYTE cUpkeepBonus;		//%단위
	BYTE cFriendBonus;		//%단위
	BYTE cUpKeepBonusLimit;	//유지보너스 맥스 지호씨 이게 0이상이면 가득찼다고 띄워주시면 데용~ 
	//일단은 가득차있어도 평상시에는 0이 가고 클리어시점에만 판단되어서 0이상이 나갑니다. 이거 받았을때 0이 아니면 파팡 찍어주시면뎀
#ifdef PRE_ADD_BEGINNERGUILD
	BYTE cBeginnerGuildBonus;	//초보길드원끼리 파티일경우
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
	BYTE cInvenMaxCount;	// 몇개까지 열려있느냐 (일반인벤만. 기간제 포함 안되어있음)
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

// api_trigger_InvenToEquip 트리거에서 사용하므로 해당구조체 변경시 같이 변경되어야 합니다!!!
struct CSMoveItem				// CS_ITEM / CS_MOVEITEM,
{
	char cMoveType;	// eItemMoveType 참조 (MoveType_EquipToInven, MoveType_InvenToEquip)
	BYTE cSrcIndex;
	INT64 biSrcItemSerial;
	BYTE cDestIndex;
	short wCount;
};

struct SCMoveItem				// SC_ITEM / SC_MOVEITEM,
{
	char cMoveType;	// eItemMoveType 참조 (MoveType_EquipToInven, MoveType_InvenToEquip...)
	int nRetCode;
	TItemInfo SrcItem;
	TItemInfo DestItem;
};

// api_trigger_UnRideVehicle 트리거에서 사용하므로 해당구조체 변경시 같이 변경되어야 합니다!!!
struct CSMoveCashItem			// CS_ITEM / CS_MOVECASHITEM,
{
	char cMoveType;	// eItemMoveType 참조 (MoveType_CashEquipToCashInven, MoveType_CashInvenToCashEquip)
	BYTE cCashEquipIndex;
	INT64 biEquipItemSerial;
	INT64 biCashInvenSerial;
	short wCount;
};

struct SCMoveCashItem			// SC_ITEM / SC_MOVECASHITEM,
{
	char cMoveType;	// eItemMoveType 참조 (MoveType_CashEquipToCashInven, MoveType_CashInvenToCashEquip)
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
	char cMoveType;	// eItemMoveType 참조 (MoveType_InvenToServerWare, MoveType_ServerWareToInven)
	BYTE cInvenIndex;
	INT64 biInvenItemSerial;
	INT64 biWareItemSerial;	
	short wCount;
};

struct SCMoveServerWare			// SC_ITEM / SC_MOVE_SERVERWARE
{
	char cMoveType;	// eItemMoveType 참조 (MoveType_CashEquipToCashInven, MoveType_CashInvenToCashEquip)
	int nRetCode;
	TItemInfo InvenItem;
	TItem ServerWareItem;
};

struct CSMoveCashServerWare		// CS_ITEM / CS_MOVE_CASHSERVERWARE
{
	char cMoveType;	// eItemMoveType 참조 (MoveType_CashToServerWare, MoveType_ServerWareToCash)
	INT64 biItemSerial;		
};

struct SCMoveCashServerWare			// SC_ITEM / SC_MOVE_CASHSERVERWARE
{
	char cMoveType;	// eItemMoveType 참조 (MoveType_CashToServerWare, MoveType_ServerWareToCash)
	int nRetCode;
	INT64 biSrcItemSerial;
	TItem SrcItem;
	TItem DestItem;
};
#endif //#if defined(PRE_ADD_SERVER_WAREHOUSE)

struct CSMoveGuildItem			// CS_ITEM / CS_MOVEGUILDITEM,
{
	char	cMoveType;				// eItemMoveType 참조 (MoveType_InvenToGuildWare, MoveType_GuildWareToInven)
	BYTE	cDestIndex;				// 도착 인덱스 (길드창고 <--> 인벤)
	INT64	biDestItemSerial;		// 이벤트를 받을 아이템 시리얼
	BYTE	cSrcIndex;				// 출발 인덱스 (길드창고 <--> 인벤)
	INT64	biSrcItemSerial;		// 이벤트 발생한 아이템
	short	wCount;					// 해당 갯수
};

struct SCMoveGuildItem			// SC_ITEM / SC_MOVEGUILDITEM,
{
	char cMoveType;	// eItemMoveType 참조 (MoveType_InvenToGuildWare, MoveType_GuildWareToInven)

	int nRetCode;
	int nTakeItemCount;

	INT64	biSrcSerial;
	INT64   biDestSerial;

	TItemInfo DestItem;
	TItemInfo SrcItem;
};

struct SCRefreshGuildItem		// SC_ITEM / SC_REFRESH_GUILDITEM
{
	char cMoveType;	// eItemMoveType 참조 (MoveType_InvenToGuildWare, MoveType_GuildWareToInven)

	INT64	biSrcSerial;
	INT64   biDestSerial;

	TItemInfo SrcItem;
	TItemInfo DestItem;	
};

struct SCRefreshGuildCoin	 // SC_ITEM / SC_REFRESH_GUILDCOIN
{
	INT64 biTotalCoin;		// 전체금액
};

struct CSMoveGuildCoin			// CS_ITEM / CS_MOVEGUILDCOIN
{
	char	cMoveType;			// eItemMoveType 참조 (MoveType_InvenToGuildWare, MoveType_GuildWareToInven)
	INT64	nMoveCoin;
};

struct SCMoveGuildCoin			// SC_ITEM / SC_MOVEGUILDCOIN
{
	int nRet;
	char cMoveType;				// eItemMoveType 참조 (MoveType_InvenToGuildWare, MoveType_GuildWareToInven)
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
	bool bNewSign;	// new표시 할껀가 안할껀가
	TItemInfo ItemInfo;
};

struct SCRefreshCashInven		// SC_ITEM / SC_REFRESHINVEN
{
	bool bNewSign;	// new표시 할껀가 안할껀가
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
	SCCreateDropItem sDropItem[20];	//	한번에 최대 보낼 수 있는 DropItem 최대 개수
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
	BYTE cWareMaxCount;	// 창고 max값이 몇까지 열수 있는지 (일반창고만. 기간제 포함 안되어있음)
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
	BYTE cInvenIndex;			// 일반인벤일때만 index넣어주면 됨 (캐쉬는 참조하지않음)
	INT64 biInvenSerial;
};

struct SCUseItem				// SC_ITEM / SC_USEITEM
{
	char cInvenType;			// eItemPositionType
	BYTE cInvenIndex;			// 일반인벤일때만 index넣어주면 됨 (캐쉬는 참조하지않음)
	INT64 biInvenSerial;
};

struct CSCompleteRandomItem
{
	char cInvenType;			// eItemPositionType
	BYTE cInvenIndex;			// 일반인벤일때만 index넣어주면 됨 (캐쉬는 참조하지않음)
	INT64 biInvenSerial;
};

// 추후에 NPC 별로 리스트 보여달라고 넣어야 할 듯..
// 지금은 임시로 CompoundTableID 넣음
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
	UINT nNpcUniqueID;			// 게임서버에서 처리해야하고, Npc위치까지 알아야해서 UniqueID 추가로 보낸다.
	BYTE cSlotIndex;
	INT64 biItemSerial;
	short nCount;
};

struct CSItemDisjointReqNew		// CS_ITEM / CS_DISJOINT_REQ 
{
	UINT nNpcUniqueID;			// 게임서버에서 처리해야하고, Npc위치까지 알아야해서 UniqueID 추가로 보낸다.
	BYTE cSlotIndex[4];
	INT64 biItemSerial[4];
	short nCount[4];
};

struct SCItemDisjointRes		// SC_ITEM / SC_DISJOINT_RES
{
	int nItemID;
	bool bSuccess;
};

struct SCItemDisjointResNew		// SC_ITEM / SC_DISJOINT_RES //rlkt_disjoint
{
	int nItemID[20];
	int nCount[20];
	bool bSuccess;
};

struct SCInventoryMaxCount		// SC_ITEM / SC_INVENTORYMAXCOUNT
{
	BYTE cInvenMaxCount;	// 몇개까지 열려있느냐
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
	BYTE cWareMaxCount;	// 창고 max값이 몇까지 열수 있는지
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
	int nUsableRebirthCoin;		// 던젼 가능 남은 코인개수
	BYTE cRebirthCoin;			// 부활코인 남은개수
	USHORT wRebirthCashCoin;	// 캐쉬템 부활코인 남은개수
	int nRet;						// 코인 사용 성공 여부
	int nRebirthType;
	UINT nSessionID;
	BYTE cPCBangRebirthCoin;			// 부활코인 남은개수
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
	char cMoveType;	// eItemMoveType 참조 (MoveType_EquipToInven, MoveType_InvenToEquip)
	INT64 nMoveCoin;
};

struct SCMoveCoin				// SC_ITEM / SC_MOVECOIN
{
	char cMoveType;	// eItemMoveType 참조 (MoveType_EquipToInven, MoveType_InvenToEquip)
	INT64 nMoveCoin;
	INT64 nInventoryCoin;
	INT64 nWarehouseCoin;
	int nRet;
};

struct SCRequestRandomItem
{
	char cInvenType;			// eItemPositionType
	BYTE cInvenIndex;			// 일반인벤일때만 index넣어주면 됨 (캐쉬는 참조하지않음)
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
	int nDelayTime;			// glyph 관련된 cooltime
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

// 강화
/*
CS_ENCHANT			강화할 아이템 올려놓고 강화버튼 누르고
SC_ENCHANT			가능한지 응답받고 동영상나오고
CS_ENCHANTCOMPLETE	동영상 끝나면 강화완료라고 보내고
SC_ENCHANTCOMPLETE	강화결과 알려주고 (끝)
CS_ENCHANTCANCEL	중간에 취소할꺼면 날려주고
SC_ENCHANTCANCEL	
*/

struct CSEnchantItem
{
#if defined(PRE_ADD_EQUIPED_ITEM_ENCHANT)
	int cItemIndex;
	bool bEnchantEquipedItem;
#else
	BYTE cInvenIndex;
#endif
	INT64 biItemSerial;
#if defined(PRE_ADD_ENCHANTSHIELD_CASHITEM) // 추후에 강화 보조 아이템이 더 추가될수 있어서 변경합니다.
	char cEnchantGuardType;				// 강화 보호 타입 0-NONE, 1-강화보호젤리(파괴x), 2-상위강화보호젤리(파괴x,다운x) eEnchantGuardType
#else
	bool bUseEnchantGuard;
#endif	// #if defined(PRE_ADD_ENCHANTSHIELD_CASHITEM)
	bool bCashItem;
};

struct SCEnchantItem
{
#if defined(PRE_ADD_EQUIPED_ITEM_ENCHANT)
	int cItemIndex;
	bool bEnchantEquipedItem;
#else
	BYTE cInvenIndex;
#endif
	bool bCashItem;
	int nRet;
};

struct SCEnchantComplete
{
	int nItemID;
	char cLevel;
	char cOption;
	bool bIsCashItem;
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

// SC_CHANGEGLYPH,					// 주변애들한테 glyph정보 날려주기
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
	bool bNewSign;	// new표시 할껀가 안할껀가
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
	bool bEternity;		// 영구아이템
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
	bool bActivate;				// 탈리스만 캐쉬 확장 슬롯 활성화 여부
	__time64_t	tTalismanExpireDate;	// 탈리스만 슬롯 기간
};
#endif

#if defined(PRE_ADD_EQUIPLOCK)
struct CSItemLockReq	// CS_ITEM / CS_ITEM_LOCK_REQ, CS_ITEM_UNLOCK_REQ
{
	bool IsCashEquip;
	BYTE nItemSlotIndex; // Lock(UnLock)시킬 Item의 인덱스 번호	
	INT64 biItemSerial;	// 해당 아이템 Serial
};

struct SCItemLockRes	// SC_ITEM / SC_ITEM_LOCK_RES, SC_ITEM_UNLOCK_RES
{
	int nRet; //결과값
	bool IsCashEquip;
	EquipItemLock::TLockItemInfo LockItems;
};

struct SCEquipItemLockList	// SC_ITEM / SC_EQUIPITEM_LOCKLIST
{
	int nEquipCount;	//일반 장비 갯수
	int nCashEquipCount; //캐쉬 장비 갯수
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

// 클라이언트에서 요청온 스킬 리셋 아이템 사용에 관해 사용 가능한지 아닌지 응답.
struct SCCanUseSkillResetCashItem
{
	INT64 biItemSerial;
	int nRet;
	int nRewardSP;
};

// 클라이언트에서 최종적으로 스킬 리셋 아이템 사용 패킷을 보냄
struct CSUseSkillResetCashItemReq
{
#if defined(PRE_MOD_SKILLRESETITEM)
	char cInvenType;			// eItemPositionType
	BYTE cInvenIndex;			// 일반인벤일때만 index넣어주면 됨 (캐쉬는 참조하지않음)
#endif
	INT64 biItemSerial;
	char cSkillPage;
};

// 스킬 리셋 캐쉬템 사용한 결과를 클라로 통보
struct SCUseSkillResetCashItemRes
{
	int nRet;
	int nResultSP;		// 최종 결과로 나온 스킬 포인트.
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

//	코스튬 합성
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
	short sInvenIndex;			// 일반인벤일때만 index넣어주면 됨 (캐쉬는 참조하지않음)
	INT64 biInvenSerial;
	BYTE cKeyInvenIndex;			// 일반인벤일때만 index넣어주면 됨 (캐쉬는 참조하지않음)
	int nKeyItemID;
	INT64 biKeyItemSerial;
};

#ifdef PRE_ADD_GETITEM_WITH_RANDOMVALUE
struct TRangomGiveItemData
{
	int nItemID;	// 아이템
	int nCount;		// 유저가 획득할 개수
	int nPeriod;	// 기간
	int nGold;
};
#endif		//#ifdef PRE_ADD_GETITEM_WITH_RANDOMVALUE

struct SCCharmItemRequest		// CS_ITEMGOODS / SC_CHARMITEMREQUEST
{
	char cInvenType;			// eItemPositionType
	short sInvenIndex;			// 일반인벤일때만 index넣어주면 됨 (캐쉬는 참조하지않음)
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
	short sInvenIndex;			// 일반인벤일때만 index넣어주면 됨 (캐쉬는 참조하지않음)
	INT64 biInvenSerial;
	BYTE cKeyInvenIndex;			// 일반인벤일때만 index넣어주면 됨 (캐쉬는 참조하지않음)
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
	int nChoiceItemID;	// 선택한 결과아이템(초이스박스에서만 사용)
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
	int nChoiceItemID;	// 선택한 결과아이템(초이스박스에서만 사용)
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

struct SCPetalTokenResult		// SC_ITEMGOODS / SC_PETALTOKENRESULT,			// 페탈 상품권 산거 채팅창에..
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
	int nRet;					//ERROR_DB_CHANGENAME_NEXTDAY일 경우tNextChangeDate로 날짜제한 표시
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
	INT64 biExpireDateItemSerial;	// 기간연장에 사용한 아이템 시리얼
	INT64 biItemSerial;				// 기간 연장할 아이템 시리얼
};

struct CSPetSkillItem
{	
	INT64 biItemSerial;				// 펫아이템 Serial 번호
};

struct SCPetSkillItem
{
	int nRet;
	char cSlotNum;					// 슬롯 Num
	int nSkillID;					// 스킬 ID
};

struct CSPetSkillDelete
{
	char cSlotNum;					// 삭제할 슬롯 Num
};

struct SCPetSkillDelete
{
	int nRet;
	char cSlotNum;
};

struct CSPetSkillExpand
{
	INT64 biItemSerial;				// 스킬 확장 아이템 Serial
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


struct CSGuildMark		// CS_ITEMGOODS / CS_GUILDMARK		// 길드마크 변경
{
	int nItemID;
	INT64 biItemSerial;
	int nInvenIndex;		// 일반아이템일 경우 인벤토리인덱스
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
	UINT		nIndex;				// 보상테이블 인덱스
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
	int nRet;					// 1: success -1: 상대없다 -2: 본인한테 보낸것
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
	UINT nAccountDBID;			// 계정 DBID
	INT64 nCharacterDBID;			// 캐릭터 DBID
	short nLen;					// 채팅 길이
	WCHAR wszChatMsg[CHATLENMAX];	// 채팅 메시지
};

#ifdef PRE_ADD_DOORS_GUILDCHAT_DISCONNECT
struct SCDoorsGuildChat
{
	TGuildUID GuildUID;
	INT64 biCharacterDBID;			// 캐릭터 DBID
	short nLen;					// 채팅 길이
	WCHAR wszChatMsg[CHATLENMAX];	// 채팅 메시지
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
	char cCaption;				// 클라의 CDnInterface::emCAPTION_TYPE 요거임.
	BYTE cFadeTime;				// 페이드 시간(second)
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
	bool bUseRotationQuat;		// SummonMonster 시그널을 사용한 경우 소환된 몬스터의 각도값을 패킷으로 보내줌.
	EtQuat qRotation;
};

// 가변 패킷으로 대체됨. /////////////////////////////////////////////////
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
	BYTE nType;							//eEventType3 참조
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
	char cRewardItemLevel[4]; //강화정보
	char cRewardItemPotential[4]; //잠재력 정보
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
	TGuildSelfView		GuildSelfView;	// 길드 시각정보 (개인)
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
	char				cAccountLevel; // 계정권한 (eAccountLevel)
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
	BYTE nLevel;		// LevelUp일 경우만 최종 레벨.
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
	BYTE cCount;				//해당 프리셋이 가지구 있는 스킬의 수
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
// npc상점
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
	BYTE cShopIndex;			// shoplist에서 받은 해당 index값
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
	int nRet;	// 현재 의미 없음.
};

struct SCSkillShopBuyResult		// SC_TRADE / SC_SKILLSHOP_BUY
{
	int nSkillID;
	short wSkillPoint;
	int nRet;
};

// 우편
struct CSMailBox				// CS_TRADE / CS_MAILBOX
{
	char cPageNum;			// 페이지 번호: 1부터 시작
};

struct TMailBoxInfo
{
	int nMailDBID;							// mail db table id
	short wIndex;
	WCHAR wszFromCharacterName[MAILNAMELENMAX];	// 보낸이
	WCHAR wszTitle[MAILTITLELENMAX];		// 우편 제목
	BYTE cRemainDay;						// 남은날짜
	char cReadType;							// 개봉 상태
	char cAttachType;						// 아이템 첨부상태
	char cDeliveryType;
	char cMailType;							// 메일종류 (0:일반메일, 1:미션메일, 2:마켓메일)
	bool bNewFlag;							// 제목/본문 코드화 여부
};

struct SCMailBox				// SC_TRADE / SC_MAILBOX
{
	int nRetCode;
	char cDailyMailCount;		// 일일발송개수
	short wTotalCount;
	short wUnreadCount;
	char cMailBoxCount;					// 몇개보내는건지
	TMailBoxInfo MailBox[MAILBOXMAX];	// 메일 내용
};

struct TAttachMailItem
{
	int nInvenIndex;	// 첨부되는 아이템의 인벤 인덱스
	int nItemID;
	int nCount;			// 첨부되는 아이템 개수
	INT64 biSerial;
};

struct CSSendMail				// CS_TRADE / CS_SENDMAIL
{
	WCHAR wszToCharacterName[NAMELENMAX];	// 받는이
	WCHAR wszTitle[MAILTITLELENMAX];		// 제목
	WCHAR wszText[MAILTEXTLENMAX];			// 내용
	char cDeliveryType;						// eMailType (MAILTYPE_NORMAL, MAILTYPE_PREMIUM)
	int nAttachCoin;						// 코인첨부 (21억을 넘진않을듯)
	char cAttachItemTotalCount;
	TAttachMailItem AttachMailItem[MAILATTACHITEMMAX];
};

struct CSMailDBIDArray			// CS_TRADE / CS_MAIL_DELETE, CS_MAIL_ATTACHALL
{
	int nMailDBID[MAILPAGEMAX];
};

struct SCSendMail				// SC_TRADE / SC_MAIL_SEND
{
	char cDailyMailCount;		// 일일발송개수
	int nRet;			// ERROR_MAIL_  로 시작되는 것들로 보내질것임
};

struct CSAttachMail
{
	int nMailDBID;
	char cAttachSlotIndex;		// -1: 5칸 모두 받기, 0 - 4: 개별받기
};

struct SCMailResult				// SC_TRADE / SC_MAIL_ATTACH
{
	int nRet;			// ERROR_MAIL_  로 시작되는 것들로 보내질것임
};

struct SCMailDeleteResult		// SC_TRADE / SC_MAIL_DELETE
{
	int nTotalMailCount;
	int nRet;			// ERROR_MAIL_  로 시작되는 것들로 보내질것임
};

struct SCAttachAllMailResult	// SC_TRADE / SC_MAIL_ATTACHALL
{
	int nMailDBID[MAILPAGEMAX];	
	int nRet;			// ERROR_MAIL_  로 시작되는 것들로 보내질것임
};

struct CSReadMail				// CS_TRADE / CS_MAIL_READ, CS_MAIL_READCADGE
{
	int nMailDBID;
};

struct SCReadMail				// SC_TRADE / SC_MAIL_READ
{
	int nRet;			// ERROR_MAIL_  로 시작되는 것들로 보내질것임
	bool bNewFlag;							// 제목, 내용 코드화 여부
	short wNotReadMailCount;				// 읽지 않은 메일
	int nMailDBID;
	WCHAR wszText[MAILTEXTLENMAX];			// 내용
	__time64_t tSendDate;
	int nAttachCoin;
	char cDeliveryType;
	char cAttachCount;
	TItemInfo AttachItem[MAILATTACHITEMMAX];	// 첨부된 아이템
};

struct TWishItemInfo
{
	int nItemSN;
	int nItemID;
	char nOptionIndex;
};

struct SCReadCadgeMail			// SC_TRADE / SC_MAIL_READCADGE
{
	int nRet;								// ERROR_MAIL_  로 시작되는 것들로 보내질것임
	bool bNewFlag;							// 제목, 내용 코드화 여부
	short wNotReadMailCount;				// 읽지 않은 메일
	int nMailDBID;
	WCHAR wszText[MAILTEXTLENMAX];			// 내용
	__time64_t tSendDate;
	bool bCadgeComplete;					// 조르기 완료 여부
	int nPackageSN;
	char cWishListCount;
	TWishItemInfo WishList[PACKAGEITEMMAX];
};

struct SCNotifyMail
{
	bool bNewMail;						// 새 편지인지 그냥 읽지않은 메일만인지
	short wTotalMailCount;				// 총 우편수
	short wNotReadMailCount;				// 읽지 않은 메일
	bool bExpiration;					// 만료경고
};

// 무인상점
#ifdef PRE_ADD_ITEMNAME_AUTOCOMPLETE
struct TSearchTrade
{
	int nSearchItemID;
	int nSearchNameID;
};
#endif		//#ifdef PRE_ADD_ITEMNAME_AUTOCOMPLETE

struct CSMarketList				// CS_TRADE / CS_MARKETLIST
{
	short wPageNum;						// 페이지 번호: 1부터 시작
	char cMinLevel;
	char cMaxLevel;
	char cItemGrade[ITEMRANK_MAX];		// 등급분류
	char cJob;							// 직업구분 (모두: -1)
	short cMainType;					// 기본분류 (모두: -1)
	char cDetailType;					// 상세분류 (모두: -1)
	WCHAR wszSearchWord1[SEARCHLENMAX];	// 검색단어
	WCHAR wszSearchWord2[SEARCHLENMAX];	// 검색단어
	WCHAR wszSearchWord3[SEARCHLENMAX];	// 검색단어
	char cSortType;						// eSortType
	WCHAR wszSearchItemName[SEARCHLENMAX];	// 아이템 검색단어
	int nExchangeItemID;				// ExchangeTable의 ItemID
	bool bLowJobGroup;					// 하위 직업군 포함
	char cPayMethodCode;					// 0-혼합, 1-코인, 3-페탈
#ifdef PRE_ADD_ITEMNAME_AUTOCOMPLETE
	BYTE cCount;
	TSearchTrade SearchList[MultiLanguage::Common::SerachCountMax];
#endif		//#ifdef PRE_ADD_ITEMNAME_AUTOCOMPLETE
};

struct TMarketInfo
{
	int nMarketDBID;				// market table db ID

	WCHAR wszSellerName[NAMELENMAX];	// 판매자 이름

	int nItemID;
	short wCount;		// 개수
	USHORT wDur;			// 내구도
	int nRandomSeed;
	char cLevel;		// 레벨
	char cPotential;	// 잠재력
	char cOption;
	char cSealCount;	// 밀봉카운트

	int nPrice;		// 가격
	int nUnitPrice;	// 단가
	bool bDisplayFlag1;		// 0을 입력해 주세요. 현재 사용하지 않는 필드
	bool bDisplayFlag2;		// 0을 입력해 주세요. 현재 사용하지 않는 필드
	bool bPremiumTrade;	// 유료아이템을 통한 프리미엄 거래아이템
	char cMethodCode;	// 1=게임머니, 3=페탈
	char cPotentialMoveCount;		//잠재이전횟수
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
	short wCount;		// 개수
	USHORT wDur;			// 내구도
	int nRandomSeed;
	DWORD dwColor;		// 색깔
	char cLevel;		// 레벨
	char cPotential;	// 잠재력
	char cOption;
	char cSealCount;	// 밀봉카운트

	int nItemPrice;						// 가격
	char cSellType;						// 판매상태 0:안팔렸음, 1:팔렸음
	int nRemainTime;
	bool bPremiumTrade;	// 유료아이템을 통한 프리미엄 거래아이템
	char cPayMethodCode; // PayMethodCode (DB데이터) 1-골드, 3-페탈
	char cItemPotentialMoveCount;		//잠재이전카운트
};

struct SCMyMarketList			// SC_TRADE / SC_MYMARKETLIST
{
	int nRetCode;
	short wSellingCount;	// 판매중
	short wClosingCount;	// 기간마감
	short wWeeklyRegisterCount;		// 토요일 AM 04:00 를 기준으로 일주일간 등록한 거래 횟수
	short wRegisterItemCount;		// 등록한 아이템 개수
	bool bPremiumTrade;
	char cMarketCount;
	TMyMarketInfo MarketInfo[MYMARKETMAX];
};

struct CSMarketRegister			// CS_TRADE / CS_MARKET_REGISTER
{
	char cInvenType;			// eItemPositionType
	BYTE cInvenIndex;			// 일반인벤일때만 index넣어주면 됨 (캐쉬는 참조하지않음)
	INT64 biInvenSerial;
	short wCount;
#if defined(PRE_ADD_PETALTRADE)
	char cPayMethodCode;		// PayMethodCode (DB데이터) 1-골드, 3-페탈
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
	short wRegisterItemCount;		// 등록한 아이템 개수
	int nRet;
};

struct TMarketCalculationInfo
{
	int nMarketDBID;					// market table db ID

	int nItemID;
	short wCount;		// 개수
	USHORT wDur;			// 내구도
	int nRandomSeed;
	char cLevel;		// 레벨
	char cPotential;	// 잠재력
	char cOption;
	char cSealCount;	// 밀봉카운트

	int nPrice;			// 가격
	int nUnitPrice;		// 단가
	__time64_t tBuyDate;	// 판매일자
	char cPayMethodCode;	// PayMethodCode (DB데이터) 1-골드, 3-페탈
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
	int nItemID;				// 판매됐을경우 ItemID를 넣어준다
	short wCalculationCount;	// 정산개수
};

struct SCMarketPetalBalance				// SC_TRADE / SC_MARKET_PETALBALANCE
{
	int nRetCode;
	int nPetalBalance;				// 페탈
};

struct TMarketPrice
{
	bool bFlag;					// 등록여부

	int nAvgPrice;
	int nMinPrice;
	int nMaxPrice;

	BYTE cPayMethodCode;		// 1 = 게임머니, 3 = 페탈
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
	TMarketPrice ItemPrices[2];		// Index 0 : 게임머니 1: 페탈
};

struct CSMarketMiniList						// CS_TRADE / CS_MARKETMINILIST
{
	int nItemID;
	char cPayMethodCode;					// 0-혼합, 1-코인, 3-페탈
};

typedef SCMarketList SCMarketMiniList;		// SC_TRADE / SC_MARKETMINILIST


// 개인거래
struct CSExchangeRequest		// CS_TRADE / CS_EXCHANGE_REQUEST		// 거래 요청
{
	UINT nReceiverSessionID;	// 거래 할 유저
	bool bCancel;				// 다시 취소할껀지
};

struct SCExchangeRequest		// SC_TRADE / SC_EXCHANGE_REQUEST		// 거래 요청
{
	UINT nSenderSessionID;		// 거래 요청한 유저
	int nRet;
};

struct SCExchangeReject			// SC_TRADE / SC_EXCHANGE_REJECT		// 거래취소
{
	UINT nSessionID;
	int nRetCode;
};

struct CSExchangeAccept			// CS_TRADE / CS_EXCHANGE_ACCEPT		// 거래 수락
{
	UINT nSenderSessionID;		// 거래 요청한 유저
	bool bAccept;				// 수락 유무
};

struct SCExchangeStart			// SC_TRADE / SC_EXCHANGE_START			// 거래 시작
{
	UINT nTargetSessionID;		// 상대방
	int nRet;
};

struct CSExchangeAddItem		// CS_TRADE / CS_EXCHANGE_ADDITEM,		// 아이템 등록
{
	BYTE cExchangeIndex;		// 거래창 인덱스
	BYTE cInvenIndex;
	short wCount;
	INT64 biItemSerial;
};

struct SCExchangeAddItem		// SC_TRADE / SC_EXCHANGE_ADDITEM		// 아이템 등록
{
	UINT nSessionID;
	BYTE cExchangeIndex;		// 거래창 인덱스
	TItemInfo ItemInfo;
	int nRet;
};

struct CSExchangeDeleteItem		// CS_TRADE / CS_EXCHANGE_DELETEITEM,	// 아이템 등록해제
{
	BYTE cExchangeIndex;		// 거래창 index
};

struct SCExchangeDeleteItem		// SC_TRADE / SC_EXCHANGE_DELETEITEM
{
	UINT nSessionID;			// 거래창에 해당하는 유저
	BYTE cExchangeIndex;		// 거래창 index
	int nRet;
};

struct CSExchangeAddCoin		// CS_TRADE / CS_EXCHANGE_ADDCOIN,		// 코인 등록
{
	INT64 nCoin;	// 21억을 넘지 않을듯
};

struct SCExchangeAddCoin		// SC_TRADE / SC_EXCHANGE_ADDCOIN
{
	UINT nSessionID;			// 거래창에 해당하는 유저
	INT64 nCoin;	// 21억을 넘지 않을듯
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
	int nItemID;	// 선택보상일때만 세팅
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
	UINT nGroupDBID;					//이동된 그룹의 디비아이디
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

// CS_GUILD / CS_CREATEGUILD					// 길드 창설 요청
struct CSCreateGuild
{
	WCHAR wszGuildName[GUILDNAME_MAX];		// 길드 이름
};

// CS_GUILD / CS_INVITEGUILDMEMBREQ				// 길드원 초대 요청
struct CSInviteGuildMemberReq
{
	WCHAR wszToCharacterName[NAMELENMAX];		// 초대할 캐릭터 이름
};

// CS_GUILD / CS_INVITEGUILDMEMBACK				// 길드원 초대 응답
struct CSInviteGuildMemberAck
{
	TGuildUID GuildUID;		// 길드 UID
	UINT nFromSessionID;		// 초대한 세션 ID
	bool bAccept;				// 수락 여부

	UINT nFromAccountID;		// 초대한 UINT
};

// CS_GUILD / CS_EXILEGUILDMEMB					// 길드원 추방 요청
struct CSExileGuildMember
{
	UINT nAccountDBID;		// 계정 DBID
	INT64 nCharacterDBID;		// 캐릭터 DBID
};

// CS_GUILD / CS_CHANGEGUILDINFO				// 길드 정보 변경 요청
struct CSChangeGuildInfo
{
	BYTE btGuildUpdate;		// 길드 정보 변경 타입 (eGuildUpdateType)
	int Int1;
	int Int2;
	int Int3;
	int Int4;
	INT64 Int64;
	WCHAR Text[GUILDPARAMTEXT_MAX+1];
};

// CS_GUILD / CS_CHANGEGUILDMEMBINFO			// 길드원 정보 변경 요청
struct CSChangeGuildMemberInfo
{
	BYTE btGuildMemberUpdate;		// 길드원 정보 변경 타입 (eGuildMemberUpdateType)
	UINT nChgAccountDBID;					// 변경할 계정 DBID (내 정보를 수정하는 것이면 0 입력)
	INT64 nChgCharacterDBID;					// 변경할 캐릭터 DBID (내 정보를 수정하는 것이면 0 입력)
	int Int1;
	int Int2;
	INT64 Int64;
	WCHAR Text[GUILDPARAMTEXT_MAX+1];
};

// CS_GUILD / CS_GETGUILDHISTORYLIST			// 길드 히스토리 목록 요청
struct CSGetGuildHistoryList
{
	INT64 biIndex;	// 길드 히스토리 목록 인덱스 (0 이면 가장 최근 기록부터 확인 ?)
	bool bDirection;				// 목록 얻을 방향 (0:인덱스 이전 / 1:인덱스 이후)
};

// CS_GUILD / CS_GETGUILDINFO					// 길드 정보 요청
struct CSGetGuildInfo
{
	bool bNeedMembList;	// 길드원 목록도 필요한지 여부
};

// SC_GUILD / SC_CREATEGUILD					// 길드 창설 결과
struct SCCreateGuild
{
	UINT nSessionID;				// 세션 ID
	TGuildUID GuildUID;			// 길드 UID (실패일 경우 무시됨)
	TGuild Info;					// 길드 정보
	UINT nAccountDBID;			// 창설한 사용자 계정 DBID
	INT64 nCharacterDBID;			// 창설한 캐릭터 DBID
	TP_JOB nJob;					// 초대한 캐릭터 전직직업
	CHAR cLevel;					// 초대한 캐릭터 레벨
	TCommunityLocation Location;	// 현재 위치 (로그인 여부도 본 멤버의 상태로 체크)
	int iErrNo;					// 결과 (NOERROR : 성공 / 그외 : 실패)
	WCHAR wszGuildName[GUILDNAME_MAX];	// 길드 이름
};

// SC_GUILD / SC_DISMISSGUILD					// 길드 해체 결과
struct SCDismissGuild
{
	UINT nSessionID;		// 세션 ID
	int iErrNo;			// 결과 (NOERROR : 성공 / 그외 : 실패)
	TGuildUID GuildUID;	// 길드 UID (실패일 경우 무시됨)
};

// SC_GUILD / SC_INVITEGUILDMEMBREQ				// 길드원 초대 요청
struct SCInviteGuildMemberReq
{
	TGuildUID GuildUID;		// 길드 UID
	UINT nFromAccountDBID;	// 요청한 사용자 계정 DBID
	UINT nFromSessionID;		// 요청한 세션 ID
	UINT nToAccountDBID;		// 초대한 사용자 계정 DBID
	UINT nToSessionID;		// 초대한 세션 ID
	int iErrNo;				// 결과 (NOERROR : 성공 / 그외 : 실패)
	WCHAR wszFromCharacterName[NAMELENMAX];	// 요청한 캐릭터 이름 (P.S.> 요청한 사용자에게 오류로 송신될 경우 초대한 상대방의 이름이 됨)
	WCHAR wszGuildName[GUILDNAME_MAX];		// 길드명
};

// SC_GUILD / SC_INVITEGUILDMEMBACK				// 길드원 초대 결과
struct SCInviteGuildMemberAck
{
	UINT nToAccountDBID;			// 초대한 계정 DBID
	INT64 nToCharacterDBID;		// 초대한 캐릭터 DBID
	UINT nFromAccountDBID;		// 요청한 계정 DBID
	INT64 nFromCharacterDBID;		// 요청한 캐릭터 DBID
	TGuildUID GuildUID;			// 길드 UID (실패일 경우 무시됨)
	TP_JOB nJob;					// 초대한 캐릭터 전직직업
	CHAR cLevel;					// 초대한 캐릭터 레벨
	TCommunityLocation Location;	// 현재 위치 (로그인 여부도 본 멤버의 상태로 체크)
	int iErrNo;					// 결과 (NOERROR : 성공 / 그외 : 실패)
	WCHAR wszToCharacterName[NAMELENMAX];	// 초대한 캐릭터 이름
};

// SC_GUILD / SC_LEAVEGUILDMEMB					// 길드원 탈퇴 결과
struct SCLeaveGuildMember
{
	UINT nAccountDBID;		// 탈퇴한 계정 DBID
	INT64 nCharacterDBID;		// 탈퇴한 캐릭터 DBID
	TGuildUID GuildUID;		// 길드 UID (실패일 경우 무시됨)
	int iErrNo;				// 결과 (NOERROR : 성공 / 그외 : 실패)
#ifdef PRE_ADD_BEGINNERGUILD
	bool bGraduateBeginnerGuild;		//초보자길드졸업
#endif		//#ifdef PRE_ADD_BEGINNERGUILD
};

// SC_GUILD / SC_EXILEGUILDMEMB					// 길드원 추방 결과
struct SCExileGuildMember
{
	UINT nAccountDBID;		// 추방한 계정 DBID
	INT64 nCharacterDBID;		// 추방한 캐릭터 DBID
	TGuildUID GuildUID;		// 길드 UID (실패일 경우 무시됨)
	int iErrNo;				// 결과 (NOERROR : 성공 / 그외 : 실패)
};

// SC_GUILD / SC_CHANGEGUILDINFO				// 길드 정보 변경 결과
struct SCChangeGuildInfo
{
	UINT nAccountDBID;				// 요청한 계정 DBID (일반적인 경우 중요하지 않고 참조 용도로만 사용)
	INT64 nCharacterDBID;				// 요청한 캐릭터 DBID (일반적인 경우 중요하지 않고 참조 용도로만 사용)
	BYTE btGuildUpdate;		// 길드 정보 변경 타입 (eGuildUpdateType)
	int Int1;
	int Int2;
	int Int3;
	int Int4;
	INT64 Int64;
	WCHAR Text[GUILDHISTORYTEXT_MAX+1];
	TGuildUID GuildUID;		// 길드 UID (실패일 경우 무시됨)
	int iErrNo;				// 결과 (NOERROR : 성공 / 그외 : 실패)
};

// SC_GUILD / SC_CHANGEGUILDMEMBINFO			// 길드원 정보 변경 결과
struct SCChangeGuildMemberInfo
{
	UINT nReqAccountDBID;	// 요청한 계정 DBID
	INT64 nReqCharacterDBID;	// 요청한 캐릭터 DBID
	UINT nChgAccountDBID;	// 변경될 계정 DBID (길드장이 길드원 정보를 변경하는 경우에는 요청자와 달라짐)
	INT64 nChgCharacterDBID;	// 변경될 캐릭터 DBID (길드장이 길드원 정보를 변경하는 경우에는 요청자와 달라짐)
	BYTE btGuildMemberUpdate;		// 길드원 정보 변경 타입 (eGuildMemberUpdateType)
	int Int1;
	int Int2;
	INT64 Int64;
	WCHAR Text[GUILDPARAMTEXT_MAX+1];
	TGuildUID GuildUID;		// 길드 UID (실패일 경우 무시됨)
	int iErrNo;				// 결과 (NOERROR : 성공 / 그외 : 실패)
};

// SC_GUILD / SC_GETGUILDHISTORYLIST			// 길드 히스토리 목록 응답
struct SCGetGuildHistoryList
{
	TGuildUID GuildUID;			// 길드 UID
	INT64 biIndex;	// 길드 히스토리 목록 인덱스 (0 이면 가장 최근 기록부터 확인 ?)
	bool bDirection;				// 목록 얻을 방향 (0:인덱스 이전 / 1:인덱스 이후)
	int nCurCount;				// 길드 히스토리 최초 인덱스 → 입력 인덱스 까지의 개수
	int nTotCount;				// 길드 히스토리 전체 개수
	int iErrNo;					// 결과 (NOERROR : 성공 / 그외 : 실패)
	int nCount;					// 길드 히스토리 목록 개수 (실패일 경우 무시됨)
	TGuildHistory HistoryList[GUILDHISTORYLIST_MAX];	// 길드 히스토리 목록 (실패일 경우 무시됨)
};

// SC_GUILD / SC_GETGUILDINFO					// 길드 정보 응답
struct SCGetGuildInfo
{
	int iErrNo;	// 결과 (NOERROR : 성공 / 그외 : 실패)
 	TGuild Info;	// 길드 정보 (실패일 경우 무시됨)	
	bool bNotRecruitMember;		// 길드모집 게시판 가입승인 가능상태 체크( false:가입가능 true:가입불가)
};
struct SCGetGuildMember
{	
	int iErrNo;	// 결과 (NOERROR : 성공 / 그외 : 실패)
	int nPage;	// 길드원 페이지 정보
	bool bEndMember;	// 마지막인지 체크
	int nCount;	// 길드원 정보 개수 (실패일 경우 무시됨)
	TGuildMember MemberList[SENDGUILDMEMBER_MAX];	// 길드원 정보

};

// SC_GUILD / SC_CHANGEGUILDSELFVIEW
struct SCChangeGuildSelfView
{
	UINT nSessionID;				// 세션 ID
	TGuildSelfView GuildSelfView;	// 길드 시각정보 (개인)
};

// SC_GUILD / SC_GUILDMEMBLOGINLIST
struct SCGuildMemberLoginList
{
	TGuildUID GuildUID;			// 길드 UID
	int nCount;					// 로그인한 길드원 수
	INT64 List[GUILDSIZE_MAX];	// 로그인한 길드원 목록
};


struct SCOpenGuildWare				// SC_GUILD	/ SC_OPEN_GUILDWARE
{
	int					nError;							// 에러값처리
	int					nResult;						// 0:리스트 포함 1:리스트미포함
	short				wWareSize;						// 창고사이즈
	
	USHORT				nTakeWareItemCount;				// 창고아이템을 꺼낸 횟수 (하루기준)
	INT64				nWithdrawCoin;					// 금일 골드를 꺼낸 금액
	INT64				nGuildCoin;						// 길드코인

	__time64_t			VersionDate;					// 창고 버젼

	BYTE				cWareCount;						// 웨어아이템카운트
	TItemInfo			WareList[GUILD_WAREHOUSE_MAX];	// 웨어리스트
};

struct CSGetGuildWareHistory		// CS_GUILD / CS_GET_GUILDWARE_HISTORY		// 길드창고 사용 히스토리
{
	int					nIndex;							// 히스토리 요청 인덱스
};

struct SCGetGuildWareHistory		// SC_GUILD / SC_GET_GUILDWARE_HISTORY
{
	int					nCurrCount;
	int					nTotalCount;
	int					nErrNo;
	int					nCount;
	TGuildWareHistory	HistoryList[GUILD_WAREHOUSE_HISTORYLIST_MAX];// 히스토리 리스트
};

// SC_GUILD / SC_GUILDMEMBLOGINLIST
struct SCChangeGuildName
{	
	WCHAR wszGuildName[GUILDNAME_MAX];		// 길드 이름 (일단 길드 창설 이후에는 이름은 변경되지 않는것을 전제로 함)
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
	int nError;				// 결과 (ERROR_NONE : 성공 / ERROR_GUILD_DAILYLIMIT : 실패)
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
	UINT nSessionID;							// 상대방 세션아이디
	WCHAR wszMasterName[NAMELENMAX];			// 길드마스터 캐릭터 이름
	WCHAR wszGuildName[GUILDNAME_MAX];		// 길드명
	int nGuildLevel;							// 길드 레벨
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
	short wScheduleID;		// 차수
	char cEventStep;		// 이벤트 스텝: GUILDWAR_STEP_NONE ~ GUILDWAR_STEP_END
	char cEventType;		// GUILDWAR_EVENT_START(시작) / GUILDWAR_EVENT_END(끝)
};

// SC_GUILD / SC_GUILDWAR_FESTIVAL_POINT
struct SCGuildWarFestivalPoint
{
	INT64 biGuildWarFestivalPoint;
};

// SC_GUILD / SC_GUILDWAR_TRIAL_RESULT_OPEN
struct SGuildWarRankingInfo	
{
	short wRanking;							// 길드 순위
	WCHAR wszGuildName[GUILDNAME_MAX];	// 길드 이름
	WCHAR wszGuildMasterName[NAMELENMAX];	// 길드장 이름
	short wCurGuildUserCount;				// 현재 길드 인원
	short wMaxGuildUserCount;				// 최대 길드 인원
	int nTotalPoint;						// 획득한 총점
	UINT nGuildDBID;						// 길드 DBID
};

struct SGuildWarMissionGuildRankingInfo	//각 부문별 길드 순위
{
	WCHAR wszGuildName[GUILDNAME_MAX];	// 길드 이름
	WCHAR wszGuildMasterName[NAMELENMAX];	// 길드장 이름
	int nTotalPoint;						// 획득한 총점
};

struct SMyGuildWarMissionGuildRankingInfo // 우리 길드의 부문별 순위
{
	short wRanking;							// 부문별 순위
	int nTotalPoint;						// 획득한 총점
};

struct SGuildWarMissionRankingInfo
{
	WCHAR wszCharName[NAMELENMAX];			// 1위 이름
	WCHAR wszGuildName[GUILDNAME_MAX];	// 길드 이름
	int nTotalPoint;						// 획득한 총점
};

struct SMyGuildWarMissionRankingInfo
{
	short wRanking;							// 순위
	int nTotalPoint;						// 획득한 총점
};

struct SGuildWarDailyAward
{
	WCHAR wszCharName[NAMELENMAX];			// 수상자 캐릭명
	TGuildUID nGuildUID;					// 수상한 길드의 UID;
};

struct SCGuildWarTrialResult
{
	// 길드 축제 결과
	int nBluePoint;			// 최종 청팀 점수
	int nRedPoint;			// 최종 홍팀 점수
	bool bGuildWar;			// 길드전 참여 여부(요게 false면 밑에 개인,우리길드 정보는 패킷에 포함되지 않습니다.)	
	bool bReward;			// 보상받기 여부
	// 본선 진출 길드
	SGuildWarRankingInfo	sGuildWarRankingInfo[GUILDWAR_FINALS_TEAM_MAX];	

	// 부문별 길드 순위(전체)
	SGuildWarMissionGuildRankingInfo sGuildWarMissionGuildRankingInfo[GUILDWAR_RANKINGTYPE_MAX];	// 전체 길드 순위
	// 부문별 개인 순위(전체)
	SGuildWarMissionRankingInfo sGuildWarMissionRankingInfo[GUILDWAR_RANKINGTYPE_MAX];			// 개인별 순위
	// 특별상
	// 토벌상과 같은건 개인순위의 넘을 그대로 보여주시면 됩니다.
	SGuildWarDailyAward sGuildWarDailyAward[GUILDWAR_DAILY_AWARD_MAX];							// 일자별 시상	

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// 내정보, 우리길드 정보들(만약 길드가 없거나 길드전에 참가하지 않는넘이면 이 정보는 보내지 않습니다.)
	int nFestivalPoint;		// 기본 점수 
	int nFestivalPointAdd;	// 수상을 통한 +된 점수
	// 우리 길드 순위 정보 (전체순위)
	SMyGuildWarMissionGuildRankingInfo sMyGuildWarRankingInfo;
	// 부문별 길드 순위(우리길드)
	SMyGuildWarMissionGuildRankingInfo sMyGuildWarMissionGuildRankingInfo[GUILDWAR_RANKINGTYPE_MAX]; // 우리 길드 순위	
	// 부문별 내 순위
	SMyGuildWarMissionRankingInfo sMyGuildWarMissionRaningInfo[GUILDWAR_RANKINGTYPE_MAX];			 // 내 순위	
};

// SC_GUILD / SC_GUILDWAR_STATUS_OPEN
struct SCGuildWarStatus
{	
	int nBluePoint;			// 청팀 점수	
	int nRedPoint;			// 홍팀 점수	
	int nMyPoint;			// 내 점수
	int nMyGuildPoint;		// 길드 현재 점수
	int nRankingCount;		// 랭킹 카운트..요넘 숫자만큼 날라 갑니다.
	SGuildWarRankingInfo	sGuildWarPointTrialRanking[GUILDWAR_TRIAL_POINT_TEAM_MAX];
};
// CS_GUILD / CS_GUILDWAR_VOTE
struct CSGuildWarVote
{
	TGuildUID GuildUID;		// 투표할 길드 UID (월드 ID + 길드 DBID)
};
// SC_GUILD / SC_GUILDWAR_VOTE
struct SCGuildWarVote
{
	int nRetCode;			// 투표 결과
	int nAddFestivalPoint;	// 얻은 축제 포인트
};
// SC_GUILD / SC_GUILDWAR_TOURNAMENT_INFO_OPEN
struct SGuildTournamentInfo
{
	TGuildUID GuildUID;						// 길드 UID (월드 ID + 길드 DBID)
	WCHAR wszGuildName[GUILDNAME_MAX];	// 길드 이름
	char cMatchTypeCode;					// 마지막 차수 1=결승, 2=4강, 3=8강, 4=16강
	bool bWin;								// 이겼는지 졌는지 여부	
	short wGuildMark;						// 길드마크 문양
	short wGuildMarkBG;						// 길드마크 배경
	short wGuildMarkBorder;					// 길드마크 테두리
};
struct SCGuildTournamentInfo
{
	SGuildTournamentInfo TournamentInfo[GUILDWAR_FINALS_TEAM_MAX];
	char cMatchTypeCode;					// 현재 진행중인 차수 1=결승, 2=4강, 3=8강, 4=16강
	__time64_t tStartTime;					// 이게 0이면 현재 진행중이고 값이 있으면 이때 시작하는 것입니다.
	bool bPopularityVote;					// 인기투표 활성화, 비활성화
};
// SC_GUILD / SC_GUILDWAR_VOTE_TOP
struct SCGuildWarVoteTop
{
	WCHAR wszGuildName[GUILDNAME_MAX];	// 길드 이름
	int nVoteCount;							// 총 득표한 수
};
// SC_GUILD / SC_GUILDWAR_COMPENSATION
struct SCGuildWarCompensation
{
	int nRetCode;				// 보상 받기 결과
};
// SC_GUILD / SC_GUILDWAR_PRE_WIN
struct SCGuildWarPreWin
{
	bool bPreWin;				// true면 우리길드가 우승길드임, false면 우승길드 해제..
};
// CS_GUILD / CS_GUILDWAR_WIN_SKILL
struct CSGuildWarWinSkill
{
	int nSkillID;				// 시전 스킬ID
};
// SC_GUILD / SC_GUILDWAR_WIN_SKILL
struct SCGuildWarWinSkill
{
	int nRetCode;
	DWORD dwCoolTime;			// 남은 쿨타임
};
// SC_GUILD / SC_GUILDWAR_USER_WINSKILL
#if defined(_GAMESERVER) || defined(_VILLAGESERVER) || defined(_CLIENT)
struct SCGuildWarUserWinSkill
{
	TGuildUID GuildUID;			// 스킬을 쓴넘의 GuildUID
	EtVector3 vPos;				// 스킬을 쓴넘의 좌표.
	int nSkillID;				// 스킬ID
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
	// 전체 길드전 스케쥴
	SGuildWarTime tGuildWarTime[GUILDWAR_STEP_END-1]; // eGuildWarStepType에서 -1씩 해주세요..
	// 본선 진행 차수
	SGuildWarTime tFinalPartTime[GUILDWAR_FINALPART_MAX-1]; //eGuildFinalPart에서 -1씩 해주세요..
};

// SC_GUILD / SC_GUILDWAR_TOURNAMENT_WIN
struct SCGuildWarTournamentWin
{
	// 승리 차수
	char cMatchTypeCode;					// 현재 진행중인 차수 1=결승, 2=4강, 3=8강, 4=16강
	// 승리한 길드명
	WCHAR wszGuildName[GUILDNAME_MAX];	// 길드 이름
};

//--------------------------------------------------------------------------------------------------
//	PVP (CS_PVP, SC_PVP)
//--------------------------------------------------------------------------------------------------

//blondy 
//클라이언트에서 쓸 컨테이너
struct sRoomInfo
{
	BYTE	cGameMode;		// PvPCommon::GameMode
	BYTE	cVSMode;		// PvPCommon::VSMode
	WCHAR	RoomName[PvPCommon::TxtMax::RoomName];
	BYTE	cRoomState;		// 방상태
	BYTE    cPlayerNum;		//현재인원
	BYTE    cMaxPlayerNum;	//최대인원
	UINT    nMapIndex;      //맵인덱스
	UINT    nPVPIndex;      //방인덱스
	bool	bInGameJoin;	//난입가능
    BYTE    cMinLevel; 
	BYTE    cMaxLevel;    
	UINT    uiObjective;      //오브젝티브
	bool	bDropItem;		//드랍아이템 허용
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
	BYTE	cRoomSortType;		// PvPCopmmon::RoomSortType 참고
	BYTE	cIsAscend;			// 오름차순:1 내림차순:0
	UINT	uiSortData;			// cRoomSortType 에 따라 필요한 부가 정보
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
	bool											bIsAscend;	// 오름차순:true 내림차순:false
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
	bool	bIsObserver;		// 관전하기
	UINT nGuildDBID;			//길드전 참전일 경우
	BYTE	cRoomPWLen;
	WCHAR	wszRoomPW[PvPCommon::TxtMax::RoomPW];
};

struct SCPVP_JOINROOM
{
	short					nRetCode;
	PvPCommon::RoomInfo		RoomInfo;				// 룸정보
	BYTE					cUserCount;				// 유저수
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
	USHORT	unCheck;		// PvPCommon::StartCheat 참고
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
	UINT	uiWinTeam;		// PvPCommon::Team 참고
	PvPCommon::FinishReason::eCode	Reason;		// PvPCommon::FinishReason 참고
	UINT	uiATeamScore;
	UINT	uiBTeamScore;
#if defined(PRE_ADD_PVP_TOURNAMENT)
	UINT	uiWinSessionID;
#endif
};

struct SCPVP_FINISH_PVPROUND
{
	UINT	uiWinTeam;		// PvPCommon::Team 참고
	PvPCommon::FinishReason::eCode	Reason;		// PvPCommon::FinishReason 참고
	UINT	uiATeamScore;	// RoundMode는 GameModeScore를 서버에서 알려준다.
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
	bool bIsGuildWar;		//이게 트루면 나가기만 활성화 나머지는 전부 막힘
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
	UINT	uiAddLadderXPScore;	//래더게임에서 제공되는 경험치
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
	UINT	uiSessionID;		// 1위한 넘의 SessionID
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
		int iSec;	// -1 인경우 알수없음
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
	UINT nSessionID;      // 죽은 대상
	UINT nKillerSessionID;// 죽인 대상
};

struct SCPVP_CHANGE_CHANNEL
{
	BYTE cType;
	int nRetCode;
};

#if defined(PRE_ADD_QUICK_PVP)
// CS_PVP / CS_QUICKPVP_INVITE					// 결투신청 요청
struct CSQuickPvPInvite
{
	UINT nReceiverSessionID;				// 초대할 유저
};

// CS_PVP / CS_QUICKPVP_RESULT				// 결투신청 응답
struct CSQuickPvPResult
{		
	bool bAccept;							// 수락 여부
	UINT nSenderSessionID;					// 초대한 유저
};

// SC_PVP / SC_QUICKPVP_INVITE			// 결투신청 요청	
struct SCQuickPvPInvite
{
	UINT nSenderSessionID;				// 초대한 유저
};

// SC_PVP / SC_QUICKPVP_RESULT			// 결투신청 응답
struct SCQuickPvPResult
{
	int	nResult;						// 신청 결과
};
#endif //#if defined(PRE_ADD_QUICK_PVP)
#if defined(PRE_ADD_PVP_VILLAGE_ACCESS)
// SC_PVP / SC_PVP_LIST_OPEN_UI
struct SCPvPListOpenUI
{
	int nResult;						// UI오픈 결과
	int nLimitLevel;					// 제한 레벨
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
	bool bAsign;		//true면 임명 false면 해임
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
	int nState;		//PvPCommon::OccupationSystemState 참조
};

struct SCPvPOccupationState		//SC_PVP / SC_PVP_OCCUPATIONSTATE
{
	BYTE cCount;
	PvPCommon::OccupationStateInfo Info[PARTYMAX];
};

struct SCPvPOccupationTeamState		//SC_PVP / SC_PVP_OCCUPATION_TEAMSTATE
{
	int nTeam;
	int nResource;			//현재자원
	int nResourceSum;		//획득총량
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
	char	cTournamentStep;		// 토너먼트 스텝(몇강?) // PvPCommon::Tournament::eStep
	bool	bWin;					// 이겼는지 졌는지.
	BYTE	cJob;					// 직업
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
	char	cTournamentStep;		// 토너먼트 스텝(몇강?) // PvPCommon::Tournament::eStep
	UINT	uiWinSessionID;			// 부전승으로 이긴넘의 SessionID
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
	UINT	uiSessionID[4];			// 1~3(3위 2명)위까지 토너먼트 순위 uiSeesionID가 없는 순위는 나간 아이임..
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
	int nItemID;	// TimeEventTable의 ItemID
	int nDuration;	// 이벤트 시작시간으로부터 진행된 시간(초)
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

struct SCGameOptionDisplayProfile	// 다른 유저에게 프로필 내용을 보여줄때 사용한다. (채팅방입장, 파티입장 등...)
{
	int			nSessionID;		// 프로필 주인
	TProfile	sProfile;		// 프로필 내용
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
	bool bIsSetSecondAuthPW;	// 2차 인증 비밀번호 설정 여부
	bool bIsSetSecondAuthLock;	// 2차 인증 계정 Lock 여부
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

struct SCCashShopOpen			// SC_CASHSHOP / SC_SHOPOPEN,			// 샵 열기
{
#if defined(_US)
	int nNxAPrepaid;
	int nNxACredit;
#endif	// #if defined(_US)
	UINT nCashAmount;	// 캐시금액
	int nReserveAmount;	// 적립금
#ifdef PRE_ADD_NEW_MONEY_SEED
	INT64 nSeedAmount;	// 귀속화폐 시드
#endif // PRE_ADD_NEW_MONEY_SEED
	int nRet;
	WCHAR wszAccountName[IDLENMAX];
};

struct SCCashShopClose			// SC_CASHSHOP / SC_SHOPCLOSE,			// 샵 닫기
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
	char cSlotIndex;	// 카트인덱스
	int nItemSN;
	int nItemID;	// Commodity에 ItemID01-05사이 아이템아이디값
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
	__time64_t tPaymentDate;	// 결제(구매)일
	TPaymentItem ItemInfo;	// 아이템 정보
};

struct TPaymentPackageItemInfo
{	
	INT64 biDBID;				// (biPurchaseOrderDetailID)
	int	 nPackageSN;			// 패키지 씨리얼
	__time64_t tPaymentDate;	// 결제(구매)일
	TPaymentItem ItemInfoList[PACKAGEITEMMAX];		// 가변적으로 처리해야 하는데..가변에 가변은..
};

struct TPaymentItemInfoEx		// 서버만 쓰는 구조체
{
	UINT uiOrderNo;				// Nexon 
	int nPrice;					// 상품 가격(환불용)
	TPaymentItemInfo PaymentItemInfo;
};

struct TPaymentPackageItemInfoEx  // 서버만 쓰는 구조체
{
	UINT uiOrderNo;
	int nPrice;					// 상품 가격(환불용)
	TPaymentPackageItemInfo PaymentPackageItemInfo;
};

struct CSCashShopBuy			// CS_CASHSHOP / CS_BUY,			// 구매하기
{
	char cType;		// 구매 카트 타입 (3,7: PREVIEWCARTLISTMAX, 나머지: CARTLISTMAX, 50 + eInstantCashShopBuyType: 간편결재)	
#if defined(PRE_ADD_CASH_REFUND)
	bool bMoveCashInven;	// 캐쉬인벤으로 바로 옮기기 플래그
#endif
	char cPaymentRules;	// Cash::PaymentRules
#if defined(PRE_ADD_SALE_COUPON)
	int nSaleCouponSN;				// Sale 쿠폰 CashCommodity SN;
	INT64 biSaleCouponSerial;		// Sale 쿠폰을 사용한 구매일경우(무조건 한개만 구매가 되야함)
#endif // #if defined(PRE_ADD_SALE_COUPON)
	char cCount;
	TCashShopInfo BuyList[PREVIEWCARTLISTMAX];
};

struct SCCashShopBuy			// SC_CASHSHOP / SC_BUY,			// 구매하기
{
#if defined(_US)
	int nNxAPrepaid;
	int nNxACredit;
#endif	// #if defined(_US)
	UINT nCashAmount;	// 캐시금액
	int nReserveAmount;	// 적립금
#ifdef PRE_ADD_NEW_MONEY_SEED
	INT64 nSeedAmount;	// 귀속화폐 시드
#endif // PRE_ADD_NEW_MONEY_SEED
	int nRet;
	char cType;	// (3,7: PREVIEWCARTLISTMAX, 나머지: CARTLISTMAX)
#if defined(PRE_ADD_CASH_REFUND)
	TPaymentItemInfo ItemList[PREVIEWCARTLISTMAX];	//그냥 채워서 갑니다.biDBID가 0이면 무시하시면 될듯.
#endif
	char cCount;
	TCashShopInfo BuyList[PREVIEWCARTLISTMAX];
};

struct CSCashShopPackageBuy		// CS_CASHSHOP / CS_PACKAGEBUY
{
	int	 nPackageSN;
#if defined(PRE_ADD_CASH_REFUND)
	bool bMoveCashInven;	// 캐쉬인벤으로 바로 옮기기 플래그
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
	int nCashAmount;	// 캐시금액
	int nReserveAmount;	// 적립금
#ifdef PRE_ADD_NEW_MONEY_SEED
	INT64 nSeedAmount;	// 귀속화폐 시드
#endif // PRE_ADD_NEW_MONEY_SEED
	int nRet;
#if defined(PRE_ADD_CASH_REFUND)
	TPaymentPackageItemInfo PackageItem;	// 패키지는 무조건 하나.
#endif
};

struct CSCashShopCheckReceiver	// CS_CASHSHOP / CS_CHECKRECEIVER,	// 받는이 체크 (선물할때 필요)
{
	WCHAR wszToCharacterName[NAMELENMAX];	// 받는이
};

struct SCCashShopCheckReceiver	// SC_CASHSHOP / SC_CHECKRECEIVER,	// 받는이 체크 (선물할때 필요)
{
	BYTE cLevel;
	BYTE cJob;
	int nRet;
};

struct CSCashShopGift			// CS_CASHSHOP / CS_GIFT,			// 선물하기, 조르기
{
	WCHAR wszToCharacterName[NAMELENMAX];	// 받는이
	WCHAR wszMessage[GIFTMESSAGEMAX];	// 메모
#if defined( _US ) || defined( PRE_ADD_NEW_MONEY_SEED )
	char cPaymentRules;	// Cash::PaymentRules
#endif	// _US or PRE_ADD_NEW_MONEY_SEED
#if defined(PRE_ADD_CADGE_CASH)
	int nMailDBID;
#endif	// #if defined(PRE_ADD_CADGE_CASH)
	char cType;		// (3,7: PREVIEWCARTLISTMAX, 나머지: CARTLISTMAX)
	char cCount;
	TCashShopInfo GiftList[PREVIEWCARTLISTMAX];
};

struct SCCashShopGift			// SC_CASHSHOP / SC_GIFT,			// 선물하기
{
#if defined(_US)
	int nNxAPrepaid;
	int nNxACredit;
#endif	// #if defined(_US)
	int nCashAmount;	// 캐시금액
	int nReserveAmount;	// 적립금
#ifdef PRE_ADD_NEW_MONEY_SEED
	INT64 nSeedAmount;	// 귀속화폐 시드
#endif // PRE_ADD_NEW_MONEY_SEED
	int nRet;
	char cType;		// (3,7: PREVIEWCARTLISTMAX, 나머지: CARTLISTMAX)
	char cCount;
	TCashShopInfo GiftList[PREVIEWCARTLISTMAX];
};

struct CSCashShopPackageGift	// CS_CASHSHOP / CS_PACKAGEGIFT
{
	WCHAR wszToCharacterName[NAMELENMAX];	// 받는이
	WCHAR wszMessage[GIFTMESSAGEMAX];	// 메모
#if defined( _US ) || defined( PRE_ADD_NEW_MONEY_SEED )
	char cPaymentRules;	// Cash::PaymentRules
#endif	// _US or PRE_ADD_NEW_MONEY_SEED
#if defined(PRE_ADD_CADGE_CASH)
	int nMailDBID;		// 우편에서 조르기 누를때만 값 세팅 (캐쉬샵에서 누를땐 0)
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
	int nCashAmount;	// 캐시금액
	int nReserveAmount;	// 적립금
#ifdef PRE_ADD_NEW_MONEY_SEED
	INT64 nSeedAmount;	// 귀속화폐 시드
#endif // PRE_ADD_NEW_MONEY_SEED
	int nRet;
};

typedef CSCashShopPackageGift CSCashShopCadge;	// CS_CASHSHOP / CS_CADGE

struct SCCashShopCadge			// SC_CASHSHOP / SC_CADGE,		// 조르기
{
	int nRet;
};

// CS_CASHSHOP / CS_GIFTLIST,		// 선물함 (받은선물) 요청

struct TGiftInfo
{
	char cPayMethodCode;				// 선물 종류 (db:PayMethodCode)
	INT64 nGiftDBID;					// (db:PurchaseOrderID)
	WCHAR wszSenderName[MAILNAMELENMAX];	// 보낸사람
	int nItemSN;
	int nItemID;						// 패키지일 경우엔 이값 무시(?)
	char cItemOption;
	__time64_t tOrderDate;
	WCHAR wszMessage[GIFTMESSAGEMAX];	// 메모
	__time64_t tGiftExpireDate;			// 선물함 저장기간
#if defined(PRE_ADD_GIFT_RETURN)
	bool bGiftReturn;					// 선물 반송여부
#endif
	bool bNewFlag;
	int nPaidCashAmount;
};

struct SCCashShopGiftList		// SC_CASHSHOP / SC_GIFTLIST,		// 선물함 (받은선물)
{
	int nRet;
	char cCount;
	TGiftInfo GiftInfo[GIFTBOXLISTMAX];
};

struct TReceiveGiftData
{
	INT64 nGiftDBID;
	char cPayMethodCode;						// 선물 종류 (db:PayMethodCode)
	WCHAR wszEmoticonTitle[MAILTITLELENMAX];
	WCHAR wszReplyMessage[GIFTMESSAGEMAX];	// 메모
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
	bool bNew;	// 새 선물인지 아닌지 (깜박깜박)
	int nGiftCount;
};

struct CSVIPBuy				// CS_CASHSHOP / CS_VIPBUY
{
	int nItemSN;
};

struct SCVIPBuy				// CS_CASHSHOP / SC_VIPBUY,						// vip 정보날려주기
{
	UINT nCashAmount;	// 캐시금액
	int nReserveAmount;	// 적립금
#ifdef PRE_ADD_NEW_MONEY_SEED
	INT64 nSeedAmount;	// 귀속화폐 시드
#endif // PRE_ADD_NEW_MONEY_SEED
	int nRet;
	int nVIPPoint;		// 포인트
	__time64_t tVIPExpirationDate;	// 만료일
	bool bAutoPay;		// 자동결제 유무
};

struct CSVIPGift			// CS_CASHSHOP / CS_VIPGIFT
{
	WCHAR wszToCharacterName[NAMELENMAX];	// 받는이
	WCHAR wszMessage[GIFTMESSAGEMAX];	// 메모
	int nItemSN;
};

struct SCVIPGift			// CS_CASHSHOP / SC_VIPGIFT,						// vip 정보날려주기
{
	UINT nCashAmount;	// 캐시금액
	int nReserveAmount;	// 적립금
#ifdef PRE_ADD_NEW_MONEY_SEED
	INT64 nSeedAmount;	// 귀속화폐 시드
#endif // PRE_ADD_NEW_MONEY_SEED
	int nRet;
};

struct SCVIPInfo			// SC_CASHSHOP / SC_VIPINFO
{
	bool bVIP;
	int nVIPPoint;		// 포인트
	__time64_t tVIPExpirationDate;	// 만료일
	bool bAutoPay;		// 자동결제 유무
};

struct SCSaleAbortList		// SC_CASHSHOP / SC_SALEABORTLIST
{
	BYTE cCount;
	int nAbortList[SALEABORTLISTMAX];
};

#if defined(PRE_ADD_CASH_REFUND)
struct SCPaymentList		// SC_CASHSHOP / SC_PAYMENT_LIST
{
	int nTotalPaymentCount;		// 전체 결재인벤 갯수(일반)
	BYTE cInvenCount;			// 지금 날라가는 인벤 갯수
	TPaymentItemInfo ItemList[CASHINVENTORYMAX];
};

struct SCPaymentPackageList	// SC_CASHSHOP / SC_PAYMENT_PACKAGELIST
{
	int nTotalPaymentCount;		// 전체 결재인벤 갯수(패키지)
	BYTE cInvenCount;			// 지금 날라가는 인벤 갯수
	TPaymentPackageItemInfo ItemList[CASHINVENTORYMAX];		// 여기서 버리는 바이트 좀 생길듯..
};

struct CSMoveCashInven		// CS_CASHSHOP / CS_MOVE_CASHINVEN
{
	BYTE cItemType;			// 1..일반, 2..패키지
	INT64 biDBID;			// (biPurchaseOrderDetailID)
};

struct SCMoveCashInven		// SC_CASHSHOP / SC_MOVE_CASHINVEN
{
	INT64 biDBID;			// (biPurchaseOrderDetailID)	
	int nRet;				// 결과
	int nReserveAmount;		// 페탈 적립금
#if defined( PRE_ADD_NEW_MONEY_SEED )
	INT64 nSeedAmount;		// 시드
#endif
};

struct CSCashRefund			// CS_CASHSHOP / CS_CASH_REFUND
{
	BYTE cItemType;			// 1..일반, 2..패키지
	INT64 biDBID;			// (biPurchaseOrderDetailID)
};

struct SCCashRefund			// SC_CASHSHOP / SC_CASH_REFUND
{
	INT64 biDBID;			// (biPurchaseOrderDetailID)
	int nRet;				// 결과
	int nCashAmount;		// 성공일시에 현재 캐쉬
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
struct SCGameQuitRewardCheckRes	//SC_ETC / SC_GAMEQUIT_REWARDCHECK_RES / 유저(클라이언트)가 받을 보상 종류 전송
{
	GameQuitReward::RewardType::eType eRewardType;	// 접속 종료시 보상 종류
};

struct CSGameQuitRewardReq		//CS_ETC / CS_GAMEQUIT_REWARD_REQ / 보상 요청 패킷
{
	GameQuitReward::RewardType::eType eRewardType;	// SCGameQuitRewardCheckRes에서 보내준 보상 종류(타입)
};

struct SCGameQuitRewardRes		//SC_ETC / SC_GAMEQUIT_REWARD_REQ / 보상 받기 결과
{
	int nRet;	// ERROR_NONE = 보상 받기 성공 / 에러인 경우 ERROR_ITEM_NOTFOUD(dnt 테이블에서 아이템 찾는데 실패한경우,  보상 받을 수 없는 유저인 경우)
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
	WCHAR	wszFarmName[Farm::Max::FARMNAMELEN];		//혹시나해서 일단 넣어놓음
};

struct TFarmItemFromDB:public TFarmItemPartial			// DB에서 얻어온 농장자체정보
{
	int		iFarmMaxUser;
	bool	bEnableFlag;
	bool bStartActivate;		//오픈시시작여부
};

struct TFarmItem:public TFarmItemFromDB					// 농장자체정보
{
	int		iFarmCurUser;
	bool	bActivate;				//사용여부
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
	bool bRemoveItem;			// 삭제해야할 아이템인지 체크(스킬로 인한 아이템인 경우는 지울 아이템이 없기 떄문에 체크
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
	BYTE					cNeedItemInvenIndex;	// 수확시 필요한 아이템 있는 인벤토리 인덱스
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
	BYTE					cWaterItemInvenIndex;	// 물이 있는 인벤토리 인덱스
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
	int nFishingRodInvenIdx;		//사용할 낚시대의 인벤인덱스	(낚시대가 복수개가 생기고 포인트에 낚시대 제약등이 생길경우 커버하기위함)
	int nFishingBaitInvenIdx;		//사용할 미끼의 인벤인덱스
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
	int nFishingRodInvenIdx;		//사용할 낚시대의 인벤인덱스	(낚시대가 복수개가 생기고 포인트에 낚시대 제약등이 생길경우 커버하기위함)
	int nFishingBaitInvenIdx;		//사용할 미끼의 인벤인덱스
#endif		//#ifdef PRE_ADD_CASHFISHINGITEM
};

struct SCFishingReady		//SC_READYFISHING
{
	int nRetCode;
};

struct CSCastBait			//CS_CASTBAIT
{
	Fishing::Cast::eCast eCastType;					//Fishing::Cast참조
#ifdef PRE_ADD_CASHFISHINGITEM
	TFishingToolInfo ToolInfo;
#else		//#ifdef PRE_ADD_CASHFISHINGITEM
	int nFishingRodInvenIdx;		//사용할 낚시대의 인벤인덱스	(낚시대가 복수개가 생기고 포인트에 낚시대 제약등이 생길경우 커버하기위함)
	int nFishingBaitInvenIdx;		//사용할 미끼의 인벤인덱스
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
	int nRetCode;			//-1이면 획득실패, 0이면 성공 nRewardItemID가 획득한 아이디 0보다 큰값이면 에러
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

struct TChatRoomView	// 채팅방 시각정보
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
	WCHAR		wszPassword[CHATROOMPASSWORDMAX + 1];	// 패스워드
};

struct CSChatRoomChangeRoomOption
{
	TChatRoomView	ChatRoomView;

	WCHAR			wszPassword[CHATROOMPASSWORDMAX + 1];
	USHORT			nRoomAllow;
};

struct CSChatRoomKickUser
{
	int			nKickUserSessionID;		// 쫒아낼 캐릭터의 SessionID
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
	int				UserSessionIDs[CHATROOMMAX];	// 참여자 리스트
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
	int				nSessionID;		// 채팅방을 가지고 있는 캐릭터
	TChatRoomView	ChatRoomView;	// 채팅방 간략 정보 - ChatRoomID가 0이면 방이 삭제되는 경우이다.
};

struct SCROOM_SYNC_CHATROOMINFO
{
	UINT			nLeaderID;		// 채팅방을 가지고 있는 캐릭터
	TChatRoomView	ChatRoomView;	// 채팅방 간략 정보 - ChatRoomID가 0이면 방이 삭제되는 경우이다.
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
	int nPresentID;		// npc 선물 테이블에 있는 itemid
	int nPresentCount;  // 선물 갯수
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
		bool bRefresh;	// 리스트 갱신여부
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
		BYTE	cJob;		// 0:모두
		BYTE	cGender;	// 0:모두
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
		bool	bRegister;	// 등록 여부
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
		bool	bIsTransactor;	// 행위 당사자인지 구분 Flag
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
		bool	bIsMaster;		// 제자추방:true 스승삭제:false
	};

	struct SCLeave
	{
		int		iRet;
		bool	bIsDelPupil;			// true:제자삭제 false:스승삭제
		int		iPenaltyRespectPoint;	// bIsDelPupil==true 인경우 감소할 존경수치값(양수)
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
		bool	bIsConfirm;					// true:소환확인패킷
		WCHAR	wszCharName[NAMELENMAX];	// bIsConfirm==true : 제자이름 bIsConfirm==false : 스승이름
	};

	struct CSBreakInto
	{
		int		iRet;
		WCHAR	wszPupilCharName[NAMELENMAX];
	};
	struct SCJoinComfirm
	{
		BYTE cLevel;				// 제자 레벨
		BYTE cJob;					// 제자 직업
		WCHAR wszPupilCharName[NAMELENMAX];		// 제자 이름
	};
	struct CSJoinComfirm
	{
		bool	bIsAccept;
		WCHAR	wszPupilCharName[NAMELENMAX];	// 제자 이름
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
		BYTE	cInvenIndex;			// 일반인벤일때만 index넣어주면 됨 (캐쉬는 참조하지않음)
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
		int		iExp;		// 최종값(assign해야함)
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

/* 절친 상태 확인
bStatus : 0 / bCancel 0
-> 누구나 절친 파기가능 함
bStatus : 1 / bCancel 1
-> 절친 파기를 한 상태이기 때문에 취소할 수 있음
bStatus : 1 / tEndDate가 현재시간에서 4일을 초과한 경우
-> 절친 삭제
*/
struct TBestFriendInfo
{
	// Common
	__time64_t tStartDate;
	__time64_t tEndDate;
	int nDay; // 일
	bool bStatus; // 0:절친 1:파기
	INT64 biWaitingTimeForDelete; // 삭제 대기시간

	// Me
	INT64 biItemSerial;
	WCHAR wszMyMemo[BESTFRIENDMEMOMAX];
	bool bCancel; // 0:절친파기당한 캐릭터 1:절친파기한 캐릭터

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

		// 요청자
		UINT nFromAccountDBID;	
		INT64 biFromCharacterDBID;
		WCHAR wszFromName[NAMELENMAX];

		// 응답자
		UINT nToAccountDBID;	
		INT64 biToCharacterDBID;
		WCHAR wszToName[NAMELENMAX];
	};

	struct SCRegistAck
	{
		int iRet;
		UINT nFromAccountDBID;	// 응답자
		UINT nToAccountDBID;	// 요청자
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
//	길드모집게시판 (CS_GUILDRECRUIT, SC_GUILDRECRUIT)
//--------------------------------------------------------------------------------------------------
namespace GuildRecruitSystem
{
	struct CSGuildRecruitList
	{
		UINT	uiPage;	
#if defined( PRE_ADD_GUILD_EASYSYSTEM )
		BYTE	cPurposeCode;				// 길드 목적(1:친목 2:전투 3:네스트 4:콜로세움
		WCHAR	wszGuildName[GUILDNAME_MAX];		//길드 홈페이지
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
		BYTE		cClassGrade[CLASSKINDMAX];	// 모집클래스
		int			nMinLevel;					// 최소 레벨
		int			nMaxLevel;					// 최대 레벨
		WCHAR		wszGuildIntroduction[GUILDRECRUITINTRODUCE+1];	//길드 소개글
#if defined( PRE_ADD_GUILD_EASYSYSTEM )
		BYTE		cPurposeCode;				// 길드 목적(1:친목 2:전투 3:네스트 4:콜로세움
		bool		bCheckHomePage;		//길드 홈페이지
#endif
	};
	struct CSGuildRecruitRegister
	{
		BYTE		cRegisterType;				// 등록 수정 삭제
		BYTE		cClassGrade[CLASSKINDMAX];	// 모집클래스
		int			nMinLevel;					// 최소 레벨
		int			nMaxLevel;					// 최대 레벨
		WCHAR		wszGuildIntroduction[GUILDRECRUITINTRODUCE+1];	//길드 소개글
#if defined( PRE_ADD_GUILD_EASYSYSTEM )
		BYTE		cPurposeCode;				// 길드 목적(1:친목 2:전투 3:네스트 4:콜로세움
		bool		bCheckHomePage;		//길드 홈페이지
#endif
	};
	struct SCGuildRecruitRegister
	{
		int			iRet;
		BYTE		cRegisterType;				// 등록 수정 삭제
	};
	struct CSGuildRecruitRequest
	{		
		BYTE		cRequestType;				// 가입 신청/해지
		TGuildUID	GuildUID;					// 길드 UID (월드 ID + 길드 DBID)
	};
	struct SCGuildRecruitRequest
	{		
		int			iRet;
		BYTE		cRequestType;				// 가입 신청/해지
	};
	struct CSGuildRecruitAccept
	{		
		BYTE		cAcceptType;				// 가입 승인/거절
		TGuildUID	GuildUID;					// 길드 UID (월드 ID + 길드 DBID)
		INT64		biAcceptCharacterDBID;		// 가입승인한 캐릭터 DBID
		WCHAR		wszToCharacterName[NAMELENMAX];		// 가입승인한 캐릭터 이름
	};
	struct SCGuildRecruitAccept
	{
		int			iRet;
		BYTE		cAcceptType;
		INT64		biAcceptCharacterDBID;		// 가입승인한 캐릭터 DBID		
		bool		bDelGuildRecruit;			// 길드 모집 목록 삭제 여부
	};
	struct SCGuildRecruitMemberResult
	{
		int			iRet;
		BYTE		cAcceptType;
		WCHAR		wszGuildName[GUILDNAME_MAX];	// 길드 이름
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
	int nRetCode;				// 결과
	int nNeedCount;				// 필요한 인벤수
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

	struct SCPrivateChatChannleResult	// CS 패킷에 대한 모든 결과값은 여기서 처리
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
		BYTE					cUserCount;				// 유저수
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
		int nWeeklyContributionPoint;	//주간 길드 공헌도
		int nTotalContributionPoint;	//길드 공헌도 총합
	};

	// SC_GUILD / SC_GUILD_CONTRIBUTION_RANK
	struct SCGuildContributionRank
	{
		int nCount;	// 공헌도 랭커 수
		GuildContribution::TGuildContributionRankingData Data[GUILDSIZE_MAX]; // 공헌도 랭커 정보
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
	__time64_t tLastPlayDate;	//필요없으면 삭제
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

struct CSInviteDWCTeamMemberReq	// CS_DWC / CS_INVITE_DWCTEAM_MEMBREQ			// 팀원 초대 요청
{
	WCHAR wszToCharacterName[NAMELENMAX];		// 초대할 캐릭터 이름
};

struct SCInviteDWCTeamMemberReq	// SC_DWC / SC_INVITE_DWCTEAM_MEMBREQ			// 팀원 초대 요청
{
	UINT nTeamID;
	UINT nFromAccountDBID;	// 발신자 계정 DBID
	UINT nToAccountDBID;	// 수신자 계정 DBID
	WCHAR wszFromCharacterName[NAMELENMAX];	// 발신자 캐릭터 이름
	WCHAR wszTeamName[GUILDNAME_MAX];		// 팀명
};

struct CSInviteDWCTeamMemberAck	// CS_DWC / CS_INVITE_DWCTEAM_MEMBACK
{
	UINT nTeamID;				// 길드 UID
	UINT nFromAccountDBID;		// 발신자 계정 DBID
	bool bAccept;				// 수락 여부
};

struct SCInviteDWCTeamMemberAck	// SC_DWC / SC_INVITE_DWCTEAM_MEMBACK
{
	UINT nToAccountDBID;			// 수신자 계정 DBID
	INT64 biToCharacterDBID;			// 수신자 캐릭터 DBID
	UINT nFromAccountDBID;			// 발신자 계정 DBID
	INT64 biFromCharacterDBID;		// 발신자 캐릭터 DBID
	UINT nTeamID;					// 팀ID
	TP_JOB nJob;					// 수신자 직업
	TCommunityLocation Location;	// 현재 위치 (로그인 여부도 본 멤버의 상태로 체크)
	int nRet;						// 결과 (NOERROR : 성공 / 그외 : 실패)
	WCHAR wszToCharacterName[NAMELENMAX];	// 수신자 캐릭터 이름
};

struct SCInviteDWCTeamMemberAckResult	// SC_DWC / SC_INVITE_DWCTEAM_MEMBACK_RESULT
{	
	int nRet;						// 결과 (NOERROR : 성공 / 그외 : 실패)
};

struct CSGetDWCTeamInfo	// CS_DWC / SC_GET_DWCTEAM_INFO
{
	bool bNeedMembList;	// 팀원 목록도 필요한지 여부
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
	BYTE cType;				//0 캐릭터명 1 팀명
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
//rlkt_test
struct SCOpenChangeJobDialog
{
	int		nNextClass;
};

struct CSSpecialize
{
	int		nSelectedClass;
};

//DragonJewel
#ifdef PRE_ADD_DRAGON_GEM
struct TGemData
{
	INT64 nParentItemSerial;
	int nSequenceNumber;
	int nItemID;
	char nDragonJewelLocation;
	char nItemOption;
	char nSealCount;
};

//SC_ITEM // SC_GEMLIST
struct TGemClientData
{
	INT64 nParentSerial;    //Parent item serial
	int	  nItemID;          //Gem ItemID
	char  cDragonJewelSlot; //Slot 0,1,2,3
};

struct SCGemList
{
	int nCount;
	TGemClientData GemData[MAX_GEM_LIST];
};

struct CSReqGemList
{
	int nCount;
	INT64 nParentSerial[MAX_GEM_LIST];
};

#endif
#pragma pack(pop)

