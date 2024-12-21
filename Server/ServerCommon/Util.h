#pragma once

#include "Log.h"

class CIDGenerator
{
private:
	UINT m_nUserBaseID;
	UINT m_nNpcBaseID;

	UINT m_nUserConnectionID;
	UINT m_nUserConnectionMask;
	bool m_bUserConnectionSet;
	UINT m_nDBConnectionID;
	UINT m_nVillageConnectionID;
	UINT m_nGameConnectionID;
	UINT m_nMasterConnectionID;
	
	UINT m_nPartyID;
	USHORT m_nVoiceChannelID;

	UINT m_nUserID;
	UINT m_nNpcID;

	UINT m_nGameServerID;
	UINT m_nFieldID;

	CSyncLock m_IDLock;

public:
	CIDGenerator();
	~CIDGenerator();

	void SetUserConnectionID(UINT nID);
	void SetUserConnectionMask(BYTE btMask);
	UINT GetUserConnectionID(bool bDoGetRaw = false);
	bool IsUserConnectionSet() const { return m_bUserConnectionSet; }
	UINT GetDBConnectionID();
	UINT GetVillageConnectionID();
	UINT GetGameConnectionID();
	UINT GetMasterConnectionID();

	void SetPartyID(UINT nID);
	UINT GetPartyID();

	void SetVoiceChannelID(UINT nID);
	UINT GetVoiceChannelID();

	UINT GetUserID();
	UINT GetNpcID();

	UINT GetGameServerID();
	UINT GetFieldID();

	bool IsUser(UINT ID);
	bool IsNpc(UINT ID);
};

extern CIDGenerator g_IDGenerator;

// ������ ��ȣ ����
//---------------------------------------------------------------------
//| 2byte(ServerID) | 2byte(Adder) | 4byte(Create Time) |
//---------------------------------------------------------------------

inline INT64 MakeSerial( short nServerID )
{
	static long Adder = rand();
	
	time_t CreateTime;
	time(&CreateTime);

	long Local = static_cast<short>(InterlockedIncrement( &Adder ));

	INT64 Serial = (static_cast<INT64>(nServerID&0xFFFF)<<48) | (static_cast<INT64>(Local&0xFFFF)<<32) | (CreateTime&0xFFFFFFFF);
	
	return Serial;
}

inline INT64 MakeCashSerial( short nServerID, UINT nPeriodSecond)
{
	static long Adder = rand();
	
	time_t CreateTime;
	time(&CreateTime);
	CreateTime += nPeriodSecond;	// ĳ�������� �����Ⱓ(��)�� �����ش�

	long Local = static_cast<short>(InterlockedIncrement( &Adder ));

	INT64 Serial = (static_cast<INT64>(nServerID&0xFFFF)<<48) | (static_cast<INT64>(Local&0xFFFF)<<32) | (CreateTime&0xFFFFFFFF);

	return Serial;
}


// ������ NULL ���� ���ڿ��� ������ִ� �Լ��̴�.
// ������ Overrun �Ǵ� ������ �־� nDestLen �߰�.
// nLen >= nDestLen �̸� ��Ʈ���� �߸��� �����Ƿ� Ȯ�ιٶ�. // ���
inline void _wcscpy( WCHAR* pDest, const int nDestLen, const WCHAR* pSrc, const int nLen )
{	
	if(pDest == pSrc) return;
	memset( pDest, 0, nDestLen*sizeof(WCHAR) );
	if( nLen <= 0 )
		return;
	wmemcpy_s( pDest, nDestLen, pSrc, nDestLen < nLen ? nDestLen : nLen );
	if( nLen >= nDestLen )
	{
#if defined(_WORK)
		if( nLen > nDestLen )
			g_Log.Log( LogType::_ERROR, L"_wcscpy err %s Len:%d DestLen:%d\n", pSrc, nLen, nDestLen );
#endif //#if defined(_WORK)
		pDest[nDestLen-1] = '\0';
	}
	else
		pDest[nLen] = '\0';
}

inline void _strcpy( char* pDest, const int nDestLen, const char* pSrc, const int nLen )
{	
	if(pDest == pSrc) return;
	memset( pDest, 0, nDestLen );
	if( nLen <= 0 )
		return;
	memcpy_s( pDest, nDestLen, pSrc, nDestLen < nLen ? nDestLen : nLen );
	if( nLen >= nDestLen )
	{
		if( nLen > nDestLen )
			g_Log.Log( LogType::_ERROR, "_strcpy err %s\n", pSrc );
		pDest[nDestLen-1] = '\0';
	}
	else
		pDest[nLen] = '\0';
}

#include <map>

#if !defined( _SERVICEMANAGER )
template <class _Tx> struct less_str : std::binary_function <_Tx, _Tx, bool>
{
public:
	bool operator() (const _Tx &a, const _Tx &b) const { return __wcsicmp_l(a, b) < 0 ? true : false; }
};

template <class _Tx>
class map_wstr : public std::map <const WCHAR*, _Tx, less_str <const WCHAR*> >
{
};
#endif	// #if !defined( _SERVICEMANAGER )

#if !defined(_GAMESERVER)
inline unsigned long _inet_addr(const char * cp)
{
	int s_b[4] = { 0, 0, 0, 0};
	struct in_addr in;
	sscanf(cp, "%d.%d.%d.%d", &s_b[0], &s_b[1], &s_b[2], &s_b[3]);
	in.S_un.S_un_b.s_b1 = s_b[0];
	in.S_un.S_un_b.s_b2 = s_b[1];
	in.S_un.S_un_b.s_b3 = s_b[2];
	in.S_un.S_un_b.s_b4 = s_b[3];
	return in.S_un.S_addr;
}

