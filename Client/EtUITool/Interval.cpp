// Interval.cpp : implementation file
//

#include "stdafx.h"
#include "EtUITool.h"
#include "Interval.h"


// CInterval dialog

IMPLEMENT_DYNAMIC(CInterval, CDialog)

CInterval::CInterval(CWnd* pParent /*=NULL*/)
	: CDialog(CInterval::IDD, pParent)
	, m_ctrlInterval(0)
{

}

CInterval::~CInterval()
{
}

void CInterval::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_ctrlInterval);
}


BEGIN_MESSAGE_MAP(CInterval, CDialog)
END_MESSAGE_MAP()


// CInterval message handlers
