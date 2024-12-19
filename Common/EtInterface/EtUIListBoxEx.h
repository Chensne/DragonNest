#pragma once
#include "EtUIListBox.h"
//#include "EtUIQuestTreeItem.h"

// 리스트박스 아이템이 일반 ui인 확장형 리스트박스다.
class CEtUIListBoxEx : public CEtUIListBox
{
public:
	CEtUIListBoxEx( CEtUIDialog *pParent );
	virtual ~CEtUIListBoxEx(void);

protected:
	void CheckListBoxItem();

	SUICoord m_SplitSelectCoord[9];
	SUICoord m_SplitSelectUVCoord[9];

	bool m_bRenderScrollBar;
	bool m_bRenderSelectBar;
	bool m_bReturnHandleMouseProcessed;

public:
	//bool SetElementUI( const char *szFileName );

	template< typename T >
	T *AddItem(int nID = -1);

	template< typename T >
	T *InsertItem(int nID = 0);

	/*template<typename T >
	void SwapItem(int nleft, int nRight);*/

	void RemoveItem( int nIndex );
	void RemoveAllItems();

	template< typename T >
	T *GetItem( int nIndex ) 
	{
		if( m_vecItems.size() == 0 || nIndex >= (int)m_vecItems.size() )
			return NULL;

		return (T*)m_vecItems[nIndex]->pData;
	}

	template<typename T>
	T*	FindItem(int nID)
	{
		std::vector<SListBoxItem*>::iterator iter = m_vecItems.begin();
		for (; iter != m_vecItems.end(); ++iter)
		{
			SListBoxItem* pItem = *iter;
			if (pItem)
			{
				T* pTmp = (T*)pItem->pData;
				if (pTmp)
				{
					int dlgID = pTmp->GetDialogID();
					if (dlgID < 0 || dlgID != nID)
						continue;
					else
						return pTmp;
				}
			}
		}

		return NULL;
	}

	// 일반 리스트박스와 달리 확장형에선 스크롤바를 렌더링 안할때도 있다.
	// 툴에서 속성추가하기엔 너무 사용하지 않을듯한 플래그라서 별로인 듯 하다.
	void SetRenderScrollBar( bool bRender = true ) { m_bRenderScrollBar = bRender; }

	// 일반 리스트박스와 달리 확장형에선 선택바를 렌더링 안할때도 있다.
	void SetRenderSelectBar( bool bRender = true ) { m_bRenderSelectBar = bRender; }

	// HandleMouse로 아이템다이얼로그 처리 후 리턴할 건지. 기본은 false.(break한다.)
	// 아무래도 이렇게 Return 하면 스크롤바처리가 제대로 안되서 이상한 점이 많다. 빼야할 듯.
	void SetReturnHandleMouseProcessed( bool bReturn = false ) { m_bReturnHandleMouseProcessed = bReturn; }

public:
	virtual void Initialize( SUIControlProperty *pProperty );
	virtual bool HandleMouse( UINT uMsg, float fX, float fY, WPARAM wParam, LPARAM lParam );
	virtual void Render( float fElapsedTime );

	virtual void UpdateRects();
};

// ui파일을 리스트박스 아이템으로 사용하려면 두가지 방법이 있다.
// 하나는 각각의 아이템에 자식윈도우를 할당해서 처리하는 방법과,
// 다른 하나는 항상 Show(false)상태의 윈도우를 만들어 직접 관리하는 방법이다.
//
// 각각의 특징을 보자면,
// 자식 윈도우로 처리할 경우엔 인풋처리에 대해서는 알아서 이뤄지니,
// 적절한 순간(스크롤을 움직여 보이는 아이템일때)에 적절한 위치(리스트박스 아이템 위치)에다가 Show(true/false)만 해주면 끝이라는 것이다.
// 그러나, 가장 큰 단점으로
// 다이얼로그 MsgProc를 그대로 사용하기때문에, Child다이얼로그에서 처리되는 인풋들이 리스트박스까지 가지 않는다는 것이다.
// 그래서 별도로 리스트박스 컨트롤에 메세지를 보내줘야하는데,
// 다이얼로그를 뜯어서 예외처리 형태로 처리하기엔 너무 지저분해 보인다.
// 또 다른 가장 큰 단점은 다이얼로그 렌더링 순서를 쉽게 바꿀 수 없는 것인데,
// 자신 윈도우 다이얼로그 그리고, 컨트롤 그리고, 자식 다이얼로그를 그리기때문에 리스트아이템 선택표시 그림이 자식윈도우보다 아래 깔리게된다.
// 이런 이유때문에, 아래 방법을 선택하게 되었다.
//
// 만약 안보이는 상태의 윈도우로 만들어 관리할 경우엔,
// 입력처리 및 렌더까지 직접 제어해야해서 별로처럼 보이지만,
// 지저분한 코드를 다이얼로그.cpp에 작성하지 않아도 된다는 장점이 있다.(특수한 처리는 특수한 EtUIListBoxEx클래스에서 하는 것.)
//

//#define _LISTITEM_CHILD

template< typename T >
T *CEtUIListBoxEx::AddItem(int nID)
{
	// SListBoxItem를 쓰긴 쓰되, pData하나만 사용한다.
	SListBoxItem *pNewItem;

	pNewItem = new SListBoxItem;

#ifdef _LISTITEM_CHILD
	T *pData = new T( UI_TYPE_CHILD, m_pParent nID);
#else
	T *pData = new T( UI_TYPE_SELF, NULL, nID );
#endif
	pData->Initialize( true );
	pData->SetElementDialog( true );
	pNewItem->pData = pData;
	wcscpy_s( pNewItem->strText, 256, L"" );
	pNewItem->nValue = 0;
	SetRect( &pNewItem->rcActive, 0, 0, 0, 0 );
	pNewItem->bSelected = false;
	pNewItem->dwColor = 0xFFFFFFFF;

	m_vecItems.push_back( pNewItem );
	m_ScrollBar.SetTrackRange( 0, ( int )m_vecItems.size() );

	CheckListBoxItem();

	return pData;
}

template< typename T >
T * CEtUIListBoxEx::InsertItem(int nID)
{
	// SListBoxItem를 쓰긴 쓰되, pData하나만 사용한다.
	SListBoxItem *pNewItem;

	pNewItem = new SListBoxItem;

#ifdef _LISTITEM_CHILD
	T *pData = new T( UI_TYPE_CHILD, m_pParent nID);
#else
	T *pData = new T( UI_TYPE_SELF, NULL, nID );
#endif
	pData->Initialize( true );
	pData->SetElementDialog( true );
	pNewItem->pData = pData;
	wcscpy_s( pNewItem->strText, 256, L"" );
	pNewItem->nValue = 0;
	SetRect( &pNewItem->rcActive, 0, 0, 0, 0 );
	pNewItem->bSelected = false;
	pNewItem->dwColor = 0xFFFFFFFF;

	m_vecItems.insert( m_vecItems.begin() + nID, pNewItem );
	m_ScrollBar.SetTrackRange( 0, ( int )m_vecItems.size() );

	CheckListBoxItem();

	return pData;
}
