// TemplateView.cpp : implementation file
//

#include "stdafx.h"
#include "EtUI.h"
#include "EtUITool.h"
#include "MainFrm.h"
#include "TemplateView.h"

#include "GlobalValue.h"

#include "EtUIMan.h"
#include "EtUI.h"
#include "EtUIDialog.h"
#include "EtUIControl.h"
#include "UIToolTemplate.h"
#include "GenTexture.h"

#include "PaneDefine.h"
#include "TemplatePaneView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define TEMPLATE_DLG_SIZE 0.4f

extern CUIToolTemplate *g_pCurSelTemplate;

SInitControl g_InitControl[] =
{
	{ 1000, L"Static", 0.1f, 0.1f, true },
	{ 1001, L"Button", 0.1f, 0.05f, true },
	{ 1002, L"CheckBox", 0.2f, 0.03f, true },
	{ 1003, L"RadioButton", 0.2f, 0.03f, true },
	{ 1004, L"ComboBox", 0.2f, 0.05f, true },
	{ 1005, L"Slider", 0.3f, 0.04f, true },
	{ 1006, L"EditBox Test", 0.4f, 0.04f, true },
	{ 1007, L"IME input test", 0.4f, 0.05f, true },
	{ 1008, L"ListBox", 0.2f, 0.2f, true },
	{ 1009, L"ScrollBar", 0.02f, 0.3f, true },
	{ 1010, L"ProgressBar", 0.2f, 0.03f, true },
	{ 1011, L"Custom", 0.1f, 0.05f, true },
	{ 1012, L"TextBox", 0.1f, 0.1f, true },
	{ 1013, L"TextureControl", 0.1f, 0.1f, true },
	{ 1014, L"HtmlTextBox", 0.1f, 0.1f, true },
	{ 1015, L"Tree", 0.2f, 0.2f, true },
	{ 1016, L"QuestTree", 0.2f, 0.2f, true },
	{ 1017, L"Animation", 0.2f, 0.2f, true },
	{ 1018, L"LineEditBox", 0.2f, 0.2f, true },
	{ 1019, L"LineIMEEditBox", 0.2f, 0.2f, true },
	{ 1020, L"MovieControl", 0.1f, 0.1f, true },
	{ 1021, L"ListBoxEx", 0.2f, 0.2f, true },
};

// CTemplateView

IMPLEMENT_DYNCREATE(CTemplateView, CFormView)

CTemplateView::CTemplateView()
	: CFormView(CTemplateView::IDD)
	, m_TemplateDlg( UI_TYPE_FOCUS, NULL )
{

}

CTemplateView::~CTemplateView()
{
}

void CTemplateView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CTemplateView, CFormView)
ON_WM_ERASEBKGND()
ON_MESSAGE( UM_SELECT_CHANGE_OPEN_VIEW, OnOpenView )
ON_MESSAGE( UM_SELECT_CHANGE_CLOSE_VIEW, OnCloseView )
ON_WM_KEYDOWN()
END_MESSAGE_MAP()


// CTemplateView diagnostics

#ifdef _DEBUG
void CTemplateView::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CTemplateView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// CTemplateView message handlers


BOOL CTemplateView::OnEraseBkgnd(CDC* pDC)
{
	return FALSE;
}

LRESULT CTemplateView::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if( message != UM_SELECT_CHANGE_OPEN_VIEW && message != UM_SELECT_CHANGE_CLOSE_VIEW )
	{
		if( EtInterface::StaticMsgProc( m_hWnd, message, wParam, lParam ) )
		{
			return 1;
		}
	}

	return CFormView::WindowProc(message, wParam, lParam);
}

void CTemplateView::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();

	SUIDialogInfo DlgInfo;

	memset( &DlgInfo, 0, sizeof( SUIDialogInfo ) );
	DlgInfo.DlgCoord.fX = 0.0f;
	DlgInfo.DlgCoord.fY = 0.0f;
	DlgInfo.DlgCoord.fWidth = TEMPLATE_DLG_SIZE;
	DlgInfo.DlgCoord.fHeight = TEMPLATE_DLG_SIZE;
	DlgInfo.dwDlgColor = 0x30000000;

	// 임시 by mapping
	m_TemplateDlg.Initialize( NULL, false );
	m_TemplateDlg.SetDlgInfo( DlgInfo );
}

