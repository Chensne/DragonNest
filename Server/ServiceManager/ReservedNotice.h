
#pragma once

#include "XMLParser.h"
#include "CriticalSection.h"
#include <vector>

struct TReservedNoticeInfo
{
	int nNoticeID;
	time_t _tBeginTime;
	time_t _tEndTime;
	int nPeriodSec;
	int nDurationSec;

	int nNoticeType;
	int nMasterMID;	//if 0 is whole world
	int nServerMID;
	int nMapIdx;
	int nChannelID;
	std::string szNoticeMsg;

	ULONG nLastNoticeTime;
};

class CReservedNotice
{
public:
	CReservedNotice();
	~CReservedNotice();

	void IdleProcess();

	bool LoadNotice(const char * pPath);
	
protected:
	CXMLParser m_XmlParser;
	std::vector <TReservedNoticeInfo*> m_NoticeList;
	CSyncLock m_Sync;

	void ClearNotice();
	int GetNoticeType(const WCHAR * pType);
};