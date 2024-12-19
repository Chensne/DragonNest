#include "StdAfx.h"
#include "ServerReporter.h"
#include "LogBuilder.h"
#include "DataManager.h"
#include "ServiceManager.h"

namespace ServerReport
{
	// CReportView
	CReportView::CReportView(const wstring& d)
		: day(d)
	{
	}

	CReportView::~CReportView()
	{
		for each (View* pView in views)
		{
			delete pView;
		}
	}

	void CReportView::AddView(View* pView)
	{
		views.push_back(pView);
	}

	const wstring& CReportView::GetDay() const
	{
		return day;
	}

	const vector<View*>& CReportView::GetViews() const
	{
		return views;
	}

	// CServerReporter
	CServerReporter::CServerReporter(void)
	{
	}

	CServerReporter::~CServerReporter(void)
	{
	}

	void CServerReporter::Report(size_t days, bool detail, OUT wstring& report)
	{
		vector<CReportView*> temp;
		Report(days, detail, temp);

		wstringstream ss;
		for each (CReportView* pReport in temp)
		{
			ss << L"<" << pReport->GetDay() << L">" << L"\r\n\r\n";
			for each (View* pView in pReport->GetViews())
			{
				ss << pView->overview.str();
				ss << pView->detailview.str();
			}
			ss << L"\r\n\r\n";

			delete pReport;
		}

		report = ss.str();
	}

	void CServerReporter::Report(size_t days, bool detail, OUT vector<CReportView*>& report)
	{
		map<wstring, wstring> times;
		GetReportTime(days, times);

		is_dbdelay = false;

		map<wstring, wstring>::const_reverse_iterator it = times.rbegin();
		for (; it != times.rend(); ++it)
		{
			CReportView* pReport = BuildReport(it->first, it->second, detail);
			if (!pReport)
				continue;

			report.push_back(pReport);
		}
	}

	CReportView* CServerReporter::BuildReport(const wstring& day, const wstring& filename, bool detail) const
	{
		CReportView* pReport = new CReportView(day);
		View* pExceptionView = new View();
		View* pDBSystemErrorView = new View();
		View* pDBErrorView = new View();
		View* pDBDelayView = new View();
		View* pGameDelayView = new View();
		View* pVillageDelayView = new View();

		try
		{
			ParseExceptionLog(filename, detail, pExceptionView->overview, pExceptionView->detailview);
			ParseDBSystemErrorLog(filename, detail, pDBSystemErrorView->overview, pDBSystemErrorView->detailview);
			ParseDBErrorLog(filename, detail, pDBErrorView->overview, pDBErrorView->detailview);
			ParseDBDelayLog(filename, detail, pDBDelayView->overview, pDBDelayView->detailview, (bool)is_dbdelay);
			ParseGameDelayLog(filename, detail, pGameDelayView->overview, pGameDelayView->detailview);
			ParseVillageDelayLog(filename, detail, pVillageDelayView->overview, pVillageDelayView->detailview);
		}
		catch (...)
		{
			delete pExceptionView;
			delete pDBSystemErrorView;
			delete pDBErrorView;
			delete pDBDelayView;
			delete pGameDelayView;
			delete pVillageDelayView;
			delete pReport;
			g_Log.Log(LogType::_FILELOG, L"BuildExceptionReport failed. [day:%s]\n", day.c_str());
			return NULL;
		}

		pReport->AddView(pExceptionView);
		pReport->AddView(pDBSystemErrorView);
		pReport->AddView(pDBErrorView);
		pReport->AddView(pDBDelayView);
		pReport->AddView(pGameDelayView);
		pReport->AddView(pVillageDelayView);

		return pReport;
	}

