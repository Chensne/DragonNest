#include "StdAfx.h"
#include <windows.h>
#include ".\luadebugger.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif


// 경고가 싫어. -_-
#pragma warning( disable : 4311 ) // 4 단계 경고
#pragma warning( disable : 4312 ) // 4 단계 경고


luaL_reg DebugGlue[];

CLuaDebugger *g_Console = NULL;


volatile bool CLuaDebugger::m_bWinIsActive = true;
volatile HWND CLuaDebugger::m_hWnd = NULL;
volatile HWND CLuaDebugger::m_hEditControl = NULL;

char CLuaDebugger::m_CommandBuffer[4096];

WNDPROC lpfnInputEdit;  // Storage for subclassed edit control 
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

HWND CLuaDebugger::StartConsole(HINSTANCE hInstance, lua_State *pScriptContext)
{
	if(!g_Console)
	{
		g_Console = new CLuaDebugger();
	}

	if(!m_hWnd)
		g_Console->Init(hInstance);

	g_Console->m_pScriptContext = pScriptContext;
	// init the glue functions required for the debug window
	for(int i=0; DebugGlue[i].name; i++)
	{
		lua_register(g_Console->m_pScriptContext, DebugGlue[i].name, DebugGlue[i].func);
	}

	return (m_hWnd);
}

void CLuaDebugger::StopConsole()
{
	if(g_Console)
	{
		delete g_Console;
		g_Console = NULL;
	}
}


CLuaDebugger::CLuaDebugger()
{
	m_hWnd = NULL;
	memset(m_CommandBuffer, 0, 4096);

}

CLuaDebugger::~CLuaDebugger()
{
	if( m_hWnd )
	{
		CloseWindow(m_hWnd);
		m_hWnd = NULL;
	}
}

void CLuaDebugger::AdjustScrollBar(void)
{
	SCROLLINFO si;

	si.cbSize = sizeof(si); 
	si.fMask  = SIF_RANGE | SIF_PAGE | SIF_POS; 
	si.nMin   = 0; 
	si.nMax   = (int)m_stringList.size(); 
	si.nPage  = m_textAreaHeight; 
	si.nPos   = (int)m_stringList.size() - m_ScrollyPos; 
	SetScrollInfo(m_hWnd, SB_VERT, &si, TRUE); 
}


void CLuaDebugger::ResizeControls(void)
{
	RECT r;

	GetClientRect(m_hWnd, &r);
	m_textAreaHeight = (r.bottom - r.top) / 16;

	SetWindowPos(m_hEditControl, HWND_TOP, r.left + 2, r.bottom - 18, r.right - r.left - 4, 16, SWP_NOZORDER);

	AdjustScrollBar();
	InvalidateRect(m_hWnd, NULL, TRUE);
}

void CLuaDebugger::Paint(HDC hDC)
{
	SetTextColor(hDC, RGB(255,255,255));
	SetBkColor(hDC, RGB(0,0,0));

	RECT r;

	GetClientRect(m_hWnd, &r);

	int x = 2;
	int y = r.bottom - 40;

	std::list<std::string>::iterator it = m_stringList.begin();
	int skip = m_ScrollyPos;
	while(skip)
	{
		++it;
		--skip;
	}

	while(it != m_stringList.end())
	{
		TextOutA(hDC, x, y, (*it).c_str(), (int)strlen((*it).c_str()));
		y -= 16;
		++it;
	}
}


