#pragma once

#include "DNServerPacketCash.h"
#include "Util.h"

#pragma pack(push, 1)

struct TQHeader
{
	BYTE cThreadID;
	char cWorldSetID;
	UINT nAccountDBID;
};

struct TAHeader
{
	UINT nAccountDBID;
	int nRetCode;
};

//--------------------------------------------------------------
//	DB Packet
//--------------------------------------------------------------

namespace DBPacket
{
	struct TSkillCoolTime
	{
		int nSkillID;
		int nCoolTime;
	};

	struct TItemCoolTime
	{
		INT64 biItemSerial;
		int nCoolTime;
	};

	struct TDurabilityInfo
	{
		INT64 biSerial;
		USHORT wDur;
	};

	struct TItemCountInfo
	{
		INT64 biSerial;
		short wCount;
	};

	struct TExchangeItem
	{
		BYTE cSlotIndex;
		int nItemID;
		INT64 biSerial;
		INT64 biNewSerial;
		short wCount;
	};

	struct TSendMailItem: public TItemInfo
	{
		INT64 biNewSerial;
	};

	struct TAttachMail
	{
		int nMailDBID;
		char cItemAttachCount;	// 첨부아이템개수
		INT64 biAttachCoin;		// 첨부코인값
#ifdef PRE_ADD_JOINGUILD_SUPPORT
		BYTE cMailType;
#endif		//#ifdef PRE_ADD_JOINGUILD_SUPPORT
		TItem Item[MAILATTACHITEMMAX];
	};

	struct TRequestTakeAttachInfo
	{
		int nMailDBID;
		char cAttachSlotIndex;		// -1: 5칸 모두 받기, 0 - 4: 개별받기		
		bool bAttachCoin;
		INT64 biSerial[MAILATTACHITEMMAX];
		BYTE cSlotIndex[MAILATTACHITEMMAX];
	};

	struct TTakeAttachInfo
	{
		int nMailDBID;
		INT64 biAttachCoin;		// 첨부코인값
#ifdef PRE_ADD_JOINGUILD_SUPPORT
		bool bAddGuildWare;
#endif		//#ifdef PRE_ADD_JOINGUILD_SUPPORT
		TItemInfo TakeItem[MAILATTACHITEMMAX];
	};

	struct TPetalInfo
	{
		char cSlotIndex;
		int nItemSN;
		INT64 biSerial;
		int nItemID;
		int nPetal;
	};

	struct TPetalInfoEx: TPetalInfo
	{
		int nFKey;
	};

	struct TCashFailItem
	{
		INT64 biPurchaseOrderID;
		INT64 biSenderCharacterDBID;
		bool bGift;
		int nItemSN;
		int nItemID;
		char cItemOption;
		int nPaidCashAmount;
	};

	struct TModCashFailItemEx: TCashItemBase
	{
		INT64 biPurchaseOrderID;
		INT64 biSenderCharacterDBID;
		bool bGift;
	};

	struct TMaterialItemInfo
	{
		DBDNWorldDef::ItemLocation::eCode Code;
		TItemInfo ItemInfo;
	};

	struct TVehicleInfo: public TVehicleItem, TVehicleBase
	{
		int nPetIndex;
		__time64_t tLastHungerModifyDate;
	};

	struct TAddMaterializedPet
	{
		BYTE cSlotIndex;
		int nItemID;
		INT64 biItemSerial;
	};

	struct TItemIndexSerial
	{
		BYTE cSlotIndex;
		INT64 biItemSerial;
	};

	struct TItemIDOption
	{
		int nItemID;
		char cOption;
	};

	struct TItemSNIDOption
	{
		int nItemSN;
		int nItemID;
		char cOption;
	};

	struct TReceiveGift
	{
		TReceiveGiftData GiftData;
		int nItemSN;
		DBPacket::TItemIDOption ItemIDOptions[PACKAGEITEMMAX];
		bool bFail;
	};
};

struct TDBListCharData
{
#if defined(PRE_MOD_SELECT_CHAR)
	BYTE cWorldID;
#endif	// #if defined(PRE_MOD_SELECT_CHAR)
	INT64 biCharacterDBID;
#if !defined(PRE_MOD_SELECT_CHAR)
	BYTE cCharIndex;
#endif	// #if defined(PRE_MOD_SELECT_CHAR)
	WCHAR wszCharacterName[NAMELENMAX];
	BYTE cLevel;
	BYTE cJob;
	int	nLastVillageMapID;
	int	nLastSubVillageMapID;
	int	nMapID;
	int	nDefaultBody;	// 처음 생성시에 설정한 equip
	int	nDefaultLeg;
	int	nDefaultHand;
	int	nDefaultFoot;
	DWORD dwHairColor;
	DWORD dwEyeColor;
	DWORD dwSkinColor;
	bool bVillageFirstVisit;
	int	nChecksum;
	int	nEquipArray[EQUIPMAX];
	int	nCashEquipArray[CASHEQUIPMAX];
	bool bDeleteFlag;
	__time64_t DeleteDate;
	__time64_t LastLoginDate;
	__time64_t CreateDate;
#if defined( PRE_ADD_NEWCOMEBACK )
	bool bCombackEffectItem;
#endif
#if defined( PRE_ADD_DWC )
	BYTE cAccountLevel;
#endif // #if defined( PRE_ADD_DWC )
};

struct TCreateCharacterItem
{
	int nItemID;
	BYTE cCount;
	USHORT wDur;
};

struct TRestraintForAccountAndCharacter
{
	char cLevelCode;
	__time64_t _tStartDate;
	__time64_t _tEndDate;
	WCHAR wszRestraintReason[RESTRAINTREASONMAX];
	int iReasonID;
};

//#########################################################################################################################################
//	MAINCMD_ETC
//#########################################################################################################################################

// Query-Queue
struct TDBQueryData : public DNTPacketHeader
{
public:
	void Set(const int iMainCmd, const int iSubCmd, const char *cBuf, const unsigned short uLen)
	{
		DN_ASSERT(iMainCmd < 255,	"Invalid!");
		DN_ASSERT(iSubCmd < 255,	"Invalid!");
		DN_ASSERT(NULL != cBuf,		"Invalid!");
		DN_ASSERT(uLen > 0,			"Invalid!");

		iLen = uLen;
		cMainCmd = iMainCmd;
		cSubCmd = iSubCmd;
		::memcpy(buf, cBuf, uLen);
	}
	void Reset()
	{
		iLen = 0;
		cMainCmd = 0;
		cSubCmd = 0;
	}
public:
	char buf[1];	// TDBQueryData 의 구조를 DNTPacket 구조와 맞추기 위해 DNTPacket::buf 와 일부러 이름을 동일하게 하였고, 버퍼 크기는 최소 1 로 전제
};

struct TDBQueryQueue
{
public:
	TDBQueryQueue() {
		Reset();
	}
	bool IsEmpty() const
	{
		return(0 >= m_nSize);
	}
	bool IsFull() const
	{
		return(sizeof(m_Buffer) <= m_nSize);
	}
	bool CheckIntegrity() const
	{
		if (IsEmpty()) {
			return false;
		}
		if (GetSize() > sizeof(*this)) {
			return false;
		}
		return true;
	}
	void Reset()
	{
		m_nCheckSum = DBQUERYAUTOCHKSUM;	// 20091022 패킷 처리 시 무결성 체크를 위해 임시 추가 ??? (b4nfter)
		m_nSize = 0;
		/*
		// P.S.> IsEmpty() 로 사전체크를 철저히 해야함
		for (int iIndex = 0 ; _countof(m_Queue) > iIndex ; ++iIndex) {
		m_Queue[iIndex].Reset();
		}
		*/
	}
	bool Add(const int iMainCmd, const int iSubCmd, const char *cBuf, const unsigned short uLen)
	{
		DN_ASSERT(iMainCmd < 255,	"Invalid!");
		DN_ASSERT(iSubCmd < 255,	"Invalid!");
		DN_ASSERT(NULL != cBuf,		"Invalid!");

		if (MAINCMD_ETC == iMainCmd && DB::QUERY_UPDATEQUERYQUEUE == iSubCmd) {
			DN_ASSERT(0,	"Invalid!");	// 이 패킷만은 절대 DB 작업으로 요청되어서는 않됨
		}

		if (IsFull() || static_cast<int>(sizeof(DNTPacketHeader) + uLen) > GetFreeBufferSize()) {
			DN_RETURN(false);
		}

		TDBQueryData* pQueryData = reinterpret_cast<TDBQueryData*>(&m_Buffer[m_nSize]);
		DN_ASSERT(NULL != pQueryData,	"Invalid!");

		pQueryData->Set(iMainCmd, iSubCmd, cBuf, uLen);

		m_nSize += (sizeof(DNTPacketHeader) + uLen);
		DN_ASSERT(GetTotalBufferSize() >= m_nSize,	"Invalid!");

		return true;
	}
	TDBQueryData* At(const int nCurPtr)
	{
		DN_ASSERT(GetTotalBufferSize() >= m_nSize,	"Invalid!");
		DN_ASSERT(m_nSize > static_cast<int>(nCurPtr + sizeof(TDBQueryData)),	"Invalid!");

		return(reinterpret_cast<TDBQueryData*>(&m_Buffer[nCurPtr]));
	}
	const TDBQueryData* At(const int nCurPtr) const
	{
		return(const_cast<TDBQueryQueue*>(this)->At(nCurPtr));
	}
	int GetTotalBufferSize() const
	{ 
		return(sizeof(m_Buffer));
	}
	int GetFreeBufferSize() const
	{ 
		return(sizeof(m_Buffer) - m_nSize);
	}
	int GetSize() const
	{
		return(static_cast<int>((sizeof(*this) - (sizeof(this->m_Buffer[0]) * (_countof(m_Buffer) - m_nSize)))&ULONG_MAX));
	}
public:
	unsigned int m_nCheckSum;	// 임시 ???
	int m_nSize;
	char m_Buffer[DBQUERYSTREAMDATAMAX];
};

//QUERY_EVENTLIST
struct TQEventList:public TQHeader
{

};

struct TEventInfo
{
	int EventID;
	int WroldID;
#if defined(PRE_ADD_WORLD_EVENT)
#else
	WCHAR wszMapIndex[EVENTMAPSTRMAX];
#endif //#if defined(PRE_ADD_WORLD_EVENT)
	time_t _tBeginTime;
	time_t _tEndTime;

	union 
	{
		struct
		{
			int EventType1;
			int EventType2;
			int EventType3;
		};
		int EventType[3];
	};

	union
	{
		struct
		{
			int Att1;
			int Att2;
			int Att3;
			int Att4;
			int Att5;
		};
		int Att[5];
	};
};

struct TAEventList: public TAHeader
{
	char cWorldSetID;
	BYTE cCount;
	TEventInfo EventList[EVENTLISTMAX];
};

#ifdef PRE_MOD_RESTRICT_IDENTITY_IP
struct TQSimpleConfig : public TQHeader
{
};

struct TASimpleConfig : public TAHeader
{
	BYTE cCount;
	SimpleConfig::ConfigData Configs[SimpleConfig::ConfigDataCountMax];
};
#endif		//#ifdef PRE_MOD_RESTRICT_IDENTITY_IP

struct TQGetDBSID : public TQHeader
{

};

struct TAGetDBSID : public TAHeader
{
	int nServerID;
};

// QUERY_LOGOUT
struct TQLogout:public TQHeader
{
	UINT nSessionID;
#if defined (_KR)
	BYTE szMachineID[MACHINEIDMAX];
#endif
};

// QUERY_LOGINCHARACTER
struct TQLoginCharacter:public TQHeader
{
	INT64 biCharacterDBID;
	UINT uiSessionID;
	WCHAR wszIP[IPLENMAX];
};

// QUERY_LOGOUTCHARACTER
struct TQLogoutCharacter:public TQHeader
{
	INT64 biCharacterDBID;
	UINT uiSessionID;
	char cViewCashEquipBitmap[VIEWCASHEQUIPMAX_BITSIZE];	// 일반/캐쉬템 장비 bitmap으로 처리
};

// GameOption ----------------------------------------------------------------------
// QUERY_GETGAMEOPTION
struct TQGetGameOption:public TQHeader
{
	INT64 biCharacterDBID;
};

struct TAGetGameOption: public TAHeader
{
	INT64 biCharacterDBID;
	TGameOptions Option;
};

//QUERY_SETGAMEOPTION
struct TQSetGameOption:public TQHeader
{
	INT64 biCharacterDBID;
	TGameOptions Option;
};

struct TQUpdateSecondAuthPassword:public TQHeader
{
	WCHAR wszOldPW[SecondAuth::Common::PWMaxLength+1];
	WCHAR wszNewPW[SecondAuth::Common::PWMaxLength+1];
};

struct TAUpdateSecondAuthPassword: public TAHeader
{
	char cFailCount;
};

struct TQUpdateSecondAuthLock:public TQHeader
{
	bool bIsLock;
	WCHAR wszPW[SecondAuth::Common::PWMaxLength+1];		// UnLock 일경우 Password
};

struct TAUpdateSecondAuthLock: public TAHeader
{
	bool bIsLock;
	char cFailCount;
};

struct TQInitSecondAuth:public TQHeader
{
};

struct TQValidateSecondAuth:public TQHeader
{
	int	nAuthCheckType;
	WCHAR wszPW[SecondAuth::Common::PWMaxLength+1];
};

struct TAValidateSecondAuth:public TAHeader
{
	int	nAuthCheckType;
	BYTE cFailCount;
};

// Restraint ----------------------------------------------------------------------
struct TQRestraint:public TQHeader		//QUERY_RESTRAINT
{
};

struct TARestraint: public TAHeader
{
	BYTE cCount;
	TRestraint restraint[RESTRAINTMAX];
};

struct TQSetRestraint :  public TQHeader	//QUERY_SETRESTRAINT
{
	BYTE cTargetCode;				//DBDNWorldDef::RestraintTargetCode::eCode
	BYTE cRestraintCode;			//DBDNWorldDef::RestraintTypeCode::eCode
	INT64 biCharacterDBID;			//if target code "character"
	WCHAR wszRestraintreason[RESTRAINTREASONMAX];	//Reason String
	WCHAR wszRestraintreasonForDolis[RESTRAINTREASONFORDOLISMAX];
	USHORT nDurationDays;				//duration
	int nDolisReasonCode;				//기본은 110 어뷰즈로그에 의한 제재는 111 코드 번호관리는 솔루션팀(원상씨 또는 시연씨에게 문의바람)
};

// QUERY_GET_KEYSETTINGOPTION
struct TQGetKeySettingOption:public TQHeader
{
};

struct TAGetKeySettingOption: public TAHeader
{
	TKeySetting	sKeySetting;
};

// QUERY_MOD_KEYSETTINGOPTION
struct TQModKeySettingOption:public TQHeader
{
	bool bUseDefault;
	TKeySetting	sKeySetting;
};

struct TAModKeySettingOption: public TAHeader
{
};

// QUERY_GET_PADSETTINGOPTION
struct TQGetPadSettingOption:public TQHeader
{
};

struct TAGetPadSettingOption: public TAHeader
{
	TPadSetting	sPadSetting;
};

// QUERY_MOD_PADSETTINGOPTION
struct TQModPadSettingOption:public TQHeader
{
	bool bUseDefault;
	TPadSetting	sPadSetting;
};

struct TAModPadSettingOption: public TAHeader
{
};

struct TQGetProfile : public TQHeader
{
	INT64 biCharacterDBID;
};

struct TAGetProfile : public TAHeader
{
	TProfile sProfile;
};

struct TQSetProfile : public TQHeader
{
	INT64 biCharacterDBID;
	TProfile sProfile;
};

struct TASetProfile : public TAHeader
{
	TProfile sProfile;
};

struct TQAddAbuseLog : public TQHeader
{
	BYTE cAbuseCode;
	INT64 biCharacterDBID;
	USHORT unLen;
	WCHAR wszBuf[1024];
};


struct TQAddAbuseMonitor:public TQHeader
{
	INT64 biCharacterDBID;
	int iCount;
#if defined(PRE_ADD_ANTI_CHAT_SPAM)
	int iCount2; // 채팅 어뷰즈용
#endif
};

struct TQDelAbuseMonitor:public TQHeader
{
	INT64 biCharacterDBID;
#if defined(PRE_ADD_ANTI_CHAT_SPAM)
	char cType;  // 1-오토어뷰징, 2-채팅어뷰징, 3-둘다 리셋
#endif
};

struct TQGetAbuseMonitor:public TQHeader
{
	INT64 biCharacterDBID;
};

struct TAGetAbuseMonitor:public TAHeader
{
	int iAbuseCount;
	int iCallCount;
	int iPlayRestraintValue;
	int iDBResetRestraintValue;
};

struct TQGetWholeAbuseMonitor:public TQHeader
{
	UINT nAccountDBID;
#if defined(PRE_ADD_MULTILANGUAGE)
	char cSelectedLang;
#endif		//#if defined(PRE_ADD_MULTILANGUAGE)
};

struct TAbuseMonitor
{
	INT64 biCharacterDBID;
	int iAbuseCount;
	int iCallCount;
	int iPlayRestraintValue;
	int iDBResetRestraintValue;
};

struct TAGetWholeAbuseMonitor:public TAHeader
{
#if defined(PRE_ADD_MULTILANGUAGE)
	char cSelectedLang;
#endif		//#if defined(PRE_ADD_MULTILANGUAGE)
	BYTE cCount;
	TAbuseMonitor Abuse[CHARCOUNTMAX];
};


// QUERY_DBRESULTERROR
struct TADBResultError: public TAHeader
{
	char cMainCmd;
	char cSubCmd;
};

#if defined( _US )
struct TQKeepAlive:public TQHeader
{

};
#endif // #if defined( _US )

// QUERY_CHANGECHARACTERNAME					// 캐릭터명 변경
struct TQChangeCharacterName : public TQHeader
{
	INT64 nCharacterDBID;		// 캐릭터 DBID
	WCHAR wszOriginName[NAMELENMAX];	   // 기존 캐릭터명
	WCHAR wszCharacterName[NAMELENMAX];  // 변경할 캐릭터명
	BYTE btChangeCode;		// 1 = 관리자변경, 2 = 캐릭터 이름변경
	int	nReservedPeriodDay;	// 변경전 캐릭터 이름이 보존되는 기간
	INT64 biItemSerial;		// 사용 아이템 ID
	int	nNextPeriodDay;		// 최근 이름 변경 일자로부터 다음 이름 변경이 가능한 기간 제한. 단위 : day
};

struct TAChangeCharacterName : public TAHeader
{
	char cWorldSetID;
	INT64 nCharacterDBID;		// 캐릭터 DBID
	INT64 biItemSerial;
	WCHAR wszOriginName[NAMELENMAX];		// 기존 캐릭터명
	WCHAR wszCharacterName[NAMELENMAX];  // 변경한 캐릭터명
	__time64_t tNextChangeDate;
};

// QUERY_GETLIST_ETCPOINT
struct TQGetListEtcPoint : public TQHeader
{
	INT64 nCharacterDBID;	
};

struct TAGetListEtcPoint : public TAHeader
{
	INT64 nCharacterDBID;
	INT64 biEtcPoint[DBDNWorldDef::EtcPointCode::Max];
};

struct TQAddEtcPoint :public TQHeader
{
	INT64 biCharacterDBID;
	BYTE cType;
	INT64 biAddPoint;
	int nMapID;
	WCHAR wszIP[IPLENMAX];
};

struct TAAddEtcPoint:public TAHeader
{
	BYTE cType;
	INT64 biLeftPoint;
};

struct TQUseEtcPoint :public TQHeader
{
	INT64 biCharacterDBID;
	BYTE cType;
	INT64 biUsePoint;
	int nMapID;
	WCHAR wszIP[IPLENMAX];
};

struct TAUseEtcPoint:public TAHeader
{
	BYTE cType;
	INT64 biLeftPoint;
};

struct TQSaveConnectDurationTime: public TQHeader
{
	INT64 biCharacterDBID;
	char cCount;
	TTimeEvent Event[TIMEEVENTMAX];
};

struct TQModTimeEventDate:public TQHeader
{
	INT64 biCharacterDBID;
};

struct TQChangeJobCode : public TQHeader
{
	INT64 biCharacterDBID;
	int nFirstJobIDBefore;
	int nFirstJobIDAfter;
	int nSecondJobIDBefore;
	int nSecondJobIDAfter;
	int nChannelID;
	int nMapID;
	char cSkillPage;
};

struct TAChangeJobCode : public TAHeader
{
	USHORT wSkillPoint;	// 스킬 리셋 후 스킬 포인트 증가량.
	USHORT wTotalSkillPoint;	// 스킬 리셋 후 보유한 총 스킬 포인트.
};

struct TQGetListVariableReset:public TQHeader
{
	INT64 biCharacterDBID;
};

struct TAGetListVariableReset:public TAHeader
{
	BYTE cCount;
	CommonVariable::Data Data[CommonVariable::Type::Max];
};

struct TQModVariableReset:public TQHeader
{
	INT64 biCharacterDBID;
	CommonVariable::Type::eCode Type;
	INT64 biValue;
	__time64_t tResetTime;
};

#if defined(PRE_ADD_GAMEQUIT_REWARD)
struct TQModNewbieRewardFlag:public TQHeader
{
	bool bRewardFlag;
};
#endif	// #if defined(PRE_ADD_GAMEQUIT_REWARD)

#if defined(PRE_ADD_CP_RANK)
struct TQAddStageClearBest : public TQHeader
{
	int nMapID;
	INT64 biCharacterDBID;
	DBDNWorldDef::ClearGradeCode::eCode Code;
	int nClearPoint;
	int nLimitLevel;
};

struct TQGetStageClearBest : public TQHeader
{
	int nMapID;
};

struct TAGetStageClearBest : public TAHeader
{
	TStageClearBest sLegendClearBest;
	TStageClearBest sMonthlyClearBest;
};

struct TQGetStageClearPersonalBest : public TQHeader
{
	int nMapID;
	INT64 biCharacterDBID;
};

struct TAGetStageClearPersonalBest : public TAHeader
{
	DBDNWorldDef::ClearGradeCode::eCode Code;
	int nClearPoint;
};

struct TQInitStageCP : public TQHeader
{
	char cInitType;
};
#endif //#if defined(PRE_ADD_CP_RANK)

// QUERY_MODCHARACTERSLOTCOUNT
struct TQModCharacterSlotCount: public TQHeader
{
	INT64 biCharacterDBID;
};


//#########################################################################################################################################
//	MAINCMD_STATUS
//#########################################################################################################################################

// QUERY_SELECTCHARACTER
struct TQSelectCharacter:public TQHeader
{
	INT64 biCharacterDBID;
#if !defined( PRE_PARTY_DB )
	TPARTYID PartyID;
#endif
	UINT nLastServerType;
	WCHAR wszAccountName[IDLENMAX];
};

const int nItemListMaxCount = EQUIPMAX + INVENTORYMAX + WAREHOUSEMAX + QUESTINVENTORYMAX + CASHEQUIPMAX + GLYPHMAX + TALISMAN_MAX + PERIODINVENTORYMAX + PERIODWAREHOUSEMAX;

struct TASelectCharacter:public TAHeader
{
	INT64 biCharacterDBID;
	TRestraintData RestraintData;	// 제재관련데이타
	TKeySetting KeySetting;		// 키세팅관련
	TPadSetting	PadSetting;
#if !defined( PRE_PARTY_DB )
	TPARTYID PartyID;
#endif
	TGuildSelfView GuildSelfView;	// 길드 시각정보 (개인)
	bool bIsSetSecondAuthPW;	// 2차 인증 비밀번호 설정 여부
	bool bIsSetSecondAuthLock;	// 2차 인증 계정 Lock 여부

#if defined(PRE_ADD_VIP)
	int nVIPTotalPoint;			// 총 VIP 포인트
	__time64_t tVIPEndDate;		// VIP 기간 끝 일자
	bool bAutoPay;				// 0=수동결제, 1=자동결제
#endif	// #if defined(PRE_ADD_VIP)
#if defined(_CH)
	bool bIntroducer;			// 중국 프로모션 추천인
#endif	// #if defined(_CH)

	TVehicleCompact VehicleEquip;
 	TVehicleCompact PetEquip;
	TUserData UserData;
#ifdef PRE_ADD_JOINGUILD_SUPPORT
	BYTE cGuildJoinLevel;				//길드가입 레벨(최초한번)
	bool bWasRewardedGuildMaxLevel;		//길드에서 만렙찍을 경우 보상을 받았는지 플래그
#endif		//#ifdef PRE_ADD_JOINGUILD_SUPPORT
#if defined(PRE_ADD_TALISMAN_SYSTEM)
	int nTalismanOpenFlag;
#endif
#if defined( PRE_ADD_CHARACTERCHECKSUM )
	__time64_t tLastLoginDate;
	__time64_t tLastLogoutDate;
	UINT uiCheckSum;
#endif // #if defined( PRE_ADD_CHARACTERCHECKSUM )
	int nItemListCount;
	DBPacket::TMaterialItemInfo ItemList[nItemListMaxCount];	// 제일 밑에있어야함다~
};

// QUERY_UPDATEUSERDATA, QUERY_CHANGESERVERUSERDATA, QUERY_LASTUPDATEUSERDATA
struct TQUpdateCharacter:public TQHeader
{
	UINT uiDBRandomSeed;

	INT64 biCharacterDBID;
	int nMapIndex;							// 현재 맵번호
	int nLastVillageMapIndex;				// 마지막 마을 맵번호
	int nLastSubVillageMapIndex;			// 마지막 Sub마을 맵번호
	char cLastVillageGateNo;				// 마지막 마을 게이트 번호
	int nPosX;								// 현재 X좌표
	int nPosY;								// 현재 Y좌표
	int nPosZ;								// 현재 Z좌표
	float fRotate;							// Rotate
	int nGlyphDelayTime;
	int nGlyphRemainTime;
	bool bExecuteScheduleedTask;
	char cViewCashEquipBitmap[VIEWCASHEQUIPMAX_BITSIZE];	// 일반/캐쉬템 장비 bitmap으로 처리
	TQuickSlot QuickSlot[QUICKSLOTMAX];		// 단축 슬롯창 (10개씩 2줄)
	DBPacket::TSkillCoolTime SkillCoolTime[SKILLMAX];	// 스킬
	DBPacket::TSkillCoolTime SkillCoolTime2[SKILLMAX];	// 스킬2
	DBPacket::TItemCoolTime Equip[EQUIPMAX];			// 장비창
#if defined(PRE_PERIOD_INVENTORY)
	DBPacket::TItemCoolTime Inventory[INVENTORYTOTALMAX];	// 인벤창
	DBPacket::TItemCoolTime Warehouse[WAREHOUSETOTALMAX];	// 창고창
#else	// #if defined(PRE_PERIOD_INVENTORY)
	DBPacket::TItemCoolTime Inventory[INVENTORYMAX];	// 인벤창
	DBPacket::TItemCoolTime Warehouse[WAREHOUSEMAX];	// 창고창
#endif	// #if defined(PRE_PERIOD_INVENTORY)
	DBPacket::TItemCoolTime CashEquip[CASHEQUIPMAX];	// 장비창 (캐쉬)
	DBPacket::TItemCoolTime CashInventory[CASHINVENTORYDBMAX];	// 인벤창 (캐쉬)
#if defined( PRE_ITEMBUFF_COOLTIME )
	DBPacket::TItemCoolTime ServerWare[WAREHOUSEMAX];	// 서버창고
	DBPacket::TItemCoolTime ServerWareCash[WAREHOUSEMAX];	// 서버창고(캐쉬)
#endif
};

