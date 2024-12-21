// BaseInformationView.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "ServiceMonitor.h"
#include "BaseInformationView.h"
#include "MainFrm.h"


// CBaseInformationView

IMPLEMENT_DYNCREATE(CBaseInformationView, CBaseView)

CBaseInformationView::CBaseInformationView()
	: CBaseView(CBaseInformationView::IDD)
{
	m_IsInitialized = FALSE;
}

CBaseInformationView::~CBaseInformationView()
{
}

void CBaseInformationView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_INFOMERITLIST, m_GridList[EV_SFT_MRT]);
	DDX_Control(pDX, IDC_INFONTLCLIST, m_GridList[EV_SFT_NLC]);
}

BEGIN_MESSAGE_MAP(CBaseInformationView, CFormView)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_LBUTTONDOWN()
	ON_WM_TIMER()
	ON_WM_DESTROY()
	ON_WM_PAINT()
END_MESSAGE_MAP()


// CBaseInformationView �����Դϴ�.

#ifdef _DEBUG
void CBaseInformationView::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CBaseInformationView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG

void CBaseInformationView::DrawTitle(CDC* pDC)
{
	if(!pDC)
		return;

	Graphics aGraphics((*pDC));
	aGraphics.SetSmoothingMode(SmoothingModeHighQuality);

	CRect aRect;
	GetClientRect(&aRect);

	// Draw Title
	{
		// Merit List
		{
			// Gradient Line
			{
				LinearGradientBrush aBrush(
					Rect(
						aRect.left + EV_UIS_TITLELINE_PAD_X,
						aRect.top + EV_UIS_SVCINFOMRTVIEW_PAD_Y - EV_UIS_TITLELINE_PAD_Y,
						EV_UIS_SVCINFOMRTVIEW_WTH,
						EV_UIS_TITLELINE_HGT
					),
					Color(100, 221, 221, 221),
					Color(255, 120, 120, 120),
					180.0f,
					FALSE
					);

				aGraphics.FillRectangle(
					&aBrush, 
					aRect.left + EV_UIS_TITLELINE_PAD_X,
					aRect.top + EV_UIS_SVCINFOMRTVIEW_PAD_Y - EV_UIS_TITLELINE_PAD_Y,
					EV_UIS_SVCINFOMRTVIEW_WTH,
					EV_UIS_TITLELINE_HGT
					);
			}

			// Draw Text
			{
				LOGFONT aLogFont;
				::memset(&aLogFont, 0x00, sizeof(aLogFont));
				_tcsncpy_s(aLogFont.lfFaceName, _T("Arial"), LF_FACESIZE);
				aLogFont.lfCharSet = DEFAULT_CHARSET;
				aLogFont.lfHeight = 20;
				aLogFont.lfWeight = FW_EXTRABOLD;

				CFont aNewFont;
				VERIFY(aNewFont.CreateFontIndirect(&aLogFont));
				CFont* aOldFont = pDC->SelectObject(&aNewFont);

				COLORREF aOldTxColor = pDC->SetTextColor(RGB(255, 255, 255));
				int aOldBkMode = pDC->SetBkMode(TRANSPARENT);

				CRect aRect(
					EV_UIS_TITLELINE_PAD_X + EV_UIS_TITLETEXT_PAD_X, 
					aRect.top + EV_UIS_SVCINFOMRTVIEW_PAD_Y - EV_UIS_TITLELINE_PAD_Y,
					aRect.left  + EV_UIS_SVCINFOMRTVIEW_WTH + EV_UIS_TITLELINE_PAD_X + EV_UIS_TITLETEXT_PAD_X,
					aRect.top + EV_UIS_SVCINFOMRTVIEW_PAD_Y + EV_UIS_TITLELINE_HGT + EV_UIS_TITLELINE_PAD_Y
					);

				pDC->DrawText(_T("Merit List"), &aRect, DT_LEFT | DT_VCENTER);

				pDC->SetTextColor(aOldTxColor);
				pDC->SetBkMode(aOldBkMode);

				pDC->SelectObject(aOldFont);
				aNewFont.DeleteObject();
			}
		}

		// Net Luncher List
		{
			// Gradient Line
			{
				LinearGradientBrush aBrush(
					Rect(
						aRect.left + EV_UIS_TITLELINE_PAD_X,
						aRect.top + EV_UIS_SVCINFOMRTVIEW_HGT + (EV_UIS_SVCINFONLCVIEW_PAD_Y * 2) - EV_UIS_TITLELINE_PAD_Y,
						EV_UIS_SVCINFONLCVIEW_WTH,
						EV_UIS_TITLELINE_HGT
					),
					Color(100, 221, 221, 221),
					Color(255, 120, 120, 120),
					180.0f,
					FALSE
					);

				aGraphics.FillRectangle(
					&aBrush, 
					aRect.left + EV_UIS_TITLELINE_PAD_X,
					aRect.top + EV_UIS_SVCINFOMRTVIEW_HGT + (EV_UIS_SVCINFONLCVIEW_PAD_Y * 2) - EV_UIS_TITLELINE_PAD_Y,
					EV_UIS_SVCINFONLCVIEW_WTH,
					EV_UIS_TITLELINE_HGT
					);
			}

			// Draw Text
			{
				LOGFONT aLogFont;
				::memset(&aLogFont, 0x00, sizeof(aLogFont));
				_tcsncpy_s(aLogFont.lfFaceName, _T("Arial"), LF_FACESIZE);
				aLogFont.lfCharSet = DEFAULT_CHARSET;
				aLogFont.lfHeight = 20;
				aLogFont.lfWeight = FW_EXTRABOLD;

				CFont aNewFont;
				VERIFY(aNewFont.CreateFontIndirect(&aLogFont));
				CFont* aOldFont = pDC->SelectObject(&aNewFont);

				COLORREF aOldTxColor = pDC->SetTextColor(RGB(255, 255, 255));
				int aOldBkMode = pDC->SetBkMode(TRANSPARENT);

				CRect aRect(
					EV_UIS_TITLELINE_PAD_X + EV_UIS_TITLETEXT_PAD_X, 
					aRect.top + EV_UIS_SVCINFOMRTVIEW_HGT + (EV_UIS_SVCINFONLCVIEW_PAD_Y * 2) - EV_UIS_TITLELINE_PAD_Y,
					aRect.left + EV_UIS_SVCINFONLCVIEW_WTH + EV_UIS_TITLELINE_PAD_X + EV_UIS_TITLETEXT_PAD_X,
					aRect.top + EV_UIS_SVCINFOMRTVIEW_HGT + (EV_UIS_SVCINFONLCVIEW_PAD_Y * 2) + EV_UIS_TITLELINE_HGT + EV_UIS_TITLELINE_PAD_Y
					);

				pDC->DrawText(_T("Net Luncher List"), &aRect, DT_LEFT | DT_VCENTER);

				pDC->SetTextColor(aOldTxColor);
				pDC->SetBkMode(aOldBkMode);

				pDC->SelectObject(aOldFont);
				aNewFont.DeleteObject();
			}
		}
	}

	
}

