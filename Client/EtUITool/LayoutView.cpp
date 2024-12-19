// LayoutView.cpp : implementation file
//

#include "stdafx.h"
#include "EtUITool.h"
#include "MainFrm.h"
#include "EtUIToolDoc.h"
#include "EtUIToolView.h"
#include "LayoutView.h"
#include "PaneDefine.h"
#include "ControlPropertyPaneView.h"
#include "TemplateView.h"
#include "DummyView.h"

#include "Interval.h"
#include "TabOrder.h"

#include "GlobalValue.h"
#include "RenderBase.h"

#include "EtUI.h"
#include "EtUIDialog.h"
#include "EtUIControl.h"
#include "EtUITextUtil.h"

#include "UIToolTemplate.h"
#include "GenTexture.h"
#include "../../Common/EtResManager/EtResourceMng.h"

#include "CustomControlSelect.h"
#include "DialogList.h"

#include "CompressCheckDlg.h"

#ifdef FILEDLG_AUTOPATH
#include "XTCustomPropertyGridItemFile.h"
#endif


//#include <nvcore/StrLib.h>
#include <nvcore/StdStream.h>

#include <nvimage/Image.h>
#include <nvimage/DirectDrawSurface.h>

#include <nvtt/nvtt.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CLayoutView
extern CUIToolTemplate *g_pCurSelTemplate;

std::vector<std::string> g_vecFileNameforCustomSave;

char *g_szNotUseTemplateListFile = "\\NotUseTemplateList.txt";

bool LayoutControlSortFunc( SCreateControlInfo Obj1, SCreateControlInfo Obj2 )
{
	if( Obj1.nTabOrder < Obj2.nTabOrder )
	{
		return true;
	}

	return false;
}

IMPLEMENT_DYNCREATE(CLayoutView, CFormView)

 CLayoutView::CLayoutView()
	: CFormView(CLayoutView::IDD)
	, m_LayoutDlg( UI_TYPE_FOCUS, NULL )
	, m_pMoveControl( NULL )
	, m_nCurTabOrder( -1 )
	, m_bDragMode( false )
	, m_bInitStateMode( false )
	, m_bTabOrderMode( false )
	, m_nViewCrosshair( 0 )
	, m_BackgroundDlg( UI_TYPE_BOTTOM , NULL )
	, m_nLastSelectGroup( -1 )
	, m_nWideScreen( 0 )
{
}

CLayoutView::~CLayoutView()
{
}

void CLayoutView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CLayoutView, CFormView)
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_KEYDOWN()
	ON_MESSAGE( UM_SELECT_CHANGE_OPEN_VIEW, OnOpenView )
	ON_MESSAGE( UM_SELECT_CHANGE_CLOSE_VIEW, OnCloseView )
	ON_COMMAND(ID_UI_SAVE, &CLayoutView::OnFileSave)
	ON_COMMAND(ID_UI_SAVE_AS, &CLayoutView::OnFileSaveAs)
	ON_COMMAND(ID_UI_OPEN, &CLayoutView::OnFileOpen)
	ON_COMMAND(ID_UI_NEW, &CLayoutView::OnFileNew)
	ON_UPDATE_COMMAND_UI(ID_ALLIGN_BOTTOM, &CLayoutView::OnUpdateAllignBottom)
	ON_UPDATE_COMMAND_UI(ID_ALLIGN_CENTER, &CLayoutView::OnUpdateAllignCenter)
	ON_UPDATE_COMMAND_UI(ID_ALLIGN_LEFT, &CLayoutView::OnUpdateAllignLeft)
	ON_UPDATE_COMMAND_UI(ID_ALLIGN_MIDDLE, &CLayoutView::OnUpdateAllignMiddle)
	ON_UPDATE_COMMAND_UI(ID_ALLIGN_RIGHT, &CLayoutView::OnUpdateAllignRight)
	ON_UPDATE_COMMAND_UI(ID_ALLIGN_TOP, &CLayoutView::OnUpdateAllignTop)
	ON_UPDATE_COMMAND_UI(ID_MAKESAMESIZE_BOTH, &CLayoutView::OnUpdateMakesamesizeBoth)
	ON_UPDATE_COMMAND_UI(ID_MAKESAMESIZE_HEIGHT, &CLayoutView::OnUpdateMakesamesizeHeight)
	ON_UPDATE_COMMAND_UI(ID_MAKESAMESIZE_WIDTH, &CLayoutView::OnUpdateMakesamesizeWidth)
	ON_COMMAND(ID_ALLIGN_BOTTOM, &CLayoutView::OnAllignBottom)
	ON_COMMAND(ID_ALLIGN_CENTER, &CLayoutView::OnAllignCenter)
	ON_COMMAND(ID_ALLIGN_LEFT, &CLayoutView::OnAllignLeft)
	ON_COMMAND(ID_ALLIGN_MIDDLE, &CLayoutView::OnAllignMiddle)
	ON_COMMAND(ID_ALLIGN_RIGHT, &CLayoutView::OnAllignRight)
	ON_COMMAND(ID_ALLIGN_TOP, &CLayoutView::OnAllignTop)
	ON_COMMAND(ID_MAKESAMESIZE_BOTH, &CLayoutView::OnMakesamesizeBoth)
	ON_COMMAND(ID_MAKESAMESIZE_HEIGHT, &CLayoutView::OnMakesamesizeHeight)
	ON_COMMAND(ID_MAKESAMESIZE_WIDTH, &CLayoutView::OnMakesamesizeWidth)
	ON_COMMAND(ID_FORMAT_TABORDER, &CLayoutView::OnFormatTaborder)
	ON_UPDATE_COMMAND_UI(ID_FORMAT_TABORDER, &CLayoutView::OnUpdateFormatTaborder)
	ON_COMMAND(ID_EDIT_UNDO, &CLayoutView::OnEditUndo)
	ON_COMMAND(ID_EDIT_REDO, &CLayoutView::OnEditRedo)
	ON_UPDATE_COMMAND_UI(ID_EDIT_UNDO, &CLayoutView::OnUpdateEditUndo)
	ON_UPDATE_COMMAND_UI(ID_EDIT_REDO, &CLayoutView::OnUpdateEditRedo)
	ON_COMMAND(ID_EDIT_DELETE, &CLayoutView::OnEditDelete)
	ON_COMMAND(ID_EDIT_SELECTALL, &CLayoutView::OnEditSelectall)
	ON_UPDATE_COMMAND_UI(ID_EDIT_DELETE, &CLayoutView::OnUpdateEditDelete)
	ON_UPDATE_COMMAND_UI(ID_EDIT_COPY, &CLayoutView::OnUpdateEditCopy)
	ON_COMMAND(ID_EDIT_COPY, &CLayoutView::OnEditCopy)
	ON_UPDATE_COMMAND_UI(ID_EDIT_PASTE, &CLayoutView::OnUpdateEditPaste)
	ON_COMMAND(ID_EDIT_PASTE, &CLayoutView::OnEditPaste)
	ON_COMMAND(ID_CENTERINDIALOG_VERTICAL, &CLayoutView::OnCenterindialogVertical)
	ON_COMMAND(ID_CENTERINDIALOG_HORIZONTAL, &CLayoutView::OnCenterindialogHorizontal)
	ON_COMMAND(ID_EDIT_DELETEDIALOGTEXTURE, &CLayoutView::OnEditDeletedialogtexture)
	ON_COMMAND(ID_ALLIGN_HORIZONTAL, &CLayoutView::OnAllignHorizontal)
	ON_COMMAND(ID_ALLIGN_VERTICAL, &CLayoutView::OnAllignVertical)
	ON_UPDATE_COMMAND_UI(ID_ALLIGN_HORIZONTAL, &CLayoutView::OnUpdateAllignHorizontal)
	ON_UPDATE_COMMAND_UI(ID_ALLIGN_VERTICAL, &CLayoutView::OnUpdateAllignVertical)
	ON_WM_RBUTTONDOWN()
	ON_COMMAND(ID_ADD_COIN_TEMPLATE, &CLayoutView::OnAddCoinTemplate)
	ON_COMMAND(ID_VIEW_CROSSHAIR, &CLayoutView::OnViewCrosshair)
	ON_UPDATE_COMMAND_UI(ID_VIEW_CROSSHAIR, &CLayoutView::OnUpdateViewCrosshair)
	ON_COMMAND(ID_VIEW_WIDESCREEN, &CLayoutView::OnViewWidescreen)
	ON_UPDATE_COMMAND_UI(ID_VIEW_WIDESCREEN, &CLayoutView::OnUpdateViewWidescreen)
	ON_COMMAND(ID_FORMAT_MAKESAMEPROPERTY, &CLayoutView::OnFormatMakesameproperty)
	ON_UPDATE_COMMAND_UI(ID_FORMAT_MAKESAMEPROPERTY, &CLayoutView::OnUpdateFormatMakesameproperty)
	ON_COMMAND(ID_DEL_NOTUSETEMPLATE, &CLayoutView::OnDelNotusetemplate)
	ON_COMMAND(ID_FORMAT_INITSTATE, &CLayoutView::OnFormatInitstate)
	ON_UPDATE_COMMAND_UI(ID_FORMAT_INITSTATE, &CLayoutView::OnUpdateFormatInitstate)
END_MESSAGE_MAP()


// CLayoutView diagnostics

#ifdef _DEBUG
void CLayoutView::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CLayoutView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG

void CLayoutView::StartTabOrder()
{
	if( !IsCurTabOrder() )
	{
		m_nCurTabOrder = 0;
		DrawTabOrder();
	}
}

void CLayoutView::EndTabOrder()
{
	m_nCurTabOrder = -1;
	CRenderBase::GetInstance().ClearTabOrder();

	SortDialogControl();
}

bool CLayoutView::SetTabOrder( int nOrder, CEtUIControl *pControl )
{
	int nFindIndex = FindControl( pControl );

	if( nFindIndex == -1 )
	{
		return false;
	}

	// 인덱스의 최대치 제한
	if (nOrder > (int)m_vecLayoutControl.size()-1)
		nOrder = (int)m_vecLayoutControl.size()-1;

	if( nOrder < m_vecLayoutControl[ nFindIndex ].nTabOrder )
	{
		AddTabOrder( nOrder, m_vecLayoutControl[ nFindIndex ].nTabOrder, 1 );
	}
	else if( nOrder > m_vecLayoutControl[ nFindIndex ].nTabOrder )
	{
		AddTabOrder( m_vecLayoutControl[ nFindIndex ].nTabOrder, nOrder, -1 );
	}
	m_vecLayoutControl[ nFindIndex ].nTabOrder = nOrder;
	DrawTabOrder();

	return true;
}

void CLayoutView::AddTabOrder( int nStart, int nEnd, int nAdd )
{
	for( int i = 0; i < ( int )m_vecLayoutControl.size(); i++ )
	{
		if( ( m_vecLayoutControl[ i ].nTabOrder >= nStart ) && ( m_vecLayoutControl[ i ].nTabOrder <= nEnd ) )
		{
			m_vecLayoutControl[ i ].nTabOrder += nAdd;
		}
	}
}

void CLayoutView::AllignControl( ALLIGN_TYPE emAllignType, int nInterval )
{
	int i, nMainUIIndex;
	CEtUIControl *pMainControl;
	SUICoord *pUICoord, MainUICoord;

	for( i = 0; i < ( int )m_vecLayoutControl.size(); i++ )
	{
		if( m_vecLayoutControl[ i ].emSelect == SELECT_LAST )
		{
			pMainControl = m_vecLayoutControl[ i ].pControl;
			nMainUIIndex = i;
			break;
		}
	}

	MainUICoord = m_vecLayoutControl[ nMainUIIndex ].Property.UICoord;

	// 가로 간격정렬, 세로 간격정렬의 경우 추가적인 처리가 필요하다.
	//float fLastX, fLastY;
	// float으로 그냥 계산하니 interval이 1024or768로 딱 떨어지지 않는 숫자의 경우(ex:1)
	// 점점 오차가 생겨서 화살표로 움직여도 2씩 움직일때가 있다.
	int nLastX, nLastY;
	nLastX = (int)(DEFAULT_UI_SCREEN_WIDTH * (MainUICoord.fX + MainUICoord.fWidth));
	nLastY = (int)(DEFAULT_UI_SCREEN_HEIGHT * (MainUICoord.fY + MainUICoord.fHeight));

	for( i = 0; i < ( int )m_vecLayoutControl.size(); i++ )
	{
		if( m_vecLayoutControl[ i ].emSelect == SELECT_DONE )
		{
			pUICoord = &m_vecLayoutControl[ i ].Property.UICoord;
			switch( emAllignType )
			{
			case ALLIGN_LEFT:
				pUICoord->fX = MainUICoord.fX;
				break;
			case ALLIGN_CENTER:
				pUICoord->fX = MainUICoord.fX + MainUICoord.fWidth * 0.5f - pUICoord->fWidth * 0.5f;
				break;
			case ALLIGN_RIGHT:
				pUICoord->fX = ( MainUICoord.fX + MainUICoord.fWidth ) - pUICoord->fWidth;
				break;
			case ALLIGN_TOP:
				pUICoord->fY = MainUICoord.fY;
				break;
			case ALLIGN_MIDDLE:
				pUICoord->fY = MainUICoord.fY + MainUICoord.fHeight * 0.5f - pUICoord->fHeight * 0.5f;
				break;
			case ALLIGN_BOTTOM:
				pUICoord->fY = ( MainUICoord.fY + MainUICoord.fHeight ) - pUICoord->fHeight;
				break;
			case ALLIGN_HORIZONTAL:
				pUICoord->fX = (nLastX + nInterval) / (float)DEFAULT_UI_SCREEN_WIDTH;
				nLastX += ((int)(pUICoord->fWidth * DEFAULT_UI_SCREEN_WIDTH) + nInterval);
				break;
			case ALLIGN_VERTICAL:
				pUICoord->fY = (nLastY + nInterval) / (float)DEFAULT_UI_SCREEN_HEIGHT;
				nLastY += ((int)(pUICoord->fHeight * DEFAULT_UI_SCREEN_HEIGHT) + nInterval);
				break;
			}
			m_vecLayoutControl[ i ].pControl->SetUICoord( m_vecLayoutControl[ i ].Property.UICoord );
		}
	}
	DrawSelectControl();

	if (m_bTabOrderMode)
		DrawTabOrder();

	UndoCheckPoint();
}

