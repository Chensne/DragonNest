#pragma once


// CLinkObjectDlg dialog

class CObjectSkin;
class CLinkObjectDlg : public CXTPDialog
{
	DECLARE_DYNAMIC(CLinkObjectDlg)

public:
	CLinkObjectDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CLinkObjectDlg();

// Dialog Data
	enum { IDD = IDD_LINKOBJECT };

protected:
	CComboBox m_SelectObject;
	CComboBox m_LinkTargetObject;
	CComboBox m_LinkTargetBone;
	CButton m_OKButton;
	std::map<std::string, CObjectSkin *> m_szMapSkinObject;
	CObjectSkin *m_pSelectSkin;
	CObjectSkin *m_pTargetSkin;
	CString m_szTargetBone;
//	std::vector<CObjectSkin *> m_pVecSkinObjectList;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	CXTTreeCtrl m_Tree;
	afx_msg void OnDestroy();
	afx_msg void OnCbnSelchangeCombo1();
	afx_msg void OnCbnSelchangeCombo3();
	afx_msg void OnCbnSelchangeCombo2();
};
