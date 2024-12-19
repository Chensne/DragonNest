// EmblemTest.h : main header file for the EmblemTest application
//
#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"       // main symbols


// CEmblemTestApp:
// See EmblemTest.cpp for the implementation of this class
//

class CEmblemTestApp : public CWinApp
{
public:
	CEmblemTestApp();


// Overrides
public:
	virtual BOOL InitInstance();

// Implementation
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CEmblemTestApp theApp;