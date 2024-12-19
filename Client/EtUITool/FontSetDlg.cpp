// FontSetDlg.cpp : implementation file
//

#include "stdafx.h"
#include "EtUITool.h"
#include "FontSetDlg.h"
#include "GetFontFile.h"
#include "DebugSet.h"

// CFontSetDlg dialog

IMPLEMENT_DYNAMIC(CFontSetDlg, CDialog)

CFontSetDlg::CFontSetDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CFontSetDlg::IDD, pParent)
	, m_nSelectIndex(0)
{
}

CFontSetDlg::~CFontSetDlg()
{
}

void CFontSetDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_FONTLIST, m_ctrlFontSetList);
}


BEGIN_MESSAGE_MAP(CFontSetDlg, CDialog)
	ON_BN_CLICKED(IDOK, &CFontSetDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDC_ADD, &CFontSetDlg::OnBnClickedAdd)
	ON_BN_CLICKED(IDC_DELETE, &CFontSetDlg::OnBnClickedDelete)
	ON_BN_CLICKED(IDC_MODIFY, &CFontSetDlg::OnBnClickedModify)
	ON_BN_CLICKED(IDC_SAVE, &CFontSetDlg::OnBnClickedSave)
	ON_BN_CLICKED(IDC_LOAD, &CFontSetDlg::OnBnClickedLoad)
	ON_BN_CLICKED(IDC_SAVE_AS, &CFontSetDlg::OnBnClickedSaveAs)
	ON_BN_CLICKED(IDC_SAME_FONTNAME, &CFontSetDlg::OnBnClickedSameFontname)
	ON_BN_CLICKED(IDC_SAME_FONTWEIGHT, &CFontSetDlg::OnBnClickedSameFontweight)
END_MESSAGE_MAP()


// CFontSetDlg message handlers

BOOL CFontSetDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	//CRect rect;
	//m_ctrlFontSetList.GetClientRect(&rect);
	m_ctrlFontSetList.InsertColumn(0, _T("Index"), LVCFMT_CENTER, 60);
	m_ctrlFontSetList.InsertColumn(1, _T("FontName"), LVCFMT_CENTER, 140);
	m_ctrlFontSetList.InsertColumn(2, _T("FileName"), LVCFMT_CENTER, 100);
	m_ctrlFontSetList.InsertColumn(3, _T("Height"), LVCFMT_CENTER, 60);
	m_ctrlFontSetList.InsertColumn(4, _T("Weight"), LVCFMT_CENTER, 60);
	//m_ctrlFontSetList.InsertColumn(4, _T("Italic"), LVCFMT_CENTER, rect.Width()-240);
	m_ctrlFontSetList.InsertColumn(5, _T("Italic"), LVCFMT_CENTER, 60);

	m_ctrlFontSetList.SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES);

	UpdateFontSetList();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CFontSetDlg::OnBnClickedOk()
{
	POSITION pos = m_ctrlFontSetList.GetFirstSelectedItemPosition();
	if (pos != NULL)
	{
		int nSelect = m_ctrlFontSetList.GetNextSelectedItem(pos);
		m_nSelectIndex = _ttoi(m_ctrlFontSetList.GetItemText( nSelect, 0 ));
		OnOK();
	}
}

void CFontSetDlg::OnBnClickedAdd()
{
	CFontDialog fontDlg;
	if( fontDlg.DoModal() == IDOK )
	{
		SUIFontSet uiFontSet;
		GetFontSetFromList( CEtFontMng::GetInstance().GetFontSetIndex(), uiFontSet, fontDlg );

		int nCount = m_ctrlFontSetList.GetItemCount();
		m_ctrlFontSetList.InsertItem( nCount, "" );
		SetFontSetToList( nCount, uiFontSet );

		// Note : 폰트셋을 폰트매니저에 등록.
		//
		CEtFontMng::GetInstance().AddFontSet( uiFontSet );
	}
}

void CFontSetDlg::OnBnClickedDelete()
{
	POSITION pos = m_ctrlFontSetList.GetFirstSelectedItemPosition();
	if (pos != NULL)
	{
		int nSelect = m_ctrlFontSetList.GetNextSelectedItem(pos);
		m_ctrlFontSetList.DeleteItem( nSelect );

		CEtFontMng::GetInstance().DeleteFontSet( nSelect );
	}
}

void CFontSetDlg::OnBnClickedModify()
{
	POSITION pos = m_ctrlFontSetList.GetFirstSelectedItemPosition();
	if (pos != NULL)
	{
		CFontDialog fontDlg;

		if( fontDlg.DoModal() == IDOK )
		{
			int nSelect = m_ctrlFontSetList.GetNextSelectedItem(pos);

			SUIFontSet uiFontSet;
			GetFontSetFromList( _ttoi(m_ctrlFontSetList.GetItemText( nSelect, 0 )), uiFontSet, fontDlg );
			SetFontSetToList( nSelect, uiFontSet );

			// Note : 수정된 폰트셋을 폰트매니져에 등록.
			//
			CEtFontMng::GetInstance().ModifyFontSet( nSelect, uiFontSet );
		}
	}
}

void CFontSetDlg::OnBnClickedSave()
{
	if( !m_strFontSetFileName.empty() )
	{
		CEtFontMng::GetInstance().SaveFontSet( m_strFontSetFileName.c_str() );
	}
	else
	{
		OnBnClickedSaveAs();
	}
}

