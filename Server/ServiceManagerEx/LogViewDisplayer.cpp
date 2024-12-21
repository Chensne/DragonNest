#include "StdAfx.h"
#include "LogViewDisplayer.h"
#include "LogSplit.h"
#include "LogBuilder.h"

CLogViewDisplayer::CLogViewDisplayer(CLogSplit* pLogView)
	: m_pLogView(pLogView)
{
}

CLogViewDisplayer::~CLogViewDisplayer()
{
}

void CLogViewDisplayer::Display(LogType::eLogType type, const wchar_t* buf)
{
	if (!m_pLogView)
		return;

	tm *tm_ptr;
	time_t raw;		
	
	time(&raw);
	tm_ptr = localtime(&raw);

	wchar_t date[256];
	wcsftime(date, 256, L"%Y-%m-%d %H:%M:%S", tm_ptr);

	wstring typeStr = CLogBuilder::TypeToString(type);
	m_pLogView->InsertLog(LogInfo(date, typeStr, buf));
}