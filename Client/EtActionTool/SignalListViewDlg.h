#pragma once
#include "afxwin.h"


// SignalListViewDlg ��ȭ �����Դϴ�.

#include "ActionSignal.h"

#ifdef _EXPORT_EXCEL_ACTION_SIGNAL

class SignalListViewDlg : public CDialog
{
	DECLARE_DYNAMIC(SignalListViewDlg)

public:
	SignalListViewDlg(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~SignalListViewDlg();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_SIGNALLIST };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnSignalList();
	
	CComboBox m_SignalListComboBox;
	
	// ���õ� �ñ׳� Ÿ�� 
	CString m_strSignalTypeName;

	virtual BOOL OnInitDialog();
	// �ñ׳� ����
	CString m_EditCtrl_SignalCount;
};

#endif
