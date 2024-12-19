// SignalReportDlg.cpp : implementation file
//

#include "stdafx.h"
#include "EtActionTool.h"
#include "SignalReportDlg.h"
#include "GlobalValue.h"


// CSignalReportDlg dialog

IMPLEMENT_DYNAMIC(CSignalReportDlg, CXTResizeDialog)

CSignalReportDlg::CSignalReportDlg(CWnd* pParent /*=NULL*/)
	: CXTResizeDialog(CSignalReportDlg::IDD, pParent)
{

}

CSignalReportDlg::~CSignalReportDlg()
{
}

void CSignalReportDlg::DoDataExchange(CDataExchange* pDX)
{
	CXTResizeDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SPREADSHEET1, m_Sheet);
}


BEGIN_MESSAGE_MAP(CSignalReportDlg, CXTResizeDialog)
	ON_WM_SIZE()
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_SAVE, &CSignalReportDlg::OnBnClickedSave)
END_MESSAGE_MAP()


// CSignalReportDlg message handlers

void CSignalReportDlg::OnSize(UINT nType, int cx, int cy)
{
	CXTResizeDialog::OnSize(nType, cx, cy);

	// TODO: Add your message handler code here
	if( m_Sheet ) m_Sheet.MoveWindow( 0, 0, cx, cy - 50 );
}

BOOL CSignalReportDlg::OnInitDialog()
{
	CXTResizeDialog::OnInitDialog();

	// TODO:  Add extra initialization here
//	SetResize( IDC_SPREADSHEET1, SZ_TOP_LEFT, SZ_BOTTOM_RIGHT );
	SetResize( IDOK, SZ_BOTTOM_RIGHT, SZ_BOTTOM_RIGHT );
	SetResize( IDC_SAVE, SZ_BOTTOM_RIGHT, SZ_BOTTOM_RIGHT );
	LoadPlacement( _T("CSignalReportDlg") );


	CString szBuffer;
	CGlobalValue::GetInstance().CalcSignalReport( szBuffer );

	m_Sheet.put_CSVData( szBuffer );
	m_Sheet.put_AutoFit( TRUE );

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CSignalReportDlg::OnDestroy()
{
	CXTResizeDialog::OnDestroy();

	// TODO: Add your message handler code here
	SavePlacement( _T("CSignalReportDlg") );
}

void CSignalReportDlg::OnBnClickedSave()
{
	// TODO: Add your control notification handler code here
	TCHAR szFilter[] = _T( "CSV File (*.csv)|*.csv|All Files (*.*)|*.*||" );
	CFileDialog dlg( FALSE, _T("csv"), _T("*.csv"), OFN_HIDEREADONLY|OFN_FILEMUSTEXIST|OFN_PATHMUSTEXIST|OFN_ENABLESIZING , szFilter, CGlobalValue::GetInstance().GetView() );

	if( dlg.DoModal() != IDOK ) return;
	CString szBuffer;
	CGlobalValue::GetInstance().CalcSignalReport( szBuffer );
	FILE *fp;
	fopen_s( &fp, dlg.GetPathName(), "w" );
	fprintf_s( fp, szBuffer );
	fclose(fp);
}
