#pragma once

#include "EtUIControl.h"
#include "EtUIScrollBar.h"
#include "EtUITreeItem.h"

class CEtUITreeCtl : public CEtUIControl
{
public:
	CEtUITreeCtl( CEtUIDialog *pParent );
	virtual ~CEtUITreeCtl(void);

protected:
	bool			m_bPressed;
	CEtUIScrollBar	m_ScrollBar;
	int				m_nCurLine;
	int				m_nVisibleCount;

	CTreeItem *m_pRootItem;
	CTreeItem *m_pSelectedItem;
	TREEITEM_LIST m_listTreeTravelItem;

protected:
	void DoubleClickExpand( float fX, float fY );
	virtual bool Expand( float fX, float fY );
	bool SelectItem( float fX, float fY );
	bool OverItem( float fX, float fY );

public:
	CTreeItem* AddItem( DWORD dwType, LPCWSTR szText, DWORD dwColor = textcolor::WHITE );
	CTreeItem* AddChildItem( CTreeItem *pTreeItem, DWORD dwType, LPCWSTR szText, DWORD dwColor = textcolor::WHITE );
	bool DeleteItem( CTreeItem *pTreeItem );
	bool DeleteItem( void *pData );
	virtual void DeleteAllItems();

	CTreeItem*	FindItemData( void *pData, bool bRecursive = true );
	CTreeItem*	FindItemInt( int nValue, bool bRecursive = true );
	CTreeItem*	FindItemInt64( INT64 nValue, bool bRecursive = true );
	CTreeItem*	FindItemFloat( float fValue, bool bRecursive = true );

public:
	// Note : pTreeItem의 아래 모든 아이템에 Type을 Add한다.
	//
	void AddItemTypeChildren( CTreeItem *pTreeItem, DWORD dwType );

	void ExpandAll();
	void ExpandAllChildren( const CTreeItem *pItem );
	void CollapseAll();

	void SetSelectItem( CTreeItem *pItem );
	void ResetSelectedItem();

	CTreeItem* GetBeginItem();
	CTreeItem* GetEndItem();

	CTreeItem* GetSelectedItem()	{ return m_pSelectedItem; }
	int GetExpandedItemCount();

public:
	void ScrollToBegin();
	void ScrollToEnd();

protected:
	bool IsScrollMode();
	int Scroll( int nScrollAmount );
	
protected:
	virtual void RenderTree( TREEITEM_LIST &VecTree );
	virtual void RenderButton( CTreeItem *pItem );
	void RenderText( CTreeItem *pItem );
	void RenderBase( CTreeItem *pItem );
	void RenderSelect( CTreeItem *pItem );
	void RenderClassIcon( CTreeItem *pItem );

	void UpdateRectsTree();
	bool UpdateVisibleCount();
	void TravelTree();

public:
	virtual void Initialize( SUIControlProperty *pProperty );
	virtual void Render( float fElapsedTime );
	virtual bool CanHaveFocus() { return ( IsShow() && IsEnable() ); }
	virtual bool HandleMouse( UINT uMsg, float fX, float fY, WPARAM wParam, LPARAM lParam );
	virtual int GetScrollBarTemplate() { return m_Property.TreeControlProperty.nScrollBarTemplate; }
	virtual void SetScrollBarTemplate( int nIndex ) { m_Property.TreeControlProperty.nScrollBarTemplate = nIndex; }
	virtual void UpdateRects();

	virtual void FindInputPos( std::vector<EtVector2> &vecPos );
	virtual bool IsHaveScrollBarControl() { return true; }
	virtual void OnReleasePressed() { m_ScrollBar.HandleMouse( WM_LBUTTONUP, 0.0f, 0.0f, 0, 0 ); }

	//#ifdef PRE_ADD_MISSION_NEST_TAB
	TREEITEM_LIST GetTravalTreeItem() { return m_listTreeTravelItem; }
	void Sort( bool bSortStandard );
	//#endif
};