void CTemplateView::AddTemplateControl( CUIToolTemplate *pTemplate )
{
	if( pTemplate == NULL )
		return;

	m_TemplateDlg.DeleteAllControl();
	m_TemplateDlg.DeleteAllTemplate();

	// 템플릿 선택할때마다 템플릿 텍스처 로드해서 들고있기로 한다.
	// 이렇게 하지 않고 처음 로드할때 모든 템플릿텍스처 들고있게 하면, 로딩이 너무 오래 걸리게 된다.
	pTemplate->m_Template.OnLoaded();

	CEtUIControl *pControl;
	std::vector< CUIToolTemplate * > vecToolTemplate;
	SUIControlProperty ControlProperty;
	UI_CONTROL_TYPE Type;

	Type = pTemplate->m_UIType;
	for( int i = 0; i < ( int )pTemplate->m_Template.m_vecElement.size(); i++ )
	{
		pTemplate->m_Template.m_vecElement[ i ].nFontIndex = pTemplate->m_vecFontSetIndex[i];
		SFontInfo FontInfo;
		CEtFontMng::GetInstance().GetFontInfo( pTemplate->m_vecFontSetIndex[i], 0, FontInfo );
		pTemplate->m_Template.m_vecElement[i].nFontHeight = FontInfo.nFontHeight;
	}

	if( g_InitControl[ Type ].bGenerateTexture )
	{
		vecToolTemplate.push_back( pTemplate );
		if( pTemplate->m_pExternControlTemplate )
		{
			vecToolTemplate.push_back( pTemplate->m_pExternControlTemplate );
		}

		EtTextureHandle hGenTexture = GenerateTexture( vecToolTemplate );
		if( hGenTexture ) m_TemplateDlg.SetUITexture( hGenTexture );
	}

	if( Type == UI_CONTROL_COMBOBOX )
	{
		if( pTemplate->m_pExternControlTemplate == NULL )
		{
			ControlProperty.ComboBoxProperty.nScrollBarTemplate = -1;
		}
		else
		{
			ControlProperty.ComboBoxProperty.nScrollBarTemplate = 1;
		}
	}
	else if( Type == UI_CONTROL_LISTBOX )
	{
		if( pTemplate->m_pExternControlTemplate == NULL )
		{
			ControlProperty.ListBoxProperty.nScrollBarTemplate = -1;
		}
		else
		{
			ControlProperty.ListBoxProperty.nScrollBarTemplate = 1;
		}
	}
	else if( Type == UI_CONTROL_STATIC )
	{
		ControlProperty.StaticProperty.dwFontFormat = pTemplate->m_Template.m_vecElement[ 0 ].dwFontFormat;
		ControlProperty.StaticProperty.bShadowFont = pTemplate->m_Template.m_vecElement[ 0 ].bShadowFont;
		ControlProperty.StaticProperty.dwFontColor = pTemplate->m_Template.m_vecElement[ 0 ].FontColor.dwColor[ 0 ];
		ControlProperty.StaticProperty.dwShadowFontColor = pTemplate->m_Template.m_vecElement[ 0 ].ShadowFontColor.dwColor[ 0 ];
		ControlProperty.StaticProperty.dwTextureColor = 0xFFFFFFFF;
	}
	else if( Type == UI_CONTROL_TEXTBOX )
	{
		if( pTemplate->m_pExternControlTemplate == NULL )
		{
			ControlProperty.TextBoxProperty.nScrollBarTemplate = -1;
		}
		else
		{
			ControlProperty.TextBoxProperty.nScrollBarTemplate = 1;
		}
	}
	else if( Type == UI_CONTROL_HTMLTEXTBOX )
	{
		if( pTemplate->m_pExternControlTemplate == NULL )
		{
			ControlProperty.HtmlTextBoxProperty.nScrollBarTemplate = -1;
		}
		else
		{
			ControlProperty.HtmlTextBoxProperty.nScrollBarTemplate = 1;
		}
	}
	else if( Type == UI_CONTROL_TREECONTROL )
	{
		if( pTemplate->m_pExternControlTemplate == NULL )
		{
			ControlProperty.TreeControlProperty.nScrollBarTemplate = -1;
		}
		else
		{
			ControlProperty.TreeControlProperty.nScrollBarTemplate = 1;
		}
	}
	else if( Type == UI_CONTROL_QUESTTREECONTROL )
	{
		if( pTemplate->m_pExternControlTemplate == NULL )
		{
			ControlProperty.QuestTreeControlProperty.nScrollBarTemplate = -1;
		}
		else
		{
			ControlProperty.QuestTreeControlProperty.nScrollBarTemplate = 1;
		}
	}
	else if( Type == UI_CONTROL_TEXTURECONTROL )
	{
		ControlProperty.TextureControlProperty.dwFontFormat = pTemplate->m_Template.m_vecElement[ 0 ].dwFontFormat;
		ControlProperty.TextureControlProperty.bShadowFont = pTemplate->m_Template.m_vecElement[ 0 ].bShadowFont;
		ControlProperty.TextureControlProperty.dwFontColor = pTemplate->m_Template.m_vecElement[ 0 ].FontColor.dwColor[ 0 ];
	}
	else if( Type == UI_CONTROL_LISTBOXEX )
	{
		if( pTemplate->m_pExternControlTemplate == NULL )
		{
			ControlProperty.ListBoxExProperty.nScrollBarTemplate = -1;
		}
		else
		{
			ControlProperty.ListBoxExProperty.nScrollBarTemplate = 1;
		}
	}

	m_TemplateDlg.AddTemplate( pTemplate->m_Template );
	if( pTemplate->m_pExternControlTemplate )
	{
		m_TemplateDlg.AddTemplate( pTemplate->m_pExternControlTemplate->m_Template );
	}

	ControlProperty.UIType = Type;
	ControlProperty.nTemplateIndex = 0;
	ControlProperty.nID = g_InitControl[ Type ].nID;
	float fWidth = ( pTemplate->m_fDefaultWidth > TEMPLATE_DLG_SIZE ) ? TEMPLATE_DLG_SIZE : pTemplate->m_fDefaultWidth;
	float fHeight = ( pTemplate->m_fDefaultHeight > TEMPLATE_DLG_SIZE ) ? TEMPLATE_DLG_SIZE : pTemplate->m_fDefaultHeight;
	ControlProperty.UICoord.fX = ( TEMPLATE_DLG_SIZE - fWidth ) * 0.5f;
	ControlProperty.UICoord.fY = ( TEMPLATE_DLG_SIZE - fHeight ) * 0.5f;
	ControlProperty.UICoord.fWidth = fWidth;
	ControlProperty.UICoord.fHeight = fHeight;
	g_pCurSelTemplate->m_ControlInfo.CopyControlInfo( &ControlProperty );
	pControl = m_TemplateDlg.CreateControl( &ControlProperty );
	pControl->SetText( g_InitControl[ Type ].wszText );

	SetSampleItem( pControl );
	Invalidate();
}

