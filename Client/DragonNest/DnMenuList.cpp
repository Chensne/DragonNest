#include "StdAfx.h"
#include "DnMenuListItem.h"
#include "DnMenuList.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnMenuList::CDnMenuList( CEtUIDialog * pParent )
: CEtUIControl( pParent )
//,m_ScrollBar( pParent )
,m_pSelectedMainItem(NULL)
,m_pSelectedSubItem(NULL)
,m_pSelectedItem(NULL)
,m_pOverItem(NULL)
,m_topMainItemIndex(0)
,m_bAutoSelectSubItem(false)
,m_bShowScrollBtn(false)
{

}

CDnMenuList::~CDnMenuList()
{
	DeleteAllItems();
}


void CDnMenuList::Initialize( SUIControlProperty *pProperty )
{
	CEtUIControl::Initialize( pProperty );
	
	//m_fIndet = 20.0f / (float)DEFAULT_UI_SCREEN_WIDTH;

	m_BtnPressOffset.x = 3.0f / (float)DEFAULT_UI_SCREEN_WIDTH;
	m_BtnPressOffset.y = 2.0f / (float)DEFAULT_UI_SCREEN_HEIGHT;
		

	//m_fIndet = 20 * 
	SUIElement * pElement = NULL;

	// Button Up.
	pElement = GetElement( 6 ); // Normal.
	ScorllButton & btnUp = m_ScrollBtn[ 0 ];
	btnUp.m_BtnCoord.fWidth = pElement->fTextureWidth;
	btnUp.m_BtnCoord.fHeight = pElement->fTextureHeight;
	btnUp.m_BtnCoord.fX = pProperty->UICoord.fX;
	btnUp.m_BtnCoord.fY = pProperty->UICoord.fY;

	pElement = GetElement( 7 ); // Over.
	btnUp.m_BtnOverCoord.fWidth = pElement->fTextureWidth;
	btnUp.m_BtnOverCoord.fHeight = pElement->fTextureHeight;
	btnUp.m_BtnOverCoord.fX = btnUp.m_BtnCoord.fX;
	btnUp.m_BtnOverCoord.fY = btnUp.m_BtnCoord.fY;

	// Test - 6의 TextureHeight 사이즈가 이상하다. 실제이미지상으론같은데 템플릿크기에서는 다름.
	//btnUp.m_BtnCoord.fHeight = btnUp.m_BtnOverCoord.fHeight; // 우선 강제로 같게함.

	// Button Down.	
	pElement = GetElement( 8 ); // Normal.
	ScorllButton & btnDown = m_ScrollBtn[ 1 ];
	btnDown.m_BtnCoord.fWidth = pElement->fTextureWidth;
	btnDown.m_BtnCoord.fHeight = pElement->fTextureHeight;
	btnDown.m_BtnCoord.fX = btnUp.m_BtnCoord.fX;
	btnDown.m_BtnCoord.fY = pProperty->UICoord.Bottom() - btnDown.m_BtnCoord.fHeight;

	pElement = GetElement( 9 ); // Over.
	btnDown.m_BtnOverCoord.fWidth = pElement->fTextureWidth;
	btnDown.m_BtnOverCoord.fHeight = pElement->fTextureHeight;
	btnDown.m_BtnOverCoord.fX = btnDown.m_BtnCoord.fX;
	btnDown.m_BtnOverCoord.fY = btnDown.m_BtnCoord.fY;
	

	// MenuItem이 보이는 영역.
	m_ViewArea.fX = pProperty->UICoord.fX;
	m_ViewArea.fY = btnUp.m_BtnCoord.Bottom() + 0.001f;
	m_ViewArea.fWidth = pProperty->UICoord.fWidth;
	//m_ViewArea.fHeight = pProperty->UICoord.fHeight - ( btnUp.m_BtnCoord.fHeight + btnDown.m_BtnCoord.fHeight );
	m_ViewArea.fHeight = pProperty->UICoord.Bottom() - ( btnUp.m_BtnCoord.fHeight + btnDown.m_BtnCoord.fHeight ) - 0.001f;

}

