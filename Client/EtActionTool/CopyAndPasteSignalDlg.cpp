// CopyAndPasteSignalDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "EtActionTool.h"
#include "CopyAndPasteSignalDlg.h"
#include "SignalManager.h"
#include "SignalItem.h"


// CCopyAndPasteSignalDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(CCopyAndPasteSignalDlg, CXTResizeDialog)

CCopyAndPasteSignalDlg::CCopyAndPasteSignalDlg(CWnd* pParent /*=NULL*/)
	: CXTResizeDialog(CCopyAndPasteSignalDlg::IDD, pParent)
{
}

CCopyAndPasteSignalDlg::~CCopyAndPasteSignalDlg()
{
}

void CCopyAndPasteSignalDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_ListCtrl);
}


BEGIN_MESSAGE_MAP(CCopyAndPasteSignalDlg, CXTResizeDialog)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDOK, &CCopyAndPasteSignalDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CCopyAndPasteSignalDlg::OnBnClickedCancel)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST1, &CCopyAndPasteSignalDlg::OnNMDblclkList1)
	ON_NOTIFY(LVN_ITEMCHANGING, IDC_LIST1, &CCopyAndPasteSignalDlg::OnLvnItemchangingList1)
END_MESSAGE_MAP()

extern bool SignalCompareProc( CSignalItem *pItem1, CSignalItem *pItem2 );

bool SignalEnableCompareProc( CSignalItem *pItem1, CSignalItem *pItem2 )
{
	return ( true == pItem1->IsEnableCopyAndPaste() && false == pItem2->IsEnableCopyAndPaste() );
}

BOOL CCopyAndPasteSignalDlg::OnInitDialog()
{
	CXTResizeDialog::OnInitDialog();

	SetResize( IDC_LIST1, SZ_TOP_LEFT, SZ_BOTTOM_RIGHT );
	SetResize( IDOK, SZ_BOTTOM_RIGHT, SZ_BOTTOM_RIGHT );
	SetResize( IDCANCEL, SZ_BOTTOM_RIGHT, SZ_BOTTOM_RIGHT );

	LoadPlacement( _T("CCopyAndPasteSignalDlg") );

	m_ListCtrl.AddColumn( "Signal Name", 190 );
	m_ListCtrl.AddColumn( "Initials", 40, LVCFMT_CENTER );
	m_ListCtrl.AddColumn( "Copy&Paste Enable", 100, LVCFMT_RIGHT );

	m_ListCtrl.SetExtendedStyle( LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT | LVS_EX_FLATSB );

	ResetList( false );

	return TRUE;
}

// CCopyAndPasteSignalDlg 메시지 처리기입니다.

void CCopyAndPasteSignalDlg::OnDestroy()
{
	CXTResizeDialog::OnDestroy();

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	SavePlacement( _T("CCopyAndPasteSignalDlg") );
}

void CCopyAndPasteSignalDlg::OnBnClickedOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	OnOK();
}

void CCopyAndPasteSignalDlg::OnBnClickedCancel()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	OnCancel();
}

void CCopyAndPasteSignalDlg::OnNMDblclkList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;

	/*POSITION p = m_ListCtrl.GetFirstSelectedItemPosition();
	std::vector<CString> szVecSelectItemList;
	while(p) {
		int nIndex = m_ListCtrl.GetNextSelectedItem(p);
		szVecSelectItemList.push_back( m_ListCtrl.GetItemText( nIndex, 0 ) );
	}
	if( szVecSelectItemList.size() == 0 ) return;

	CSignalItem *pItem = CSignalManager::GetInstance().GetSignalItemFromName( szVecSelectItemList[0] );
	if( !pItem ) return;
	pItem->SetEnableCopyAndPaste( !pItem->IsEnableCopyAndPaste() );

	ResetList( true );*/
}

void CCopyAndPasteSignalDlg::OnLvnItemchangingList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;
}

void CCopyAndPasteSignalDlg::ResetList( bool bOKButton )
{
	m_ListCtrl.DeleteAllItems();

	std::vector<CSignalItem *> pVecList;
	for( DWORD i=0; i<CSignalManager::GetInstance().GetSignalItemCount(); i++ )
		pVecList.push_back( CSignalManager::GetInstance().GetSignalItem(i) );

	std::sort( pVecList.begin(), pVecList.end(), SignalCompareProc );
	std::sort( pVecList.begin(), pVecList.end(), SignalEnableCompareProc );

	CString szStr;
	for( DWORD i=0; i<pVecList.size(); i++ ) {
		CSignalItem *pItem = pVecList[i];
		m_ListCtrl.InsertItem( i, pItem->GetName() );

		szStr.Format( "%c|%d|%d", pItem->GetName()[0], pItem->GetInitialsColor(), pItem->GetBackgroundColor() );
		m_ListCtrl.SetItemText( i, 1, szStr );

		szStr.Format( "%s", pItem->IsEnableCopyAndPaste() ? "true" : "false" );
		m_ListCtrl.SetItemText( i, 2, szStr );
	}

	GetDlgItem( IDOK )->EnableWindow( bOKButton );
}