#pragma once

class CDNDonationScheduler : public CSingleton<CDNDonationScheduler>
{
public:
	CDNDonationScheduler();
	~CDNDonationScheduler();

public:
	void AddWorldID(char cWorldID);
	void DoUpdate(DWORD dwCurTick);

private:
	bool PassedTime();
	void QueryTopRanker();

private:
	std::set<char> m_WorldIDs;
	DWORD m_dwPrevTick;
	BYTE m_btNextHour;
};

extern CDNDonationScheduler g_DonationScheduler;