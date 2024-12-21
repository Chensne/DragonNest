#pragma once

#include <windows.h>
#include <string>
using namespace std;

extern "C" 
{
#include "Lua.h"
#include "LuaLib.h"
#include "lauxlib.h"
};

#define LUA_GLUE extern "C" int 
extern "C" 
{
typedef int (*LuaFunctionType)(struct lua_State* pLuaState);
};


// 싱글톤 편하게 쓰자~
#define LUA_DELEGATE	LuaDelegate_Singleton()


//--------------------------------------------------------------------------//
// Class Name: CLuaDelegate
//
// Author : 정한기
//
// Desc: 루아 인터페이스 대리자~~		  
//--------------------------------------------------------------------------//
class CLuaDelegate
{
private:
	lua_State*		m_pLuaState;
	void (*m_pErrorHandler)( const char* pError );

	// 싱글톤이다.
	CLuaDelegate(void);

	// 텍스트 파일을 못 찾을 경우엔 바이너리 파일(lub)로 찾아본다.
	void			_ConvertProperFileExt( IN OUT string& strFileName );

public:
	~CLuaDelegate(void);

	// 디버그일 때만 허용.

//#if defined (_DEBUG) | (DEBUG)
#if defined (_DEBUG)
	void StartDebugConsole( HINSTANCE hInst );
#endif

	bool RunScript( const char* pFileName );
	bool RunString( const char* pCommand );
	const char* GetErrorString( void );
	bool AddFunction( const char* pFunctionName, LuaFunctionType pFunc );
	const char* GetStringArgument( int iNum, const char* pDefault = NULL );
	double GetNumberArgument( int iNum, double dDefault = 0.0 );
	void PushString( const char* pString );
	void PushNumber( double dValue );

	// 글로벌 변수를 얻어오기
	double GetNumber( const char* pVarName );
	const char* GetString( const char* pVarName );

	// 글로벌 변수를 셋팅
	void SetNumber( const char* pVarName, double dNumber );
	void SetString( const char* pVarName, const char* pString );

	// 테이블 얻어오기.
	int GetTable( const char* pTableName );

	// 테이블에 키&값 페어를 넣는다.
	void AddToTable( int iTableIndex, const char* pStrKey, const char* pStrValue );
	void AddToTable( int iTableIndex, const char* pStrKey, lua_Number NumericValue );

	// 테이블은 스택의 Top 에 있다고 가정한다.
	bool IsExistField( const char* pStrKey );
	void GetField( const char* pStrKey, /*IN OUT*/ string& strValue  );
	lua_Number GetField( const char* pStrKey );

	void SetErrorHandler( void(*pErrorHandler)(const char* pError) );

	lua_State* GetLuaState( void );

	friend CLuaDelegate& LuaDelegate_Singleton( void );
};
