#include "stdafx.h"
#include "DnCommonUtil.h"
#include "SundriesFunc.h"

#if defined _CLIENT || defined _SERVER

#if defined _GAMESERVER || defined _CLIENT
#include "DnActor.h"
#include "DnItem.h"
#include "DnParts.h"
#endif

#include <wininet.h>
#include <sys/stat.h>

#include "md5.h"
#include "DnTableDB.h"
#include "EtUIXML.h"

#ifdef _CLIENT
#include "DnItemTask.h"
#ifdef PRE_MOD_GACHA_SYSTEM
#include "DnInterfaceString.h"
#endif // PRE_MOD_GACHA_SYSTEM
#endif // _CLIENT

#else // defined _CLIENT || defined _SERVER

#include "DNTableFile.h"

//	For ActionTool
#ifdef _ACTIONTOOL
#include "../Client/EtActionTool/DnGameTable.h"
#endif

#endif // defined _CLIENT || defined _SERVER

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif 
int g_JobKindMax = 0;

namespace CommonUtil
{
#if defined _CLIENT || defined _SERVER
	bool IsValidCharacterNameLen(int len)
	{
		if (len > NAMELENMAX - 1)
		{
			_ASSERT(0 && _T("INVALID NAME LENGTH!!"));
			return false;
		}
		return true;
	}

	bool IsValidPartyNameLen(int len)
	{
		if (len > PARTYNAMELENMAX - 1)
		{
			_ASSERT(0 && _T("INVALID PARTY NAME LENGTH!!"));
			return false;
		}

		return true;
	}

#if defined( PRE_PARTY_DB )
	eErrorPartySearchWord IsValidPartySearchWord(const WCHAR* pSearchWord)
	{
		if (pSearchWord == NULL)
			return ePARTYWORD_NULL_STRING;

		int len = int(wcslen(pSearchWord));
		if (len < 0 || len >= Party::Max::SearchWordLen - 1)
			return ePARTYWORD_OVER_LENGTH;

		int i = 0, count = 0;
		for (; i < len; ++i)
		{
			if (pSearchWord[i] == ' ')
			{
				++count;
				if (count >= Party::Max::WordSpacing)
					return ePARTYWORD_OVER_SPACING;
			}
		}

		return ePARTYWORD_NONE;
	}
#else
	bool IsValidPartyPwdLen(int len)
	{
		if (len > PARTYPASSWORDMAX - 1)
		{
			_ASSERT(0 && _T("INVALID PARTY PASSWORD LENGTH!!"));
			return false;
		}
		return true;
	}
#endif // #if defined( PRE_PARTY_DB )

	bool IsValidDungeonDifficulty(TDUNGEONDIFFICULTY difficulty)
	{
		return (difficulty >= Dungeon::Difficulty::Easy && difficulty < Dungeon::Difficulty::Max);
	}

	bool IsValidFriendGroupNameLen(int len)
	{
		if (len > FRIEND_GROUP_NAMELENMAX - 1)
		{
			_ASSERT(0 && _T("INVALID FRIEND GROUP NAME LENGTH!!"));
			return false;
		}

		return true;
	}

	bool IsCtrlChar(WCHAR ch)
	{
		switch(ch)
		{
		case VK_BACK:
		case VK_DELETE:
		case 24:        // Ctrl-X Cut
		case VK_CANCEL: // Ctrl-C Copy
		case 22:		// Ctrl-V Paste
		case 1:			// Ctrl-A Select All
		case VK_RETURN:
		case 26:  // Ctrl Z
		case 2:   // Ctrl B
		case 14:  // Ctrl N
		case 19:  // Ctrl S
		case 4:   // Ctrl D
		case 6:   // Ctrl F
		case 7:   // Ctrl G
		case 10:  // Ctrl J
		case 11:  // Ctrl K
		case 12:  // Ctrl L
		case 17:  // Ctrl Q
		case 23:  // Ctrl W
		case 5:   // Ctrl E
		case 18:  // Ctrl R
		case 20:  // Ctrl T
		case 25:  // Ctrl Y
		case 21:  // Ctrl U
		case 9:   // Ctrl I
		case 15:  // Ctrl O
		case 16:  // Ctrl P
		case 27:  // Ctrl [
		case 29:  // Ctrl ]
		case 28:  // Ctrl 
			{
				return true;
			}
		}

		return false;
	}

	TAX_TYPE CalcTax(int taxType, int charLev, MONEY_TYPE attachAmount)
	{
		DNTableFileFormat *pSox = GetDNTable(CDnTableDB::TTAX);
		if (!pSox)
		{
			_ASSERT(0);
			return 0;
		}

		for (int i = 1; i <= pSox->GetItemCount(); i++)
		{
			int nItemID = pSox->GetItemID(i);
			int taxSort = pSox->GetFieldFromLablePtr(nItemID, "_TaxSort")->GetInteger();
			if (taxSort != taxType)
				continue;

			int startLev	= pSox->GetFieldFromLablePtr(nItemID, "_LevelS")->GetInteger();
			int endLev		= pSox->GetFieldFromLablePtr(nItemID, "_LevelE")->GetInteger();
			if (charLev < startLev || charLev > endLev)
				continue;

			int startAmount	= pSox->GetFieldFromLablePtr(nItemID, "_AmountS")->GetInteger();	// Gold단위
			int endAmount	= pSox->GetFieldFromLablePtr(nItemID, "_AmountE")->GetInteger();
			int attachGold	= (int)(attachAmount / 10000);
			if (attachGold < startAmount || attachGold > endAmount)
				continue;

			if (taxType == TAX_POSTDEFAULT)
				return TAX_TYPE(pSox->GetFieldFromLablePtr(nItemID, "_TaxAmount")->GetInteger());

			float taxRate = pSox->GetFieldFromLablePtr(nItemID, "_Rate")->GetFloat();

			return TAX_TYPE(attachAmount * taxRate + 0.5f );
		}

		_ASSERT(0);
		return 0;
	}

