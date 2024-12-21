#pragma once
#include "etuibutton.h"
#include "EtUIScrollBar.h"

#define PRE_FIX_COMBOBOX_ELLIPSIS

struct SComboBoxItem
{
	WCHAR	strText[ 256 ];
#ifdef PRE_FIX_COMBOBOX_ELLIPSIS
	std::wstring strOriginal;
#endif
	void *	pData;
	int		nValue;
	SUICoord ItemCoord;
	RECT	rcActive;
	bool	bVisible;
	bool	bEllipsis;

	SComboBoxItem()
		: pData( NULL )
		, nValue( 0 )
		, bVisible( false )
		, bEllipsis( false )
	{
#ifdef PRE_FIX_COMBOBOX_ELLIPSIS
		strOriginal = L"";
#endif
		memset( strText, 0, sizeof(WCHAR)*256 );
		memset( &rcActive, 0, sizeof(RECT) );
	}
};

class CEtUIComboBox :public CEtUIButton
{
public:
	CEtUIComboBox( CEtUIDialog *pParent );
	virtual ~CEtUIComboBox(void);

protected:
	CEtUIScrollBar m_ScrollBar;
	std::vector< SComboBoxItem * > m_vecItems;

	int m_iSelected;
	int m_iFocused;
	bool m_bOpened;
	bool m_bEditMode;

	SUICoord m_TextCoord;
	SUICoord m_ButtonCoord;
	SUICoord m_MoveButtonCoord;
	SUICoord m_DropDownCoord;
	SUICoord m_DropDownTextCoord;

	float m_fLineSpace;

public:
	void AddItem( const WCHAR* strText, void* pData, int nValue, bool bEllipsis = false );
	void RemoveAllItems();
	void RemoveItem( int nIndex );
	bool ContainsItem( const WCHAR* strText, UINT iStart=0 );
	int FindItem( const WCHAR* strText, UINT iStart=0 );
	int FindItemByValue( int nValue, UINT iStart=0 );
	UINT	GetItemCount() const { return static_cast<UINT>(m_vecItems.size()); }
	void *GetItemData( const WCHAR* strText );
	void *GetItemData( int nIndex );
	void *GetSelectedData();
	bool GetItemValue( const WCHAR* strText, int &nValue );
	bool GetItemValue( int nIndex, int &nValue );
	bool GetSelectedValue( int &nValue );
	const SUICoord& GetButtonCoord() const { return m_ButtonCoord; }
	SComboBoxItem *GetSelectedItem();
	void ClearSelectedItem();
	int GetSelectedIndex() { return m_iSelected; }
	const SUICoord& GetDropDownTextCoord() const { return m_DropDownTextCoord; }

	void SetSelectedByIndex( int nIndex );
	void SetSelectedByText( const WCHAR* strText );
	void SetSelectedByData( void* pData );
	void SetSelectedByValue( int nValue );
	bool IsOpenedDropDownBox() { return m_bOpened; }

public:
	virtual void Initialize( SUIControlProperty *pProperty );
	virtual int GetScrollBarTemplate() { return m_Property.ComboBoxProperty.nScrollBarTemplate; }
	virtual void SetScrollBarTemplate( int nIndex ) { m_Property.ComboBoxProperty.nScrollBarTemplate = nIndex; }
	virtual void SetTextColor(  DWORD dwColor );
	virtual bool CanHaveFocus() { return ( IsShow() && IsEnable() ); }
	virtual void Focus( bool bFocus );
	void		 SetEditMode(bool bEnable) { m_bEditMode = bEnable; }
	virtual bool IsInside( float fX, float fY );
	bool ToggleDropDownList();

public:
	virtual bool HandleKeyboard( UINT uMsg, WPARAM wParam, LPARAM lParam );
	virtual bool HandleMouse( UINT uMsg, float fX, float fY, WPARAM wParam, LPARAM lParam );
	virtual void OnHotkey();
	virtual void UpdateRects();
	virtual void Render( float fElapsedTime );

	virtual void FindInputPos( std::vector<EtVector2> &vecPos );
};
