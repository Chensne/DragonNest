// ConnectionDialog.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "ServiceMonitor.h"
#include "ConnectionDialog.h"
#include "SubControlBaseView.h"
#include "ConfigMngr.h"
#include "NetConnection.h"
#include "VarArg.hpp"
#include "MainFrm.h"
#include "NetConnection.h"
#include "BasePartitionView.h"

// CConnectionDialog 대화 상자입니다.

IMPLEMENT_DYNAMIC(CConnectionDialog, CDialog)

CConnectionDialog::CConnectionDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CConnectionDialog::IDD, pParent)
{
}

CConnectionDialog::~CConnectionDialog()
{
}

void CConnectionDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_IPADDRESS_SVMN, m_IpAddress);
	DDX_Control(pDX, IDC_PORT_SVMN, m_PortNo);
	DDX_Control(pDX, IDC_ID_SVMN, m_ID);
	DDX_Control(pDX, IDC_PASS_SVMN, m_Pass);	
}


BEGIN_MESSAGE_MAP(CConnectionDialog, CDialog)
	ON_WM_TIMER()
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_CNNDLG_CONNECT, &CConnectionDialog::OnBnClickedButtonConnect)
	ON_BN_CLICKED(IDC_CNNDLG_DISCONNECT, &CConnectionDialog::OnBnClickedButtonDisconnect)
	ON_BN_CLICKED(IDC_CNNDLG_CANCEL, &CConnectionDialog::OnBnClickedCnndlgCancel)
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_WM_LBUTTONDOWN()
	ON_WM_CLOSE()
END_MESSAGE_MAP()


void CConnectionDialog::SaveConfig()
{

	{
		DWORD aIpAddress;

		m_IpAddress.GetAddress(aIpAddress);

		CConfigMngr::GetInstancePtr()->m_IpAddress.Set(static_cast<UINT>(::htonl(aIpAddress)));
	}
	{
		USES_CONVERSION;

		TCHAR aPortNo[8] = { _T('\0'), };

		m_PortNo.GetWindowText(aPortNo, _countof(aPortNo));

		CConfigMngr::GetInstancePtr()->m_PortNo = static_cast<WORD>(_ttoi(aPortNo));
	}
	{
		USES_CONVERSION;

		TCHAR aID[SMIDLENMAX+1] = { _T('\0'), };

		m_ID.GetWindowText(aID, _countof(aID));

		CConfigMngr::GetInstancePtr()->m_ID = aID;
	}
}

void CConnectionDialog::EnableButton(BOOL pIsConnect)
{
	GetDlgItem(IDC_CNNDLG_CONNECT)->EnableWindow(!pIsConnect);
	GetDlgItem(IDC_CNNDLG_DISCONNECT)->EnableWindow(pIsConnect);
}

CString CConnectionDialog::GetID() const
{
	CString aRetVal;

	TCHAR aID[SMIDLENMAX+1] = { _T('\0'), };
	
	m_ID.GetWindowText(aID, _countof(aID));

	aRetVal = aID;

	return aRetVal;
}

CString CConnectionDialog::GetPass() const
{
	CString aRetVal;

	TCHAR aPass[SMPASSLENMAX+1] = { _T('\0'), };
	
	m_Pass.GetWindowText(aPass, _countof(aPass));

	aRetVal = aPass;

	return aRetVal;
}


// CConnectionDialog 메시지 처리기입니다.

BOOL CConnectionDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.

	CenterWindow();

	{
		m_IpAddress.SetAddress(::htonl(CConfigMngr::GetInstancePtr()->m_IpAddress.GetDecimal()));
	}
	{
		USES_CONVERSION;

		m_PortNo.SetWindowText(I2T(CConfigMngr::GetInstancePtr()->m_PortNo));
	}
	{
		m_ID.SetWindowText(CConfigMngr::GetInstancePtr()->m_ID);
	}

	SetTimer(EV_TMD_UPDATECONNECTDIALOG, g_TimerInterval[EV_TMD_UPDATECONNECTDIALOG - EV_TIMERID_DEF], NULL);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

