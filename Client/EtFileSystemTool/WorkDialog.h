
#pragma once
#include "afxcmn.h"
#include "resource.h"

// CWorkDialog 대화 상자입니다.

class CWorkDialog : public CDialog
{
	DECLARE_DYNAMIC(CWorkDialog)

public:
	CWorkDialog(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CWorkDialog();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_WORKDIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()

public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg LRESULT OnImportProgress(WPARAM wParam, LPARAM lParam);
	CProgressCtrl m_Progress;
};

extern CWorkDialog*		g_pWorking;

//! 다이얼로그 모달리스로 띄우는것임.. 귀찮아서 매크로로....
#define DrawWorking()	g_pWorking = new CWorkDialog();  \
						g_pWorking->Create(IDD_WORKDIALOG, this); \
						g_pWorking->ShowWindow(SW_SHOW);
