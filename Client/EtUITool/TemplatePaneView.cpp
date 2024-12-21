// TemplatePaneView.cpp : implementation file
//

#include "stdafx.h"
#include "EtUITool.h"
#include "EtUIToolDoc.h"
#include "EtUIToolView.h"
#include "TemplatePaneView.h"
#include "TemplateNameDlg.h"
#include "TemplateView.h"
#include "DummyView.h"

#include "TemplatePropertyPaneView.h"
#include "MainFrm.h"
#include "PaneDefine.h"

#include "GlobalValue.h"
#include "UIToolTemplate.h"
#include "RenderBase.h"
#include "DebugSet.h"
#include "../../Common/EtResManager/EtResourceMng.h"
#include "GenTexture.h"

#include "LayoutView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CTemplatePaneView

std::vector< CUIToolTemplate * > g_vecScrollBar;
std::vector< std::string > g_vecCustomControlName;
CUIToolTemplate *g_pCurSelTemplate = NULL;

char *g_aszTreeItem[] =
{
	"Static",
	"Button",
	"CheckBox",
	"RadioButton",
	"ComboBox",
	"Slider", 
	"EditBox",
	"IMEEditBox",
	"ListBox",
	"ScrollBar",
	"ProgressBar",
	"Custom",
	"TextBox",
	"TextureControl",
	"HtmlTextbox",
	"Tree",
	"QuestTree",
	"Animation",
	"LineEditBox",
	"LineIMEEditBox",
	"MovieControl",
	"ListBoxEx",

#if defined(SW_ADD_SEPARATE_STATIC_20091014_hayannal2009)
	"Static_Text",	// 이 아래부터는 별도로 보관하는 Static 템플릿들이다.(너무 많아서 안보이게 하려고 분리했다.)
	"Static_Line",	// 마지막 디폴트컨트롤보다 아래 있어야한다.
	"Static_Board",
	"Static_Icon",
	"Static_Etc",
	"Static_Select",
	"Static_Image",
#endif
};

#if defined(SW_ADD_SEPARATE_STATIC_20091014_hayannal2009)
char *g_aszSeparateStaticFile[CTemplatePaneView::MAX_SEPARATE_STATIC] =
{
	"\\Static_Text.txt",
	"\\Static_Line.txt",
	"\\Static_Board.txt",
	"\\Static_Icon.txt",
	"\\Static_Etc.txt",
	"\\Static_Select.txt",
	"\\Static_Image.txt",
};
#endif

IMPLEMENT_DYNCREATE(CTemplatePaneView, CFormView)

CTemplatePaneView::CTemplatePaneView()
	: CFormView(CTemplatePaneView::IDD)
{
	m_bActivate = false;
	m_pContextMenu = new CMenu();
	m_pContextMenu->LoadMenu( IDR_CONTEXT_MENU );
}

CTemplatePaneView::~CTemplatePaneView()
{
#if defined(SW_ADD_SEPARATE_STATIC_20091014_hayannal2009)
	SaveSeparateStatic();
#endif
	SAFE_DELETE( m_pContextMenu );
	CDebugSet::Finalize();
}

void CTemplatePaneView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TEMPLATE_TREE, m_TemplateTree);
}

BEGIN_MESSAGE_MAP(CTemplatePaneView, CFormView)
	ON_WM_SIZE()
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_TEMPLATE_ADDTEMPLATE, &CTemplatePaneView::OnTemplateAddtemplate)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TEMPLATE_TREE, &CTemplatePaneView::OnTvnSelchangedTemplateTree)
	ON_WM_DESTROY()
	ON_WM_ERASEBKGND()
	ON_COMMAND(ID_ELEMENT_ADDELEMENT, &CTemplatePaneView::OnTemplateAddelement)
	ON_NOTIFY(NM_CLICK, IDC_TEMPLATE_TREE, &CTemplatePaneView::OnNMClickTemplateTree)
	ON_COMMAND(ID_ELEMENT_DELELEMENT, &CTemplatePaneView::OnElementDelelement)
	ON_COMMAND(ID_TEMPLATE_ADDTEMPLATES, &CTemplatePaneView::OnTemplateAddtemplates)
	ON_COMMAND(ID_ELEMENT_COPYTEMPLATE, &CTemplatePaneView::OnElementCopytemplate)
	ON_COMMAND(ID_ELEMENT_SAVE_TEMPLATE_TEXTURE, &CTemplatePaneView::OnElementSaveTemplateTexture)
	ON_COMMAND(ID_ELEMENT_SAVEUI_USEDTEMPLATE, &CTemplatePaneView::OnElementSaveuiUsedtemplate)
END_MESSAGE_MAP()


// CTemplatePaneView diagnostics

#ifdef _DEBUG
void CTemplatePaneView::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CTemplatePaneView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// CTemplatePaneView message handlers

void CTemplatePaneView::OnSize(UINT nType, int cx, int cy)
{
	CFormView::OnSize(nType, cx, cy);

	// TODO: Add your message handler code here
	if( m_TemplateTree )
	{
		CRect rcRect;
		GetClientRect( &rcRect );
		m_TemplateTree.MoveWindow( &rcRect );
	}
}

void CTemplatePaneView::OnInitialUpdate()
{
	__super::OnInitialUpdate();

	if( m_bActivate )
	{
		return;
	}
	m_bActivate = TRUE;

	CBitmap bitmap;
	bitmap.LoadBitmap( IDB_TEMPLATE );

	m_ImageList.Create( 16, 16, ILC_COLOR24|ILC_MASK, 4, 1 );
	m_ImageList.Add( &bitmap, RGB(0,255,0) );

	m_TemplateTree.EnableMultiSelect( TRUE );
	m_TemplateTree.DeleteAllItems();
	m_TemplateTree.SetImageList( &m_ImageList, TVSIL_NORMAL );
	m_hRoot = m_TemplateTree.InsertItem( "Templates", 0, 0 );

	int i, nCount;

	nCount = sizeof( g_aszTreeItem ) / sizeof( char * );
	for( i = 0; i < nCount; i++ )
	{
		m_TemplateTree.InsertItem( g_aszTreeItem[ i ], 0, 1, m_hRoot );
	}
	m_TemplateTree.Expand( m_hRoot, TVE_EXPAND );

	CDebugSet::Initialize( CEtResourceMng::GetInstance().GetCurrentDirectory().c_str(), true );

#if defined(SW_ADD_SEPARATE_STATIC_20091014_hayannal2009)
	LoadSeparateStatic();
#endif
	LoadExistUITemplate();
}

#if defined(SW_ADD_SEPARATE_STATIC_20091014_hayannal2009)
void CTemplatePaneView::LoadSeparateStatic()
{
	//char szName[7][_MAX_PATH] = {
	//	"\\Static_Text.txt",
	//	"\\Static_Line.txt",
	//	"\\Static_Board.txt",
	//	"\\Static_Etc.txt",
	//	"\\Static_Select.txt",
	//	"\\Static_Image.txt",
	//};
	char szFileName[ _MAX_PATH ];
	FILE *fp;
	char szString[_MAX_PATH]={0};

	for( int i = 0; i < MAX_SEPARATE_STATIC; ++i )
	{
		strcpy_s( szFileName, _MAX_PATH, CMainFrame::GetUITemplateFolder() );
		strcat_s( szFileName, _MAX_PATH, g_aszSeparateStaticFile[i] );
		fopen_s( &fp, szFileName, "r" );
		if( fp )
		{
			while( fgets( szString, _MAX_PATH, fp ) )
			{
				if( szString[strlen(szString)-1] == '\n' ) szString[strlen(szString)-1] = '\0';
				m_vecSeparateStatic[i].push_back( szString );
			}
			fclose( fp );
		}
	}
}

void CTemplatePaneView::SaveSeparateStatic()
{
	//char szName[4][_MAX_PATH] = {
	//	"\\Static_Text.txt",
	//	"\\Static_Line.txt",
	//	"\\Static_Board.txt",
	//	"\\Static_Etc.txt"
	//};
	char szFileName[ _MAX_PATH ];
	FILE *fp;
	char szString[_MAX_PATH]={0};

	for( int i = 0; i < MAX_SEPARATE_STATIC; ++i )
	{
		std::vector<std::string> *pVec = &m_vecSeparateStatic[i];
		if( (int)pVec->size() > 0 )
		{
			strcpy_s( szFileName, _MAX_PATH, CMainFrame::GetUITemplateFolder() );
			strcat_s( szFileName, _MAX_PATH, g_aszSeparateStaticFile[i] );
			fopen_s( &fp, szFileName, "w" );
			if( fp )
			{
				for( int j = 0; j < (int)pVec->size(); ++j )
				{
					fputs( pVec->at(j).c_str(), fp );
					fputs( "\n", fp );
				}
				fclose( fp );
			}
		}
	}
}
#endif

void CTemplatePaneView::LoadExistUITemplate()
{
	char szFindFile[ _MAX_PATH ], szFileName[ _MAX_PATH ];
	WIN32_FIND_DATA FindFileData;
	HANDLE hFind;

	strcpy_s( szFindFile, _MAX_PATH, CMainFrame::GetUITemplateFolder() );
	strcat_s( szFindFile, _MAX_PATH, "\\*.uit" );

	hFind = FindFirstFile( szFindFile, &FindFileData );
	if( hFind == INVALID_HANDLE_VALUE )
	{
		FindClose( hFind );
		return;
	}
	while( hFind != INVALID_HANDLE_VALUE )
	{
		if( !( FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) )
		{
			sprintf_s( szFileName, _MAX_PATH, "%s\\%s", CMainFrame::GetUITemplateFolder(), FindFileData.cFileName );

			CUIToolTemplate *pTemplate;
			CFileStream Stream( szFileName, CFileStream::OPEN_READ );

			pTemplate = new CUIToolTemplate();
			pTemplate->Load( Stream );
			InsertTemplate( pTemplate );
		}
		if( !FindNextFile( hFind, &FindFileData ) )
		{
			break;
		}
	}
	if( hFind != INVALID_HANDLE_VALUE )
	{
		FindClose( hFind );
	}

	CheckExternControl();

	strcpy_s( szFindFile, _MAX_PATH, CMainFrame::GetUITemplateFolder() );
	strcat_s( szFindFile, _MAX_PATH, "\\CustomControlList.txt" );

	FILE *fp;
	fopen_s( &fp, szFindFile, "r" );

	if( !fp )
	{
		MessageBoxA(szFindFile, "Cannot find file", MB_OK);
		return;
	}

	char szString[_MAX_PATH]={0};

	while( fgets( szString, _MAX_PATH, fp ) )
	{
		if( szString[strlen(szString)-1] == '\n' )
			szString[strlen(szString)-1] = '\0';
		g_vecCustomControlName.push_back( szString );
	}

	fclose( fp );
}

void CTemplatePaneView::CheckExternControl()
{
	for( int i = 0; i < ( int )m_vecUIToolTemplate.size(); i++ )
	{
		if( ( m_vecUIToolTemplate[ i ]->m_UIType == UI_CONTROL_COMBOBOX ) || 
			( m_vecUIToolTemplate[ i ]->m_UIType == UI_CONTROL_LISTBOX ) ||
			( m_vecUIToolTemplate[ i ]->m_UIType == UI_CONTROL_TEXTBOX ) ||
			( m_vecUIToolTemplate[ i ]->m_UIType == UI_CONTROL_HTMLTEXTBOX ) ||
			( m_vecUIToolTemplate[ i ]->m_UIType == UI_CONTROL_TREECONTROL ) ||
			( m_vecUIToolTemplate[ i ]->m_UIType == UI_CONTROL_QUESTTREECONTROL ) ||
			( m_vecUIToolTemplate[ i ]->m_UIType == UI_CONTROL_LISTBOXEX ) )
		{
			if( !m_vecUIToolTemplate[ i ]->m_szExternalControlName.empty() )
			{
				for( int j = 0; j < ( int )m_vecUIToolTemplate.size(); j++ )
				{
					if( _stricmp( m_vecUIToolTemplate[ j ]->m_Template.m_szTemplateName.c_str(), m_vecUIToolTemplate[ i ]->m_szExternalControlName.c_str() ) == 0 )
					{
						m_vecUIToolTemplate[ i ]->m_pExternControlTemplate = m_vecUIToolTemplate[ j ];
					}
				}
			}
		}
		else if( m_vecUIToolTemplate[ i ]->m_UIType == UI_CONTROL_SCROLLBAR )
		{
			g_vecScrollBar.push_back( m_vecUIToolTemplate[ i ] );
		}
	}
}

