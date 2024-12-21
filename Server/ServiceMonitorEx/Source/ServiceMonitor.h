// ServiceMonitor.h : main header file for the ServiceMonitor application
//
#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"       // main symbols

#include "Define.h"
#include "CriticalSection.hpp"

class CMainFrame;
class CConnectionDialog;
class CBasePartitionView;
class CBaseMonitorView;
class CBaseInformationView;
class CBaseErrorLogView;
class CNetSession;
class CChangePasswordDialog;

// SERVICEMONITORCONFIG

typedef struct SERVICEMONITORCONFIG
{

public:
	SERVICEMONITORCONFIG();

	VOID Reset();

public:
	UINT m_ServiceManagerIpAddr;
	WORD m_ServiceManagerPortNo;

} *LPSERVICEMONITORCONFIG, * const LPCSERVICEMONITORCONFIG;


// CServiceMonitorApp:
// See ServiceMonitor.cpp for the implementation of this class
//

class CServiceMonitorApp : public CWinApp
{
public:
	CServiceMonitorApp();
	~CServiceMonitorApp();

public:
	void SetAppDoc(CDocument* pDoc);
	CDocument* GetAppDoc();

	void SetMainFrame (CMainFrame* pWnd);
	CMainFrame* GetMainFrame ();

	CConnectionDialog& GetConnectionDialog ();

	void SetPartitionView(ULONG nConID, CBasePartitionView* pView);
	CBasePartitionView* GetPartitionView (ULONG nConID);
	CBaseMonitorView* GetMoniterView (ULONG nConID);
	CBaseInformationView* GetInformationView (ULONG nConID);
	CBaseErrorLogView* GetErrorLogView (ULONG nConID);

	void ForceAllTabViewActivate();

	void ResetPartitionIndex();


private:
	CDocument* m_AppDoc;

	std::map <ULONG, CBasePartitionView*> m_MapPartitionView;
	CMainFrame* m_MainWnd;


// Overrides
public:
	virtual BOOL InitInstance();

// Implementation
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
	virtual int ExitInstance();

	mutable CCriticalSection	m_Lock;
};

extern CServiceMonitorApp theApp;
