#pragma once

#if defined _CLIENT || defined _SERVER
#include "DNSecure.h"
#else
typedef int ITEMCLSID;
#endif

#define MD5_DIGEST_LENGTH 16

class CDnItem;
class CDnState;
#include "DNTableFile.h"

namespace CommonUtil
{
#if defined _CLIENT || defined _SERVER

#ifdef PRE_PARTY_DB
	enum eErrorPartySearchWord
	{
		ePARTYWORD_NONE,
		ePARTYWORD_NULL_STRING,
		ePARTYWORD_OVER_LENGTH,
		ePARTYWORD_OVER_SPACING,
	};
#endif

	bool IsValidCharacterNameLen(int len);
	bool IsValidPartyNameLen(int len);
#ifdef PRE_PARTY_DB
	eErrorPartySearchWord IsValidPartySearchWord(const WCHAR* pSearchWord);
#else
	bool IsValidPartyPwdLen(int len);
#endif
	bool IsValidDungeonDifficulty(TDUNGEONDIFFICULTY difficulty);
	bool IsValidFriendGroupNameLen(int len);
	bool IsValidCharacterClassId(int classId);
	bool IsValidCharacterJobId(int jobId);
#if defined _CLIENT || defined _GAMESERVER
	bool IsValidPartsType(int partsType, bool bCash);
#endif
	void GetMD5(std::string& result, const char* str);
	void GetDateStringForMD5(std::string& result, const tm& info);
	void GetMD5(unsigned char* pResult, const unsigned char* pData, int dataSize);
	void MakeMD5String(std::string& result, unsigned char* pDigest);
	DWORD BlendColor(float fElapsedTime, DWORD srcColor, DWORD destColor, float fRate);

	int ConvertUnionTableIDToUnionType(int unionTableId);
	void CalcJobKindMax();
	int GetCipherDecimal(int number);

	template<typename T>
	class CDnMoney
	{
	public:
		CDnMoney() : m_Money(0) {}
		CDnMoney(const T& init) : m_Money(init) {}
		void		SetMoney(const T& amount)	{ m_Money = amount; }
		T			GetMoney() const			{ return m_Money; }
		void		Clear()						{ m_Money = 0; }

		T	GetG() const
		{
			return m_Money / 10000;
		}
		T	GetS() const
		{
			return (m_Money % 10000) / 100;
		}
		T	GetB() const
		{
			return m_Money % 100;
		}

	private:
		T	m_Money;
	};
	typedef CDnMoney<MONEY_TYPE>	MONEY;
	typedef CDnMoney<int>			TAX;

	bool	IsCtrlChar(WCHAR ch);
	TAX_TYPE CalcTax(int taxType, int charLev, MONEY_TYPE attachAmount);
	TAX_TYPE GetFixedTax(int taxType);

	template<typename T>
	void	ClipNumber(T& target, const T& min, const T& max)
	{
		target = (target<min) ? min : ((target>max) ? max : target);
	}

	void GetCashRemainDate(const time_t& nRemain, WCHAR *pRemainDate, size_t SizeInWord);
	bool ConvertTimeFormatString(WCHAR *dateString, std::wstring& stringProcessed);
	double logWithBase(double x, double base);

	enum eDateStringType
	{
		DATESTR_FULL,
		DATESTR_COMPACT,
		DATESTR_REFUNDCASHINVEN,
		DATESTR_ACCEPT_LIMITTIME,
	};
#ifdef _CLIENT
	void GetDateString(eDateStringType type, std::wstring& result, const tm& info);
	void GetItemDescriptionInGainTable(std::string& result, const int& nItemID);

	bool IsInfiniteRebirthOnDungeon();
#ifdef PRE_ADD_EQUIPLOCK
	bool IsLockableEquipItem(eItemTypeEnum type);
#endif // PRE_ADD_EQUIPLOCK
#endif

#if defined( PRE_PARTY_DB )
	const WCHAR* GetDungeonDifficultyString( Dungeon::Difficulty::eCode Difficulty );
#endif // #if defined( PRE_PARTY_DB )