struct TAUpdateCharacter:public TAHeader
{
};

struct TQMWTest:public TQHeader
{
	int iCount;
};

// QUERY_CHANGESTAGEUSERDATA
struct TQChangeStageUserData:public TQHeader
{
	INT64 biCharacterDBID;
	int nChannelID;
	int nMapID;
	TPARTYID PartyID;
	int nExp;
	INT64 biCoin;
	BYTE cRebirthCoin;			// 사용한 일반 부활 코인
	BYTE cPCBangRebirthCoin;	// 사용한 PC방 부활 코인
	short nCount;
	short nDeathCount;			// 사망횟수
	BYTE cDifficult;			// 난이도
	INT64 biPetItemSerial;
	int nPetExp;
	INT64 biPickUpCoin;
	DBPacket::TDurabilityInfo ItemDurArr[INVENTORYMAX + EQUIPMAX];
};

struct TAChangeStageUserData:public TAHeader
{
	BYTE cRebirthCoin;			// 사용한 일반 부활 코인
	BYTE cPCBangRebirthCoin;	// 사용한 PC방 부활 코인
};

// QUERY_GETCHARACTERPARTIALYBYNAME,
struct TQGetCharacterPartialyByName:public TQHeader
{
	WCHAR wszName[NAMELENMAX];
	BYTE cReqType;			//eReqCharacterPartialy
};

// QUERY_GETCHARACTERPARTIALYBYDBID
struct TQGetCharacterPartialyByDBID:public TQHeader
{
	INT64 biCharacterDBID;
	BYTE cReqType;			//eReqCharacterPartialy
};

struct TAGetCharacterPartialy:public TAHeader
{
	INT64 biCharacterDBID;
	BYTE cClass;
	BYTE cLevel;
	BYTE cJob;
	BYTE cReqType;			//eReqCharacterPartialy
};

// QUERY_CHECKFIRSTVILLAGE
struct TQCheckFirstVillage:public TQHeader
{
	INT64 biCharacterDBID;
};

// QUERY_LEVEL,					// 레벨
struct TQLevel:public TQHeader
{
	INT64 biCharacterDBID;
	BYTE cLevel;
	char cChangeCode;	// CharacterLevelChangeCode 
	int nChannelID;
	int nMapID;
	WCHAR wszCharName[NAMELENMAX];
};

// QUERY_EXP,						// exp
struct TQExp:public TQHeader
{
	INT64 biCharacterDBID;
	int nExp;
	char cChangeCode;	// eCharacterExpChangeCode 
	int nChannelID;
	int nMapID;
	INT64 biFKey;			// CharacterExpChangeCode별 참조 키. 1:QuestID, 2:PartyID or NULL, 3:NULL, 4:AuditLogID 5:ItemSerial
};

// QUERY_COIN
struct TQCoin:public TQHeader
{
	INT64 biCharacterDBID;
	char cCoinChangeCode;	// CoinChangeCode
	INT64 biChangeKey;			// 퀘스트아이디
	INT64 nChangeCoin;		// 코인변경치만 (+/-)
	int nChannelID;
	int nMapID;
	INT64 nTotalCoin;		// 디비저장후 리턴값이랑 비교하기위해
};

// QUERY_WAREHOUSECOIN
struct TQWarehouseCoin:public TQHeader
{
	INT64 biCharacterDBID;
	char cCoinChangeCode;	// CoinChangeCode
	INT64 nChangeCoin;		// 코인변경치만 (+/-)
	int nChannelID;
	int nMapID;
	INT64 nTotalCoin;			// 디비저장후 리턴값이랑 비교하기위해
	INT64 nTotalWarehouseCoin;	// 디비저장후 리턴값이랑 비교하기위해
};

// QUERY_MAPINFO
struct TQMapInfo:public TQHeader
{
	INT64 biCharacterDBID;
	char cLastVillageGateNo;				// 마지막 마을 게이트 번호
	int nLastVillageMapIndex;				// 마지막 마을 맵번호
	int nLastSubVillageMapIndex;			// 마지막 Sub마을 맵번호
	int nMapIndex;							// 현재 맵번호
};

// QUERY_ADDCASHREBIRTHCOIN
struct TQAddCashRebirthCoin: public TQHeader
{
	INT64 biCharacterDBID;
	int nRebirthCount;
	int nRebirthPrice;
	int nRebirthCode;
	int nRebirthKey;
};

// QUERY_CASHREBIRTHCOIN
struct TQCashRebirthCoin:public TQHeader
{
	INT64 biCharacterDBID;
	int nChannelID;
	int nMapID;
	WCHAR wszIP[IPLENMAX];
};

// QUERY_FATIGUE
struct TQFatigue:public TQHeader
{
	INT64 biCharacterDBID;
	char cFatigueTypeCode;		// FatigueTypeCode
	int nChangeFatigue;			// 변경치
	int nChannelID;
	int nMapID;
};

// QUERY_EVENTFATIGUE
struct TQEventFatigue:public TQHeader
{
	INT64 biCharacterDBID;
	int nChangeFatigue;		// 변경치
	bool bReset;			// 이벤트 피로도 초기화 여부. 0=초기화 아님, 1=초기화
	char cInitTime;			// 이벤트 피로도가 초기화되는 시간. 0 ~ 23
	int nChannelID;
	int nMapID;
};

// QUERY_JOB
struct TQJob:public TQHeader
{
	INT64 biCharacterDBID;
	BYTE cJob;
};

// QUERY_GLYPHDELAYTIME
// QUERY_GLYPHREMAINTIME
struct TQGlyphAttributeTime: public TQHeader
{
	INT64 biCharacterDBID;
	int nTime;
};

// QUERY_NOTIFIER
struct TQNotifier:public TQHeader
{
	INT64 biCharacterDBID;
	char cNotifierSlot;	// Notifier_
	char cNotifierType;	// DNNotifier::Type 값
	int nID;
};

// QUERY_DUNGEONCLEAR
struct TQDungeonClear:public TQHeader
{
	INT64 biCharacterDBID;
	char cType;
	int nMapIndex;
};

// QUERY_ETERNITYITEM
struct TQEternityItem:public TQHeader
{
	INT64 biCharacterDBID;
	char cEternityCode;		// eEternityItem
	int nEternityValue;
};

// QUERY_COLOR
struct TQColor: public TQHeader
{
	INT64 biCharacterDBID;
	char cItemType;
	DWORD dwColor;
};

#ifdef PRE_FIX_63822
struct TAColor : public TAHeader
{
};
#endif		//#ifdef PRE_FIX_63822

// QUERY_REBIRTHCOIN
struct TQRebirthCoin: public TQHeader
{
	INT64 biCharacterDBID;
	BYTE cRebirthCoin;
	BYTE cPCBangRebirthCoin;
};

struct TAEffectItem:public TAHeader
{
	int	iOffset;
	TEffectItemInfo ItemInfo;
};

// QUERY_NESTCLEARCOUNT,			// NEST 클리어 횟수 UPSERT 
struct TQNestClearCount: public TQHeader
{
	INT64 biCharacterDBID;
	int nMapIndex;
	char cClearType;		// eNestClearType
};

// QUERY_INITNESTCLEARCOUNT,		// NEST 주간 클리어 횟수 초기화 
struct TQInitNestClearCount: public TQHeader
{
	INT64 biCharacterDBID;
};

struct TAOwnChracterLevel : public TAHeader		//MAINCMD_STATUS / QUERY_OWNCHRACTERLEVEL
{
	BYTE cCount;
	TChracterLevel Level[CHARCOUNTMAX];
};

struct TQChangeSkillPage : public TQHeader
{
	INT64 biCharacterDBID;
	char cSkillPage;
};

#if defined( PRE_ADD_TOTAL_LEVEL_SKILL )
struct TQGetTotalLevelSkill : public TQHeader
{	
	INT64 biCharacterDBID;
};
struct TAGetTotalLevelSkill : public TAHeader
{	
	int nTotalSkillLevel;
	TotalLevelSkill::TTotalLevelSkillData TotalSkill[TotalLevelSkill::Common::MAXSLOTCOUNT];
};
struct TQAddTotalLevelSkill : public TQHeader
{	
	INT64 biCharacterDBID;
	int nSlotIndex;
	int nSkillID;
};
#endif

#ifdef PRE_ADD_PRESET_SKILLTREE
struct TQGetSKillPreSetList : public TQHeader
{	
	INT64 biCharacterDBID;
};

struct TAGetSKillPreSetList : public TAHeader
{
	TSkillSetIndexData SKillIndex[SKILLPRESETMAX];
	TSkillSetPartialData SKills[SKILLMAX * SKILLPRESETMAX];
};

struct TQAddSkillPreSet : public TQHeader
{
	INT64 biCharacterDBID;
	BYTE cIndex;
	WCHAR wszName[SKILLPRESETNAMEMAX];
	BYTE cCount;
	TSkillSetPartialData SKill[SKILLMAX];
};

struct TAAddSkillPreSet : public TAHeader
{
};

struct TQDelSkillPreSet : public TQHeader
{
	INT64 biCharacterDBID;
	BYTE cIndex;
};

struct TADelSkillPreSet : public TAHeader
{
};
#endif		//#ifdef PRE_ADD_PRESET_SKILLTREE

//#########################################################################################################################################
//	MAINCMD_MISSION
//#########################################################################################################################################

// QUERY_MISSIONGAIN
struct TQMissionGain:public TQHeader
{
	INT64 biCharacterDBID;
	char MissionGain[MISSIONMAX_BITSIZE];		// 획득여부	
};

// QUERY_MISSIONACHIEVE
struct TQMissionAchieve:public TQHeader
{
	INT64 biCharacterDBID;
	int nMissionID;
	char cMissionType;		// eMissionType
	char MissionAchieve[MISSIONMAX_BITSIZE];	// 목표달성(on/off)
};

// QUERY_SETDAILYMISSION,	// 일일 미션 정보 저장하기 (처음 싹~ 넣어줄때)
struct TQSetDailyMission:public TQHeader
{
	INT64 biCharacterDBID;
	int nMissionIDs[DAILYMISSIONMAX];
};

// QUERY_SETWEEKLYMISSION,	// 주간 미션 정보 저장하기 (처음 싹~ 넣어줄때)
struct TQSetWeeklyMission:public TQHeader
{
	INT64 biCharacterDBID;
	int nMissionIDs[WEEKLYMISSIONMAX];
};

#if defined(PRE_ADD_MONTHLY_MISSION)
// QUERY_SETMONTHLYMISSION,	// 월간 이벤트 미션 정보 저장하기 (처음 싹~ 넣어줄때)
struct TQSetMonthlyMission:public TQHeader
{
	INT64 biCharacterDBID;
	int nMissionIDs[MONTHLYMISSIONMAX];
};
#endif	// #if defined(PRE_ADD_MONTHLY_MISSION)

// QUERY_SETWEEKENDEVENTMISSION,	// 주말 이벤트 미션 정보 저장하기 (처음 싹~ 넣어줄때)
struct TQSetWeekendEventMission:public TQHeader
{
	INT64 biCharacterDBID;
	int nMissionIDs[WEEKENDEVENTMISSIONMAX];
};

// QUERY_SETGUILDWARMISSION, // 길드 축제미션 정보 저장하기
struct TQSetGuildWarMission:public TQHeader
{
	INT64 biCharacterDBID;
	int nMissionIDs[GUILDWARMISSIONMAX];
};

// QUERY_SETGUILDCOMMONMISSION, // 길드 일반 미션 정보 저장하기
struct TQSetGuildCommonMission:public TQHeader
{
	INT64 biCharacterDBID;
	int nMissionIDs[GUILDCOMMONMISSIONMAX];
};

struct TQSetWeekendRepeatMission:public TQHeader
{
	INT64 biCharacterDBID;
	int nMissionIDs[WEEKENDREPEATMISSIONMAX];
};

// QUERY_SETPCBANGMISSION,	// 일일 미션 정보 저장하기 (처음 싹~ 넣어줄때)
struct TQSetPCBangMission:public TQHeader
{
	INT64 biCharacterDBID;
	int nMissionIDs[PCBangMissionMax];		// 0: silver, 1: gold
};

// QUERY_MODDAILYWEEKLYMISSION,	// 일일/주간 미션 정보 저장하기
struct TQModDailyWeeklyMission:public TQHeader
{
	INT64 biCharacterDBID;
	char cDailyMissionType;	// eDailyMissionType
	char cMissionIndex;
	TDailyMission Mission;
	int nMissionScore;
};

struct TQClearMission : public TQHeader
{
	INT64 biCharacterDBID;
};

//#########################################################################################################################################
//	MAINCMD_APPELLATION
//#########################################################################################################################################

// QUERY_ADDAPPELLATION
struct TQAddAppellation:public TQHeader
{
	INT64 biCharacterDBID;
	int nAppellationID;
	char Appellation[APPELLATIONMAX_BITSIZE];	// 호칭
};

// QUERY_SELECTAPPELLATION
struct TQSelectAppellation:public TQHeader
{
	INT64 biCharacterDBID;
	int nSelectAppellation;
	char cAppellationKind;		// DBDNWorldDef::AppellationKind
};
// QUERY_DELAPPELLATION
struct TQDelAppellation:public TQHeader
{
	INT64 biCharacterDBID;	
	int	  nDelAppellation;	// 삭제할 칭호(개별 삭제)
};


//#########################################################################################################################################
//	MAINCMD_SKILL
//#########################################################################################################################################

// QUERY_ADDSKILL,					// 스킬 추가
struct TQAddSkill:public TQHeader
{
	INT64 biCharacterDBID;
	int nSkillID;			// 스킬아이디
	char cSkillLevel;
	int nCoolTime;			// 쿨타임
	char cSkillChangeCode;	// SkillChangeCode
	INT64 biCoin;			// 스킬을 구입한 경우 구입 비용. 구입한 경우가 아니면 0을 입력합니다.
	int nChannelID;
	int nMapID;
	char cSkillPage;		// 스킬페이지(스킬코드)
	INT64 biCurrentCoin;
	INT64 biPickUpCoin;
};

// QUERY_MODSKILLLEVEL,			// 스킬레벨변경
struct TQModSkillLevel:public TQHeader
{
	INT64 biCharacterDBID;
	int nSkillID;			// 스킬아이디
	BYTE cSkillLevel;		// 스킬레벨
	int nCoolTime;			// 쿨타임
	USHORT wUsedSkillPoint;	// 스킬레벨올리는데 사용한 스킬포인트
	char cSkillChangeCode;	// SkillChangeCode (2=스킬레벨업 & 스킬포인트 사용, 13=스킬레벨 조정 (by 관리자))
	int nChannelID;
	int nMapID;
	char cSkillPage;
};

// QUERY_INCREASESKILLPOINT,		// 스킬포인트 증가
struct TQIncreaseSkillPoint:public TQHeader
{
	INT64 biCharacterDBID;
	USHORT wSkillPoint;
	int nChannelID;
	int nMapID;
	char cSkillPointChangeCode;
	char cSkillPage;
};

// QUERY_DELSKILL
struct TQDelSkill:public TQHeader
{
	INT64 biCharacterDBID;
	int nSkillID;		// 스킬아이디
	char cSkillChangeCode;	// SkillChangeCode (9=스킬 삭제 (드랍), 10=스킬 삭제 (퀘스트), 11=스킬 삭제 (이벤트 만료), 12=스킬 삭제 (by 관리자))
	bool bSkillPointBack;	// 스킬 삭제 후 스킬 포인트를 돌려 줄지 여부. 0=스킬포인트 돌려주지 않음, 1=스킬 포인트 돌려줌
	int nChannelID;
	int nMapID;
	char cSkillPage;		// 스킬페이지(스킬코드)
};

// QUERY_RESETSKILL
struct TQResetSkill:public TQHeader
{
	INT64 biCharacterDBID;
	int nChannelID;
	int nMapID;
	char cSkillPage;		// 스킬페이지(스킬코드)
};

struct TAResetSkill: public TAHeader
{
	USHORT wSkillPoint;	// 스킬 리셋 후 보유한 총 스킬 포인트
	USHORT wTotalSkillPoint;	// 스킬 리셋 후 보유한 총 스킬 포인트
	char cSkillPage;		// 스킬페이지(스킬코드)
};

// QUERY_RESETSKILLBYSKILLIDS
struct TQResetSkillBySkillIDS:public TQHeader
{
	INT64	biCharacterDBID;
	int		iSkillIDs[SKILLMAX];
	int		iChannelID;
	int		iMapID;
	char	cSkillPage;
};

struct TAResetSkillBySkillIDS:public TAHeader
{
	short	nSkillPoint;
	short	nTotalSkillPoint;
	char	cSkillPage;
};

struct TQDecreaseSkillPoint:public TQHeader
{
	INT64 biCharacterDBID;
	short nDecreaseSkillPoint;
	int iChannelID;
	int iMapID;
	char cSkillPage;
};

struct TQUseExpandSkillPage:public TQHeader
{
	INT64 biCharacterDBID;
	int		nSkillArray[DEFAULTSKILLMAX];	
	short	wSkillPoint;
};
struct TAUseExpandSkillPage:public TAHeader
{

};
struct TQSetSkillPoint:public TQHeader
{
	INT64 biCharacterDBID;
	short nSkillPoint;	
	char cSkillPage;
};


//#########################################################################################################################################
//	MAINCMD_ITEM
//#########################################################################################################################################

struct TQAddMaterializedItem:public TQHeader
{
	INT64 biCharacterDBID;
	BYTE cItemLocationIndex;	// 아이템이 위치할 인덱스 값
	DBDNWorldDef::AddMaterializedItem::eCode Code;
	INT64 biFKey;
	TItem AddItem;
	bool bStackableFlag;
	INT64 biItemPrice;	// 아이템 가격 가치. 구입하지 않은 아이템인 경우 0을 입력합니다. 단, 패키지 상품을 선물 또는 구입한 경우, 상품을 구성하는 각 아이템의 가격은 상품 가격의 1/n을 입력합니다
	INT64 biSenderCharacterDBID;	// 선물인 경우 선물한 캐릭터 ID
	BYTE cItemLocationCode;	// 2=인벤토리, 4=퀘스트 인벤토리, 5=캐쉬 인벤토리, 6=선물함, 7=캐쉬 이큅먼트
	BYTE cPayMethodCode;	// NULL=구입아님, 1=코인, 2=캐쉬, 3=적립금, 4=DNA포인트
	int	iChannelID;
	int	iMapIndex;
	WCHAR wszIP[IPLENMAX];
	bool bMerge;			// 0=아이템이 merge되지 않음, 1=아이템이 merge됨
	INT64 biMergeTargetItemSerial;	// 생성된 아이템이 목적지에서 merge되는 경우, target 아이템의 아이템 시리얼
	DWORD dwPetPartsColor1;
	DWORD dwPetPartsColor2;
	int	nPetSkillID1;
	int	nPetSkillID2;
	char cPetCount;		// 펫, 탈것 리스트 보내줘야함
	DBPacket::TAddMaterializedPet PetList[Pet::Slot::Max];
};

struct TQUseItem:public TQHeader
{
	DBDNWorldDef::UseItem::eUseItemCode	Code;
	INT64 biItemSerial;
	short nUsedItemCount;
	int	iChannelID;
	int	iMapID;
	WCHAR wszIP[IPLENMAX];
	bool bAllFlag;
	int	iTargetLogItemID;
	char cTargetLogItemLevel;
};

struct TQResellItem:public TQHeader
{
	INT64 biCharacterDBID;
	INT64 biItemSerial;
	short nItemCount;
	INT64 biResellPrice;
	int	iNpcID;
	int	iChannelID;
	int	iMapID;
	WCHAR wszIP[IPLENMAX];
	bool bAllFlag;
	bool bRebuyFlag;
#ifdef PRE_MOD_SELL_SEALEDITEM
	bool bBelongFlag;
#endif		//#ifdef PRE_MOD_SELL_SEALEDITEM
};

struct TQEnchantItem:public TQHeader
{
	INT64 biItemSerial;
	INT64 biCoin;						// 강화 비용
	bool bEnchantSuccessFlag;		// 0=실패, 1=성공
	bool bItemDestroyFlag;			// 강화가 실패한 경우 본래 아이템이 파괴되었는지 여부. 0=파괴되지 않음, 1=파괴됨
	BYTE cCurrentItemLevel;			// 강화 후 레벨
	char cCurrentItemPotential;		// 강화 후 아이템 잠재력
	int	iChannelID;
	int	iMapID;
	int	iNpcID;

	//Optional
	TItem NewItem;			// 강화 실패 시 대신 지급하는 아이템 

	BYTE cMaterialCount;
	INT64 biCharacterDBID;			// 강화 소유주
	int	nSupportItemID;				// 사용한 강화 보조 아이템의 ItemID(예..강화보호젤리)
#if defined(PRE_ADD_EXCHANGE_ENCHANT)
	INT64 biExtractItemSerial;			// 강화 이전할때 추출한 원본 아이템(요놈은 DB에서 알아서 강화 레벨 0으로 바꿔줌)
#endif
	DBPacket::TItemCountInfo MaterialArray[INVENTORYMAX];
};

struct TQChangeItemLocation:public TQHeader
{
	INT64 biCharacterDBID;
	INT64 biItemSerial;		// 이동할 아이템 고유 번호
	INT64 biNewItemSerial;	// (optional) 아이템의 일부 수량을 이동한 경우 새로 생성된 아이템 고유 번호.
	int	iItemID;			// 이동할 아이템 ID
	DBDNWorldDef::ItemLocation::eCode Code;	// 이동 후 ItemLocationCode 1=이큅먼트, 2=인벤토리, 3=창고, 4=퀘스트 인벤토리, 5=캐쉬 인벤토리, 6=선물함, 7=캐쉬 이큅먼트
	BYTE cItemLocationIndex;	// 이동 후 아이템 위치 인덱스
	short nItemCount;			// 이동할 아이템 개수
	int	iChannelID;
	int	iMapID;
	bool bVehicleParts;		// 이동하는 아이템이 "탈것 or 펫에 장착하는 아이템"인지 여부. 0=false, 1=true -> 잔챙이(?)만 true로!
	INT64 biVehicleSerial;	// 탈것 or 펫에 장착하는 경우 -> 탈것 or 펫의 아이템 고유 번호
	char cVehicleSlotIndex;	// 탈것 or 펫에 장착하는 경우 -> 장착 위치
	bool bMerge;				// 0=아이템이 merge되지 않음, 1=아이템이 merge됨
	INT64 biMergeTargetItemSerial;	// 생성된 아이템이 목적지에서 merge되는 경우, target 아이템의 아이템 시리얼
	bool bMergeAll;			// 아이템이 merge되는 경우. 0=merge되는 수량이 original의 전체 수량이 아님, 1= merge되는 수량이 original의 전체 수량임
#if defined(PRE_ADD_SERVER_WAREHOUSE)
	int nGoldFee;			// 서버창고 골드 수수료
#endif
};

struct TQSwitchItemLocation: public TQHeader
{
	INT64 biCharacterDBID;
	DBDNWorldDef::ItemLocation::eCode SrcCode;	// 이동 후 ItemLocationCode 1=이큅먼트, 2=인벤토리, 3=창고, 4=퀘스트 인벤토리, 5=캐쉬 인벤토리, 6=선물함, 7=캐쉬 이큅먼트
	BYTE cSrcSlotIndex;
	INT64 biDestItemSerial;
	DBDNWorldDef::ItemLocation::eCode DestCode;	// 이동 후 ItemLocationCode 1=이큅먼트, 2=인벤토리, 3=창고, 4=퀘스트 인벤토리, 5=캐쉬 인벤토리, 6=선물함, 7=캐쉬 이큅먼트
	BYTE cDestSlotIndex;
	INT64 biSrcItemSerial;
};

struct TQModItemDurability:public TQHeader
{
	INT64 biCharacterDBID;
	INT64 biTax;				// 아이템을 수리하는 경우 수리 비용.
	int	iChannelID;
	int	iMapID;
	short nCount;
	INT64 biCurrentCoin;
	INT64 biPickUpCoin;
	DBPacket::TDurabilityInfo ItemDurArr[INVENTORYMAX + EQUIPMAX];
};

struct TQModMaterializedItem:public TQHeader
{
	INT64 biItemSerial;
	int	iChannelID;
	int	iMapID;
	int	iModFieldBitmap;
	USHORT nItemDurability;
	BYTE cItemLevel;
	char cItemPotential;
	bool bSoudBoundFlag;
	BYTE cSealCount;
	BYTE cItemOption;
	WCHAR wszIP[IPLENMAX];
    int nEnchantJewelID;	// 강화코드 아이템아이디
#if defined(PRE_MOD_POTENTIAL_JEWEL_RENEWAL)
	char cItemPotentialMoveCount;	//잠재력 이전 횟수
#endif	// #if defined(PRE_MOD_POTENTIAL_JEWEL_RENEWAL)
};

struct TQGetListMaterializedItem:public TQHeader
{
	INT64 biCharacterDBID;
};

struct TAGetListMaterializedItem:public TAHeader
{
	short nItemCount;
	DBPacket::TMaterialItemInfo	MaterialItemInfoArr[256];	// 현재 최대 개수를 몰라 임시로 256 넣어둔다.
};

