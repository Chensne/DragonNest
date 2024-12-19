// ModifyEventDlg.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "EtActionTool.h"
#include "ModifyEventDlg.h"

#include "WizardCommon.h"
#include "UnionValueProperty.h"


// CModifyEventDlg ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(CModifyEventDlg, CDialog)

CModifyEventDlg::CModifyEventDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CModifyEventDlg::IDD, pParent)
{
	m_pCurrentView = NULL;
	m_CurrentViewType = CWizardCommon::UNKNOWN_TYPE;
}

CModifyEventDlg::~CModifyEventDlg()
{
}

void CModifyEventDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BACK, m_BackButton);
	DDX_Control(pDX, IDC_NEXT, m_NextButton);
	DDX_Control(pDX, IDOK, m_FinishButton);
}


BEGIN_MESSAGE_MAP(CModifyEventDlg, CDialog)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_NEXT, &CModifyEventDlg::OnBnClickedNext)
	ON_BN_CLICKED(IDC_BACK, &CModifyEventDlg::OnBnClickedBack)
	ON_BN_CLICKED(IDOK, &CModifyEventDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CModifyEventDlg::OnBnClickedCancel)
	ON_WM_DESTROY()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()


// CModifyEventDlg �޽��� ó�����Դϴ�.

BOOL CModifyEventDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	CWizardCommon::s_nStartType = -1;
	// TODO:  ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.
//	int nCount = sizeof(g_SignalWizardStructDefine) / sizeof(SignalWizardStruct);
	for( int i=0; ; i++ ) {
		if( g_SignalWizardStructDefine[i].pRuntimeClass == NULL ) break;
		CFrameWnd * pFrame = new CFrameWnd;

		CCreateContext context;
		context.m_pNewViewClass = g_SignalWizardStructDefine[i].pRuntimeClass;
		context.m_pCurrentDoc = NULL;

		DWORD dwStyle;
		dwStyle = WS_CHILD|WS_VISIBLE|WS_CLIPCHILDREN|WS_CLIPSIBLINGS;
		pFrame->Create( NULL, NULL, dwStyle, CRect(0, 0, 0, 0), this, 0, 0, &context );
		pFrame->ModifyStyleEx( WS_EX_WINDOWEDGE | WS_EX_CLIENTEDGE | WS_EX_STATICEDGE | WS_EX_OVERLAPPEDWINDOW, 0 );
		pFrame->ModifyStyle( WS_BORDER | WS_THICKFRAME, 0 );
		pFrame->SendMessageToDescendants( WM_INITIALUPDATE, 0, 0, TRUE, TRUE );	

		CWnd *pChild = pFrame->GetWindow(GW_CHILD);
		if( pChild ) {
			pChild->ModifyStyle( DS_3DLOOK, 0 );
			pChild->ModifyStyleEx( WS_EX_CLIENTEDGE, 0 );
		}

		pFrame->ShowWindow( SW_HIDE );
		g_SignalWizardStructDefine[i].pThis = pFrame;
	}
	SetCurrentView( m_CurrentViewType );

	return TRUE;  // return TRUE unless you set the focus to a control
	// ����: OCX �Ӽ� �������� FALSE�� ��ȯ�ؾ� �մϴ�.
}


int CModifyEventDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  ���⿡ Ư��ȭ�� �ۼ� �ڵ带 �߰��մϴ�.

	return 0;
}