bool CDnMenuList::HandleMouse( UINT uMsg, float fX, float fY, WPARAM wParam, LPARAM lParam )
{
	if( !IsEnable() || !IsShow() )
	{
		return false;
	}
	
	if( ( WM_LBUTTONDOWN == uMsg ) && ( !m_bFocus ) )
	{
		m_pParent->RequestFocus( this );
	}

	switch( uMsg )
	{
	case WM_MOUSEMOVE:
		{
			if( IsInside( fX, fY ) )
			{
				if( OverItem( fX, fY ) )
					return true;
				else
				{
					CEtUIDialog::ShowTooltipDlg( NULL, false );
					if( m_pOverItem )
						m_pOverItem->SetOver( false );						
				}
			}
			else
			{
				if( m_pOverItem )
					m_pOverItem->SetOver( false );
			}

		}
		break;
	case WM_LBUTTONDOWN:
	//case WM_LBUTTONDBLCLK:
		{
			if( IsInside( fX, fY ) )
			{
				m_bPressed = true;
				SetCapture( m_pParent->GetHWnd() );

				if( !m_bFocus )
				{
					m_pParent->RequestFocus( this );
				}

				if( SelectButton( fX, fY ) )
				{
					//...
				}

				else
				{
					PressItem( fX, fY );
				}
			

				return true;
			}
		}
		break;
	case WM_LBUTTONUP:
		{
			m_ScrollBtn[0].bSelect = m_ScrollBtn[1].bSelect = false;
			
			if( m_bPressed )
			{
				m_bPressed = false;
				ReleaseCapture();

				if( IsInside( fX, fY ) )
				{								
					if( SelectItem( fX, fY ) )
					{
						m_pParent->ProcessCommand( EVENT_MENULIST_SELECTION, true, this );
					}					
				}

				return true;
			}
		}
		break;
	//case WM_MOUSEWHEEL:
	//	{
	//		UINT uLines;
	//		SystemParametersInfo( SPI_GETWHEELSCROLLLINES, 0, &uLines, 0 );
	//		int nScrollAmount = int( ( short )HIWORD( wParam ) ) / WHEEL_DELTA * uLines;
	//		int nScrollPos = Scroll( -nScrollAmount );
	//		m_ScrollBar.ShowItem( nScrollPos );

	//		ClearTooltipText();
	//		CEtUIDialog::ShowTooltipDlg( NULL, false );

	//		return true;
	//	}
	//	break;
	}

	return false;
}

void CDnMenuList::Render( float fElapsedTime )
{
	if( IsShow() == false )
		return;

	int elementIdx = 0;
	CDnMenuListItem * pItem = NULL;
	CDnMenuListItem * pSubItem = NULL;
	int size = (int)m_vMainMenu.size();
	for( int i=0; i<size; ++i )
	{
		pItem = m_vMainMenu[ i ];		

		// MainItem.
		if( pItem->IsShow() && pItem->IsGetOutArea() == false )
		{
			elementIdx = (int)pItem->GetType() * 3 + 1;
			RenderBase( pItem,   elementIdx );
			RenderSelect( pItem, elementIdx+1 );
			RenderOver( pItem,   elementIdx+2 );			
			RenderText( pItem,   elementIdx );
		}

		// SubItems.	
		if( pItem->IsSelected() )
		{
			std::vector<CDnMenuListItem *> & rItems = pItem->GetSubItems();
			int cnt = (int)rItems.size();
			for( int k=0; k<cnt; ++k )
			{
				pSubItem = rItems[ k ];
				if( pSubItem->IsShow() && pSubItem->IsGetOutArea() == false  )
				{
					elementIdx = (int)pSubItem->GetType() * 3 + 1;
					RenderBase( pSubItem,   elementIdx );
					RenderSelect( pSubItem, elementIdx+1 );
					RenderOver( pSubItem,   elementIdx+2 );				
					//RenderOver( pSubItem,   elementIdx+1 ); // Test
					RenderText( pSubItem,   elementIdx );
				}
			}
		}
		
	}

	// Scroll Button.
	RenderButton();

	//if( m_pOverItem && m_pOverItem->IsWithEllipsis() )
	//	CEtUIDialog::ShowTooltipDlg( this, true, m_pOverItem->GetTextOrg() );				
}