int CTemplatePaneView::GetTreeDepth( HTREEITEM hItem )
{
	int nDepth;
	nDepth = 1;
	while( 1 )
	{
		hItem = m_TemplateTree.GetParentItem( hItem );
		if( hItem )
		{
			nDepth++;
		}
		else
		{
			break;
		}
	}

	return nDepth;
}

int CTemplatePaneView::GetCurrentTreeDepth()
{
	HTREEITEM hSelectItem;

	hSelectItem = m_TemplateTree.GetSelectedItem();

	return GetTreeDepth( hSelectItem );
}

bool CTemplatePaneView::CheckDuplicateItem( CString &szItemName )
{
	HTREEITEM hItem;

	hItem = m_hRoot;
	while( 1 )
	{
		CString szItemText;

		szItemText = m_TemplateTree.GetItemText( hItem );
		if( _stricmp( szItemText, szItemName ) == 0 )
		{
			return true;
		}
		hItem = m_TemplateTree.GetNextItem( hItem );
		if( !hItem )
		{
			break;
		}
	}

	return false;
}

bool CTemplatePaneView::CheckDuplicateTemplate( CString &szTemplateName )
{
	HTREEITEM hItem;

	hItem = m_hRoot;
	while( 1 )
	{
		CString szItemText;

		// 엘리먼트들은 할 필요 없으니 패스하기로 한다.
		szItemText = m_TemplateTree.GetItemText( hItem );
		if( GetTreeDepth( hItem ) != 4 && _stricmp( szItemText, szTemplateName ) == 0 )
		{
			return true;
		}
		hItem = m_TemplateTree.GetNextItem( hItem );
		if( !hItem )
		{
			break;
		}
	}

	return false;
}

void CTemplatePaneView::DeleteAllTreeItemData()
{
	SAFE_DELETE_PVEC( m_vecUIToolTemplate );
}

