//#ifndef __SHELLBROWSE_H__20020531
//#define __SHELLBROWSE_H__20020531
#pragma once

#include "Shlobj.h"

typedef BOOL (CALLBACK *FOLDERCONTENTPROC)(LPCSTR, LPITEMIDLIST, DWORD);

class CSHFolderDialog {
public:
	CSHFolderDialog() {}

	static int BrowseForFolder( CString& sPath, CString sTitle, HWND hParent=NULL, 
						UINT uiFlag=0, UINT uiSpecial=0, LPITEMIDLIST pidlRoot=NULL )
	{
		BROWSEINFO bi;
		ZeroMemory( &bi, sizeof( BROWSEINFO ) );
		bi.hwndOwner = (hParent == NULL) ? AfxGetMainWnd()->GetSafeHwnd() : hParent;
		bi.pidlRoot = pidlRoot;
		TCHAR pszDisplayName[_MAX_PATH];
		bi.pszDisplayName = pszDisplayName;
		bi.lpszTitle = sTitle;
		bi.ulFlags = uiFlag;
		bi.lParam = (LPARAM)(LPCTSTR)sPath; // set lParam to point to path
		bi.lpfn = BrowseCallbackProc;	// set the callback procedure
		LPITEMIDLIST pidl=NULL;
		
		if( uiSpecial )
		{
			SHGetSpecialFolderLocation( NULL, uiSpecial, &pidl );
			bi.pidlRoot = pidl;
		}
		else
			bi.pidlRoot = pidlRoot;

		LPITEMIDLIST pidlFolder = SHBrowseForFolder( &bi );
		TCHAR szPath[_MAX_PATH] = "";
		int nRet = IDCANCEL;
		if( pidlFolder && SHGetPathFromIDList( pidlFolder, szPath ) )
		{
			sPath.Format( "%s", szPath );
			nRet = IDOK;
		}

		LPMALLOC pMalloc = NULL;
		SHGetMalloc( &pMalloc );
		pMalloc->Free( pidl );
		pMalloc->Free( pidlFolder );
		pMalloc->Release();

		return nRet;
	}

private:
	// This is the default callback procedure for the SHBrowseForFolder function.
	// It will set the current selection to the directory specified in the edit control
	static int CALLBACK BrowseCallbackProc( HWND hWnd, UINT uMsg, LPARAM lParam, LPARAM dwData )
	{
		switch( uMsg )
		{
		case BFFM_INITIALIZED:
			{
				//캡션에서 Help 삭제
				DWORD dwStyle = GetWindowLong( hWnd, GWL_EXSTYLE );
				SetWindowLong( hWnd, GWL_EXSTYLE, dwStyle & ~WS_EX_CONTEXTHELP );

				//지정된 폴더가 선택되도록 한다.
				SendMessage( hWnd, BFFM_SETSELECTION, TRUE, dwData );

				//대화상자가 중앙에 오도록 한다.
				RECT rc;
				GetClientRect( hWnd, &rc );
				SetWindowPos( hWnd, NULL,
					(GetSystemMetrics(SM_CXSCREEN) - (rc.right - rc.left)) / 2,
					(GetSystemMetrics(SM_CYSCREEN) - (rc.bottom - rc.top)) / 2,
					0, 0, SWP_NOZORDER | SWP_NOSIZE );

				//Status text에 3D 테두리 첨가
				HWND hwndLabel = GetDlgItem( hWnd, 0x3743 );
				if( IsWindow( hwndLabel ) )
				{
					TCHAR szClass[MAX_PATH] = {0};
					GetClassName( hwndLabel, szClass, MAX_PATH );
					if( lstrcmpi( szClass, __TEXT( "static" ) ) )
						break;
				}
				else
					break;

				dwStyle = GetWindowLong( hwndLabel, GWL_EXSTYLE );
				SetWindowLong( hwndLabel, GWL_EXSTYLE, dwStyle | WS_EX_STATICEDGE );
				SetWindowPos( hwndLabel, NULL, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_DRAWFRAME );
			}
			break;
		case BFFM_SELCHANGED:
			{
				TCHAR szText[MAX_PATH] = {0};
				SHGetPathFromIDList( reinterpret_cast<LPITEMIDLIST>(lParam), szText );
				SendMessage( hWnd, BFFM_SETSTATUSTEXT, 0, reinterpret_cast<LPARAM>(szText) );
			}
			break;
		case BFFM_VALIDATEFAILED:
			break;
		}
		return 0;
	}
};

//#endif //__SHELLBROWSE_H__20020531