LRESULT CTemplateView::OnOpenView( WPARAM wParam, LPARAM lParam )
{
	CTemplatePaneView *pTemplatePane;

	pTemplatePane = ( CTemplatePaneView * )GetPaneWnd( TEMPLATE_PANE );
	if( pTemplatePane )
	{
		LRESULT Result;
		pTemplatePane->OnTvnSelchangedTemplateTree( NULL, &Result );
	}

	m_TemplateDlg.Show( true );

	if( g_pCurSelTemplate )
	{
		AddTemplateControl( g_pCurSelTemplate );
	}

	return 1;
}

LRESULT CTemplateView::OnCloseView( WPARAM wParam, LPARAM lParam )
{
	m_TemplateDlg.Show( false );
	return 1;
}

void CTemplateView::SetSampleItem( CEtUIControl *pControl )
{
	if( !pControl )
	{
		CDebugSet::ToLogFile( "CTemplateView::SetSampleItem, pControl is NULL!" );
		return;
	}

	switch( pControl->GetType() )
	{
	case UI_CONTROL_COMBOBOX:
		{
			WCHAR wszText[ 256 ]={0};
			((CEtUIComboBox*)pControl)->RemoveAllItems();

			for( int i = 0; i < 50; i++ )
			{
				swprintf_s( wszText, 256, L"%d", i );
				( ( CEtUIComboBox * )pControl )->AddItem( wszText, NULL, i );
			}
		}
		break;
	case UI_CONTROL_LISTBOX:
		{
			WCHAR wszText[ 256 ]={0};
			((CEtUIListBox*)pControl)->RemoveAllItems();

			for( int i = 0; i < 50; i++ )
			{
				swprintf_s( wszText, 256, L"List Item %d", i );
				( ( CEtUIListBox * )pControl )->AddItem( wszText, NULL, i );
			}
		}
		break;
	case UI_CONTROL_TEXTBOX:
		{
			WCHAR wszText[ 256 ]={0};
			CEtUITextBox *pTextBox = static_cast<CEtUITextBox*>(pControl);
			pTextBox->ClearText();

			for( int i = 0; i < 50; i++ )
			{
				swprintf_s( wszText, 256, L"[%d]무궁화 꽃이 피었습니다. 무궁화 꽃이 피었습니다.", i );
				pTextBox->AddText( wszText, textcolor::WHITE, UITEXT_SYMBOL );
			}
		}
		break;
	case UI_CONTROL_HTMLTEXTBOX:
		{
			WCHAR wszText[ 256 ]={0};
			swprintf_s( wszText, 256, L"<font color=\"red\">  A - 무궁화 꽃이 피었습니다.</font><br>  B - 무궁화 꽃이 피었습니다.</br>" );

			CEtUIHtmlTextBox *pTextBox = static_cast<CEtUIHtmlTextBox*>(pControl);
			pTextBox->ReadHtmlString( wszText );
			//pTextBox->ReadHtmlFile( L"E:\\DragonNest\\test.html" );
		}
		break;
	case UI_CONTROL_TREECONTROL:
		{
			CEtUITreeCtl *pTreeCtl = static_cast<CEtUITreeCtl*>(pControl);
			CTreeItem *pItem1(NULL);

			pItem1 = pTreeCtl->AddItem( CTreeItem::typeOpen, L"[그룹 1]", textcolor::FORESTGREEN );
			pTreeCtl->AddChildItem( pItem1, CTreeItem::typeOpen, L"아이덴티티 게임즈", textcolor::DARKGRAY );
			pTreeCtl->AddChildItem( pItem1, CTreeItem::typeOpen, L"옆집에 이사 온 아이", textcolor::DARKGRAY );
			pTreeCtl->AddChildItem( pItem1, CTreeItem::typeOpen, L"저주가 걸린 공주를 찾아라...", textcolor::DARKGRAY );
			
			pItem1 = pTreeCtl->AddItem( CTreeItem::typeOpen, L"[그룹 2]", textcolor::FORESTGREEN );
			pTreeCtl->AddChildItem( pItem1, CTreeItem::typeOpen, L"우리 결혼 했어요", textcolor::DARKGRAY );
			pTreeCtl->AddChildItem( pItem1, CTreeItem::typeOpen, L"산이좋아~~~", textcolor::DARKGRAY );
			
			//pTreeCtl->CollapseAll();
			//pTreeCtl->ExpandAllChildren( pItemTemp );
		}
		break;
	case UI_CONTROL_QUESTTREECONTROL:
		{
			CEtUIQuestTreeCtl *pTreeCtl = static_cast<CEtUIQuestTreeCtl*>(pControl);
			//CEtUIQuestTreeItem *pItem1, *pItem2, *pItemTemp;

			/*pItem1 = pTreeCtl->AddQuestItem( CTreeItem::typeOpen, L"Ch. 1\n악마성의 저주", textcolor::RED );
			pItem1 = pTreeCtl->AddQuestChildItem( pItem1, CTreeItem::typeOpen, L"저주가 걸린 공주를 찾아라...", textcolor::YELLOW );
			pItem1 = pTreeCtl->AddQuestChildItem( pItem1, CTreeItem::typeOpen, L"샬랄라 공주는 어디에...", textcolor::GOLD );

			pItem1 = pTreeCtl->AddQuestItem( CTreeItem::typeOpen, L"Ch. 2\n악마성의 저주", textcolor::RED );
			pItem1 = pTreeCtl->AddQuestChildItem( pItem1, CTreeItem::typeOpen, L"저주가 걸린 공주를 찾아라...", textcolor::YELLOW );
			pItem1 = pTreeCtl->AddQuestChildItem( pItem1, CTreeItem::typeOpen, L"샬랄라 공주는 어디에...", textcolor::GOLD );

			pItem1 = pTreeCtl->AddQuestItem( CTreeItem::typeOpen, L"Ch. 3\n적의 포로로 잡힌 2MB를 제거하라.", textcolor::RED );
			pItem2 = pTreeCtl->AddQuestChildItem( pItem1, CTreeItem::typeOpen, L"저주가 걸린 공주를 찾아라...", textcolor::YELLOW );
			pItem1 = pTreeCtl->AddQuestChildItem( pItem1, CTreeItem::typeOpen, L"샬랄라 공주는 어디에...", textcolor::YELLOW );
			pItemTemp = pItem2 = pTreeCtl->AddQuestChildItem( pItem2, CTreeItem::typeOpen, L"저주가 걸린 공주를 찾아라...", textcolor::GOLD );

			pItem1 = pTreeCtl->AddQuestItem( CTreeItem::typeOpen, L"Ch. 4\n무궁화 꽃이...", textcolor::RED );
			pItem1 = pTreeCtl->AddQuestChildItem( pItem1, CTreeItem::typeOpen, L"필까? 말까? 영구없다~...", textcolor::YELLOW );

			pItem1 = pTreeCtl->AddQuestItem( CTreeItem::typeOpen, L"Ch. 5\n무궁화 꽃이...", textcolor::RED );

			pTreeCtl->CollapseAll();
			pTreeCtl->ExpandAllChildren( pItemTemp );*/
		}
		break;
	/*case UI_CONTROL_LISTBOXEX:
		{
			WCHAR wszText[ 256 ]={0};
			((CEtUIListBoxEx*)pControl)->RemoveAllItems();

			((CEtUIListBox*)pControl)->AddItem( L"확장형 리스트박스는", NULL, 0 );
			((CEtUIListBox*)pControl)->AddItem( L"ui파일을 리스트아이템으로 사용합니다.", NULL, 0 );
			((CEtUIListBox*)pControl)->AddItem( L"그래서 리스트아이템을", NULL, 0 );
			((CEtUIListBox*)pControl)->AddItem( L"템플릿뷰에선 미리 볼 수 없어요.", NULL, 0 );
			((CEtUIListBox*)pControl)->AddItem( L"폰트도 안쓰기때문에,", NULL, 0 );
			((CEtUIListBox*)pControl)->AddItem( L"폰트설정도 하실 필요없어요.", NULL, 0 );
			((CEtUIListBox*)pControl)->AddItem( L"그냥 Select Item 엘리먼트의", NULL, 0 );
			((CEtUIListBox*)pControl)->AddItem( L" 텍스처만 설정해주시면,", NULL, 0 );
			((CEtUIListBox*)pControl)->AddItem( L"툴에서 할 일은 끝입니다.", NULL, 0 );
			((CEtUIListBox*)pControl)->AddItem( L"Test Data 0", NULL, 0 );
			((CEtUIListBox*)pControl)->AddItem( L"Test Data 1", NULL, 0 );
		}
		break;*/
	}
}

void CTemplateView::AlignDialog( UIAllignHoriType alignHori, UIAllignVertType alignVert )
{
	m_TemplateDlg.SetAllignType( alignHori, alignVert );
}

void CTemplateView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	CFormView::OnKeyDown(nChar, nRepCnt, nFlags);
}