struct TQExchangeProperty: public TQHeader
{
	INT64 biCharacterDBID;	// 캐릭터 ID
	DBPacket::TExchangeItem ExchangeItem[EXCHANGEMAX];
	INT64 biExchangeCoin;	// 교환할 코인
	int nExchangeTax;	// 교환 수수료
	INT64 biTargetCharacterDBID;
	DBPacket::TExchangeItem TargetExchangeItem[EXCHANGEMAX];
	INT64 biTargetExchangeCoin;
	int nTargetExchangeTax;	// 교환 수수료
	int nChannelID;
	int nMapID;
	UINT uiTargetAccountDBID;
};

// QUERY_GETPAGEMATERIALIZEDITEM
struct TQGetPageMaterializedItem: public TQHeader
{
	INT64 biCharacterDBID;	// 캐릭터 ID
	int nPageNumber;		// 페이지 번호.
	int nPageSize;			// 페이지 사이즈.
};

struct TAGetPageMaterializedItem: public TAHeader
{
	short wPageNumber;		// 페이지 번호.
	int nTotalListCount;	// 페이지번호 1번일때만 값이 있다 (전체 개수)
	char cCount;
	TItem CashItem[CASHINVENTORYMAX];
};

// QUERY_UPSEQUIP
struct TQUpsEquip: public TQHeader
{
	INT64 biCharacterDBID;	// 캐릭터 ID
	char cItemLocation;
	char cEquipSlotIndex;
	INT64 biSerial;
};

// QUERY_PUTONEPIECEON,		// 원피스 장착
struct TQPutOnepieceOn: public TQHeader
{
	INT64 biCharacterDBID;
	INT64 biOnepieceItemSerial;
	char cOnepieceCashEquipIndex;
	char cCashEquipTotalCount;
	BYTE cSubCashEquipIndexArray[CASHEQUIPMAX];
};

// QUERY_TAKECASHEQUIPMENTOFF,	// 한꺼번에 탈착 (cashequip -> cashinven)
struct TQTakeCashEquipmentOff: public TQHeader
{
	INT64 biCharacterDBID;
	char cCashEquipTotalCount;
	BYTE cCashEquipIndexArray[CASHEQUIPMAX];
};

// QUERY_GETPAGEVEHICLE
struct TQGetPageVehicle: public TQHeader
{
	INT64 biCharacterDBID;	// 캐릭터 ID
	int nPageNumber;		// 페이지 번호.
	int nPageSize;			// 페이지 사이즈.
};

struct TAGetPageVehicle: public TAHeader
{
	short wPageNumber;		// 페이지 번호.
	int nTotalListCount;	// 페이지번호 1번일때만 값이 있다 (전체 개수)
	char cCount;
	TVehicleCompact VehicleItem[VEHICLEINVENTORYPAGEMAX];
};

// QUERY_DELPETEQUIPMENT
struct TQDelPetEquipment: public TQHeader
{
	INT64 biBodySerial;
	INT64 biPartsSerial;
	WCHAR wszIp[IPLENMAX];
};

struct TADelPetEquipment: public TAHeader
{
};

// QUERY_MODPETSKINCOLOR
struct TQModPetSkinColor: public TQHeader
{
	INT64 biBodySerial;
	DWORD dwSkinColor;
	DWORD dwSkinColor2;
};

struct TAModPetSkinColor: public TAHeader
{
};

// QUERY_MISSINGITEMLIST,
struct TAMissingItemList: public TAHeader
{
	char cCount;
	TItem MissingList[MISSINGITEMMAX];
};

// QUERY_RECOVERMISSINGITEM,
struct TQRecoverMissingItem: public TQHeader
{
	INT64 biCharacterDBID;
	//INT64 biItemSerial;
	char cItemLocationCode;
	BYTE cSlotIndex;
	TItem RecoverItem;
};

struct TARecoverMissingItem: public TAHeader
{
	char cItemLocationCode;
	BYTE cSlotIndex;
	TItem RecoverItem;
};

struct TQGetListRepurchaseItem:public TQHeader
{
	INT64 biCharacterDBID;
};

struct TAGetListRepurchaseItem:public TAHeader
{
	BYTE cCount;
	TRepurchaseItemInfo ItemList[Shop::Repurchase::MaxList];
};

struct TQDelExpiritem:public TQHeader
{
	INT64 biCharacterDBID;	
};
struct TADelExpiritem:public TAHeader
{	
	BYTE cCount;
	INT64 biItemSerial[MAXDELEXPIREITEM];		
};

struct TQDelCashItem: public TQHeader	// QUERY_DELCASHITEM,
{
	INT64 biItemSerial;
	short wCount;
	int nWatingTime;	// 제거 대기 기간. 단위 : 분
};

struct TADelCashItem: public TAHeader
{
	INT64 biItemSerial;
	__time64_t tExpireDate;
};

struct TQRecoverCashItem: public TQHeader	// QUERY_RECOVERCASHITEM,
{
	INT64 biItemSerial;
	short wCount;
};

struct TARecoverCashItem: public TAHeader
{
	INT64 biItemSerial;
	bool bEternity;
	__time64_t tExpireDate;
};

#if defined( PRE_ADD_NAMEDITEM_SYSTEM )
struct TQCheckNamedItemCount: public TQHeader	// QUERY_CHECK_NAMEDITEMCOUNT
{
	int nItemID;	
	BYTE cIndex;
};
struct TACheckNamedItemCount: public TAHeader
{
	int nItemID;
	int nCount;
	BYTE cIndex;
};
#endif

#if defined (PRE_ADD_EXCHANGE_POTENTIAL)
struct TQMovePotential : public TQHeader
{
	INT64 biCharacterDBID;
	INT64 biExtractSerial;
	char cExtractPotential;
	INT64 biInjectSerial;
	char cInjectPotential;
	char cMoveableLimitCount;
	int nMapID;
};

struct TAMovePotential : public TAHeader
{
};
#endif		//#if defined (PRE_ADD_EXCHANGE_POTENTIAL)

// QUERY_MODDEGREEOFHUNGER
struct TQModDegreeOfHunger : public TQHeader
{
	INT64 biBodySerial;
	DWORD nDegreeOfHunger;
};

struct TQItemExpire : public TQHeader
{
	INT64 biCharacterDBID;
	INT64 biSerial;
};

struct TAItemExpire : public TAHeader
{	
	INT64 biSerial;
};

struct TQChangePetName : public TQHeader
{
	INT64 itemSerial;
	INT64 petSerial;
	WCHAR name[NAMELENMAX];
};

struct TAChangePetName : public TAHeader
{
	INT64 itemSerial;
	INT64 petSerial;
	WCHAR name[NAMELENMAX];
};

struct TQClearPetVehicleInven : public TQHeader
{
	INT64 biCharacterDBID;
};

struct TAClearPetVehicleInven : public TAHeader
{

};

struct TQModItemExpireDate : public TQHeader
{
	int	iMapID;
	WCHAR wszIP[IPLENMAX];
	INT64 biItemSerial;
	INT64 biExpireDateItemSerial;
	INT64 biCharacterDBID;
	int	nMin;
};

struct TAModItemExpireDate : public TAHeader
{
	INT64 biCharacterDBID;
	INT64 biItemSerial;
	int nMin;
};

struct TQModPetExp : public TQHeader
{
	INT64 biPetSerial;
	int nExp;
};

struct TAModPetExp : public TAHeader
{

};

struct TQModPetSkill : public TQHeader
{
	INT64 biItemSerial;
	char cSkillSlot;
	int nSkillID;
};

struct TQModPetSkillExpand : public TQHeader
{
	INT64 biItemSerial;
};

struct TQModAdditiveItem:public TQHeader
{
	INT64	biCharacterDBID;
	int		nAdditiveItemID;

	INT64	biItemSerial;		// 능력치 아이템
	INT64	biAdditiveSerial;	// 디자인 아이템
	INT64	biMixItemSerial;	// 합성기

	// 아이템 제거용 정보
	int		nMapID;
	WCHAR	wszIP[IPLENMAX];	
};

struct TAModAdditiveItem:public TAHeader
{
	INT64	biItemSerial;
	int		nAdditiveItemID;
	int		nInvenIndex;
	INT64	biAdditiveSerial;
};

#if defined (PRE_ADD_COSRANDMIX)
struct TQModRandomItem:public TQHeader
{
	INT64 biCharacterDBID;

	INT64 biStuffSerialA;
	INT64 biStuffSerialB;
	INT64 biMixItemSerial;	// 합성기
	short wMixItemCount;

	INT64 biFee;
	DBDNWorldDef::AddMaterializedItem::eCode Code;
	TItem RandomItem;
	BYTE cItemLocationCode;

	int		nChannelID;
	int		nMapIndex;
	WCHAR	wszIP[IPLENMAX];	
};

struct TAModRandomItem:public TAHeader
{
	INT64 biStuffSerialA;
	INT64 biStuffSerialB;
	INT64 biMixItemSerial;	// 합성기
	INT64 biFee;
	TItem RandomItem;
};
#endif

// QUERY_SAVEITEMLOCATIONINDEX,	// P_SaveItemLocationIndex
struct TQSaveItemLocationIndex: public TQHeader
{
	INT64 biCharacterDBID;
	int nTotalInvenWareCount;	// 인벤/창고 구분없이 순서만 지켜서 넣으면 된다. 
	DBPacket::TItemIndexSerial SaveList[INVENTORYMAX + WAREHOUSEMAX];
};

#if defined( PRE_ADD_LIMITED_SHOP )
struct TQGetLimitedShopItem: public TQHeader
{
	INT64 biCharacterDBID;	
};
struct TAGetLimitedShopItem:public TAHeader
{
	int nCount;
	LimitedShop::LimitedItemData ItemData[INVENTORYMAX + WAREHOUSEMAX];
};
struct TQAddLimitedShopItem: public TQHeader
{
	INT64 biCharacterDBID;
	int nBuyCount;
	LimitedShop::LimitedItemData ItemData;
};
struct TQResetLimitedShopItem: public TQHeader
{
	INT64 biCharacterDBID;
	int nResetCycle;
};
#endif

struct TQDelEffectItem: public TQHeader
{
	INT64 biCharacterDBID;
	INT64 nItemSerial;	
};

#if defined(PRE_ADD_TALISMAN_SYSTEM)
struct TQModTalismanSlotOpenFlag: public TQHeader
{
	INT64 biCharacterDBID;
	int nTalismanOpenflag;	
};
#endif

#if defined(PRE_ADD_CHNC2C)
struct TQGetGameMoney : public TQHeader
{
	INT64 biCharacterDBID;
	char szSeqID[64];
};

struct TAGetGameMoney : public TAHeader
{
	INT64 biCharacterDBID;
	INT64 biCoinBalance;
	char szSeqID[64];
};

struct TQKeepGameMoney : public TQHeader
{	
	INT64 biCharacterDBID;
	INT64 biReduceCoin;
	char szBookID[50];
	char szSeqID[64];
};

struct TAKeepGameMoney : public TAHeader
{
	char cWorldSetID;
	INT64 biCharacterDBID;
	INT64 biReduceCoin;
	char szSeqID[64];
};

struct TQTransferGameMoney : public TQHeader
{
	INT64 biCharacterDBID;
	INT64 biAddCoin;
	char szBookID[50];
	char szSeqID[64];
};

struct TATransferGameMoney : public TAHeader
{
	char cWorldSetID;
	INT64 biCharacterDBID;
	INT64 biAddCoin;
	char szSeqID[64];
};

#endif //#if defined(PRE_ADD_CHNC2C)

#if defined(PRE_ADD_EQUIPLOCK)
struct TQLockItemInfo: public TQHeader	//잠금, 잠금해제
{
	INT64 biCharacterDBID;
	DBDNWorldDef::ItemLocation::eCode Code;
	BYTE cItemSlotIndex;
	int nItemID;
	INT64 biItemSerial;
};

struct TALockItemInfo: public TAHeader //잠금 요청시 사용함
{
	DBDNWorldDef::ItemLocation::eCode Code;
	BYTE cItemSlotIndex;
	__time64_t tLockDate;
};

struct TQUnLockRequsetItemInfo: public TQHeader	//잠금해제 요청시 사용함
{
	INT64 biCharacterDBID;
	DBDNWorldDef::ItemLocation::eCode Code;
	BYTE cItemSlotIndex;
	int nItemID;
	int nUnLockWaitTime;
	INT64 biItemSerial;
};

struct TAUnLockRequestItemInfo: public TAHeader //잠금해제 요청시 사용함
{
	DBDNWorldDef::ItemLocation::eCode Code;
	BYTE cItemSlotIndex;
	__time64_t tUnLockDate;
	__time64_t tUnLockRequestDate;
};

struct TQGetListLockedItems: public TQHeader
{
	INT64 biCharacterDBID;
};

struct TAGetListLockedItems: public TAHeader
{
	int nCount;
	TDBLockItemInfo LockItem[EQUIPMAX+ CASHEQUIPMAX];
};
#endif	// #if defined(PRE_ADD_EQUIPLOCK)


//#########################################################################################################################################
//	MAINCMD_QUEST
//#########################################################################################################################################

// QUERY_ADDQUEST
struct TQAddQuest:public TQHeader
{
	INT64 biCharacterDBID;
	char cIdx;
	TQuest Quest;
};

// QUERY_DELQUEST
struct TQDelQuest:public TQHeader
{
	INT64 biCharacterDBID;
	char cIdx;
	int nQuestID;
	bool bComplete;		// 0=단순 퀘스트 삭제, 1=퀘스트 수행 완료로 인한 삭제
	bool bRepeat;		// 0=1회 퀘스트, 1=반복 퀘스트
	char CompleteQuest[COMPLETEQUEST_BITSIZE];
};

// QUERY_MODQUEST,					// 퀘스트 수정
struct TQModQuest:public TQHeader
{
	INT64 biCharacterDBID;
	char cQuestStatusCode;
	char cIndex;
	TQuest Quest;
};

// QUERY_CLEARQUEST
struct TQClearQuest:public TQHeader
{
	INT64 biCharacterDBID;
};


// QUERY_FORCECOMPLETEQUEST
struct TQForceCompleteQuest : public TQHeader
{
public:
	TQForceCompleteQuest()
	{
		Reset();
	}
	void Set(BYTE pThreadID, char pWorldID, UINT pAccountDBID, INT64 pCharacterDBID, int pQuestID, int pQuestCode, char pCompleteQuest[COMPLETEQUEST_BITSIZE])
	{
		cThreadID = pThreadID;
		cWorldSetID = pWorldID;
		nAccountDBID = pAccountDBID;
		biCharacterDBID = pCharacterDBID;
		nQuestID = pQuestID;
		nQuestCode = pQuestCode;
		::memcpy_s(CompleteQuest, sizeof(CompleteQuest), pCompleteQuest, sizeof(CompleteQuest));
	}
	void Reset()
	{
		cThreadID = 0;
		cWorldSetID = 0;
		nAccountDBID = 0;
		biCharacterDBID = 0;
		nQuestID = 0;
		nQuestCode = 0;
	}
	int GetSize() const
	{ 
		return(static_cast<int>(sizeof(*this)));
	}
public:
	INT64 biCharacterDBID;						// 캐릭터 ID
	int nQuestID;								// 퀘스트 ID
	int nQuestCode;								// 퀘스트 코드 (0:일반 / 1:시간제)
	char CompleteQuest[COMPLETEQUEST_BITSIZE];	// 완료된 퀘스트 바이너리 목록
};


// QUERY_DELETE_CHARACTER_PERIODQUEST
struct TQDeletePeriodQuest : public TQHeader
{
	char cWorldID;
	INT64 biCharacterDBID;
	int nPeriodQuestType;
	int nQuestCount;
	int nQuestIDs[MAX_PERIODQUEST_LIST];
};

struct TADeletePeriodQuest : public TAHeader
{
	char cWorldID;
	INT64 biCharacterDBID;
	__time64_t tAssignDate;

	int nPeriodQuestType;
	int nQuestCount;
	int nQuestIDs[MAX_PERIODQUEST_LIST];
};

// QUERY_GET_CHARACTER_PERIODQUESTDATE
struct TQGetPeriodQuestDate : public TQHeader
{
	char cWorldID;
	INT64 biCharacterDBID;
};

struct TAGetPeriodQuestDate : public TAHeader
{
	char cWorldID;
	INT64 biCharacterDBID;

	int nCount;
	TPeriodQuestDate PeriodQuestDate[PERIODQUEST_RESET_MAX];
};

// QUERY_GET_LISTCOMPLETE_EVENTQUEST
struct TQGetListCompleteEventQuest : public TQHeader
{
	char cWorldID;
	INT64 biCharacterDBID;
};

struct TAGetListCompleteEventQuest : public TAHeader
{
	char cWorldID;
	INT64 biCharacterDBID;

	int nCount;
	TCompleteEventQuest EventQuestList[EVENTQUESTMAX];
};

// QUERY_COMPLETE_EVENTQUEST
struct TQCompleteEventQuest : public TQHeader
{
	char cWorldID;
	INT64 biCharacterDBID;

	int nScheduleID;
	__time64_t tExpireDate;
};

struct TACompleteEventQuest : public TAHeader
{
	char cWorldID;
	INT64 biCharacterDBID;

	int nScheduleID;
	short wCompleteCount;
};

// QUERY_GET_WORLDEVENTQUESTCOUNTER
struct TQGetWorldEventQuestCounter : public TQHeader
{
	int nScheduleID;
};

struct TAGetWorldEventQuestCounter : public TAHeader
{
	char cWorldSetID;
	int nScheduleID;
	int nCount;
};

// QUERY_MOD_WORLDEVENTQUESTCOUNTER
struct TQModWorldEventQuestCounter : public TQHeader
{
	INT64 biCharacterDBID;
	int nScheduleID;
	int nCounterDelta;
};

struct TAModWorldEventQuestCounter : public TAHeader
{
	char cWorldSetID;
	int nScheduleID;
	int nCounterDelta;
	int nCountAfter;
};

//#########################################################################################################################################
//	MAINCMD_MAIL
//#########################################################################################################################################

// QUERY_GETCOUNTRECEIVEMAIL
struct TQGetCountReceiveMail:public TQHeader
{
	INT64 biReceiverCharacterDBID;
};

struct TAGetCountReceiveMail:public TAHeader
{
	int	iTotalMailCount;
	int	iNotReadMailCount;
	int	i7DaysLeftMailCount;
};

// QUERY_GETPAGERECEIVEMAIL
struct TQGetPageReceiveMail:public TQHeader
{
	int	iPageNumber;
	INT64 biReceiverCharacterDBID;
};

struct TAGetPageReceiveMail:public TAHeader
{
	int	iBasicMailCount;

	int	iTotalMailCount;
	int	iNotReadMailCount;
	int	i7DaysLeftMailCount;

	BYTE cMailCount;
	TMailBoxInfo sMail[MAILPAGEMAX];
};

// QUERY_SENDMAIL
struct TQSendMail:public TQHeader
{
	INT64 biSenderCharacterDBID;
	wchar_t	wszReceiverCharacterName[MAILNAMELENMAX];
	MailType::Delivery Code;
	wchar_t wszSubject[MAILTITLELENMAX];
	wchar_t wszContent[MAILTEXTLENMAX];
	INT64 biAttachCoin;
	int	iChannelID;
	int	iMapID;
	int	nTax;								// 수수료
	BYTE cAttachItemCount;
	DBPacket::TSendMailItem AttachItemArr[MAILATTACHITEMMAX];
};

struct TASendMail:public TAHeader
{
	UINT nReceiverAccountDBID;
	INT64 biReceiverCharacterDBID;
	int nReceiverTotalMailCount;
	int	nReceiverNotReadMailCount;
	int	nReceiver7DaysLeftMailCount;
	int nBasicMailCount;
	MailType::Delivery Code;

	INT64 biAttachCoin;
	int nTax;								// 수수료
	BYTE cAttachItemCount;
	DBPacket::TSendMailItem AttachItemArr[MAILATTACHITEMMAX];
};

// QUERY_SENDSYSTEMMAIL
struct TQSendSystemMail:public TQHeader
{
	wchar_t	wszSenderName[MAILNAMELENMAX];
	INT64 biReceiverCharacterDBID;
	DBDNWorldDef::MailTypeCode::eCode Code;
	INT64 biFKey;
	wchar_t	wszSubject[MAILTITLELENMAX];
	wchar_t wszContent[MAILTEXTLENMAX];
	INT64 biAttachCoin;
	int	iChannelID;
	int	iMapID;
	bool bNewFlag;
	char cAttachItemCount;
	TItem sAttachItem[MAILATTACHITEMMAX];
};

struct TASendSystemMail:public TAHeader
{
	INT64 biReceiverCharacterDBID;
	int nMailDBID;
#if defined( PRE_ADD_NPC_REPUTATION_SYSTEM )
	BYTE cWorldSetID;
#endif // #if defined( PRE_ADD_NPC_REPUTATION_SYSTEM )
	int	iTotalMailCount;
	int	iNotReadMailCount;
	int	i7DaysLeftMailCount;
};

// QUERY_READMAIL
struct TQReadMail:public TQHeader
{
	int iMailID;
};

struct TAReadMail:public TAHeader
{
	int	nMailDBID;							// mail db table id
	int	iNotReadMailCount;
	wchar_t	wszContent[MAILTEXTLENMAX];
	__time64_t SendDate;
	INT64 biAttachCoin;
	char cDeliveryType;
	bool bNewFlag;		// 리뉴얼된 시스템 메일 여부
	BYTE cMailType;
	BYTE cAttachItemCount;
	TItem sAttachItem[MAILATTACHITEMMAX];
};

// QUERY_GETLISTMAILATTACHMENT
struct TQGetListMailAttachment: public TQHeader
{
	INT64 biCharacterDBID;
	int nMailDBIDArray[MAILPAGEMAX];
};

struct TAGetListMailAttachment: public TAHeader
{
	char cCount;
	DBPacket::TAttachMail MailAttachArray[MAILPAGEMAX];
};

// QUERY_TAKEATTACHMAILLIST
struct TQTakeAttachMailList: public TQHeader
{
	int nChannelID;
	int	nMapID;
	WCHAR wszIP[IPLENMAX];
#ifdef PRE_ADD_JOINGUILD_SUPPORT
	UINT nGuildDBID;
#endif		//PRE_ADD_JOINGUILD_SUPPORT
	char cPageCount;
	DBPacket::TRequestTakeAttachInfo Info[MAILPAGEMAX];
};

struct TATakeAttachMailList: public TAHeader
{
	char cPageCount;
	DBPacket::TTakeAttachInfo TakeMailList[MAILPAGEMAX];
};

// QUERY_TAKEATTACHMAIL
struct TQTakeAttachMail: public TQHeader
{
	int nChannelID;
	int	nMapID;
	WCHAR wszIP[IPLENMAX];
#ifdef PRE_ADD_JOINGUILD_SUPPORT
	UINT nGuildDBID;
#endif		//PRE_ADD_JOINGUILD_SUPPORT
	DBPacket::TRequestTakeAttachInfo Info;
};

struct TATakeAttachMail: public TAHeader
{
	char cAttachSlotIndex;		// -1: 5칸 모두 받기, 0 - 4: 개별받기
	DBPacket::TTakeAttachInfo TakeMail;
};

// QUERY_DELMAIL
struct TQDelMail:public TQHeader
{
	INT64 biReceiverCharacterDBID;
	int	iMailIDArray[MAILPAGEMAX];
};

struct TADelMail:public TAHeader
{
	int	iTotalMailCount;
};

// QUERY_GETWILLSENDMAIL
struct TQGetWillMails : public TQHeader
{
	INT64 biCharacterDBID;
};

struct TAGetWillMails : public TAHeader
{
	BYTE cCount;
	WillSendMail::TWillSendInfo Mails[WillSendMail::Common::WIllMailPacketMax];
};

// QUERY_DELWILLSENDMAIL
struct TQDelWillMail : public TQHeader
{
	INT64 biCharacterDBID;
	int nMailID;
};

// QUERY_SENDWISHMAIL,					// 조르기 우편 발송
struct TQSendWishMail: public TQHeader
{
	INT64 biSenderCharacterDBID;
	WCHAR wszReceiverCharacterName[MAILNAMELENMAX];
	WCHAR wszSubject[MAILTITLELENMAX];
	WCHAR wszContent[MAILTEXTLENMAX];
	int nWishLimitCount;	// 1일 가능 조르기 횟수
	int nPackageSN;
	char cWishListCount;
	DBPacket::TItemSNIDOption WishList[PACKAGEITEMMAX];
};

struct TASendWishMail: public TAHeader
{
	UINT nReceiverAccountDBID;
	INT64 biReceiverCharacterDBID;
	int nReceiverTotalMailCount;
	int	nReceiverNotReadMailCount;
	int	nReceiver7DaysLeftMailCount;
};

// QUERY_READWISHMAIL,					// 조르기 우편 읽기
struct TQReadWishMail: public TQHeader
{
	int nMailID;
};

struct TAReadWishMail: public TAHeader
{
	int	nMailDBID;							// mail db table id
	wchar_t	wszContent[MAILTEXTLENMAX];
	__time64_t SendDate;
	INT64 biPurchaseOrderID;
	int	nNotReadMailCount;
	bool bNewFlag;		// 리뉴얼된 시스템 메일 여부
	int nPackageSN;
	char cWishListCount;
	DBPacket::TItemSNIDOption WishList[PACKAGEITEMMAX];
};

#if defined(PRE_SPECIALBOX)
// QUERY_ADDEVENTREWARD
struct TQAddEventReward: public TQHeader
{
	BYTE cReceiveTypeCode;		// SpecialBox::ReceiveTypeCode
	BYTE cTargetTypeCode;		// SpecialBox::TargetTypeCode
	WCHAR wszAccountName[IDLENMAX];
	INT64 biCharacterDBID;
	WCHAR wszCharacterName[NAMELENMAX];
	BYTE cTargetClassCode;
	BYTE cTargetMinLevel;
	BYTE cTargetMaxLevel;
	int nEventNameUIString;
	int nSenderNameUIString;
	int nContentUIString;
	int nRewardCoin;
	char cEventRewardTypeCode;	// eEventRewardType
	BYTE cCount;
	TSpecialBoxItemInfo EventRewardItems[SpecialBox::Common::RewardSelectMax];
};

struct TAAddEventReward: public TAHeader
{
	int nEventRewardID;
	int nEventTotalCount;
};

// QUERY_GETCOUNTEVENTREWARD
struct TQGetCountEventReward: public TQHeader
{
	INT64 biCharacterDBID;
};

struct TAGetCountEventReward: public TAHeader
{
	int nEventTotalCount;
};

