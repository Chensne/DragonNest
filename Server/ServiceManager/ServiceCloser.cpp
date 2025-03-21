
#include "Stdafx.h"
#include "ServiceCloser.h"
#include "ServiceManager.h"

CserviceCloser::CserviceCloser()
{
	m_tServiceCloseOderedTime = 0;
	m_tServiceCloseTime = 0;
}

CserviceCloser::~CserviceCloser()
{
}

bool CserviceCloser::IdleProcess()
{
	ScopeLock <CSyncLock> Lock(m_Sync);
	if (m_tServiceCloseTime <= 0)
		return false;

	__time64_t _tNow;
	time(&_tNow);

	if (m_tServiceCloseTime < _tNow)
	{
		m_tServiceCloseOderedTime = 0;
		m_tServiceCloseTime = 0;			//초단위로 몇분뒤에 바이바이할지 구해서
		return true;
	}
	return false;
}

bool CserviceCloser::ServiceClose(int nMinAfter)
{
	__time64_t _tCloseTime;
	if (IsClosing(_tCloseTime))
		return false;

	{
		ScopeLock <CSyncLock> Lock(m_Sync);

		__time64_t _tNow;
		time(&_tNow);

		m_tServiceCloseOderedTime = _tNow;
		m_tServiceCloseTime = _tNow + (nMinAfter * 60);			//초단위로 몇분뒤에 바이바이할지 구해서
	}
	
	//서비스매니저를 통해 각서버에 알린다.
	g_pServiceManager->SendServiceClose(m_tServiceCloseOderedTime, m_tServiceCloseTime);
	return true;
}

bool CserviceCloser::CancelServiceClose()
{
	{
		ScopeLock <CSyncLock> Lock(m_Sync);
		if (m_tServiceCloseTime <= 0)
			return false;

		m_tServiceCloseOderedTime = 0;
		m_tServiceCloseTime = 0;			//초단위로 몇분뒤에 바이바이할지 구해서
	}

	g_pServiceManager->SendServiceClose(0, 0);
	return true;
}

bool CserviceCloser::IsClosing(__time64_t &_tCloseTime)
{
	ScopeLock <CSyncLock> Lock(m_Sync);
	_tCloseTime = m_tServiceCloseTime;
	return m_tServiceCloseTime > 0 ? true : false;
}
