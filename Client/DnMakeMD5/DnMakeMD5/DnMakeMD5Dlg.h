// DnMakeMD5Dlg.h : 헤더 파일
//

#pragma once


// CDnMakeMD5Dlg 대화 상자
class CDnMakeMD5Dlg : public CDialog
{
// 생성입니다.
public:
	CDnMakeMD5Dlg(CWnd* pParent = NULL);	// 표준 생성자입니다.

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DNMAKEMD5_DIALOG };

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

private:
	CString m_szText;
	CString m_szFilePath;
	CString m_szChecksum;

public:
	afx_msg void OnBnClickedButtonOpen();
	afx_msg void OnBnClickedButtonSave();
	void DropFile(HDROP hDrop);
	
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	bool LoadFile(CString& str);
	bool SaveFile(CString str);
};
