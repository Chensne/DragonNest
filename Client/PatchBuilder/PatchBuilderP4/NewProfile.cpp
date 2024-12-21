// NewProfile.cpp : implementation file
//

#include "stdafx.h"
#include "PatchBuilderP4.h"
#include "NewProfile.h"


// CNewProfile dialog

IMPLEMENT_DYNAMIC(CNewProfile, CDialog)

CNewProfile::CNewProfile(CWnd* pParent /*=NULL*/)
	: CDialog(CNewProfile::IDD, pParent)
	, m_szProfileName(_T(""))
{

}
 
CNewProfile::~CNewProfile()
{
}

void CNewProfile::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_PROFILE_NAME, m_szProfileName);
}


BEGIN_MESSAGE_MAP(CNewProfile, CDialog)
END_MESSAGE_MAP()


// CNewProfile message handlers
