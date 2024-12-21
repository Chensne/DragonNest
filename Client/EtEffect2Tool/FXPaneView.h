#pragma once
#include "afxcmn.h"


// CFXPaneView form view
#include "TreeCtrlEx.h"

class CFXPaneView : public CFormView
{
	DECLARE_DYNCREATE(CFXPaneView)

protected:
	CFXPaneView();           // protected constructor used by dynamic creation
	virtual ~CFXPaneView();

public:
	enum { IDD = IDD_FXEDIT };
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CTreeCtrlEx m_TreeCtrl;
	CImageList m_ImageList;
	bool	m_bActivate;
	HTREEITEM	m_rootItem;
	CMenu *m_pContextMenu;
	char			m_szSelectName[255];
	std::map< std::string, int > m_uniqueStringCounter;
	CString m_strCopyFXName;


	virtual void OnInitialUpdate();
	HTREEITEM GetCurrentItem();
	int GetCurrentDepth();
	int GetCurrentPeerOrder();
	void OnSelected();
	afx_msg void OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/);
	bool IsCanAddParticle();
	void SetCurrentName(char *name) {strcpy(m_szSelectName, name); }
	void AddItem( char *pDescription);
	void RemoveItem( char *pDescription);
	void SetLayer( char *pDescription, bool bUp );
	char* GetUniqueName( char *name);
	char* AddSelectedParticleToGrid();
	char *GetCurrentSelectName() {return m_szSelectName;}
	bool IsVisibleItem( char *pDesc );
	void Reset();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnTvnSelchangedTree1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMClickTree1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnFxUp();
	afx_msg void OnFxDown();
	afx_msg void OnFxCopy();
	afx_msg void OnFxPaste();
};