void CLayoutView::MakeSameSizeControl( MAKESIZE_TYPE emMakeSize )
{
	int i, nMainIndex;
	CEtUIControl *pMainControl;
	SUICoord *pUICoord, MainUICoord;

	for( i = 0; i < ( int )m_vecLayoutControl.size(); i++ )
	{
		if( m_vecLayoutControl[ i ].emSelect == SELECT_LAST )
		{
			nMainIndex = i;
			pMainControl = m_vecLayoutControl[ i ].pControl;
			break;
		}
	}

	MainUICoord = m_vecLayoutControl[ nMainIndex ].Property.UICoord;
	for( i = 0; i < ( int )m_vecLayoutControl.size(); i++ )
	{
		if( m_vecLayoutControl[ i ].emSelect == SELECT_DONE )
		{
			pUICoord = &m_vecLayoutControl[ i ].Property.UICoord;
			switch( emMakeSize )
			{
			case MAKESIZE_WIDTH:
				pUICoord->fWidth = MainUICoord.fWidth;
				break;
			case MAKESIZE_HEIGHT:
				pUICoord->fHeight = MainUICoord.fHeight;
				break;
			case MAKESIZE_BOTH:
				pUICoord->fWidth = MainUICoord.fWidth;
				pUICoord->fHeight = MainUICoord.fHeight;
				break;
			}
			m_vecLayoutControl[ i ].pControl->SetUICoord( m_vecLayoutControl[ i ].Property.UICoord );
		}
	}
	DrawSelectControl();

	UndoCheckPoint();
}

void CLayoutView::GetLayoutDlgPosition( float &fX, float &fY )
{
	m_LayoutDlg.GetPosition(fX, fY);
}

void CLayoutView::SetDefaultControl( SUIControlProperty *pProperty )
{
	int nSize = GetControlCount();

	for( int i = 0; i < nSize; i++ )
	{
		if( &m_vecLayoutControl[ i ].Property == pProperty )
		{
			m_vecLayoutControl[ i ].Property.bDefaultControl = TRUE;
			m_vecLayoutControl[ i ].pControl->DefaultControl( true );
		}
		else
		{
			m_vecLayoutControl[ i ].Property.bDefaultControl = FALSE;
			m_vecLayoutControl[ i ].pControl->DefaultControl( false );
		}
	}
}

int CLayoutView::FindControl( CEtUIControl *pControl )
{
	int nSize = GetControlCount();

	for( int i = 0; i < nSize; i++ )
	{
		if( m_vecLayoutControl[ i ].pControl == pControl )
		{
			return i;
		}
	}

	return -1;
}

CEtUIControl *CLayoutView::GetControl( POINT Point )
{
	float fX, fY;

	m_LayoutDlg.PointToFloat( Point, fX, fY );

	return m_LayoutDlg.GetControlAtPoint( fX, fY );
}

CEtUIControl *CLayoutView::GetSelectedControl()
{
	for( int i=0; i<(int)m_vecLayoutControl.size(); i++ )
	{
		if( m_vecLayoutControl[i].emSelect == SELECT_LAST )
		{
			return m_vecLayoutControl[i].pControl;
		}
	}

	return NULL;
}

void CLayoutView::SelectControl( int nIndex )
{
	if( m_vecLayoutControl[ nIndex ].emSelect == SELECT_NONE )
	{
		for( int i = 0; i < ( int )m_vecLayoutControl.size(); i++ )
		{
			if( m_vecLayoutControl[ i ].emSelect == SELECT_LAST )
			{
				m_vecLayoutControl[ i ].emSelect = SELECT_DONE;
			}
		}
		m_vecLayoutControl[ nIndex ].emSelect = SELECT_LAST;
	}
	else
	{
		// MFC와 달리 기존에 선택되어있던 컨트롤을 다시 누르면 선택이 풀린다.
		// 달라서 별로일까 했는데, 직접 해보니 해제되는게 더 편한거 같다.(해제 없으면 다시 처음부터 선택해야한다.)
		int nPrev = m_vecLayoutControl[ nIndex ].emSelect;
		m_vecLayoutControl[ nIndex ].emSelect = SELECT_NONE;

		if( nPrev == SELECT_LAST )
		{
			for( int i = 0; i < ( int )m_vecLayoutControl.size(); i++ )
			{
				if( m_vecLayoutControl[ i ].emSelect == SELECT_DONE )
				{
					m_vecLayoutControl[ i ].emSelect = SELECT_LAST;
					return;
				}
			}
		}
	}
}

CEtUIControl *CLayoutView::SelectControl( POINT Point, bool bMultiSelect )
{
	CEtUIControl *pControl;
	CControlPropertyPaneView *pView;
	float fX, fY;
	int nControlIndex;

	m_LayoutDlg.PointToFloat( Point, fX, fY );
	pControl = m_LayoutDlg.GetControlAtPoint( fX, fY );
	nControlIndex = FindControl( pControl );
	if( nControlIndex != -1 )
	{
		if( !bMultiSelect )
		{
			SelectAllControl( false );
		}

		SelectControl( nControlIndex );
		DrawSelectControl();

		pView = ( CControlPropertyPaneView * )GetPaneWnd( CONTROL_PROPERTY_PANE );
		if( GetLastSelectedControlIndex() != -1 )
			pView->RefreshControlProperty( &m_vecLayoutControl[ GetLastSelectedControlIndex() ].Property );
		else
			pView->RefreshControlProperty( NULL );

		return pControl;
	}

	SelectAllControl( false );
	DrawSelectControl();

	pView = ( CControlPropertyPaneView * )GetPaneWnd( CONTROL_PROPERTY_PANE );
	pView->RefreshDialogProperty();

	return NULL;
}

void CLayoutView::SelectAllControl( bool bSelect )
{
	int nSize = GetControlCount();

	if( bSelect )
	{
		for( int i = 1; i < nSize; i++ )
		{
			m_vecLayoutControl[ i ].emSelect = SELECT_DONE;
		}

		m_vecLayoutControl[0].emSelect = SELECT_LAST;
	}
	else
	{
		for( int i = 0; i < nSize; i++ )
		{
			m_vecLayoutControl[ i ].emSelect = SELECT_NONE;
		}
	}
}

void CLayoutView::SelectControlRect()
{
	float fLeft, fTop, fRight, fBottom;
	POINT point;
	SUICoord sDragRectCoord;

	point.x = m_CurDragRect.left;
	point.y = m_CurDragRect.top;
	m_LayoutDlg.PointToFloat( point, fLeft, fTop );

	point.x = m_CurDragRect.right;
	point.y = m_CurDragRect.bottom;
	m_LayoutDlg.PointToFloat( point, fRight, fBottom );

	sDragRectCoord.SetRect(fLeft, fTop, fRight, fBottom);

	int nSize = GetControlCount();
	int nFirstIndex(-1);
	for( int i = 0; i < nSize; i++ )
	{
		if( m_vecLayoutControl[i].Property.UICoord.IntersectRect(sDragRectCoord) )
		{
			m_vecLayoutControl[i].emSelect = SELECT_DONE;
			
			if( nFirstIndex == -1 )
			{
				nFirstIndex = i;
			}
		}
	}

	if( nFirstIndex != -1 )
	{
		m_vecLayoutControl[nFirstIndex].emSelect = SELECT_LAST;
	}

	DrawSelectControl();
}

bool CLayoutView::IsSelectedControl( CEtUIControl *pControl )
{
	int nFindIndex;

	nFindIndex = FindControl( pControl );
	if( nFindIndex != -1 )
	{
		if( m_vecLayoutControl[ nFindIndex ].emSelect )
		{
			return true;
		}
	}

	return false;
}

int CLayoutView::GetSelectedControlCount()
{
	int nCount(0);
	int nSize = GetControlCount();

	for( int i = 0; i < nSize; i++ )
	{
		if( m_vecLayoutControl[ i ].emSelect )
		{
			nCount++;
		}
	}

	return nCount;
}

int CLayoutView::GetLastSelectedControlIndex()
{
	int nSize = GetControlCount();

	for( int i = 0; i < nSize; i++ )
	{
		if( m_vecLayoutControl[ i ].emSelect == SELECT_LAST )
		{
			return i;
		}
	}

	return -1;
}

void CLayoutView::MoveSelectedControl( POINT &Point )
{
	SUICoord *pUICoord;
	CEtUIControl *pControl;

	int nSize = GetControlCount();
	for( int i = 0; i < nSize; i++ )
	{
		if( m_vecLayoutControl[ i ].emSelect )
		{
			pUICoord = &m_vecLayoutControl[ i ].Property.UICoord;
			pUICoord->fX += Point.x / ( float )DEFAULT_UI_SCREEN_WIDTH;
			pUICoord->fY += Point.y / ( float )DEFAULT_UI_SCREEN_HEIGHT;
			pControl = m_vecLayoutControl[ i ].pControl;
			pControl->SetUICoord( m_vecLayoutControl[ i ].Property.UICoord );

			if( m_vecLayoutControl[i].emSelect == SELECT_LAST )
			{
				CControlPropertyPaneView *pView = ( CControlPropertyPaneView * )GetPaneWnd( CONTROL_PROPERTY_PANE );
				if( pView) pView->RefreshControlProperty( &m_vecLayoutControl[ i ].Property );
			}
		}
	}

	DrawSelectControl();

	UndoCheckPoint();
}

void CLayoutView::ChangeValueSelectedControl( CUnionValueProperty *pVariable, DWORD dwIndex )
{
	int nSize = GetControlCount();
	for( int i = 0; i < nSize; i++ )
	{
		if( m_vecLayoutControl[ i ].emSelect == SELECT_DONE )
		{
			CControlPropertyPaneView *pView = ( CControlPropertyPaneView * )GetPaneWnd( CONTROL_PROPERTY_PANE );
			if( pView ) pView->OnChangeControlValue( pVariable, dwIndex, false, &m_vecLayoutControl[ i ].Property );
		}
	}

	DrawSelectControl();

	UndoCheckPoint();
}

void CLayoutView::ResizeSelectedControl( SUICoord &ControlCoord )
{
	CEtUIControl *pControl;
	SUICoord *pUICoord;

	int nSize = GetControlCount();
	for( int i = 0; i < nSize; i++ )
	{
		if( m_vecLayoutControl[ i ].emSelect )
		{
			pUICoord = &m_vecLayoutControl[ i ].Property.UICoord;
			(*pUICoord) = ControlCoord;
			
			float fX, fY;
			m_LayoutDlg.GetPosition( fX, fY );
			pUICoord->fX -= fX;
			pUICoord->fY -= fY;

			pControl = m_vecLayoutControl[ i ].pControl;
			pControl->SetUICoord( m_vecLayoutControl[ i ].Property.UICoord );

			if( m_vecLayoutControl[i].emSelect == SELECT_LAST )
			{
				CControlPropertyPaneView *pView = ( CControlPropertyPaneView * )GetPaneWnd( CONTROL_PROPERTY_PANE );
				if( pView) pView->RefreshControlProperty( &m_vecLayoutControl[ i ].Property );
			}
		}
	}

	DrawSelectControl();

	UndoCheckPoint();
}

void CLayoutView::ResizeDialog( SUICoord &ControlCoord )
{
	m_DlgInfo.DlgCoord.fWidth = ControlCoord.fWidth;
	m_DlgInfo.DlgCoord.fHeight = ControlCoord.fHeight;

	RefreshLayoutDlg();

	CControlPropertyPaneView *pView;
	pView = ( CControlPropertyPaneView * )GetPaneWnd( CONTROL_PROPERTY_PANE );
	pView->RefreshDialogProperty();
}

void CLayoutView::DeleteSelectedControl()
{
	int nSize = GetControlCount();

	for( int i = 0; i < nSize; i++ )
	{
		if( m_vecLayoutControl[ i ].emSelect )
		{
			m_LayoutDlg.DeleteControl( m_vecLayoutControl[ i ].pControl );
		}
	}

	std::vector< SCreateControlInfo >::iterator it;
	it = m_vecLayoutControl.begin();
	while( it != m_vecLayoutControl.end() )
	{
		if( ( *it ).emSelect )
		{
			it = m_vecLayoutControl.erase( it );
		}
		else
		{
			it++;
		}
	}

	std::sort( m_vecLayoutControl.begin(), m_vecLayoutControl.end(), LayoutControlSortFunc );
	nSize = GetControlCount();
	for( int i = 0; i < nSize; i++ )
	{
		m_vecLayoutControl[ i ].nTabOrder = i;
	}

	ReinitAllControl();
}

