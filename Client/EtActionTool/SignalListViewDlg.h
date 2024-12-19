#pragma once
#include "afxwin.h"


// SignalListViewDlg 대화 상자입니다.

#include "ActionSignal.h"

#ifdef _EXPORT_EXCEL_ACTION_SIGNAL

class SignalListViewDlg : public CDialog
{
	DECLARE_DYNAMIC(SignalListViewDlg)

public:
	SignalListViewDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~SignalListViewDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_SIGNALLIST };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnSignalList();
	
	CComboBox m_SignalListComboBox;
	
	// 선택된 시그널 타입 
	CString m_strSignalTypeName;

	virtual BOOL OnInitDialog();
	// 시그널 갯수
	CString m_EditCtrl_SignalCount;
};

#endif