	TAX_TYPE GetFixedTax(int taxType)
	{
		DNTableFileFormat *pSox = GetDNTable(CDnTableDB::TTAX);
		if (!pSox)
		{
			_ASSERT(0);
			return 0;
		}

		int i = 1;
		for (; i <= pSox->GetItemCount(); i++)
		{
			int nItemID = pSox->GetItemID(i);
			int taxSort = pSox->GetFieldFromLablePtr(nItemID, "_TaxSort")->GetInteger();
			if (taxSort != taxType)
				continue;

			return TAX_TYPE(pSox->GetFieldFromLablePtr(nItemID, "_TaxAmount")->GetInteger());
		}

		return 0;
	}

	void GetCashRemainDate(const time_t& nRemain, WCHAR *pRemainDate, size_t SizeInWord)
	{
		tm Day;
		if( !DnLocalTime_s(&Day, &nRemain) ) return;

#ifdef _US
		wcsftime(pRemainDate, SizeInWord, L"%m,%d,%Y,%H,%M,%S", &Day);
#elif _RU
		wcsftime(pRemainDate, SizeInWord, L"%d,%m,%Y,%H,%M,%S", &Day);
#else // _US
		wcsftime(pRemainDate, SizeInWord, L"%Y,%m,%d,%H,%M,%S", &Day);
#endif // _US
	}

	bool ConvertTimeFormatString(WCHAR *dateString, std::wstring& stringProcessed)
	{
		
#ifdef _US
		enum
		{
			MONTH,
			DAY,
			YEAR,
			HOUR,
			MINUTE,
			SECOND,
			DATE_MAX
		};
#elif _RU
		enum
		{
			DAY,
			MONTH,
			YEAR,
			HOUR,
			MINUTE,
			SECOND,
			DATE_MAX
		};
#else
		enum
		{
			YEAR,
			MONTH,
			DAY,
			HOUR,
			MINUTE,
			SECOND,
			DATE_MAX
		};
#endif // _US

		std::vector<std::wstring> tokens;
		TokenizeW(std::wstring(dateString ? dateString : L""), tokens, L",");

		std::wstring dateSeperator = L"/";
		std::wstring timeSeperator = L":";

#ifdef _US
		dateSeperator = L"/";
#elif _RU
		dateSeperator = L".";
#endif

		if (tokens.size() == DATE_MAX)
		{
			int i = 0;
			for (; i < DATE_MAX; ++i)
			{
				//마지막 초단위 정보는 추가 하지 않는다.
				if (i == SECOND)
					continue;

				std::wstring& str = tokens[i];

				if (i == YEAR)
				{
					str.substr(2, std::wstring::npos);
				}

				stringProcessed += str;

				//앞에 3개는 날짜 포맷...(맨 마지막 날짜(i==2)인 경우 공백 추가
				if (i < HOUR)
				{
					if (i == 2)
						stringProcessed += L" ";
					else
						stringProcessed += dateSeperator;
				}
				//시간 포맷
				else if (i >= HOUR)
				{
					//분단위는 마지막이라 시간 분리자 추가 안함
					if (i < MINUTE)
						stringProcessed += timeSeperator;
				}
			}
		}

		return true;
	}

#ifdef _CLIENT
	void GetDateString(eDateStringType type, std::wstring& result, const tm& info)
	{
		std::wstring::size_type startPos = 0;
		std::wstring::size_type endPos = 0;

		std::wstring cur;

		int uiStringNum = -1;
		if (type == DATESTR_FULL)			uiStringNum = 1981;
		else if (type == DATESTR_COMPACT)	uiStringNum = 4681;
		else if (type == DATESTR_REFUNDCASHINVEN) uiStringNum = 4803;
		else if (type == DATESTR_ACCEPT_LIMITTIME ) uiStringNum = 8204;

		std::wstring formatString = GetEtUIXML().GetUIString(CEtUIXML::idCategory1, uiStringNum);
		startPos = endPos = 0;
		while(endPos != std::string::npos)
		{
			startPos = formatString.find_first_of(L"<");
			endPos = formatString.find_first_of(L">", startPos);

			if (endPos == std::string::npos ||
				startPos == std::string::npos)
				break;

			cur = formatString.substr(startPos, endPos - startPos + 1);
			if (cur == TEXT("<YEAR>"))			cur = FormatW(L"%d", info.tm_year + 1900);
			else if (cur == TEXT("<MONTH>"))		cur = FormatW(L"%d", info.tm_mon + 1);
			else if (cur == TEXT("<DAY>"))		cur = FormatW(L"%d", info.tm_mday);
			else if (cur == TEXT("<AMPM>"))		cur = ((info.tm_hour / 12) < 1) ? L"AM" : L"PM";
			else if (cur == TEXT("<TIME>"))		cur = FormatW(L"%d:%02d", info.tm_hour % 12, info.tm_min);

			formatString.replace(startPos, endPos - startPos + 1, cur);
		}

		result = formatString;
	}

