#include "stdafx.h"
#include "DnScript.h"
#include "EtResourceMng.h"
#include <algorithm>

#if defined(_VILLAGESERVER)
#include "DNScriptAPI.h"
#elif defined(_GAMESERVER)
#include "DNGameServerScriptAPI.h"
#endif

CDNScript::CDNScript()
:m_pLua(NULL)
{
	ms_ScriptList.push_back(this);
}

CDNScript::~CDNScript()
{
	Destroy();
	std::vector<CDNScript*>::iterator it = std::find(ms_ScriptList.begin(), ms_ScriptList.end(), this);
	if ( it != ms_ScriptList.end() )
	{
		ms_ScriptList.erase(it);
	}
}

void CDNScript::ReloadAllScript()
{
	for ( size_t i = 0 ; i < ms_ScriptList.size() ; i++ )
	{
		ms_ScriptList[i]->Destroy();
		ms_ScriptList[i]->Create(ms_ScriptList[i]->m_wszScriptFileName.c_str());
	}

		
}

bool CDNScript::Create(const WCHAR* wszScriptFileName)
{
	char szScriptFileName[512] = "";
	ZeroMemory(szScriptFileName, sizeof(char)*512);
	WideCharToMultiByte( CP_ACP, 0, wszScriptFileName, -1, szScriptFileName, 512, NULL, NULL );

	m_wszScriptFileName = wszScriptFileName;

	CFileStream Stream( CEtResourceMng::GetInstance().GetFullName(szScriptFileName).c_str() , CFileStream::OPEN_READ );

	if ( Stream.IsValid() == false )
	{
		g_Log.Log(LogType::_FILELOG, L"Cannot open file : %s\n" , wszScriptFileName);
		return false;
	}

	int nLen = Stream.Size();

	char* pBuffer = new char[nLen];
	ZeroMemory(pBuffer, nLen);

	class __Temp
	{
	public:
		__Temp(char* p) { m_p = p;}
		~__Temp() { delete[] m_p; }
		char* m_p;
	};
	__Temp ScopeDelete(pBuffer);

	int nReadSize = Stream.Read(pBuffer, nLen);

	m_pLua = lua_open();
	luaL_openlibs(m_pLua);
	if ( lua_tinker::dobuffer(m_pLua, pBuffer, nLen) != 0 )
	{
		g_Log.Log(LogType::_ERROR, L"Cannot open lua file : %s\n" , wszScriptFileName);
		return false;
	}

	DefAllAPIFunc(m_pLua);

	

	return true;
}

void CDNScript::Destroy()
{
	if ( m_pLua )
		lua_close(m_pLua);



}