void CDnMenuList::RenderBase( CDnMenuListItem *pItem, int eleIdx )
{
	if( pItem->IsOver() || pItem->IsSelected() )
		return;

	SUIElement * pElement = GetElement( eleIdx );
	if( pElement )
	{
		if( m_bExistTemplateTexture )
			m_pParent->DrawSprite( m_Template.m_hTemplateTexture, pElement->TemplateUVCoord, 0xffffffff, pItem->m_BaseCoord );
		else
			m_pParent->DrawSprite( pElement->UVCoord, 0xffffffff, pItem->m_BaseCoord );
	}
}

void CDnMenuList::RenderSelect( CDnMenuListItem *pItem, int eleIdx )
{
	if( !pItem->IsSelected() )
		return;

	SUIElement * pElement = GetElement( eleIdx );
	if( pElement )
	{
		if( m_bExistTemplateTexture )
			m_pParent->DrawSprite( m_Template.m_hTemplateTexture, pElement->TemplateUVCoord, 0xffffffff, pItem->m_SelectCoord );
		else
			m_pParent->DrawSprite( pElement->UVCoord, 0xffffffff, pItem->m_SelectCoord );
	}
}

void CDnMenuList::RenderOver( CDnMenuListItem *pItem, int eleIdx )
{
	if( !pItem->IsOver() || pItem->IsSelected() )
		return;

	SUIElement * pElement = GetElement( eleIdx );
	if( pElement )
	{		
		SUICoord uiCoord = pItem->m_OverCoord;

		if( pItem->IsPress() )
		{
			uiCoord.fX = pItem->m_OverCoord.fX + m_BtnPressOffset.x;
			uiCoord.fY = pItem->m_OverCoord.fY + m_BtnPressOffset.y;
		}

		if( m_bExistTemplateTexture )
			m_pParent->DrawSprite( m_Template.m_hTemplateTexture, pElement->TemplateUVCoord, 0xffffffff, uiCoord );
		else
			m_pParent->DrawSprite( pElement->UVCoord, 0xffffffff, uiCoord );
	}
}


void CDnMenuList::RenderText( CDnMenuListItem *pItem, int eleIdx )
{
	//if( !pItem->IsOpened() )
	//	return;

	SUIElement * pElement = GetElement( eleIdx );
	if( pElement )
	{		
		SUICoord coord;
		if( pItem->IsSelected() )
			coord = pItem->m_strSelectCoord;
		else if( pItem->IsOver() )
			coord = pItem->m_strOverCoord;
		else
			//coord = pItem->m_strCoord;
			coord = pItem->m_strBaseCoord;

		if( !pItem->IsSelected() && pItem->IsPress() )
		{
			coord.fX += m_BtnPressOffset.x;
			coord.fY += m_BtnPressOffset.y;
		}

		m_pParent->DrawDlgText( pItem->GetText().c_str(), pElement, pItem->m_TextColor, coord, -1, -1, true );		
	}
	//m_pParent->DrawDlgText( pItem->GetText(), pElement, pItem->GetTextColor(), pItem->m_strCoord, -1, -1, true );
	//m_pParent->DrawRect( pItem->m_strCoord, debug::YELLOW );
}