inline void _inet_addr(unsigned long i, char * p)
{
	sprintf(p, "%d.%d.%d.%d", ((unsigned char*)&i)[0], ((unsigned char*)&i)[1], ((unsigned char*)&i)[2], ((unsigned char*)&i)[3]);
}

#endif	// #if !defined(_GAMESERVER)

//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CMisc::GetNextDay
//*---------------------------------------------------------------
// DESC : Ư�� ��¥�� ���� ����Ǵ� �ð� ������ ��¥�� ���
// PARM :	1 . stTime - [IN/OUT] ��ȯ�� �ð� �����͸� �Է��ϰ� ����� ���� SYSTEMTIME ����ü�� ����
//			2 . iAfterDay - ����Ǵ� ��¥ (- ���� �����ð� / + ���� ���Ľð�)
//			3 . iAfterHour - ����Ǵ� �ð� (- ���� �����ð� / + ���� ���Ľð�)
//			4 . iAfterMin - ����Ǵ� �� (- ���� �����ð� / + ���� ���Ľð�)
//			5 . iAfterSec - ����Ǵ� �� (- ���� �����ð� / + ���� ���Ľð�)
// RETV : N/A
// PRGM : B4nFter
//*---------------------------------------------------------------
inline VOID GetNextDay(SYSTEMTIME &stTime, INT iAfterDay, INT iAfterHour, INT iAfterMin, INT iAfterSec)
{
	INT64 i64;
	FILETIME stTempTime;

	stTime.wSecond = stTime.wMilliseconds = stTime.wDayOfWeek = 0;

	::SystemTimeToFileTime(&stTime,&stTempTime);
	i64=(((INT64)stTempTime.dwHighDateTime) << 32) + stTempTime.dwLowDateTime;
	i64 = i64 + (INT64)864000000000*(INT64)iAfterDay + (INT64)36000000000*(INT64)iAfterHour + (INT64)600000000*(INT64)iAfterMin + (INT64)10000000*(INT64)iAfterSec;
	stTempTime.dwHighDateTime = (DWORD)(i64 >> 32);
	stTempTime.dwLowDateTime = (DWORD)(i64 & 0xffffffff);
	::FileTimeToSystemTime(&stTempTime, &stTime);
}

//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CMisc::GetTickTerm
//*---------------------------------------------------------------
// DESC : �� Tick ���� ������ ����
// PARM :	1 . dwOldTick - ���� Tick
//			2 . dwCurTick - ���� Tick
// RETV : �� ƽī��Ʈ�� ���̰� (���)
// PRGM : B4nFter
// P.S.>
//		- �ܼ��� ��/�� ƽī��Ʈ�� ���̸� ���ϴ� ���̹Ƿ� �׻� �����
//		- ���� �� ������� dwOldTick, dwCurTick ���� ũ��񱳰� �ƴ϶� �� ���ڰ��� ���� ���̸� ��Ÿ���� ����
//		- ���� ��츦 ���Ͽ� dwOldTick ���� �� ����� ƽī��Ʈ�� ��� (::GetTickCount())
//*---------------------------------------------------------------
inline DWORD GetTickTerm(DWORD dwOldTick, DWORD dwCurTick)
{
	return((dwCurTick >= dwOldTick)?(dwCurTick - dwOldTick):(ULONG_MAX - dwOldTick + dwCurTick));
}

inline bool CheckLastSpace(const TCHAR * pwords)
{
	if (pwords == NULL) return true;

	int nLen = (int)_tcslen(pwords);
	if (nLen <= 0 || nLen == 1) return false;
	if (pwords[nLen - 1] == _T(' ') || pwords[0] == _T(' '))
		return true;
	return false;
}



std::wstring MakeSecondPasswordString( char *szVersion, const int nSeed, const int nValue[] );

#define MAKELONG64(a, b)      ((INT64)(((DWORD)((INT64)(a) & 0xffffffff)) | ((INT64)((DWORD)((INT64)(b) & 0xffffffff))) << 32))
#define LODWORD(l)           ((DWORD)((INT64)(l) & 0xffffffff))
#define HIDWORD(l)           ((DWORD)((INT64)(l) >> 32))

const int MD5_DIGEST_LEN = 16;

#include "md5.h"
inline void MakeMD5String(std::string& result, unsigned char* pDigest)
{
	result.clear();
	result.resize(MD5_DIGEST_LEN * 2 + 1);

	int di = 0;
	for (; di < MD5_DIGEST_LEN; ++di)
		sprintf(&result[di * 2], "%02x", pDigest[di]);
}

inline void MD5Hash(const char * pin, int nlen, std::string &out)
{
	md5_state_s md5struct;
	memset(&md5struct, 0, sizeof(md5_state_s));

	md5_init(&md5struct);
	md5_append(&md5struct, (const md5_byte_t*)pin, nlen);

	md5_byte_t digest[MD5_DIGEST_LEN];
	memset(digest, 0, sizeof(digest));
	md5_finish(&md5struct, digest);
 
	MakeMD5String(out, digest);
}

#include "SHA256.h"
inline void SHA256Hash(const char* pin, int nlen, unsigned char* pDigest)
{
	CSha256 c = {0,};
	//Byte digest[SHA256_DIGEST_SIZE] = {0,};

	Sha256_Init(&c);
	Sha256_Update(&c, (Byte*)pin, nlen);
	Sha256_Final(&c, pDigest);
}

#if defined( PRE_ADD_CHARACTERCHECKSUM )
UINT MakeCharacterCheckSum( INT64 biCharacterDBID, char cLevel, int nExp, INT64 biCoin, INT64 biWarehouseCoin );
#endif // #if defined( PRE_ADD_CHARACTERCHECKSUM )