// MakeTorrentDlg.h : 헤더 파일
//

#pragma once


// CMakeTorrentDlg 대화 상자
class CMakeTorrentDlg : public CDialog
{
// 생성입니다.
public:
	CMakeTorrentDlg(CWnd* pParent = NULL);	// 표준 생성자입니다.

// 대화 상자 데이터입니다.
	enum { IDD = IDD_MAKETORRENT_DIALOG };

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
	afx_msg void OnBnClickedButtonFile();
	afx_msg void OnBnClickedButtonPath();
	afx_msg void OnBnClickedButtonMake();
	afx_msg void OnBnClickedButtonReset();

private:
	CString m_strFileName;
};