void CDnMenuList::RenderButton()
{	
	if( m_bShowScrollBtn == false )
		return;

	SUIElement *pElement = NULL;
	
	SUICoord coord;

	// Up.
	ScorllButton & btnUp = m_ScrollBtn[ 0 ];
	if( btnUp.bOver )
		pElement = GetElement( 8 );
	else
		pElement = GetElement( 7 );

	if( pElement )
	{
		if( btnUp.bOver )
		{
			coord = btnUp.m_BtnOverCoord;
			if( btnUp.bSelect )
			{
				coord.fX += m_BtnPressOffset.x;
				coord.fY += m_BtnPressOffset.y;
			}
		}
		else if( btnUp.bSelect )
		{
			coord = btnUp.m_BtnCoord;
			coord.fX += m_BtnPressOffset.x;
			coord.fY += m_BtnPressOffset.y;
		}
		else
			coord = btnUp.m_BtnCoord;

		if( m_bExistTemplateTexture )
			m_pParent->DrawSprite( m_Template.m_hTemplateTexture, pElement->TemplateUVCoord, 0xffffffff, coord );
		else
			m_pParent->DrawSprite( pElement->UVCoord, 0xffffffff, coord );
	}

	// Down.
	ScorllButton & btnDown = m_ScrollBtn[ 1 ];
	if( btnDown.bOver )
		pElement = GetElement( 10 );
	else
		pElement = GetElement( 9 );

	if( pElement )
	{
		if( btnDown.bOver )
		{
			coord = btnDown.m_BtnOverCoord;
			if( btnDown.bSelect )
			{
				coord.fX += m_BtnPressOffset.x;
				coord.fY += m_BtnPressOffset.y;
			}
		}
		else if( btnDown.bSelect )
		{
			coord = btnDown.m_BtnCoord;
			coord.fX += m_BtnPressOffset.x;
			coord.fY += m_BtnPressOffset.y;
		}
		else
			coord = btnDown.m_BtnCoord;

		if( m_bExistTemplateTexture )
			m_pParent->DrawSprite( m_Template.m_hTemplateTexture, pElement->TemplateUVCoord, 0xffffffff, coord );
		else
			m_pParent->DrawSprite( pElement->UVCoord, 0xffffffff, coord );
			//m_pParent->DrawSprite( pElement->UVCoord, 0xffffffff, ( btnDown.bOver == true ? btnDown.m_BtnOverCoord : btnDown.m_BtnCoord ) );
	}

}

CDnMenuListItem * CDnMenuList::AddItem( LPCWSTR szText, DWORD dwColor  )
{	
	// Item 생성.
	CDnMenuListItem * pItem = new CDnMenuListItem();
	pItem->Initialize( this, CDnMenuListItem::EITEMTYPE::EITEMTYPE_MAIN, IsShow() );
	
	pItem->SetText( szText, dwColor );
	
	if( !m_vMainMenu.empty() )
		pItem->SetUICoordY( m_vMainMenu.back()->m_uiCoord.Bottom() );

	// 메인메뉴 배열에 추가.
	m_vMainMenu.push_back( pItem );

	UpdatePosition();

	return pItem;
}


CDnMenuListItem * CDnMenuList::AddChildItem( CDnMenuListItem * pMenuItem, LPCWSTR szText, DWORD dwColor )
{	
	// Item 생성.
	CDnMenuListItem * pItem = new CDnMenuListItem();
	pItem->Initialize( this, CDnMenuListItem::EITEMTYPE::EITEMTYPE_SUB, false );	
	pItem->SetText( szText, dwColor );

	pItem->SetParentItem( pMenuItem );
	pMenuItem->AddItem( pItem );	

	UpdatePosition();

	return pItem;
}


bool CDnMenuList::DeleteItem( CDnMenuListItem * pMenuItem )
{
	if( pMenuItem == NULL )
		return false;

	std::vector< CDnMenuListItem * >::iterator it;
	CDnMenuListItem * pItem = NULL;
	CDnMenuListItem * pMain = NULL;
	int size = (int)m_vMainMenu.size();
	
	// Main은 하위자식들까지 제거.
	if( pMenuItem->GetType() == CDnMenuListItem::EITEMTYPE::EITEMTYPE_MAIN )
	{
		if( m_pSelectedMainItem == pMenuItem )
			Reset();

		for( it = m_vMainMenu.begin(); it!=m_vMainMenu.end(); it ++ )
		{
			pItem = (*it);
			if( pItem == pMenuItem )
			{
				pItem->DeleteSubItem();
				m_vMainMenu.erase( it );
				delete pItem;

				UpdatePosition(); // Item위치갱신.
				return true;
			}
		}
	}

	// Sub.
	else
	{
		CDnMenuListItem * pSubItem = NULL;
		for( int i=0; i<size; ++ i )
		{
			pItem = m_vMainMenu[ i ];			
			std::vector<CDnMenuListItem *> & rItems = pItem->GetSubItems();
			int cnt = (int)rItems.size();
			for( int k=0; k<cnt; ++k )
			{
				pSubItem = rItems[ k ];
				if( pMenuItem = pSubItem )
				{
					if( pSubItem == m_pSelectedSubItem )
					{
						m_pSelectedSubItem = m_pSelectedItem = NULL;
					}
					delete pSubItem;

					UpdatePosition(); // Item위치갱신.
					return true;
				}
			}
		}	
	}

	return false;
}


