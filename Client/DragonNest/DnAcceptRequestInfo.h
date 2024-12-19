#pragma once

#ifdef PRE_MOD_INTEG_SYSTEM_STATE

class CDnAcceptRequestInfo
{
public:
	CDnAcceptRequestInfo() : m_fAcceptTime(10.f) {}
	virtual void Clear() { m_szOtherSideName = L""; m_fAcceptTime = 10.f; }
	virtual bool OnProcess(float fElapsedTime) { m_fAcceptTime -= fElapsedTime; return true; }
	virtual bool OnClear() { return true; }
	virtual bool OnTerminate() { return true; }

	std::wstring m_szOtherSideName;
	float m_fAcceptTime;
};

class CDnPartyAcceptRequestAskInfo : public CDnAcceptRequestInfo
{
public:
	CDnPartyAcceptRequestAskInfo() : m_JobId(-1), m_Level(0) {}
	void Clear() { m_JobId = -1; m_Level = -1; CDnAcceptRequestInfo::Clear(); }
	bool OnProcess(float fElapsedTime);
	bool OnClear();
	bool OnTerminate();

	int	m_JobId;
	int	m_Level;
};

class CDnQuickPvPAcceptRequestInfo : public CDnAcceptRequestInfo
{
public:
	CDnQuickPvPAcceptRequestInfo() : m_SenderSessionID(0) {}
	void Clear() { m_SenderSessionID = 0; CDnAcceptRequestInfo::Clear(); }
	bool OnProcess(float fElapsedTime);
	bool OnClear() { return true; }
	bool OnTerminate();

	UINT m_SenderSessionID;
};

#endif // PRE_MOD_INTEG_SYSTEM_STATE