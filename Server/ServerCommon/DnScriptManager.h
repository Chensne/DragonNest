#pragma once

#include "LuaStateManager.h"
#if defined( _GAMESERVER )
#include "DNGameRoom.h"
#endif // #if defined( _GAMESERVER )

class CStringSet;

// 각 쓰레드 별로 lua_state 를 생성/관리 하기 위한 클래스 이다.
// 만약 쓰레드와 별개로 lua_state 를 생성 하고 싶다면 
// DnScriptManager 의 인스턴스를 별개로 생성하여 사용 하도록 한다.
class DnScriptManager : public LuaStateManager
{
public:
	DnScriptManager();
	virtual ~DnScriptManager();

	bool LoadScript( const char* szLuaFile, bool bIsCheckServer, int nLuaIndex, bool bQuestScript );

#if defined( _GAMESERVER )

	template<typename RVal, typename T1, typename T2, typename T3>
	RVal call(lua_State* L, const char* name, T1 arg1, T2 arg2, T3 arg3)
	{
#if defined( PRE_QUESTSCRIPT_LOG )
		CScopeScriptLog Log( arg1->m_iMapIdx, name );
#endif // #if defined( PRE_QUESTSCRIPT_LOG )
		lua_tinker::call<RVal>( L, name, arg1, arg2, arg3 );
	}

	template<typename RVal, typename T1, typename T2, typename T3, typename T4>
	RVal call(lua_State* L, const char* name, T1 arg1, T2 arg2, T3 arg3, T4 arg4)
	{
#if defined( PRE_QUESTSCRIPT_LOG )
		CScopeScriptLog Log( arg1->m_iMapIdx, name );
#endif // #if defined( PRE_QUESTSCRIPT_LOG )
		return lua_tinker::call<RVal>( L, name, arg1, arg2, arg3, arg4 );
	}

	template<typename RVal, typename T1, typename T2, typename T3, typename T4, typename T5>
	RVal call(lua_State* L, const char* name, T1 arg1, T2 arg2, T3 arg3, T4 arg4, T5 arg5)
	{
#if defined( PRE_QUESTSCRIPT_LOG )
		CScopeScriptLog Log( arg1->m_iMapIdx, name );
#endif // #if defined( PRE_QUESTSCRIPT_LOG )
		lua_tinker::call<RVal>( L, name, arg1, arg2, arg3, arg4, arg5 );
	}

	template<typename RVal, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6>
	RVal call(lua_State* L, const char* name, T1 arg1, T2 arg2, T3 arg3, T4 arg4, T5 arg5, T6 arg6)
	{
#if defined( PRE_QUESTSCRIPT_LOG )
		CScopeScriptLog Log( arg1->m_iMapIdx, name );
#endif // #if defined( PRE_QUESTSCRIPT_LOG )
		lua_tinker::call<RVal>( L, name, arg1, arg2, arg3, arg4, arg5, arg6 );
	}

	template<typename RVal, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7>
	RVal call(lua_State* L, const char* name, T1 arg1, T2 arg2, T3 arg3, T4 arg4, T5 arg5, T6 arg6, T7 arg7)
	{
#if defined( PRE_QUESTSCRIPT_LOG )
		CScopeScriptLog Log( arg1->m_iMapIdx, name );
#endif // #if defined( PRE_QUESTSCRIPT_LOG )
		lua_tinker::call<RVal>( L, name, arg1, arg2, arg3, arg4, arg5, arg6, arg7 );
	}

#endif // #if defined( _GAMESERVER )

private:
	void RegistLuaAPI();
	bool IsValidLuaAPI(LPCSTR pApiName);
	bool CheckLuaAPI(CStringSet& LuaChunkSub);
	void CheckRemoteComplete(const char* szLuaFile, CStringSet& LuaChunkSub);

private:
	std::map<std::string, int> m_mLuaAPI;
};

extern DnScriptManager* g_pNpcQuestScriptManager;

