#pragma once
#include "afxwin.h"


// CDnTextModifyDlg ��ȭ �����Դϴ�.

class CDnTextModifyDlg : public CDialog
{
	DECLARE_DYNAMIC(CDnTextModifyDlg)

public:
	CDnTextModifyDlg(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CDnTextModifyDlg();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_DIALOG_TEXTMODIFY };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	CEdit m_editText;
	afx_msg void OnEnChangeEditText();
	afx_msg void OnDestroy();
	virtual BOOL OnInitDialog();
};
