#pragma once
#include "etuicontrol.h"
#include "EtUIScrollBar.h"
#define PRE_ADD_LISTBOX_ELLIPSIS
struct SListBoxItem
{
	SListBoxItem & operator=( const SListBoxItem & nRight );

	WCHAR strText[256];
	void *pData;
	int nValue;
	RECT rcActive;
	bool bSelected;
	DWORD dwColor;
#ifdef PRE_ADD_LISTBOX_ELLIPSIS
	bool bHasEllipsis;
	WCHAR strEllipsisText[256];
	SListBoxItem() : pData( NULL ), nValue( -1 ), dwColor( 0 ), bSelected( false ), bHasEllipsis( false ) {}
#endif // PRE_ADD_LISTBOX_ELLIPSIS
};

class CEtUIListBox : public CEtUIControl
{
public:
	CEtUIListBox( CEtUIDialog *pParent );
	virtual ~CEtUIListBox(void);

protected:
	enum STYLE 
	{ 
		MULTISELECTION = 1 
	};

	CEtUIScrollBar m_ScrollBar;
	std::vector< SListBoxItem * > m_vecItems;

	int m_nSelected;
	int m_nSelStart;
	float m_fTextHeight;
	float m_fLineSpace;

	SUICoord m_SelectionCoord;
	SUICoord m_TextCoord;
	bool m_bDrag;

public:
	virtual void Initialize( SUIControlProperty *pProperty );

	virtual int GetScrollBarTemplate() { return m_Property.ListBoxProperty.nScrollBarTemplate; }
	virtual void SetScrollBarTemplate( int nIndex ) { m_Property.ListBoxProperty.nScrollBarTemplate = nIndex; }

	int GetSize() { return ( int )m_vecItems.size(); }
#ifdef PRE_ADD_LISTBOX_ELLIPSIS
	void AddItem( const WCHAR *wszText, void *pData, int nValue, DWORD dwColor = 0xFFFFFFFF, bool bEllipsis = false );
	void InsertItem( int nIndex, const WCHAR *wszText, void *pData, int nValue, bool bEllipsis = false );
#else // PRE_ADD_LISTBOX_ELLIPSIS
	void AddItem( const WCHAR *wszText, void *pData, int nValue, DWORD dwColor = 0xFFFFFFFF );
	void InsertItem( int nIndex, const WCHAR *wszText, void *pData, int nValue );
#endif // PRE_ADD_LISTBOX_ELLIPSIS
	void RemoveItem( int nIndex );
	void RemoveItemByText( WCHAR *wszText );
	void RemoveItemByData( void *pData );
	void RemoveAllItems();

	void SwapItem(int nLeft, int nRight);

	bool GetItemValue( int nIndex, int &nValue );
	bool GetSelectedValue( int &nValue );

	SListBoxItem *GetItem( int nIndex );
	int GetSelectedIndex( int nPreviousSelected = -1 );
	SListBoxItem *GetSelectedItem( int nPreviousSelected = -1 ) { return GetItem( GetSelectedIndex( nPreviousSelected ) ); }
	void SelectItem( int nNewIndex );
	void DeselectItem() { m_nSelected = -1; }

	void DrawText( LPCWSTR szText, SUIElement *pElement, DWORD dwFontColor, SUICoord &Coord  );

	virtual void FindInputPos( std::vector<EtVector2> &vecPos );
	virtual bool IsHaveScrollBarControl() { return true; }
	virtual void OnReleasePressed() { m_ScrollBar.HandleMouse( WM_LBUTTONUP, 0.0f, 0.0f, 0, 0 ); }

	virtual bool CanHaveFocus() { return ( IsShow() && IsEnable() ); }
	virtual bool HandleKeyboard( UINT uMsg, WPARAM wParam, LPARAM lParam );
	virtual bool HandleMouse( UINT uMsg, float fX, float fY, WPARAM wParam, LPARAM lParam );
	virtual bool MsgProc( UINT uMsg, WPARAM wParam, LPARAM lParam );

	virtual void UpdateRects();

	virtual void Render( float fElapsedTime );

	CEtUIScrollBar* GetScrollBar( void ) { return &m_ScrollBar; };

	void SecDragMode( const bool bDrageMode ) { m_bDrag = bDrageMode; }
};
