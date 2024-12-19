#include "stdafx.h"
#include ".\luadelegate.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif



// 디버그 빌드 시에만 디버그 콘솔 창을 띄우도록 한다.
#if defined	(_DEBUG)/* | (DEBUG)*/
#include "LuaDebugger.h"
#endif


CLuaDelegate& LuaDelegate_Singleton( void )
{
	static CLuaDelegate LuaDelegate;

	return LuaDelegate;
}


CLuaDelegate::CLuaDelegate(void) : m_pErrorHandler( NULL ),
								   m_pLuaState( NULL )
{
	// Lua State 를 연다.
	m_pLuaState = lua_open();
	luaL_openlibs( m_pLuaState );
	//luaopen_base( m_pLuaState );
	//luaopen_io( m_pLuaState );
	//luaopen_string( m_pLuaState );
	//luaopen_math( m_pLuaState );
	//luaopen_debug( m_pLuaState );
	//luaopen_table( m_pLuaState );
}

CLuaDelegate::~CLuaDelegate(void)
{
	// 디버그 콘솔 종료
#if defined (_DEBUG)/* | (DEBUG)*/
	CLuaDebugger::StopConsole();
#endif

	// Lua State 를 닫는다.
	if( m_pLuaState )
		lua_close( m_pLuaState );
}


#if defined (_DEBUG)/* | (DEBUG)*/
void DefaultErrorHandler(const char *pError)
{
	CLuaDebugger::Write(pError);
	OutputDebugStringA( pError );
	BringWindowToTop( CLuaDebugger::GetConsoleHWND() );
}

void CLuaDelegate::StartDebugConsole( HINSTANCE hInst )
{
	CLuaDebugger::StartConsole( hInst, m_pLuaState );
	m_pErrorHandler = DefaultErrorHandler;
}
#endif



void CLuaDelegate::_ConvertProperFileExt( IN OUT string& strFileName )
{
	FILE* pf = NULL;
	pf = fopen( strFileName.c_str(), "r" );

	// 파일을 찾았을 경우.
	if( NULL != pf )
	{
		fclose( pf );
		return;
	}
	else
	// 텍스트 파일로 안 열려서 못 찾았을 경우.
	{
		strFileName = strFileName.substr( 0, strFileName.find_first_of( '.', 0 ) );
		strFileName.append( ".lub" );
		pf = fopen( strFileName.c_str(), "rb" );
		if( NULL != pf )
		{
			fclose( pf );
			return;
		}
	}

	// 결국 파일 못 찾았음. 에라가 뜰 것임
	return;
}


bool CLuaDelegate::RunScript( const char* pFileName )
{
	bool bResult = true;

	string strFileName( pFileName );
	_ConvertProperFileExt( strFileName );

	if( 0 != luaL_loadfile( m_pLuaState, strFileName.c_str() ) )
	{
		if( m_pErrorHandler )
		{
			char buf[ 256 ];
			sprintf( buf, "Lua Error - Script Load\nScript Name:%s\nError Message:%s\n", strFileName.c_str(), 
					 luaL_checkstring( m_pLuaState, -1 ) );

			m_pErrorHandler( buf );
		}
		
		bResult = false;
	}
	
	if( 0 != lua_pcall( m_pLuaState, 0, LUA_MULTRET, 0 ) )
	{
		if( m_pErrorHandler )
		{
			char buf[ 256 ];
			sprintf( buf, "Lua Error - Script Run\nScript Name:%s\nError Message:%s\n", strFileName.c_str(),
					 luaL_checkstring( m_pLuaState, -1 ) );
			m_pErrorHandler( buf );
		}

		bResult = false;
	}

	return bResult;
}

bool CLuaDelegate::RunString( const char* pCommand )
{
	bool bResult = true;

	if( 0 != luaL_loadbuffer( m_pLuaState, pCommand, strlen(pCommand), NULL ) )
	{
		char buf[ 256 ];
		sprintf_s( buf, "Lua Error - Error Message:%s\n", luaL_checkstring( m_pLuaState, -1 ) );
		OutputDebugStringA( buf );
		if( m_pErrorHandler )
		{
			m_pErrorHandler( buf );
		}

		bResult = false;
	}
	
	if( 0 != lua_pcall( m_pLuaState, 0, LUA_MULTRET, 0 ) )
	{
		char buf[ 256 ];
		sprintf( buf, "Lua Error - Error Message:%s\n", luaL_checkstring( m_pLuaState, -1 ) );
		OutputDebugStringA( buf );
		if( m_pErrorHandler )
		{
			m_pErrorHandler( buf );
		}

		bResult = false;
	}

	return bResult;
}