// QUERY_GETLISTEVENTREWARD
struct TQGetListEventReward: public TQHeader
{
	INT64 biCharacterDBID;
	BYTE cClassCode;
	int nLevel;
};

struct TAGetListEventReward: public TAHeader
{
	BYTE cCount;
	TSpecialBoxInfo SpecialBoxInfo[SpecialBox::Common::ListMax];
};

// QUERY_GETLISTEVENTREWARDITEM
struct TQGetListEventRewardItem: public TQHeader
{
	int nEventRewardID;
	INT64 biCharacterDBID;
};

struct TAGetListEventRewardItem: public TAHeader
{
	int nEventRewardID;
	char cCharacterCount;
	BYTE cCount;
	TSpecialBoxItemInfo SpecialBoxItem[SpecialBox::Common::RewardSelectMax];
};

// QUERY_ADDEVENTREWARDRECEIVER
struct TQAddEventRewardReceiver: public TQHeader
{
	int nEventRewardID;
	int nItemID;
	INT64 biCharacterDBID;
};

struct TAAddEventRewardReceiver: public TAHeader
{
	int nEventRewardID;
	int nItemID;
};
#endif	// #if defined(PRE_SPECIALBOX)

#ifdef PRE_ADD_JOINGUILD_SUPPORT
// QUERY_GUILDSUPPORT_REWARDINFO
struct TQGuildSupportRewardInfo:public TQHeader
{
	INT64 biCharacterDBID;
};

struct TAGuildSupportRewardInfo:public TAHeader
{
	INT64 biCharacterDBID;
	bool bWasGuildSupportRewardFlag;
	BYTE cJoinGuildLevel;
};

// QUERY_SENDGUILDMAIL
struct TQSendGuildMail:public TQHeader
{
	wchar_t	wszSenderName[MAILNAMELENMAX];
	UINT nReceiverAccountDBID;
	INT64 biReceiverCharacterDBID;
	INT64 biAchivementCharacterDBID;
	DBDNWorldDef::MailTypeCode::eCode Code;
	INT64 biFKey;
	wchar_t	wszSubject[MAILTITLELENMAX];
	wchar_t wszContent[MAILTEXTLENMAX];
	INT64 biAttachCoin;
	int	iChannelID;
	int	iMapID;
	bool bNewFlag;
	char cAttachItemCount;
	TItem sAttachItem[MAILATTACHITEMMAX];
};

struct TASendGuildMail:public TAHeader
{
	UINT nReceiverAccountDBID;
	INT64 biReceiverCharacterDBID;
	int nMailDBID;
	BYTE cWorldSetID;
	int	iTotalMailCount;
	int	iNotReadMailCount;
	int	i7DaysLeftMailCount;
};
#endif		//#ifdef PRE_ADD_JOINGUILD_SUPPORT


//#########################################################################################################################################
//	MAINCMD_MARKET
//#########################################################################################################################################

// QUERY_MARKETTAX
struct TQMarketTax:public TQHeader
{
	INT64 biCharacterDBID;
	int nMarketRegisterTax;					// 마켓 누적 등록 수수료
	int nMarketSellTax;						// 마켓 누적 판매 수수료
};

// QUERY_GETPAGETRADE
struct TQGetPageTrade: public TQHeader
{
	short wPageNum;						// 페이지 번호
	char cMinLevel;						// 최소레벨
	char cMaxLevel;						// 최대레벨
	char cItemGrade[ITEMRANK_MAX];		// 등급분류
	char cJob;							// 직업구분
	short nMainType;					// 기본분류
	char cDetailType;					// 상세분류
	WCHAR wszSearchWord1[SEARCHLENMAX];	// 검색단어
	WCHAR wszSearchWord2[SEARCHLENMAX];	// 검색단어
	WCHAR wszSearchWord3[SEARCHLENMAX];	// 검색단어
	char cSortType;						// eSortType
	char cItemExchange[ITEMCATAGORY_MAX]; // 아이템 카테고리
	WCHAR wszSearchItemName[SEARCHLENMAX];	// 아이템 이름
	char cJobCode[ITEMJOB_MAX];			// 직업 코드
	bool bCash;							// 0=일반 아이템, 1=캐쉬 아이템
	char cMethodCode;					// 1=게임머니, 3=페탈
};

struct TAGetPageTrade: TAHeader
{
	int nMarketTotalCount;
	char cMarketCount;
	TMarketInfo MarketInfo[MARKETMAX];
};

// QUERY_GETLISTMYTRADE
struct TQGetListMyTrade:public TQHeader
{
	INT64 biCharacterDBID;
};

struct TAGetListMyTrade: public TAHeader
{
	short wSellingCount;	// 판매중
	short wExpiredCount;
	short wWeeklyRegisterCount;		// 토요일 AM 04:00 를 기준으로 일주일간 등록한 거래 횟수 / 지금등록할수있는 횟수
	short wRegisterItemCount;		// 올려놓은 아이템 갯수
	char cMarketCount;
	TMyMarketInfo MarketInfo[MYMARKETMAX];
};

// QUERY_ADDTRADE
struct TQAddTrade: public TQHeader
{
	INT64 biCharacterDBID;			// 캐릭터 ID
	short wRegisterMaxCount;		// 등록 가능 횟수
	short wItemRegisterMaxCount;	// 등록 가능 개수
	char cInvenType;				// eItemPositionType
	BYTE cInvenIndex;				// 인벤인덱스
	char cSkillUsingType;			// 판매 가격
	int nPrice;						// 거래소등록 수수료
	int nSellTax;					// 거래성립 수수료 (팔린물건값 찾을때 수수료)
	char cSellPeriod;				// 판매기간
	WCHAR wszItemName[EXTDATANAMELENMAX];	// 아이템 이름
	short nMainType;				// 메인타입
	char cDetailType;				// 상세타입
	char cItemGrade;				// 0=노멀, 1=매직, 2=레어, 3=에픽, 4=유니크, 5=히로익
	char cLimitLevel;				// 아이템을 사용하기 위한 최소 캐릭터 레벨
	char cJob;						// 직업 코드
	int nItemID;					// 등록한 아이템 ID
	INT64 nSerial;					// 등록할 아이템 고유번호
	short wCount;					// 등록할 아이템 수량
	char cLevel;					// 등록할 아이템 레벨
	INT64 biNewSerial;				// 아이템의 일부 수량만 등록하는 경우 새 ItemSerial을 입력하고 그렇지 않으면 NULL을 입력합니다
	int nUnitPrice;					// 판매단가
	int nRegisterTax;				// 거래소등록 수수료
	bool bDisplayFlag1;				// 0을 입력해 주세요. 현재 사용하지 않는 필드
	bool bDisplayFlag2;				// 0을 입력해 주세요. 현재 사용하지 않는 필드
	int nChannelID;					// 채널 ID
	int nMapID;						// 맵 ID
	bool bPremiumTrade;				// 0=프리미엄 아님, 1=프리미엄
	bool bCash;						// 0=일반 아이템, 1=캐쉬 아이템
	char cMethodCode;				// 1=게임머니, 3=페탈
	int nExchangeCode;				// 아이템이 속한 카테고리 ID
};

struct TAAddTrade: TAHeader
{
	int nMarketDBID;			// 거래소 거래 ID
	char cInvenType;			// eItemPositionType
	INT64 biSerial;				// 시리얼
	BYTE cInvenIndex;			// 인벤 인덱스
	short wCount;				// 아이템 갯수
	int nPrice;					// 거래소 등록 수수료
	bool bPremiumTrade;			// 0=프리미엄 아님, 1=프리미엄
#if defined(PRE_ADD_PETALTRADE)
	char cMethodCode;			// 1=게임머니, 3=페탈
#endif
};

// QUERY_CANCELTRADE
struct TQCancelTrade:public TQHeader
{
	INT64 biCharacterDBID;		// 캐릭터 ID
	int nMarketDBID;			// 거래 ID
	BYTE cInvenSlotIndex;		// 아이템이 들어갈 인덱스 번호
	int nChannelID;				// 채널 ID
	int nMapID;					// 맵 ID
	bool bCashItem;				
};

struct TACancelTrade: public TAHeader
{
	int nMarketDBID;
	BYTE cInvenSlotIndex;
	TItem Item;
	int nPrice;
	bool bCashItem;
};

// QUERY_BUYTRADEITEM
struct TQBuyTradeItem: public TQHeader
{
	INT64 biCharacterDBID;
	int nMarketDBID;
	BYTE cInvenSlotIndex;
	int nChannelID;
	int nMapID;
	bool bCashItem;
#if defined(PRE_ADD_DIRECT_BUY_UPGRADEITEM)
	bool bMini;
#else	// #if defined(PRE_ADD_DIRECT_BUY_UPGRADEITEM)
#if defined(PRE_ADD_PETALTRADE)
	int nPetalPrice;
#endif
#endif	// #if !defined(PRE_ADD_DIRECT_BUY_UPGRADEITEM)
	WCHAR wszIP[IPLENMAX];
};

struct TABuyTradeItem: public TAHeader
{
	int nMarketDBID;
	BYTE cInvenSlotIndex;
	TItem Item;
	UINT nSellerADBID;	// 판매자 ADBID
	INT64 biSellerCharacterDBID;	// 판매자 CDBID
	int nPrice;
	char cPayMethodCode;
#if defined(PRE_ADD_DIRECT_BUY_UPGRADEITEM)
	bool bMini;
#endif	// #if defined(PRE_ADD_DIRECT_BUY_UPGRADEITEM)
};

// QUERY_GETLISTTRADEFORCALCULATION
struct TQGetListTradeForCalculation: public TQHeader
{
	INT64 biCharacterDBID;
};

struct TAGetListTradeForCalculation: public TAHeader
{
	char cMarketCount;
	TMarketCalculationInfo MarketInfo[MYMARKETMAX];
};

// QUERY_TRADECALCULATE
struct TQTradeCalculate: public TQHeader
{
	INT64 biCharacterDBID;
	int nMarketDBID;
	int nChannelID;
	int nMapID;
};

struct TATradeCalculate: public TAHeader
{
	int nMarketDBID;
	int nCalculatePrice;
#if defined(PRE_ADD_PETALTRADE)
	char cPayMethodCode;
#endif
};

// QUERY_TRADECALCULATEALL
struct TQTradeCalculateAll: public TQHeader
{
	INT64 biCharacterDBID;
	int nChannelID;
	int nMapID;
};

struct TATradeCalculateAll: public TAHeader
{
	int nCalculatePrice;
#if defined(PRE_ADD_PETALTRADE)
	int nCalculatePetal;
#endif
};

#if !defined(PRE_ADD_DIRECT_BUY_UPGRADEITEM)
// QUERY_GETTRADEITEMID,				// MarketDBID로 ItemID알아내기
struct TQGetTradeItemID: public TQHeader
{
	int nMarketDBID;
};
#endif	// #if defined(PRE_ADD_DIRECT_BUY_UPGRADEITEM)

struct TAGetTradeItemID: public TAHeader
{
	int nMarketDBID;
	int nMarketItemID;
#if defined(PRE_ADD_PETALTRADE)
	char cMethodCode;
	int nPrice;
#endif
};

// QUERY_GETPETALBALANCE,				// 페탈 잔액 조회
struct TAGetPetalBalance : public TAHeader
{
	int nPetalBalance;
};

// QUERY_GETTRADEPRICE
struct TQGetTradePrice : public TQHeader		
{
	int nMarketDBID;
	int nItemID;
	BYTE cLevel;
	BYTE cOption;
};

struct TAGetTradePrice : public TAHeader
{
	int nMarketDBID;
	int nItemID;
	BYTE cLevel;
	BYTE cOption;

	TMarketPrice ItemPrices[2];		// Index 0 : 게임머니 1: 페탈
};

//QUERY_GETLISTMINITRADE
struct TQGetListMiniTrade: public TQHeader
{
	INT64 biCharacterDBID;
	int nItemID;
	char cPayMethodCode;					// 0-혼합, 1-코인, 3-페탈
};

typedef TAGetPageTrade TAGetListMiniTrade;


//#########################################################################################################################################
//	MAINCMD_FRIEND
//#########################################################################################################################################

//QUERY_FRIENDLIST,
struct TQFriendList:public TQHeader
{
	INT64 biCharacterDBID;
};

struct TDBFriendGroupData
{
	UINT nGroupDBID;
	WCHAR wszGroupName[FRIEND_GROUP_NAMELENMAX];
};

struct TDBFriendData
{
	UINT nGroupDBID;
	WCHAR wszGroupName[FRIEND_GROUP_NAMELENMAX];
	INT64 biCharacterDBID;
	UINT nFriendAccountDBID;
	INT64 biFriendCharacterDBID;
	WCHAR wszFriendCharacterName[NAMELENMAX];
	WCHAR wszMemo[FRIEND_MEMO_LEN_MAX];
	bool bMobileAuthentication;
};

struct TAFriendList: public TAHeader
{
	INT64 biCharacterDBID;
	BYTE cCount;
	TDBFriendData FriendData[FRIEND_MAXCOUNT + FRIEND_GROUP_MAX];
};

// QUERY_ADDGROUP,					// 그룹 추가
struct TQAddGroup:public TQHeader
{
	INT64 biCharacterDBID;
	WCHAR wszGroupName[FRIEND_GROUP_NAMELENMAX];
	int nGroupCountLimit;		// 친구 그룹 개수 상한치
};

struct TAAddGroup: public TAHeader
{
	WCHAR wszGroupName[FRIEND_GROUP_NAMELENMAX];	
	UINT nGroupDBID;
};

// QUERY_MODGROUPNAME,				// 그룹이름 수정
struct TQModGroupName:public TQHeader
{
	INT64 biCharacterDBID;
	int nGroupDBID;
	WCHAR wszGroupName[FRIEND_GROUP_NAMELENMAX];
};

struct TAModGroupName: public TAHeader
{
	UINT nGroupDBID;
	WCHAR wszGroupName[FRIEND_GROUP_NAMELENMAX];	
};

// QUERY_DELGROUP,					// 그룹삭제
struct TQDelGroup:public TQHeader
{
	INT64 biCharacterDBID;
	UINT nGroupDBID;
};

struct TADelGroup: public TAHeader
{
	UINT nGroupDBID;
};

// QUERY_MODFRIENDANDGROUPMAPPING,	// 친구와 친구그룹 메핑수정
struct TQModFriendAndGroupMapping:public TQHeader
{
	INT64 biCharacterDBID;
	int nGroupDBID;
	char cCount;
	INT64 biFriendCharacterDBIDs[FRIEND_MAXCOUNT];
};

struct TAModFriendAndGroupMapping: public TAHeader
{
	int nGroupDBID;
	char cCount;
	INT64 biFriendCharacterDBIDs[FRIEND_MAXCOUNT];
};

//QUERY_ADDFRIEND,
struct TQAddFriend:public TQHeader
{
	INT64 biCharacterDBID;

	WCHAR wszFriendName[NAMELENMAX];
	UINT nGroupDBID;
	int nFriendCountLimit;	// 등록 가능한 친구 수 상한치
};

struct TAAddFriend: public TAHeader
{
	INT64 biCharacterDBID;
	UINT nFriendAccountDBID;
	INT64 biFriendCharacterDBID;
	UINT nGroupDBID;
	WCHAR wszFriendName[NAMELENMAX];
	BYTE cNeedDelIsolate;
};

// QUERY_MODFRIENDMEMO,			// 친구메모수정
struct TQModFriendMemo:public TQHeader
{
	INT64 biCharacterDBID;
	INT64 biFriendCharacterDBID;
	WCHAR wszMemo[FRIEND_MEMO_LEN_MAX];
};

struct TAModFriendMemo: public TAHeader
{
	INT64 biCharacterDBID;
};

//QUERY_DELFRIEND,
struct TQDelFriend:public TQHeader
{
	INT64 biCharacterDBID;
	//일단 하나짜리 인터페이스만 있음
	WCHAR wszDelCharacterName[NAMELENMAX];
	char cCount;
	INT64 biFriendCharacterDBIDs[FRIEND_MAXCOUNT];
};

struct TADelFriend: public TAHeader
{
	char cCount;
	INT64 biFriendCharacterDBIDs[FRIEND_MAXCOUNT];
};


//#########################################################################################################################################
//	MAINCMD_ISOLATE
//#########################################################################################################################################

struct TIsolateItem:public TBoostMemoryPool<TIsolateItem>
{
	INT64 biIsolateCharacterDBID;
	WCHAR wszIsolateName[NAMELENMAX];
};

//QUERY_GETISOLATELIST
struct TQGetIsolateList:public TQHeader
{
	INT64 biCharacterDBID;
};

struct TAGetIsolateList: public TAHeader
{
	BYTE cCount;
	TIsolateItem Isolate[ISOLATELISTMAX];
};

//QUERY_ADDISOLATE
struct TQAddIsolate:public TQHeader
{
	INT64 biCharacterDBID;
	WCHAR wszIsolateName[NAMELENMAX];
	INT64 biFriendCharacterDBID;
};

struct TAAddIsolate: public TAHeader
{
	BYTE cNeedDelFriend;
	TIsolateItem IsolateItem;
};

//QUERY_DELISOLATE
struct TQDelIsolate:public TQHeader
{
	INT64 biCharacterDBID;
	char cCount;
	INT64 biIsolateCharacterDBIDs[ISOLATELISTMAX];
};

struct TADelIsolate: public TAHeader
{
	char cCount;
	INT64 biIsolateCharacterDBIDs[ISOLATELISTMAX];
};


//#########################################################################################################################################
//	MAINCMD_GUILD
//#########################################################################################################################################

// QUERY_CREATEGUILD				// 길드 창설 요청/결과
struct TQCreateGuild: public TQHeader
{
	INT64 nCharacterDBID;		// 길드장 캐릭터 DBID
	WCHAR wszGuildName[GUILDNAME_MAX];				// 길드 이름
	int wGuildRoleAuth[GUILDROLE_TYPE_CNT];	// 길드 역할

	int nMapID;				// 맵번호
	int nEstablishExpense;	// 길드창설금액
	char cLevel;				// 창설자 레벨
	INT nRoleMaxItem[GUILDROLE_TYPE_CNT];				// 길드 역할별 창고 아이템 횟수제한 (인덱스)
	INT nRoleMaxCoin[GUILDROLE_TYPE_CNT];				// 길드 역할별 최대 출금액 (인덱스)
};

struct TACreateGuild: public TAHeader
{
	INT64 nCharacterDBID;		// 길드장 캐릭터 DBID
	WCHAR wszGuildName[GUILDNAME_MAX];	// 길드 이름
	char cWorldSetID;			// 월드 ID
	UINT nGuildDBID;	// 길드 DBID (실패일 경우 무시됨)
	__time64_t JoinDate;	// 길드 가입 날짜
	int nEstablishExpense;	// 길드창설금액
};

// QUERY_GETGUILDINFO				// 길드 정보 요청
struct TQGetGuildInfo: public TQHeader
{
	UINT nGuildDBID;	// 길드 DBID
	bool bNeedMembList;		// 길드원 목록도 필요한지 여부
};

struct TAGetGuildInfo: public TAHeader
{
	UINT nGuildDBID;	// 길드 DBID
	char cWorldSetID;	// 월드 ID
	TGuild Info;		// 길드 정보 (실패일 경우 무시됨)
};

struct TAGetGuildMember: public TAHeader
{
	UINT nGuildDBID;	// 길드 DBID
	char cWorldSetID;	// 월드 ID	
	bool bFirstPage;	// 처음인지 체크
	bool bEndPage;		// 마지막인지 체크
	int nCount;			// 길드원 정보 개수 (실패일 경우 무시됨)	
	TGuildMember MemberList[SENDGUILDMEMBER_MAX];	// 길드원 정보
};

// QUERY_DISMISSGUILD				// 길드 해체 요청
struct TQDismissGuild: public TQHeader
{
	INT64 nCharacterDBID;		// 길드장 캐릭터 DBID
	UINT nGuildDBID;	// 길드 DBID
};

struct TADismissGuild: public TAHeader
{
	INT64 nCharacterDBID;		// 길드장 캐릭터 DBID
	UINT nGuildDBID;	// 길드 DBID
	char cWorldSetID;			// 월드 ID
};

// QUERY_ADDGUILDMEMBER				// 길드원 추가 요청
struct TQAddGuildMember: public TQHeader
{
	UINT nAccountDBID;		// 추가할 계정 DBID
	INT64 nCharacterDBID;		// 추가할 캐릭터 DBID
	UINT nGuildDBID;	// 길드 DBID
	char cLevel;				// 캐릭터 레벨
	short wGuildSize;			// 최대 길드원 수
};

struct TAAddGuildMember: public TAHeader
{
	INT64 nCharacterDBID;		// 추가할 캐릭터 DBID
	UINT nGuildDBID;	// 길드 DBID
	char cWorldSetID;			// 월드 ID
	__time64_t JoinDate;	// 길드 가입 날짜
};

// QUERY_DELGUILDMEMBER				// 길드원 제거 (탈퇴/추방) 요청
struct TQDelGuildMember: public TQHeader
{
	UINT nReqAccountDBID;	// 요청한 계정 DBID (추방의 경우에는 제거대상과 달라짐)
	INT64 nReqCharacterDBID;	// 요청한 캐릭터 DBID (추방의 경우에는 제거대상과 달라짐)
	UINT nDelAccountDBID;	// 제거할 계정 DBID 
	INT64 nDelCharacterDBID;	// 제거할 캐릭터 DBID
	UINT nGuildDBID;	// 길드 DBID
	bool bIsExiled;			// 추방하는지 여부 (true : 추방 / false : 탈퇴)
	char cLevel;				// 캐릭터 레벨
#ifdef PRE_ADD_BEGINNERGUILD
	bool bIsGraduateBeginnerGuild;
#endif		//#ifdef PRE_ADD_BEGINNERGUILD
};

struct TADelGuildMember: public TAHeader
{
	UINT nReqAccountDBID;	// 요청한 계정 DBID (추방의 경우에는 제거대상과 달라짐)
	INT64 nReqCharacterDBID;	// 요청한 캐릭터 DBID (추방의 경우에는 제거대상과 달라짐)
	UINT nDelAccountDBID;	// 제거할 계정 DBID 
	INT64 nDelCharacterDBID;	// 제거할 캐릭터 DBID
	UINT nGuildDBID;	// 길드 DBID
	char cWorldSetID;			// 월드 ID
	bool bIsExiled;			// 추방하는지 여부 (true : 추방 / false : 탈퇴)
#ifdef PRE_ADD_BEGINNERGUILD
	bool bIsGraduateBeginnerGuild;
#endif		//#ifdef PRE_ADD_BEGINNERGUILD
};

// QUERY_CHANGEGUILDINFO			// 길드 정보 변경 요청
struct TQChangeGuildInfo:public TQHeader
{
	INT64 nCharacterDBID;				// 캐릭터 DBID
	UINT nGuildDBID;			// 길드 DBID
	BYTE btGuildUpdate;		// 길드 정보 변경 타입 (eGuildUpdateType)
	long nAddCommonPoint;	// 길드 일반 포인트 변화량 (0 : 변화없음 / 그외 : 음수이어야 차감 !!!)
	int nInt1;
	int nInt2;
	int nInt3;
	int nInt4;
	INT64 biInt64;
	WCHAR Text[GUILDHISTORYTEXT_MAX+1];
};

struct TAChangeGuildInfo: public TAHeader
{
	INT64 nCharacterDBID;				// 캐릭터 DBID
	UINT nGuildDBID;			// 길드 DBID
	char cWorldSetID;					// 월드 ID
	BYTE btGuildUpdate;		// 길드 정보 변경 타입 (eGuildUpdateType)
	long nCurCommonPoint;	// 길드 일반 포인트 현재량 (변경이 요청된 경우에만 채워짐)
	long nTotCommonPoint;	// 길드 일반 포인트 누적량 (변경이 요청된 경우에만 채워짐)
	int nInt1;
	int nInt2;
	int nInt3;
	int nInt4;
	INT64 biInt64;
	WCHAR Text[GUILDHISTORYTEXT_MAX+1];
};

// QUERY_CHANGEGUILDMEMBERINFO		// 길드원 정보 변경 요청
struct TQChangeGuildMemberInfo:public TQHeader
{
	UINT nReqAccountDBID;	// 요청한 계정 DBID (길드장이 길드원 정보를 변경하는 경우에는 변경대상과 달라짐)
	INT64 nReqCharacterDBID;	// 요청한 캐릭터 DBID (길드장이 길드원 정보를 변경하는 경우에는 변경대상과 달라짐)
	UINT nChgAccountDBID;	// 변경될 계정 DBID 
	INT64 nChgCharacterDBID;	// 변경될 캐릭터 DBID
	UINT nGuildDBID;	// 길드 DBID
	BYTE btGuildMemberUpdate;		// 길드원 정보 변경 타입 (eGuildMemberUpdateType)
	int nInt1;
	int nInt2;
	INT64 biInt64;
	WCHAR Text[GUILDHISTORYTEXT_MAX+1];
	char cLevel;
};

struct TAChangeGuildMemberInfo: public TAHeader
{
	UINT nReqAccountDBID;	// 요청한 계정 DBID (길드장이 길드원 정보를 변경하는 경우에는 변경대상과 달라짐)
	INT64 nReqCharacterDBID;	// 요청한 캐릭터 DBID (길드장이 길드원 정보를 변경하는 경우에는 변경대상과 달라짐)
	UINT nChgAccountDBID;	// 변경될 계정 DBID 
	INT64 nChgCharacterDBID;	// 변경될 캐릭터 DBID
	UINT nGuildDBID;	// 길드 DBID
	char cWorldSetID;			// 월드 ID
	BYTE btGuildMemberUpdate;		// 길드원 정보 변경 타입 (eGuildMemberUpdateType)
	int nInt1;
	int nInt2;
	INT64 biInt64;
	WCHAR Text[GUILDHISTORYTEXT_MAX+1];
};

// QUERY_ADDGUILDHISTORY:			// 길드 히스토리 추가 요청
struct TQAddGuildHistory: public TQHeader
{
	UINT nGuildDBID;	// 길드 DBID
	TGuildHistory History;	// 길드 히스토리 정보
};

// QUERY_GETGUILDHISTORYLIST		// 길드 히스토리 목록 요청
struct TQGetGuildHistoryList: public TQHeader
{
	INT64 nCharacterDBID;			// 캐릭터 DBID
	UINT nGuildDBID;		// 길드 DBID
	INT64 biIndex;	// 길드 히스토리 목록 인덱스 (0 이면 가장 최근 기록부터 확인 ?)
	bool bDirection;				// 목록 얻을 방향 (0:인덱스 이전 / 1:인덱스 이후)
};