	void CServerReporter::ParseExceptionLog(const wstring& filename, bool detail, OUT wstringstream& overview, OUT wstringstream& detailview) const
	{
		static const wstring exceptionLog(L"log\\ExceptionReport_");

		CLogBuilder builder;
		log_error error = builder.Load(exceptionLog + filename);
		if (error == error_size_overflow)
		{
			overview << L"ExceptionReport_" << filename << L" file is too large.\r\n";
			return;
		}

		map<wstring, VECTOR_EXCEPTION*> terminated;
		map<wstring, VECTOR_EXCEPTION*> sessionDump;

		const vector<LogInfo*>& logs = builder.GetLogs();
		for each (const LogInfo* pLog in logs)
		{
			//terminated
			size_t pos = pLog->text.find(L"Terminated");
			if (pos != wstring::npos)
			{
				pos = pLog->text.find(L"[SID:");
				if (pos == wstring::npos)
				{
					g_Log.Log(LogType::_FILELOG, L"Parse failed.(step_t1) %s\n", pLog->text.c_str());
					continue;
				}

				size_t pos_end = pLog->text.find(L"]", pos);
				if (pos_end == wstring::npos)
				{
					g_Log.Log(LogType::_FILELOG, L"Parse failed.(step_t2) %s\n", pLog->text.c_str());
					continue;
				}

				wstring strId = pLog->text.substr(pos + 5, pos_end - pos - 5);
				int sid = _wtoi(strId.c_str());
				const TServerExcuteData* pExecuteData = g_pServiceManager->GetServerExecuteData(sid);
				if (!pExecuteData)
				{
					g_Log.Log(LogType::_ERROR, L"Server data not found. SID[%d]", sid);
					continue;
				}

				const TNetLauncher* pNetLauncherData = g_pServiceManager->GetLauncherInfo(pExecuteData->nAssignedLauncherID);
				if (!pNetLauncherData)
				{
					g_Log.Log(LogType::_ERROR, L"NetLauncher data not found. NID[%d]\n", pExecuteData->nAssignedLauncherID);
					continue;
				}

				VECTOR_EXCEPTION* list = NULL;
				map<wstring, VECTOR_EXCEPTION*>::iterator it = terminated.find(pExecuteData->wszType);
				if (it == terminated.end())
				{
					list = new VECTOR_EXCEPTION;
					terminated.insert(make_pair(pExecuteData->wszType, list));
				}
				else
				{
					list = it->second;
				}

				ExceptionLog log;
				log.time = pLog->date;
				if (log.time.size() >= 11)
					log.time.substr(11, 8);

				log.type = pExecuteData->wszType;
				log.sid = strId;
				log.ip = pNetLauncherData->szIP;
				list->push_back(log);

				continue;
			}

			//session dump
			pos = pLog->text.find(L"Exception");
			if (pos != wstring::npos)
			{
				pos = pLog->text.find(L"MID[");
				if (pos == wstring::npos)
				{
					g_Log.Log(LogType::_FILELOG, L"Parse failed.(step_e1) %s\n", pLog->text.c_str());
					continue;
				}

				size_t pos_end = pLog->text.find(L"]", pos);
				if (pos_end == wstring::npos)
				{
					g_Log.Log(LogType::_FILELOG, L"Parse failed.(step_e2) %s\n", pLog->text.c_str());
					continue;
				}

				wstring strId = pLog->text.substr(pos + 4, pos_end - pos - 4);
				int sid = _wtoi(strId.c_str());
				const TServerExcuteData* pExecuteData = g_pServiceManager->GetServerExecuteData(sid);
				if (!pExecuteData)
				{
					g_Log.Log(LogType::_ERROR, L"Server data not found. SID[%d]\n", sid);
					continue;
				}

				const TNetLauncher* pNetLauncherData = g_pServiceManager->GetLauncherInfo(pExecuteData->nAssignedLauncherID);
				if (!pNetLauncherData)
				{
					g_Log.Log(LogType::_ERROR, L"NetLauncher data not found. NID[%d]\n", pExecuteData->nAssignedLauncherID);
					continue;
				}

				VECTOR_EXCEPTION* list = NULL;
				map<wstring, VECTOR_EXCEPTION*>::iterator it = sessionDump.find(pExecuteData->wszType);
				if (it == sessionDump.end())
				{
					list = new VECTOR_EXCEPTION;
					sessionDump.insert(make_pair(pExecuteData->wszType, list));
				}
				else
				{
					list = it->second;
				}
				
				ExceptionLog log;
				log.time = pLog->date;
				if (log.time.size() >= 11)
					log.time.substr(11, 8);

				log.type = pExecuteData->wszType;
				log.sid = strId;
				log.ip = pNetLauncherData->szIP;
				list->push_back(log);

				continue;
			}
		}

		for each (map<wstring, VECTOR_EXCEPTION*>::value_type v in terminated)
		{
			overview << v.first << L" server full dump. " << v.second->size() << L" times.\r\n";

			for each (ExceptionLog log in *(v.second))
			{
				overview << log.time << L" SID:" << log.sid << L" IP:" << log.ip << "\r\n";
			}

			overview << L"\r\n";
			delete v.second;
		}

		if (sessionDump.empty())
			return;

		for each (map<wstring, VECTOR_EXCEPTION*>::value_type v in sessionDump)
		{
			overview << v.first << L" server session dump. " << v.second->size() << L" times.\r\n";

			for each (ExceptionLog log in *(v.second))
			{
				overview << log.time << L" SID:" << log.sid << L" IP:" << log.ip << L"\r\n";
			}

			overview << L"\r\n";
			delete v.second;
		}

		detailview << L"\r\n";
	}

