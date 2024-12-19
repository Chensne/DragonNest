// DnManualUpdater.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CDnManualUpdaterApp:
// See DnManualUpdater.cpp for the implementation of this class
//

class CDnManualUpdaterApp : public CWinApp
{
public:
	CDnManualUpdaterApp();

// Overrides
	public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
};

extern CDnManualUpdaterApp theApp;