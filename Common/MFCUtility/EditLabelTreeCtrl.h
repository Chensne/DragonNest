#pragma once


// CEditLabelTreeCtrl

class CEditLabelTreeCtrl : public CXTTreeCtrl
{
	DECLARE_DYNAMIC(CEditLabelTreeCtrl)

public:
	CEditLabelTreeCtrl();
	virtual ~CEditLabelTreeCtrl();

protected:
	DECLARE_MESSAGE_MAP()

	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
};