	void CServerReporter::ParseDBDelayLog(const wstring& filename, bool detail, OUT wstringstream& overview, OUT wstringstream& detailview, OUT bool& delay) const
	{
		static const wstring dbLog(L"log\\DBDelay_");

		VECTOR_DBDELAY dbdelay;

		CLogBuilder builder;
		log_error error = builder.Load(dbLog + filename);
		if (error == error_size_overflow)
		{
			overview << L"DBDelay_" << filename << L" file is too large.\r\n";
			return;
		}

		const vector<LogInfo*>& logs = builder.GetLogs();
		for each (const LogInfo* pLog in logs)
		{
			size_t pos = pLog->text.find(L"[Query Over Time:");
			if (pos != wstring::npos)
			{
				size_t pos_end = pLog->text.find(L"]");
				if (pos_end == wstring::npos)
				{
					g_Log.Log(LogType::_FILELOG, L"Parse failed.(step_d1) %s\n", pLog->text.c_str());
					continue;
				}

				DBDelayLog log;
				log.time = pLog->date;
				if (log.time.size() >= 11)
					log.time.substr(11, 8);

				log.query = pLog->text.substr(pos + 17, pos_end - 17);

				pos = pLog->text.find(L"MID:");
				if (pos == wstring::npos)
				{
					g_Log.Log(LogType::_FILELOG, L"Parse failed.(step_d2) %s\n", pLog->text.c_str());
					continue;
				}

				pos_end = pLog->text.find(L" ", pos);
				if (pos_end == wstring::npos)
				{
					g_Log.Log(LogType::_FILELOG, L"Parse failed.(step_d3) %s\n", pLog->text.c_str());
					continue;
				}

				log.mid = pLog->text.substr(pos + 4, pos_end - pos - 4);

				pos = pLog->text.find(L"ThreadID:");
				if (pos == wstring::npos)
				{
					g_Log.Log(LogType::_FILELOG, L"Parse failed.(step_d4) %s\n", pLog->text.c_str());
					continue;
				}

				pos_end = pLog->text.find(L" ", pos);
				if (pos_end == wstring::npos)
				{
					g_Log.Log(LogType::_FILELOG, L"Parse failed.(step_d5) %s\n", pLog->text.c_str());
					continue;
				}

				log.tid = pLog->text.substr(pos + 9, pos_end - pos - 9);

				pos = pLog->text.find(L"time=");
				if (pos == wstring::npos)
				{
					g_Log.Log(LogType::_FILELOG, L"Parse failed.(step_d6) %s\n", pLog->text.c_str());
					continue;
				}

				pos_end = pLog->text.find(L" ", pos);
				if (pos_end == wstring::npos)
				{
					g_Log.Log(LogType::_FILELOG, L"Parse failed.(step_d7) %s\n", pLog->text.c_str());
					continue;
				}

				log.elapsed = pLog->text.substr(pos + 5, pos_end - pos - 5);

				dbdelay.push_back(log);
			}
		}

		if (dbdelay.empty())
			return;

		delay = true;

		overview << L"DB delayed. " << dbdelay.size() << L" times.\r\n";

		if (detail)
		{
			for each (DBDelayLog log in dbdelay)
			{
				detailview << log.time << L" [" << log.query << L"]" << L" MID:" << log.mid << L" TID:" << log.tid << L" elapsed:" << log.elapsed << L" ms\r\n";
			}

			detailview << L"\r\n";
		}
	}

