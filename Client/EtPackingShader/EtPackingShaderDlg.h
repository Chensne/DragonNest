// EtPackingShaderDlg.h : 헤더 파일
//

#pragma once
#include "afxwin.h"


// CEtPackingShaderDlg 대화 상자
class CEtPackingShaderDlg : public CDialog
{
// 생성입니다.
public:
	CEtPackingShaderDlg(CWnd* pParent = NULL);	// 표준 생성자입니다.

// 대화 상자 데이터입니다.
	enum { IDD = IDD_ETPACKINGSHADER_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.


// 구현입니다.
protected:
	HICON m_hIcon;
	int		m_nPrevComboSel;
	char m_szFileName[MAX_PATH];

	// 생성된 메시지 맵 함수
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	static DWORD WINAPI CompileProc( LPVOID pThisPointer );
	CString m_szCurrentDefine;
	CString	m_szDefineList[3];
	afx_msg void OnBnClickedButtonCompile();
	CString m_szProgress;
	CComboBox m_ComboBox;
	afx_msg void OnCbnSelchangeCombo();
	afx_msg void OnBnClickedButtonFolder();
	CString m_szShaderFolder;	
	CEdit m_progressEdit;
	afx_msg void OnDestroy();
	CString m_szOutputFolder;
	afx_msg void OnBnClickedButtonOutput();
};
