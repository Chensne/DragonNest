#pragma once

#include <windows.h>

//#include "../CommonSet/STLCommon.h"
#include <list>

extern "C" 
{
#include "Lua.h"
#include "LuaLib.h"
#include "lauxlib.h"
};


//--------------------------------------------------------------------------//
// Class Name: CLuaDebugger
//
// Author : 책 쓴 사람 + 한기가 약간 수정
//
// Desc: 루아 디버그 커맨드 창을 띄운다.
//--------------------------------------------------------------------------//
class CLuaDebugger  
{
public:
	static HWND StartConsole(HINSTANCE hInstance, lua_State *pScriptContext);
	static void StopConsole();
	static void Write(const char *pString);

	static LRESULT WINAPI MsgProc( HWND hWnd, unsigned uMsg, WPARAM wParam, LPARAM lParam);
	static LRESULT CALLBACK SubclassInputEditProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	static HWND	GetConsoleHWND( void ) {return m_hWnd;};

	bool	ConsoleReady(void)	{return m_hWnd!= NULL;}

private:
	CLuaDebugger();
	virtual ~CLuaDebugger();

	void	Init(HINSTANCE hInstance);
	void	ResizeControls(void);
	void	AdjustScrollBar(void);
	void	Paint(HDC hDC);


private:
	static volatile bool	m_bWinIsActive;
	static volatile HWND m_hWnd;
	static volatile HWND m_hEditControl;
	static char m_CommandBuffer[4096];

	std::list<std::string> m_stringList;
	HINSTANCE m_hInstance;

	lua_State* m_pScriptContext;

	int m_ScrollyPos;
	int m_textAreaHeight;
};