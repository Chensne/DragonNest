#pragma once

// CWorldMaxUserDlg 대화 상자입니다.

class CWorldMaxUserDlg : public CDialog
{
	DECLARE_DYNAMIC(CWorldMaxUserDlg)

public:
	CWorldMaxUserDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CWorldMaxUserDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_WORLD_MAX_USER };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnBnClickedOk();

public:
	UINT GetMaxUser() const { return m_MaxUser; }

private:
	UINT m_MaxUser;
};
