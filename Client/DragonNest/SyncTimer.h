#pragma once

class CSyncTimer : public CSingleton<CSyncTimer>
{
public:
	CSyncTimer();
	virtual ~CSyncTimer();

	void SetServerTime( __time64_t _tStartTime );
	void Process();

	__time64_t GetCurTime();
	bool IsStarted() { return m_bStarted; }

protected:
	bool m_bStarted;
	__time64_t m_tCurTime;
	DWORD m_dwBasisTime;
};