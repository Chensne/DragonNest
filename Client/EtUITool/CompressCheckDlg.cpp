#include "StdAfx.h"
#include "resource.h"
#include "CompressCheckDlg.h"

IMPLEMENT_DYNAMIC(CCompressCheckDlg, CFileDialog)

CCompressCheckDlg::CCompressCheckDlg(BOOL bOpenFileDialog, LPCTSTR lpszDefExt, LPCTSTR lpszFileName,
		DWORD dwFlags, LPCTSTR lpszFilter, CWnd* pParentWnd) :
		CFileDialog(bOpenFileDialog, lpszDefExt, lpszFileName, dwFlags, lpszFilter, pParentWnd)
{
	m_ofn.Flags = dwFlags | OFN_EXPLORER | OFN_ENABLETEMPLATE | OFN_ENABLEHOOK;
	m_ofn.lpTemplateName = MAKEINTRESOURCE(IDD_COMPRESSCHECKDLG);

	m_nDoNotCompressTexture = 0;
}

CCompressCheckDlg::~CCompressCheckDlg(void)
{
}

BEGIN_MESSAGE_MAP(CCompressCheckDlg, CFileDialog)
	ON_BN_CLICKED(IDC_CHECK1, &CCompressCheckDlg::OnBnClickedCompressCheck)
END_MESSAGE_MAP()

void CCompressCheckDlg::OnBnClickedCompressCheck()
{
	m_nDoNotCompressTexture = ((CButton*)GetDlgItem( IDC_CHECK1 ))->GetCheck();
}
