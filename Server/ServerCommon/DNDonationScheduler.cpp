#include "StdAfx.h"
#include "DNDonationScheduler.h"
#include "DNDBConnectionManager.h"
#include "DNDBConnection.h"
#include "TimeSet.h"

CDNDonationScheduler g_DonationScheduler;

const DWORD SCHEDULED_TICK = 10000;
const BYTE DONATION_BROADCAST_TIME = 7;

CDNDonationScheduler::CDNDonationScheduler()
: m_dwPrevTick(0), m_btNextHour(0)
{

}

CDNDonationScheduler::~CDNDonationScheduler()
{

}

void CDNDonationScheduler::AddWorldID(char cWorldID)
{
	m_WorldIDs.insert(cWorldID);
}

void CDNDonationScheduler::DoUpdate(DWORD dwCurTick)
{
	if (dwCurTick - m_dwPrevTick < SCHEDULED_TICK)
		return;

	m_dwPrevTick = dwCurTick;

	if (!PassedTime())
		return;

	QueryTopRanker();
}

bool CDNDonationScheduler::PassedTime()
{
	// 지정된 날에 포함 되는지
	CTimeSet CurTime;
	WORD wNextYear = (CurTime.GetMonth() < 12) ? CurTime.GetYear() : CurTime.GetYear() + 1;
	WORD wNextMonth = (CurTime.GetMonth() < 12) ? CurTime.GetMonth() + 1 : 1;
	CTimeSet NextTime(wNextYear, wNextMonth, 1, 0, 0, 0);

	INT64 nInterval = (NextTime - CurTime) / 3600 / 24;
	if (nInterval > DONATION_BROADCAST_TIME)
	{
		m_btNextHour = 0;
		return false;
	}

	// 매 정각인지 (라기보단 정각이 지난 시간인지. 그래서 서버를 껐다키면 처음에 한번은 항상 걸린다.)
	if (m_btNextHour > CurTime.GetHour())
		return false;

	m_btNextHour = (CurTime.GetHour() < 24) ? CurTime.GetHour() + 1 : 0;

	return true;
}

void CDNDonationScheduler::QueryTopRanker()
{
	if (m_WorldIDs.empty())
		return;

	BYTE cThreadID;
	CDNDBConnection* pDBCon = g_pDBConnectionManager->GetDBConnection(cThreadID);
	if (!pDBCon)
	{
		_DANGER_POINT();
		return;
	}

#if defined (PRE_ADD_DONATION)
	for each (char cWorldID in m_WorldIDs)
	{
		pDBCon->QueryDonationTopRanker(cThreadID, cWorldID);
	}
#endif
}