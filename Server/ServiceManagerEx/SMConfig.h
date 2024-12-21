#pragma once

wstring GenerateServerReportNation();
wstring GenerateServerReportFileName();

class FrameConfig
{
public:
	FrameConfig();
	~FrameConfig();

public:
	CPoint size;
};

class ViewConfig
{
public:
	ViewConfig();
	~ViewConfig();

public:
	COLORREF GetBgColor() const;
	COLORREF GetFontColor(const wstring& name) const;
	COLORREF GetDefaultFontColor() const;

public:
	COLORREF bgColor;
	map<wstring, COLORREF> fontColors;
};

class EMailConfig
{
public:
	void Reset();

public:
	wstring serverIp;
	wstring serverPage;
	wstring boundary;
	wstring sender;
	wstring subject;
	vector<wstring> msg;
	vector<wstring> recipients;
	vector<wstring> allowance;
};

class ScheduleJob
{
public:
	wstring type;
	wstring name;
	CTime beginTime;
	CTime endTime;
	CTimeSpan interval;
	bool repeat;
};

class ConfigEx
{
public:
	ConfigEx();
	~ConfigEx();

public:
	bool CreateServiceManagerExXML();
	void LoadConfig();
	void SetDefault();
	void Reset();

	const FrameConfig* GetFrameConfig(const wstring& name) const;
	const ViewConfig* GetViewConfig(const wstring& name) const;
	const EMailConfig& GetEMailConfig() const;

public:
	size_t cmdHistoryCount;
	size_t maxLogCount;
	size_t serverReportDays;
	bool serverReportDetail;
	size_t exceptLogSize;
	map<wstring, const FrameConfig*> frames;
	map<wstring, const ViewConfig*> views;
	vector<const ScheduleJob*> schedules;
	EMailConfig email;
};