#pragma once


// CTabOrder dialog

class CTabOrder : public CDialog
{
	DECLARE_DYNAMIC(CTabOrder)

public:
	CTabOrder(CWnd* pParent = NULL);   // standard constructor
	virtual ~CTabOrder();

// Dialog Data
	enum { IDD = IDD_TABORDER };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	int m_ctrlTabNumber;
};