void CTemplatePaneView::OnContextMenu( CWnd *pWnd, CPoint point )
{
	CRect rcRect;

	GetClientRect( &rcRect );

	int nTreeDepth;
	nTreeDepth = GetCurrentTreeDepth();
	if( nTreeDepth == 2 )
	{
		CMenu *pMenu = m_pContextMenu->GetSubMenu( 0 );
		pMenu->TrackPopupMenu( TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, this );
	}
	else if( nTreeDepth == 3 )
	{
		CMenu *pMenu = m_pContextMenu->GetSubMenu( 1 );
		pMenu->EnableMenuItem(ID_ELEMENT_ADDELEMENT, MF_BYCOMMAND | MF_ENABLED);
		pMenu->EnableMenuItem(ID_ELEMENT_DELELEMENT, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
		pMenu->TrackPopupMenu( TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, this );
	}
	else if( nTreeDepth == 4 )
	{
		CMenu *pMenu = m_pContextMenu->GetSubMenu( 1 );
		pMenu->EnableMenuItem(ID_ELEMENT_ADDELEMENT, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
		pMenu->EnableMenuItem(ID_ELEMENT_DELELEMENT, MF_BYCOMMAND | MF_ENABLED);
		pMenu->TrackPopupMenu( TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, this );
	}
}

void CTemplatePaneView::OnTemplateAddtemplate()
{
	int i, nCount, nFindIndex;
	HTREEITEM hSelectItem;
	CString szItemName;

	hSelectItem = m_TemplateTree.GetSelectedItem();
	szItemName = m_TemplateTree.GetItemText( hSelectItem );
	nCount = sizeof( g_aszTreeItem ) / sizeof( char * );
	nFindIndex = -1;
	for( i = 0; i < nCount; i++ )
	{
		if( _stricmp( g_aszTreeItem[ i ], szItemName ) == 0 )
		{
			nFindIndex = i;
			break;
		}
	}
	if( nFindIndex == -1 )
	{
		return;
	}

	CTemplateNameDlg NameDlg;
	CString szDefaultName;

	srand( timeGetTime() );
	szDefaultName.Format( "%s%d", szItemName.GetBuffer(), rand() % 10000 );
	NameDlg.m_szTemplateName = szDefaultName;
	while( 1 )
	{
		if( NameDlg.DoModal() == IDOK )
		{
			if( ( NameDlg.m_szTemplateName.GetLength() > 0 ) && ( !CheckDuplicateTemplate( NameDlg.m_szTemplateName ) ) )
			{
				NewTemplate( NameDlg.m_szTemplateName );
				break;
			}
			else
			{
				MessageBox( "같은 이름을 가진 Template이 있습니다" );
			}
		}
		else
		{
			break;
		}
	}
}

void CTemplatePaneView::InsertTemplate( CUIToolTemplate *pTemplate )
{
	int i;
	HTREEITEM hCurTreeItem, hInsertItem;
	CString szItemName;

	hCurTreeItem = m_TemplateTree.GetChildItem( m_hRoot );
	while( hCurTreeItem != NULL )
	{
		szItemName = m_TemplateTree.GetItemText( hCurTreeItem );
		if( _stricmp( szItemName, g_aszTreeItem[ pTemplate->m_UIType ] ) == 0 )
		{
#if defined(SW_ADD_SEPARATE_STATIC_20091014_hayannal2009)
			if( pTemplate->UIType() == UI_CONTROL_STATIC )
			{
				int nTreeItemOffset = 0;
				int nResult = 0;	// 0 - not, 1 - static_text, 2 - static_line, 3 - static_board, 4 - static_icon, 5 - static_etc, 6 - static_select, 7 - static_image

				for( int ndx = 0; ndx < MAX_SEPARATE_STATIC; ++ndx )
				{
					for( int i = 0; i < (int)m_vecSeparateStatic[ndx].size(); ++i )
					{
						if( _stricmp( pTemplate->GetTemplateName(), m_vecSeparateStatic[ndx][i].c_str() ) == 0 )
						{
							nResult = ndx + 1;
							break;
						}
					}
				}

				//for( int i = 0; i < (int)m_vecStaticLine.size(); ++i )
				//{
				//	if( _stricmp( pTemplate->GetTemplateName(), m_vecStaticLine[i].c_str() ) == 0 )
				//	{
				//		nResult = 2;
				//		break;
				//	}
				//}
				//for( int i = 0; i < (int)m_vecStaticBoard.size(); ++i )
				//{
				//	if( _stricmp( pTemplate->GetTemplateName(), m_vecStaticBoard[i].c_str() ) == 0 )
				//	{
				//		nResult = 3;
				//		break;
				//	}
				//}
				//for( int i = 0; i < (int)m_vecStaticEtc.size(); ++i )
				//{
				//	if( _stricmp( pTemplate->GetTemplateName(), m_vecStaticEtc[i].c_str() ) == 0 )
				//	{
				//		nResult = 4;
				//		break;
				//	}
				//}

				if( nResult > 0 )
				{
					nTreeItemOffset = UI_CONTROL_SIZE - UI_CONTROL_STATIC + nResult - 1;
					while( nTreeItemOffset > 0 )
					{
						hCurTreeItem = m_TemplateTree.GetNextSiblingItem( hCurTreeItem );
						--nTreeItemOffset;
					}
				}
			}
#endif

			hInsertItem = m_TemplateTree.InsertItem( pTemplate->m_Template.m_szTemplateName.c_str(), 2, 3, hCurTreeItem );
			for( i = 0; i < ( int )pTemplate->m_vecElementName.size(); i++ )
			{
				m_TemplateTree.InsertItem( pTemplate->m_vecElementName[ i ].c_str(), 2, 3, hInsertItem );
			}
			m_TemplateTree.SetItemData( hInsertItem, ( DWORD_PTR )pTemplate );
			m_vecUIToolTemplate.push_back( pTemplate );

			// 처음 열릴때 확장 안되도록.
			//m_TemplateTree.Expand( hCurTreeItem, TVE_EXPAND );
			return;
		}
		hCurTreeItem = m_TemplateTree.GetNextSiblingItem( hCurTreeItem );
	}
}

void CTemplatePaneView::NewTemplate( CString &szTemplateName )
{
	HTREEITEM hInsertItem, hSelectItem;
	UI_CONTROL_TYPE UIType;
	SUIElement Element;

	hSelectItem = m_TemplateTree.GetSelectedItem();
	hInsertItem = m_TemplateTree.InsertItem( szTemplateName, 2, 3, hSelectItem );

	g_pCurSelTemplate = new CUIToolTemplate();
	g_pCurSelTemplate->SetTemplateName( szTemplateName );
	UIType = ( UI_CONTROL_TYPE )GetTreeItemIndex( hSelectItem );

#if defined(SW_ADD_SEPARATE_STATIC_20091014_hayannal2009)
	if( UIType >= UI_CONTROL_SIZE )
	{
		CString szItemName = m_TemplateTree.GetItemText( hSelectItem );

		for( int ndx = 0; ndx < MAX_SEPARATE_STATIC; ++ndx )
		{
			if( _stricmp( szItemName, g_aszTreeItem[UI_CONTROL_SIZE+ndx] ) == 0 )
			{
				UIType = UI_CONTROL_STATIC;
				bool bFind = false;
				for( int i = 0; i < (int)m_vecSeparateStatic[ndx].size(); ++i )
				{
					if( _stricmp( szTemplateName, m_vecSeparateStatic[ndx][i].c_str() ) == 0 )
					{
						bFind = true;
						break;
					}
				}
				if( !bFind ) m_vecSeparateStatic[ndx].push_back((const char *)szTemplateName);
			}
		}



		//if( _stricmp( szItemName, "Static_Text" ) == 0 )
		//{
		//	UIType = UI_CONTROL_STATIC;
		//	bool bFind = false;
		//	for( int i = 0; i < (int)m_vecSeparateStatic[0][i].size(); ++i )
		//	{
		//		if( _stricmp( szTemplateName, m_vecSeparateStatic[0][i].c_str() ) == 0 )
		//		{
		//			bFind = true;
		//			break;
		//		}
		//	}
		//	if( !bFind ) m_vecSeparateStatic[0].push_back((const char *)szTemplateName);
		//}
		//else if( _stricmp( szItemName, "Static_Line" ) == 0 )
		//{
		//	UIType = UI_CONTROL_STATIC;
		//	bool bFind = false;
		//	for( int i = 0; i < (int)m_vecStaticText.size(); ++i )
		//	{
		//		if( _stricmp( szTemplateName, m_vecStaticText[i].c_str() ) == 0 )
		//		{
		//			bFind = true;
		//			break;
		//		}
		//	}
		//	if( !bFind ) m_vecStaticLine.push_back((const char *)szTemplateName);
		//}
		//else if( _stricmp( szItemName, "Static_Board" ) == 0 )
		//{
		//	UIType = UI_CONTROL_STATIC;
		//	bool bFind = false;
		//	for( int i = 0; i < (int)m_vecStaticText.size(); ++i )
		//	{
		//		if( _stricmp( szTemplateName, m_vecStaticText[i].c_str() ) == 0 )
		//		{
		//			bFind = true;
		//			break;
		//		}
		//	}
		//	if( !bFind ) m_vecStaticBoard.push_back((const char *)szTemplateName);
		//}
		//else if( _stricmp( szItemName, "Static_Etc" ) == 0 )
		//{
		//	UIType = UI_CONTROL_STATIC;
		//	bool bFind = false;
		//	for( int i = 0; i < (int)m_vecStaticText.size(); ++i )
		//	{
		//		if( _stricmp( szTemplateName, m_vecStaticText[i].c_str() ) == 0 )
		//		{
		//			bFind = true;
		//			break;
		//		}
		//	}
		//	if( !bFind ) m_vecStaticEtc.push_back((const char *)szTemplateName);
		//}
	}
#endif

	g_pCurSelTemplate->SetUIType( UIType );
	m_TemplateTree.SetItemData( hInsertItem, ( DWORD_PTR )g_pCurSelTemplate );
	m_vecUIToolTemplate.push_back( g_pCurSelTemplate );

	if( g_InitControl[ UIType ].fWidth == 0.0f )
		ASSERT(0&&"TemplateView.cpp의 g_InitControl의 값이 없는 듯 합니다.");

	g_pCurSelTemplate->m_fDefaultWidth = g_InitControl[ UIType ].fWidth;
	g_pCurSelTemplate->m_fDefaultHeight = g_InitControl[ UIType ].fHeight;
	g_pCurSelTemplate->m_bChanged = true;

	switch( UIType )
	{
		case UI_CONTROL_STATIC:
			m_TemplateTree.InsertItem( "Normal", 2, 3, hInsertItem );
			g_pCurSelTemplate->AddUIElement( "", 0, "Normal", Element );
			break;
		case UI_CONTROL_BUTTON:
			g_pCurSelTemplate->m_ControlInfo.ButtonInfo.ButtonType = UI_BUTTON_CLICK;
			g_pCurSelTemplate->m_ControlInfo.ButtonInfo.fMoveOffsetHori = 1.0f / DEFAULT_UI_SCREEN_WIDTH;
			g_pCurSelTemplate->m_ControlInfo.ButtonInfo.fMoveOffsetVert = 1.0f / DEFAULT_UI_SCREEN_HEIGHT;
			m_TemplateTree.InsertItem( "Normal", 2, 3, hInsertItem );
			g_pCurSelTemplate->AddUIElement( "", 0, "Normal", Element );
			break;
		case UI_CONTROL_CHECKBOX:
			g_pCurSelTemplate->m_ControlInfo.CheckBoxInfo.fSpace = 2.0f / DEFAULT_UI_SCREEN_WIDTH;
			Element.dwFontFormat = DT_LEFT | DT_VCENTER;
			m_TemplateTree.InsertItem( "Main", 2, 3, hInsertItem );
			g_pCurSelTemplate->AddUIElement( "", 0, "Main", Element );
			m_TemplateTree.InsertItem( "Check Mark", 2, 3, hInsertItem );
			g_pCurSelTemplate->AddUIElement( "", 0, "Check Mark", Element );
			break;
		case UI_CONTROL_RADIOBUTTON:
			g_pCurSelTemplate->m_ControlInfo.RadioButtonInfo.fSpace = 2.0f / DEFAULT_UI_SCREEN_WIDTH;
			g_pCurSelTemplate->m_ControlInfo.RadioButtonInfo.fMoveOffsetHori = 1.0f / DEFAULT_UI_SCREEN_WIDTH;
			g_pCurSelTemplate->m_ControlInfo.RadioButtonInfo.fMoveOffsetVert = 1.0f / DEFAULT_UI_SCREEN_HEIGHT;
			Element.dwFontFormat = DT_LEFT | DT_VCENTER;
			m_TemplateTree.InsertItem( "Main", 2, 3, hInsertItem );
			g_pCurSelTemplate->AddUIElement( "", 0, "Main", Element );
			m_TemplateTree.InsertItem( "Check Mark", 2, 3, hInsertItem );
			g_pCurSelTemplate->AddUIElement( "", 0, "Check Mark", Element );
			break;
		case UI_CONTROL_COMBOBOX:
			g_pCurSelTemplate->m_ControlInfo.ComboBoxInfo.fMoveOffsetHori = 1.0f / DEFAULT_UI_SCREEN_WIDTH;
			g_pCurSelTemplate->m_ControlInfo.ComboBoxInfo.fMoveOffsetVert = 1.0f / DEFAULT_UI_SCREEN_HEIGHT;
			g_pCurSelTemplate->m_ControlInfo.ComboBoxInfo.fDropdownSize = 100.0f / DEFAULT_UI_SCREEN_HEIGHT;
			g_pCurSelTemplate->m_ControlInfo.ComboBoxInfo.fScrollBarSize = 16.0f / DEFAULT_UI_SCREEN_WIDTH;
			Element.FontColor.dwColor[ 0 ] = 0xff000000;
			Element.FontColor.dwColor[ 3 ] = 0xff000000;
			Element.FontColor.dwColor[ 4 ] = 0xff000000;
			Element.FontColor.dwColor[ 5 ] = 0xff000000;
			m_TemplateTree.InsertItem( "Body", 2, 3, hInsertItem );
			g_pCurSelTemplate->AddUIElement( "", 0, "Body", Element );
			m_TemplateTree.InsertItem( "Combo Button", 2, 3, hInsertItem );
			g_pCurSelTemplate->AddUIElement( "", 0, "Combo Button", Element );
			Element.dwFontFormat = DT_LEFT | DT_VCENTER;
			m_TemplateTree.InsertItem( "Dropdown", 2, 3, hInsertItem );
			g_pCurSelTemplate->AddUIElement( "", 0, "Dropdown", Element );
			m_TemplateTree.InsertItem( "Select Item", 2, 3, hInsertItem );
			g_pCurSelTemplate->AddUIElement( "", 0, "Select Item", Element );
			break;
		case UI_CONTROL_SLIDER:
			m_TemplateTree.InsertItem( "Track", 2, 3, hInsertItem );
			g_pCurSelTemplate->AddUIElement( "", 0, "Track", Element );
			m_TemplateTree.InsertItem( "Button", 2, 3, hInsertItem );
			g_pCurSelTemplate->AddUIElement( "", 0, "Button", Element );
			break;
		case UI_CONTROL_EDITBOX:
			g_pCurSelTemplate->m_ControlInfo.EditBoxInfo.fBorder = 5.0f / DEFAULT_UI_SCREEN_WIDTH;
			g_pCurSelTemplate->m_ControlInfo.EditBoxInfo.fSpace = 4.0f / DEFAULT_UI_SCREEN_WIDTH;
			g_pCurSelTemplate->m_ControlInfo.EditBoxInfo.dwTextColor = 0xff101010;
			g_pCurSelTemplate->m_ControlInfo.EditBoxInfo.dwSelTextColor = 0xffffffff;
			g_pCurSelTemplate->m_ControlInfo.EditBoxInfo.dwSelBackColor = 0xff28325c;
			g_pCurSelTemplate->m_ControlInfo.EditBoxInfo.dwCaretColor = 0xff000000;
			Element.dwFontFormat = DT_LEFT | DT_VCENTER;
			m_TemplateTree.InsertItem( "Body", 2, 3, hInsertItem );
			g_pCurSelTemplate->AddUIElement( "", 0, "Body", Element );
			m_TemplateTree.InsertItem( "Left Side", 2, 3, hInsertItem );
			g_pCurSelTemplate->AddUIElement( "", 0, "Left Side", Element );
			m_TemplateTree.InsertItem( "Right Side", 2, 3, hInsertItem );
			g_pCurSelTemplate->AddUIElement( "", 0, "Right Side", Element );
			m_TemplateTree.InsertItem( "Top Side", 2, 3, hInsertItem );
			g_pCurSelTemplate->AddUIElement( "", 0, "Top Side", Element );
			m_TemplateTree.InsertItem( "Bottom Side", 2, 3, hInsertItem );
			g_pCurSelTemplate->AddUIElement( "", 0, "Bottom Side", Element );
			break;
		case UI_CONTROL_IMEEDITBOX:
			g_pCurSelTemplate->m_ControlInfo.IMEEditBoxInfo.fBorder = 5.0f / DEFAULT_UI_SCREEN_WIDTH;
			g_pCurSelTemplate->m_ControlInfo.IMEEditBoxInfo.fSpace = 4.0f / DEFAULT_UI_SCREEN_WIDTH;
			g_pCurSelTemplate->m_ControlInfo.IMEEditBoxInfo.fIndicatorSize = 16.0f / DEFAULT_UI_SCREEN_WIDTH;
			g_pCurSelTemplate->m_ControlInfo.IMEEditBoxInfo.dwTextColor = 0xff101010;
			g_pCurSelTemplate->m_ControlInfo.IMEEditBoxInfo.dwSelTextColor = 0xffffffff;
			g_pCurSelTemplate->m_ControlInfo.IMEEditBoxInfo.dwSelBackColor = 0xff28325c;
			g_pCurSelTemplate->m_ControlInfo.IMEEditBoxInfo.dwCaretColor = 0xff000000;
			g_pCurSelTemplate->m_ControlInfo.IMEEditBoxInfo.dwCompTextColor = 0xffc8c8ff;
			g_pCurSelTemplate->m_ControlInfo.IMEEditBoxInfo.dwCompBackColor = 0xc6000000;
			g_pCurSelTemplate->m_ControlInfo.IMEEditBoxInfo.dwCompCaretColor = 0xffffffff;
			g_pCurSelTemplate->m_ControlInfo.IMEEditBoxInfo.dwCandiTextColor = 0xffc8c8c8;
			g_pCurSelTemplate->m_ControlInfo.IMEEditBoxInfo.dwCandiBackColor = 0x80000000;
			g_pCurSelTemplate->m_ControlInfo.IMEEditBoxInfo.dwCandiSelTextColor = 0xffffffff;
			g_pCurSelTemplate->m_ControlInfo.IMEEditBoxInfo.dwCandiSelBackColor = 0x809e9e9e;
			g_pCurSelTemplate->m_ControlInfo.IMEEditBoxInfo.dwIndiTextColor = 0xffffffff;
			g_pCurSelTemplate->m_ControlInfo.IMEEditBoxInfo.dwReadingTextColor = 0xbcffffff;
			g_pCurSelTemplate->m_ControlInfo.IMEEditBoxInfo.dwReadingBackColor = 0x80000000;
			g_pCurSelTemplate->m_ControlInfo.IMEEditBoxInfo.dwReadingSelTextColor = 0xffff0000;
			g_pCurSelTemplate->m_ControlInfo.IMEEditBoxInfo.dwReadingSelBackColor = 0x80505050;
			Element.dwFontFormat = DT_LEFT | DT_VCENTER;
			m_TemplateTree.InsertItem( "Body", 2, 3, hInsertItem );
			g_pCurSelTemplate->AddUIElement( "", 0, "Body", Element );
			m_TemplateTree.InsertItem( "Left Side", 2, 3, hInsertItem );
			g_pCurSelTemplate->AddUIElement( "", 0, "Left Side", Element );
			m_TemplateTree.InsertItem( "Right Side", 2, 3, hInsertItem );
			g_pCurSelTemplate->AddUIElement( "", 0, "Right Side", Element );
			m_TemplateTree.InsertItem( "Top Side", 2, 3, hInsertItem );
			g_pCurSelTemplate->AddUIElement( "", 0, "Top Side", Element );
			m_TemplateTree.InsertItem( "Bottom Side", 2, 3, hInsertItem );
			g_pCurSelTemplate->AddUIElement( "", 0, "Bottom Side", Element );
			Element.dwFontFormat = DT_CENTER | DT_VCENTER;
			m_TemplateTree.InsertItem( "Indicator", 2, 3, hInsertItem );
			g_pCurSelTemplate->AddUIElement( "", 0, "Indicator", Element );
			break;
		case UI_CONTROL_LISTBOX:
			Element.dwFontFormat = DT_LEFT | DT_VCENTER;
			Element.FontColor.dwColor[ 0 ] = 0xff000000;
			Element.FontColor.dwColor[ 3 ] = 0xff000000;
			Element.FontColor.dwColor[ 4 ] = 0xff000000;
			Element.FontColor.dwColor[ 5 ] = 0xff000000;
			g_pCurSelTemplate->m_ControlInfo.ListBoxInfo.fScrollBarSize = 16.0f / DEFAULT_UI_SCREEN_WIDTH;
			m_TemplateTree.InsertItem( "Body", 2, 3, hInsertItem );
			g_pCurSelTemplate->AddUIElement( "", 0, "Body", Element );
			m_TemplateTree.InsertItem( "Select Item", 2, 3, hInsertItem );
			g_pCurSelTemplate->AddUIElement( "", 0, "Select Item", Element );
			break;
		case UI_CONTROL_SCROLLBAR:
			g_pCurSelTemplate->m_ControlInfo.ScrollBarInfo.fThumbVariableLenth = 4.0f / DEFAULT_UI_SCREEN_HEIGHT;
			m_TemplateTree.InsertItem( "Base", 2, 3, hInsertItem );
			g_pCurSelTemplate->AddUIElement( "", 0, "Base", Element );
			m_TemplateTree.InsertItem( "Up Button", 2, 3, hInsertItem );
			g_pCurSelTemplate->AddUIElement( "", 0, "Up Button", Element );
			m_TemplateTree.InsertItem( "Down Button", 2, 3, hInsertItem );
			g_pCurSelTemplate->AddUIElement( "", 0, "Down Button", Element );
			m_TemplateTree.InsertItem( "Thumb", 2, 3, hInsertItem );
			g_pCurSelTemplate->AddUIElement( "", 0, "Thumb", Element );
			m_TemplateTree.InsertItem( "Up Button Light", 2, 3, hInsertItem );
			g_pCurSelTemplate->AddUIElement( "", 0, "Up Button Light", Element );
			m_TemplateTree.InsertItem( "Down Button Light", 2, 3, hInsertItem );
			g_pCurSelTemplate->AddUIElement( "", 0, "Down Button Light", Element );
			m_TemplateTree.InsertItem( "Thumb Light", 2, 3, hInsertItem );
			g_pCurSelTemplate->AddUIElement( "", 0, "Thumb Light", Element );
			g_vecScrollBar.push_back( g_pCurSelTemplate );
			break;
		case UI_CONTROL_PROGRESSBAR:
			m_TemplateTree.InsertItem( "Track", 2, 3, hInsertItem );
			g_pCurSelTemplate->AddUIElement( "", 0, "Track", Element );
			m_TemplateTree.InsertItem( "Progress", 2, 3, hInsertItem );
			g_pCurSelTemplate->AddUIElement( "", 0, "Progress", Element );
			g_vecScrollBar.push_back( g_pCurSelTemplate );
			break;
		case UI_CONTROL_CUSTOM:
			m_TemplateTree.InsertItem( "Main", 2, 3, hInsertItem );
			g_pCurSelTemplate->AddUIElement( "", 0, "Main", Element );
			break;
		case UI_CONTROL_TEXTBOX:
			g_pCurSelTemplate->m_ControlInfo.TextBoxInfo.fScrollBarSize = 16.0f / DEFAULT_UI_SCREEN_WIDTH;
			m_TemplateTree.InsertItem( "Normal", 2, 3, hInsertItem );
			g_pCurSelTemplate->AddUIElement( "", 0, "Normal", Element );
			break;
		case UI_CONTROL_HTMLTEXTBOX:
			g_pCurSelTemplate->m_ControlInfo.HtmlTextBoxInfo.fScrollBarSize = 16.0f / DEFAULT_UI_SCREEN_WIDTH;
			m_TemplateTree.InsertItem( "Normal", 2, 3, hInsertItem );
			g_pCurSelTemplate->AddUIElement( "", 0, "Normal", Element );
			break;
		case UI_CONTROL_TEXTURECONTROL:
			m_TemplateTree.InsertItem( "Normal", 2, 3, hInsertItem );
			g_pCurSelTemplate->AddUIElement( "", 0, "Normal", Element );
			break;
		case UI_CONTROL_TREECONTROL:
			g_pCurSelTemplate->m_ControlInfo.TreeInfo.fScrollBarSize = 16.0f / DEFAULT_UI_SCREEN_WIDTH;
			g_pCurSelTemplate->m_ControlInfo.TreeInfo.fLineSpace = 0.0f;
			g_pCurSelTemplate->m_ControlInfo.TreeInfo.fIndentSize = 0.0f;
			m_TemplateTree.InsertItem( "Button1", 2, 3, hInsertItem );
			g_pCurSelTemplate->AddUIElement( "", 0, "Button1", Element );
			m_TemplateTree.InsertItem( "Button2", 2, 3, hInsertItem );
			g_pCurSelTemplate->AddUIElement( "", 0, "Button2", Element );
			break;
		case UI_CONTROL_QUESTTREECONTROL:
			g_pCurSelTemplate->m_ControlInfo.QuestTreeInfo.fScrollBarSize = 16.0f / DEFAULT_UI_SCREEN_WIDTH;
			g_pCurSelTemplate->m_ControlInfo.QuestTreeInfo.fLineSpace = 0.0f;
			g_pCurSelTemplate->m_ControlInfo.QuestTreeInfo.fIndentSize = 0.0f;
			m_TemplateTree.InsertItem( "Button1", 2, 3, hInsertItem );
			g_pCurSelTemplate->AddUIElement( "", 0, "Button1", Element );
			m_TemplateTree.InsertItem( "Button2", 2, 3, hInsertItem );
			g_pCurSelTemplate->AddUIElement( "", 0, "Button2", Element );
			m_TemplateTree.InsertItem( "Frame 01", 2, 3, hInsertItem );
			g_pCurSelTemplate->AddUIElement( "", 0, "Frame Left", Element );
			m_TemplateTree.InsertItem( "Frame 02", 2, 3, hInsertItem );
			g_pCurSelTemplate->AddUIElement( "", 0, "Frame Middle", Element );
			m_TemplateTree.InsertItem( "Frame 03", 2, 3, hInsertItem );
			g_pCurSelTemplate->AddUIElement( "", 0, "Frame Right", Element );
			m_TemplateTree.InsertItem( "FrameSelect 01", 2, 3, hInsertItem );
			g_pCurSelTemplate->AddUIElement( "", 0, "SubFrame Left", Element );
			m_TemplateTree.InsertItem( "FrameSelect 02", 2, 3, hInsertItem );
			g_pCurSelTemplate->AddUIElement( "", 0, "SubFrame Middle", Element );
			m_TemplateTree.InsertItem( "FrameSelect 03", 2, 3, hInsertItem );
			g_pCurSelTemplate->AddUIElement( "", 0, "SubFrame Right", Element );
			m_TemplateTree.InsertItem( "IconCheck", 2, 3, hInsertItem );
			g_pCurSelTemplate->AddUIElement( "", 0, "IconCheck", Element );
			m_TemplateTree.InsertItem( "IconIng", 2, 3, hInsertItem );
			g_pCurSelTemplate->AddUIElement( "", 0, "IconIng", Element );
			break;
		case UI_CONTROL_ANIMATION:
			g_pCurSelTemplate->m_ControlInfo.AnimationInfo.bLoop = false;
#ifdef _ANIMATION_PLAY_CONTROL
			g_pCurSelTemplate->m_ControlInfo.AnimationInfo.PlayTimeType = UI_ANIMATION_PLAYTIME_PERFRAME;
			g_pCurSelTemplate->m_ControlInfo.AnimationInfo.timePerAction = 0;
#endif
			m_TemplateTree.InsertItem( "Frame01", 2, 3, hInsertItem );
			g_pCurSelTemplate->AddUIElement( "", 0, "Frame01", Element );
			break;
		case UI_CONTROL_LINE_EDITBOX:
			g_pCurSelTemplate->m_ControlInfo.LineEditBoxInfo.dwTextColor = 0xff101010;
			g_pCurSelTemplate->m_ControlInfo.LineEditBoxInfo.dwSelTextColor = 0xffffffff;
			g_pCurSelTemplate->m_ControlInfo.LineEditBoxInfo.dwSelBackColor = 0xff28325c;
			g_pCurSelTemplate->m_ControlInfo.LineEditBoxInfo.dwCaretColor = 0xff000000;
			Element.dwFontFormat = DT_LEFT | DT_VCENTER;
			m_TemplateTree.InsertItem( "Normal", 2, 3, hInsertItem );
			g_pCurSelTemplate->AddUIElement( "", 0, "Normal", Element );
			break;
		case UI_CONTROL_LINE_IMEEDITBOX:
			g_pCurSelTemplate->m_ControlInfo.LineIMEEditBoxInfo.dwTextColor = 0xff101010;
			g_pCurSelTemplate->m_ControlInfo.LineIMEEditBoxInfo.dwSelTextColor = 0xffffffff;
			g_pCurSelTemplate->m_ControlInfo.LineIMEEditBoxInfo.dwSelBackColor = 0xff28325c;
			g_pCurSelTemplate->m_ControlInfo.LineIMEEditBoxInfo.dwCaretColor = 0xff000000;
			g_pCurSelTemplate->m_ControlInfo.LineIMEEditBoxInfo.dwCompTextColor = 0xffc8c8ff;
			g_pCurSelTemplate->m_ControlInfo.LineIMEEditBoxInfo.dwCompBackColor = 0xc6000000;
			g_pCurSelTemplate->m_ControlInfo.LineIMEEditBoxInfo.dwCompCaretColor = 0xffffffff;
			g_pCurSelTemplate->m_ControlInfo.LineIMEEditBoxInfo.dwCandiTextColor = 0xffc8c8c8;
			g_pCurSelTemplate->m_ControlInfo.LineIMEEditBoxInfo.dwCandiBackColor = 0x80000000;
			g_pCurSelTemplate->m_ControlInfo.LineIMEEditBoxInfo.dwCandiSelTextColor = 0xffffffff;
			g_pCurSelTemplate->m_ControlInfo.LineIMEEditBoxInfo.dwCandiSelBackColor = 0x809e9e9e;
			g_pCurSelTemplate->m_ControlInfo.LineIMEEditBoxInfo.dwReadingTextColor = 0xbcffffff;
			g_pCurSelTemplate->m_ControlInfo.LineIMEEditBoxInfo.dwReadingBackColor = 0x80000000;
			g_pCurSelTemplate->m_ControlInfo.LineIMEEditBoxInfo.dwReadingSelTextColor = 0xffff0000;
			g_pCurSelTemplate->m_ControlInfo.LineIMEEditBoxInfo.dwReadingSelBackColor = 0x80505050;
			Element.dwFontFormat = DT_LEFT | DT_VCENTER;
			m_TemplateTree.InsertItem( "Normal", 2, 3, hInsertItem );
			g_pCurSelTemplate->AddUIElement( "", 0, "Normal", Element );
			break;
		case UI_CONTROL_MOVIECONTROL:
			m_TemplateTree.InsertItem( "Normal", 2, 3, hInsertItem );
			g_pCurSelTemplate->AddUIElement( "", 0, "Normal", Element );
			break;
		case UI_CONTROL_LISTBOXEX:
			Element.dwFontFormat = DT_LEFT | DT_VCENTER;
			Element.FontColor.dwColor[ 0 ] = 0xff000000;
			Element.FontColor.dwColor[ 3 ] = 0xff000000;
			Element.FontColor.dwColor[ 4 ] = 0xff000000;
			Element.FontColor.dwColor[ 5 ] = 0xff000000;
			g_pCurSelTemplate->m_ControlInfo.ListBoxExInfo.fScrollBarSize = 16.0f / DEFAULT_UI_SCREEN_WIDTH;
			m_TemplateTree.InsertItem( "Body", 2, 3, hInsertItem );
			g_pCurSelTemplate->AddUIElement( "", 0, "Body", Element );
			m_TemplateTree.InsertItem( "Select Item", 2, 3, hInsertItem );
			g_pCurSelTemplate->AddUIElement( "", 0, "Select Item", Element );
			break;
		default:
			ASSERT(0&&"CTemplatePaneView::NewTemplate");
			break;
	}

	CEtUIToolView *pView;
	CTemplateView *pTemplateView;

	pView = ( CEtUIToolView * )CGlobalValue::GetInstance().m_pCurView;
	pTemplateView = ( CTemplateView * )pView->GetTabView( CDummyView::TEMPLATE_VIEW );
	pTemplateView->AddTemplateControl( g_pCurSelTemplate );

	m_TemplateTree.Expand( hSelectItem, TVE_EXPAND );
}


void CTemplatePaneView::NewTemplate( CString &szTemplateName ,UI_CONTROL_TYPE UIType)
{
	HTREEITEM hInsertItem, hSelectItem;
	//UI_CONTROL_TYPE UIType;
	SUIElement Element;

	hSelectItem = m_TemplateTree.GetSelectedItem();
	hInsertItem = m_TemplateTree.InsertItem( szTemplateName, 2, 3, hSelectItem );

	g_pCurSelTemplate = new CUIToolTemplate();
	g_pCurSelTemplate->SetTemplateName( szTemplateName );
	UIType = ( UI_CONTROL_TYPE )GetTreeItemIndex( hSelectItem );

#if defined(SW_ADD_SEPARATE_STATIC_20091014_hayannal2009)
	if( UIType >= UI_CONTROL_SIZE )
	{
		CString szItemName = m_TemplateTree.GetItemText( hSelectItem );

		for( int ndx = 0; ndx < MAX_SEPARATE_STATIC; ++ndx )
		{
			if( _stricmp( szItemName, g_aszTreeItem[UI_CONTROL_SIZE+ndx] ) == 0 )
			{
				UIType = UI_CONTROL_STATIC;
				bool bFind = false;
				for( int i = 0; i < (int)m_vecSeparateStatic[ndx].size(); ++i )
				{
					if( _stricmp( szTemplateName, m_vecSeparateStatic[ndx][i].c_str() ) == 0 )
					{
						bFind = true;
						break;
					}
				}
				if( !bFind ) m_vecSeparateStatic[ndx].push_back((const char *)szTemplateName);
			}
		}



		//if( _stricmp( szItemName, "Static_Text" ) == 0 )
		//{
		//	UIType = UI_CONTROL_STATIC;
		//	bool bFind = false;
		//	for( int i = 0; i < (int)m_vecSeparateStatic[0][i].size(); ++i )
		//	{
		//		if( _stricmp( szTemplateName, m_vecSeparateStatic[0][i].c_str() ) == 0 )
		//		{
		//			bFind = true;
		//			break;
		//		}
		//	}
		//	if( !bFind ) m_vecSeparateStatic[0].push_back((const char *)szTemplateName);
		//}
		//else if( _stricmp( szItemName, "Static_Line" ) == 0 )
		//{
		//	UIType = UI_CONTROL_STATIC;
		//	bool bFind = false;
		//	for( int i = 0; i < (int)m_vecStaticText.size(); ++i )
		//	{
		//		if( _stricmp( szTemplateName, m_vecStaticText[i].c_str() ) == 0 )
		//		{
		//			bFind = true;
		//			break;
		//		}
		//	}
		//	if( !bFind ) m_vecStaticLine.push_back((const char *)szTemplateName);
		//}
		//else if( _stricmp( szItemName, "Static_Board" ) == 0 )
		//{
		//	UIType = UI_CONTROL_STATIC;
		//	bool bFind = false;
		//	for( int i = 0; i < (int)m_vecStaticText.size(); ++i )
		//	{
		//		if( _stricmp( szTemplateName, m_vecStaticText[i].c_str() ) == 0 )
		//		{
		//			bFind = true;
		//			break;
		//		}
		//	}
		//	if( !bFind ) m_vecStaticBoard.push_back((const char *)szTemplateName);
		//}
		//else if( _stricmp( szItemName, "Static_Etc" ) == 0 )
		//{
		//	UIType = UI_CONTROL_STATIC;
		//	bool bFind = false;
		//	for( int i = 0; i < (int)m_vecStaticText.size(); ++i )
		//	{
		//		if( _stricmp( szTemplateName, m_vecStaticText[i].c_str() ) == 0 )
		//		{
		//			bFind = true;
		//			break;
		//		}
		//	}
		//	if( !bFind ) m_vecStaticEtc.push_back((const char *)szTemplateName);
		//}
	}
#endif

	g_pCurSelTemplate->SetUIType( UIType );
	m_TemplateTree.SetItemData( hInsertItem, ( DWORD_PTR )g_pCurSelTemplate );
	m_vecUIToolTemplate.push_back( g_pCurSelTemplate );

	if( g_InitControl[ UIType ].fWidth == 0.0f )
		ASSERT(0&&"TemplateView.cpp의 g_InitControl의 값이 없는 듯 합니다.");

	g_pCurSelTemplate->m_fDefaultWidth = g_InitControl[ UIType ].fWidth;
	g_pCurSelTemplate->m_fDefaultHeight = g_InitControl[ UIType ].fHeight;
	g_pCurSelTemplate->m_bChanged = true;

	switch( UIType )
	{
		case UI_CONTROL_STATIC:
			m_TemplateTree.InsertItem( "Normal", 2, 3, hInsertItem );
			g_pCurSelTemplate->AddUIElement( "", 0, "Normal", Element );
			break;
		case UI_CONTROL_BUTTON:
			g_pCurSelTemplate->m_ControlInfo.ButtonInfo.ButtonType = UI_BUTTON_CLICK;
			g_pCurSelTemplate->m_ControlInfo.ButtonInfo.fMoveOffsetHori = 1.0f / DEFAULT_UI_SCREEN_WIDTH;
			g_pCurSelTemplate->m_ControlInfo.ButtonInfo.fMoveOffsetVert = 1.0f / DEFAULT_UI_SCREEN_HEIGHT;
			m_TemplateTree.InsertItem( "Normal", 2, 3, hInsertItem );
			g_pCurSelTemplate->AddUIElement( "", 0, "Normal", Element );
			break;
		case UI_CONTROL_CHECKBOX:
			g_pCurSelTemplate->m_ControlInfo.CheckBoxInfo.fSpace = 2.0f / DEFAULT_UI_SCREEN_WIDTH;
			Element.dwFontFormat = DT_LEFT | DT_VCENTER;
			m_TemplateTree.InsertItem( "Main", 2, 3, hInsertItem );
			g_pCurSelTemplate->AddUIElement( "", 0, "Main", Element );
			m_TemplateTree.InsertItem( "Check Mark", 2, 3, hInsertItem );
			g_pCurSelTemplate->AddUIElement( "", 0, "Check Mark", Element );
			break;
		case UI_CONTROL_RADIOBUTTON:
			g_pCurSelTemplate->m_ControlInfo.RadioButtonInfo.fSpace = 2.0f / DEFAULT_UI_SCREEN_WIDTH;
			g_pCurSelTemplate->m_ControlInfo.RadioButtonInfo.fMoveOffsetHori = 1.0f / DEFAULT_UI_SCREEN_WIDTH;
			g_pCurSelTemplate->m_ControlInfo.RadioButtonInfo.fMoveOffsetVert = 1.0f / DEFAULT_UI_SCREEN_HEIGHT;
			Element.dwFontFormat = DT_LEFT | DT_VCENTER;
			m_TemplateTree.InsertItem( "Main", 2, 3, hInsertItem );
			g_pCurSelTemplate->AddUIElement( "", 0, "Main", Element );
			m_TemplateTree.InsertItem( "Check Mark", 2, 3, hInsertItem );
			g_pCurSelTemplate->AddUIElement( "", 0, "Check Mark", Element );
			break;
		case UI_CONTROL_COMBOBOX:
			g_pCurSelTemplate->m_ControlInfo.ComboBoxInfo.fMoveOffsetHori = 1.0f / DEFAULT_UI_SCREEN_WIDTH;
			g_pCurSelTemplate->m_ControlInfo.ComboBoxInfo.fMoveOffsetVert = 1.0f / DEFAULT_UI_SCREEN_HEIGHT;
			g_pCurSelTemplate->m_ControlInfo.ComboBoxInfo.fDropdownSize = 100.0f / DEFAULT_UI_SCREEN_HEIGHT;
			g_pCurSelTemplate->m_ControlInfo.ComboBoxInfo.fScrollBarSize = 16.0f / DEFAULT_UI_SCREEN_WIDTH;
			Element.FontColor.dwColor[ 0 ] = 0xff000000;
			Element.FontColor.dwColor[ 3 ] = 0xff000000;
			Element.FontColor.dwColor[ 4 ] = 0xff000000;
			Element.FontColor.dwColor[ 5 ] = 0xff000000;
			m_TemplateTree.InsertItem( "Body", 2, 3, hInsertItem );
			g_pCurSelTemplate->AddUIElement( "", 0, "Body", Element );
			m_TemplateTree.InsertItem( "Combo Button", 2, 3, hInsertItem );
			g_pCurSelTemplate->AddUIElement( "", 0, "Combo Button", Element );
			Element.dwFontFormat = DT_LEFT | DT_VCENTER;
			m_TemplateTree.InsertItem( "Dropdown", 2, 3, hInsertItem );
			g_pCurSelTemplate->AddUIElement( "", 0, "Dropdown", Element );
			m_TemplateTree.InsertItem( "Select Item", 2, 3, hInsertItem );
			g_pCurSelTemplate->AddUIElement( "", 0, "Select Item", Element );
			break;
		case UI_CONTROL_SLIDER:
			m_TemplateTree.InsertItem( "Track", 2, 3, hInsertItem );
			g_pCurSelTemplate->AddUIElement( "", 0, "Track", Element );
			m_TemplateTree.InsertItem( "Button", 2, 3, hInsertItem );
			g_pCurSelTemplate->AddUIElement( "", 0, "Button", Element );
			break;
		case UI_CONTROL_EDITBOX:
			g_pCurSelTemplate->m_ControlInfo.EditBoxInfo.fBorder = 5.0f / DEFAULT_UI_SCREEN_WIDTH;
			g_pCurSelTemplate->m_ControlInfo.EditBoxInfo.fSpace = 4.0f / DEFAULT_UI_SCREEN_WIDTH;
			g_pCurSelTemplate->m_ControlInfo.EditBoxInfo.dwTextColor = 0xff101010;
			g_pCurSelTemplate->m_ControlInfo.EditBoxInfo.dwSelTextColor = 0xffffffff;
			g_pCurSelTemplate->m_ControlInfo.EditBoxInfo.dwSelBackColor = 0xff28325c;
			g_pCurSelTemplate->m_ControlInfo.EditBoxInfo.dwCaretColor = 0xff000000;
			Element.dwFontFormat = DT_LEFT | DT_VCENTER;
			m_TemplateTree.InsertItem( "Body", 2, 3, hInsertItem );
			g_pCurSelTemplate->AddUIElement( "", 0, "Body", Element );
			m_TemplateTree.InsertItem( "Left Side", 2, 3, hInsertItem );
			g_pCurSelTemplate->AddUIElement( "", 0, "Left Side", Element );
			m_TemplateTree.InsertItem( "Right Side", 2, 3, hInsertItem );
			g_pCurSelTemplate->AddUIElement( "", 0, "Right Side", Element );
			m_TemplateTree.InsertItem( "Top Side", 2, 3, hInsertItem );
			g_pCurSelTemplate->AddUIElement( "", 0, "Top Side", Element );
			m_TemplateTree.InsertItem( "Bottom Side", 2, 3, hInsertItem );
			g_pCurSelTemplate->AddUIElement( "", 0, "Bottom Side", Element );
			break;
		case UI_CONTROL_IMEEDITBOX:
			g_pCurSelTemplate->m_ControlInfo.IMEEditBoxInfo.fBorder = 5.0f / DEFAULT_UI_SCREEN_WIDTH;
			g_pCurSelTemplate->m_ControlInfo.IMEEditBoxInfo.fSpace = 4.0f / DEFAULT_UI_SCREEN_WIDTH;
			g_pCurSelTemplate->m_ControlInfo.IMEEditBoxInfo.fIndicatorSize = 16.0f / DEFAULT_UI_SCREEN_WIDTH;
			g_pCurSelTemplate->m_ControlInfo.IMEEditBoxInfo.dwTextColor = 0xff101010;
			g_pCurSelTemplate->m_ControlInfo.IMEEditBoxInfo.dwSelTextColor = 0xffffffff;
			g_pCurSelTemplate->m_ControlInfo.IMEEditBoxInfo.dwSelBackColor = 0xff28325c;
			g_pCurSelTemplate->m_ControlInfo.IMEEditBoxInfo.dwCaretColor = 0xff000000;
			g_pCurSelTemplate->m_ControlInfo.IMEEditBoxInfo.dwCompTextColor = 0xffc8c8ff;
			g_pCurSelTemplate->m_ControlInfo.IMEEditBoxInfo.dwCompBackColor = 0xc6000000;
			g_pCurSelTemplate->m_ControlInfo.IMEEditBoxInfo.dwCompCaretColor = 0xffffffff;
			g_pCurSelTemplate->m_ControlInfo.IMEEditBoxInfo.dwCandiTextColor = 0xffc8c8c8;
			g_pCurSelTemplate->m_ControlInfo.IMEEditBoxInfo.dwCandiBackColor = 0x80000000;
			g_pCurSelTemplate->m_ControlInfo.IMEEditBoxInfo.dwCandiSelTextColor = 0xffffffff;
			g_pCurSelTemplate->m_ControlInfo.IMEEditBoxInfo.dwCandiSelBackColor = 0x809e9e9e;
			g_pCurSelTemplate->m_ControlInfo.IMEEditBoxInfo.dwIndiTextColor = 0xffffffff;
			g_pCurSelTemplate->m_ControlInfo.IMEEditBoxInfo.dwReadingTextColor = 0xbcffffff;
			g_pCurSelTemplate->m_ControlInfo.IMEEditBoxInfo.dwReadingBackColor = 0x80000000;
			g_pCurSelTemplate->m_ControlInfo.IMEEditBoxInfo.dwReadingSelTextColor = 0xffff0000;
			g_pCurSelTemplate->m_ControlInfo.IMEEditBoxInfo.dwReadingSelBackColor = 0x80505050;
			Element.dwFontFormat = DT_LEFT | DT_VCENTER;
			m_TemplateTree.InsertItem( "Body", 2, 3, hInsertItem );
			g_pCurSelTemplate->AddUIElement( "", 0, "Body", Element );
			m_TemplateTree.InsertItem( "Left Side", 2, 3, hInsertItem );
			g_pCurSelTemplate->AddUIElement( "", 0, "Left Side", Element );
			m_TemplateTree.InsertItem( "Right Side", 2, 3, hInsertItem );
			g_pCurSelTemplate->AddUIElement( "", 0, "Right Side", Element );
			m_TemplateTree.InsertItem( "Top Side", 2, 3, hInsertItem );
			g_pCurSelTemplate->AddUIElement( "", 0, "Top Side", Element );
			m_TemplateTree.InsertItem( "Bottom Side", 2, 3, hInsertItem );
			g_pCurSelTemplate->AddUIElement( "", 0, "Bottom Side", Element );
			Element.dwFontFormat = DT_CENTER | DT_VCENTER;
			m_TemplateTree.InsertItem( "Indicator", 2, 3, hInsertItem );
			g_pCurSelTemplate->AddUIElement( "", 0, "Indicator", Element );
			break;
		case UI_CONTROL_LISTBOX:
			Element.dwFontFormat = DT_LEFT | DT_VCENTER;
			Element.FontColor.dwColor[ 0 ] = 0xff000000;
			Element.FontColor.dwColor[ 3 ] = 0xff000000;
			Element.FontColor.dwColor[ 4 ] = 0xff000000;
			Element.FontColor.dwColor[ 5 ] = 0xff000000;
			g_pCurSelTemplate->m_ControlInfo.ListBoxInfo.fScrollBarSize = 16.0f / DEFAULT_UI_SCREEN_WIDTH;
			m_TemplateTree.InsertItem( "Body", 2, 3, hInsertItem );
			g_pCurSelTemplate->AddUIElement( "", 0, "Body", Element );
			m_TemplateTree.InsertItem( "Select Item", 2, 3, hInsertItem );
			g_pCurSelTemplate->AddUIElement( "", 0, "Select Item", Element );
			break;
		case UI_CONTROL_SCROLLBAR:
			g_pCurSelTemplate->m_ControlInfo.ScrollBarInfo.fThumbVariableLenth = 4.0f / DEFAULT_UI_SCREEN_HEIGHT;
			m_TemplateTree.InsertItem( "Base", 2, 3, hInsertItem );
			g_pCurSelTemplate->AddUIElement( "", 0, "Base", Element );
			m_TemplateTree.InsertItem( "Up Button", 2, 3, hInsertItem );
			g_pCurSelTemplate->AddUIElement( "", 0, "Up Button", Element );
			m_TemplateTree.InsertItem( "Down Button", 2, 3, hInsertItem );
			g_pCurSelTemplate->AddUIElement( "", 0, "Down Button", Element );
			m_TemplateTree.InsertItem( "Thumb", 2, 3, hInsertItem );
			g_pCurSelTemplate->AddUIElement( "", 0, "Thumb", Element );
			m_TemplateTree.InsertItem( "Up Button Light", 2, 3, hInsertItem );
			g_pCurSelTemplate->AddUIElement( "", 0, "Up Button Light", Element );
			m_TemplateTree.InsertItem( "Down Button Light", 2, 3, hInsertItem );
			g_pCurSelTemplate->AddUIElement( "", 0, "Down Button Light", Element );
			m_TemplateTree.InsertItem( "Thumb Light", 2, 3, hInsertItem );
			g_pCurSelTemplate->AddUIElement( "", 0, "Thumb Light", Element );
			g_vecScrollBar.push_back( g_pCurSelTemplate );
			break;
		case UI_CONTROL_PROGRESSBAR:
			m_TemplateTree.InsertItem( "Track", 2, 3, hInsertItem );
			g_pCurSelTemplate->AddUIElement( "", 0, "Track", Element );
			m_TemplateTree.InsertItem( "Progress", 2, 3, hInsertItem );
			g_pCurSelTemplate->AddUIElement( "", 0, "Progress", Element );
			g_vecScrollBar.push_back( g_pCurSelTemplate );
			break;
		case UI_CONTROL_CUSTOM:
			m_TemplateTree.InsertItem( "Main", 2, 3, hInsertItem );
			g_pCurSelTemplate->AddUIElement( "", 0, "Main", Element );
			break;
		case UI_CONTROL_TEXTBOX:
			g_pCurSelTemplate->m_ControlInfo.TextBoxInfo.fScrollBarSize = 16.0f / DEFAULT_UI_SCREEN_WIDTH;
			m_TemplateTree.InsertItem( "Normal", 2, 3, hInsertItem );
			g_pCurSelTemplate->AddUIElement( "", 0, "Normal", Element );
			break;
		case UI_CONTROL_HTMLTEXTBOX:
			g_pCurSelTemplate->m_ControlInfo.HtmlTextBoxInfo.fScrollBarSize = 16.0f / DEFAULT_UI_SCREEN_WIDTH;
			m_TemplateTree.InsertItem( "Normal", 2, 3, hInsertItem );
			g_pCurSelTemplate->AddUIElement( "", 0, "Normal", Element );
			break;
		case UI_CONTROL_TEXTURECONTROL:
			m_TemplateTree.InsertItem( "Normal", 2, 3, hInsertItem );
			g_pCurSelTemplate->AddUIElement( "", 0, "Normal", Element );
			break;
		case UI_CONTROL_TREECONTROL:
			g_pCurSelTemplate->m_ControlInfo.TreeInfo.fScrollBarSize = 16.0f / DEFAULT_UI_SCREEN_WIDTH;
			g_pCurSelTemplate->m_ControlInfo.TreeInfo.fLineSpace = 0.0f;
			g_pCurSelTemplate->m_ControlInfo.TreeInfo.fIndentSize = 0.0f;
			m_TemplateTree.InsertItem( "Button1", 2, 3, hInsertItem );
			g_pCurSelTemplate->AddUIElement( "", 0, "Button1", Element );
			m_TemplateTree.InsertItem( "Button2", 2, 3, hInsertItem );
			g_pCurSelTemplate->AddUIElement( "", 0, "Button2", Element );
			break;
		case UI_CONTROL_QUESTTREECONTROL:
			g_pCurSelTemplate->m_ControlInfo.QuestTreeInfo.fScrollBarSize = 16.0f / DEFAULT_UI_SCREEN_WIDTH;
			g_pCurSelTemplate->m_ControlInfo.QuestTreeInfo.fLineSpace = 0.0f;
			g_pCurSelTemplate->m_ControlInfo.QuestTreeInfo.fIndentSize = 0.0f;
			m_TemplateTree.InsertItem( "Button1", 2, 3, hInsertItem );
			g_pCurSelTemplate->AddUIElement( "", 0, "Button1", Element );
			m_TemplateTree.InsertItem( "Button2", 2, 3, hInsertItem );
			g_pCurSelTemplate->AddUIElement( "", 0, "Button2", Element );
			m_TemplateTree.InsertItem( "Frame 01", 2, 3, hInsertItem );
			g_pCurSelTemplate->AddUIElement( "", 0, "Frame Left", Element );
			m_TemplateTree.InsertItem( "Frame 02", 2, 3, hInsertItem );
			g_pCurSelTemplate->AddUIElement( "", 0, "Frame Middle", Element );
			m_TemplateTree.InsertItem( "Frame 03", 2, 3, hInsertItem );
			g_pCurSelTemplate->AddUIElement( "", 0, "Frame Right", Element );
			m_TemplateTree.InsertItem( "FrameSelect 01", 2, 3, hInsertItem );
			g_pCurSelTemplate->AddUIElement( "", 0, "SubFrame Left", Element );
			m_TemplateTree.InsertItem( "FrameSelect 02", 2, 3, hInsertItem );
			g_pCurSelTemplate->AddUIElement( "", 0, "SubFrame Middle", Element );
			m_TemplateTree.InsertItem( "FrameSelect 03", 2, 3, hInsertItem );
			g_pCurSelTemplate->AddUIElement( "", 0, "SubFrame Right", Element );
			m_TemplateTree.InsertItem( "IconCheck", 2, 3, hInsertItem );
			g_pCurSelTemplate->AddUIElement( "", 0, "IconCheck", Element );
			m_TemplateTree.InsertItem( "IconIng", 2, 3, hInsertItem );
			g_pCurSelTemplate->AddUIElement( "", 0, "IconIng", Element );
			break;
		case UI_CONTROL_ANIMATION:
			g_pCurSelTemplate->m_ControlInfo.AnimationInfo.bLoop = false;
#ifdef _ANIMATION_PLAY_CONTROL
			g_pCurSelTemplate->m_ControlInfo.AnimationInfo.PlayTimeType = UI_ANIMATION_PLAYTIME_PERFRAME;
			g_pCurSelTemplate->m_ControlInfo.AnimationInfo.timePerAction = 0;
#endif
			m_TemplateTree.InsertItem( "Frame01", 2, 3, hInsertItem );
			g_pCurSelTemplate->AddUIElement( "", 0, "Frame01", Element );
			break;
		case UI_CONTROL_LINE_EDITBOX:
			g_pCurSelTemplate->m_ControlInfo.LineEditBoxInfo.dwTextColor = 0xff101010;
			g_pCurSelTemplate->m_ControlInfo.LineEditBoxInfo.dwSelTextColor = 0xffffffff;
			g_pCurSelTemplate->m_ControlInfo.LineEditBoxInfo.dwSelBackColor = 0xff28325c;
			g_pCurSelTemplate->m_ControlInfo.LineEditBoxInfo.dwCaretColor = 0xff000000;
			Element.dwFontFormat = DT_LEFT | DT_VCENTER;
			m_TemplateTree.InsertItem( "Normal", 2, 3, hInsertItem );
			g_pCurSelTemplate->AddUIElement( "", 0, "Normal", Element );
			break;
		case UI_CONTROL_LINE_IMEEDITBOX:
			g_pCurSelTemplate->m_ControlInfo.LineIMEEditBoxInfo.dwTextColor = 0xff101010;
			g_pCurSelTemplate->m_ControlInfo.LineIMEEditBoxInfo.dwSelTextColor = 0xffffffff;
			g_pCurSelTemplate->m_ControlInfo.LineIMEEditBoxInfo.dwSelBackColor = 0xff28325c;
			g_pCurSelTemplate->m_ControlInfo.LineIMEEditBoxInfo.dwCaretColor = 0xff000000;
			g_pCurSelTemplate->m_ControlInfo.LineIMEEditBoxInfo.dwCompTextColor = 0xffc8c8ff;
			g_pCurSelTemplate->m_ControlInfo.LineIMEEditBoxInfo.dwCompBackColor = 0xc6000000;
			g_pCurSelTemplate->m_ControlInfo.LineIMEEditBoxInfo.dwCompCaretColor = 0xffffffff;
			g_pCurSelTemplate->m_ControlInfo.LineIMEEditBoxInfo.dwCandiTextColor = 0xffc8c8c8;
			g_pCurSelTemplate->m_ControlInfo.LineIMEEditBoxInfo.dwCandiBackColor = 0x80000000;
			g_pCurSelTemplate->m_ControlInfo.LineIMEEditBoxInfo.dwCandiSelTextColor = 0xffffffff;
			g_pCurSelTemplate->m_ControlInfo.LineIMEEditBoxInfo.dwCandiSelBackColor = 0x809e9e9e;
			g_pCurSelTemplate->m_ControlInfo.LineIMEEditBoxInfo.dwReadingTextColor = 0xbcffffff;
			g_pCurSelTemplate->m_ControlInfo.LineIMEEditBoxInfo.dwReadingBackColor = 0x80000000;
			g_pCurSelTemplate->m_ControlInfo.LineIMEEditBoxInfo.dwReadingSelTextColor = 0xffff0000;
			g_pCurSelTemplate->m_ControlInfo.LineIMEEditBoxInfo.dwReadingSelBackColor = 0x80505050;
			Element.dwFontFormat = DT_LEFT | DT_VCENTER;
			m_TemplateTree.InsertItem( "Normal", 2, 3, hInsertItem );
			g_pCurSelTemplate->AddUIElement( "", 0, "Normal", Element );
			break;
		case UI_CONTROL_MOVIECONTROL:
			m_TemplateTree.InsertItem( "Normal", 2, 3, hInsertItem );
			g_pCurSelTemplate->AddUIElement( "", 0, "Normal", Element );
			break;
		case UI_CONTROL_LISTBOXEX:
			Element.dwFontFormat = DT_LEFT | DT_VCENTER;
			Element.FontColor.dwColor[ 0 ] = 0xff000000;
			Element.FontColor.dwColor[ 3 ] = 0xff000000;
			Element.FontColor.dwColor[ 4 ] = 0xff000000;
			Element.FontColor.dwColor[ 5 ] = 0xff000000;
			g_pCurSelTemplate->m_ControlInfo.ListBoxExInfo.fScrollBarSize = 16.0f / DEFAULT_UI_SCREEN_WIDTH;
			m_TemplateTree.InsertItem( "Body", 2, 3, hInsertItem );
			g_pCurSelTemplate->AddUIElement( "", 0, "Body", Element );
			m_TemplateTree.InsertItem( "Select Item", 2, 3, hInsertItem );
			g_pCurSelTemplate->AddUIElement( "", 0, "Select Item", Element );
			break;
		default:
			ASSERT(0&&"CTemplatePaneView::NewTemplate");
			break;
	}

	CEtUIToolView *pView;
	CTemplateView *pTemplateView;

	pView = ( CEtUIToolView * )CGlobalValue::GetInstance().m_pCurView;
	pTemplateView = ( CTemplateView * )pView->GetTabView( CDummyView::TEMPLATE_VIEW );
	pTemplateView->AddTemplateControl( g_pCurSelTemplate );

	m_TemplateTree.Expand( hSelectItem, TVE_EXPAND );
}


void CTemplatePaneView::InsertElement( CString &szElementName, HTREEITEM hInsertItem )
{
	if( !g_pCurSelTemplate )
	{
		return;
	}

	SUIElement Element;
	m_TemplateTree.InsertItem( szElementName, 2, 3, hInsertItem );
	g_pCurSelTemplate->AddUIElement( "", 0, szElementName, Element );
	g_pCurSelTemplate->m_bChanged = true;
}

int CTemplatePaneView::GetTreeItemIndex( HTREEITEM hItem )
{
	HTREEITEM hParentItem, hChildItem;
	int nIndex;

	hParentItem = m_TemplateTree.GetParentItem( hItem );
	if( !hParentItem )
	{
		return -1;
	}
	hChildItem = m_TemplateTree.GetChildItem( hParentItem );
	nIndex = 0;
	while( 1 )
	{
		if( hChildItem == hItem )
		{
			return nIndex;
		}
		hChildItem = m_TemplateTree.GetNextSiblingItem( hChildItem );
		nIndex++;
		if( !hChildItem )
		{
			break;
		}
	}

	return -1;
}

void CTemplatePaneView::ProcessMultiSelect()
{
	int nCount, nDepth;
	HTREEITEM hSelectedItem, hSelectParentItem;

	nCount = m_TemplateTree.GetSelectedCount();
	if( nCount <= 1 )
	{
		return;
	}

	hSelectedItem = m_TemplateTree.GetFirstSelectedItem();
	hSelectParentItem = m_TemplateTree.GetParentItem( hSelectedItem );
	while( hSelectedItem != NULL )
	{
		nDepth = GetTreeDepth( hSelectedItem );
		if( nDepth != 4 )
		{
			m_TemplateTree.SelectAll( FALSE, m_TemplateTree.GetFirstSelectedItem() );
			return;
		}
		if( m_TemplateTree.GetParentItem( hSelectedItem ) != hSelectParentItem )
		{
			m_TemplateTree.SelectAll( FALSE, m_TemplateTree.GetFirstSelectedItem() );
			return;
		}
		hSelectedItem = m_TemplateTree.GetNextSelectedItem( hSelectedItem );
	}
}

void CTemplatePaneView::SaveAllTemplate()
{
	for( int i = 0; i < ( int )m_vecUIToolTemplate.size(); i++ )
	{
		if( ( m_vecUIToolTemplate[ i ] ) && ( m_vecUIToolTemplate[ i ]->m_bChanged ) )
		{
			char szFileName[ _MAX_PATH ];

			sprintf_s( szFileName, _MAX_PATH, "%s\\%s.uit", CMainFrame::GetUITemplateFolder(), m_vecUIToolTemplate[ i ]->m_Template.m_szTemplateName.c_str() );

			CFileStream Stream( szFileName, CFileStream::OPEN_WRITE );
			m_vecUIToolTemplate[ i ]->Save( Stream );

			if( m_vecUIToolTemplate[ i ]->m_Template.m_hTemplateTexture )
			{
				sprintf_s( szFileName, _MAX_PATH, "%s\\UITemplateTexture\\UIT_%s.dds", CMainFrame::GetResourceFolder(), m_vecUIToolTemplate[ i ]->m_Template.m_szTemplateName.c_str() );
				D3DXSaveTextureToFile( szFileName, D3DXIFF_DDS, m_vecUIToolTemplate[ i ]->m_Template.m_hTemplateTexture->GetTexturePtr(), NULL );
			}
		}
	}
}

CUIToolTemplate *CTemplatePaneView::FindTemplate( const char *pszName )
{
	for( int i = 0; i < ( int )m_vecUIToolTemplate.size(); i++ )
	{
		if( _stricmp( m_vecUIToolTemplate[ i ]->m_Template.m_szTemplateName.c_str(), pszName ) == 0 )
		{
			return m_vecUIToolTemplate[ i ];
		}else{
				CString name = pszName;
				this->NewTemplate(name,UI_CONTROL_TYPE::UI_CONTROL_STATIC);
				return m_vecUIToolTemplate[m_vecUIToolTemplate.size()-1];
		}
	}

	//
	char szMsg[128];
	sprintf_s( szMsg, _countof(szMsg), "%s.uit 파일을 찾을 수 없습니다.", pszName );
	MessageBox( szMsg );
	MessageBox( "ui파일 로드를 취소합니다." );
	CDebugSet::ToLogFile( "CTemplatePaneView::FindTemplate, %s not found!", pszName );
	return NULL;
}

void CTemplatePaneView::OnTvnSelchangedTemplateTree(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);

	int nDepth;
	HTREEITEM hSelectItem;
	std::vector< int > vecItemIndex;

	ProcessMultiSelect();
	hSelectItem = m_TemplateTree.GetSelectedItem();
	if( hSelectItem == NULL )
	{
		return;
	}
	nDepth = GetTreeDepth( hSelectItem );
	if( nDepth < 3 ) 
	{
		return;
	}

	if( nDepth == 3 )
	{
		g_pCurSelTemplate = ( CUIToolTemplate * )m_TemplateTree.GetItemData( hSelectItem );

		CTemplatePropertyPaneView *pPane;
		pPane = ( CTemplatePropertyPaneView * )GetPaneWnd( TEMPLATE_PROPERTY_PANE );
		pPane->RefreshProperty( g_pCurSelTemplate, vecItemIndex );
	}
	else if( nDepth == 4 )
	{
		HTREEITEM hParentItem;

		if( m_TemplateTree.GetSelectedCount() > 1 )
		{
			hSelectItem = m_TemplateTree.GetFirstSelectedItem();
		}
		hParentItem = m_TemplateTree.GetParentItem( hSelectItem );
		g_pCurSelTemplate = ( CUIToolTemplate * )m_TemplateTree.GetItemData( hParentItem );

		if( m_TemplateTree.GetSelectedCount() > 1 )
		{
			while( hSelectItem != NULL )
			{
				vecItemIndex.push_back( GetTreeItemIndex( hSelectItem ) );
				hSelectItem = m_TemplateTree.GetNextSelectedItem( hSelectItem );
			}
		}
		else
		{
			vecItemIndex.push_back( GetTreeItemIndex( hSelectItem ) );
		}

		CTemplatePropertyPaneView *pPane;
		pPane = ( CTemplatePropertyPaneView * )GetPaneWnd( TEMPLATE_PROPERTY_PANE );
		pPane->RefreshProperty( g_pCurSelTemplate, vecItemIndex );
	}

	*pResult = 0;
}

void CTemplatePaneView::OnDestroy()
{
	CFormView::OnDestroy();

	DeleteAllTreeItemData();
}

BOOL CTemplatePaneView::OnEraseBkgnd(CDC* pDC)
{
	return FALSE;
}

bool CTemplatePaneView::CheckDuplicateElement( CString &szElementName )
{
	HTREEITEM hItem;

	hItem = m_TemplateTree.GetSelectedItem();
	hItem = m_TemplateTree.GetChildItem( hItem );
	while( hItem )
	{
		CString szItemText;

		szItemText = m_TemplateTree.GetItemText( hItem );
		if( _stricmp( szItemText, szElementName ) == 0 )
		{
			return true;
		}
		hItem = m_TemplateTree.GetNextSiblingItem( hItem );
	}

	return false;
}

int CTemplatePaneView::GetElementCount()
{
	HTREEITEM treeItem(NULL);

	treeItem = m_TemplateTree.GetSelectedItem();
	if( !treeItem )
		return 0;

	treeItem = m_TemplateTree.GetParentItem( treeItem );
	treeItem = m_TemplateTree.GetChildItem( treeItem );

	int nCount(0);
	while( treeItem )
	{
		treeItem = m_TemplateTree.GetNextSiblingItem( treeItem );
		nCount++;
	}

	return nCount;
}

void CTemplatePaneView::OnTemplateAddelement()
{
	CTemplateNameDlg NameDlg;

	while( 1 )
	{
		if( NameDlg.DoModal() == IDOK )
		{
			if( ( NameDlg.m_szTemplateName.GetLength() > 0 ) && ( !CheckDuplicateElement( NameDlg.m_szTemplateName ) ) )
			{
				InsertElement( NameDlg.m_szTemplateName, m_TemplateTree.GetSelectedItem() );
				break;
			}
			else
			{
				MessageBox( "같은 이름을 가진 Element가 있습니다" );
			}
		}
		else
		{
			break;
		}
	}
}

void CTemplatePaneView::OnNMClickTemplateTree(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);

	int nDepth;
	HTREEITEM hSelectItem;
	std::vector< int > vecItemIndex;

	ProcessMultiSelect();
	hSelectItem = m_TemplateTree.GetSelectedItem();
	if( hSelectItem == NULL )
	{
		return;
	}
	nDepth = GetTreeDepth( hSelectItem );
	if( nDepth < 3 ) 
	{
		return;
	}

	if( nDepth == 3 )
	{
		g_pCurSelTemplate = ( CUIToolTemplate * )m_TemplateTree.GetItemData( hSelectItem );

		CTemplatePropertyPaneView *pPane;
		pPane = ( CTemplatePropertyPaneView * )GetPaneWnd( TEMPLATE_PROPERTY_PANE );
		pPane->RefreshProperty( g_pCurSelTemplate, vecItemIndex );
	}
	else if( nDepth == 4 )
	{
		HTREEITEM hParentItem;

		if( m_TemplateTree.GetSelectedCount() > 1 )
		{
			hSelectItem = m_TemplateTree.GetFirstSelectedItem();
		}
		hParentItem = m_TemplateTree.GetParentItem( hSelectItem );
		g_pCurSelTemplate = ( CUIToolTemplate * )m_TemplateTree.GetItemData( hParentItem );

		if( m_TemplateTree.GetSelectedCount() > 1 )
		{
			while( hSelectItem != NULL )
			{
				vecItemIndex.push_back( GetTreeItemIndex( hSelectItem ) );
				hSelectItem = m_TemplateTree.GetNextSelectedItem( hSelectItem );
			}
		}
		else
		{
			vecItemIndex.push_back( GetTreeItemIndex( hSelectItem ) );
		}

		CTemplatePropertyPaneView *pPane;
		pPane = ( CTemplatePropertyPaneView * )GetPaneWnd( TEMPLATE_PROPERTY_PANE );
		pPane->RefreshProperty( g_pCurSelTemplate, vecItemIndex );
	}

	*pResult = 0;
}

