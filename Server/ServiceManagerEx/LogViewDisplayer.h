#pragma once

#include "Log.h"

class CLogSplit;

class CLogViewDisplayer : public CDisplayer
{
public:
	CLogViewDisplayer(CLogSplit* pLogView);
	virtual ~CLogViewDisplayer();

public:
	virtual void Display(LogType::eLogType type, const wchar_t* buf) override;

private:
	CLogSplit* m_pLogView;
};
