// DnScaleDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "DnResourceEditer.h"
#include "DnScaleDlg.h"
#include "DnControlManager.h"


// CDnScaleDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(CDnScaleDlg, CDialog)

CDnScaleDlg::CDnScaleDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDnScaleDlg::IDD, pParent)
{
}

CDnScaleDlg::~CDnScaleDlg()
{
}

void CDnScaleDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SLIDER_WIDTH, m_slideWidth);
	DDX_Control(pDX, IDC_SLIDER_HEIGHT, m_slideHeight);
	DDX_Control(pDX, IDC_EDIT_SCALEX, m_editScaleX);
	DDX_Control(pDX, IDC_EDIT_SCALEY, m_editScaleY);
}


BEGIN_MESSAGE_MAP(CDnScaleDlg, CDialog)
	ON_WM_CREATE()
	ON_WM_HSCROLL()
	ON_WM_VSCROLL()
	ON_EN_CHANGE(IDC_EDIT_SCALEX, &CDnScaleDlg::OnEnChangeEditScalex)
	ON_EN_CHANGE(IDC_EDIT_SCALEY, &CDnScaleDlg::OnEnChangeEditScaley)
END_MESSAGE_MAP()


// CDnScaleDlg 메시지 처리기입니다.

int CDnScaleDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	return 0;
}

BOOL CDnScaleDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 사용 영역을 정한다.
	m_slideWidth.SetRange(50, 700);

	// 최대,최소 값 설정.
	m_slideWidth.SetRangeMin(50);
	m_slideWidth.SetRangeMax(700);


	// 위치 설정
	m_slideWidth.SetPos(700/2);

	// 눈금을 설정.
	m_slideWidth.SetTicFreq(10);

	// 키보드 커서로 이동시 증가 크기 설정
	m_slideWidth.SetLineSize(5);



	//--------------------------------------
	// 사용 영역을 정한다.
	m_slideHeight.SetRange(5, 400);

	// 최대,최소 값 설정.
	m_slideHeight.SetRangeMin(5);
	m_slideHeight.SetRangeMax(400);


	// 위치 설정
	m_slideHeight.SetPos(400/2);

	// 눈금을 설정.
	m_slideHeight.SetTicFreq(10);

	// 키보드 커서로 이동시 증가 크기 설정
	m_slideHeight.SetLineSize(5);

	return TRUE;  // return TRUE unless you set the focus to a control
}

void CDnScaleDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// 가로 사이즈
	int nSlidePos = m_slideWidth.GetPos();
	RECT ControlRT;

	if(DNCTRLMANAGER.m_pCurrentControl)
	{
		DNCTRLMANAGER.m_pCurrentControl->GetClientRect(&ControlRT);
		const int nBottomPos = ControlRT.bottom;
		DNCTRLMANAGER.m_pCurrentControl->SetWindowPos(NULL, ControlRT.left, ControlRT.top, nSlidePos, nBottomPos, SWP_NOMOVE);
		
		CString szWinText;
		szWinText.Format(_T("%d"),nSlidePos);
		m_editScaleX.SetWindowText(szWinText);
	}

	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CDnScaleDlg::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// 세로
	int nSlidePos = m_slideHeight.GetPos();
	RECT ControlRT;
	
	if(DNCTRLMANAGER.m_pCurrentControl)
	{
		DNCTRLMANAGER.m_pCurrentControl->GetClientRect(&ControlRT);
		const int nRightPos = ControlRT.right;
		DNCTRLMANAGER.m_pCurrentControl->SetWindowPos(NULL, ControlRT.left, ControlRT.top, nRightPos, nSlidePos, SWP_NOMOVE);
		
		CString szWinText;
		szWinText.Format(_T("%d"), nSlidePos);
		m_editScaleY.SetWindowText(szWinText);
	}

	CDialog::OnVScroll(nSBCode, nPos, pScrollBar);
}

void CDnScaleDlg::OnEnChangeEditScalex()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CDialog::OnInitDialog() 함수를 재지정하고  마스크에 OR 연산하여 설정된
	// ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출해야만
	// 해당 알림 메시지를 보냅니다.

	CString szEditValue;
	m_editScaleX.GetWindowText(szEditValue);
	int nValue = _ttoi(szEditValue);
	m_slideWidth.SetPos(nValue);
	
	
	int nSlidePos = nValue;
	RECT ControlRT;
	DNCTRLMANAGER.m_pCurrentControl->GetClientRect(&ControlRT);
	const int nBottomPos = ControlRT.bottom;
	DNCTRLMANAGER.m_pCurrentControl->SetWindowPos(NULL, ControlRT.left, ControlRT.top, nSlidePos, nBottomPos, SWP_NOMOVE);
}

void CDnScaleDlg::OnEnChangeEditScaley()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CDialog::OnInitDialog() 함수를 재지정하고  마스크에 OR 연산하여 설정된
	// ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출해야만
	// 해당 알림 메시지를 보냅니다.

	CString szEditValue;
	m_editScaleY.GetWindowText(szEditValue);
	int nValue = _ttoi(szEditValue);
	m_slideHeight.SetPos(nValue);


	int nSlidePos = nValue;
	RECT ControlRT;
	DNCTRLMANAGER.m_pCurrentControl->GetClientRect(&ControlRT);
	const int nRightPos = ControlRT.right;
	DNCTRLMANAGER.m_pCurrentControl->SetWindowPos(NULL, ControlRT.left, ControlRT.top, nRightPos, nSlidePos, SWP_NOMOVE);
}
