/////////////////////////////////////////////////////////////
//	
//	Author	 :	Sami (M.ALSAMSAM), ittiger@ittiger.net
//
//	Filename :	ColorTree.h
//
//	http	 :	www.ittiger.net
//
//////////////////////////////////////////////////////////////
#if !defined(AFX_COLORTREE_H__B7065043_5903_4486_BE69_D353B07E2FBD__INCLUDED_)
#define AFX_COLORTREE_H__B7065043_5903_4486_BE69_D353B07E2FBD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
class CColorTreeApp : public CWinApp
{
public:
	CColorTreeApp();

	//{{AFX_VIRTUAL(CColorTreeApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CColorTreeApp)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
#endif // !defined(AFX_COLORTREE_H__B7065043_5903_4486_BE69_D353B07E2FBD__INCLUDED_)