	bool IsInfiniteRebirthOnDungeon()
	{
		return (CDnItemTask::GetInstance().GetUsableRebirthCoin()  == -1);
	}

#ifdef PRE_ADD_ITEM_GAINTABLE
	void GetItemDescriptionInGainTable(std::string& result, const int& nItemID)
	{
		DNTableFileFormat* pGainTable = GetDNTable(CDnTableDB::TITEMGAIN);
		if (pGainTable == NULL)
			return;

		DNTableFileFormat::Cell* pCell = pGainTable->GetFieldFromLablePtr(nItemID, "_ItemGainText");
		if (pCell == NULL)
			return;

		result = pCell->GetString();
	}
#endif

#ifdef PRE_ADD_EQUIPLOCK
	bool IsLockableEquipItem(eItemTypeEnum type)
	{
		if (type == ITEMTYPE_PARTS ||
			type == ITEMTYPE_WEAPON ||
			type == ITEMTYPE_PARTS_RING)
		{
			return true;
		}

		return false;
	}
#endif
#endif // _CLIENT

#if defined( PRE_PARTY_DB )
	const WCHAR* GetDungeonDifficultyString( Dungeon::Difficulty::eCode Difficulty )
	{
		switch( Difficulty )
		{
			case Dungeon::Difficulty::Easy: return GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 2370 );	//	UISTRING : 쉬움
			case Dungeon::Difficulty::Normal: return GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 2371 );	//	UISTRING : 보통
			case Dungeon::Difficulty::Hard: return GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 2372 ); 	//	UISTRING : 어려움
			case Dungeon::Difficulty::Master: return GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 2373 );	//	UISTRING : 마스터
			case Dungeon::Difficulty::Abyss: return GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 2374); 	//	UISTRING : 어비스
			//case Dungeon::Difficulty::Chaos: return GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 2374); 	
			//case Dungeon::Difficulty::Hell: return GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 2374); 	
		}
		return NULL;
	}
#endif // #if defined( PRE_PARTY_DB )

	void GetDateStringForMD5(std::string& result, const tm& info)
	{
		result.clear();
		int hour = info.tm_hour % 12;
		if (hour == 0) hour = 12;

		result = FormatA("%04d%02d%02d%02d", info.tm_year + 1900, info.tm_mon + 1, info.tm_mday, hour);
	}

	bool IsValidCharacterClassId(int classId)
	{
		return (classId <= CLASSKINDMAX && classId >= 1);
	}

	bool IsValidCharacterJobId(int jobId)
	{
		return (jobId <= g_JobKindMax && jobId >= 1);
	}

#if defined _CLIENT || defined _GAMESERVER
	bool IsValidPartsType(int partsType, bool bCash)
	{
		if (partsType > EQUIPMAX)
			return false;

		return (bCash) ? (partsType >= 0 && partsType < CDnParts::PartsTypeEnum_Amount) : (partsType >= 0 && partsType < CDnParts::CashPartsTypeEnum_Amount);
	}
#endif

	void GetMD5(std::string& result, const char* str)
	{
		md5_state_t state;
		md5_byte_t digest[MD5_DIGEST_LENGTH];

		md5_init(&state);
		md5_append(&state, (const md5_byte_t *)str, (int)strlen(str));
		md5_finish(&state, digest);

		MakeMD5String(result, digest);
	}

	void GetMD5(unsigned char* pResult, const unsigned char* pData, int dataSize)
	{
		md5_state_t state;

		md5_init(&state);
		md5_append(&state, (const md5_byte_t *)pData, dataSize);
		md5_finish(&state, pResult);
	}

	void MakeMD5String(std::string& result, unsigned char* pDigest)
	{
		result.clear();
		result.resize(MD5_DIGEST_LENGTH * 2 + 1);

		int di = 0;
		for (; di < MD5_DIGEST_LENGTH; ++di)
			sprintf(&result[di * 2], "%02x", pDigest[di]);
	}

	DWORD BlendColor(float fElapsedTime, DWORD srcColor, DWORD destColor, float fRate)
	{
		EtColor CurrentColor;
		EtColorLerp(&CurrentColor, &EtColor(srcColor), &EtColor(destColor), 1.0f - powf(fRate, 30 * fElapsedTime));
		return CurrentColor;
	}

	int ConvertUnionTableIDToUnionType(int unionTableId)
	{
		switch(unionTableId)
		{
		case 1: return NpcReputation::UnionType::Commercial;
		case 2: return NpcReputation::UnionType::Royal;
		case 3: return NpcReputation::UnionType::Liberty;
		default: return NpcReputation::UnionType::Etc;
		}
	}

#if defined _CLIENT
	bool GetHtmlFromURL(const std::string& URL, WCHAR** ppHTMLDataBuffer)
	{
		if (URL.empty())
			return false;

		const std::string tempHTMLName("HTMLTemp.htm");
		DeleteUrlCacheEntryA(URL.c_str());

		HRESULT hr = ::URLDownloadToFileA(NULL, URL.c_str(), tempHTMLName.c_str(), 0, NULL);
		if (hr != S_OK)
			return false;

		struct stat buffer;
		int ret = stat(tempHTMLName.c_str(), &buffer);
		if (ret != 0)
			return false;
		DWORD fileLength = buffer.st_size;

		FILE* pHTMLFile;
		if (fopen_s(&pHTMLFile, tempHTMLName.c_str(), "r") != 0)
			return false;

		char* pRawHTML = new char[fileLength + 1];
		ZeroMemory(pRawHTML, fileLength + 1);

		fread(pRawHTML, 1, fileLength, pHTMLFile);
		pRawHTML[fileLength] = '\0';
		fclose(pHTMLFile);

		int wideCharSize = MultiByteToWideChar(CP_ACP, 0, pRawHTML, -1, NULL, NULL);
		*ppHTMLDataBuffer = new WCHAR[wideCharSize];
		MultiByteToWideChar(CP_ACP, 0, pRawHTML, int(strlen(pRawHTML) + 1), *ppHTMLDataBuffer, wideCharSize);
		delete[] pRawHTML;

		DeleteFileA(tempHTMLName.c_str());

		return true;
	}

