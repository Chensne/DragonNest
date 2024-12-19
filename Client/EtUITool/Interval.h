#pragma once


// CInterval dialog

class CInterval : public CDialog
{
	DECLARE_DYNAMIC(CInterval)

public:
	CInterval(CWnd* pParent = NULL);   // standard constructor
	virtual ~CInterval();

// Dialog Data
	enum { IDD = IDD_INTERVAL };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	int m_ctrlInterval;
};
