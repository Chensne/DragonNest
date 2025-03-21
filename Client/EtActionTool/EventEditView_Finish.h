#pragma once



// CEventEditView_Finish 폼 뷰입니다.

#include "WizardCommon.h"
#include "afxwin.h"
class CEventEditView_Finish : public CFormView, public CWizardCommon
{
	DECLARE_DYNCREATE(CEventEditView_Finish)

protected:
	CEventEditView_Finish();           // 동적 만들기에 사용되는 protected 생성자입니다.
	virtual ~CEventEditView_Finish();

public:
	enum { IDD = IDD_EVENTEDITVIEW_FINISH };
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()

public:
	virtual VIEW_TYPE GetNextView() { return UNKNOWN_TYPE; }

	virtual bool IsVisivleNextButton() { return false; }
	virtual bool IsVisivlePrevButton() { return false; }
	virtual bool IsVisivleFinishButton() { return true; }
	virtual bool IsFinishOrCancel() { return true; }
	virtual void PreProcess();
	virtual bool PostProcess();
	afx_msg void OnBnClickedCheck1();
	BOOL m_bCheckExport;
	CString m_szExportFileName;
	afx_msg void OnBnClickedButton1();
};


