#pragma once


// CSingleLineEdit

class CSingleLineEdit : public CEdit
{
	DECLARE_DYNAMIC(CSingleLineEdit)

public:
	CSingleLineEdit();
	virtual ~CSingleLineEdit();

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
};


#define EN_EDIT_CHANGE	( WM_USER + 2000 )