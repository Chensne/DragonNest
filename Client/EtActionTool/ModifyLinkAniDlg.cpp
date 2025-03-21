// ModifyLinkAniDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "EtActionTool.h"
#include "ModifyLinkAniDlg.h"
#include "GlobalValue.h"
#include "ActionObject.h"


// CModifyLinkAniDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(CModifyLinkAniDlg, CDialog)

CModifyLinkAniDlg::CModifyLinkAniDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CModifyLinkAniDlg::IDD, pParent)
	, m_szActionName(_T(""))
	, m_szOriginalAniName(_T(""))
{

}

CModifyLinkAniDlg::~CModifyLinkAniDlg()
{
}

void CModifyLinkAniDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT2, m_szActionName);
	DDX_Text(pDX, IDC_EDIT1, m_szOriginalAniName);
	DDX_Control(pDX, IDC_COMBO1, m_Combo);
}


BEGIN_MESSAGE_MAP(CModifyLinkAniDlg, CDialog)
	ON_BN_CLICKED(IDOK, &CModifyLinkAniDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CModifyLinkAniDlg::OnBnClickedCancel)
END_MESSAGE_MAP()


// CModifyLinkAniDlg 메시지 처리기입니다.

BOOL CModifyLinkAniDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	CActionObject *pRoot = (CActionObject *)CGlobalValue::GetInstance().GetRootObject();
	m_Combo.AddString( " None" );
	for( int i=0; i<pRoot->GetAniCount(); i++ ) {
		m_Combo.AddString( pRoot->GetAniName(i) );
	}
	m_Combo.SetCurSel(0);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CModifyLinkAniDlg::OnBnClickedOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData();
	m_Combo.GetLBText( m_Combo.GetCurSel(), m_szResultString );
	OnOK();
}

void CModifyLinkAniDlg::OnBnClickedCancel()
{
	OnCancel();
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}
