// MakeManualPatchDlg.h : 헤더 파일
//

#pragma once


// CMakeManualPatchDlg 대화 상자
class CMakeManualPatchDlg : public CDialog
{
// 생성입니다.
public:
	CMakeManualPatchDlg(CWnd* pParent = NULL);	// 표준 생성자입니다.

// 대화 상자 데이터입니다.
	enum { IDD = IDD_MAKEMANUALPATCH_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.


// 구현입니다.
protected:
	HICON m_hIcon;

	// 생성된 메시지 맵 함수
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnBnClickedButtonSelectPak();
	afx_msg void OnBnClickedButtonSelectTxt();
	afx_msg void OnBnClickedButtonSelectExe();
	afx_msg void OnBnClickedButtonMakeMd5();
	afx_msg void OnBnClickedButtonMakeManualpatch();

protected:
	CString m_strExeFilePath;
	CString m_strPakFilePath;
	CString m_strTxtFilePath;

	int m_nManualPatchStartVersion;
	int m_nManualPatchEndVersion;
public:
	afx_msg void OnBnClickedButtonClear();
};
