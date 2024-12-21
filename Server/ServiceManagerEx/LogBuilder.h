#pragma once

#include "Log.h"
#include "LogInfo.h"
#include "ErrorCode.h"

struct ExceptionLog
{
	wstring time;
	wstring type;
	wstring sid;
	wstring ip;
};

typedef vector<ExceptionLog> VECTOR_EXCEPTION;

struct DBDelayLog
{
	wstring time;
	wstring query;
	wstring mid;
	wstring wid;
	wstring tid;
	wstring elapsed;
};

typedef vector<DBDelayLog> VECTOR_DBDELAY;

struct GameDelayLog
{
	wstring time;
	wstring log;
};

typedef vector<GameDelayLog> VECTOR_GAMEDELAY;

struct DBErrorLog
{
	wstring query;
	wstring mid;
};

typedef vector<DBErrorLog> VECTOR_DBERROR;

class CLogBuilder
{
public:
	CLogBuilder() : m_pFile(NULL) {}
	~CLogBuilder() {}

public:
	log_error Load(const wstring& filename);
	void Unload();

	const vector<LogInfo*>& GetLogs() const { return m_logs; }

public:
	static LogInfo* Parse(const wchar_t* line);

	static void Initialize();
	static wstring TypeToString(LogType::eLogType type);
	static LogType::eLogType StringToType(const wstring& type);

private:
	static void AddType(LogType::eLogType type, const wstring& typeStr);

private:
	FILE* m_pFile;
	vector<LogInfo*> m_logs;

	typedef map<LogType::eLogType, const wstring> MAP_LOG_STRING;
	static MAP_LOG_STRING s_LogStrings;

	typedef map<const wstring, LogType::eLogType> MAP_LOG_ENUMERATION; 
	static MAP_LOG_ENUMERATION s_LogEnumerations;
};
