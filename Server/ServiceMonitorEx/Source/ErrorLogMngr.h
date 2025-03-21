

#pragma once

#include "Define.h"
#include "Singleton.hpp"
#include "CriticalSection.hpp"

#include <map>
#include <vector>


// ERRLOGINFO

typedef struct ERRLOGINFO
{

public:

private:

public:
	

} *LPERRLOGINFO, * const LPCERRLOGINFO;;


// CErrorLogMngr

class CErrorLogMngr
{

public:
	typedef	CCriticalSection					TP_LOCK;
	typedef	CLockAutoEx<TP_LOCK>				TP_LOCKAUTO;

private:
	DECLARE_SINGLETON_CLASS(CErrorLogMngr);

public:
	CErrorLogMngr();

	DWORD Open();
	VOID Close();
	BOOL IsOpen() const { return(m_IsOpen); }

	

private:
	BOOL m_IsOpen;

	
};