struct TAGetGuildHistoryList: public TAHeader
{
	INT64 nCharacterDBID;			// 캐릭터 DBID
	UINT nGuildDBID;		// 길드 DBID
	char cWorldSetID;				// 월드 ID
	INT64 biIndex;	// 길드 히스토리 목록 인덱스 (0 이면 가장 최근 기록부터 확인 ?)
	bool bDirection;				// 목록 얻을 방향 (0:인덱스 이전 / 1:인덱스 이후)
	int nCurCount;				// 길드 히스토리 최초 인덱스 → 입력 인덱스 까지의 개수
	int nTotCount;				// 길드 히스토리 전체 개수
	int nCount;					// 길드 히스토리 목록 개수 (실패일 경우 무시됨)
	TGuildHistory HistoryList[GUILDHISTORYLIST_MAX];	// 길드 히스토리 목록 (실패일 경우 무시됨)
};

//QUERY_INVITEGUILDMEMBER:			// 길드원 초대 요청
struct TQInviteGuildMember : public TQHeader
{
	TQGetGuildInfo GuildInfo;
	UINT nToSessionID;		// 초대에 응한 자의 세션ID
};

struct TAInviteGuildMember: public TAHeader
{
	TAGetGuildInfo GuildInfo;
	UINT nToSessionID;				// 초대에 응한 자의 세션ID
};

// QUERY_MOVE_ITEM_IN_GUILDWARE		// 인벤 -> 길드창고로 아이템 이동
struct TQMoveItemInGuildWare : public TQHeader
{
	INT64 nCharacterDBID;			// 캐릭터 DBID
	UINT nGuildDBID;				// 길드 DBID

	int	nItemID;				// 이동할 아이템 ID
	INT64 biItemSerial;			// 이동할 아이템의 고유번호

	INT64 biNewItemSerial;		// Split되어 나온 아이템 고유 번호
	INT64 biMergeTargetItemSerial;// Merge 대상 아이템의 고유 번호

	short wItemCount;				// @inbItemSerial 값에 해당하는 아이템의 변경 전 총 수량
	short wSplitItemCount;		// @inbNewItemSerial 값에 해당하는 아이템 수량
	short wMergeItemCount;		// 합쳐지는 아이템의 이전 수량

	int	nMapIndex;				// 맵 ID

	DBPacket::TExchangeItem SrcItem;
	DBPacket::TExchangeItem DestItem;
};

struct TAMoveItemInGuildWare: public TAHeader
{
	char cWorldSetID;					// 월드 ID
	INT64 nCharacterDBID;				// 캐릭터 DBID
	UINT nGuildDBID;					// 길드 DBID

	DBPacket::TExchangeItem SrcItem;
	DBPacket::TExchangeItem DestItem;
};

// QUERY_MOVE_INVEN_TO_GUILDWARE		// 인벤 -> 길드창고로 아이템 이동
struct TQMoveInvenToGuildWare : public TQHeader
{
	INT64 nCharacterDBID;		// 캐릭터 DBID
	UINT nGuildDBID;			// 길드 DBID

	int nItemID;			// 이동할 아이템 ID
	INT64 biItemSerial;		// 이동할 아이템의 고유번호
	
	INT64 biNewItemSerial;	// 아이템의 일부 수량을 옮기는 경우, Split되어 나온 아이템의 새 고유 번호. Split 되지 않는다면 NULL을 입력합니다.
	INT64 biMergeTargetItemSerial; // 아이템을 기존 아이템에 Merge하는 경우, 대상 아이템의 고유 번호. Merge 되지 않는다면 NULL을 입력합니다.

	short wItemCount;			// 이동할 아이템의 수량
	short wMovingItemCount;	// 옮기는 아이템의 이전 수량
	short wMergeItemCount;	// 합쳐지는 아이템의 이전 수량
	int nMapIndex;			// 맵 ID

	DBPacket::TExchangeItem InvenItem;
	DBPacket::TExchangeItem GuildWareItem;
};

struct TAMoveInvenToGuildWare: public TAHeader
{
	INT64 nCharacterDBID;		// 캐릭터 DBID
	UINT nGuildDBID;			// 길드 DBID
	char cWorldSetID;			// 월드 ID

	DBPacket::TExchangeItem InvenItem;
	DBPacket::TExchangeItem GuildWareItem;
};

// QUERY_MOVE_GUILDWARE_TO_INVEN		// 길드창고 -> 인벤로 아이템 이동
struct TQMoveGuildWareToInven : public TQHeader
{
	INT64 nCharacterDBID;		// 캐릭터 DBID
	UINT nGuildDBID;			// 길드 DBID

	int nItemID;			// 이동할 아이템 ID
	INT64 biItemSerial;		// 이동할 아이템의 고유번호

	INT64 biNewItemSerial;	// 아이템의 일부 수량을 옮기는 경우, Split되어 나온 아이템의 새 고유 번호. Split 되지 않는다면 NULL을 입력합니다.
	INT64 biMergeTargetItemSerial; // 아이템을 기존 아이템에 Merge하는 경우, 대상 아이템의 고유 번호. Merge 되지 않는다면 NULL을 입력합니다.

	short wItemCount;			// 이동할 아이템의 수량
	short wMovingItemCount;	// 옮기는 아이템의 이전 수량
	short wMergeItemCount;	// 합쳐지는 아이템의 이전 수량

	int	nMaxDailyTakeItemCount;
	int	nMapIndex;			// 맵 ID

	DBPacket::TExchangeItem GuildWareItem;
	DBPacket::TExchangeItem InvenItem;
};

struct TAMoveGuildWareToInven: public TAHeader
{
	INT64 nCharacterDBID;		// 캐릭터 DBID
	UINT nGuildDBID;			// 길드 DBID
	char cWorldSetID;			// 월드 ID

	int	nDailyTakeItemCount;
	__time64_t tUseDate;

	DBPacket::TExchangeItem GuildWareItem;
	DBPacket::TExchangeItem InvenItem;
};

struct TQGuildWareHouseCoin : public TQHeader
{
	INT64 nCharacterDBID;		// 캐릭터 DBID
	UINT nGuildDBID;			// 길드 DBID

	char cMoveType;
	bool bInOutFlag;			// 0:입금 1:출금
	INT64 nMoveCoin;			// 입금액
	int	nMapIndex;			// 맵ID

	char szIP[IPLENMAX];		// IP주소
};

struct TAGuildWareHouseCoin: public TAHeader
{
	INT64 nCharacterDBID;		// 캐릭터 DBID
	UINT nGuildDBID;			// 길드 DBID
	char cWorldSetID;		// 월드 ID

	char cMoveType;
	bool bInOutFlag;			// 0:입금 1:출금
	INT64 nMoveCoin;			// 이동 금액
	INT64 biTotalCoin;		// 전체 금액

	__time64_t tUseDate;			
};

// QUERY_GET_GUILDWAREINFO				// 길드창고 아이템 요청
struct TQGetGuildWareInfo : public TQHeader
{	
	UINT nGuildDBID;	// 길드 DBID
	short wMaxCount;	// 창고 최대수
};

struct TAGetGuildWareInfo
{
	UINT nGuildDBID;			// 길드 DBID
	char cWorldSetID;			// 월드 ID
	int nErrNo;				// 에러

	BYTE cItemListCount;					// 아이템리스트 카운트
	TItemInfo ItemList[GUILD_WAREHOUSE_MAX];	// 웨어아이템리스트
};

//QUERY_SET_GUILDWAREINFO			// 길드창고 아이템 저장
struct TQSetGuildWareInfo : public TQHeader
{
	UINT nGuildDBID;							// 길드 DBID
	BYTE cSlotListCount;						// 슬롯카운트
	TGuildWareSlot WareSlotList[GUILD_WAREHOUSE_MAX];	// 슬롯리스트
};

struct TASetGuildWareInfo
{
	UINT nGuildDBID;			// 길드 DBID
	char cWorldSetID;			// 월드 ID
	int	nRetCode;			
};

// QUERY_GET_GUILDWAREHISTORY				// 길드창고 히스토리 요청
struct TQGetGuildWareHistory : public TQHeader
{
	INT64 nCharacterDBID;	// 캐릭터 DBID
	UINT nGuildDBID;		// 길드 DBID
	int	nIndex;			// 요청 인덱스
};

struct TAGetGuildWareHistory
{	
	UINT nAccountDBID;		// 계정 DBID
	INT64 nCharacterDBID;		// 캐릭터 DBID
	UINT nGuildDBID;			// 길드 DBID
	char cWorldSetID;			// 월드 ID

	int	nErrNo;				

	int	nCurrCount;			// 현재 리스트 수
	int	nTotalCount;		// 전체 리스트 수
	int	nCount;
	TGuildWareHistory HistoryList[GUILD_WAREHOUSE_HISTORYLIST_MAX];	// 히스토리 리스트
};

// QUERY_CHANGEGUILDNAME					// 길드명 변경
struct TQChangeGuildName : public TQHeader
{
	UINT nGuildDBID;	
	INT64 biItemSerial;
	WCHAR wszGuildName[GUILDNAME_MAX];  // 변경할 길드명	
};
struct TAChangeGuildName : public TAHeader
{
	char cWorldSetID;
	UINT nGuildDBID;
	INT64 biItemSerial;
	WCHAR wszGuildName[GUILDNAME_MAX];  // 변경한 길드명
};

// QUERY_MOD_GUILDEXP				// 길드 경험치 변경
struct TQModGuildExp: public TQHeader
{
	UINT nGuildDBID;
	char cPointType;
	int nPointValue;
	int nMaxPoint;
	int nLevel;
	INT64 biCharacterDBID;
	int nMissionID;
#if defined(PRE_ADD_GUILD_CONTRIBUTION)
	bool IsGuildLevelMax;
#endif	// #if defined(PRE_ADD_GUILD_CONTRIBUTION)
};

struct TAModGuildExp: public TAHeader
{
	char cWorldSetID;
	UINT nGuildDBID;
	char cPointType;
	int nPointValue;
	int nTotalGuildExp;
	int nDailyPointValue;
	int nLevel;
	INT64 biCharacterDBID;
	int nMissionID;
};

// QUERY_MOD_GUILDCHEAT				// 길드 치트
struct TQModGuildCheat: public TQHeader
{
	UINT nGuildDBID;
	char cCheatType;
	int nPoint;
	char cLevel;
};

struct TAModGuildCheat: public TAHeader
{
	char cWorldSetID;
	UINT nGuildDBID;
	char cCheatType;
	int nPoint;
	char cLevel;
};

struct TQGetGuildRewardItem: public TQHeader
{
	UINT nGuildDBID;	
};
struct TAGetGuildRewardItem: public TAHeader
{
	char cWorldSetID;
	UINT nGuildDBID;
	int nCount;
	TGuildRewardItem RewardItemInfo[GUILDREWARDEFFECT_TYPE_CNT]; 
};
struct TQAddGuildRewardItem: public TQHeader
{
	INT64 biCharacterDBID;
	UINT nGuildDBID;
	int nItemID;
	bool bEnternityFlag;
	int nLifeSpan;
	BYTE cItemBuyType;
	int nNeedGold;
	int nMpaID;
	int nRemoveItemID;
};
struct TAAddGuildRewardItem: public TAHeader
{
	INT64 biCharacterDBID;
	char cWorldSetID;
	UINT nGuildDBID;
	int nItemID;	
	BYTE cItemBuyType;
	__time64_t	m_tExpireDate;

};
struct TQChangeGuildSize: public TQHeader
{	
	UINT	nGuildDBID;
	short	nGuildSize;
};
struct TAChangeGuildSize: public TAHeader
{	
	char	cWorldSetID;
	UINT	nGuildDBID;
	short	nGuildSize;
};
struct TQDelGuildEffectItem:public TQHeader
{
	INT64		biCharacterDBID;	// 캐릭터코드	
	UINT		nGuildDBID;			// 길드 DBID
	int			nItemID;
};

// QUERY_ENROLL_GUILDWAR
struct TQEnrollGuildWar : public TQHeader
{
	short			wScheduleID;
	UINT	nGuildDBID;
};

struct TAEnrollGuildWar : public TAHeader
{
	char			cWorldSetID;
	UINT	nGuildDBID;
	short			wScheduleID;
	BYTE			cTeamColorCode;
};

// QUERY_GET_GUILDWARSCHEDULE
struct TQGetGuildWarSchedule : public TQHeader
{

};

struct TAGetGuildWarSchedule : public TAHeader
{	
	short wScheduleID;				// 길드전 차수
	short wWinersWeightRate;		// 승리팀 가중치
	bool bFinalProgress;			// 본선 진행 여부
	TGuildWarEventInfo EventInfo[GUILDWAR_STEP_END];	// 기간정보	
	__time64_t tRewardExpireDate;	// 보상(농장, 제스쳐) 만료 기간 
};

// QUERY_GET_GUILDWARFINALSCHEDULE	// 본선 일정 가져오기
struct TQGetGuildWarFinalSchedule : public TQHeader
{
	short wScheduleID;				// 길드전 차수
};

struct TAGetGuildWarFinalSchedule : public TAHeader
{
	TGuildWarEventInfo GuildWarFinalSchedule[GUILDWAR_FINALPART_MAX];	
};

// QUERY_ADD_GUILDWARPOINT	// 예선 미션 클리어시 포인트 적립
struct TQAddGuildWarPointRecodrd : public TQHeader
{
	INT64 biCharacterDBID;			// 캐릭터 DBID
	int nMissionID;					// 길드전 미션 ID
	int nOpeningPoint;				// 획득한 미션 포인트
	UINT nGuildDBID;		// 소속 길드 ID
	char cTeamColorCode;			// 1-청팀, 2-홍팀
};

struct TAAddGuildWarPointRecodrd : public TAHeader
{
	char cWorldSetID;
	char cTeamColorCode;			// 1-청팀, 2-홍팀
	UINT nGuildDBID;	 	// 소속 길드 ID
	int nOpeningPoint;				// 획득한 미션 포인트	
};
// QUERY_GET_GUILDWARPOINT	// 예선 포인트 조회
struct TQGetGuildWarPoint : public TQHeader
{
	char cQueryType;				// 'C'=캐릭터 예선 포인트 조회, 'G'=길드 예선포인트 조회, 'T'=팀 예선포인트 조회
	INT64 biDBID;					// C-캐릭터 ID, G-길드 ID, T-0
};

struct TAGetGuildWarPoint  : public TAHeader
{
	char cQueryType;
	INT64 biDBID;
	int nCharOpeningPoint;
	int nGuildOpeningPoint;
	int nBlueTeamPoint;
	int nRedTeamPoint;
};

// QUERY_GET_GUILDWARPOINT_PARTTOTAL			// 부문별 1위 조회(개인)
struct TQGetGuildWarPointPartTotal : public TQHeader
{
	char cQueryType;					// 'A' = 전체, 'C' = 개인
	INT64 biCharacterDBID;
};

struct SGuildWarPointPartTotal
{
	short wRanking;							// 순위
	INT64 biCharacterDBID;					// 캐릭터 ID
	WCHAR wszCharName[NAMELENMAX];			// 캐릭터 이름
	UINT nGuildDBID;				// 길드 ID
	WCHAR wszGuildName[GUILDNAME_MAX];	// 길드 이름
	int nPoint;								// 점수	
};

struct TAGetGuildWarPointPartTotal : public TAHeader
{
	char cQueryType;
	SGuildWarPointPartTotal GuildWarPointPartTotal[GUILDWAR_RANKINGTYPE_MAX]; // 각 부문별 집계 순위
};
// QUERY_GET_GUILDWARPOINT_GUILD_PARTTOTAL		// 부문별 길드 순위
struct TQGetGuildWarPointGuildPartTotal : public TQHeader
{
	char cQueryType;						// 'A' = 전체, 'G' = 길드
	UINT nGuildDBID;				// 길드 ID
};

struct TAGetGuildWarPointGuildPartTotal : public TAHeader
{
	char cQueryType;						// 'A' = 전체, 'G' = 길드
	SGuildWarPointPartTotal GuildWarPointPartTotal[GUILDWAR_RANKINGTYPE_MAX]; // 각 부문별 집계 순위
};
// QUERY_GET_GUILDWARPOINT_DAILY				// 예선전 각 일자별 1위		
struct TQGetGuildWarPointDaily : public TQHeader
{

};

struct SGuildWarPointDaily
{
	int nTotalPoint;
	INT64 biCharacterDBID;					// 캐릭터 ID
	WCHAR wszCharName[NAMELENMAX];			// 캐릭터 이름
	UINT nGuildDBID;				// 길드 ID
};

struct TAGetGuildWarPointDaily : public TAHeader
{	
	SGuildWarPointDaily sGuildWarPointDaily[GUILDWAR_DAILY_AWARD_MAX];
};
// QUERY_GET_GUILDWARPOINT_GUILD_TOTAL			// 각 길드의 포인트 집계 순위현황(여기서 본선 진출팀 조회도 가능함)
struct TQGetGuildWarPointGuildTotal : public TQHeader
{
	bool bMasterServer;				// 마스터 서버에서의 요청인지..
	char cQueryType;				// 'A' = 전체, 'G' = 길드
	short wScheduleID;				// 길드전 차수	
	UINT nGuildDBID;		// 길드 ID
};

struct SGuildWarPointGuildTotal
{
	short wRanking;							// 길드 랭킹
	UINT nGuildDBID;				// 길드 ID
	WCHAR wszGuildName[GUILDNAME_MAX];	// 길드 이름
	INT64 biMasterDBID;						// 길드장 캐릭터 ID
	WCHAR wszMasterName[NAMELENMAX];		// 길드장 이름
	short wGuildLevel;				// 길드 레벨
	short wGuildMemberCount;				// 현재 길드 멤버수
	int nOpeningPointSum;					// 예선 점수 합산
	int nMaxMemberCount;					// 길드원 최대 가입 인원수
};

struct TAGetGuildWarPointGuildTotal : public TAHeader
{
	bool bMasterServer;				// 마스터 서버에서의 요청인지..
	char cQueryType;				// 'A' = 전체, 'G' = 길드
	int nTotalCount;
	SGuildWarPointGuildTotal sGuildWarPointGuildTotal[GUILDWAR_FINALS_TEAM_MAX];
};

//QUERY_GET_GUILDWARPOINT_RUNNING_TOTAL			// 길드전 예선전 점수 현황(24위까지)
struct TQGetGuildWarPointRunningTotal : public TQHeader
{
	//일단 GUILD별로 뽑는건 안쓰니 패쓰..
};

struct TAGetGuildWarPointRunningTotal : public TAHeader
{	
	int nTotalCount;
	SGuildWarPointGuildTotal sGuildWarPointGuildTotal[GUILDWAR_TRIAL_POINT_TEAM_MAX];
};

//QUERY_GET_GUILDWARPOINT_FINAL_REWARDS
struct TAGetGuildWarPointFinalRewards : public TAHeader
{
	UINT nGuildDBID[GUILDWAR_FINALS_TEAM_MAX]; // 갱신할 GuildDBID
	int nGuildPoint[GUILDWAR_FINALS_TEAM_MAX]; // 갱신할 GuildPoint
};

// QUERY_ADD_GUILDWAR_FINAL_MATCHLIST		// 본선 대진표 저장
struct TQAddGuildWarFinalMatchList : public TQHeader
{
	short wScheduleID;										// 길드전 차수
	UINT nGuildDBID[GUILDWAR_FINALS_TEAM_MAX];		// 대진표 순서별 길드 ID(중간에 공백 있을 수 있음)
};

struct TAAddGuildWarFinalMatchList : public TAHeader
{

};

// QUERY_ADD_GUILDWARPOPULARITYVOTE	// 인기 투표 저장
struct TQAddGuildWarPopularityVote : public TQHeader
{
	INT64 biCharacterDBID;			// 캐릭터 DBID
	UINT nGuildDBID;		// 투표 길드 ID	
};

struct TAAddGuildWarPopularityVote : public TAHeader
{
	
};
// QUERY_GET_GUILDWARPOPULARITYVOTE	// 인기 투표 결과(캐릭터가 투표를 했는지 조회에도 사용)
struct TQGetGuildWarPopularityVote : public TQHeader
{
	char cQueryType;				// 'C' - 캐릭터 투표여부, 'G'-길드조회
	INT64 biCharacterDBID;			// 캐릭터 DBID	
};

struct TAGetGuildWarPopularityVote : public TAHeader
{
	char cQueryType;				// 'C' - 캐릭터 투표여부, 'G'-길드조회
	UINT nGuildDBID;		// 투표 or 1위한 길드 ID	
	WCHAR wszGuildName[GUILDNAME_MAX];	// 길드 이름
	int nPopularityVoteCount;			// 총득표수
};
// QUERY_ADD_GUILDWARFINALRESULTS		// 길드전 본선 결과 저장
struct TQAddGuildWarFinalResults : public TQHeader
{
	UINT nGuildDBID;		// 투표 길드 ID	
	short wScheduleID;				// 길드전 차수
	char cMatchSequence;			// 대진표상 번호
	char cMatchTypeCode;			// 1=결승, 2=4강, 3=8강, 4=16강
	char cMatchResultCode;			// 1=승리, 2=패배, 3=기권, 4=부전승
};

struct TAAddGuildWarFinalResults : public TAHeader
{
	
};
// QUERY_GET_GUILDWARFINALRESULTS		// 길드전 본선 결과 가져오기
struct TQGetGuildWarFinalResults : public TQHeader
{	
	short wScheduleID;				// 길드전 차수
};

struct SGuildFinalResultInfo
{
	UINT nGuildDBID;				// 길드 ID
	WCHAR wszGuildName[GUILDNAME_MAX];	// 길드 이름
	char cMatchSequence;			// 대진표상 번호
	char cMatchTypeCode;			// 1=결승, 2=4강, 3=8강, 4=16강
	char cMatchResultCode;			// 1=승리, 2=패배, 3=기권, 4=부전승
	short wGuildMark;				// 길드마크 문양
	short wGuildMarkBG;				// 길드마크 배경
	short wGuildMarkBorder;			// 길드마크 테두리
};

struct TAGetGuildWarFinalResults : public TAHeader
{
	SGuildFinalResultInfo GuildFinalResultInfo[GUILDWAR_FINALS_TEAM_MAX]; // 결승전 결과 정보..
};

// QUERY_GET_GUILDWAR_PRE_WIN_GUILD				// 길드전 지난 차수 우승팀 가져오기
struct TQGetGuildWarPreWinGuild : public TQHeader
{	

};

struct TAGetGuildWarPreWinGuild : public TAHeader
{
	UINT nGuildDBID;		// 길드 ID
	short wScheduleID;				// 길드전 차수
};

//QUERY_ADD_GUILDWAR_REWARD_CHARACTER
struct TQAddGuildWarRewardCharacter : public TQHeader
{
	short wScheduleID;				// 길드전 차수
	INT64 biCharacterDBID;			// 캐릭터 DBID	
	char cRewardType;				// 보상 코드(1=예선 보상 선물, 2=예선 보상 메일, 3=예선 보상 포인트)
};
struct TAAddGuildWarRewardCharacter : public TAHeader
{
	char cRewardType;				// 보상 코드(1=예선 보상 선물, 2=예선 보상 메일, 3=예선 보상 포인트)	
};

//QUERY_GET_GUILDWAR_REWARD_CHARACTER
struct TQGetGuildWarRewardCharacter : public TQHeader
{
	short wScheduleID;				// 길드전 차수
	INT64 biCharacterDBID;			// 캐릭터 DBID	
};

struct TAGetGuildWarRewardCharacter : public TAHeader
{
	bool bRewardResults[GUILDWAR_REWARD_CHAR_MAX];  //저장된 개인보상 결과
};
//QUERY_ADD_GUILDWAR_REWARD_GUILD		// 길드전 길드 보상결과 저장
struct TQAddGuildWarRewardGuild : public TQHeader
{
	short wScheduleID;				// 길드전 차수
	UINT nGuildDBID;				// 길드 DBID	
	char cRewardType;				// 보상 코드(1=본선 보상 스킬, 2=본선 보상 선물, 3=본선 보상 메일, 4=예선 보상 포인트, 5=본선 보상 포인트)
};
struct TAAddGuildWarRewardGuild : public TAHeader
{
	char cWorldSetID;
	UINT nGuildDBID;				// 길드 DBID	
	char cRewardType;				// 보상 코드(1=본선 보상 스킬, 2=본선 보상 선물, 3=본선 보상 메일, 4=예선 보상 포인트, 5=본선 보상 포인트)
};
//QUERY_GET_GUILDWAR_REWARD_GUILD		// 길드전 길드 보상결과 가져오기
struct TQGetGuildWarRewardGuild : public TQHeader
{
	char cQueryType;				// 'M' - 마스터 제스쳐 지급 여부 확인, 'V'-빌리지 요청
	short wScheduleID;				// 길드전 차수
	UINT nGuildDBID;				// 길드 DBID
};

struct TAGetGuildWarRewardGuild : public TAHeader
{
	char cQueryType;				// 'M' - 마스터 제스쳐 지급 여부 확인, 'V'-빌리지 요청
	char cWorldSetID;
	UINT nGuildDBID;				// 길드 DBID	
	bool bRewardResults[GUILDWAR_REWARD_GUILD_MAX];  //저장된 길드보상 결과
};

//QUERY_ADD_GUILDWAR_POINT_QUEUE (축제포인트 지급)
struct TQAddGuildWarPointQueue : public TQHeader
{
	UINT nGuildDBID;				// 길드 ID	
	INT64 biAddPoint;	
	WCHAR wszIP[IPLENMAX];			// 그냥 서버 아이피?
};
//QUERY_ADD_GUILDWAR_SYSTEM_MAIL_QUEUE
//요거 TQSendSystemMail 구조체 그대로씀..
//QUERY_ADD_GUILDWAR_MAKE_GIFT_QUEUE
struct TQAddGuilWarMakeGiftQueue : public TQHeader
{		
	char cRewardType;
	INT64 biCharacterDBIDs[GUILDSIZE_MAX];	// 보낼 애들의 캐릭터 DBID
	char cClass[GUILDSIZE_MAX];				// 보낼 애들의 Class
};
//QUERY_ADD_GUILDWAR_MAKE_GIFT_NOTGUILDINFO
struct TQAddGuildWarMakeGiftNotGuildInfo : public TQHeader
{
	char cRewardType;
	UINT nGuildDBID;
};
// QUERY_ADD_JOB_RESERVE
struct TQAddJobReserve : public TQHeader
{
	char cJobType;
};
struct TAAddJobReserve : public TAHeader
{
	char cJobType;
	int nJobSeq;
};
// QUERY_GET_JOB_RESERVE
struct TQGetJobReserve : public TQHeader
{
	int nJobSeq;
};
struct TAGetJobReserve : public TAHeader
{
	int nJobSeq;
	char cJobStatus;
};

