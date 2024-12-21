#include "stdafx.h"
#include "LuaStateManager.h"
#include <utility>

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

LuaStateManager::LuaStateManager()
{


}

LuaStateManager::~LuaStateManager()
{
	CloseAllState();
}

bool LuaStateManager::CreateLuaState(int nMakeCount)
{
	if (nMakeCount <= 0)
		return false;

	bool bCheck = true;
	for (int i = 0; i < nMakeCount; i++)
	{
		TLUA_STATE lua;
		memset(&lua, 0, sizeof(TLUA_STATE));
		lua.pState = lua_open();
		luaL_openlibs(lua.pState);

		m_vLuaState.push_back(lua);
	}
	return bCheck;
}

lua_State * LuaStateManager::OpenStateByIndex(int nIdx)
{
	if (nIdx >= (int)m_vLuaState.size() || nIdx < 0) return NULL;
	return m_vLuaState[nIdx].pState;
}

lua_State * LuaStateManager::OpenState(DWORD nCurrentThreadID/* = GetCurrentThreadId()*/)
{
	std::vector <TLUA_STATE>::iterator ii;
	for (ii = m_vLuaState.begin(); ii != m_vLuaState.end(); ii++)
	{
		if ((*ii).nAssignedThreadID == nCurrentThreadID)
			return (*ii).pState;
	}

	for (ii = m_vLuaState.begin(); ii != m_vLuaState.end(); ii++)
	{
		if ((*ii).nAssignedThreadID <= 0)
		{
			(*ii).nAssignedThreadID = nCurrentThreadID;
			return (*ii).pState;
		}
	}
	return NULL;
}

void LuaStateManager::CloseAllState()
{
	std::vector <TLUA_STATE>::iterator ii;
	for (ii = m_vLuaState.begin(); ii != m_vLuaState.end(); ii++)
	{
		lua_State* pLua = (*ii).pState;
		lua_close(pLua);
	}

	m_vLuaState.clear();
}