void CTemplatePaneView::OnElementDelelement()
{
	//if( !g_pCurSelTemplate )
	//{
	//	return;
	//}

	//CTypedPtrList<CPtrList, HTREEITEM> list;
	//m_TemplateTree.GetSelectedList(list);

	//int nCount = GetElementCount()-(int)list.GetCount();
	//if( nCount == 0 )
	//{
	//	AfxMessageBox("Element는 한개 이상 존재해야 합니다.");
	//	return;
	//}

	//POSITION position = list.GetHeadPosition();
	//CString strElementName;
	//HTREEITEM treeItem(NULL);

	//while( position )
	//{
	//	treeItem = list.GetAt(position);
	//	
	//	strElementName = m_TemplateTree.GetItemText(treeItem);
	//	g_pCurSelTemplate->DelUIElement(strElementName);

	//	m_TemplateTree.DeleteItem(treeItem);

	//	list.GetNext(position);
	//}

	//CEtUIToolView *pView;
	//CTemplateView *pTemplateView;

	//pView = ( CEtUIToolView * )CGlobalValue::GetInstance().m_pCurView;
	//pTemplateView = ( CTemplateView * )pView->GetTabView( 1 );
	//pTemplateView->AddTemplateControl( g_pCurSelTemplate );
}
void CTemplatePaneView::OnTemplateAddtemplates()
{
	// TODO: Add your command handler code here
	int i, nCount, nFindIndex;
	HTREEITEM hSelectItem;
	CString szItemName;

	hSelectItem = m_TemplateTree.GetSelectedItem();
	szItemName = m_TemplateTree.GetItemText( hSelectItem );
	nCount = sizeof( g_aszTreeItem ) / sizeof( char * );
	nFindIndex = -1;
	for( i = 0; i < nCount; i++ )
	{
		if( _stricmp( g_aszTreeItem[ i ], szItemName ) == 0 )
		{
			nFindIndex = i;
			break;
		}
	}
	if( nFindIndex == -1 )
	{
		return;
	}

	TCHAR szFilter[] = _T( "Text File (*.txt)|*.txt|All Files (*.*)|*.*||" );
	CFileDialog FileDlg( TRUE, _T("txt"), _T("*.txt"), OFN_HIDEREADONLY | OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_ENABLESIZING | OFN_NOCHANGEDIR, szFilter, this );

	if( FileDlg.DoModal() == IDOK )
	{
		FILE *fp = NULL;
		fopen_s( &fp, FileDlg.m_ofn.lpstrFile, "r" );
		if( fp )
		{
			char szString[_MAX_PATH]={0};
			while( fgets( szString, _MAX_PATH, fp ) )
			{
				if( szString[strlen(szString)-1] == '\n' ) szString[strlen(szString)-1] = '\0';
				CString str = szString;
				if( !CheckDuplicateTemplate( str ) )
				{
					NewTemplate( str );
				}
				else
				{
					str.Format("같은 이름을 가진 Template이 있습니다. %s", szString);
					MessageBox(str);
				}
			}
			fclose( fp );
		}
	}
}

