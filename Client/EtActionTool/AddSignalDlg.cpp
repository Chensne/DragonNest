// AddSignalDlg.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "EtActionTool.h"
#include "AddSignalDlg.h"
#include "SignalManager.h"
#include "SignalItem.h"


// CAddSignalDlg ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(CAddSignalDlg, CXTResizeDialog)

CAddSignalDlg::CAddSignalDlg(CWnd* pParent /*=NULL*/)
	: CXTResizeDialog(CAddSignalDlg::IDD, pParent)
{
	m_nSelectSignalIndex = -1;
}

CAddSignalDlg::~CAddSignalDlg()
{
}

void CAddSignalDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_ListCtrl);
}


BEGIN_MESSAGE_MAP(CAddSignalDlg, CXTResizeDialog)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDOK, &CAddSignalDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CAddSignalDlg::OnBnClickedCancel)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST1, &CAddSignalDlg::OnNMDblclkList1)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST1, &CAddSignalDlg::OnLvnItemchangedList1)
END_MESSAGE_MAP()


// CAddSignalDlg �޽��� ó�����Դϴ�.

extern bool SignalCompareProc( CSignalItem *pItem1, CSignalItem *pItem2 );

BOOL CAddSignalDlg::OnInitDialog()
{
	CXTResizeDialog::OnInitDialog();

	// TODO:  ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.
	SetResize( IDC_LIST1, SZ_TOP_LEFT, SZ_BOTTOM_RIGHT );
	SetResize( IDOK, SZ_BOTTOM_RIGHT, SZ_BOTTOM_RIGHT );
	SetResize( IDCANCEL, SZ_BOTTOM_RIGHT, SZ_BOTTOM_RIGHT );

	LoadPlacement( _T("CAddSignalDlg") );

	m_ListCtrl.AddColumn( "Signal Name", 190 );
	m_ListCtrl.AddColumn( "Initials", 40, LVCFMT_CENTER );

	m_ListCtrl.SetExtendedStyle( LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT|LVS_EX_FLATSB );

	// ������ �μ�Ʈ~
	m_ListCtrl.DeleteAllItems();

	std::vector<CSignalItem *> pVecList;
	for( DWORD i=0; i<CSignalManager::GetInstance().GetSignalItemCount(); i++ ) {
		pVecList.push_back( CSignalManager::GetInstance().GetSignalItem(i) );
	}
	std::sort( pVecList.begin(), pVecList.end(), SignalCompareProc );

	CString szStr;
	for( DWORD i=0; i<pVecList.size(); i++ ) {
		CSignalItem *pItem = pVecList[i];
		m_ListCtrl.InsertItem( i, pItem->GetName() );

		szStr.Format( "%c|%d|%d", pItem->GetName()[0], pItem->GetInitialsColor(), pItem->GetBackgroundColor() );
		m_ListCtrl.SetItemText( i, 1, szStr );
	}

	GetDlgItem( IDOK )->EnableWindow( FALSE );
	return TRUE;  // return TRUE unless you set the focus to a control
	// ����: OCX �Ӽ� �������� FALSE�� ��ȯ�ؾ� �մϴ�.
}

void CAddSignalDlg::OnDestroy()
{
	CXTResizeDialog::OnDestroy();

	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
	SavePlacement( _T("CAddSignalDlg") );
}

void CAddSignalDlg::OnBnClickedOk()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	POSITION p = m_ListCtrl.GetFirstSelectedItemPosition();
	std::vector<CString> szVecSelectItemList;
	while(p) {
		int nIndex = m_ListCtrl.GetNextSelectedItem(p);
		szVecSelectItemList.push_back( m_ListCtrl.GetItemText( nIndex, 0 ) );
	}
	if( szVecSelectItemList.size() == 0 ) return;

	CSignalItem *pItem = CSignalManager::GetInstance().GetSignalItemFromName( szVecSelectItemList[0] );
	if( !pItem ) return;
	m_nSelectSignalIndex = pItem->GetUniqueIndex();

	OnOK();
}

void CAddSignalDlg::OnBnClickedCancel()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	OnCancel();
}

void CAddSignalDlg::OnNMDblclkList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	OnBnClickedOk();
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	*pResult = 0;
}

void CAddSignalDlg::OnLvnItemchangedList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	POSITION p = m_ListCtrl.GetFirstSelectedItemPosition();
	int nIndex = -1;
	while(p) {
		nIndex = m_ListCtrl.GetNextSelectedItem(p);
	}

	if( nIndex == -1 ) GetDlgItem( IDOK )->EnableWindow( FALSE );
	else GetDlgItem( IDOK )->EnableWindow( TRUE );

	*pResult = 0;
}