#ifdef PRE_FIX_61545
	WCHAR GetPartySearchWordSeperator()
	{
	#ifdef _JP
		return 0x3000;
	#else
		return 0x0020;
	#endif
	}
#endif

#ifdef PRE_MOD_GACHA_SYSTEM
	void GetItemStatString_Basic(std::wstring& strItemStat, CDnState* pItemState)
	{
		WCHAR awcBuffer[ 256 ] = { 0 };

		// 기본 스택 정보(랜덤 최고치의 능력치)
		if( pItemState->GetStrength() > 0 )
		{
			swprintf_s( awcBuffer, 256, L"%s : %d ", DN_INTERFACE::STRING::ITEM::GET_STATIC( emTOOLTIP_STATIC::ITEM_STRENGTH ), pItemState->GetStrength() );
			strItemStat.append( awcBuffer );
		}
		if( pItemState->GetAgility() > 0 )
		{
			swprintf_s( awcBuffer, 256, L"%s : %d ", DN_INTERFACE::STRING::ITEM::GET_STATIC( emTOOLTIP_STATIC::ITEM_AGILITY ), pItemState->GetAgility() );
			strItemStat.append( awcBuffer );
		}
		if( pItemState->GetIntelligence() > 0 )
		{
			swprintf_s( awcBuffer, 256, L"%s : %d ", DN_INTERFACE::STRING::ITEM::GET_STATIC( emTOOLTIP_STATIC::ITEM_INTELLIGENCE ), pItemState->GetIntelligence() );
			strItemStat.append( awcBuffer );
		}
		if( pItemState->GetStamina() > 0 )
		{
			swprintf_s( awcBuffer, 256, L"%s : %d ", DN_INTERFACE::STRING::ITEM::GET_STATIC( emTOOLTIP_STATIC::ITEM_STAMINA ), pItemState->GetStamina() );
			strItemStat.append( awcBuffer );
		}
	}

	void GetItemStatString_ExceptBasic(std::wstring& strItemStat, CDnState* pItemState)
	{
		WCHAR awcBuffer[ 256 ] = { 0 };

		if( pItemState->GetAttackPMin() > 0 || pItemState->GetAttackPMax() > 0 )
		{
			swprintf_s( awcBuffer, 256, L"%s : %d~%d ", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 5018 ), pItemState->GetAttackPMin(), pItemState->GetAttackPMax() );
			strItemStat.append( awcBuffer );
		}
		if( pItemState->GetAttackMMin() > 0 || pItemState->GetAttackMMax() > 0 )
		{
			swprintf_s( awcBuffer, 256, L"%s : %d~%d ", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 5019 ), pItemState->GetAttackMMin(), pItemState->GetAttackMMax() );
			strItemStat.append( awcBuffer );
		}

		if( pItemState->GetDefenseP() > 0 )
		{
			swprintf_s( awcBuffer, 256, L"%s : %d ", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 5020 ), pItemState->GetDefenseP() );
			strItemStat.append( awcBuffer );
		}
		if( pItemState->GetDefenseM() > 0 )
		{
			swprintf_s( awcBuffer, 256, L"%s : %d ", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 5062 ), pItemState->GetDefenseM() );
			strItemStat.append( awcBuffer );
		}

		if( pItemState->GetSpirit() > 0 )
		{
			swprintf_s( awcBuffer, 256, L"%s : %d ", DN_INTERFACE::STRING::ITEM::GET_STATIC( emTOOLTIP_STATIC::ITEM_SPIRIT ), pItemState->GetSpirit() );
			strItemStat.append( awcBuffer );
		}

		int nTemp(0);
		INT64 nTempHP = 0;

		// 내부 스탯 증가
		nTemp = pItemState->GetStiff();
		if( nTemp > 0 )
		{
			swprintf_s( awcBuffer, 256, L"%s : %d ", DN_INTERFACE::STRING::ITEM::GET_STATIC( emTOOLTIP_STATIC::ITEM_STIFFNESS_P ), nTemp );
			strItemStat.append( awcBuffer );
		}

		nTemp = pItemState->GetStiffResistance();
		if( nTemp > 0 )
		{
			swprintf_s( awcBuffer, 256, L"%s : %d ", DN_INTERFACE::STRING::ITEM::GET_STATIC( emTOOLTIP_STATIC::ITEM_STIFFNESS_R ), nTemp );
			strItemStat.append( awcBuffer );
		}

		nTemp = pItemState->GetStun();
		if( nTemp > 0 )
		{
			swprintf_s( awcBuffer, 256, L"%s : %d ", DN_INTERFACE::STRING::ITEM::GET_STATIC( emTOOLTIP_STATIC::ITEM_STUN_P ), nTemp );
			strItemStat.append( awcBuffer );
		}

		nTemp = pItemState->GetStunResistance();
		if( nTemp > 0 )
		{
			swprintf_s( awcBuffer, 256, L"%s : %d ", DN_INTERFACE::STRING::ITEM::GET_STATIC( emTOOLTIP_STATIC::ITEM_STUN_R ), nTemp );
			strItemStat.append( awcBuffer );
		}

		nTemp = pItemState->GetCritical();
		if( nTemp > 0 )
		{
			swprintf_s( awcBuffer, 256, L"%s : %d ", DN_INTERFACE::STRING::ITEM::GET_STATIC( emTOOLTIP_STATIC::ITEM_CRITICAL_P ), nTemp );
			strItemStat.append( awcBuffer );
		}

		nTemp = pItemState->GetCriticalResistance();
		if( nTemp > 0 )
		{
			swprintf_s( awcBuffer, 256, L"%s : %d ", DN_INTERFACE::STRING::ITEM::GET_STATIC( emTOOLTIP_STATIC::ITEM_CRITICAL_R ), nTemp );
			strItemStat.append( awcBuffer );
		}

		nTemp = pItemState->GetMoveSpeed();
		if( nTemp > 0 )
		{
			swprintf_s( awcBuffer, 256, L"%s : +%d ", DN_INTERFACE::STRING::ITEM::GET_STATIC( emTOOLTIP_STATIC::ITEM_MOVE ), nTemp );
			strItemStat.append( awcBuffer );
		}

		nTempHP = pItemState->GetMaxHP();
		if( nTempHP > 0 )
		{
			swprintf_s( awcBuffer, 256, L"%s +%I64d ", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 2254 ), nTempHP );
			strItemStat.append( awcBuffer );
		}

		nTemp = pItemState->GetMaxSP();
		if( nTemp > 0 )
		{
			swprintf_s( awcBuffer, 256, L"%s +%d ", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 2255 ), nTemp );
			strItemStat.append( awcBuffer );
		}

		nTemp = pItemState->GetRecoverySP();
		if( nTemp > 0 )
		{
			swprintf_s( awcBuffer, 256, L"%s : %d ", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 2256 ), nTemp );
			strItemStat.append( awcBuffer );
		}

		// 슈퍼아머, 파이널데미지
		nTemp = pItemState->GetSuperAmmor();
		if( nTemp > 0 )
		{
			swprintf_s( awcBuffer, 256, L"%s : %d ", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 2279 ), nTemp );
			strItemStat.append( awcBuffer );
		}

		nTemp = pItemState->GetFinalDamage();
		if( nTemp > 0 )
		{
			swprintf_s( awcBuffer, 256, L"%s : %d ", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 2280 ), nTemp );
			strItemStat.append( awcBuffer );
		}

		// 아이템 능력치 표시 두번째. Ratio관련.
		// 공격력/방어력
		if( pItemState->GetAttackPMinRatio() > 0 || pItemState->GetAttackPMaxRatio() > 0 )
		{
			swprintf_s( awcBuffer, 256, L"%s : %.1f%%~%.1f%% ", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 5018 ), pItemState->GetAttackPMinRatio() * 100.0f, pItemState->GetAttackPMaxRatio() * 100.0f );
			strItemStat.append( awcBuffer );
		}
		if( pItemState->GetAttackMMinRatio() > 0 || pItemState->GetAttackMMaxRatio() > 0 )
		{
			swprintf_s( awcBuffer, 256, L"%s : %.1f%%~%.1f%% ", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 5019 ), pItemState->GetAttackMMinRatio() * 100.0f, pItemState->GetAttackMMaxRatio() * 100.0f );
			strItemStat.append( awcBuffer );
		}

		if( pItemState->GetDefensePRatio() > 0 )
		{
			swprintf_s( awcBuffer, 256, L"%s : %.1f%% ", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 5020 ), pItemState->GetDefensePRatio() * 100.0f );
			strItemStat.append( awcBuffer );
		}
		if( pItemState->GetDefenseMRatio() > 0 )
		{
			swprintf_s( awcBuffer, 256, L"%s : %.1f%% ", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 5062 ), pItemState->GetDefenseMRatio() * 100.0f );
			strItemStat.append( awcBuffer );
		}

		// 기본 스택 정보(랜덤 최고치의 능력치)
		if( pItemState->GetStrengthRatio() > 0 )
		{
			swprintf_s( awcBuffer, 256, L"%s : %.2f%% ", DN_INTERFACE::STRING::ITEM::GET_STATIC( emTOOLTIP_STATIC::ITEM_STRENGTH ), pItemState->GetStrengthRatio() * 100.0f );
			strItemStat.append( awcBuffer );
		}
		if( pItemState->GetAgilityRatio() > 0 )
		{
			swprintf_s( awcBuffer, 256, L"%s : %.2f%% ", DN_INTERFACE::STRING::ITEM::GET_STATIC( emTOOLTIP_STATIC::ITEM_AGILITY ), pItemState->GetAgilityRatio() * 100.0f );
			strItemStat.append( awcBuffer );
		}
		if( pItemState->GetIntelligenceRatio() > 0 )
		{
			swprintf_s( awcBuffer, 256, L"%s : %.2f%% ", DN_INTERFACE::STRING::ITEM::GET_STATIC( emTOOLTIP_STATIC::ITEM_INTELLIGENCE ), pItemState->GetIntelligenceRatio() * 100.0f );
			strItemStat.append( awcBuffer );
		}
		if( pItemState->GetStaminaRatio() > 0 )
		{
			swprintf_s( awcBuffer, 256, L"%s : %.2f%% ", DN_INTERFACE::STRING::ITEM::GET_STATIC( emTOOLTIP_STATIC::ITEM_STAMINA ), pItemState->GetStaminaRatio() * 100.0f );
			strItemStat.append( awcBuffer );
		}
		if( pItemState->GetSpiritRatio() > 0 )
		{
			swprintf_s( awcBuffer, 256, L"%s : %.2f%% ", DN_INTERFACE::STRING::ITEM::GET_STATIC( emTOOLTIP_STATIC::ITEM_SPIRIT ), pItemState->GetSpiritRatio() * 100.0f );
			strItemStat.append( awcBuffer );
		}

		float fTemp(0);

		// 내부 스탯 증가
		fTemp = pItemState->GetStiffRatio();
		if( fTemp > 0 )
		{
			swprintf_s( awcBuffer, 256, L"%s : %.2f%% ", DN_INTERFACE::STRING::ITEM::GET_STATIC( emTOOLTIP_STATIC::ITEM_STIFFNESS_P ), fTemp * 100.0f );
			strItemStat.append( awcBuffer );
		}

		fTemp = pItemState->GetStiffResistanceRatio();
		if( fTemp > 0 )
		{
			swprintf_s( awcBuffer, 256, L"%s : %.2f%% ", DN_INTERFACE::STRING::ITEM::GET_STATIC( emTOOLTIP_STATIC::ITEM_STIFFNESS_R ), fTemp * 100.0f );
			strItemStat.append( awcBuffer );
		}

		fTemp = pItemState->GetStunRatio();
		if( fTemp > 0 )
		{
			swprintf_s( awcBuffer, 256, L"%s : %.2f%% ", DN_INTERFACE::STRING::ITEM::GET_STATIC( emTOOLTIP_STATIC::ITEM_STUN_P ), fTemp * 100.0f );
			strItemStat.append( awcBuffer );
		}

		fTemp = pItemState->GetStunResistanceRatio();
		if( fTemp > 0 )
		{
			swprintf_s( awcBuffer, 256, L"%s : %.2f%% ", DN_INTERFACE::STRING::ITEM::GET_STATIC( emTOOLTIP_STATIC::ITEM_STUN_R ), fTemp * 100.0f );
			strItemStat.append( awcBuffer );
		}

		fTemp = pItemState->GetCriticalRatio();
		if( fTemp > 0 )
		{
			swprintf_s( awcBuffer, 256, L"%s : %.2f%% ", DN_INTERFACE::STRING::ITEM::GET_STATIC( emTOOLTIP_STATIC::ITEM_CRITICAL_P ), fTemp * 100.0f );
			strItemStat.append( awcBuffer );
		}

		fTemp = pItemState->GetCriticalResistanceRatio();
		if( fTemp > 0 )
		{
			swprintf_s( awcBuffer, 256, L"%s : %.2f%% ", DN_INTERFACE::STRING::ITEM::GET_STATIC( emTOOLTIP_STATIC::ITEM_CRITICAL_R ), fTemp * 100.0f );
			strItemStat.append( awcBuffer );
		}


		fTemp = pItemState->GetMoveSpeedRatio();
		if( fTemp > 0 )
		{
			swprintf_s( awcBuffer, 256, L"%s : +%.2f%% ", DN_INTERFACE::STRING::ITEM::GET_STATIC( emTOOLTIP_STATIC::ITEM_MOVE ), fTemp * 100.0f );
			strItemStat.append( awcBuffer );
		}

		fTemp = pItemState->GetMaxHPRatio();
		if( fTemp > 0 )
		{
			swprintf_s( awcBuffer, 256, L"%s +%.2f%% ", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 2254 ), fTemp * 100.0f );
			strItemStat.append( awcBuffer );
		}

		fTemp = pItemState->GetMaxSPRatio();
		if( fTemp > 0 )
		{
			swprintf_s( awcBuffer, 256, L"%s +%.2f%% ", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 2255 ), fTemp * 100.0f );
			strItemStat.append( awcBuffer );
		}

		fTemp = pItemState->GetRecoverySPRatio();
		if( fTemp > 0 )
		{
			swprintf_s( awcBuffer, 256, L"%s +%.2f%% ", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 2256 ), fTemp * 100.0f );
			strItemStat.append( awcBuffer );
		}

		// 슈퍼아머, 파이널데미지
		fTemp = pItemState->GetSuperAmmorRatio();
		if( fTemp > 0 )
		{
			swprintf_s( awcBuffer, 256, L"%s : %.2f%% ", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 2279 ), fTemp * 100.0f );
			strItemStat.append( awcBuffer );
		}

		fTemp = pItemState->GetFinalDamageRatio();
		if( fTemp > 0 )
		{
			swprintf_s( awcBuffer, 256, L"%s : %.2f%% ", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 2280 ), fTemp * 100.0f );
			strItemStat.append( awcBuffer );
		}



		// 속성 공격력/저항 만 별도의 Ratio가 없으므로 아래 출력한다.
		//
		fTemp = ( pItemState->GetElementAttack( CDnState::Light ) * 100.f );
		if( fTemp > 0 )
		{
			swprintf_s( awcBuffer, 256, L"%s : +%.2f%% ", DN_INTERFACE::STRING::ITEM::GET_STATIC( emTOOLTIP_STATIC::ITEM_LIGHT_A ), fTemp );
			strItemStat.append( awcBuffer );
		}

		fTemp = ( pItemState->GetElementAttack( CDnState::Dark ) * 100.f );
		if( fTemp > 0 )
		{
			swprintf_s( awcBuffer, 256, L"%s : +%.2f%% ", DN_INTERFACE::STRING::ITEM::GET_STATIC( emTOOLTIP_STATIC::ITEM_DARK_A ), fTemp );
			strItemStat.append( awcBuffer );
		}

		fTemp = ( pItemState->GetElementAttack( CDnState::Fire ) * 100.f );
		if( fTemp > 0 )
		{
			swprintf_s( awcBuffer, 256, L"%s : +%.2f%% ", DN_INTERFACE::STRING::ITEM::GET_STATIC( emTOOLTIP_STATIC::ITEM_FIRE_A ), fTemp );
			strItemStat.append( awcBuffer );
		}

		fTemp = ( pItemState->GetElementAttack( CDnState::Ice ) * 100.f );
		if( fTemp > 0 )
		{
			swprintf_s( awcBuffer, 256, L"%s : +%.2f%% ", DN_INTERFACE::STRING::ITEM::GET_STATIC( emTOOLTIP_STATIC::ITEM_ICE_A ), fTemp );
			strItemStat.append( awcBuffer );
		}


		fTemp = ( pItemState->GetElementDefense( CDnState::Light ) * 100.f );
		if( fTemp > 0 )
		{
			swprintf_s( awcBuffer, 256, L"%s : +%.2f%% ", DN_INTERFACE::STRING::ITEM::GET_STATIC( emTOOLTIP_STATIC::ITEM_LIGHT_R ), fTemp );
			strItemStat.append( awcBuffer );
		}

		fTemp = ( pItemState->GetElementDefense( CDnState::Dark ) * 100.f );
		if( fTemp > 0 )
		{
			swprintf_s( awcBuffer, 256, L"%s : +%.2f%% ", DN_INTERFACE::STRING::ITEM::GET_STATIC( emTOOLTIP_STATIC::ITEM_DARK_R ), fTemp );
			strItemStat.append( awcBuffer );
		}

		fTemp = ( pItemState->GetElementDefense( CDnState::Fire ) * 100.f );
		if( fTemp > 0 )
		{
			swprintf_s( awcBuffer, 256, L"%s : +%.2f%% ", DN_INTERFACE::STRING::ITEM::GET_STATIC( emTOOLTIP_STATIC::ITEM_FIRE_R ), fTemp );
			strItemStat.append( awcBuffer );
		}

		fTemp = ( pItemState->GetElementDefense( CDnState::Ice ) * 100.f );
		if( fTemp > 0 )
		{
			swprintf_s( awcBuffer, 256, L"%s : +%.2f%% ", DN_INTERFACE::STRING::ITEM::GET_STATIC( emTOOLTIP_STATIC::ITEM_ICE_R ), fTemp );
			strItemStat.append( awcBuffer );
		}
	}
