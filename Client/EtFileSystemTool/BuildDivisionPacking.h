#pragma once
#include "afxcmn.h"
#include "resource.h"


struct _PackingInfo 
{
	_PackingInfo()
	{
		bIncludeSubFolder = true;
	}
	tstring szSrcFolder;
	tstring szFileName;
	tstring szBaseFolder;
	bool    bIncludeSubFolder;
};


bool ParseSettingFile(const CHAR* szFileName, OUT std::vector<_PackingInfo>& PackingInfo);

// BuildDivisionPacking dialog

class BuildDivisionPacking : public CDialog
{
	DECLARE_DYNAMIC(BuildDivisionPacking)

public:
	BuildDivisionPacking(CWnd* pParent = NULL);   // standard constructor
	virtual ~BuildDivisionPacking();

// Dialog Data
	enum { IDD = IDD_BUILDDIVISIONPACKING };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

private:

	bool	m_bWorking;
	CString m_SettingFileName;
	CString m_PackingState;
	CString m_szOutFolder;

	CProgressCtrl m_DivPackingProgress;
	CProgressCtrl m_DivPackingProgressTotal;
	int m_nProgressRange;
	int m_nProgressTotalRange;

	std::vector<_PackingInfo>	m_PackingInfo;
	
	
public:
	afx_msg void OnBnClickedButtonFileOpen();
	afx_msg void OnBnClickedButtonViewFile();
	afx_msg void OnBnClickedButtonPacking();
	afx_msg void OnBnClickedButtonSetOutputFolder();
	afx_msg LRESULT OnCompleteMsg(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT	OnProgress(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT	OnProgressTotal(WPARAM wParam, LPARAM lParam);
	
	
	virtual void OnOK()
	{
		CDialog::OnOK();
	}
	virtual void OnCancel()
	{
		if ( m_bWorking )
		{
			AfxMessageBox(_T("작업중에는 취소 할수 없습니다. ㅋㅋ"));
			return;
		}
		CDialog::OnCancel();
	}
	
};

//extern BuildDivisionPacking*	g_pBudil