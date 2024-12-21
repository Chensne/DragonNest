#pragma once
#include "afxwin.h"
#include <map>



// CPreviewPaneView �� ���Դϴ�.

class CPreviewPaneView : public CFormView
{
	DECLARE_DYNCREATE(CPreviewPaneView)

protected:
	CPreviewPaneView();           // ���� ����⿡ ���Ǵ� protected �������Դϴ�.
	virtual ~CPreviewPaneView();

public:
	enum { IDD = IDD_EFFECTVIEW };
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

public:
	std::map< std::string, int > m_mapEffectData;

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	CListBox m_ctrlListBox;
	afx_msg void OnDropFiles(HDROP hDropInfo);
	afx_msg void OnBnClickedButton1();
	afx_msg void OnLbnSelchangeList1();
};