#endif

#endif // _CLIENT

	int GetJobID(int classId, const std::vector<int>& jobArray)
	{
		int nJob = classId;
		if (!jobArray.empty())
			nJob = jobArray[jobArray.size() - 1];

		return nJob;
	}

	int GetJobID(int classId, const BYTE* pJobArray)
	{
		int nJob = classId;
		if (pJobArray)
		{
			int i = 0, lastJob = nJob;
			for (; i < JOBMAX; ++i)
			{
				if (pJobArray[i] != 0)
					lastJob = pJobArray[i];
				else
					break;
			}

			return lastJob;
		}

		return nJob;
	}

	void CalcJobKindMax()
	{
		g_JobKindMax = 0;
		DNTableFileFormat *pSox = GetDNTable( CDnTableDB::TJOB );
		if ( !pSox ) return;

		for( int i=0; i<pSox->GetItemCount(); i++ ) 
		{
			int nItemID = pSox->GetItemID(i);
			if (g_JobKindMax < nItemID)
				g_JobKindMax = nItemID;
		}
	}

	int GetCipherDecimal(int number)
	{
		int count = 0;
		while (number > 0)
		{
			count++;
			number /= 10;
		}

		return count;
	}

	double logWithBase(double x, double base)
	{
		return (std::log(x) / std::log(base));
	}