void CDnMenuList::DeleteAllItems()
{
	Reset();

	if( !m_vMainMenu.empty() )
	{
		int size = (int)m_vMainMenu.size();
		for( int i=0; i<size; ++ i )
			SAFE_DELETE( m_vMainMenu[ i ] );
		m_vMainMenu.clear();
	}
}

bool CDnMenuList::SelectItem( float fX, float fY )
{
	bool bSelected = false;
	CDnMenuListItem * pItem = NULL;
	CDnMenuListItem * pSelectedItem = NULL;
	int size = (int)m_vMainMenu.size();
	for( int i=0; i<size; ++i )
	{
		pItem = m_vMainMenu[ i ];		
		pSelectedItem = pItem->GetClickItem( fX, fY );

		// ~~~~
		if( pSelectedItem && ( pSelectedItem->IsGetOutArea() || pSelectedItem->IsShow() == false ) )
			continue;
		
		if( pSelectedItem && m_pSelectedMainItem != pSelectedItem && !pSelectedItem->IsSelected() )
		{	
			pSelectedItem->SetPress( false );

			bSelected = true;

			// Main.
			if( pSelectedItem->GetType() == CDnMenuListItem::EITEMTYPE::EITEMTYPE_MAIN )
			{
				// 기존선택해제.
				if( m_pSelectedMainItem )
				{
					m_pSelectedMainItem->SetPress( false );
					m_pSelectedMainItem->SetSelect( false );
					m_pSelectedMainItem->ShowItems( false ); // 자식Item들 hide.
				}
				// 기존선택해제 - Sub.
				if( m_pSelectedSubItem )
				{
					m_pSelectedSubItem->SetPress( false );
					m_pSelectedSubItem->SetSelect( false );
				}

				m_pSelectedMainItem = m_pSelectedItem = pSelectedItem;
				m_pSelectedMainItem->SetSelect( true );
				m_pSelectedMainItem->ShowItems( true ); // 자식Item들 Show.

				// MainItem 선택시 첫번째 SubItem 선택.
				if( m_bAutoSelectSubItem )
				{
					CDnMenuListItem * pFrontSubItem = m_pSelectedMainItem->GetFrontSubItem();
					if( pFrontSubItem )
					{
						//m_pSelectedSubItem = m_pSelectedItem = pFrontSubItem;
						m_pSelectedSubItem = pFrontSubItem;
						m_pSelectedSubItem->SetSelect( true );
					}
				}
			}

			// Sub.
			else if( pSelectedItem->GetType() == CDnMenuListItem::EITEMTYPE::EITEMTYPE_SUB )
			{
				// 기존선택해제.
				if( m_pSelectedSubItem )
				{
					m_pSelectedSubItem->SetPress( false );
					m_pSelectedSubItem->SetSelect( false );
				}

				m_pSelectedMainItem = pSelectedItem->GetParentItem();

				m_pSelectedSubItem = m_pSelectedItem = pSelectedItem;
				m_pSelectedSubItem->SetSelect( true );
			}
			
		}
		
	}


	// Item들의 위치조정.
	if( bSelected )
	{
		UpdatePosition();
	}

	return bSelected;
}


