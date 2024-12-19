#include "stdafx.h"
#include "ServiceMonitor.h"
#include "PartitionDialog.h"
#include "ConnectionDialog.h"
#include "NetConnection.h"

#include "MainFrm.h"
#include "BasePartitionView.h"

#define DNPRJSAFE	// DNPrjSafe.h 제외
#include "DNPacket.h"
#include "DNServerDef.h"
#include "DNServerPacket.h"

CPartitionDlg::CPartitionDlg(CWnd* pParent /*=NULL*/)
: CDialog(CPartitionDlg::IDD, pParent)
{
	m_pSession = NULL;
}

void CPartitionDlg::OnOK()
{
	// 다이얼로그가 ESC를 눌렀을때 꺼지지 않도록 처리한다.
}


void CPartitionDlg::OnCancel()
{
	// 다이얼로그가 ESC를 눌렀을때 꺼지지 않도록 처리한다.
}


void CPartitionDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_LOGDAYS, m_LogDays);
}


BEGIN_MESSAGE_MAP(CPartitionDlg, CDialog)

	ON_BN_CLICKED(IDC_MDB_CONNECTBUTTON, &CPartitionDlg::OnBnClickedMdbConnectbutton)
	ON_BN_CLICKED(IDC_MDB_EXIT, &CPartitionDlg::OnBnClickedMdbExit)
	ON_BN_CLICKED(IDC_MDB_ERRORLOGBUTTON, &CPartitionDlg::OnBnClickedMdbErrorlogbutton)
	ON_BN_CLICKED(IDC_MDB_REFRESHWORLDINFO, &CPartitionDlg::OnBnClickedMdbRefreshworldinfo)
	ON_BN_CLICKED(IDC_START_PROCESS, &CPartitionDlg::OnBnClickedStartProcess)
	ON_BN_CLICKED(IDC_STOP_PROCESS, &CPartitionDlg::OnBnClickedStopProcess)
	ON_BN_CLICKED(IDC_STOPFORCE_PROCESS, &CPartitionDlg::OnBnClickedStopForceProcess)
	ON_BN_CLICKED(IDC_EXCEPT_REPORT, &CPartitionDlg::OnBnClickedExceptReport)
	ON_BN_CLICKED(IDC_MDB_RESETDELAYCOUNT, &CPartitionDlg::OnBnClickedMdbResetdelaycount)
	ON_BN_CLICKED(IDC_MDB_CHANGEPW, &CPartitionDlg::OnBnClickedMdbChangepw)
END_MESSAGE_MAP()


BOOL CPartitionDlg::Create(const RECT &rect, CWnd *pParentWnd)
{
	if(!CDialog::Create(IDD_PARDLG, pParentWnd))
		return FALSE;

	MoveWindow(rect.left, rect.top, (rect.right-rect.left), (rect.bottom-rect.top),TRUE);
	ModifyStyle(0,WS_VISIBLE,0);
	InvalidateRect(NULL, TRUE);

	m_pParentWnd = pParentWnd;

	if (!::IsWindow(m_ChangePasswordDialog.GetSafeHwnd())) {
		m_ChangePasswordDialog.Create(IDD_CHGPWDLG, this);
	}

	return TRUE;
}

void CPartitionDlg::AddProcessType(int nWorldID)
{
	TCHAR szProcessType[MAX_PATH] = { _T('\0'), };

	if (EV_WORLDID_ALL == nWorldID)
		_sntprintf_s(szProcessType, _countof(szProcessType), _T("All Servers"));
	else
		_sntprintf_s(szProcessType, _countof(szProcessType), _T("World %d"), nWorldID);

	CComboBox* pCombox = static_cast<CComboBox*>(GetDlgItem(IDC_COMBO_PROCESSTYPE));
	if (!pCombox)
		return;

	for (int i=0; i<(int)m_ProcessType.size(); i++)
	{
		if (nWorldID == m_ProcessType[i].nWorldID)
			return;
	}

	ComboBoxProcess process;
	process.nSelect = pCombox->GetCount();
	process.nWorldID = nWorldID;
	m_ProcessType.push_back(process);

	pCombox->AddString(szProcessType);
	pCombox->SetCurSel(0);

}

