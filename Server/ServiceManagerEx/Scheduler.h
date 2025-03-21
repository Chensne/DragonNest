#pragma once

class CServiceManagerExApp;
class ScheduleJob;

namespace SimpleScheduler
{
	const int NO_EVENT = 0;
	const int REPEAT_SCHEDULE = 1;
	const int END_SCHEDULE = 2;

	typedef CServiceManagerExApp Reference;

	// ScheduleObj
	class CScheduleObj
	{
	public:
		CScheduleObj(Reference* pReference, const wstring& name, CTime curTime, CTime beginTime, CTime endTime, CTimeSpan interval, bool repeat);
		virtual ~CScheduleObj();

	public:
		const wstring& Name() const;
		bool IsRepeat() const;
		bool IsPassedEndTime(CTime curTime) const;

	public:
		int Update(CTime curTime);

	public:
		virtual void OnRegister();

	protected:
		virtual void OnEvent() = 0;

	private:
		void ExtendEventTime(CTime curTime);

	private:
		const wstring name;
		const CTime beginTime;
		const CTime endTime;
		const CTimeSpan interval;
		const bool repeat;

	private:
		CTime eventTime;

	protected:
		Reference* pReference;
	};

	// ScheduleLog
	class CScheduleLog : public CScheduleObj
	{
	public:
		CScheduleLog(Reference* pReference, const wstring& name, CTime curTime, CTime beginTime, CTime endTime, CTimeSpan interval, bool repeat);
		virtual ~CScheduleLog();

	protected:
		virtual void OnEvent() override;
	};

	// ScheduleServerReport
	class CScheduleServerReport : public CScheduleObj
	{
	public:
		CScheduleServerReport(Reference* pReference, const wstring& name, CTime curTime, CTime beginTime, CTime endTime, CTimeSpan interval, bool repeat);
		virtual ~CScheduleServerReport();

	protected:
		virtual void OnEvent() override;
	};

	// Scheduler
	class CScheduler
	{
	public:
		CScheduler();
		~CScheduler();

	public:
		bool Register(CScheduleObj* pObj);
		void Unregister(const wstring& name);
		void Update();
		void Clear();

	public:
		static CScheduleObj* GenerateScheduleObj(Reference* pReference, CTime curTime, const ScheduleJob* pScheduleJob);

	private:
		map<wstring, CScheduleObj*> schedules;
	};
};