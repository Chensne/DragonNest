
#pragma once

class CCloseSystem
{
public:
	CCloseSystem();
	~CCloseSystem();

	bool AddClose(__time64_t _tOderedTime, __time64_t _tCloseTime);
	bool CancelClose();

	bool IsClosing(__time64_t &_tOderedTime, __time64_t &_tCloseTime);

private:
	CSyncLock m_Sync;

	__time64_t m_tOderedTime;
	__time64_t m_tCloseTime;
};

extern CCloseSystem * g_pCloseSystem;