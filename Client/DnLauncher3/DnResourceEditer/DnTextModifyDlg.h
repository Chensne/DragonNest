#pragma once
#include "afxwin.h"


// CDnTextModifyDlg 대화 상자입니다.

class CDnTextModifyDlg : public CDialog
{
	DECLARE_DYNAMIC(CDnTextModifyDlg)

public:
	CDnTextModifyDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CDnTextModifyDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_TEXTMODIFY };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	CEdit m_editText;
	afx_msg void OnEnChangeEditText();
	afx_msg void OnDestroy();
	virtual BOOL OnInitDialog();
};