void CPartitionDlg::SetSession(CNetSession* pSession)
{
	m_pSession = pSession;
	m_ChangePasswordDialog.SetSession (pSession);
}

void CPartitionDlg::OnBnClickedMdbConnectbutton()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CConnectionDialog& ConnectionDialog = static_cast<CServiceMonitorApp*>(::AfxGetApp())->GetConnectionDialog();
	
	if (::IsWindow(ConnectionDialog.GetSafeHwnd()))
		ConnectionDialog.ShowWindow((!ConnectionDialog.IsWindowVisible())?(SW_NORMAL):(SW_HIDE));	
}
void CPartitionDlg::OnBnClickedMdbExit()
{
	return;

	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CMainFrame* pMainFrame = static_cast<CServiceMonitorApp*>(::AfxGetApp())->GetMainFrame();

	CBasePartitionView* pPartitionView = reinterpret_cast<CBasePartitionView*>(m_pParentWnd);
	pMainFrame->RemovePartition (pPartitionView);

}

void CPartitionDlg::OnBnClickedMdbErrorlogbutton()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CBasePartitionView* pPartitionView = reinterpret_cast<CBasePartitionView*>(m_pParentWnd);

	if (pPartitionView)
		pPartitionView->SetActiveErrorLog();

}

void CPartitionDlg::OnBnClickedMdbRefreshworldinfo()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CBasePartitionView* pPartitionView = reinterpret_cast<CBasePartitionView*>(m_pParentWnd);

	pPartitionView->Refreshworldinfo();
/*
	// test code
	if (!m_pSession || !m_pSession->IsConnect())
	{
		::AfxMessageBox(_T("Session is not connected !"), MB_ICONINFORMATION);
		return;
	}

	TMSServerDealyReq TxPacket;
	TxPacket.nType = SERVERTYPE_GAME;
	TxPacket.nSID = 1004;
	m_pSession->SendData(MANAGER2MONITOR_SERVER_DEALYINFO_REQ, 0, reinterpret_cast<CHAR*>(&TxPacket), sizeof(TxPacket));

	TMSServerDealyReq TxPacket2;
	TxPacket2.nType = SERVERTYPE_DB;
	TxPacket2.nSID = 1000;
	m_pSession->SendData(MANAGER2MONITOR_SERVER_DEALYINFO_REQ, 0, reinterpret_cast<CHAR*>(&TxPacket2), sizeof(TxPacket2));
*/

}