struct TQGetListGuildWarItemTradeRecord : public TQHeader
{
	INT64 characterDBID;
};

struct TAGetListGuildWarItemTradeRecord : public TAHeader
{
	int count;
	ItemCount itemCount[INVENTORYMAX + WAREHOUSEMAX];
};

struct TQAddGuildWarItemTradeRecord : public TQHeader
{
	INT64 characterDBID;
	ItemCount itemCount;
};

//QUERY_CHANGEGUILDWARESIZE
struct TQChangeGuildWareSize : public TQHeader
{
	INT64 nCharacterDBID;		// 캐릭터 DBID
	UINT nGuildDBID;			// 길드 DBID
	short wStorageSize;		// 창고 사이즈
};

struct TAChangeGuildWareSize: public TAHeader
{
	INT64 nCharacterDBID;		// 캐릭터 DBID
	UINT nGuildDBID;			// 길드 DBID
	char cWorldSetID;		// 월드 ID
	short wStorageSize;					// 창고 사이즈
};

//QUERY_CHANGEGUILDMARK
struct TQChangeGuildMark : public TQHeader
{
	INT64 nCharacterDBID;		// 캐릭터 DBID
	UINT nGuildDBID;			// 길드 DBID
	short wGuildMark;			// 길드마크 문양
	short wGuildMarkBG;		// 길드마크 배경
	short wGuildMarkBorder;	// 길드마크 테두리
};

struct TAChangeGuildMark: public TAHeader
{
	INT64 nCharacterDBID;		// 캐릭터 DBID
	UINT nGuildDBID;			// 길드 DBID
	char cWorldSetID;		// 월드 ID

	short wGuildMark;		// 길드마크 문양
	short wGuildMarkBG;		// 길드마크 배경
	short wGuildMarkBorder;	// 길드마크 테두리
};

#if defined(PRE_ADD_GUILD_CONTRIBUTION)
struct TQGetGuildContributionPoint: public TQHeader
{
	INT64 biCharacterDBID;
};
struct TAGetGuildContributionPoint: public TAHeader
{
	int nWeeklyGuildContributionPoint;	// 주간 길드 공헌도
	int nTotalGuildContributionPoint;	// 여태까지 올린 길드 공헌도 수치
};
struct TQGetGuildContributionRanking: public TQHeader
{
	UINT nGuildDBID;
};
struct TAGetGuildContributionRanking: public TAHeader
{
	int nCount;			// 길드원 정보 개수
	GuildContribution::TGuildContributionRankingData ContributionRankingList[GUILDSIZE_MAX];
};
#endif


//#########################################################################################################################################
//	MAINCMD_PVP
//#########################################################################################################################################

struct TQUpdatePvPData:public TQHeader
{
	INT64		biCharacterDBID;
	PvPCommon::QueryUpdatePvPDataType::eCode Type;
	TPvPGroup	PvP;
};

struct TAUpdatePvPData: public TAHeader
{
	PvPCommon::QueryUpdatePvPDataType::eCode Type;
};

struct TQUpdatePvPRankCriteria : public TQHeader
{
	UINT nThresholdPvPExp;
};

struct TQForceCalcPvPRank : public TQHeader
{
};

struct LadderKillResult
{
	BYTE	cJobCode;
	short	nKillCount;
	short	nDeathCount;
};

struct TQAddPvPLadderResult:public TQHeader
{
	PvPCommon::QueryUpdatePvPDataType::eCode Type;
	INT64				biCharacterDBID;	
	BYTE				cPvPLadderCode;	// LadderSystem::MatchType::eCode
	int					iPvPLadderGradePoint;
	int					iHiddenPvPLadderGradePoint;
	BYTE				cResult;		// LadderSystem::MatchResult::eCode
	BYTE				cVersusCharacterJobCode;
	BYTE				cKillDeathCount;
	LadderKillResult	KillDeathCounts[JOB_KIND_MAX];
};

struct TAAddPvPLadderResult:public TAHeader
{
	PvPCommon::QueryUpdatePvPDataType::eCode Type;
};

struct TQGetListPvPLadderScore:public TQHeader
{
	INT64	biCharacterDBID;
};

struct TAGetListPvPLadderScore:public TAHeader
{
	TPvPLadderScoreInfo	Data;
};

struct TQGetListPvPLadderScoreByJob:public TQHeader
{
	INT64	biCharacterDBID;
};

struct TAGetListPvPLadderScoreByJob:public TAHeader
{
	TPvPLadderScoreInfoByJob Data;
};

struct TQInitPvPLadderGradePoint:public TQHeader
{
	INT64	biCharacterDBID;
	BYTE	cPvPLadderCode;
	int		iPvPLadderPoint;		// 증가치 입력(-가능)
	int		iPvPLadderGradePoint;	// 초기화할 PvP 래더 평점 포인트
	char	szIP[IPLENMAX];
};

struct TAInitPvPLadderGradePoint:public TAHeader
{

};

struct TQUsePvPLadderPoint:public TQHeader
{
	INT64	biCharacterDBID;
	int		iPvPLadderPoint;		// 사용한 래더 포인트
	int		iMapID;
	char	szIP[IPLENMAX];
	int		iAfterLadderPoint;
};

struct TAUsePvPLadderPoint:public TAHeader
{
	int		iPvPLadderPointAfter;	// 사용후남은 PvP래더평점
};

struct TQAddPvPLadderCUCount:public TQHeader
{
	BYTE	cPvPLadderCode;
	int		iCUCount;
};

struct TQModPvPLadderScoresForCheat:public TQHeader
{
	INT64	biCharacterDBID;
	BYTE	cPvPLadderCode;
	int		iPvPLadderPoint;
	int		iPvPLadderGradePoint;
	int		iHiddenPvPLadderGradePoint;
};

struct TAModPvPLadderScoresForCheat:public TAHeader
{
	BYTE	cPvPLadderCode;
	int		iPvPLadderPoint;
	int		iPvPLadderGradePoint;
	int		iHiddenPvPLadderGradePoint;
};

struct TQGetListPvPLadderRanking:public TQHeader
{
	INT64	biCharacterDBID;
	BYTE	cPvPLadderCode;
};

struct TAGetListPvPLadderRanking:public TAHeader
{
	TPvPLadderRanking MyRanking;
	TPvPLadderRanking Top[LadderSystem::Common::RankingTop];
};

#if defined(PRE_ADD_PVP_RANKING)
//일반 Ranking
struct TQGetPvPRankBoard:public TQHeader
{
	INT64	biCharacterDBID;	
};

struct TAGetPvPRankBoard:public TAHeader
{
	TPvPRankingDetail MyRanking;
};

struct TQGetPvPRankList:public TQHeader
{
	int	iPage;
	BYTE cClassCode;
	BYTE cSubClassCode;
	WCHAR wszGuildName[GUILDNAME_MAX];
};

struct TAGetPvPRankList:public TAHeader
{
	INT64 biRankingTotalCount;
	BYTE cRankingCount;
	TPvPRanking RankingInfo[RankingSystem::RANKINGMAX];
};

struct TQGetPvPRankInfo:public TQHeader
{
	WCHAR	wszCharName[NAMELENMAX];	
};

struct TAGetPvPRankInfo:public TAHeader
{
	TPvPRankingDetail RankingInfo;
};

//Ladder Ranking
struct TQGetPvPLadderRankBoard:public TQHeader
{
	INT64	biCharacterDBID;
	BYTE	cPvPLadderCode;
};

struct TAGetPvPLadderRankBoard:public TAHeader
{
	BYTE cPvPLadderCode;
	TPvPLadderRankingDetail MyRanking;
};

struct TQGetPvPLadderRankList:public TQHeader
{
	int	iPage;
	BYTE cClassCode;
	BYTE cSubClassCode;
	BYTE cPvPLadderCode;
	WCHAR wszGuildName[GUILDNAME_MAX];
};

struct TAGetPvPLadderRankList:public TAHeader
{
	INT64 biRankingTotalCount;
	BYTE cRankingCount;
	BYTE cPvPLadderCode;
	TPvPLadderRanking2 RankingInfo[RankingSystem::RANKINGMAX];
};

struct TQGetPvPLadderRankInfo:public TQHeader
{
	WCHAR	wszCharName[NAMELENMAX];
	BYTE	cPvPLadderCode;
};

struct TAGetPvPLadderRankInfo:public TAHeader
{
	BYTE cPvPLadderCode;
	TPvPLadderRankingDetail RankingInfo;
};

struct TQForceAggregatePvPRank:public TQHeader
{
	bool bIsPvPLadder;
};

#endif

struct TQAddPVPGhoulScores : public TQHeader
{
	INT64	biCharacterDBID;
	int		nGhoulWin;
	int		nHumanWin;
	int		nTimeOver;
	int		nGhoulKill;
	int		nHumanKill;
	int		nHolyWaterUse;
};

struct TQGetPVPGhoulScores : public TQHeader
{
	INT64	biCharacterDBID;
};

struct TAGetPVPGhoulScores : public TAHeader
{
	int		nPlayCount;
	int		nGhoulWin;
	int		nHumanWin;
	int		nTimeOver;
	int		nGhoulKill;
	int		nHumanKill;
	int		nHolyWaterUse;
};
#if defined(PRE_ADD_PVP_TOURNAMENT)
struct TQAddPVPTournamentResult : public TQHeader
{
	int nTotalCount;
#if defined( PRE_PVP_GAMBLEROOM )
	INT64 nGambleDBID;
#endif
	INT64 biCharacterDBID[PvPCommon::Common::PvPTournamentUserMax];
	char cTournamentStep[PvPCommon::Common::PvPTournamentUserMax];
	bool bWin[PvPCommon::Common::PvPTournamentUserMax];
};
#endif

#if defined( PRE_PVP_GAMBLEROOM )
struct TQAddGambleRoom: public TQHeader
{
	int nRoomID;
	BYTE cGambleType;
	int nGamblePrice;
};

struct TAAddGambleRoom: public TAHeader
{
	int nRetCode;
	char cWorldSetID;
	int nRoomID;
	INT64 nGambleDBID;
};

struct TQAddGambleRoomMember: public TQHeader
{
	INT64 nGambleDBID;
	INT64 biCharacterDBID;
};

struct TQEndGambleRoom: public TQHeader
{
	INT64 nGambleDBID;
	TPvPGambleRoomWinData Member[PvPGambleRoom::Max];	
};
#endif

//#########################################################################################################################################
//	MAINCMD_DARKLAIR
//#########################################################################################################################################

struct SDarkLairUserData
{
	USHORT					unLevel;									// 레벨
	int						iJobIndex;									// 직업Index
	WCHAR					wszCharacterName[NAMELENMAX];				// 캐릭터이름(NULL포함)

	SDarkLairUserData()
	{
		unLevel				= 0;
		iJobIndex			= 0;
		memset( wszCharacterName, 0, sizeof(wszCharacterName) );
	}

	void Set( USHORT _unLevel, int _iJob, WCHAR* _pwszName )
	{
		unLevel				= _unLevel;
		iJobIndex			= _iJob;
		if( _pwszName )
			_wcscpy( wszCharacterName, _countof(wszCharacterName), _pwszName, (int)wcslen(_pwszName) );
	}
};

struct SDarkLairUpdateUserData : public SDarkLairUserData
{
	bool					bUpdate;									// 최종까지 플레이하여 개인기록 Update 할지 플래그
	INT64					i64CharacterDBID;							// CharDBID

	SDarkLairUpdateUserData()
	{
		bUpdate				= false;
		i64CharacterDBID	= 0;
	}
};

struct SDarkLairHistory
{
	USHORT unPlayRound;								// 플레이한 Round 수
	UINT uiPlaySec;									// 플레이한시간(초)
	WCHAR wszPartyName[PARTYNAMELENMAX];			// 파티이름(NULL포함)
	BYTE cPartyUserCount;
	SDarkLairUserData sUserData[DarkLair::Common::MaxPlayer];

	SDarkLairHistory()
	{
		unPlayRound		= 0;
		uiPlaySec		= 0;
		cPartyUserCount = 0;
		memset( wszPartyName, 0, sizeof(wszPartyName) );
	}

	void Set( USHORT _unPlayRound, UINT _uiPlaySec, WCHAR* _pwszPartyName )
	{
		unPlayRound = _unPlayRound;
		uiPlaySec	= _uiPlaySec;
		if( _pwszPartyName )
			_wcscpy( wszPartyName, _countof(wszPartyName), _pwszPartyName, (int)wcslen(_pwszPartyName) );
	}

	void AddUserData( USHORT _unLevel, int _iJob, WCHAR* _pwszName )
	{
		if( cPartyUserCount >= DarkLair::Common::MaxPlayer )
			return;

		sUserData[cPartyUserCount++].Set( _unLevel, _iJob, _pwszName );
	}
};

struct SDarkLairBestHistory : public SDarkLairHistory
{
	INT64	i64CharacterDBID;
	int		iRank;

	SDarkLairBestHistory()
	{
		i64CharacterDBID	= 0;
		iRank				= 0;
	}
};

struct TQUpdateDarkLairResult:public TQHeader
{
	int	iRoomID;									// 게임서버RoomID
	int	iProtectionKey;								// 보호키
	int	iMapIndex;									// 플레이한 MapIndex
	USHORT unPlayRound;								// 플레이한 Round 수
	UINT uiPlaySec;									// 플레이시간(초)
	WCHAR wszPartyName[PARTYNAMELENMAX];			// 파티이름(NULL포함)
	BYTE cPartyUserCount;
	SDarkLairUpdateUserData	sUserData[DarkLair::Common::MaxPlayer];
};

struct TAUpdateDarkLairResult:public TBoostMemoryPool<TAUpdateDarkLairResult>
{
	short					nRetCode;
	int						iRoomID;									// 게임서버RoomID
	int						iProtectionKey;								// 보호키
	int						iRank;										// 현재 플레이한 다크레어 랭킹
	SDarkLairHistory		sHistoryTop[DarkLair::Rank::SelectTop];		// 상위랭킹
	BYTE					cPartyUserCount;
	SDarkLairBestHistory	sBestUserData[DarkLair::Common::MaxPlayer];	// 개인베스트
};

struct TQGetDarkLairRankBoard:public TQHeader
{
	INT64	biCharacterDBID;
	int		iMapIndex;
	BYTE	cPartyUserCount;
};

struct TAGetDarkLairRankBoard:public TAHeader
{
	SDarkLairBestHistory sBestHistory;
	BYTE cPartyUserCount;
#if defined(PRE_ADD_CHALLENGE_DARKLAIR)
	int iMapIndex;
#endif
	SDarkLairHistory sHistoryTop[DarkLair::Rank::SelectRankBoardTop];
};

//#########################################################################################################################################
//	MAINCMD_AUTH
//#########################################################################################################################################

struct TABeginAuth: public TAHeader
{
	UINT nSessionID;						// 세션 ID
	char cCertifyingStep;					// 인증 단계
	char cWorldSetID;							// 월드 ID (*)
};

struct TQStoreAuth:public TQHeader						// GA, VI 사용
{
	INT64 biCertifyingKey;					// 인증키
	int nCurServerID;						// 현재 서버 고유번호
	char cWorldSetID;							// 월드 ID (*)
	UINT nSessionID;						// 세션 ID
	INT64 biCharacterDBID;					// 캐릭터 DBID
	WCHAR wszAccountName[IDLENMAX];			// 계정 이름
	WCHAR wszCharacterName[NAMELENMAX];		// 캐릭터 이름
	BYTE bIsAdult;							// 성인 여부
	BYTE nAge;								// 나이
	BYTE nDailyCreateCount;					// 일일 캐릭터 생성가능 횟수
	int nPrmInt1;							// 파라메터 1 (국가별 다목적)
	char cLastServerType;					// 최근 서버 타입 (eServerType / 0 : 로그인, 2 : 빌리지, 3 : 게임)
};

struct TAStoreAuth: public TAHeader			// GA, VI 사용
{
	UINT nSessionID;						// 세션 ID
};

struct TQCheckAuth:public TQHeader						// GA, VI 사용
{
	INT64 biCertifyingKey;					// 인증키
	int nCurServerID;						// 현재 서버 고유번호
	int nSessionID;
};

struct TACheckAuth: public TAHeader						// GA, VI 사용
{
	char cWorldSetID;							// 월드 ID (*)
	UINT nSessionID;						// 세션 ID
	INT64 biCertifyingKey;					// 인증키
	INT64 biCharacterDBID;					// 캐릭터 DBID
	WCHAR wszAccountName[IDLENMAX];			// 계정 이름
	WCHAR wszCharacterName[NAMELENMAX];		// 캐릭터 이름
	char cAccountLevel;						// 운영자, 개발자 등등의 계정레벨
	BYTE bIsAdult;							// 성인 여부
	char nAge;								// 나이
	BYTE nDailyCreateCount;					// 일일 캐릭터 생성가능 횟수
	int nPrmInt1;							// 파라메터 1 (국가별 다목적)
	char cLastServerType;					// 최근 서버 타입 (eServerType / 0 : 로그인, 2 : 빌리지, 3 : 게임)
};

struct TQResetAuth:public TQHeader						// GA, VI 사용
{
	UINT nSessionID;						// 세션 ID
};

struct TAResetAuth: public TAHeader						// GA, VI 사용 (안함 ???)
{
	UINT nSessionID;						// 세션 ID
};

struct TQResetAuthServer:public TQHeader
{
	int nServerID;							// 서버 고유번호
};

struct TAResetAuthServer
{
	int nServerID;							// 서버 고유번호
	int nResult;							// 쿼리 결과
};

struct TQGetAuthWorldUserCnt:public TQHeader		//QUERY_AUTHUSERCOUNT
{
	int nServerID;
};

struct TAuthWorldUserCnt
{
	int nWorldSetID;
	UINT nWorldUserCnt;
};

struct TAGetAuthWorldUserCnt
{
	int nServerID;
	short nResult;
	BYTE cCount;
	TAuthWorldUserCnt User[WORLDCOUNTMAX];
};

struct TQSetWorldIDAuth:public TQHeader		//QUERY_SETWOLRDID
{
};

//#########################################################################################################################################
//	MAINCMD_CASH
//#########################################################################################################################################

// QUERY_GETLISTGIVEFAILITEM,			// 캐쉬아이템 실패한거 리스트 얻어오기 (select할때 같이 받는거임)
struct TAGetListGiveFailItem: public TAHeader
{
	int nCount;
	DBPacket::TCashFailItem CashFailItem[CASHINVENTORYDBMAX];
};

// QUERY_MODGIVEFAILFLAG
struct TQModGiveFailFlag: public TQHeader
{
	INT64 biCharacterDBID;
	int nMapID;
	int nChannelID;
	WCHAR wszIP[IPLENMAX];
	int nCount;
	DBPacket::TModCashFailItemEx CashItem[CASHINVENTORYDBMAX];
};

struct TAModGiveFailFlag: public TAHeader
{
	int nCount;
	TCashItemBase CashItem[CASHINVENTORYDBMAX];
};

// QUERY_GETREMAINPETAL
struct TQGetRemainPetal: public TQHeader
{
};

struct TAGetRemainPetal: public TAHeader
{
	int nPetal;
};

// QUERY_PURCHASEITEMBYFAKEPETAL
struct TQPurchaseItemByFakePetal:public TQHeader
{
	int nChannelID;
	int nMapID;
	char cCartType;
	char cCount;
	DBPacket::TPetalInfo PetalInfo[CARTLISTMAX];
};

struct TAPurchaseItemByFakePetal: public TAHeader
{
	char cCartType;
	char cCount;
	DBPacket::TPetalInfoEx PetalInfo[CARTLISTMAX];
};

// QUERY_CHECKGIFTRECEIVER
struct TQCheckGiftReceiver:public TQHeader
{
	WCHAR wszCharacterName[NAMELENMAX];
};

struct TACheckGiftReceiver: public TAHeader
{
	BYTE cLevel;
	BYTE cJob;
};

// QUERY_GETLISTGIFTBOX
struct TQGetListGiftBox: public TQHeader
{
	INT64 biCharacterDBID;
#if defined(PRE_ADD_MULTILANGUAGE)
	char cSelectedLang;
#endif		//#if defined(PRE_ADD_MULTILANGUAGE)
};

struct TAGetListGiftBox: public TAHeader
{
#if defined(PRE_ADD_MULTILANGUAGE)
	char cSelectedLang;
#endif		//#if defined(PRE_ADD_MULTILANGUAGE)
	char cCount;
	TGiftInfo GiftBox[GIFTBOXLISTMAX];
};

// QUERY_MODGIFTRECEIVEFLAG
struct TQModGiftReceiveFlag: public TQHeader
{
	INT64 biCharacterDBID;
#if defined(PRE_RECEIVEGIFTALL)
	bool bReceiveAll;
#endif	// #if defined(PRE_RECEIVEGIFTALL)
	char cCount;
	TReceiveGiftData GiftData[GIFTPAGEMAX];
};

struct TAModGiftReceiveFlag: public TAHeader
{
#if defined(PRE_RECEIVEGIFTALL)
	bool bReceiveAll;
#endif	// #if defined(PRE_RECEIVEGIFTALL)
	char cCount;
	DBPacket::TReceiveGift ReceiveGift[GIFTPAGEMAX];
};

// QUERY_RECEIVEGIFT,					// 선물받기
struct TGiftItem
{
	int nItemSN;
	TItem AddItem;
	DWORD dwPartsColor1;		// 파츠 색깔
	DWORD dwPartsColor2;
	TVehicleItem VehiclePart1;
	TVehicleItem VehiclePart2;
};

struct TQReceiveGift: public TQHeader
{
#if defined(PRE_RECEIVEGIFTALL)
	bool bReceiveAll;
	char cTotalCount;
#endif	// #if defined(PRE_RECEIVEGIFTALL)
	TReceiveGiftData GiftData;

	int nChannelID;
	int nMapID;
	WCHAR wszIP[IPLENMAX];
	int nItemSN;
	char cCount;
	TGiftItem GiftItem[PACKAGEITEMMAX];
};

struct TAReceiveGift: public TAHeader
{
	INT64 biPurchaseOrderID;
	UINT nReceiverAccountDBID;
	INT64 biReceiverCharacterDBID;
	int nReceiverTotalMailCount;
	int	nReceiverNotReadMailCount;
	int	nReceiver7DaysLeftMailCount;
#if defined(PRE_ADD_VIP)
	char cPayMethodCode;
#endif	// #if defined(PRE_ADD_VIP)
#if defined(PRE_RECEIVEGIFTALL)
	bool bReceiveAll;
	char cTotalCount;
#endif	// #if defined(PRE_RECEIVEGIFTALL)
	int nItemSN;
	char cCount;
	TGiftItem GiftItem[PACKAGEITEMMAX];
};

// QUERY_GIFTRETURN
#if defined(PRE_ADD_GIFT_RETURN)
struct TQGiftReturn: public TQHeader
{
	INT64 biPurchaseOrderID;
};
struct TAGiftReturn : public TAHeader
{
	INT64 biPurchaseOrderID;
	UINT nReceiverAccountDBID;
	INT64 biReceiverCharacterDBID;
	int nReceiverTotalMailCount;
};
#endif //#if defined(PRE_ADD_GIFT_RETURN)

#if defined(PRE_ADD_CASH_REFUND)
struct TQPaymentItemList : public TQHeader
{
	INT64 biCharacterDBID;
};

struct TAPaymentItemList : public TAHeader
{
	int nPaymentCount;
	TPaymentItemInfoEx PaymentItemList[CASHINVENTORYMAX]; // 무한대로 만들어야 하는데..일단 150개
};

struct TAPaymentPackageItemList : public TAHeader
{
	int nPaymentPackegeCount;
	TPaymentPackageItemInfoEx PaymentPackageItemList[CASHINVENTORYMAX];
};
#endif

// QUERY_NOTIFYGIFT,					// 선물 알리미
struct TQNotifyGift: public TQHeader
{
	INT64 biCharacterDBID;
	bool bNew;
};

struct TANotifyGift: public TAHeader
{
	bool bNew;
	int nGiftCount;
};

struct TQAddEffectItems:public TQHeader
{
	INT64	biCharacterDBID;
	char	cItemLifeSpanRenewal;
	int		iItemID;
	DBDNWorldDef::EffectItemGetCode::eCode Code;
	INT64	biFKey;
	int		iPrice;
	bool	bEternityFlag;
	int		iItemLifeSpan;
	int		iProperty[5];
	INT64	biItemSerial;
	int		iRemoveItemID;
};

// QUERY_MAKEGIFTBYQUEST,			// 퀘스트 완료 보상
struct TQMakeGiftByQuest: public TQHeader
{
	INT64 biCharacterDBID;
	bool bPCBang;
	int nQuestID;
	WCHAR wszMemo[GIFTMESSAGEMAX];
	int nLifeSpan;
	char szIp[IPLENMAX];
	bool bNewFlag;
	char cRewardCount;
	int nRewardItemSN[MAILATTACHITEMMAX];
};

struct TAMakeGiftByQuest: public TAHeader
{
	int nGiftCount;
};

// QUERY_MAKEGIFTBYMISSION,			// 미션 완료 보상
struct TQMakeGiftByMission: public TQHeader
{
	INT64 biCharacterDBID;
	bool bPCBang;
	int nMissionID;
	WCHAR wszMemo[GIFTMESSAGEMAX];
	int nLifeSpan;
	char szIp[IPLENMAX];
	bool bNewFlag;
	char cRewardCount;
	int nRewardItemSN[MISSIONREWARDITEMMAX];
};

struct TAMakeGiftByMission: public TAHeader
{
	int nGiftCount;
};

// QUERY_MAKEGIFT
struct TQMakeGift: public TQHeader
{
	INT64 biCharacterDBID;
	bool bPCBang;
	WCHAR wszMemo[GIFTMESSAGEMAX];
	int nLifeSpan;
	char cPayMethodCode;	// PayMethodCode에 따라서 FKey값을 결정
	int iOrderKey;
	char szIp[IPLENMAX];
	bool bNewFlag;
	char cRewardCount;
	int nRewardItemSN[5];
};

struct TAMakeGift: public TAHeader
{
	int nGiftCount;
};