void CModifyEventDlg::SetCurrentView( CWizardCommon::VIEW_TYPE Type, bool bPreview )
{
//	int nCount = sizeof(g_SignalWizardStructDefine) / sizeof(SignalWizardStruct);
	for( int i=0; ; i++ ) {
		if( g_SignalWizardStructDefine[i].pRuntimeClass == NULL ) break;
		if( g_SignalWizardStructDefine[i].ViewType == Type ) {
			CWizardCommon::VIEW_TYPE PrevViewType = m_CurrentViewType;
			m_CurrentViewType = Type;
			if( g_SignalWizardStructDefine[i].pThis ) {

				m_pCurrentView = g_SignalWizardStructDefine[i].pThis;
				CRect rcRect;
				GetClientRect( &rcRect );
				m_pCurrentView->MoveWindow( &CRect( 188, 0, rcRect.Width(), rcRect.Height() - 43 ) );
				m_pCurrentView->ShowWindow( SW_SHOW );

				CWnd *pWnd = ((CFrameWnd*)m_pCurrentView)->GetWindow(GW_CHILD);
				CWizardCommon *pWizard = dynamic_cast<CWizardCommon *>(pWnd);

				if( !bPreview ) pWizard->SetPrevViewType( PrevViewType );

				m_NextButton.EnableWindow( pWizard->IsVisivleNextButton() );
				m_BackButton.EnableWindow( pWizard->IsVisivlePrevButton() );
				m_FinishButton.EnableWindow( pWizard->IsVisivleFinishButton() );
				if( pWizard->IsFinishOrCancel() == true )
					m_FinishButton.SetWindowText( "Finish" );
				else m_FinishButton.SetWindowText( "Cancel" );

				pWnd->SetFocus();

				pWizard->PreProcess();

			}
		}
		else {
			if( g_SignalWizardStructDefine[i].pThis ) 
				g_SignalWizardStructDefine[i].pThis->ShowWindow( SW_HIDE );
		}
	}
}

void CModifyEventDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
	if( m_pCurrentView ) {
		m_pCurrentView->MoveWindow( &CRect( 188, 0, cx, cy - 43 ) );
	}
}

void CModifyEventDlg::OnBnClickedNext()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CWnd *pWnd = ((CFrameWnd*)m_pCurrentView)->GetWindow(GW_CHILD);
	CWizardCommon *pWizard = dynamic_cast<CWizardCommon *>(pWnd);

	CWizardCommon::VIEW_TYPE ViewType = pWizard->GetNextView();

	if( ViewType == CWizardCommon::UNKNOWN_TYPE ) return;
	if( pWizard->PostProcess() == false ) {
		pWnd->SetFocus();
		return;
	}
	SetCurrentView( ViewType );

}

void CModifyEventDlg::OnBnClickedBack()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CWnd *pWnd = ((CFrameWnd*)m_pCurrentView)->GetWindow(GW_CHILD);
	CWizardCommon *pWizard = dynamic_cast<CWizardCommon *>(pWnd);

	CWizardCommon::VIEW_TYPE ViewType = pWizard->GetPrevView();

	if( ViewType == CWizardCommon::UNKNOWN_TYPE ) return;
	SetCurrentView( ViewType, true );
}

void CModifyEventDlg::OnBnClickedOk()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CString szStr;
	m_FinishButton.GetWindowText( szStr );
	if( _stricmp( szStr, "Cancel" ) == NULL ) {
		OnBnClickedCancel();
		return;
	}
	CWnd *pWnd = ((CFrameWnd*)m_pCurrentView)->GetWindow(GW_CHILD);
	CWizardCommon *pWizard = dynamic_cast<CWizardCommon *>(pWnd);

	if( pWizard->PostProcess() == false ) {
		pWnd->SetFocus();
		return;
	}

	OnOK();
}

void CModifyEventDlg::OnBnClickedCancel()
{
	switch( CWizardCommon::s_nStartType ) {
		case 0:
		case 1:
			SAFE_DELETE( CWizardCommon::s_pCurrentSignalItem );
			break;
		case 2:
			break;
		case 3:
		case 4:
			CWizardCommon::s_pCurrentSignalItem = NULL;
			SAFE_DELETE( CWizardCommon::s_pCurrentPropertyVariable );
			break;
		case 5:
			CWizardCommon::s_pCurrentSignalItem = NULL;
			break;
	}
	OnCancel();
}


void CModifyEventDlg::OnDestroy()
{
	CDialog::OnDestroy();

	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
//	int nCount = sizeof(g_SignalWizardStructDefine) / sizeof(SignalWizardStruct);
	for( int i=0;; i++ ) {
		if( g_SignalWizardStructDefine[i].pRuntimeClass == NULL ) break;
		g_SignalWizardStructDefine[i].pThis->DestroyWindow();
		g_SignalWizardStructDefine[i].pThis = NULL;
	}
}

BOOL CModifyEventDlg::OnEraseBkgnd(CDC* pDC)
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.

	return CDialog::OnEraseBkgnd(pDC);
}

