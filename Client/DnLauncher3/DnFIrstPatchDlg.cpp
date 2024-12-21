#include "stdafx.h"
#include "resource.h"
#include "DnFIrstPatchDlg.h"
#include "DnControlData.h"

IMPLEMENT_DYNAMIC(CDnFIrstPatchDlg, CDialog)

CDnFIrstPatchDlg::CDnFIrstPatchDlg(CWnd* pParent /*=NULL*/)
: CDialog(CDnFIrstPatchDlg::IDD, pParent)
, m_staticBaseColor(BLACK)
, m_nRectControlMax(em_FIRST_PATCH_MAX)
, m_pFirstRectControl(NULL)
, m_pProgress(NULL)
, m_dwWidth(0)
, m_dwHeight(0)
{
	m_strFirstPatchTitle.Empty();
}

CDnFIrstPatchDlg::~CDnFIrstPatchDlg()
{
	SAFE_DELETE_ARRAY(m_pFirstRectControl);
	SAFE_DELETE(m_pProgress);
}

void CDnFIrstPatchDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	// ProgressBar Ctrl
	//DDX_Control( pDX, IDC_STATIC_FIRSTPATCH_PROGRESS, m_StaticDownloadProgress );
	
	// Text
	DDX_Text( pDX,		IDC_STATIC_FIRSTPATCH_TEXT, m_strFirstPatchTitle );
	DDX_Control( pDX,	IDC_STATIC_FIRSTPATCH_TEXT, m_StaticFirstPatchText );
}


BEGIN_MESSAGE_MAP(CDnFIrstPatchDlg, CDialog)
	ON_WM_TIMER()
	ON_WM_DESTROY()
END_MESSAGE_MAP()


BOOL CDnFIrstPatchDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// * 윈도우 크기 설정 *
	m_dwWidth	= 510;
	m_dwHeight	= 104;
	SetWindowPos( &wndNoTopMost, 0, 0, m_dwWidth, m_dwHeight, SWP_NOZORDER | SWP_NOMOVE | SWP_NOACTIVATE );
		
	// * xml파일에서 컨트롤 위치를 파싱 *
	m_pFirstRectControl = new CRect[m_nRectControlMax];
	if(!m_pFirstRectControl)
	{
		LogWnd::TraceLog(_T("xml파일에서 컨트롤 정보 파싱 에러."));
		return FALSE;
	}
	if( ParsingControlPosition(m_pFirstRectControl, m_nRectControlMax) == E_FAIL )
	{
		LogWnd::TraceLog(_T("FirstPatch Control 셋팅 에러"));
		LogWnd::Log( LogLevel::Error, _T("FirstPatchControlData Parsing Error") );
		return FALSE;
	}

	// * 컨트롤 생성 *
	InitControl();
	
	// * 타이머 설정 *
	SetTimer( IDD_DIALOG_FIRSTPATCH_TIMER, 40, NULL );

	SAFE_DELETE_ARRAY(m_pFirstRectControl);

	return TRUE;
}

HRESULT CDnFIrstPatchDlg::ParsingControlPosition(CRect* pRect, int nMaxSize)
{
	return DNCTRLDATA.SetControlDataFromResource(17000 /*IDR_XML_FIRSTPATCH_DATA*/, FIRSTPATCH_CONTROL_NAME, pRect, nMaxSize);
}

void CDnFIrstPatchDlg::MakeFont()
{
	m_Font.CreateFont( 25, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, 0, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_ROMAN, _T("돋움") );
}

void CDnFIrstPatchDlg::InitControl()
{
	// * Font 생성 *
	MakeFont();

	// Text Control
	m_StaticFirstPatchText.SetTextColor( m_staticBaseColor );
	m_StaticFirstPatchText.SetFont( &m_Font );
	m_StaticFirstPatchText.MoveWindow(&m_pFirstRectControl[em_FIRST_PATCH_TEXT]);
	m_strFirstPatchTitle.Format( _S(STR_LAUNCHER_INITIALIZE + DNPATCHINFO.GetLanguageOffset()) );


	// ProgressBar Control
	/*
	m_pDownloadProgressBitmap = new CBitmap();
	HBITMAP hDownloadBitmap = (HBITMAP)::LoadImage( ::AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_BITMAP_PROGRESS_COUNT), IMAGE_BITMAP, 0, 0, LR_LOADMAP3DCOLORS );
	m_pDownloadProgressBitmap->Attach( hDownloadBitmap );

	m_pDownloadProgress = new CSkinProgress( &m_StaticDownloadProgress, 100, m_pDownloadProgressBitmap );
	m_pDownloadProgress->SetRange( 0, 100 );
	m_StaticDownloadProgress.MoveWindow(&m_pFirstRectControl[em_FIRST_PATCH_PROGRESS_BAR]);
	m_StaticDownloadProgress.SetWindowText( L"" );
	*/

	// 기본 ProgressBar
	m_pProgress = new CProgressCtrl;
	if(!m_pProgress)
	{
		return;
	}
	m_pProgress->Create(WS_CHILD|WS_VISIBLE|PBS_SMOOTH, CRect(10,10,200,30), this, 777);
	m_pProgress->MoveWindow(&m_pFirstRectControl[em_FIRST_PATCH_PROGRESS_BAR]);
	m_pProgress->SetRange(0,100);

	UpdateData( FALSE );
}

void CDnFIrstPatchDlg::OnTimer(UINT_PTR nIDEvent)
{
	UpdateProgress();

	CDialog::OnTimer(nIDEvent);
}

void CDnFIrstPatchDlg::UpdateProgress()
{
	// 그냥 쭉쭉쭉 지나가는 프로그레시브바....
	static int nProgressPercent = 0;
	
	( nProgressPercent < 100 ) ? nProgressPercent++ : nProgressPercent = 0;

	m_pProgress->SetRange(0,100);
	m_pProgress->SetPos(nProgressPercent);	

	/*
	m_pDownloadProgress->SetRange( 0, 100 );
	m_pDownloadProgress->SetPos(nProgressPercent);	
	*/
}

void CDnFIrstPatchDlg::OnDestroy()
{
	CDialog::OnDestroy();

	m_Font.DeleteObject();

	KillTimer(IDD_DIALOG_FIRSTPATCH_TIMER);
}

BOOL CDnFIrstPatchDlg::PreTranslateMessage(MSG* pMsg)
{
	if( pMsg->wParam == VK_RETURN )
	{
		return false;
	}

	if( pMsg->wParam == VK_ESCAPE )
	{
		/*
		CString strMessage;
		strMessage.Format(_T("Exit The Launcher?"));

		int nResponse = AfxMessageBox(strMessage, MB_OKCANCEL, MB_ICONQUESTION);
		if( nResponse == IDOK )
		{
			this->EndDialog(0);
			return true;
		}
		*/
		return false;
	}
	return CDialog::PreTranslateMessage(pMsg);
}