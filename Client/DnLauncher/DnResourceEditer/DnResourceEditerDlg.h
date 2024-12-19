// DnResourceEditerDlg.h : 헤더 파일
//

#pragma once


class DnEditLauncherDlg;

// CDnResourceEditerDlg 대화 상자
class CDnResourceEditerDlg : public CDialog
{
// 생성입니다.
public:
	CDnResourceEditerDlg(CWnd* pParent = NULL);	// 표준 생성자입니다.

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DNRESOURCEEDITER_DIALOG };

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

	DnEditLauncherDlg* m_pEditDlg; // 메인 수정 다이얼로그

public:
	afx_msg void OnBnClickedBtnNewfile();
	afx_msg void OnBnClickedBtnLoadfile();
	afx_msg void OnDestroy();
	afx_msg void OnMenuClick();
};
