// TabOrder.cpp : implementation file
//

#include "stdafx.h"
#include "EtUITool.h"
#include "TabOrder.h"


// CTabOrder dialog

IMPLEMENT_DYNAMIC(CTabOrder, CDialog)

CTabOrder::CTabOrder(CWnd* pParent /*=NULL*/)
	: CDialog(CTabOrder::IDD, pParent)
	, m_ctrlTabNumber(0)
{

}

CTabOrder::~CTabOrder()
{
}

void CTabOrder::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_ctrlTabNumber);
	DDV_MinMaxInt(pDX, m_ctrlTabNumber, 1, 1000000);
}


BEGIN_MESSAGE_MAP(CTabOrder, CDialog)
END_MESSAGE_MAP()


// CTabOrder message handlers
