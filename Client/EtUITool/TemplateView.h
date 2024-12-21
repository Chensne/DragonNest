#pragma once


#include "EtUIDialog.h"

struct SInitControl
{
	int nID;
	WCHAR wszText[ 256 ];
	float fWidth;
	float fHeight;
	bool bGenerateTexture;
};
class CUIToolTemplate;
extern SInitControl g_InitControl[];

// CTemplateView form view

class CTemplateView : public CFormView
{
	DECLARE_DYNCREATE(CTemplateView)

protected:
	CTemplateView();           // protected constructor used by dynamic creation
	virtual ~CTemplateView();

public:
	enum { IDD = IDD_TEMPLATEVIEW };
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	CEtUIDialog m_TemplateDlg;

protected:
	void SetSampleItem( CEtUIControl *pControl );

public:
	void AddTemplateControl( CUIToolTemplate *pTemplate );
	void AlignDialog( UIAllignHoriType alignHori, UIAllignVertType alignVert );

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
public:
	virtual void OnInitialUpdate();
	afx_msg LRESULT OnOpenView( WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT OnCloseView( WPARAM wParam, LPARAM lParam );
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
};