void CLayoutView::AddLayoutControl( SUICoord &ControlCoord, bool bUseTemplateCoord )
{
	if( !g_pCurSelTemplate )
	{
		return;
	}

	float fX, fY;
	SCreateControlInfo CreateInfo;
	SUICoord CreateCoord;
	bool bAdd;

	CreateCoord = ControlCoord;
	if( bUseTemplateCoord )
	{
		CreateCoord.fWidth = g_pCurSelTemplate->m_fDefaultWidth;
		CreateCoord.fHeight = g_pCurSelTemplate->m_fDefaultHeight;
	}
	SelectAllControl( false );

	for( int i = 0; i < ( int )g_pCurSelTemplate->m_Template.m_vecElement.size(); i++ )
	{
		g_pCurSelTemplate->m_Template.m_vecElement[i].nFontIndex = g_pCurSelTemplate->m_vecFontSetIndex[i];
		SFontInfo FontInfo;
		CEtFontMng::GetInstance().GetFontInfo( g_pCurSelTemplate->m_vecFontSetIndex[i], 0, FontInfo );
		g_pCurSelTemplate->m_Template.m_vecElement[i].nFontHeight = FontInfo.nFontHeight;
	}

	CreateInfo.Property.nTemplateIndex = AddLayoutTemplate( g_pCurSelTemplate, bAdd );

	if( bAdd )
	{
		m_LayoutDlg.AddTemplate( g_pCurSelTemplate->m_Template );
	}

	switch( g_pCurSelTemplate->m_UIType )
	{
		case UI_CONTROL_STATIC:
			{
				CreateInfo.Property.StaticProperty.dwFontFormat = g_pCurSelTemplate->m_Template.m_vecElement[ 0 ].dwFontFormat;
				CreateInfo.Property.StaticProperty.bShadowFont = g_pCurSelTemplate->m_Template.m_vecElement[ 0 ].bShadowFont;
				CreateInfo.Property.StaticProperty.dwFontColor = g_pCurSelTemplate->m_Template.m_vecElement[ 0 ].FontColor.dwColor[ 0 ];
				CreateInfo.Property.StaticProperty.dwShadowFontColor = g_pCurSelTemplate->m_Template.m_vecElement[ 0 ].ShadowFontColor.dwColor[ 0 ];
				CreateInfo.Property.StaticProperty.dwTextureColor = 0xFFFFFFFF;
			}
			break;
			// 아래 쭉 AddLayoutTemplate 있는 것들은,
			// 콤보, 리스트, 텍스트박스 등처럼 스크롤바 템플릿을 별도로 추가해줘야하는 컨트롤들에 대해 처리하는 것이다.
		case UI_CONTROL_COMBOBOX:
			if( g_pCurSelTemplate->m_pExternControlTemplate )
			{
				CreateInfo.Property.ComboBoxProperty.nScrollBarTemplate = AddLayoutTemplate( g_pCurSelTemplate->m_pExternControlTemplate, bAdd );
				if( bAdd )
				{
					m_LayoutDlg.AddTemplate( g_pCurSelTemplate->m_pExternControlTemplate->m_Template );
				}
			}
			else
			{
				CreateInfo.Property.ComboBoxProperty.nScrollBarTemplate = -1;
			}
			break;
		case UI_CONTROL_LISTBOX:
			if( g_pCurSelTemplate->m_pExternControlTemplate )
			{
				CreateInfo.Property.ListBoxProperty.nScrollBarTemplate = AddLayoutTemplate( g_pCurSelTemplate->m_pExternControlTemplate, bAdd );
				if( bAdd )
				{
					m_LayoutDlg.AddTemplate( g_pCurSelTemplate->m_pExternControlTemplate->m_Template );
				}
			}
			else
			{
				CreateInfo.Property.ListBoxProperty.nScrollBarTemplate = -1;
			}
			break;
		case UI_CONTROL_CUSTOM:
			{
				// 컨트롤 추가시 기본값 흰색.
				CreateInfo.Property.CustomProperty.dwColor = 0xFFFFFFFF;
			}
			break;
		case UI_CONTROL_TEXTBOX:
			if( g_pCurSelTemplate->m_pExternControlTemplate )
			{
				CreateInfo.Property.TextBoxProperty.nScrollBarTemplate = AddLayoutTemplate( g_pCurSelTemplate->m_pExternControlTemplate, bAdd );
				if( bAdd )
				{
					m_LayoutDlg.AddTemplate( g_pCurSelTemplate->m_pExternControlTemplate->m_Template );
				}
			}
			else
			{
				CreateInfo.Property.TextBoxProperty.nScrollBarTemplate = -1;
			}
			break;
		case UI_CONTROL_HTMLTEXTBOX:
			if( g_pCurSelTemplate->m_pExternControlTemplate )
			{
				CreateInfo.Property.HtmlTextBoxProperty.nScrollBarTemplate = AddLayoutTemplate( g_pCurSelTemplate->m_pExternControlTemplate, bAdd );
				if( bAdd )
				{
					m_LayoutDlg.AddTemplate( g_pCurSelTemplate->m_pExternControlTemplate->m_Template );
				}
			}
			else
			{
				CreateInfo.Property.HtmlTextBoxProperty.nScrollBarTemplate = -1;
			}
			break;
		case UI_CONTROL_TREECONTROL:
			if( g_pCurSelTemplate->m_pExternControlTemplate )
			{
				CreateInfo.Property.TreeControlProperty.nScrollBarTemplate = AddLayoutTemplate( g_pCurSelTemplate->m_pExternControlTemplate, bAdd );
				if( bAdd )
				{
					m_LayoutDlg.AddTemplate( g_pCurSelTemplate->m_pExternControlTemplate->m_Template );
				}
			}
			else
			{
				CreateInfo.Property.TreeControlProperty.nScrollBarTemplate = -1;
			}
			break;
		case UI_CONTROL_QUESTTREECONTROL:
			if( g_pCurSelTemplate->m_pExternControlTemplate )
			{
				CreateInfo.Property.QuestTreeControlProperty.nScrollBarTemplate = AddLayoutTemplate( g_pCurSelTemplate->m_pExternControlTemplate, bAdd );
				if( bAdd )
				{
					m_LayoutDlg.AddTemplate( g_pCurSelTemplate->m_pExternControlTemplate->m_Template );
				}
			}
			else
			{
				CreateInfo.Property.QuestTreeControlProperty.nScrollBarTemplate = -1;
			}
			break;
		case UI_CONTROL_TEXTURECONTROL:
			{
				CreateInfo.Property.TextureControlProperty.dwFontFormat = g_pCurSelTemplate->m_Template.m_vecElement[ 0 ].dwFontFormat;
				CreateInfo.Property.TextureControlProperty.bShadowFont = g_pCurSelTemplate->m_Template.m_vecElement[ 0 ].bShadowFont;
				CreateInfo.Property.TextureControlProperty.dwFontColor = g_pCurSelTemplate->m_Template.m_vecElement[ 0 ].FontColor.dwColor[ 0 ];
			}
			break;
		case UI_CONTROL_LISTBOXEX:
			if( g_pCurSelTemplate->m_pExternControlTemplate )
			{
				CreateInfo.Property.ListBoxExProperty.nScrollBarTemplate = AddLayoutTemplate( g_pCurSelTemplate->m_pExternControlTemplate, bAdd );
				if( bAdd )
				{
					m_LayoutDlg.AddTemplate( g_pCurSelTemplate->m_pExternControlTemplate->m_Template );
				}
			}
			else
			{
				CreateInfo.Property.ListBoxExProperty.nScrollBarTemplate = -1;
			}
			break;
	}

	RegenerateTexture();

	m_LayoutDlg.GetPosition( fX, fY );
	CreateCoord.fX -= fX;
	CreateCoord.fY -= fY;

	GenerateControlID( g_pCurSelTemplate->m_UIType, CreateInfo.Property.szUIName );
	CreateInfo.Property.nID = 0;
	CreateInfo.Property.UIType = g_pCurSelTemplate->m_UIType;
	CreateInfo.Property.UICoord = CreateCoord;
	g_pCurSelTemplate->m_ControlInfo.CopyControlInfo( &CreateInfo.Property );
	CreateInfo.pControl = m_LayoutDlg.CreateControl( &CreateInfo.Property );

	CreateInfo.emSelect = SELECT_LAST;
	CreateInfo.nTabOrder = ( int )m_vecLayoutControl.size();
	CreateInfo.Property.nTooltipStringIndex = 0;
	m_vecLayoutControl.push_back( CreateInfo );

	for( int i = 0; i < ( int )m_vecLayoutTemplate.size(); i++ )
	{
		m_LayoutDlg.SetTemplate( i, m_vecLayoutTemplate[ i ]->m_Template );
	}

	ReinitAllControl();

	CControlPropertyPaneView *pView;
	pView = ( CControlPropertyPaneView * )GetPaneWnd( CONTROL_PROPERTY_PANE );
	pView->RefreshControlProperty( &m_vecLayoutControl[ m_vecLayoutControl.size() - 1 ].Property );
}

void CLayoutView::GenerateControlID( UI_CONTROL_TYPE Type, char *pszControlID )
{
	char szControlID[32]={0};
	int nCurID;

	// 먼저 복사원본의 이름이 숫자로 끝나는지 확인한다.
	// 스트링ID다음에 바로 숫자가 붙어있기때문에 %s%d로는 떼어낼 수 없다.
	// 숫자로 끝날 경우 뒤에다가 증가시켜서 리턴한다.
	// 숫자로 끝나지 않을 경우 이전대로 처리한다.
	char c;
	bool bLastWordIsNum = false;
	int nNum = 0;
	int cipher = 1;

	// '\0'은 빼고 시작.
	int nLength = (int)strlen(pszControlID)-1;
	while ( 1 )
	{
		c = pszControlID[nLength];
		if (isdigit(c))
		{
			bLastWordIsNum = true;
			nNum += (atoi(&c) * cipher);
			cipher *= 10;
			--nLength;
		}
		else
			break;
	}

	if (bLastWordIsNum)
	{
		// 숫자로 끝나는 ID의 경우 기본이름을 얻어와서 숫자만 증가 형태로 처리한다.
		strncpy_s( szControlID, 32, pszControlID, nLength+1 );
		nCurID = nNum;
	}
	else
	{
		// 아니면 예전대로 처리.
		switch( Type )
		{
		case UI_CONTROL_STATIC:				strcpy_s( szControlID, 32, "ID_STATIC" );			break;
		case UI_CONTROL_BUTTON:				strcpy_s( szControlID, 32, "ID_BUTTON" );			break;
		case UI_CONTROL_CHECKBOX:			strcpy_s( szControlID, 32, "ID_CHECKBOX" );			break;
		case UI_CONTROL_RADIOBUTTON:		strcpy_s( szControlID, 32, "ID_RADIOBUTTON" );		break;
		case UI_CONTROL_COMBOBOX:			strcpy_s( szControlID, 32, "ID_COMBOBOX" );			break;
		case UI_CONTROL_SLIDER:				strcpy_s( szControlID, 32, "ID_SLIDER" );			break;
		case UI_CONTROL_EDITBOX:			strcpy_s( szControlID, 32, "ID_EDITBOX" );			break;
		case UI_CONTROL_IMEEDITBOX:			strcpy_s( szControlID, 32, "ID_IMEEDITBOX" );		break;
		case UI_CONTROL_LISTBOX:			strcpy_s( szControlID, 32, "ID_LISTBOX" );			break;
		case UI_CONTROL_SCROLLBAR:			strcpy_s( szControlID, 32, "ID_SCROLLBAR" );		break;
		case UI_CONTROL_PROGRESSBAR:		strcpy_s( szControlID, 32, "ID_PROGRESSBAR" );		break;
		case UI_CONTROL_CUSTOM:				strcpy_s( szControlID, 32, "ID_CUSTOM" );			break;
		case UI_CONTROL_TEXTBOX:			strcpy_s( szControlID, 32, "ID_TEXTBOX" );			break;
		case UI_CONTROL_HTMLTEXTBOX:		strcpy_s( szControlID, 32, "ID_HTMLTEXTBOX" );		break;
		case UI_CONTROL_TEXTURECONTROL:		strcpy_s( szControlID, 32, "ID_TEXTUREL" );			break;
		case UI_CONTROL_TREECONTROL:		strcpy_s( szControlID, 32, "ID_TREE" );				break;
		case UI_CONTROL_QUESTTREECONTROL:	strcpy_s( szControlID, 32, "ID_QUESTTREE" );		break;
		case UI_CONTROL_ANIMATION:			strcpy_s( szControlID, 32, "ID_ANIMATION" );		break;
		case UI_CONTROL_LINE_EDITBOX:		strcpy_s( szControlID, 32, "ID_LINEEDITBOX" );		break;
		case UI_CONTROL_LINE_IMEEDITBOX:	strcpy_s( szControlID, 32, "ID_LINEIMEEDITBOX" );	break;
		case UI_CONTROL_MOVIECONTROL:		strcpy_s( szControlID, 32, "ID_MOVIE" );			break;
		case UI_CONTROL_LISTBOXEX:			strcpy_s( szControlID, 32, "ID_LISTBOXEX" );		break;
		default:
			ASSERT(0&&"CLayoutView::GenerateControlID");
			break;
		}
		nCurID = 0;
	}

	while( 1 )
	{
		sprintf_s( pszControlID, 32, "%s%d", szControlID, nCurID );
		if( !IsExistControlID( pszControlID ) )
		{
			break;
		}
		nCurID++;
	}
}

bool CLayoutView::IsExistControlID( const char *pszControlID )
{
	for( int i = 0; i < ( int )m_vecLayoutControl.size(); i++ )
	{
		if( _stricmp( m_vecLayoutControl[ i ].Property.szUIName, pszControlID ) == 0 )
		{
			return true;
		}
	}

	return false;
}

const char *CLayoutView::GetTemplateName( SUIControlProperty *pProperty )
{
	for( int i = 0; i < ( int )m_vecLayoutControl.size(); i++ )
	{
		if( &m_vecLayoutControl[ i ].Property == pProperty )
		{
			int nTemplateIndex = m_vecLayoutControl[ i ].pControl->GetTemplateIndex();
			return m_vecLayoutTemplate[ nTemplateIndex ]->GetTemplateName();
		}
	}

	CDebugSet::ToLogFile( "CLayoutView::GetTemplateName, SUIControlProperty not found!" );
	return NULL;
}

void CLayoutView::DrawSelectControl()
{
	int i;
	DWORD dwColor;

	CRenderBase::GetInstance().ClearSelectBox();
	for( i = 0; i < ( int )m_vecLayoutControl.size(); i++ )
	{
		switch( m_vecLayoutControl[ i ].emSelect )
		{
		case SELECT_DONE:
			dwColor = 0xffffff00;
			break;
		case SELECT_LAST:
			dwColor = 0xffff0000;
			break;
		default:
			dwColor = 0;
			break;
		}
		if( dwColor )
		{
			RECT Rect;
			SUICoord ControlCoord;

			ControlCoord = m_vecLayoutControl[ i ].Property.UICoord;
			ControlCoord.fX += m_LayoutDlg.GetXCoord();
			ControlCoord.fY += m_LayoutDlg.GetYCoord();
			SetRect( &Rect, ( int )( ControlCoord.fX * DEFAULT_UI_SCREEN_WIDTH ), 
							( int )( ControlCoord.fY * DEFAULT_UI_SCREEN_HEIGHT ), 
							( int )( ( ControlCoord.fX + ControlCoord.fWidth ) * DEFAULT_UI_SCREEN_WIDTH ), 
							( int )( ( ControlCoord.fY + ControlCoord.fHeight ) * DEFAULT_UI_SCREEN_HEIGHT ) );
			CRenderBase::GetInstance().AddSelectBox( Rect, dwColor );
		}
	}

	// DrawSelectControl이 호출되는 타임에 맞춰서 갱신하면 제대로 나올거다.
	DrawInitState();
}

void CLayoutView::DrawTabOrder()
{
	CRenderBase::GetInstance().ClearTabOrder();

	for( int i = 0; i < ( int )m_vecLayoutControl.size(); i++ )
	{
		SUICoord ControlCoord;

		ControlCoord = m_vecLayoutControl[ i ].Property.UICoord;
		ControlCoord.fX += m_LayoutDlg.GetXCoord();
		ControlCoord.fX /= m_LayoutDlg.GetScreenWidthRatio();
		ControlCoord.fY += m_LayoutDlg.GetYCoord();
		CRenderBase::GetInstance().AddTabOrder( m_vecLayoutControl[ i ].nTabOrder + 1, ControlCoord.fX, ControlCoord.fY, 0xff000000 );
	}
}

void CLayoutView::DrawInitState()
{
	if( m_bInitStateMode )
	{
		CRenderBase::GetInstance().ClearInitState();

		for( int i = 0; i < ( int )m_vecLayoutControl.size(); i++ )
		{
			if( m_vecLayoutControl[ i ].Property.nInitState == 0 ) continue;

			RECT Rect;
			SUICoord ControlCoord;

			ControlCoord = m_vecLayoutControl[ i ].Property.UICoord;
			ControlCoord.fX += m_LayoutDlg.GetXCoord();
			ControlCoord.fY += m_LayoutDlg.GetYCoord();
			SetRect( &Rect, ( int )( ControlCoord.fX * DEFAULT_UI_SCREEN_WIDTH ), 
				( int )( ControlCoord.fY * DEFAULT_UI_SCREEN_HEIGHT ), 
				( int )( ( ControlCoord.fX + ControlCoord.fWidth ) * DEFAULT_UI_SCREEN_WIDTH ), 
				( int )( ( ControlCoord.fY + ControlCoord.fHeight ) * DEFAULT_UI_SCREEN_HEIGHT ) );
			ControlCoord.fX /= m_LayoutDlg.GetScreenWidthRatio();
			CRenderBase::GetInstance().AddInitState( m_vecLayoutControl[ i ].Property.nInitState, Rect, ControlCoord.fX, ControlCoord.fY );
		}
	}
	else
	{
		CRenderBase::GetInstance().ClearInitState();
	}
}

