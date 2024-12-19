/*//===============================================================

	<< IP VERSION 4 >>

	PRGM : B4nFter

	FILE : IPV4.HPP
	DESC : IPV4 타입의 주소정보를 관리
	INIT BUILT DATE : 2008. 02. 26
	LAST BUILT DATE : 2008. 02. 26

	Copyrightⓒ 2002~2009 B4nFter (b4nfter@gmail.com) and Co-Writers. All Rights Reserved.

*///===============================================================


#pragma once


///////////////////////////////////////////////////////////////////
//	INCLUDE
///////////////////////////////////////////////////////////////////

#include "SYSBASEBASE.H"

#pragma comment(lib, "MSWSOCK.LIB")
#pragma comment(lib, "WS2_32.LIB")
#pragma comment(lib, "RPCRT4.LIB")	// UuidCreateSequential()


///////////////////////////////////////////////////////////////////
//	DEFINE
///////////////////////////////////////////////////////////////////

#define DF_IPADDR_SIZE		(16)	// IP주소 길이


///////////////////////////////////////////////////////////////////
//	DECLARATION
///////////////////////////////////////////////////////////////////

//*---------------------------------------------------------------
// TYPE : CLASS
// NAME : CNetStatistics
//*---------------------------------------------------------------
// DESC : Session Base Class
// PRGM : B4nFter
//*---------------------------------------------------------------
class CIpV4
{

public:
	CIpV4();

	VOID Set(LPCSTR pDot);
	VOID Set(UINT pDecimal);
	VOID Reset();
	BOOL IsSet() { return(INADDR_NONE != m_Decimal); }

	LPCSTR GetDot() const { return m_Dot; }
	UINT GetDecimal() const { return m_Decimal; }

private:
	CHAR m_Dot[DF_IPADDR_SIZE+1];
	UINT m_Decimal;

};


inline CIpV4::CIpV4()
{
	Reset();
}


inline VOID CIpV4::Set(LPCSTR pDot)
{
	STRNCPYA(m_Dot, pDot, COUNT_OF(m_Dot));
	m_Decimal = ::inet_addr(m_Dot);
}


inline VOID CIpV4::Set(UINT pDecimal)
{
	m_Decimal = pDecimal;
	IN_ADDR aInAddr;
	aInAddr.s_addr = m_Decimal;
	LPCSTR aDot = ::inet_ntoa(aInAddr);
	STRNCPYA(m_Dot, aDot, COUNT_OF(m_Dot));
}


inline VOID CIpV4::Reset()
{
	m_Dot[0] = '\0';
	m_Decimal = INADDR_NONE;
}