	enum ePartyClassIndex   //[OK]
	{
		eWARRIOR,
		eARCHER ,
		eSORCERESS,
		eCLERIC,
#ifdef PRE_ADD_ACADEMIC
		eACADEMIC,
#endif // #ifdef PRE_ADD_ACADEMIC

#ifdef PRE_ADD_KALI
		eKALI,
#endif // PRE_ADD_KALI

#ifdef PRE_ADD_ASSASSIN
		eASSASSIN,
#endif // PRE_ADD_ASSASSIN
#ifdef PRE_ADD_LENCEA
		eLENCEA,
#endif
#ifdef PRE_ADD_MACHINA
		eMACHINA,
#endif 

	};

#ifdef PRE_ADD_ACADEMIC
	//	note by kalliste : DNServerDef.h / ext 에 있는 클래스 인덱스와 동일한 값이 필요한 경우 사용하기 위한 인덱스 추가
	enum eClassIndex   //[OK]
	{
		eCLASS_NONE,
		eCLASS_WARRIOR,
		eCLASS_ARCHER ,
		eCLASS_SORCERESS,
		eCLASS_CLERIC,
		eCLASS_ACADEMIC,
#ifdef PRE_ADD_KALI
		eCLASS__KALI,
#endif // PRE_ADD_KALI

#ifdef PRE_ADD_ASSASSIN
		eCLASS_ASSASSIN,
#endif // PRE_ADD_ASSASSIN

#ifdef PRE_ADD_LENCEA
		eCLASS_LENCEA,
#endif
#ifdef PRE_ADD_MACHINA
		eCLASS_MACHINA,
#endif 
		eCLASS_Max
	};
#endif

#if defined _CLIENT
	bool GetHtmlFromURL(const std::string& URL, WCHAR** ppHTMLDataBuffer);
	#ifdef PRE_FIX_61545
	WCHAR GetPartySearchWordSeperator();
	#endif
	#ifdef PRE_MOD_GACHA_SYSTEM
	void GetItemStatString_Basic(std::wstring& strItemStat, CDnState* pItemState);
	void GetItemStatString_ExceptBasic(std::wstring& strItemStat, CDnState* pItemState);
	#endif
#endif
	int GetJobID(int classId, const std::vector<int>& jobArray);
	int GetJobID(int classId, const BYTE* pJobArray);

#endif // defined _CLIENT || defined _SERVER

	void GetItemJobUsable(std::vector<int>& usableClassIds, ITEMCLSID itemId, DNTableFileFormat* pItemTable = NULL);

	char*	GetFileNameFromFileEXT(DNTableFileFormat* pRawSox, int itemId, const char* pFieldName, DNTableFileFormat* pFileNameSox = NULL);
	void	GetFileNameFromFileEXT(std::string& result, DNTableFileFormat* pRawSox, int itemId, const char* pFieldName, DNTableFileFormat* pFileNameSox = NULL);
	char*	GetFileNameFromFileEXT(int index, DNTableFileFormat* pFileSox = NULL);
	bool	IsIntersect(int sourcePointStart, int sourcePointEnd, int targetPointStart, int targetPointEnd);


	// job의 상위 직업 반환.
	int GetParentJob( int job );

	// 직업의 직종 구하기( 디스트로이어 => 워리어,  셀레아나 => 소서리스 )
	int GetClassByJob( int job );

#if defined( PRE_FIX_CLIENT_FREEZING )
	void PrintFreezingLog( char * szLog );
#endif	// PRE_FIX_CLIENT_FREEZING
}

#if defined _CLIENT || defined _SERVER
#ifdef PRE_FIX_MEMOPT_SIGNALH
//	todo : server util.h와 통합
#include "SignalHeader.h"

void CopyShallow_ProjectileStruct(ProjectileStruct& dest, const ProjectileStruct* pSource);
void CopyShallow_HitStruct(HitStruct& dest, const HitStruct* pSource);
void CopyShallow_SummonMonsterStruct(SummonMonsterStruct& dest, const SummonMonsterStruct* pSource);
void CopyShallow_HeadLook(HeadLookStruct& dest, const HeadLookStruct* pSource);
#endif
#endif // defined _CLIENT || defined _SERVER 