void CLayoutView::ReinitAllControl()
{
	int nSize = GetControlCount();
	for( int i = 0; i < nSize; i++ )
	{
		CUIToolTemplate *pToolTemplate = NULL;
		if( m_vecLayoutControl[ i ].Property.nTemplateIndex < (int)m_vecLayoutTemplate.size() )
			pToolTemplate = m_vecLayoutTemplate[ m_vecLayoutControl[ i ].Property.nTemplateIndex ];

		if( pToolTemplate )
		{
			pToolTemplate->m_ControlInfo.CopyControlInfo( &m_vecLayoutControl[ i ].Property );
			m_vecLayoutControl[ i ].pControl->Initialize( &m_vecLayoutControl[ i ].Property );

			// InitState때문에 비활성화 되어있을 수도 있으니 풀어준다.
			// (RenderBase에서 InitState확인 모드 가서 테두리랑 설명이랑 따로 찍어준다.)
			m_vecLayoutControl[ i ].pControl->Show( true );
			m_vecLayoutControl[ i ].pControl->Enable( true );
		}
	}

	DrawSelectControl();

	UndoCheckPoint();

	m_pMoveControl = NULL;
}

void CLayoutView::ReinitAllTemplate()
{
	for( int i = 0; i < ( int )m_vecLayoutTemplate.size(); i++ )
	{
		for( int j = 0; j < ( int )m_vecLayoutTemplate[ i ]->m_Template.m_vecElement.size(); j++ )
		{
			m_vecLayoutTemplate[i]->m_Template.m_vecElement[j].nFontIndex = m_vecLayoutTemplate[i]->m_vecFontSetIndex[j];
			SFontInfo FontInfo;
			CEtFontMng::GetInstance().GetFontInfo( m_vecLayoutTemplate[i]->m_vecFontSetIndex[j], 0, FontInfo );
			m_vecLayoutTemplate[i]->m_Template.m_vecElement[j].nFontHeight = FontInfo.nFontHeight;
		}
	}
	
	for( int i = 0; i < ( int )m_vecLayoutTemplate.size(); i++ )
	{
		m_LayoutDlg.SetTemplate( i, m_vecLayoutTemplate[ i ]->m_Template );
	}
}

void CLayoutView::RegenerateTexture()
{
	EtTextureHandle hGenTexture = GenerateTexture( m_vecLayoutTemplate );
	if( hGenTexture ) m_LayoutDlg.SetUITexture( hGenTexture );
	ReinitAllTemplate();
}

void CLayoutView::ReconstructLayout()
{
	std::vector< bool > vecUsingTemplate;
	std::vector< int > vecMatichingTable;

	vecUsingTemplate.resize( m_vecLayoutTemplate.size() );
	vecMatichingTable.resize( m_vecLayoutTemplate.size() );
	for( int i = 0; i < ( int )m_vecLayoutTemplate.size(); i++ )
	{
		vecUsingTemplate[ i ] = false;
		vecMatichingTable [ i ] = -1;
	}

	int nTemplateIndex(-1);

	for( int i = 0; i < ( int )m_vecLayoutControl.size(); i++ )
	{
		nTemplateIndex = m_vecLayoutControl[ i ].pControl->GetTemplateIndex();
		vecUsingTemplate[ nTemplateIndex ] = true;

		nTemplateIndex = m_vecLayoutControl[ i ].pControl->GetScrollBarTemplate();
		if( nTemplateIndex != -1 )
		{
			vecUsingTemplate[ nTemplateIndex ] = true;
		}
	}

	int nCurCount = 0;
	for( int i = 0; i < ( int )m_vecLayoutTemplate.size(); i++ )
	{
		if( vecUsingTemplate[ i ] )
		{
			vecMatichingTable[ i ] = nCurCount;
			nCurCount++;
		}
	}

	for( int i = ( int )m_vecLayoutTemplate.size() - 1; i >= 0; i-- )
	{
		if( !vecUsingTemplate[ i ] )
		{
			m_vecLayoutTemplate.erase( m_vecLayoutTemplate.begin() + i );
			m_LayoutDlg.DeleteTemplate( i );
		}
	}

	for( int i = 0; i < ( int )m_vecLayoutControl.size(); i++ )
	{
		nTemplateIndex = m_vecLayoutControl[i].pControl->GetTemplateIndex();
		m_vecLayoutControl[i].pControl->SetTemplateIndex( vecMatichingTable[ nTemplateIndex ] );
		
		nTemplateIndex = m_vecLayoutControl[ i ].pControl->GetScrollBarTemplate();
		if( nTemplateIndex != -1 )
		{
			m_vecLayoutControl[ i ].pControl->SetScrollBarTemplate( vecMatichingTable[ nTemplateIndex ] );
		}
	}
}

void CLayoutView::RefreshLayoutDlg()
{
	m_LayoutDlg.SetDlgInfo( m_DlgInfo );
	m_LayoutDlg.ReloadDlgTexture();

	if( m_nViewCrosshair )
	{
		// 토글이라 두번 해야한다.
		OnViewCrosshair();
		OnViewCrosshair();
	}

	UndoCheckPoint();
}

int CLayoutView::AddLayoutTemplate( CUIToolTemplate *pTemplate, bool &bAdd )
{
	for( int i = 0; i < ( int )m_vecLayoutTemplate.size(); i++ )
	{
		// 이렇게 주소로 검사하는 것과 Undo가 만나면서 제대로 검사가 안이뤄질 수도 있지 않나.. 하는 의문이 든다.
		// 여기때문에 가끔 템플릿추가할때 깨지는 현상이 일어나는지 확신할 수는 없지만, 우선 처리해두고 넘어가도록 하겠다.
		// 근데 이거 맞으려나. 잘하면 Undo처리를 뜯어야할듯..
		//if( m_vecLayoutTemplate[ i ] == pTemplate )
		if( m_vecLayoutTemplate[ i ]->m_Template.m_szTemplateName == pTemplate->m_Template.m_szTemplateName )
		{
			bAdd = false;
			return i;
		}
	}

	m_vecLayoutTemplate.push_back( pTemplate );
	bAdd = true;

	return ( int )m_vecLayoutTemplate.size() - 1;
}

void CLayoutView::NewLayoutDlg()
{
	m_LayoutDlg.DeleteAllControl();
	m_LayoutDlg.DeleteAllTemplate();
	m_LayoutDlg.FreeDialogTexture();
	g_pCurSelTemplate = NULL;

	m_vecLayoutTemplate.clear();
	m_vecLayoutControl.clear();

	m_szCurFileName = "";

	SecureZeroMemory( &m_DlgInfo, sizeof( SUIDialogInfo ) );
	m_DlgInfo.AllignHori = AT_HORI_CENTER;
	m_DlgInfo.AllignVert = AT_VERT_CENTER;
	m_DlgInfo.DlgCoord.fWidth = 0.625f;
	m_DlgInfo.DlgCoord.fHeight = 0.625f;
	m_DlgInfo.DlgCoord.fX = 0.0f;
	m_DlgInfo.DlgCoord.fY = 0.0f;
	m_DlgInfo.dwDlgColor = 0xffffffff;

	if ( m_bTabOrderMode )
	{
		EndTabOrder();
		m_bTabOrderMode = false;
	}

	RefreshLayoutDlg();
}

void CLayoutView::AddBackgroundDialog( const char *pFileName )
{
	CEtUIDialog *pDialog = new CEtUIDialog( UI_TYPE_BOTTOM );
	pDialog->Initialize( pFileName, true );
	m_listBackgroundDlg.push_front( pDialog );
}

void CLayoutView::ClearBackgroundDialog()
{
	std::list<CEtUIDialog *>::iterator iter = m_listBackgroundDlg.begin();
	for( ; iter != m_listBackgroundDlg.end(); ++iter )
	{
		SAFE_DELETE( *iter );
	}
	m_listBackgroundDlg.clear();
}

void CLayoutView::DeleteBackgroundDialog( int nIndex )
{
	int nCount = 0;
	std::list<CEtUIDialog *>::iterator iter = m_listBackgroundDlg.begin();
	for( ; iter != m_listBackgroundDlg.end(); ++iter, ++nCount )
	{
		if( nCount == nIndex )
		{
			SAFE_DELETE( *iter );
			m_listBackgroundDlg.erase( iter );
			return;
		}
	}
}

struct MyOutputHandler : public nvtt::OutputHandler
{
	MyOutputHandler(const char * name) : total(0), progress(0), percentage(0), stream(new nv::StdOutputStream(name)) {}
	virtual ~MyOutputHandler() { delete stream; }

	void setTotal(int64 t)
	{
		total = t + 128;
	}
	void setDisplayProgress(bool b)
	{
		verbose = b;
	}

	virtual void beginImage(int size, int width, int height, int depth, int face, int miplevel)
	{
		// ignore.
	}

	// Output data.
	virtual bool writeData(const void * data, int size)
	{
		nvDebugCheck(stream != NULL);
		stream->serialize(const_cast<void *>(data), size);

		progress += size;
		int p = int((100 * progress) / total);
		if (verbose && p != percentage)
		{
			nvCheck(p >= 0);

			percentage = p;
			printf("\r%d%%", percentage);
			fflush(stdout);
		}

		return true;
	}

	int64 total;
	int64 progress;
	int percentage;
	bool verbose;
	nv::StdOutputStream * stream;
};

struct MyErrorHandler : public nvtt::ErrorHandler
{
	virtual void error(nvtt::Error e)
	{
		nvDebugBreak();
	}
};

void CompressDXT5( const char *pFileName )
{
	nvtt::InputOptions inputOptions;
	if( pFileName )
	{
		nv::DirectDrawSurface dds(pFileName);

		inputOptions.setTextureLayout(nvtt::TextureType_2D, dds.width(), dds.height());
		nv::Image mipmap;
		dds.mipmap(&mipmap, 0, 0);
		inputOptions.setMipmapData(mipmap.pixels(), mipmap.width(), mipmap.height(), 1, 0, 0);
		inputOptions.setWrapMode(nvtt::WrapMode_Clamp);
		inputOptions.setNormalMap(false);
		inputOptions.setConvertToNormalMap(false);
		inputOptions.setGamma(2.2f, 2.2f);
		inputOptions.setNormalizeMipmaps(false);
		inputOptions.setMipmapGeneration(false);
	}
	else
	{
		return;
	}

	nvtt::CompressionOptions compressionOptions;
	compressionOptions.setFormat(nvtt::Format_BC3);
	compressionOptions.setQuality(nvtt::Quality_Normal);

	MyErrorHandler errorHandler;
	MyOutputHandler outputHandler(pFileName);
	if (outputHandler.stream->isError())
	{
		return;
	}

	nvtt::Compressor compressor;
	compressor.enableCudaAcceleration(false);

	outputHandler.setTotal(compressor.estimateSize(inputOptions, compressionOptions));
	outputHandler.setDisplayProgress(false);

	nvtt::OutputOptions outputOptions;
	outputOptions.setOutputHandler(&outputHandler);
	outputOptions.setErrorHandler(&errorHandler);

	compressor.process(inputOptions, compressionOptions, outputOptions);
}

void CLayoutView::SaveLayoutDlg( const char *pszFileName, bool bDoNotCompressTexture, bool bMessageBoxSaveFailed )
{
	CFileStream Stream( pszFileName, CFileStream::OPEN_WRITE );

	if( !Stream.IsValid() )
	{
		if( bMessageBoxSaveFailed ) {
			char szString[ 1024 ];
			sprintf_s( szString, 1024, "%s 파일 세이브 실패!!!", pszFileName );
			MessageBox( szString );
		}
		return;
	}

	char szTextureName[ _MAX_PATH ], *pFindPtr, szFullName[ _MAX_PATH ];
	EtTextureHandle hTexture;

	ReconstructLayout();
	SortDialogControl();
	RegenerateTexture();
	ReinitAllTemplate();

	strcpy_s( szTextureName, _MAX_PATH, pszFileName );
	pFindPtr = strrchr( szTextureName, '.' );
	if( pFindPtr == NULL )
	{
		pFindPtr = szTextureName + strlen( szTextureName );
	}
	strcpy_s( pFindPtr, 5, ".dds" );
	hTexture = m_LayoutDlg.GetUITexture();
	if( hTexture )
	{
//		hTexture->ChangeFormat( FMT_DXT5, USAGE_DEFAULT, POOL_MANAGED );
		D3DXSaveTextureToFile( szTextureName, D3DXIFF_DDS, hTexture->GetTexturePtr(), NULL );
		if( !bDoNotCompressTexture )
		{
			CompressDXT5( szTextureName );
		}
	}
	else
	{
		// 나중에 일괄로 지울때 이렇게 하면 될거다.
		//DeleteFile( szTextureName );
	}

	_GetFullFileName( szFullName, _countof(szFullName), szTextureName );
	m_LayoutDlg.SetUITextureName( szFullName );

	m_LayoutDlg.Save( Stream );
	m_szCurFileName = pszFileName;
}

