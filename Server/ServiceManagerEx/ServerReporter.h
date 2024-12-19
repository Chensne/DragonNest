#pragma once

#include <sstream>

namespace ServerReport
{
	struct View
	{
		wstringstream overview;
		wstringstream detailview;
	};

	class CReportView
	{
	public:
		CReportView(const wstring& d);
		~CReportView();

	public:
		void AddView(View* pView);

	public:
		const wstring& GetDay() const;
		const vector<View*>& GetViews() const;

	private:
		wstring day;
		vector<View*> views;
	};

	class CServerReporter
	{
	public:
		CServerReporter(void);
		~CServerReporter(void);

		bool is_dbdelay;

	public:
		void Report(size_t days, bool detail, OUT wstring& report);
		void Report(size_t days, bool detail, OUT vector<CReportView*>& report);

	private:
		CReportView* BuildReport(const wstring& day, const wstring& filename, bool detail) const;

	private:
		void ParseExceptionLog(const wstring& filename, bool detail, OUT wstringstream& overview, OUT wstringstream& detailview) const;
		void ParseDBDelayLog(const wstring& filename, bool detail, OUT wstringstream& overview, OUT wstringstream& detailview, OUT bool& delay) const;
		void ParseGameDelayLog(const wstring& filename, bool detail, OUT wstringstream& overview, OUT wstringstream& detailview) const;
		void ParseVillageDelayLog(const wstring& filename, bool detail, OUT wstringstream& overview, OUT wstringstream& detailview) const;
		void ParseDBErrorLog(const wstring& filename, bool detail, OUT wstringstream& overview, OUT wstringstream& detailview) const;
		void ParseDBSystemErrorLog(const wstring& filename, bool detail, OUT wstringstream& overview, OUT wstringstream& detailview) const;

	private:
		void GetReportTime(size_t days, OUT map<wstring, wstring>& times) const;
		wstring ConvertDayOfWeek(int weekday) const;
	};
}