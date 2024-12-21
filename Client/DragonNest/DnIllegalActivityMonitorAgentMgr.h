#pragma once

#include "RyeolHttpClient.h"

#ifdef PRE_MONITOR_SUPER_NOTE

using namespace Ryeol;

enum eIAMA_CheckType
{
	eIAMA_MONEY,
	eIAMA_MAX
};

class CDnHTTPReporter
{
public:
	struct ReportingServerInfo
	{
		ReportingServerInfo()
		{
			// kb ¥‹¿ß
			nMaxReportSize = 3;
		}
		std::string szServerURL;
		std::string szViewServerURL;
		DWORD		nMaxReportSize;
	};

	CDnHTTPReporter();
	virtual ~CDnHTTPReporter() {}

	long HttpReporting();
	void SetReportFileName(const std::string& str) { m_szReportFileName = str; }

private:
	CHttpClientA			m_HttpClient;
	ReportingServerInfo		m_ReportHttpInfo;
	std::string				m_szReportFileName;
};

class CDnIllegalActivityMonitorAgent
{
public:
	CDnIllegalActivityMonitorAgent(CDnHTTPReporter* pReporter) : m_pHTTPReporter(pReporter) { m_Type = eIAMA_MAX; m_fCheckTerm = 0.f; m_fCurrentTerm = 0.f; }

	virtual bool OnAdd(eIAMA_CheckType type, float checkTerm)	{ m_Type = type; m_fCheckTerm = checkTerm; return true; }
	virtual void OnTerminate()					{}
	virtual bool Process(float fElapsedTime)	{ return true; }
	eIAMA_CheckType GetType() const				{ return m_Type; }

protected:
	eIAMA_CheckType m_Type;
	float			m_fCheckTerm;
	float			m_fCurrentTerm;
	CDnHTTPReporter* m_pHTTPReporter;
};

class CDnIllegalActivityMonitorAgentMgr
{
public:
	CDnIllegalActivityMonitorAgentMgr();
	virtual ~CDnIllegalActivityMonitorAgentMgr();

	void AddCheckMonitor(eIAMA_CheckType type, float checkTerm);
	void DelCheckMonitor(eIAMA_CheckType type);

	void Process(float fElapsedTime);

private:
	bool IsMonitorInList(eIAMA_CheckType type) const;

	std::vector<CDnIllegalActivityMonitorAgent*>	m_Activities;
	CDnHTTPReporter m_HTTPReporter;
};

//-------------------------------------------------------------------------

class CDnIAMMoneyAgent : public CDnIllegalActivityMonitorAgent
{
public:
	CDnIAMMoneyAgent(CDnHTTPReporter* pReporter) : CDnIllegalActivityMonitorAgent(pReporter), m_InitialCoin( 0 ) {}

	bool OnAdd(eIAMA_CheckType type, float checkTerm);
	bool Process(float fElapsedTime);

private:
	INT64	m_InitialCoin;
};

#endif