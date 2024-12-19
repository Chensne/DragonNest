#pragma once
// CCommandListDlg 대화 상자입니다.

class CCommandListDlg : public CDialog
{
	DECLARE_DYNAMIC(CCommandListDlg)

public:
	CCommandListDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CCommandListDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_COMMAND_LIST };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();

public:
	afx_msg void OnLbnDblclkListCommandList();
	afx_msg void OnBnClickedOk();

public:
	void SetCommand();
	const wstring& GetCommand() const { return m_Command; }

private:
	wstring m_Command;
};