// QUERY_INCREASEVIPPOINT,				// VIP 기본 포인트 가산
struct TQIncreaseVIPPoint: public TQHeader
{
	INT64 biCharacterDBID;
	int nBasicPoint;			// VIP 정액제 상품 구입에 따른 기본 포인트
	INT64 biPurchaseOrderID;	// 아이템 구입 주문 ID
	short wVIPPeriod;			// VIP 기간. 단위 : Day. 30일, 90일, 180일, 360일
	bool bAutoPay;				// 0=수동결제, 1=자동결제
};

struct TAIncreaseVIPPoint: public TAHeader
{
	int nVIPTotalPoint;			// 총 VIP 포인트
	__time64_t tVIPEndDate;		// VIP 기간 끝 일자
};

// QUERY_GETVIPPOINT,					// VIP 기간 및 포인트 조회
struct TQGetVIPPoint: public TQHeader
{
	INT64 biCharacterDBID;
};

struct TAGetVIPPoint: public TAHeader
{
	int nVIPTotalPoint;			// 총 VIP 포인트
	__time64_t tVIPEndDate;		// VIP 기간 끝 일자
	bool bAutoPay;				// 0=수동결제, 1=자동결제
};

// QUERY_MODVIPAUTOPAYFLAG,			// VIP 자동 결제 상태 변경
struct TQModVIPAutoPayFlag: public TQHeader
{
	INT64 biCharacterDBID;
	bool bAutoPay;			// 0=수동결제, 1=자동결제
};

struct TAModVIPAutoPayFlag: public TAHeader
{
	bool bAutoPay;			// 0=수동결제, 1=자동결제
};

// QUERY_GIFTBYCHEAT
struct TQGiftByCheat: public TQHeader
{
	INT64 biCharacterDBID;
	WCHAR wszCharacterName[NAMELENMAX];
	int nItemSN;
	char szIp[IPLENMAX];
};

// QUERY_PETAL
struct TQPetal: public TQHeader
{
	INT64 biCharacterDBID;
	int nUseItemID;
	int nAddPetal;
	char cInvenType;			// eItemPositionType
	BYTE cInvenIndex;			// 일반인벤일때만 index넣어주면 됨 (캐쉬는 참조하지않음)
	INT64 biInvenSerial;
	int	iMapID;
	WCHAR wszIP[IPLENMAX];
};

struct TAPetal: public TAHeader
{
	int nUseItemID;
	int nTotalPetal;
	char cInvenType;			// eItemPositionType
	BYTE cInvenIndex;			// 일반인벤일때만 index넣어주면 됨 (캐쉬는 참조하지않음)
	INT64 biInvenSerial;
};

// QUERY_CHEAT_GIFTCLEAR
struct TQCheatGiftClear : public TQHeader
{
	INT64 biCharacterDBID;
};


#if defined( PRE_PVP_GAMBLEROOM )
struct TQAddPetalLogCode: public TQHeader
{
	INT64 biCharacterDBID;
	int nUseItemID;
	int nAddPetal;
	BYTE cLogCode;
	INT64 nGambleDBID;
};

struct TQUsePetal: public TQHeader
{
	INT64 biCharacterDBID;	
	int nDelPetal;
	BYTE cLogCode;
	INT64 nGambleDBID;
};
#endif

//#########################################################################################################################################
//	MAINCMD_MSGADJUST
//#########################################################################################################################################

struct TQMsgAdjust :public TQHeader
{
	INT64 biCharacterDBID;
	int nChannelID;
	int nVerifyMapIndex;
	char cGateNo;
};

struct TAMsgAdjust
{
	UINT nAccountDBID;
	INT64 biCharacterDBID;
	int nChannelID;
	int nVerifyMapIndex;
	char cGateNo;
};

#ifdef PRE_FIX_63822
struct TQMsgAdjustSeq :public TQHeader
{
};

struct TAMsgAdjustSeq :public TAHeader
{
};
#endif		//#ifdef PRE_FIX_63822

//#########################################################################################################################################
//	MAINCMD_LOG
//#########################################################################################################################################

struct TQAddPartyStartLog:public TQHeader
{
	INT64 biPartyLogID;
};

struct TQAddPartyEndLog:public TQHeader
{
	INT64 biPartyLogID;
};

struct TQAddStageStartLog:public TQHeader
{
	INT64	biRoomLogID;
	INT64	biPartyLogID;
	BYTE	cPartySize;
	int		iMapID;
	DBDNWorldDef::DifficultyCode::eCode Code;
};

struct TQAddStageClearLog:public TQHeader
{
	INT64	biRoomLogID;
	INT64	biCharacterDBID;
	int		iJobID;
	BYTE	cCharacterLevel;
	bool	bClearFlag;
	DBDNWorldDef::ClearGradeCode::eCode Code;
	int		iMaxComboCount;
	BYTE	cBossKillCount;
	int		iRewardCharacterExp;
	int		iBonusCharacterExp;
	int		iClearPoint;
	int		iBonusClearPoint;
	int		iClearTimeSec;
#if defined( PRE_ADD_36870 )
	int		iClearRound;
#endif // #if defined( PRE_ADD_36870 )
};

struct TQAddStageRewardLog:public TQHeader
{
	INT64	biRoomLogID;
	INT64	biCharacterDBID;
	DBDNWorldDef::RewardBoxCode::eCode Code;
	INT64	biRewardItemSerial;
	int		iRewardItemID;
	int		iRewardRet;
};

struct TQAddStageEndLog:public TQHeader
{
	INT64	biRoomLogID;
	DBDNWorldDef::WhereToGoCode::eCode Code;
};

struct TQAddPvPStartLog:public TQHeader
{
	INT64	biRoomID1;
	int		iRoomID2;
	BYTE	cMaxPlayerCount;
	DBDNWorldDef::PvPModeCode::eCode Code;
	int		iPvPModeDetailValue;
	bool	bBreakIntoFlag;
	bool	bItemUseFlag;
	bool	bSecretFlag;
	int		iMapID;
	INT64	biRoomMasterCharacterDBID;
	INT64	biCreateRoomCharacterDBID;
};

struct TQAddPvPResultLog:public TQHeader
{
	INT64	biRoomID1;
	int		iRoomID2;
	INT64	biCharacterDBID;
	DBDNWorldDef::PvPTeamCode::eCode TeamCode;
	bool	bBreakIntoFlag;
	DBDNWorldDef::PvPResultCode::eCode ResultCode;
	int		iPlayTimeSec;
	int		iPvPKillPoint;
	int		iPvPAssistPoint;
	int		iPvPSupportPoint;
	int		iVsKOWin[PvPCommon::Common::MaxClass];
	int		iVsKOLose[PvPCommon::Common::MaxClass];
	int nOccupationCount;
	BYTE cOccupationWinType;
};

struct TQAddPvPEndLog:public TQHeader
{
	INT64	biRoomID1;
	int		iRoomID2;
};

struct TQAddNestGateStartLog:public TQHeader
{
	INT64	biRoomID;
	BYTE	cPartySize;
	int		nGateID;
};

struct TQAddNestGateEndLog:public TQHeader
{
	INT64	biRoomID;
	BYTE	cPartySize;
};

struct TQAddNestGateClearLog:public TQHeader
{
	INT64	biRoomID;
	bool	bClearFlag;
	INT64 biChracterDBID;
	BYTE cCharacterJob;
	BYTE cCharacterLevel;
};

#if defined( PRE_ADD_NPC_REPUTATION_SYSTEM )

struct TQModNPCLocation:public TQHeader
{
	int		iNPCID;
	int		iMapID;
};

struct TQModNPCFavor:public TQHeader
{
	INT64				biCharacterDBID;
	BYTE				cCount;
	TNpcReputation		UpdateArr[NpcReputation::Common::MaxEffectNpcCount];
};

struct TQGetListNpcFavor:public TQHeader
{
	INT64				biCharacterDBID;
	int					iMapID;
};

struct TAGetListNpcFavor:public TAHeader
{
	bool				bLastPage;
	BYTE				cCount;
	TNpcReputation		ReputationArr[30];
};

#endif // #if defined( PRE_ADD_NPC_REPUTATION_SYSTEM )

struct TQGetMasterPupilInfo:public TQHeader
{
	INT64							biCharacterDBID;
	bool							bClientSend;
	MasterSystem::EventType::eCode	EventCode;
};

struct TAGetMasterPupilInfo:public TAHeader
{
	bool							bClientSend;
	MasterSystem::EventType::eCode	EventCode;
	TMasterSystemSimpleInfo			SimpleInfo;
};

struct TQAddMasterCharacter:public TQHeader
{
	INT64	biCharacterDBID;
	WCHAR	wszSelfIntroduction[MasterSystem::Max::SelfIntrotuctionLen];
};

struct TAAddMasterCharacter:public TAHeader
{
	INT64	biCharacterDBID;
	WCHAR	wszSelfIntroduction[MasterSystem::Max::SelfIntrotuctionLen];
};

struct TQDelMasterCharacter:public TQHeader
{
	INT64	biCharacterDBID;
};

struct TADelMasterCharacter:public TAHeader
{

};

struct TQGetPageMasterCharacter:public TQHeader
{
	UINT	uiPage;
	BYTE	cJobCode;		// 0 : 전체
	BYTE	cGenderCode;	// 0 : 전체 1 : 남자 2 : 여자
};

struct TAGetPageMasterCharacter:public TAHeader
{
	UINT		uiPage;
	BYTE		cJobCode;
	BYTE		cGenderCode;
	BYTE		cCount;
	TMasterInfo	MasterInfoList[MasterSystem::Max::MasterPageRowCount];
};

struct TQGetListPupil:public TQHeader
{
	INT64		biCharacterDBID;
};

struct TAGetListPupil:public TAHeader
{
	INT64		biCharacterDBID;
	BYTE		cCount;
	TPupilInfo	PupilInfoList[MasterSystem::Max::PupilCount];
};

struct TQAddMasterAndPupil:public TQHeader
{
	INT64								biMasterCharacterDBID;
	INT64								biPupilCharacterDBID;
	bool								bCheckRegisterFlag;
	bool								bIsDirectMenu;
	DBDNWorldDef::TransactorCode::eCode Code;
	bool								bIsSkip;
};

struct TAAddMasterAndPupil:public TAHeader
{
	INT64								biMasterCharacterDBID;
	INT64								biPupilCharacterDBID;
	bool								bIsDirectMenu;
	DBDNWorldDef::TransactorCode::eCode Code;
	UINT								uiOppositeAccountDBID;
};

struct TQDelMasterAndPupil:public TQHeader
{
	INT64								biMasterCharacterDBID;
	INT64								biPupilCharacterDBID;
	DBDNWorldDef::TransactorCode::eCode	Code;
	int									iPenaltyDay;
	int									iPenaltyRespectPoint;
	bool								bIsSkip;
};

struct TADelMasterAndPupil:public TAHeader
{
	INT64								biMasterCharacterDBID;
	INT64								biPupilCharacterDBID;
	DBDNWorldDef::TransactorCode::eCode	Code;
	int									iPenaltyRespectPoint;
};

struct TQGetMasterCharacter:public TQHeader
{
	DBDNWorldDef::GetMasterCharacterCode::eCode Code;
	INT64										biMasterCharacterDBID;
	INT64										biPupilCharacterDBID;
};

struct TAGetMasterCharacterType1:public TAHeader
{
	INT64					biMasterCharacterDBID;
	TMasterCharacterInfo	MasterCharacterInfo;
};

struct TAGetMasterCharacterType2:public TAHeader
{
	INT64			biPupilCharacterDBID;
	INT64			biMasterCharacterDBID;
	TMyMasterInfo	MasterInfo;
};

struct TAGetMasterCharacterType3:public TAHeader
{
	INT64	biCharacterDBID;
	WCHAR	wszCharName[NAMELENMAX];
	WCHAR	wszSelfIntroduction[MasterSystem::Max::SelfIntrotuctionLen];
	BYTE	cGenderCode;	// 0 : 전체 1 : 남자 2 : 여자
	BYTE	cLevel;
	BYTE	cJob;
	int		iGraduateCount;
	int		iRespectPoint;
};

struct TQGetListMyMasterAndClassmate:public TQHeader
{
	INT64	biCharacterDBID;
};

struct TAGetListMyMasterAndClassmate:public TAHeader
{
	INT64					biCharacterDBID;
	BYTE					cCount;
	TMasterAndClassmateInfo	MasterAndClassmateInfoList[MasterSystem::Max::MasterAndClassmateCount];
};

struct TQGetMyClassmate:public TQHeader
{
	INT64	biCharacterDBID;
	INT64	biClassmateCharacterDBID;
};

struct TAGetMyClassmate:public TAHeader
{
	TClassmateInfo ClassmateInfo;
};

struct TQGraduate:public TQHeader
{
	INT64	biCharacterDBID;
	WCHAR	wszPupilCharName[NAMELENMAX];
};

struct TAGraduate:public TAHeader
{
	char	cWorldSetID;
	WCHAR	wszPupilCharName[NAMELENMAX];
	BYTE	cCount;
	INT64	biMasterCharacterDBIDList[MasterSystem::Max::MasterCount];
};

struct TQModRespectPoint:public TQHeader
{
	INT64	biCharacterDBID;
	int		iRespectPoint;
};

struct TAModRespectPoint:public TAHeader
{
	int		iRespectPoint;
};

struct TQModMasterFavorPoint:public TQHeader
{
	INT64	biMasterCharacterDBID;
	INT64	biPupilCharacterDBID;
	int		iFavorPoint;
};

struct TAModMasterFavorPoint:public TAHeader
{
	INT64	biMasterCharacterDBID;
	INT64	biPupilCharacterDBID;
	int		iFavorPoint;
};

struct TQGetMasterAndFavorPoint:public TQHeader
{
	WCHAR	wszPupilCharName[NAMELENMAX];
	INT64	biPupilCharacterDBID;
	BYTE	cLevel;
};

struct TAGetMasterAndFavorPoint:public TAHeader
{
	WCHAR					wszPupilCharName[NAMELENMAX];
	INT64					biPupilCharacterDBID;
	BYTE					cLevel;
	BYTE					cWorldSetID;
	BYTE					cCount;
	TMasterAndFavorPoint	MasterAndFavorPoint[MasterSystem::Max::MasterCount];
};

struct TQGetMasterSystemCountInfo:public TQHeader
{
	bool	bClientSend;
	INT64	biCharacterDBID;
	BYTE	cCount;
	INT64	biPartyCharacterDBID[PARTYCOUNTMAX-1];
};

struct TAGetMasterSystemCountInfo:public TAHeader
{
	INT64	biCharacterDBID;
	bool	bClientSend;
	int		iMasterCount;
	int		iPupilCount;
	int		iClassmateCount;
};

struct TQModMasterSystemGraduateCount:public TQHeader
{
	INT64	biCharacterDBID;
	int		iGraduateCount;
};

#if defined( PRE_ADD_SECONDARY_SKILL )

struct TQAddSecondarySkill:public TQHeader
{
	INT64									biCharacterDBID;
	int										iSecondarySkillID;
	SecondarySkill::Type::eType				Type;
};

struct TQDelSecondarySkill:public TQHeader
{
	INT64	biCharacterDBID;
	int		iSecondarySkillID;
};

struct TQGetListSecondarySkill:public TQHeader
{
	INT64	biCharacterDBID;
};

struct TAGetListSecondarySkill:public TAHeader
{
	BYTE			cCount;
	TSecondarySkill	SkillList[SecondarySkill::Max::LearnSecondarySkill];
};

struct TQModSecondarySkillExp:public TQHeader
{
	INT64	biCharacterDBID;
	int		iSecondarySkillID;
	int		iSecondarySkillExp;			// 증감치
	int		iSecondarySkillExpAfter;	// 변경 후 exp(비교값)
};

struct TQSetManufactureSkillRecipe:public TQHeader
{
	INT64								biCharacterDBID;
	DBDNWorldDef::ItemLocation::eCode	LocationCode;
	INT64								biItemSerial;
	int									iSecondarySkillID;
};

struct TQExtractManufactureSkillRecipe:public TQHeader
{
	INT64								biCharacterDBID;
	INT64								biItemSerial;
	DBDNWorldDef::ItemLocation::eCode	LocationCode;
};

struct TQDelManufactureSkillRecipe:public TQHeader
{
	INT64	biCharacterDBID;
	INT64	biItemSerial;
	char	szIP[IPLENMAX];
};

struct TQGetListManufactureSkillRecipe:public TQHeader
{
	INT64	biCharacterDBID;
};

struct TAGetListManufactureSkillRecipe:public TAHeader
{
	BYTE					cCount;
	TSecondarySkillRecipe	RecipeList[SecondarySkill::Max::LearnManufactureRecipe];
};

struct TQModManufactureSkillRecipe:public TQHeader
{
	INT64	biCharacterDBID;
	INT64	biItemSerial;
	USHORT	nDurability;			// 증감치
	USHORT	nDurabilityAfter;		// 변경 후 Durability(비교값)
};

#endif // #if defined( PRE_ADD_SECONDARY_SKILL )




struct TQGetListFarm:public TQHeader
{
	char		cEnableFlag;	// 0:Disable 1:Enable -1:ALL
};

struct TAGetListFarm: public TAHeader
{
	BYTE			cCount;
	TFarmItemFromDB	Farms[Farm::Max::FARMCOUNT];
};

struct TQGetListField:public TQHeader
{
	int			iFarmDBID;
	int			iRoomID;
};

struct TAGetListField:public TAHeader
{
	int			iFarmDBID;
	int			iRoomID;
	BYTE		cCount;
	TFarmField	Fields[Farm::Max::FIELDCOUNT];
};

struct TQGetListFieldForCharacter:public TQHeader
{
	int			iRoomID;
	INT64		biCharacterDBID;
};

struct TAGetListFieldForCharacter:public TAHeader
{
	int						iRoomID;			// RoomID 는 절대로 맨 위에 있어야합니다!!!!!!
	INT64					biCharacterDBID;
	BYTE					cCount;
	TFarmFieldForCharacter	Fields[Farm::Max::PRIVATEFIELDCOUNT];
};

struct TQGetListFieldByCharacter:public TQHeader
{
	INT64		biCharacterDBID;
	bool		bRefreshGate;
};

struct TAGetListFieldByCharacter:public TAHeader
{
	bool				bRefreshGate;
	BYTE				cCount;
	TFarmFieldPartial	Fields[Farm::Max::FIELDCOUNT_PERCHARACTER];
};

struct TQAddField:public TQHeader
{
	int						iRoomID;
	int						iFarmDBID;
	short					nFieldIndex;
	INT64					biCharacterDBID;
	int						iItemID;
	int						iElapsedTimeSec;
	int						iAttachItemID;
	int						iMaxFieldCount;
	// 씨앗 제거용 정보
	BYTE					cInvenIndex;
	INT64					biSeedItemSerial;
	int						iMapID;
	WCHAR					wszIP[IPLENMAX];
	BYTE					cAttachCount;
	TFarmAttachItemSerial	AttachItems[Farm::Max::PLANTING_ATTACHITEM];
};

struct TAAddField:public TAHeader
{
	int						iRoomID;			// RoomID 는 절대로 맨 위에 있어야합니다!!!!!!
	short					nFieldIndex;
	INT64					biCharacterDBID;
	// 씨앗 제거용 정보
	BYTE					cInvenIndex;
	INT64					biSeedItemSerial;
	BYTE					cAttachCount;
	TFarmAttachItemSerial	AttachItems[Farm::Max::PLANTING_ATTACHITEM];
};

struct TQDelField:public TQHeader
{
	int		iFarmDBID;
	int		iRoomID;
	short	nFieldIndex;
};

struct TADelField:public TAHeader
{
	int		iRoomID;		// RoomID 는 절대로 맨 위에 있어야합니다!!!!!!
	short	nFieldIndex;
};

struct TQDelFieldForCharacter:public TQHeader
{
	int		iRoomID;
	short	nFieldIndex;
	INT64	biCharacterDBID;
};

struct TADelFieldForCharacter:public TAHeader
{
	int		iRoomID;		// RoomID 는 절대로 맨 위에 있어야합니다!!!!!!
	short	nFieldIndex;
	INT64	biCharacterDBID;
};

struct TQAddFieldAttachment:public TQHeader
{
	int		iRoomID;
	int		iFarmDBID;
	INT64	biCharacterDBID;
	short	nFieldIndex;
	int		iAttachItemID;
	// AttachItem 제거용 정보
#if defined( PRE_ADD_VIP_FARM )
	bool	bVirtualAttach;	// vip농장에 가상 boostitem 주는것 처럼 true 인경우 attach만 이루어지고 아이템 삭제는 없음
#endif // #if defined( PRE_ADD_VIP_FARM )
	BYTE	cInvenType;
	BYTE	cInvenIndex;
	INT64	biAttachItemSerial;
	int		iMapID;
	WCHAR	wszIP[IPLENMAX];
};

struct TAAddFieldAttachment:public TAHeader
{
	int		iRoomID;			// RoomID 는 절대로 맨 위에 있어야합니다!!!!!!
	INT64	biCharacterDBID;
	short	nFieldIndex;
	int		iAttachItemID;
	// AttachItem 제거용 정보
#if defined( PRE_ADD_VIP_FARM )
	bool	bVirtualAttach;	// vip농장에 가상 boostitem 주는것 처럼 true 인경우 attach만 이루어지고 아이템 삭제는 없음
#endif // #if defined( PRE_ADD_VIP_FARM )
	BYTE	cInvenType;
	BYTE	cInvenIndex;
	INT64	biAttachItemSerial;
};

struct TUpdateFieldElapsedTime
{
	short	nFieldIndex;
	int		iElapsedTimeSec;
};

struct TQModFieldElapsedTime:public TQHeader
{
	int						iFarmDBID;
	BYTE					cCount;
	TUpdateFieldElapsedTime	Updates[Farm::Max::FIELDCOUNT];
};

struct TQModFieldForCharacterElapsedTime:public TQHeader
{
	INT64					biCharacterDBID;
	BYTE					cCount;
	TUpdateFieldElapsedTime	Updates[Farm::Max::PRIVATEFIELDCOUNT];
};

struct TAModFieldForCharacterElapsedTime:public TAHeader
{
	
};

struct TAModFieldElapsedTime:public TAHeader
{

};

struct THarvestItem
{
	INT64	biItemSerial;
	int		iItemID;
	short	nItemCount;
	bool	bSoulBoundFlag;
	BYTE	cSealCount;
	BYTE	cItemOption;
	bool	bEternityFlag;
	int		iLifeSpan;
};

struct TQHarvest:public TQHeader
{
	int				iFarmDBID;
	int				iRoomID;
	short			nFieldIndex;
	INT64			biCharacterDBID;
	INT64			biHarvestCharacterDBID;
	int				iMapID;
	CHAR			szIP[IPLENMAX];
	THarvestItem	HarvestItems[Farm::Max::HARVESTITEM_COUNT];
};

struct TAHarvest:public TAHeader
{
	int				iRoomID;			// RoomID 는 절대로 맨 위에 있어야합니다!!!!!!
	int				nFieldIndex;
	INT64			biCharacterDBID;
};

struct TQGetListHarvestDepotItem:public TQHeader
{
	INT64	biCharacterDBID;
	INT64	biLastUniqueID;
};

struct TAGetListHarvestDepotItem:public TAHeader
{
	INT64				biLastUniqueID;
	BYTE				cCount;
	TFarmWareHouseItem	Items[Farm::Max::HARVESTDEPOT_COUNT];
};

struct TQGetCountHarvestDepotItem:public TQHeader
{
	INT64	biCharacterDBID;
};

struct TAGetCountHarvestDepotItem:public TAHeader
{
	int		iCount;
};

struct TQGetFieldCountByCharacter:public TQHeader
{
	INT64	biCharacterDBID;
	bool	bIsSend;
};

struct TAGetFieldCountByCharacter:public TAHeader
{
	int		iFieldCount;
	bool	bIsSend;
};

struct TQUpdateFarmActivation : public TQHeader			//QUERY_UPDATEFARM_ACTIVATION
{
	int nFarmDBID;
};

struct TFarmActivation
{
	int nFarmDBID;
	bool bFarmActivation;
};

struct TAUpdateFarmActivation : public TAHeader			//QUERY_UPDATEFARM_ACTIVATION
{
	BYTE cCount;
	TFarmActivation Farms[Farm::Max::FARMCOUNT];
};

struct TQGetFieldItemCount:public TQHeader
{
	INT64	biCharacterDBID;
	int		iItemID;
	int		iRoomID;
	int		iAreaIndex;
	UINT nSessionID;
};

struct TAGetFieldItemCount:public TAHeader
{
	int		iRoomID;
	int		iAreaIndex;
	int		iItemCount;
	UINT nSessionID;
};



