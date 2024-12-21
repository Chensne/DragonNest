#pragma once
#include <list>

class CTreeItem;
class CEtUIControl;

typedef std::list<CTreeItem*>		TREEITEM_LIST;
typedef TREEITEM_LIST::iterator		TREEITEM_LIST_ITER;

class CTreeItem
{
public:
	struct DeleteItem
	{
		void operator() ( const CTreeItem *ptr ) const
		{
			ASSERT(ptr&&"CTreeItem<TYPE>::DeleteItem()");
			SAFE_DELETE( ptr );
		}
	};

public:
	enum emITEM_TYPE
	{
		typeNone			= 0x0000,
		typeMain			= 0x0001,
		typeOpen			= 0x0002,
		typeSelect			= 0x0004,
		typeExpand			= 0x0008,
		typeComplete		= 0x0010,
	};

	enum eClassIconType
	{
		eNONE,
		eGREEN = 4,		// ui tool 에서 지정된 번호
		eYELLOW,
		eBLUE,
		eMail = 11,
		eNoneMail,
	};

public:
	CTreeItem();
	virtual ~CTreeItem();

protected:
	std::wstring m_strText;
	std::wstring m_strBackText;
	bool m_bWithEllipsis;
	std::wstring m_strOrigText;
	DWORD m_dwColor;
	DWORD m_dwBackColor;
	DWORD m_dwItemType;
	int m_nTreeDepth;

	float m_fTreeHeight;
	float m_fLineSpace;
	float m_fIndentSize;
	
	TREEITEM_LIST m_listItem;
	CTreeItem *m_pParentItem;

	void *	m_pData;
	int		m_nValue;
	INT64	m_nValue64;
	float	m_fValue;
	std::wstring m_strValue;
	std::string m_strValue2;

	int m_nSubCount;
	static int m_nGoalID;

	eClassIconType	m_ClassIconType;

	CEtUIControl		*m_pParentControl;
	
	char* m_szSortValue[10];
public:
	SUICoord m_uiCoord;			// Note : 아이템의 좌표
	SUICoord m_ButtonCoord;		// Note : 왼쪽 버튼 좌표
	SUICoord m_strCoord;		// Note : 텍스트 출력될 좌표
	SUICoord m_strBackCoord;	// Note : 뒤에서 출력될 텍스트 좌표
	SUICoord m_BaseCoord;
	SUICoord m_SelectCoord;
	SUICoord m_ClassIconCoord[4];

public:
	void SetText( LPCWSTR szText, DWORD dwColor = textcolor::WHITE );
	void SetBackText( LPCWSTR szText, DWORD dwColor = textcolor::WHITE );
	LPCWSTR GetText()						{ return m_strText.c_str(); }
	LPCWSTR GetBackText()						{ return m_strBackText.c_str(); }
	bool IsWithEllipsis()					{ return m_bWithEllipsis; }
	LPCWSTR GetOrigText()					{ return m_strOrigText.c_str(); }
	void SetTextColor( DWORD dwColor )		{ m_dwColor = dwColor; }
	DWORD GetTextColor()					{ return m_dwColor; }
	void SetBackTextColor( DWORD dwColor )	{ m_dwBackColor = dwColor; }
	DWORD GetBackTextColor()				{ return m_dwBackColor; }
	SUICoord& GetCoord()					{ return m_uiCoord; }

	void SetItemData( void *pData )							{ m_pData = pData; }
	void* GetItemData()										{ return m_pData; }
	void SetItemValueInt( int nValue )						{ m_nValue = nValue; }
	int GetItemValueInt()									{ return m_nValue; }
	void SetItemValueFloat( float fValue )					{ m_fValue = fValue; }
	float GetItemValueFloat()								{ return m_fValue; }
	void SetItemValueString( const std::wstring &strValue )	{ m_strValue = strValue; }
	std::wstring &GetItemValueString()						{ return m_strValue; }
	void SetItemValueStringA( const std::string &strValue )	{ m_strValue2 = strValue; }
	std::string &GetItemValueStringA()						{ return m_strValue2; }
	void SetItemValueInt64( INT64 nValue )					{ m_nValue64 = nValue; }
	INT64 GetItemValueInt64()								{ return m_nValue64; }

	int  GetSubCount()		{ return m_nSubCount; }
	void AddSubCount()		{ ++m_nSubCount; }
	void DelSubCount()		{ --m_nSubCount; }
	void ClearSubCount()	{ m_nSubCount = 0; }

