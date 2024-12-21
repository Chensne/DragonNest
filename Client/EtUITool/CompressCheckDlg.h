#pragma once
#include "afxdlgs.h"

class CCompressCheckDlg : public CFileDialog
{
	DECLARE_DYNAMIC(CCompressCheckDlg)

public:
	CCompressCheckDlg(BOOL bOpenFileDialog, // FileOpen�� TRUE, FileSaveAs�� FALSE�Դϴ�.
		LPCTSTR lpszDefExt = NULL,
		LPCTSTR lpszFileName = NULL,
		DWORD dwFlags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		LPCTSTR lpszFilter = NULL,
		CWnd* pParentWnd = NULL);
	~CCompressCheckDlg(void);

protected:

public:
	int m_nDoNotCompressTexture;

protected:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnBnClickedCompressCheck();
};