void CBaseInformationView::AddGridList(const MERITINFO* pMeritInfo)
{
	if(!pMeritInfo)
		return;

	if(!pMeritInfo->IsSet())
		return;
	

	COXGridList* aGridList = &m_GridList[EV_SFT_MRT];
	if(!aGridList)
		return;

	BeginWaitCursor();
	aGridList->LockWindowUpdate();

	LV_ITEM aLvItem;
	aLvItem.mask		= LVIF_TEXT | LVIF_PARAM;
	aLvItem.cchTextMax	= 0;
	aLvItem.lParam		= 0;
	aLvItem.iItem		= aGridList->GetItemCount();
	aLvItem.iSubItem	= 0;
	aLvItem.lParam		= pMeritInfo->m_MeritID;	// ���� ���� ���� �˻��� ���� �Ķ���ͷ� ����

	TCHAR sBuffer[MAX_PATH*2] = { _T('\0'), };

	// Merit ID
	_sntprintf_s(sBuffer, _countof(sBuffer), _T("%d"), pMeritInfo->m_MeritID);
	aLvItem.pszText = sBuffer;

	int nRtnValue = aGridList->InsertItem(&aLvItem);
	if(-1 == nRtnValue)
		return;
	
	// Min Level
	_sntprintf_s(sBuffer, _countof(sBuffer), _T("%d"), pMeritInfo->m_MinLevel);
	aGridList->SetItemText(aLvItem.iItem, ++aLvItem.iSubItem, sBuffer);

	// Max Level
	_sntprintf_s(sBuffer, _countof(sBuffer), _T("%d"), pMeritInfo->m_MaxLevel);
	aGridList->SetItemText(aLvItem.iItem, ++aLvItem.iSubItem, sBuffer);

	// Merit Type
	_sntprintf_s(sBuffer, _countof(sBuffer), _T("%d"), pMeritInfo->m_MeritType);
	aGridList->SetItemText(aLvItem.iItem, ++aLvItem.iSubItem, sBuffer);

	// Extend Value
	_sntprintf_s(sBuffer, _countof(sBuffer), _T("%d"), pMeritInfo->m_ExtendValue);
	aGridList->SetItemText(aLvItem.iItem, ++aLvItem.iSubItem, sBuffer);

	aGridList->UnlockWindowUpdate();
	EndWaitCursor();

}