#endif // _CLIENT || _SERVER

	void GetItemJobUsable(std::vector<int>& usableClassIds, ITEMCLSID itemId, DNTableFileFormat* pItemTable)
	{
#if defined (_CLIENT) || defined (_SERVER)
		if (pItemTable == NULL)
			pItemTable = GetDNTable( CDnTableDB::TITEM );
#endif

		if( !pItemTable || !pItemTable->IsExistItem( itemId ) )
			return;

		char *szPermitStr = pItemTable->GetFieldFromLablePtr( itemId, "_NeedJobClass" )->GetString();
		int nJobIndex;
		for( int i=0;; i++ ) {
			std::string strValue = _GetSubStrByCountSafe( i, szPermitStr, ';' );
			if( strValue.size() == 0 ) break;
			nJobIndex = atoi(strValue.c_str());
			if (nJobIndex == 0)
			{
				usableClassIds.push_back(nJobIndex);
				return;
			}
			else if (nJobIndex < 0)
			{
				continue;
			}

			usableClassIds.push_back(nJobIndex);
		}
	}

	char* GetFileNameFromFileEXT(DNTableFileFormat* pRawSox, int itemId, const char* pFieldName, DNTableFileFormat* pFileNameSox)
	{
		if (pRawSox == NULL || pFieldName == NULL || pFieldName[0] == '\0')
			return NULL;

		DNTableCell* pNameField = pRawSox->GetFieldFromLablePtr( itemId, pFieldName );
		return ((pNameField != NULL) && (pNameField->GetInteger() != 0)) ? GetFileNameFromFileEXT(pNameField->GetInteger(), pFileNameSox) : NULL;
	}

	void GetFileNameFromFileEXT(std::string& result, DNTableFileFormat* pRawSox, int itemId, const char* pFieldName, DNTableFileFormat* pFileNameSox)
	{
		if (pRawSox == NULL || pFieldName == NULL || pFieldName[0] == '\0')
			return;

		DNTableCell* pNameField = pRawSox->GetFieldFromLablePtr( itemId, pFieldName );
		if (pNameField != NULL)
		{
			char* pFileName = GetFileNameFromFileEXT(pNameField->GetInteger(), pFileNameSox);
			result = (pFileName) ? pFileName : "";
		}
		else
		{
			result = "";
		}
	}

	char* GetFileNameFromFileEXT(int index, DNTableFileFormat* pFileSox)
	{
		if (pFileSox == NULL)
		{
#ifdef _ACTIONTOOL
			pFileSox = CDnGameTable::GetInstance().GetTable( CDnGameTable::FILE_TABLE );
#else
	
			// 컷신툴에서는 외부에서 직접 pFileSox 넣어줍니다. TableDB 는 사용하지 않아서 컴파일 에러나서 제외시킵니다.
#if !defined (_CUTSCENETOOL) && !defined (_TABLEVERIFIER)
			pFileSox = GetDNTable( CDnTableDB::TFILE );
#endif

#endif 
			if (pFileSox == NULL)
			{
				_ASSERT(0);
				return NULL;
			}
		}

		const DNTableCell* pField = pFileSox->GetFieldFromLablePtr(index, "_FileName");
		if (pField)
			return pField->GetString();

		return NULL;
	}

	bool IsIntersect(int sourcePointStart, int sourcePointEnd, int targetPointStart, int targetPointEnd)
	{
		if ((sourcePointStart < targetPointStart && sourcePointEnd < targetPointStart) ||
			(sourcePointStart > targetPointEnd && sourcePointEnd > targetPointEnd))
			return false;

		return true;
	}