LRESULT WINAPI CLuaDebugger::MsgProc( HWND hWnd, unsigned uMsg, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;

	switch (uMsg)
	{
	case WM_ACTIVATEAPP:
		m_bWinIsActive = (wParam != 0);
		return 0L;

	case WM_ACTIVATE:
		// Pause if minimized or not the top window
		m_bWinIsActive = (wParam == WA_ACTIVE) || (wParam == WA_CLICKACTIVE);
		return 0L;

	case WM_DESTROY:
		m_bWinIsActive = false;
		m_hWnd = NULL;
		break;

	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		g_Console->Paint(hdc);
		EndPaint(hWnd, &ps);
		break;

	case WM_CHAR:
		break;

	case WM_LBUTTONDOWN:
		break;

	case WM_RBUTTONDOWN:
		break;

	case WM_LBUTTONUP:
		break;

	case WM_RBUTTONUP:
		break;

	case WM_MOUSEMOVE:
		break;

	case WM_COMMAND:
		break;

	case WM_SIZING:
	case WM_SIZE:
		g_Console->ResizeControls();
		break;

	case WM_SETCURSOR:
		SetCursor(LoadCursor(NULL, IDC_ARROW));
		ShowCursor(TRUE);
		break;

	case WM_VSCROLL: 
		switch(LOWORD (wParam)) 
		{ 
			// User clicked the shaft above the scroll box. 

		case SB_PAGEUP: 
			g_Console->m_ScrollyPos = min((int) (g_Console->m_ScrollyPos+g_Console->m_textAreaHeight), (int) (g_Console->m_stringList.size() - g_Console->m_textAreaHeight)+1);
			//yInc = min(-1, -yClient / yChar); 
			break; 

			// User clicked the shaft below the scroll box. 

		case SB_PAGEDOWN: 
			// yInc = max(1, yClient / yChar); 
			break; 

			// User clicked the top arrow. 

		case SB_LINEUP: 
			g_Console->m_ScrollyPos = min(g_Console->m_ScrollyPos+1, (int) (g_Console->m_stringList.size() - g_Console->m_textAreaHeight)+1);
			break; 

			// User clicked the bottom arrow. 

		case SB_LINEDOWN: 
			//yInc = 1; 
			g_Console->m_ScrollyPos = max(g_Console->m_ScrollyPos-1, 0);
			break; 

			// User dragged the scroll box. 

		case SB_THUMBTRACK: 
			//yInc = HIWORD(wParam) - yPos; 
			break; 

		default: 
			//yInc = 0;
			break;

		} 
		{
			SCROLLINFO si;
			si.cbSize = sizeof(si); 
			si.fMask  = SIF_POS; 
			si.nPos   = (int)g_Console->m_stringList.size() - g_Console->m_ScrollyPos; 
			SetScrollInfo(hWnd, SB_VERT, &si, TRUE); 
		}
		InvalidateRect(m_hWnd, NULL, TRUE);

		break;

		case WM_USER:
			// command ready from edit control
			// string should be in m_CommandBuffer
			Write(m_CommandBuffer);
			if (0 != luaL_loadbuffer(g_Console->m_pScriptContext, m_CommandBuffer, strlen(m_CommandBuffer), NULL))
			{
				Write("Error loading Command\n");
			}
			if (0 != lua_pcall(g_Console->m_pScriptContext, 0, LUA_MULTRET, 0))
			{
				Write("Error in Command\n");
				Write(luaL_checkstring(g_Console->m_pScriptContext, -1));
			}
			// clear buffer when done processing
			memset(m_CommandBuffer, 0, 4096);
			break;
	}

	return DefWindowProc( hWnd, uMsg, wParam, lParam );
}


void CLuaDebugger::Write(const char *pString)
{
	if(g_Console)
	{
		if(m_hWnd)
		{
			//remove any linefeed chars (look goofy in log)
			char *buf = new char[strlen(pString)+1];
			int indx=0;
			for(int i=0; i<(int) strlen(pString); i++)
			{
				if(pString[i] != 10)
				{
					buf[indx++] = pString[i];
				}
				else
				{
					buf[indx] = 0;
					g_Console->m_stringList.push_front(buf);
					indx = 0;
				}


			}
			if(indx > 0)
			{
				buf[indx] = 0;
				g_Console->m_stringList.push_front(buf);
			}
			InvalidateRect(m_hWnd, NULL, TRUE);
			delete [] buf;
		}
		g_Console->AdjustScrollBar();
	}
}