BOOL CConnectionDialog::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	switch(pMsg->message) {
	case WM_KEYDOWN:
		{
			switch(pMsg->wParam) {
			case VK_ESCAPE:
//				return TRUE;	// ESC 키에 대한 연결창 해제는 허용 (CDialog::OnCancel() 호출도 함께 유지되어야 함)
				break;
			}
		}
		break;
	}

	return CDialog::PreTranslateMessage(pMsg);
}

void CConnectionDialog::OnOK()
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

//	CDialog::OnOK();
}

void CConnectionDialog::OnCancel()
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	SaveConfig();

	CDialog::OnCancel();
}

void CConnectionDialog::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	switch(nIDEvent) {
	case EV_TMD_UPDATECONNECTDIALOG:
		{
			DWORD aIpAddress = 0;
			m_IpAddress.GetAddress(aIpAddress);
			ULONG nConID = ::htonl(aIpAddress);

			CNetConnection* pNetConnection = CNetConnectionMgr::GetInstancePtr()->GetConnection (nConID);

			if (pNetConnection) // 기존에 접속한 경우
			{
				CNetSession* pNetSession = pNetConnection->GetSession();
				if (!pNetSession)
					BASE_RETURN_NONE;

				if (pNetSession->IsConnect()) 
				{
					if (GetDlgItem(IDC_CNNDLG_CONNECT)->IsWindowEnabled())
						GetDlgItem(IDC_CNNDLG_CONNECT)->EnableWindow(FALSE);
					
					if (!GetDlgItem(IDC_CNNDLG_DISCONNECT)->IsWindowEnabled())
						GetDlgItem(IDC_CNNDLG_DISCONNECT)->EnableWindow(TRUE);
					
				}
				else
				{
					if (!GetDlgItem(IDC_CNNDLG_CONNECT)->IsWindowEnabled())
						GetDlgItem(IDC_CNNDLG_CONNECT)->EnableWindow(TRUE);
					
					if (GetDlgItem(IDC_CNNDLG_DISCONNECT)->IsWindowEnabled())
						GetDlgItem(IDC_CNNDLG_DISCONNECT)->EnableWindow(FALSE);
				}

			}
			else // 새로운 주소인 경우
			{
				GetDlgItem(IDC_CNNDLG_CONNECT)->EnableWindow(TRUE);
				GetDlgItem(IDC_CNNDLG_DISCONNECT)->EnableWindow(TRUE);
			}
		}
		break;
	default:
		break;
	}

	CDialog::OnTimer(nIDEvent);
}

void CConnectionDialog::OnDestroy()
{
	CDialog::OnDestroy();

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.

	KillTimer(EV_TMD_UPDATECONNECTDIALOG);
}

