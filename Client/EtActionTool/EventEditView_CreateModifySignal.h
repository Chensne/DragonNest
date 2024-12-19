#pragma once



// CEventEditView_CreateModifySignal 폼 뷰입니다.

#include "WizardCommon.h"
class CEventEditView_CreateModifySignal : public CFormView, public CWizardCommon
{
	DECLARE_DYNCREATE(CEventEditView_CreateModifySignal)

protected:
	CEventEditView_CreateModifySignal();           // 동적 만들기에 사용되는 protected 생성자입니다.
	virtual ~CEventEditView_CreateModifySignal();

public:
	enum { IDD = IDD_EVENTEDITVIEW_CREATEMODIFYSIGNAL };
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
	virtual VIEW_TYPE GetNextView() { return FINISH; }

	virtual bool IsVisivleNextButton() { return true; }
	virtual bool IsVisivlePrevButton() { return true; }
	virtual bool IsVisivleFinishButton() { return true; }
	virtual bool IsFinishOrCancel() { return false; }
	virtual void PreProcess();
	virtual bool PostProcess();

	CString m_szSignalName;
	CXTColorPicker m_InitialsColorBtn;
	CXTColorPicker m_BackgroundColorBtn;
	COLORREF m_InitialsColor;
	COLORREF m_BackgroundColor;
	BOOL m_bHasLength;

	afx_msg void OnPaint();
	void OnSelEndOkInitialsClr();
	void OnSelEndOkBackgroundClr();

	CDC *m_pmemDC;
	CBitmap *m_pdcBitmap;
	void CreateMemDC();
	void DeleteMemDC();

	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnBnClickedCheck1();
	afx_msg void OnEnChangeEdit1();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};


