#pragma once
#include "afxwin.h"
#include "afxcmn.h"
#include "EtFontMng.h"

// CFontSetDlg dialog

class CFontSetDlg : public CDialog
{
	DECLARE_DYNAMIC(CFontSetDlg)

public:
	CFontSetDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CFontSetDlg();

// Dialog Data
	enum { IDD = IDD_FONTSET_SELECT };

public:
	int m_nSelectIndex;

protected:
	CListCtrl m_ctrlFontSetList;
	std::string m_strFontSetFileName;

protected:
	void UpdateFontSetList();
	void GetFontSetFromList( int nIndex, SUIFontSet &fontSet, CFontDialog &fontDialog );
	void SetFontSetToList( int nIndex, SUIFontSet &fontSet );

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedAdd();
	afx_msg void OnBnClickedDelete();
	afx_msg void OnBnClickedModify();
	afx_msg void OnBnClickedSave();
	afx_msg void OnBnClickedLoad();
	afx_msg void OnBnClickedSaveAs();
	afx_msg void OnBnClickedSameFontname();
	afx_msg void OnBnClickedSameFontweight();
};