void CConnectionDialog::OnBnClickedButtonConnect()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	DWORD aIpAddress = 0;
	TCHAR aPortNo[8] = { _T('\0'), };
	{
		m_IpAddress.GetAddress(aIpAddress);

		m_PortNo.GetWindowText(aPortNo, _countof(aPortNo));
	}

	if (INADDR_ANY == aIpAddress || INADDR_NONE == aIpAddress)
	{
		::AfxMessageBox(_T("IP Address is not valid !"), MB_ICONERROR);
		return;
	}

	if (0 == aPortNo) 
	{
		::AfxMessageBox(_T("Port Number is not valid !"), MB_ICONERROR);
		return;
	}

	ULONG nConID = ::htonl(aIpAddress);
	CNetConnection* pNetConnection = CNetConnectionMgr::GetInstancePtr()->GetConnection (nConID);

	if (pNetConnection) // 기존에 접속한 경우
	{
		CNetSession* pNetSession = pNetConnection->GetSession();
		if (!pNetSession)
			BASE_RETURN_NONE;

		DWORD dwRetVal = pNetSession->Connect(::htonl(static_cast<UINT>(aIpAddress)), static_cast<WORD>(_ttoi(aPortNo)));
		if (NOERROR != dwRetVal)
			BASE_RETURN_NONE;
	}
	else	// 처음접속하는 경우
	{	
		// 아래 호출 순서대로 해야 정상작동합니다.
		pNetConnection = CNetConnectionMgr::GetInstancePtr()->CreateConnection();
		if (!pNetConnection)
			BASE_RETURN_NONE;

		CNetSession* pNetSession = pNetConnection->GetSession();
		if (!pNetSession)
			BASE_RETURN_NONE;

		CMainFrame* pMainFrame = static_cast<CServiceMonitorApp*>(::AfxGetApp())->GetMainFrame();
		if (!pMainFrame)
			BASE_RETURN_NONE;

		CBasePartitionView* pPartitionView = pMainFrame->AddPartition (pNetConnection, nConID);
		if (!pPartitionView)
			BASE_RETURN_NONE;

		DWORD dwRetVal = pNetSession->Connect(::htonl(static_cast<UINT>(aIpAddress)), static_cast<WORD>(_ttoi(aPortNo)));
		if (NOERROR != dwRetVal)
			BASE_RETURN_NONE;

		pNetSession->SetConnect(true);

		// 커낵션 아이디 저장
		pPartitionView->SetConnectionID(nConID);

		// 커넥션등록
		CNetConnectionMgr::GetInstancePtr()->RegistConnection (pNetConnection);
	}

	GetDlgItem(IDC_CNNDLG_CONNECT)->EnableWindow(FALSE);
}

void CConnectionDialog::OnBnClickedButtonDisconnect()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	DWORD aIpAddress = 0;
	TCHAR aPortNo[8] = { _T('\0'), };
	{
		m_IpAddress.GetAddress(aIpAddress);

		m_PortNo.GetWindowText(aPortNo, _countof(aPortNo));
	}

	if (INADDR_ANY == aIpAddress || INADDR_NONE == aIpAddress)
	{
		::AfxMessageBox(_T("IP Address is not valid !"), MB_ICONERROR);
		return;
	}

	if (0 == aPortNo) 
	{
		::AfxMessageBox(_T("Port Number is not valid !"), MB_ICONERROR);
		return;
	}

	ULONG nConID = ::htonl(aIpAddress);
	CNetConnection* pNetConnection = CNetConnectionMgr::GetInstancePtr()->GetConnection (nConID);

	if (pNetConnection) // 기존에 접속한 경우
	{
		CNetSession* pNetSession = pNetConnection->GetSession();
		if (!pNetSession)
			BASE_RETURN_NONE;

		if (pNetSession->IsConnect()) 
		{
			pNetSession->SetConnect(false);
			pNetSession->Disconnect();
			EnableButton(FALSE);
		}
		else
			AfxMessageBox(_T("Not Connected !"), MB_ICONERROR);
		
	}
	else // 새로운 주소인 경우
	{
		AfxMessageBox(_T("Not Connected !"), MB_ICONERROR);
	}
}

void CConnectionDialog::OnBnClickedCnndlgCancel()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CConnectionDialog::OnCancel();
}

