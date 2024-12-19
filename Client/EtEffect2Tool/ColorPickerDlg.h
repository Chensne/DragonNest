#pragma once
#include "XColorSpectrumCtrl.h"

// CColorPickerDlg dialog

class CColorPickerDlg : public CDialog
{
	DECLARE_DYNAMIC(CColorPickerDlg)

public:
	CColorPickerDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CColorPickerDlg();

// Dialog Data
	enum { IDD = IDD_COLORPICKER };

protected:
	CXColorSpectrumCtrl m_ColorSpectrum;
	COLORREF	m_InitialColor;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	void UpdateColor();
	void UpdatePreviewBox();
	COLORREF GetColor();
	void SetColor( COLORREF color );
public:
	afx_msg void OnPaint();
	afx_msg LRESULT OnSelChange(WPARAM, LPARAM);
	afx_msg LRESULT OnSelendOk(WPARAM, LPARAM);
	CString m_strRed;
	CString m_strGreen;
	CString m_strBlue;
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton2();
	afx_msg void OnEnChangeEdit1();
	afx_msg void OnEnChangeEdit2();
	afx_msg void OnEnChangeEdit3();
};
