#include "StdAfx.h"
#include "LogBuilder.h"
#include "ServiceManagerEx.h"

const wstring TYPE_HEAD = L"<";
const wstring TYPE_TAIL = L">";

CLogBuilder::MAP_LOG_STRING CLogBuilder::s_LogStrings;
CLogBuilder::MAP_LOG_ENUMERATION CLogBuilder::s_LogEnumerations;

log_error CLogBuilder::Load(const wstring& filename)
{
	m_pFile = _wfopen(filename.c_str(), L"r");
	if (!m_pFile)
		return error_file_not_found;

	fseek(m_pFile, 0, SEEK_END);
	long size = ftell(m_pFile);
	fseek(m_pFile, 0, SEEK_SET);

	const ConfigEx& config = ((CServiceManagerExApp*)::AfxGetApp())->GetConfigEx();
	if (size > (long)(1024 * config.exceptLogSize))
		return error_size_overflow;

	/////////
	Unload();
	/////////

	wchar_t line[1024];

	while (1)
	{
		if (fgetws(line, _countof(line), m_pFile) == NULL)
			break;

		LogInfo* pLog = Parse(line);
		if (!pLog)
			continue;

		m_logs.push_back(pLog);
	}

	fclose(m_pFile);
	m_pFile = NULL;

	return error_none;
}

void CLogBuilder::Unload()
{
	for each (const LogInfo* v in m_logs)
	{
		delete v;
	}
	m_logs.clear();
}

LogInfo* CLogBuilder::Parse(const wchar_t* line)
{
	wstring buf = line;
	
	try
	{
		// date
		size_t pos1 = buf.find(L'\'');
		size_t pos2 = buf.find(L'\'', pos1 + 1);
		if (pos1 == wstring::npos || pos2 == wstring::npos)
			return NULL;

		wstring date = buf.substr(pos1, pos2);
		date = date.substr(1);
		date = date.substr(0, date.length() - 1);

		// type
		wstring type;
		wstring log = buf.substr(pos2 + 2);

		size_t header_size = TYPE_HEAD.size();
		wstring header = log.substr(0, header_size);
		if (header.compare(TYPE_HEAD) == 0)
		{
			size_t pos = log.find(TYPE_TAIL, header_size);
			type = log.substr(header_size, pos - header_size);
			log = log.substr(pos + 2);
		}
		else
		{
			type = L"_FILELOG";
		}

		// erase \r\n
		size_t pos = log.rfind('\n');
		if (pos != wstring::npos)
			log = log.substr(0, pos);

		pos = log.rfind('\r');
		if (pos != wstring::npos)
			log = log.substr(0, pos);

		return new LogInfo(date, type, log);
	}
	catch (...)
	{
		return NULL;
	}
}

wstring CLogBuilder::TypeToString(LogType::eLogType type)
{
	MAP_LOG_STRING::iterator it = s_LogStrings.find(type);
	if (it == s_LogStrings.end())
		return wstring(L"_NORMAL");

	return it->second;
}

LogType::eLogType CLogBuilder::StringToType(const wstring& type)
{
	MAP_LOG_ENUMERATION::iterator it = s_LogEnumerations.find(type);
	if (it == s_LogEnumerations.end())
		return LogType::_NORMAL;

	return it->second;
}

void CLogBuilder::Initialize()
{
	s_LogStrings.clear();
	s_LogEnumerations.clear();

	AddType(LogType::_NORMAL, wstring(L"_NORMAL"));
	AddType(LogType::_ERROR, wstring(L"_ERROR"));
	AddType(LogType::_DANGERPOINT, wstring(L"_DANGERPOINT"));
	AddType(LogType::_FILELOG, wstring(L"_FILELOG"));
	AddType(LogType::_HACK, wstring(L"_HACK"));
	AddType(LogType::_HACKSHIELD, wstring(L"_HACKSHIELD"));
	AddType(LogType::_ITEMDUPLICATE, wstring(L"_ITEMDUPLICATE"));
	AddType(LogType::_WEAPONTYPE, wstring(L"_WEAPONTYPE"));
	AddType(LogType::_SESSIONCRASH, wstring(L"_SESSIONCRASH"));
	AddType(LogType::_ROOMCRASH, wstring(L"_ROOMCRASH"));
	AddType(LogType::_UNREACHABLE, wstring(L"_UNREACHABLE"));
	AddType(LogType::_LOADINGDISCONNECT, wstring(L"_LOADINGDISCONNECT"));
	AddType(LogType::_ROOMFINALIZECRASH, wstring(L"_ROOMFINALIZECRASH"));
	AddType(LogType::_GAMECONNECTLOG, wstring(L"_GAMECONNECTLOG"));
	AddType(LogType::_GAMESERVERDELAY, wstring(L"_GAMESERVERDELAY"));
	AddType(LogType::_ENTERGAMECHECK, wstring(L"_ENTERGAMECHECK"));
	AddType(LogType::_DROPITEMPENALTY, wstring(L"_DROPITEMPENALTY"));
	AddType(LogType::_TREASUREPENALTY, wstring(L"_TREASUREPENALTY"));
	AddType(LogType::_CLEARGRADEPENALTY, wstring(L"_CLEARGRADEPENALTY"));
	AddType(LogType::_PRELOADED_DYNAMICLOAD, wstring(L"_PRELOADED_DYNAMICLOAD"));
	AddType(LogType::_DELOBJECT_NAMEKEYFINDERR, wstring(L"_DELOBJECT_NAMEKEYFINDERR"));
	AddType(LogType::_OCTREELOG, wstring(L"_OCTREELOG"));
	AddType(LogType::_INVALIDACTOR, wstring(L"_INVALIDACTOR"));
	AddType(LogType::_GPKERROR, wstring(L"_GPKERROR"));
	AddType(LogType::_ACCEPTORERROR, wstring(L"_ACCEPTORERROR"));
	AddType(LogType::_QUESTHACK, wstring(L"_QUESTHACK"));
	AddType(LogType::_ZOMBIEUSER, wstring(L"_ZOMBIEUSER"));
	AddType(LogType::_EVENTLIST, wstring(L"_EVENTLIST"));
	AddType(LogType::_FARM, wstring(L"_FARM"));
	AddType(LogType::_GUILDWAR, wstring(L"_GUILDWAR"));
	AddType(LogType::_MOVEPACKET_SEQ, wstring(L"_MOVEPACKET_SEQ"));
	AddType(LogType::_SKILL_ACTION, wstring(L"_SKILL_ACTION"));
	AddType(LogType::_FILEDBLOG, wstring(L"_FILEDBLOG"));
	AddType(LogType::_LADDER, wstring(L"_LADDER"));
	AddType(LogType::_PVPROOM, wstring(L"_PVPROOM"));
	AddType(LogType::_PING, wstring(L"_PING"));
}

void CLogBuilder::AddType(LogType::eLogType type, const wstring& typeStr)
{
	s_LogStrings.insert(make_pair(type, typeStr));
	s_LogEnumerations.insert(make_pair(typeStr, type));
}