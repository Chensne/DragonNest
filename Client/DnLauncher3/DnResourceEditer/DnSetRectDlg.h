#pragma once
#include "afxwin.h"


// CDnSetRectDlg ��ȭ �����Դϴ�.

class CDnSetRectDlg : public CDialog
{
	DECLARE_DYNAMIC(CDnSetRectDlg)

public:
	CDnSetRectDlg(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CDnSetRectDlg();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_DIALOG_SETRECT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	CEdit m_editRect1;
	CEdit m_editRect2;
	CEdit m_editRect3;
	CEdit m_editRect4;
	afx_msg void OnBnClickedOk();
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	
	bool GetClipBoardData();
	void ParsingClipBoard(char* pString, std::vector<CString>* pList);
};