	void			SetClassIconType(eClassIconType type)	{ m_ClassIconType = type; }
	eClassIconType	GetClassIconType() const				{ return m_ClassIconType; }

	CTreeItem* FindItemData( void *pData, bool bRecursive = true );
	CTreeItem* FindItemInt( int nValue, bool bRecursive = true );
	CTreeItem* FindItemInt64( INT64 nValue, bool bRecursive = true );
	CTreeItem* FindItemFloat( float fValue, bool bRecursive = true );

	bool HasChild()							{ return !m_listItem.empty(); }
	int GetDepth()							{ return m_nTreeDepth; }
	void SetDepth( int nDepth )				{ m_nTreeDepth = nDepth; }

	void ExpandAll();
	bool ExpandAllChildren( const CTreeItem *pItem );
	void CollapseAll();

	void Expand()							{ m_dwItemType|=typeExpand; }
	void Collapse()							{ m_dwItemType&=~typeExpand; }
	bool IsExpanded()						{ return (m_dwItemType&typeExpand)?true:false; }

	void Select()							{ m_dwItemType|=typeSelect; }
	void Unselect()							{ m_dwItemType&=~typeSelect; }
	bool IsSelected()						{ return (m_dwItemType&typeSelect)?true:false; }

	void Complete( bool bComplete )			{ bComplete? m_dwItemType|=typeComplete:m_dwItemType&=~typeComplete; }
	bool IsCompleted()						{ return (m_dwItemType&typeComplete)?true:false; }
/*
	void SetDynamicNotifier(bool bDynamic )	{ bDynamic ? m_dwItemType|=typeDynamicNotifier:m_dwItemType&=~typeDynamicNotifier; }
	bool IsDynamicNotifier()				{ return (m_dwItemType&typeDynamicNotifier)?true:false; }

	void SetActiveNotifier(bool bActive )	{ bActive ? m_dwItemType|=typeActiveNotifier:m_dwItemType&=~typeActiveNotifier; }
	bool IsActiveNotifier()					{ return (m_dwItemType&typeActiveNotifier)?true:false; }
*/
	void Open()								{ m_dwItemType|=typeOpen; }
	void Close()							{ m_dwItemType&=~typeOpen; }
	bool IsOpened()							{ return (m_dwItemType&typeOpen)?true:false; }

	void SetItemType( DWORD dwType )		{ m_dwItemType=dwType; }
	void AddItemType( DWORD dwType )		{ m_dwItemType|=dwType; }
	void AddItemTypeAllChildren( DWORD dwType );
	DWORD GetItemType()						{ return m_dwItemType; }

	bool GetVisibleCount( float fControlHeight, float &fTreeHeight, int nCurLine, int &nCount );
	void GetChildCount( int &nCount );
	void GetExpandedItemCount( int &nCount );
	CTreeItem* GetButtonClickItem( float fX, float fY );
	CTreeItem* GetClickItem( float fX, float fY );
	float GetTreeHeight();
	TREEITEM_LIST& GetTreeItemList()		{ return m_listItem; }
	CTreeItem* GetBeginItem();
	CTreeItem* GetEndItem();
	void SetParentItem( CTreeItem *pItem ) { m_pParentItem = pItem; }
	CTreeItem *GetParentItem() { return m_pParentItem; }

	CTreeItem* Add( CTreeItem *pItem );
	bool Delete( CTreeItem *pItem );
	bool Delete( void *pData );
	void DeleteAll();

	void TravelTree( TREEITEM_LIST &vecTreeTravelItem, int &nCount, int nCurLine, int nVisibleCount );

	CEtUIControl *GetParentControl() { return m_pParentControl; }

public:
	virtual void SetUICoord( SUICoord &uiCoord );
	virtual void UpdatePos( float fX, float fY );

public:
	virtual void Initialize( CEtUIControl *pControl, int nDepth = 1 );
	virtual void DeleteCheckBox() {} 
	void SetExceptLastSort( int nGoalID );
	
	static bool EqualQuestID( CTreeItem * pFirst, CTreeItem * pSecond );

//#ifdef PRE_ADD_MISSION_NEST_TAB
	static int GetSplitItemStringValue( std::string szStrValue, char ch, bool bfront );
	static bool Assending( CTreeItem* pLitem, CTreeItem* pRIitem );
	static bool Desending( CTreeItem* pLitem, CTreeItem* pRIitem );
	void SortBySubCategory( bool bSortStandard );
//#endif 
};