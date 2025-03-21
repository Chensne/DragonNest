
#include "Stdafx.h"
#include "ReservedNotice.h"
#include "ServiceManager.h"
#include "Log.h"
#include "StringUtil.h"

CReservedNotice::CReservedNotice()
{
}

CReservedNotice::~CReservedNotice()
{
	ScopeLock<CSyncLock> Lock(m_Sync);
	ClearNotice();
}

void CReservedNotice::IdleProcess()
{
	ScopeLock<CSyncLock> Lock(m_Sync);
	
	time_t tt;
	time(&tt);
	localtime(&tt);
	ULONG nCurTime = timeGetTime();

	TReservedNoticeInfo * pInfo = NULL;
	std::vector <TReservedNoticeInfo*>::iterator ii;
	int nNoticeCount = 0;
	for (ii = m_NoticeList.begin(); ii != m_NoticeList.end(); )
	{
		pInfo = (*ii);
		if (pInfo->_tBeginTime < tt)
		{
			bool bIsPeriod = false;
			if (pInfo->nPeriodSec > 0 && pInfo->_tEndTime > tt)
				bIsPeriod = true;

			if (bIsPeriod && pInfo->nLastNoticeTime > 0 && pInfo->nLastNoticeTime + (pInfo->nPeriodSec * 1000) > nCurTime)
			{
				ii++;
				continue;
			}
			
			switch (pInfo->nNoticeType)
			{
			case _NOTICETYPE_ALL: g_pServiceManager->NoticeWholeWorld(pInfo->nDurationSec, pInfo->szNoticeMsg.c_str()); break;
			case _NOTICETYPE_WORLD: g_pServiceManager->NoticeWorld(pInfo->nMasterMID, pInfo->nDurationSec, pInfo->szNoticeMsg.c_str()); break;
			case _NOTICETYPE_ZONE: g_pServiceManager->NoticeZone(pInfo->nMasterMID, pInfo->nMapIdx, pInfo->nDurationSec, pInfo->szNoticeMsg.c_str()); break;
			case _NOTICETYPE_CHANNEL: g_pServiceManager->NoticeChannel(pInfo->nMasterMID, pInfo->nChannelID, pInfo->nDurationSec, pInfo->szNoticeMsg.c_str()); break;
			}

			g_Log.Log(LogType::_FILELOG, L"ReserveNotice [ID:%d][Msg:%S][Type:%d][Period:%d][Duration:%d]\n", pInfo->nNoticeID, pInfo->szNoticeMsg.c_str(), pInfo->nNoticeType, pInfo->nPeriodSec, pInfo->nDurationSec);
			
			if (bIsPeriod == false)
				ii = m_NoticeList.erase(ii);
			else
			{
				pInfo->nLastNoticeTime = nCurTime;
				ii++;
			}

			if (nNoticeCount >= 20)
			{
				g_Log.Log(LogType::_FILELOG, L"ReserveNotice break");
				return;
			}
			nNoticeCount++;
		}
		else
			ii++;
	}
}

bool CReservedNotice::LoadNotice(const char * pPath)
{
	ScopeLock<CSyncLock> Lock(m_Sync);
	if (m_XmlParser.Open(pPath) == false) return false;

	time_t tt;
	time(&tt);
	localtime(&tt);

	ClearNotice();
	if (m_XmlParser.FirstChildElement("ReservedNotice", true))
	{
		if (m_XmlParser.FirstChildElement("Notice"))
		{
			do 
			{
				TReservedNoticeInfo * pInfo = new TReservedNoticeInfo;
				memset(pInfo, 0, sizeof(TReservedNoticeInfo));

				pInfo->nNoticeID = _wtoi(m_XmlParser.GetAttribute("ID"));
				pInfo->nNoticeType = GetNoticeType(m_XmlParser.GetAttribute("Type"));
				pInfo->_tBeginTime = _wtoTime_t(m_XmlParser.GetAttribute("BeginTime"));

				pInfo->nPeriodSec = _wtoi(m_XmlParser.GetAttribute("PeriodSec"));
				if (pInfo->nPeriodSec > 0)
					pInfo->_tEndTime = _wtoTime_t(m_XmlParser.GetAttribute("EndTime"));

				pInfo->nMasterMID = _wtoi(m_XmlParser.GetAttribute("MasterMID"));
				pInfo->nServerMID = _wtoi(m_XmlParser.GetAttribute("ServerMID"));
				pInfo->nMapIdx = _wtoi(m_XmlParser.GetAttribute("MapIdx"));
				pInfo->nChannelID = _wtoi(m_XmlParser.GetAttribute("ChannelID"));
				pInfo->nDurationSec = _wtoi(m_XmlParser.GetAttribute("SlideSec"));
				std::wstring wszTemp = m_XmlParser.GetAttribute("Content");
				ToMultiString(wszTemp, pInfo->szNoticeMsg);

				if (pInfo->_tEndTime > 0)
				{
					if (pInfo->_tEndTime < tt)
					{
						g_Log.Log(LogType::_FILELOG, L"Notice Has Expired [ID:%d] [EndTime:%s]\n", pInfo->nNoticeID, m_XmlParser.GetAttribute("BeginTime"));
						SAFE_DELETE(pInfo);
						continue;
					}
				}
				else if (pInfo->_tBeginTime < tt)
				{
					g_Log.Log(LogType::_FILELOG, L"Notice Has Expired [ID:%d] [BeingTime:%s]\n", pInfo->nNoticeID, m_XmlParser.GetAttribute("BeginTime"));
					SAFE_DELETE(pInfo);
					continue;
				}
				
				m_NoticeList.push_back(pInfo);
			} while (m_XmlParser.NextSiblingElement("Notice"));
		}
		m_XmlParser.GoParent();
	}
	else return false;
	return true;
}

void CReservedNotice::ClearNotice()
{
	std::vector <TReservedNoticeInfo*>::iterator ii;
	for (ii = m_NoticeList.begin(); ii != m_NoticeList.end(); ii++)
		SAFE_DELETE((*ii));
	m_NoticeList.clear();
}

int CReservedNotice::GetNoticeType(const WCHAR * pType)
{
	if (!_wcsicmp(pType, L"Whole"))
		return _NOTICETYPE_ALL;
	else if (!_wcsicmp(pType, L"World"))
		return _NOTICETYPE_WORLD;
	else if (!_wcsicmp(pType, L"Server"))
		return _NOTICETYPE_EACHSERVER;
	else if (!_wcsicmp(pType, L"Zone"))
		return _NOTICETYPE_ZONE;
	else if (!_wcsicmp(pType, L"Channel"))
		return _NOTICETYPE_CHANNEL;
	return -1;
}