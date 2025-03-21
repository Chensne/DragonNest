#pragma once
#include "DnCustomDlg.h"
#include "DnDataManager.h"
#include "DnItem.h"

#ifdef PRE_ADD_NPC_REPUTATION_SYSTEM

// 퀘스트 보상 UI 파일을 똑같이 사용합니다.
// 코드는 퀘스트 보상에서 그대로 가져 왔으나 서로 용도는 다르기 때문에 분리해 놓습니다.
// 각자 따로따로 변경될 것입니다.
class CDnNpcAcceptPresentDlg : public CDnCustomDlg
{
	struct SAcceptPresentItemSlot
	{
		CDnItemSlotButton *pItemButton;
		CEtUIStatic *pStaticSelected;
		int iPresentID;				// npc 선물 테이블의 itemid
		int iCoin;					// 코인일 경우 얼마인지.

		SAcceptPresentItemSlot()
			: pItemButton(NULL)
			, pStaticSelected(NULL)
			, iPresentID( 0 )
			, iCoin( 0 )
		{
		}

		void ResetSlot()
		{
			CDnItem *pItem = (CDnItem*)pItemButton->GetItem();
			SAFE_DELETE( pItem );
			pItemButton->ResetSlot();
			//pItemButton->Enable( true );
			pItemButton->SetRegist( false );
			pStaticSelected->Show( false );
			iPresentID = 0;
			iCoin = 0;
		}

		void SetSlot( CDnItem *pItem, int count = -1)
		{
			pItemButton->SetItem(pItem, (count < 0) ? CDnSlotButton::ITEM_ORIGINAL_COUNT : count);
		}

		CDnItem* GetSlot()
		{
			if ( pItemButton )
				return static_cast<CDnItem*>(pItemButton->GetItem());

			return NULL;
		}

		void SetCheck( bool bCheck )
		{
			pStaticSelected->Show( bCheck );
		}

		bool IsCheckSlot()
		{
			return pStaticSelected->IsShow();
		}

		void SetPresentID( int _iPresentID )
		{
			iPresentID = _iPresentID;
		}

		void SetCoin( int _iCoin )
		{
			iCoin = _iCoin;
		}
	};

public:
	CDnNpcAcceptPresentDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL  );
	virtual ~CDnNpcAcceptPresentDlg(void);

protected:
	CEtUIStatic *m_pStaticComment;
	CEtUIStatic *m_pMoneyGold;
	CEtUIStatic *m_pMoneySilver;
	CEtUIStatic *m_pMoneyBronze;
	CEtUIStatic *m_pMoneyExp;
	CEtUIStatic *m_pMoneyExpBack;
	CEtUIStatic *m_pMoneyBack;

	CEtUIStatic *m_pItemBG;

	std::vector<std::pair< SUICoord, CEtUIStatic* > >  m_vlMoneyAndExp;
	std::vector<SAcceptPresentItemSlot> m_vlAcceptPresentItemInfo;

	int m_iSelectedSlotIndex;
	int m_iAvailableCount;

protected:
	void DeleteAllItem();
	void ResetSlotCheck();

public:
	void SetPresent( int nNpcID );

public:
	virtual void Initialize(bool bShow);
	virtual void InitialUpdate();
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0 );
	virtual void Show( bool bShow );

	bool IsPresentSelected( void ) { return (-1 != m_iSelectedSlotIndex); };
	int GetSelectedPresentID( void );
	CDnSlotButton* GetSelectedPresentSlotButton() const;

#ifdef PRE_ADD_QUEST_BACK_DIALOG_BUTTON
	void ResetPresentDlg() { ResetSlotCheck(); this->Show(false); }
#endif
};

#endif // #ifdef PRE_ADD_NPC_REPUTATION_SYSTEM