// StringSelect.cpp : implementation file
//

#include "stdafx.h"
#include "EtUITool.h"
#include "StringSelect.h"
#include "../../Common/EtStringManager/EtUIXML.h"

// CStringSelect dialog

IMPLEMENT_DYNAMIC(CStringSelect, CDialog)

CStringSelect::CStringSelect(CWnd* pParent /*=NULL*/)
	: CDialog(CStringSelect::IDD, pParent)
	, m_nSelectItem(-1)
	, m_nStringIndex(-1)
{
}

CStringSelect::~CStringSelect()
{
}

void CStringSelect::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_STRING, m_ctrlListString);
	DDX_Control(pDX, IDC_EDIT_FIND, m_ctrlEditFind);
}


BEGIN_MESSAGE_MAP(CStringSelect, CDialog)
	ON_BN_CLICKED(IDOK, &CStringSelect::OnBnClickedOk)
	ON_BN_CLICKED(IDC_BUTTON_FIND, &CStringSelect::OnBnClickedButtonFind)
	ON_WM_SHOWWINDOW()
END_MESSAGE_MAP()


// CStringSelect message handlers

BOOL CStringSelect::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_ctrlListString.InsertColumn(0, _T("Index"), LVCFMT_CENTER, 100 );
	m_ctrlListString.InsertColumn(1, _T("String"), LVCFMT_CENTER, 400 );
	m_ctrlListString.SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CStringSelect::OnBnClickedOk()
{
	POSITION pos = m_ctrlListString.GetFirstSelectedItemPosition();
	if (pos != NULL)
	{
		m_nSelectItem = m_ctrlListString.GetNextSelectedItem(pos);
		m_nStringIndex = _ttoi(m_ctrlListString.GetItemText( m_nSelectItem, 0 ));
		OnOK();
	}
}

void CStringSelect::OnBnClickedButtonFind()
{
	CString strFindText;
	m_ctrlEditFind.GetWindowText( strFindText );
	if( strFindText.IsEmpty() )
		return;

	int nItem(-1);

	POSITION pos = m_ctrlListString.GetFirstSelectedItemPosition();
	if (pos != NULL)
	{
		nItem = m_ctrlListString.GetNextSelectedItem(pos);
	}

	CString strString;

	while( (nItem = m_ctrlListString.GetNextItem( nItem, LVNI_ALL )) != -1 )
	{
		strString = m_ctrlListString.GetItemText( nItem, 1 );

		if( strString.Find( strFindText ) != -1 )
		{
			int nTopIndex = m_ctrlListString.GetTopIndex();
			nTopIndex = nItem - nTopIndex;

			CRect rect;
			m_ctrlListString.GetItemRect(0, &rect, LVIR_BOUNDS);
			m_ctrlListString.Scroll( CSize( 0, (nTopIndex)*rect.Height() ) );

			m_ctrlListString.SetFocus();
			m_ctrlListString.SetItemState( nItem, LVIS_SELECTED, LVIS_SELECTED );

			m_nSelectItem = nItem;
			return;
		}
	}
}

void CStringSelect::SetScroll()
{
	if( m_nSelectItem != -1 )
	{
		CRect rect;
		m_ctrlListString.GetItemRect(0, &rect, LVIR_BOUNDS);
		m_ctrlListString.Scroll( CSize( 0, (m_nSelectItem)*rect.Height() ) );
	}
}

void CStringSelect::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialog::OnShowWindow(bShow, nStatus);

	m_ctrlListString.DeleteAllItems();
	m_ctrlListString.InsertItem( 0, "" );
	m_ctrlListString.SetItemText( 0, 0, "0" );
	m_ctrlListString.SetItemText( 0, 1, "" );

	const CEtUIXML::XML_STRING_MAP *pCategory(NULL);
	pCategory = GetEtUIXML().GetCategoryList( CEtUIXML::idCategory1 );
	if( pCategory )
	{
		CString strTemp;
		char szTemp[2048]={0};

		CEtUIXML::XML_STRING_MAP::const_iterator iter = pCategory->begin();
		for( int i=1; iter!=pCategory->end(); ++iter, ++i )
		{
			m_ctrlListString.InsertItem( i, "" );
			strTemp.Format( "%d", iter->first );
			m_ctrlListString.SetItemText( i, 0, strTemp );
			WideCharToMultiByte( CP_ACP, 0, iter->second.c_str(), -1, szTemp, 2048, NULL, NULL);
			m_ctrlListString.SetItemText( i, 1, szTemp );
			SecureZeroMemory( szTemp, sizeof(szTemp) );
		}
	}

	SetScroll();
}