struct TQGetGuildRecruit:public TQHeader
{
	UINT	uiPage;
	BYTE	cJobCode;
	BYTE	cLevel;	
#if defined( PRE_ADD_GUILD_EASYSYSTEM )
	BYTE	cPurposeCode;
	WCHAR wszGuildName[GUILDNAME_MAX];
	BYTE	cSortType;
#endif
};
struct TAGetGuildRecruit:public TAHeader
{
	UINT		uiPage;
	BYTE		cJobCode;
	BYTE		cLevel;
	BYTE		cCount;
#if defined( PRE_ADD_GUILD_EASYSYSTEM )
	BYTE	cPurposeCode;
	WCHAR wszGuildName[GUILDNAME_MAX];
	BYTE	cSortType;
#endif
	TGuildRecruitInfo	GuildRecruitList[GuildRecruitSystem::Max::GuildRecruitRowCount];
};
struct TQGetGuildRecruitCharacter:public TQHeader
{		
	UINT nGuildDBID;	// 길드 DBID
};
struct TAGetGuildRecruitCharacter:public TAHeader
{
	UINT nGuildDBID;	// 길드 DBID
	char cWorldSetID;	// 월드 ID
	BYTE cCount;
	TGuildRecruitCharacter	GuildRecruitCharacterList[GuildRecruitSystem::Max::GuildRecruitCharacterRowCount];
};
struct TQGetMyGuildRecruit:public TQHeader
{
	INT64 biCharacterDBID;	// 캐릭터코드
};
struct TAGetMyGuildRecruit:public TAHeader
{
	BYTE				cCount;
	TGuildRecruitInfo	GuildRecruitList[GuildRecruitSystem::Max::MaxRequestCount];
};
struct TQGetGuildRecruitRequestCount:public TQHeader
{
	INT64 biCharacterDBID;	// 캐릭터코드
};
struct TAGetGuildRecruitRequestCount:public TAHeader
{
	BYTE				cCount;	
};
struct TQRegisterInfoGuildRecruit:public TQHeader
{	
	UINT		nGuildDBID;					// 길드 DBID		
};
struct TARegisterInfoGuildRecruit:public TAHeader
{
	UINT		nGuildDBID;					// 길드 DBID	
	BYTE		cClassCode[CLASSKINDMAX];	// 모집클래스
	int			nMinLevel;					// 최소 레벨
	int			nMaxLevel;					// 최대 레벨
	WCHAR		wszGuildIntroduction[GUILDRECRUITINTRODUCE+1];	//길드 소개글
#if defined( PRE_ADD_GUILD_EASYSYSTEM )
	BYTE		cPurposeCode;				// 길드 목적(1:친목 2:전투 3:네스트 4:콜로세움
	bool		bCheckHomePage;				//길드 홈페이지	
#endif
};
struct TQRegisterOnGuildRecruit:public TQHeader
{	
	UINT		nGuildDBID;					// 길드 DBID	
	BYTE		cClassCode[CLASSKINDMAX];	// 모집클래스
	int			nMinLevel;					// 최소 레벨
	int			nMaxLevel;					// 최대 레벨
	WCHAR		wszGuildIntroduction[GUILDRECRUITINTRODUCE+1];	//길드 소개글
#if defined( PRE_ADD_GUILD_EASYSYSTEM )
	BYTE		cPurposeCode;				// 길드 목적(1:친목 2:전투 3:네스트 4:콜로세움
	bool		bCheckHomePage;				//길드 홈페이지
#endif
};
struct TARegisterOnGuildRecruit:public TAHeader
{
};
struct TQRegisterModGuildRecruit:public TQHeader
{	
	UINT		nGuildDBID;					// 길드 DBID	
	BYTE		cClassCode[CLASSKINDMAX];	// 모집클래스
	int			nMinLevel;					// 최소 레벨
	int			nMaxLevel;					// 최대 레벨
	WCHAR		wszGuildIntroduction[GUILDRECRUITINTRODUCE+1];	//길드 소개글
#if defined( PRE_ADD_GUILD_EASYSYSTEM )
	BYTE		cPurposeCode;				// 길드 목적(1:친목 2:전투 3:네스트 4:콜로세움
	bool		bCheckHomePage;				//길드 홈페이지
#endif
};
struct TARegisterModGuildRecruit:public TAHeader
{
};
struct TQRegisterOffGuildRecruit:public TQHeader
{	
	UINT		nGuildDBID;					// 길드 DBID		
};
struct TARegisterOffGuildRecruit:public TAHeader
{
};
struct TQGuildRecruitRequestOn:public TQHeader
{
	INT64		biCharacterDBID;	// 캐릭터코드
	UINT		nGuildDBID;			// 길드 DBID
};
struct TAGuildRecruitRequestOn:public TAHeader
{
};
struct TQGuildRecruitRequestOff:public TQHeader
{
	INT64		biCharacterDBID;	// 캐릭터코드
	UINT		nGuildDBID;			// 길드 DBID
};
struct TAGuildRecruitRequestOff:public TAHeader
{
};
struct TQGuildRecruitAcceptOn:public TQHeader
{
	INT64		biCharacterDBID;	// 캐릭터코드
	UINT		nGuildDBID;			// 길드 DBID
	INT64		biAcceptCharacterDBID;	// 가입승인한 캐릭터코드
	WCHAR		wszToCharacterName[NAMELENMAX];
	short		wGuildSize;			// 최대 길드원 수	
};
struct TAGuildRecruitAcceptOn:public TAHeader
{
	UINT		nAddAccountDBID;			//추가할 DBID
	INT64		biAddCharacterDBID;			// 추가할 캐릭터 DBID	
	WCHAR		wszToCharacterName[NAMELENMAX];
	BYTE		cJob;						// 직업	
	BYTE		cLevel;						// 레벨	
	__time64_t	JoinDate;					// 길드 가입 날짜
	bool		bDelGuildRecruit;			// 길드모집게시판유지	
};
struct TQGuildRecruitAcceptOff:public TQHeader
{
	INT64		biCharacterDBID;	// 캐릭터코드	
	UINT		nGuildDBID;			// 길드 DBID
	INT64		biDenyCharacterDBID;	// 거절할 캐릭터코드
	WCHAR		wszToCharacterName[NAMELENMAX];
};
struct TAGuildRecruitAcceptOff:public TAHeader
{
	WCHAR		wszToCharacterName[NAMELENMAX];
	INT64		biDenyCharacterDBID;// 거절할캐릭터코드
};
struct TQDelGuildRecruit:public TQHeader
{
	INT64		biCharacterDBID;	// 캐릭터코드	
	UINT		nGuildDBID;			// 길드 DBID
	BYTE		nDelType;			// 삭제타입(1:길드 2:개인)
};

#if defined (PRE_ADD_DONATION)
struct TQDonate : public TQHeader
{
	INT64 biCharacterDBID;
	time_t tDonationTime;
	INT64 nCoin;
};

struct TADonate : public TAHeader
{
};

struct TQDonationRanking : public TQHeader
{
	INT64 biCharacterDBID;
};

struct TADonationRaking : public TAHeader
{
	Donation::Ranking Ranking[Donation::MAX_RANKING];
	INT64 nMyCoin;
	int nMyRanking;
};

struct TQDonationTopRanker : public TQHeader
{

};

struct TADonationTopRanker : public TAHeader
{
	char cWorldID;
	INT64 biCharacterDBID;
	WCHAR wszCharacterName[NAMELENMAX];
	INT64 nCoin;
};
#endif // #if defined (PRE_ADD_DONATION)

struct TQMaintenanceInfo : public TQHeader
{
	int nFlag;
};

#if defined( PRE_PARTY_DB )

struct DBCreateParty
{
	Party::Data PartyData;
#if defined(PRE_FIX_62281)
	WCHAR wszAfterInviteCharacterName[NAMELENMAX];
#else
	UINT uiAfterInviteAccountDBID;	
#endif
};

struct TQAddParty:public TQHeader
{
	DBCreateParty Data;
	bool bCheat;
};

struct TAAddParty:public TAHeader
{
	DBCreateParty Data;
	bool bCheat;
};

struct DBAddPartyMemberGame
{	
	INT64 biCharacterDBID;
	UINT nSessionID;
#if defined( PRE_ADD_NEWCOMEBACK )
	bool bCheckComebackAppellation;
#endif
};
struct TQAddPartyAndMemberGame:public TQHeader
{
	DBCreateParty Data;
	UINT nKickedMemberList[PARTYKICKMAX];
	DBAddPartyMemberGame Member[PARTYMAX];	// 파티원만 가변이고 위에 킥멤버는 가변 아님
};
struct TAAddPartyAndMemberGame:public TAHeader
{
	DBCreateParty Data;
	UINT nKickedMemberList[PARTYKICKMAX];
};
struct TQAddPartyAndMemberVillage:public TQHeader
{
	DBCreateParty Data;		
	int nKickCount;
	UINT nVoiceChannelID;
	UINT nKickedMemberList[PARTYKICKMAX];	
	TMemberInfo MemberInfo[PARTYMAX];	
};
struct TAAddPartyAndMemberVillage:public TAHeader
{
	DBCreateParty Data;	
	UINT nVoiceChannelID;
	UINT nKickedMemberList[PARTYKICKMAX];	
	TMemberInfo MemberInfo[PARTYMAX];
};

struct TQModParty:public TQHeader
{
	Party::Data PartyData;	
	int iRoomID; // 게임서버에서만 사용
};

struct TAModParty:public TAHeader
{
	Party::Data PartyData;	
	int iRoomID; // 게임서버에서만 사용
};

struct TQDelParty:public TQHeader
{
	TPARTYID PartyID;
};

struct TADelParty:public TAHeader
{
	TPARTYID PartyID;
};

struct TQDelPartyForServer:public TQHeader
{
	int iServerID;
};

struct TADelPartyForServer:public TAHeader
{
	int iServerID;
};

struct TQJoinParty:public TQHeader
{
	TPARTYID PartyID;
	INT64 biCharacterDBID;
	UINT nSessionID;
	int iMaxUserCount;
#if defined( PRE_ADD_NEWCOMEBACK )
	bool bCheckComeBackParty;
#endif
};

struct TAJoinParty:public TAHeader
{
	TPARTYID PartyID;
	INT64 biCharacterDBID;
	UINT nSessionID;
	BYTE cThreadID;
};

struct TQOutParty:public TQHeader
{
	TPARTYID PartyID;
	INT64 biCharacterDBID;
	UINT nSessionID;
	Party::QueryOutPartyType::eCode Type;
};

struct TAOutParty:public TAHeader
{
	TPARTYID PartyID;
	INT64 biCharacterDBID;
	UINT nSessionID;
	Party::QueryOutPartyType::eCode Type;
	bool bIsDelParty;
};

struct TQGetListParty:public TQHeader
{
	int iExecptServerID;
};

struct TAGetListParty:public TAHeader
{
	bool bIsStart;
	short nCount;
	Party::Data DataArr[100];
};

struct TQGetListPartyMember:public TQHeader
{
	TPARTYID PartyID;
};

struct TAGetListPartyMember:public TAHeader
{
	TPARTYID PartyID;
	ePartyType PartyType;
	short nCount;
	Party::MemberData MemberData[PARTYCOUNTMAX];
};

struct TQModPartyLeader:public TQHeader
{
	TPARTYID PartyID;
	INT64 biCharacterDBID;
};

struct TAModPartyLeader:public TAHeader
{

};

#if defined( PRE_ADD_NEWCOMEBACK )
struct TQModComebackFlag:public TQHeader
{
	bool bRewardComeback;
};

struct TQModPartyMemberComeback:public TQHeader
{
	TPARTYID PartyID;
	INT64 biCharacterDBID;
	bool bComebackAppellation;
};
#endif	// #if defined( PRE_ADD_NEWCOMEBACK )
#endif // #if defined( PRE_PARTY_DB )
#if defined(PRE_ADD_DOORS_PROJECT)
struct TQSaveCharacterAbility : public TQHeader
{
	INT64 biCharacterDBID;
	int nCharacterAblity[Doors::CharacterAbility::MaxType];	
};
#endif // #if defined(PRE_ADD_DOORS_PROJECT)

#if defined (PRE_ADD_BESTFRIEND)

// QUERY_GET_BESTFRIEND
struct TQGetBestFriend: public TQHeader
{
	bool bSend;
	INT64 biCharacterDBID;
};

struct TAGetBestFriend: public TAHeader
{
	bool bSend;
	TBestFriendInfo Info;
};

// QUERY_REGIST_BESTFRIEND
struct TQRegistBestFriend: public TQHeader
{
	UINT nFromAccountDBID;
	INT64 biFromCharacterDBID;
	WCHAR wszFromName[NAMELENMAX];

	UINT nToAccountDBID;
	INT64 biToCharacterDBID;
	WCHAR wszToName[NAMELENMAX];

	int nItemID;

	// 아이템제거용정보
	INT64 biRegistSerial;
	int	nMapID;
	WCHAR wszIP[IPLENMAX];	
};

struct TARegistBestFriend: public TAHeader
{
	UINT nFromAccountDBID;
	INT64 biFromCharacterDBID;
	WCHAR wszFromName[NAMELENMAX];

	UINT nToAccountDBID;
	WCHAR wszToName[NAMELENMAX];

	int nItemID;
	INT64 biRegistSerial;
};

// QUERY_CANCEL_BESTFRIEND
struct TQCancelBestFriend: public TQHeader
{
	INT64 biCharacterDBID;
	bool bCancel;
};

struct TACancelBestFriend: public TAHeader
{
	INT64 biCharacterDBID;
	bool bCancel;
};

// QUERY_CLOSE_BESTFRIEND
struct TQCloseBestFriend: public TQHeader
{
	INT64 biCharacterDBID;
};

struct TACloseBestFriend: public TAHeader
{
	INT64 biCharacterDBID;
};

// QUERY_EDIT_BESTFRIENDMEMO
struct TQEditBestFriendMemo: public TQHeader
{
	INT64 biCharacterDBID;
	WCHAR wszMemo[BESTFRIENDMEMOMAX];
};

struct TAEditBestFriendMemo: public TAHeader
{
	INT64 biToCharacterDBID;
	WCHAR wszMemo[BESTFRIENDMEMOMAX];
};

#endif // #if defined (PRE_ADD_BESTFRIEND)

#if defined( PRE_ADD_58761 )
struct TQNestDeathLog: public TQHeader
{
	INT64 biCharacterDBID;
	INT64 biRoomID;
	int nMonterID;
	int nSkillID;
	BYTE cCharacterJob;
	BYTE cCharacterLevel;
};
#endif


//#########################################################################################################################################
//	MAINCMD_PRIVATECHATCHANNEL
//#########################################################################################################################################

#if defined( PRE_PRIVATECHAT_CHANNEL )
struct TQPrivateChatChannelInfo: public TQHeader
{	
};
struct TAPrivateChatChannelInfo: public TAHeader
{	
	char cWorldSetID;
	int nCount;
	TPrivateChatChannelInfo tPrivateChatChannel[PrivateChatChannel::Common::GetDBMaxChannel];		
};

struct TQPrivateChatChannelMember: public TQHeader
{
	INT64 nPrivateChatChannelID;
};

struct TAPrivateChatChannelMember: public TAHeader
{	
	char cWorldSetID;
	INT64 nPrivateChatChannelID;
	int nCount;
	TPrivateChatChannelMember Member[PrivateChatChannel::Common::MaxMemberCount];
};

struct TQAddPrivateChatChannel: public TQHeader
{	
	time_t tJoinDate;
	WCHAR wszCharacterName[NAMELENMAX];
	TPrivateChatChannelInfo tPrivateChatChannel;		
};

struct TAAddPrivateChatChannel: public TAHeader
{
	char cWorldSetID;
	time_t tJoinDate;
	WCHAR wszCharacterName[NAMELENMAX];
	TPrivateChatChannelInfo tPrivateChatChannel;
};

struct TQAddPrivateChatMember: public TQHeader
{
	int nServerID;
	PrivateChatChannel::Common::eModType eType;
	INT64 nPrivateChatChannelID;	
	TPrivateChatChannelMember Member;
};

struct TAAddPrivateChatMember: public TAHeader
{
	char cWorldSetID;
	INT64 nPrivateChatChannelID;
	TPrivateChatChannelMember Member;
};

struct TQInvitePrivateChatMember: public TQHeader
{
	int nServerID;
	UINT nMasterAccountDBID;
	INT64 nPrivateChatChannelID;	
	TPrivateChatChannelMember Member;
};

struct TAInvitePrivateChatMember: public TAHeader
{
	char cWorldSetID;
	UINT nMasterAccountDBID;
	INT64 nPrivateChatChannelID;
	TPrivateChatChannelMember Member;
};

struct TQDelPrivateChatMember: public TQHeader
{
	PrivateChatChannel::Common::eModType eType;
	INT64 biCharacterDBID;
	WCHAR wszName[NAMELENMAX];
	INT64 nPrivateChatChannelID;
};

struct TADelPrivateChatMember: public TAHeader
{		
	char cWorldSetID;
	INT64 biCharacterDBID;
	WCHAR wszName[NAMELENMAX];
	INT64 nPrivateChatChannelID;
};

struct TQModPrivateChatChannelInfo: public TQHeader
{
	PrivateChatChannel::Common::eModType eType;
	INT64 nPrivateChatChannelID;
	WCHAR wszChannelName[PrivateChatChannel::Common::MaxNameLen];
	int nPassWord;
	INT64 biCharacterDBID;	
};

struct TAModPrivateChatChannelInfo: public TAHeader
{
	char cWorldSetID;
	PrivateChatChannel::Common::eModType eType;
	INT64 nPrivateChatChannelID;
	WCHAR wszChannelName[PrivateChatChannel::Common::MaxNameLen];
	int nPassWord;
	INT64 biCharacterDBID;	
};

struct TQModPrivateChatMemberServerID: public TQHeader
{
	int nServerID;	
	INT64 biCharacterDBID;
};

struct TQDelPrivateChatMemberServerID: public TQHeader
{
	int nServerID;		
};

struct TADelPrivateChatMemberServerID: public TAHeader
{
	char cWorldSetID;
	int nCount;
	TPrivateMemberDelServer Member[PrivateChatChannel::Common::MaxMemberCount];	
};
#endif

#if defined( PRE_FIX_67546 )

struct TQAddThreadCount: public TQHeader
{
	BYTE cChoiceThreadID;
};

struct TAAddThreadCount: public TAHeader
{
	int nConnectionCount[THREADMAX];
};

#endif

#if defined( PRE_WORLDCOMBINE_PVP )

struct TQAddWorldPvPRoom: public TQHeader
{
	TWorldPvPMissionRoom TMissionRoom;
	int nServerID;	
	int nRoomID;
	WCHAR wszRoomName[PARTYNAMELENMAX];		
	bool bBreakIntoFlag;
	bool bRegulationFlag;
	bool bObserverFlag;
	bool bRandomFlag;	
	int nPassWord;	
};

struct TAAddWorldPvPRoom: public TAHeader
{
	char cWorldSetID;
	int nRoomIndex;
	int nRoomID;
	int nPvPRoomDBIndex;
	TWorldPvPRoomDBData Data;
};

struct TQAddWorldPvPRoomMember: public TQHeader
{
	int nWorldPvPRoomDBIndex;
	INT64 biCharacterDBID;
	bool bObserverFlag;
	BYTE cMaxPlayers;
};

struct TQDelWorldPvPRoom: public TQHeader
{	
	int nWorldPvPRoomDBIndex;	
};

struct TQDelWorldPvPRoomMember: public TQHeader
{
	int nWorldPvPRoomDBIndex;
	INT64 biCharacterDBID;	
};

struct TQDelWorldPvPRoomForServer: public TQHeader
{	
	int nServerID;	
};

struct TQGetListWorldPvPRoom: public TQHeader
{	
	int nCombineWorldID;
};

struct TAGetListWorldPvPRoom: public TAHeader
{
	int nCount;
	TWorldPvPRoomDBData WorldPvPRoomData[WorldPvPMissionRoom::Common::MaxDBDataCount];
};

struct TQUpdateWorldPvPRoom: public TQHeader
{	
};

struct TAUpdateWorldPvPRoom: public TAHeader
{
	bool bFlag;
};

#endif

//#########################################################################################################################################
//	MAINCMD_ACTOZCOMMON
//#########################################################################################################################################

// QUERY_UPDATECHARACTERINFO
struct TQActozUpdateCharacterInfo: public TQHeader
{
	char cUpdateType;
	INT64 biCharacterDBID;
	BYTE cJob;
	int nLevel;
	int nExp;
	INT64 biCoin;
	char szIp[IPLENMAX];
};

// QUERY_UPDATECHARACTERNAME
struct TQActozUpdateCharacterName: public TQHeader
{
	INT64 biCharacterDBID;
	char szCharacterName[NAMELENMAX];
};


//#########################################################################################################################################
//	MAINCMD_ALTEIAWORLD
//#########################################################################################################################################

#if defined( PRE_ALTEIAWORLD_EXPLORE )

struct TQResetAlteiaWorldEvent: public TQHeader
{	
};

struct TQAddAlteiaWorldEvent: public TQHeader
{
	time_t tAlteiaEventStartTime;
	time_t tAlteiaEventEndTime;
};

struct TQADDAlteiaWorldPlayResult: public TQHeader
{	
	INT64 biCharacterDBID;
	int nGoldKeyCount;
	DWORD dwPlayTime;
	int nGuildID;
};

struct TQGetAlteiaWorldInfo: public TQHeader
{	
	INT64 biCharacterDBID;	
};

struct TAGetAlteiaWorldInfo: public TAHeader
{
	int nBestGoldKeyCount;
	UINT uiBestPlayTime;	
	int nDailyPlayCount;
	int nWeeklyPlayCount;
};

struct TQGetAlteiaWorldRankInfo: public TQHeader
{	
	int nMaxRank;
	AlteiaWorld::Info::eInfoType eType;
};

struct TAGetAlteiaWorldPrivateGoldKeyRank: public TAHeader
{
	int nCount;
	AlteiaWorld::GoldKeyRankMemberInfo RankMemberInfo[AlteiaWorld::Common::MaxRankCount];
};

struct TAGetAlteiaWorldPrivatePlayTimeRank: public TAHeader
{
	int nCount;
	AlteiaWorld::PlayTimeRankMemberInfo RankMemberInfo[AlteiaWorld::Common::MaxRankCount];
};

struct TAGetAlteiaWorldGuildGoldKeyRank: public TAHeader
{
	int nCount;
	AlteiaWorld::GuildGoldKeyRankInfo RankMemberInfo[AlteiaWorld::Common::MaxRankCount];
};

struct TQGetAlteiaWorldSendTicketList: public TQHeader
{	
	INT64 biCharacterDBID;
};

struct TAGetAlteiaWorldSendTicketList: public TAHeader
{
	int nSendTicketCount;
	int nRecvTicketCount;
	INT64 biCharacterDBID[AlteiaWorld::Common::MaxSendCount];
};

struct TQAddAlteiaWorldSendTicketList: public TQHeader
{	
	INT64 biCharacterDBID;
	INT64 biSendCharacterDBID;
	WCHAR wszSendCharacterName[NAMELENMAX];
	int nSendMaxCount;
};

struct TAAddAlteiaWorldSendTicketList: public TAHeader
{	
	INT64 biSendCharacterDBID;
	WCHAR wszSendCharacterName[NAMELENMAX];
};

struct TQResetAlteiaWorldPlayAlteia: public TQHeader
{	
	INT64 biCharacterDBID;
	AlteiaWorld::ResetType::eResetType eType;
};

#endif // #if defined( PRE_ALTEIAWORLD_EXPLORE )


//#########################################################################################################################################
//	MAINCMD_STAMPSYSTEM
//#########################################################################################################################################

#if defined( PRE_ADD_STAMPSYSTEM )
struct TQGetListCompleteChallenges: public TQHeader
{
	INT64 biCharacterDBID;
};

struct TAGetListCompleteChallenges: public TAHeader
{
	StampSystem::StampCompleteInfo CompleteInfo[StampSystem::Common::MaxStampSlotCount];
};

struct TQInitCompleteChallenge: public TQHeader
{
	INT64 biCharacterDBID;
};

struct TQAddCompleteChallenge: public TQHeader
{
	INT64 biCharacterDBID;
	BYTE cChallengeIndex;
	int nWeekDay;
};

struct TAAddCompleteChallenge: public TAHeader
{
	BYTE cChallengeIndex;
};

#endif // #if defined( PRE_ADD_STAMPSYSTEM )


//#########################################################################################################################################
//	MAINCMD_DWC
//#########################################################################################################################################

#if defined(PRE_ADD_DWC)
struct TQCreateDWCTeam : public TQHeader
{
	INT64 biCharacterDBID;
	WCHAR wszTeamName[GUILDNAME_MAX];	
};

struct TACreateDWCTeam : public TAHeader
{
	INT64 biCharacterDBID;
	UINT nTeamID;
	WCHAR wszTeamName[GUILDNAME_MAX];	
};

struct TQAddDWCTeamMember : public TQHeader
{
	INT64 biCharacterDBID;
	UINT nTeamID;	
};

struct TAAddDWCTeamMember : public TAHeader
{
	INT64 biCharacterDBID;
	UINT nTeamID;
};

struct TQDWCInviteMember : public TQHeader
{
	INT64 biCharacterDBID;
	UINT nTeamID;
	bool bNeedMembList;	
};

struct TADWCInviteMember : public TAHeader
{
	INT64 biCharacterDBID;
	UINT nTeamID;
	BYTE cMemberCount;
	TDWCTeam Info;	
	TDWCTeamMember MemberList[DWC::DWC_MAX_MEMBERISZE];
};

struct TQDelDWCTeamMember : public TQHeader
{
	INT64 biCharacterDBID;
	UINT nTeamID;	
};

struct TADelDWCTeamMember : public TAHeader
{
	INT64 biCharacterDBID;
	UINT nTeamID;	
};

struct TQGetDWCTeamInfo: public TQHeader
{
	INT64 biCharacterDBID;
	bool bNeedMembList;		// 길드원 목록도 필요한지 여부
};

struct TAGetDWCTeamInfo: public TAHeader
{
	UINT nTeamID;
	TDWCTeam Info;
};

struct TAGetDWCTeamMember: public TAHeader
{
	UINT nTeamID;
	BYTE cCount;			// 팀 정보 개수 (실패일 경우 무시됨)
	TDWCTeamMember MemberList[DWC::DWC_MAX_MEMBERISZE];
};


struct TQAddPvPDWCResult: public TQHeader
{
	int nRoomID;			// 게임서버RoomID
	UINT nTeamID;
	UINT nOppositeTeamID;
	BYTE cPvPLadderCode;	// LadderSystem::MatchType::eCode
	BYTE cResult;			// LadderSystem::MatchResult::eCode
	int nDWCGradePoint;
	int	nHiddenDWCGradePoint;
};

struct TAAddPvPDWCResult: public TAHeader
{
	int nRoomID;
	UINT nTeamID;
	TDWCScore DWCScore;
};

struct TQGetListDWCScore: public TQHeader
{
	int nRoomID;			// 게임서버RoomID
	INT64 biATeamCharacterDBID;
	INT64 biBTeamCharacterDBID;
};

struct TAGetListDWCScore: public TAHeader
{
	int nRoomID;
	UINT nATeamID;
	TDWCTeam ATeamInfo;
	UINT nBTeamID;
	TDWCTeam BTeamInfo;
};

struct TQGetDWCRankPage : public TQHeader
{
	int nPageNum;
	int nPageSize;		//DWC::eCommon::RankPageMaxSize
};

struct TAGetDWCRankPage : public TAHeader
{
	UINT nTotalRankSize;
	int nPageNum;
	int nPageSize;
	TDWCRankData RankList[DWC::Common::RankPageMaxSize];
};

struct TQGetDWCFindRank	: public TQHeader
{
	BYTE cFindType;		//DWC::FindRankType참조
	WCHAR wszFindKey[NAMELENMAX + GUILDNAME_MAX];
};

struct TAGetDWCFindRank : public TAHeader
{
	int nRetCode;
	TDWCRankData Rank;
};

struct TQGetDWCChannelInfo : public TQHeader
{
};

struct TAGetDWCChannelInfo : public TAHeader
{
	TDWCChannelInfo ChannelInfo;
};

#endif

#pragma pack(pop)

