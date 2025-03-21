#pragma once

#define MATCH_CASE			(BYTE)0x01
#define MATCH_WHOLE_WORD	(BYTE)0x02

class CLogFindDlg : public CDialog
{
	DECLARE_DYNAMIC(CLogFindDlg)

public:
	CLogFindDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CLogFindDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_LOG_FIND };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()

public:
	virtual BOOL OnInitDialog();

public:
	afx_msg void OnEnChangeEditLogFind();
	afx_msg void OnBnClickedOk();

public:
	wstring GetFindWhat() const { return m_FindWhat; }
	BYTE GetOptionFlags() const { return m_OptionFlags; }

private:
	wstring m_FindWhat;
	BYTE m_OptionFlags;
};
