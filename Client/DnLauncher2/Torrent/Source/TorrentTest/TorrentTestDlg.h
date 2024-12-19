// TorrentTestDlg.h : 헤더 파일
//

#pragma once

#include <vector>
#include "WLibTorrent.h"

#define PROGRESS_TIMER  1000

// CTorrentTestDlg 대화 상자
class CTorrentTestDlg : public CDialog
{
// 생성입니다.
public:
	CTorrentTestDlg(CWnd* pParent = NULL);	// 표준 생성자입니다.

// 대화 상자 데이터입니다.
	enum { IDD = IDD_TORRENTTEST_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.


// 구현입니다.
protected:
	HICON m_hIcon;

	// 생성된 메시지 맵 함수
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	afx_msg void OnPaint();
	afx_msg void OnTimer( UINT_PTR nIDEvent );
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedButton3();

private:
	CString AddSuffix( float val, char const* suffix = 0 );
	bool CheckPeers( std::vector<peer_info>& peers );
	CString m_strTorrentName;
	CString m_strSavePath;
	std::vector<peer_info> m_vecPeers;
};