void CPartitionDlg::OnBnClickedStartProcess()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	// 세션
	if (!m_pSession || !m_pSession->IsConnect())
	{
		::AfxMessageBox(_T("Session is not connected !"), MB_ICONINFORMATION);
		return;
	}

	// 컨트롤
	CComboBox* pCombox = static_cast<CComboBox*>(GetDlgItem(IDC_COMBO_PROCESSTYPE));
	if (!pCombox)
	{
		::AfxMessageBox(_T("Process Control Error !"), MB_ICONERROR);
		return;
	}

	// 권한
	if (SERVERMONITOR_LEVEL_ADMIN > m_pSession->GetMonitorLevel()) 
	{
		::AfxMessageBox(_T("You have no authority to request"), MB_ICONWARNING);
		return;
	}

	int nWorldID = -1;
	int nSelect = pCombox->GetCurSel();

	for (int i=0; i<(int)m_ProcessType.size(); i++)
	{
		if (nSelect == m_ProcessType[i].nSelect)
		{
			nWorldID = m_ProcessType[i].nWorldID;
			break;
		}	
	}
	
	if (nWorldID == -1)
	{
		::AfxMessageBox(_T("Process is not selected"), MB_ICONERROR);
		return;
	}

	CServiceInfo::TP_LISTSERVAUTO ServerList;
	if (m_pSession->GetServiceInfo()->GetRecvMode(EV_SRT_STRUCT)) 
		m_pSession->GetServiceInfo()->GetServerList(EV_SMT_REAL, ServerList, nWorldID, TRUE);
	else
	{
		::AfxMessageBox(_T("Now loading, please try again for a while"), MB_ICONINFORMATION);
		return;
	}

	if (ServerList.size() <= 0)
	{
		::AfxMessageBox(_T("None Process!"), MB_ICONWARNING);
		return;
	}

	// Confirm
	if (IDOK != ::AfxMessageBox(_T("Are you sure to request ?"), MB_ICONQUESTION | MB_OKCANCEL))
		return;

	TSMServiceStart TxPacket;
	memset (&TxPacket, 0x00, sizeof(TxPacket));

	// START ALL
	if (nWorldID == EV_WORLDID_ALL)
	{
		::AfxMessageBox(_T("Start All Process"), MB_ICONINFORMATION);

		TxPacket.nStartType = _SERVICE_START_ALL;	
		m_pSession->SendData(MONITOR2MANAGER_STARTSERVICE, 0, reinterpret_cast<CHAR*>(&TxPacket), 
							(static_cast<int>((sizeof(TxPacket) - (sizeof(TxPacket.nIDs[0]) * (_countof(TxPacket.nIDs) - TxPacket.cCount)))&ULONG_MAX)));

		return;
	}


	// START EACH
	TxPacket.nStartType = _SERVICE_START_EACH;
	for (int i=0; i<(int)ServerList.size(); i++)
	{
		TxPacket.nIDs[TxPacket.cCount++] = ServerList[i].m_ServerID;

		if (_countof(TxPacket.nIDs) <= TxPacket.cCount)
		{
			::AfxMessageBox(CVarArg<MAX_PATH>(_T("Item count you requested is over %d.\n\novered item will be ignored."), _countof(TxPacket.nIDs)), MB_ICONWARNING | MB_OK);
			break;
		}
	}

	if (TxPacket.cCount <= 0)
		ASSERT(0);

	::AfxMessageBox(CVarArg<MAX_PATH>(_T("Start Process[WorldID:%d]"), nWorldID), MB_ICONINFORMATION);

	m_pSession->SendData(MONITOR2MANAGER_STARTSERVICE, 0, reinterpret_cast<CHAR*>(&TxPacket), 
						(static_cast<int>((sizeof(TxPacket) - (sizeof(TxPacket.nIDs[0]) * (_countof(TxPacket.nIDs) - TxPacket.cCount)))&ULONG_MAX)));
}

