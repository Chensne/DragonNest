// AddressDlg.cpp : implementation file
//

#include "stdafx.h"
#include "NetTest.h"
#include "AddressDlg.h"

// CAddressDlg dialog

IMPLEMENT_DYNAMIC(CAddressDlg, CDialog)

CAddressDlg::CAddressDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAddressDlg::IDD, pParent)
	, m_szAddress( _T( "192.168.0.107" ) )
	, m_nPort( 6666 )
	, m_szID(_T(""))
	, m_szPassword(_T(""))
{
}

CAddressDlg::~CAddressDlg()
{
}

void CAddressDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_ADDRESS, m_szAddress);
	DDX_Text(pDX, IDC_EDIT_PORT, m_nPort);
	DDX_Text(pDX, IDC_EDIT_ID, m_szID);
	DDX_Text(pDX, IDC_EDIT_PASSWORD, m_szPassword);
}


BEGIN_MESSAGE_MAP(CAddressDlg, CDialog)
END_MESSAGE_MAP()


// CAddressDlg message handlers


