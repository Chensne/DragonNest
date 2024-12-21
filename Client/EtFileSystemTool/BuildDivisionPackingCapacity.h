#pragma once
#include "afxcmn.h"
#include "resource.h"


// BuildDivisionPackingCapacity dialog

class BuildDivisionPackingCapacity : public CDialog
{
	DECLARE_DYNAMIC(BuildDivisionPackingCapacity)

public:
	BuildDivisionPackingCapacity(CWnd* pParent = NULL);   // standard constructor
	virtual ~BuildDivisionPackingCapacity();

// Dialog Data
	enum { IDD = IDD_BUILDDIVISIONPACKING_CAPACITY };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	bool	m_bWorking;
	int m_nCapacity;
	CString m_szBaseFileName;
	CString m_szInputFolder;
	CString m_szOutputFolder;
	CProgressCtrl m_CurProcess;
	CProgressCtrl m_TotalProgress;
	int m_nProgressRange;
	int m_nProgressTotalRange;

public:
	virtual void OnOK()
	{
		CDialog::OnOK();
	}
	virtual void OnCancel()
	{
		if ( m_bWorking )
		{
			AfxMessageBox(_T("�۾��߿��� ��� �Ҽ� �����ϴ�. ����"));
			return;
		}
	
		CDialog::OnCancel();
	}
	afx_msg void OnBnClickedButtonPackingCap();
	afx_msg void OnBnClickedButtonSetOutputFolderCap();
	afx_msg void OnBnClickedButtonSetInputFolderCap();
	afx_msg LRESULT OnCompleteMsg(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT	OnProgress(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT	OnProgressTotal(WPARAM wParam, LPARAM lParam);
};
