// ChangePasswordDialog.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "ChangePasswordDialog.h"
#include "StringUtil.h"

#include "NetSession.h"

#define DNPRJSAFE	// DNPrjSafe.h 제외
#include "DNServerDef.h"
#include "DNServerPacket.h"

// CChangePasswordDialog 대화 상자입니다.

IMPLEMENT_DYNAMIC(CChangePasswordDialog, CDialog)

CChangePasswordDialog::CChangePasswordDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CChangePasswordDialog::IDD, pParent)
{
	m_pSession = NULL;
}

CChangePasswordDialog::~CChangePasswordDialog()
{
}

void CChangePasswordDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_PASSWORD, m_Password);
}


BEGIN_MESSAGE_MAP(CChangePasswordDialog, CDialog)
	ON_BN_CLICKED(IDCANCEL, &CChangePasswordDialog::OnBnClickedCancel)
	ON_BN_CLICKED(IDSUMMIT, &CChangePasswordDialog::OnBnClickedSummit)
END_MESSAGE_MAP()


BOOL CChangePasswordDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	CComboBox* pCombox = static_cast<CComboBox*>(GetDlgItem(IDC_COMBO_USERNAME));
	if (!pCombox)
		return false;

	ComboBoxMonitorUser Monitor;
	strcpy_s (Monitor.szUserName, "monitor");
	Monitor.nSelect = pCombox->GetCount();
	Monitor.nMonitorLevel = SERVERMONITOR_LEVEL_MONITOR;
	m_MonitorUser.push_back (Monitor);
	pCombox->AddString (L"monitor");

	
	ComboBoxMonitorUser Admin;
	strcpy_s (Admin.szUserName, "admin");
	Admin.nSelect = pCombox->GetCount();
	Admin.nMonitorLevel = SERVERMONITOR_LEVEL_ADMIN;
	m_MonitorUser.push_back (Admin);
	pCombox->AddString (L"admin");
	
	
	ComboBoxMonitorUser Super;
	strcpy_s (Super.szUserName, "super");
	Super.nSelect = pCombox->GetCount();
	Super.nMonitorLevel = SERVERMONITOR_LEVEL_SUPERADMIN;
	m_MonitorUser.push_back (Super);
	pCombox->AddString (L"super");

	pCombox->SetCurSel(0);

	m_Password.SetLimitText(10);

	return true;
}

// CChangePasswordDialog 메시지 처리기입니다.
void CChangePasswordDialog::OnBnClickedCancel()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	OnCancel();
}

void CChangePasswordDialog::OnBnClickedSummit()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (!m_pSession || !m_pSession->IsConnect())
	{
		::AfxMessageBox(_T("Session is not connected !"), MB_ICONINFORMATION);
		return;
	}

	// 권한
	if (SERVERMONITOR_LEVEL_SUPERADMIN != m_pSession->GetMonitorLevel()) 
	{
		::AfxMessageBox(_T("You have no authority to request"), MB_ICONWARNING);
		return;
	}

	CComboBox* pCombox = static_cast<CComboBox*>(GetDlgItem(IDC_COMBO_USERNAME));
	if (!pCombox)
		return;

	int nSelect = pCombox->GetCurSel ();

	TCHAR szPassword[PASSWORDLENMAX+1] = { _T('\0'), };
	m_Password.GetWindowText(szPassword, _countof(szPassword));
	m_Password.SetWindowText(L"");

	int nMonitorLevel = 0;
	std::string name;
	for (int i=0; i<(int)m_MonitorUser.size(); i++)
	{
		if (m_MonitorUser[i].nSelect == nSelect)
		{
			name = m_MonitorUser[i].szUserName;
			nMonitorLevel = m_MonitorUser[i].nMonitorLevel;
			break;
		}
	}

	std::string password;
	ToMultiString (szPassword, password);

	if (name.empty() || password.empty() || nMonitorLevel == 0)
	{
		::AfxMessageBox(_T("Invalid Input"), MB_ICONINFORMATION);
		return;
	}

	TChangePassword TxPacket;
	memset (&TxPacket, 0, sizeof(TxPacket));
	strncpy_s (TxPacket.szUserName, name.c_str(), NAMELENMAX);
	strncpy_s (TxPacket.szPassWord, password.c_str(), PASSWORDLENMAX);
	TxPacket.nMonitorLevel = nMonitorLevel;

	m_pSession->SendData(MONITOR2MANAGER_CHANGEPASSWORD, 0, reinterpret_cast<CHAR*>(&TxPacket), sizeof(TxPacket));

	OnOK();

}

void CChangePasswordDialog::SetSession(CNetSession* pSession)
{
	m_pSession = pSession;
}