void CConnectionDialog::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	// 그리기 메시지에 대해서는 CDialog::OnPaint()을(를) 호출하지 마십시오.

	Graphics aGraphics(dc);
	aGraphics.SetSmoothingMode(SmoothingModeHighQuality);

	CRect aRect;
	GetClientRect(&aRect);

	// Draw Title
	{
		// Gradient Line
		{
			// Title 1
			{
				LinearGradientBrush aBrush(
					Rect(
						EV_UIS_TITLELINE_1_PAD_LFT, 
						EV_UIS_TITLELINE_1_PAD_TOP, 
						EV_UIS_TITLELINE_1_WTH, 
						EV_UIS_TITLELINE_1_HGT
					),
					Color(100, 221, 221, 221),
					Color(255, 120, 120, 120),
					180.0f,
					FALSE
					);

				aGraphics.FillRectangle(
					&aBrush, 
					EV_UIS_TITLELINE_1_PAD_LFT, 
					EV_UIS_TITLELINE_1_PAD_TOP, 
					EV_UIS_TITLELINE_1_WTH - 1, 
					EV_UIS_TITLELINE_1_HGT
					);
			}

			// Title 2
			{
				LinearGradientBrush aBrush(
					Rect(
						EV_UIS_TITLELINE_2_PAD_LFT, 
						EV_UIS_TITLELINE_2_PAD_TOP, 
						EV_UIS_TITLELINE_2_WTH, 
						EV_UIS_TITLELINE_2_HGT
					),
					Color(100, 221, 221, 221),
					Color(255, 120, 120, 120),
					180.0f,
					FALSE
					);

				aGraphics.FillRectangle(
					&aBrush, 
					EV_UIS_TITLELINE_2_PAD_LFT, 
					EV_UIS_TITLELINE_2_PAD_TOP, 
					EV_UIS_TITLELINE_2_WTH - 1, 
					EV_UIS_TITLELINE_2_HGT
					);
			}
		}

		// Draw Text
		{
			LOGFONT aLogFont;
			::memset(&aLogFont, 0x00, sizeof(aLogFont));
			_tcsncpy_s(aLogFont.lfFaceName, _T("Arial"), LF_FACESIZE);
			aLogFont.lfCharSet = DEFAULT_CHARSET;
			aLogFont.lfHeight = 16;
			aLogFont.lfWeight = FW_ULTRABOLD;

			CFont aNewFont;
			VERIFY(aNewFont.CreateFontIndirect(&aLogFont));
			CFont* aOldFont = dc.SelectObject(&aNewFont);

			COLORREF aOldTxColor = dc.SetTextColor(RGB(255, 255, 255));
			int aOldBkMode = dc.SetBkMode(TRANSPARENT);

			// Title 1
			{
				CRect aRect(
					EV_UIS_TITLELINE_1_PAD_LFT + EV_UIS_TITLETEXT_1_PAD_X, 
					EV_UIS_TITLELINE_1_PAD_TOP, 
					EV_UIS_TITLELINE_1_PAD_LFT + EV_UIS_TITLELINE_1_WTH, 
					EV_UIS_TITLELINE_1_PAD_TOP + EV_UIS_TITLELINE_1_HGT
					);

				dc.DrawText(_T("Dragon Nest"), &aRect, DT_LEFT | DT_VCENTER);
			}

			// Title 2
			{
				CRect aRect(
					EV_UIS_TITLELINE_2_PAD_LFT + EV_UIS_TITLETEXT_2_PAD_X, 
					EV_UIS_TITLELINE_2_PAD_TOP, 
					EV_UIS_TITLELINE_2_PAD_LFT + EV_UIS_TITLELINE_2_WTH, 
					EV_UIS_TITLELINE_2_PAD_TOP + EV_UIS_TITLELINE_2_HGT
					);

				dc.DrawText(_T("Service Monitor"), &aRect, DT_LEFT | DT_VCENTER);
			}

			dc.SetTextColor(aOldTxColor);
			dc.SetBkMode(aOldBkMode);

			dc.SelectObject(aOldFont);
			aNewFont.DeleteObject();
		}


	}


}

void CConnectionDialog::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
}

void CConnectionDialog::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	CDialog::OnLButtonDown(nFlags, point);

	PostMessage(WM_NCLBUTTONDOWN, HTCAPTION, MAKELPARAM(point.x, point.y));	// WM_NCHITTEST 대신 사용 (마우스 버튼 이벤트 받기 위함)
}

void CConnectionDialog::OnClose()
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	SaveConfig();

	CDialog::OnClose();
}
