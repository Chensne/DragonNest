#include "StdAFx.h"
#include "LuaSetting.h"
#include "lua_tinker.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

LuaSetting::LUA_MAP LuaSetting::ms_LuaMap;

const char* _gs_AIFile[] = { "BasiliskAI.lua" };

bool
LuaSetting::Create()
{
	Destroy();

	for ( int i = 0 ;  i < _countof(_gs_AIFile) ; i++ )
	{
		const char* szFileName = _gs_AIFile[i];
		lua_State* pLua =  lua_open();
		if ( pLua )
		{
			std::string szFullName;
			szFullName = CEtResourceMng::GetInstance().GetFullName( szFileName ).c_str();

			luaL_openlibs(pLua); 
			if ( lua_tinker::dofile(pLua, szFullName.c_str()) != 0 )
			{
				lua_close(pLua);
				return false;
			}

			OutputDebug("Load LuaFile %s \n", szFullName.c_str());

			std::string sz;
			sz = szFileName;
			std::transform(sz.begin(), sz.end(), sz.begin(), towlower); 

			ms_LuaMap.insert(std::pair<std::string, lua_State*>(std::string(sz),pLua));
		}
	}
	return true;

}

void
LuaSetting::Destroy()
{
	LUA_MAP_IT it = ms_LuaMap.begin();

	for ( it ; it != ms_LuaMap.end() ; ++it )
	{
		lua_State* pLua = it->second;
		if ( pLua )
			lua_close(pLua);
	}

	ms_LuaMap.clear();
}

lua_State*
LuaSetting::GetLua(const char* szLuaFile)
{
	std::string sz;
	sz = szLuaFile;
	std::transform(sz.begin(), sz.end(), sz.begin(), towlower); 

	LUA_MAP_IT it = ms_LuaMap.find(sz);
	if ( it != ms_LuaMap.end() )
	{
		return it->second;
	}
	else
	{
		lua_State* pLua =  lua_open();
		if ( pLua )
		{
			std::string szFullName;
			szFullName = CEtResourceMng::GetInstance().GetFullName( szLuaFile ).c_str();

			luaL_openlibs(pLua); 
			if ( lua_tinker::dofile(pLua, szFullName.c_str()) != 0 )
			{
				lua_close(pLua);
				return NULL;
			}
			OutputDebug("Load LuaFile %s \n", szFullName.c_str());

			std::string szName;
			szName = szLuaFile;
			std::transform(szName.begin(), szName.end(), szName.begin(), towlower); 

			ms_LuaMap.insert(std::pair<std::string, lua_State*>(szName,pLua));
			return pLua;
		}
	}

	return NULL;
}

