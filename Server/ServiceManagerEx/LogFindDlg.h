#pragma once

#define MATCH_CASE			(BYTE)0x01
#define MATCH_WHOLE_WORD	(BYTE)0x02

class CLogFindDlg : public CDialog
{
	DECLARE_DYNAMIC(CLogFindDlg)

public:
	CLogFindDlg(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CLogFindDlg();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_DIALOG_LOG_FIND };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

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