void CLayoutView::LoadLayoutDlg( const char *pszFileName )
{
	CFileStream Stream( pszFileName, CFileStream::OPEN_READ );

	if( !Stream.IsValid() )
	{
		char szString[ 1024 ];
		sprintf_s( szString, 1024, "%s 파일 로드 실패!!!", pszFileName );
		MessageBox( szString );
		return;
	}

	char szPath[ _MAX_PATH ];

	_GetPath( szPath, _countof(szPath), pszFileName );
	CEtResourceMng::GetInstance().AddResourcePath( szPath );

	NewLayoutDlg();

	m_LayoutDlg.Load( Stream );

	// 다이얼로그 텍스처가 다름을 알린다.
	char szFullName[ _MAX_FNAME ];
	_GetFullFileName( szFullName, _countof(szFullName), pszFileName );
	int nLen = (int)strlen( szFullName );
	szFullName[ nLen-3 ] = '\0';
	strcat_s( szFullName, _countof(szFullName), ".dds" );
	const char *pUITextureFileName = m_LayoutDlg.GetDialogUITextureFileName();
	if( _stricmp( szFullName, pUITextureFileName ) != 0 )
	{
		MessageBox("다이얼로그이름과 UI텍스처이름이 매칭되지 않습니다.\n텍스처가 깨지거나 안 보일 수 있습니다.");
	}

	// 읽기 전용 검사
	DWORD dwAttr;
	dwAttr = GetFileAttributes( pszFileName );
	if( dwAttr & FILE_ATTRIBUTE_READONLY ) MessageBox("ui 파일이 읽기전용 속성입니다.");

	/*
	sprintf_s( szFullName, _countof(szFullName), "%s%s", szPath, pUITextureFileName );
	dwAttr = GetFileAttributes( szFullName );
	if( dwAttr & FILE_ATTRIBUTE_READONLY ) MessageBox("dds 파일이 읽기전용 속성입니다.");
	*/

	CUIToolTemplate *pToolTemplate;
	CEtUITemplate *pTemplate;
	CEtUIControl *pControl;
	CTemplatePaneView *pView;
	SCreateControlInfo CreateInfo;

	pView = ( CTemplatePaneView * )GetPaneWnd( TEMPLATE_PANE );

	for( int i = 0; i < m_LayoutDlg.GetTemplateCount(); i++ )
	{
		pTemplate = m_LayoutDlg.GetTemplate( i );
		pToolTemplate = pView->FindTemplate( pTemplate->m_szTemplateName.c_str() );

		if( !pToolTemplate )
		{
			// 어차피 제대로 로딩도 안될바엔 로드를 포기하자.
			NewLayoutDlg();
			return;
		}


		if( pToolTemplate )
		{
			m_vecLayoutTemplate.push_back( pToolTemplate );

			// UI에서 로딩되는 템플릿들의 템플릿 텍스처를 로드한다.
			// 툴에서 미리 전부 로드해두고있는 템플릿은 처음부터 텍스처를 로드하는 구조가 아니기 때문에 이렇게 필요할때 로드하는 것.
			pToolTemplate->m_Template.OnLoaded();
		}
	}

	for( int i = 0; i < m_LayoutDlg.GetControlCount(); i++ )
	{
		pControl = m_LayoutDlg.GetControl( i );
		CreateInfo.pControl = pControl;
		CreateInfo.emSelect = SELECT_NONE;
		pControl->GetProperty( CreateInfo.Property );
		CreateInfo.nTabOrder = i;
		m_vecLayoutControl.push_back( CreateInfo );
	}

	m_szCurFileName = pszFileName;
	m_LayoutDlg.GetDlgInfo( m_DlgInfo );

	CEtResourceMng::GetInstance().RemoveResourcePath( szPath );
	ReinitAllControl();
}

void CLayoutView::SaveCustomUI()
{
	CCustomControlSelect ControlSelectDlg;

	if( ControlSelectDlg.DoModal() == IDOK )
	{
		// 임시변수에 기억해두고,
		m_nCurCustomControlIDforCustomSave = ControlSelectDlg.m_nCurSel;
		g_vecFileNameforCustomSave.clear();

		char szFindFile[ _MAX_PATH ];
		WIN32_FIND_DATA FindFileData;
		HANDLE hFind;

		strcpy_s( szFindFile, _MAX_PATH, CMainFrame::GetResourceFolder() );
		strcat_s( szFindFile, _MAX_PATH, "\\UI" );
		strcat_s( szFindFile, _MAX_PATH, "\\*.*" );

		hFind = FindFirstFile( szFindFile, &FindFileData );
		if( hFind == INVALID_HANDLE_VALUE )
		{
			FindClose( hFind );
			return;
		}

		// 안정성을 위해 핸들닫고 재귀를 통한 검색 시작.(다시 핸들 열거다.)
		FindClose( hFind );

		strcpy_s( szFindFile, _MAX_PATH, CMainFrame::GetResourceFolder() );
		strcat_s( szFindFile, _MAX_PATH, "\\UI" );
		_SearchSubFolderUIFile( szFindFile );

		NewLayoutDlg();

		// 찾은 파일들을 리스트에 보여준 후
		CDialogList DialogListDlg;
		if( DialogListDlg.DoModal() == IDOK )
		{
			for( int i = 0; i < (int)g_vecFileNameforCustomSave.size(); ++i )
			{
				LoadLayoutDlg( g_vecFileNameforCustomSave[i].c_str() );
				SaveLayoutDlg( g_vecFileNameforCustomSave[i].c_str() );
			}
			// 확인을 누르면 변환.
			NewLayoutDlg();
		}
	}
}

