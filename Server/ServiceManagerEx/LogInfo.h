#pragma once

class LogInfo
{
public:
	LogInfo(const wstring& date, const wstring& type, const wstring& text)
		: date(date), type(type), text(text)
	{
	}

	LogInfo(const LogInfo& log)
		: date(log.date), type(log.type), text(log.text)
	{
	}

	wstring ToString() const
	{
		return date + L" " + text;
	}

public:
	wstring date;
	wstring type;
	wstring text;
};