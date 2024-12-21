#pragma once

class LuaSetting
{
public:
	static bool	Create();
	static void	Destroy();
	static lua_State* GetLua(const char* szLuaFile);

private:

	static bool	_Init(const char* szLuaFile);

	typedef std::map<std::string, lua_State*>	LUA_MAP;
	typedef LUA_MAP::iterator					LUA_MAP_IT;

	static LUA_MAP	ms_LuaMap;

};