const char* CLuaDelegate::GetErrorString( void )
{
	return luaL_checkstring( m_pLuaState, -1 );
}

bool CLuaDelegate::AddFunction( const char* pFunctionName, LuaFunctionType pFunc )
{
	lua_register( m_pLuaState, pFunctionName, pFunc );

	return true;
}

const char* CLuaDelegate::GetStringArgument( int iNum, const char* pDefault )
{
	return luaL_optstring( m_pLuaState, iNum, pDefault );
}

double CLuaDelegate::GetNumberArgument( int iNum, double dDefault )
{
	return luaL_optnumber( m_pLuaState, iNum, dDefault );
}

void CLuaDelegate::PushString( const char* pString )
{
	lua_pushstring( m_pLuaState, pString );
}

void CLuaDelegate::PushNumber( double dValue )
{
	lua_pushnumber( m_pLuaState, dValue );
}

void CLuaDelegate::SetErrorHandler( void(*pErrorHandler)(const char* pError) )
{
	m_pErrorHandler = pErrorHandler;
}

lua_State* CLuaDelegate::GetLuaState( void )
{
	return m_pLuaState;
}


double CLuaDelegate::GetNumber( const char* pVarName )
{
	lua_pushstring( m_pLuaState, pVarName );
	lua_gettable( m_pLuaState, LUA_GLOBALSINDEX );

	double dResult = lua_tonumber( m_pLuaState, -1 );

	lua_pop( m_pLuaState, -1 );
	
	return dResult;
}


const char* CLuaDelegate::GetString( const char* pVarName )
{
	lua_pushstring( m_pLuaState, pVarName );
	lua_gettable( m_pLuaState, LUA_GLOBALSINDEX );

	const char* pResult =lua_tostring( m_pLuaState, -1 );

	lua_pop( m_pLuaState, -1 );

	return pResult;
}



void CLuaDelegate::SetNumber( const char* pVarName, double dNumber )
{
	lua_pushnumber( m_pLuaState, dNumber );
	lua_setglobal( m_pLuaState, pVarName );

	lua_pop( m_pLuaState, -1 );
}



void CLuaDelegate::SetString( const char* pVarName, const char* pString )
{
	lua_pushstring( m_pLuaState, pString );
	lua_setglobal( m_pLuaState, pVarName );

	lua_pop( m_pLuaState, -1 );
}



int CLuaDelegate::GetTable( const char* pTableName )
{
	lua_pushstring( m_pLuaState, pTableName );
	lua_gettable( m_pLuaState, LUA_GLOBALSINDEX );
	int	iTableIndex	= lua_gettop( m_pLuaState );
	lua_pushnil( m_pLuaState );

	return iTableIndex;
}



void CLuaDelegate::AddToTable( int iTableIndex, const char* pStrKey, const char* pStrValue )
{
	lua_pushstring( m_pLuaState, pStrKey );
	lua_pushstring( m_pLuaState, pStrValue );
	lua_settable( m_pLuaState, iTableIndex );
}



void CLuaDelegate::AddToTable( int iTableIndex, const char* pStrKey, lua_Number NumericValue )
{
	lua_pushstring( m_pLuaState, pStrKey );
	lua_pushnumber( m_pLuaState, NumericValue );
	lua_settable( m_pLuaState, iTableIndex );
}


bool CLuaDelegate::IsExistField( const char* pStrKey )
{
	bool bResult = false;

	lua_pushstring( m_pLuaState, pStrKey );
	lua_gettable( m_pLuaState, -2 );

	bResult = (false == lua_isnil( m_pLuaState, -1 ));

	lua_pop( m_pLuaState, 1 );
	
	return bResult;
}


void CLuaDelegate::GetField( const char* pStrKey, string& strValue )
{
	lua_pushstring( m_pLuaState, pStrKey );
	lua_gettable( m_pLuaState, -2 );

	if( lua_isstring( m_pLuaState, -1 ) )
	{
		const char* pStrValue = lua_tostring( m_pLuaState, -1 );
		strValue.assign( pStrValue );
	}

	lua_pop( m_pLuaState, 1 );
}



lua_Number CLuaDelegate::GetField( const char* pStrKey )
{
	lua_Number result = 0;

	lua_pushstring( m_pLuaState, pStrKey );
	lua_gettable( m_pLuaState, -2 );
	
	if( lua_isnumber( m_pLuaState, -1 ) )
	{
		result = lua_tonumber( m_pLuaState, -1 );
	}

	lua_pop( m_pLuaState, 1 );

	return result;
}