bool CDnMenuList::OverItem( float fX, float fY )
{
	bool bOver = false;

	// Button //
	m_ScrollBtn[0].bOver = false;
	m_ScrollBtn[1].bOver = false;

	// Up.
	if( m_ScrollBtn[0].m_BtnCoord.IsInside( fX, fY ) )
	{
		m_ScrollBtn[0].bOver = true;
		bOver = true;
	}
	else
	{
		m_ScrollBtn[0].bOver = false;
	}

	// Down.
	if( m_ScrollBtn[1].m_BtnCoord.IsInside( fX, fY ) )
	{
		m_ScrollBtn[1].bOver = true;
		bOver = true;
	}
	else
	{
		m_ScrollBtn[1].bOver = false;
	}
	
	if( bOver )
	{
		if( m_pOverItem )
			m_pOverItem->SetOver( false );

		return bOver;
	}
	

	// Items //
	CDnMenuListItem * pItem = NULL;
	CDnMenuListItem * pSelectedItem = NULL;
	int size = (int)m_vMainMenu.size();
	for( int i=0; i<size; ++i )
	{
		pItem = m_vMainMenu[ i ];
		pSelectedItem = pItem->GetClickItem( fX, fY );

		if( pSelectedItem )
		{
			if( m_pOverItem == pSelectedItem )
				bOver = true;

			else if( !pSelectedItem->IsOver() && m_pOverItem != pSelectedItem )
			{	
				bOver = true;

				if( m_pOverItem )
					m_pOverItem->SetOver( false );

				m_pOverItem = pSelectedItem;
				m_pOverItem->SetOver( true );			
			}			
		}
	}

	if( m_pOverItem && m_pOverItem->IsWithEllipsis() )
		CEtUIDialog::ShowTooltipDlg( this, true, m_pOverItem->GetTextOrg() );

	return bOver;
}


void CDnMenuList::PressItem( float fX, float fY )
{
	// Items //
	CDnMenuListItem * pItem = NULL;
	CDnMenuListItem * pSelectedItem = NULL;
	int size = (int)m_vMainMenu.size();
	for( int i=0; i<size; ++i )
	{
		pItem = m_vMainMenu[ i ];
		pSelectedItem = pItem->GetClickItem( fX, fY );

		if( pSelectedItem ) // && pSelectedItem->IsOver() )
		{	
			pSelectedItem->SetPress( true );
		}
	}
}


bool CDnMenuList::SelectButton( float fX, float fY )
{
	bool bSelect = false;

	// Button Up.
	if( m_ScrollBtn[0].m_BtnCoord.IsInside( fX, fY ) )
	{
		if( m_topMainItemIndex > 0 )
		{
			--m_topMainItemIndex;
			Scroll( 1 );
		}
		
		m_ScrollBtn[0].bSelect = true;
		bSelect = true;
	}

	// Button Down.
	if( m_ScrollBtn[1].m_BtnCoord.IsInside( fX, fY ) )
	{
		m_ScrollBtn[1].bSelect = true;

		bool bGetOut = false;		
		int size = (int)m_vMainMenu.size();
		for( int i=0; i<size; ++i )
		{
			if( m_vMainMenu[ i ]->m_uiCoord.Bottom() <= GetProperty()->UICoord.Bottom() )
				continue;

			if( m_vMainMenu[ i ]->IsGetOutArea() )
			{
				bGetOut = true;
				break;
			}
			
			if( m_vMainMenu[ i ]->IsSelected() )
			{
				std::vector<CDnMenuListItem *> & vSubItems = m_vMainMenu[ i ]->GetSubItems();
				int cnt = (int)vSubItems.size();
				for( int k=0; k<cnt; ++k )
				{
					if( vSubItems[ k ]->IsGetOutArea() )
					{
						bGetOut = true;
						break;
					}
				}
			}
		}

		if( bGetOut )
		{
			++m_topMainItemIndex;
			
			// 위로한칸 올림.
			Scroll( -1 );
		}

		bSelect = true;
	}

	return bSelect;
}


bool CDnMenuList::OverButton( float fX, float fY )
{

	return false;
}

