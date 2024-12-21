#pragma once
#include "EtUIType.h"

// CDummyView form view

class CDummyView : public CFormView
{
	DECLARE_DYNCREATE(CDummyView)

protected:
	CDummyView();           // protected constructor used by dynamic creation
	virtual ~CDummyView();

public:
	enum { IDD = IDD_DUMMYVIEW };

	enum EM_VIEWTYPE
	{
		LAYOUT_VIEW,
		TEMPLATE_VIEW
	};

#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

	struct TabViewStruct 
	{
		CXTPTabManagerItem *pItem;
		CWnd *pView;
	};

	void InitView( CXTPTabControl *pControl );
	TabViewStruct *GetTabViewStruct( int nIndex ) { return &m_pTabViewItem[nIndex]; }
	void SetSelectChangeView( EM_VIEWTYPE emActiveView );
	void EnableTemplateOnLayoutView( bool bEnable );
	void AlignTemplateDialog( UIAllignHoriType alignHori, UIAllignVertType alignVert );
	bool IsShowTemplateOnLayoutView() { return m_bTemplateOnLayoutView; }

protected:
	TabViewStruct *m_pTabViewItem;
	bool m_bTemplateOnLayoutView;
	UIAllignHoriType m_TemplateDlgAlignHori;
	UIAllignVertType m_TemplateDlgAlignVert;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

public:
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
};