#if defined (_CLIENT) || defined (_SERVER)
	// job의 상위 직업명 반환.
	int GetParentJob( int job )
	{
		DNTableFileFormat * pTable = GetDNTable( CDnTableDB::TJOB );
		if( !pTable )
			return 0;

		DNTableCell * pCell = NULL;

		int nParentJob = 0;
		int nSize = pTable->GetItemCount();
		for( int i=0; i<nSize; ++i )
		{
			int nID = pTable->GetItemID( i );
			if( nID != job )
				continue;

			pCell = pTable->GetFieldFromLablePtr( nID, "_ParentJob" );
			if( pCell && pCell->GetInteger() > 0 )
			{
				nParentJob = pCell->GetInteger();
				break;
			}		
		}

		return nParentJob;	
	}

	// 직업의 직종 구하기( 디스트로이어 => 워리어,  셀레아나 => 소서리스 )
	int GetClassByJob( int job )
	{
		DNTableFileFormat * pTable = GetDNTable( CDnTableDB::TJOB );
		if( !pTable )
			return 0;

		if( !pTable->IsExistItem( job ) )
			return 0;

		DNTableCell * pCell = pTable->GetFieldFromLablePtr( job, "_Class" );
		if( !pCell )
			return 0;

		return pCell->GetInteger();
	}

