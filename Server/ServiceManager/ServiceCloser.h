
#pragma once

class CserviceCloser
{
public:
	CserviceCloser();
	~CserviceCloser();

	bool IdleProcess();

	bool ServiceClose(int nMinAfter);
	bool CancelServiceClose();

	bool IsClosing(__time64_t &_tCloseTime);

private:
	CSyncLock m_Sync;

	__time64_t m_tServiceCloseOderedTime;		//명령내린시각
	__time64_t m_tServiceCloseTime;				//떨어질시각
};