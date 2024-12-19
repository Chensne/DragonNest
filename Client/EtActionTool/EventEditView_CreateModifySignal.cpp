// EventEditView_CreateModifySignal.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "EtActionTool.h"
#include "EventEditView_CreateModifySignal.h"
#include "SignalManager.h"


// CEventEditView_CreateModifySignal

IMPLEMENT_DYNCREATE(CEventEditView_CreateModifySignal, CFormView)

CEventEditView_CreateModifySignal::CEventEditView_CreateModifySignal()
	: CFormView(CEventEditView_CreateModifySignal::IDD)
	, m_szSignalName(_T(""))
	, m_bHasLength(FALSE)
{
	m_pmemDC = NULL;
	m_pdcBitmap = NULL;
}

CEventEditView_CreateModifySignal::~CEventEditView_CreateModifySignal()
{
	DeleteMemDC();
}

void CEventEditView_CreateModifySignal::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_szSignalName);
	DDX_Check(pDX, IDC_CHECK1, m_bHasLength);
	DDX_Control(pDX, IDC_BUTTON1, m_InitialsColorBtn);
	DDX_Control(pDX, IDC_BUTTON2, m_BackgroundColorBtn);
	DDX_XTColorPicker(pDX, IDC_BUTTON1, m_InitialsColor);
	DDX_XTColorPicker(pDX, IDC_BUTTON2, m_BackgroundColor);
	DDV_MaxChars(pDX, m_szSignalName, 32);
}

BEGIN_MESSAGE_MAP(CEventEditView_CreateModifySignal, CFormView)
	ON_WM_PAINT()
	ON_CPN_XT_SELENDOK(IDC_BUTTON1, OnSelEndOkInitialsClr)
	ON_CPN_XT_SELENDOK(IDC_BUTTON2, OnSelEndOkBackgroundClr)
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_CHECK1, &CEventEditView_CreateModifySignal::OnBnClickedCheck1)
	ON_EN_CHANGE(IDC_EDIT1, &CEventEditView_CreateModifySignal::OnEnChangeEdit1)
END_MESSAGE_MAP()


// CEventEditView_CreateModifySignal 진단입니다.

#ifdef _DEBUG
void CEventEditView_CreateModifySignal::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CEventEditView_CreateModifySignal::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// CEventEditView_CreateModifySignal 메시지 처리기입니다.

void CEventEditView_CreateModifySignal::PreProcess()
{
	m_szSignalName = s_pCurrentSignalItem->GetName();
	m_InitialsColor = s_pCurrentSignalItem->GetInitialsColor();
	m_BackgroundColor = s_pCurrentSignalItem->GetBackgroundColor();
	m_bHasLength = s_pCurrentSignalItem->IsHasLength();
	UpdateData( FALSE );
}

bool CEventEditView_CreateModifySignal::PostProcess()
{
	UpdateData();

	if( m_szSignalName.IsEmpty() ) {
		MessageBox( "사용할 시그널 이름을 입력해 주세요.", "에러", MB_OK );
		return false;
	}
	bool bImpossibleChar = false;
	for( int i=0; i<m_szSignalName.GetLength(); i++ ) {
		if( m_szSignalName[i] < 'A' || m_szSignalName[i] > 'z' ) {
			bImpossibleChar = true;
			break;
		}
	}
	if( bImpossibleChar ) {
		MessageBox( "이름에 사용 불가능한 문자가 있습니다.", "에러", MB_OK );
		return false;
	}


	if( CSignalManager::GetInstance().IsExistSignalItem( m_szSignalName, ( s_nStartType == 1 ) ? s_szCurrentSignalName : "" ) == true ) {
		MessageBox( "중복되는 이름이 있습니다.", "에러", MB_OK );
		return false;
	}

	s_pCurrentSignalItem->SetName( m_szSignalName );
	s_pCurrentSignalItem->SetInitialsColor( m_InitialsColor );
	s_pCurrentSignalItem->SetBackgroundColor( m_BackgroundColor );
	s_pCurrentSignalItem->SetHasLength( ( m_bHasLength ) ? true : false );

	return true;
}