void CBaseInformationView::AddGridList(const NETLUNCHERINFO* pNetLuncherInfo)
{
	if(!pNetLuncherInfo)
		return;

	if(!pNetLuncherInfo->IsSet())
		return;

	COXGridList* aGridList = &m_GridList[EV_SFT_NLC];
	if(!aGridList)
		return;
	

	BeginWaitCursor();
	aGridList->LockWindowUpdate();

	LV_ITEM aLvItem;
//	aLvItem.mask		= LVIF_TEXT | LVIF_IMAGE;
	aLvItem.mask		= LVIF_TEXT | LVIF_PARAM;
	aLvItem.cchTextMax	= 0;
	aLvItem.lParam		= 0;
	aLvItem.iItem		= aGridList->GetItemCount();
	aLvItem.iSubItem	= 0;
//	aLvItem.iImage		= 1;
	aLvItem.lParam		= pNetLuncherInfo->m_NetLuncherID;	// ���� ���� ���� �˻��� ���� �Ķ���ͷ� ����

	TCHAR sBuffer[MAX_PATH*2] = { _T('\0'), };

	// Net Luncher ID
	_sntprintf_s(sBuffer, _countof(sBuffer), _T("%d"), pNetLuncherInfo->m_NetLuncherID);
	aLvItem.pszText = sBuffer;

	int nRtnValue = aGridList->InsertItem(&aLvItem);
	if(-1 == nRtnValue)
		return;
	
	// IP Address
	_sntprintf_s(sBuffer, _countof(sBuffer), _T("%u.%u.%u.%u"), 
		reinterpret_cast<const BYTE*>(&pNetLuncherInfo->m_IpAddr)[0],
		reinterpret_cast<const BYTE*>(&pNetLuncherInfo->m_IpAddr)[1],
		reinterpret_cast<const BYTE*>(&pNetLuncherInfo->m_IpAddr)[2],
		reinterpret_cast<const BYTE*>(&pNetLuncherInfo->m_IpAddr)[3]
		);
	aGridList->SetItemText(aLvItem.iItem, ++aLvItem.iSubItem, sBuffer);

	aGridList->UnlockWindowUpdate();
	EndWaitCursor();

}


// CBaseInformationView �޽��� ó�����Դϴ�.

int CBaseInformationView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFormView::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  ���⿡ Ư��ȭ�� �ۼ� �ڵ带 �߰��մϴ�.

	return 0;
}

void CBaseInformationView::OnSize(UINT nType, int cx, int cy)
{
	CFormView::OnSize(nType, cx, cy);

	CRect aRect;
	GetClientRect(&aRect);

	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.

	if (m_IsInitialized) {
		m_LayoutManager.OnSize(cx,cy);

		// Draw Text
		{
			CClientDC aDC(this);

			DrawTitle(&aDC);
		}
	}
	
//	MoveWindow(CRect(0,0,cx,cy));	// �� ��� �� �信 ���� �����Ƿ� �ڽ��� ��ġ�� �̵���Ű�� �ʵ�
}