	void CServerReporter::ParseGameDelayLog(const wstring& filename, bool detail, OUT wstringstream& overview, OUT wstringstream& detailview) const
	{
		static const wstring gameLog(L"log\\GameDelay_");

		CLogBuilder builder;
		log_error error = builder.Load(gameLog + filename);
		if (error == error_size_overflow)
		{
			overview << L"GameDelay_" << filename << L" file is too large.\r\n";
			return;
		}

		const vector<LogInfo*>& logs = builder.GetLogs();
		if (logs.empty())
			return;

		overview << L"Game delayed. " << logs.size() << L" times.\r\n";

		if (detail)
		{
			for each (const LogInfo* pLog in logs)
			{
				detailview << pLog->date << L" " << pLog->text << L"\r\n";
			}

			detailview << L"\r\n";
		}
	}

	void CServerReporter::ParseVillageDelayLog(const wstring& filename, bool detail, OUT wstringstream& overview, OUT wstringstream& detailview) const
	{
		static const wstring villageLog(L"log\\VillageDelay_");

		CLogBuilder builder;
		log_error error = builder.Load(villageLog + filename);
		if (error == error_size_overflow)
		{
			overview << L"VillageDelay_" << filename << L" file is too large.\r\n";
			return;
		}

		const vector<LogInfo*>& logs = builder.GetLogs();
		if (logs.empty())
			return;

		overview << L"Village delayed. " << logs.size() << L" times.\r\n";

		if (detail)
		{
			for each (const LogInfo* pLog in logs)
			{
				detailview << pLog->date << L" " << pLog->text << L"\r\n";
			}

			detailview << L"\r\n";
		}
	}

	void CServerReporter::ParseDBErrorLog(const wstring& filename, bool detail, OUT wstringstream& overview, OUT wstringstream& detailview) const
	{
		static const wstring dbLog(L"log\\DBError_");

		CLogBuilder builder;
		log_error error = builder.Load(dbLog + filename);
		if (error == error_size_overflow)
		{
			overview << L"DBError_" << filename << L" file is too large.\r\n";
			return;
		}

		const vector<LogInfo*>& logs = builder.GetLogs();
		if (logs.empty())
			return;

		overview << L"DB error. " << logs.size() << L" times.\r\n";

		if (detail)
		{
			for each (const LogInfo* pLog in logs)
			{
				detailview << pLog->date << L" " << pLog->text << L"\r\n";
			}

			detailview << L"\r\n";
		}
	}

	void CServerReporter::ParseDBSystemErrorLog(const wstring& filename, bool detail, OUT wstringstream& overview, OUT wstringstream& detailview) const
	{
		static const wstring dbLog(L"log\\DBSystemError_");

		CLogBuilder builder;
		log_error error = builder.Load(dbLog + filename);
		if (error == error_size_overflow)
		{
			overview << L"DBSystemError_" << filename << L" file is too large.\r\n";
			return;
		}

		const vector<LogInfo*>& logs = builder.GetLogs();
		if (logs.empty())
			return;

		overview << L"DB system error. " << logs.size() << L" times.\r\n";

		if (detail)
		{
			for each (const LogInfo* pLog in logs)
			{
				detailview << pLog->date << L" " << pLog->text << L"\r\n";
			}

			detailview << L"\r\n";
		}
	}

	void CServerReporter::GetReportTime(size_t days, OUT map<wstring, wstring>& times) const
	{
		CTime time = CTime::GetCurrentTime();
		CTimeSpan oneDay(1, 0, 0, 0);

		for (size_t i = 0; i < days; ++i)
		{
			times.insert(make_pair(time.Format(L"%Y-%m-%d ").GetBuffer() + ConvertDayOfWeek(time.GetDayOfWeek()), 
				time.Format(L"%Y-%m-%d").GetBuffer() + wstring(L".log")));
			time = time - oneDay;
		}
	}

	wstring CServerReporter::ConvertDayOfWeek(int weekday) const
	{
		switch (weekday)
		{
		case 1: 
			return L"Sunday";

		case 2: 
			return L"Monday";

		case 3: 
			return L"Tuesday";

		case 4: 
			return L"Wednesday";

		case 5: 
			return L"Thursday";

		case 6: 
			return L"Friday";

		case 7: 
			return L"Saturday";

		default: 
			return L"Unknown";
		}
	}
}