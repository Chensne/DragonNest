

#include "StdAfx.h"
#include "ErrorLogMngr.h"


// ERRLOGINFO

// CErrorLogMngr

DEFINE_SINGLETON_CLASS(CErrorLogMngr);

CErrorLogMngr::CErrorLogMngr() : m_IsOpen(FALSE)
{
	
}

DWORD CErrorLogMngr::Open()
{	
	m_IsOpen = TRUE;

	return NOERROR;
}

VOID CErrorLogMngr::Close()
{
	
	m_IsOpen = FALSE;
}

