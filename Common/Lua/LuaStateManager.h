#pragma once

class LuaStateManager
{
public:
	LuaStateManager();
	virtual ~LuaStateManager();

	bool CreateLuaState(int nMakeCount);
	lua_State * OpenStateByIndex(int nIdx);
	lua_State * OpenState(DWORD nCurrentThreadID = GetCurrentThreadId());	
	void CloseAllState();

protected:

	struct TLUA_STATE
	{
		bool bLoaded;
		DWORD nAssignedThreadID;
		lua_State * pState;
	};
	std::vector <TLUA_STATE> m_vLuaState;
};