void CDnMenuList::UpdatePosition()
{
	CDnMenuListItem * pItem = NULL;
	CDnMenuListItem * pItemBefore = NULL;
	CDnMenuListItem * pSubItem = NULL;

	bool bOut = false;

	int size = (int)m_vMainMenu.size();
	for( int i=0; i<size; ++i )
	{
		pItem = m_vMainMenu[ i ];		

		// MainItem 의 Y값 계산.
		if( i == 0 )
		{			
			pItem->SetUICoordX( GetProperty()->UICoord.fX );
			//pItem->SetUICoordY( m_ScrollBtn[0].m_BtnCoord.Bottom() );
						
			if( m_topMainItemIndex == 0 )
				pItem->SetUICoordY( m_ScrollBtn[0].m_BtnCoord.Bottom() );
		}
		else if( i > 0 )
		{
			pItemBefore = m_vMainMenu[ i-1 ];

			if( pItemBefore->IsSelected() )
			{
				pSubItem = pItemBefore->GetEndSubItem();				
				if( pSubItem )
					pItem->SetUICoordY( pSubItem->m_uiCoord.Bottom() );
				else
					pItem->SetUICoordY( pItemBefore->m_uiCoord.Bottom() );
			}
			else
			{				
				pItem->SetUICoordY( pItemBefore->m_uiCoord.Bottom() );
			}
		}

		// 영역을 벗어낫는가?
		if( m_ViewArea.IsInside( pItem->m_uiCoord.fX, pItem->m_uiCoord.Bottom() ) )
			pItem->SetGetOutArea( false );
		else
		{
			pItem->SetGetOutArea( true );
			bOut = true;
		}


		// 선택되어 있다면 자식아이템들위치조정.
		if( pItem->IsSelected() )
		{
			float parentBottom = pItem->m_uiCoord.Bottom();

			std::vector<CDnMenuListItem *> & rItems = pItem->GetSubItems();
			int cnt = (int)rItems.size();
			for( int k=0; k<cnt; ++k )
			{
				pSubItem = rItems[ k ];
				pSubItem->SetUICoordY( parentBottom + ( pSubItem->m_uiCoord.fHeight * k ) );

				// 영역을 벗어낫는가?
				if( m_ViewArea.IsInside( pSubItem->m_uiCoord.fX, pSubItem->m_uiCoord.Bottom() ) )
					pSubItem->SetGetOutArea( false );
				else
				{
					pSubItem->SetGetOutArea( true );
					bOut = true;
				}
			}
		}

	}

	m_bShowScrollBtn = bOut; // 스크롤버튼 출력여부.

}


void CDnMenuList::Show( bool bShow )
{
	CDnMenuListItem * pItem = NULL;	

	int cnt = 0;
	int size = (int)m_vMainMenu.size();
	for( int i=0; i<size; ++i )
	{
		pItem = m_vMainMenu[ i ];
		pItem->Show( bShow );

		// 선택되어 있다면 자식아이템들위치조정.
		if( pItem->IsSelected() )
		{	
			std::vector<CDnMenuListItem *> & rItems = pItem->GetSubItems();
			cnt = (int)rItems.size();
			for( int k=0; k<cnt; ++k )
				rItems[ k ]->Show( bShow );				
		}
	}
}


void CDnMenuList::OnChangeResolution()
{
	float w = m_pParent->GetScreenWidth();	
	m_BtnPressOffset.x = 3.0f / w;
	m_BtnPressOffset.y = 2.0f / m_pParent->GetScreenHeight();
}


void CDnMenuList::Scroll( int nScroll )
{
	CDnMenuListItem * pItem = NULL;

	int size = (int)m_vMainMenu.size();
	for( int i=0; i<size; ++i )
	{
		pItem = m_vMainMenu[ i ];
		pItem->SetUICoordY( pItem->GetCoord().fY + ( pItem->GetCoord().fHeight * nScroll ) );		
		
		// 영역을 벗어낫는가?
		if( m_ViewArea.IsInside( pItem->m_uiCoord.fX, pItem->m_uiCoord.Bottom() ) )
			pItem->SetGetOutArea( false );
		else
			pItem->SetGetOutArea( true );

		if( pItem->IsSelected() )
		{
			float subY = pItem->m_uiCoord.Bottom();

			std::vector<CDnMenuListItem *> & vSubItems = pItem->GetSubItems();
			int cnt = (int)vSubItems.size();
			for( int k=0; k<cnt; ++k )
			{
				pItem = vSubItems[ k ];
				pItem->SetUICoordY( subY + (pItem->GetCoord().fHeight * k) ); //pItem->GetCoord().fY + ( pItem->GetCoord().fHeight * nScroll ) );

				// 영역을 벗어낫는가?
				if( m_ViewArea.IsInside( pItem->m_uiCoord.fX, pItem->m_uiCoord.Bottom() ) )
					pItem->SetGetOutArea( false );
				else
					pItem->SetGetOutArea( true );
			}
		}
	}


}