void CPartitionDlg::OnBnClickedStopProcess()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	// 세션
	if (!m_pSession || !m_pSession->IsConnect())
	{
		::AfxMessageBox(_T("Session is not connected !"), MB_ICONINFORMATION);
		return;
	}

	// 컨트롤
	CComboBox* pCombox = static_cast<CComboBox*>(GetDlgItem(IDC_COMBO_PROCESSTYPE));
	if (!pCombox)
	{
		::AfxMessageBox(_T("Process Control Error !"), MB_ICONERROR);
		return;
	}

	// 권한
	if (SERVERMONITOR_LEVEL_SUPERADMIN > m_pSession->GetMonitorLevel()) 
	{
		::AfxMessageBox(_T("You have no authority to request"), MB_ICONWARNING);
		return;
	}

	int nWorldID = -1;
	int nSelect = pCombox->GetCurSel();

	for (int i=0; i<(int)m_ProcessType.size(); i++)
	{
		if (nSelect == m_ProcessType[i].nSelect)
		{
			nWorldID = m_ProcessType[i].nWorldID;
			break;
		}	
	}

	if (nWorldID == -1)
	{
		::AfxMessageBox(_T("Process is not selected"), MB_ICONERROR);
		return;
	}

	CServiceInfo::TP_LISTSERVAUTO ServerList;
	if (m_pSession->GetServiceInfo()->GetRecvMode(EV_SRT_STRUCT)) 
		m_pSession->GetServiceInfo()->GetServerList(EV_SMT_REAL, ServerList, nWorldID, TRUE);
	else
	{
		::AfxMessageBox(_T("Now loading, please try again for a while"), MB_ICONINFORMATION);
		return;
	}

	if (ServerList.size() <= 0)
	{
		::AfxMessageBox(_T("None Process!"), MB_ICONWARNING);
		return;
	}

	// Confirm
	if (IDOK != ::AfxMessageBox(_T("Are you sure to request ?"), MB_ICONQUESTION | MB_OKCANCEL))
		return;

	TSMServiceStart TxPacket;
	memset (&TxPacket, 0x00, sizeof(TxPacket));

	// STOP ALL
	if (nWorldID == EV_WORLDID_ALL)
	{
		::AfxMessageBox(_T("Stop All Process (without db, village, game) for 5minutes"), MB_ICONINFORMATION);

		TxPacket.nStartType = _SERVICE_STOP_ALL;	
		m_pSession->SendData(MONITOR2MANAGER_STOPSERVICE, 0, reinterpret_cast<CHAR*>(&TxPacket), 
			(static_cast<int>((sizeof(TxPacket) - (sizeof(TxPacket.nIDs[0]) * (_countof(TxPacket.nIDs) - TxPacket.cCount)))&ULONG_MAX)));

		return;
	}


	// STOP EACH
	TxPacket.nStartType = _SERVICE_STOP_EACH;
	for (int i=0; i<(int)ServerList.size(); i++)
	{
		switch (ServerList[i].m_ServerType)
		{
		case EV_SVT_LG:
		case EV_SVT_DB:
		case EV_SVT_MA:
			continue;
		}


		if (ServerList[i].m_ServerState != EV_SVS_CNNT)
		{
			::AfxMessageBox(CVarArg<MAX_PATH>(_T("Terminate Process Busy[SID:%d]"), ServerList[i].m_ServerID), MB_ICONINFORMATION);
			continue;
		}

		TxPacket.nIDs[TxPacket.cCount++] = ServerList[i].m_ServerID;

		if (_countof(TxPacket.nIDs) <= TxPacket.cCount)
		{
			::AfxMessageBox(CVarArg<MAX_PATH>(_T("Item count you requested is over %d.\n\novered item will be ignored."), _countof(TxPacket.nIDs)), MB_ICONWARNING | MB_OK);
			break;
		}
	}

	if (TxPacket.cCount <= 0)
		return;

	::AfxMessageBox(CVarArg<MAX_PATH>(_T("Stop All[WorldID:%d] (without db, village, game) for 5minutes"), nWorldID), MB_ICONINFORMATION);

	m_pSession->SendData(MONITOR2MANAGER_STOPSERVICE, 0, reinterpret_cast<CHAR*>(&TxPacket), 
		(static_cast<int>((sizeof(TxPacket) - (sizeof(TxPacket.nIDs[0]) * (_countof(TxPacket.nIDs) - TxPacket.cCount)))&ULONG_MAX)));
}

