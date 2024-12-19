#pragma once

#include "EtUIControl.h"

class CDnMenuListItem;

// 사용방법은 Tree와 비슷합니다.
// 사용예는 DnCashShopMenuListDlg( 캐시샵 카테고리 ) 에 있습니다.
class CDnMenuList : public CEtUIControl
{

private:

	//CEtUIScrollBar m_ScrollBar;

	std::vector< CDnMenuListItem * > m_vMainMenu;
	CDnMenuListItem * m_pSelectedMainItem;
	CDnMenuListItem * m_pSelectedSubItem;
	CDnMenuListItem * m_pSelectedItem; // - MainItem만 선택이면 MainItem,
									   //   SubItem만 선택이면  SubItem,
									   //   둘다선택이면 SubItem.


	SUICoord m_ViewArea; // MenuItem이 보이는 영역( Cliping 영역 ).

	CDnMenuListItem * m_pOverItem; // MouserOver.

	// 메뉴리스트 스크롤 버튼들.
	struct ScorllButton
	{
		SUICoord m_BtnCoord;
		SUICoord m_BtnOverCoord;

		bool bOver;
		bool bSelect;
		ScorllButton() : bOver(false), bSelect(false){}
	};
	ScorllButton m_ScrollBtn[2];
	EtVector2 m_BtnPressOffset;
	

	int m_topMainItemIndex; // 최상단에 보이는 MainItem Index;

	bool m_bAutoSelectSubItem; // MainItem 선택시 첫번째 SubItem 이 자동선택하는지 여부.
	
	bool m_bShowScrollBtn; // 스크롤버튼 출력여부.

public:

	CDnMenuList( CEtUIDialog * pParent );
	virtual ~CDnMenuList();


	virtual void Initialize( SUIControlProperty *pProperty );
	virtual bool HandleMouse( UINT uMsg, float fX, float fY, WPARAM wParam, LPARAM lParam );
	virtual void Render( float fElapsedTime );
	virtual void Show( bool bShow );
	virtual void OnChangeResolution();
	virtual bool CanHaveFocus() { return true; }

private:

	void RenderBase( CDnMenuListItem *pItem, int eleIdx );
	void RenderSelect( CDnMenuListItem *pItem, int eleIdx );
	void RenderOver( CDnMenuListItem *pItem, int eleIdx );
	void RenderText( CDnMenuListItem *pItem, int eleIdx );
	void RenderButton();

	
	// Item들의 위치설정.
	void UpdatePosition();

	// 마우스처리.
	bool SelectItem( float fX, float fY );
	bool OverItem( float fX, float fY );

	bool SelectButton( float fX, float fY );
	bool OverButton( float fX, float fY );
	void PressItem( float fX, float fY );
	void Scroll( int nScroll );

public:

	CDnMenuListItem * AddItem( LPCWSTR szText, DWORD dwColor=textcolor::WHITE  ); // MainItem 추가.

	CDnMenuListItem * AddChildItem( CDnMenuListItem * pMenuItem, LPCWSTR szText, DWORD dwColor=textcolor::WHITE ); // MainItem에 SubItem추가.

	// 제거.
	bool DeleteItem( CDnMenuListItem * pMenuItem );
	void DeleteAllItems();

	int GetMainTabSize(){
		return (int)m_vMainMenu.size();
	}
	CDnMenuListItem * GetMainTab( int idx ){
		if( idx < (int)m_vMainMenu.size() )
			return m_vMainMenu[ idx ];
		return NULL;
	}	

	// MainTab 선택시 첫번째 SubTab이 자동 선택되도록 처리.
	void AutoSelectToSubItem( bool bAuto ){
		m_bAutoSelectSubItem = bAuto;
	}
	bool IsAutoSelectToSubItem(){
		return m_bAutoSelectSubItem;
	}

	// Selected Item.
	CDnMenuListItem * GetSelectedMainItem(){
		return m_pSelectedMainItem;
	}
	CDnMenuListItem * GetSelectedSubItem(){
		return m_pSelectedSubItem;
	}
	CDnMenuListItem * GetSelectedItem(){
		return m_pSelectedItem; 
	}

	// 해당아이템이 Select되도록한다.
	void SetSelectItem( CDnMenuListItem * pSelectedItem );
	
	void SelectByIndex( int idx ); // 메인아이템들중 해당인덱스가 선택되도록한다.

	void Reset(); // 선택해제.
	void ResetnUpdatePos(); // 선택해제 & 위치조정.
	void ResetItem( CDnMenuListItem * pItem );

};