void CBaseInformationView::OnUpdate(CView* /*pSender*/, LPARAM /*lHint*/, CObject* /*pHint*/)
{
	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.

	CRect aRect;
	GetClientRect(&aRect);

	if (!m_IsInitialized) {
		m_IsInitialized = TRUE;
		
		// Image List
		{
			VERIFY(m_ImageList.Create(IDB_IMG_SMALL, 16, 11, RGB(255,0,255)));
		}

		// Grid Control
		{
			// Merit List
			{
				int aColNo = 0;

				COXGridList* aGridList = &m_GridList[EV_SFT_MRT];
				if(!aGridList)
					return;

				aGridList->InsertColumn(aColNo++, _T("Merit ID"), LVCFMT_LEFT, 80);
				aGridList->InsertColumn(aColNo++, _T("Min Level"), LVCFMT_LEFT, 80);
				aGridList->InsertColumn(aColNo++, _T("Max Level"), LVCFMT_LEFT, 80);
				aGridList->InsertColumn(aColNo++, _T("Merit Type"), LVCFMT_LEFT, 80);
				aGridList->InsertColumn(aColNo++, _T("Extend Value"), LVCFMT_LEFT, 100);

				aGridList->MoveWindow(
					aRect.left + EV_UIS_SVCINFOVIEW_PAD_A,
					aRect.top + EV_UIS_SVCINFOMRTVIEW_PAD_Y,
					EV_UIS_SVCINFOMRTVIEW_WTH,
					EV_UIS_SVCINFOMRTVIEW_HGT,
					TRUE
					);
			}

			// Net Luncher List
			{
				int aColNo = 0;

				COXGridList* aGridList = &m_GridList[EV_SFT_NLC];
				if(!aGridList)
					return;

				aGridList->InsertColumn(aColNo++, _T("Net Luncher ID"), LVCFMT_LEFT, 100);
				aGridList->InsertColumn(aColNo++, _T("IP Address"), LVCFMT_LEFT, 110);

				aGridList->MoveWindow(
					aRect.left + EV_UIS_SVCINFOVIEW_PAD_A,
					aRect.top + EV_UIS_SVCINFOMRTVIEW_PAD_Y + EV_UIS_SVCINFOMRTVIEW_HGT + EV_UIS_SVCINFONLCVIEW_PAD_Y,
					EV_UIS_SVCINFONLCVIEW_WTH,
					aRect.bottom - (aRect.top + EV_UIS_SVCINFOMRTVIEW_PAD_Y + EV_UIS_SVCINFOMRTVIEW_HGT + EV_UIS_SVCINFONLCVIEW_PAD_Y + EV_UIS_SVCINFOVIEW_PAD_A),
					TRUE
					);
			}
		}

		// Layout Manager
		{
			m_LayoutManager.Attach(this);
 			m_LayoutManager.TieChild(IDC_INFOMERITLIST, OX_LMS_TOP | OX_LMS_LEFT, OX_LMT_SAME);
 			m_LayoutManager.TieChild(IDC_INFONTLCLIST, OX_LMS_TOP | OX_LMS_BOTTOM | OX_LMS_LEFT, OX_LMT_SAME);
		}

		SetTimer(EV_TMD_SHOWETCINFO, g_TimerInterval[EV_TMD_SHOWETCINFO - EV_TIMERID_DEF], NULL);
	}

}

void CBaseInformationView::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.	
	CFormView::OnLButtonDown(nFlags, point);
}

BOOL CBaseInformationView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.

	cs.style &= ~(WS_HSCROLL | WS_VSCROLL);

	return CFormView::PreCreateWindow(cs);
}

void CBaseInformationView::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.

	switch(nIDEvent) {
	case EV_TMD_SHOWETCINFO:
		{
			
		}
		break;
	default:
		break;
	}

	CFormView::OnTimer(nIDEvent);
}

void CBaseInformationView::OnDestroy()
{
	CFormView::OnDestroy();

	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.

	KillTimer(EV_TMD_SHOWETCINFO);
}

void CBaseInformationView::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
	// �׸��� �޽����� ���ؼ��� CBaseView::OnPaint()��(��) ȣ������ ���ʽÿ�.

	DrawTitle(&dc);

}
