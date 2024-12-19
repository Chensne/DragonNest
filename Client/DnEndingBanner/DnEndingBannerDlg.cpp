// DnEndingBannerDlg.cpp : ���� ����
//

#include "stdafx.h"
#include "DnEndingBanner.h"
#include "DnEndingBannerDlg.h"
#include "DnUtil.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CDnEndingBannerDlg ��ȭ ����

CDnEndingBannerDlg::CDnEndingBannerDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDnEndingBannerDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_dwWidth = 0;
	m_dwHeight = 0;

	int i = 0;
	for (; i < HTMLVIEW_MAX; ++i)
	{
		m_pDnHtmlView[i] = NULL;
	}
}

void CDnEndingBannerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CDnEndingBannerDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_ERASEBKGND()
	ON_WM_DESTROY()
	ON_WM_TIMER()
	ON_WM_CLOSE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


// CDnEndingBannerDlg �޽��� ó����

BOOL CDnEndingBannerDlg::InitSkin()
{
 	CenterWindow();

	return TRUE;
}

BOOL CDnEndingBannerDlg::InitWebPage()
{
	CCreateContext pContext;
	CWnd* pFrameWnd = this;

	pContext.m_pCurrentDoc = NULL;
	pContext.m_pNewViewClass = RUNTIME_CLASS(CDnHtmlView);

	m_pDnHtmlView[HTMLVIEW_MAIN] = (CDnHtmlView*)((CFrameWnd*)pFrameWnd)->CreateView( &pContext );
	if (m_pDnHtmlView[HTMLVIEW_MAIN] == NULL)
		return FALSE;
	m_pDnHtmlView[HTMLVIEW_MAIN]->ShowWindow(SW_SHOW);
	m_pDnHtmlView[HTMLVIEW_MAIN]->MoveWindow(m_CtrlPosData.GetCtrlRect(eDPI_MainWebHtml));
	m_pDnHtmlView[HTMLVIEW_MAIN]->Navigate(MAIN_URL);
	m_pDnHtmlView[HTMLVIEW_MAIN]->SetOwner(this);
	m_pDnHtmlView[HTMLVIEW_MAIN]->ModifyStyleEx(WS_EX_CLIENTEDGE, 0, SWP_FRAMECHANGED);

	return TRUE;
}

#define ID_TIMER 1

BOOL CDnEndingBannerDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// �� ��ȭ ������ �������� �����մϴ�. ���� ���α׷��� �� â�� ��ȭ ���ڰ� �ƴ� ��쿡��
	//  �����ӿ�ũ�� �� �۾��� �ڵ����� �����մϴ�.
	SetIcon(m_hIcon, TRUE);			// ū �������� �����մϴ�.
	SetIcon(m_hIcon, FALSE);		// ���� �������� �����մϴ�.

	this->SetWindowText(_S(IDS_STRING_TITLE));

	InitSkin();

	InitWebPage();

	MoveWindow(m_CtrlPosData.GetCtrlRect(eDPI_MainWebHtml));

	SetTimer(ID_TIMER, 10000, NULL);

	return TRUE;  // ��Ŀ���� ��Ʈ�ѿ� �������� ������ TRUE�� ��ȯ�մϴ�.
}

// ��ȭ ���ڿ� �ּ�ȭ ���߸� �߰��� ��� �������� �׸�����
//  �Ʒ� �ڵ尡 �ʿ��մϴ�. ����/�� ���� ����ϴ� MFC ���� ���α׷��� ��쿡��
//  �����ӿ�ũ���� �� �۾��� �ڵ����� �����մϴ�.

void CDnEndingBannerDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // �׸��⸦ ���� ����̽� ���ؽ�Ʈ

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Ŭ���̾�Ʈ �簢������ �������� ����� ����ϴ�.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// �������� �׸��ϴ�.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// ����ڰ� �ּ�ȭ�� â�� ���� ���ȿ� Ŀ���� ǥ�õǵ��� �ý��ۿ���
//  �� �Լ��� ȣ���մϴ�.
HCURSOR CDnEndingBannerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

BOOL CDnEndingBannerDlg::OnEraseBkgnd( CDC* pDC )
{
	BitBlt(pDC->m_hDC, 0, 0, m_dwWidth, m_dwHeight, m_dcBkGrnd, 0, 0, SRCCOPY);
	return FALSE;
}

void CDnEndingBannerDlg::OnDestroy()
{
}

void CDnEndingBannerDlg::OnClose()
{
	CDialog::OnCancel();
}

void CDnEndingBannerDlg::OnTimer(UINT nIDEvent)
{
	if (nIDEvent == ID_TIMER)
		CDialog::OnCancel();
}

BOOL CDnEndingBannerDlg::PreTranslateMessage(MSG* pMsg)
{
	if ((pMsg->message == WM_KEYDOWN) && (pMsg->wParam == VK_ESCAPE))
		return true;

	if ((pMsg->message == WM_KEYDOWN) && (pMsg->wParam == VK_RETURN))
		return true;

	return CDialog::PreTranslateMessage(pMsg);
}