#endif//

#if defined( PRE_FIX_CLIENT_FREEZING )
	void PrintFreezingLog( char * szLog )
	{
		char temp_file_name[256] = {0,};
		char temp_date_buf[256] = {0,};
		struct	tm *tm_ptr;
		time_t	raw;
		time(&raw);
		tm_ptr = localtime(&raw);

		strftime(temp_date_buf, _countof(temp_date_buf), "%Y-%m-%d", tm_ptr);
		sprintf_s( temp_file_name, _countof(temp_file_name), "Log\\HLog_%s.txt", temp_date_buf );

		FILE * pFile = NULL;
		fopen_s( &pFile, temp_file_name, "a" );
		if( NULL == pFile )
			return;

		char szOutputString[1024] = {0,};
		strftime(temp_date_buf, _countof(temp_date_buf), "%H-%M-%S", tm_ptr);

		sprintf_s( szOutputString, _countof(szOutputString), "%s : %s\n", temp_date_buf, szLog );

		fputs( szOutputString , pFile );

		fflush( pFile );
		fclose( pFile );
	}
#endif	// #if defined( PRE_FIX_CLIENT_FREEZING )
}

#if defined _CLIENT || defined _SERVER
#ifdef PRE_FIX_MEMOPT_SIGNALH
void CopyShallow_ProjectileStruct(ProjectileStruct& dest, const ProjectileStruct* pSource)
{
	memset(&dest, 0, sizeof(ProjectileStruct));
#ifdef WIN64
	memcpy_s(&dest, sizeof(ProjectileStruct), pSource, GetSignalDataUsingCount64(STE_Projectile) * sizeof(int));
#else
	memcpy_s(&dest, sizeof(ProjectileStruct), pSource, GetSignalDataUsingCount(STE_Projectile) * sizeof(int));
#endif
}

void CopyShallow_HitStruct(HitStruct& dest, const HitStruct* pSource)
{
	memset(&dest, 0, sizeof(HitStruct));
#ifdef WIN64
	memcpy_s(&dest, sizeof(HitStruct), pSource, GetSignalDataUsingCount64(STE_Hit) * sizeof(int));
#else
	memcpy_s(&dest, sizeof(HitStruct), pSource, GetSignalDataUsingCount(STE_Hit) * sizeof(int));
#endif
}

void CopyShallow_SummonMonsterStruct(SummonMonsterStruct& dest, const SummonMonsterStruct* pSource)
{
	memset(&dest, 0, sizeof(SummonMonsterStruct));
#ifdef WIN64
	memcpy_s(&dest, sizeof(SummonMonsterStruct), pSource, GetSignalDataUsingCount64(STE_SummonMonster) * sizeof(int));
#else
	memcpy_s(&dest, sizeof(SummonMonsterStruct), pSource, GetSignalDataUsingCount(STE_SummonMonster) * sizeof(int));
#endif
}

void CopyShallow_HeadLook(HeadLookStruct& dest, const HeadLookStruct* pSource)
{
	memset(&dest, 0, sizeof(HeadLookStruct));
#ifdef WIN64
	memcpy_s(&dest, sizeof(HeadLookStruct), pSource, GetSignalDataUsingCount64(STE_HeadLook) * sizeof(int));
#else
	memcpy_s(&dest, sizeof(HeadLookStruct), pSource, GetSignalDataUsingCount(STE_HeadLook) * sizeof(int));
#endif
}
#endif // PRE_FIX_MEMOPT_SIGNALH
#endif // defined _CLIENT || defined _SERVER