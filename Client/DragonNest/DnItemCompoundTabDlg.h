#pragma once
#include "DnCustomDlg.h"
#include "DnItemTask.h"
#include "DnWeapon.h"
#include "DnItemTask.h"
#include "DnInterfaceString.h"

class CDnItem;
class CDnItemSlotButton;

class CDnItemCompoundPopupDlg;
class CDnItemCompoundMixDlg;

// 탭이 있지만, 탭마다 다른 다이얼로그가 아니기때문에, TabDialog를 상속받지 않는다.
class CDnItemCompoundTabDlg : public CDnCustomDlg, public CEtUICallback
{
	struct SCompoundListItem
	{
		int m_nCompoundID;
		CDnItem *m_pItem;
		CDnItemSlotButton *m_pItemSlotButton;
		CEtUIStatic *m_pStaticBase;
		CEtUIStatic *m_pStaticName;
		CEtUIStatic *m_pStaticLevelText;	// 레벨 적혀있는 텍스트
		CEtUIStatic *m_pStaticLevel;
		CEtUIStatic *m_pStaticType;

		SCompoundListItem()
			: m_pItem(NULL)
			, m_pItemSlotButton(NULL)
			, m_pStaticBase(NULL)
			, m_pStaticName(NULL)
			, m_pStaticLevelText(NULL)
			, m_pStaticLevel(NULL)
			, m_pStaticType(NULL)
			, m_nCompoundID(0)
		{
		}

		~SCompoundListItem()
		{
			SAFE_DELETE( m_pItem );
		}

		void SetInfo( int nCompoundID, int nItemID )
		{
			m_nCompoundID = nCompoundID;

			if( nItemID > 0 )
			{
				SAFE_DELETE( m_pItem );

				TItemInfo itemInfo;
				if( CDnItem::MakeItemInfo( nItemID, 1, itemInfo ) == false ) return;
				m_pItem = GetItemTask().CreateItem( itemInfo );

				// 내구도 재설정. 위에서는 맥스치를 알 수 없으니 여기서 이렇게 맥스치로 설정한다.
				if( m_pItem->GetItemType() == ITEMTYPE_WEAPON )
				{
					CDnWeapon *pWeapon = dynamic_cast<CDnWeapon *>(m_pItem);
					pWeapon->SetDurability( pWeapon->GetMaxDurability() );
				}
				else if( m_pItem->GetItemType() == ITEMTYPE_PARTS )
				{
					CDnParts *pParts = dynamic_cast<CDnParts *>(m_pItem);
					pParts->SetDurability( pParts->GetMaxDurability() );
				}
				m_pItemSlotButton->SetItem(m_pItem, CDnSlotButton::ITEM_ORIGINAL_COUNT);

				m_pStaticBase->Show( true );
				m_pStaticName->SetText( m_pItem->GetName() );
				m_pStaticLevelText->Show( true );
				m_pStaticLevel->SetIntToText( m_pItem->GetLevelLimit() );

				// 아이템 세부 타입. 세부타입이 있을때만 설정한다.
				{
					WCHAR szTemp[64];
					if( m_pItem->GetItemType() == ITEMTYPE_WEAPON )
					{
						CDnWeapon *pWeapon = dynamic_cast<CDnWeapon *>(m_pItem);
						if( pWeapon )
							swprintf_s( szTemp, _countof(szTemp), L"%s : %s", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 7009 ), DN_INTERFACE::STRING::ITEM::GetEquipString( pWeapon->GetEquipType() ) );
						m_pStaticType->SetText( szTemp );
					}
					else if( m_pItem->GetItemType() == ITEMTYPE_PARTS )
					{
						CDnParts *pParts = dynamic_cast<CDnParts *>(m_pItem);
						if( pParts )
							swprintf_s( szTemp, _countof(szTemp), L"%s : %s", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 7009 ), DN_INTERFACE::STRING::ITEM::GetPartsString( pParts->GetPartsType() ) );
						m_pStaticType->SetText( szTemp );
					}
				}
			}
		}

		void Clear()
		{
			m_nCompoundID = 0;
			SAFE_DELETE( m_pItem );
			m_pItemSlotButton->ResetSlot();
			m_pStaticBase->Show( false );
			m_pStaticName->SetText(L"");
			m_pStaticLevelText->Show( false );
			m_pStaticLevel->SetText(L"");
			m_pStaticType->SetText(L"");
		}

		bool IsInsideItem( float fX, float fY )
		{
			SUICoord uiCoordsBase;
			m_pStaticBase->GetUICoord(uiCoordsBase);
			return uiCoordsBase.IsInside(fX, fY);
		}
	};

	enum
	{
		STUFF_VIEW_DIALOG,
		COMPOUND_CONFIRM_DIALOG,
		COMPOUND_MOVIE_DIALOG,
		NUM_MAX_COMPOUND_ITEM = 6,
		NUM_NEED_ITEM = 5,
	};

