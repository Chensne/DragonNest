#pragma once
#include "Task.h"
#include "Singleton.h"
#include "InputReceiver.h"

extern enum DebugCmdLineIndexEnum;

class CDnDebugTask : public CTask, public CSingleton<CDnDebugTask>, public CInputReceiver
{
public:
	CDnDebugTask();
	virtual ~CDnDebugTask();

	struct CmdLineDefineStruct {
		DebugCmdLineIndexEnum Index;
		TCHAR *szString;
		TCHAR *szDescription;
	};

protected:
	HWND m_hWnd;
	static CDnDebugTask *s_pThis;
	std::basic_string<TCHAR> m_szString;
	bool m_bShow;
	TCHAR m_szParam[256];

	static bool s_bDialogMode;
	bool m_bScrollLock;
	std::basic_string<TCHAR> m_szAccumulationString;
	std::basic_string<TCHAR> m_szCurString;
	int m_nCursorPos;
	int m_nVertScrollPos;

	int m_nMaxLineCount;
	int m_nTotalLineCount;
	int m_nStartLength;
	int m_nEndLength;

	float m_fVertVelocity;
	float m_fVertVelocityTemp;

	HINSTANCE m_hLibrary;

	static INT_PTR CALLBACK DialogProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);

	void CheckDebugCommand( TCHAR *szStr );
	void CommandDebug( DebugCmdLineIndexEnum Index );

	void CalcAccumulationValue();

public:
	static void OutputScriptString( const char *szString, ... );

	virtual void Process( LOCAL_TIME LocalTime, float fDelta );
	virtual void OnInputReceive( int nReceiverState, LOCAL_TIME LocalTime );

	void Show( bool bShow );
	void OutputScript( char *szString, ... );
};