// EtPackingShaderDlg.h : ��� ����
//

#pragma once
#include "afxwin.h"


// CEtPackingShaderDlg ��ȭ ����
class CEtPackingShaderDlg : public CDialog
{
// �����Դϴ�.
public:
	CEtPackingShaderDlg(CWnd* pParent = NULL);	// ǥ�� �������Դϴ�.

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_ETPACKINGSHADER_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV �����Դϴ�.


// �����Դϴ�.
protected:
	HICON m_hIcon;
	int		m_nPrevComboSel;
	char m_szFileName[MAX_PATH];

	// ������ �޽��� �� �Լ�
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	static DWORD WINAPI CompileProc( LPVOID pThisPointer );
	CString m_szCurrentDefine;
	CString	m_szDefineList[3];
	afx_msg void OnBnClickedButtonCompile();
	CString m_szProgress;
	CComboBox m_ComboBox;
	afx_msg void OnCbnSelchangeCombo();
	afx_msg void OnBnClickedButtonFolder();
	CString m_szShaderFolder;	
	CEdit m_progressEdit;
	afx_msg void OnDestroy();
	CString m_szOutputFolder;
	afx_msg void OnBnClickedButtonOutput();
};
