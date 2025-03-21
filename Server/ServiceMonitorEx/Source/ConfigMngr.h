

#pragma once

#include "Define.h"
#include "CriticalSection.hpp"
#include "Singleton.hpp"
#include "IniFile.h"
#include "IpV4.hpp"

#include <map>
#include <vector>


// CConfigMngr

class CConfigMngr
{

public:
	typedef	CCriticalSection					TP_LOCK;
	typedef	CLockAutoEx<TP_LOCK>				TP_LOCKAUTO;

private:
	DECLARE_SINGLETON_CLASS(CConfigMngr);

public:
	CConfigMngr();

	VOID Reset();

	DWORD Open(LPCTSTR pFileName);
	VOID Close();
	BOOL IsOpen() const { return(m_IniFile.IsOpen()); }

	VOID Flush();

public:
	CIpV4 m_IpAddress;	// IP 주소
	WORD m_PortNo;		// 포트 번호
	CString m_ID;		// ID
	WINDOWPLACEMENT m_WindowPlacement;
	CString m_WavFile;

private:
	CIniFile m_IniFile;
	
};