void CFontSetDlg::OnBnClickedLoad()
{
	TCHAR szFilter[] = _T( "Eternity UIFontSet (*.fontset)|*.fontset|All Files (*.*)|*.*||" );
	CFileDialog FileDlg( TRUE, _T("fontset"), _T("*.fontset"), OFN_HIDEREADONLY | OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_ENABLESIZING | OFN_NOCHANGEDIR, szFilter, this );
	if( FileDlg.DoModal() == IDOK )
	{
		CEtFontMng::GetInstance().LoadFontSet( FileDlg.m_ofn.lpstrFile );
		m_strFontSetFileName = FileDlg.m_ofn.lpstrFile;
		UpdateFontSetList();
	}
}

void CFontSetDlg::UpdateFontSetList()
{
	m_ctrlFontSetList.DeleteAllItems();

	// Note : 폰트매니저에서 폰트셋을 읽어서 리스트에 등록한다.
	//
	std::vector< SUIFontSet > &vecFontSet = CEtFontMng::GetInstance().GetFontSetList();

	for( int i=0 ; i<(int)vecFontSet.size(); i++)
	{
		m_ctrlFontSetList.InsertItem( i, "" );
		SetFontSetToList( i, vecFontSet[i] );
	}
}

void CFontSetDlg::GetFontSetFromList( int nIndex, SUIFontSet &fontSet, CFontDialog &fontDialog )
{
	fontSet.nIndex = nIndex;
	fontSet.strFontName = fontDialog.GetFaceName();
	CString strDisplayName, strFontFile;
	GetFontFile(fontDialog.GetFaceName(), strDisplayName, strFontFile);
	fontSet.strFileName = strFontFile;
	fontSet.nFontHeight = fontDialog.GetSize()/10;
	fontSet.nFontWeight = fontDialog.GetWeight()/100;
	fontSet.bItalic = fontDialog.IsItalic() ? true : false;
}

void CFontSetDlg::SetFontSetToList( int nIndex, SUIFontSet &fontSet )
{
	CString strTemp;
	strTemp.Format( "%d", fontSet.nIndex );
	m_ctrlFontSetList.SetItemText(nIndex, 0, strTemp );
	m_ctrlFontSetList.SetItemText(nIndex, 1, fontSet.strFontName.c_str() );
	m_ctrlFontSetList.SetItemText(nIndex, 2, fontSet.strFileName.c_str() );
	strTemp.Format( "%d", fontSet.nFontHeight );
	m_ctrlFontSetList.SetItemText(nIndex, 3, strTemp);
	strTemp.Format( "%d", fontSet.nFontWeight );
	m_ctrlFontSetList.SetItemText(nIndex, 4, strTemp);
	strTemp.Format( "%s", fontSet.bItalic ? "true":"false" );
	m_ctrlFontSetList.SetItemText(nIndex, 5, strTemp);
}
void CFontSetDlg::OnBnClickedSaveAs()
{
	TCHAR szFilter[] = _T( "Eternity UIFontSet (*.fontset)|*.fontset|All Files (*.*)|*.*||" );
	CFileDialog FileDlg( FALSE, _T("fontset"), _T("*.fontset"), OFN_HIDEREADONLY | OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_ENABLESIZING | OFN_NOCHANGEDIR, szFilter, this );
	if( FileDlg.DoModal() == IDOK )
	{
		CEtFontMng::GetInstance().SaveFontSet( FileDlg.m_ofn.lpstrFile );
		m_strFontSetFileName = FileDlg.m_ofn.lpstrFile;
	}	
}

void CFontSetDlg::OnBnClickedSameFontname()
{
	POSITION pos = m_ctrlFontSetList.GetFirstSelectedItemPosition();
	if (pos != NULL)
	{
		int nSelect = m_ctrlFontSetList.GetNextSelectedItem(pos);
		int nFontSetIndex = _ttoi(m_ctrlFontSetList.GetItemText( nSelect, 0 ));

		SUIFontSet *pFontSet = CEtFontMng::GetInstance().GetFontSet( nFontSetIndex );
		if( pFontSet )
		{
			std::vector< SUIFontSet > &vecFontSet = CEtFontMng::GetInstance().GetFontSetList();
			for( int i=0 ; i<(int)vecFontSet.size(); i++)
			{
				vecFontSet[i].strFontName = pFontSet->strFontName;
				vecFontSet[i].strFileName = pFontSet->strFileName;
				vecFontSet[i].strFullFileName = pFontSet->strFullFileName;
			}

			UpdateFontSetList();
		}
	}
}

void CFontSetDlg::OnBnClickedSameFontweight()
{
	POSITION pos = m_ctrlFontSetList.GetFirstSelectedItemPosition();
	if (pos != NULL)
	{
		int nSelect = m_ctrlFontSetList.GetNextSelectedItem(pos);
		int nFontWeight = _ttoi(m_ctrlFontSetList.GetItemText( nSelect, 4 ));

		std::vector< SUIFontSet > &vecFontSet = CEtFontMng::GetInstance().GetFontSetList();
		for( int i=0 ; i<(int)vecFontSet.size(); i++)
		{
			vecFontSet[i].nFontWeight = nFontWeight;
		}

		UpdateFontSetList();
	}
}