void CDnMenuList::ResetItem( CDnMenuListItem * pItem )
{
	if( pItem )
	{
		pItem->SetPress( false );
		pItem->SetSelect( false );
		pItem->SetOver( false );
	}
}

// 선택해제.
void CDnMenuList::Reset()
{
	ResetItem( m_pSelectedMainItem );
	ResetItem( m_pSelectedSubItem );
	
	m_pSelectedItem = m_pSelectedMainItem = m_pSelectedSubItem = NULL;
}

// 선택해제 & 위치조정.
void CDnMenuList::ResetnUpdatePos()
{
	Reset();
	UpdatePosition();
}


void CDnMenuList::SetSelectItem( CDnMenuListItem * pSelectedItem )
{
	if( m_pSelectedItem != pSelectedItem )
	{
		// Main.
		if( pSelectedItem->GetType() == CDnMenuListItem::EITEMTYPE::EITEMTYPE_MAIN )
		{
			// 기존선택해제.
			if( m_pSelectedMainItem )
			{
				m_pSelectedMainItem->SetSelect( false );
				m_pSelectedMainItem->ShowItems( false ); // 자식Item들 hide.
			}
			// 기존선택해제 - Sub.
			if( m_pSelectedSubItem )
				m_pSelectedSubItem->SetSelect( false );

			m_pSelectedMainItem = m_pSelectedItem = pSelectedItem;
			m_pSelectedMainItem->SetSelect( true );
			m_pSelectedMainItem->ShowItems( true ); // 자식Item들 Show.

			// MainItem 선택시 첫번째 SubItem 선택.
			if( m_bAutoSelectSubItem )
			{
				CDnMenuListItem * pFrontSubItem = m_pSelectedMainItem->GetFrontSubItem();
				if( pFrontSubItem )
				{
					//m_pSelectedSubItem = m_pSelectedItem = pFrontSubItem;
					m_pSelectedSubItem = m_pSelectedItem = pFrontSubItem;
					m_pSelectedSubItem->SetSelect( true );
				}
			}
		}

		// Sub.
		else if( pSelectedItem->GetType() == CDnMenuListItem::EITEMTYPE::EITEMTYPE_SUB )
		{
			//m_pSelectedMainItem = pSelectedItem->GetParentItem();
			CDnMenuListItem * pMainItem = pSelectedItem->GetParentItem();
			if( pMainItem != m_pSelectedMainItem )
			{
				// 기존선택해제.
				m_pSelectedMainItem->SetSelect( false );
				m_pSelectedMainItem->ShowItems( false ); // 자식Item들 hide.

				m_pSelectedMainItem = pMainItem;
				m_pSelectedMainItem->SetSelect( true );
				m_pSelectedMainItem->ShowItems( true ); // 자식Item들 Show.
			}
			
			// 기존선택해제 - Sub.
			if( pSelectedItem != m_pSelectedSubItem )
				m_pSelectedSubItem->SetSelect( false );
			
			m_pSelectedSubItem = m_pSelectedItem = pSelectedItem;
			m_pSelectedSubItem->SetSelect( true );
		}

		UpdatePosition();

		m_pParent->ProcessCommand( EVENT_MENULIST_SELECTION, true, this );
	}

}


// 메인아이템들중 해당인덱스가 선택되도록한다.
void CDnMenuList::SelectByIndex( int idx )
{
	if( m_vMainMenu.empty() || idx >= (int)m_vMainMenu.size() )
		return;

	SetSelectItem( m_vMainMenu[ idx ] );

}