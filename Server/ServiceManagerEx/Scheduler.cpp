#include "StdAfx.h"
#include "Scheduler.h"
#include "Log.h"
#include "ServiceManagerEx.h"
#include "HttpUpdater.h"
#include "ServerReporter.h"
#include "ServiceManager.h"
#include <sstream>

namespace SimpleScheduler
{
	// ScheduleObj
	CScheduleObj::CScheduleObj(Reference* pReference, const wstring& name, CTime curTime, CTime beginTime, CTime endTime, CTimeSpan interval, bool repeat)
		: pReference(pReference), name(name), beginTime(beginTime), endTime(endTime), interval(interval), repeat(repeat), eventTime(beginTime)
	{
		ExtendEventTime(curTime);
	}

	CScheduleObj::~CScheduleObj()
	{

	}

	const wstring& CScheduleObj::Name() const
	{
		return name;
	}

	bool CScheduleObj::IsRepeat() const
	{
		return repeat;
	}

	bool CScheduleObj::IsPassedEndTime(CTime curTime) const
	{
		return (endTime < curTime);
	}

	int CScheduleObj::Update(CTime curTime)
	{
		if (eventTime > curTime)
			return NO_EVENT;

		if (IsPassedEndTime(curTime))
		{
			g_Log.Log(LogType::_NORMAL, L"%s Schedule ended.\n", name.c_str());
			return END_SCHEDULE;
		}

		OnEvent();
		ExtendEventTime(curTime);
		g_Log.Log(LogType::_NORMAL, L"%s Scheduled. [next time : %s]\n", name.c_str(), eventTime.Format(L"%Y-%m-%d %H:%M:%S").GetBuffer(0));

		if (!IsRepeat())
		{
			g_Log.Log(LogType::_NORMAL, L"%s Schedule ended.\n", name.c_str());
			return END_SCHEDULE;
		}

		return REPEAT_SCHEDULE;
	}

	void CScheduleObj::OnRegister()
	{
		g_Log.Log(LogType::_NORMAL, L"%s schedule registered. [next time : %s]\n", name.c_str(), eventTime.Format(L"%Y-%m-%d %H:%M:%S").GetBuffer(0));
	}

	void CScheduleObj::ExtendEventTime(CTime curTime)
	{
		while (eventTime <= curTime)
		{
			eventTime += interval;
		}
	}

	// ScheduleLog
	CScheduleLog::CScheduleLog(Reference* pReference, const wstring& name, CTime curTime, CTime beginTime, CTime endTime, CTimeSpan interval, bool repeat)
		: CScheduleObj(pReference, name, curTime, beginTime, endTime, interval, repeat)
	{

	}

	CScheduleLog::~CScheduleLog()
	{

	}

	void CScheduleLog::OnEvent()
	{

	}

	// ScheduleServerReport
	CScheduleServerReport::CScheduleServerReport(Reference* pReference, const wstring& name, CTime curTime, CTime beginTime, CTime endTime, CTimeSpan interval, bool repeat)
		: CScheduleObj(pReference, name, curTime, beginTime, endTime, interval, repeat)
	{

	}

	CScheduleServerReport::~CScheduleServerReport()
	{

	}

	void CScheduleServerReport::OnEvent()
	{
		const EMailConfig& email = pReference->GetConfigEx().GetEMailConfig();
		RequestParam::boundary = email.boundary;

		vector<RequestParam> parameters;
		parameters.push_back(RequestParam(L"SenderAddress", email.sender));

		ServerReport::CServerReporter reporter;
		const ConfigEx& config = pReference->GetConfigEx();
		vector<ServerReport::CReportView*> report;
		reporter.Report(config.serverReportDays, config.serverReportDetail, report);

		wstring buffer;
		reporter.Report(config.serverReportDays, config.serverReportDetail, buffer);

		wstringstream ss;
		for (int i=0; i<(int)email.allowance.size(); i++)
		{
			if (!reporter.is_dbdelay && email.allowance[i].find(L"DB delayed") == 0)
				continue;

			ss << email.recipients[i] << L";";
		}
		parameters.push_back(RequestParam(L"ReceiverAddress", ss.str()));

		if (buffer.size() >= 1024 * 1024 * 10)
			buffer.clear();

		ss.str(L"");
		CTime time = CTime::GetCurrentTime();
		ss << GenerateServerReportNation() << L" " << time.Format(L"%Y-%m-%d").GetBuffer() << L" " << email.subject;
		parameters.push_back(RequestParam(L"Subject", ss.str()));

		ss.str(L"");
		wchar_t partition_text [256];
		g_pServiceManager->GetPartitionText(partition_text);
		ss << L"<" << partition_text << L">" << L"\r\n\r\n";
		for each (ServerReport::CReportView* pReport in report)
		{
			ss << L"<" << pReport->GetDay() << L">" << L"\r\n\r\n";

			for each (ServerReport::View* pView in pReport->GetViews())
			{
				ss << pView->overview.str();
			}

			ss << L"\r\n\r\n";
			delete pReport;
		}

		parameters.push_back(RequestParam(L"Contents", ss.str()));

		vector<RequestParam> files;
		files.push_back(RequestParam(L"AttachFile", GenerateServerReportFileName(), buffer));

		CHttpUpdater http;
		if (!http.QueryPostUpload(email.serverIp.c_str(), NULL, NULL, email.serverPage.c_str(), parameters, files))
			return;
	}

	// Scheduler
	CScheduler::CScheduler()
	{
	}

	CScheduler::~CScheduler()
	{
		for each (map<wstring, CScheduleObj*>::value_type v in schedules)
		{
			delete v.second;
		}
	}

	bool CScheduler::Register(CScheduleObj* pObj)
	{
		if (!pObj)
			return false;

		if (!schedules.insert(make_pair(pObj->Name(), pObj)).second)
			return false;

		pObj->OnRegister();

		return true;
	}

	void CScheduler::Unregister(const wstring& name)
	{
		map<wstring, CScheduleObj*>::iterator it = schedules.find(name);
		if (it == schedules.end())
			return;

		delete it->second;
		schedules.erase(it);
	}

	void CScheduler::Update()
	{
		CTime time = CTime::GetCurrentTime();

		for (map<wstring, CScheduleObj*>::iterator it = schedules.begin(); it != schedules.end();)
		{
			CScheduleObj* pObj = it->second;
			if (pObj->Update(time) == END_SCHEDULE)
				it = schedules.erase(it);
			else
				++it;
		}
	}

	void CScheduler::Clear()
	{
		for each (map<wstring, CScheduleObj*>::value_type v in schedules)
		{
			delete v.second;
		}
		schedules.clear();
	}

	CScheduleObj* CScheduler::GenerateScheduleObj(Reference* pReference, CTime curTime, const ScheduleJob* pScheduleJob)
	{
		if (curTime >= pScheduleJob->endTime)
			return NULL;

		if (pScheduleJob->type.compare(L"ServerReport") == 0)
			return new CScheduleServerReport(pReference, pScheduleJob->name, curTime, pScheduleJob->beginTime, pScheduleJob->endTime, pScheduleJob->interval, pScheduleJob->repeat);
		else if (pScheduleJob->type.compare(L"Log") == 0)
			return new CScheduleLog(pReference, pScheduleJob->name, curTime, pScheduleJob->beginTime, pScheduleJob->endTime, pScheduleJob->interval, pScheduleJob->repeat);

		return NULL;
	}
};