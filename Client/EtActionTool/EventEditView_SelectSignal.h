#pragma once



// CEventEditView_SelectSignal 폼 뷰입니다.

#include "WizardCommon.h"
#include "afxcmn.h"
#include "ColorListCtrl.h"
class CEventEditView_SelectSignal : public CFormView, public CWizardCommon
{
	DECLARE_DYNCREATE(CEventEditView_SelectSignal)

protected:
	CEventEditView_SelectSignal();           // 동적 만들기에 사용되는 protected 생성자입니다.
	virtual ~CEventEditView_SelectSignal();

public:
	enum { IDD = IDD_EVENTEDITVIEW_SELECTSIGNAL };
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif
	bool m_bActivate;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual VIEW_TYPE GetNextView();

	virtual bool IsVisivleNextButton() { return true; }
	virtual bool IsVisivlePrevButton() { return true; }
	virtual bool IsVisivleFinishButton() { return true; }
	virtual bool IsFinishOrCancel() { return false; }
	virtual void PreProcess();
	virtual bool PostProcess();

	virtual void OnInitialUpdate();
	CColorListCtrl m_ListCtrl;
};