public:
	CDnItemCompoundTabDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL );
	virtual ~CDnItemCompoundTabDlg(void);

protected:
	CEtUIRadioButton *m_pTabButtonWeapon;
	CEtUIRadioButton *m_pTabButtonParts;
	CEtUIRadioButton *m_pTabButtonAccessory;
	CEtUIRadioButton *m_pTabButtonNormal;

	// 탭 다이얼로그를 상속받지 않아서 GetCurrentTabID함수로 얻어올 수 없어서 직접 가지고 있는다.
	int m_nCurTab;

	CEtUIComboBox *m_pComboBoxJob;
	CEtUIComboBox *m_pComboBoxType;

	CEtUIButton *m_pButtonOK;
	CEtUIButton *m_pButtonCancel;
	bool m_bOK;

	SCompoundListItem m_sCompoundItem[NUM_MAX_COMPOUND_ITEM];
	CEtUIStatic *m_pStaticSelect;
	int m_nCurSelect;

	CEtUIButton *m_pButtonPagePrev;
	CEtUIButton *m_pButtonPageNext;
	CEtUIStatic *m_pStaticPage;
	int m_nCurPage;
	int m_nMaxPage;

	// 데이터 관련
	int m_nCurCompoundListID;
	std::vector<int> m_vecCompoundID;

	// 현재 리스트에 보여줄 것들만 담고있는 벡터
	std::vector<int> m_vecCompoundIDinList;

	CDnItemCompoundMixDlg *m_pCompoundMixDlg;

	// 재료템 슬롯
	CDnItem *m_pNeedItem[NUM_NEED_ITEM];
	CDnItemSlotButton *m_pNeedItemSlotButton[NUM_NEED_ITEM];

	CEtUIStatic *m_pGold;
	CEtUIStatic *m_pSilver;
	CEtUIStatic *m_pBronze;
	DWORD m_dwColorGold;
	DWORD m_dwColorSilver;
	DWORD m_dwColorBronze;

	// 재료 슬롯 설정
	void SetNeedItemSlot( int nSlotIndex, int nItemID, int nNeedItemCount, int nCurItemCount );

	int FindInsideItem( float fX, float fY );
	void UpdateSelectBar();

	void ResetList();

	void CheckUsableTab();

	void UpdateComboBoxType();
	void UpdateCompoundList();
	void UpdatePage();
	void UpdateCompoundSlot();
	void UpdateCompoundableSlot();

	void UpdateList();

	void PrevPage();
	void NextPage();

	void ShowConfirmDlg( int nCompoundID, bool bOK );

	// 조합이 가능한지 본다.
	void CheckCompound();
	bool CheckCompound( int nCompoundID );

	// 서버에 전송할 필요아이템 구조체
	vector<CDnItemTask::ItemCompoundInfo> m_vNeedItemInfo;
public:
	// NPC의 CompountItemShopID를 얻어와 조합리스트를 세팅한다.
	void SetCompoundListID( int nListID );


	void OnRecvItemCompoundOpen();
	void OnRecvItemCompound();

public:
	virtual void InitialUpdate();
	virtual void Initialize( bool bShow );
	virtual void Show( bool bShow );
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0 );
	virtual bool MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
	virtual void OnUICallbackProc( int nID, UINT nCommand, CEtUIControl *pControl, UINT uiMsg = 0 );
};