#pragma once

// http://www.codeproject.com/Articles/3082/Tree-control-with-bitmap-checkboxes-supported
class CTreeCtrlEx : public CXTTreeCtrl
{
	DECLARE_DYNAMIC(CTreeCtrlEx)

public:
	CTreeCtrlEx();
	virtual ~CTreeCtrlEx();

protected:
	DECLARE_MESSAGE_MAP()
	CImageList m_imageState;

public:
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);

	void Activate();
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);

	BOOL IsItemChecked(HTREEITEM hItem);
};