void CLayoutView::_SearchSubFolderUIFile( const char *pszFolderName )
{
	char szFindFile[ _MAX_PATH ], szFileName[ _MAX_PATH ];
	WIN32_FIND_DATA FindFileData;
	HANDLE hFind;
	BOOL bResult = TRUE;

	strcpy_s( szFindFile, _MAX_PATH, pszFolderName );
	strcat_s( szFindFile, _MAX_PATH, "\\*.*" );

	// 폴더안에 있는 파일을 찾아 순회한다.
	hFind = FindFirstFile( szFindFile, &FindFileData );
	if (hFind == INVALID_HANDLE_VALUE)
		return;

	while( bResult )
	{
		if( !( FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) )
		{
			sprintf_s( szFileName, _MAX_PATH, "%s\\%s", pszFolderName, FindFileData.cFileName );
			int nLen = (int)strlen( szFileName );
			if( szFileName[nLen-3] == '.' && szFileName[nLen-2] == 'u' && szFileName[nLen-1] == 'i' )
			{
				// 여기서만 로딩 후 처리.
				LoadLayoutDlg( szFileName );

				// 뭔가 다른 처리.
				int nControlCount = m_LayoutDlg.GetControlCount();
				for( int i = 0; i < nControlCount; ++i )
				{
					CEtUIControl *pControl = m_LayoutDlg.GetControl(i);
					if( pControl->GetProperty()->nCustomControlID == m_nCurCustomControlIDforCustomSave )
					{
						g_vecFileNameforCustomSave.push_back( szFileName );
						break;
					}
				}
			}
		}
		else if( FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
		{
			// 모든 폴더에는 항상 . 과 .. 폴더가 있다. 이건 그냥 넘기고, 다른 폴더라면,
			if( FindFileData.cFileName[0] != '.' )
			{
				// 하위폴더의 경우 재귀호출로 처리해야한다.
				sprintf_s( szFileName, _MAX_PATH, "%s\\%s", pszFolderName, FindFileData.cFileName );
				_SearchSubFolderUIFile( szFileName );
			}
		}

		bResult = FindNextFile( hFind, &FindFileData );
	}

	FindClose( hFind );
}

void CLayoutView::SaveUIWithUsedTemplate()
{
	if( !g_pCurSelTemplate )
		return;

	g_vecFileNameforCustomSave.clear();
	m_nCurSelTemplateNameforCustomSave = g_pCurSelTemplate->m_Template.m_szTemplateName;

	char szFindFile[ _MAX_PATH ];
	WIN32_FIND_DATA FindFileData;
	HANDLE hFind;

	strcpy_s( szFindFile, _MAX_PATH, CMainFrame::GetResourceFolder() );
	strcat_s( szFindFile, _MAX_PATH, "\\UI" );
	strcat_s( szFindFile, _MAX_PATH, "\\*.*" );

	hFind = FindFirstFile( szFindFile, &FindFileData );
	if( hFind == INVALID_HANDLE_VALUE )
	{
		FindClose( hFind );
		return;
	}

	// 안정성을 위해 핸들닫고 재귀를 통한 검색 시작.(다시 핸들 열거다.)
	FindClose( hFind );

	strcpy_s( szFindFile, _MAX_PATH, CMainFrame::GetResourceFolder() );
	strcat_s( szFindFile, _MAX_PATH, "\\UI" );
	_SaveUIWithUsedTemplate( szFindFile );

	NewLayoutDlg();

	// 찾은 파일들을 리스트에 보여준 후
	CDialogList DialogListDlg;
	if( DialogListDlg.DoModal() == IDOK )
	{
		for( int i = 0; i < (int)g_vecFileNameforCustomSave.size(); ++i )
		{
			LoadLayoutDlg( g_vecFileNameforCustomSave[i].c_str() );
			SaveLayoutDlg( g_vecFileNameforCustomSave[i].c_str() );
		}
		NewLayoutDlg();
	}
}

void CLayoutView::_SaveUIWithUsedTemplate( const char *pszFolderName )
{
	char szFindFile[ _MAX_PATH ], szFileName[ _MAX_PATH ];
	WIN32_FIND_DATA FindFileData;
	HANDLE hFind;
	BOOL bResult = TRUE;

	strcpy_s( szFindFile, _MAX_PATH, pszFolderName );
	strcat_s( szFindFile, _MAX_PATH, "\\*.*" );

	// 폴더안에 있는 파일을 찾아 순회한다.
	hFind = FindFirstFile( szFindFile, &FindFileData );
	if (hFind == INVALID_HANDLE_VALUE)
		return;

	while( bResult )
	{
		if( !( FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) )
		{
			sprintf_s( szFileName, _MAX_PATH, "%s\\%s", pszFolderName, FindFileData.cFileName );
			int nLen = (int)strlen( szFileName );
			if( szFileName[nLen-3] == '.' && szFileName[nLen-2] == 'u' && szFileName[nLen-1] == 'i' )
			{
				LoadLayoutDlg( szFileName );

				for( int i = 0; i < m_LayoutDlg.GetTemplateCount(); i++ )
				{
					CEtUITemplate *pTemplate = m_LayoutDlg.GetTemplate( i );

					if( _stricmp( m_nCurSelTemplateNameforCustomSave.c_str(), pTemplate->m_szTemplateName.c_str() ) == 0 )
					{
						g_vecFileNameforCustomSave.push_back( szFileName );
						break;
					}
				}
			}
		}
		else if( FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
		{
			// 모든 폴더에는 항상 . 과 .. 폴더가 있다. 이건 그냥 넘기고, 다른 폴더라면,
			if( FindFileData.cFileName[0] != '.' )
			{
				// 하위폴더의 경우 재귀호출로 처리해야한다.
				sprintf_s( szFileName, _MAX_PATH, "%s\\%s", pszFolderName, FindFileData.cFileName );
				_SaveUIWithUsedTemplate( szFileName );
			}
		}

		bResult = FindNextFile( hFind, &FindFileData );
	}

	FindClose( hFind );
}

void CLayoutView::SaveAllUI()
{
	char szFindFile[ _MAX_PATH ];
	WIN32_FIND_DATA FindFileData;
	HANDLE hFind;

	strcpy_s( szFindFile, _MAX_PATH, CMainFrame::GetResourceFolder() );
	strcat_s( szFindFile, _MAX_PATH, "\\UI" );
	strcat_s( szFindFile, _MAX_PATH, "\\*.*" );

	hFind = FindFirstFile( szFindFile, &FindFileData );
	if( hFind == INVALID_HANDLE_VALUE )
	{
		FindClose( hFind );
		return;
	}

	// 안정성을 위해 핸들닫고 재귀를 통한 검색 시작.(다시 핸들 열거다.)
	FindClose( hFind );

	strcpy_s( szFindFile, _MAX_PATH, CMainFrame::GetResourceFolder() );
	strcat_s( szFindFile, _MAX_PATH, "\\UI" );
	_SaveAllUI( szFindFile );
}

void CLayoutView::_SaveAllUI( const char *pszFolderName )
{
	char szFindFile[ _MAX_PATH ], szFileName[ _MAX_PATH ];
	WIN32_FIND_DATA FindFileData;
	HANDLE hFind;
	BOOL bResult = TRUE;

	strcpy_s( szFindFile, _MAX_PATH, pszFolderName );
	strcat_s( szFindFile, _MAX_PATH, "\\*.*" );

	// 폴더안에 있는 파일을 찾아 순회한다.
	hFind = FindFirstFile( szFindFile, &FindFileData );
	if (hFind == INVALID_HANDLE_VALUE)
		return;

	while( bResult )
	{
		if( !( FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) )
		{
			sprintf_s( szFileName, _MAX_PATH, "%s\\%s", pszFolderName, FindFileData.cFileName );
			int nLen = (int)strlen( szFileName );
			if( szFileName[nLen-3] == '.' && szFileName[nLen-2] == 'u' && szFileName[nLen-1] == 'i' )
			{
				// 로딩, 저장. 반복
				LoadLayoutDlg( szFileName );
				SaveLayoutDlg( szFileName, false, false );
			}
		}
		else if( FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
		{
			// 모든 폴더에는 항상 . 과 .. 폴더가 있다. 이건 그냥 넘기고, 다른 폴더라면,
			if( FindFileData.cFileName[0] != '.' )
			{
				// 하위폴더의 경우 재귀호출로 처리해야한다.
				sprintf_s( szFileName, _MAX_PATH, "%s\\%s", pszFolderName, FindFileData.cFileName );
				_SaveAllUI( szFileName );
			}
		}

		bResult = FindNextFile( hFind, &FindFileData );
	}

	FindClose( hFind );
}

// CLayoutView message handlers

BOOL CLayoutView::OnEraseBkgnd(CDC* pDC)
{
//	return CFormView::OnEraseBkgnd( pDC );
	return FALSE;
}

void CLayoutView::OnLButtonDown(UINT nFlags, CPoint point)
{
	if( !IsCurTabOrder() )
	{
		m_CusorPt = point;
		SetRect( &m_CurDragRect, 0, 0, 0, 0 );

		CEtUIControl *pControl;
		bool bMultiSelect = false;
		bool bResizeSelect = false;

		if( nFlags & MK_CONTROL )
		{
			bMultiSelect = true;
		}
		
		if( nFlags & MK_SHIFT )
		{
			bResizeSelect = true;
		}

		pControl = GetControl( point );

		if( IsSelectedControl( pControl ) )
		{
			// 이렇게 처리하면, 다중선택에서 Last를 바꿀 수 없게 된다.
			//if( bResizeSelect )
			//{
			//	SelectAllControl(false);
			//	SelectControl(point, bMultiSelect);
			//}

			if ( bResizeSelect )
				SelectAllControl( false );
			SelectControl( point, bMultiSelect );

			m_pMoveControl = pControl;
		}
		else
		{
			m_pMoveControl = SelectControl( point, bMultiSelect );
		}

		m_bMoveMode = false;
		m_bDragMode = true;
	}

	CFormView::OnLButtonDown(nFlags, point);
}

void CLayoutView::OnLButtonUp(UINT nFlags, CPoint point)
{
	if( !IsCurTabOrder() )
	{
		SUICoord ControlCoord;
		ControlCoord.fX = m_CurDragRect.left / m_LayoutDlg.GetScreenWidth();
		ControlCoord.fY = m_CurDragRect.top / m_LayoutDlg.GetScreenHeight();
		ControlCoord.fWidth = ( m_CurDragRect.right - m_CurDragRect.left ) / m_LayoutDlg.GetScreenWidth();
		ControlCoord.fHeight = ( m_CurDragRect.bottom - m_CurDragRect.top ) / m_LayoutDlg.GetScreenHeight();

		if( !m_pMoveControl )
		{
			if( m_bDragMode )
			{
				if( ( abs( m_CurDragRect.right - m_CurDragRect.left ) > 5 ) && ( abs( m_CurDragRect.bottom - m_CurDragRect.top ) > 5 ) )
				{
					if( g_pCurSelTemplate == NULL )
					{
						if( nFlags & MK_SHIFT )
						{
							ResizeDialog( ControlCoord );
						}
						else
						{
							SelectControlRect();
						}
					}
					else
					{
						if( nFlags & MK_CONTROL )
						{
							AddLayoutControl( ControlCoord, false );
						}
						else
						{
							AddLayoutControl( ControlCoord, true );
						}

						g_pCurSelTemplate = NULL;
					}
				}
			}
		}
		else
		{
			if( m_bMoveMode )
			{
				if( nFlags & MK_SHIFT )
				{
					ResizeSelectedControl( ControlCoord );
				}
				else
				{
					m_CusorPt = point - m_CusorPt;
					MoveSelectedControl( m_CusorPt );
				}
			}
			else if( ( m_pMoveControl ) && ( !( nFlags & MK_CONTROL ) ) )
			{
				SelectControl( point, false );
			}
		}

		SetRect( &m_CurDragRect, 0, 0, 0, 0 );
		CRenderBase::GetInstance().SetDragRect( m_CurDragRect );
	}
	else
	{
		CEtUIControl *pControl;
		pControl = GetControl( point );
		if( pControl )
		{
			if( SetTabOrder( m_nCurTabOrder, pControl ) )
			{
				m_nCurTabOrder++;
				if( GetControlCount() <= m_nCurTabOrder )
				{
					EndTabOrder();
					m_bTabOrderMode = false;
				}
			}
		}
		else
		{
			EndTabOrder();
			m_bTabOrderMode = false;
		}
	}

	m_bDragMode = false;

	CFormView::OnLButtonUp(nFlags, point);
}

void CLayoutView::OnMouseMove(UINT nFlags, CPoint point)
{
	if( !IsCurTabOrder() )
	{
		CMainFrame *pFrame;
		CString szPosition;

		if( nFlags & MK_LBUTTON )
		{
			float fX, fY;
			SUICoord Coord;
			DWORD dwColor(0xffff0000);
			bool bSolid(true);

			if( m_pMoveControl )
			{
				GetLayoutDlgPosition( fX, fY );
				m_pMoveControl->GetUICoord( Coord );

				if( m_bMoveMode )
				{
					if( nFlags & MK_SHIFT )
					{
						LONG lnX = LONG((fX + Coord.fX)*DEFAULT_UI_SCREEN_WIDTH);
						LONG lnY = LONG((fY + Coord.fY)*DEFAULT_UI_SCREEN_HEIGHT);

						SetRect( &m_CurDragRect, min( lnX, point.x ), min( lnY, point.y ), max( lnX, point.x ), max( lnY, point.y ) );
						dwColor = 0xff00ff00; // Note : 녹색 속빈 테두리
						bSolid = true;
					}
					else
					{
						CPoint MovePoint;
						MovePoint = point - m_CusorPt;
						Coord.fX += fX + MovePoint.x / ( float )DEFAULT_UI_SCREEN_WIDTH;
						Coord.fY += fY + MovePoint.y / ( float )DEFAULT_UI_SCREEN_HEIGHT;

						SetRect( &m_CurDragRect,( int )( Coord.fX * DEFAULT_UI_SCREEN_WIDTH ), 
												( int )( Coord.fY * DEFAULT_UI_SCREEN_HEIGHT ), 
												( int )( ( Coord.fX + Coord.fWidth ) * DEFAULT_UI_SCREEN_WIDTH ),
												( int )( ( Coord.fY + Coord.fHeight ) * DEFAULT_UI_SCREEN_HEIGHT ) );
						dwColor = 0xff0000ff; // Note : 파란색 속빈 테두리
						bSolid = true;
					}
				}
				else if( ( abs( point.x - m_CusorPt.x ) > 5 ) || ( abs( point.y - m_CusorPt.y ) > 5 ) )
				{
					m_bMoveMode = true;
				}
			}
			else
			{
				if( nFlags & MK_SHIFT )
				{
					GetLayoutDlgPosition( fX, fY );

					LONG lnX = LONG(fX*DEFAULT_UI_SCREEN_WIDTH);
					LONG lnY = LONG(fY*DEFAULT_UI_SCREEN_HEIGHT);

					SetRect( &m_CurDragRect, min( lnX, point.x ), min( lnY, point.y ), max( lnX, point.x ), max( lnY, point.y ) );
					dwColor = 0xff00ff00; // Note : 녹색 속빈 테두리
					bSolid = true;
				}
				else
				{
					SetRect( &m_CurDragRect, min( point.x, m_CusorPt.x ), min( point.y, m_CusorPt.y ), max( point.x, m_CusorPt.x ), max( point.y, m_CusorPt.y ) );

					if( g_pCurSelTemplate == NULL )
					{
						bSolid = false;
					}
				}
			}

			if( m_bDragMode )
			{
				CRenderBase::GetInstance().SetDragRect( m_CurDragRect, dwColor, bSolid );
			}
			szPosition.Format( "X : %d, Y: %d Width : %d Height : %d", point.x, point.y, m_CurDragRect.right - m_CurDragRect.left, m_CurDragRect.bottom - m_CurDragRect.top );
		}
		else
		{
			szPosition.Format( "X : %d, Y: %d", point.x, point.y );
		}

		pFrame = ( CMainFrame * )AfxGetMainWnd();
		pFrame->SetPaneText( 1, szPosition );
	}

	CFormView::OnMouseMove(nFlags, point);
}

void CLayoutView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	CPoint Point;

	CEtUIToolView *pView(NULL);
	pView = ( CEtUIToolView * )CGlobalValue::GetInstance().m_pCurView;
	ASSERT(pView&&"CLayoutView::OnKeyDown, pView is NULL!");
	
	switch( nChar )
	{
	case VK_LEFT:
		Point.x = -1;
		Point.y = 0;
		if( ( GetAsyncKeyState( VK_LSHIFT ) ) < 0 )
			Point.x *= 10;
		MoveSelectedControl( Point );
		break;
	case VK_RIGHT:
		Point.x = 1;
		Point.y = 0;
		if( ( GetAsyncKeyState( VK_LSHIFT ) ) < 0 )
			Point.x *= 10;
		MoveSelectedControl( Point );
		break;
	case VK_UP:
		Point.x = 0;
		Point.y = -1;
		if( ( GetAsyncKeyState( VK_LSHIFT ) ) < 0 )
			Point.y *= 10;
		MoveSelectedControl( Point );
		break;
	case VK_DOWN:
		Point.x = 0;
		Point.y = 1;
		if( ( GetAsyncKeyState( VK_LSHIFT ) ) < 0 )
			Point.y *= 10;
		MoveSelectedControl( Point );
		break;
	}

	if( pView->GetDummyView()->IsShowTemplateOnLayoutView() == true )
	{
		switch( nChar )
		{
		case 0x31:
			pView->GetDummyView()->AlignTemplateDialog( AT_HORI_LEFT, AT_VERT_TOP );
			break;
		case 0x32:
			pView->GetDummyView()->AlignTemplateDialog( AT_HORI_RIGHT, AT_VERT_TOP );
			break;
		case 0x33:
			pView->GetDummyView()->AlignTemplateDialog( AT_HORI_LEFT, AT_VERT_BOTTOM );
			break;
		case 0x34:
			pView->GetDummyView()->AlignTemplateDialog( AT_HORI_RIGHT, AT_VERT_BOTTOM );
			break;
		case 0x35:
			pView->GetDummyView()->AlignTemplateDialog( AT_HORI_CENTER, AT_VERT_CENTER );
			break;
		}
	}
	else
	{
		if( nChar >= 0x31 && nChar <= 0x38 )
		{
			int nIndex = nChar - 0x31;
			if( ( GetAsyncKeyState( VK_LCONTROL ) ) < 0 )
			{
				// 그룹화
				if( GetSelectedControlCount() )
					MakeGroupSelectedControl( nIndex );
			}
			else
			{
				// 그룹선택
				SelectControlGroup( nIndex );
			}
		}
		else if( nChar == 0x30 )
		{
			if( m_nLastSelectGroup >= 0 && m_nLastSelectGroup < NUM_CONTROL_GROUP )
			{
				m_vecControlGroup[m_nLastSelectGroup].clear();
				SelectAllControl( false );
				DrawSelectControl();
				pView->InvalidateRect( NULL, TRUE );
			}
		}
	}

	CFormView::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CLayoutView::OnFileSave()
{
	if( m_szCurFileName.IsEmpty() )
	{
		OnFileSaveAs();
	}
	else
	{
		SaveLayoutDlg( m_szCurFileName );

		CEtUIToolDoc *pDoc = (CEtUIToolDoc*)GetDocument();
		if( pDoc ) pDoc->SetPathName( m_szCurFileName );
	}
}

void CLayoutView::OnFileSaveAs()
{
	TCHAR szFilter[] = _T( "Eternity UI File (*.ui)|*.ui|All Files (*.*)|*.*||" );
	CCompressCheckDlg FileDlg( FALSE, _T("ui"), _T("*.ui"), OFN_HIDEREADONLY | OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_ENABLESIZING | OFN_NOCHANGEDIR, szFilter, this );

	if( FileDlg.DoModal() == IDOK )
	{
		SaveLayoutDlg( FileDlg.m_ofn.lpstrFile, FileDlg.m_nDoNotCompressTexture ? true : false );

		CEtUIToolDoc *pDoc = (CEtUIToolDoc*)GetDocument();
		if( pDoc ) pDoc->SetPathName(FileDlg.m_ofn.lpstrFile);
	}
}

void CLayoutView::OnFileOpen()
{
	TCHAR szFilter[] = _T( "Eternity UI File (*.ui)|*.ui|All Files (*.*)|*.*||" );
	CFileDialog FileDlg( TRUE, _T("ui"), _T("*.ui"), OFN_HIDEREADONLY | OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_ENABLESIZING | OFN_NOCHANGEDIR | OFN_ALLOWMULTISELECT, szFilter, this );

#ifdef FILEDIG_AUTOPATH
	if( strcmp(CXTCustomPropertyGridItemFile::s_szExt.GetBuffer(), "*.wav") == 0 ) {
		FileDlg.GetOFN().lpstrInitialDir = CXTCustomPropertyGridItemFile::s_szDirectory;
		CXTCustomPropertyGridItemFile::s_szExt = "*.ui";
		CXTCustomPropertyGridItemFile::s_nMode = 1;
	}
#endif

	if( FileDlg.DoModal() == IDOK )
	{
		LoadLayoutDlg( FileDlg.m_ofn.lpstrFile );

		CEtUIToolDoc *pDoc = (CEtUIToolDoc*)GetDocument();
		if( pDoc ) pDoc->SetPathName(FileDlg.m_ofn.lpstrFile);
#ifdef FILEDLG_AUTOPATH
		GetCurrentDirectory(256, CXTCustomPropertyGridItemFile::s_szDirectory);
#endif
	}
}

void CLayoutView::OnFileNew()
{
	NewLayoutDlg();
	DrawSelectControl();

	CEtUIToolDoc *pDoc = (CEtUIToolDoc*)GetDocument();
	if( pDoc ) pDoc->SetPathName("제목 없음");
}

void CLayoutView::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();

	if( m_szCurFileName.GetLength() > 0 ) return;

	m_DlgInfo.AllignHori = AT_HORI_CENTER;
	m_DlgInfo.AllignVert = AT_VERT_CENTER;
	m_DlgInfo.DlgCoord.fWidth = 0.625f;
	m_DlgInfo.DlgCoord.fHeight = 0.625f;
	m_DlgInfo.DlgCoord.fX = 0.0f;
	m_DlgInfo.DlgCoord.fY = 0.0f;
	m_DlgInfo.dwDlgColor = 0xffffffff;

	m_LayoutDlg.Initialize( true );
	m_LayoutDlg.SetDlgInfo( m_DlgInfo );

	m_BackgroundDlg.Initialize( false );

	CControlPropertyPaneView *pView;
	pView = ( CControlPropertyPaneView * )GetPaneWnd( CONTROL_PROPERTY_PANE );
	if( pView )
	{
		pView->SetDlgInfo( &m_DlgInfo );
	}

	LoadNotUseTemplateList();

	UndoCheckPoint();
}

LRESULT CLayoutView::OnOpenView( WPARAM wParam, LPARAM lParam )
{
	m_LayoutDlg.Show( true );
	return 1;
}

LRESULT CLayoutView::OnCloseView( WPARAM wParam, LPARAM lParam )
{
	m_LayoutDlg.Show( false );
	return 1;
}

BOOL CLayoutView::CheckMultiSelectedCount()
{
	int nCount;

	nCount = GetSelectedControlCount();
	if( nCount >= 2 )
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

void CLayoutView::OnUpdateAllignBottom(CCmdUI *pCmdUI)
{
	pCmdUI->Enable( CheckMultiSelectedCount() );
}

void CLayoutView::OnUpdateAllignCenter(CCmdUI *pCmdUI)
{
	pCmdUI->Enable( CheckMultiSelectedCount() );
}

void CLayoutView::OnUpdateAllignLeft(CCmdUI *pCmdUI)
{
	pCmdUI->Enable( CheckMultiSelectedCount() );
}

void CLayoutView::OnUpdateAllignMiddle(CCmdUI *pCmdUI)
{
	pCmdUI->Enable( CheckMultiSelectedCount() );
}

void CLayoutView::OnUpdateAllignRight(CCmdUI *pCmdUI)
{
	pCmdUI->Enable( CheckMultiSelectedCount() );
}

void CLayoutView::OnUpdateAllignTop(CCmdUI *pCmdUI)
{
	pCmdUI->Enable( CheckMultiSelectedCount() );
}

void CLayoutView::OnUpdateMakesamesizeBoth(CCmdUI *pCmdUI)
{
	pCmdUI->Enable( CheckMultiSelectedCount() );
}

void CLayoutView::OnUpdateMakesamesizeHeight(CCmdUI *pCmdUI)
{
	pCmdUI->Enable( CheckMultiSelectedCount() );
}

void CLayoutView::OnUpdateMakesamesizeWidth(CCmdUI *pCmdUI)
{
	pCmdUI->Enable( CheckMultiSelectedCount() );
}

void CLayoutView::OnAllignLeft()
{
	AllignControl( ALLIGN_LEFT );
}

void CLayoutView::OnAllignCenter()
{
	AllignControl( ALLIGN_CENTER );
}

void CLayoutView::OnAllignRight()
{
	AllignControl( ALLIGN_RIGHT );
}

void CLayoutView::OnAllignTop()
{
	AllignControl( ALLIGN_TOP );
}

void CLayoutView::OnAllignMiddle()
{
	AllignControl( ALLIGN_MIDDLE );
}

void CLayoutView::OnAllignBottom()
{
	AllignControl( ALLIGN_BOTTOM );
}

void CLayoutView::OnMakesamesizeWidth()
{
	MakeSameSizeControl( MAKESIZE_WIDTH );
}

void CLayoutView::OnMakesamesizeHeight()
{
	MakeSameSizeControl( MAKESIZE_HEIGHT );
}

void CLayoutView::OnMakesamesizeBoth()
{
	MakeSameSizeControl( MAKESIZE_BOTH );
}

void CLayoutView::OnFormatTaborder()
{
	if (m_bTabOrderMode == false)
	{
		StartTabOrder();
	}
	else
	{
		EndTabOrder();
	}

	m_bTabOrderMode ^= true;
	
}

void CLayoutView::OnFormatInitstate()
{
	m_bInitStateMode ^= true;
	DrawInitState();
}

void CLayoutView::OnUpdateFormatTaborder(CCmdUI *pCmdUI)
{
	if( ( ( CEtUIToolView * )( CGlobalValue::GetInstance().m_pCurView ) )->GetActiveView() == 0 )
	{
		pCmdUI->Enable();
	}
	else
	{
		pCmdUI->Enable( FALSE );
	}

	pCmdUI->SetCheck(m_bTabOrderMode == true);
}

void CLayoutView::OnUpdateFormatInitstate(CCmdUI *pCmdUI)
{
	if( ( ( CEtUIToolView * )( CGlobalValue::GetInstance().m_pCurView ) )->GetActiveView() == 0 )
	{
		pCmdUI->Enable();
	}
	else
	{
		pCmdUI->Enable( FALSE );
	}

	pCmdUI->SetCheck(m_bInitStateMode == true);
}

void CLayoutView::OnEditUndo()
{
	Undo();
	DrawSelectControl();

	if( IsCurTabOrder() )
	{
		EndTabOrder();
	}
}

void CLayoutView::OnEditRedo()
{
	Redo();
	DrawSelectControl();

	if( IsCurTabOrder() )
	{
		EndTabOrder();
	}
}

void CLayoutView::PostNcDestroy()
{
	CFormView::PostNcDestroy();
}

void CLayoutView::OnUpdateEditUndo(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(CanUndo());
}

void CLayoutView::OnUpdateEditRedo(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(CanRedo());
}

void CLayoutView::UndoLoad( CMemFile* pFile)
{
	CByteStream Stream;
	int nCount = (int)pFile->GetLength();
	std::auto_ptr<char> pBuffer(new char[nCount]);

	pFile->SeekToBegin();
	pFile->Read(pBuffer.get(), nCount);
	Stream.WriteBuffer(pBuffer.get(), nCount);

	// Note : 이전 다이얼로그 정보를 삭제한다.
	//
	m_LayoutDlg.DeleteAllControl();
	m_LayoutDlg.DeleteAllTemplate();

	m_vecLayoutControl.clear();

	// Note : 메모리에 저장된 다이얼로그 정보를 로드한다.
	//
	m_LayoutDlg.Load( Stream );

	CEtUIControl *pControl;
	SCreateControlInfo CreateInfo;

	for( int i = 0; i < m_LayoutDlg.GetControlCount(); i++ )
	{
		pControl = m_LayoutDlg.GetControl( i );
		CreateInfo.pControl = pControl;
		CreateInfo.emSelect = SELECT_NONE;
		pControl->GetProperty( CreateInfo.Property );
		CreateInfo.nTabOrder = i;
		m_vecLayoutControl.push_back( CreateInfo );
	}

	RegenerateTexture();

	m_LayoutDlg.GetDlgInfo( m_DlgInfo );
}

void CLayoutView::UndoStore( CMemFile* pFile)
{
	CByteStream Stream;
	m_LayoutDlg.Save( Stream );

	pFile->SeekToBegin();
	pFile->Write( Stream.GetBuffer(), Stream.Size() );
}

void CLayoutView::OnUpdateEditDelete(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(GetSelectedControlCount());
}

void CLayoutView::OnEditDelete()
{
	DeleteSelectedControl();
}

void CLayoutView::OnEditSelectall()
{
	SelectAllControl(true);
	DrawSelectControl();
}

void CLayoutView::OnUpdateEditCopy(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(GetSelectedControlCount());
}

void CLayoutView::OnEditCopy()
{
	m_vecLayoutControlCopy.clear();

	int nSize = GetControlCount();
	for( int i=0; i < nSize; i++ )
	{
		if( m_vecLayoutControl[i].emSelect )
		{
			m_vecLayoutControlCopy.push_back( m_vecLayoutControl[i] );
		}
	}
}

void CLayoutView::OnUpdateEditPaste(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(!m_vecLayoutControlCopy.empty());
}

void CLayoutView::OnEditPaste()
{
	SelectAllControl( false );

	SCreateControlInfo CreateInfo;

	int nCopySize = (int)m_vecLayoutControlCopy.size();
	int nSize = GetControlCount();

	for( int i=0; i<nCopySize; i++ )
	{
		CreateInfo = m_vecLayoutControlCopy[i];

		// 그냥 0.015f 만큼 밀면, float 소수점 오차때문에 텍셀을 깨끗하게 못읽어와 약간 뿌옇게 나오게 된다.
		CreateInfo.Property.UICoord.fX += 15 / ( float )DEFAULT_UI_SCREEN_WIDTH;
		CreateInfo.Property.UICoord.fY += 11 / ( float )DEFAULT_UI_SCREEN_HEIGHT;
		GenerateControlID( CreateInfo.Property.UIType, CreateInfo.Property.szUIName );
		CreateInfo.Property.nID = 0;
		CreateInfo.pControl = m_LayoutDlg.CreateControl( &CreateInfo.Property );
		CreateInfo.emSelect = SELECT_DONE;
		CreateInfo.Property.nTooltipStringIndex = 0;
		CreateInfo.nTabOrder = nSize + i;	//nSize+m_vecLayoutControlCopy[i].nTabOrder; 버그였던 듯.

		m_vecLayoutControl.push_back( CreateInfo );
	}

	m_vecLayoutControl[nSize].emSelect = SELECT_LAST;

	ReinitAllControl();

	if( GetSelectedControlCount() == 1 )
	{
		CControlPropertyPaneView *pView;
		pView = ( CControlPropertyPaneView * )GetPaneWnd( CONTROL_PROPERTY_PANE );
		pView->RefreshControlProperty( &m_vecLayoutControl[ m_vecLayoutControl.size() - 1 ].Property );
	}
}

void CLayoutView::OnCenterindialogVertical()
{
	SUICoord *pUICoord;
	SUICoord sDlgCoord;;
	CEtUIControl *pControl;

	int nSize = GetControlCount();
	for( int i = 0; i < nSize; i++ )
	{
		if( m_vecLayoutControl[ i ].emSelect )
		{
			pUICoord = &m_vecLayoutControl[ i ].Property.UICoord;

			m_LayoutDlg.GetDlgCoord(sDlgCoord);
			pUICoord->fY = sDlgCoord.fY + ((sDlgCoord.fHeight - pUICoord->fHeight)/2.0f);
			pControl = m_vecLayoutControl[ i ].pControl;
			pControl->SetUICoord( m_vecLayoutControl[ i ].Property.UICoord );

			if( m_vecLayoutControl[i].emSelect == SELECT_LAST )
			{
				CControlPropertyPaneView *pView = ( CControlPropertyPaneView * )GetPaneWnd( CONTROL_PROPERTY_PANE );
				if( pView) pView->RefreshControlProperty( &m_vecLayoutControl[ i ].Property );
			}
		}
	}

	DrawSelectControl();
	UndoCheckPoint();
}

void CLayoutView::OnCenterindialogHorizontal()
{
	SUICoord *pUICoord;
	SUICoord sDlgCoord;;
	CEtUIControl *pControl;

	int nSize = GetControlCount();
	for( int i = 0; i < nSize; i++ )
	{
		if( m_vecLayoutControl[ i ].emSelect )
		{
			pUICoord = &m_vecLayoutControl[ i ].Property.UICoord;

			m_LayoutDlg.GetDlgCoord(sDlgCoord);
			pUICoord->fX = sDlgCoord.fX + ((sDlgCoord.fWidth - pUICoord->fWidth)/2.0f);
			pControl = m_vecLayoutControl[ i ].pControl;
			pControl->SetUICoord( m_vecLayoutControl[ i ].Property.UICoord );

			if( m_vecLayoutControl[i].emSelect == SELECT_LAST )
			{
				CControlPropertyPaneView *pView = ( CControlPropertyPaneView * )GetPaneWnd( CONTROL_PROPERTY_PANE );
				if( pView) pView->RefreshControlProperty( &m_vecLayoutControl[ i ].Property );
			}
		}
	}

	DrawSelectControl();
	UndoCheckPoint();
}

void CLayoutView::SortDialogControl()
{
	std::sort( m_vecLayoutControl.begin(), m_vecLayoutControl.end(), LayoutControlSortFunc );
	for( int i = 0; i < ( int )m_vecLayoutControl.size(); i++ )
	{
		m_LayoutDlg.SetControl( i, m_vecLayoutControl[ i ].pControl );
	}
}

void CLayoutView::OnEditDeletedialogtexture()
{
	m_LayoutDlg.DeleteDlgTexture();
	m_LayoutDlg.GetDlgInfo( m_DlgInfo );

	CControlPropertyPaneView *pView = ( CControlPropertyPaneView * )GetPaneWnd( CONTROL_PROPERTY_PANE );
	if( pView ) pView->RefreshPropertyGridVariable();
}

void CLayoutView::OnAllignHorizontal()
{
	CInterval IntervalDlg;

	if( IntervalDlg.DoModal() == IDOK )
	{
		AllignControl( ALLIGN_HORIZONTAL, IntervalDlg.m_ctrlInterval );
	}
}

void CLayoutView::OnAllignVertical()
{
	CInterval IntervalDlg;

	if( IntervalDlg.DoModal() == IDOK )
	{
		AllignControl( ALLIGN_VERTICAL, IntervalDlg.m_ctrlInterval );
	}
}

void CLayoutView::OnUpdateAllignHorizontal(CCmdUI *pCmdUI)
{
	pCmdUI->Enable( CheckMultiSelectedCount() );
}

void CLayoutView::OnUpdateAllignVertical(CCmdUI *pCmdUI)
{
	pCmdUI->Enable( CheckMultiSelectedCount() );
}

void CLayoutView::OnRButtonDown(UINT nFlags, CPoint point)
{
	// 탭 설정때만 작동한다.
	if (m_bTabOrderMode)
	{
		CEtUIControl *pControl = GetControl( point );
		if( pControl )
		{
			CTabOrder TabOrderDlg;

			// 미리 이렇게 컨트롤연결된 변수에 값 넣어두면 DoModal될때 알아서 UpdateData(false) 호출해,
			// 컨트롤에 값이 설정되어있게한다.
			TabOrderDlg.m_ctrlTabNumber = m_vecLayoutControl[ FindControl(pControl) ].nTabOrder+1;

			if ( TabOrderDlg.DoModal() == IDOK )
			{
				int nTabNumber = TabOrderDlg.m_ctrlTabNumber;
				SetTabOrder( nTabNumber-1, pControl );
			}
		}
	}

	CFormView::OnRButtonDown(nFlags, point);
}

void CLayoutView::OnAddCoinTemplate()
{
	// 강제로 코인 템플릿들을 집어넣는다.
	
	CTemplatePaneView *pView;
	pView = ( CTemplatePaneView * )GetPaneWnd( TEMPLATE_PANE );
	ASSERT(pView&&"준후를 호출하세요.");

	// 코인 틀
	g_pCurSelTemplate = pView->FindTemplate( "MoneyBase2" );
	SUICoord ControlCoord;
	ControlCoord.fX = 142 / m_LayoutDlg.GetScreenWidth();
	ControlCoord.fY = 479 / m_LayoutDlg.GetScreenHeight();
	ControlCoord.fWidth = 201 / m_LayoutDlg.GetScreenWidth();
	ControlCoord.fHeight = 25 / m_LayoutDlg.GetScreenHeight();
	AddLayoutControl( ControlCoord, false );

	// 골드
	g_pCurSelTemplate = pView->FindTemplate( "Text_Nanum_12_G" );
	ControlCoord.fX = 148 / m_LayoutDlg.GetScreenWidth();
	ControlCoord.fY = 482 / m_LayoutDlg.GetScreenHeight();
	ControlCoord.fWidth = 62 / m_LayoutDlg.GetScreenWidth();
	ControlCoord.fHeight = 18 / m_LayoutDlg.GetScreenHeight();
	AddLayoutControl( ControlCoord, false );
	SCreateControlInfo &Control1 = m_vecLayoutControl[m_vecLayoutControl.size()-1];
	sprintf_s(Control1.Property.szUIName, _countof(Control1.Property.szUIName), "ID_GOLD");
	Control1.Property.StaticProperty.dwFontFormat &= 0xfffffffc;
	Control1.Property.StaticProperty.dwFontFormat |= DT_RIGHT;
	Control1.Property.StaticProperty.dwFontColor = D3DCOLOR_RGBA(255, 173, 17, 255);

	// 실버
	g_pCurSelTemplate = pView->FindTemplate( "Text_Nanum_12_G" );
	ControlCoord.fX = 239 / m_LayoutDlg.GetScreenWidth();
	ControlCoord.fY = 482 / m_LayoutDlg.GetScreenHeight();
	ControlCoord.fWidth = 24 / m_LayoutDlg.GetScreenWidth();
	ControlCoord.fHeight = 18 / m_LayoutDlg.GetScreenHeight();
	AddLayoutControl( ControlCoord, false );
	SCreateControlInfo &Control2 = m_vecLayoutControl[m_vecLayoutControl.size()-1];
	sprintf_s(Control2.Property.szUIName, _countof(Control2.Property.szUIName), "ID_SILVER");
	Control2.Property.StaticProperty.dwFontFormat &= 0xfffffffc;
	Control2.Property.StaticProperty.dwFontFormat |= DT_RIGHT;
	Control2.Property.StaticProperty.dwFontColor = D3DCOLOR_RGBA(221, 221, 221, 255);

	// 쿠퍼
	g_pCurSelTemplate = pView->FindTemplate( "Text_Nanum_12_G" );
	ControlCoord.fX = 292 / m_LayoutDlg.GetScreenWidth();
	ControlCoord.fY = 482 / m_LayoutDlg.GetScreenHeight();
	ControlCoord.fWidth = 24 / m_LayoutDlg.GetScreenWidth();
	ControlCoord.fHeight = 18 / m_LayoutDlg.GetScreenHeight();
	AddLayoutControl( ControlCoord, false );
	SCreateControlInfo &Control3 = m_vecLayoutControl[m_vecLayoutControl.size()-1];
	sprintf_s(Control3.Property.szUIName, _countof(Control3.Property.szUIName), "ID_BRONZE");
	Control3.Property.StaticProperty.dwFontFormat &= 0xfffffffc;
	Control3.Property.StaticProperty.dwFontFormat |= DT_RIGHT;
	Control3.Property.StaticProperty.dwFontColor = D3DCOLOR_RGBA(178, 119, 71, 255);

	g_pCurSelTemplate = NULL;
	ReinitAllControl();
}

void CLayoutView::OnViewCrosshair()
{
	// TODO: Add your command handler code here
	m_nViewCrosshair = !m_nViewCrosshair;
	RECT rcDialog;
	rcDialog.left = (int)(m_LayoutDlg.GetXCoord() * DEFAULT_UI_SCREEN_WIDTH);
	rcDialog.top = (int)(m_LayoutDlg.GetYCoord() * DEFAULT_UI_SCREEN_HEIGHT);
	rcDialog.right = (int)((m_LayoutDlg.GetXCoord() + m_LayoutDlg.Width()) * DEFAULT_UI_SCREEN_WIDTH);
	rcDialog.bottom = (int)((m_LayoutDlg.GetYCoord() + m_LayoutDlg.Height()) * DEFAULT_UI_SCREEN_HEIGHT);
	CRenderBase::GetInstance().SetCrosshair( m_nViewCrosshair ? true : false, rcDialog );
}

void CLayoutView::OnUpdateViewCrosshair(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	pCmdUI->SetCheck(m_nViewCrosshair);
}

void CLayoutView::OnViewWidescreen()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	m_nWideScreen = !m_nWideScreen;

	CRenderBase::GetInstance().Reinitialize( m_nWideScreen ? DEFAULT_UI_WIDESCREEN_WIDTH : DEFAULT_UI_SCREEN_WIDTH, DEFAULT_UI_SCREEN_HEIGHT );

	CEtUIToolView *pView(NULL);
	pView = ( CEtUIToolView * )CGlobalValue::GetInstance().m_pCurView;
	pView->InvalidateRect( NULL, TRUE );
}

void CLayoutView::OnUpdateViewWidescreen(CCmdUI *pCmdUI)
{
	// TODO: 여기에 명령 업데이트 UI 처리기 코드를 추가합니다.
	pCmdUI->SetCheck( m_nWideScreen );
}

void CLayoutView::OnFormatMakesameproperty()
{
	// TODO: Add your command handler code here
	int i, nMainIndex;
	CEtUIControl *pMainControl;
	SUIControlProperty *pUIProperty;
	SUIControlProperty *pMainUIProperty;

	for( i = 0; i < ( int )m_vecLayoutControl.size(); i++ )
	{
		if( m_vecLayoutControl[ i ].emSelect == SELECT_LAST )
		{
			nMainIndex = i;
			pMainControl = m_vecLayoutControl[ i ].pControl;
			break;
		}
	}

	int nNumSelectedDone = 0;
	for( i = 0; i < ( int )m_vecLayoutControl.size(); i++ )
	{
		if( m_vecLayoutControl[ i ].emSelect == SELECT_DONE )
			++nNumSelectedDone;
	}

	pMainUIProperty = &m_vecLayoutControl[ nMainIndex ].Property;
	for( i = 0; i < ( int )m_vecLayoutControl.size(); i++ )
	{
		if( m_vecLayoutControl[ i ].emSelect == SELECT_DONE )
		{
			// 템플릿 종류가 같은지 확인.
			if( m_vecLayoutControl[ i ].Property.UIType == pMainUIProperty->UIType )
			{
				pUIProperty = &m_vecLayoutControl[ i ].Property;
				pUIProperty->nHotKey = pMainUIProperty->nHotKey;
				pUIProperty->UICoord = pMainUIProperty->UICoord;
				strcpy_s( pUIProperty->szUIName, 32, pMainUIProperty->szUIName );
				pUIProperty->nCustomControlID = pMainUIProperty->nCustomControlID;
				pUIProperty->nTooltipStringIndex = pMainUIProperty->nTooltipStringIndex;

				// 선택되어있는게, LAST와 DONE 단 두개라면, 디폴트 컨트롤을 교체.
				if( nNumSelectedDone == 1 && pMainUIProperty->bDefaultControl )
				{
					pUIProperty->bDefaultControl = TRUE;
					pMainUIProperty->bDefaultControl = FALSE;
				}

				switch( pMainUIProperty->UIType )
				{
				case UI_CONTROL_STATIC:
					pUIProperty->StaticProperty = pMainUIProperty->StaticProperty;
					break;
				case UI_CONTROL_SLIDER:
					pUIProperty->SliderProperty = pMainUIProperty->SliderProperty;
					break;
				case UI_CONTROL_TEXTURECONTROL:
					pUIProperty->TextureControlProperty = pMainUIProperty->TextureControlProperty;
					break;
				case UI_CONTROL_BUTTON:
					pUIProperty->ButtonProperty = pMainUIProperty->ButtonProperty;
					break;
				case UI_CONTROL_CHECKBOX:
					pUIProperty->CheckBoxProperty = pMainUIProperty->CheckBoxProperty;
					break;
				case UI_CONTROL_RADIOBUTTON:		
					pUIProperty->RadioButtonProperty = pMainUIProperty->RadioButtonProperty;
					break;
				case UI_CONTROL_EDITBOX:			
					pUIProperty->EditBoxProperty = pMainUIProperty->EditBoxProperty;	
					break;
				case UI_CONTROL_IMEEDITBOX:			
					pUIProperty->IMEEditBoxProperty = pMainUIProperty->IMEEditBoxProperty;
					break;
				case UI_CONTROL_COMBOBOX:			
					pUIProperty->ComboBoxProperty = pMainUIProperty->ComboBoxProperty;
					break;
				case UI_CONTROL_LISTBOX:			
					pUIProperty->ListBoxProperty = pMainUIProperty->ListBoxProperty;
					break;
				case UI_CONTROL_PROGRESSBAR:		
					pUIProperty->ProgressBarProperty = pMainUIProperty->ProgressBarProperty;
					break;
				case UI_CONTROL_CUSTOM:
					pUIProperty->CustomProperty = pMainUIProperty->CustomProperty;
					break;
				case UI_CONTROL_TEXTBOX:			
					pUIProperty->TextBoxProperty = pMainUIProperty->TextBoxProperty;
					break;
				case UI_CONTROL_HTMLTEXTBOX:
					{
						// Note : 상위 클래스인 TextBox에 속성을 넣어줘야 한다.
						//
						pUIProperty->TextBoxProperty.bVerticalScrollBar = pMainUIProperty->HtmlTextBoxProperty.bVerticalScrollBar;
						pUIProperty->TextBoxProperty.nScrollBarTemplate = pMainUIProperty->HtmlTextBoxProperty.nScrollBarTemplate;
						pUIProperty->TextBoxProperty.fScrollBarSize = pMainUIProperty->HtmlTextBoxProperty.fScrollBarSize;
						pUIProperty->TextBoxProperty.bLeftScrollBar = pMainUIProperty->HtmlTextBoxProperty.bLeftScrollBar;
						pUIProperty->TextBoxProperty.fLineSpace = pMainUIProperty->HtmlTextBoxProperty.fLineSpace;
						pUIProperty->TextBoxProperty.bRollOver = pMainUIProperty->HtmlTextBoxProperty.bRollOver;
					}
					pUIProperty->HtmlTextBoxProperty = pMainUIProperty->HtmlTextBoxProperty;
					break;
				case UI_CONTROL_TREECONTROL:		
					pUIProperty->TreeControlProperty = pMainUIProperty->TreeControlProperty;
					break;
				case UI_CONTROL_SCROLLBAR:			
					pUIProperty->ScrollBarProperty = pMainUIProperty->ScrollBarProperty;
					break;
				case UI_CONTROL_QUESTTREECONTROL:	
					pUIProperty->QuestTreeControlProperty = pMainUIProperty->QuestTreeControlProperty;
					break;
				case UI_CONTROL_ANIMATION:			
					pUIProperty->AnimationProperty = pMainUIProperty->AnimationProperty;
					break;
				case UI_CONTROL_LINE_EDITBOX:
					pUIProperty->LineEditBoxProperty = pMainUIProperty->LineEditBoxProperty;
					break;
				case UI_CONTROL_LINE_IMEEDITBOX:
					pUIProperty->LineIMEEditBoxProperty = pMainUIProperty->LineIMEEditBoxProperty;
				case UI_CONTROL_MOVIECONTROL:
					pUIProperty->MovieControlPropery = pMainUIProperty->MovieControlPropery;
					break;
				case UI_CONTROL_LISTBOXEX:
					pUIProperty->ListBoxExProperty = pMainUIProperty->ListBoxExProperty;
					break;
				default: 
					ASSERT(0&&"LayouView::OnFormatMakesameproperty, Invalid Control Type!");
					CDebugSet::ToLogFile( "LayouView::OnFormatMakesameproperty, Invalid Control Type!" );
				}
			}

			m_vecLayoutControl[ i ].pControl->SetUICoord( m_vecLayoutControl[ i ].Property.UICoord );
		}
	}
	ReinitAllControl();
}

void CLayoutView::OnUpdateFormatMakesameproperty(CCmdUI *pCmdUI)
{
	pCmdUI->Enable( CheckMultiSelectedCount() );
}

void CLayoutView::LoadNotUseTemplateList()
{
	m_vecNotUseTemplate.clear();

	char szFileName[ _MAX_PATH ];
	FILE *fp;
	char szString[_MAX_PATH]={0};

	strcpy_s( szFileName, _MAX_PATH, CMainFrame::GetUITemplateFolder() );
	strcat_s( szFileName, _MAX_PATH, g_szNotUseTemplateListFile );
	fopen_s( &fp, szFileName, "r" );
	if( fp )
	{
		while( fgets( szString, _MAX_PATH, fp ) )
		{
			if( szString[strlen(szString)-1] == '\n' ) szString[strlen(szString)-1] = '\0';
			m_vecNotUseTemplate.push_back( szString );
		}
		fclose( fp );
	}
}

bool CLayoutView::IsNotUseTemplate( CEtUITemplate *pTemplate )
{
	for( int i = 0; i < (int)m_vecNotUseTemplate.size(); ++i )
	{
		if( _stricmp( pTemplate->m_szTemplateName.c_str(), m_vecNotUseTemplate[i].c_str() ) == 0 )
		{
			return true;
		}
	}
	return false;
}
void CLayoutView::OnDelNotusetemplate()
{
	// 사용하지 않는 템플릿으로 만들어진 컨트롤들을 제거한다.
	std::string szDelControls;

	// 우선 선택한거 다 풀고,
	int nSize = GetControlCount();
	for( int i = 0; i < nSize; i++ )
		m_vecLayoutControl[ i ].emSelect = SELECT_NONE;

	for( int i = 0; i < m_LayoutDlg.GetControlCount(); ++i ) {
		CEtUIControl *pControl = m_vecLayoutControl[i].pControl;
		if( pControl ) {
			if( IsNotUseTemplate( &pControl->GetTemplate() ) ) {
				m_vecLayoutControl[i].emSelect = SELECT_DONE;
				szDelControls += pControl->GetControlName();
				szDelControls += "컨트롤이 삭제되었습니다.\n";
			}
		}
	}

	DeleteSelectedControl();

	if( !szDelControls.empty() ) {
		std::string szMsg;
		szMsg += "[";
		szMsg += m_szCurFileName;
		szMsg += "]\n";
		szMsg += szDelControls;
		MessageBox( szMsg.c_str(), "안쓰는 템플릿으로 만들어진 컨트롤 삭제" );
	}
}

void CLayoutView::MakeGroupSelectedControl( int nGroupIndex )
{
	if( nGroupIndex < 0 || nGroupIndex >= NUM_CONTROL_GROUP ) return;
	m_vecControlGroup[nGroupIndex].clear();

	int nSize = GetControlCount();
	for( int i = 0; i < nSize; i++ )
	{
		if( m_vecLayoutControl[ i ].emSelect )
		{
			m_vecControlGroup[nGroupIndex].push_back( m_vecLayoutControl[ i ].pControl->GetControlName() );
		}
	}
	CEtUIToolView *pView(NULL);
	pView = ( CEtUIToolView * )CGlobalValue::GetInstance().m_pCurView;
	pView->InvalidateRect( NULL, TRUE );
}

void CLayoutView::SelectControlGroup( int nGroupIndex )
{
	if( nGroupIndex < 0 || nGroupIndex >= NUM_CONTROL_GROUP ) return;
	if( m_vecControlGroup[nGroupIndex].empty() ) return;
	SelectAllControl( false );

	bool bFirst = true;
	int nSize = GetControlCount();
	for( int i = 0; i < nSize; i++ )
	{
		if( IsGroupControl( nGroupIndex, m_vecLayoutControl[ i ].pControl ) )
		{
			if( bFirst )
			{
				m_vecLayoutControl[ i ].emSelect = SELECT_LAST;
				bFirst = false;
			}
			else
			{
				m_vecLayoutControl[ i ].emSelect = SELECT_DONE;
			}
		}
	}

	DrawSelectControl();

	m_nLastSelectGroup = nGroupIndex;
}

bool CLayoutView::IsGroupControl( int nGroupIndex, CEtUIControl *pControl )
{
	if( nGroupIndex < 0 || nGroupIndex >= NUM_CONTROL_GROUP ) return false;
	if( pControl == NULL ) return false;
	std::string szName = pControl->GetControlName();

	int nSize = (int)m_vecControlGroup[nGroupIndex].size();
	for( int i = 0; i < nSize; i++ )
	{
		if( m_vecControlGroup[nGroupIndex][i] == szName )
			return true;
	}
	return false;
}

void CLayoutView::DrawControlGroupInfo( CDC* pDC, int nX, int nY )
{
	// Direct 렌더링 영역 밖에 그리는거라 DC로 그린다.
	CRect TextRect;
	CString strGroup;
	for( int i = 0; i < NUM_CONTROL_GROUP; ++i )
	{
		if( m_vecControlGroup[i].empty() ) continue;

		TextRect.left = nX;
		TextRect.top = nY;
		TextRect.left += 10;
		TextRect.top += (16 * i);
		TextRect.right = TextRect.left + 16;
		TextRect.bottom = TextRect.top + 16;
		strGroup.Format( "%d", i+1 );
		pDC->DrawText( strGroup, &TextRect, 0 );
	}
}