void CEventEditView_CreateModifySignal::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	CreateMemDC();

	CRect rcRect = CRect( 0, 0, 170, 110 );
	CBitmap *pOldBitmap;
	pOldBitmap = m_pmemDC->SelectObject( m_pdcBitmap );

	m_pmemDC->FillSolidRect( &rcRect, RGB(255,255,255) );

	CRect rcGrid = CRect( 10, 10, 21, rcRect.bottom - 10 );
	COLORREF Col = RGB( 100, 100, 100 );
	// 구찮으니까 걍 3dRect 로 그린다
	// 가로줄
	for( int i=0; i<14; i++ ) {
		m_pmemDC->Draw3dRect( &rcGrid, Col, Col );
		rcGrid.left += 10;
		rcGrid.right += 10;
	}
	// 세로줄
	rcGrid = CRect( 10, 10, rcRect.right - 10, 30 );
	for( int i=0; i<4; i++ ) {
		m_pmemDC->Draw3dRect( &rcGrid, Col, Col );
		rcGrid.top += 19;
		rcGrid.bottom += 19;
	}
	// 보기싫은줄 지워주구
	rcGrid = CRect( 10, rcRect.bottom - 11, rcRect.right - 10, rcRect.bottom - 10 );
	m_pmemDC->Draw3dRect( &rcGrid, RGB(255,255,255), RGB(255,255,255) );
	rcGrid = CRect( rcRect.right - 11, 10, rcRect.right - 10, rcRect.bottom - 10 );
	m_pmemDC->Draw3dRect( &rcGrid, RGB(255,255,255), RGB(255,255,255) );

	// 그린다.
	rcGrid = CRect( 10 + ( 10 * 4 ), 10 + ( 19 * 2 ), 0, 0 );
	if( m_bHasLength ) rcGrid.right = rcGrid.left + 90;
	else rcGrid.right = rcGrid.left + 10;
	rcGrid.bottom = rcGrid.top + 19;
	m_pmemDC->FillSolidRect( &rcGrid, m_BackgroundColor );

	m_pmemDC->SetBkMode( TRANSPARENT );
	m_pmemDC->SelectStockObject( DEFAULT_GUI_FONT );

	char szStr[2048] = { 0, };
	if( m_bHasLength ) {
		sprintf_s( szStr, m_szSignalName );
	}
	else {
		szStr[0] = m_szSignalName[0];
		_strupr_s( szStr );
	}
	rcGrid += CPoint( 1, 1 );
	m_pmemDC->SetTextColor( RGB(0,0,0) );
	m_pmemDC->DrawText( szStr, &rcGrid, DT_SINGLELINE | DT_CENTER | DT_VCENTER );
	rcGrid -= CPoint( 1, 1 );
	m_pmemDC->SetTextColor( m_InitialsColor );
	m_pmemDC->DrawText( szStr, &rcGrid, DT_SINGLELINE | DT_CENTER | DT_VCENTER );

	CRect rcStatic, rcWnd;
	CPoint cPoint;
	CWnd *pStatic = GetDlgItem( IDC_STATIC_PREVIEW );
	pStatic->GetWindowRect( &rcStatic );
	GetWindowRect( &rcWnd );
	cPoint.x = rcStatic.left - rcWnd.left;
	cPoint.y = rcStatic.top - rcWnd.top;
	cPoint += CPoint( 15, 25 );


	dc.BitBlt( cPoint.x, cPoint.y, rcRect.Width(), rcRect.Height(), m_pmemDC, 0, 0, SRCCOPY );

	m_pmemDC->SelectObject(pOldBitmap);
}

void CEventEditView_CreateModifySignal::OnSelEndOkInitialsClr()
{
	UpdateData();
	Invalidate();
}

void CEventEditView_CreateModifySignal::OnSelEndOkBackgroundClr()
{
	UpdateData();
	Invalidate();
}


void CEventEditView_CreateModifySignal::CreateMemDC()
{
	if( m_pdcBitmap || m_pmemDC ) return;

	m_pmemDC = new CDC;
	m_pdcBitmap = new CBitmap;

	CRect rcRect = CRect( 0, 0, 170, 110 );

	CDC *pDC = GetDC();
	m_pmemDC->CreateCompatibleDC( pDC );
	m_pdcBitmap->CreateCompatibleBitmap( pDC, rcRect.Width(), rcRect.Height() );
	ReleaseDC( pDC );
}

void CEventEditView_CreateModifySignal::DeleteMemDC()
{
	if( m_pdcBitmap ) {
		m_pdcBitmap->DeleteObject();
		SAFE_DELETE( m_pdcBitmap );
	}
	if( m_pmemDC ) {
		m_pmemDC->DeleteDC();
		SAFE_DELETE( m_pmemDC );
	}
}

void CEventEditView_CreateModifySignal::OnSize(UINT nType, int cx, int cy)
{
	__super::OnSize(nType, cx, cy);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	DeleteMemDC();
	CreateMemDC();
}

void CEventEditView_CreateModifySignal::OnBnClickedCheck1()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData();
	Invalidate();
}

void CEventEditView_CreateModifySignal::OnEnChangeEdit1()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// __super::OnInitDialog() 함수를 재지정하고  마스크에 OR 연산하여 설정된
	// ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출해야만
	// 해당 알림 메시지를 보냅니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData();
	Invalidate( FALSE );
}

BOOL CEventEditView_CreateModifySignal::PreTranslateMessage(MSG* pMsg)
{
	// TODO: Add your specialized code here and/or call the base class
	return FALSE;

//	return __super::PreTranslateMessage(pMsg);
}