void CPartitionDlg::OnBnClickedStopForceProcess()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	// 세션
	if (!m_pSession || !m_pSession->IsConnect())
	{
		::AfxMessageBox(_T("Session is not connected !"), MB_ICONINFORMATION);
		return;
	}

	// 컨트롤
	CComboBox* pCombox = static_cast<CComboBox*>(GetDlgItem(IDC_COMBO_PROCESSTYPE));
	if (!pCombox)
	{
		::AfxMessageBox(_T("Process Control Error !"), MB_ICONERROR);
		return;
	}

	// 권한
	if (SERVERMONITOR_LEVEL_SUPERADMIN > m_pSession->GetMonitorLevel()) 
	{
		::AfxMessageBox(_T("You have no authority to request"), MB_ICONWARNING);
		return;
	}

	int nWorldID = -1;
	int nSelect = pCombox->GetCurSel();

	for (int i=0; i<(int)m_ProcessType.size(); i++)
	{
		if (nSelect == m_ProcessType[i].nSelect)
		{
			nWorldID = m_ProcessType[i].nWorldID;
			break;
		}	
	}

	if (nWorldID == -1)
	{
		::AfxMessageBox(_T("Process is not selected"), MB_ICONERROR);
		return;
	}

	CServiceInfo::TP_LISTSERVAUTO ServerList;
	if (m_pSession->GetServiceInfo()->GetRecvMode(EV_SRT_STRUCT)) 
		m_pSession->GetServiceInfo()->GetServerList(EV_SMT_REAL, ServerList, nWorldID, TRUE);
	else
	{
		::AfxMessageBox(_T("Now loading, please try again for a while"), MB_ICONINFORMATION);
		return;
	}

	if (ServerList.size() <= 0)
	{
		::AfxMessageBox(_T("None Process!"), MB_ICONWARNING);
		return;
	}

	// Confirm
	if (IDOK != ::AfxMessageBox(_T("Are you sure to request ?"), MB_ICONQUESTION | MB_OKCANCEL))
		return;

	CServiceInfo::TP_LISTWRLDAUTO WorldList;
	m_pSession->GetServiceInfo()->GetWorldList(EV_SMT_REAL, WorldList, TRUE);
	for (int i=0; i<(int)WorldList.size(); i++)
	{
		if (nWorldID != EV_WORLDID_ALL && nWorldID != WorldList[i].m_WorldID)
			continue;

		TSMServiceStart TxPacket;
		memset (&TxPacket, 0x00, sizeof(TxPacket));
		TxPacket.nStartType = _SERVICE_STOP_EACH_FORCE;

		m_pSession->GetServiceInfo()->GetServerList(EV_SMT_REAL, ServerList, WorldList[i].m_WorldID, TRUE);
		for (int i=0; i<(int)ServerList.size(); i++)
		{
			switch (ServerList[i].m_ServerType)
			{
				case EV_SVT_DB:
				case EV_SVT_LG:
				case EV_SVT_MA:
				{
					if (TxPacket.cCount < SERVERCONTROLMAX)
						TxPacket.nIDs[TxPacket.cCount++] = ServerList[i].m_ServerID;
				}
				break;
			}
		}

		if (TxPacket.cCount <= 0)
			return;

		m_pSession->SendData(MONITOR2MANAGER_STOPSERVICE, 0, reinterpret_cast<CHAR*>(&TxPacket), 
			(static_cast<int>((sizeof(TxPacket) - (sizeof(TxPacket.nIDs[0]) * (_countof(TxPacket.nIDs) - TxPacket.cCount)))&ULONG_MAX)));
	}
}

void CPartitionDlg::OnBnClickedExceptReport()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	TCHAR szLogDays[SMIDLENMAX+1] = { _T('\0'), };
	m_LogDays.GetWindowText(szLogDays, _countof(szLogDays));

	int nLogDays = static_cast<int>(_ttoi(szLogDays));

	if (nLogDays > 255)
		return;

	TSMReportReq TxPacket;
	TxPacket.bBuild = true;
	TxPacket.cDays = (BYTE)nLogDays;
	TxPacket.nPage = 1;

	m_pSession->SendData(MONITOR2MANAGER_REPORT_REQ, 0, reinterpret_cast<CHAR*>(&TxPacket), sizeof(TxPacket));

}

void CPartitionDlg::OnBnClickedMdbResetdelaycount()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (!m_pSession || !m_pSession->IsConnect())
	{
		::AfxMessageBox(_T("Session is not connected !"), MB_ICONINFORMATION);
		return;
	}

	m_pSession->GetServiceInfo()->ResetDelayCount(EV_SMT_REAL);
	
}

void CPartitionDlg::OnBnClickedMdbChangepw()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (::IsWindow(m_ChangePasswordDialog.GetSafeHwnd()))
		m_ChangePasswordDialog.ShowWindow((!m_ChangePasswordDialog.IsWindowVisible())?(SW_NORMAL):(SW_HIDE));	
}