LRESULT CALLBACK CLuaDebugger::SubclassInputEditProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) 
{     
	switch(message)     
	{         
	case WM_CHAR:              // Found a RETURN keystroke!             
		if ((TCHAR) wParam == VK_RETURN)             
		{ 
			// get the command string
			long lSizeofString;                  
			// Get the size of the string
			lSizeofString = (long)SendMessage(hWnd, WM_GETTEXTLENGTH, 0, 0);

			// Get the string                 
			SendMessage(hWnd, WM_GETTEXT, lSizeofString + 1, (LPARAM) m_CommandBuffer); 

			// send message to parent that command was entered
			SendMessage(m_hWnd, WM_USER, 0, lSizeofString);

			// clear the edit string
			SendMessage(hWnd, WM_SETTEXT, 0, (long)""); 
			return 1;
		}
	}
	return CallWindowProc(lpfnInputEdit, hWnd, message, wParam, lParam); 
}

void CLuaDebugger::Init(HINSTANCE hInstance)
{
	m_hInstance = hInstance;
	m_ScrollyPos = 0;
	// create application handler and link to our WindowProc
	WNDCLASSA wc;

	// Set up and register window class
	wc.style = 0;
	wc.lpfnWndProc = (WNDPROC) MsgProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = sizeof(DWORD);
	wc.hInstance = m_hInstance;
	wc.hIcon = NULL;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH) GetStockObject(BLACK_BRUSH);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = "Debugger";
#if 0
	if (!RegisterClass(&wc))
		ThrowError();
#else
	RegisterClassA(&wc);
#endif

	m_hWnd = CreateWindowA("Debugger", // class
		"LUA Debugger", // caption
		WS_OVERLAPPEDWINDOW, // style 
		0, // left
		0, // top
		640, // width
		480, // height
		NULL, // parent window
		NULL, // menu 
		m_hInstance, // instance
		NULL); // parms

	ShowWindow(m_hWnd, SW_SHOW);
	UpdateWindow(m_hWnd);
	SetFocus(m_hWnd);

	m_hEditControl = CreateWindowA("EDIT", // class
		"", // caption
		ES_LEFT | WS_CHILD, // style 
		2, // left
		404, // top
		228, // width
		16, // height
		m_hWnd, // parent window
		(HMENU)0xa7, // menu 
		m_hInstance, // instance
		NULL); // parms

	ShowWindow(m_hEditControl, SW_SHOW);
	UpdateWindow(m_hEditControl);
	SetFocus(m_hEditControl);
	m_ScrollyPos = 0;

#if defined(WIN64)
	lpfnInputEdit = (WNDPROC) SetWindowLong(m_hEditControl, GWLP_WNDPROC, (long) SubclassInputEditProc); 
#else
	lpfnInputEdit = (WNDPROC) SetWindowLong(m_hEditControl, GWL_WNDPROC, (long) SubclassInputEditProc); 
#endif
	g_Console->ResizeControls();


}

static int Debug_Print(lua_State *L) 
{
#ifdef _DEBUG
	int n = lua_gettop(L);  /* number of arguments */
	int i;
	lua_getglobal(L, "tostring");
	for (i=1; i<=n; i++) {
		const char *s;
		lua_pushvalue(L, -1);  /* function to be called */
		lua_pushvalue(L, i);   /* value to print */
		lua_call(L, 1, 1);
		s = lua_tostring(L, -1);  /* get result */
		if (s == NULL)
			return luaL_error(L, "`tostring' must return a string to `print'");
		if (i>1) CLuaDebugger::Write("\t");
		CLuaDebugger::Write(s);
		lua_pop(L, 1);  /* pop result */
	}
	CLuaDebugger::Write("\n");
#endif
	return 0;
}

luaL_reg DebugGlue[] = 
{
	{"Print",					Debug_Print},
	{"print",					Debug_Print},

	{NULL, NULL}
};