void CTemplatePaneView::OnElementCopytemplate()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	char szFileName[ _MAX_PATH ];
	int i, nCount, nFindIndex;
	HTREEITEM hSelectItem;
	HTREEITEM hParentItem;
	CString szItemName;
	CString szTemplateName;

	hSelectItem = m_TemplateTree.GetSelectedItem();
	szTemplateName = m_TemplateTree.GetItemText( hSelectItem );
	hParentItem = m_TemplateTree.GetParentItem(hSelectItem);
	m_TemplateTree.SelectItem(hParentItem);

	hSelectItem = m_TemplateTree.GetSelectedItem();
	szItemName = m_TemplateTree.GetItemText( hSelectItem );
	nCount = sizeof( g_aszTreeItem ) / sizeof( char * );
	nFindIndex = -1;
	for( i = 0; i < nCount; i++ )
	{
		if( _stricmp( g_aszTreeItem[ i ], szItemName ) == 0 )
		{
			nFindIndex = i;
			break;
		}
	}
	if( nFindIndex == -1 )
	{
		return;
	}

	CTemplateNameDlg NameDlg;
	CString szDefaultName;

	srand( timeGetTime() );
	szDefaultName.Format( "%s%s", szTemplateName.GetBuffer(), "_copy" );
	NameDlg.m_szTemplateName = szDefaultName;

	if( NameDlg.DoModal() == IDOK )
	{
		if( ( NameDlg.m_szTemplateName.GetLength() > 0 ) && ( !CheckDuplicateTemplate( NameDlg.m_szTemplateName ) ) )
		{
			sprintf_s( szFileName, _MAX_PATH, "%s\\%s%s", CMainFrame::GetUITemplateFolder(), szTemplateName.GetBuffer(), ".uit" );

			CUIToolTemplate *pTemplate;
			CFileStream Stream( szFileName, CFileStream::OPEN_READ );

			pTemplate = new CUIToolTemplate();
			pTemplate->Load( Stream );
			pTemplate->m_Template.m_szTemplateName = NameDlg.m_szTemplateName.GetBuffer();
			pTemplate->m_bChanged = true;
			InsertTemplate( pTemplate );

			// 외부 컨트롤(스크롤바) 연결.
			if( !pTemplate->m_szExternalControlName.empty() )
			{
				for( int j = 0; j < ( int )m_vecUIToolTemplate.size(); j++ )
				{
					if( _stricmp( m_vecUIToolTemplate[ j ]->m_Template.m_szTemplateName.c_str(), pTemplate->m_szExternalControlName.c_str() ) == 0 )
					{
						pTemplate->m_pExternControlTemplate = m_vecUIToolTemplate[ j ];
					}
				}
			}
		}
		else
		{
			MessageBox( "같은 이름을 가진 Template이 있습니다" );
		}
	}
}

void CTemplatePaneView::OnElementSaveTemplateTexture()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	if( !g_pCurSelTemplate )
		return;

	std::vector< CUIToolTemplate * > vecTemplate;
	vecTemplate.push_back( g_pCurSelTemplate );
	EtTextureHandle hGenTexture = GenerateTexture( vecTemplate, true );
	if( hGenTexture ) g_pCurSelTemplate->m_Template.m_hTemplateTexture = hGenTexture;

	if( g_pCurSelTemplate->m_Template.m_hTemplateTexture )
	{
		char szFileName[ _MAX_PATH ];
		sprintf_s( szFileName, _MAX_PATH, "%s\\UITemplateTexture\\UIT_%s.dds", CMainFrame::GetResourceFolder(), g_pCurSelTemplate->m_Template.m_szTemplateName.c_str() );
		D3DXSaveTextureToFile( szFileName, D3DXIFF_DDS, g_pCurSelTemplate->m_Template.m_hTemplateTexture->GetTexturePtr(), NULL );
	}
}
void CTemplatePaneView::OnElementSaveuiUsedtemplate()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	if( !g_pCurSelTemplate )
		return;

	CLayoutView *pLayoutView;
	pLayoutView = ( CLayoutView * )( ( CEtUIToolView * )( CGlobalValue::GetInstance().m_pCurView ) )->GetTabView( CDummyView::LAYOUT_VIEW );
	pLayoutView->SaveUIWithUsedTemplate();
}
